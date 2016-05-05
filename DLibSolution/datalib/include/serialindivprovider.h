#pragma once
#ifndef __SERIALINDIVPROVIDER_H__
#define __SERIALINDIVPROVIDER_H__
/////////////////////////////////////
#include "indiv.h"
//////////////////////////////////////
#include <dlib/threads.h>
/////////////////////////////////////////
namespace info {
	/////////////////////////////////
	class SerialIndivProvider : public ISerialIndivProvider {
	private:
		IIndivProvider *m_provider;
		size_t m_current;
	private:
		dlib::mutex _mutex;
	public:
		SerialIndivProvider(IIndivProvider *pProvider);
		~SerialIndivProvider();
		virtual bool reset(void);
		virtual bool next(Indiv &oInd, const VariableMode mode = VariableMode::modeAll);
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
	};// class SerialIndivProvider
	////////////////////////////////////
}// namespace info
//////////////////////////////////////
#endif // !__SERIALINDIVPROVIDER_H__
