/*
* database.cpp
*
*  Created on: 16 sept. 2013
*      Author: Boubacar
*/
#include "sqlite3.h"
////////////////////////
#include "../include/stringconvert.h"
#include "../include/sqlite_database.h"
#include "../include/sqlite_statement.h"
/////////////////////////////
#include <algorithm>
//////////////////////////////////
namespace info {
	///////////////////////////////////////////////
	static const char *SQL_COMMIT_TRANSACTION = "COMMIT TRANSACTION";
	static const char *SQL_BEGIN_TRANSACTION = "BEGIN TRANSACTION";
	static const char *SQL_ROLLBACK_TRANSACTION = "ROLLBACK TRANSACTION";
	////////////////////////////////////////////
	SQLite_Database::SQLite_Database() :
		m_errorcode(SQLITE_OK), m_pDb(nullptr) {
	}
	SQLite_Database::SQLite_Database(const char *pszFilename) :
		m_errorcode(SQLITE_OK), m_pDb(nullptr) {
		std::string s(pszFilename);
		this->open(s);
	} // SQLite_Database
	SQLite_Database::SQLite_Database(const wchar_t *pszFilename) :
		m_errorcode(SQLITE_OK), m_pDb(nullptr) {
		std::wstring ss(pszFilename);
		std::string s = StringConvert::ws2s(ss);
		this->open(s);
	} // SQLite_Database
	SQLite_Database::SQLite_Database(const std::string &filename) :
		m_errorcode(SQLITE_OK), m_pDb(nullptr) {
		this->open(filename);
	} // SQLite_Database
	SQLite_Database::SQLite_Database(const std::wstring &filename) :
		m_errorcode(SQLITE_OK), m_pDb(nullptr) {
		std::string s = StringConvert::ws2s(filename);
		this->open(filename);
	} // SQLite_Database
	void SQLite_Database::internal_clear_error(void) {
		std::lock_guard<std::mutex> oLock(this->_mutex);
		this->m_errorcode = SQLITE_OK;
		this->m_errorstring.clear();
	} // internal_clear_error
	void SQLite_Database::internal_get_error(void) {
		std::lock_guard<std::mutex> oLock(this->_mutex);
		::sqlite3 *p = this->m_pDb;
		if (p != nullptr) {
			this->m_errorcode = ::sqlite3_errcode(p);
			const char *pSrc = ::sqlite3_errmsg(p);
			if (pSrc != nullptr) {
				{
					this->m_errorstring = pSrc;
					throw sqlite_error(this->m_errorstring);
				}
			}
		}
	} // internal_get_error
	bool SQLite_Database::is_open(void) const {
		return (this->m_pDb != nullptr);
	} // is_open
	bool SQLite_Database::has_error(void) const {
		return (this->m_errorcode != 0);
	} // has_error
	bool SQLite_Database::get_last_error(std::string &sErr) const {
		SQLite_Database &o = const_cast<SQLite_Database &>(*this);
		std::lock_guard<std::mutex> oLock(o._mutex);
		if (this->m_errorcode != 0) {
			sErr = this->m_errorstring;
			return (true);
		}
		return (false);
	} // get_last_error
	bool SQLite_Database::open(const char *pszFilename) {
		this->internal_clear_error();
		::sqlite3 *p = this->m_pDb;
		if (p != nullptr) {
			if (!this->close()) {
				return (false);
			}
		}
		int rc = ::sqlite3_open_v2(pszFilename, &p,
			SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
		if (rc != SQLITE_OK) {
			this->internal_get_error();
		}
		{
			std::lock_guard<std::mutex> oLock(this->_mutex);
			this->m_pDb = p;
		}
		return (rc == SQLITE_OK);
	} // open
	bool SQLite_Database::open(const wchar_t *pszFilename) {
		std::wstring ss(pszFilename);
		std::string s = StringConvert::ws2s(ss);
		return this->open(s.c_str());
	} // open
	bool SQLite_Database::open(const std::string &filename) {
		return this->open(filename.c_str());
	} // open
	bool SQLite_Database::open(const std::wstring &filename) {
		return this->open(filename.c_str());
	} // open
	bool SQLite_Database::get_last_error(std::wstring &sErr) const {
		std::string s;
		if (!this->get_last_error(s)) {
			return (false);
		}
		sErr = StringConvert::s2ws(s);
		return (true);
	}//get_last_error
	SQLite_Database::~SQLite_Database() {
		this->close();
	}
	void SQLite_Database::prepare_close(void) {
		statements_list &oList = this->m_stmts;
		for (auto it = oList.begin(); it != oList.end(); ++it) {
			PSQLite_Statement px = *it;
			if (px != nullptr) {
				px->force_close();
			}
		} // it
		oList.clear();
	} // prepare_close
	bool SQLite_Database::close(void) {
		this->internal_clear_error();
		bool bRet = true;
		::sqlite3 *p = this->m_pDb;
		if (p != nullptr) {
			std::lock_guard<std::mutex> oLock(this->_mutex);
			this->prepare_close();
			if (::sqlite3_close_v2(p) != SQLITE_OK) {
				this->internal_get_error();
				bRet = false;
			}
			else {
				this->m_pDb = nullptr;
			}
		}
		return (bRet);
	} // close
	  ///////////////////////////////
	bool SQLite_Database::exec_sql(const wchar_t *pszSQL) {
		std::wstring ss(pszSQL);
		std::string s = StringConvert::ws2s(ss);
		return (this->exec_sql(s.c_str()));
	} // exec_sql
	bool SQLite_Database::exec_sql(const char *pszSQL) {
		::sqlite3 *p = this->m_pDb;
		if (p == nullptr) {
			return (false);
		}
		char *error = nullptr;
		int rc = ::sqlite3_exec(p, pszSQL, nullptr, nullptr, &error);
		if (error != nullptr) {
			std::lock_guard<std::mutex> oLock(this->_mutex);
			this->m_errorcode = rc;
			this->m_errorstring = error;
			::sqlite3_free(error);
			throw sqlite_error(this->m_errorstring);
		}
		return (rc == SQLITE_OK);
	} // exec_sql
	bool SQLite_Database::exec_sql(const std::string &s) {
		return this->exec_sql(s.c_str());
	} // exec_sql
	bool SQLite_Database::exec_sql(const std::wstring &s) {
		return this->exec_sql(s.c_str());
	} // exec_sql
	void SQLite_Database::begin_transaction(void) {
		this->exec_sql(SQL_BEGIN_TRANSACTION);
	}
	void SQLite_Database::commit_transaction(void) {
		this->exec_sql(SQL_COMMIT_TRANSACTION);
	}
	void SQLite_Database::rollback_transaction(void) {
		this->exec_sql(SQL_ROLLBACK_TRANSACTION);
	}
	  ////////////////////////////
} /* namespace sqlite */
