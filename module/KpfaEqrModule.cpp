/*
 * KpfaEqrModule.cpp
 *
 *  Created on: Oct 13, 2015
 *      Author: youngsun
 */

#include "KpfaUtility.h"
#include "KpfaEqrModule.h"
#ifdef KPFA_RESULT_SUPPORT
#include "KpfaInterface.h"
#include "KpfaResultData.h"
#endif

/**
 * This function will apply a stabilization algorithm to the given raw data for EQR.
 *
 * @param pRawDataMgmt raw data management to be stabilized
 * @param pCtrlDataMgmt control data management
 * @return error information
 */
KpfaError_t
KpfaEqrModule::Execute(KpfaRawDataMgmt *pRawDataMgmt, KpfaCtrlDataMgmt *pCtrlDataMgmt) {

	KpfaRawDataList_t::iterator fiter;
	KpfaRawDataList_t &factsList = pRawDataMgmt->GetFactsDataList();

	// System required EQR C, L
	m_nSysReqC = pCtrlDataMgmt->m_nEqrMarginC - m_nEqrC;
	m_nSysReqL = pCtrlDataMgmt->m_nEqrMarginL - m_nEqrL;

	// caculate the total sensitivity
	double totalSensitivity = 0;

	for(fiter = factsList.begin(); fiter != factsList.end(); fiter++) {
		KpfaFactsData *factsData = (KpfaFactsData *)*fiter;

		KpfaGenData *factsGenData = pRawDataMgmt->GetGenData(factsData->m_nI);
		KPFA_CHECK(factsGenData != NULL, KPFA_ERROR_INVALID_FACTS_DATA);

		// EQR C
		if(m_nSysReqC > 0) {
			factsData->m_nSensitivity = factsGenData->m_nWFactor[0];
		}
		// EQR L
		else if(m_nSysReqL > 0) {
			factsData->m_nSensitivity = factsGenData->m_nWFactor[1];
		}

		totalSensitivity += factsData->m_nSensitivity;
	}

	// EQR C
	if(m_nSysReqC > 0) {

		// caculate the required Q of each FACTS
		for(fiter = factsList.begin(); fiter != factsList.end(); fiter++) {
			KpfaFactsData *factsData = (KpfaFactsData *)*fiter;

			factsData->m_nRequiredQ = (factsData->m_nSensitivity / totalSensitivity) * m_nSysReqC;
		}
	}
	// EQR L
	else if(m_nSysReqL < 0) {

		// caculate the required Q of each FACTS
		for(fiter = factsList.begin(); fiter != factsList.end(); fiter++) {
			KpfaFactsData *factsData = (KpfaFactsData *)*fiter;

			factsData->m_nRequiredQ = (factsData->m_nSensitivity / totalSensitivity) * m_nSysReqL;
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will calculate EQR.
 *
 * @param pRawDataMgmt raw data management
 * @param bFirst indicates whether the EQR calculation is exeucted at first
 * @return error information
 */
KpfaError_t
KpfaEqrModule::Calculate(KpfaRawDataMgmt *pDataMgmt, bool_t bFirst) {

	KpfaError_t error;
	KPFA_CHECK(pDataMgmt != NULL, KPFA_ERROR_INVALID_ARGUMENT);

	// performs powerflow analysis
	KpfaPowerflow pfa(m_pCtrlDataMgmt);
	error = pfa.DoAnalysis(pDataMgmt);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// get Y matrix
	KpfaYMatrix *ymat = pfa.GetYMatrix();

	// build B matrix
	error = BuildBMatrix(pDataMgmt, ymat);
	KPFA_CHECK(error == KPFA_SUCCESS, KPFA_ERROR_BMATRIX_BUILD);

	// calculate WeightFactor(WF) matrix
	KpfaDoubleMatrix_t wmat;
	error = CalculateWeightFactor(wmat);
	KPFA_CHECK(error == KPFA_SUCCESS, KPFA_ERROR_WFACTOR_CALCULATE);

	// calculate CQR matrix
	KpfaDoubleMatrix_t cmat;
	error = CalculateCQR(pDataMgmt, cmat, bFirst);
	KPFA_CHECK(error == KPFA_SUCCESS, KPFA_ERROR_CQR_CALCULATE);

	// calculate EQR matrix
	error = CalculateEQR(pDataMgmt, wmat, cmat, m_rEqrMat, bFirst);
	KPFA_CHECK(error == KPFA_SUCCESS, KPFA_ERROR_EQR_CALCULATE);

	return KPFA_SUCCESS;
}

/**
 * This function will calculate CQR using generator buses.
 *
 * @param pDataMgmt raw data management for providing generator info.
 * @param rCmat output CQR matrix
 * @param bFirst indicates whether the CQR is calculated at first or not
 * @return error information
 */
KpfaError_t
KpfaEqrModule::CalculateCQR(KpfaRawDataMgmt *pDataMgmt, KpfaDoubleMatrix_t &rCmat, bool_t bFirst) {

	KPFA_CHECK(pDataMgmt != NULL, KPFA_ERROR_INVALID_ARGUMENT);

	uint32_t i = 0;
	int32_t k = pDataMgmt->GetCtgIndex();

	double cqrL = 0;
	double cqrC = 0;

	KpfaRawDataList_t::iterator iter;
	KpfaRawDataList_t &genList = pDataMgmt->GetGenDataList();

	uint32_t ngen = genList.size();

	// reset CQR matrix
	rCmat.resize(ngen, 2, false);
	rCmat.clear();

	for(iter = genList.begin(); iter != genList.end(); iter++, i++) {

		KpfaGenData *gen = (KpfaGenData *)*iter;

		if(gen->m_bStat == TRUE) {

			KpfaBusData *bus = pDataMgmt->GetBusData(gen->m_nI);
			KPFA_CHECK(bus != NULL, KPFA_ERROR_INVALID_BUS_ID);

			rCmat(i, 0) =  gen->m_nQt - bus->m_nQg;
			rCmat(i, 1) = -gen->m_nQb - bus->m_nQg;

			cqrC += rCmat(i, 0);
			cqrL += rCmat(i, 1);
		}
	}

	m_nCqrC = cqrC * pDataMgmt->m_nSysBase;
	m_nCqrL = cqrL * pDataMgmt->m_nSysBase;
	
#ifdef KPFA_RESULT_SUPPORT
	KpfaEqrList_t *eqrList = &g_pResultData->hEqrList;
	KpfaEqrCtg_t *eqrCtgList = (bFirst == TRUE) ?
		&eqrList->pPrevList[k] : &eqrList->pNextList[k];
	eqrCtgList->nEqrMarginC = m_nEqrMarginC;
	eqrCtgList->nEqrMarginL = m_nEqrMarginL;
	eqrCtgList->nCqrValueC = m_nCqrC;
	eqrCtgList->nCqrValueL = m_nCqrL;
#endif

	return KPFA_SUCCESS;
}

/**
 * This function will calculate EQR using generator buses.
 *
 * @param pDataMgmt raw data management for providing generator info.
 * @param rWmat input Wegith Factor matrix
 * @param rCmat input CQR matrix
 * @param rEmat output EQR matrix
 * @param bFirst indicates whether the CQR is calculated at first or not
 * @return error information
 */
KpfaError_t
KpfaEqrModule::CalculateEQR(KpfaRawDataMgmt *pDataMgmt,
						 	KpfaDoubleMatrix_t &rWmat,
							KpfaDoubleMatrix_t &rCmat,
							KpfaDoubleMatrix_t &rEmat,
							bool_t bFirst) {

	KPFA_CHECK(pDataMgmt != NULL, KPFA_ERROR_INVALID_ARGUMENT);

	uint32_t i, j;
	int32_t k = pDataMgmt->GetCtgIndex();

	double minEqrC, minEqrL;

	KpfaRawDataList_t::iterator riter;
	KpfaRawDataList_t::iterator citer;

	KpfaRawDataList_t &genList = pDataMgmt->GetGenDataList();
	KpfaRawDataList_t &loadList = pDataMgmt->GetLoadDataList();

	uint32_t nload = loadList.size();

	// reset EQR matrix
	rEmat.resize(nload, 2, false);
	rEmat.clear();

#ifdef KPFA_RESULT_SUPPORT
	KpfaEqrList_t *eqrList = &g_pResultData->hEqrList;
	KpfaEqrCtg_t *eqrCtgList = (bFirst == TRUE) ?
		&eqrList->pPrevList[k] : &eqrList->pNextList[k];
	eqrCtgList->pList = (KpfaEqr_t *)malloc(sizeof(KpfaEqr_t) * nload);
	eqrCtgList->nSize = nload;
#endif

	uint32_t minIdxC = 0;
	uint32_t minIdxL = 0;

	// for each load
	for(i = 0, riter = loadList.begin(); riter != loadList.end(); riter++, i++) {

		double eqrC = 0;	// t
		double eqrL = 0;	// b

	 	// for all generators
		for(j = 0, citer = genList.begin(); citer != genList.end(); citer++, j++) {

			eqrC += (rWmat(j, i) * rCmat(j, 0));
			eqrL += (rWmat(j, i) * rCmat(j, 1));
		}

		eqrC *= pDataMgmt->m_nSysBase;
		eqrL *= pDataMgmt->m_nSysBase;

#ifdef KPFA_RESULT_SUPPORT
		KpfaLoadData *loadData = (KpfaLoadData *)*riter;

		eqrCtgList->pList[i].nBusId = loadData->m_nI;
		eqrCtgList->pList[i].nEqrValueL = eqrL;
		eqrCtgList->pList[i].nEqrValueC = eqrC;
#endif

		rEmat(i, 0) = eqrC;
		rEmat(i, 1) = eqrL;

		// find minimum values of EQR (C, L)
		if(i == 0) {
			minEqrC = eqrC;
			minEqrL = eqrL;
			continue;
		}

		if(eqrC < minEqrC) {
			minEqrC = eqrC;
			minIdxC = i;
		}
		if(eqrL < minEqrL) {
			minEqrL = eqrL;
			minIdxL = i;
		}
	}

	m_nEqrC = minEqrC;
	m_nEqrL = minEqrL;

	if(bFirst == FALSE) {
		return KPFA_SUCCESS;
	}

 	// for all FACTS generators, update the value of the weight factor 
	for(j = 0, citer = genList.begin(); citer != genList.end(); citer++, j++) {

		// Keep the weight factor for calculating EQR
		KpfaGenData *genData = (KpfaGenData *)*citer;

		if(genData->m_bFacts == TRUE) {
			genData->m_nWFactor[0] = rWmat(j, minIdxC);
			genData->m_nWFactor[1] = rWmat(j, minIdxL);
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will build the Z matrix with the given Y matrix.
 *
 * @param pYmat Y matrix
 * @param rZmat return Z matrix (inverse Y matrix)
 * @return error information
 */
KpfaError_t 
KpfaEqrModule::BuildZMatrix(KpfaYMatrix *pYmat, KpfaDoubleMatrix_t &rZmat) {

	KPFA_CHECK(pYmat != NULL, KPFA_ERROR_INVALID_ARGUMENT);

	KpfaComplexMatrix_t::iterator1 riter;
	KpfaComplexMatrix_t::iterator2 citer;

	// Y matrix
	KpfaComplexMatrix_t &ymat = pYmat->GetMatrix();

	// build temporary matrix including only the imaginary values of the given Y matrix 
	KpfaDoubleMatrix_t tmp;
	tmp.resize(ymat.size1(), ymat.size2(), false);	
	tmp.clear();

	// copy only imaginary value from the Y matrix
	for(riter = ymat.begin1(); riter != ymat.end1(); riter++) {
		uint32_t i = riter.index1();
		for(citer = riter.begin(); citer != riter.end(); citer++) {
			uint32_t j = citer.index2();
			KpfaComplex_t y_ij = *citer;
			tmp(i,j) = y_ij.imag();
		}
	}
	
	return InverseMatrix(tmp, rZmat);
}

/**
 * This function will build the B matrix with the given Y matrix.
 *
 * @param pDataMgmt raw data management
 * @param pYmat Y matrix
 * @return error information
 */
KpfaError_t
KpfaEqrModule::BuildBMatrix(KpfaRawDataMgmt *pDataMgmt, KpfaYMatrix *pYmat) {

	KPFA_CHECK(pYmat != NULL, KPFA_ERROR_INVALID_ARGUMENT);
	KPFA_CHECK(pDataMgmt != NULL, KPFA_ERROR_INVALID_ARGUMENT);

	KpfaRawDataList_t::iterator riter;
	KpfaRawDataList_t::iterator citer;

	// Z matrix
	KpfaDoubleMatrix_t zmat;
	BuildZMatrix(pYmat, zmat);

	// generator, load list
	KpfaRawDataList_t &genList = pDataMgmt->GetGenDataList();
	KpfaRawDataList_t &loadList = pDataMgmt->GetLoadDataList();

	// build Bgg, Bgl, Blg, Bll matrices
	uint32_t ngen = genList.size();
	uint32_t nload = loadList.size();

	m_rBggMat.resize( ngen,  ngen, false);	m_rBggMat.clear();
	m_rBglMat.resize( ngen, nload, false);	m_rBglMat.clear();
	m_rBlgMat.resize(nload,  ngen, false);	m_rBlgMat.clear();
	m_rBllMat.resize(nload, nload, false);	m_rBllMat.clear();

	// B matrix index
	uint32_t b_k = 0, b_j = 0;

	// Bgg
	for(riter = genList.begin(), b_k = 0; riter != genList.end(); riter++, b_k++) {

		KpfaGenData *gen_k = (KpfaGenData *)*riter;

		for(citer = genList.begin(), b_j = 0; citer != genList.end(); citer++, b_j++) {

			KpfaGenData *gen_j = (KpfaGenData *)*citer;

			// get the generator k, j indices of Y matrix
			uint32_t y_k = pDataMgmt->GetBusIndex(gen_k->m_nI);
			uint32_t y_j = pDataMgmt->GetBusIndex(gen_j->m_nI);

			m_rBggMat(b_k, b_j) = zmat(y_k, y_j);
		}
	}

	// Bgl, Blg
	for(riter = genList.begin(), b_k = 0; riter != genList.end(); riter++, b_k++) {

		KpfaGenData *gen_k = (KpfaGenData *)*riter;

		for(b_j = 0, citer = loadList.begin(); citer != loadList.end(); citer++, b_j++) {

			KpfaLoadData *load_j = (KpfaLoadData *)*citer;

			// get the generator k, load j indices of Y matrix
			uint32_t y_k = pDataMgmt->GetBusIndex( gen_k->m_nI);
			uint32_t y_j = pDataMgmt->GetBusIndex(load_j->m_nI);

			m_rBglMat(b_k, b_j) = zmat(y_k, y_j);
			m_rBlgMat(b_j, b_k) = zmat(y_j, y_k);
		}
	}

#if 0
	// Blg
	for(riter = loadList.begin(), b_k = 0; riter != loadList.end(); riter++, b_k++) {

		KpfaLoadData *load_k = (KpfaLoadData *)*riter;

		for(citer = genList.begin(), b_j = 0; citer != genList.end(); citer++, b_j++) {

			KpfaGenData *gen_j = (KpfaGenData *)*citer;

			// get the load k, generator j indices of Y matrix
			uint32_t y_k = pDataMgmt->GetBusIndex(load_k->m_nI);
			uint32_t y_j = pDataMgmt->GetBusIndex( gen_j->m_nI);

			KpfaComplex_t y_kj = ymat(y_k, y_j);
			m_rBlgMat(b_k, b_j) = y_kj.imag();
		}
	}
#endif

	// Bll
	for(riter = loadList.begin(), b_k = 0; riter != loadList.end(); riter++, b_k++) {

		KpfaLoadData *load_k = (KpfaLoadData *)*riter;

		for(citer = loadList.begin(), b_j = 0; citer != loadList.end(); citer++, b_j++) {

			KpfaLoadData *load_j = (KpfaLoadData *)*citer;

			// get the load k, j indices of Y matrix
			uint32_t y_k = pDataMgmt->GetBusIndex(load_k->m_nI);
			uint32_t y_j = pDataMgmt->GetBusIndex(load_j->m_nI);

			m_rBllMat(b_k, b_j) = zmat(y_k, y_j);
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will calculate the inverse matrix of the given matrix.
 *
 * @param rMat original input matrix
 * @param rInvMat output inverse matrix
 * @return error information
 */
KpfaError_t
KpfaEqrModule::InverseMatrix(KpfaDoubleMatrix_t &rMat, KpfaDoubleMatrix_t &rInvMat) {

	uint32_t msize = rMat.size1();

	// create identity matrix of "inverse"
	rInvMat.resize(msize, msize, false);
	rInvMat.clear();

	rInvMat.assign(identity_matrix<double>(msize));

	// create a permutation matrix for the LU-factorization
	permutation_matrix<std::size_t> pmat(msize);

	// perform LU-factorization
	if(lu_factorize(rMat, pmat) != 0) {
		return KPFA_ERROR_LU_FACTORIZE;
	}

	// back-substitute to get the inverse
	lu_substitute(rMat, pmat, rInvMat);

	return KPFA_SUCCESS;
}

/**
 * This function will calculate the weight factor using the B matrix.
 *
 * @param rWmat weight factor matrix
 * @return error information
 */
KpfaError_t
KpfaEqrModule::CalculateWeightFactor(KpfaDoubleMatrix_t &rWmat) {

	KpfaError_t error;

	// temporary matrix
	KpfaDoubleMatrix_t tmp;

	// S matrix
	KpfaDoubleMatrix_t sgg;
	KpfaDoubleMatrix_t sgl;

	// inverse Sgg, Bll matrices
	KpfaDoubleMatrix_t invSgg;
	KpfaDoubleMatrix_t invBll;

	error = InverseMatrix(m_rBllMat, invBll);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// 1. build Sgg matrix:
	///////////////////////////////////////////////////////////
	// Sgg = Inv{[Bgg] - [Bgl] x Inv[Bll] x [Blg]}
	///////////////////////////////////////////////////////////

	tmp = block_prod<KpfaDoubleMatrix_t, 64>(m_rBglMat, invBll);
	tmp = block_prod<KpfaDoubleMatrix_t, 64>(tmp, m_rBlgMat);
	invSgg = m_rBggMat - tmp;

	error = InverseMatrix(invSgg, sgg);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// 2. build Sgl matrix (transpose of Slg matrix):
	///////////////////////////////////////////////////////////
	// Sgl = Transpose{Inv[Bll] x [Blg] x [Sgg]}
	///////////////////////////////////////////////////////////
	tmp = block_prod<KpfaDoubleMatrix_t, 64>(invBll, m_rBlgMat);
	tmp = block_prod<KpfaDoubleMatrix_t, 64>(tmp, sgg);

	sgl = trans(tmp);

	// 3. calculate w-factor matrix:
	///////////////////////////////////////////////////////////
	// w-factor = Inv[Sgg] x Sgl
	///////////////////////////////////////////////////////////
	rWmat = block_prod<KpfaDoubleMatrix_t, 64>(invSgg, sgl);


	// 4. normalize w-factor matrix:
	///////////////////////////////////////////////////////////
	// w-factor /= max value
	///////////////////////////////////////////////////////////
	int i;

	KpfaDoubleMatrix_t::iterator1 riter;
	KpfaDoubleMatrix_t::iterator2 citer;

	double *normArray = new double[rWmat.size1()];

	for(i = 0, riter = rWmat.begin1(); riter != rWmat.end1(); riter++) {
		double norm = 0;
		for(citer = riter.begin(); citer != riter.end(); citer++) {
			if(norm < *citer) {
				norm = *citer;
			}
		}
		normArray[i++] = norm;
	}

	for(i = 0, riter = rWmat.begin1(); riter != rWmat.end1(); riter++) {
		for(citer = riter.begin(); citer != riter.end(); citer++) {
			*citer /= normArray[i];
		}
		i++;
	}

	return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaEqrModule::Write(ostream &rOut) {
    // do nothing
}

ostream &operator << (ostream &rOut, KpfaEqrModule *pModule) {
	pModule->Write(rOut);
	return rOut;
}
