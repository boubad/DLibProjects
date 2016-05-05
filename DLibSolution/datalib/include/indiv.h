#pragma once
#ifndef __INDIV_H__
#define __INDIV_H__
///////////////////////////////////////
#include <map>
#include <vector>
///////////////////////////////////////
#include "dbstatitems.h"
///////////////////////////////////////////
namespace info {
	////////////////////////////////////////////////
	class Indiv : public DBStatIndiv {
	public:
		typedef std::map<IntType, DbValue> DbValueMap;
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
		const DbValueMap &data(void) const;
		DbValueMap &data(void);
		double distance(const Indiv &other) const;
	public:
		virtual void get_data(DbValueMap &oMap) const;
		virtual void set_data(const DbValueMap &oMap);
		//
		virtual std::ostream & write_to(std::ostream &os) const;
		virtual std::istream & read_from(std::istream &in);
	}; // class Indiv
	////////////////////////////////////////////////
	class IIndivProvider {
	public:
		typedef std::vector<IntType> ints_vector;
		typedef std::map<IntType, DBStatVariable> variables_map;
	public:
		virtual bool is_valid(void) = 0;
		virtual bool indivs_count(size_t &nCount) = 0;
		virtual bool all_indivs_ids(ints_vector &oIds) = 0;
		virtual bool contains_id(const IntType aId) = 0;
		virtual bool find_indiv(const IntType aIndex, Indiv &oInd,
			const VariableMode mode = VariableMode::modeAll) = 0;
		virtual bool find_indiv_at(const size_t pos, Indiv &oInd,
			const VariableMode mode = VariableMode::modeAll) = 0;
		virtual bool distance(const IntType aIndex1, const IntType &Index2,
			double &dRes, const VariableMode mode = VariableMode::modeAll) = 0;
		virtual bool distance_at(const size_t pos1, const size_t pos2,
			double &dRes, const VariableMode mode = VariableMode::modeAll) = 0;
		virtual bool find_variables(variables_map &ovars) = 0;
	}; // class IIndivProvider
	////////////////////////////////////////////////
	class ISerialIndivProvider : public IIndivProvider {
	public:
		virtual bool reset(void) = 0;
		virtual bool next(Indiv &oInd, const VariableMode mode = VariableMode::modeAll) = 0;
	};
	////////////////////////////////////////////////
}// namespace info
///////////////////////////////////////////////
inline void swap(info::Indiv &v1, info::Indiv &v2) {
	v1.swap(v2);
}
inline std::ostream & serialize(const info::Indiv &v, std::ostream &os) {
	return v.write_to(os);
}
inline std::istream & deserialize(info::Indiv &v, std::istream &in) {
	return v.read_from(in);
}
/////////////////////////////////////////
#endif // !__INDIV_H__
