/**
* Declaration of class PreAlgorithm
*
* @author Missae Yamamoto (missae at dpi . inpe . br)
* $Id$
*
* LICENSE INFORMATION
* 
* Copyright(c) 2008 by INPE -
* Instituto Nacional de Pesquisas Espaciais
*
* http://www.inpe.br
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

#ifndef PRE_ALGORITHM_HH
  #define PRE_ALGORITHM_HH

  #include <openmodeller/om.hh>
  #include "PreParameters.hh"
  #include <map>

 // adapted from Terralib PDI (Digital Image Processing) to OM Pre-analysis (Missae & Emiliano - DPI/INPE)
  class dllexp PreAlgorithm
  {
    public :

      //Default Destructor
      virtual ~PreAlgorithm();

      //return a default object.
      static PreAlgorithm* DefaultObject( const PreParameters& params )
	  {throw;};

      /**
       * Applies the algorithm following the current state and
       * internal stored parameters.
       *
       * return true if OK. false on error.
       */
      bool apply();

      /**
       * Reset the internal state with new supplied parameters.
       *
       * params: The new supplied parameters.
       * return true if parameters OK, false on error.
       */
      bool reset( const PreParameters& params );

      /**
       * Reset the supplied parameters with internal state.
       *
       * params: The supplied parameters. 
       */
       void resetState( PreParameters& params );
 
      /**
       * Checks if the supplied parameters fits the requirements of each
       * PRE algorithm implementation.
       *
       * note: Error log messages must be generated. No exceptions generated.
       *
       * parameters: The parameters to be checked.
       * return true if the parameters are OK. false if not.
       */
      virtual bool checkParameters( const PreParameters& parameters ) const = 0;

      //Returns a reference to the current internal parameters.
      const PreParameters& getParameters() const;

      //get input information
      typedef std::map<string, string> stringMap;
      virtual void getAcceptedParameters( stringMap& info) = 0;

      //get output information
      virtual void getLayersetResultSpec ( stringMap& info) = 0;
     
      //get output information for each layer
      virtual void getLayerResultSpec ( stringMap& info) = 0;
      
      /**
       * Method used to retrieve statistics calculated for a specified layer.
       *
       * @param layer_id Layer id.
       * @param result PreParameters object where the results are stored
       * @return PreParamers object associated with the specified layer.
       */
      void getLayerResult( const string layer_id, PreParameters& result );

    protected :

      //Internal parameters reference
      mutable PreParameters params_;

      // Layer id => PreParams obj whose keys can be discovered by calling getLayerResultSpec
      mutable std::map<string, PreParameters> result_by_layer_;

      //Default Constructor
      PreAlgorithm();

      /**
       * Runs the current algorithm implementation.
       *
       * return true if OK. false on error.
       */
      virtual bool runImplementation() = 0;

      /**
       * Checks if current internal parameters fits the requirements of each
       * PRE algorithm implementation.
       *
       * note: Error log messages must be generated. No exceptions generated.
       *
       * return true if the internal parameters are OK. false if not.
       */
      bool checkInternalParameters() const;
      
    private :
    
      /**
       * Alternative constructor.
       *
       * note: Algorithms cannot be copied.
       */    
      PreAlgorithm( const PreAlgorithm& );
    
      /**
       * Operator = overload.
       *
       * note: Algorithms cannot be copied.
       *
       * external: External algorithm reference.
       * return: A const Algorithm reference.
       */
      const PreAlgorithm& operator=( const PreAlgorithm& external );
  };

#endif //PREALGORITHM_HPP
