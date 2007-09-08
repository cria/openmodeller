/**
 * Test class for MapFormat
 * 
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-07-30
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

/** \ingroup test
* \brief Test for MapFormat Class
*/

#ifndef TEST_MAPFORMAT_HH
#define TEST_MAPFORMAT_HH

#include "cxxtest/TestSuite.h"
#include <openmodeller/MapFormat.hh>
#include <openmodeller/Exceptions.hh>


class test_MapFormat: public CxxTest :: TestSuite 
{
  public:
    void setUp (){
      a = new std::string;
      b = new double;
      c = new int;
      A = new MapFormat();
      B = new MapFormat(0.01,0.02,0.03,0.04,0.05,0.06,0.07,"name");
    }

    void tearDown (){
      delete a;
      delete b;
      delete c;
      delete A;
      delete B;
    }

/**
 *Test for MapFormat Default Constructor.
 */

    void testDefaultConstructor (){
      std::cout << std::endl;
      std::cout << "Testing MapFormat() ..." << std::endl;
      *c=A->getFormat();
      TS_ASSERT(*c==MapFormat::ByteHFA);
      *a="Cell width not set";
      try{
        A->getXCel();
        TS_FAIL("No Exception Thrown");
      }
      catch(InvalidParameterException& e){
        TS_ASSERT(*a==e.what());
      }
      catch(...){
      TS_FAIL("Incorrect Exception Thrown");
      }
      *a="Cell height not set";
      try{
      A->getYCel();
      TS_FAIL("No Exception Thrown");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(*a==e.what());
      }
      catch(...){
      TS_FAIL("Incorrect Exception Thrown");
      }
      *a="XMin not set";
      try{
      A->getXMin();
      TS_FAIL("No Exception Thrown");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(*a==e.what());
      }
      catch(...){
      TS_FAIL("Incorrect Exception Thrown");
      }
      *a="Ymin not set";
      try{
      A->getYMin();
      TS_FAIL("No Exception Thrown");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(*a==e.what());
      }
      catch(...){
      TS_FAIL("Incorrect Exception Thrown");
      }
      *a="Xmax not set";
      try{
      A->getXMax();
      TS_FAIL("No Exception Thrown");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(*a==e.what());
      }
      catch(...){
      TS_FAIL("Incorrect Exception Thrown");
      }
      *a="Ymax not set";
      try{
      A->getYMax();
      TS_FAIL("No Exception Thrown");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(*a==e.what());
      }
      catch(...){
      TS_FAIL("Incorrect Exception Thrown");
      }
      *a="NoDataValue not set";
      try{
      A->getNoDataValue();
      TS_FAIL("No Exception Thrown");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(*a==e.what());
      }
      catch(...){
      TS_FAIL("Incorrect Exception Thrown");
      }
      *a="Projection not set";
      try{
      A->getProjection();
      TS_FAIL("No Exception Thrown");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(*a==e.what());
      }
      catch(...){
      TS_FAIL("Incorrect Exception Thrown");
      }
    }

/**
 *Test for MapFormat Constructor.
 */

