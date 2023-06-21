/*
 * KpfaEqrModule.h
 *
 *  Created on: Oct 13, 2015
 *      Author: youngsun
 */

#ifndef _KPFA_EQR_MODULE_H_
#define _KPFA_EQR_MODULE_H_

#include "KpfaDebug.h"
#include "KpfaConfig.h"
#include "KpfaYMatrix.h"
#include "KpfaRawDataMgmt.h"
#include "KpfaCtrlDataMgmt.h"

/**
 * The declaration of the class for EQR algorithm module
 */
class KpfaEqrModule {

private:

	// CQR value
	double m_nCqrL;
	double m_nCqrC;

	// EQR values (L, C)
	double m_nEqrL;
	double m_nEqrC;

	// EQR margin values (L, C)
	double m_nEqrMarginL;
	double m_nEqrMarginC;

	// System required L, C
	double m_nSysReqL;
	double m_nSysReqC;

	// EQR matrix
	KpfaDoubleMatrix_t m_rEqrMat;

	// B matrix
	KpfaDoubleMatrix_t m_rBggMat;
	KpfaDoubleMatrix_t m_rBglMat;
	KpfaDoubleMatrix_t m_rBlgMat;
	KpfaDoubleMatrix_t m_rBllMat;

	// Control data management
	KpfaCtrlDataMgmt *m_pCtrlDataMgmt;

public:

	KpfaEqrModule(KpfaCtrlDataMgmt *pCtrlDataMgmt = NULL) {

		m_pCtrlDataMgmt = pCtrlDataMgmt;

		m_nCqrC = 0;
		m_nCqrL = 0;

		m_nEqrC = 0;
		m_nEqrL = 0;

		m_nEqrMarginC = pCtrlDataMgmt->m_nEqrMarginC;
		m_nEqrMarginL = pCtrlDataMgmt->m_nEqrMarginL;

		m_nSysReqL = 0;
		m_nSysReqC = 0;

		m_rEqrMat.clear();

		m_rBggMat.clear();
		m_rBglMat.clear();
		m_rBlgMat.clear();
		m_rBllMat.clear();
	}

	virtual ~KpfaEqrModule() {

		m_rEqrMat.clear();

		m_rBggMat.clear();
		m_rBglMat.clear();
		m_rBlgMat.clear();
		m_rBllMat.clear();
	}

	KpfaError_t Execute(KpfaRawDataMgmt *pRawDataMgmt, KpfaCtrlDataMgmt *pCtrlDataMgmt);

	KpfaError_t Calculate(KpfaRawDataMgmt *pRawDataMgmt, bool_t bFirst = TRUE);

	/**
	 * This function will return the reactive value of EQR.
	 *
	 * @return reactive value of EQR
	 */
	inline double GetEqrL() {	return m_nEqrL;		}

	/**
	 * This function will return the capacitive value of EQR.
	 *
	 * @return capacitive value of EQR
	 */
	inline double GetEqrC() {	return m_nEqrC;		}

	/**
	 * This function will return the reactive value of CQR.
	 *
	 * @return reactive value of CQR
	 */
	inline double GetCqrL() {	return m_nCqrL;		}

	/**
	 * This function will return the capacitive value of CQR.
	 *
	 * @return capacitive value of CQR
	 */
	inline double GetCqrC() {	return m_nCqrC;		}

	/**
	 * This function will return the EQR matrix.
	 *
	 * @return EQR matrix
	 */
	inline KpfaDoubleMatrix_t &GetEqrMatrix() {
		return m_rEqrMat;
	}

private:

	KpfaError_t InverseMatrix(KpfaDoubleMatrix_t &rMat,
							  KpfaDoubleMatrix_t &rInvMat);

	KpfaError_t CalculateWeightFactor(KpfaDoubleMatrix_t &rWmat);

	KpfaError_t CalculateCQR(KpfaRawDataMgmt *pDataMgmt, 
							 KpfaDoubleMatrix_t &rCmat, 
							 bool_t bFirst);

	KpfaError_t CalculateEQR(KpfaRawDataMgmt *pDataMgmt,
							 KpfaDoubleMatrix_t &rWmat,
							 KpfaDoubleMatrix_t &rCmat,
							 KpfaDoubleMatrix_t &rEmat,
							 bool_t bFirst);

	KpfaError_t BuildZMatrix(KpfaYMatrix *pYmat, KpfaDoubleMatrix_t &rZmat);

	KpfaError_t BuildBMatrix(KpfaRawDataMgmt *pDataMgmt, KpfaYMatrix *pYmat);

public:

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaEqrModule *pModule);
};

#endif /* _KPFA_EQR_MODULE_H_ */
