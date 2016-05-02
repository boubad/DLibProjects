#include "../include/dbindivprovider.h"
#include "../include/statdbmanager.h"
#include "../include/stringconvert.h"
///////////////////////////////////
#include <algorithm>
//////////////////////////////////
#include <dlib/assert.h>
///////////////////////////////////////////
namespace info {
	/*
	StatDBManager *m_pman;
	size_t   m_count;
	ints_vector m_ids;
	DBStatDataset m_oset;
	variables_vector m_vars;
	*/
	////////////////////////////////////
	DBIndivProvider::DBIndivProvider(StatDBManager *pMan, const DBStatDataset &oSet) :m_pman(pMan), m_oset(oSet) {
		this->initialize();
	}
	DBIndivProvider::DBIndivProvider(StatDBManager *pMan, const std::string &setName) : m_pman(pMan) {
		this->initialize(setName);
	}
	DBIndivProvider::DBIndivProvider(StatDBManager *pMan, const std::wstring &setName) : m_pman(pMan) {
		this->initialize(setName);
	}
	DBIndivProvider::DBIndivProvider(const DBIndivProvider &other) : m_pman(other.m_pman),
		m_oset(other.m_oset), m_ids(other.m_ids), m_vars(other.m_vars) {
	}
	DBIndivProvider & DBIndivProvider::operator=(const DBIndivProvider &other) {
		if (this != &other) {
			this->m_pman = other.m_pman;
			this->m_oset = other.m_oset;
			this->m_ids = other.m_ids;
			this->m_vars = other.m_vars;
		}
		return (*this);
	}
	DBIndivProvider::~DBIndivProvider() {
	}
	void DBIndivProvider::initialize(void) {
		DLIB_ASSERT(this->m_oset.id() != 0, "DBDataset is not valid");
		StatDBManager *pMan = this->m_pman;
		DLIB_ASSERT(pMan != nullptr, "DBManager is null");
		DLIB_ASSERT(pMan->is_valid(), "DBManager is not valid");
		this->m_ids.clear();
		bool bRet = pMan->get_dataset_indivs_ids(this->m_oset, this->m_ids);
		DLIB_ASSERT(bRet, "Cannot retrieve indivs ids");
		this->m_vars.clear();
		variables_vector oVars;
		bRet = pMan->get_dataset_variables(this->m_oset, oVars);
		DLIB_ASSERT(bRet, "Cannot get dataset variables");
		variables_map &oMap = this->m_vars;
		oMap.clear();
		std::for_each(oVars.begin(), oVars.end(), [&oMap](const DBStatVariable &v) {
			IntType key = v.id();
			DBStatVariable vv(v);
			oMap[key] = vv;
		});
	}// initialize
	void DBIndivProvider::initialize(const std::string &setName) {
		StatDBManager *pMan = this->m_pman;
		DLIB_ASSERT(pMan != nullptr, "DBManager is null");
		DLIB_ASSERT(pMan->is_valid(), "DBManager is not valid");
		this->m_oset.set_sigle(setName);
		bool bRet = pMan->find_dataset(this->m_oset);
		DLIB_ASSERT(bRet, "DBDataset not found");
		this->initialize();
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
		DLIB_ASSERT(this->is_valid(), "this DBIndivProvider is not valid");
		if (this->is_valid()) {
			nRows = (this->m_ids).size();
			return (true);
		}
		return (false);
	}
	bool DBIndivProvider::all_indivs_ids(ints_vector &oIds) {
		DLIB_ASSERT(this->is_valid(), "this DBIndivProvider is not valid");
		oIds = this->m_ids;
		return (true);
	}
	bool DBIndivProvider::contains_id(const IntType aIndex) {
		DLIB_ASSERT(this->is_valid(), "this DBIndivProvider is not valid");
		bool bFound = false;
		ints_vector &vv = this->m_ids;
		const size_t n = vv.size();
		for (size_t i = 0; i < n; ++i) {
			if (vv[i] == aIndex) {
				bFound = true;
				break;
			}
		}
		return (bFound);
	}//contains_ids

	bool DBIndivProvider::find_indiv(const IntType aIndex, Indiv &oInd, const VariableMode mode /*= VariableMode::modeAll*/) {
		DLIB_ASSERT(this->is_valid(), "this DBIndivProvider is not valid");
		DLIB_ASSERT(aIndex != 0, "index cannot be zero");
		bool bRet = false;
		if (this->is_valid()) {
			StatDBManager *pMan = this->m_pman;
			DLIB_ASSERT(pMan != nullptr, "StatDataManager cannot be null");
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
			variables_map &vars = this->m_vars;
			std::for_each(vv.begin(), vv.end(), [mode, vars, &oMap](const DBStatValue &v) {
				DbValue vx = v.get_value();
				if (!vx.empty()) {
					IntType key = v.get_variable_id();
					auto jt = vars.find(key);
					if (jt != vars.end()) {
						const DBStatVariable &vz = (*jt).second;
						switch (mode) {
						case VariableMode::modeNumeric:
							if (vz.is_numeric()) {
								oMap[key] = vx;
							}
							break;
						case VariableMode::modeNominal:
							if (vz.is_categ()) {
								oMap[key] = vx;
							}
							break;
						case VariableMode::modeAll:
							oMap[key] = vx;
							break;
						default:
							break;
						}// mode
					}// found
				}// not empty
			});
			oInd.set_data(oMap);
			return (true);
		}// vamid
		return (bRet);
	}//bool
	bool DBIndivProvider::find_indiv_at(const size_t pos, Indiv &oInd, const VariableMode mode /*= VariableMode::modeAll*/) {
		DLIB_ASSERT(this->is_valid(), "this DBIndivProvider is not valid");
		bool bRet = false;
		if (this->is_valid()) {
			StatDBManager *pMan = this->m_pman;
			DLIB_ASSERT(pMan != nullptr, "StatDataManager cannot be null");
			ints_vector &v = this->m_ids;
			if (pos >= v.size()) {
				return (false);
			}
			IntType aIndex = v[pos];
			return this->find_indiv(aIndex, oInd, mode);
		}// vamid
		return (bRet);
	}// indiv_at
	bool DBIndivProvider::distance(const IntType aIndex1, const IntType &aIndex2,
		double &dRes, const VariableMode mode /*= VariableMode::modeAll*/) {
		DLIB_ASSERT(this->is_valid(), "this DBIndivProvider is not valid");
		Indiv oInd1, oInd2;
		dRes = 0;
		if (!this->find_indiv(aIndex1, oInd1, mode)) {
			return (false);
		}
		if (!this->find_indiv(aIndex2, oInd2, mode)) {
			return (false);
		}
		dRes = oInd1.distance(oInd2);
		return (true);
	}

	bool DBIndivProvider::distance_at(const size_t pos1, const size_t pos2,
		double &dRes, const VariableMode mode /*= VariableMode::modeAll*/) {
		DLIB_ASSERT(this->is_valid(), "this DBIndivProvider is not valid");
		Indiv oInd1, oInd2;
		dRes = 0;
		if (!this->find_indiv_at(pos1, oInd1, mode)) {
			return (false);
		}
		if (!this->find_indiv_at(pos2, oInd2, mode)) {
			return (false);
		}
		dRes = oInd1.distance(oInd2);
		return (true);
	}


	/////////////////////////////////////////
}// namespace info
/////////////////////////////////////
