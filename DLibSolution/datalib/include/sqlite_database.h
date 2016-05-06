#pragma once
#ifndef __SQLITE_DATABASE_H__
#define __SQLITE_DATABASE_H__
/////////////////////////////////
#include <string>
#include <list>
#include <stdexcept>
#include <mutex>
//////////////////////////////////////
#include <boost/noncopyable.hpp>
////////////////////////////////////
typedef struct sqlite3 sqlite3;
////////////////////////////////////////
namespace info {
	///////////////////////////////////////
	class sqlite_error : public std::runtime_error {
	public:
		sqlite_error(const std::string &what_arg) :std::runtime_error(what_arg) {}
		sqlite_error(const char *what_arg):std::runtime_error(what_arg){}
		sqlite_error(const sqlite_error &other):std::runtime_error(other){}
		sqlite_error & operator=(const sqlite_error &other) {
			if (this != &other) {
				std::runtime_error::operator=(other);
			}
			return (*this);
		}
		virtual ~sqlite_error(){}
	};// class sqlite_error
	///////////////////////////////////////////
	class SQLite_Statement;
	typedef SQLite_Statement *PSQLite_Statement;
	////////////////////////////////////////
	class SQLite_Database : private boost::noncopyable {
		friend class SQLite_Statement;
	public:
		typedef std::list<PSQLite_Statement> statements_list;
	private:
		int m_errorcode;
		::sqlite3 *m_pDb;
		std::string m_errorstring;
		statements_list m_stmts;
	private:
		std::mutex _mutex;
	public:
		SQLite_Database();
		SQLite_Database(const char *pszFilename);
		SQLite_Database(const wchar_t *pszFilename);
		SQLite_Database(const std::string &filename);
		SQLite_Database(const std::wstring &filename);
		virtual ~SQLite_Database();
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
		void exec(const std::string &s) {
			this->exec_sql(s);
		}
		void exec(const std::wstring &s) {
			this->exec_sql(s);
		}
		void exec(const char *s) {
			this->exec_sql(s);
		}
		void exec(const wchar_t *s) {
			this->exec_sql(s);
		}
	public:
		void begin_transaction(void);
		void commit_transaction(void);
		void rollback_transaction(void);
	protected:
		virtual void prepare_close(void);
		void internal_clear_error(void);
		void internal_get_error(void);
	}; // class SQLite_Database
	/////////////////////////////////////////
}// namespace info
//////////////////////////////////////
#endif // !__SQLITE_DATABASE_H__
