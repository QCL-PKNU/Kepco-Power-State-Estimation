/*
 * KpfaResult.cpp
 *
 *  Created on: Oct 19, 2015
 *      Author: youngsun
 */

#include "KpfaResultData.h"

////////////////////////////////////////////////////////
// Global handle for the result data
////////////////////////////////////////////////////////

KpfaResultData_t *g_pResultData = NULL;

////////////////////////////////////////////////////////
// Function definition
////////////////////////////////////////////////////////

/**
 * Gathers the data of FACTS.
 */

KpfaError_t
KpfaGatherFactsData(KpfaRawDataMgmt *pRawDataMgmt, int nCtgIndex) {

	if(g_pResultData == NULL) {
		return KPFA_ERROR_INVALID_RESULT;
	}

	KpfaFactsCtg_t *factsCtg = &g_pResultData->hFactsList.pList[nCtgIndex];

	int factsIndex = 0;

	KpfaRawDataList_t::iterator iter;
	KpfaRawDataList_t &genList = pRawDataMgmt->GetGenDataList();

	for(iter = genList.begin(); iter != genList.end(); iter++) {

		KpfaGenData *gen = (KpfaGenData *)*iter;

		if(gen->m_bFacts == TRUE) {

			KpfaFacts_t *facts = &factsCtg->pNextList[factsIndex++];

			facts->nBusId = gen->m_nI;
			facts->nQGen = (float)gen->m_nQg;
			facts->nVoltage = (float)gen->m_nVs;
		}
	}

	return KPFA_SUCCESS;
}

/**
 * Gathers the data of BUS, BRANCH, GENERATOR, OPF.
 */
KpfaError_t
KpfaGatherResultData(KpfaRawDataMgmt *pRawDataMgmt, KpfaPowerflow *pPowerflow, int nCtgIndex, int nPfIndex) {

	int i;

	if(g_pResultData == NULL) {
		return KPFA_ERROR_INVALID_RESULT;
	}

	// POWERFLOW
	KpfaRawDataList_t::iterator iter;

	// 1) BUS
	KpfaRawDataList_t &busDataList = pRawDataMgmt->GetBusDataList();
	KpfaBusCtg_t *busList = &g_pResultData->hPowerflow.hBusList[nPfIndex].pList[nCtgIndex];

	busList->nSize = busDataList.size();
	busList->pList = (KpfaBus_t *)malloc(sizeof(KpfaBus_t) * busList->nSize);

	KPFA_CHECK(busList->pList != NULL, KPFA_ERROR_MEMORY_ALLOC);

	KpfaComplexVector_t &vmat = pPowerflow->GetVMatrix();

	for(i = 0, iter = busDataList.begin(); iter != busDataList.end(); iter++) {

		KpfaBusData *bus = (KpfaBusData *)*iter;

		if(bus->m_nIde == KPFA_ISOLATED_BUS) {
			continue;
		}

		busList->pList[i].nBusId = bus->m_nI;

		if(nPfIndex == 0) {
			busList->pList[i].nVoltage = (float)bus->m_nVm;
		}
		else 
		{
			KpfaComplex_t tmp = vmat(i);
			busList->pList[i].nVoltage = (float)tmp.real();
		}

		i++;
	}

	// 2) BRANCH
	KpfaRawDataList_t &branchDataList = pRawDataMgmt->GetBranchDataList();
	KpfaBranchCtg_t *branchList = &g_pResultData->hPowerflow.hBranchList[nPfIndex].pList[nCtgIndex];

	branchList->nSize = branchDataList.size();
	branchList->pList = (KpfaBranch_t *)malloc(sizeof(KpfaBranch_t) * branchList->nSize);

	KPFA_CHECK(branchList->pList != NULL, KPFA_ERROR_MEMORY_ALLOC);

	for(i = 0, iter = branchDataList.begin(); iter != branchDataList.end(); iter++, i++) {

		KpfaBranchData *branch = (KpfaBranchData *)*iter;

		branchList->pList[i].nCkt = branch->m_nCkt;
		branchList->pList[i].nStartId = branch->m_nI;
		branchList->pList[i].nEndId = branch->m_nJ;
		branchList->pList[i].nFlowValueP = (float)branch->m_nPflow;
		branchList->pList[i].nFlowValueQ1 = (float)branch->m_nQflow;
		branchList->pList[i].nFlowValueQ2 = (float)branch->m_nQflow_JI;
	}

	// 3) GENERATOR
	KpfaRawDataList_t &genDataList = pRawDataMgmt->GetGenDataList();
	KpfaGeneratorCtg_t *genList = &g_pResultData->hPowerflow.hGeneratorList[nPfIndex].pList[nCtgIndex];

	genList->nSize = genDataList.size();
	genList->pList = (KpfaGenerator_t *)malloc(sizeof(KpfaGenerator_t) * genList->nSize);

	KPFA_CHECK(genList->pList != NULL, KPFA_ERROR_MEMORY_ALLOC);

	for(i = 0, iter = genDataList.begin(); iter != genDataList.end(); iter++, i++) {

		KpfaGenData* gen = (KpfaGenData *)*iter;
		KpfaBusData* bus = pRawDataMgmt->GetBusData(gen->m_nI);

		genList->pList[i].nBusId = gen->m_nI;
		genList->pList[i].nQGen = (float)bus->m_nQg;
	}

	return KPFA_SUCCESS;
}

