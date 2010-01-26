/**
 * This is an ABC (Abstract Base Class) for use in enfa modelling.
 * It provides the core functionality and leaves the specifics
 * of what component cutoff methodology should be used to 
 * inheriting classes.
 * 
 * @file enfa
 * @author Chris Yesson (chris.yesson@ioz.ac.uk)
 * @date   2009-12-01
 * $Id: enfa.hh
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2009 by CRIA -
 * Centro de Referencia em Informacao Ambiental
 *
 * http://www.cria.org.br
 * 
 * Copyright(c) AH Hirzel et al 2002 (Model Methodology)
 * Copyright(c) Chris Yesson, Derek Tittensor and Tim Sutton 2009 (C++ implementation)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details:
 * 
 * http://www.gnu.org/copyleft/gpl.html
 */
 
#ifndef ENFA_H
#define ENFA_H

#include <openmodeller/om.hh>
#include <openmodeller/Exceptions.hh>
#include <gsl/gsl_matrix.h>

/**

Herewith follows a detailed explanation of the Enivironmental Niche Factor Analysis (ENFA). 
This implementation of ENFA was written by Chris Yesson using the openmodeller cpp code for the CSM algorithm by Tim Sutton as a code foundation and the Matlab implentation of ENFA written by Derek Tittensor as a logical guide.

\verbatim
//////////////////////////////////////////////////////
// Model Creation
//////////////////////////////////////////////////////

Inputs:

File contiaing xy point localties
List of gdal layers
----------------------------------

- taken from Derek Tittensors matlab implementation of enfa

BACGROUND DATA NORMALISATION

% Calculate background mean, std, and covariance

% Standardise the background data matrix following Hirzel et al. (2002)

% Estimate the global covariance matrix (for non-NaN data only)
% use the pseudo-absence generator to get a sample of background points
% this sample could theoretically be every background point by setting
% a high numpoints and tweaking the threshold too.

% Calculate the covariance matrix for the species only environmental data

% RUN THE ENFA

% calculate species means
%--------------------------------------------------------------------------
m = mean(speclocegvs);
z = cov_species^-0.5 * m';
y = z / sqrtm(z' * z);

W = cov_species^-0.5 * cov_global * cov_species^-0.5;
H = (eye(no_egvs) - y * y') * W * (eye(no_egvs) - y * y');

[v, eigenvalues] = eig(H);
eigenvalues = diag(eigenvalues);

% OK, we have a problem here in that one eigenvalue is non-zero (negative?)
% but it's not immediately clear which one.
t = eigenvalues ~= min(eigenvalues);
eigenvalues(2:no_egvs) = eigenvalues(t);
eigenvalues(1) = trace(W) - trace(H)

% Figure out which eigenvalue has gone AWOL
u = cov_species^-0.5 * v;
u_orig = u;

% OK, now replace the first column with m
u(:,2:no_egvs) = u(:,t);
u(:,1) = m';

% Now norm u
for ii = 1:no_egvs
    u(:,ii) = u(:,ii) / norm(u(:,ii));
end

% Remember that signs are unimportant for every factor 
% except the first
score_matrix = u

% Calculate global marginality
marginality = sqrt(sum(m.^2));
marginality = marginality / 1.96

% Calculate global specialization
specialization = sqrt(sum(eigenvalues)/length(m))
factor_weights = eigenvalues / sum(eigenvalues);

The next step is to remove unwanted components either by specifying the number to retain, keeping those that sum to a specified level of variation, or by comparison to the broken stick distribution.

That complete the ENFA model definition

//////////////////////////////////////////////////////
// Model Projection:
//////////////////////////////////////////////////////

%--------------------------------------------------------------------
% HABITAT_SUITABILITY_GEO - A function to calculate and plot habitat 
% suitability from ENFA results. Uses the geometric mean algorithm
% described in Hirzel & Arlettaz (2003)
% 
% INPUTS: 
% score_matrix -    The score matrix from the ENFA
% no_factors -      Number of factors to be used in the analysis
% (following broken stick advice)
% factor_weights -  Weights to give to each factor
% varmat        -   A matrix of EGV data (each column one EGV)
% specloc       -   The EGV locations (rows) with species
% observations
% no_egvs       -   The number of EGVs
%
% NOTE THAT hsmap_specs MUST BE A SINGLE COLUMN VECTOR OF SPECIES LOCATIONS
%
% Derek Tittensor
% 27th August 2007
%-----------------------------------------------------------------


function final_hs = habitat_suitability(score_matrix, no_factors, factor_weights, hsmap_egvs, hsmap_specs, no_egvs)

% Create vectors to hold results
geo_mean = zeros(length(hsmap_egvs),1);
hs = zeros(length(hsmap_egvs),1);
temp_egvs = zeros(length(hsmap_egvs),no_factors);
temp_spec = zeros(length(hsmap_specs),no_factors);

% Convert EGVs and species locations to
% factor axes by multiplying by columns of 
% the score matrix (this gives identical results to Biomapper)
for ii = 1:no_factors
    temp_egvs(:,ii) = hsmap_egvs(:,3:no_egvs + 2) * score_matrix(:,ii);
end

% Create a matrix holding the factor-converted species data
temp_spec = temp_egvs(hsmap_specs,:);

% Use only the weights for the factors that we are using
weights = factor_weights(1:no_factors)'

weights = weights ./ sum(weights);

% Now calculate the geometric mean of points P in the
% environmental space to all species observation points
% Loop through locations
for ii = 1:length(temp_egvs)
    dist = zeros(length(temp_spec),1);
    % Look through locations with species
    for jj = 1:length(temp_spec);
        % Calculate Euclidian distance
        
        disttemp = (temp_egvs(ii,:) - temp_spec(jj,:)).^2;
        disttemp = weights .* disttemp;
        dist(jj) = sqrt(sum(disttemp));
    end

    
    % MATLAB has a built in geometric mean function
    % Note that we exclude points with a distance of zero
    % otherwise function gives a result of zero
    %geo_mean(ii) = geomean(dist(dist~=0));
    geo_mean(ii) = prod(dist(dist~=0));
    geo_mean(ii) = geo_mean(ii)^(1 / length(dist));
   
end

% Extract the geometric means of all the species observation points
species_geomeans = geo_mean(hsmap_specs);


% OK, now to convert these into habitat suitability values.
% Determine what percentage of the species presence points
% are further away from zero

% Loop through the geometric mean distances
for ii = 1:length(geo_mean)
    % Find out how many observed points are further away
    hs(ii) = length(find(species_geomeans >= geo_mean(ii)));
end

% Now convert to habitat suitability by dividing through
% by the number of species presence points and multiplying by 100
final_hs = (hs / length(species_geomeans)) * 100;

@author Chris Yesson
*/
class Enfa : public AlgorithmImpl
{
public:
    /** Constructor for Enfa */
    Enfa();
    /** This is the descructor for the Enfa class */
    ~Enfa();

