#include "stdafx.h"
/*
 * test_matitem.cpp
 *
 *  Created on: 23 mai 2016
 *      Author: boubad
 */
#include <boost/test/unit_test.hpp>
/////////////////////////////
#include <matitem.h>
#include "storeindivprovider_fixture.h"
#include "infotestdata.h"
//////////////////////////////////
using namespace info;
//////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(MatItemTestSuite, StoreIndivSorceFixture<>)
;
BOOST_AUTO_TEST_CASE(testMatItem) {
	using IndexType = unsigned long;
	using MatItemType = MatItem<IndexType, long, std::string>;
	IndivSourceType *pMan = get_source();
	BOOST_CHECK(pMan != nullptr);
	MatItemType oMat(pMan);
	oMat.permute();
	MatItemType::ints_vector  ids;
	oMat.ids(ids);
	//BOOST_TEST_MESSAGE("");
	for (auto aIndex : ids) {
		MatItemType::IndivTypePtr oInd = pMan->find(aIndex);
		MatItemType::IndivType *pInd = oInd.get();
		BOOST_CHECK(pInd != nullptr);
		std::string s = pInd->sigle();
		BOOST_TEST_MESSAGE(s);
	}
	//BOOST_TEST_MESSAGE("");
} //testTreeItemMean
BOOST_AUTO_TEST_SUITE_END();





