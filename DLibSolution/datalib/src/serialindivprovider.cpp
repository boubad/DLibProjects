#include "../include/serialindivprovider.h"
//////////////////////
#include <dlib/assert.h>
////////////////
namespace info {
	/////////////////////////////////////////
	SerialIndivProvider::SerialIndivProvider(IIndivProvider *pProvider) :m_provider(pProvider), m_current(0) {
		DLIB_ASSERT(this->m_provider != nullptr, "Provider cannot be null");
		DLIB_ASSERT(this->m_provider->is_valid(), "Provider is note valid");
	}
	SerialIndivProvider::~SerialIndivProvider() {

	}
	bool SerialIndivProvider::reset(void) {
		IIndivProvider *p = this->m_provider;
		if (p == nullptr) {
			return (false);
		}
		if (!p->is_valid()) {
			return (false);
		}
		{
			dlib::auto_mutex oLock(this->_mutex);
			this->m_current = 0;
		}
		return (true);
	}
	bool SerialIndivProvider::next(Indiv &oInd, const VariableMode mode /*= VariableMode::modeAll*/) {
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
			dlib::auto_mutex oLock(this->_mutex);
			size_t n = this->m_current;
			if (n < nMax) {
				bRet = p->find_indiv_at(n, oInd, mode);
			}
			if (bRet) {
				this->m_current = n + 1;
			}
		}
		return (bRet);
	} // next
	  //
	bool SerialIndivProvider::is_valid(void) {
		return ((this->m_provider != nullptr) && this->m_provider->is_valid());
	}
	bool SerialIndivProvider::indivs_count(size_t &nCount) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		IIndivProvider *p = this->m_provider;
		DLIB_ASSERT(p != nullptr, "Provider cannot be null");
		return( p->indivs_count(nCount));
	}
	bool SerialIndivProvider::all_indivs_ids(ints_vector &oIds) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		IIndivProvider *p = this->m_provider;
		DLIB_ASSERT(p != nullptr, "Provider cannot be null");
		return (p->all_indivs_ids(oIds));
	}
	bool SerialIndivProvider::contains_id(const IntType aId) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		IIndivProvider *p = this->m_provider;
		DLIB_ASSERT(p != nullptr, "Provider cannot be null");
		return (p->contains_id(aId));
	}
	bool SerialIndivProvider::find_indiv(const IntType aIndex, Indiv &oInd,
		const VariableMode mode/* = VariableMode::modeAll*/) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		IIndivProvider *p = this->m_provider;
		DLIB_ASSERT(p != nullptr, "Provider cannot be null");
		return (p->find_indiv(aIndex, oInd, mode));
	}
	bool SerialIndivProvider::find_indiv_at(const size_t pos, Indiv &oInd,
		const VariableMode mode /*= VariableMode::modeAll*/) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		IIndivProvider *p = this->m_provider;
		DLIB_ASSERT(p != nullptr, "Provider cannot be null");
		return (p->find_indiv_at(pos, oInd, mode));
	}
	bool SerialIndivProvider::distance(const IntType aIndex1, const IntType &aIndex2,
		double &dRes, const VariableMode mode /*= VariableMode::modeAll*/) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		IIndivProvider *p = this->m_provider;
		DLIB_ASSERT(p != nullptr, "Provider cannot be null");
		return (p->distance(aIndex1, aIndex2, dRes, mode));
	}
	bool SerialIndivProvider::distance_at(const size_t pos1, const size_t pos2,
		double &dRes, const VariableMode mode /*= VariableMode::modeAll*/) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		IIndivProvider *p = this->m_provider;
		DLIB_ASSERT(p != nullptr, "Provider cannot be null");
		return (p->distance_at(pos1, pos2, dRes, mode));
	}
	bool SerialIndivProvider::find_variables(variables_map &ovars) {
		DLIB_ASSERT(this->is_valid(), "this is not valid");
		IIndivProvider *p = this->m_provider;
		DLIB_ASSERT(p != nullptr, "Provider cannot be null");
		return (p->find_variables(ovars));
	}
	///////////////////////////////////////////
}//namespace info
