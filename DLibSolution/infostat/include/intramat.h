#pragma once
#ifndef INTRAMAT_H_
#define INTRAMAT_H_
/////////////////////////////
#include "matresult.h"
#include "indiv.h"
#include "distancemap.h"
#include "indivmap.h"
/////////////////////////////////
namespace info {
	///////////////////////////////////////////
	template<typename IDTYPE = unsigned long, typename DISTANCETYPE = long,
		typename STRINGTYPE = std::string>
		class IntraMatElem : boost::noncopyable {
		public:
			using sizets_pair = std::pair<size_t, size_t>;
			using pairs_list = std::list<sizets_pair>;
			using ints_vector = std::vector<IDTYPE>;
			using sizets_vector = std::vector<size_t>;
			using DistanceMapType = DistanceMap<IDTYPE, DISTANCETYPE>;
			using IndivType = Indiv<IDTYPE, STRINGTYPE>;
			using IndivTypePtr = std::shared_ptr<IndivType>;
			using SourceType = IIndivSource<IDTYPE, STRINGTYPE>;
			using IntraMatElemType = IntraMatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
			using RescritType = std::atomic<DISTANCETYPE>;
			using IntraMatElemResultType = IntraMatElemResult<IDTYPE, DISTANCETYPE, STRINGTYPE>;
			using IntraMatElemResultPtr = std::shared_ptr<IntraMatElemResultType>;
			using IndivMapType = IndivMap<IDTYPE, STRINGTYPE, DISTANCETYPE>;
			using queue_type = MatElemResultBackgounder<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		private:
			DispositionType m_disp;
			queue_type *m_pqueue;
			DISTANCETYPE m_crit;
			DistanceMapType *m_pdist;
			STRINGTYPE m_sigle;
			sizets_vector m_indexes;
			std::unique_ptr<DistanceMapType> m_odist;
		public:
			IntraMatElem(DispositionType disp = DispositionType::invalid,
				queue_type *pq = nullptr) :m_disp(disp), m_pqueue(pq), m_crit(0), m_pdist(nullptr) {
			}
			virtual ~IntraMatElem() {
			}
		public:
			void sigle(const STRINGTYPE &s) {
				this->m_sigle = s;
			}
			IntraMatElemResultPtr getResult(StageType stage = StageType::current) const {
				const DistanceMapType *pDist = this->m_pdist;
				assert(pDist != nullptr);
				IntraMatElemResultPtr oRet(new IntraMatElemResultType(this->m_crit, this->m_indexes, this->m_disp, stage));
				IntraMatElemResultType *p = oRet.get();
				assert(p != nullptr);
				p->sigle = this->m_sigle;
				return (oRet);
			}
			DISTANCETYPE criteria(void) const {
				return (this->m_crit);
			}
			void indexes(sizets_vector &oIndex) {
				oIndex = this->m_indexes;
			}
			void arrange(queue_type *pSubscriber = nullptr) {
				assert(this->m_pdist != nullptr);
				if (pSubscriber != nullptr) {
					this->m_pqueue = pSubscriber;
				}
				DISTANCETYPE oCrit(this->m_crit);
				this->notify(StageType::started);
				do {
					if (!this->one_iteration(oCrit)) {
						break;
					}
					this->notify(StageType::current);
				} while (true);
				this->notify(StageType::finished);
			}// arrange
			void arrange(SourceType *pProvider,
				queue_type *pSubscriber = nullptr) {
				assert(pProvider != nullptr);
				this->initialize(pProvider);
				this->arrange(pSubscriber);
			}// arrange
			void arrange(IndivMapType *pMap,
				queue_type *pSubscriber = nullptr) {
				assert(pMap != nullptr);
				this->initialize(pMap);
				this->arrange(pSubscriber);
			}// arrange
			void arrange(DistanceMapType *pDist,
				queue_type *pSubscriber = nullptr) {
				assert(pDist != nullptr);
				this->initialize(pDist);
				this->arrange(pSubscriber);
			}// arrange
			static IntraMatElemResultPtr perform_arrange(SourceType *pSource, queue_type *pSubscriber = nullptr) {
				IntraMatElemType oMat;
				oMat.arrange(pSource, pSubscriber);
				return (oMat.getResult());
			}// perform_arrange
			static IntraMatElemResultPtr perform_arrange(DistanceMapType *pSource, queue_type *pSubscriber = nullptr) {
				IntraMatElemType oMat;
				oMat.arrange(pSource, pSubscriber);
				return (oMat.getResult());
			}// perform_arrange
			static IntraMatElemResultPtr perform_arrange(IndivMapType *pSource, queue_type *pSubscriber = nullptr) {
				IntraMatElemType oMat;
				oMat.arrange(pSource, pSubscriber);
				return (oMat.getResult());
			}// perform_arrange
		protected:
			void notify(StageType stage = StageType::current) {
				if (this->m_pqueue != nullptr) {
					IntraMatElemResultPtr res = this->getResult(stage);
					this->m_pqueue->put(res);
				}
			}//notify
#if defined(_MSC_VER)
			bool one_iteration(DISTANCETYPE &oCrit) {
				using task_type = concurrency::task<IntraMatElemResultPtr>;
				using tasks_vector = std::vector<task_type>;
				using results_vector = std::vector<IntraMatElemResultPtr>;
				//
				pairs_list q;
				bool bRet = this->find_best_try(q, oCrit);
				if (!bRet) {
					return (false);
				}
				const size_t nx = q.size();
				if (nx < 1) {
					return (false);
				}
				size_t i1 = 0, i2 = 0;
				sizets_pair p = q.front();
				q.pop_front();
				i1 = p.first;
				i2 = p.second;
				if (i1 == i2) {
					return (false);
				}
				if (oCrit >= this->m_crit) {
					return (false);
				}
				this->permute_items(i1, i2);
				this->m_crit = oCrit;
				if (q.empty()) {
					return (true);
				}
				sizets_vector oldIndexes(this->m_indexes);
				DISTANCETYPE oldCrit(oCrit);
				tasks_vector tasks;
				DistanceMapType *pDist = this->m_pdist;
				while(!q.empty()) {
					sizets_pair pp = q.front();
					size_t j1 = pp.first;
					size_t j2 = pp.second;
					q.pop_front();
					if (j1 != j2) {
						task_type t([j1, j2, pDist,oldIndexes]()->IntraMatElemResultPtr {
							IntraMatElemType xMat;
							xMat.m_pdist = pDist;
							xMat.m_indexes = oldIndexes;
							xMat.permute_items(j1, j2);
							xMat.m_crit = xMat.criteria(xMat.m_indexes);
							DISTANCETYPE cx(xMat.m_crit);
							do {
								if (!xMat.one_iteration(cx)) {
									break;
								}
							} while (true);
							return (xMat.getResult());
						});
						tasks.push_back(t);
					}
				}// q
				auto joinTask = concurrency::when_all(tasks.begin(), tasks.end()).then([this](results_vector vv) {
					for (auto &oRes : vv) {
						IntraMatElemResultType *p = oRes.get();
						if (p != nullptr) {
							if (p->first < this->m_crit) {
								this->m_crit = p->first;
								this->m_indexes = p->second;
							}
						}// p
					}// oRes
				});
				joinTask.wait();
				return (true);
			} //one_iteration
#else
			bool one_iteration(DISTANCETYPE &oCrit) {
				//
				pairs_list q;
				bool bRet = this->find_best_try(q, oCrit);
				if (!bRet) {
					return (false);
				}
				const size_t nx = q.size();
				if (nx < 1) {
					return (false);
				}
				size_t i1 = 0, i2 = 0;
				sizets_pair p = q.front();
				q.pop_front();
				i1 = p.first;
				i2 = p.second;
				if (i1 == i2) {
					return (false);
				}
				if (oCrit >= this->m_crit) {
					return (false);
				}
				this->permute_items(i1, i2);
				this->m_crit = oCrit;
				//
				if (!q.empty()) {
					sizets_vector oldIndexes(this->m_indexes);
					DISTANCETYPE oldCrit(oCrit);
					while (!q.empty()) {
						sizets_pair pp = q.front();
						size_t j1 = pp.first;
						size_t j2 = pp.second;
						q.pop_front();
						if (j1 != j2) {
							IntraMatElemType xMat;
							xMat.m_pdist = this->m_pdist;
							xMat.m_indexes = oldIndexes;
							xMat.permute_items(j1, j2);
							xMat.m_crit = xMat.criteria(xMat.m_indexes);
							xMat.one_iteration(oldCrit);
							if (xMat.m_crit < this->m_crit) {
								this->m_crit = xMat.m_crit;
								this->m_indexes = xMat.m_indexes;
							}
						}
					} // more paths
				} // not empty
				return (true);
			} //one_iteration
#endif // _MSC_VER

