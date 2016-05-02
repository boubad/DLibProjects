#pragma once
#ifndef __INDIVTREEITEM_H__
#define __INDIVTREEITEM_H__
///////////////////////////////////
#include "indiv.h"
//////////////////////////////////////
namespace info {
	/////////////////////////////////
	class IndivTree;
	////////////////////////////////
	class IndivTreeItem {
		friend class IndivTree;
	public:
		typedef std::vector<IntType> ints_vector;
		typedef std::map<IntType, DbValue> DbValueMap;
		typedef IndivTreeItem *PIndivTreeItem;
	private:
		IntType m_index;
		IndivTreeItem *m_pleft;
		IndivTreeItem *m_pright;
		size_t m_order;
		double m_dist;
		DbValueMap m_center;
	private:
		void update_center(void);
		double distance(const PIndivTreeItem pElem) const;
	public:
		IndivTreeItem();
		IndivTreeItem(const IntType aIndex, const DbValueMap &oCenter);
		~IndivTreeItem();
	public:
		bool isLeaf(void) const {
			return ((this->m_pleft == nullptr) && (this->m_pright == nullptr));
		}
		PIndivTreeItem left(void) const {
			return (this->m_pleft);
		}
		PIndivTreeItem right(void) const {
			return (this->m_pright);
		}
		IntType index(void) const {
			return (this->m_index);
		}
		int order(void) const {
			return (this->m_order);
		}
		double criteria(void) const {
			return (this->m_dist);
		}
		const DbValueMap center(void) const {
			return (this->m_center);
		}
		void get_indivs_ids(ints_vector &oVec) const;
	};// class IndivTreeElem
	////////////////////////////////////////
	class IndivTree {
	public:
		typedef std::vector<IntType> ints_vector;
		typedef std::map<IntType, DbValue> DbValueMap;
		typedef IndivTreeItem *PIndivTreeItem;
		typedef std::vector<PIndivTreeItem> elems_vector;
		typedef std::pair<size_t, size_t> SizetPair;
		typedef std::vector<SizetPair> pairs_vector;
		typedef std::map<size_t, ints_vector>  intsvector_map;
	private:
		elems_vector m_elems;
	private:
		void clear(void);
		void generate_sequence(pairs_vector &oVec) const;
		double compute_distance(const SizetPair &oPair) const;
		bool find_best_pair(SizetPair &oPair, double &dRes);
		bool aggreg_one_step(size_t &nOrder);
	public:
		IndivTree();
		~IndivTree();
	public:
		const elems_vector & elements(void) const {
			return (this->m_elems);
		}
		bool aggregate(IIndivProvider *pProvider, 
			const size_t nbClasses = 5,
			const VariableMode mode = VariableMode::modeNumeric);
		void get_result(intsvector_map &oMap) const;
	};// class IndivTree
	/////////////////////////////////////
}// namespace info
//////////////////////////////////////
#endif // !__INDIVTREEITEM_H__
