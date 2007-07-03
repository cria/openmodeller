#ifndef TEST_SAMPLE_HH
#define TEST_SAMPLE_HH

#include "cxxtest/TestSuite.h"
#include "Sample.hh"
#include <algorithm>
#include <istream>
#include <sstream>
#include <iostream>

class FixtureSuite : public CxxTest :: TestSuite 
{

	public:
		void setUp (){
				a = new Sample;
				b = new Sample;
				c = new Sample;
				d = new Sample;
				}

		void tearDown (){
				delete a;
				delete b;
				delete c;
				delete d;
				}

		void test1 (){
				std::cout << std::endl;
				std::cout << "Testing Default Constructor..." << std::endl;

				TS_ASSERT(a->size()==0);
				}

  		void test2 (){
				std::cout << std::endl;
				std::cout << "Testing Resize Function..." << std::endl;
				a->resize(10);				
				TS_ASSERT(a->size()==10);		
				}

		void test3 (){
				std::cout << std::endl;
				std::cout << "Testing Sample Constructor initialized with single Scalar..." << std::endl;
				*b = Sample (10,-0.1);
				for(unsigned int i=0; i<b->size(); ++i){
				TS_ASSERT_DELTA(-0.1,(*b)[i],1e-10);}
				}

		void test4 (){
				std::cout << std::endl;
				std::cout << "Testing Sample assign through operator[]..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0; i<a->size(); ++i){
				(*a)[i]=i;}

