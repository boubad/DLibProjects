#pragma once
///////////////////////////
#include "infoglobaldata.h"
///////////////////////////////////
#include <indiv.h>
#include <transformed_storeindivsource.h>
////////////////////////////////////
#include "infotestdata.h"
//////////////////////////////////
template<typename IDTYPE = unsigned long, typename INTTYPE = unsigned long,
	typename STRINGTYPE = std::string, typename WEIGHTYPE = double>
	class TestIndivSourceFixture : public InfoGlobalInit {
	private:
		using DatasetType = info::StatDataset<IDTYPE, INTTYPE, STRINGTYPE>;
		using StoreIndivSourceType = info::StoreIndivSource<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	public:
		using IndexType = IDTYPE;
		using IndivType = info::Indiv<IDTYPE, STRINGTYPE>;
		using IndivTypePtr = std::shared_ptr<IndivType>;
		using StoreType = info::IStatStore<unsigned long, unsigned long, STRINGTYPE, double>;
		using SourceType = info::IIndivSource<IDTYPE, STRINGTYPE>;
	protected:
		std::unique_ptr<StoreIndivSourceType> m_source;
	protected:
		StoreType *store(void) {
			StoreType *p = this->m_man.get();
			assert(p != nullptr);
			return p;
		}
		SourceType *source(void) {
			SourceType *p = this->m_source.get();
			assert(p != nullptr);
			return p;
		}
	public:
		TestIndivSourceFixture() {
			STRINGTYPE sigle;
			info::InfoTestData::get_test_name(sigle);
			StoreType *pStore = this->store();
			StoreIndivSourceType *p = new StoreIndivSourceType(pStore, sigle);
			assert(p != nullptr);
			this->m_source.reset(p);
		}// TestIndivSorceFixture
		virtual ~TestIndivSourceFixture(){}
};
