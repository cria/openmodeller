/**
 * Declaration of AbortionCommand class
 * 
 * @author Renato De Giovanni
 * @date 2007-07-23
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2007 by CRIA -
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

#ifndef _OM_ABORTIONCOMMANDHH_
#define _OM_ABORTIONCOMMANDHH_

/** Abortion command interface which reflects how openModeller checks 
 *  internally if the current job needs to be aborted. 
 */
class AbortionCommand {

  public: 

    virtual ~AbortionCommand() {};

    /** Indicates if the current job needs to be aborted
     * @return True to request abortion, false otherwise.
     */
    virtual bool operator()() = 0;
};

#endif
