/**
 * Declaration of CallbackWrapper class
 * 
 * @author Renato De Giovanni
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2008 by CRIA -
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

#ifndef _OM_CALLBACK_WRAPPER_
#define _OM_CALLBACK_WRAPPER_

#include <openmodeller/om_defs.hh>

/** This class is used to wrap openModeller callbacks since they may need to be called with  
 *  an extra custom parameter. This class stores the extra parameter and passes it whenever 
 *  the callback needs to be called. 
 */
class CallbackWrapper {

  public: 

    /** Default constructor */
    CallbackWrapper();

    /** Destructor */
    ~CallbackWrapper() {};

    /** Set a model creation callback with an optional additional parameter.
     * @param func Model creation callback.
     * @param param Extra parameter to be passed to the callback.
     */
    void setModelCreationCallback( ModelCreationCallback func, void *param );

    /** Set a model projection callback with an optional additional parameter.
     * @param func Model projection callback.
     * @param param Extra parameter to be passed to the callback.
     */
    void setModelProjectionCallback( ModelProjectionCallback func, void *param );

    /** Set a job abortion callback with an optional additional parameter.
     * @param func Job abortion callback.
     * @param param Extra parameter to be passed to the callback.
     */
    void setAbortionCallback( AbortionCallback func, void *param );

    /** Method used only by model creation callbacks to indicate job progress.
     * @param progress Model creation progress.
     */
    void notifyModelCreationProgress( float progress );

    /** Method used only by model projection callbacks to indicate job progress.
     * @param progress Model projection progress.
     */
    void notifyModelProjectionProgress( float progress );

    /** Method used only by abortion callbacks to indicate that the current job needs
     *  to be aborted.
     * @return True to indicate that abortion was requested, false otherwise.
     */
    bool abortionRequested();

  private:

    // Model creation callback
    ModelCreationCallback _model_creation_callback;

    // Pointer to extra parameter in model creation callbacks
    void * _model_creation_arg;

    // Model projection callback
    ModelProjectionCallback _model_projection_callback;

    // Pointer to extra parameter in model projection callbacks
    void * _model_projection_arg;

    // Job abortion callback
    AbortionCallback _abortion_callback;

    // Pointer to extra parameter in abortion callbacks
    void * _abortion_arg;
};

#endif
