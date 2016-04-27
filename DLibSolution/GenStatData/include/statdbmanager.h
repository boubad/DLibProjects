#pragma once
#ifndef __STATDBMANAGER_H__
#define __STATDBMANAGER_H__
///////////////////////////////////
#include "dbstatitems.h"
//////////////////////////////////
#include <memory>
#include <vector>
/////////////////////////////////////
namespace info {
	class Database;
	//////////////////////////////////////////
	class StatDBManager {
	public:
		typedef std::vector<DBStatDataset> datasets_vector;
		typedef std::vector<DBStatValue> values_vector;
		typedef std::vector<DBStatIndiv> indivs_vector;
		typedef std::vector<DBStatVariable> variables_vector;
		typedef std::vector<IntType> ints_vector;
	private:
		bool m_intransaction;
		std::unique_ptr<Database> m_database;
	private:
		bool check_schema(void);
	public:
		StatDBManager();
		StatDBManager(const std::wstring &s);
		StatDBManager(const std::string &s);
		~StatDBManager();
	public:
		bool open(const std::wstring &filename);
		bool open(const std::string &filename);
		bool is_valid(void) const;
		bool close(void);
		//
		bool get_all_datasets(datasets_vector &oVec);
		bool find_dataset(DBStatDataset &cur);
		bool remove_dataset(const DBStatDataset &cur);
		bool update_dataset(const DBStatDataset &cur);
		bool insert_dataset(const DBStatDataset &cur);
		//
		bool get_dataset_variables(const DBStatDataset &oSet, variables_vector &oVec);
		bool find_variable(DBStatVariable &cur);
		bool check_variable(DBStatVariable &cur);
		bool maintains_variables(const variables_vector &oVec);
		bool remove_variables(const variables_vector &oVec);
		//
		bool get_dataset_indivs_ids(const DBStatDataset &oSet, ints_vector &oVec);
		bool find_indiv(DBStatIndiv &cur);
		bool get_dataset_indivs(const DBStatDataset &oSet, indivs_vector &oVec);
		bool maintains_indivs(const indivs_vector &oVec);
		bool remove_indivs(const indivs_vector &oVec);
		//
		bool maintains_values(const values_vector &oVec);
		bool remove_values(const values_vector &oVec);
		bool find_value(DBStatValue &cur);
		bool get_dataset_values_count(const  DBStatDataset &oSet, int &nCount);
		bool get_dataset_values(const  DBStatDataset &oSet, values_vector &oVec, int skip, int taken);
		//
		bool get_indiv_values(const DBStatIndiv &oInd, values_vector &oVec);
		bool get_variable_values(const DBStatVariable &oVar, values_vector &oVec);
		//
		template <typename X, class ALLOCX, class TSTRING, class ALLOCTSTRING>
		bool import_dataset(const TSTRING &name, const size_t nRows, const size_t nCols,
			const std::vector<X, ALLOCX> &gdata,
			const std::vector<TSTRING, ALLOCTSTRING> &indNames,
			const std::vector<TSTRING, ALLOCTSTRING> &varNames,
			const TSTRING &type, const TSTRING &genre, const TSTRING &status) {
			assert(this->is_valid());
			DBStatDataset oSet;
			oSet.set_sigle(name);
			bool bRet = this->find_dataset(oSet);
			if (!bRet) {
				oSet.set_name(name);
				oSet.set_desc(name);
				if (!this->insert_dataset(oSet)) {
					return (false);
				}
				if (!this->find_dataset(oSet)) {
					return (false);
				}
			}
			int nDatasetId = oSet.id();
			assert(nDatasetId != 0);
			variables_vector oVars;
			std::for_each(varNames.begin(), varNames.end(), [&](const TSTRING &sigle) {
				DBStatVariable oVar(oSet);
				oVar.set_sigle(sigle);
				bRet = this->find_variable(oVar);
				if (!bRet) {
					oVar.set_name(sigle);
					oVar.set_desc(sigle);
					oVar.set_vartype(type);
					oVar.set_genre(genre);
					oVars.push_back(oVar);
				}
			});
			if (!oVars.empty()) {
				if (!this->maintains_variables(oVars)) {
					return (false);
				}
			}//oVars
			indivs_vector oInds;
			std::for_each(indNames.begin(), indNames.end(), [&](const TSTRING&sigle) {
				DBStatIndiv oInd(oSet);
				oInd.set_sigle(sigle);
				bRet = this->find_indiv(oInd);
				if (!bRet) {
					oInd.set_name(sigle);
					oInd.set_desc(sigle);
					oInd.set_status(status);
					oInds.push_back(oInd);
				}// bRet
			});
			if (!oInds.empty()) {
				if (!this->maintains_indivs(oInds)) {
					return (false);
				}
			}
			if (!this->get_dataset_indivs(oSet, oInds)) {
				return (false);
			}
			if (!this->get_dataset_variables(oSet, oVars)) {
				return (false);
			}
			values_vector oVals;
			for (size_t icol = 0; icol < nCols; ++icol) {
				TSTRING varSigle = varNames[icol];
				for (auto it = oVars.begin(); it != oVars.end(); ++it) {
					const DBStatVariable &oVar = *it;
					TSTRING sxvar;
					oVar.get_sigle(sxvar);
					if (sxvar == varSigle) {
						for (size_t irow = 0; irow < nRows; ++irow) {
							TSTRING indSigle = indNames[irow];
							for (auto jt = oInds.begin(); jt != oInds.end(); ++jt) {
								const DBStatIndiv &oInd = *jt;
								TSTRING sxind;
								oInd.get_sigle(sxind);
								if (sxind == indSigle) {
									X val = gdata[irow * nCols + icol];
									DBStatValue oVal(oVar, oInd);
									DbValue vz(val);
									oVal.set_value(vz);
									oVals.push_back(oVal);
								}// found indiv
							}// jt
						}// irow
					}// found var
				} // it
			}// icol
			if (!oVals.empty()) {
				if (!this->maintains_values(oVals)) {
					return (false);
				}
			}// oVals
			return (true);
		}// import_dataset
	protected:
		bool begin_transaction(void);
		bool commit_transaction(void);
		bool rollback_transaction(void);
	};// class StatDBManager
	/////////////////////////////////////////////
}// namespace info_sqlite
//////////////////////////////////////
#endif // !__STATDBMANAGER_H__
