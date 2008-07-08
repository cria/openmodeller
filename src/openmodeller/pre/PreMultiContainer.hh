
/**
* Declaration of class PreMultiContainer
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

#ifndef PRE_MULTICONTAINER_HH
  #define PRE_MULTICONTAINER_HH

  #include <openmodeller/Log.hh>
  #include <openmodeller/Exceptions.hh>
  
  #include <time.h>
  
  #include <vector>
  #include <map>
  #include <typeinfo>

  //adapted from Terralib PDI (Digital Image Processing) to OM Pre-analysis ( Missae & Emiliano - DPI/INPE )
  //Multi-container node interface.
  //author Emiliano F. Castejon <castejon@dpi.inpe.br>
  template< typename PreMultiContainerKeyT >
  class MCNodeInterface {
    public :
    
      //Default Constructor.    
      MCNodeInterface() {};
      
      //Default Destructor.  
      virtual ~MCNodeInterface() {};
      
      /**
       * Copies the current node instance by calling
       * the operator= from the contained object instance.
       * return: A pointer to the new node copy.
       */            
      virtual MCNodeInterface< PreMultiContainerKeyT >* clone() 
        const = 0;
        
      /**
       * Get the internal object type info.
       * return: The internal object type info.
       */        
      virtual const std::string& getObjTypeInfo() const = 0;
      
    private :
    
      //Alternative constructor.  
      MCNodeInterface( 
        const MCNodeInterface< PreMultiContainerKeyT >& ) {};    
    
      /**
      * operator= overload.
      * return: The external instance reference.
      */
      const MCNodeInterface< PreMultiContainerKeyT >& operator=( 
        const MCNodeInterface< PreMultiContainerKeyT >& ) {};        
  };

  //adapted from Terralib PDI (Digital Image Processing) to OM Pre-analysis ( Missae & Emiliano - DPI/INPE )
  // Multi-container node class.
  //author Emiliano F. Castejon <castejon@dpi.inpe.br>
  template< typename ObjectT, typename PreMultiContainerKeyT >
  class MCNode : public MCNodeInterface< PreMultiContainerKeyT > {
    public :
    
      //Default Constructor.
      MCNode() 
      {
        obj_ptr_ = 0;
      };
      
      //Default Destructor.
      ~MCNode()
      {
        if( obj_ptr_ ) {
          delete obj_ptr_;
        }
      };
      
      /**
       * Copies the the current node instance by calling
       * the operator= from the contained object instance.
       * return: A pointer to the new node copy.
       */              
      MCNodeInterface< PreMultiContainerKeyT >* clone() const
      {
        MCNode< ObjectT, PreMultiContainerKeyT >* new_node_ptr =
          new MCNode< ObjectT, PreMultiContainerKeyT >;
          
        if( obj_ptr_ ) {
          new_node_ptr->obj_ptr_ = new ObjectT;
          ( *( new_node_ptr->obj_ptr_ ) ) = ( *obj_ptr_ );
          
          new_node_ptr->obj_type_str_ = obj_type_str_;
        }
        
        return new_node_ptr;
      };
      
      /**
       * Set the internal object pointer.
       * ptr: Object pointer.
       */        
      void setObjPtr( ObjectT* ptr ) 
      { 
		if( !ptr ) {
			std::string msg = "MCNode::setObjPtr: Invalid pointer.\n";
			Log::instance()->error( msg.c_str() );
			throw InvalidParameterException( msg );
		}
        
        if( obj_ptr_ ) {
          delete obj_ptr_;
        }      
      
        obj_ptr_ = ptr;
        obj_type_str_ = std::string( typeid( *ptr ).name() );
      };
      
      /**
       * Get the internal object pointer.
       * return: The object pointer.
       */        
      ObjectT* getObjPtr() const
      { 
        return obj_ptr_; 
      };
      
      /**
       * Get the internal object type info.
       * return: The internal object type info.
       */        
      const std::string& getObjTypeInfo() const
      { 
        return obj_type_str_; 
      };      
      
    protected :
      
 
      //The internal object pointer.
      ObjectT* obj_ptr_;
      
      //The internal object type.
      std::string obj_type_str_;      
      
  }; 

  //adapted from Terralib PDI (Digital Image Processing) to OM Pre-analysis ( Missae & Emiliano - DPI/INPE )
  //A container class to store multiple types os object copies.
  //author Emiliano F. Castejon <castejon@dpi.inpe.br>
  //note: This is a thread-safe class.
  //note: Do not use this class with polymorphic types !!
  template< typename PreMultiContainerKeyT >
  class PreMultiContainer {
  
    public :
    
      //brief Default Constructor
      PreMultiContainer();
      
      //Alternative Constructor
      //external: External reference.
      PreMultiContainer( 
        const PreMultiContainer< PreMultiContainerKeyT >& external );      

      //Default Destructor
      ~PreMultiContainer();
                           
      /**
       * Operator == overload.
       *
       * ext_instance: External instance reference.
       * return true if this instance have the same internal
       * update time that the external instance.
       */
      bool operator==( 
        const PreMultiContainer< PreMultiContainerKeyT >& ext_instance ) const;
      
      /**
       * Operator != overload.
       *
       * ext_instance: External instance reference.
       * return true if this instance don't have the same internal
       * update time that the external instance.
       */
      bool operator!=( 
        const PreMultiContainer< PreMultiContainerKeyT >& ext_instance ) const;
      
      /**
       * operator= overload.
       *
       * ext_instance: External instance reference.
       * return The external instance reference.
       */
      const PreMultiContainer< PreMultiContainerKeyT >& operator=( 
        const PreMultiContainer< PreMultiContainerKeyT >& ext_instance );
      
      //Clear all contents.
      void clear();
                         
      /**
       * Store a object copy.
       *
       * obj_key: Object key.
       * obj_reference: Object instance.
       */                         
      template< typename ObjectT >
      void store( const PreMultiContainerKeyT& obj_key,
                         const ObjectT& obj_reference );
      
      /**
       * Retrive a copy of a stored object.
       *
       * obj_key: Object key.
       * obj_reference: Object instance.
       * return true if OK, false if the parameter was no found or error.
       */      
      template< typename ObjectT >
      bool retrive( const PreMultiContainerKeyT& obj_key,
        ObjectT& obj_reference ) const;
        
      /**
       * Retrive copies of all stored objects of a defined type.
       *
       * objs_vector: A vector with copies of all found objects.
       * note: An empty vector will be returned if no object was found.
       */      
      template< typename ObjectT >
      void multiRetrive( std::vector< std::pair< PreMultiContainerKeyT, 
        ObjectT > >& objs_vector ) const;        
     
      /** 
       * Checks if a object is stored.
       * 
       * obj_key: Object key.
       * note: The object type must be specified.
       * return true if OK, false if the object is not stored.
       */
      template< typename ObjectT >
      bool isStored( const PreMultiContainerKeyT& obj_key ) const;
      
      /** 
       * Remove a stored object.
       * 
       * obj_key: Object key.
       */
      void remove( const PreMultiContainerKeyT& obj_key );      
      
    protected :

      //Internal container type definition. 
      typedef typename std::map< PreMultiContainerKeyT,
        MCNodeInterface< PreMultiContainerKeyT >* > IntContainerT;
      
      //The nodes container instance. 
      IntContainerT container_instance_;    
      
      //The last update time.
      time_t last_up_time_;

      /**
       * Updates the internal last update time.
       *
       * note: Needed by comparison between two multi containers.
       */
      void update_time();            

};

