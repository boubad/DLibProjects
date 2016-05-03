#include "../include/dlibstathelper.h"
#include "../include/info_constants.h"
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
	InfoStatData::InfoStatData(const InfoStatData &other) : _index(other._index), _vmin(other._vmin), _vmax(other._vmax),
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
		}
		else {
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
		this->initialize();
	}
	DLibStatHelper::~DLibStatHelper() {

	}
	void DLibStatHelper::initialize(void) {
		size_t nc = 0;
		IIndivProvider *pProvider = this->m_provider;
		DLIB_ASSERT(pProvider != nullptr, "Provider cannot be null");
		bool bRet = pProvider->indivs_count(nc);
		infostatdata_map &oSet = this->m_stats;
		dlib::mutex _mutex;
		size_t num_threads = INFO_NUM_THREADS;
		VariableMode mode = VariableMode::modeNumeric;
		dlib::parallel_for(num_threads, (size_t)0, nc, [&](size_t i) {
			Indiv oInd;
			if (pProvider->find_indiv_at(i, oInd, mode)) {
				const DbValueMap &oMap = oInd.data();
				std::for_each(oMap.begin(), oMap.end(), [&](const std::pair<IntType, DbValue> &oPair) {
					DbValue v = oPair.second;
					if (!v.empty()) {
						double dval = v.double_value();
						InfoStatData *pStat = nullptr;
						IntType key = oPair.first;
						{
							dlib::auto_mutex oLock(_mutex);
							auto it = oSet.find(key);
							if (it == oSet.end()) {
								InfoStatDataPtr o = std::make_shared<InfoStatData>(key);
								oSet[key] = o;
								pStat = o.get();
							}
							else {
								InfoStatDataPtr o = (*it).second;
								pStat = o.get();
							}
						}// sync
						DLIB_ASSERT(pStat != nullptr, "pStat cannot be null");
						pStat->add(dval);
					}// not empty;
				});
			}// find
		});
		bool bDone = false;
		while (!bDone) {
			bDone = true;
			if (oSet.empty()) {
				break;
			}
			std::set<IntType> oRemove;
			std::for_each(oSet.begin(), oSet.end(), [&](const std::pair<IntType, InfoStatDataPtr> &oPair) {
				InfoStatDataPtr o = oPair.second;
				InfoStatData *p = o.get();
				p->compute();
				double vmin, vmax, vmean, vstd;
				IntType nIndex;
				p->get(nIndex, vmin, vmax, vmean, vstd);
				if ((vmin >= vmax)) {
					bDone = false;
					oRemove.insert(nIndex);
				}
			});
			if (!oRemove.empty()) {
				std::for_each(oRemove.begin(), oRemove.end(), [&](IntType aIndex) {
					auto it = oSet.find(aIndex);
					if (it != oSet.end()) {
						bDone = false;
						oSet.erase(it);
					}
				});
			}// do
		}// not done
	}// initialize
///////////////////////////////////////
}// namespace info
