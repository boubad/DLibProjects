#pragma once
#ifndef __DBINDIVPROVIDER_H__
#define __DBINDIVPROVIDER_H__
////////////////////////////////////
#include "indiv.h"
////////////////////////////
#include <vector>
//////////////////////////////////////////
#include <dlib/threads.h>
////////////////////////////////////////////
namespace info {
	class StatDBManager;
	//////////////////////////////////
	class DBIndivProvider : public IIndivProvider {
	public:
		typedef std::vector<IntType> ints_vector;
		typedef std::vector<DBStatValue> values_vector;
		typedef std::map<IntType, DbValue> DataMapType;
		typedef std::vector<Indiv> indivs_vector;
	private:
		StatDBManager *m_pman;
		size_t   m_current;
		ints_vector m_ids;
		DBStatDataset m_oset;
	private:
		dlib::mutex _mutex;
	public:
		DBIndivProvider(StatDBManager *pMan, const DBStatDataset &oSet);
		DBIndivProvider(StatDBManager *pMan, const std::string &setName);
		DBIndivProvider(StatDBManager *pMan, const std::wstring &setName);
		DBIndivProvider(const DBIndivProvider &other);
		DBIndivProvider & operator=(const DBIndivProvider &other);
		~DBIndivProvider();
	public:
		// overrides
		virtual bool is_valid(void);
		virtual bool indivs_count(size_t &nRows);
		virtual bool find(const IntType aIndex, Indiv &oInd);
		virtual bool indiv_at(const size_t pos, Indiv &oInd);
		virtual bool get_random_indivs(const size_t n, indivs_vector &oRes);
		virtual bool reset(void);
		virtual bool next(Indiv &oInd);
	private:
		void initialize(const std::string &setName);
		void initialize(const std::wstring &setName);
	};// class DBIndivProvider
	/////////////////////////////////////
}// namspace info
////////////////////////////////////
#endif // !__DBINDIVPROVIDER_H__