template< typename PreMultiContainerKeyT >
PreMultiContainer< PreMultiContainerKeyT >::PreMultiContainer()
{
  last_up_time_ = 0;
}

template< typename PreMultiContainerKeyT >
PreMultiContainer< PreMultiContainerKeyT >::PreMultiContainer( 
  const PreMultiContainer< PreMultiContainerKeyT >& external )
{
  last_up_time_ = 0;
  
  operator=( external );
}

template< typename PreMultiContainerKeyT >
PreMultiContainer< PreMultiContainerKeyT >::~PreMultiContainer()
{
  clear();
}

template< typename PreMultiContainerKeyT >
void PreMultiContainer< PreMultiContainerKeyT >::clear()
{
  
  typename IntContainerT::iterator it = container_instance_.begin();
  typename IntContainerT::iterator it_end = container_instance_.end();
  
  while( it != it_end ) {
    delete (it->second);
    
    ++it;
  }
  
  container_instance_.clear();
  
}

template< typename PreMultiContainerKeyT >
bool PreMultiContainer< PreMultiContainerKeyT >::operator==( 
  const PreMultiContainer< PreMultiContainerKeyT >& ext_instance ) const
{
  if( last_up_time_ == ext_instance.last_up_time_ ) {
    return true;
  } else {
    return false;
  }
}

template< typename PreMultiContainerKeyT >
bool PreMultiContainer< PreMultiContainerKeyT >::operator!=( 
  const PreMultiContainer< PreMultiContainerKeyT >& ext_instance ) 
  const
{
  if( last_up_time_ == ext_instance.last_up_time_ ) {
    return false;
  } else {
    return true;
  }
}

