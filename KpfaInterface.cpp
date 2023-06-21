/*
 * KpfaInterface.cpp
 *
 *  Created on: Oct 28, 2015
 *      Author: youngsun
 */

#include "KpfaInterface.h"
#include "KpfaPowerflow.h"
#include "KpfaRawDataReader.h"

#include "KpfaGvModule.h"
#include "KpfaEqrModule.h"
#include "KpfaFactsModule.h"

#ifdef KPFA_RESULT_SUPPORT
#include "KpfaResultData.h"
#endif

static KpfaError_t CheckValidity(KpfaRawDataReader *pRawDataReader,
				  	  	  	  	 KpfaCtrlDataMgmt *pCtrlDataMgmt,
								 uint32_t *pnErrorIndex);

static KpfaError_t AnalyzeStability(KpfaRawDataMgmt *pRawDataMgmt,
				     	 	 	 	KpfaCtrlDataMgmt *pCtrlDataMgmt,
									KpfaCtgData *pCtgData);

/**
 * This function will be used to check the validity of the given raw data.
 *
 * @param pRawDataReader raw data reader
 * @param pCtrlDataMgmt control data management
 * @param pnErrorIndex the variable to return the error index
 * @return error information
 */
static KpfaError_t
CheckValidity(KpfaRawDataReader *pRawDataReader,
			  KpfaCtrlDataMgmt *pCtrlDataMgmt,
			  uint32_t *pnErrorIndex) {

	KPFA_CHECK(pRawDataReader != NULL, KPFA_ERROR_INVALID_ARGUMENT);
	KPFA_CHECK(pCtrlDataMgmt != NULL, KPFA_ERROR_INVALID_ARGUMENT);
	KPFA_CHECK(pnErrorIndex != NULL, KPFA_ERROR_INVALID_ARGUMENT);

	uint32_t i = 0;

	KpfaError_t error;

	// Powerflow analysis
	KpfaPowerflow pfa(pCtrlDataMgmt);

    // Raw data management list
	KpfaRawDataMgmtList_t::iterator iter;
	KpfaRawDataMgmtList_t &dataMgmtList = pRawDataReader->GetRawDataMgmts();

	// For each power system with connected buses
	for(iter = dataMgmtList.begin(); iter != dataMgmtList.end(); iter++, i++) {

		// Perform powerflow analysis for each system
		error = pfa.DoAnalysis(*iter);

		if(error != KPFA_SUCCESS) {
			*pnErrorIndex = i;
			return error;
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will analyze the stability of the given raw data in the contingency.
 *
 * @param pRawDataReader raw data reader
 * @param pCtrlDataMgmt control data management
 * @param pCtgData contingency data
 * @return error information
 */
static KpfaError_t
AnalyzeStability(KpfaRawDataMgmt *pRawDataMgmt,
				 KpfaCtrlDataMgmt *pCtrlDataMgmt,
				 KpfaCtgData *pCtgData) {

#ifdef KPFA_RESULT_SUPPORT
	int pfIndex = 0;

	KPFA_CHECK(g_pResultData != NULL, KPFA_ERROR_INVALID_RESULT);
#endif
	KPFA_CHECK(pRawDataMgmt != NULL, KPFA_ERROR_INVALID_ARGUMENT);
	KPFA_CHECK(pCtrlDataMgmt != NULL, KPFA_ERROR_INVALID_ARGUMENT);
	KPFA_CHECK(pCtgData != NULL, KPFA_ERROR_INVALID_ARGUMENT);

	KpfaError_t error;

	uint32_t i = pCtgData->GetIndex();

	// Powerflow analysis
	KpfaPowerflow pfa(pCtrlDataMgmt);

	// GV, EQR, FACTS module
	KpfaGvModule gv(pCtrlDataMgmt);
	KpfaEqrModule eqr(pCtrlDataMgmt);
	KpfaFactsModule facts(pCtrlDataMgmt);

#ifdef KPFA_RESULT_SUPPORT
	error = KpfaGatherResultData(pRawDataMgmt, &pfa, i, pfIndex++);
	KPFA_CHECK(error == KPFA_SUCCESS, error);
#endif

	// Powerflow analysis with contingency
	error = pfa.DoAnalysis(pRawDataMgmt, pCtgData);

	if(true /*error == KPFA_ERROR_NOT_CONVERGED*/) {

		// Perform GV module
		error = gv.Execute(pRawDataMgmt, pCtgData);
		KPFA_CHECK(error == KPFA_SUCCESS, error);

		double margin0 = gv.GetMaxMargin();

		if(margin0 < 1.0) {

#ifdef KPFA_RESULT_SUPPORT
			g_pResultData->hFactsList.pList[i].nStatus = KPFA_STATUS_1;
#endif

KPFA_REPEAT_GV:
			// capacitive
			if(facts.Execute(pRawDataMgmt, FALSE) == FALSE) {
				goto KPFA_STOP_REPEAT_GV;
			}

			// Perform GV module
			error = gv.Execute(pRawDataMgmt, pCtgData);
			KPFA_CHECK(error == KPFA_SUCCESS, error);

			if(gv.GetMaxMargin() < 1.0) {
				goto KPFA_REPEAT_GV;
			}
KPFA_STOP_REPEAT_GV:

#ifdef KPFA_RESULT_SUPPORT
			error = KpfaGatherResultData(pRawDataMgmt, &pfa, i, pfIndex);
			KPFA_CHECK(error == KPFA_SUCCESS, error);

			error = KpfaGatherFactsData(pRawDataMgmt, i);
			KPFA_CHECK(error == KPFA_SUCCESS, error);
#endif
			return KPFA_SUCCESS;
		}
	}
	else KPFA_CHECK(error == KPFA_SUCCESS, error);

#if 1
#ifdef KPFA_RESULT_SUPPORT
		error = KpfaGatherResultData(pRawDataMgmt, &pfa, i, pfIndex);
		KPFA_CHECK(error == KPFA_SUCCESS, error);

		error = KpfaGatherFactsData(pRawDataMgmt, i);
		KPFA_CHECK(error == KPFA_SUCCESS, error);
#endif
#else
	// Perform powerflow analysis for each system
	error = pRawDataMgmt->ApplyContingencyData(pCtgData);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

    // Call the EQR module to check whether the EQR value is greater than the given margin or not.
	error = eqr.Calculate(pRawDataMgmt);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

    // If the EQR value is greater than the margin, call the FACTS module finally.
    // Otherwise, call the OPT module optionally.
	double eqrMarginC = pCtrlDataMgmt->m_nEqrMarginC;
	double eqrMarginL = pCtrlDataMgmt->m_nEqrMarginL;
	
	if(eqr.GetEqrC() < eqrMarginC && eqr.GetEqrL() > eqrMarginL) {
		pRawDataMgmt->RetrieveFromContingency();
		return KPFA_ERROR_EQR_INVALID_MARGIN;
	}
	else if(eqr.GetEqrC() < eqrMarginC || eqr.GetEqrL() > eqrMarginL) {

#ifdef KPFA_RESULT_SUPPORT
		g_pResultData->hFactsList.pList[i].nStatus = KPFA_STATUS_2;
#endif
		error = eqr.Execute(pRawDataMgmt, pCtrlDataMgmt);
		KPFA_CHECK(error == KPFA_SUCCESS, error);

		error = facts.Execute(pRawDataMgmt);
		KPFA_CHECK(error == KPFA_SUCCESS, error);

		error = eqr.Calculate(pRawDataMgmt, FALSE);
		KPFA_CHECK(error == KPFA_SUCCESS, error);

#ifdef KPFA_RESULT_SUPPORT
		error = KpfaGatherResultData(pRawDataMgmt, &pfa, i, pfIndex);
		KPFA_CHECK(error == KPFA_SUCCESS, error);

		error = KpfaGatherFactsData(pRawDataMgmt, i);
		KPFA_CHECK(error == KPFA_SUCCESS, error);
#endif
		return pRawDataMgmt->RetrieveFromContingency();
	}
#endif

#if 0
	KpfaRawDataList_t::iterator biter;
	KpfaRawDataList_t &busList = pRawDataMgmt->GetBusDataList();

	for(biter = busList.begin(); biter != busList.end(); biter++) {

		KpfaBusData *bus = (KpfaBusData *)*biter;

		// Powerflow inductive
		if(bus->m_nVm > 1.05f) {

KDA_REPEAT_PF_IND:
			// inductive
			if(facts.Execute(pRawDataMgmt, FALSE) != FALSE) {

				error = pfa.DoAnalysis(pRawDataMgmt);
				KPFA_CHECK(error == KPFA_SUCCESS, error);

				if(bus->m_nVm > 1.05f) {
					goto KDA_REPEAT_PF_IND;
				}
			}

#ifdef KPFA_RESULT_SUPPORT
			error = KpfaGatherResultData(pRawDataMgmt, &pfa, i, pfIndex);
			KPFA_CHECK(error == KPFA_SUCCESS, error);

			error = KpfaGatherFactsData(pRawDataMgmt, i);
			KPFA_CHECK(error == KPFA_SUCCESS, error);
#endif
		}
		// Powerflow capacitive
		else if(bus->m_nVm < 0.95f) {

KDA_REPEAT_PF_CAP:
			// capacitive
			if(facts.Execute(pRawDataMgmt, FALSE) != FALSE) {

				error = pfa.DoAnalysis(pRawDataMgmt);
				KPFA_CHECK(error == KPFA_SUCCESS, error);

				if(bus->m_nVm < 0.95f) {
					goto KDA_REPEAT_PF_CAP;
				}
			}

#ifdef KPFA_RESULT_SUPPORT
			error = KpfaGatherResultData(pRawDataMgmt, &pfa, i, pfIndex);
			KPFA_CHECK(error == KPFA_SUCCESS, error);

			error = KpfaGatherFactsData(pRawDataMgmt, i);
			KPFA_CHECK(error == KPFA_SUCCESS, error);
#endif
		}
	}
#endif

	return KPFA_SUCCESS;
}

//////////////////////////////////////////////////////////////////////
// Interface functions for stability analysis
//////////////////////////////////////////////////////////////////////

int 
KpfaDoAnalysis(KpfaInputParam_t *pParam, KpfaResultData_t *pResultData) {

	KpfaError_t error;

	KPFA_CHECK(pParam != NULL, -1);

#ifdef KPFA_RESULT_SUPPORT
    KPFA_CHECK(pResultData != NULL, -2);

	g_pResultData = pResultData;
#endif

    // Contingency data file read
    KpfaCtgDataMgmt *ctgDataMgmt = new KpfaCtgDataMgmt();
    error = ctgDataMgmt->ReadCtgDataFile(pParam->pCtgFilePath);
    KPFA_CHECK(error == KPFA_SUCCESS, -3);

    // Control data file read
    KpfaCtrlDataMgmt *ctrlDataMgmt = new KpfaCtrlDataMgmt();
    error = ctrlDataMgmt->ReadCtrlDataFile(pParam->pCtrlFilePath);
    KPFA_CHECK(error == KPFA_SUCCESS, -4);

	// Raw(network) data file read
	KpfaRawDataReader *rawDataReader = new KpfaRawDataReader();
	error = rawDataReader->ReadRawDataFile(pParam->pRawFilePath);
    KPFA_CHECK(error == KPFA_SUCCESS, -5);

	uint32_t errorIndex = 0;

	error = CheckValidity(rawDataReader, ctrlDataMgmt, &errorIndex);

	if(error != KPFA_SUCCESS) {
		KPFA_ERROR("KpfaCheckStability: cluster(%d) error - %d", errorIndex, error);
		return -6;
	}

	KPFA_DEBUG("main", ">> Stability Check Completed.");

    // Contingency data list
    KpfaCtgDataList_t::iterator citer;
    KpfaCtgDataList_t &ctgDataList = ctgDataMgmt->GetCtgDataList();

    // Raw data management list
	KpfaRawDataMgmtList_t &rawDataMgmtList = rawDataReader->GetRawDataMgmts();
	KPFA_CHECK(rawDataMgmtList.size() > 0, -7);

	// 1st raw data management
    KpfaRawDataMgmt *rawDataMgmt = rawDataMgmtList[0];

#ifdef KPFA_RESULT_SUPPORT
	// ResultData Initialization
	error = KpfaAllocResultData(rawDataMgmt, ctgDataMgmt);
	KPFA_CHECK(error == KPFA_SUCCESS, -8);
#endif

    // For each contingency
	for(citer = ctgDataList.begin(); citer != ctgDataList.end(); citer++) {

		KpfaCtgData *ctgData = *citer;

		cout << ctgData << endl;

		error = AnalyzeStability(rawDataMgmt, ctrlDataMgmt, ctgData);
		KPFA_CHECK(error == KPFA_SUCCESS, -9);
	}

	return 0;
}

void
KpfaReleaseResult(KpfaResultData_t *pResultData) {

	int i;

	if(pResultData == NULL) {
		return;
	}

	// FACTS
	///////////////////////////////////////////////////////////
	KpfaFactsList_t *factsList = &pResultData->hFactsList;

	KpfaFactsCtg_t *factsCtgList = factsList->pList;

	for(i = 0; i < factsList->nSize; i++) {

		delete factsCtgList[i].pPrevList;
		delete factsCtgList[i].pNextList;
	}

	delete factsCtgList;

	factsList->nSize = 0;

	// Final Operating Point
	///////////////////////////////////////////////////////////
	KpfaFactsCtg_t *finalOpPoint = pResultData->pFinalOpFacts;

	if(finalOpPoint) {
		delete finalOpPoint->pPrevList;
		delete finalOpPoint->pNextList;
		delete finalOpPoint;
	}

	pResultData->pFinalOpFacts = NULL;

	// GV
	///////////////////////////////////////////////////////////
	KpfaGvList_t *gvList = &pResultData->hGvList;

	KpfaGvCtg_t *gvCtgList = gvList->pList;

	for(i = 0; i < gvList->nSize; i++) {

		KpfaGv_t *tmp = gvCtgList[i].pList;

		if(tmp != NULL) {
			delete gvCtgList[i].pList;
		}
	}

	delete gvCtgList;

	gvList->nSize = 0;

#if 0
	// EQR
	///////////////////////////////////////////////////////////
	KpfaEqrList_t *eqrList = &pResultData->hEqrList;

	KpfaEqrCtg_t *eqrCtgList = eqrList->pPrevList;

	for(i = 0; i < eqrList->nSize; i++) {

		KpfaEqr_t *tmp = eqrCtgList[i].pList;

		if(tmp != NULL) {
			delete eqrCtgList[i].pList;
		}
	}

	delete eqrCtgList;

	eqrCtgList = eqrList->pNextList;

	for(i = 0; i < eqrList->nSize; i++) {

		KpfaEqr_t *tmp = eqrCtgList[i].pList;

		if(tmp != NULL) {
			delete eqrCtgList[i].pList;
		}
	}

	delete eqrCtgList;

	eqrList->nSize = 0;

	// POWERFLOW
	///////////////////////////////////////////////////////////
	for(int k = 0; k < 2; k++) {

		// BUS ------------------------------------------------
		KpfaBusList_t *busList = &pResultData->hPowerflow.hBusList[k];

		KpfaBusCtg_t *busCtgList = busList->pList;

		for(i = 0; i < busList->nSize; i++) {

			delete busCtgList[i].pList;
		}

		delete busCtgList;

		busList->nSize = 0;

		// BRANCH ------------------------------------------------
		KpfaBranchList_t *branchList = &pResultData->hPowerflow.hBranchList[k];

		KpfaBranchCtg_t *branchCtgList = branchList->pList;

		for(i = 0; i < branchList->nSize; i++) {

			delete branchCtgList[i].pList;
		}

		delete branchCtgList;

		branchList->nSize = 0;

		// GENERATOR ------------------------------------------------
		KpfaGeneratorList_t *genList = &pResultData->hPowerflow.hGeneratorList[k];

		KpfaGeneratorCtg_t *genCtgList = genList->pList;

		for(i = 0; i < genList->nSize; i++) {

			delete genCtgList[i].pList;
		}

		delete genCtgList;

		genList->nSize = 0;
	}
#endif
}


void
KpfaPrintResult(char *pFilePath, KpfaResultData_t *pResultData) {

	int i, j;

	FILE *fp = NULL;

	if(pFilePath != NULL) {
		fp = fopen(pFilePath, "w+");
	}

	if(fp == NULL) {
		fp = stdout;
	}

	if(g_pResultData == NULL) {
		return;
	}

	// FACTS
	KpfaFactsList_t *factsList = &pResultData->hFactsList;

	fprintf(fp, ">> 1. FACTS ============\n");

	for(i = 0; i < factsList->nSize; i++) {

		KpfaFactsCtg_t *factsCtgList = &factsList->pList[i];

		fprintf(fp, "facts ctg[id - %d, status - %d, name - %s] ===\n", i, factsCtgList->nStatus, factsCtgList->rName);

		for(j = 0; j < factsCtgList->nSize; j++) {

			fprintf(fp, "\tBus ID: %d -> %d\n", factsCtgList->pPrevList[j].nBusId, factsCtgList->pNextList[j].nBusId);
			fprintf(fp, "\tVoltage: %6.5f -> %6.5f\n", factsCtgList->pPrevList[j].nVoltage, factsCtgList->pNextList[j].nVoltage);
			fprintf(fp, "\tQGen: %6.5f -> %6.5f\n\n", factsCtgList->pPrevList[j].nQGen, factsCtgList->pNextList[j].nQGen);
		}
	}

	fprintf(fp, ">> Final Operating Point ============\n");

	KpfaFactsCtg_t *finalOpPoint = pResultData->pFinalOpFacts;

	if(finalOpPoint) {

		fprintf(fp, "facts ctg[status - %d, name - %s] ===\n", finalOpPoint->nStatus, finalOpPoint->rName);

		for(int j = 0; j < finalOpPoint->nSize; j++) {

			fprintf(fp, "\tBus ID: %d -> %d\n", finalOpPoint->pPrevList[j].nBusId, finalOpPoint->pNextList[j].nBusId);
			fprintf(fp, "\tVoltage: %6.5f -> %6.5f\n", finalOpPoint->pPrevList[j].nVoltage, finalOpPoint->pNextList[j].nVoltage);
			fprintf(fp, "\tQGen: %6.5f -> %6.5f\n\n", finalOpPoint->pPrevList[j].nQGen, finalOpPoint->pNextList[j].nQGen);
		}
	}

	fprintf(fp, ">> 2. GV ============\n");

	KpfaGvList_t *gvList = &pResultData->hGvList;

	for(i = 0; i < gvList->nSize; i++) {

		KpfaGvCtg_t *gvCtgList = &gvList->pList[i];

		fprintf(fp, "gv ctg[name - %s] ===\n", gvCtgList->rName);
		fprintf(fp, "gv Margin - %6.5f\n", gvCtgList->nMargin);

		for(int j = 0; j < gvCtgList->nSize; j++) {
			fprintf(fp, "gv[%d]: GenParam - %6.5f, Voltage - %6.5f\n", j,
					gvCtgList->pList[j].nGenParam,
					gvCtgList->pList[j].nVoltage);
		}
	}

	fprintf(fp, ">> 3. EQR ============\n");

	KpfaEqrList_t *eqrList = &pResultData->hEqrList;

	fprintf(fp, ">> Prev ==========================\n");
	
	for(i = 0; i < eqrList->nSize; i++) {

		KpfaEqrCtg_t *eqrCtgList = &eqrList->pPrevList[i];

		fprintf(fp, "eqr ctg[name - %s] ===\n", eqrCtgList->rName);
		fprintf(fp, "eqr margin L - %6.5f\n", eqrCtgList->nEqrMarginL);
		fprintf(fp, "eqr margin C - %6.5f\n", eqrCtgList->nEqrMarginC);
		fprintf(fp, "cqr value L - %6.5f\n", eqrCtgList->nCqrValueL);
		fprintf(fp, "cqr value C - %6.5f\n", eqrCtgList->nCqrValueC);

		for(int j = 0; j < eqrCtgList->nSize; j++) {
			fprintf(fp, "eqr[%d]: Bus ID - %d, EqrValue L - %6.5f, EqrValue C - %6.5f\n", j,
					eqrCtgList->pList[j].nBusId,
					eqrCtgList->pList[j].nEqrValueL, 
					eqrCtgList->pList[j].nEqrValueC);
		}
	}

	fprintf(fp, ">> Next ==========================\n");

	for(i = 0; i < eqrList->nSize; i++) {

		KpfaEqrCtg_t *eqrCtgList = &eqrList->pNextList[i];

		fprintf(fp, "eqr ctg[name - %s] ===\n", eqrCtgList->rName);
		fprintf(fp, "eqr margin L - %6.5f\n", eqrCtgList->nEqrMarginL);
		fprintf(fp, "eqr margin C - %6.5f\n", eqrCtgList->nEqrMarginC);
		fprintf(fp, "cqr value L - %6.5f\n", eqrCtgList->nCqrValueL);
		fprintf(fp, "cqr value C - %6.5f\n", eqrCtgList->nCqrValueC);

		for(int j = 0; j < eqrCtgList->nSize; j++) {
			fprintf(fp, "eqr[%d]: Bus ID - %d, EqrValue L - %6.5f, EqrValue C - %6.5f\n", j,
					eqrCtgList->pList[j].nBusId,
					eqrCtgList->pList[j].nEqrValueL, 
					eqrCtgList->pList[j].nEqrValueC);
		}
	}

	fprintf(fp, ">> 4. OPF ============\n");

	KpfaOpf_t *opf = &pResultData->hOpf;

	fprintf(fp, "opf: Before Loss - %6.5f, After Loss - %6.5f\n", opf->nLossBefore, opf->nLossAfter);

	for(int k = 0; k < 2; k++) {

		if(k == 0) 	fprintf(fp, ">> Prev ==========================\n");
		else        fprintf(fp, ">> Next ==========================\n");

		fprintf(fp, ">> 5. Powerflow - BUS ============\n");

		KpfaBusList_t *busList = &pResultData->hPowerflow.hBusList[k];

		for(i = 0; i < busList->nSize; i++) {

			KpfaBusCtg_t *busCtg = &busList->pList[i];
			fprintf(fp, "bus ctg[name - %s] ===\n", busCtg->rName);

			for(j = 0; j < busCtg->nSize; j++) {

				fprintf(fp, "bus[%d]: Bus ID - %d, Voltage - %6.5f\n", j,
							busCtg->pList[j].nBusId,
							busCtg->pList[j].nVoltage);
			}
		}

		fprintf(fp, ">> 5. Powerflow - BRANCH ============\n");

		KpfaBranchList_t *branchList = &pResultData->hPowerflow.hBranchList[k];

		for(i = 0; i < branchList->nSize; i++) {

			KpfaBranchCtg_t *branchCtg = &branchList->pList[i];
			fprintf(fp, "branch ctg[name - %s] ===\n", branchCtg->rName);

			for(j = 0; j < branchCtg->nSize; j++) {

				fprintf(fp, "branch[%d]: Bus ID0 - %d, Bus ID1: %d, Branch Ckt: %d, P: %6.5f, Q1: %6.5f, Q2: %6.5f\n", j,
							branchCtg->pList[j].nStartId,
							branchCtg->pList[j].nEndId,
							branchCtg->pList[j].nCkt,
							branchCtg->pList[j].nFlowValueP,
							branchCtg->pList[j].nFlowValueQ1,
							branchCtg->pList[j].nFlowValueQ2);
			}
		}

		fprintf(fp, ">> 5. Powerflow - GENERATOR ============\n");

		KpfaGeneratorList_t *genList = &pResultData->hPowerflow.hGeneratorList[k];

		for(i = 0; i < genList->nSize; i++) {

			KpfaGeneratorCtg_t *genCtg = &genList->pList[i];
			fprintf(fp, "generator ctg[name - %s] ===\n", genCtg->rName);

			for(j = 0; j < genCtg->nSize; j++) {

				fprintf(fp, "generator[%d]: Bus ID: %d, Q Gen - %6.5f\n", j,
							genCtg->pList[j].nBusId,
							genCtg->pList[j].nQGen);
			}
		}
	}

	fclose(fp);
}

