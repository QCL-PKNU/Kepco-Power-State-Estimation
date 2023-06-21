/*
 * KpfaLoadData.cpp
 *
 *  Created on: 2014. 5. 21.
 *      Author: Youngsun Han
 */

#include "KpfaLoadData.h"

KpfaLoadData::KpfaLoadData()
:KpfaRawData(KPFA_RAW_LOAD){

#ifdef RAW_DATA_INITIALIZED
	// No default allowed
	m_nI = 0;

	// Set as the default values
	m_rId = "";
	m_bStatus = 1;
	m_nPl = 0.0;
	m_nQl = 0.0;
	m_nIp = 0.0;
	m_nIq = 0.0;
	m_nYp = 0.0;
	m_nYq = 0.0;
	m_nArea = 0;
	m_nZone = 0;
	m_nOwner = 0;

	m_nHvdcI = 0;
#endif
}

KpfaLoadData::~KpfaLoadData() {
	// Do nothing
}

/**
 * This function must be implemented to parse the input string into the load data.
 *
 * @param rInputString a string with the load data to be parsed
 * @return error information
 */
KpfaError_t
KpfaLoadData::ParseInput(string &rInputString, uint32_t nId) {

	KpfaStringList_t tokens;
	uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

	if(nTokens != KPFA_NUM_LOAD_DATA_ITEMS) {
		return KPFA_ERROR_LOAD_PARSE;
	}

#if 0
	KPFA_DEBUG("KpfaLoadData", "%s\n", rInputString.c_str());
#endif

	// Bus number (1 ~ 999997)
	m_nI = (uint32_t)atoi(tokens[0].c_str());

	if(m_nI == 0) {
		return KPFA_ERROR_LOAD_PARSE;
	}

	// Sub Load ID to distinguish among multiple loads at a bus.
	m_rId = tokens[1];

	// Load status (0: out-of-service, 1: in-service)
	m_bStatus = (bool_t)atoi(tokens[2].c_str());

	// Area to which the load is assigned (1 ~ 9999)
	m_nArea = (uint16_t)atoi(tokens[3].c_str());

	// Zone to which the load is assigned (1 ~ 9999)
	m_nZone = (uint16_t)atoi(tokens[4].c_str());

	// Active power of MVA load
	m_nPl = atof(tokens[5].c_str());

	// Reactive power of MVA load
	m_nQl = atof(tokens[6].c_str());

	// Active power of current load
	m_nIp = atof(tokens[7].c_str());

	// Reactive power of current load
	m_nIq = atof(tokens[8].c_str());

	// Active power of admittance load
	m_nYp = atof(tokens[9].c_str());

	// Reactive power of admittance load
	m_nYq = atof(tokens[10].c_str());

	// Owner to which the load is assigned (1 ~ 9999)
	m_nOwner = (uint16_t)atoi(tokens[11].c_str());

	return KPFA_SUCCESS;
}

/**
 * This function will be used to transform the units of the values of the raw data.
 *
 * @param nSbase System MVA base
 * @return error information
 */
KpfaError_t
KpfaLoadData::TransformUnit(double nSbase) {

	if(nSbase == 0) {
		return KPFA_ERROR_INVALID_SBASE;
	}

	m_nPl /= nSbase;
	m_nQl /= nSbase;

	m_nIp /= nSbase;
	m_nIq /= nSbase;

	m_nYp /= nSbase;
	m_nYq /= nSbase;

	return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaLoadData::Write(ostream &rOut) {

	rOut << "KPFA LOAD DATA: " << endl;

	rOut << "\tI: " << m_nI << endl;

	rOut << "\tID: " << m_rId << endl;

	rOut << "\tSTATUS: " << (uint32_t)m_bStatus << endl;

	rOut << "\tAREA: " << m_nArea << endl;

	rOut << "\tZONE: " << m_nZone << endl;

	rOut << "\tPL: " << m_nPl << endl;

	rOut << "\tQL: " << m_nQl << endl;

	rOut << "\tIP: " << m_nIp << endl;

	rOut << "\tIQ: " << m_nIq << endl;

	rOut << "\tYP: " << m_nYp << endl;

	rOut << "\tYQ: " << m_nYq << endl;

	rOut << "\tOWNER: " << m_nOwner << endl;
}

ostream &operator << (ostream &rOut, KpfaLoadData *pRawData) {

	pRawData->Write(rOut);
	return rOut;
}




