/*
 * KpfaYMatrix.cpp
 *
 *  Created on: 2014. 5. 27.
 *      Author: Youngsun Han
 */

#include "KpfaYMatrix.h"
#include "KpfaDebug.h"
#include "KpfaUtility.h"
#include <iomanip>

KpfaYMatrix::KpfaYMatrix() {
	// Do nothing
}

KpfaYMatrix::KpfaYMatrix(KpfaYMatrix *pYmat) {
	m_rMatrix = KpfaComplexMatrix_t(pYmat->GetMatrix());
}

KpfaYMatrix::~KpfaYMatrix() {
	m_rMatrix.clear();
}

/**
 * This function will be used to apply the bus data, especially shunt (BL), for building the Y matrix.
 *
 * [FORTRAN] BUSDAT
 *
 * @param pDataMgmt raw data management
 * @return error information
 */
KpfaError_t
KpfaYMatrix::ApplyBusData(KpfaRawDataMgmt *pDataMgmt) {

	uint32_t k;
	KpfaComplexMatrix_t &mat = GetMatrix();
	KpfaRawDataList_t &dataList = pDataMgmt->GetBusDataList();

	// Update the admittance of the Y matrix with fixed shunt data.
	KpfaRawDataList_t::iterator iter;

	for(iter = dataList.begin(), k = 0; iter != dataList.end(); iter++, k++) {

		KpfaBusData* bus = (KpfaBusData *)*iter;

        // Skip the isolated bus
        if(bus->m_nIde == KPFA_ISOLATED_BUS) {
            continue;
        }

		// For diagonal elements
		//---------------------------------------------------------------------------
		// Y_kk = (Sum of y_kj + Sum of Image (B_kj/2)) + Shunt(GL, BL)
		//---------------------------------------------------------------------------
		mat(k, k) += KpfaComplex_t(bus->m_nGl, bus->m_nBl);
	}

	return KPFA_SUCCESS;
}

#if KPFA_RAW_DATA_VERSION == 33
/**
 * This function will be used to apply the fixed-shunt data for building the Y matrix.
 *
 * @param pDataMgmt raw data management
 * @return error information
 */
KpfaError_t
KpfaYMatrix::ApplyFixedShuntData(KpfaRawDataMgmt *pDataMgmt) {

	uint32_t msize = GetSize();
	KpfaComplexMatrix_t &mat = GetMatrix();
	KpfaRawDataList_t &dataList = pDataMgmt->GetFixedShuntDataList();

	// Update the admittance of the Y matrix with fixed shunt data.
	KpfaRawDataList_t::iterator iter;

	for(iter = dataList.begin(); iter != dataList.end(); iter++) {

		KpfaFixedShuntData *shunt = (KpfaFixedShuntData *)*iter;

		double gl = shunt->m_nGl;
		double bl = shunt->m_nBl;

		// Each index of the array must start from 0.
		uint32_t k = pDataMgmt->GetBusIndex(shunt->m_nI);

		KPFA_CHECK(k >= 0 && k < msize, KPFA_ERROR_YMATIRX_BUILD);

		// For diagonal elements
		//---------------------------------------------------------------------------
		// Y_kk = (Sum of y_kj + Sum of Image (B_kj/2)) + FixedShunt(GL, BL)
		//---------------------------------------------------------------------------
		mat(k, k) += KpfaComplex_t(gl, bl);
	}

	return KPFA_SUCCESS;
}
#endif

/**
 * This function will be used to apply the transformer data for building the Y matrix.
 *
 * @param pDataMgmt raw data management
 * @return error information
 */
KpfaError_t
KpfaYMatrix::ApplyTransformerData(KpfaRawDataMgmt *pDataMgmt) {

    // YOUNGSUN - CHKME
	return KPFA_SUCCESS;
}

/**
 * This function will be used to apply the branch data for building the Y matrix.
 *
 * @param pDataMgmt raw data management
 * @return error information
 */