    //
    // Methods used to build the model
    //

    /** Initialise the model specifying a threshold / cutoff point.
     * This is optional (model dependent).
     * @note This method is inherited from the Algorithm class
     * @return 0 on error
     */
    virtual int initialize();

    /** Start model execution (build the model).     
     * @note This method is inherited from the Algorithm class
     * @return 0 on error 
     */
    int iterate();
    
    /** Use this method to find out if the model has completed (e.g. convergence
     * point has been met. 
     * @note This method is inherited from the Algorithm class
     * @return     
     * @return Implementation specific but usually 1 for completion.
     */
    int done() const;
    
    //
    // Methods used to project the model
    //
    
    
    /** This method is used when projecting the model.  
     * @note This method is inherited from the Algorithm class
     * @return Scalar of the probablitiy of occurence    
     * @param Scalar *x a pointer to a vector of openModeller Scalar type 
     * (currently double). The vector should contain values looked up on 
     * the environmental variable layers into which the mode is being projected. */
    Scalar getValue( const Sample& x ) const;
    
    /** Returns a value that represents the convergence of the algorithm
     * expressed as a number between 0 and 1 where 0 represents model
     * completion. 
     * @return 
     * @param Scalar *val 
     */
    int getConvergence( Scalar * const val ) const;
    
    
    
private:
    
protected:  
    /** This is a utility function to convert a Sampler to a gsl_matrix.
     * @return 0 on error
     */
    int SamplerToMatrix();
    
