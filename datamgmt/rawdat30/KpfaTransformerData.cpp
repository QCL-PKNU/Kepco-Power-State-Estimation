/*
 * KpfaTransformerData.cpp
 *
 *  Created on: 2014. 5. 21.
 *      Author: Youngsun Han
 */

#include "KpfaTransformerData.h"

KpfaTransformerData::KpfaTransformerData()
:KpfaRawData(KPFA_RAW_TRANSFORMER){

#ifdef RAW_DATA_INITIALIZED
	// No default allowed
	m_nI = 0;
	m_nJ = 0;
	m_nK = 0;

	// Set as the default values
	m_nCkt = 1;
	m_nCw = 1;
	m_nCz = 1;
	m_nCm = 1;
	m_nMag1 = 0.0;
	m_nMag2 = 0.0;
	m_nNmetr = 2;
	m_rName = "            "; // 12 blanks
	m_nStat = 1;

	m_nWindingCount = 1;
	m_nOwnerCount = 0;

	for(int i = 0; i < KPFA_MAX_NUM_OWNERS; i++) {
		m_nO[i] = 0;
		m_nF[i] = 1.0;
	}

	for(int i = 0; i < KPFA_MAX_NUM_WINDINGS; i++) {
		m_nR[i] = 0.0;
		m_nX[i] = 0.0;
		m_nSbase[i] = 0.0;
	}

	m_nVmstar = 1.0;
	m_nAnstar = 0.0;

	for(int i = 0; i < KPFA_MAX_NUM_WINDINGS; i++) {
		m_nWindv[i] = 1.0;
		m_nNomv[i] = 0.0;
		m_nAng[i] = 0.0;
		m_nRata[i] = 0.0;
		m_nRatb[i] = 0.0;
		m_nRatc[i] = 0.0;
		m_nCod[i] = 0;
		m_nCont[i] = 0;
		m_nRma[i] = 1.1;
		m_nRmi[i] = 0.9;
		m_nVma[i] = 1.1;
		m_nVmi[i] = 0.9;
		m_nNtp[i] = 33;
		m_nTap[i] = 0;
		m_nCr[i] = 0.0;
		m_nCx[i] = 0.0;
	}
#endif
}

KpfaTransformerData::~KpfaTransformerData() {
	// Do nothing
}

/**
 * This function must be implemented to parse the input string into the transformer data.
 *
 * @param rInputString a string at line #1 with the transformer data to be parsed
 * @return error information
 */
