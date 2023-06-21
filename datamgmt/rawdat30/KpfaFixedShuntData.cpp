/*
 * KpfaFixedShuntData.cpp
 *
 *  Created on: 2014. 5. 21.
 *      Author: Youngsun Han
 */

#include "KpfaFixedShuntData.h"

KpfaFixedShuntData::KpfaFixedShuntData()
:KpfaRawData(KPFA_RAW_FIXED_SHUNT){

#ifdef RAW_DATA_INITIALIZED
	// No default allowed
	m_nI = 0;

	// Set as the default values
	m_rId = "";
	m_bStatus = TRUE;
	m_nGl = 0.0;
	m_nBl = 0.0;
#endif
}

KpfaFixedShuntData::~KpfaFixedShuntData() {
	// Do nothing
}

/**
 * This function must be implemented to parse the input string into the load data.
 *
 * @param rInputString a string with the fixed shunt data to be parsed
 * @return error information
 */
KpfaError_t
KpfaFixedShuntData::ParseInput(string &rInputString, uint32_t nId) {

	KpfaStringList_t tokens;
	uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

	if(nTokens != KPFA_NUM_FIXED_SHUNT_DATA_ITEMS) {
		return KPFA_ERROR_FIXED_SHUNT_PARSE;
	}

#if 0
	KPFA_DEBUG("KpfaFixedShuntData", "%s\n", rInputString.c_str());
#endif

	// Bus number
	m_nI = (uint32_t)atoi(tokens[0].c_str());

	// Shunt ID
	m_rId = tokens[1];

	// Shunt status (1: in-service, 0: out-of-service)
	m_bStatus = (bool_t)atoi(tokens[2].c_str());

	// Active component of shunt admittance to ground
	m_nGl = atof(tokens[3].c_str());

	// Reactive component of shunt admittance to ground
	m_nBl = atof(tokens[4].c_str());

	return KPFA_SUCCESS;
}

/**
 * This function will be used to transform the units of the values of the raw data.
 *
 * @param nSbase System MVA base
 * @return error information
 */
KpfaError_t
KpfaFixedShuntData::TransformUnit(double nSbase) {

	if(nSbase == 0) {
		return KPFA_ERROR_INVALID_SBASE;
	}

	m_nGl /= nSbase;
	m_nBl /= nSbase;

	return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaFixedShuntData::Write(ostream &rOut) {

	rOut << "KPFA FIXED SHUNT DATA: " << endl;

	rOut << "\tI: " << m_nI << endl;

	rOut << "\tID: " << m_rId << endl;

	rOut << "\tSTATUS: " << (uint32_t)m_bStatus << endl;

	rOut << "\tGL: " << m_nGl << endl;

	rOut << "\tBL: " << m_nBl << endl;
}

ostream &operator << (ostream &rOut, KpfaFixedShuntData *pRawData) {

	pRawData->Write(rOut);
	return rOut;
}




