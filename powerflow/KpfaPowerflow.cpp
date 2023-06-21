/*
 * KpfaPowerflow.cpp
 *
 *  Created on: Oct 13, 2015
 *      Author: youngsun
 */

#include "KpfaPowerflow.h"

KpfaPowerflow::KpfaPowerflow(KpfaCtrlDataMgmt *pCtrlDataMgmt) {

	m_pCtrlDataMgmt = pCtrlDataMgmt;

	m_pNtrap = NULL;
}

KpfaPowerflow::~KpfaPowerflow() {

	if(m_pNtrap != NULL) {
		delete m_pNtrap;
		m_pNtrap = NULL;
	}

	if(m_pYmat != NULL) {
		delete m_pYmat;
		m_pYmat = NULL;
	}
}

/**
 * This function will perform the powerflow analysis.
 *
 * @param pRawDataMgmt raw data management
 * @return error information
 */
KpfaError_t
KpfaPowerflow::DoAnalysis(KpfaRawDataMgmt *pRawDataMgmt) {

	KpfaError_t error;

	KPFA_CHECK(pRawDataMgmt != NULL, KPFA_ERROR_INVALID_ARGUMENT);

	m_pYmat = new KpfaYMatrix();
	error = m_pYmat->BuildMatrix(pRawDataMgmt);

	if(error != KPFA_SUCCESS) {
		KPFA_ERROR("KpfaYMatrix->BuildMatrix: error - %d", error);
		return error;
	}

	// Build newton-raphson parameter
	KpfaNtrapParam_t param = {
		m_pCtrlDataMgmt->m_nMaxIteration,
		m_pCtrlDataMgmt->m_nTolerence,
		KpfaComplex_t(1.0, 0)
	};

	// Perform the Newton-Raphson method
	m_pNtrap = new KpfaNewtonRaphson(pRawDataMgmt, &param);
	error = m_pNtrap->Calculate(m_pYmat);

	if(error != KPFA_SUCCESS) {
		KPFA_ERROR("KpfaNewtonRaphson->Calculate: error - %d", error);
		return error;
	}

	// Update P, Q flow
	return UpdateBranchFlow(pRawDataMgmt);
}

/**
 * This function will perform the powerflow analysis with the given contingency data.
 *
 * @param pRawDataMgmt raw data management
 * @param pCtgDataMgmt contingency data
 * @return error information
 */
KpfaError_t
KpfaPowerflow::DoAnalysis(KpfaRawDataMgmt *pRawDataMgmt,
						  KpfaCtgData *pCtgData) {

	KpfaError_t error;

	KPFA_CHECK(pRawDataMgmt != NULL, KPFA_ERROR_INVALID_ARGUMENT);
	KPFA_CHECK(pCtgData != NULL, KPFA_ERROR_INVALID_ARGUMENT);

    error = pRawDataMgmt->ApplyContingencyData(pCtgData);
    KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Perform powerflow analysis for each system
	error = DoAnalysis(pRawDataMgmt);

	if(error != KPFA_SUCCESS) {
		KPFA_ERROR("KpfaPerformPowerflowAnalysis: error - %d", error);
		return error;
	}

    // Retrieve the current system for applying the next contingency data
    error = pRawDataMgmt->RetrieveFromContingency();
    KPFA_CHECK(error == KPFA_SUCCESS, error);

	return KPFA_SUCCESS;
}

/**
 * This function will be used to update the P, Q flow values of branches 
 * after the powerflow anlaysis.
 *
 * @param pRawDataMgmt raw data management
 * @return error information
 */
KpfaError_t 
KpfaPowerflow::UpdateBranchFlow(KpfaRawDataMgmt *pRawDataMgmt) {

	KPFA_CHECK(pRawDataMgmt != NULL, KPFA_ERROR_INVALID_ARGUMENT);

	double sysbase = pRawDataMgmt->m_nSysBase;

	KpfaComplexVector_t &vmat = m_pNtrap->GetVMatrix();
	KpfaComplexMatrix_t &ymat = m_pYmat->GetPolarMatrix();
	
	KpfaRawDataList_t::iterator iter;
	KpfaRawDataList_t &branchList = pRawDataMgmt->GetBranchDataList();

	for(iter = branchList.begin(); iter != branchList.end(); iter++) {

		KpfaBranchData *branchData = (KpfaBranchData *)*iter;

		uint32_t i = pRawDataMgmt->GetBusIndex(branchData->m_nI);
		uint32_t j = pRawDataMgmt->GetBusIndex(branchData->m_nJ);

		KpfaComplex_t v_i = vmat(i);
		KpfaComplex_t v_j = vmat(j);
		KpfaComplex_t y_ij = ymat(i, j);

		double mag = v_i.real() * y_ij.real() * v_j.real();
		double ang1 = v_i.imag() - y_ij.imag() - v_j.imag();
		double ang2 = v_j.imag() - y_ij.imag() - v_i.imag();

		double p_ij = mag * cos(ang1);
		double q_ij = mag * sin(ang1);
		double p_ji = mag * cos(ang2);
		double q_ji = mag * sin(ang2);

		// update branch data
		branchData->m_nPflow = p_ij * sysbase;
		branchData->m_nQflow = q_ij * sysbase;

		branchData->m_nPflow_JI = p_ji * sysbase;
		branchData->m_nQflow_JI = q_ji * sysbase;
	}

	return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaPowerflow::Write(ostream &rOut) {

}

ostream &operator << (ostream &rOut, KpfaPowerflow *pPowerflow) {
	pPowerflow->Write(rOut);
	return rOut;
}