KpfaError_t
KpfaYMatrix::ApplyBranchData(KpfaRawDataMgmt *pDataMgmt) {

	uint32_t msize = GetSize();
	KpfaComplexMatrix_t &mat = m_rMatrix;
	KpfaRawDataList_t &dataList = pDataMgmt->GetBranchDataList();

	// Calculate the admittance of each branch.
	KpfaRawDataList_t::iterator iter;

	for(iter = dataList.begin(); iter != dataList.end(); iter++) {

		KpfaBranchData *branch = (KpfaBranchData *)*iter;

        // Skip the branch out of service
        if(branch->m_bSt == FALSE) {
            continue;
        }

		double r = branch->m_nR;
		double x = branch->m_nX;
		double hb = branch->m_nB / 2.0;

		double tapInv = 0;
		double tap = branch->m_nTap;

		// Each index of the array must start from 0.
		uint32_t k = pDataMgmt->GetBusIndex(branch->m_nI);
		uint32_t j = pDataMgmt->GetBusIndex(branch->m_nJ);

		KPFA_CHECK(k >= 0 && k < msize, KPFA_ERROR_YMATRIX_BUILD);
		KPFA_CHECK(j >= 0 && j < msize, KPFA_ERROR_YMATRIX_BUILD);

		// For off-diagonal elements
		//--------------------------------------------------------------------------
		// Y_kj = -y_kj = -1/(R+jX)
		//--------------------------------------------------------------------------

		KpfaComplex_t admittance(r, x);
		admittance = pow(admittance, -1);

		if(tap != 1.0) {
			tapInv = 1.0 / tap;
			admittance *= tapInv;
		}

		mat(k, j) -= admittance;
		mat(j, k) = mat(k, j);

		// For diagonal elements
		//--------------------------------------------------------------------------
		// Y_kj = Sum of y_kj + Sum of Image (B_kj/2)
		//--------------------------------------------------------------------------
		if(tap != 1.0) {
			double tmpR = admittance.real();
			double tmpX = admittance.imag();

			mat(k, k) += KpfaComplex_t(tmpR * tapInv, tmpX * tapInv + hb);
			mat(j, j) += KpfaComplex_t(tmpR * tap, tmpX * tap + hb);  
		}
		else {
			admittance.imag(admittance.imag() + hb);

			mat(k, k) += admittance;
			mat(j, j) += admittance;
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will be used to apply the switched-shunt data for building the Y matrix.
 *
 * @param pDataMgmt raw data management
 * @return error information
 */
KpfaError_t
KpfaYMatrix::ApplySwitchedShuntData(KpfaRawDataMgmt *pDataMgmt) {

	uint32_t msize = GetSize();
	KpfaComplexMatrix_t &mat = GetMatrix();
	KpfaRawDataList_t &dataList = pDataMgmt->GetSwitchedShuntDataList();

	// Update the admittance of the Y matrix with fixed shunt data.
	KpfaRawDataList_t::iterator iter;

	for(iter = dataList.begin(); iter != dataList.end(); iter++) {

		KpfaSwitchedShuntData *shunt = (KpfaSwitchedShuntData *)*iter;

		KpfaBusData *shuntBusData = pDataMgmt->GetBusData(shunt->m_nI);
		KPFA_CHECK(shuntBusData != NULL, KPFA_ERROR_INVALID_SSHUNT_DATA);

		double gl = 0;
		double bl = shuntBusData->m_nBl;

		// Each index of the array must start from 0.
		uint32_t k = pDataMgmt->GetBusIndex(shunt->m_nI);

		KPFA_CHECK(k >= 0 && k < msize, KPFA_ERROR_YMATRIX_BUILD);

		// For diagonal elements
		//---------------------------------------------------------------------------
		// Y_kk = (Sum of y_kj + Sum of Image (B_kj/2)) + SwitchedShunt(GL(0), BL)
		//---------------------------------------------------------------------------
		mat(k, k) += KpfaComplex_t(gl, bl);
	}

	return KPFA_SUCCESS;
}

/**
 * This function will build a polar coordinate matrix with the original Y matrix.
 */
void
KpfaYMatrix::BuildPolarMatrix() {

	KpfaComplexMatrix_t &pymat = m_rPolarMatrix;
	KpfaComplexMatrix_t &ymat = m_rMatrix;

	uint32_t msize = ymat.size1();
	pymat.resize(msize, msize, false);
	pymat.clear();

	KpfaComplexMatrix_t::iterator1 riter;
	KpfaComplexMatrix_t::iterator2 citer;

	for(riter = ymat.begin1(); riter != ymat.end1(); riter++) {
		uint32_t i = riter.index1();
		for(citer = riter.begin(); citer != riter.end(); citer++) {
			uint32_t j = citer.index2();
			KpfaComplex_t y_ij = *citer;
			pymat(i,j) = KpfaComplex_t(abs(y_ij), arg(y_ij));
		}
	}
}

/**
 * This function will be used to build a new Y matrix with the given branch data list.
 *
 * @param pDataMgmt raw data management
 * @return error information
 */
KpfaError_t
KpfaYMatrix::BuildMatrix(KpfaRawDataMgmt *pDataMgmt) {

	KPFA_CHECK(pDataMgmt != NULL, KPFA_ERROR_INVALID_ARGUMENT);

	KpfaError_t error;

	uint32_t msize = pDataMgmt->GetBusCount();

	// Resize and reset the matrix.
	m_rMatrix.resize(msize, msize, false);
	m_rMatrix.clear();

	// Branch
	error = ApplyBranchData(pDataMgmt);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	//KpfaPrintComplexMatrix(cout, m_rMatrix);

#if KPFA_RAW_DATA_VERSION == 33
	// Fixed Shunt
	error = ApplyFixedShuntData(pDataMgmt);
	KPFA_CHECK(error == KPFA_SUCCESS, error);
#elif KPFA_RAW_DATA_VERSION == 30
	// Bus
	error = ApplyBusData(pDataMgmt);
	KPFA_CHECK(error == KPFA_SUCCESS, error);
#endif

	// Transformer
	error = ApplyTransformerData(pDataMgmt);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// CHKME - YOUNGSUN
#if 0
	// Switched Shunt
	error = ApplySwitchedShuntData(pDataMgmt);
	KPFA_CHECK(error == KPFA_SUCCESS, error);
#endif

	// Build a polar coordinate matrix also
	BuildPolarMatrix();

	return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaYMatrix::Write(ostream &rOut) {

	KpfaComplexMatrix_t &mat = GetMatrix();

	rOut << "Y Matrix (" << GetSize() << " x " << GetSize() << "):" << endl;

	KpfaComplexMatrix_t::iterator1 riter;
	KpfaComplexMatrix_t::iterator2 citer;

	for(riter = mat.begin1(); riter != mat.end1(); riter++) {
		for(citer = riter.begin(); citer != riter.end(); citer++) {
			rOut << (citer.index1() + 1) << ",\t";
			rOut << (citer.index2() + 1) << ",\t";
			rOut << *citer << endl;
		}
	}
}

ostream &operator << (ostream &rOut, KpfaYMatrix *pMatrix) {
	pMatrix->Write(rOut);
	return rOut;
}
