#include "stdafx.h"
/*
* test_treeitem.cpp
*
*  Created on: 20 mai 2016
*      Author: boubad
*/
#include <boost/test/unit_test.hpp>
/////////////////////////////
#include <treecollection.h>
#include <clusterize.h>
#include "storeindivprovider_fixture.h"
#include "infotestdata.h"
//////////////////////////////////
#include <boost/foreach.hpp>
/////////////////////////////
using namespace info;
//////////////////////////////////
BOOST_FIXTURE_TEST_SUITE(TreeClusterizeTestSuite, StoreIndivSorceFixture<>)
;
BOOST_AUTO_TEST_CASE(testTreeItemMean) {
	using ClusterizeKMeansType = ClusterizeKMeans<>;
	using IndivsTreeType = IndivsTree<>;
	IndivSourceType *pMan = get_source();
	BOOST_CHECK(pMan != nullptr);
	size_t nbClusters = 6;
	LinkMode mode = LinkMode::linkMean;
	//
	IndivsTreeType oTree;
	oTree.link_mode(mode);
	oTree.process(pMan, nbClusters);
	IndivsTreeType::datamaps_vector oCenters;
	oTree.get_centers(oCenters);
	size_t nbIters = 20;
	//
	ClusterizeKMeansType oMan;
	size_t nx = oMan.compute(oCenters, pMan, nbIters);
	BOOST_CHECK(nx > 0);
	std::stringstream os;
	os << oMan;
	std::string ss = os.str();
	BOOST_TEST_MESSAGE("Clusters count: " << oMan.clusters_count() << "/" << nbClusters);
	BOOST_TEST_MESSAGE("CLUSTERIZE KMEANS HIERAR\n" << ss);
} //testTreeItemMean

BOOST_AUTO_TEST_SUITE_END();

