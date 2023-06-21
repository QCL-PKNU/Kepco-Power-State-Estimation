/*
 * KpfaFactsModule.cpp
 *
 *  Created on: Oct 13, 2015
 *      Author: youngsun
 */

#include "KpfaFactsModule.h"

/**
 * This function will apply a stabilization algorithm to the given raw data for FACTS.
 *
 * @param pRawDataMgmt raw data management to be stabilized
 * @return error information
 */
KpfaError_t
KpfaFactsModule::Execute(KpfaRawDataMgmt *pRawDataMgmt) {

	KpfaRawDataList_t::iterator fiter;
	KpfaRawDataList_t &factsList = pRawDataMgmt->GetFactsDataList();

	for(fiter = factsList.begin(); fiter != factsList.end(); fiter++) {

		KpfaFactsData *factsData = (KpfaFactsData *)*fiter;

		// insert the new shunt data at the end of the shunt list
		KpfaSwitchedShuntData *shuntData = new KpfaSwitchedShuntData();
		shuntData->m_nBinit = -factsData->m_nRequiredQ;
		pRawDataMgmt->GetSwitchedShuntDataList().push_back(shuntData);

		// update the BL value of the shunt bus 
		KpfaBusData *shuntBus = pRawDataMgmt->GetBusData(factsData->m_nI);
		KPFA_CHECK(shuntBus != NULL, KPFA_ERROR_INVALID_FACTS_DATA);

		shuntBus->m_nBl += shuntData->m_nBinit;
	}

	return KPFA_SUCCESS;
}

/**
 * This function will apply a stabilization algorithm to the given raw data for FACTS.
 *
 * @param pRawDataMgmt raw data management to be stabilized
 * @param bIncrement if the FACTS will be incrementally updated
 * @return if the FACTS was updated
 */
bool_t
KpfaFactsModule::Execute(KpfaRawDataMgmt *pRawDataMgmt, bool_t bIncrement) {

	bool_t updated = FALSE;

	// generator list
	KpfaRawDataList_t::iterator giter;
	KpfaRawDataList_t &genList = pRawDataMgmt->GetGenDataList();

	for(giter = genList.begin(); giter != genList.end(); giter++) {

		KpfaGenData *gen = (KpfaGenData *)*giter;

		// skip non-FACTS generators
		if(gen->m_bFacts != TRUE) {
			continue;
		}

		// get the bus of the generator
		KpfaBusData *bus = pRawDataMgmt->GetBusData(gen->m_nI);
		KPFA_CHECK(bus != NULL, KPFA_ERROR_INVALID_BUS_ID);

		double qg = gen->m_nQg;
		double qt = gen->m_nQt;
		double qb = gen->m_nQb;

		if(bIncrement == TRUE) {

			if(qt > qg) {

				bus->m_nVm += m_nVsStep;
				gen->m_nVs += m_nVsStep;

				if(((qt - qg) / qt) < KPFA_FACTS_VS_MARGIN) {
					gen->m_nQg = qt;
				}
				else updated = TRUE;
			}
		}
		else {

			if(qb < qg) {

				bus->m_nVm -= m_nVsStep;
				gen->m_nVs -= m_nVsStep;

				if(((qg - qb) / qb) < KPFA_FACTS_VS_MARGIN) {
					gen->m_nQg = qb;
				}
				else updated = TRUE;
			}
		}
	}

	// error check
	if(updated == FALSE) {
		return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaFactsModule::Write(ostream &rOut) {
    // do nothing
}

ostream &operator << (ostream &rOut, KpfaFactsModule *pModule) {
	pModule->Write(rOut);
	return rOut;
}
