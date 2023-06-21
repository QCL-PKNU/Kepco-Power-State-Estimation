/*
 * KpfaAreaData.cpp
 *
 *  Created on: 2014. 5. 21.
 *      Author: Youngsun Han
 */

#include "KpfaAreaData.h"

KpfaAreaData::KpfaAreaData()
:KpfaRawData(KPFA_RAW_AREA_INTERCHANGE){

#ifdef RAW_DATA_INITIALIZED
  // No default allowed
  m_nI = 0;
  m_nIsw = 0;
  m_nPdes = 0;
  m_nPtol = 0;
#endif
}

KpfaAreaData::~KpfaAreaData() {
  // Do nothing
}

/**
 * This function must be implemented to parse the input string into the load data.
 *
 * @param rInputString a string with the load data to be parsed
 * @return error information
 */
KpfaError_t
KpfaAreaData::ParseInput(string &rInputString, uint32_t nId) {

  KpfaStringList_t tokens;
  uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

  if(nTokens != KPFA_NUM_AREA_DATA_ITEMS) {
    return KPFA_ERROR_AREA_PARSE;
  }

#if 0
  KPFA_DEBUG("KpfaAreaData", "%s\n", rInputString.c_str());
#endif

  // Bus number (1 ~ 999997)
  m_nI = (uint32_t)atoi(tokens[0].c_str());

  if(m_nI == 0) {
    return KPFA_ERROR_AREA_PARSE;
  }

  // Area slack bus number
  m_nIsw = (uint32_t)atoi(tokens[1].c_str());

  // Desired MW area interchange
  m_nPdes = atof(tokens[2].c_str());

  // Desired MW interchange tolerance
  m_nPtol = atof(tokens[3].c_str());

  // Area name
  m_rName = tokens[4];

  return KPFA_SUCCESS;
}

/**
 * This function will be used to transform the units of the values of the raw data.
 *
 * @param nSbase System MVA base
 * @return error information
 */
KpfaError_t
KpfaAreaData::TransformUnit(double nSbase) {
  return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaAreaData::Write(ostream &rOut) {

  rOut << "KPFA INTERCHG DATA: " << endl;

  rOut << "\tI: " << m_nI << endl;

  rOut << "\tSW: " << m_nIsw << endl;

  rOut << "\tPDES: " << m_nPdes << endl;

  rOut << "\tPTOL: " << m_nPtol << endl;

  rOut << "\tNAME: " << m_rName << endl;
}

ostream &operator << (ostream &rOut, KpfaAreaData *pRawData) {

  pRawData->Write(rOut);
  return rOut;
}