KpfaError_t
KpfaTransformerData::ParseRawDataLineType1(string &rInputString) {

	uint32_t i = 0, j;
	KpfaStringList_t tokens;
	uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

	if(nTokens < KPFA_MIN_NUM_TRANSFORMER_DATA_ITEMS_1 ||
	   nTokens > KPFA_MAX_NUM_TRANSFORMER_DATA_ITEMS_1) {
		return KPFA_ERROR_TRANSFORMER_PARSE;
	}

#if 0
	KPFA_DEBUG("KpfaTransformerData Line #1", "%s\n", rInputString.c_str());
#endif

	// Bus number
	m_nI = (uint32_t)atoi(tokens[i++].c_str());

	if(m_nI == 0) {
		return KPFA_ERROR_TRANSFORMER_PARSE;
	}

	// Bus number of Winding 2
	m_nJ = (uint32_t)atoi(tokens[i++].c_str());

	// Bus number of Winding 3
	m_nK = (uint32_t)atoi(tokens[i++].c_str());

	// Transformer Circuit ID
#if 0
	char *ckt = tokens[i++].c_str();
	m_nCkt = (uint32_t)(ckt[1] - '0');
#else
	string &ckt = KpfaStripQuotes(tokens[i++]);
	m_nCkt = (uint32_t)atoi(ckt.c_str());
#endif

	// Winding data I/O code
	m_nCw = (uint8_t)atoi(tokens[i++].c_str());

	// Impedance data I/O code
	m_nCz = (uint8_t)atoi(tokens[i++].c_str());

	// Magnetizing admittance I/O code
	m_nCm = (uint8_t)atoi(tokens[i++].c_str());

	// Transformer magnetizing admittance connected to ground at bus I
	m_nMag1 = atof(tokens[i++].c_str());
	m_nMag2 = atof(tokens[i++].c_str());

	// Nonmetered end code of either 1 (for the Winding 1 bus) or 2 (for the Winding 2 bus)
	m_nNmetr = (uint8_t)atoi(tokens[i++].c_str());

	// Transformer ID
	m_rName = tokens[i++];

	// Transformer status
	// 0: out-of-service
	// 1: in-service and
	// 2: only Winding 2 out-of-service
	// 3: only Winding 3 out-of-service
	// 4: only Winding 1 out-of-service
	m_nStat = (uint8_t)atoi(tokens[i++].c_str());

	// The number of windings
	m_nWindingCount = (m_nK != 0) ? KPFA_MAX_NUM_WINDINGS : 1;

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
 * This function must be implemented to parse the input string into the transformer data.
 *
 * @param rInputString a string at line #2 with the transformer data to be parsed
 * @return error information
 */
KpfaError_t
KpfaTransformerData::ParseRawDataLineType2(string &rInputString) {

	KpfaStringList_t tokens;

	uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

#if 0
	KPFA_DEBUG("KpfaTransformerData Line #2", "%s\n", rInputString.c_str());
#endif

	// 2 Winding
	if(nTokens == KPFA_MIN_NUM_TRANSFORMER_DATA_ITEMS_2) {

		KPFA_CHECK(m_nK == 0, KPFA_ERROR_TRANSFORMER_PARSE);

		m_nR[KPFA_WINDING_1_TO_2] = atof(tokens[0].c_str());
		m_nX[KPFA_WINDING_1_TO_2] = atof(tokens[1].c_str());
		m_nSbase[KPFA_WINDING_1_TO_2] = atof(tokens[2].c_str());

		return KPFA_SUCCESS;
	}
	// 3 Winding
	else if(nTokens == KPFA_MAX_NUM_TRANSFORMER_DATA_ITEMS_2) {

		KPFA_CHECK(m_nK != 0, KPFA_ERROR_TRANSFORMER_PARSE);

		int j = 0;

		for(int i = 0; i < KPFA_MAX_NUM_WINDINGS; i++) {
			m_nR[i] = atof(tokens[j++].c_str());
			m_nX[i] = atof(tokens[j++].c_str());
			m_nSbase[i] = atof(tokens[j++].c_str());
		}

		m_nVmstar = atof(tokens[j++].c_str());
		m_nAnstar = atof(tokens[j++].c_str());

		return KPFA_SUCCESS;
	}

	return KPFA_ERROR_TRANSFORMER_PARSE;
}

/**
 * This function must be implemented to parse the input string into the transformer data.
 *
 * @param rInputString a string at line #3 with the transformer data to be parsed
 * @param nIdx index
 * @return error information
 */
KpfaError_t
KpfaTransformerData::ParseRawDataLineType3(string &rInputString, uint32_t nIdx) {

	KpfaStringList_t tokens;

	uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

#if 0
	KPFA_DEBUG("KpfaTransformerData Line #3", "%s\n", rInputString.c_str());
#endif

	if(nTokens != KPFA_NUM_TRANSFORMER_DATA_ITEMS_3_1 &&
	   nTokens != KPFA_NUM_TRANSFORMER_DATA_ITEMS_3_2) {
		return KPFA_ERROR_TRANSFORMER_PARSE;
	}

	m_nWindv[nIdx] = atof(tokens[0].c_str());
	m_nNomv[nIdx] = atof(tokens[1].c_str());
	m_nAng[nIdx] = atof(tokens[2].c_str());

	m_nRata[nIdx] = atof(tokens[3].c_str());
	m_nRatb[nIdx] = atof(tokens[4].c_str());
	m_nRatc[nIdx] = atof(tokens[5].c_str());

	if(nTokens == KPFA_NUM_TRANSFORMER_DATA_ITEMS_3_1) {
		m_nCod[nIdx] = (uint8_t)atoi(tokens[6].c_str());
		m_nCont[nIdx] = (uint32_t)atoi(tokens[7].c_str());

		m_nRma[nIdx] = atof(tokens[8].c_str());
		m_nRmi[nIdx] = atof(tokens[9].c_str());
		m_nVma[nIdx] = atof(tokens[10].c_str());
		m_nVmi[nIdx] = atof(tokens[11].c_str());

		m_nNtp[nIdx] = (uint32_t)atoi(tokens[12].c_str());
		m_nTap[nIdx] = (uint32_t)atoi(tokens[13].c_str());

		m_nCr[nIdx] = atof(tokens[14].c_str());
		m_nCx[nIdx] = atof(tokens[15].c_str());
	}

	return KPFA_SUCCESS;
}

/**
 * This function must be implemented to parse the input string into the transformer data.
 *
 * @param rInputString a string at line #4 with the transformer data to be parsed
 * @return error information
 */
KpfaError_t
KpfaTransformerData::ParseRawDataLineType4(string &rInputString) {

	KpfaStringList_t tokens;

	uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

#if 0
	KPFA_DEBUG("KpfaTransformerData Line #4", "%s\n", rInputString.c_str());
#endif

	if(nTokens != KPFA_NUM_TRANSFORMER_DATA_ITEMS_4) {
		return KPFA_ERROR_TRANSFORMER_PARSE;
	}

	m_nWindv[1] = atof(tokens[0].c_str());
	m_nNomv[1] = atof(tokens[1].c_str());

	return KPFA_SUCCESS;
}

/**
 * This function must be implemented to parse the input string into the transformer data.
 *
 * @param rInputString a string with the transformer data to be parsed
 * @param nId the type of the routine of parsing the input
 * @return error information
 */
KpfaError_t
KpfaTransformerData::ParseInput(string &rInputString, uint32_t nId) {

	switch(nId) {
		case 0:
			return ParseRawDataLineType1(rInputString);
		case 1:
			return ParseRawDataLineType2(rInputString);
		case 2:
			return ParseRawDataLineType3(rInputString, 0);
		case 3:
			if(m_nK != 0) {
				return ParseRawDataLineType3(rInputString, 1);
			}
			return ParseRawDataLineType4(rInputString);
		case 4:
			return ParseRawDataLineType3(rInputString, 2);
		default:
			return KPFA_ERROR_TRANSFORMER_PARSE;
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
KpfaTransformerData::TransformUnit(double nSbase) {

	if(nSbase == 0) {
		return KPFA_ERROR_INVALID_SBASE;
	}

	return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaTransformerData::Write(ostream &rOut) {

	rOut << "KPFA TRANSFORMER DATA: " << endl;

	rOut << "#1 Line ------------------------------" << endl;

	rOut << "I: " << m_nI << endl;

	rOut << "J: " << m_nJ << endl;

	rOut << "K: " << m_nK << endl;

	rOut << "CKT: " << m_nCkt << endl;

	rOut << "CW: " << (uint32_t)m_nCw << endl;

	rOut << "CZ: " << (uint32_t)m_nCz << endl;

	rOut << "CM: " << (uint32_t)m_nCm << endl;

	rOut << "MAG1: " << m_nMag1 << endl;

	rOut << "MAG2: " << m_nMag2 << endl;

	rOut << "NMETR: " << (uint32_t)m_nNmetr << endl;

	rOut << "NAME: " << m_rName << endl;

	rOut << "STAT: " << m_nStat << endl;

	for(uint32_t i = 0; i < m_nOwnerCount; i++) {

		rOut << "\tO" << i << ": " << m_nO[i] << endl;

		rOut << "\tF" << i << ": " << m_nF[i] << endl;
	}

	rOut << "#2 Line ------------------------------" << endl;

	const string windingStr[] = {
		"1-2", "2-3", "3-1"
	};

	for(uint32_t i = 0; i < m_nWindingCount; i++) {

		rOut << "R" << windingStr[i] << ":" << m_nR[i] << endl;

		rOut << "X" << windingStr[i] << ":" << m_nX[i] << endl;

		rOut << "SBASE" << windingStr[i] << ":" << m_nSbase[i] << endl;
	}

	rOut << "VMSTAR: " << m_nVmstar << endl;

	rOut << "ANSTAR: " << m_nAnstar << endl;

	// Line #3 (#4, #5)
	for(uint32_t i = 0; i < m_nWindingCount; i++) {

		rOut << "#" << (i+3) << " Line ------------------------------" << endl;

		uint32_t windingI = i + 1;

		rOut << "WINDV" << windingI << ": " <<  m_nWindv[i] << endl;

		rOut << "NOMV"  << windingI << ": " << m_nNomv[i] << endl;

		rOut << "ANG" << windingI << ": " <<  m_nAng[i] << endl;

		rOut << "RATA" << windingI << ": " << m_nRata[i] << endl;

		rOut << "RATB" << windingI << ": " << m_nRatb[i] << endl;

		rOut << "RATC" << windingI << ": " << m_nRatc[i] << endl;

		rOut << "COD" << windingI << ": " << (uint32_t)m_nCod[i] << endl;

		rOut << "CONT" << windingI << ": " << m_nCont[i] << endl;

		rOut << "RMA" << windingI << ": " << m_nRma[i] << endl;

		rOut << "RMI" << windingI << ": " << m_nRmi[i] << endl;

		rOut << "VMA" << windingI << ": " << m_nVma[i] << endl;

		rOut << "VMI" << windingI << ": " << m_nVmi[i] << endl;

		rOut << "NTP" << windingI << ": " << m_nNtp[i] << endl;

		rOut <<	"TAP" << windingI << ": " << m_nTap[i] << endl;

		rOut << "CR" << windingI << ": " << m_nCr[i] << endl;

		rOut << "CX" << windingI << ": " << m_nCx[i] << endl;
	}

	// Line #4 (Windings 2)
	if(m_nWindingCount == 1) {

		rOut << "#4 Line ------------------------------" << endl;

		rOut << "WINDV2: " <<  m_nWindv[1] << endl;

		rOut << "NOMV2: " << m_nNomv[1] << endl;
	}
}

ostream &operator << (ostream &rOut, KpfaTransformerData *pRawData) {

	pRawData->Write(rOut);
	return rOut;
}




