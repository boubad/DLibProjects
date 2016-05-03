#include "../include/dlibstathelper.h"
///////////////////////////////
#include <dlib/assert.h>
/////////////////////////////
#include <cmath>
#include <algorithm>
/////////////////////////////////
namespace info {
	/////////////////////////////////////
	/*
	IntType _index;
	double _vmin;
	double _vmax;
	double _vmean;
	double _vstd;
	size_t _count;
	size_t _somme1;
	size_t _somme2;
	*/
	///////////////////////////////////////
	InfoStatData::InfoStatData(IntType aIndex) :_index(aIndex), _vmin(0), _vmax(0), _vmean(0), _vstd(0), _count(0),
		_somme1(0), _somme2(0) {

	}
	InfoStatData::InfoStatData(const InfoStatData &other): _index(other._index), _vmin(other._vmin), _vmax(other._vmax),
		_vmean(other._vmean), _vstd(other._vstd), _count(other._count),
		_somme1(other._somme1), _somme2(other._somme2) {

	}
	InfoStatData & InfoStatData::operator=(const InfoStatData &other) {
		if (this != &other) {
			this->_index = other._index;
			this->_vmin = other._vmin;
			this->_vmax = other._vmax;
			this->_vmean = other._vmean;
			this->_vstd = other._vstd;
			this->_count = other._count;
			this->_somme1 = other._somme1;
			this->_somme2 = other._somme2;
		}
		return (*this);
	}
	InfoStatData::~InfoStatData() {

	}
	bool InfoStatData::operator==(const InfoStatData &other) const {
		return (this->_index == other._index);
	}
	bool InfoStatData::operator<(const InfoStatData &other) const {
		return (this->_index < other._index);
	}
	void InfoStatData::add(const double dval) {
		dlib::auto_mutex oLock(this->_mutex);
		if (this->_count < 1) {
			this->_vmin = dval;
			this->_vmax = dval;
		} else {
			if (dval < this->_vmin) {
				this->_vmin = dval;
			}
			if (dval > this->_vmax) {
				this->_vmax = dval;
			}
		}
		this->_count = this->_count + 1;
		this->_somme1 += dval;
		this->_somme2 += dval * dval;
	}
	void InfoStatData::compute(void) {
		dlib::auto_mutex oLock(this->_mutex);
		size_t nc = this->_count;
		if (nc > 0) {
			const double s1 = this->_somme1 / nc;
			this->_vmean = s1;
			double s2 = ((this->_somme2) / nc) - (s1 * s1);
			DLIB_ASSERT(s2 >= 0, "Variance cannot be negative");
			this->_vstd = std::sqrt(s2);
		}// nc
	}
	void InfoStatData::get(IntType &index, double &vmin, double &vmax, double &vmean, double &vstd) const {
		dlib::auto_mutex oLock(this->_mutex);
		index = this->_index;
		vmin = this->_vmin;
		vmax = this->_vmax;
		vmean = this->_vmean;
		vstd = this->_vstd;
	}
/////////////////////////////////////////
	DLibStatHelper::DLibStatHelper(IIndivProvider *pProvider) :m_provider(pProvider) {
		DLIB_ASSERT(this->m_provider != nullptr, "Provider cannot be null");
		DLIB_ASSERT(this->m_provider->is_valid(), "Provider is note valid");
	}
	DLibStatHelper::~DLibStatHelper() {

	}
///////////////////////////////////////
}// namespace info
