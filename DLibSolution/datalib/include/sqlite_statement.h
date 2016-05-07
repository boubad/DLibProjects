/*
* statement.h
*
*  Created on: 17 sept. 2013
*      Author: Boubacar
*/

#ifndef SQLITE_STATEMENT_H_
#define SQLITE_STATEMENT_H_
//////////////////////////////
#include <vector>
#include "dbvalue.h"
//////////////////////////////////
#include <boost/noncopyable.hpp>
#include <boost/container/vector.hpp>
/////////////////////////////////
typedef struct sqlite3_stmt sqlite3_stmt;
///////////////////////////////
namespace info {
	///////////////////////////
	class SQLite_Database;
	//////////////////////////////////
	class SQLite_Statement : private boost::noncopyable {
		friend class SQLite_Database;
		typedef boost::container::vector<std::string> strings_vector;
		typedef boost::container::vector<DbValue> values_vector;
	private:
		bool m_first;
		SQLite_Database *m_pBase;
		::sqlite3_stmt *m_pstmt;
		int m_lastcode;
		values_vector m_vals;
		strings_vector m_names;
	private:
		void init(SQLite_Database &oBase, const char *pszSQL);
		void init(SQLite_Database *pBase, const char *pszSQL);
		void init(SQLite_Database &oBase, const wchar_t *pszSQL);
		void init(SQLite_Database *pBase, const wchar_t *pszSQL);
		void init(SQLite_Database &oBase, const std::string &s);
		void init(SQLite_Database *pBase, const std::string &s);
		void init(SQLite_Database &oBase, const std::wstring &s);
		void init(SQLite_Database *pBase, const std::wstring &s);
	public:
		SQLite_Statement(SQLite_Database &oBase, const char *pszSQL);
		SQLite_Statement(SQLite_Database *pBase, const char *pszSQL);
		SQLite_Statement(SQLite_Database &oBase, const wchar_t *pszSQL);
		SQLite_Statement(SQLite_Database *pBase, const wchar_t *pszSQL);
		SQLite_Statement(SQLite_Database &oBase, const std::string &s);
		SQLite_Statement(SQLite_Database *pBase, const std::string &s);
		SQLite_Statement(SQLite_Database &oBase, const std::wstring &s);
		SQLite_Statement(SQLite_Database *pBase, const std::wstring &s);
		virtual ~SQLite_Statement();
	public:
		bool is_valid(void) const;
		bool close(void);
		bool reset(void);
		//
		bool set_parameter_null(int iParam);
		bool set_parameter(int iParam, int ivalue);
		bool set_parameter(int iParam, long ivalue);
		bool set_parameter(int iParam, unsigned long ivalue);
		bool set_parameter(int iParam, double dval);
		bool set_parameter(int iParam, const char *pszVal);
		bool set_parameter(int iParam, const wchar_t *pwszVal);
		bool set_parameter(int iParam, const std::string &sVal);
		bool set_parameter(int iParam, const std::wstring &sVal);
		//
		bool set_parameter_null(const std::string &sname);
		bool set_parameter_null(const std::wstring &sname);
		bool set_parameter(const std::string &sname, int ivalue);
		bool set_parameter(const std::wstring &sname, int ivalue);
		bool set_parameter(const std::string &sname, double dval);
		bool set_parameter(const std::wstring &sname, double dval);
		bool set_parameter(const std::string &sname, const std::string &sval);
		bool set_parameter(const std::wstring &sname, const std::wstring &sval);
		//
		bool exec(void);
		bool next(void);
		int cols(void) const;
		bool has_values(void) const;
		bool col_name(int icol, std::string &sRet) const;
		bool col_name(int icol, std::wstring &sRet) const;
		bool col_value(int icol, DbValue &val) const;
		bool col_value(const std::string &sname, DbValue &val) const;
		bool col_value(const std::wstring &sname, DbValue &val) const;
	public:
		std::string get_column_name(unsigned long idx) {
			std::string sRet;
			this->col_name((int)idx, sRet);
			return (sRet);
		}
		bool move_next(void) {
			return this->next();
		}
		unsigned long get_num_columns(void) {
			return this->cols();
		}
		void get_column(unsigned long idx, int &item) {
			DbValue v;
			this->col_value((int)idx, v);
			item = v.int_value();
		}
		void get_column(unsigned long idx, long &item) {
			DbValue v;
			this->col_value((int)idx, v);
			item = v.long_value();
		}
		void get_column(unsigned long idx, unsigned long &item) {
			DbValue v;
			this->col_value((int)idx, v);
			item = v.unsigned_long_value();
		}
		void get_column(unsigned long idx, double &item) {
			DbValue v;
			this->col_value((int)idx, v);
			item = v.double_value();
		}
		void get_column(unsigned long idx, std::string &item) {
			DbValue v;
			this->col_value((int)idx, v);
			v.string_value(item);
		}
		void get_column(unsigned long idx, std::wstring &item) {
			DbValue v;
			this->col_value((int)idx, v);
			v.string_value(item);
		}
		
	public:
		void bind(unsigned long parameter_id, int item) {
			this->set_parameter((int)parameter_id, item);
		}
		void bind(unsigned long parameter_id, double item) {
			this->set_parameter((int)parameter_id, item);
		}
		void bind(unsigned long parameter_id, long item) {
			this->set_parameter((int)parameter_id, item);
		}
		void bind(unsigned long parameter_id, unsigned long item) {
			this->set_parameter((int)parameter_id, item);
		}
		void bind(unsigned long parameter_id, const std::string &item) {
			this->set_parameter((int)parameter_id, item);
		}
		void bind(unsigned long parameter_id, const std::wstring &item) {
			this->set_parameter((int)parameter_id, item);
		}
		void bind_null(unsigned long parameter_id) {
			this->set_parameter_null((int)parameter_id);
		}
		void bind_text(unsigned long parameter_id, const std::string &item) {
			this->set_parameter((int)parameter_id, item);
		}
		void bind_int64(unsigned long parameter_id, unsigned long item) {
			this->set_parameter((int)parameter_id, item);
		}
		void bind_int(unsigned long parameter_id, int item) {
			this->set_parameter((int)parameter_id, item);
		}
		void bind_double(unsigned long parameter_id, double item) {
			this->set_parameter((int)parameter_id, item);
		}
	};

} /* namespace sqlite */
#endif /* STATEMENT_H_ */