				for(unsigned int i=0;i<a->size();++i){
				TS_ASSERT((*a)[i]==i);}
				}

		void test5 (){
				std::cout << std::endl;
				std::cout << "Testing Sample equals( const Sample& ) and operator==()..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;}
				*c = Sample (3);

				//CHECK( ! c.equals(a) );
				TS_ASSERT(!((*c)==(*a)));
				TS_ASSERT((*c)!=(*a));

				*d = Sample(a->size());	
				//CHECK( ! d.equals(a) );
				TS_ASSERT(!((*d)==(*a)));
				TS_ASSERT(((*d)!=(*a)));

				for(unsigned int i=0; i<a->size(); ++i){
				(*d)[i] = (*a)[i];}

				for(unsigned int i=0; i<a->size(); ++i){
				TS_ASSERT((*a)[i]==i);}

				//TS_ASSERT(d.equals(a));
				TS_ASSERT(*d==*a);
				TS_ASSERT(!(*d!=*a));

				}

		void test6 (){
				std::cout << std::endl;
				std::cout << "Testing Sample Copy Constructor..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;
				}

				*c = Sample(*a);
				TS_ASSERT(*a==*c);
				}

		void test7 (){
				std::cout << std::endl;
				std::cout << "Testing Sample operator=( const Sample & )..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;
				}

				*b = *a;
				TS_ASSERT(*b==*a);
				}

		void test8 (){
				std::cout << std::endl;
				std::cout << "Testing Sample operator>> and operator<<..." << std::endl;
				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;
				}
				std::cerr << "A= " << *a << std::endl;
				std::stringstream ss( std::ios::in | std::ios::out );
				ss << *a;
				ss >> *b;
				std::cerr << "B= " << *b << std::endl;
				TS_ASSERT(*a==*b);
				}

		void test9 (){
				std::cout << std::endl;
				std::cout << "Testing Sample operator>> and operator<< precision..." << std::endl;
				std::string testString="1.0 1.01 1.001 1.0001 1.00001 1.000001 1.0000001 1.00000001 1.000000001";
				std::stringstream ss(testString, std::ios::in);

				*b = Sample();
				ss >> *b;
				std::cerr << "B= " << *b << std::endl;

				std::stringstream s2(std::ios::in | std::ios::out);
				s2 << *b;

				*c = Sample();
				s2 >> *c;

				TS_ASSERT(*b==*c);
				}

		void test10 (){
				std::cout << std::endl;
				std::cout << "Testing Sample operator+=(Sample)..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;
				}
				*c = Sample(*a);
				
				*c += *a;

				for(unsigned int i=0;i<c->size();++i){
				TS_ASSERT((*c)[i]==2*(*a)[i]);
				}

				}

		void test11 (){
				std::cout << std::endl;
				std::cout << "Testing Sample operator+=(Scalar)..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;}
				*c = Sample(*a);

				*c += 35;

				for(unsigned int i=0;i<c->size();++i){
				TS_ASSERT((*c)[i]==35+(*a)[i]);}

				}

		void test12 (){
				std::cout << std::endl;
				std::cout << "Testing Sample operator-=(Sample)..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;}
				*c = Sample(*a);

				*c -= *a;

				for(unsigned int i=0;i<c->size();i++){
				TS_ASSERT((*c)[i]==0);}

				}

		void test13 (){
				std::cout << std::endl;
				std::cout << "Testing Sample operator-=(Scalar)..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;}
				*c = Sample(*a);

				*c -= 35;

				for(unsigned int i=0;i<c->size();++i){
				TS_ASSERT((*c)[i]==(*a)[i] - 35);}

				}

		void test14 (){
				std::cout << std::endl;
				std::cout << "Testing Sample operator*=(Sample)..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;}
				*c = Sample(*a);

				*c *= *a;

				for(unsigned int i=0;i<c->size();++i){
				TS_ASSERT((*c)[i]==(*a)[i]*(*a)[i]);}

				}

		void test15 (){
				std::cout << std::endl;
				std::cout << "Testing Sample operator*=(Scalar)..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;}
				*c = Sample(*a);

				*c *= 35;

				for(unsigned int i=0;i<c->size();++i){
				TS_ASSERT((*c)[i] == 35*(*a)[i]);}

				}	

		void test16 (){
				std::cout << std::endl;
				std::cout << "Testing operator/=(Sample)..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;}
				*c = Sample(*a);
				*d = Sample(*a);

				*d += 1;

				*c /= *d;

				for(unsigned int i=0;i<c->size();++i){
				TS_ASSERT_DELTA((*c)[i],(*a)[i]/(*d)[i],1e-10);
				}

				}

		void test17 (){
				std::cout << std::endl;
				std::cout << "Testing Sample operator/=(Scalar)..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;}
				*c = Sample(*a);

				*c /= 35;

				for(unsigned int i=0;i<c->size();++i){
				TS_ASSERT_DELTA((*c)[i],(*a)[i]/35,1e-10);}

				}

		void test18 (){
				//Sample operator&=(Scalar)
				std::cout << std::endl;
				std::cout << "Testing Sample operator&=(Scalar)..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;}
				*b = Sample(*a);

				for(unsigned int i=0;i<b->size();++i){
				(*b)[i] = i-(*b)[i];}

				*c = Sample(*a);

				*c &= *b;

				for(unsigned int i=0;i<c->size();++i){
				TS_ASSERT( (*c)[i] == std::min((*a)[i],(*b)[i]) );}

				}

		void test19 (){
				std::cout << std::endl;
				std::cout << "Testing Sample operator|=(Scalar)..." << std::endl;

				*a = Sample(10);
				for(unsigned int i=0;i<a->size();++i){
				(*a)[i]=i;}
				*b = Sample(*a);

				for(unsigned int i=0;i<b->size();++i){
				(*b)[i] = i-(*b)[i];}

				*c |= *b;

				for(unsigned int i=0;i<c->size();++i){
				TS_ASSERT((*c)[i] == std::max((*a)[i],(*b)[i]));} 

				}

		void test20 (){
				std::cout << std::endl;
				std::cout << "Testing Sample sqr()..." << std::endl;

				*c = Sample(3);
				(*c)[0] = -5;
				(*c)[1] = -0;
				(*c)[2] = -0.5;

				c->sqr();

				TS_ASSERT((*c)[0] == 25.0);
				TS_ASSERT((*c)[1] == 0.0);
				TS_ASSERT((*c)[2] == 0.25);

				}

		void test21 (){
				std::cout << std::endl;
				std::cout << "Testing Sample sqrt()..." << std::endl;

				*c = Sample(3);
				(*c)[0] = 0;
				(*c)[1] = 25;
				(*c)[2] = 625;

				c->sqrt();

				TS_ASSERT((*c)[0] == 0.0);
				TS_ASSERT((*c)[1] == 5.0);
				TS_ASSERT((*c)[2] == 25.0);

				}

		void test22 (){
				std::cout << std::endl;
				std::cout << "Testing Sample norm()..." << std::endl;

				*c = Sample(3);
				(*c)[0] = 0;
				(*c)[1] = 3;
				(*c)[2] = 4;

				TS_ASSERT(c->norm() == 5.0);

				}

		void test23 (){
				std::cout << std::endl;
				std::cout << "Testing Sample doProduct()..." << std::endl;

				*b = Sample(3);
				*c = Sample(3);
				(*b)[0] = 0;
				(*b)[1] = 3;
				(*b)[2] = 2;

				(*c)[0] = 10;
				(*c)[1] = 3;
				(*c)[2] = 8;

				TS_ASSERT(b->dotProduct(*c) == 5.0);

				}

	private:
				Sample *a;
				Sample *b;
				Sample *c;
				Sample *d;
};


#endif
