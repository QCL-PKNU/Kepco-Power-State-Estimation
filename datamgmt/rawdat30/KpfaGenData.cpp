/*
 * KpfaGenData.cpp
 *
 *  Created on: 2014. 5. 21.
 *      Author: Youngsun Han
 */

#include "KpfaGenData.h"

KpfaGenData::KpfaGenData()
:KpfaRawData(KPFA_RAW_GEN){

#ifdef RAW_DATA_INITIALIZED
	// No default allowed
	m_nI = 0;

	// Set as the default values
	m_rId = "";

	m_nPg = 0.0;
	m_nQg = 0.0;
	m_nQt = 9999.0;
	m_nQb = -9999.0;
	m_nVs = 1.0;
	m_nIreg = 0;

	m_nMbase = 0.0;

	m_nZr = 0.0;
	m_nZx = 1.0;
	m_nRt = 0.0;
	m_nXt = 0.0;
	m_nGtap = 1.0;
	m_bStat = 1;
	m_nRmpct = 100.0;
	m_nPt = 9999.0;
	m_nPb = -9999.0;

	m_nOwnerCount = 0;

	for(int i = 0; i < KPFA_MAX_NUM_OWNERS; i++) {
		m_nO[i] = 0;
		m_nF[i] = 1.0;
	}

	m_nWmod = 0;
#endif

	m_bQtx = FALSE;
	m_bQbx = FALSE;

	m_bFacts = FALSE;
	m_nWFactor[0] = 0;
	m_nWFactor[1] = 0;
}

KpfaGenData::~KpfaGenData() {
	// Do nothing
}

/**
 * This function must be implemented to parse the input string into the branch data.
 *
 * @param rInputString a string with the branch data to be parsed
 * @return error information
 */
KpfaError_t
KpfaGenData::ParseInput(string &rInputString, uint32_t nId) {

	uint32_t i = 0, j;
	KpfaStringList_t tokens;
	uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

	if(nTokens < KPFA_MIN_NUM_GENERATOR_DATA_ITEMS ||
	   nTokens > KPFA_MAX_NUM_GENERATOR_DATA_ITEMS) {
		return KPFA_ERROR_GENERATOR_PARSE;
	}

#if 0
	KPFA_DEBUG("KpfaGenData", "%s\n", rInputString.c_str());
#endif

	// Bus number
	m_nI = (uint32_t )atoi(tokens[i++].c_str());

	if(m_nI == 0) {
		return KPFA_ERROR_GENERATOR_PARSE;
	}

	// Machine identifier used to distinguish among multiple machines at bus I
	m_rId = tokens[i++];

	// Generator active power output
	m_nPg = atof(tokens[i++].c_str());

	// Generator reactive power output
	m_nQg = atof(tokens[i++].c_str());

	// Maximum generator reactive power output
	m_nQt = atof(tokens[i++].c_str());

	// Minimum generator reactive power output
	m_nQb = atof(tokens[i++].c_str());

	// Regulated voltage set-point
	m_nVs = atof(tokens[i++].c_str());

	// Sub bus number
	m_nIreg = (uint32_t)atoi(tokens[i++].c_str());

	// Total MVA base of the units represented by this machine
	m_nMbase = atof(tokens[i++].c_str());

	// Complex machine impedance
	m_nZr = atof(tokens[i++].c_str());
	m_nZx = atof(tokens[i++].c_str());

	// Step-up transformer impedance
	m_nRt = atof(tokens[i++].c_str());
	m_nXt = atof(tokens[i++].c_str());

	// Step-up transformer off-nominal turns ratio
	m_nGtap = atof(tokens[i++].c_str());

	// Machine status (1: in-service, 0: out-of-service)
	m_bStat = (bool_t)atoi(tokens[i++].c_str());

	// Percent of the total Mvar required
	m_nRmpct = atof(tokens[i++].c_str());

	// Maximum generator active power output
	m_nPt = atof(tokens[i++].c_str());

	// Minimum generator active power output
	m_nPb = atof(tokens[i++].c_str());

	// Owner, Fraction
	for(j = 0; i < nTokens; j++) {

		// Owner number (1 ~ 9999).
		m_nO[j] = (uint16_t)atoi(tokens[i++].c_str());

		// Fraction of total ownership assigned to owner Qi
		m_nF[j] = atof(tokens[i++].c_str());
	}

	m_nOwnerCount = j;

	return KPFA_SUCCESS;
}

/**
 * This function will be used to transform the units of the values of the raw data.
 *
 * @param nSbase System MVA base
 * @return error information
 */
KpfaError_t
KpfaGenData::TransformUnit(double nSbase) {

	if(nSbase == 0) {
		return KPFA_ERROR_INVALID_SBASE;
	}

	m_nPg /= nSbase;
	m_nQg /= nSbase;

	m_nPt /= nSbase;
	m_nPb /= nSbase;

	m_nQt /= nSbase;
	m_nQb /= nSbase;

	double mbaseRatio = m_nMbase / nSbase;

	m_nZr *= mbaseRatio;
	m_nZx *= mbaseRatio;

	m_nRt *= mbaseRatio;
	m_nXt *= mbaseRatio;

	return KPFA_SUCCESS;
}

/**
 * This function will be used to merge the given generator with this generator.
 *
 * @param pGenData a generator to be merged
 * @return error information
 */
void
KpfaGenData::MergeWith(KpfaGenData *pGenData) {
	m_nPg += pGenData->m_nPg;
	m_nQt += pGenData->m_nQt;
	m_nQb += pGenData->m_nQb;
	m_nXt += pGenData->m_nXt;
	m_nPt += pGenData->m_nPt;
	m_nPb += pGenData->m_nPb;
	m_nMbase += pGenData->m_nMbase;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaGenData::Write(ostream &rOut) {

	rOut << "KPFA GENERATOR DATA: " << endl;

	rOut << "\tI: " << m_nI << endl;

	rOut << "\tID: " << m_rId << endl;

	rOut << "\tPG: " << m_nPg << endl;

	rOut << "\tQG: " << m_nQg << endl;

	rOut << "\tQT: " << m_nQt << endl;

	rOut << "\tQB: " << m_nQb << endl;

	rOut << "\tVS: " << m_nVs << endl;

	rOut << "\tIREG: " << m_nIreg << endl;

	rOut << "\tMBASE: " << m_nMbase << endl;

	rOut << "\tZR: " << m_nZr << endl;

	rOut << "\tZX: " << m_nZx << endl;

	rOut << "\tRT: " << m_nRt << endl;

	rOut << "\tXT: " << m_nXt << endl;

	rOut << "\tGTAP: " << m_nGtap << endl;

	rOut << "\tSTAT: " << (uint32_t)m_bStat << endl;

	rOut << "\tRMPCT: " << m_nRmpct << endl;

	rOut << "\tPT: " << m_nPt << endl;

	rOut << "\tPB: " << m_nPb << endl;

	for(uint32_t i = 0; i < m_nOwnerCount; i++) {

		rOut << "\tO" << i << ": " << m_nO[i] << endl;

		rOut << "\tF" << i << ": " << m_nF[i] << endl;
	}

#if KPFA_RAW_DATA_VERSION > 35
	rOut << "\tWMOD: " << m_nWmod << endl;
#endif

	rOut << "\tQtx: " << m_bQtx << endl;

	rOut << "\tQbx: " << m_bQbx << endl;
}

ostream &operator << (ostream &rOut, KpfaGenData *pRawData) {
	pRawData->Write(rOut);
	return rOut;
}




