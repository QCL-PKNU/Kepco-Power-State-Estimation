/*
 * KpfaNewtonRaphson.h
 *
 *  Created on: 2014. 6. 2.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_NEWTON_RAPHSON_H_
#define _KPFA_NEWTON_RAPHSON_H_

#include "KpfaLinearSystem.h"
#include "KpfaYMatrix.h"
#include "KpfaConfig.h"
#include "KpfaUtility.h"
#include "KpfaRawDataMgmt.h"

/**
 * Data structure of input parameters
 */
typedef struct {

	// maximum number of iterations
	uint32_t nMaxIteration;

	// tolerance for completing the Newton-Raphson iteration
	double nTolerance;

	// Initial value of the busbar voltage
	KpfaComplex_t rVoltage;

} KpfaNtrapParam_t;

typedef std::vector<uint32_t> KpfaBusIndexList_t;
typedef std::map<uint32_t, uint32_t> KpfaBusIndexMap_t;

class KpfaNewtonRaphson {

private:

	// Raw data management
	KpfaRawDataMgmt *m_pDataMgmt;

	// Parameters for powerflow analysis
	KpfaNtrapParam_t *m_pParam;

	// V matrix
	KpfaComplexVector_t m_rVmat;
	
	// S matrix
	KpfaComplexVector_t m_rSmat;

	// J matrix
    KpfaDoubleMatrix_t m_rJmat;

    // Delta S, V matrices
    KpfaDoubleVector_t m_rDeltaSmat;
    KpfaDoubleVector_t m_rDeltaVmat;

	// P, Q bus index table
    KpfaBusIndexMap_t m_rPbusMap;
    KpfaBusIndexMap_t m_rQbusMap;

	// Tolerance check
	double m_nMaxTolerance;
	uint32_t m_nMaxPbusId;
	uint32_t m_nMaxQbusId;

public:

	KpfaNewtonRaphson(KpfaRawDataMgmt *pDataMgmt, KpfaNtrapParam_t *pParam = NULL);

	virtual ~KpfaNewtonRaphson();

	/**
	 * This function will return the V matrix.
	 * 
	 * @return V matrix
	 */
	inline KpfaComplexVector_t &GetVMatrix() {
		return m_rVmat;
	}

	KpfaError_t Calculate(KpfaYMatrix *pYmat);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaNewtonRaphson *pNtrap);

private:

	KpfaError_t BuildVMatrix();

	KpfaError_t BuildSMatrix(KpfaYMatrix *pYmat);

	KpfaError_t BuildPqBusIndexMaps();
    
    KpfaError_t CalculateJacobiMatrix(KpfaYMatrix *pYmat, KpfaDoubleMatrix_t &rJmat);

    KpfaError_t CalculateDeltaVMatrix(KpfaDoubleMatrix_t &rJmat, 
                                      KpfaDoubleVector_t &rDeltaSmat,
                                      KpfaDoubleVector_t &rDeltaVmat);
    
	KpfaError_t CalculateDeltaSMatrix(KpfaDoubleVector_t &rDeltaSmat);

	KpfaError_t UpdateVMatrix(KpfaDoubleVector_t &rDeltaVmat);

	KpfaError_t UpdateSMatrix(KpfaDoubleVector_t &rDeltaSmat);

	KpfaError_t CheckValidity(KpfaComplexMatrix_t &rYmat, KpfaComplexVector_t &rVmat);
};

#endif /* _KPFA_NEWTON_RAPHSON_H_ */
