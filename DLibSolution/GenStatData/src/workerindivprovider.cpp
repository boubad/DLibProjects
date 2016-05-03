#include "../include/workerindivprovider.h"
////////////////
#include <dlib/assert.h>

////////////////////////////
#include <algorithm>
//////////////////////
namespace info {
	///////////////////////////////////////////
	WorkerIndivProvider::WorkerIndivProvider(IIndivProvider *pProvider) :m_provider(pProvider)
	{
		DLIB_ASSERT(this->m_provider != nullptr, "Provider cannot be null");
		DLIB_ASSERT(this->m_provider->is_valid(), "Provider is note valid");
		variables_map ovars;
		bool bRet = this->m_provider->find_variables(this->m_vars);
		DLIB_ASSERT(bRet, "Cannot get variables");
		ints_set &oSet = this->m_filter;
		oSet.clear();
		std::for_each(this->m_vars.begin(), this->m_vars.end(), [&oSet](const std::pair<IntType, DBStatVariable> &oPair) {
			oSet.insert(oPair.first);
		});
	}
	WorkerIndivProvider::~WorkerIndivProvider() {
		this->m_provider = nullptr;
	}
	void WorkerIndivProvider::filter_indiv(Indiv &oInd) const {
		WorkerIndivProvider & o = const_cast<WorkerIndivProvider &>(*this);
		DbValueMap oRes;
		const DbValueMap &oSrc = oInd.data();
		{
			dlib::auto_unlock_readonly oLock(o._mutex);
			const ints_set &oSet = this->m_filter;
			std::for_each(oSrc.begin(), oSrc.end(), [&](const std::pair<IntType, DbValue> &oPair) {
				const IntType key = oPair.first;
				DbValue val = oPair.second;
				if (!val.empty()) {
					if (oSet.find(key) != oSet.end()) {
						oRes[key] = oPair.second;
					}
				}// not empty
			});
		}// sync
		oInd.set_data(oRes);
	}

