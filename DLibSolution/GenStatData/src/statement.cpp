/*
 * statement.cpp
 *
 *  Created on: 17 sept. 2013
 *      Author: Boubacar
 */
#include "sqlite3.h"
 //////////////////////////////
#include "../include/dbvalue.h"
#include "../include/database.h"
#include "../include/statement.h"
#include "../include/stringconvert.h"
/////////////////////////////
#include <dlib/assert.h>
#include <dlib/logger.h>
////////////////////////////
namespace info {
	dlib::logger dlog_statement("Statement");
	////////////////////////////
	using namespace info;
	////////////////////////////////////////////
	void Statement::init(Database &oBase, const char *pszSQL) {
		DLIB_ASSERT(pszSQL != nullptr,"SQL Statement is null");
		DLIB_ASSERT(oBase.is_open(), "SQLite database is not opened");
		this->m_pBase = &oBase;
		this->m_pstmt = nullptr;
		this->m_lastcode = SQLITE_OK;
		::sqlite3 *pdb = this->m_pBase->m_pDb;
		assert(pdb != nullptr);
		const char *pszTail = nullptr;
		int nbytes = (int)(::strlen(pszSQL) + 1);
		::sqlite3_stmt *pRes = nullptr;
		int rc = ::sqlite3_prepare_v2(pdb, pszSQL, nbytes, &pRes, &pszTail);
		if ((rc == SQLITE_OK) && (pRes != nullptr)) {
			this->m_pstmt = pRes;
			{
				dlib::auto_mutex oLock(this->m_pBase->_mutex);
				this->m_pBase->m_stmts.push_back(this);
			}
		}
		else {
			this->m_pBase->internal_get_error();
			dlog_statement << dlib::LDEBUG << "Preparing statement " << pszSQL;
			dlog_statement << dlib::LERROR << "SQLite error code " << rc;
		}
	} // Statement
	void Statement::init(Database *pBase, const char *pszSQL) {
		this->init(*pBase, pszSQL);
	} // Statement
	void Statement::init(Database &oBase, const wchar_t *pszSQL) {
		DLIB_ASSERT(pszSQL != nullptr, "SQL Statement is null");
		std::wstring ss(pszSQL);
		std::string s = StringConvert::ws2s(ss);
		this->init(&oBase, s.c_str());
	} // Statement
	void Statement::init(Database *pBase, const wchar_t *pszSQL) {
		DLIB_ASSERT(pszSQL != nullptr, "SQL Statement is null");
		std::wstring ss(pszSQL);
		std::string s = StringConvert::ws2s(ss);
		this->init(pBase, s.c_str());
	} // Statement
	void Statement::init(Database &oBase, const std::string &s) {
		this->init(&oBase, s.c_str());
	} // Statement
	void Statement::init(Database *pBase, const std::string &s) {
		this->init(pBase, s.c_str());
	} // Statement
	void Statement::init(Database &oBase, const std::wstring &ss) {
		this->init(&oBase, ss.c_str());
	} // Statement
	void Statement::init(Database *pBase, const std::wstring &ss) {
		this->init(pBase, ss.c_str());
	} // Statement
	////////////////////////////////////////////////
	Statement::Statement(Database &oBase, const char *pszSQL) :
		m_pBase(nullptr), m_pstmt(nullptr), m_lastcode(SQLITE_OK) {
		this->init(oBase, pszSQL);
	} // Statement
	Statement::Statement(Database *pBase, const char *pszSQL) :
		m_pBase(nullptr), m_pstmt(nullptr), m_lastcode(SQLITE_OK) {
		this->init(pBase, pszSQL);
	} // Statement
	Statement::Statement(Database &oBase, const wchar_t *pszSQL) :
		m_pBase(nullptr), m_pstmt(nullptr), m_lastcode(SQLITE_OK) {
		this->init(&oBase, pszSQL);
	} // Statement
	Statement::Statement(Database *pBase, const wchar_t *pszSQL) :
		m_pBase(nullptr), m_pstmt(nullptr), m_lastcode(SQLITE_OK) {
		this->init(pBase, pszSQL);
	} // Statement
	Statement::Statement(Database &oBase, const std::string &s) :
		m_pBase(nullptr), m_pstmt(nullptr), m_lastcode(SQLITE_OK) {
		this->init(&oBase, s.c_str());
	} // Statement
	Statement::Statement(Database *pBase, const std::string &s) :
		m_pBase(nullptr), m_pstmt(nullptr), m_lastcode(SQLITE_OK) {
		this->init(pBase, s.c_str());
	} // Statement
	Statement::Statement(Database &oBase, const std::wstring &ss) :
		m_pBase(nullptr), m_pstmt(nullptr), m_lastcode(SQLITE_OK) {
		this->init(&oBase, ss.c_str());
	} // Statement
	Statement::Statement(Database *pBase, const std::wstring &ss) :
		m_pBase(nullptr), m_pstmt(nullptr), m_lastcode(SQLITE_OK) {
		this->init(pBase, ss.c_str());
	} // Statement
	bool Statement::is_valid(void) const {
		return ((this->m_pBase != nullptr) && m_pBase->is_open()
			&& (this->m_pstmt != nullptr));
	} // is_valid
	int Statement::cols(void) const {
		return ((int) this->m_vals.size());
	}
	bool Statement::has_values(void) const {
		return (this->m_vals.size() > 0);
	}
	bool Statement::col_name(int icol, std::string &sRet) const {
		bool bRet = false;
		const strings_vector &vv = this->m_names;
		if ((icol >= 0) && ((size_t)icol < vv.size())) {
			sRet = vv[icol];
			bRet = true;
		}
		return (bRet);
	} // col_name
	bool Statement::col_name(int icol, std::wstring &sRet) const {
		std::string s;
		if (!this->col_name(icol, s)) {
			return (false);
		}
		sRet = StringConvert::s2ws(s);
		return (true);
	}
	bool Statement::col_value(int icol, DbValue &val) const {
		bool bRet = false;
		const values_vector &vv = this->m_vals;
		if ((icol >= 0) && ((size_t)icol < vv.size())) {
			val = vv[icol];
			bRet = true;
		}
		return (bRet);
	} // col_value
	bool Statement::col_value(const std::string &sname, DbValue &val) const {
		bool bRet = false;
		const strings_vector &names = this->m_names;
		const int n = (int)names.size();
		for (int i = 0; i < n; ++i) {
			const std::string & s = names[i];
			if (s == sname) {
				return (this->col_value(i, val));
			}
		} // i
		return (bRet);
	} // col_value
	bool Statement::exec(void) {
		DLIB_ASSERT(this->is_valid(), "Statement is not valid");
		values_vector &vals = this->m_vals;
		vals.clear();
		strings_vector &names = this->m_names;
		names.clear();
		::sqlite3_stmt *p = this->m_pstmt;
		this->m_lastcode = SQLITE_OK;
		int rc = ::sqlite3_step(p);
		this->m_lastcode = rc;
		if (rc == SQLITE_DONE) {
			return (true);
		}
		else if (rc != SQLITE_ROW) {
			this->m_pBase->internal_get_error();
			return (false);
		}
		else {
			int nCount = ::sqlite3_column_count(p);
			if (nCount < 1) {
				return (true);
			}
			names.resize(nCount);
			vals.resize(nCount);
			for (int icol = 0; icol < nCount; ++icol) {
				const char *pn = ::sqlite3_column_name(p, icol);
				if (pn != nullptr) {
					names[icol] = pn;
				}
				int ntype = ::sqlite3_column_type(p, icol);
				switch (ntype) {
				case SQLITE_INTEGER: {
					int ival = ::sqlite3_column_int(p, icol);
					vals[icol] = DbValue(ival);
				}
									 break;
				case SQLITE_FLOAT: {
					double dval = ::sqlite3_column_double(p, icol);
					vals[icol] = DbValue(dval);
				}
								   break;
				case SQLITE_TEXT: {
					std::string sval;
					const unsigned char *ptext = ::sqlite3_column_text(p, icol);
					if (ptext != nullptr) {
						const char *ps = reinterpret_cast<const char *>(ptext);
						size_t n = ::strlen(ps);
						std::string ss(n, ' ');
						std::copy(ps, ps + n, ss.begin());
						sval = ss;
					}
					vals[icol] = DbValue(sval);
				}
								  break;
				case SQLITE_BLOB: {
					Blob oBlob;
					const void *p0 = ::sqlite3_column_blob(p, icol);
					int nSize = ::sqlite3_column_bytes(p, icol);
					if ((p0 != nullptr) && (nSize > 0)) {
						const byte *pData = reinterpret_cast<const byte *>(p0);
						oBlob.data(pData, nSize);
					} //nSize
					vals[icol] = DbValue(oBlob);
				}
								  break;
				default:
					break;
				} // nType
			} // icol
		}
		return (true);
	} // exec
	bool Statement::next(void) {
		assert(this->is_valid());
		int rc = this->m_lastcode;
		if (rc != SQLITE_ROW) {
			return (false);
		}
		::sqlite3_stmt *p = this->m_pstmt;
		rc = ::sqlite3_step(p);
		this->m_lastcode = rc;
		if (rc != SQLITE_ROW) {
			return (false);
		}
		int nCount = ::sqlite3_column_count(p);
		if (nCount < 1) {
			return (true);
		}
		values_vector &vals = this->m_vals;
		if (vals.size() != (size_t)nCount) {
			vals.resize(nCount);
		}
		for (int icol = 0; icol < nCount; ++icol) {
			DbValue v;
			vals[icol] = v;
			int ntype = ::sqlite3_column_type(p, icol);
			switch (ntype) {
			case SQLITE_INTEGER: {
				int ival = ::sqlite3_column_int(p, icol);
				vals[icol] = DbValue(ival);
			}
								 break;
			case SQLITE_FLOAT: {
				double dval = ::sqlite3_column_double(p, icol);
				vals[icol] = DbValue(dval);
			}
							   break;
			case SQLITE_TEXT: {
				std::string sval;
				const unsigned char *ptext = ::sqlite3_column_text(p, icol);
				if (ptext != nullptr) {
					const char *ps = reinterpret_cast<const char *>(ptext);
					size_t n = ::strlen(ps);
					std::string ss(n, ' ');
					std::copy(ps, ps + n, ss.begin());
					sval = ss;
				}
				vals[icol] = DbValue(sval);
			}
							  break;
			case SQLITE_BLOB: {
				Blob oBlob;
				const void *p0 = ::sqlite3_column_blob(p, icol);
				int nSize = ::sqlite3_column_bytes(p, icol);
				if ((p0 != nullptr) && (nSize > 0)) {
					const byte *pData = reinterpret_cast<const byte *>(p0);
					oBlob.data(pData, nSize);
				} //nSize
				vals[icol] = DbValue(oBlob);
			}
							  break;
			default:
				break;
			} // nType
		} // icol
		return (true);
	} // next
	void Statement::force_close(void) {
		::sqlite3_stmt *p = this->m_pstmt;
		if (p != nullptr) {
			::sqlite3_finalize(p);
			this->m_pstmt = nullptr;
		} // p
	} // force_close
	bool Statement::close(void) {
		bool bRet = true;
		::sqlite3_stmt *p = this->m_pstmt;
		if (p != nullptr) {
			int rc = ::sqlite3_finalize(p);
			if (rc != SQLITE_OK) {
				bRet = false;
				this->m_pBase->internal_get_error();
			}
			else {
				this->m_pstmt = nullptr;
			}
		}
		Database *pBase = this->m_pBase;
		if (pBase != nullptr) {
			dlib::auto_mutex oLock(this->m_pBase->_mutex);
			Database::statements_list &olist = pBase->m_stmts;
			for (auto it = olist.begin(); it != olist.end(); ++it) {
				PStatement p0 = *it;
				if (p0 == this) {
					olist.erase(it);
					return (true);
				}
			} // it
		} // pBase
		return (bRet);
	} // close
	bool Statement::reset(void) {
		DLIB_ASSERT(this->m_pBase != nullptr, "Parent SQLite database is null");
		DLIB_ASSERT(this->m_pBase->is_open(), "Parent SQLite Database is not opened");
		::sqlite3_stmt *p = this->m_pstmt;
		assert(p != nullptr);
		if (::sqlite3_reset(p) != SQLITE_OK) {
			this->m_pBase->internal_get_error();
			return (false);
		}
		this->m_names.clear();
		this->m_vals.clear();
		return true;
	} // reset
	bool Statement::set_parameter_null(int iParam) {
		DLIB_ASSERT(this->m_pBase != nullptr, "Parent SQLite database is null");
		DLIB_ASSERT(this->m_pBase->is_open(), "Parent SQLite Database is not opened");
		::sqlite3_stmt *p = this->m_pstmt;
		DLIB_ASSERT(p != nullptr,"this Statement is not prepared (null)");
		DLIB_ASSERT(iParam > 0,"Parameter position must be greater than 0");
		if (::sqlite3_bind_null(p, iParam) != SQLITE_OK) {
			this->m_pBase->internal_get_error();
			return (false);
		}
		return (true);
	} // set_parameter_null
	bool Statement::set_parameter(int iParam, int ivalue) {
		DLIB_ASSERT(this->m_pBase != nullptr, "Parent SQLite database is null");
		DLIB_ASSERT(this->m_pBase->is_open(), "Parent SQLite Database is not opened");
		::sqlite3_stmt *p = this->m_pstmt;
		DLIB_ASSERT(p != nullptr, "this Statement is not prepared (null)");
		DLIB_ASSERT(iParam > 0, "Parameter position must be greater than 0");
		if (::sqlite3_bind_int(p, iParam, ivalue) != SQLITE_OK) {
			this->m_pBase->internal_get_error();
			return (false);
		}
		return (true);
	} // set_parameter
	bool Statement::set_parameter(int iParam, long ivalue) {
		DLIB_ASSERT(this->m_pBase != nullptr, "Parent SQLite database is null");
		DLIB_ASSERT(this->m_pBase->is_open(), "Parent SQLite Database is not opened");
		::sqlite3_stmt *p = this->m_pstmt;
		DLIB_ASSERT(p != nullptr, "this Statement is not prepared (null)");
		DLIB_ASSERT(iParam > 0, "Parameter position must be greater than 0");
		if (::sqlite3_bind_int(p, iParam, ivalue) != SQLITE_OK) {
			this->m_pBase->internal_get_error();
			return (false);
		}
		return (true);
	} // set_parameter
	bool Statement::set_parameter(int iParam, unsigned long ivalue) {
		DLIB_ASSERT(this->m_pBase != nullptr, "Parent SQLite database is null");
		DLIB_ASSERT(this->m_pBase->is_open(), "Parent SQLite Database is not opened");
		::sqlite3_stmt *p = this->m_pstmt;
		DLIB_ASSERT(p != nullptr, "this Statement is not prepared (null)");
		DLIB_ASSERT(iParam > 0, "Parameter position must be greater than 0");
		if (::sqlite3_bind_int(p, iParam, ivalue) != SQLITE_OK) {
			this->m_pBase->internal_get_error();
			return (false);
		}
		return (true);
	} // set_parameter
	bool Statement::set_parameter(int iParam, double dval) {
		DLIB_ASSERT(this->m_pBase != nullptr, "Parent SQLite database is null");
		DLIB_ASSERT(this->m_pBase->is_open(), "Parent SQLite Database is not opened");
		::sqlite3_stmt *p = this->m_pstmt;
		DLIB_ASSERT(p != nullptr, "this Statement is not prepared (null)");
		DLIB_ASSERT(iParam > 0, "Parameter position must be greater than 0");
		if (::sqlite3_bind_double(p, iParam, dval) != SQLITE_OK) {
			this->m_pBase->internal_get_error();
			return (false);
		}
		return (true);
	} // set_parameter
	bool Statement::set_parameter(int iParam, const char *pszVal) {
		DLIB_ASSERT(this->m_pBase != nullptr, "Parent SQLite database is null");
		DLIB_ASSERT(this->m_pBase->is_open(), "Parent SQLite Database is not opened");
		::sqlite3_stmt *p = this->m_pstmt;
		DLIB_ASSERT(p != nullptr, "this Statement is not prepared (null)");
		DLIB_ASSERT(iParam > 0, "Parameter position must be greater than 0");
		DLIB_ASSERT(pszVal != nullptr,"String must not be null");
		int nbytes = -1;
		if (::sqlite3_bind_text(p, iParam, pszVal, nbytes,
			SQLITE_TRANSIENT) != SQLITE_OK) {
			this->m_pBase->internal_get_error();
			return (false);
		}
		return (true);
	} // set_parameter
	bool Statement::set_parameter(int iParam, const Blob &oBlob) {
		DLIB_ASSERT(this->m_pBase != nullptr, "Parent SQLite database is null");
		DLIB_ASSERT(this->m_pBase->is_open(), "Parent SQLite Database is not opened");
		::sqlite3_stmt *p = this->m_pstmt;
		DLIB_ASSERT(p != nullptr, "this Statement is not prepared (null)");
		DLIB_ASSERT(iParam > 0, "Parameter position must be greater than 0");
		size_t nSize = oBlob.size();
		const void *pData = (const void *)oBlob.data();
		if ((nSize < 1) || (pData == nullptr)) {
			dlog_statement << dlib::LWARN << "Blob data is invalid ";
			return (false);
		}
		int nbytes = (int)(nSize * sizeof(byte));
		if (::sqlite3_bind_blob(p, iParam, pData, nbytes,
			SQLITE_TRANSIENT) != SQLITE_OK) {
			this->m_pBase->internal_get_error();
			return (false);
		}
		return (true);
	} // set_parameter
	bool Statement::set_parameter(int iParam, const wchar_t *pwszVal) {
		DLIB_ASSERT(pwszVal != nullptr,"String value is null");
		std::wstring ss(pwszVal);
		std::string s = StringConvert::ws2s(ss);
		const char *px = s.c_str();
		return this->set_parameter(iParam, px);
	} // set_parameter
	bool Statement::set_parameter(int iParam, const std::string &sVal) {
		return this->set_parameter(iParam, sVal.c_str());
	} // set_parameter
	bool Statement::set_parameter(int iParam, const std::wstring &sVal) {
		return this->set_parameter(iParam, sVal.c_str());
	} // set_parameter
	///////////////////////////////
	bool Statement::set_parameter_null(const std::string &sname) {
		::sqlite3_stmt *p = this->m_pstmt;
		DLIB_ASSERT(p != nullptr,"Statement is null");
		int iParam = ::sqlite3_bind_parameter_index(p, sname.c_str());
		if (iParam < 1) {
			dlog_statement << dlib::LWARN << "Parameter " << sname << " position not found ";
			return (false);
		}
		return this->set_parameter_null(iParam);
	} // set_paraemter_null
	bool Statement::set_parameter_null(const std::wstring &sname) {
		std::string ss = StringConvert::ws2s(sname);
		return this->set_parameter_null(ss);
	} // set_parameter_null
	bool Statement::set_parameter(const std::string &sname, int ivalue) {
		::sqlite3_stmt *p = this->m_pstmt;
		DLIB_ASSERT(p != nullptr, "Statement is null");
		int iParam = ::sqlite3_bind_parameter_index(p, sname.c_str());
		if (iParam < 1) {
			dlog_statement << dlib::LWARN << "Parameter " << sname << " position not found ";
			return (false);
		}
		return this->set_parameter(iParam, ivalue);
	} // set_parameter
	bool Statement::set_parameter(const std::wstring &sname, int ivalue) {
		std::string ss = StringConvert::ws2s(sname);
		return this->set_parameter(ss, ivalue);
	}
	bool Statement::set_parameter(const std::string &sname, double dval) {
		::sqlite3_stmt *p = this->m_pstmt;
		DLIB_ASSERT(p != nullptr, "Statement is null");
		int iParam = ::sqlite3_bind_parameter_index(p, sname.c_str());
		if (iParam < 1) {
			dlog_statement << dlib::LWARN << "Parameter " << sname << " position not found ";
			return (false);
		}
		return this->set_parameter(iParam, dval);
	}
	bool Statement::set_parameter(const std::string &sname, const Blob &oBlob) {
		::sqlite3_stmt *p = this->m_pstmt;
		DLIB_ASSERT(p != nullptr, "Statement is null");
		int iParam = ::sqlite3_bind_parameter_index(p, sname.c_str());
		if (iParam < 1) {
			dlog_statement << dlib::LWARN << "Parameter " << sname << " position not found ";
			return (false);
		}
		return this->set_parameter(iParam, oBlob);
	}
	bool Statement::set_parameter(const std::wstring &sname, double dval) {
		std::string ss = StringConvert::ws2s(sname);
		return this->set_parameter(ss, dval);
	}
	bool Statement::set_parameter(const std::wstring &sname, const Blob &oBlob) {
		std::string ss = StringConvert::ws2s(sname);
		return this->set_parameter(ss, oBlob);
	}
	bool Statement::set_parameter(const std::string &sname,
		const std::string &sval) {
		::sqlite3_stmt *p = this->m_pstmt;
		DLIB_ASSERT(p != nullptr, "Statement is null");
		int iParam = ::sqlite3_bind_parameter_index(p, sname.c_str());
		if (iParam < 1) {
			dlog_statement << dlib::LWARN << "Parameter " << sname << " position not found ";
			return (false);
		}
		return this->set_parameter(iParam, sval);
	}
	bool Statement::set_parameter(const std::wstring &sname,
		const std::wstring &sval) {
		::sqlite3_stmt *p = this->m_pstmt;
		DLIB_ASSERT(p != nullptr, "Statement is null");
		std::string ss = StringConvert::ws2s(sname);
		int iParam = ::sqlite3_bind_parameter_index(p, ss.c_str());
		if (iParam < 1) {
			dlog_statement << dlib::LWARN << "Parameter " << ss << " position not found ";
			return (false);
		}
		std::string sv = StringConvert::ws2s(sval);
		return this->set_parameter(iParam, sv);
	}
	///////////////////////////////////
	Statement::~Statement() {
		this->close();
	}

} /* namespace sqlite */
