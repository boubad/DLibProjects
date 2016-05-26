#include "stdafx.h"
#include "CppUnitTest.h"
//////////////////////
#include <memory>
#include <map>
//////////////////////////////
#include <infosqlitestore.h>
//////////////////////////
#include "infotestdata.h"
///////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;
using namespace std;
////////////////////////////////
using STRINGTYPE = std::string;
using DATATYPE = int;
//////////////////////////////////
namespace UnitTestInfoStat
{
	TEST_CLASS(UnitTestSQLiteStore)
	{
		std::unique_ptr<info::SQLiteStatHelper> m_man;
		//
		void import(const STRINGTYPE &name, size_t nRows, size_t nCols,
			const std::vector<DATATYPE> &data,
			const std::vector<STRINGTYPE> &rowNames,
			const std::vector<STRINGTYPE> &colNames) {
			SQLiteStatHelper *p = this->m_man.get();
			Assert::IsTrue(p != nullptr);
			Assert::IsTrue(p->is_valid());
			SQLiteStatHelper::DatasetType oSet(name);
			bool bRet = false;
			if (!p->find_dataset(oSet)) {
				bool bRet = p->maintains_dataset(oSet);
				Assert::IsTrue(bRet);
			}
			Assert::IsTrue(oSet.id() != 0);
			STRINGTYPE type("float");
			STRINGTYPE genre("initial");
			SQLiteStatHelper::variables_vector oVars;
			for (size_t i = 0; i < nCols; ++i) {
				STRINGTYPE sigle = colNames[i];
				SQLiteStatHelper::VariableType v(oSet, sigle);
				if (!p->find_variable(v)) {
					v.vartype(type);
					v.genre(genre);
					oVars.push_back(v);
				}
			} // i
			if (!oVars.empty()) {
				bRet = p->maintains_variables(oVars);
				Assert::IsTrue(bRet);
			}
			SQLiteStatHelper::indivs_vector oInds;
			for (size_t i = 0; i < nRows; ++i) {
				STRINGTYPE sigle = rowNames[i];
				SQLiteStatHelper::IndivType v(oSet, sigle);
				if (!p->find_indiv(v)) {
					oInds.push_back(v);
				}
			} // i
			if (!oInds.empty()) {
				bRet = p->maintains_indivs(oInds);
				Assert::IsTrue(bRet);
			}
			oInds.clear();
			bRet = p->find_dataset_indivs(oSet, oInds, 0, nRows);
			Assert::IsTrue(bRet);
			Assert::IsTrue(nRows == oInds.size());
			oVars.clear();
			bRet = p->find_dataset_variables(oSet, oVars, 0, nCols);
			Assert::IsTrue(bRet);
			Assert::IsTrue(nCols == oVars.size());
			//
			std::map<STRINGTYPE, SQLiteStatHelper::VariableType *> pVars;
			for (auto &s : colNames) {
				STRINGTYPE sigle = s;
				STRINGTYPE rsigle;
				SQLiteStatHelper::VariableType ovar(oSet, sigle);
				rsigle = ovar.sigle();
				SQLiteStatHelper::VariableType *p = nullptr;
				for (size_t i = 0; i < oVars.size(); ++i) {
					SQLiteStatHelper::VariableType &vv = oVars[i];
					STRINGTYPE sx = vv.sigle();
					if (sx == rsigle) {
						p = &vv;
						break;
					}
				} // i
				Assert::IsTrue(p != nullptr);
				pVars[sigle] = p;
			}// s
			std::map<STRINGTYPE, SQLiteStatHelper::IndivType *> pInds;
			for (auto &s : rowNames) {
				STRINGTYPE sigle = s;
				STRINGTYPE rsigle;
				SQLiteStatHelper::IndivType ovar(oSet, sigle);
				rsigle = ovar.sigle();
				SQLiteStatHelper::IndivType *p = nullptr;
				for (size_t i = 0; i < oInds.size(); ++i) {
					SQLiteStatHelper::IndivType &vv = oInds[i];
					STRINGTYPE sx = vv.sigle();
					if (sx == rsigle) {
						p = &vv;
						break;
					}
				} // i
				Assert::IsTrue(p != nullptr);
				pInds[sigle] = p;
			}// s
			SQLiteStatHelper::values_vector oVals;
			for (size_t i = 0; i < nRows; ++i) {
				STRINGTYPE sigleind = rowNames[i];
				SQLiteStatHelper::IndivType *pInd = pInds[sigleind];
				Assert::IsTrue(pInd != nullptr);
				for (size_t j = 0; j < nCols; ++j) {
					STRINGTYPE siglevar = colNames[j];
					SQLiteStatHelper::VariableType *pVar = pVars[siglevar];
					Assert::IsTrue(pVar != nullptr);
					SQLiteStatHelper::ValueType val(*pVar, *pInd);
					if (!p->find_value(val)) {
						float f = (float)data[i * nCols + j];
						InfoValue vv(f);
						val.value(vv);
						oVals.push_back(val);
					}
				} // j
			} // i
			if (!oVals.empty()) {
				bRet = p->maintains_values(oVals);
				Assert::IsTrue(bRet);
			}
		} // import

	public:
		TEST_METHOD_INITIALIZE(setUp)
		{
			STRINGTYPE filename;
			InfoTestData::get_database_filename(filename);
			Assert::IsTrue(!filename.empty());
			m_man.reset(new SQLiteStatHelper(filename));
			SQLiteStatHelper *p = this->m_man.get();
			Assert::IsTrue(p != nullptr);
			Assert::IsTrue(p->is_valid());
		}// setUp
		TEST_METHOD_CLEANUP(tearDown)
		{
			m_man.reset();
		}// tearDown
	public:
		TEST_METHOD(TestFillMortalData)
		{
			STRINGTYPE name;
			size_t nRows = 0, nCols = 0;
			std::vector<DATATYPE> gdata;
			std::vector<STRINGTYPE> rowNames, colNames;
			InfoTestData::get_mortal_data(name, nRows, nCols, gdata, rowNames,
				colNames);
			Assert::IsTrue(!name.empty());
			Assert::IsTrue(nRows > 2);
			Assert::IsTrue(nCols > 2);
			Assert::IsTrue(colNames.size() >= nCols);
			Assert::IsTrue(rowNames.size() >= nRows);
			Assert::IsTrue(gdata.size() >= (size_t)(nCols * nRows));
			this->import(name, nRows, nCols, gdata, rowNames, colNames);
		}// TestFillMortalData
		TEST_METHOD(TestFillConsoData)
		{
			STRINGTYPE name;
			size_t nRows = 0, nCols = 0;
			std::vector<DATATYPE> gdata;
			std::vector<STRINGTYPE> rowNames, colNames;
			InfoTestData::get_conso_data(name, nRows, nCols, gdata, rowNames,
				colNames);
			Assert::IsTrue(!name.empty());
			Assert::IsTrue(nRows > 2);
			Assert::IsTrue(nCols > 2);
			Assert::IsTrue(colNames.size() >= nCols);
			Assert::IsTrue(rowNames.size() >= nRows);
			Assert::IsTrue(gdata.size() >= (size_t)(nCols * nRows));
			this->import(name, nRows, nCols, gdata, rowNames, colNames);
		}// TestFillConsolData
		TEST_METHOD(TestFillTestData)
		{
			STRINGTYPE name;
			size_t nRows = 0, nCols = 0;
			std::vector<DATATYPE> gdata;
			std::vector<STRINGTYPE> rowNames, colNames;
			InfoTestData::get_test_data(name, nRows, nCols, gdata, rowNames,
				colNames);
			Assert::IsTrue(!name.empty());
			Assert::IsTrue(nRows > 2);
			Assert::IsTrue(nCols > 2);
			Assert::IsTrue(colNames.size() >= nCols);
			Assert::IsTrue(rowNames.size() >= nRows);
			Assert::IsTrue(gdata.size() >= (size_t)(nCols * nRows));
			this->import(name, nRows, nCols, gdata, rowNames, colNames);
		}// TestFillMortalData
	};
}