			bool find_best_try(pairs_list &qq, DISTANCETYPE &oCrit) {
				const sizets_vector indexes(this->m_indexes);
				const size_t n = this->m_pdist->size();
				DISTANCETYPE oldCrit = oCrit;
				for (size_t i = 0; i < n; ++i) {
					for (size_t j = 0; j < i; ++j) {
						sizets_vector temp(indexes);
						if (temp.size() < n) {
							return (false);
						}
						const size_t tt = temp[i];
						temp[i] = temp[j];
						temp[j] = tt;
						DISTANCETYPE c = this->criteria(temp);
						if (c <= oldCrit) {
							auto it =
								std::find_if(qq.begin(), qq.end(),
									[i, j](const sizets_pair &p)->bool {
								if ((p.first == (size_t)i) && (p.second == (size_t)j)) {
									return (true);
								}
								else if ((p.first == (size_t)j) && (p.second == (size_t)i)) {
									return (true);
								}
								else {
									return (false);
								}
							});
							if (it == qq.end()) {
								if ((c == oldCrit) && (!qq.empty())) {
									sizets_pair oPair(std::make_pair(i, j));
									qq.push_back(oPair);
								}
								else if (c < oldCrit) {
									oldCrit = c;
									qq.clear();
									sizets_pair oPair(std::make_pair(i, j));
									qq.push_back(oPair);
								}
							} // may add
						} // candidate
					} // j
				} // i
				oCrit = oldCrit;
				return (!qq.empty());
			} //find_best_try_serial
		private:
			DISTANCETYPE distance(const size_t i1, const size_t i2) const {
				assert(this->m_pdist != nullptr);
				DISTANCETYPE d = 0;
				this->m_pdist->indexed_get(i1, i2, d);
				return (d);
			} // distance
			DISTANCETYPE criteria(const sizets_vector &indexes) {
				const size_t n = indexes.size();
				DISTANCETYPE dRet = 0;
				if (n < 2) {
					return (dRet);
				}
				const size_t nx = (size_t)(n - 1);
				for (size_t i = 0; i < nx; ++i) {
					const size_t i1 = indexes[i];
					const size_t i2 = indexes[i + 1];
					dRet = (DISTANCETYPE)(dRet + this->distance(i1, i2));
				} // i
				return (dRet);
			} // criteria
			void permute_items(const size_t i1, const size_t i2) {
				sizets_vector &vv = this->m_indexes;
				const size_t tt = vv[i1];
				vv[i1] = vv[i2];
				vv[i2] = tt;
			} // permute_items
			void initialize(DistanceMapType *pDist) {
				assert(pDist != nullptr);
				this->m_pdist = pDist;
				const size_t n = pDist->size();
				sizets_vector & indexes = this->m_indexes;
				indexes.resize(n);
				for (size_t i = 0; i < n; ++i) {
					indexes[i] = i;
				} // i
				this->m_crit = this->criteria(indexes);
			} // initialize
			void initialize(IndivMapType *pMap) {
				assert(pMap != nullptr);
				m_odist.reset(new DistanceMapType());
				DistanceMapType *pDist = m_odist.get();
				assert(pDist != nullptr);
				pMap->distance_map(*pDist);
				this->initialize(pDist);
			} // initialize
			void initialize(SourceType *pProvider) {
				assert(pProvider != nullptr);
				this->m_odist.reset(new DistanceMapType(pProvider));
				DistanceMapType *pDist = this->m_odist.get();
				assert(pDist != nullptr);
				initialize(pDist);
			} // initialize
	};
	////////////////////////////////////////////
	template<typename IDTYPE = unsigned long, typename DISTANCETYPE = long,
		typename STRINGTYPE = std::string>
		class IntraMatOrd : boost::noncopyable {
		public:
			using MatElemType = IntraMatElem<IDTYPE, DISTANCETYPE, STRINGTYPE>;
			//
			using DistanceMapType = typename MatElemType::DistanceMapType;
			using MatElemResultType = typename MatElemType::IntraMatElemResultType;
			using MatElemResultPtr = typename MatElemType::IntraMatElemResultPtr;
			using SourceType = typename MatElemType::SourceType;
			using ints_vector = typename MatElemType::ints_vector;
			using sizets_vector = typename MatElemType::sizets_vector;
			using RescritType = typename MatElemType::RescritType;
			using IndivType = typename MatElemType::IndivType;
			using IndivTypePtr = typename MatElemType::IndivTypePtr;
			using MatOrdType = IntraMatOrd<IDTYPE, DISTANCETYPE, STRINGTYPE>;
			using queue_type = MatElemResultBackgounder<IDTYPE, DISTANCETYPE, STRINGTYPE>;
		private:
			queue_type *m_pqueue;
			std::unique_ptr<MatElemType> m_vars;
			std::unique_ptr<MatElemType> m_inds;
			STRINGTYPE m_sigle;
		protected:
			bool prep_vars(SourceType *pProvider, queue_type *pq) {
				this->m_vars.reset(new MatElemType(DispositionType::variable));
				MatElemType *pMat = this->m_vars.get();
				assert(pMat != nullptr);
				pMat->sigle(this->m_sigle);
				pMat->arrange(pProvider, pq);
				return (true);
			} // prep_vars
			bool prep_inds(SourceType *pProvider, queue_type *pq) {
				this->m_inds.reset(new MatElemType(DispositionType::indiv));
				MatElemType *pMat = this->m_inds.get();
				assert(pMat != nullptr);
				pMat->sigle(this->m_sigle);
				pMat->arrange(pProvider, pq);
				return (true);
			} // prep_inds
		public:
			IntraMatOrd(queue_type *pq = nullptr) : m_pqueue(pq) {
			} // MatOrd
			IntraMatOrd(SourceType *pIndsSource, SourceType *pVarsSource, queue_type *pRes = nullptr) : m_pqueue(pq) {
				this->arrange(pIndsSource, pVarsSource, pRes);
			} // MatOrd
			virtual ~IntraMatOrd() {
			}
			void sigle(const STRINGTYPE &s) {
				this->m_sigle = s;
			}
			MatElemType *variables(void) const {
				return (this->m_vars.get());
			}
			MatElemType *indivs(void) const {
				return (this->m_inds.get());
			}
			void arrange(SourceType *pIndsSource, SourceType *pVarsSource, queue_type *pRes = nullptr) {
				assert(pIndsSource != nullptr);
				assert(pVarsSource != nullptr);
				queue_type *pq = this->m_pqueue;
				if (pRes != nullptr) {
					pq = pRes;
				}
#if defined(_MSC_VER)
				concurrency::parallel_invoke([this,pIndsSource,pq]() {
					(void)this->prep_inds(pIndsSource, pq);
				},
					[this,pVarsSource,pq]() {
					(void)this->prep_vars(pVarsSource, pq);
				});
#else
				std::future<bool> fInd = std::async([&]()->bool {
					return this->prep_inds(pIndsSource, pq);
				});
				std::future<bool> fVar = std::async([&]()->bool {
					return this->prep_vars(pVarsSource, pq);
				});
				bool bRet = fInd.get() && fVar.get();
#endif // _MSC_VER

			} // arrange
	};
	////////////////////////////////////

}// namespace infO
 ///////////////////////////////////
#endif /* INTRAMAT_H_ */