/*
 * KpfaCaseData.cpp
 *
 *  Created on: 2014. 5. 21.
 *      Author: Youngsun Han
 */

#include "KpfaCaseData.h"

KpfaCaseData::KpfaCaseData()
:KpfaRawData(KPFA_RAW_CASE_ID){

#ifdef RAW_DATA_INITIALIZED
	// Set as the default values
	m_bIc = 0;
	m_nSbase = 100.0;
	m_nRev = 32;
	m_nXfrrat = 0.0;
	m_nNxfrat = 0.0;
	m_nBasfrq = 0.0;
#endif
}

KpfaCaseData::~KpfaCaseData() {
	// Do nothing
}

/**
 * This function must be implemented to parse the input string into the case ID data.
 *
 * @param rInputString a string with the case ID data to be parsed
 * @return error information
 */
KpfaError_t
KpfaCaseData::ParseInput(string &rInputString, uint32_t nId) {

	KpfaStringList_t tokens;
	uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

	if(nTokens != KPFA_NUM_CASE_DATA_ITEMS) {
		return KPFA_ERROR_CASE_PARSE;
	}

#if 0
	KPFA_DEBUG("KpfaCaseData", "%s\n", rInputString.c_str());
#endif

	// Case flag
	m_bIc = (bool_t)atoi(tokens[0].c_str());

	// System MVA base
	m_nSbase = atof(tokens[1].c_str());

	// PSSE revision number
	m_nRev = (uint8_t)atoi(tokens[2].c_str());

	// Units of transformer ratings
	m_nXfrrat = atof(tokens[3].c_str());

	// Units of ratings of non-transformer branches
	m_nNxfrat = atof(tokens[4].c_str());

	// System base frequency in Hertz
	m_nBasfrq = atof(tokens[5].c_str());

	return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaCaseData::Write(ostream &rOut) {

	rOut << "KPFA CASE DATA: " << endl;

	rOut << "\tIC: " << (uint32_t)m_bIc << endl;

	rOut << "\tSBASE: " << m_nSbase << endl;

	rOut << "\tREV: " << (uint32_t)m_nRev << endl;

	rOut << "\tXFRRAT: " << m_nXfrrat << endl;

	rOut << "\tNXFRAT: " << m_nNxfrat << endl;

	rOut << "\tBASFRQ: " << m_nBasfrq << endl;
}

ostream &operator << (ostream &rOut, KpfaCaseData *pRawData) {

	pRawData->Write(rOut);
	return rOut;
}



