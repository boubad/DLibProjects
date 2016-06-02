/*
 * variablesource_fixture.h
 *
 *  Created on: 29 mai 2016
 *      Author: boubad
 */

#ifndef VARIABLESOURCE_FIXTURE_H_
#define VARIABLESOURCE_FIXTURE_H_
///////////////////////////////////////
#include <storevariablesource.h>
////////////////////////////////////////
namespace info {
///////////////////////////////////////
template<typename IDTYPE = unsigned long, typename INTTYPE = unsigned long,
		typename STRINGTYPE = std::string, typename WEIGHTYPE = double>
class TestVariableSourceFixture {
public:
	using IndivType = Indiv<IDTYPE, STRINGTYPE>;
	using DataMap = std::map<IDTYPE, InfoValue>;
	using IndivTypePtr = std::shared_ptr<IndivType>;
	using StoreType = IStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using StoreIndivSourceType = StoreVariableSource<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
	using SourceType = IIndivSource<IDTYPE, STRINGTYPE>;
	using StoreFixture = TestStoreFixture<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
private:
	std::unique_ptr<StoreFixture> m_store;
	std::unique_ptr<StoreIndivSourceType> m_test_source;
	std::unique_ptr<StoreIndivSourceType> m_conso_source;
	std::unique_ptr<StoreIndivSourceType> m_mortal_source;
public:
	TestVariableSourceFixture(StoreType *pStore = nullptr,
			bool bMemory = true) {
		StoreType *ps = pStore;
		if (ps == nullptr) {
			StoreFixture *pf = m_store.get();
			if (pf == nullptr) {
				m_store.reset(new StoreFixture());
				pf = m_store.get();
				assert(pf != nullptr);
			}
			ps = pf->get_store(bMemory);
			assert(ps != nullptr);
		}
		{
			STRINGTYPE sigle;
			InfoTestData::get_conso_name(sigle);
			assert(!sigle.empty());
			m_conso_source.reset(new StoreIndivSourceType(ps, sigle));
		}
		{
			STRINGTYPE sigle;
			InfoTestData::get_mortal_name(sigle);
			assert(!sigle.empty());
			m_mortal_source.reset(new StoreIndivSourceType(ps, sigle));
		}
		{
			STRINGTYPE sigle;
			InfoTestData::get_test_name(sigle);
			assert(!sigle.empty());
			m_test_source.reset(new StoreIndivSourceType(ps, sigle));
		}
	} //TestSourceFixture
	virtual ~TestVariableSourceFixture() {
	}
public:
	SourceType *mortal_source(void) {
		SourceType *p = m_mortal_source.get();
		assert(p != nullptr);
		return (p);
	}
	SourceType *conso_source(void) {
		SourceType *p = m_conso_source.get();
		assert(p != nullptr);
		return (p);
	}
	SourceType *test_source(void) {
		SourceType *p = m_test_source.get();
		assert(p != nullptr);
		return (p);
	}
};
////////////////////////////////////////////////
}// namespace info
//////////////////////////////////////

#endif /* VARIABLESOURCE_FIXTURE_H_ */
