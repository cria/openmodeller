/**
 * Definition of RF algorithm class.
 * 
 * @author Renato De Giovanni (renato [at] cria org br)
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2010 by CRIA -
 * Centro de Referencia em Informacao Ambiental
 *
 * http://www.cria.org.br
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

#include "rf_alg.hh"

#include <openmodeller/Sampler.hh>
#include <openmodeller/Exceptions.hh>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#include "librf/instance_set.h"
#include "librf/tree.h"
#include "librf/weights.h"

using namespace std;
using namespace librf;

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 3

#define NUMTREES_ID   "NumTrees"
#define K_ID          "VarsPerTree"
#define UNSUP_ID      "ForceUnsupervisedLearning"

#define RF_LOG_PREFIX "RfAlgorithm: "

/******************************/
/*** Algorithm's parameters ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // Number of Trees
  {
    NUMTREES_ID,        // Id.
    "Number of trees",  // Name.
    Integer,            // Type.
    "Number of trees",  // Overview
    "Number of trees",  // Description.
    1,         // Not zero if the parameter has lower limit.
    1,         // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    1000,      // Parameter's upper limit.
    "10"       // Parameter's typical (default) value.
  },
  // Number of variables per tree
  {
    K_ID,     // Id.
    "Number of variables per tree", // Name.
    Integer,  // Type.
    "Number of variables per tree (zero defaults to the square root of the number of layers)", // Overview
    "Number of variables per tree (zero defaults to the square root of the number of layers)", // Description.
    0,         // Not zero if the parameter has lower limit.
    0,         // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0,         // Parameter's upper limit.
    "0"        // Parameter's typical (default) value.
  },
  // Force unsupervised learning
  {
    UNSUP_ID,     // Id.
    "Force unsupervised learning", // Name.
    Integer,  // Type.
    "Force unsupervised learning", // Overview
    "When absence points are provided, this parameter can be used to ignore them forcing unsupervised learning. Note that if no absences are provided, unsupervised learning will be used anyway.", // Description.
    1,         // Not zero if the parameter has lower limit.
    0,         // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    1,         // Parameter's upper limit.
    "0"        // Parameter's typical (default) value.
  },
};

/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "RF", 	     // Id.
  "Random Forests",  // Name.
  "0.2",       	     // Version.

  // Overview
  "Random Forests",

  // Description.
  "Random Forests",

  "Leo Breiman & Adele Cutler", // Algorithm author.
  "Breiman, L. (2001). Random forests. Machine Learning, 45, 5–32.", // Bibliography.

  "Renato De Giovanni", // Code author.
  "renato [at] cria . org . br", // Code author's contact.

  0, // Does not accept categorical data.
  0, // Does not need (pseudo)absence points.

  NUM_PARAM, // Algorithm's parameters.
  parameters
};

/****************************************************************/
/****************** Algorithm's factory function ****************/

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new RfAlgorithm();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}


/*********************************************/
/************** SVM algorithm ****************/

/*******************/
/*** constructor ***/

RfAlgorithm::RfAlgorithm() :
  AlgorithmImpl( &metadata ),
  _done( false ),
  _initialized( false )
{
}


/******************/
/*** destructor ***/

RfAlgorithm::~RfAlgorithm()
{
  if ( _initialized ) {

    delete _set;
  }

  for ( unsigned int i = 0; i < _trees.size(); ++i ) {

    delete _trees[i];
  }
}

/**************************/
/*** need Normalization ***/
int RfAlgorithm::needNormalization()
{
  return 0;
}

