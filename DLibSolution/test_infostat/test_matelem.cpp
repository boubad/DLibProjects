#include "stdafx.h"
/*
* test_matelem.cpp
*
*  Created on: 23 mai 2016
*      Author: boubad
*/
#include <boost/test/unit_test.hpp>
/////////////////////////////
#include <matelem.h>
#include "storeindivprovider_fixture.h"
#include "infotestdata.h"
//////////////////////////////////
using namespace info;
//////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(MatElemTestSuite, StoreIndivSorceFixture<>)
;
BOOST_AUTO_TEST_CASE(testMatItem) {
	using IndexType = unsigned long;
	using MatElemType = MatElem<IndexType, unsigned long, std::string>;
	IndivSourceType *pMan = get_source();
	BOOST_CHECK(pMan != nullptr);
	MatElemType::DistanceMapType oDist(pMan);
	MatElemType::ints_vector oIds;
	oDist.indexes(oIds);
	//BOOST_TEST_MESSAGE("\nTEST MATELEM");
	MatElemType oMat(&oDist, &oIds);
	//BOOST_TEST_MESSAGE("Start criteria: " << oMat.criteria());
	oMat.process();
	//BOOST_TEST_MESSAGE("End criteria: " << oMat.criteria());
	MatElemType::ints_vector  ids;
	oMat.ids(ids);
	for (auto aIndex : ids) {
		MatElemType::IndivTypePtr oInd = pMan->find(aIndex);
		MatElemType::IndivType *pInd = oInd.get();
		BOOST_CHECK(pInd != nullptr);
		std::string s = pInd->sigle();
		//BOOST_TEST_MESSAGE(s);
	}
	//BOOST_TEST_MESSAGE("");
} //testTreeItemMean
BOOST_AUTO_TEST_SUITE_END();





