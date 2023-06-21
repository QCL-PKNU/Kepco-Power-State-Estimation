/*
 * KpfaBranchData.cpp
 *
 *  Created on: 2014. 5. 21.
 *      Author: Youngsun Han
 */

#include "KpfaBranchData.h"

using namespace std;

KpfaBranchData::KpfaBranchData()
:KpfaRawData(KPFA_RAW_BRANCH){

#ifdef RAW_DATA_INITIALIZED
  // No default allowed
  m_nI = 0;
  m_nJ = 0;

  // Set as the default values
  m_nCkt = 1;
  m_nR = 0.0;
  m_nX = 0.0;
  m_nB = 0.0;
  m_nRatea = 0.0;
  m_nRateb = 0.0;
  m_nRatec = 0.0;
  m_nGi = 0.0;
  m_nBi = 0.0;
  m_nGj = 0.0;
  m_nBj = 0.0;
  m_bSt = 1;
  m_nLen = 0.0;

  m_nOwnerCount = 0;

  // Set as the values from the bus
  for(int i = 0; i < KPFA_MAX_NUM_OWNERS; i++) {
    m_nO[i] = 0;
    m_nF[i] = 1.0;
  }
#endif

  m_nTap = 1.0;

  m_nPflow = 0.0;
  m_nQflow = 0.0;

  m_nPflow_JI = 0.0;
  m_nQflow_JI = 0.0;
}

KpfaBranchData::~KpfaBranchData() {
  // Do nothing
}

/**
 * This function must be implemented to parse the input string into the branch data.
 *
 * @param rInputString a string with the branch data to be parsed
 * @return error information
 */
KpfaError_t
KpfaBranchData::ParseInput(string &rInputString, uint32_t nId) {

  uint32_t i = 0, j;
  KpfaStringList_t tokens;
  uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

  if(nTokens < KPFA_MIN_NUM_BRANCH_DATA_ITEMS ||
    nTokens > KPFA_MAX_NUM_BRANCH_DATA_ITEMS) {
    return KPFA_ERROR_BRANCH_PARSE;
  }

#if 0
  KPFA_DEBUG("KpfaBranchData", "%s\n", rInputString.c_str());
#endif

  // Branch from bus number
  m_nI = (uint32_t)abs(atoi(tokens[i++].c_str()));

  if(m_nI == 0) {
    return KPFA_ERROR_BRANCH_PARSE;
  }

  // Branch to bus number
  m_nJ = (uint32_t)abs(atoi(tokens[i++].c_str()));

  // Branch circuit ID
#if 0
  char *ckt = tokens[i++].c_str();
  m_nCkt = (uint32_t)(ckt[1] - '0');
#else
  string &ckt = KpfaStripQuotes(tokens[i++]);
  m_nCkt = (uint32_t)atoi(ckt.c_str());
#endif

  // Branch resistance
  m_nR = atof(tokens[i++].c_str());

  // Branch reactance
  m_nX = atof(tokens[i++].c_str());

  // Total branch charging susceptance
  m_nB = atof(tokens[i++].c_str());

  // First, second, and third ratings
  m_nRatea = atof(tokens[i++].c_str());
  m_nRateb = atof(tokens[i++].c_str());
  m_nRatec = atof(tokens[i++].c_str());

  // Complex admittance of the line shunt at the bus I end of the branch
  m_nGi = atof(tokens[i++].c_str());
  m_nBi = atof(tokens[i++].c_str());

  // Complex admittance of the line shunt at the bus J end of the branch
  m_nGj = atof(tokens[i++].c_str());
  m_nBj = atof(tokens[i++].c_str());

  // Branch status (0: out-of-service, 1: in-service)
  m_bSt = (bool_t)atoi(tokens[i++].c_str());

  // Line length
  m_nLen = atof(tokens[i++].c_str());

  // The number of owners
  for(j = 0; i < nTokens; j++) {

  // Owner number (1 ~ 9999).
  m_nO[j] = (uint16_t)atoi(tokens[i++].c_str());

  // Fraction of total ownership assigned to owner Qi
  m_nF[j] = atof(tokens[i++].c_str());
  }

  m_nOwnerCount = j;

  return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaBranchData::Write(ostream &rOut) {

	rOut << "KPFA BRANCH DATA: " << endl;

	rOut << "\tI: " << m_nI << endl;

	rOut << "\tJ: " << m_nJ << endl;

	rOut << "\tCKT: " << m_nCkt << endl;

	rOut << "\tR: " << m_nR << endl;

	rOut << "\tX: " << m_nX << endl;

	rOut << "\tB: " << m_nB << endl;

	rOut << "\tRATEA: " << m_nRatea << endl;

	rOut << "\tRATEB: " << m_nRateb << endl;

	rOut << "\tRATEC: " << m_nRatec << endl;

	rOut << "\tGi: " << m_nGi << endl;

	rOut << "\tBi: " << m_nBi << endl;

	rOut << "\tGj: " << m_nGj << endl;

	rOut << "\tBj: " << m_nBj << endl;

	rOut << "\tST: " << (uint32_t)m_bSt << endl;

	rOut << "\tLEN: " << m_nLen << endl;

	for(uint32_t i = 0; i < m_nOwnerCount; i++) {

		rOut << "\tO" << i << ": " << m_nO[i] << endl;

		rOut << "\tF" << i << ": " << m_nF[i] << endl;
	}
}

ostream &operator << (ostream &rOut, KpfaBranchData *pRawData) {
	pRawData->Write(rOut);
	return rOut;
}