/**
 * Allocates the memory of the Facts data.
 */
KpfaError_t
KpfaAllocResultData(KpfaRawDataMgmt *pRawDataMgmt,
					KpfaCtgDataMgmt *pCtgDataMgmt) {

	if(g_pResultData == NULL) {
		return KPFA_ERROR_INVALID_RESULT;
	}

	uint32_t i = 0;
	uint32_t len = 0;
	uint32_t nctg = pCtgDataMgmt->GetCtgDataList().size();
	uint32_t nfacts = pRawDataMgmt->GetFactsDataList().size();

	//EQR
	KpfaEqrList_t *eqrList = &g_pResultData->hEqrList;

	if(eqrList->pPrevList != NULL) delete eqrList->pPrevList;
	if(eqrList->pNextList != NULL) delete eqrList->pNextList;

	eqrList->nSize = nctg;
	len = sizeof(KpfaEqrCtg_t) * nctg;
	eqrList->pPrevList = (KpfaEqrCtg_t *)malloc(len);
	eqrList->pNextList = (KpfaEqrCtg_t *)malloc(len);

	if(eqrList->pPrevList == NULL || eqrList->pNextList == NULL) {
		return KPFA_ERROR_MEMORY_ALLOC;
	}

	memset(eqrList->pPrevList, 0, len);
	memset(eqrList->pNextList, 0, len);

	// GV
	KpfaGvList_t *gvList = &g_pResultData->hGvList;

	if(gvList->pList != NULL) {
		delete gvList->pList;
	}

	gvList->nSize = nctg;
	len = sizeof(KpfaGvCtg_t) * nctg;
	gvList->pList = (KpfaGvCtg_t *)malloc(len);

	if(gvList->pList == NULL) {
		return KPFA_ERROR_MEMORY_ALLOC;
	}

	memset(gvList->pList, 0, len);

	// FACTS
	KpfaFactsList_t *factsList = &g_pResultData->hFactsList;

	if(factsList->pList != NULL) {
		delete factsList->pList;
	}

	factsList->nSize = nctg;
	factsList->pList = (KpfaFactsCtg_t *)malloc(sizeof(KpfaFactsCtg_t) * nctg);

	if(factsList->pList == NULL) {
		return KPFA_ERROR_MEMORY_ALLOC;
	}

	for(i = 0; i < nctg; i++) {

		int factsIdx = 0;
		KpfaFactsCtg_t *factsCtg = &factsList->pList[i];

		factsCtg->pPrevList = (KpfaFacts_t *)malloc(sizeof(KpfaFacts_t) * nfacts);
		factsCtg->pNextList = (KpfaFacts_t *)malloc(sizeof(KpfaFacts_t) * nfacts);
		factsCtg->nStatus = KPFA_STATUS_0;

		if(factsCtg->pPrevList == NULL || factsCtg->pNextList == NULL) {
			return KPFA_ERROR_MEMORY_ALLOC;
		}

		factsCtg->nSize = nfacts;

		KpfaRawDataList_t::iterator riter;
		KpfaRawDataList_t &genList = pRawDataMgmt->GetGenDataList();

		for(riter = genList.begin(); riter != genList.end(); riter++) {

			KpfaGenData *gen = (KpfaGenData *)*riter;

			if(gen->m_bFacts == FALSE) {
				continue;
			}

			KpfaFacts_t *facts = &factsCtg->pPrevList[factsIdx++];

			facts->nBusId = gen->m_nI;
			facts->nQGen = (float)gen->m_nQg;
			facts->nVoltage = (float)gen->m_nVs;
		}
	}

	for(int k = 0; k < 2; k++) {

		// POWER FLOW (BUS, BRANCH, GENERATOR)
		KpfaBusList_t *busList = &g_pResultData->hPowerflow.hBusList[k];
		busList->pList = (KpfaBusCtg_t *)malloc(sizeof(KpfaBusCtg_t) * nctg);
		busList->nSize = nctg;

		KpfaBranchList_t *branchList = &g_pResultData->hPowerflow.hBranchList[k];
		branchList->pList = (KpfaBranchCtg_t *)malloc(sizeof(KpfaBranchCtg_t) * nctg);
		branchList->nSize = nctg;

		KpfaGeneratorList_t *genList = &g_pResultData->hPowerflow.hGeneratorList[k];
		genList->pList = (KpfaGeneratorCtg_t *)malloc(sizeof(KpfaGeneratorCtg_t) * nctg);
		genList->nSize = nctg;

		KpfaCtgDataList_t::iterator citer;
		KpfaCtgDataList_t &ctgList = pCtgDataMgmt->GetCtgDataList();

		char ctgName[32];

		for(i = 0, citer = ctgList.begin(); citer != ctgList.end(); citer++, i++) {

			KpfaCtgData *ctg = *citer;

			strcpy(ctgName, ctg->GetName().c_str());

			if(k == 0) {
				strcpy(eqrList->pPrevList[i].rName, ctgName);
				strcpy(eqrList->pNextList[i].rName, ctgName);
				strcpy(factsList->pList[i].rName, ctgName);
				strcpy(gvList->pList[i].rName, ctgName);
			}

			strcpy(busList->pList[i].rName, ctgName);
			strcpy(branchList->pList[i].rName, ctgName);
			strcpy(genList->pList[i].rName, ctgName);
		}
	}

	return KPFA_SUCCESS;
}
