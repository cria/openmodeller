/**
 * Test class for ChiSquare
 *
 * adapted from om_test_chisquare (Missae - DPI/INPE - 2008/April)
 * 
 * @authors Renato De Giovanni and Albert Massayuki Kuniyoshi 
 * $Id: om_test_chisquare.h 4118 2008-03-12 17:18:58Z rdg $
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

/** \ingroup test
 * \brief Test for Configuration Class
 */


#ifndef TEST_CHISQUARE_HH
#define TEST_CHISQUARE_HH

class MyLog : public Log::LogCallback 
{
  void operator()( Log::Level l, const std::string& msg ) 
  {
    std::cout << msg;
  }
};

#endif

