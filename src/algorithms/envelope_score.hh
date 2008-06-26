/**
 * Declaration of EnvelopeScoreatic Envelope Algorithm - Nix, 1986.
 * 
 * @author Mauro Muñoz <mauro@cria.org.br>
 * @author EnvelopeScore_or implemantation by Tim Sutton and Chris Yesson
 * @date 2007-08-22
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Based on Bioclim alg 
 * Copyright(c) 2004 by CRIA -
 * Centro de Referência em Informação Ambiental
 *
 * http://www.cria.org.br
 * 
 * Envelope Score implementation (c) 2007 Chris Yesson and Tim Sutton 
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

#ifndef _ENVELOPESCOREHH_
#define _ENVELOPESCOREHH_

#include <openmodeller/om.hh>

/****************************************************************/
/*********************** EnvelopeScore ***********************/

/** 
 * Bioclimatic Envelope Algorithm with occurrence probability
 * based on the number variables within the min/max  range
 *
 */
class EnvelopeScore : public AlgorithmImpl
{
public:

  EnvelopeScore();
  ~EnvelopeScore();

  // Inherited from Algorithm class.
  int initialize();
  int iterate();
  int done() const;

  Scalar getValue( const Sample& x ) const;
  int    getConvergence( Scalar * const val ) const;

protected:
  virtual void _getConfiguration( ConfigurationPtr& ) const;
  virtual void _setConfiguration( const ConstConfigurationPtr & );

private:

  /** compute the statistic member variables based on
      the data in the OccurrencesPtr */
  void computeStats( const OccurrencesPtr& );

  /** Log the bioclimatic envelopes information. */
  void logEnvelope();

  bool _done;  ///> is true if the algorithm is finished.

  Sample _minimum; ///> Mininum value for each variable.
  Sample _maximum; ///> Maximum value for each variable.

};


#endif //ENVELOPESCOREHH

