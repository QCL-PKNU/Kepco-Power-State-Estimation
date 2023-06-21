/*
 * KpfaBusData.cpp
 *
 *  Created on: 2014. 5. 21.
 *      Author: Youngsun Han
 */

#include "KpfaBusData.h"

KpfaBusData::KpfaBusData()
:KpfaRawData(KPFA_RAW_BUS){

#ifdef RAW_DATA_INITIALIZED
	// No default allowed
	m_nI = 0;

	// Set as the default values
	m_rName = "";
	m_nBaskv = 0.0;
	m_nIde = KPFA_LOAD_BUS;
	m_nOrigIde = KPFA_LOAD_BUS;
	m_nGl = 0.0;
	m_nBl = 0.0;
	m_nArea = 1;
	m_nZone = 1;
	m_nVm = 1.0;
	m_nVa = 0.0;
	m_nOwner = 1;
#endif

	m_nIdx = 0;
	m_nPl = 0.0;
	m_nQl = 0.0;
	m_nPg = 0.0;
	m_nQg = 0.0;
}

KpfaBusData::~KpfaBusData() {
	// Do nothing
}

/**
 * This function must be implemented to parse the input string into the bus data.
 *
 * @param rInputString a string with the bus data to be parsed
 * @return error information
 */
KpfaError_t
KpfaBusData::ParseInput(string &rInputString, uint32_t nId) {

	KpfaStringList_t tokens;
	uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

	if(nTokens != KPFA_NUM_BUS_DATA_ITEMS) {
		return KPFA_ERROR_BUS_PARSE;
	}

#if 0
	KPFA_DEBUG("KpfaBusData", "%s\n", rInputString.c_str());
#endif

	// Bus number (1 ~ 999997)
	m_nI = (uint32_t)atoi(tokens[0].c_str());

	if(m_nI == 0) {
		return KPFA_ERROR_BUS_PARSE;
	}

	// Bus name (up to 12 characters)
	m_rName = tokens[1];

	// Bus base voltage
	m_nBaskv = atof(tokens[2].c_str());

	// Bus type code
	m_nIde = (KpfaBusType_t)atoi(tokens[3].c_str());

	// Gl, Bl
	m_nGl = atof(tokens[4].c_str());
	m_nBl = atof(tokens[5].c_str());

	// Area number (1 ~ 9999)
	m_nArea = (uint16_t)atoi(tokens[6].c_str());

	// Zone number (1 ~ 9999)
	m_nZone = (uint16_t)atoi(tokens[7].c_str());

	// Bus voltage magnitude
	m_nVm = atof(tokens[8].c_str());

	// Bus voltage phase angle
	m_nVa = atof(tokens[9].c_str());

	// Owner number (1 ~ 9999)
	m_nOwner = (uint16_t)atoi(tokens[10].c_str());

	return KPFA_SUCCESS;
}

/**
 * This function will be used to transform the units of the values of the raw data.
 *
 * @param nSbase System MVA base
 * @return error information
 */
KpfaError_t
KpfaBusData::TransformUnit(double nSbase) {

	if(nSbase == 0) {
		return KPFA_ERROR_INVALID_SBASE;
	}

	m_nGl /= nSbase;
	m_nBl /= nSbase;

	// Change angle to radians
	m_nVa /= KPFA_ANGLE_TO_RADIAN;

	return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaBusData::Write(ostream &rOut) {

	rOut << "KPFA BUS DATA: " << endl;

	rOut << "\tI: " << m_nI << endl;

	rOut << "\tNAME: " << m_rName << endl;

	rOut << "\tBASKV: " << m_nBaskv << endl;

	rOut << "\tIDE: " << m_nIde << endl;

	rOut << "\tGL: " << m_nGl << endl;

	rOut << "\tBL: " << m_nBl << endl;

	rOut << "\tAREA: " << m_nArea << endl;

	rOut << "\tZONE: " << m_nZone << endl;

	rOut << "\tVM: " << m_nVm << endl;

	rOut << "\tVA: " << m_nVa << endl;

	rOut << "\tOWNER: " << m_nOwner << endl;
}

ostream &operator << (ostream &rOut, KpfaBusData *pRawData) {

	pRawData->Write(rOut);
	return rOut;
}