template< typename PreMultiContainerKeyT >
const PreMultiContainer< PreMultiContainerKeyT >& 
PreMultiContainer< PreMultiContainerKeyT >::operator=( 
  const PreMultiContainer< PreMultiContainerKeyT >& ext_instance )
{
  if( ( &ext_instance ) != this ) {
    
    //Clearing the current objects 
    
    typename IntContainerT::iterator my_container_it = 
      container_instance_.begin();
    typename IntContainerT::iterator my_container_it_end = 
      container_instance_.end();
    
    while( my_container_it != my_container_it_end ) {
      delete (my_container_it->second);
      
      ++my_container_it;
    }    
    
    container_instance_.clear();
    
    //Cloning external objects 
    
    typename IntContainerT::const_iterator container_it = 
      ext_instance.container_instance_.begin();
    typename IntContainerT::const_iterator container_it_end = 
      ext_instance.container_instance_.end();
          
    while( container_it != container_it_end ) {
      container_instance_[ container_it->first ] = 
        container_it->second->clone();
          
      ++container_it;
    }
  
    last_up_time_ = ext_instance.last_up_time_;
  
     
  }

  return *this;
}

template< typename PreMultiContainerKeyT >
void PreMultiContainer< PreMultiContainerKeyT >::update_time()
{
  last_up_time_ = time( 0 );
}

template< typename PreMultiContainerKeyT >
template< typename ObjectT >
void PreMultiContainer< PreMultiContainerKeyT >::store( 
  const PreMultiContainerKeyT& obj_key, const ObjectT& obj_reference )
{

  //Creating a new node 
  
  ObjectT* newobjptr = new ObjectT;
  ( *newobjptr ) = obj_reference;
  
  MCNode< ObjectT, PreMultiContainerKeyT >* newnodeptr =
    new MCNode< ObjectT, PreMultiContainerKeyT >;  
  newnodeptr->setObjPtr( newobjptr );
  
  typename IntContainerT::iterator container_it = 
    container_instance_.find( obj_key );
    
  //If a old node with the same key exists, it will be deleted 
    
  if( container_it == container_instance_.end() ) {
    container_instance_[ obj_key ] = newnodeptr;
  } else {
    delete (container_it->second);
    
    container_it->second = newnodeptr;
  }
      
  update_time();
  
}

template< typename PreMultiContainerKeyT >
template< typename ObjectT >
bool PreMultiContainer< PreMultiContainerKeyT >::retrive(
  const PreMultiContainerKeyT& obj_key, ObjectT& obj_reference ) const
{
  
  typename IntContainerT::const_iterator container_it = 
    container_instance_.find( obj_key );
    
  if( container_it == container_instance_.end() ) {
    
    return false;
  } else {
    if( typeid( ObjectT ).name() == 
      container_it->second->getObjTypeInfo() ) {
      
      obj_reference = 
        ( *( ( (MCNode< ObjectT, PreMultiContainerKeyT >* )
        container_it->second )->getObjPtr() ) );
    
      return true;
    } else {
   
      return false;
    }
  }
}

template< typename PreMultiContainerKeyT >
template< typename ObjectT >
void PreMultiContainer< PreMultiContainerKeyT >::multiRetrive(
  std::vector< std::pair< PreMultiContainerKeyT, 
  ObjectT > >& objs_vector ) const
{
  objs_vector.clear();
  
  typename IntContainerT::const_iterator container_it = 
    container_instance_.begin();
  typename IntContainerT::const_iterator container_it_end = 
    container_instance_.end();
  
  std::pair< PreMultiContainerKeyT, ObjectT > temp_pair;
  
  while( container_it != container_it_end ) {
    if( typeid( ObjectT ).name() == 
      container_it->second->getObjTypeInfo() ) {  
      
      temp_pair.first = container_it->first;
      temp_pair.second = 
        ( *( ( ( MCNode< ObjectT, PreMultiContainerKeyT >* )
        container_it->second )->getObjPtr() ) );
    
      objs_vector.push_back( temp_pair );      
    }  
  
    ++container_it;
  }
  
}

template< typename PreMultiContainerKeyT >
template< typename ObjectT >
bool PreMultiContainer< PreMultiContainerKeyT >::isStored( 
  const PreMultiContainerKeyT& obj_key ) const
{
 
  typename IntContainerT::const_iterator container_it = 
    container_instance_.find( obj_key );
    
  if( container_it == container_instance_.end() ) {
    
    return false;
  } else {
    if( typeid( ObjectT ).name() == 
      container_it->second->getObjTypeInfo() ) {
    
      return true;
    } else {
    
      return false;
    }
  }
}


template< typename PreMultiContainerKeyT >
void PreMultiContainer< PreMultiContainerKeyT >::remove( 
  const PreMultiContainerKeyT& obj_key )
{
  
  typename IntContainerT::iterator container_it = 
    container_instance_.find( obj_key );
    
  //If a old node with the same key exists, it will be deleted 
    
  if( container_it != container_instance_.end() ) {
    delete (container_it->second);
    
    container_instance_.erase( container_it );
  }
      
  update_time();
  
}

#endif

