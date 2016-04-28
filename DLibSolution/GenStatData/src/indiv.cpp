#include "../include/indiv.h"
////////////////////////////////
namespace info {
///////////////////////////////////////////
	Indiv::Indiv(){}
	Indiv::Indiv(const IntType aIndex):DBStatIndiv(aIndex) {

	}
	Indiv::Indiv(const IntType aIndex, const DataMapType &oMap): DBStatIndiv(aIndex),m_map(oMap){

	}
	Indiv::Indiv(const DBStatIndiv &oBaseInd):DBStatIndiv(oBaseInd) {

	}
	Indiv::Indiv(const Indiv &other): DBStatIndiv(other),m_map(other.m_map) {

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
	const Indiv::DataMapType &Indiv::data(void) const {
		return (this->m_map);
	}
	Indiv::DataMapType &Indiv::data(void) {
		return (this->m_map);
	}
	void Indiv::get_data(DataMapType &oMap) const {
		oMap = this->m_map;
	}
	void Indiv::set_data(const DataMapType &oMap) {
		this->m_map = oMap;
	}
	double Indiv::distance(const Indiv &other) const {
		double dRet = 0;
		DataMapType m1, m2;
		this->get_data(m1);
		other.get_data(m2);
		size_t nc = 0;
		for (auto it = m1.begin(); it != m1.end(); ++it) {
			const IntType key = (*it).first;
			auto jt = m2.find(key);
			if (jt != m2.end()) {
				const DbValue &v1 = (*it).second;
				const DbValue &v2 = (*jt).second;
				double t = v1.double_value() - v2.double_value();
				dRet += t * t;
				++nc;
			}// ok
		}// it
		if (nc > 1) {
			dRet /= nc;
		}
		return dRet;
	}// distance
	//////////////////////////////////////////
}// namespace info