    /** This is a utility function to convert a Sampler to a gsl_matrix.
     * @return 0 on error
     */
    int BackgroundToMatrix();
    
    /** This is a wrapper to call several of the methods below to generate the
     * initial model. */
    bool enfa1();
    
    /** Calculate the mean and standard deviation of the environment
     * variables at the occurence points.
     * @return 0 on error
     */
    int calculateMeanAndSd( gsl_matrix * theMatrix, 
			    gsl_vector * theMeanVector,
			    gsl_vector * theStdDevVector);
    
    /** Center and standardise.
     * Subtract the column mean from every value in each column
     * Divide each resultant column value by the stddev for that column
     * @note This method must be called after calculateMeanAndSd
     * @return 0 on error    
     */
    int center(gsl_matrix * theMatrix,
	       int spCount);
    
    /** Calculate square root of a gsl-matrix
	input must be positive semi-definite symmetric matrix */
    gsl_matrix* sqrtm(gsl_matrix* original_matrix) const;
    
    /** Calculate inverse of a matrix using cholesky decomposition
	input must be positive definite symmetric matrix */
    gsl_matrix* inverse(gsl_matrix* _m) const;

    /** Calculate the geometric mean of the distance from point v to 
	all species observation points in factored environmental space
	weighting each factor accordingly **/
    double getGeomean(gsl_vector* v) const;

    /** Discard unwanted components.
     * decide how many components to keep based on one of three methods
     * 0 - fixed number of components
     * 1 - keep components until a fixed amount of variation is explained
     * 2 - keep components showing greater variation than the broken stick model
     * returns the number of components to keep
     * @return 0 on error    
     */  
    int discardComponents() const;
    
    /** This a utility function to display the content of a gsl vector.
     * @param v gsl_vector Input vector
     * @param name char Vector name / message
     * @param roundFlag Whether to round numbers to 4 decimal places (default is true)
     */
    void displayVector(const gsl_vector * v, const char * name,const  bool roundFlag=true) const;
    
    /** This a utility function to display the content of a gsl matrix.
     * @param m gsl_matrix Input matrix
     * @param name char Matrix name / message
     * @param roundFlag Whether to round numbers to 4 decimal places (default is true)
     */
    void displayMatrix(const gsl_matrix * m, const char * name, const bool roundFlag=true) const;
    
    /** This a utility function to calculate the auto covariance of a gsl matrix.
     * @param m gsl_matrix Input matrix
     * @return gsl_matrix Output matrix
     */
    gsl_matrix * autoCovariance(gsl_matrix * m);
    
    /** Method to serialize a ENFA model.
     * @param ConfigurationPtr Pointer to the serializer object
     */
    virtual void _getConfiguration( ConfigurationPtr& ) const;
    
    /** Method to deserialize a ENFA model.
     * @param ConstConfigurationPtr Pointer to the serializer object
     */
    virtual void _setConfiguration( const ConstConfigurationPtr& );
    
    
    /** This is a flag to indicate that the algorithm was initialized. */
    int _initialized;
    /** This member variable is used to indicate whether the model 
     * building process has completed yet. */
    int _done;
    /** This is a pointer to a gsl matrix containing the 'looked up' environmental 
     * variables at each locality. It is converted to a gsl matrix from the oM 
     * Sampler.samples primitive structure. */
    gsl_matrix * _gsl_environment_matrix;
    /* copy of _gsl_environment_matrix. */
    gsl_matrix * _gsl_environment_matrix_original;
    /* Pointer to gsl matrix with environmental data for each locality 
       converted to factors using the enfa score matrix */
    gsl_matrix * _gsl_environment_factor_matrix;
    
