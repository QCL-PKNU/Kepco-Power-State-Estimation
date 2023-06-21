/*
 * KpfaGvModule.cpp
 *
 *  Created on: Oct 13, 2015
 *      Author: youngsun
 */

#include "KpfaGvModule.h"
#include "KpfaPowerflow.h"
#ifdef KPFA_RESULT_SUPPORT
#include "KpfaResultData.h"
#endif

#define KPFA_MIN_RATIO_PQ	0.4
#define KPFA_MAX_RATIO_PQ	0.7

/**
 * This function will apply a stabilization algorithm of GV to the given raw data.
 *
 * @param pRawDataMgmt raw data management to be stabilized
 * @param pCtgData contingency data
 * @return error information
 */
KpfaError_t
KpfaGvModule::Execute(KpfaRawDataMgmt *pRawDataMgmt, KpfaCtgData *pCtgData) {

	KpfaError_t error;

	uint32_t i;

	double hvdcP = 0;
	double hvdcQ = 0;
	double ratioPQ = 0;

	m_rGvDataList.clear();

	double gstep = m_pCtrlDataMgmt->m_nGstep / pRawDataMgmt->m_nSysBase;

	uint32_t inc0 = m_pCtrlDataMgmt->m_nIncrement0;
	uint32_t inc1 = m_pCtrlDataMgmt->m_nIncrement1;

	uint32_t hvdcid = KPFA_HVDC_1_BID;

	// FACTS information
	KpfaRawDataList_t::iterator fiter;
	KpfaRawDataList_t &factsList = pRawDataMgmt->GetFactsDataList();

	uint32_t nfacts = factsList.size();
	double *prevVmList = new double[nfacts];
	memset(prevVmList, 0, sizeof(double) * nfacts);

	// New Scheme for GV
	if(pCtgData->HasOutageGen() || pCtgData->HasOutageHvdc()) {

		KpfaBusData *loadBus = pRawDataMgmt->GetBusData(hvdcid);
		KPFA_CHECK(loadBus != NULL, KPFA_ERROR_INVALID_BUS_ID);

		// HVDC #1 P
		hvdcP = -loadBus->m_nPl;

		if(hvdcP > KPFA_MAX_HVDC_1_P || hvdcid == pCtgData->GetOutageHvdcId()) {

			hvdcid = KPFA_HVDC_2_BID;

			loadBus = pRawDataMgmt->GetBusData(hvdcid);
			KPFA_CHECK(loadBus != NULL, KPFA_ERROR_INVALID_BUS_ID);

			// HVDC #2 P
			hvdcP = -loadBus->m_nPl;

			if(hvdcP > KPFA_MAX_HVDC_2_P || hvdcid == pCtgData->GetOutageHvdcId()) {
				return KPFA_ERROR_INVALID_HVDC_P;
			}
		}

		// HVDC Q (if the value of GetHvdcFilter > 0, hvdcQ is C)
		switch(hvdcid) {
			case KPFA_HVDC_1_BID:
				hvdcQ = loadBus->m_nQl + (-1.0) * pRawDataMgmt->GetHvdcFilter(0, inc0, hvdcP * pRawDataMgmt->m_nSysBase);
				break;
			case KPFA_HVDC_2_BID:
				hvdcQ = loadBus->m_nQl + (-1.0) * pRawDataMgmt->GetHvdcFilter(1, inc1, hvdcP * pRawDataMgmt->m_nSysBase);
				break;
			default:
				return KPFA_ERROR_INVALID_HVDC_ID;
		}

		hvdcQ /= pRawDataMgmt->m_nSysBase;

		// PQ ratio
		ratioPQ = fabs(hvdcQ / hvdcP);

		// Exception handling for GV
		if(ratioPQ < KPFA_MIN_RATIO_PQ)			ratioPQ = KPFA_MIN_RATIO_PQ;
		else if(ratioPQ > KPFA_MAX_RATIO_PQ)	ratioPQ = KPFA_MAX_RATIO_PQ;
	}
#ifdef KPFA_RESULT_SUPPORT
	else {
		g_pResultData->hGvList.pList[pCtgData->GetIndex()].nSize = 0;

		return KPFA_SUCCESS;
	}
#endif

	/////////////////////////////////////////////////////////////////
	// Calculate the total Pg and its initial value
	/////////////////////////////////////////////////////////////////

	double totalPg = 0;
	double totalPg0 = 0;

	KpfaOutageDataList_t::iterator otgIter;
	KpfaOutageDataList_t &otgList = pCtgData->GetOutageDataList();

	// 1. Sum of Generator Pg
	for(otgIter = otgList.begin(); otgIter != otgList.end(); otgIter++) {

		KpfaOutageData *otg = (KpfaOutageData *)*otgIter;

		if(otg->GetDataType() != KPFA_OUTAGE_GEN) {
			continue;
		}

		KpfaBusData *genBus = pRawDataMgmt->GetBusData(otg->m_nI);
		KPFA_CHECK(genBus != NULL, KPFA_ERROR_INVALID_BUS_ID);

		totalPg0 += genBus->m_nPg;
	}

	// 2. Sum of HVDC Pg
	if(pCtgData->HasOutageHvdc()) {

		KpfaBusData *loadBus = pRawDataMgmt->GetBusData(pCtgData->GetOutageHvdcId());
		KPFA_CHECK(loadBus != NULL, KPFA_ERROR_INVALID_BUS_ID);

		totalPg0 -= loadBus->m_nPl;
	}

	totalPg = totalPg0;

	/////////////////////////////////////////////////////////////////
	// GV calculation
	/////////////////////////////////////////////////////////////////

#ifdef KPFA_RESULT_SUPPORT
	KpfaGvList_t *gvCtgList = &g_pResultData->hGvList;
	KpfaGvCtg_t *gvCtgItem = &gvCtgList->pList[pCtgData->GetIndex()];

	// Calculate the number of GV steps
	int numGvStep = ((int)(totalPg0 / gstep) + 1);

	gvCtgItem->nMargin = 1.0;
	gvCtgItem->nSize = numGvStep;
	gvCtgItem->pList = (KpfaGv_t *)malloc(sizeof(KpfaGv_t) * numGvStep);

	uint32_t k = 0;
#endif

	KpfaPowerflow pfa(m_pCtrlDataMgmt);

	// HVDC load
	KpfaBusData *hvdcBus = pRawDataMgmt->GetBusData(hvdcid);
	KPFA_CHECK(hvdcBus != NULL, KPFA_ERROR_INVALID_HVDC_ID);

	// For each outage
	for(otgIter = otgList.begin(); otgIter != otgList.end(); otgIter++) {

		// Keep P, Q values
		///////////////////////////////////////////////////////////
		double pgen0 = 0;
		double qmin0 = 0;
		double qmax0 = 0;

		double pload0 = 0;
		double phvdc0 = 0;
		double qhvdc0 = 0;
		///////////////////////////////////////////////////////////

		KpfaOutageData *otg = (KpfaOutageData *)*otgIter;
		KpfaBusData *loadBus = NULL;
		KpfaBusData *genBus = NULL;

		double pvalue = 0;

		if(otg->GetDataType() == KPFA_OUTAGE_GEN) {
			genBus = pRawDataMgmt->GetBusData(otg->m_nI);
			pvalue = genBus->m_nPg;
		}
		else if(otg->GetDataType() == KPFA_OUTAGE_HVDC){
			loadBus = pRawDataMgmt->GetBusData(otg->m_nI);
			pvalue = -loadBus->m_nPl;
		}
		else continue;

		// Keep P, Q values
		///////////////////////////////////////////////////////////
		if(genBus != NULL) {
			pgen0 = genBus->m_nPg;
			qmin0 = genBus->m_nQb;
			qmax0 = genBus->m_nQt;
		}
		else if(loadBus != NULL){
			pload0 = loadBus->m_nPl;
			phvdc0 = hvdcBus->m_nPl;
			qhvdc0 = hvdcBus->m_nQl;
		}
		///////////////////////////////////////////////////////////

		while(pvalue != 0) {

			//Modify the information of the generator
			if(pvalue < gstep) {
				gstep = pvalue;
			}

			pvalue -= gstep;

	        // Decrement the value of the total Pg
			totalPg -= gstep;

			if(genBus != NULL) {

				genBus->m_nPg -= gstep;

				// Qmax, Qmin
				genBus->m_nQb = qmin0 * (pvalue / pgen0);
				genBus->m_nQt = qmax0 * (pvalue / pgen0);
			}
			else if(loadBus != NULL) {
				// HVDC P&Q
				loadBus->m_nPl += gstep;
				loadBus->m_nQl -= gstep * ratioPQ;
			}

			// HVDC P, Q
			hvdcP += gstep;
			hvdcQ += gstep * ratioPQ;

			hvdcBus->m_nPl = -hvdcP;
			hvdcBus->m_nQl =  hvdcQ;

			error = pfa.DoAnalysis(pRawDataMgmt);

			if(error == KPFA_ERROR_NOT_CONVERGED) {

				// maximum margin
				m_nMaxMargin = (totalPg0 - totalPg) / totalPg0;

				// system required Q
				m_nSysReqQ = (1.0 - m_nMaxMargin) * totalPg0 * ratioPQ;

				// calculate the required Q for each FACTS using the sensitivity
				double totalSensitivity = 0;

				for(fiter = factsList.begin(); fiter != factsList.end(); fiter++) {
					KpfaFactsData *factsData = (KpfaFactsData *)*fiter;
					totalSensitivity += factsData->m_nSensitivity;
				}

				for(fiter = factsList.begin(); fiter != factsList.end(); fiter++) {
					KpfaFactsData *factsData = (KpfaFactsData *)*fiter;
					factsData->m_nRequiredQ = (factsData->m_nSensitivity / totalSensitivity);
				}

				return error;
			}
			else KPFA_CHECK(error == KPFA_SUCCESS, error);

			// caculate the value of senstivity for each facts
			for(i = 0, fiter = factsList.begin(); fiter != factsList.end(); fiter++, i++) {

				KpfaFactsData *factsData = (KpfaFactsData *)*fiter;

				// get facts bus
				KpfaBusData *factsBusData = pRawDataMgmt->GetBusData(factsData->m_nI);
				KPFA_CHECK(factsBusData != NULL, KPFA_ERROR_INVALID_FACTS_DATA);

				// calculate sensitivity value
				if(prevVmList[i] == 0) {
					return KPFA_ERROR_GV_REDUCE_GSTEP;
				}

				factsData->m_nSensitivity = (factsBusData->m_nVm - prevVmList[i]) / gstep;

				// update the previous Vm 
				prevVmList[i] = factsBusData->m_nVm;
			}

#ifdef KPFA_RESULT_SUPPORT

			// monitor bus ID
			uint32_t mid = m_pCtrlDataMgmt->m_nHvdcFreqControl;

			// G margin
			gvCtgItem->pList[k].nGenParam = (float)((totalPg0 - totalPg) / totalPg0);

			// Voltage
			KpfaComplexVector_t &vmat = pfa.GetVMatrix();
			int busIndex = pRawDataMgmt->GetBusIndex(mid);
			gvCtgItem->pList[k].nVoltage = vmat(busIndex).real();

			k++;
#endif
		}

		// Restore P, Q
		///////////////////////////////////////////////////////////
		if(genBus != NULL) {
			genBus->m_nPg = pgen0;
			genBus->m_nQb = qmin0;
			genBus->m_nQt = qmax0;
		}
		else if(loadBus != NULL){
			loadBus->m_nPl = pload0;
			hvdcBus->m_nPl = phvdc0;
			hvdcBus->m_nQl = qhvdc0;
		}
		///////////////////////////////////////////////////////////
	}

	return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaGvModule::Write(ostream &rOut) {
    // do nothing
}

ostream &operator << (ostream &rOut, KpfaGvModule *pModule) {
	pModule->Write(rOut);
	return rOut;
}
