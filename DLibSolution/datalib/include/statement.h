/*
 * statement.h
 *
 *  Created on: 17 sept. 2013
 *      Author: Boubacar
 */

#ifndef STATEMENT_H_
#define STATEMENT_H_
//////////////////////////////
#include <vector>
#include "dbvalue.h"
/////////////////////////////////
typedef struct sqlite3_stmt sqlite3_stmt;
///////////////////////////////
namespace info {
	///////////////////////////
	class Database;
//////////////////////////////////
class Statement {
	friend class Database;
	typedef std::vector<std::string> strings_vector;
	typedef std::vector<info::DbValue> values_vector;
private:
	Database *m_pBase;
	::sqlite3_stmt *m_pstmt;
	int m_lastcode;
	values_vector m_vals;
	strings_vector m_names;
private:
	void init(Database &oBase, const char *pszSQL);
	void init(Database *pBase, const char *pszSQL);
	void init(Database &oBase, const wchar_t *pszSQL);
	void init(Database *pBase, const wchar_t *pszSQL);
	void init(Database &oBase, const std::string &s);
	void init(Database *pBase, const std::string &s);
	void init(Database &oBase, const std::wstring &s);
	void init(Database *pBase, const std::wstring &s);
public:
	Statement(Database &oBase, const char *pszSQL);
	Statement(Database *pBase, const char *pszSQL);
	Statement(Database &oBase, const wchar_t *pszSQL);
	Statement(Database *pBase, const wchar_t *pszSQL);
	Statement(Database &oBase, const std::string &s);
	Statement(Database *pBase, const std::string &s);
	Statement(Database &oBase, const std::wstring &s);
	Statement(Database *pBase, const std::wstring &s);
	virtual ~Statement();
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
	bool set_parameter(int iParam, const Blob &oBlob);
	//
	bool set_parameter_null(const std::string &sname);
	bool set_parameter_null(const std::wstring &sname);
	bool set_parameter(const std::string &sname, int ivalue);
	bool set_parameter(const std::wstring &sname, int ivalue);
	bool set_parameter(const std::string &sname, double dval);
	bool set_parameter(const std::wstring &sname, double dval);
	bool set_parameter(const std::string &sname, const std::string &sval);
	bool set_parameter(const std::wstring &sname, const std::wstring &sval);
	bool set_parameter(const std::string &sname, const info::Blob &oBlob);
	bool set_parameter(const std::wstring &sname, const info::Blob &oBlob);
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
private:
	void force_close(void);
private:
	// no implementation
	Statement(const Statement &other);
	Statement & operator=(const Statement &other);
};

} /* namespace sqlite */
#endif /* STATEMENT_H_ */
