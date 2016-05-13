#include "stdafx.h"
#include <boost/test/unit_test.hpp>
/////////////////////////////
#include "databasefixture.h"
//////////////////////////////////
#include <storeindivprovider.h>
#include <numericindivprovider.h>
////////////////////////////////
#include <boost/foreach.hpp>
/////////////////////////////
using namespace info;
using namespace std;
////////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(IndivProviderTestSuite, DatabaseFixture);
BOOST_AUTO_TEST_CASE(StoreIndivProviderTest)
{
	IStoreHelper *pMan = m_man.get();
	DBStatDataset &oSet = this->m_oset;
	//
	StoreIndivProvider oProvider(pMan, oSet);
	BOOST_CHECK(oProvider.is_valid());
	//
	size_t nc = 0;
	bool bRet = oProvider.indivs_count(nc);
	BOOST_CHECK(bRet);
	BOOST_CHECK(m_nbrows == nc);
	//
	variables_map vars;
	bRet = oProvider.get_variables_map(vars);
	BOOST_CHECK(bRet);
	BOOST_CHECK(m_nbcols == vars.size());
	//
	for (size_t i = 0; i < nc; ++i) {
		Indiv oInd;
		bRet = oProvider.find_indiv_at(i, oInd);
		BOOST_CHECK(bRet);
		IntType aIndex = oInd.id();
		BOOST_CHECK(aIndex != 0);
	} // i
}//StoreIndivProviderTest
BOOST_AUTO_TEST_SUITE_END();
