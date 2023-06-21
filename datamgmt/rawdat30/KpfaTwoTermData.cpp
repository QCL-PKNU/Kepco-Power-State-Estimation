/*
 * KpfaTwoTermData.cpp
 *
 *  Created on: 2014. 5. 21.
 *      Author: Youngsun Han
 */

#include "KpfaTwoTermData.h"

KpfaTwoTermData::KpfaTwoTermData()
:KpfaRawData(KPFA_RAW_TWO_TERM){

#ifdef RAW_DATA_INITIALIZED
	// No default allowed
#endif
}

KpfaTwoTermData::~KpfaTwoTermData() {
	// Do nothing
}

/**
 * This function must be implemented to parse the input string into the two term data.
 *
 * @param rInputString a string with the two term data to be parsed
 * @return error information
 */
KpfaError_t
KpfaTwoTermData::ParseInput(string &rInputString, uint32_t nId) {

#if 0
	KPFA_DEBUG("KpfaTwoTermData", "%s\n", rInputString.c_str());
#endif

	return KPFA_SUCCESS;
}

/**
 * This function will be used to transform the units of the values of the raw data.
 *
 * @param nSbase System MVA base
 * @return error information
 */
KpfaError_t
KpfaTwoTermData::TransformUnit(double nSbase) {

	return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaTwoTermData::Write(ostream &rOut) {

	rOut << "KPFA TWO TERMINAL DC DATA: " << endl;
}

ostream &operator << (ostream &rOut, KpfaTwoTermData *pRawData) {

	pRawData->Write(rOut);
	return rOut;
}
