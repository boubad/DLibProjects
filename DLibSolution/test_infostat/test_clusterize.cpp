#include "stdafx.h"
/*
 * test_clusterize.cpp
 *
 *  Created on: 20 mai 2016
 *      Author: boubad
 */
#include <boost/test/unit_test.hpp>
/////////////////////////////
#include <clusterize.h>
#include "storeindivprovider_fixture.h"
#include "infotestdata.h"
//////////////////////////////////
#include <boost/foreach.hpp>
/////////////////////////////
using namespace info;
//////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(ClusterizeTestSuite, StoreIndivSorceFixture)
;

BOOST_AUTO_TEST_CASE(testClusterizeRandom) {
	using ClusterizeKMeansType = ClusterizeKMeans<>;
	IndivSourceType *pMan = get_source();
	BOOST_CHECK(pMan != nullptr);
	size_t nbClusters = 5;
	//LinkMode mode = LinkMode::linkMean;
	size_t nbIters = 20;
	//
	ClusterizeKMeansType oMan;
	size_t nx = oMan.compute_random(nbClusters,pMan,nbIters);
	BOOST_CHECK(nx > 0);
	//
	double fIntra = 0, fInter = 0, ff = 0;
	bool bRet = oMan.get_criterias(fIntra, fInter, ff);
	BOOST_CHECK(bRet);
	std::stringstream os;
	os << oMan;
	std::string ss = os.str();
	BOOST_TEST_MESSAGE("CLUSTERIZE KMEANS RANDOM\n" << ss);
	//
	const ClusterizeKMeansType::clusters_vector &oVec = oMan.clusters();
	std::for_each(oVec.begin(), oVec.end(),
			[&](const ClusterizeKMeansType::IndivClusterType &c) {
				std::string s;
				test_write_map(c.center(),s);
				BOOST_TEST_MESSAGE(s);
			});
} //testClusterizeMean
BOOST_AUTO_TEST_SUITE_END();





