
#ifndef DATABASE_H_
#define DATABASE_H_
//////////////////////
#include <string>
#include <list>
#include <dlib/threads.h>
/////////////////////////////
typedef struct sqlite3 sqlite3;
//////////////////////////////
namespace info {
////////////////////////////////////////
class Statement;
typedef Statement *PStatement;
////////////////////////////////////////
class Database {
	friend class Statement;
public:
	typedef std::list<PStatement> statements_list;
private:
	int m_errorcode;
	::sqlite3 *m_pDb;
	std::string m_errorstring;
	statements_list m_stmts;
private:
	dlib::mutex  _mutex;
public:
	Database();
	Database(const char *pszFilename);
	Database(const wchar_t *pszFilename);
	Database(const std::string &filename);
	Database(const std::wstring &filename);
	virtual ~Database();
public:
	bool open(const char *pszFilename);
	bool open(const wchar_t *pszFilename);
	bool open(const std::string &filename);
	bool open(const std::wstring &filename);
	bool is_open(void) const;
	bool has_error(void) const;
	bool get_last_error(std::string &sErr) const;
	bool get_last_error(std::wstring &sErr) const;
	bool close(void);
	bool exec_sql(const char *pszSQL);
	bool exec_sql(const wchar_t *pszSQL);
	bool exec_sql(const std::string &s);
	bool exec_sql(const std::wstring &s);
protected:
	virtual void prepare_close(void);
	void internal_clear_error(void);
	void internal_get_error(void);
private:
	// no implementation
	Database(const Database &other);
	Database & operator=(const Database &other);
};
///////////////////////////////////
} /* namespace sqlite */
#endif /* DATABASE_H_ */
