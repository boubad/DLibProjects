#pragma once
#ifndef __STOREINDIVPROVIDER_FIXTURE_H__
#define __STOREINDIVPROVIDER_FIXTURE_H__
///////////////////////////////////////////
#include <indiv.h>
#include <transformed_storeindivsource.h>
//////////////////////////////////
#include "memorystorefixture.h"
	//////////////////////////////////////
	template<typename IDTYPE = unsigned long, typename INTTYPE = int,
		typename STRINGTYPE = std::string, typename WEIGHTYPE = float>
		class StoreIndivSorceFixture : public MemoryStoreFixture<IDTYPE, INTTYPE,STRINGTYPE, WEIGHTYPE> {
		public:
			using StoreIndivSourceType = info::TranformedStoreIndivSource<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
			using IndivType = typename  StoreIndivSourceType::IndivType;
			using IndexType = typename IndivType::IndexType;
			using DataMap = typename IndivType::DataMap;
			using IndivTypePtr = std::shared_ptr<IndivType>;
			using MemoryStoreType = info::MemoryStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
			using IStoreType = info::IStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
			using DatasetType = typename IStoreType::DatasetType;
			using IndivSourceType = typename  StoreIndivSourceType::SourceType;
			using StoreIndivSourceType2 = info::StoreIndivSource<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
			//
			std::unique_ptr<StoreIndivSourceType> m_source;
			std::unique_ptr<StoreIndivSourceType2> m_testsource;
			IndivSourceType *get_source(void) {
				IndivSourceType *p = this->m_source.get();
				assert(p != nullptr);
				return (p);
			}
			IndivSourceType *get_test_source(void) {
				IndivSourceType *p = this->m_testsource.get();
				assert(p != nullptr);
				return (p);
			}
		public:
			StoreIndivSorceFixture() {
				std::string sigle = this->m_oset.sigle();
				IStoreType *pStore = this->get_store();
				assert(pStore != nullptr);
				StoreIndivSourceType *p = new StoreIndivSourceType(pStore, sigle);
				p->transformation(TransformationType::recode1000);
				assert(p != nullptr);
				this->m_source.reset(p);
				/*
				sigle = this->m_testset.sigle();
				StoreIndivSourceType2 *pp = new StoreIndivSourceType2(pStore, sigle);
				assert(pp != nullptr);
				this->m_testsource.reset(pp);
				*/
			}
			virtual ~StoreIndivSorceFixture() {
				this->m_source.reset();
			}
	};
////////////////////////////////////////////
#endif // !__STOREINDIVPROVIDER_FIXTURE_H__
