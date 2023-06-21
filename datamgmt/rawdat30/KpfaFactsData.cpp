/*
 * KpfaFactsData.cpp
 *
 *  Created on: 2014. 5. 21.
 *      Author: Youngsun Han
 */

#include "KpfaFactsData.h"

KpfaFactsData::KpfaFactsData()
:KpfaRawData(KPFA_RAW_FACTS){

#ifdef RAW_DATA_INITIALIZED
    // No default allowed
	m_nN = 0;
	m_nI = 0;
	m_nJ = 0;

	m_nMode = 0;

	m_nPdes = 0;
	m_nQdes = 0;
	m_nVset = 0;
	m_nShmx = 0;
	m_nTrmx = 0;
	m_nVtmn = 0;
	m_nVtmx = 0;
	m_nImx  = 0;
	m_nLinx = 0;
	m_nRmpct = 0;

	m_nOwner = 0;

	m_nSet1 = 0;
	m_nSet2 = 0;
	m_nVsref = 0;

	m_nRequiredQ = 0;
	m_nSensitivity = 0;
#endif
}

KpfaFactsData::~KpfaFactsData() {
  // Do nothing
}

/**
 * This function must be implemented to parse the input string into the load data.
 *
 * @param rInputString a string with the load data to be parsed
 * @return error information
 */
KpfaError_t
KpfaFactsData::ParseInput(string &rInputString, uint32_t nId) {

  KpfaStringList_t tokens;
  uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

  if(nTokens != KPFA_NUM_FACTS_DATA_ITEMS) {
    return KPFA_ERROR_FACTS_PARSE;
  }

#if 0
  KPFA_DEBUG("KpfaFactsData", "%s\n", rInputString.c_str());
#endif

  uint32_t i = 0;

  // Facts ID
  m_nN = (uint32_t)atoi(tokens[i++].c_str());

  // Bus number (1 ~ 999997)
  m_nI = (uint32_t)atoi(tokens[i++].c_str());

  if(m_nI == 0) {
    return KPFA_ERROR_FACTS_PARSE;
  }

  m_nJ = (uint32_t)atoi(tokens[i++].c_str());

  m_nMode = (uint32_t)atoi(tokens[i++].c_str());

  m_nPdes = atof(tokens[i++].c_str());
  m_nQdes = atof(tokens[i++].c_str());
  m_nVset = atof(tokens[i++].c_str());
  m_nShmx = atof(tokens[i++].c_str());

#if 0
  m_nTrmx = atof(tokens[i++].c_str());
  m_nVtmn = atof(tokens[i++].c_str());
  m_nVtmx = atof(tokens[i++].c_str());
  m_nImx  = atof(tokens[i++].c_str());
  m_nLinx = atof(tokens[i++].c_str());
  m_nRmpct= atof(tokens[i++].c_str());

  m_nOwner= (uint32_t)atoi(tokens[i++].c_str());

  m_nSet1 = atof(tokens[i++].c_str());
  m_nSet2 = atof(tokens[i++].c_str());
  m_nVsref= atof(tokens[i++].c_str());
  m_nQout = atof(tokens[i++].c_str());
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
KpfaFactsData::TransformUnit(double nSbase) {
  return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaFactsData::Write(ostream &rOut) {

  rOut << "KPFA FACTS DATA: " << endl;

  rOut << "Facts ID: " << m_nN << endl;

  rOut << "Bus ID: " << m_nI << endl;

  rOut << "Mode: " << m_nMode << endl;

  rOut << "Maximum Shunt: " << m_nShmx << endl;
}

ostream &operator << (ostream &rOut, KpfaFactsData *pRawData) {

  pRawData->Write(rOut);
  return rOut;
}
