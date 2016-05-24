#pragma once
/*
 * clusterize.h
 *
 *  Created on: 20 mai 2016
 *      Author: boubad
 */

#ifndef CLUSTERIZE_H_
#define CLUSTERIZE_H_
/////////////////////////////////
#include "clusterscollection.h"
/////////////////////////////////
namespace info {
////////////////////////////////////////
template<typename U = unsigned long,typename STRINGTYPE = std::string,typename DISTANCETYPE = unsigned long>
class ClusterizeKMeans: public ClustersCollection<U,STRINGTYPE> {
public:
	using IndexType = U;
	using sizets_vector = std::vector<size_t>;
	using IndivType = Indiv<U,STRINGTYPE>;
	using IndivTypePtr = std::shared_ptr<IndivType>;
	using indivptrs_vector = std::vector<IndivTypePtr>;
	using DataMap = std::map<U, InfoValue>;
	using ints_sizet_map = std::map<U, size_t>;
	using IndivClusterType = IndivCluster<U,STRINGTYPE>;
	using SourceType = IIndivSource<U>;
	using clusters_vector = std::vector<IndivClusterType>;
	using ints_vector = std::vector<U>;
	using sizet_intsvector_map = std::map<size_t, ints_vector>;
	using datamaps_vector = std::vector<DataMap>;
	using ClustersCollectionType = ClustersCollection<U,STRINGTYPE>;
private:
	size_t m_niter;
	ints_sizet_map m_map;
	sizets_vector m_args;
	std::mutex _mutex;
public:
	ClusterizeKMeans(std::atomic_bool *pCancel = nullptr) :
			ClustersCollectionType(pCancel), m_niter(0) {
	}
	virtual ~ClusterizeKMeans() {
	}
public:
	size_t compute(const datamaps_vector &oSeeds, SourceType *pProvider,
			size_t nbIters, std::atomic_bool *pxCancel = nullptr) {
		const size_t nbClusters = oSeeds.size();
		if (!this->initialize_process(pProvider, nbClusters,
				nbIters, pxCancel)) {
			return (0);
		}
		this->clear();
		clusters_vector &clusters = this->clusters();
		std::atomic_bool *pCancel = this->get_cancelleable_flag();
		for (size_t i = 0; i < nbClusters; ++i) {
			const DataMap &oCenter = oSeeds[i];
			U aIndex = (U) (i + 1);
			IndivClusterType c(aIndex, oCenter,STRINGTYPE(), pCancel);
			clusters.push_back(c);
		} // i
		while (this->m_niter < nbIters) {
			if (this->check_interrupt()) {
				return (0);
			}
			if (!this->one_iteration()) {
				break;
			}
		} // while
		this->post_terminate_process();
		return (this->m_niter);
	} // compute
	size_t compute_random(const size_t nbClusters, SourceType *pProvider,
			size_t nbIters = 100, std::atomic_bool *pxCancel = nullptr) {
		assert(nbClusters > 1);
		assert(pProvider != nullptr);
		assert(nbIters > 0);
		if (!this->initialize_process(pProvider, nbClusters,
				nbIters, pxCancel)) {
			return (0);
		}
		indivptrs_vector oInds;
		if (!this->get_random_indivs(nbClusters, oInds)) {
			return (0);
		}
		datamaps_vector oSeeds;
		for (auto &o : oInds) {
			const IndivType *p = o.get();
			assert(p != nullptr);
			DataMap oMap = p->center();
			oSeeds.push_back(oMap);
		}
		return (this->compute(oSeeds, pProvider, nbIters, pxCancel));
	} //compute_random
	virtual bool process(SourceType *pSource, const size_t nbClusters = 5,
			const size_t nbMaxIters = 100,
			std::atomic_bool *pCancel = nullptr) {
		return (this->compute_random(nbClusters, pSource, nbMaxIters,
				pCancel) > 0);
	} // process
protected:
	virtual void clear(void) {
		ClustersCollectionType::clear();
		this->m_niter = 0;
		this->m_map.clear();
	} // clear
	virtual bool initialize_process(SourceType *pSource,
		const size_t nbClusters, const size_t nbMaxIters = 100,
		std::atomic_bool *pCancel = nullptr) {
		if (!ClustersCollectionType::initialize_process(pSource, nbClusters,
			nbMaxIters, pCancel)) {
			return (0);
		}
		const size_t n = this->get_nbIndivs();
		sizets_vector & v = this->m_args;
		v.resize(n);
		for (size_t i = 0; i < n; ++i) {
			v[i] = i;
		}
		return ((this->check_interrupt()) ? false : true);
	} // initalize_process
	virtual bool one_iteration(void) {
		if (this->m_niter > this->get_nbMaxIters()) {
			return (false);
		}
		clusters_vector &clusters = this->clusters();
		const size_t nc = clusters.size();
		SourceType *pProvider = this->source();
		//
		for (auto && c : clusters) {
			c.clear_members();
		}
		const sizets_vector &args = this->m_args;
		info_parallel_for_each(args.begin(), args.end(), [&](const size_t &i) {
			DISTANCETYPE dMin = 0;
			IndivClusterType *pRes = nullptr;
			IndivTypePtr oInd = pProvider->get(i);
			IndivType *pInd = oInd.get();
			if (pInd != nullptr) {
				for (size_t j = 0; j < nc; ++j) {
					IndivClusterType *pc = &( clusters[j]);
					DISTANCETYPE d = 0;
					if (pc->distance(oInd, d)) {
						if (pRes == nullptr) {
							dMin = d;
							pRes = pc;
						}
						else if (d < dMin) {
							pRes = pc;
						}
					} // distance
				} // j
				if (pRes != nullptr) {
					std::lock_guard<std::mutex> oLock(this->_mutex);
					pRes->add(oInd);
				}
			} // ind
		});
		for (auto && c : clusters) {
			c.update_center();
		}
		if (this->check_interrupt()) {
			return (false);
		}
		ints_sizet_map oMap;
		this->get_indivs_map(oMap);
		if (this->m_map.empty()) {
			this->m_map = oMap;
			return (this->check_interrupt()) ? false : true;
		}
		this->m_niter++;
		bool done = true;
		for (auto it = this->m_map.begin(); it != this->m_map.end(); ++it) {
			const U key = (*it).first;
			const size_t val = (*it).second;
			if (oMap.find(key) == oMap.end()) {
				return (0);
			}
			if (oMap[key] != val) {
				done = false;
				break;
			}
		} // it
		this->m_map = oMap;
		if (this->check_interrupt()) {
			return (false);
		}
		return (!done);
	} // one_iteration
	virtual bool post_terminate_process(void) {
		clusters_vector &clusters = this->clusters();
		std::set<U> oSet;
		for (auto &c : clusters) {
			if (c.is_empty()) {
				oSet.insert(c.id());
			}
		}
		if (this->check_interrupt()) {
			return (false);
		}
		for (auto &key : oSet) {
			auto it = std::find_if(clusters.begin(), clusters.end(), [key](IndivClusterType &c)->bool {
				return (c.id() == key);
			});
			if (it != clusters.end()) {
				clusters.erase(it);
			}
		}// key
		this->update_center();
		return (ClustersCollectionType::post_terminate_process());
	} // post_terminate_process
};
// class ClusterizeKMeans<U>
/////////////////////////////////////////
}// namespace info
//////////////////////////
#endif /* CLUSTERIZE_H_ */
