/*
TerraLib - a library for developing GIS applications.
Copyright  2001, 2002, 2003 INPE and Tecgraf/PUC-Rio.

This code is part of the TerraLib library.
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

You should have received a copy of the GNU Lesser General Public
License along with this library.

The authors reassure the license terms regarding the warranties.
They specifically disclaim any warranties, including, but not limited to,
the implied warranties of merchantability and fitness for a particular
purpose. The library provided hereunder is on an "as is" basis, and the
authors have no obligation to provide maintenance, support, updates,
enhancements, or modifications.
In no event shall INPE be held liable to any party
for direct, indirect, special, incidental, or consequential damages arising
out of the use of this library and its documentation.
*/

#ifndef PREALGORITHM_HH
  #define PREALGORITHM_HH

  #include <openmodeller/om.hh>
  #include "PreParameters.hh"

/**
 * @brief This is the base class for digital image processing algorithms.
 * @author Emiliano F. Castejon <castejon@dpi.inpe.br>
 * adapted from PDI (Digital Image Processing) to OM Pre-analysis (Missae & Emiliano - DPI/INPE - 2008/April)
 */
  class dllexp PreAlgorithm
  {
    public :

      /**
       * @brief Default Destructor
       */
      virtual ~PreAlgorithm();

      /**
       * @brief Applies the algorithm following the current state and
       * internal stored parameters.
       *
       * @return true if OK. false on error.
       */
      bool Apply();

      /**
       * @brief Reset the internal state with new supplied parameters.
       *
       * @param params The new supplied parameters.
       * @return true if parameters OK, false on error.
       */
      bool Reset( const PreParameters& params );

      /**
       * @brief Checks if the supplied parameters fits the requirements of each
       * PDI algorithm implementation.
       *
       * @note Error log messages must be generated. No exceptions generated.
       *
       * @param parameters The parameters to be checked.
       * @return true if the parameters are OK. false if not.
       */
      virtual bool CheckParameters( const PreParameters& parameters ) const = 0;

      /**
       * @brief Returns a reference to the current internal parameters.
       *
       * @return A reference to the current internal parameters.
       */
      const PreParameters& GetParameters() const;
      
    protected :
      /**
       * @brief Internal parameters reference
       */
      mutable PreParameters params_;
        
      /**
       * @brief Default Constructor
       */
      PreAlgorithm();

      /**
       * @brief Runs the current algorithm implementation.
       *
       * @return true if OK. false on error.
       */
      virtual bool RunImplementation() = 0;

      /**
       * @brief Reset params state to the params_ state.
       */
      virtual void ResetState( PreParameters& params ) = 0;
     
      /**
       * @brief Checks if current internal parameters fits the requirements of each
       * PDI algorithm implementation.
       *
       * @note Error log messages must be generated. No exceptions generated.
       *
       * @return true if the internal parameters are OK. false if not.
       */
      bool CheckInternalParameters() const;
      
    private :
    
      /**
       * @brief Alternative constructor.
       *
       * @note Algorithms cannot be copied.
       */    
      PreAlgorithm( const PreAlgorithm& );
    
      /**
       * @brief Operator = overload.
       *
       * @note Algorithms cannot be copied.
       *
       * @param external External algorithm reference.
       * @return A const Algorithm reference.
       */
      const PreAlgorithm& operator=( const PreAlgorithm& external );
  };

#endif //PREALGORITHM_HPP
