/*
 * database.cpp
 *
 *  Created on: 16 sept. 2013
 *      Author: Boubacar
 */
#include "sqlite3.h"
 ////////////////////////
#include "../include/stringconvert.h"
#include "../include/database.h"
#include "../include/statement.h"
/////////////////////////////
#include <cassert>
#include <algorithm>
////////////////////////////////
namespace info {
	////////////////////////////////////////////
	Database::Database() :
		m_errorcode(SQLITE_OK), m_pDb(nullptr) {
	}
	Database::Database(const char *pszFilename) :
		m_errorcode(SQLITE_OK), m_pDb(nullptr) {
		std::string s(pszFilename);
		this->open(s);
	} // Database
	Database::Database(const wchar_t *pszFilename) :
		m_errorcode(SQLITE_OK), m_pDb(nullptr) {
		std::wstring ss(pszFilename);
		std::string s = StringConvert::ws2s(ss);
		this->open(s);
	} // Database
	Database::Database(const std::string &filename) :
		m_errorcode(SQLITE_OK), m_pDb(nullptr) {
		this->open(filename);
	} // Database
	Database::Database(const std::wstring &filename) :
		m_errorcode(SQLITE_OK), m_pDb(nullptr) {
		std::string s = StringConvert::ws2s(filename);
		this->open(filename);
	} // Database
	void Database::internal_clear_error(void) {
		this->m_errorcode = SQLITE_OK;
		this->m_errorstring.clear();
	} // internal_clear_error
	void Database::internal_get_error(void) {
		::sqlite3 *p = this->m_pDb;
		if (p != nullptr) {
			this->m_errorcode = ::sqlite3_errcode(p);
			const char *pSrc = ::sqlite3_errmsg(p);
			if (pSrc != nullptr) {
				this->m_errorstring = pSrc;
			}
		}
	} // internal_get_error
	bool Database::is_open(void) const {
		return (this->m_pDb != nullptr);
	} // is_open
	bool Database::has_error(void) const {
		return (this->m_errorcode != 0);
	} // has_error
	bool Database::get_last_error(std::string &sErr) const {
		if (this->m_errorcode != 0) {
			sErr = this->m_errorstring;
			return (true);
		}
		return (false);
	} // get_last_error
	bool Database::open(const char *pszFilename) {
		assert(pszFilename != nullptr);
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
		this->m_pDb = p;
		return (rc == SQLITE_OK);
	} // open
	bool Database::open(const wchar_t *pszFilename) {
		assert(pszFilename != nullptr);
		std::wstring ss(pszFilename);
		std::string s = StringConvert::ws2s(ss);
		return this->open(s.c_str());
	} // open
	bool Database::open(const std::string &filename) {
		return this->open(filename.c_str());
	} // open
	bool Database::open(const std::wstring &filename) {
		return this->open(filename.c_str());
	} // open
	bool Database::get_last_error(std::wstring &sErr) const {
		std::string s;
		if (!this->get_last_error(s)) {
			return (false);
		}
		sErr = StringConvert::s2ws(s);
		return (true);
	}//get_last_error
	Database::~Database() {
		this->close();
	}
	void Database::prepare_close(void) {
		statements_list &oList = this->m_stmts;
		for (auto it = oList.begin(); it != oList.end(); ++it) {
			PStatement px = *it;
			if (px != nullptr) {
				px->force_close();
			}
		} // it
		oList.clear();
	} // prepare_close
	bool Database::close(void) {
		this->internal_clear_error();
		bool bRet = true;
		::sqlite3 *p = this->m_pDb;
		if (p != nullptr) {
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
	bool Database::exec_sql(const wchar_t *pszSQL) {
		assert(pszSQL != nullptr);
		std::wstring ss(pszSQL);
		std::string s = StringConvert::ws2s(ss);
		return (this->exec_sql(s.c_str()));
	} // exec_sql
	bool Database::exec_sql(const char *pszSQL) {
		assert(pszSQL != nullptr);
		::sqlite3 *p = this->m_pDb;
		if (p == nullptr) {
			return (false);
		}
		char *error = nullptr;
		int rc = ::sqlite3_exec(p, pszSQL, nullptr, nullptr, &error);
		if (error != nullptr) {
			this->m_errorcode = rc;
			this->m_errorstring = error;
			::sqlite3_free(error);
			return (false);
		}
		return (rc == SQLITE_OK);
	} // exec_sql
	bool Database::exec_sql(const std::string &s) {
		return this->exec_sql(s.c_str());
	} // exec_sql
	bool Database::exec_sql(const std::wstring &s) {
		return this->exec_sql(s.c_str());
	} // exec_sql
	////////////////////////////
} /* namespace sqlite */
