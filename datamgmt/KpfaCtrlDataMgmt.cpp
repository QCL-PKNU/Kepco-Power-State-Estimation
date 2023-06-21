/*
 * KpfaCtrlDataMgmt.cpp
 *
 *  Created on: Oct 13, 2015
 *      Author: youngsun
 */

#include <KpfaCtrlDataMgmt.h>

KpfaCtrlDataMgmt::KpfaCtrlDataMgmt() {

	m_bAdjustTap = FALSE;
	m_bAdjustShunt = FALSE;
	m_bFlatStart = FALSE;
	m_nMaxIteration = 0;
	m_nHvdcFreqControl = 0;
	m_nTolerence = 0.0f;
	m_nEqrMarginL = 0.0f;
	m_nEqrMarginC = 0.0f;
	m_nGstep = 0.0f;

	m_nIncrement0 = 1;
	m_nIncrement1 = 1;

	m_rFactsParamList.clear();
}

KpfaCtrlDataMgmt::~KpfaCtrlDataMgmt() {
	// Do nothing
}

/**
* This function will be used to read a file including control data on the given path.
*
* @param pFilePath file path
* @return error information
*/

#define _NOT_FOUND(STR)  (linebuf.find(STR) == string::npos)
#define _FOUND(STR)		 (linebuf.find(STR) != string::npos)

KpfaError_t
KpfaCtrlDataMgmt::ReadCtrlDataFile(const char *pFilePath) {

	string linebuf;
	KpfaStringList_t tokens;

    // open the control file with the given path
	ifstream ctrlfile(pFilePath);

	if(!ctrlfile.is_open()) {
		cerr << "File Not Open: " << pFilePath << endl;
		return KPFA_ERROR_FILE_OPEN;
	}

	while(!getline(ctrlfile, linebuf).fail()) {
		// find the start of control data
		if(_FOUND(KPFA_CTRL_TAG_START)) break;
	}

	while(!getline(ctrlfile, linebuf).fail()) {
		// find the end of control data
		if(_FOUND(KPFA_CTRL_TAG_END)) break;

		// parse each control data item
		////////////////////////////////////////////////////////
		if(KpfaTokenize(linebuf, tokens, "=") != 2) {
			return KPFA_ERROR_CONTROL_PARAM_PARSE;
		}

		if(tokens[0] == KPFA_CTRL_TAG_TAPOP) {
			m_bAdjustTap = (tokens[1] == "T") ? TRUE : FALSE;
		}
		else if(tokens[0] == KPFA_CTRL_TAG_SHUNTOP) {
			m_bAdjustShunt = (tokens[1] == "T") ? TRUE : FALSE;
		}
		else if(tokens[0] == KPFA_CTRL_TAG_FLATSTART) {
			m_bAdjustShunt = (tokens[1] == "T") ? TRUE : FALSE;
		}
		else if(tokens[0] == KPFA_CTRL_TAG_HVDCFREQ) {
			m_nHvdcFreqControl = (uint32_t)atoi(tokens[1].c_str());
		}
		else if(tokens[0] == KPFA_CTRL_TAG_MAXITER) {
			m_nMaxIteration = (uint32_t)atoi(tokens[1].c_str());
		}
		else if(tokens[0] == KPFA_CTRL_TAG_TOLERANCE) {
			m_nTolerence = atof(tokens[1].c_str());
		}
		else if(tokens[0] == KPFA_CTRL_TAG_GSTEP) {
			m_nGstep = atof(tokens[1].c_str());
		}
		else if(tokens[0] == KPFA_CTRL_TAG_EQRMARGIN_L) {
			m_nEqrMarginL = atof(tokens[1].c_str());
		}
		else if(tokens[0] == KPFA_CTRL_TAG_EQRMARGIN_C) {
			m_nEqrMarginC = atof(tokens[1].c_str());
		}
		else if(tokens[0] == KPFA_CTRL_TAG_INCREMENT) {

			KpfaStringList_t subtokens;

			if(KpfaTokenize(tokens[1], subtokens, ",") != 2) {
				return KPFA_ERROR_CONTROL_PARAM_PARSE;
			}

			m_nIncrement0 = (uint32_t)atoi(subtokens[0].c_str());
			m_nIncrement1 = (uint32_t)atoi(subtokens[1].c_str());
		}
		else {
			return KPFA_ERROR_CONTROL_UNKNOWN_PARAM;
		}
		////////////////////////////////////////////////////////
	}

	ctrlfile.close();

	return KPFA_SUCCESS;
}

#undef _NOT_FOUND
#undef _FOUND

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaCtrlDataMgmt::Write(ostream &rOut) {

	rOut << ">> Control parameters: " << endl;

	rOut << "Adjust tap: " << m_bAdjustTap << endl;

	rOut << "Adjust shunt: " << m_bAdjustShunt << endl;

	rOut << "Flat start: " << m_bFlatStart << endl;

	rOut << "Maximum iteration: " << m_nMaxIteration << endl;

	rOut << "HVDC frequency control: " << m_nHvdcFreqControl << endl;

	rOut << "Tolerance: " << m_nTolerence << endl;

	rOut << "EQR margin (L): " << m_nEqrMarginL << endl;

	rOut << "EQR margin (C): " << m_nEqrMarginC << endl;

	rOut << "G-step: " << m_nGstep << endl;

	rOut << "Increment 0: " << m_nIncrement0 << endl;

	rOut << "Increment 1: " << m_nIncrement1 << endl;
}

ostream &operator << (ostream &rOut, KpfaCtrlDataMgmt *pDataMgmt) {
	pDataMgmt->Write(rOut);
	return rOut;
}