/******************/
/*** initialize ***/
int
RfAlgorithm::initialize()
{
  int num_layers = _samp->numIndependent();

  // Number of trees
  if ( ! getParameter( NUMTREES_ID, &_num_trees ) ) {

    Log::instance()->error( RF_LOG_PREFIX "Parameter '" NUMTREES_ID "' not passed.\n" );
    return 0;
  }

  if ( _num_trees < 1 ) {

    Log::instance()->error( RF_LOG_PREFIX "Parameter '" NUMTREES_ID "' must be greater than zero.\n" );
    return 0;
  }

  // Don't allow people to use too much memory
  if ( _num_trees > 1000 ) {

    Log::instance()->error( RF_LOG_PREFIX "Parameter '" NUMTREES_ID "' is greater than 1000.\n" );
    return 0;
  }

  _trees.reserve( _num_trees );

  // Number of variables per tree
  if ( ! getParameter( K_ID, &_k ) ) {

    Log::instance()->error( RF_LOG_PREFIX "Parameter '" K_ID "' not passed.\n" );
    return 0;
  }

  if ( _k < 1 ) {

    _k = int( sqrt( double( num_layers ) ) );
  }

  // Unsupervised learning
  bool force_unsupervised_learning = false;
  int unsup;
  if ( getParameter( UNSUP_ID, &unsup ) && unsup == 1 ) {

    force_unsupervised_learning = true;
  }

  _class_weights.resize(2, 1);

  // Check the number of presences
  int num_presences = _samp->numPresence();

  if ( num_presences == 0 ) {

    Log::instance()->warn( RF_LOG_PREFIX "No presence points inside the mask!\n" );
    return 0;
  }

  // Load input points

  unsigned int seed = (unsigned int)_rand.get();

  stringstream sdata("");
  stringstream slabels("");

  OccurrencesImpl::const_iterator p_iterator;
  OccurrencesImpl::const_iterator p_end;

  OccurrencesPtr presences = _samp->getPresences();

  p_iterator = presences->begin();
  p_end = presences->end();

  while ( p_iterator != p_end ) {

    Sample presence = (*p_iterator)->environment();

    _sampleToLine( presence, sdata );

    slabels << "0" << endl; // presence

    ++p_iterator;
  }

  if ( _samp->numAbsence() && ! force_unsupervised_learning ) {

    OccurrencesPtr absences = _samp->getAbsences();

    p_iterator = absences->begin();
    p_end = absences->end();

    while ( p_iterator != p_end ) {

      Sample absence = (*p_iterator)->environment();

      _sampleToLine( absence, sdata );

      slabels << "1" << endl; // absence

      ++p_iterator;
    }

    istream data( sdata.rdbuf() );
    istream labels( slabels.rdbuf() );

    _set = InstanceSet::load_csv_and_labels( data, labels );
  }
  else {

    // Prepare for unsupervised learning

    istream data( sdata.rdbuf() );

    _set = InstanceSet::load_unsupervised( data, &seed );
  }

  _initialized = true;

  return 1;
}


/**********************/
/*** sample to line ***/
void
RfAlgorithm::_sampleToLine( Sample sample, stringstream& ss ) const
{
  for ( unsigned int j = 0; j < sample.size(); ++j ) {

    ss << sample[j] << ",";
  }

  ss << endl;
}


/***************/
/*** iterate ***/
int
RfAlgorithm::iterate()
{
  if ( (int)_trees.size() < _num_trees ) {

    weight_list* w = new weight_list( _set->size(), _set->size());

    // sample with replacement
    for ( unsigned int j = 0; j < _set->size(); ++j ) {

      int instance = _rand.get( 0, _set->size() - 1 );
      w->add( instance, _class_weights[_set->label(instance)] );
    }

    Tree* tree = new Tree( *_set, w, _k, 1, 0, _rand.get(1000) );
    tree->grow();

    _trees.push_back(tree);
  }
  else {

    _done = true;
  }

  return 1;
}

/********************/
/*** get Progress ***/
float RfAlgorithm::getProgress() const
{
  if ( done() ) {

    return 1.0;
  }

  return (float)_trees.size() / (float)_num_trees;
}


/************/
/*** done ***/
int
RfAlgorithm::done() const
{
  return _done;
}

/*****************/
/*** get Value ***/
Scalar
RfAlgorithm::getValue( const Sample& x ) const
{
  stringstream sdata("");

  _sampleToLine( x, sdata );

  istream data( sdata.rdbuf() );

  stringstream slabels("0");

  istream labels( slabels.rdbuf() );

  InstanceSet* set = InstanceSet::load_csv_and_labels( data, labels );

  DiscreteDist votes;

  for ( unsigned int i = 0; i < _trees.size(); ++i ) {

    int predict = _trees[i]->predict( *set, 0 );
    votes.add( predict );
  }

  float prob = votes.percentage(0);

  delete set;

  return (double)prob;
}