	bool WorkerIndivProvider::find_indiv_filtered(const IntType aIndex, Indiv &oInd, const VariableMode mode /*= VariableMode::modeAll*/) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		IIndivProvider *p = this->m_provider;
		DLIB_ASSERT(p != nullptr, "Provider cannot be null");
		if (!p->find_indiv(aIndex, oInd, mode)) {
			return (false);
		}
		this->filter_indiv(oInd);
		return (true);
	}
	bool WorkerIndivProvider::find_indiv_at_filtered(const size_t pos, Indiv &oInd, const VariableMode mode /*= VariableMode::modeAll*/) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		IIndivProvider *p = this->m_provider;
		DLIB_ASSERT(p != nullptr, "Provider cannot be null");
		if (!p->find_indiv_at(pos, oInd, mode)) {
			return (false);
		}
		this->filter_indiv(oInd);
		return (true);
	}
	bool WorkerIndivProvider::distance_filtered(const IntType aIndex1, const IntType &aIndex2, double &dRes, const VariableMode mode /*= VariableMode::modeAll*/) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		Indiv oInd1, oInd2;
		dRes = 0;
		IIndivProvider *p = this->m_provider;
		DLIB_ASSERT(p != nullptr, "Provider cannot be null");
		if (!p->find_indiv(aIndex1, oInd1, mode)) {
			return (false);
		}
		if (!p->find_indiv(aIndex2, oInd2, mode)) {
			return (false);
		}
		this->filter_indiv(oInd1);
		this->filter_indiv(oInd2);
		dRes = oInd1.distance(oInd2);
		return (true);
	}
	bool WorkerIndivProvider::distance_at_filtered(const size_t pos1, const size_t pos2, double &dRes, const VariableMode mode /*= VariableMode::modeAll*/) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		Indiv oInd1, oInd2;
		dRes = 0;
		if (!this->find_indiv_at(pos1, oInd1, mode)) {
			return (false);
		}
		if (!this->find_indiv_at(pos2, oInd2, mode)) {
			return (false);
		}
		this->filter_indiv(oInd1);
		this->filter_indiv(oInd2);
		dRes = oInd1.distance(oInd2);
		return (true);
	}
	void WorkerIndivProvider::get_filter(variables_vector &oVars) const {
		oVars.clear();
		WorkerIndivProvider & o = const_cast<WorkerIndivProvider &>(*this);
		dlib::auto_unlock_readonly oLock(o._mutex);
		const ints_set &oSet = this->m_filter;
		const variables_map &xmap = this->m_vars;
		std::for_each(oSet.begin(), oSet.end(), [&](const IntType &key) {
			auto it = xmap.find(key);
			if (it != xmap.end()) {
				DBStatVariable v = (*it).second;
				oVars.push_back(v);
			}
		});
	}
	void WorkerIndivProvider::set_filter(const variables_vector &oVars) {
		dlib::auto_unlock oLock(this->_mutex);
		ints_set &oSet = this->m_filter;
		oSet.clear();
		std::for_each(oVars.begin(), oVars.end(), [&](const DBStatVariable &ovar) {
			oSet.insert(ovar.id());
		});
	}
	bool WorkerIndivProvider::reset(void) {
		IIndivProvider *p = this->m_provider;
		if (p == nullptr) {
			return (false);
		}
		if (!p->is_valid()) {
			return (false);
		}
		{
			dlib::auto_unlock oLock(this->_mutex);
			this->m_current = 0;
		}
		return (true);
	}
	bool WorkerIndivProvider::next(Indiv &oInd, const VariableMode mode /*= VariableMode::modeAll*/) {
		IIndivProvider *p = this->m_provider;
		if (p == nullptr) {
			return (false);
		}
		if (!p->is_valid()) {
			return (false);
		}
		size_t nMax = 0;
		if (!p->indivs_count(nMax)) {
			return (false);
		}
		bool bRet = false;
		{
			dlib::auto_unlock oLock(this->_mutex);
			size_t n = this->m_current;
			if (n < nMax) {
				bRet = p->find_indiv_at(n, oInd, mode);
			}
			if (bRet) {
				this->m_current = n + 1;
			}
		}
		this->filter_indiv(oInd);
		return (bRet);
	} // next
	bool WorkerIndivProvider::is_valid(void) {
		bool bRet = (this->m_provider != nullptr) && (this->m_provider->is_valid());
		return (bRet);
	}
	bool WorkerIndivProvider::indivs_count(size_t &nCount) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		return (this->m_provider->indivs_count(nCount));
	}
	bool WorkerIndivProvider::all_indivs_ids(ints_vector &oIds) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		return (this->m_provider->all_indivs_ids(oIds));
	}
	bool WorkerIndivProvider::contains_id(const IntType aId) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		return (this->m_provider->contains_id(aId));
	}
	bool WorkerIndivProvider::find_indiv(const IntType aIndex, Indiv &oInd,
		const VariableMode mode /*= VariableMode::modeA*/) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		return (this->find_indiv_filtered(aIndex, oInd, mode));
	}
	bool WorkerIndivProvider::find_indiv_at(const size_t pos, Indiv &oInd,
		const VariableMode mode /*= VariableMode::modeA*/) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		return (this->find_indiv_at_filtered(pos, oInd, mode));
	}
	bool WorkerIndivProvider::distance(const IntType aIndex1, const IntType &aIndex2,
		double &dRes, const VariableMode mode /*= VariableMode::modeAll*/) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		return (this->distance_filtered(aIndex1, aIndex2, dRes, mode));
	}
	bool WorkerIndivProvider::distance_at(const size_t pos1, const size_t pos2,
		double &dRes, const VariableMode mode /*= VariableMode::modeAl*/) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		return (this->distance_at_filtered(pos1, pos2, dRes, mode));
	}
	bool WorkerIndivProvider::find_variables(variables_map &ovars) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		ovars = this->m_vars;
		return (true);
	}
	///////////////////////////////////////////////
}// namespace info
