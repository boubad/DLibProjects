//statdbmanager.cpp
#include "../include/statdbmanager.h"
/////////////////////////////////////
#include "../include/database.h"
#include "../include/statement.h"
#include "../include/dbvalue.h"
#include "../include/stringconvert.h"
///////////////////////////////////
#include <boost/algorithm/string.hpp>
/////////////////////////////////////
namespace info {
	//////////////////////////////////////////
	static const char *SQL_COMMIT_TRANSACTION = "COMMIT TRANSACTION";
	static const char *SQL_BEGIN_TRANSACTION = "BEGIN TRANSACTION";
	static const char *SQL_ROLLBACK_TRANSACTION = "ROLLBACK TRANSACTION";
	////////////////////////////////////
	static const char *SQL_CREATE_DATASET =
		"CREATE TABLE IF NOT EXISTS dbdataset("
		" datasetid INTEGER PRIMARY KEY AUTOINCREMENT,"
		" optlock INTEGER NOT NULL DEFAULT 1,"
		" sigle TEXT NOT NULL UNIQUE,"
		" nom TEXT DEFAULT NULL,"
		" description TEXT DEFAULT NULL,"
		" status TEXT DEFAULT NULL"
		" )";
	//////////////////////////////////////
	static const char *SQL_FIND_ALL_DATASETS =
		"SELECT datasetid,optlock,sigle,nom,description,status"
		" FROM dbdataset ORDER BY sigle";
	static const char *SQL_FIND_DATASET_BY_ID =
		"SELECT datasetid,optlock,sigle,nom,description,status"
		" FROM dbdataset WHERE datasetid = ?1";
	static const char *SQL_FIND_DATASET_BY_SIGLE =
		"SELECT datasetid,optlock,sigle,nom,description,status"
		" FROM dbdataset WHERE UPPER(LTRIM(RTRIM(sigle))) = ?1";
	static const char *SQL_INSERT_DATASET =
		"INSERT INTO dbdataset (sigle,nom,description,status)"
		" VALUES (?1,?2,?3,?4)";
	static const char *SQL_UPDATE_DATASET =
		"UPDATE dbdataset SET optlock = optlock + 1,"
		" sigle = ?1, nom = ?2, description = ?3, status = ?4 WHERE datasetid = ?5";
	static const char *SQL_REMOVE_DATASET =
		"DELETE FROM dbdataset WHERE datasetid = ?1";
	///////////////////////////////////////
	static const char *SQL_CREATE_VARIABLE =
		"CREATE TABLE IF NOT EXISTS dbvariable("
		" variableid INTEGER PRIMARY KEY AUTOINCREMENT,"
		" optlock INTEGER NOT NULL DEFAULT 1,"
		" datasetid INTEGER NOT NULL,"
		" sigle TEXT NOT NULL,"
		" vartype TEXT NOT NULL,"
		" categvar INTEGER NOT NULL DEFAULT 0,"
		" nom TEXT DEFAULT NULL,"
		" description TEXT DEFAULT NULL,"
		" genre TEXT DEFAULT NULL,"
		" status TEXT DEFAULT NULL,"
		" CONSTRAINT uc_variable UNIQUE (datasetid, sigle),"
		" CONSTRAINT fk_variable_dataset FOREIGN KEY (datasetid) REFERENCES dbdataset (datasetid) ON DELETE CASCADE"
		" )";
	/////////////////////////////////////////////////////////
	static const char *SQL_FIND_DATASET_VARIABLES =
		"SELECT variableid, optlock, datasetid , sigle, vartype, categvar, nom, description, genre, status"
		" FROM dbvariable WHERE datasetid = ?1 ORDER BY categvar DESC, sigle ASC";
	static const char *SQL_VARIABLE_BY_ID =
		"SELECT variableid,optlock,datasetid,sigle,vartype,categvar,nom,description,genre,status"
		" FROM dbvariable WHERE variableid = ?1";
	static const char *SQL_VARIABLE_BY_DATASET_AND_SIGLE =
		"SELECT variableid,optlock,datasetid,sigle,vartype,categvar,nom,description,genre,status"
		" FROM dbvariable WHERE datasetid = ?1 AND UPPER(LTRIM(RTRIM(sigle))) = ?2";
	static const char *SQL_INSERT_VARIABLE =
		"INSERT INTO dbvariable (datasetid,sigle,vartype,categvar,nom,description,genre,status)"
		" VALUES (?1, ?2,?3,?4,?5,?6,?7,?8)";
	static const char *SQL_UPDATE_VARIABLE =
		"UPDATE dbvariable SET optlock = optlock + 1,"
		" sigle = ?1, vartype = ?2, categvar = ?3, nom = ?4, description = ?5, genre = ?6, status = ?7 WHERE variableid = ?8";
	static const char *SQL_REMOVE_VARIABLE =
		"DELETE FROM dbvariable WHERE variableid = ?1";
	////////////////////////////////////////////////////////
	static const char *SQL_CREATE_INDIV =
		"CREATE TABLE IF NOT EXISTS dbindiv("
		" individ INTEGER PRIMARY KEY AUTOINCREMENT,"
		" optlock INTEGER NOT NULL DEFAULT 1,"
		" datasetid INTEGER NOT NULL,"
		" sigle TEXT NOT NULL,"
		" nom TEXT DEFAULT NULL,"
		" description TEXT DEFAULT NULL,"
		" status TEXT DEFAULT NULL,"
		" CONSTRAINT uc_indiv UNIQUE (datasetid, sigle),"
		" CONSTRAINT fk_indiv_dataset FOREIGN KEY (datasetid) REFERENCES dbdataset (datasetid) ON DELETE CASCADE"
		" )";
	////////////////////////////////////////////////
	static const char *SQL_FIND_DATASET_INDIVS_COUNT =
		"SELECT COUNT(*) FROM dbindiv"
		" WHERE datasetid = ?1";
	static const char *SQL_FIND_DATASET_INDIVS =
		"SELECT individ,optlock,datasetid,sigle,nom,description,status"
		" FROM dbindiv WHERE datasetid = ?1 ORDER BY sigle";
	static const char *SQL_INDIV_BY_ID =
		"SELECT individ,optlock,datasetid,sigle,nom,description,status"
		" FROM dbindiv WHERE individ = ?1";
	static const char *SQL_INDIV_BY_DATASET_AND_SIGLE =
		"SELECT individ,optlock,datasetid,sigle,nom,description,status"
		" FROM dbindiv WHERE datasetid = ?1 AND UPPER(LTRIM(RTRIM(sigle))) = ?2";
	static const char *SQL_INSERT_INDIV =
		"INSERT INTO dbindiv (datasetid,sigle,nom,description,status)"
		" VALUES(?1,?2,?3,?4, ?5)";
	static const char *SQL_UPDATE_INDIV =
		"UPDATE dbindiv SET optlock = OPTLOCK + 1,"
		" sigle = ?1, nom = ?2, description = ?3, status = ?4 WHERE individ = ?5";
	static const char *SQL_REMOVE_INDIV =
		"REMOVE FROM dbindiv WHERE individ = ?1";
	static const char *SQL_GET_DATASET_INDIV_IDS =
		"SELECT individ FROM dbindiv"
		" WHERE datasetid = ?1 ORDER BY individ ASC";
	////////////////////////////////////////////////
	static const char *SQL_CREATE_VALUE =
		"CREATE TABLE IF NOT EXISTS dbvalue("
		" valueid INTEGER PRIMARY KEY AUTOINCREMENT,"
		" optlock INTEGER NOT NULL DEFAULT 1,"
		" variableid INTEGER NOT NULL,"
		" individ INTEGER NOT NULL,"
		" stringval TEXT NULL,"
		" status TEXT DEFAULT NULL,"
		" CONSTRAINT uc_value UNIQUE (variableid, individ),"
		" CONSTRAINT fk_value_variable FOREIGN KEY (variableid) REFERENCES dbvariable (variableid) ON DELETE CASCADE,"
		" CONSTRAINT fk_value_indiv FOREIGN KEY (individ) REFERENCES dbindiv (individ) ON DELETE CASCADE"
		" )";
	/////////////////////////////////////////////////////
	static const char *SQL_FIND_VARIABLE_TYPE =
		"SELECT vartype FROM dbvariable WHERE variableid = ?1";
	static const char *SQL_FIND_DATASET_VALUES_COUNT = "SELECT COUNT(*)"
		" FROM dbvalue a, dbvariable b"
		" WHERE a.variableid = b.variableid AND b.datasetid = ?1";
	static const char *SQL_FIND_DATASET_VALUES =
		"SELECT a.valueid,a.optlock,a.variableid,a.individ,a.stringval,a.status"
		" FROM dbvalue a, dbvariable b"
		" WHERE a.variableid = b.variableid AND b.datasetid = ?1"
		" ORDER BY a.variableid ASC, a.individ ASC"
		" LIMIT ?2 OFFSET ?3";
	static const char *SQL_VALUES_BY_VARIABLE_INDIV =
		"SELECT valueid,optlock,variableid,individ,stringval,status"
		" FROM dbvalue WHERE variableid = ?1 AND individ = ?2";
	static const char *SQL_VALUE_BY_ID =
		"SELECT valueid,optlock,variableid,individ,stringval,status"
		" FROM dbvalue WHERE valueid = ?1";
	static const char *SQL_INSERT_VALUE =
		"INSERT INTO dbvalue (variableid,individ,stringval,status)"
		" VALUES(?1,?2,?3,?4)";
	static const char *SQL_UPDATE_VALUE =
		"UPDATE dbvalue SET optlock = optlock + 1,"
		" stringval = ?1, status = ?2 WHERE valueid = ?3 ";
	static const char *SQL_REMOVE_VALUE =
		"DELETE from dbvalue WHERE valueid = ?1";
	static const char *SQL_VALUES_BY_VARIABLEID =
		"SELECT valueid,optlock,variableid,individ,stringval,status"
		" FROM dbvalue WHERE variableid = ?1";
	static const char *SQL_VALUES_BY_INDIVID =
		"SELECT valueid,optlock,variableid,individ,stringval,status"
		" FROM dbvalue WHERE individ = ?1";
	//////////////////////////////////////////////////////
	static const char *CREATE_SQL[] = {
		SQL_CREATE_DATASET,
		SQL_CREATE_VARIABLE,
		SQL_CREATE_INDIV,
		SQL_CREATE_VALUE,
		nullptr };
	/////////////////////////////////////////////
	static void read_value(Statement &stmt, DBStatValue &cur) {
		{
			DbValue v;
			if (stmt.col_value(0, v)) {
				int nId = v.int_value();
				cur.id(nId);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(1, v)) {
				int nVersion = v.int_value();
				cur.version(nVersion);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(2, v)) {
				int nx = v.int_value();
				cur.set_variable_id(nx);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(3, v)) {
				int nx = v.int_value();
				cur.set_indiv_id(nx);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(4, v)) {
				cur.set_value(v);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(5, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_status(s);
				}
			}
		}
	}// read_value
	static void read_dataset(Statement &stmt, DBStatDataset &cur) {
		assert(stmt.cols() >= 5);
		{
			DbValue v;
			if (stmt.col_value(0, v)) {
				int nId = v.int_value();
				cur.id(nId);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(1, v)) {
				int nVersion = v.int_value();
				cur.version(nVersion);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(2, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_sigle(s);
				}
			}
		}
		{
			DbValue v;
			if (stmt.col_value(3, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_name(s);
				}
			}
		}
		{
			DbValue v;
			if (stmt.col_value(4, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_desc(s);
				}
			}
		}
		{
			DbValue v;
			if (stmt.col_value(5, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_status(s);
				}
			}
		}
	}//read_dataset
	static bool find_dataset_by_sigle(Database *pBase, const std::string &xSigle, DBStatDataset &cur) {
		Statement stmt(pBase, SQL_FIND_DATASET_BY_SIGLE);
		if (!stmt.is_valid()) {
			return (false);
		}
		std::string sx = boost::to_upper_copy(boost::trim_copy(xSigle));
		if (!stmt.set_parameter(1, sx.c_str())) {
			return (false);
		}
		if (!stmt.exec()) {
			return (false);
		}
		if (!stmt.has_values()) {
			return (false);
		}
		read_dataset(stmt, cur);
		return (true);
	} // find_dataset_by_sigle
	static bool find_dataset_by_id(Database *pBase, int xId, DBStatDataset &cur) {
		Statement stmt(pBase, SQL_FIND_DATASET_BY_ID);
		if (!stmt.is_valid()) {
			return (false);
		}
		if (!stmt.set_parameter(1, xId)) {
			return (false);
		}
		if (!stmt.exec()) {
			return (false);
		}
		if (!stmt.has_values()) {
			return (false);
		}
		read_dataset(stmt, cur);
		return (true);
	}//find_dataset_by_id
	static void read_indiv(Statement &stmt, DBStatIndiv &cur) {
		{
			DbValue v;
			if (stmt.col_value(0, v)) {
				int nId = v.int_value();
				cur.id(nId);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(1, v)) {
				int nVersion = v.int_value();
				cur.version(nVersion);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(2, v)) {
				int nx = v.int_value();
				cur.set_dataset_id(nx);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(3, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_sigle(s);
				}
			}
		}
		{
			DbValue v;
			if (stmt.col_value(4, v)) {
				std::string s; s;
				if (v.string_value(s)) {
					cur.set_name(s);
				}
			}
		}
		{
			DbValue v;
			if (stmt.col_value(5, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_desc(s);
				}
			}
		}
		{
			DbValue v;
			if (stmt.col_value(6, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_status(s);
				}
			}
		}
	}// read_indiv
	static bool find_indiv_by_id(Database *pBase, int xId, DBStatIndiv &cur) {
		assert(pBase != nullptr);
		assert(xId != 0);
		Statement stmt(pBase, SQL_INDIV_BY_ID);
		if (!stmt.is_valid()) {
			return (false);
		}
		if (!stmt.set_parameter(1, xId)) {
			return (false);
		}
		if (!stmt.exec()) {
			return (false);
		}
		if (!stmt.has_values()) {
			return (false);
		}
		read_indiv(stmt, cur);
		return (true);
	}//find_indiv_by_id
	static bool find_indiv_by_dataset_sigle(Database *pBase, int nDatasetId, const std::string &xSigle,
		DBStatIndiv &cur) {
		assert(pBase != nullptr);
		assert(nDatasetId != 0);
		assert(!xSigle.empty());
		Statement stmt(pBase, SQL_INDIV_BY_DATASET_AND_SIGLE);
		if (!stmt.is_valid()) {
			return (false);
		}
		std::string sigle = boost::to_upper_copy(boost::trim_copy(xSigle));
		stmt.set_parameter(1, nDatasetId);
		stmt.set_parameter(2, sigle);
		if (!stmt.exec()) {
			return (false);
		}
		if (!stmt.has_values()) {
			return (false);
		}
		read_indiv(stmt, cur);
		return (true);
	}//find_indiv_by_dataset_sigle
	static void read_variable(Statement &stmt, DBStatVariable &cur) {
		{
			DbValue v;
			if (stmt.col_value(0, v)) {
				int nId = v.int_value();
				cur.id(nId);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(1, v)) {
				int nVersion = v.int_value();
				cur.version(nVersion);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(2, v)) {
				int nx = v.int_value();
				cur.set_dataset_id(nx);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(3, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_sigle(s);
				}
			}
		}
		{
			DbValue v;
			if (stmt.col_value(4, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_vartype(s);
				}
			}
		}
		{
			DbValue v;
			if (stmt.col_value(5, v)) {
				int nx = v.int_value();
				bool b = (nx != 0) ? true : false;
				cur.is_categ(b);
			}
		}
		{
			DbValue v;
			if (stmt.col_value(6, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_name(s);
				}
			}
		}
		{
			DbValue v;
			if (stmt.col_value(7, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_desc(s);
				}
			}
		}
		{
			DbValue v;
			if (stmt.col_value(8, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_genre(s);
				}
			}
		}
		{
			DbValue v;
			if (stmt.col_value(9, v)) {
				std::string s;
				if (v.string_value(s)) {
					cur.set_status(s);
				}
			}
		}
	}// read_variable
	static bool find_variable_by_id(Database *pBase, int xId, DBStatVariable &cur) {
		assert(pBase != nullptr);
		assert(xId != 0);
		Statement stmt(pBase, SQL_VARIABLE_BY_ID);
		if (!stmt.is_valid()) {
			return (false);
		}
		if (!stmt.set_parameter(1, xId)) {
			return (false);
		}
		if (!stmt.exec()) {
			return (false);
		}
		if (!stmt.has_values()) {
			return (false);
		}
		read_variable(stmt, cur);
		return (true);
	}//find_variable_by_id
	static bool find_variable_by_dataset_sigle(Database *pBase, int nDatasetId, const std::string &xSigle,
		DBStatVariable &cur) {
		assert(pBase != nullptr);
		assert(nDatasetId != 0);
		assert(!xSigle.empty());
		Statement stmt(pBase, SQL_VARIABLE_BY_DATASET_AND_SIGLE);
		if (!stmt.is_valid()) {
			return (false);
		}
		std::string sigle = boost::to_upper_copy(boost::trim_copy(xSigle));
		stmt.set_parameter(1, nDatasetId);
		stmt.set_parameter(2, sigle);
		if (!stmt.exec()) {
			return (false);
		}
		if (!stmt.has_values()) {
			return (false);
		}
		read_variable(stmt, cur);
		return (true);
	}//find_variable_by_dataset_sigle
	static bool find_value_by_id(Database *pBase, int xId, DBStatValue &cur) {
		assert(pBase != nullptr);
		assert(xId != 0);
		Statement stmt(pBase, SQL_VALUE_BY_ID);
		if (!stmt.is_valid()) {
			return (false);
		}
		if (!stmt.set_parameter(1, xId)) {
			return (false);
		}
		if (!stmt.exec()) {
			return (false);
		}
		if (!stmt.has_values()) {
			return (false);
		}
		read_value(stmt, cur);
		return (true);
	}//find_value_by_id
	static bool find_value_by_variable_indiv(Database *pBase, int nVarId, int nIndId, DBStatValue &cur) {
		assert(pBase != nullptr);
		assert(nVarId != 0);
		assert(nIndId != 0);
		//
		Statement stmt(pBase, SQL_VALUES_BY_VARIABLE_INDIV);
		if (!stmt.is_valid()) {
			return (false);
		}
		if (!stmt.set_parameter(1, nVarId)) {
			return (false);
		}
		if (!stmt.set_parameter(2, nIndId)) {
			return (false);
		}
		if (!stmt.exec()) {
			return (false);
		}
		if (!stmt.has_values()) {
			return (false);
		}
		read_value(stmt, cur);
		return (true);
	}//find_value_by_id
	/////////////////////////////////////////
	//
	///////////////////////////////////////////
	bool StatDBManager::get_indiv_values(const DBStatIndiv &oInd, values_vector &oVec) {
		assert(this->is_valid());
		oVec.clear();
		int nId = oInd.id();
		if (nId == 0) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmt(pBase, SQL_VALUES_BY_INDIVID);
		if (!stmt.is_valid()) {
			return (false);
		}
		stmt.set_parameter(1, nId);
		if (!stmt.exec()) {
			return (false);
		}
		while (stmt.has_values()) {
			DBStatValue cur;
			read_value(stmt, cur);
			oVec.push_back(cur);
			if (!stmt.next()) {
				break;
			}
		} // values
		return (true);
	}//get_indiv_values
	bool StatDBManager::get_variable_values(const DBStatVariable &oVar, values_vector &oVec) {
		assert(this->is_valid());
		oVec.clear();
		int nId = oVar.id();
		if (nId == 0) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmt(pBase, SQL_VALUES_BY_VARIABLEID);
		if (!stmt.is_valid()) {
			return (false);
		}
		stmt.set_parameter(1, nId);
		if (!stmt.exec()) {
			return (false);
		}
		while (stmt.has_values()) {
			DBStatValue cur;
			read_value(stmt, cur);
			oVec.push_back(cur);
			if (!stmt.next()) {
				break;
			}
		} // values
		return (true);
	}//get_variable_values
	bool StatDBManager::get_dataset_values(const  DBStatDataset &oSet, values_vector &oVec, int skip, int taken) {
		assert(this->is_valid());
		oVec.clear();
		int nDatasetId = oSet.id();
		if (nDatasetId == 0) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmt(pBase, SQL_FIND_DATASET_VALUES);
		if (!stmt.is_valid()) {
			return (false);
		}
		stmt.set_parameter(1, nDatasetId);
		stmt.set_parameter(2, taken);
		stmt.set_parameter(3, skip);
		if (!stmt.exec()) {
			return (false);
		}
		while (stmt.has_values()) {
			DBStatValue cur;
			read_value(stmt, cur);
			oVec.push_back(cur);
			if (!stmt.next()) {
				break;
			}
		} // values
		return (true);
	}//get_dataset_values
	bool StatDBManager::get_dataset_values_count(const  DBStatDataset &oSet, int &nCount) {
		assert(this->is_valid());
		nCount = 0;
		int nId = oSet.id();
		if (nId == 0) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmt(pBase, SQL_FIND_DATASET_VALUES_COUNT);
		if (!stmt.is_valid()) {
			return false;
		}
		stmt.set_parameter(1, nId);
		if (!stmt.exec()) {
			return (false);
		}
		if (!stmt.has_values()) {
			return (false);
		}
		DbValue v;
		if (stmt.col_value(0, v)) {
			nCount = v.int_value();
		}
		return (true);
	}//get_dataset_values_count
	bool StatDBManager::remove_values(const values_vector &oVec) {
		assert(this->is_valid());
		if (!this->begin_transaction()) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmt(pBase, SQL_REMOVE_VALUE);
		if (!stmt.is_valid()) {
			this->rollback_transaction();
			return (false);
		}
		for (auto it = oVec.begin(); it != oVec.end(); ++it) {
			const DBStatValue &cur = *it;
			int nId = cur.id();
			if (nId != 0) {
				stmt.reset();
				stmt.set_parameter(1, nId);
				if (!stmt.exec()) {
					this->rollback_transaction();
					return (false);
				}
			}
		} // it
		if (!this->commit_transaction()) {
			this->rollback_transaction();
			return (false);
		}
		return (true);
	}//remove_values
	bool  StatDBManager::maintains_values(const values_vector &oVec) {
		assert(this->is_valid());
		if (!this->begin_transaction()) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmtFetch(pBase, SQL_VALUES_BY_VARIABLE_INDIV);
		Statement stmtInsert(pBase, SQL_INSERT_VALUE);
		Statement stmtUpdate(pBase, SQL_UPDATE_VALUE);
		Statement stmtFirst(pBase, SQL_VALUE_BY_ID);
		Statement stmtRemove(pBase, SQL_REMOVE_VALUE);
		if ((!stmtFetch.is_valid())
			|| (!stmtUpdate.is_valid())
			|| (!stmtFirst.is_valid())
			|| (!stmtRemove.is_valid())
			|| (!stmtInsert.is_valid())
			) {
			this->rollback_transaction();
			return (false);
		}
		for (auto it = oVec.begin(); it != oVec.end(); ++it) {
			const DBStatValue &cur = *it;
			const DbValue &vx = cur.get_value();
			int nId = cur.id();
			std::string sval;
			std::string status;
			cur.get_status(status);
			vx.string_value(sval);
			if (nId != 0) {
				stmtFirst.reset();
				stmtFirst.set_parameter(1, nId);
				if (!stmtFirst.exec()) {
					this->rollback_transaction();
					return (false);
				}
				if (stmtFirst.has_values()) {
					if (sval.empty()) {
						stmtRemove.reset();
						stmtRemove.set_parameter(1, nId);
						if (!stmtRemove.exec()) {
							this->rollback_transaction();
							return (false);
						}
					}
					else {
						stmtUpdate.reset();
						stmtUpdate.set_parameter(1, sval);
						stmtUpdate.set_parameter(2, nId);
						if (!stmtUpdate.exec()) {
							this->rollback_transaction();
							return (false);
						}
					}
					continue;
				}
			} // nId
			int nVarId = cur.get_variable_id();
			int nIndivId = cur.get_indiv_id();
			if ((nVarId == 0) || (nIndivId == 0)) {
				continue;
			}
			stmtFetch.reset();
			stmtFetch.set_parameter(1, nVarId);
			stmtFetch.set_parameter(2, nIndivId);
			if (!stmtFetch.exec()) {
				return (false);
			}
			if (stmtFetch.has_values()) {
				nId = 0;
				DbValue v;
				if (stmtFetch.col_value(0, v)) {
					nId = v.int_value();
				}
				if (nId != 0) {
					if (sval.empty()) {
						stmtRemove.reset();
						stmtRemove.set_parameter(1, nId);
						if (!stmtRemove.exec()) {
							this->rollback_transaction();
							return (false);
						}
					}
					else {
						stmtUpdate.reset();
						stmtUpdate.set_parameter(1, sval);
						stmtUpdate.set_parameter(2, status);
						stmtUpdate.set_parameter(3, nId);
						if (!stmtUpdate.exec()) {
							this->rollback_transaction();
							return (false);
						}
					}
				} // nId
				continue;
			}
			else if (!sval.empty()) {
				stmtInsert.reset();
				stmtInsert.set_parameter(1, nVarId);
				stmtInsert.set_parameter(2, nIndivId);
				stmtInsert.set_parameter(3, sval);
				stmtInsert.set_parameter(4, status);
				if (!stmtInsert.exec()) {
					this->rollback_transaction();
					return (false);
				}
			}
		} // it
		if (!this->commit_transaction()) {
			this->rollback_transaction();
			return (false);
		}
		return (true);
	}// maintains_values
	bool  StatDBManager::maintains_indivs(const indivs_vector &oVec) {
		assert(this->is_valid());
		if (!this->begin_transaction()) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmtInsert(pBase, SQL_INSERT_INDIV);
		Statement stmtUpdate(pBase, SQL_UPDATE_INDIV);
		if ((!stmtUpdate.is_valid()) || (!stmtInsert.is_valid())) {
			this->rollback_transaction();
			return (false);
		}
		for (auto it = oVec.begin(); it != oVec.end(); ++it) {
			const DBStatIndiv &cur = *it;
			int nId = cur.id();
			int nDatasetId = cur.get_dataset_id();
			std::string sigle, name, desc, status;
			cur.get_sigle(sigle);
			cur.get_name(name);
			cur.get_desc(desc);
			cur.get_status(status);
			if (cur.is_updateable()) {
				stmtUpdate.reset();
				stmtUpdate.set_parameter(1, sigle);
				stmtUpdate.set_parameter(2, name);
				stmtUpdate.set_parameter(3, desc);
				stmtUpdate.set_parameter(4, status);
				stmtUpdate.set_parameter(5, nId);
				if (!stmtUpdate.exec()) {
					this->rollback_transaction();
					return (false);
				}
			}
			else if (cur.is_writeable()) {
				stmtInsert.reset();
				stmtInsert.set_parameter(1, nDatasetId);
				stmtInsert.set_parameter(2, sigle);
				stmtInsert.set_parameter(3, name);
				stmtInsert.set_parameter(4, desc);
				stmtInsert.set_parameter(5, status);
				if (!stmtInsert.exec()) {
					this->rollback_transaction();
					return (false);
				}
			}
		} // it
		if (!this->commit_transaction()) {
			this->rollback_transaction();
			return (false);
		}
		return (true);
	}// remove_indivs
	bool  StatDBManager::remove_indivs(const indivs_vector &oVec) {
		assert(this->is_valid());
		if (!this->begin_transaction()) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmt(pBase, SQL_REMOVE_INDIV);
		if (!stmt.is_valid()) {
			this->rollback_transaction();
			return (false);
		}
		for (auto it = oVec.begin(); it != oVec.end(); ++it) {
			const DBStatIndiv &cur = *it;
			int nId = cur.id();
			if (nId != 0) {
				stmt.reset();
				stmt.set_parameter(1, nId);
				if (!stmt.exec()) {
					this->rollback_transaction();
					return (false);
				}
			}
		} // it
		if (!this->commit_transaction()) {
			this->rollback_transaction();
			return (false);
		}
		return (true);
	}//remove_indivs
	bool StatDBManager::get_dataset_indivs(const DBStatDataset &oSet, indivs_vector &oVec) {
		assert(this->is_valid());
		oVec.clear();
		int nDatasetId = oSet.id();
		if (nDatasetId == 0) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmt(pBase, SQL_FIND_DATASET_INDIVS);
		if (!stmt.is_valid()) {
			return (false);
		}
		stmt.set_parameter(1, nDatasetId);
		if (!stmt.exec()) {
			return (false);
		}
		while (stmt.has_values()) {
			DBStatIndiv cur;
			read_indiv(stmt, cur);
			oVec.push_back(cur);
			if (!stmt.next()) {
				break;
			}
		} // values
		return (true);
	}//get_dataset_indivs
	bool StatDBManager::get_dataset_indivs_ids(const DBStatDataset &oSet, ints_vector &oInds) {
		oInds.clear();
		int nDatasetId = oSet.id();
		if (nDatasetId == 0) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmt(pBase, SQL_GET_DATASET_INDIV_IDS);
		if (!stmt.is_valid()) {
			return false;
		}
		stmt.set_parameter(1, nDatasetId);
		if (!stmt.exec()) {
			return (false);
		}
		while (stmt.has_values()) {
			DbValue v;
			if (stmt.col_value(0, v)) {
				int nId = v.int_value();
				oInds.push_back(nId);
			}
			if (!stmt.next()) {
				break;
			}
		} // while
		return (true);
	}//get_dataset_indivs_ids
	bool StatDBManager::remove_variables(const variables_vector &oVec) {
		assert(this->is_valid());
		if (!this->begin_transaction()) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmt(pBase, SQL_REMOVE_VARIABLE);
		if (!stmt.is_valid()) {
			this->rollback_transaction();
			return (false);
		}
		for (auto it = oVec.begin(); it != oVec.end(); ++it) {
			const DBStatVariable &cur = *it;
			int nId = cur.id();
			if (nId != 0) {
				stmt.reset();
				stmt.set_parameter(1, nId);
				if (!stmt.exec()) {
					this->rollback_transaction();
					return (false);
				}
			}
		} // it
		if (!this->commit_transaction()) {
			this->rollback_transaction();
			return (false);
		}
		return (true);
	}//remove_variables
	bool StatDBManager::maintains_variables(const variables_vector &oVec) {
		assert(this->is_valid());
		if (!this->begin_transaction()) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		Statement stmtInsert(pBase, SQL_INSERT_VARIABLE);
		Statement stmtUpdate(pBase, SQL_UPDATE_VARIABLE);
		if ((!stmtUpdate.is_valid()) || (!stmtInsert.is_valid())) {
			this->rollback_transaction();
			return (false);
		}
		for (auto it = oVec.begin(); it != oVec.end(); ++it) {
			const DBStatVariable &cur = *it;
			int nId = cur.id();
			int nDatasetId = cur.get_dataset_id();
			std::string sigle, name, vartype, genre, desc, status;
			cur.get_sigle(sigle);
			int nz = (cur.is_categ()) ? 1 : 0;
			cur.get_name(name);
			cur.get_desc(desc);
			cur.get_vartype(vartype);
			cur.get_genre(genre);
			cur.get_status(status);
			if (cur.is_updateable()) {
				stmtUpdate.reset();
				stmtUpdate.set_parameter(1, sigle);
				stmtUpdate.set_parameter(2, vartype);
				stmtUpdate.set_parameter(3, nz);
				stmtUpdate.set_parameter(4, name);
				stmtUpdate.set_parameter(5, desc);
				stmtUpdate.set_parameter(6, genre);
				stmtUpdate.set_parameter(7, status);
				stmtUpdate.set_parameter(8, nId);
				if (!stmtUpdate.exec()) {
					this->rollback_transaction();
					return (false);
				}
			}
			else if (cur.is_writeable()) {
				stmtInsert.reset();
				stmtInsert.set_parameter(1, nDatasetId);
				stmtInsert.set_parameter(2, sigle);
				stmtInsert.set_parameter(3, vartype);
				stmtInsert.set_parameter(4, nz);
				stmtInsert.set_parameter(5, name);
				stmtInsert.set_parameter(6, desc);
				stmtInsert.set_parameter(7, genre);
				stmtInsert.set_parameter(8, status);
				if (!stmtInsert.exec()) {
					this->rollback_transaction();
					return (false);
				}
			}
		} // it
		if (!this->commit_transaction()) {
			this->rollback_transaction();
			return (false);
		}
		return (true);
	}//maintains_variables
	bool StatDBManager::get_dataset_variables(const DBStatDataset &oSet, variables_vector &oVec) {
		assert(this->is_valid());
		oVec.clear();
		int nDatasetId = oSet.id();
		if (nDatasetId == 0) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmt(pBase, SQL_FIND_DATASET_VARIABLES);
		if (!stmt.is_valid()) {
			return (false);
		}
		stmt.set_parameter(1, nDatasetId);
		if (!stmt.exec()) {
			return (false);
		}
		while (stmt.has_values()) {
			DBStatVariable cur;
			read_variable(stmt, cur);
			oVec.push_back(cur);
			if (!stmt.next()) {
				break;
			}
		} // values
		return (true);
	}//get_dataset_variables
	bool StatDBManager::get_all_datasets(datasets_vector &oVec) {
		assert(this->is_valid());
		oVec.clear();
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmt(pBase, SQL_FIND_ALL_DATASETS);
		if (!stmt.is_valid()) {
			return (false);
		}
		if (!stmt.exec()) {
			return (false);
		}
		while (stmt.has_values()) {
			DBStatDataset cur;
			read_dataset(stmt, cur);
			oVec.push_back(cur);
			if (!stmt.next()) {
				break;
			}
		} // values
		return (true);
	}//get_all_datasets
	bool StatDBManager::find_value(DBStatValue &cur) {
		assert(this->is_valid());
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		int nId = cur.id();
		if (nId != 0) {
			if (find_value_by_id(pBase, nId, cur)) {
				return (true);
			}
		}
		int nVarId = cur.get_variable_id();
		int nIndId = cur.get_indiv_id();
		if ((nVarId == 0) || (nIndId != 0)) {
			return (false);
		}
		return (find_value_by_variable_indiv(pBase, nVarId, nIndId, cur));
	}// find_value
	bool StatDBManager::find_indiv(DBStatIndiv &cur) {
		assert(this->is_valid());
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		int nId = cur.id();
		if (nId != 0) {
			if (find_indiv_by_id(pBase, nId, cur)) {
				return (true);
			}
		}
		int nDatasetId = cur.get_dataset_id();
		if (nDatasetId == 0) {
			return (false);
		}
		std::string xSigle;
		cur.get_sigle(xSigle);
		if (xSigle.empty()) {
			return (false);
		}
		return find_indiv_by_dataset_sigle(pBase, nDatasetId, xSigle, cur);
	}//find_indiv
	bool StatDBManager::check_variable(DBStatVariable &cur) {
		assert(this->is_valid());
		if (this->find_variable(cur)) {
			return (true);
		}
		std::string xSigle;
		cur.get_sigle(xSigle);
		int nDatasetId = cur.get_dataset_id();
		if ((nDatasetId == 0) || xSigle.empty()) {
			return (false);
		}
		if (!this->begin_transaction()) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		Statement stmtInsert(pBase, SQL_INSERT_VARIABLE);
		if (!stmtInsert.is_valid()) {
			return (false);
		}
		std::string sigle, type, genre, status;
		cur.get_sigle(sigle);
		cur.get_vartype(type);
		cur.get_genre(genre);
		int nz = (cur.is_categ()) ? 1 : 0;
		std::string sSigle = boost::trim_copy(sigle);
		std::string sType = boost::trim_copy(type);
		std::string sGenre = boost::trim_copy(genre);
		stmtInsert.reset();
		stmtInsert.set_parameter(1, nDatasetId);
		stmtInsert.set_parameter(2, sSigle);
		stmtInsert.set_parameter(3, sType);
		stmtInsert.set_parameter(4, nz);
		stmtInsert.set_parameter(5, sSigle);
		stmtInsert.set_parameter(6, sSigle);
		stmtInsert.set_parameter(7, sGenre);
		stmtInsert.set_parameter(8, status);
		if (!stmtInsert.exec()) {
			this->rollback_transaction();
			return (false);
		}
		if (!this->commit_transaction()) {
			this->rollback_transaction();
			return (false);
		}
		return (this->find_variable(cur));
	} // check_variable
	bool StatDBManager::find_variable(DBStatVariable &cur) {
		assert(this->is_valid());
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		int nId = cur.id();
		if (nId != 0) {
			if (find_variable_by_id(pBase, nId, cur)) {
				return (true);
			}
		}
		int nDatasetId = cur.get_dataset_id();
		if (nDatasetId == 0) {
			return (false);
		}
		std::string xSigle;
		cur.get_sigle(xSigle);
		if (xSigle.empty()) {
			return (false);
		}
		return find_variable_by_dataset_sigle(pBase, nDatasetId, xSigle, cur);
	}//find_variable
	bool StatDBManager::remove_dataset(const DBStatDataset &cur) {
		assert(this->is_valid());
		if (!cur.is_removeable()) {
			return (false);
		}
		if (!this->begin_transaction()) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		Statement stmt(pBase, SQL_REMOVE_DATASET);
		if (!stmt.is_valid()) {
			this->rollback_transaction();
			return (false);
		}
		if (!stmt.set_parameter(1, cur.id())) {
			this->rollback_transaction();
			return (false);
		}
		if (!stmt.exec()) {
			this->rollback_transaction();
			return (false);
		}
		if (!this->commit_transaction()) {
			this->rollback_transaction();
			return (false);
		}
		return (true);
	} // remove_dataset
	bool StatDBManager::update_dataset(const DBStatDataset &cur) {
		assert(this->is_valid());
		if (!cur.is_updateable()) {
			return (false);
		}
		if (!this->begin_transaction()) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		Statement stmt(pBase, SQL_UPDATE_DATASET);
		if (!stmt.is_valid()) {
			this->rollback_transaction();
			return (false);
		}
		std::string sigle;
		cur.get_sigle(sigle);
		if (!stmt.set_parameter(1, sigle)) {
			this->rollback_transaction();
			return (false);
		}
		std::string name;
		cur.get_name(name);
		if (!stmt.set_parameter(2, name)) {
			this->rollback_transaction();
			return (false);
		}
		std::string desc;
		cur.get_desc(desc);
		if (!stmt.set_parameter(3, desc)) {
			this->rollback_transaction();
			return (false);
		}
		if (!stmt.set_parameter(4, cur.id())) {
			this->rollback_transaction();
			return (false);
		}
		if (!stmt.exec()) {
			this->rollback_transaction();
			return (false);
		}
		if (!this->commit_transaction()) {
			this->rollback_transaction();
			return (false);
		}
		return (true);
	} // update_dataset
	bool StatDBManager::insert_dataset(const DBStatDataset &cur) {
		assert(this->is_valid());
		if (!cur.is_writeable()) {
			return (false);
		}
		if (!this->begin_transaction()) {
			return (false);
		}
		Database *pBase = this->m_database.get();
		Statement stmt(pBase, SQL_INSERT_DATASET);
		if (!stmt.is_valid()) {
			this->rollback_transaction();
			return (false);
		}
		std::string sigle;
		cur.get_sigle(sigle);
		if (!stmt.set_parameter(1, sigle)) {
			this->rollback_transaction();
			return (false);
		}
		std::string name;
		cur.get_name(name);
		if (!stmt.set_parameter(2, name)) {
			this->rollback_transaction();
			return (false);
		}
		std::string desc;
		cur.get_desc(desc);
		if (!stmt.set_parameter(3, desc)) {
			this->rollback_transaction();
			return (false);
		}
		if (!stmt.exec()) {
			this->rollback_transaction();
			return (false);
		}
		if (!this->commit_transaction()) {
			this->rollback_transaction();
			return (false);
		}
		return (true);
	} // insert_dataset
	bool StatDBManager::find_dataset(DBStatDataset &cur) {
		assert(this->is_valid());
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		int xId = cur.id();
		if (xId != 0) {
			if (find_dataset_by_id(pBase, xId, cur)) {
				return (true);
			}
		}// id
		std::string sigle;
		cur.get_sigle(sigle);
		if (!sigle.empty()) {
			return (find_dataset_by_sigle(pBase, sigle, cur));
		}
		return (false);
	}// find_dataset
	/////////////////////////////////////////
	StatDBManager::StatDBManager() :
		m_intransaction(false) {
	} // StatDBManager
	StatDBManager::StatDBManager(const std::wstring &s) :
		m_intransaction(false) {
		this->open(s);
	} // StatDBManager
	StatDBManager::StatDBManager(const std::string &s) :
		m_intransaction(false) {
		this->open(s);
	} // StatDBManager
	bool StatDBManager::begin_transaction(void) {
		if (this->m_intransaction) {
			return (true);
		}
		assert(this->is_valid());
		Database *pBase = this->m_database.get();
		Statement stmt(pBase, SQL_BEGIN_TRANSACTION);
		if (!stmt.is_valid()) {
			return (false);
		}
		if (stmt.exec()) {
			this->m_intransaction = true;
			return (true);
		}
		return (false);
	} // begin_transaction
	bool StatDBManager::commit_transaction(void) {
		if (!this->m_intransaction) {
			return (false);
		}
		assert(this->is_valid());
		Database *pBase = this->m_database.get();
		Statement stmt(pBase, SQL_COMMIT_TRANSACTION);
		if (!stmt.is_valid()) {
			return (false);
		}
		if (stmt.exec()) {
			this->m_intransaction = false;
			return (true);
		}
		return (false);
	} // commit_transaction
	bool StatDBManager::rollback_transaction(void) {
		if (!this->m_intransaction) {
			return (false);
		}
		assert(this->is_valid());
		Database *pBase = this->m_database.get();
		Statement stmt(pBase, SQL_ROLLBACK_TRANSACTION);
		if (!stmt.is_valid()) {
			return (false);
		}
		if (stmt.exec()) {
			this->m_intransaction = false;
			return (true);
		}
		return (false);
	} // rollback_transaction
	bool StatDBManager::close(void) {
		bool bRet = true;
		Database *pbase = this->m_database.get();
		if (pbase != nullptr) {
			if (pbase->close()) {
				this->m_database.reset();
			}
			else {
				bRet = false;
			}
		}
		return (bRet);
	} // close
	StatDBManager::~StatDBManager() {
	}
	bool StatDBManager::open(const std::string &filename) {
		this->m_database.reset(new Database());
		Database *pBase = this->m_database.get();
		assert(pBase != nullptr);
		if (!pBase->open(filename)) {
			this->m_database.reset();
			return (false);
		}
		if (!this->check_schema()) {
			this->m_database.reset();
			return (false);
		}
		return (true);
	} // open
	bool StatDBManager::open(const std::wstring &filename) {
		std::string s = StringConvert::ws2s(filename);
		return this->open(s);
	} // open
	bool StatDBManager::is_valid(void) const {
		Database *pbase = this->m_database.get();
		return ((pbase != nullptr) && pbase->is_open());
	} // is_valid
	bool StatDBManager::check_schema(void) {
		Database *pbase = this->m_database.get();
		assert(pbase != nullptr);
		assert(pbase->is_open());
		int i = 0;
		while (CREATE_SQL[i] != nullptr) {
			const char *pszSQL = CREATE_SQL[i];
			if (!pbase->exec_sql(pszSQL)) {
				return (false);
			}
			++i;
		} // ok
		return (true);
	} // check_schema
	///////////////////////////////////////////
}// namespace info_sql
