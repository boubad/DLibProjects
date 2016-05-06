#include "stdafx.h"
#include "CppUnitTest.h"
/////////////////////////////////
#include <sqlitestathelper.h>
/////////////////////////////////
#include "infotestdata.h"
/////////////////////////////////
#include <memory>
///////////////////////////////////
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace info;

namespace UnitTestDatalib
{		
	TEST_CLASS(UnitTestSQLiteStatHelper)
	{
	protected:
		std::unique_ptr<SQLiteStatHelper> m_man;
		DBStatDataset m_oset;
	public:
		TEST_METHOD_INITIALIZE(Setup)
		{
			std::string name;
			InfoTestData::get_database_filename(name);
			m_man.reset(new SQLiteStatHelper(name));
			IStoreHelper *p = m_man.get();
			Assert::IsNotNull(p);
			Assert::IsTrue(p->is_valid());
			std::string setName;
			InfoTestData::get_mortal_name(setName);
			m_oset.set_sigle(setName);
			if (!p->find_dataset(m_oset)) {
				bool bRet = p->maintains_dataset(m_oset);
				Assert::IsTrue(bRet);
			}
			Assert::IsTrue(this->m_oset.id() != 0);
		}// Setup
		TEST_METHOD_CLEANUP(Teardown)
		{
			m_man.reset();
		}// Teardown
		TEST_METHOD(TestDBDatasets)
		{
			IStoreHelper *pMan = m_man.get();
			Assert::IsNotNull(pMan);
			Assert::IsTrue(pMan->is_valid());
			//
			int count = 0;
			bool bRet = pMan->find_all_datasets_count(count);
			Assert::IsTrue(bRet);
			Assert::IsTrue(count > 0);
			std::vector<DBStatDataset> oSets;
			bRet = pMan->find_all_datasets(oSets, 0, count);
			Assert::IsTrue(bRet);
			Assert::AreEqual((size_t)count, oSets.size());
			//
			std::vector<IntType> oIds;
			bRet = pMan->find_all_datasets_ids(oIds, 0, count);
			Assert::IsTrue(bRet);
			Assert::AreEqual((size_t)count, oIds.size());
			//
			std::string sigle;
			this->m_oset.get_sigle(sigle);
			DBStatDataset xSet(sigle);
			bRet = pMan->find_dataset(xSet);
			Assert::IsTrue(bRet);
			bRet = pMan->remove_dataset(xSet);
			Assert::IsTrue(bRet);
			bRet = pMan->find_dataset(xSet);
			Assert::IsFalse(bRet);
		}//TestDBDatasets

	};
}