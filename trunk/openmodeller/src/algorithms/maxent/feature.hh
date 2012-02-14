/**
 * Declaration of Feature Interface
 * 
 * @author Renato De Giovanni
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2011 by CRIA -
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

#ifndef _FEATURE_HH
#define _FEATURE_HH

#include <openmodeller/Configurable.hh>
#include <openmodeller/Sample.hh>
#include <openmodeller/Environment.hh>

#include <string>

#define F_LINEAR 0
#define F_QUADRATIC 1
#define F_PRODUCT 2
#define F_HINGE 3
#define F_THRESHOLD 4

/**
 * Documents this.
 */
class Feature : public Configurable {

public:

  virtual Scalar getVal( const Sample& sample ) const = 0;

  virtual bool isBinary() const = 0;

  virtual std::string getDescription( const EnvironmentPtr& env ) const = 0;

  Scalar type() {return _type;}

  void activate(){_active = 1;}
  void deactivate(){_active = 0;}

  bool isActive() {return _active;}

  void setLower( Scalar lower ){_lower = lower;}
  Scalar lower() {return _lower;}

  void setUpper( Scalar upper ){_upper = upper;}
  Scalar upper() {return _upper;}

  void setMean( Scalar mean ){_mean = mean;}
  Scalar mean() {return _mean;}

  void setStd( Scalar std ){_std = std;}
  Scalar std() {return _std;}

  void setExp( Scalar exp ){_exp = exp;}
  Scalar exp() {return _exp;}

  void setSampExp( Scalar samp_exp ){_samp_exp = samp_exp;}
  Scalar sampExp() {
    // Log::instance()->debug("samp_exp() called\n");
    Scalar retvalue;

    retvalue = _samp_exp;
    // Log::instance()->debug("samp_exp() returned %f\n", retvalue);
    return retvalue;
  }

  void setSampDev( Scalar samp_dev ){_samp_dev = samp_dev;}
  Scalar sampDev() {
    // Log::instance()->debug("samp_dev() called\n");
    Scalar retvalue;

    retvalue = _samp_dev;
    // Log::instance()->debug("samp_dev() returned %f\n", retvalue);
    return retvalue;
  }

  void setLambda( Scalar lambda ){_lambda = lambda;}
  Scalar lambda() {
    // Log::instance()->debug("lambda() called\n");
    Scalar retvalue;
    
    retvalue = _lambda;
    // Log::instance()->debug("lambda() returned %f\n", retvalue);
    return _lambda;
  }

  void setPrevLambda( Scalar lambda ){_prevLambda = lambda;}
  Scalar prevLambda() {return _prevLambda;}

  void setBeta( Scalar beta ){_beta = beta;}
  Scalar beta() {return _beta;}

  void setLastExpChange( int last_exp_change ){_last_exp_change = last_exp_change;}
  int lastExpChange() {return _last_exp_change;}

protected:

  Feature(){_active = 1; _lower = 0.0; _upper = 0.0;_mean = 0.0; _std = 0.0; _exp = 0.0; _samp_exp = 0.0; _samp_dev = 0.0; _lambda = 0.0; _prevLambda = 0.0; _last_exp_change = -1;}

  int _type;
  bool _active;
  Scalar _lower;
  Scalar _upper;
  Scalar _mean;
  Scalar _std;
  Scalar _exp;
  Scalar _samp_exp;
  Scalar _samp_dev;
  Scalar _lambda;
  Scalar _prevLambda;
  Scalar _beta;
  int _last_exp_change;
};

#endif
