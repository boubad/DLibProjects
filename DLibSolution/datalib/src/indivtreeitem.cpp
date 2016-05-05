#include "../include/indivtreeitem.h"
#include "../include/info_constants.h"
////////////////////////////
#include <dlib/assert.h>
#include <dlib/threads.h>
//////////////////////////////
#include <algorithm>
namespace info {
	///////////////////////////////////////////////
	IndivTree::IndivTree()
	{

	}
	IndivTree::~IndivTree() {
		this->clear();
	}
	void IndivTree::get_result(intsvector_map &oMap) const {
		oMap.clear();
		const elems_vector &vv = this->m_elems;
		const size_t n = vv.size();
		for (size_t i = 0; i < n; ++i) {
			const PIndivTreeItem p = vv[i];
			DLIB_ASSERT(p != nullptr, "p cannot be null");
			ints_vector oVec;
			p->get_indivs_ids(oVec);
			if (!oVec.empty()) {
				size_t key = (size_t)(i + 1);
				oMap[key] = oVec;
			}
		}// i
	}//get_result
	bool IndivTree::aggregate(IIndivProvider *pProvider, const size_t nbClasses /*= 5 */,
		const VariableMode mode /*= VariableMode::modeNumeric*/) {
		DLIB_ASSERT(pProvider != nullptr, "Provider cannot be null");
		DLIB_ASSERT(pProvider->is_valid(), "Provider is not valid");
		DLIB_ASSERT(nbClasses > 0, "Classes count must be greater than 0");
		this->clear();
		size_t n = 0;
		if (!pProvider->indivs_count(n)) {
			return (false);
		}
		elems_vector &vv = this->m_elems;
		dlib::mutex _mutex;
		size_t num_threads = INFO_NUM_THREADS;
		const IndivTree *pTree = this;
		dlib::parallel_for(num_threads, (size_t)0, n, [&](size_t i) {
			Indiv oInd;
			if (pProvider->find_indiv_at(i, oInd, mode)) {
				IntType aIndex = oInd.id();
				DbValueMap oCenter;
				oInd.get_data(oCenter);
				PIndivTreeItem p = new IndivTreeItem(aIndex, oCenter);
				{
					dlib::auto_mutex oLock(_mutex);
					vv.push_back(p);
				}
			}// i
		});
		size_t nOrder = 0;
		while (this->m_elems.size() > nbClasses) {
			if (!this->aggreg_one_step(nOrder)) {
				break;
			}
		}// while
		return (true);
	}// aggregate
	void IndivTree::clear(void) {
		std::for_each(this->m_elems.begin(), this->m_elems.end(), [](PIndivTreeItem p) {
			delete p;
		});
		this->m_elems.clear();
	}// clear
	void IndivTree::generate_sequence(pairs_vector &oVec) const {
		oVec.clear();
		const size_t n = this->m_elems.size();
		for (size_t j = 0; j < n; ++j) {
			for (size_t i = 0; i < j; ++i) {
				oVec.push_back(std::make_pair(i, j));
			}// i
		}// j
	}//generate_sequence
	double IndivTree::compute_distance(const SizetPair &oPair) const {
		double dRet = 0;
		const elems_vector &v = this->m_elems;
		const size_t i = oPair.first;
		const size_t j = oPair.second;
		const size_t n = v.size();
		if ((i < n) && (j < n) && (i != j)) {
			const PIndivTreeItem p1 = v[i];
			DLIB_ASSERT(p1 != nullptr, "p1 cannot be null");
			const PIndivTreeItem p2 = v[j];
			dRet = p1->distance(p2);
		}
		return dRet;
	}//compute_distance
	bool IndivTree::find_best_pair(SizetPair &oPair, double &dRes) {
		const elems_vector &v = this->m_elems;
		const size_t n = v.size();
		if (n < 2) {
			oPair.first = 0;
			oPair.second = 0;
			dRes = 0;
			return (false);
		}
		bool bFirst = true;
		pairs_vector oVec;
		this->generate_sequence(oVec);
		const size_t nn = oVec.size();
		dlib::mutex _mutex;
		size_t num_threads = INFO_NUM_THREADS;
		const IndivTree *pTree = this;
		dlib::parallel_for(num_threads, (size_t)0, nn, [&](size_t i) {
			SizetPair &xPair = oVec[i];
			double d = pTree->compute_distance(xPair);
			{
				dlib::auto_mutex oLock(_mutex);
				if (bFirst) {
					bFirst = false;
					oPair.first = xPair.first;
					oPair.second = xPair.second;
					dRes = d;
				}
				else if (d < dRes) {
					oPair.first = xPair.first;
					oPair.second = xPair.second;
					dRes = d;
				}
			}// sync
		});
		return (oPair.first != oPair.second);
	}//find_best_pair
	bool IndivTree::aggreg_one_step(size_t &nOrder) {
		elems_vector &v = this->m_elems;
		const size_t n = v.size();
		if (n < 2) {
			return (false);
		}
		SizetPair oPair;
		double dRes = 0;
		if (!this->find_best_pair(oPair, dRes)) {
			return (false);
		}
		size_t i1 = oPair.first;
		size_t i2 = oPair.second;
		DLIB_ASSERT(i1 != i2, "i1  must be different");
		PIndivTreeItem p1 = v[i1];
		DLIB_ASSERT(p1 != nullptr, "p1 must not be null");
		PIndivTreeItem p2 = v[i2];
		DLIB_ASSERT(p2 != nullptr, "p2 must not be null");
		PIndivTreeItem pNew = new IndivTreeItem();
		DLIB_ASSERT(pNew != nullptr, "pNew must not be null");
		pNew->m_order = ++nOrder;
		pNew->m_dist = dRes;
		pNew->m_pleft = p1;
		pNew->m_pright = p2;
		pNew->update_center();
		elems_vector vv;
		for (size_t i = 0; i < n; ++i) {
			if ((i != i1) && (i != i2)) {
				vv.push_back(v[i]);
			}
		}// i
		vv.push_back(pNew);
		v.clear();
		this->m_elems = vv;
		return (true);
	}//aggreg_one_step
	//////////////////////////////////////////////
	IndivTreeItem::IndivTreeItem() :m_index(0), m_pleft(nullptr), m_pright(nullptr), m_order(0), m_dist(0)
	{

	}
	IndivTreeItem::IndivTreeItem(const IntType aIndex, const DbValueMap &oCenter) :
		m_index(aIndex), m_pleft(nullptr), m_pright(nullptr), m_order(0), m_dist(0), m_center(oCenter) {

	}
	IndivTreeItem::~IndivTreeItem() {
		if (this->m_pleft != nullptr) {
			delete this->m_pleft;
			this->m_pleft = nullptr;
		}
		if (this->m_pright != nullptr) {
			delete this->m_pright;
			this->m_pright = nullptr;
		}
	}
	void IndivTreeItem::get_indivs_ids(ints_vector &oVec) const {
		PIndivTreeItem pLeft = this->m_pleft;
		if (pLeft != nullptr) {
			pLeft->get_indivs_ids(oVec);
		}
		PIndivTreeItem pRight = this->m_pright;
		if ((pRight == nullptr) && (this->m_index != 0)) {
			oVec.push_back(this->m_index);
		}
		else if (pRight != nullptr) {
			pRight->get_indivs_ids(oVec);
		}
	}//get_indivs_ids
	void  IndivTreeItem::update_center(void) {
		PIndivTreeItem pLeft = this->m_pleft;
		PIndivTreeItem pRight = this->m_pright;
		if ((pLeft == nullptr) && (pRight == nullptr)) {
			return;
		}
		if ((pRight == nullptr) && (pLeft != nullptr)) {
			this->m_center = pLeft->m_center;
		}
		else if ((pRight != nullptr) && (pLeft == nullptr)) {
			this->m_center = pRight->m_center;
		}
		else if ((pLeft != nullptr) && (pRight != nullptr)) {
			DbValueMap & m1 = pLeft->m_center;
			DbValueMap &m2 = pRight->m_center;
			DbValueMap oCenter;
			//
			std::for_each(m1.begin(), m1.end(), [m2, &oCenter](const std::pair<IntType, DbValue> &oPair) {
				const IntType key = oPair.first;
				auto jt = m2.find(key);
				if (jt != m2.end()) {
					const DbValue &v1 = oPair.second;
					const DbValue &v2 = (*jt).second;
					double ss = (v1.double_value() + v2.double_value()) / 2.0;
					DbValue res(ss);
					oCenter[key] = res;
				}
			});
			this->m_center = oCenter;
		}
	}//update_center
	double IndivTreeItem::distance(const PIndivTreeItem pElem) const {
		DLIB_ASSERT(pElem != nullptr, "pElem cannot be null");
		double dRet = 0;
		size_t nc = 0;
		const DbValueMap & m1 = this->m_center;
		const DbValueMap &m2 = pElem->m_center;
		std::for_each(m1.begin(), m1.end(), [m2, &dRet, &nc](const std::pair<IntType, DbValue> &oPair) {
			const IntType key = oPair.first;
			auto jt = m2.find(key);
			if (jt != m2.end()) {
				const DbValue &v1 = oPair.second;
				const DbValue &v2 = (*jt).second;
				double x = v1.double_value() - v2.double_value();
				dRet += x * x;
				++nc;
			}
		});
		//
		if (nc > 1) {
			dRet /= nc;
		}
		return (dRet);
	}// distance
	////////////////////////////////////////////////
}// namespace info