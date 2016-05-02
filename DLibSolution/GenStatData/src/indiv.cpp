#include "../include/indiv.h"
////////////////////////////////
#include <algorithm>
/////////////////////////////////////
namespace info {
	///////////////////////////////////////////
	Indiv::Indiv() {}
	Indiv::Indiv(const IntType aIndex) :DBStatIndiv(aIndex) {

	}
	Indiv::Indiv(const IntType aIndex, const DbValueMap &oMap) : DBStatIndiv(aIndex), m_map(oMap) {

	}
	Indiv::Indiv(const DBStatIndiv &oBaseInd) : DBStatIndiv(oBaseInd) {

	}
	Indiv::Indiv(const Indiv &other) : DBStatIndiv(other), m_map(other.m_map) {

	}
	Indiv & Indiv::operator=(const Indiv &other) {
		if (this != &other) {
			DBStatIndiv::operator=(other);
			this->m_map = other.m_map;
		}
		return (*this);
	}
	Indiv::~Indiv() {

	}
	void Indiv::swap(Indiv &other) {
		Indiv t(*this);
		*this = other;
		other = t;
	}
	const Indiv::DbValueMap &Indiv::data(void) const {
		return (this->m_map);
	}
	Indiv::DbValueMap &Indiv::data(void) {
		return (this->m_map);
	}
	void Indiv::get_data(DbValueMap &oMap) const {
		oMap = this->m_map;
	}
	void Indiv::set_data(const DbValueMap &oMap) {
		this->m_map = oMap;
	}
	double Indiv::distance(const Indiv &other) const {
		double dRet = 0;
		DbValueMap m1;
		this->get_data(m1);
		DbValueMap m2;
		other.get_data(m2);
		size_t nc = 0;
		std::for_each(m1.begin(), m1.end(), [&](const std::pair<IntType, DbValue> &oPair) {
			const IntType key = oPair.first;
			auto jt = m2.find(key);
			if (jt != m2.end()) {
				const DbValue &v1 = oPair.second;
				const DbValue &v2 = (*jt).second;
				double t = v1.double_value() - v2.double_value();
				dRet += t * t;
				++nc;
			}// found
		});
		if (nc > 1) {
			dRet /= nc;
		}
		return dRet;
	}// distance
	std::ostream & Indiv::write_to(std::ostream &os) const {
		DBStatIndiv::write_to(os);
		const DbValueMap &m = this->m_map;
		size_t n = m.size();
		dlib::serialize(n, os);
		std::for_each(m.begin(), m.end(), [&](const std::pair<IntType, DbValue> &oPair) {
			const IntType key = oPair.first;
			const DbValue &v = oPair.second;
			dlib::serialize(key, os);
			serialize(v, os);
		});
		return (os);
	}
	std::istream & Indiv::read_from(std::istream &in) {
		DBStatIndiv::read_from(in);
		DbValueMap &m = this->m_map;
		m.clear();
		size_t n = 0;
		dlib::deserialize(n, in);
		for (size_t i = 0; i < n; ++i) {
			IntType key = 0;
			DbValue v;
			dlib::deserialize(key, in);
			deserialize(v, in);
			if ((key != 0) && (!v.empty())) {
				m[key] = v;
			}
		}// i
		return (in);
	}
	//////////////////////////////////////////
}// namespace info
