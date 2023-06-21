/*
 * KpfaSwitchedShuntData.cpp
 *
 *  Created on: 2014. 5. 21.
 *      Author: Youngsun Han
 */

#include "KpfaSwitchedShuntData.h"

KpfaSwitchedShuntData::KpfaSwitchedShuntData()
:KpfaRawData(KPFA_RAW_SWITCHED_SHUNT){

#ifdef RAW_DATA_INITIALIZED
	// No default allowed
	m_nI = 0;
	m_nSwrem = 0;

	// Set as the default values
	m_nModsw = 0;
	m_nVswhi = 1.0;
	m_nVswlo = 1.0;
	m_nRmpct = 100.0;
	m_rRmidnt = "";
	m_nBinit = 0.0;

	for(int i = 0; i < KPFA_NUM_SWITCHED_SHUNT_BLOCKS; i++) {
		m_nN[i] = 0;
		m_nB[i] = 0.0;
	}
#endif

	m_nMaxShunt = 0;
	m_nMinShunt = 0;
	m_nShuntMode = 0;
}

KpfaSwitchedShuntData::~KpfaSwitchedShuntData() {
	// Do nothing
}

/**
 * This function must be implemented to parse the input string into the switched shunt data.
 *
 * @param rInputString a string with the switched shunt data to be parsed
 * @return error information
 */
KpfaError_t
KpfaSwitchedShuntData::ParseInput(string &rInputString, uint32_t nId) {

	uint32_t i = 0, j, k;
	KpfaStringList_t tokens;
	uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

	if(nTokens < KPFA_MIN_NUM_SWITCHED_SHUNT_DATA_ITEMS ||
	   nTokens > KPFA_MAX_NUM_SWITCHED_SHUNT_DATA_ITEMS) {
		return KPFA_ERROR_SWITCHED_SHUNT_PARSE;
	}

#if 0
	KPFA_DEBUG("KpfaSwitchedShuntData", "%s\n", rInputString.c_str());
#endif

	// Bus number
	m_nI = (uint32_t)atoi(tokens[i++].c_str());

	if(m_nI == 0) {
		return KPFA_ERROR_SWITCHED_SHUNT_PARSE;
	}

	// Control mode
	m_nModsw = (uint16_t)atoi(tokens[i++].c_str());

	// Desired voltage upper limit
	m_nVswhi = atof(tokens[i++].c_str());

	// Desired voltage lower limit
	m_nVswlo = atof(tokens[i++].c_str());

	// Remote bus number
	m_nSwrem = (uint32_t)atoi(tokens[i++].c_str());

	// Percent of contributed reactive power (100.0 by default)
	m_nRmpct = atof(tokens[i++].c_str());
	
	// VSC DC line name (MODSW = 4) or FACTS device name (MODSW = 6)
	m_rRmidnt = tokens[i++];

	// Switched shunt admittance
	m_nBinit = atof(tokens[i++].c_str());

	m_nMinShunt = 0;
	m_nMaxShunt = 0;

	for(j = 0; i < nTokens; j++) {

		// Number of steps for each block
		m_nN[j] = (uint32_t)atoi(tokens[i++].c_str());;

		// Admittance increment per step for each block
		m_nB[j] = atof(tokens[i++].c_str());

		// For updating max, min shunt values
		double tmp = m_nN[j] * m_nB[j];

		if(tmp >= 0)	m_nMaxShunt += tmp;
		else			m_nMinShunt += tmp;
	}

	m_nBlockNum = j;

	// Adjust the value of binit using max and min shunt values
	if(m_nBinit > m_nMaxShunt) {
		m_nBinit = m_nMaxShunt;
	}
	else if(m_nBinit < m_nMinShunt) {
		m_nBinit = m_nMinShunt;
	}

	m_rBankShunt.clear();

	for(k = 0, j = 0; k < m_nBlockNum; k++) {

		for(i = 0; i < m_nN[k]; i++) {

			double tmp = m_nB[k];

			if(j != 0) {
				tmp += m_rBankShunt[j-1];
			}

			m_rBankShunt.push_back(tmp);
		}
	}

	// For updating shunt mode
	m_nShuntMode = 0;

	for(k = 0; k < m_nBlockNum; k++) {
		if(m_nB[k] < 0) {
			m_nShuntMode += KPFA_REACTOR_MODE;
			break;
		}
	}

	for(k = 0; k < m_nBlockNum; k++) {
		if(m_nB[k] > 0) {
			m_nShuntMode += KPFA_SHUNT_MODE;
			break;
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will be used to transform the units of the values of the raw data.
 *
 * @param nSbase System MVA base
 * @return error information
 */
KpfaError_t
KpfaSwitchedShuntData::TransformUnit(double nSbase) {

	if(nSbase == 0) {
		return KPFA_ERROR_INVALID_SBASE;
	}

	uint32_t nBankShunt = m_rBankShunt.size();

	for(uint32_t i = 0; i < nBankShunt; i++) {
		m_rBankShunt[i] /= nSbase;
	}

	m_nBinit /= nSbase;

	return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaSwitchedShuntData::Write(ostream &rOut) {

	uint32_t i;

	rOut << "KPFA SWITCHED_SHUNT DC DATA: " << endl;

	rOut << "\tI: " << m_nI << endl;

	rOut << "\tSWREM: " << m_nSwrem << endl;

	rOut << "\tMODSW: " << m_nModsw << endl;

	rOut << "VSWHI" << m_nVswhi << endl;

	rOut << "VSWLO" << m_nVswlo << endl;

	rOut << "RMPCT" << m_nRmpct << endl;

	rOut << "RMIDNT" << m_rRmidnt << endl;

	rOut << "BINIT" << m_nBinit << endl;

	for(i = 0; i < m_nBlockNum; i++) {
		rOut << "\tN[" << i << "]: " << m_nN[i] << endl;
		rOut << "\tB[" << i << "]: " << m_nB[i] << endl;
	}

	rOut << "\tMAX SHUNT" << m_nMaxShunt << endl;

	rOut << "\tMIN SHUNT" << m_nMinShunt << endl;

	for(i = 0; i < m_rBankShunt.size(); i++) {
		rOut << "\tBANK SHUNT[" << i << "]: " << m_rBankShunt[i] << endl;
	}

	rOut << "\tSHUNT MODE" << m_nShuntMode << endl;
}

ostream &operator << (ostream &rOut, KpfaSwitchedShuntData *pRawData) {

	pRawData->Write(rOut);
	return rOut;
}