    /** This is a pointer to a gsl matrix containing the 'looked up' environmental 
     * variables at each background sample. It is converted to a gsl matrix from the oM 
     * Sampler.samples primitive structure. */
    gsl_matrix * _gsl_background_matrix;
    
    /** This is a pointer to a gsl matrix that will hold the covariance matrix generated
	from the environmental data matrix */
    gsl_matrix * _gsl_covariance_matrix;
    
    /** This is a pointer to a gsl matrix that will hold the covariance matrix generated
	from the background data matrix */
    gsl_matrix * _gsl_covariance_background_matrix;
    
    /** This is a pointer to a gsl vector that will hold the mean of each environmental 
	variable column */
    gsl_vector * _gsl_avg_vector;
    /** This is a pointer to a gsl vector that will hold the stddev of each environmental variable column */
    gsl_vector * _gsl_stddev_vector;
    
    /** This is a pointer to a gsl vector that will hold the mean of each
	background environmental variable column */
    gsl_vector * _gsl_avg_background_vector;
    
    /** This is a pointer to a gsl vector that will hold the stddev of each
	background environmental variable column */
    gsl_vector * _gsl_stddev_background_vector;
    
    /** This is a pointer to a gsl vector that will hold the eigen values */
    gsl_vector * _gsl_eigenvalue_vector;
    /** This is a pointer to a gsl matrix that will hold the eigen vectors */
    gsl_matrix * _gsl_eigenvector_matrix;
    /** Dimension of environmental space. */
    
    gsl_matrix * _gsl_score_matrix ;
    /** score matrix **/
    
    gsl_matrix * _gsl_covariance_matrix_root_inverse;
    /** store for transformation of covariance matrix (used repeatedly) **/
    gsl_matrix * _gsl_workspace_H;
    /** gsl matrix used to store transitional phase of enfa calculation **/
    gsl_matrix * _gsl_workspace_W;
    /** gsl matrix used to store transitional phase of enfa calculation **/
    gsl_matrix * _gsl_workspace_y;
    /** gsl vector used to store transitional phase of enfa calculation **/
    gsl_vector * _gsl_workspace_z;
    /** gsl vector to store the weights of the original factors **/
    gsl_vector* _gsl_factor_weights_all_components;
    /** gsl vector to store the weights of the final factors **/
    gsl_vector* _gsl_factor_weights;
    /** gsl vector to store the geometric mean distance of each locality 
	from every other locality based on the factored data **/
    gsl_vector* _gsl_geomean_vector;
    
    int _layer_count; 
    /** Number of components that are actually kept after Keiser-Gutman test */
    int _retained_components_count;
    /** the number of localities used to construct the model */
    int _localityCount; 
    
    /** the number of sample background points used to construct the model */
    int _backgroundCount; 
    
    /** flag identifying the method for component selection **/
    int _discardMethod; 

    /** the number of components to keep */
    int _retainComponents; 

    /** the percent variation for selection of components to retain */
    int _retainVariation; 

    /** Minumum number of components required for a valid model */
    int minComponentsInt;
    
    /** verbose debugging to print lots of details */
    int _verboseDebug; 

    /** find the index of the minimum vector **/
    int _gsl_vector_min;
    
    /** model detail - marginality **/
    double _marginality;

    /** model detail - specialisation **/
    double _specialisation;

    /** number of times to retry failing model **/
    int _numRetries;

    /** number of times currently retrying failing model **/
    int _retryCount;


};

/* catch the case that generating an inverse fails
   gsl doesnt do this well, and we want to catch, 
   ignore and retry when this happens */
class InverseFailedException : public OmException {
public:
  InverseFailedException( const std::string& msg ) :
    OmException( msg )
  {}
};

#endif
