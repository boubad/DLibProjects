#pragma once
#ifndef __DATAVECTOR_INDIVPROVIDER_H__
#define __DATAVECTOR_INDIVPROVIDER_H__
//////////////////////////////////////////////
#include "indiv.h"
////////////////////////////////////////////////
namespace info {
	/////////////////////////////
	template<typename U = unsigned long, typename STRINGTYPE = std::string>
	class DataVectorIndivSource : public IIndivSource<U, STRINGTYPE>, private boost::noncopyable {
		using mutex_type = std::mutex;
	public:
		using IndexType = U;
		using IndivType = Indiv<U, STRINGTYPE>;
		using DataMap = std::map<U, InfoValue>;
		using IndivTypePtr = std::shared_ptr<IndivType>;
		using SourceType = IIndivSource<U, STRINGTYPE>;
		using ints_vector = std::vector<U>;
		using doubles_vector = std::vector<double>;
		using strings_vector = std::vector<STRINGTYPE>;
		using indivptrs_vector = std::vector<IndivTypePtr>;
	private:
		std::atomic<size_t> m_current;
		size_t m_nrows;
		size_t m_ncols;
		doubles_vector m_data;
		ints_vector m_ids;
		ints_vector m_varids;
		strings_vector m_names;
		indivptrs_vector m_inds;
		//
		mutex_type _mutex;
	public:
		template <typename T>
		DataVectorIndivSource(const size_t nRows, const size_t nCols, const std::vector<T> &data,
			const ints_vector &indIds, const ints_vector &varIds, const strings_vector &names) :m_current(0), m_nrows(nRows), m_ncols(nCols),
			m_data(nRows * nCols),m_ids(nRows),m_varids(nCols),m_names(nRows),m_inds(nRows){
			assert(this->m_nrows > 0);
			assert(this->m_ncols > 0);
			size_t nn = (size_t)(this->m_nrows * this->m_ncols);
			assert(data.size() >= nn);
			assert(indIds.size() >= nRows);
			assert(varIds.size() >= nCols);
			assert(names.size() >= nRows);
			doubles_vector &vv = this->m_data;
			for (size_t i = 0; i < nRows; ++i) {
				(this->m_ids)[i] = indIds[i];
				(this->m_names)[i] = names[i];
				for (size_t j = 0; j < nCols; ++j) {
					size_t pos = (size_t)(i * nCols + j);
					vv[pos] = (double)data[pos];
				}// j
			}// i
			for (size_t i = 0; i < nCols; ++i) {
				(this->m_varids)[i] = varIds[i];
			}
		}
	public:
		virtual size_t count(void) {
			return (this->m_nrows);
		}
		virtual IndivTypePtr get(const size_t pos) {
			std::unique_lock<mutex_type> oLock(this->_mutex);
			indivptrs_vector &vv = this->m_inds;
			if (pos >= vv.size()) {
				return (IndivTypePtr());
			}
			IndivTypePtr oRet = vv[pos];
			if (oRet.get() != nullptr) {
				return (oRet);
			}
			DataMap oMap;
			const size_t nCols = this->m_ncols;
			const size_t base_pos = (size_t)(pos * nCols);
			ints_vector &varids = this->m_varids;
			doubles_vector &data = this->m_data;
			for (size_t i = 0; i < nCols; ++i) {
				double x = data[base_pos + i];
				InfoValue v(x);
				U key = varids[i];
				oMap[key] = v;
			}
			U aIndex = (this->m_ids)[pos];
			STRINGTYPE sSigle = (this->m_names)[pos];
			oRet.reset(new IndivType(aIndex, oMap, sSigle));
			vv[pos] = oRet;
			return (oRet);
		}
		virtual void reset(void) {
			std::unique_lock<mutex_type> oLock(this->_mutex);
			m_current = 0;
		}
		virtual IndivTypePtr next(void) {
			size_t n = this->m_current.load();
			if (n < this->m_ids.size()) {
				this->m_current.store((size_t)(n + 1));
				return (this->get(n));
			}
			return (IndivTypePtr());
		}
		virtual IndivTypePtr find(const IndexType aIndex) {
			ints_vector &vv = this->m_ids;
			const size_t n = vv.size();
			for (size_t i = 0; i < n; ++i) {
				if (vv[i] == aIndex) {
					return (this->get(i));
				}
			}// i
			return (IndivTypePtr());
		}
	};
	///////////////////////////////
}// namespace info
//////////////////////////////////////////////
#endif // !__DATAVECTOR_INDIVPROVIDER_H__
