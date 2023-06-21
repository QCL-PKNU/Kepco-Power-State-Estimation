/*
 * KpfaPowerflow.h
 *
 *  Created on: Oct 13, 2015
 *      Author: youngsun
 */

#ifndef _KPFA_POWERFLOW_H_
#define _KPFA_POWERFLOW_H_

#include "KpfaDebug.h"
#include "KpfaConfig.h"
#include "KpfaRawDataMgmt.h"
#include "KpfaCtrlDataMgmt.h"
#include "KpfaNewtonRaphson.h"

class KpfaPowerflow {

private:

	// Control data management
	KpfaCtrlDataMgmt *m_pCtrlDataMgmt;

	// Newton-raphson 
	KpfaNewtonRaphson *m_pNtrap;

	// Y matrix
	KpfaYMatrix *m_pYmat;

public:

	KpfaPowerflow(KpfaCtrlDataMgmt *pCtrlDataMgmt = NULL);

	virtual ~KpfaPowerflow();

	/**
	 * This function will be used to set the control data management.
	 *
	 * @param pDataMgmt control data management
	 */
	inline void SetCtrlDataMgmt(KpfaCtrlDataMgmt *pDataMgmt) {
		m_pCtrlDataMgmt = pDataMgmt;
	}

	/**
	 * This function will return the Y matrix.
	 *
	 * @return Y matrix
	 */
	inline KpfaComplexVector_t &GetVMatrix() {
		return m_pNtrap->GetVMatrix();
	}

	/**
	 * This function will return the V matrix after executing newton-raphson method.
	 *
	 * @return V matrix
	 */
	inline KpfaYMatrix *GetYMatrix() {
		return m_pYmat;
	}

	KpfaError_t UpdateBranchFlow(KpfaRawDataMgmt *pRawDataMgmt);

	KpfaError_t DoAnalysis(KpfaRawDataMgmt *pRawDataMgmt);

	KpfaError_t DoAnalysis(KpfaRawDataMgmt *pRawDataMgmt, KpfaCtgData *pCtgData);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaPowerflow *pPowerflow);
};

#endif /* _KPFA_POWERFLOW_H_ */
