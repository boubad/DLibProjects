// Copyright (C) 2008  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.

#include <dlib/test/tester.h>
#include "infotestdata.h"
#include <statdbmanager.h>
// This is called an unnamed-namespace and it has the effect of making everything 
// inside this file "private" so that everything you declare will have static linkage.  
// Thus we won't have any multiply defined symbol errors coming out of the linker when 
// we try to compile the test suite.
namespace
{
	using namespace test;
	using namespace info;
	// Declare the logger we will use in this test.  The name of the logger 
	// should start with "test."
	dlib::logger dlog("test.statdbmanager");


	class statdbmanager_tester : public tester
	{
		/*!
		WHAT THIS OBJECT REPRESENTS
		This object represents a unit test.  When it is constructed
		it adds itself into the testing framework.
		!*/
	public:
		statdbmanager_tester() :
			tester(
				"statdbmanager",       // the command line argument name for this test
				"Run StatDBManager tests.", // the command line argument description
				0                     // the number of command line arguments for this test
			)
		{}
		void fill_random_data(StatDBManager *pMan, const std::string &setName, 
			size_t nTotalInds = 1024,
			size_t nTotalVars = 20) {
			DLIB_TEST(pMan != nullptr);
			DLIB_TEST(pMan->is_valid());
			DLIB_TEST(!setName.empty());
			DLIB_TEST(nTotalInds > 0);
			DLIB_TEST(nTotalVars > 0);
			//
			std::string name(setName);
			std::string desc(setName);
			std::string status("OK");
			//
			DBStatDataset oSet(setName);
			bool bRet = pMan->find_dataset(oSet);
			if (!bRet) {
				bRet = pMan->insert_dataset(oSet);
				DLIB_TEST_MSG(bRet, "Insert database failed");
				bRet = pMan->find_dataset(oSet);
				DLIB_TEST_MSG(bRet, "Database not found.");
			}
			DLIB_TEST_MSG(oSet.id() != 0,"Database id must not be zero");
			//
			std::vector<DBStatIndiv> oInds;
			for (size_t i = 0; i < nTotalInds; ++i) {
				print_spinner();
				std::stringstream os;
				os << "ind" << (i + 1);
				std::string sSigle = os.str();
				DBStatIndiv oInd(oSet);
				oInd.set_sigle(sSigle);
				oInd.set_name(sSigle);
				oInd.set_desc(sSigle);
				oInd.set_status(status);
				if (!pMan->find_indiv(oInd)) {
					oInds.push_back(oInd);
				}
			}// i
			if (!oInds.empty()) {
				print_spinner();
				bRet = pMan->maintains_indivs(oInds);
				DLIB_TEST_MSG(bRet, "Insert Indivs update failed");
			}
			//
			std::string vartype("float");
			std::string genre("initial");
			std::vector<DBStatVariable> oVars;
			for (size_t i = 0; i < nTotalVars; ++i) {
				print_spinner();
				std::stringstream os;
				os << "var" << (i + 1);
				std::string sSigle = os.str();
				DBStatVariable oVar(oSet);
				oVar.set_sigle(sSigle);
				oVar.set_name(sSigle);
				oVar.set_vartype(vartype);
				oVar.set_genre(genre);
				oVar.set_desc(sSigle);
				oVar.set_status(status);
				if (!pMan->find_variable(oVar)) {
					oVars.push_back(oVar);
				}
			}// i
			if (!oVars.empty()) {
				print_spinner();
				bRet = pMan->maintains_variables(oVars);
				DLIB_TEST_MSG(bRet, "Insert Variables update failed");
			}
			//
			oVars.clear();
			oInds.clear();
			bRet = pMan->get_dataset_indivs(oSet, oInds);
			bRet = bRet && pMan->get_dataset_variables(oSet, oVars);
			DLIB_TEST(bRet);
			size_t nCols = oVars.size();
			size_t nRows = oInds.size();
			DLIB_TEST(nRows > 0);
			DLIB_TEST(nCols > 0);
			//
			std::vector<DBStatValue> oVals;
			for (size_t ivar = 0; ivar < nCols; ++ivar) {
				print_spinner();
				const DBStatVariable &oVar = oVars[ivar];
				for (size_t irow = 0; irow < nRows; ++irow) {
					const DBStatIndiv &oInd = oInds[irow];
					DBStatValue val(oVar, oInd);
					if (!pMan->find_value(val)) {
						std::stringstream os;
						os << (::rand() % 100);
						std::string s = os.str();
						DbValue vv(s);
						val.set_value(vv);
						oVals.push_back(val);
					}
				}// irow
			}// ivar
			if (!oVals.empty()) {
				print_spinner();
				bRet = pMan->maintains_values(oVals);
				DLIB_TEST_MSG(bRet, "Insert Values update failed");
			}
		}// fill_tandom_data
		void perform_test()
		{
			// This message gets logged to the file debug.txt if the user has enabled logging by
			// supplying the -d option on the command line (and they haven't set the logging level
			// to something higher than LINFO).
			dlog << dlib::LINFO << "ENTERING StatDBManager tests...";
			//
			std::string dbName;
			InfoTestData::get_database_filename(dbName);
			DLIB_TEST_MSG(!dbName.empty(), "Database filename cannot be empty");
			StatDBManager oMan(dbName);
			DLIB_TEST_MSG(oMan.is_valid(), "StatDataManager is not valid");
			//
			size_t nRows = 0, nCols = 0;
			std::string setName, type, genre, status;
			std::vector<std::string> rowNames, colNames;
			std::vector<int> gdata;
			InfoTestData::get_default_type_genre_status(type, genre, status);
			//
			InfoTestData::get_mortal_data(setName, nRows, nCols, gdata, rowNames, colNames);
			DLIB_TEST(nRows > 2);
			DLIB_TEST(nCols > 2);
			DLIB_TEST(gdata.size() >= (size_t)(nCols * nRows));
			DLIB_TEST(rowNames.size() >= nRows);
			DLIB_TEST(colNames.size() >= nCols);
			DLIB_TEST(!type.empty());
			DLIB_TEST(!genre.empty());
			DLIB_TEST(!status.empty());
			bool bRet = oMan.import_dataset(setName, nRows, nCols, gdata, rowNames, colNames, type, genre, status);
			DLIB_TEST_MSG(bRet, "mortal data import failed!");
			//
			nRows = 0;
			nCols = 0;
			setName.clear();
			gdata.clear();
			rowNames.clear();
			colNames.clear();
			InfoTestData::get_conso_data(setName, nRows, nCols, gdata, rowNames, colNames);
			DLIB_TEST(nRows > 2);
			DLIB_TEST(nCols > 2);
			DLIB_TEST(gdata.size() >= (size_t)(nCols * nRows));
			DLIB_TEST(rowNames.size() >= nRows);
			DLIB_TEST(colNames.size() >= nCols);
			DLIB_TEST(!type.empty());
			DLIB_TEST(!genre.empty());
			DLIB_TEST(!status.empty());
			bRet = oMan.import_dataset(setName, nRows, nCols, gdata, rowNames, colNames, type, genre, status);
			DLIB_TEST_MSG(bRet, "conso data import failed!");
			//
			nRows = 0;
			nCols = 0;
			InfoTestData::get_test_indiv_data(setName, nRows, nCols);
			this->fill_random_data(&oMan, setName, nRows, nCols);
			//
			// If your test takes a long time to run you can also call print_spinner() 
			// periodically.  This will cause a spinning / character to display on the
			// console to indicate to the user that your test is still running (rather
			// than hung) 
			print_spinner();
		}
	};

	// Create an instance of this object.  Doing this causes this test
	// to be automatically inserted into the testing framework whenever this cpp file
	// is linked into the project.  Note that since we are inside an unnamed-namespace 
	// we won't get any linker errors about the symbol a being defined multiple times. 
	statdbmanager_tester a;

}




