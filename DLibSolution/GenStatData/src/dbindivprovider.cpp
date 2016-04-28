#include "../include/dbindivprovider.h"
#include "../include/statdbmanager.h"
#include "../include/stringconvert.h"
//////////////////////////////////
#include <dlib/assert.h>
///////////////////////////////////////////
namespace info {
	/*
	StatDBManager *m_pman;
	size_t   m_current;
	ints_vector m_ids;
	DBStatDataset m_oset;
	*/
	////////////////////////////////////
	DBIndivProvider::DBIndivProvider(StatDBManager *pMan, const std::string &setName) :m_pman(pMan), m_current(0) {
		this->initialize(setName);
	}
	DBIndivProvider::DBIndivProvider(StatDBManager *pMan, const std::wstring &setName) : m_pman(pMan), m_current(0) {
		this->initialize(setName);
	}
	DBIndivProvider::DBIndivProvider(const DBIndivProvider &other) : m_pman(other.m_pman), m_current(other.m_current),
		m_ids(other.m_ids), m_oset(other.m_oset) {
	}
	DBIndivProvider & DBIndivProvider::operator=(const DBIndivProvider &other) {
		if (this != &other) {
			this->m_pman = other.m_pman;
			this->m_current = other.m_current;
			this->m_ids = other.m_ids;
			this->m_oset = other.m_oset;
		}
		return (*this);
	}
	DBIndivProvider::~DBIndivProvider() {
	}
	void DBIndivProvider::initialize(const std::string &setName) {
		StatDBManager *pMan = this->m_pman;
		if ((pMan != nullptr) && pMan->is_valid()) {
			this->m_oset.set_sigle(setName);
			if (pMan->find_dataset(this->m_oset)) {
				pMan->get_dataset_indivs_ids(this->m_oset, this->m_ids);
			}// found
		}// valid
	}// initialize
	void DBIndivProvider::initialize(const std::wstring &setName) {
		std::string s = StringConvert::ws2s(setName);
		this->initialize(s);
	}
	//////////////////////////////////////////
	bool DBIndivProvider::is_valid(void) {
		return (
			(this->m_pman != nullptr) &&
			this->m_pman->is_valid() &&
			(this->m_oset.id() != 0)
			);
	}
	bool DBIndivProvider::indivs_count(size_t &nRows) {
		if (this->is_valid()) {
			nRows = (this->m_ids).size();
			return (true);
		}
		return (false);
	}
	bool DBIndivProvider::indiv_at(const size_t pos, Indiv &oInd) {
		bool bRet = false;
		if (this->is_valid()) {
			StatDBManager *pMan = this->m_pman;
			DLIB_ASSERT(pMan != nullptr, "StatDataManager cannot be null");
			ints_vector &v = this->m_ids;
			if (pos >= v.size()) {
				return (false);
			}
			IntType aIndex = v[pos];
			DLIB_ASSERT(aIndex != 0, "index cannot be zero");
			DBStatIndiv xInd(this->m_oset);
			xInd.id(aIndex);
			DLIB_ASSERT(this->m_oset.id() != 0, "Invalid dataset id");
			bool bRet = pMan->find_indiv(xInd);
			if (!bRet) {
				return (false);
			}
			values_vector vv;
			if ((bRet = pMan->get_indiv_values(xInd, vv)) != true) {
				return (false);
			}
			oInd = Indiv(xInd);
			DataMapType oMap;
			for (auto it = vv.begin(); it != vv.end(); ++it) {
				const DBStatValue &vx = *it;
				const DbValue vz = vx.get_value();
				if (!vz.empty()) {
					IntType key = vx.get_variable_id();
					oMap[key] = vz;
				}
			}// id
			oInd.set_data(oMap);
			return (true);
		}// vamid
		return (bRet);
	}// indiv_at
	bool DBIndivProvider::reset(void) {
		if (this->is_valid()) {
			this->m_current = 0;
			return (true);
		}
		return (false);
	}
	bool DBIndivProvider::next(Indiv &oInd) {
		if (this->is_valid()) {
			size_t nCur = 0;
			{
				dlib::auto_mutex lock(this->_mutex);
				size_t nCur = this->m_current;
				this->m_current = (size_t)(nCur + 1);
			}
			return this->indiv_at(nCur, oInd);
		}// valid
		return (false);
	}// next
	/////////////////////////////////////////
}// namespace info
/////////////////////////////////////