/***********************/
/*** get Convergence ***/
int
RfAlgorithm::getConvergence( Scalar * const val ) const
{
  *val = 1.0;
  return 1;
}

/****************************************************************/
/****************** configuration *******************************/
void
RfAlgorithm::_getConfiguration( ConfigurationPtr& config ) const
{
  if ( ! _done )
    return;

  ConfigurationPtr model_config( new ConfigurationImpl("Rf") );
  config->addSubsection( model_config );

  model_config->addNameValue( "Trees", _num_trees );
  model_config->addNameValue( "K", _k );

  Tree* p_tree = NULL;
  tree_node* p_node = NULL;

  unsigned int num_nodes;

  char buffer[5];

  for ( int i=0; i < _num_trees; ++i ) {

    p_tree = _trees[i];

    ConfigurationPtr tree_config( new ConfigurationImpl("Tree") );

    num_nodes = p_tree->num_nodes();

    tree_config->addNameValue( "Nodes", (int)num_nodes );

    sprintf( buffer, "%4.2f", p_tree->training_accuracy() );

    tree_config->addNameValue( "Accuracy", buffer );
    tree_config->addNameValue( "Split", (int)p_tree->num_split_nodes() );
    tree_config->addNameValue( "Terminal", (int)p_tree->num_terminal_nodes() );

    for ( unsigned int j= 0; j < num_nodes; ++j ) {
    
      p_node = p_tree->get_node( j );    

      ConfigurationPtr node_config( new ConfigurationImpl("Node") );

      librf::NodeStatusType status = p_node->status;

      node_config->addNameValue( "Status", (int)status );

      if ( status == SPLIT ) {

        node_config->addNameValue( "L", (int)p_node->left );
        node_config->addNameValue( "R", (int)p_node->right );
        node_config->addNameValue( "A", (int)p_node->attr );
        node_config->addNameValue( "S", (float)p_node->split_point );
      }
      else if ( status == TERMINAL ) {

        node_config->addNameValue( "V", (char)p_node->label );
      }

      tree_config->addSubsection( node_config ); 
    }

    model_config->addSubsection( tree_config ); 
  }  
}

void
RfAlgorithm::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "Rf", false );

  if ( ! model_config )
    return;

  _num_trees = model_config->getAttributeAsInt( "Trees", 0 );

  _k = model_config->getAttributeAsInt( "K", 0 );

  _trees.reserve( _num_trees );

  Configuration::subsection_list trees = model_config->getAllSubsections();

  Configuration::subsection_list::iterator tree = trees.begin();
  Configuration::subsection_list::iterator last_tree = trees.end();

  for ( ; tree != last_tree; ++tree ) {

    if ( (*tree)->getName() != "Tree" ) {

      continue;
    }

    Tree* my_tree = new Tree();

    Configuration::subsection_list nodes = (*tree)->getAllSubsections();

    Configuration::subsection_list::iterator node = nodes.begin();
    Configuration::subsection_list::iterator last_node = nodes.end();

    for ( ; node != last_node; ++node ) {

      if ( (*node)->getName() != "Node" ) {

        continue;
      }

      int status = (*node)->getAttributeAsInt( "Status", 0 );

      tree_node my_node;
      
      if ( status == SPLIT ) {

        my_node.status = SPLIT;
        my_node.left = (*node)->getAttributeAsInt( "L", 0 );
        my_node.right = (*node)->getAttributeAsInt( "R", 0 );
        my_node.attr = (*node)->getAttributeAsInt( "A", 0 );
        double split_point = (*node)->getAttributeAsDouble( "S", 0.0 );
        my_node.split_point = (float)split_point;
      }
      else if ( status == TERMINAL ) {

        my_node.status = TERMINAL;
        int label = (*node)->getAttributeAsInt( "V", 0 );
        my_node.label = uchar(label);
      }
      else {

        continue;
      }

      my_tree->add_node( my_node );
    }

    _trees.push_back( my_tree );
  }

  _initialized = true;

  _done = true;
}
