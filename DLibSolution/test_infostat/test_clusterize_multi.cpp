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
#include "testsourceprovider.h"
/////////////////////////////
using namespace info;
//////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(ClusterizeMultiTestSuite, TestIndivSourceFixture<>)
;
BOOST_AUTO_TEST_CASE(testClusterizeMunti) {
	using ClusterizeKMeansType = ClusterizeKMeans<>;
	SourceType *pMan = this->source();
	BOOST_CHECK(pMan != nullptr);
	size_t nbClusters = 6;
	size_t nbIters = 20;
	//
	ClusterizeKMeansType oMan;
	size_t nx = oMan.compute_random(nbClusters, pMan, nbIters);
	BOOST_CHECK(nx > 0);
	std::stringstream os;
	os << oMan;
	std::string ss = os.str();
	BOOST_TEST_MESSAGE("Clusters count: " << oMan.clusters_count() << "/" << nbClusters);
	BOOST_TEST_MESSAGE("CLUSTERIZE MULTI\n" << ss);
} //testClusterizeMean
BOOST_AUTO_TEST_SUITE_END();





