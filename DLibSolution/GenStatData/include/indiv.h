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
		typedef std::map<IntType, DbValue> DataMapType;
	private:
		DataMapType m_map;
	public:
		Indiv();
		Indiv(const IntType aIndex);
		Indiv(const IntType aIndex, const DataMapType &oMap);
		Indiv(const DBStatIndiv &oBaseInd);
		Indiv(const Indiv &other);
		Indiv & operator=(const Indiv &other);
		virtual ~Indiv();
	public:
		void swap(Indiv &other);
		const DataMapType &data(void) const;
		DataMapType &data(void);
		double distance(const Indiv &other) const;
	public:
		virtual void get_data(DataMapType &oMap) const;
		virtual void set_data(const DataMapType &oMap);
	}; // class Indiv
	////////////////////////////////////////////////
	class IIndivProvider {
	public:
		typedef std::vector<IntType> ints_vector;
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
	}; // class IIndivProvider
	////////////////////////////////////////////////
	class ISerialIndivProvider {
	public:
		virtual bool reset(void) = 0;
		virtual bool next(Indiv &oInd, const VariableMode mode = VariableMode::modeAll) = 0;
	};
	/////////////////////////////////////////////////
}// namespace info
///////////////////////////////////////////////
inline void swap(info::Indiv &v1, info::Indiv &v2) {
	v1.swap(v2);
}
/////////////////////////////////////////
#endif // !__INDIV_H__
