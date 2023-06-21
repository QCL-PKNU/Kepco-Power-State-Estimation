/*
 * KpfaVscData.cpp
 *
 *  Created on: 2014. 5. 21.
 *      Author: Youngsun Han
 */

#include "KpfaVscData.h"

KpfaVscData::KpfaVscData()
:KpfaRawData(KPFA_RAW_VSC){

#ifdef RAW_DATA_INITIALIZED
	// No default allowed
#endif
}

KpfaVscData::~KpfaVscData() {
	// Do nothing
}

/**
 * This function must be implemented to parse the input string into the VSC data.
 *
 * @param rInputString a string with the VSC data to be parsed
 * @return error information
 */
KpfaError_t
KpfaVscData::ParseInput(string &rInputString, uint32_t nId) {

#if 0
	KPFA_DEBUG("KpfaVscData", "%s\n", rInputString.c_str());
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
KpfaVscData::TransformUnit(double nSbase) {

	return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaVscData::Write(ostream &rOut) {

	rOut << "KPFA VSC DC DATA: " << endl;
}

ostream &operator << (ostream &rOut, KpfaVscData *pRawData) {

	pRawData->Write(rOut);
	return rOut;
}
