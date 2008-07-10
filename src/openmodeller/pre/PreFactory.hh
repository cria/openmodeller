/**
* Declaration of class PreFactory
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
#ifndef  PRE_FACTORY_HH
#define  PRE_FACTORY_HH

#include <map>
#include <string>

using namespace std;

//adapted from Terralib to OM Pre-analysis ( Missae DPI/INPE )

/**
  Implements a template for the "PreFactory" pattern.

  The "factory" pattern is a technique for creating new
  instances of objects which defines an abstract interface,
  (represented by the "Make" module). The subclasses of
  factory decide which class to instantiate.
  
  Each subclass of factory "registers" itself at compile time;
  therefore, the addition of a new factory requires no change
  the parent class. 

  author: Gilberto Camara
*/
template <class T, class Arg>
class PreFactory
{
public:

    // Dictionary of factories (indexed by name)
	typedef map<string, PreFactory<T,Arg>* > PreFactoryMap; 

    // Returns the single instance of the factory dictionay
	static PreFactoryMap& instance ()
	{ 
		static PreFactoryMap Fmap_;
		return Fmap_;
		
	} 
	
	// Normal Constructor
	PreFactory (const string& factoryName);

	virtual ~PreFactory() {}

	// Virtual constructor using name
	static T* make  ( string name, const Arg& arg );

protected:

	// Builds a new type (should be implemented by descendants)
    virtual T* build ( const Arg& arg ) = 0;

private:
	string  Fname_;
};

// Constructor 
template <class T, class Arg>
PreFactory<T,Arg>::PreFactory(const string& name):
	Fname_(name)
{
	PreFactory<T,Arg>::instance()[name] = this;
}
  
// Builds an object, based on the input parameters
template <class T, class Arg> 
T*
PreFactory<T,Arg>::make ( string name, const Arg& arg )
{
	// try to find the name on the factory dictionary
	typename PreFactoryMap::iterator i = PreFactory<T,Arg>::instance().find ( name );

	// Not found ?  
	if ( i == PreFactory<T,Arg>::instance().end() )
	{
		std::string msg = "PreFactory<T,Arg>::make( string name, const Arg& arg ): Not found the name on the factory\n";
		Log::instance()->error( msg.c_str() );
		throw InvalidParameterException( msg );
	}

	// Create an object, based on the input parameters
	return (*i).second->build ( arg );
	return 0;

} 

#endif