    void testConstructor (){
      std::cout << std::endl;
      std::cout << "Testing MapFormat::MapFormat( Coord xcel, Coord ycel, Coord xmin, Coord ymin, Coord xmax, Coord ymax, Scalar noval, char const * proj ) ..." << std::endl;
      *c=B->getFormat();
      TS_ASSERT(*c==MapFormat::ByteHFA);
      *b=0.01;
      try{
      B->getXCel();
      TS_ASSERT(*b==B->getXCel());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("Cell width not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
      *b=0.02;
      try{
      B->getYCel();
      TS_ASSERT(*b==B->getYCel());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("Cell height not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
      *b=0.03;
      try{
      B->getXMin();
      TS_ASSERT(*b==B->getXMin());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("XMin not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
      *b=0.04;
      try{
      B->getYMin();
      TS_ASSERT(*b==B->getYMin());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("YMin not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
      *b=0.05;
      try{
      B->getXMax();
      TS_ASSERT(*b==B->getXMax());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("XMax not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
      *b=0.06;
      try{
      B->getYMax();
      TS_ASSERT(*b==B->getYMax());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("YMax not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
      *b=0.07;
      try{
      B->getNoDataValue();
      TS_ASSERT(*b==B->getNoDataValue());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("NoDataValue not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
      *a="name";
      try{
      B->getProjection();
      TS_ASSERT(*a==B->getProjection());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("Projection not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

//Need to test MapFormat(char const *filenameWithFormat) (after testing RasterFactory)

/**
 *Test for setFormat(int f).
 */

    void testSetFormat (){
      std::cout << std::endl;
      std::cout << "Testing setFormat(int f) ..." << std::endl;
      A->setFormat(0);
      TS_ASSERT(A->getFormat()==0);
      A->setFormat(MapFormat::ByteHFA);
      TS_ASSERT(A->getFormat()==MapFormat::ByteHFA);
      A->setFormat(MapFormat::ByteHFA+1);
      TS_ASSERT(A->getFormat()==MapFormat::FloatingTiff);
      A->setFormat(-1);
      TS_ASSERT(A->getFormat()==MapFormat::FloatingTiff);
    }

/**
 *Test for setXCel(Coord v).
 */

    void testSetXCel (){
      std::cout << std::endl;
      std::cout << "Testing setXCel(Coord v) ..." << std::endl;
      A->setXCel(0.01);
      *b=0.01;
      try{
      A->getXCel();
      TS_ASSERT(*b==A->getXCel());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("Cell width not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for setYCel(Coord v).
 */

    void testSetYCel (){
      std::cout << std::endl;
      std::cout << "Testing setYCel(Coord v) ..." << std::endl;
      A->setYCel(0.02);
      *b=0.02;
      try{
      A->getYCel();
      TS_ASSERT(*b==A->getYCel());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("Cell height not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for setXMin(Coord v).
 */

    void testSetXMin (){
      std::cout << std::endl;
      std::cout << "Testing setXMin(Coord v) ..." << std::endl;
      A->setXMin(0.03);
      *b=0.03;
      try{
      A->getXMin();
      TS_ASSERT(*b==A->getXMin());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("XMin not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for setYMin(Coord v).
 */

    void testSetYMin (){
      std::cout << std::endl;
      std::cout << "Testing setYMin(Coord v) ..." << std::endl;
      A->setYMin(0.04);
      *b=0.04;
      try{
      A->getYMin();
      TS_ASSERT(*b==A->getYMin());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("YMin not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for setXMax(Coord v).
 */

    void testSetXMax (){
      std::cout << std::endl;
      std::cout << "Testing setXMax(Coord v) ..." << std::endl;
      A->setXMax(0.05);
      *b=0.05;
      try{
      A->getXMax();
      TS_ASSERT(*b==A->getXMax());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("XMax not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for setYMax(Coord v)
 */

    void testSetYMax (){
      std::cout << std::endl;
      std::cout << "Testing setYMax(Coord v) ..." << std::endl;
      A->setYMax(0.06);
      *b=0.06;
      try{
      A->getYMax();
      TS_ASSERT(*b==A->getYMax());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("YMax not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for setNoDataValue(Scalar v).
 */

      void testSetNoDataValue (){
      std::cout << std::endl;
      std::cout << "Testing setNoDataValue(Scalar v) ..." << std::endl;
      A->setNoDataValue(0.07);
      *b=0.07;
      try{
      A->getNoDataValue();
      TS_ASSERT(*b==A->getNoDataValue());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("NoDataValue not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for setProjection(const string& v).
 */

    void testSetProjection (){
      std::cout << std::endl;
      std::cout << "Testing setProjection(const string& v) ..." << std::endl;
      A->setProjection("name");
      *a="name";
      try{
      A->getProjection();
      TS_ASSERT(*a==A->getProjection());
      }
      catch(InvalidParameterException& e){
      TS_FAIL("Projection not set");
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for unsetFormat().
 */

    void testUnSetFormat (){
      std::cout << std::endl;
      std::cout << "Testing unsetFormat() ..." << std::endl;
      A->unsetFormat();
      TS_ASSERT(A->getFormat()==MapFormat::FloatingTiff);
      }

/**
 *Test for unsetXCel().
 */

    void testUnSetXCel (){
      std::cout << std::endl;
      std::cout << "Testing unsetXCel() ..." << std::endl;
      A->unsetXCel();
      try{
      A->getXCel();
      TS_FAIL("Cell width set");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(true);
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for unsetYCel().
 */

    void testUnSetYCel (){
      std::cout << std::endl;
      std::cout << "Testing unsetYCel() ..." << std::endl;
      A->unsetYCel();
      try{
      A->getYCel();
      TS_FAIL("Cell height set");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(true);
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
      }

/**
 *Test for unsetXMin().
 */

    void testUnSetXMin (){
      std::cout << std::endl;
      std::cout << "Testing unsetXMin() ..." << std::endl;
      A->unsetXMin();
      try{
      A->getXMin();
      TS_FAIL("XMin set");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(true);
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for unsetYMin().
 */

    void testUnSetYMin (){
      std::cout << std::endl;
      std::cout << "Testing unsetYMin() ..." << std::endl;
      A->unsetYMin();
      try{
      A->getYMin();
      TS_FAIL("YMin set");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(true);
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for unsetXMax().
 */

    void testUnSetXMax (){
      std::cout << std::endl;
      std::cout << "Testing unsetXMax() ..." << std::endl;
      A->unsetXMax();
      try{
      A->getXMax();
      TS_FAIL("XMax set");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(true);
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for unsetYMax().
 */

    void testUnSetYMax (){
      std::cout << std::endl;
      std::cout << "Testing unsetYMax() ..." << std::endl;
      A->unsetYMax();
      try{
      A->getYMax();
      TS_FAIL("YMax set");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(true);
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for unsetNoDataValue().
 */

    void testUnSetNoDataValue (){
      std::cout << std::endl;
      std::cout << "Testing unsetNoDataValue() ..." << std::endl;
      A->unsetNoDataValue();
      try{
      A->getNoDataValue();
      TS_FAIL("NoDataValue set");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(true);
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for unsetProjection().
 */

    void testUnSetProjection (){
      std::cout << std::endl;
      std::cout << "Testing unsetProjection() ..." << std::endl;
      A->unsetProjection();
      try{
      A->getProjection();
      TS_FAIL("Projection set");
      }
      catch(InvalidParameterException& e){
      TS_ASSERT(true);
      }
      catch(...){
      TS_FAIL("Invalid Exception Thrown");
      }
    }

/**
 *Test for getWidth().
 */

    void testGetWidth (){
      std::cout << std::endl;
      std::cout << "Testing getWidth() ..." << std::endl;
      *c = static_cast<int>((B->getXMax()-B->getXMin())/B->getXCel()+0.5);
      TS_ASSERT(*c==B->getWidth());
    }

/**
 *Test for getHeight().
 */

    void testGetHeight (){
      std::cout << std::endl;
      std::cout << "Testing getHeight() ..." << std::endl;
      *c = static_cast<int>((B->getYMax()-B->getYMin())/B->getYCel()+0.5);
      TS_ASSERT(*c==B->getHeight());
    }

  private:
    std::string *a;
    double *b;
    int *c;
    MapFormat *A;
    MapFormat *B;
};

#endif

