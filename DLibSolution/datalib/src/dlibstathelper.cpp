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
		DLIB_ASSERT(bRet, "Cannot get indivs count");
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
		ints_vector &oIds = this->m_ids;
		oIds.clear();
		variables_map &oVars = this->m_vars;
		oVars.clear();
		variables_map xVars;
		bRet = pProvider->find_variables(xVars);
		DLIB_ASSERT(bRet, "Cannot get variables");
		std::for_each(oSet.begin(), oSet.end(), [&](const std::pair<IntType, InfoStatDataPtr> &oPair) {
			IntType key = oPair.first;
			auto it = xVars.find(key);
			if (it != xVars.end()) {
				oIds.push_back(key);
				oVars[key] = xVars[key];
			}// found
		});
	}// initialize
	void DLibStatHelper::convert_indiv(const Indiv &oInd, double_ptr &xarray) const {
		const ints_vector &oIds = this->m_ids;
		const size_t n = oIds.size();
		const DbValueMap &src = oInd.data();
		const infostatdata_map &oStats = this->m_stats;
		xarray.reset(new double[n]);
		double *pData = xarray.get();
		DLIB_ASSERT(pData != nullptr, "pData cannot be null");
		for (size_t i = 0; i < n; ++i) {
			const IntType key = oIds[i];
			double dval = 0;
			auto it = src.find(key);
			if (it != src.end()) {
				DbValue v = (*it).second;
				dval = v.double_value();
			}
			else {
				auto jt = oStats.find(key);
				if (jt != oStats.end()) {
					const InfoStatDataPtr &st = (*jt).second;
					const InfoStatData *ps = st.get();
					dval = ps->average();
				}
			}
			pData[i] = dval;
		}// i
	}// convert_indiv
	void DLibStatHelper::convert_indiv(const Indiv &oInd, indiv_type &xarray) const {
		const ints_vector &oIds = this->m_ids;
		const size_t n = oIds.size();
		xarray.set_size(1, n);
		const DbValueMap &src = oInd.data();
		const infostatdata_map &oStats = this->m_stats;
		for (size_t i = 0; i < n; ++i) {
			const IntType key = oIds[i];
			double dval = 0;
			auto it = src.find(key);
			if (it != src.end()) {
				DbValue v = (*it).second;
				dval = v.double_value();
			}
			else {
				auto jt = oStats.find(key);
				if (jt != oStats.end()) {
					const InfoStatDataPtr &st = (*jt).second;
					const InfoStatData *ps = st.get();
					dval = ps->average();
				}
			}
			xarray(i) = dval;
		}// i
	}// convert_indiv
	size_t  DLibStatHelper::convert_indivs(std::vector<double_ptr> &ovec) {
		ovec.clear();
		size_t nc = 0;
		IIndivProvider *pProvider = this->m_provider;
		DLIB_ASSERT(pProvider != nullptr, "Provider cannot be null");
		bool bRet = pProvider->indivs_count(nc);
		DLIB_ASSERT(bRet, "Cannot get indivs count");
		infostatdata_map &oSet = this->m_stats;
		dlib::mutex _mutex;
		size_t num_threads = INFO_NUM_THREADS;
		VariableMode mode = VariableMode::modeNumeric;
		dlib::parallel_for(num_threads, (size_t)0, nc, [&](size_t i) {
			Indiv oInd;
			if (pProvider->find_indiv_at(i, oInd, mode)) {
				double_ptr xind;
				this->convert_indiv(oInd, xind);
				{
					dlib::auto_mutex oLock(_mutex);
					ovec.push_back(xind);
				}
			}// find
		});
		return (ovec.size());
	}// convert_indivs
	size_t DLibStatHelper::convert_indivs(std::vector<indiv_type> &ovec) {
		ovec.clear();
		size_t nc = 0;
		size_t nbVars = this->m_ids.size();
		IIndivProvider *pProvider = this->m_provider;
		DLIB_ASSERT(pProvider != nullptr, "Provider cannot be null");
		bool bRet = pProvider->indivs_count(nc);
		DLIB_ASSERT(bRet, "Cannot get indivs count");
		infostatdata_map &oSet = this->m_stats;
		dlib::mutex _mutex;
		size_t num_threads = INFO_NUM_THREADS;
		VariableMode mode = VariableMode::modeNumeric;
		dlib::parallel_for(num_threads, (size_t)0, nc, [&](size_t i) {
			Indiv oInd;
			if (pProvider->find_indiv_at(i, oInd, mode)) {
				indiv_type xind;
				xind.set_size(1, nbVars);
				this->convert_indiv(oInd, xind);
				{
					dlib::auto_mutex oLock(_mutex);
					ovec.push_back(xind);
				}
			}// find
		});
		return (ovec.size());
	}// convert_indivs
	void  DLibStatHelper::convert_indivs(size_t &nRows, size_t &nCols,
		double_ptr &oPtr) {
		std::vector<double_ptr> ovec;
		nRows = this->convert_indivs(ovec);
		nCols = this->m_ids.size();
		if ((nRows < 1) || (nCols < 1)) {
			oPtr.reset();
			return;
		}
		oPtr.reset(new double[nRows * nCols]);
		double *pp = oPtr.get();
		DLIB_ASSERT(pp != nullptr, "pp cannot be null");
		for (size_t i = 0; i < nRows; ++i) {
			const double_ptr &o = ovec[i];
			const double *pSrc = o.get();
			DLIB_ASSERT(pSrc != nullptr, "pSrc cannot be null");
			for (size_t j = 0; j < nCols; ++j) {
				pp[i * nCols + j] = pSrc[j];
			}// j
		}// i
	}//convert_indivs
///////////////////////////////////////
}// namespace info
