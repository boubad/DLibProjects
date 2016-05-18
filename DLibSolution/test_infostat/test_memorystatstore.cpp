#include "stdafx.h"
#include <boost/test/unit_test.hpp>
/////////////////////////////
#include <memorystatstore.h>
/////////////////////////////
#include "infotestdata.h"
//////////////////////////////////
#include <boost/foreach.hpp>
/////////////////////////////
using namespace info;
using StoreType = MemoryStatStore<>;
//////////////////////////////////
class MemoryStoreFixture  {
protected:
	size_t	 m_nbcols;
	size_t	 m_nbrows;
	StoreType m_store;
public:
	MemoryStoreFixture() :m_nbcols(0), m_nbrows(0) {
	}
	virtual ~MemoryStoreFixture() {}
};// class  DatabaseFixture
  ////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(MemoryStoreTestSuite, MemoryStoreFixture);
BOOST_AUTO_TEST_CASE(testDatasets) {
	
} // testDatasets
BOOST_AUTO_TEST_CASE(testVariables) {
	
} // testVariables
BOOST_AUTO_TEST_CASE(testIndivs) {
	
} // testIndivs
BOOST_AUTO_TEST_CASE(testValues) {
	
} // testValues
BOOST_AUTO_TEST_SUITE_END();
