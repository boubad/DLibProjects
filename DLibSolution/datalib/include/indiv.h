#pragma once
#ifndef __INDIV_H__
#define __INDIV_H__
///////////////////////////////////////
#include "dbstatitems.h"
//////////////////////////////////
namespace info {
	////////////////////////////////////////////////
	class Indiv : public DBStatIndiv {
	public:
		typedef boost::container::flat_map<IntType, DbValue> DbValueMap;
	private:
		DbValueMap m_map;
	public:
		Indiv();
		Indiv(const IntType aIndex);
		Indiv(const IntType aIndex, const DbValueMap &oMap);
		Indiv(const DBStatIndiv &oBaseInd);
		Indiv(const Indiv &other);
		Indiv & operator=(const Indiv &other);
		virtual ~Indiv();
	public:
		void swap(Indiv &other);
		double distance(const Indiv &other) const;
	public:
		virtual void get_data(DbValueMap &oMap) const;
		virtual void set_data(const DbValueMap &oMap);
	}; // class Indiv
	////////////////////////////////////////////////
	class IIndivProvider {
	public:
		virtual bool is_valid(void) = 0;
		virtual bool indivs_count(size_t &nCount) = 0;
		virtual bool get_variables_map(variables_map &oMap) = 0;
		virtual bool find_indiv(const IntType aIndex, Indiv &oInd,
			const VariableMode mode = VariableMode::modeAll) = 0;
		virtual bool find_indiv_at(const size_t pos, Indiv &oInd,
			const VariableMode mode = VariableMode::modeAll) = 0;
	public:
		virtual ~IIndivProvider(){}
	}; // class IIndivProvider
	////////////////////////////////////////////////
	class ISerialIndivProvider  {
	public:
		virtual bool reset(void) = 0;
		virtual bool next(Indiv &oInd, const VariableMode mode = VariableMode::modeAll) = 0;
	public:
		virtual ~ISerialIndivProvider(){}
	};
	////////////////////////////////////////////////
}// namespace info
///////////////////////////////////////////////
inline void swap(info::Indiv &v1, info::Indiv &v2) {
	v1.swap(v2);
}
/////////////////////////////////////////
#endif // !__INDIV_H__
