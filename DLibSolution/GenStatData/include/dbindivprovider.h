#pragma once
#ifndef __DBINDIVPROVIDER_H__
#define __DBINDIVPROVIDER_H__
////////////////////////////////////
#include "indiv.h"
////////////////////////////
#include <map>
////////////////////////////
namespace info {
	class StatDBManager;
	//////////////////////////////////
	class DBIndivProvider : public IIndivProvider {
	public:
		typedef std::vector<IntType> ints_vector;
		typedef std::vector<DBStatValue> values_vector;
		typedef std::map<IntType, DbValue> DataMapType;
		typedef std::vector<Indiv> indivs_vector;
		typedef std::vector<DBStatVariable> variables_vector;
		typedef std::map<IntType,DBStatVariable> variables_map;
	private:
		StatDBManager *m_pman;
		DBStatDataset m_oset;
		ints_vector m_ids;
		variables_map m_vars;
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
		virtual bool indivs_count(size_t &nCount);
		virtual bool all_indivs_ids(ints_vector &oIds);
		virtual bool contains_id(const IntType aId);
		virtual bool find_indiv(const IntType aIndex, Indiv &oInd,
			const VariableMode mode = VariableMode::modeNumeric);
		virtual bool find_indiv_at(const size_t pos, Indiv &oInd,
			const VariableMode mode = VariableMode::modeNumeric);
		virtual bool distance(const IntType aIndex1, const IntType &Index2,
			double &dRes, const VariableMode mode = VariableMode::modeNumeric);
		virtual bool distance_at(const size_t pos1, const size_t pos2,
			double &dRes, const VariableMode mode = VariableMode::modeNumeric);
	private:
		void initialize(const std::string &setName);
		void initialize(const std::wstring &setName);
		void initialize(void);
	};// class DBIndivProvider
	/////////////////////////////////////
}// namspace info
////////////////////////////////////
#endif // !__DBINDIVPROVIDER_H__
