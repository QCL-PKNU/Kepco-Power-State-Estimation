/*
 * KpfaYMatrix.h
 *
 *  Created on: 2014. 5. 27.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_Y_MATRIX_H_
#define _KPFA_Y_MATRIX_H_

#include "KpfaConfig.h"
#include "KpfaRawDataMgmt.h"

using namespace boost::numeric::ublas;

/**
 * Admittance (Y) matrix class
 */
class KpfaYMatrix {

private:

	KpfaComplexMatrix_t m_rMatrix;

	KpfaComplexMatrix_t m_rPolarMatrix;

public:

	KpfaYMatrix();

	KpfaYMatrix(KpfaYMatrix *pYmat);

	virtual ~KpfaYMatrix();

	/**
	 * This function will return the size of the matrix.
	 *
	 * @return the height
	 */
	inline uint32_t GetSize() {
		return m_rMatrix.size1();
	}

	/**
	 * This function will return the matrix.
	 *
	 * @return the matrix
	 */
	inline KpfaComplexMatrix_t &GetMatrix() {
		return m_rMatrix;
	}

	/**
	 * This function will return the matrix with polar coordinate values.
	 *
	 * @return the polar matrix
	 */
	inline KpfaComplexMatrix_t &GetPolarMatrix() {
		return m_rPolarMatrix;
	}

	KpfaError_t BuildMatrix(KpfaRawDataMgmt *pDataMgmt);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaYMatrix *pMatrix);

private:

	KpfaError_t ApplyBusData(KpfaRawDataMgmt *pDataMgmt);

	KpfaError_t ApplyBranchData(KpfaRawDataMgmt *pDataMgmt);

#if KPFA_RAW_DATA_VERSION == 33
	KpfaError_t ApplyFixedShuntData(KpfaRawDataMgmt *pDataMgmt);
#endif

	KpfaError_t ApplyTransformerData(KpfaRawDataMgmt *pDataMgmt);

	KpfaError_t ApplySwitchedShuntData(KpfaRawDataMgmt *pDataMgmt);

	void BuildPolarMatrix();
};

#endif /* _KPFA_Y_MATRIX_H_ */
