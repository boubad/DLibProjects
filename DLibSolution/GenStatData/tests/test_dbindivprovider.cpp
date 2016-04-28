// Copyright (C) 2008  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.

#include <dlib/test/tester.h>
////////////////////////////////
#include <dlib/logger.h>
#include "infotestdata.h"
#include <statdbmanager.h>
#include <dbindivprovider.h>
////////////////////////////////////////////
// This is called an unnamed-namespace and it has the effect of making everything 
// inside this file "private" so that everything you declare will have static linkage.  
// Thus we won't have any multiply defined symbol errors coming out of the linker when 
// we try to compile the test suite.
namespace
{
	using namespace test;
	using namespace info;
	using namespace dlib;
	// Declare the logger we will use in this test.  The name of the logger 
	// should start with "test."
	dlib::logger dlog_tprovider("test.dbindivprovider");


	class dbindivprovider_tester : public tester
	{
		/*!
		WHAT THIS OBJECT REPRESENTS
		This object represents a unit test.  When it is constructed
		it adds itself into the testing framework.
		!*/
	public:
		dbindivprovider_tester() :
			tester(
				"dbindivprovider",       // the command line argument name for this test
				"Run DbIndivProvider tests.", // the command line argument description
				0                     // the number of command line arguments for this test
			)
		{
		}
		
		
		void perform_test()
		{
			// This message gets logged to the file debug.txt if the user has enabled logging by
			// supplying the -d option on the command line (and they haven't set the logging level
			// to something higher than LINFO).
			dlog_tprovider << LINFO << "ENTERING DBInndivProvider tests...";
			//
			std::string dbName;
			InfoTestData::get_database_filename(dbName);
			DLIB_TEST_MSG(!dbName.empty(), "Database filename cannot be empty");
			StatDBManager oMan(dbName);
			DLIB_TEST_MSG(oMan.is_valid(), "StatDataManager is not valid");
			//
			std::string setName;
			InfoTestData::get_conso_name(setName);
			DLIB_TEST_MSG(!setName.empty(), "set name cannot be empty");
			DBIndivProvider oProvider(&oMan, setName);
			IIndivProvider *pProvider = &oProvider;
			DLIB_TEST_MSG(pProvider->is_valid(), "Provider is not valid");
			bool bRet = pProvider->reset();
			DLIB_TEST_MSG(bRet, "reset method returns false");
			size_t nCount = 0;
			bRet = pProvider->indivs_count(nCount);
			DLIB_TEST_MSG(bRet, "indivs_count method returs false");
			DLIB_TEST_MSG(nCount > 0, "count must be greater than 0");
			size_t nc = 0;
			do {
				print_spinner();
				Indiv oInd1;
				bRet = pProvider->next(oInd1);
				if (!bRet) {
					DLIB_TEST_MSG(nc == nCount, "Count mismatch");
					break;
				}
				++nc;
				DLIB_TEST_MSG(oInd1.id() != 0, "Indiv is not valid");
				Indiv oInd2;
				bRet = pProvider->next(oInd2);
				if (!bRet) {
					DLIB_TEST_MSG(nc == nCount, "Count mismatch");
					break;
				}
				++nc;
				DLIB_TEST_MSG(oInd2.id() != 0, "Indiv is not valid");
				double dist = oInd1.distance(oInd2);
				DLIB_TEST(dist >= 0);
			} while (true);
		}
	};

	// Create an instance of this object.  Doing this causes this test
	// to be automatically inserted into the testing framework whenever this cpp file
	// is linked into the project.  Note that since we are inside an unnamed-namespace 
	// we won't get any linker errors about the symbol a being defined multiple times. 
	dbindivprovider_tester a;

}




