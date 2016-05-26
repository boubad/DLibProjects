#pragma once
#ifndef __MEMORYSTOREFIXTURE_H__
#define __MEMORYSTOREFIXTURE_H__
////////////////////////////////////
#include <memorystatstore.h>
/////////////////////////////////
#include "infotestdata.h"
////////////////////////////////
template<typename IDTYPE = unsigned long, typename INTTYPE = int,
	typename STRINGTYPE = std::string, typename WEIGHTYPE = float>
	class MemoryStoreFixture {
	protected:
		using MemoryStoreType = info::MemoryStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
		using IStoreType = info::IStatStore<IDTYPE, INTTYPE, STRINGTYPE, WEIGHTYPE>;
		using DatasetType = typename IStoreType::DatasetType;
		//
		size_t m_nbrows;
		size_t m_nbcols;
		std::unique_ptr<MemoryStoreType> m_man;

		DatasetType m_mortalset;
		DatasetType m_consoset;
		DatasetType m_testset;
	public:
		MemoryStoreFixture() :
			m_nbrows(0), m_nbcols(0) {
			this->init_data();
			this->fill_mortal_data();
			this->fill_conso_data();
			this->fill_test_data();
			{
				STRINGTYPE sigle;
				info::InfoTestData::get_mortal_name(sigle);
				this->m_mortalset.sigle(sigle);
				IStoreType *p = this->m_man.get();
				assert(p != nullptr);
				bool bRet = p->find_dataset(this->m_mortalset);
				assert(bRet);
				assert(this->m_mortalset.id() != 0);
			}
			{
				STRINGTYPE sigle;
				info::InfoTestData::get_conso_name(sigle);
				this->m_consoset.sigle(sigle);
				IStoreType *p = this->m_man.get();
				assert(p != nullptr);
				bool bRet = p->find_dataset(this->m_consoset);
				assert(bRet);
				assert(this->m_consoset.id() != 0);
			}
			{
				STRINGTYPE sigle;
				info::InfoTestData::get_test_name(sigle);
				this->m_testset.sigle(sigle);
				IStoreType *p = this->m_man.get();
				assert(p != nullptr);
				bool bRet = p->find_dataset(this->m_testset);
				assert(bRet);
				assert(this->m_testset.id() != 0);
			}
		} // init
		virtual ~MemoryStoreFixture() {
			this->data_teardown();
		}
		IStoreType *get_store(void) {
			IStoreType *p = this->m_man.get();
			assert(p != nullptr);
			assert(p->is_valid());
			return (p);
		}
		const DatasetType & mortal_set(void) const {
			return (this->m_mortalset);
		}
		const DatasetType & conso_set(void) const {
			return (this->m_consoset);
		}
		const DatasetType & test_set(void) const {
			return (this->m_testset);
		}
	private:
		void data_teardown(void) {
			this->m_man.reset();
		} // data_teardown
		void fill_test_data(void) {
			STRINGTYPE name;
			size_t nRows = 0, nCols = 0;
			std::vector<int> gdata;
			std::vector<STRINGTYPE> rowNames, colNames;
			info::InfoTestData::get_test_data(name, nRows, nCols, gdata, rowNames,
				colNames);
			assert(!name.empty());
			assert(nRows > 2);
			assert(nCols > 2);
			this->m_nbrows = nRows;
			this->m_nbcols = nCols;
			assert(colNames.size() >= nCols);
			assert(rowNames.size() >= nRows);
			assert(gdata.size() >= (size_t)(nCols * nRows));
			this->import(name, nRows, nCols, gdata, rowNames, colNames);
		} // fill_test_data
		void fill_mortal_data(void) {
			STRINGTYPE name;
			size_t nRows = 0, nCols = 0;
			std::vector<int> gdata;
			std::vector<STRINGTYPE> rowNames, colNames;
			info::InfoTestData::get_mortal_data(name, nRows, nCols, gdata, rowNames,
				colNames);
			assert(!name.empty());
			assert(nRows > 2);
			assert(nCols > 2);
			this->m_nbrows = nRows;
			this->m_nbcols = nCols;
			assert(colNames.size() >= nCols);
			assert(rowNames.size() >= nRows);
			assert(gdata.size() >= (size_t)(nCols * nRows));
			this->import(name, nRows, nCols, gdata, rowNames, colNames);
		} // fill_mortal_data
		void fill_conso_data(void) {
			STRINGTYPE name;
			size_t nRows = 0, nCols = 0;
			std::vector<int> gdata;
			std::vector<STRINGTYPE> rowNames, colNames;
			info::InfoTestData::get_conso_data(name, nRows, nCols, gdata, rowNames,
				colNames);
			assert(!name.empty());
			assert(nRows > 2);
			assert(nCols > 2);
			assert(colNames.size() >= nCols);
			assert(rowNames.size() >= nRows);
			assert(gdata.size() >= (size_t)(nCols * nRows));
			this->import(name, nRows, nCols, gdata, rowNames, colNames);
		} // fill_mortal_data
		void init_data(void) {
			m_man.reset(new MemoryStoreType());
			MemoryStoreType *p = this->m_man.get();
			assert(p != nullptr);
			assert(p->is_valid());
		}
		void import(const std::string &name, size_t nRows, size_t nCols,
			const std::vector<int> &data,
			const std::vector<STRINGTYPE> &rowNames,
			const std::vector<STRINGTYPE> &colNames) {
			MemoryStoreType *p = this->m_man.get();
			assert(p != nullptr);
			assert(p->is_valid());
			std::string stype("float");
			bool bRet = p->import(name, nRows, nCols, data, rowNames, colNames,
				stype);
			assert(bRet);
		} // import
};
/////////////////////////////////////
#endif // !__MEMORYSTOREFIXTURE_H__
