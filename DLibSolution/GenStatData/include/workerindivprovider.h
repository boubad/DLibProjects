#pragma once
#ifndef __WORKERINDIVPROVIDER_H__
#define __WORKERINDIVPROVIDER_H__
//////////////////////////////////////////////
#include "indiv.h"
/////////////////////////////////////////////////
#include <set>
///////////////////////////
#include <dlib/threads.h>
/////////////////////////////////////////////////
namespace info {
	///////////////////////////////////////////////
	class WorkerIndivProvider : public  ISerialIndivProvider {
	public:
		typedef std::map<IntType, DbValue> DbValueMap;
		typedef std::vector<IntType> ints_vector;
		typedef std::vector<DBStatVariable> variables_vector;
		typedef std::map<IntType, DBStatVariable> variables_map;
		typedef std::set<IntType> ints_set;
	private:
		IIndivProvider *m_provider;
		size_t m_current;
		variables_map	m_vars;
		ints_set	m_filter;
	private:
		dlib::read_write_mutex _mutex;
	public:
		WorkerIndivProvider(IIndivProvider *pProvider);
		~WorkerIndivProvider();
	public:
		virtual bool is_valid(void);
		virtual bool indivs_count(size_t &nCount);
		virtual bool all_indivs_ids(ints_vector &oIds);
		virtual bool contains_id(const IntType aId);
		virtual bool find_indiv(const IntType aIndex, Indiv &oInd,
			const VariableMode mode = VariableMode::modeAll);
		virtual bool find_indiv_at(const size_t pos, Indiv &oInd,
			const VariableMode mode = VariableMode::modeAll);
		virtual bool distance(const IntType aIndex1, const IntType &Index2,
			double &dRes, const VariableMode mode = VariableMode::modeAll);
		virtual bool distance_at(const size_t pos1, const size_t pos2,
			double &dRes, const VariableMode mode = VariableMode::modeAll);
		virtual bool find_variables(variables_map &ovars);
		//
		virtual bool reset(void);
		virtual bool next(Indiv &oInd, const VariableMode mode = VariableMode::modeAll);
		//
		void filter_indiv(Indiv &oInd) const;
		void get_filter(variables_vector &oVars) const;
		void set_filter(const variables_vector &oVars);
		bool find_indiv_filtered(const IntType aIndex, Indiv &oInd, const VariableMode mode = VariableMode::modeAll);
		bool find_indiv_at_filtered(const size_t pos, Indiv &oInd, const VariableMode mode = VariableMode::modeAll);
		bool distance_filtered(const IntType aIndex1, const IntType &Index2,double &dRes, const VariableMode mode = VariableMode::modeAll);
		bool distance_at_filtered(const size_t pos1, const size_t pos2,double &dRes, const VariableMode mode = VariableMode::modeAll);
	};// class WorkerIndivProvider
	/////////////////////////////////////////////////
}// namespace info
////////////////////////////////////////////////
#endif // !__WORKERINDIVPROVIDER_H__

