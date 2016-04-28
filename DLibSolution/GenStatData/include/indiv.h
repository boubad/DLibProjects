#pragma once
#ifndef __INDIV_H__
#define __INDIV_H__
///////////////////////////////////////
#include <map>
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
		virtual bool is_valid(void) = 0;
		virtual bool indivs_count(size_t &nRows) = 0;
		virtual bool indiv_at(const size_t pos, Indiv &oInd) = 0;
		virtual bool reset(void) = 0;
		virtual bool next(Indiv &oInd) = 0;
	}; // class IIndivProvider
	////////////////////////////////////////////////
}// namespace info
///////////////////////////////////////////////
inline void swap(info::Indiv &v1, info::Indiv &v2) {
	v1.swap(v2);
}
/////////////////////////////////////////
#endif // !__INDIV_H__
