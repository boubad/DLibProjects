#pragma once
#ifndef __CRITERIA_FUNC_H__
#define __CRITERIA_FUNC_H__
/////////////////////////////////
#include "info_includes.h"
////////////////////////////////////
namespace info {
	//////////////////////////////////
	template <typename T>
	class InfoImage {
	public:
		using image_type = InfoImage<T>;
		using sizets_vector = std::vector<size_t>;
	private:
		size_t m_nrows;
		size_t m_ncols;
		std::unique_ptr<T> m_data;
	public:
		InfoImage(size_t nRows = 0, size_t nCols = 0, const T *pData = nullptr) :m_nrows(0), m_ncols(0) {
			if ((nRows > 0) && (nCols > 0)) {
				this->m_nrows = nRows;
				this->m_ncols = nCols;
				const size_t nn = (size_t)(nRows * nCols);
				this->m_data.reset(new T[nn]);
				T *pDest = this->m_data.get();
				assert(pDest != nullptr);
				if (pData != nullptr) {
					for (size_t i = 0; i < nn; ++i) {
						pDest[i] = pData[i];
					}
				}
				else {
					for (size_t i = 0; i < nn; ++i) {
						pDest[i] = (T)0;
					}
				}
			}
		}
		template <typename X>
		InfoImage(size_t nRows, size_t nCols, const std::vector<X> &oData) :m_nrows(0), m_ncols(0) {
			const size_t nn = (size_t)(nCols * nRows);
			if ((nRows > 0) && (nCols > 0) && (oData.size() >= nn)) {
				this->m_nrows = nRows;
				this->m_ncols = nCols;
				this->m_data.reset(new T[nn]);
				T *pData = this->m_data.get();
				assert(pData != nullptr);
				for (size_t i = 0; i < nn; ++i) {
					pData[i] = oData[i];
				}
			}
		}
		virtual ~InfoImage() {}
	public:
		bool get_pixel(size_t i, size_t j, T &val) const {
			bool bRet = false;
			const size_t nc = this->m_ncols;
			if ((i < this->m_nrows) && (j < nc)) {
				const T * pData = this->m_data.get();
				if (pData != nullptr) {
					val = pData[i * nc + j];
					bRet = true;
				}
			}
			return (bRet);
		}// get_pixel
		bool set_pixel(size_t i, size_t j, T val) {
			bool bRet = false;
			const size_t nc = this->m_ncols;
			if ((i < this->m_nrows) && (j < nc)) {
				T * pData = const_cast<T *>(this->m_data.get());
				if (pData != nullptr) {
					pData[i * nc + j] = val;
					bRet = true;
				}
			}
			return (bRet);
		}// set_pixel
		void get_size(size_t &nRows, size_t &nCols) const {
			nRows = this->m_nrows;
			nCols = this->m_ncols;
		}
		T operator()(size_t i, size_t j) const {
			T val = (T)0;
			(void)this->get_pixel(i, j,val);
			return (val);
		}
		std::future<bool> filterAsync(InfoImage<double> *pDest, const sizets_vector *pRowIndexes = nullptr,
			const sizets_vector *pColIndexes = nullptr) const {
			return std::async(std::launch::async, [this, pRowIndexes, pColIndexes,&pDest]()->bool {
				return (this->compute_criterias(pDest, pRowIndexes,pColIndexes));
			});
		}// filterAnsync
		std::future<bool> writeAsync(std::ostream &os,bool bCoords = false) {
			return std::async(std::launch::async, [this,bCoords, &os]()->bool {
				bool bRet = false;
				try {
					const size_t nRows = this->m_nrows;
					const size_t nCols = this->m_ncols;
					os << "# nbCols: " << nCols << "\t nbRows: " << nRows << std::endl;
					if (!bCoords) {
						for (size_t i = 0; i < nRows; ++i) {
							for (size_t j = 0; j < nCols; ++j) {
								T val;
								(void)this->get_pixel(i, j, val);
								if (j != 0) {
									os << "\t";
								}
								os << val;
							}// j
							os << std::endl;
						}// i
					}
					else {
						for (size_t i = 0; i < nRows; ++i) {
							for (size_t j = 0; j < nCols; ++j) {
								T val;
								(void)this->get_pixel(i, j, val);
								os << i << "\t" << j << "\t" << val << std::endl;
							}// j
							os << std::endl;
						}// i
					}
				}catch(...){}
				return (bRet);
			});
		}// writeAsync
		std::future<bool> writeAsync(std::wostream &os,bool bCoords = false) {
			return std::async(std::launch::async, [this,bCoords, &os]()->bool {
				bool bRet = false;
				try {
					const size_t nRows = this->m_nrows;
					const size_t nCols = this->m_ncols;
					os << L"# nbCols: " << nCols << L"\t nbRows: " << nRows << std::endl;
					if (!bCoords) {
						for (size_t i = 0; i < nRows; ++i) {
							for (size_t j = 0; j < nCols; ++j) {
								T val;
								(void)this->get_pixel(i, j, val);
								if (j != 0) {
									os << L"\t";
								}
								os << val;
							}// j
							os << std::endl;
						}// i
					}
					else {
						for (size_t i = 0; i < nRows; ++i) {
							for (size_t j = 0; j < nCols; ++j) {
								T val;
								(void)this->get_pixel(i, j, val);
								os << i << L"\t" << j << L"\t" << val << std::endl;
							}// j
							os << std::endl;
						}// i
					}
				}
				catch (...) {}
				return (bRet);
			});
		}// writeAsync
	private:
		void set_size(size_t nRows, size_t nCols) {
			if ((nRows > 0) && (nCols > 0)) {
				this->m_nrows = nRows;
				this->m_ncols = nCols;
				const size_t nn = (size_t)(nRows * nCols);
				this->m_data.reset(new T[nn]);
				T *pDest = this->m_data.get();
				assert(pDest != nullptr);
				for (size_t i = 0; i < nn; ++i) {
					pDest[i] = (T)0;
				}
			}
		}
		const T *get_data(void) const {
			return (this->m_data.get());
		}
		
		
		bool compute_criterias(InfoImage<double> *pDest, const sizets_vector *pRowIndexes = nullptr,
			const sizets_vector *pColIndexes = nullptr) const {
			bool bRet = false;
			const size_t nRows = this->m_nrows;
			const size_t nCols = this->m_ncols;
			const T *pData = this->m_data.get();
			if ((pDest == nullptr) || (nRows < 3) || (nCols < 3) || (pData == nullptr)) {
				return (bRet);
			}
			pDest->set_size(nRows - 2, nCols - 2);
			const size_t nr1 = (size_t)(nRows - 1);
			const size_t nc1 = (size_t)(nCols - 1);
			if ((pRowIndexes != nullptr) && (pColIndexes != nullptr) && (pRowIndexes->size() >= nRows) &&
				(pColIndexes->size() >= nCols)) {
				const sizets_vector &rowindexes = *pRowIndexes;
				const sizets_vector &colindexes = *pColIndexes;
				InfoImage oImage(nRows, nCols);
				for (size_t i = 0; i < nRows; ++i) {
					size_t ii = rowindexes[i];
					for (size_t j = 0; j < nCols;++j) {
						size_t jj = colindexes[j];
						T val;
						(void)this->get_pixel(ii, jj,val);
						oImage.set_pixel(i, j, val);
					}
				}// i
				for (size_t i = 1; i < nr1; ++i) {
					for (size_t j = 1; j < nc1; ++j) {
						double val = image_type::compute_criteria(&oImage, i, j);
						pDest->set_pixel(i - 1, j - 1, val);
					}// j
				}// i
			}
			else {
				for (size_t i = 1; i < nr1; ++i) {
					for (size_t j = 1; j < nc1; ++j) {
						double val = image_type::compute_criteria(this, i, j);
						pDest->set_pixel(i - 1, j - 1, val);
					}// j
				}// i
			}
			double *pRes = const_cast<double *>(pDest->get_data());
			size_t r = 0, c = 0;
			pDest->get_size(r, c);
			size_t nn = (size_t)(r * c);
			assert(nn > 0);
			assert(pRes != nullptr);
			auto it = std::minmax_element(pRes, pRes + nn);
			double vmin = *(it.first);
			double vmax = *(it.second);
			assert(vmax > vmin);
			double delta = 1.0 / (vmax - vmin);
			std::transform(pRes, pRes + nn, pRes, [vmin,delta](double v)->double {
				return ((v - vmin) * delta);
			});
			return (true);
		}// compute_criterias
		static double compute_criteria(const image_type *pIm, size_t i, size_t j) {
			const size_t r = pIm->m_nrows;
			const size_t c = pIm->m_ncols;
			if ((i >= r) || (j >= c)) {
				return (T(0));
			}
			const size_t r1 = (size_t)(r - 1);
			const size_t c1 = (size_t)(c - 1);
			if ((i == 0) || (j == 0) || (i == r1) || (j == c1)) {
				return (0);
			}// i < 1
			const image_type &f = *pIm;
			double x = (double)f(i, j);
			double s = (std::abs(x - f(j - 1, i)) + std::abs(x - f(i - 1, j - 1)) + std::abs(x - f(i - 1, j)) + std::abs(x - f(i - 1, j + 1)) +
				std::abs(x - f(i, j + 1)) + std::abs(x - f(i + 1, j + 1)) + std::abs(x - f(i + 1, j)) + std::abs(x - f(i - 1, j + 1))) / 8.0;
			return (s);
		}// compute_criteria
	};// class InfoImage<T>
	///////////////////////////
}// namespace info
///////////////////////////////////
#endif // !__CRITERIA_FUNC_H__

