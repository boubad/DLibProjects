//indivcluster.cpp
///////////////////////////////
#include "../include/indivcluster.h"
///////////////////////////////////////
#include <dlib/assert.h>
/////////////////////////////////////
namespace info {
	////////////////////////////////////////
	IndivCluster::IndivCluster(IIndivProvider *pProvider /*= nullptr*/) :m_provider(pProvider) {

	}
	IndivCluster::IndivCluster(const IntType aIndex, const DataMapType &oMap, IIndivProvider *pProvider /*= nullptr*/) : Indiv(aIndex, oMap), m_provider(pProvider) {

	}
	IndivCluster::IndivCluster(const Indiv &oInd, IIndivProvider *pProvider /*= nullptr*/) : Indiv(oInd), m_provider(pProvider) {

	}
	IndivCluster::IndivCluster(const IndivCluster &other) {
		IndivCluster &o = const_cast<IndivCluster &>(other);
		dlib::auto_mutex oLock2(other._mutex);
		Indiv::operator=(other);
		this->m_provider = other.m_provider;
		this->m_members = other.m_members;
	}
	IndivCluster & IndivCluster::operator=(const IndivCluster &other) {
		if (this != &other) {
			dlib::auto_mutex oLock1(this->_mutex);
			dlib::auto_mutex oLock2(other._mutex);
			Indiv::operator=(other);
			this->m_provider = other.m_provider;
			this->m_members = other.m_members;
		}
		return (*this);
	}
	IndivCluster::~IndivCluster() {

	}
	void IndivCluster::swap(IndivCluster &other) {
		IndivCluster t(*this);
		*this = other;
		other = t;
	}
	bool IndivCluster::add(const Indiv &oInd, bool bOrder /*= false*/) {
		ints_deque old;
		const IntType aIndex = oInd.id();
		if (aIndex == 0) {
			return (false);
		}
		{
			dlib::auto_mutex oLock(this->_mutex);
			old = this->m_members;
		}
		for (auto it = old.begin(); it != old.end(); ++it) {
			if ((*it) == aIndex) {
				return (false);
			}
		}// it
		if (bOrder) {
			if (old.size() < 2) {
				old.push_back(aIndex);
			}
			else {
				IIndivProvider *pProvider = this->m_provider;
				DLIB_ASSERT(pProvider != nullptr, "Provider must not be null");
				Indiv oInd1, oInd2;
				bool bRet = pProvider->find_indiv(old.front(), oInd1) && pProvider->find_indiv(old.back(), oInd2);
				DLIB_ASSERT(bRet, "Indivs not found by provider");
				double d1 = oInd.distance(oInd1);
				double d2 = oInd.distance(oInd2);
				if (d1 < d2) {
					old.push_front(aIndex);
				}
				else {
					old.push_back(aIndex);
				}
			}
		}
		else {
			old.push_back(aIndex);
		}
		{
			dlib::auto_mutex oLock(this->_mutex);
			this->m_members = old;
		}
		return (true);
	}// add
	IndivCluster & IndivCluster::operator+=(const Indiv &oInd) {
		this->add(oInd);
		return (*this);
	}
	void IndivCluster::get_data(DataMapType &oMap) const {
		IndivCluster &o = const_cast<IndivCluster &>(*this);
		{
			dlib::auto_mutex oLock(o._mutex);
			Indiv::get_data(oMap);
		}
	}
	void IndivCluster::set_data(const DataMapType &oMap) {
		dlib::auto_mutex oLock(this->_mutex);
		Indiv::set_data(oMap);
	}
	IIndivProvider *IndivCluster::get_provider(void) const {
		return (this->m_provider);
	}
	void IndivCluster::set_provider(IIndivProvider *pProvider) {
		this->m_provider = pProvider;
	}
	void IndivCluster::get_members(ints_deque &oRes) const {
		IndivCluster &o = const_cast<IndivCluster &>(*this);
		{
			dlib::auto_mutex oLock(o._mutex);
			oRes = this->m_members;
		}
	}
	void IndivCluster::clear_members(void) {
		dlib::auto_mutex oLock(this->_mutex);
		this->m_members.clear();
	}
	void IndivCluster::update_center(void) {
		IIndivProvider *pProvider = this->m_provider;
		DLIB_ASSERT(pProvider != nullptr, "Provider must not be null");
		ints_deque members;
		this->get_members(members);
		if (members.empty()) {
			return;
		}
		std::map<IntType, size_t> counts;
		std::map<IntType, double> sommes;
		for (auto it = members.begin(); it != members.end(); ++it) {
			const IntType aIndex = *it;
			DLIB_ASSERT(aIndex != 0, "Index cannot be null");
			Indiv oInd;
			bool bRet = pProvider->find_indiv(aIndex, oInd);
			DLIB_ASSERT(bRet, "Indiv not found");
			DataMapType curData;
			oInd.get_data(curData);
			for (auto jt = curData.begin(); jt != curData.end(); ++jt) {
				const IntType key = (*jt).first;
				const DbValue & vv = (*jt).second;
				const double v = vv.double_value();
				if (counts.find(key) == counts.end()) {
					counts[key] = 1;
					sommes[key] = v;
				}
				else if (sommes.find(key) != sommes.end()) {
					double s = sommes[key];
					s += v;
					sommes[key] = s;
					size_t n = counts[key];
					++n;
					counts[key] = n;
				}
			}// jt
		}// it
		DataMapType oCenter;
		for (auto it = sommes.begin(); it != sommes.end(); ++it) {
			const IntType key = (*it).first;
			if (counts.find(key) != counts.end()) {
				size_t n = counts[key];
				double s = (*it).second;
				if (n > 1) {
					s /= n;
				}
				DbValue vv(s);
				oCenter[key] = vv;
			}//
		}// it
		this->set_data(oCenter);
	}//update_center
	/////////////////////////////////////////
}// namespace info
