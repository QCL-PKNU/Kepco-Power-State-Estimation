/*
 * KpfaCtrlDataMgmt.h
 *
 *  Created on: Oct 13, 2015
 *      Author: youngsun
 */

#ifndef _KPFA_CTRL_DATA_MGMT_H_
#define _KPFA_CTRL_DATA_MGMT_H_

#include "KpfaDebug.h"
#include "KpfaConfig.h"
#include "KpfaUtility.h"

/**
 * Control tag
 */
#define KPFA_CTRL_TAG_START  		"{Control parameter data}"
#define KPFA_CTRL_TAG_END    		"{End of control parameter data}"

#define KPFA_CTRL_TAG_TAPOP    		"TAPOP"
#define KPFA_CTRL_TAG_SHUNTOP		"SWITCHEDSHUNTOP"
#define KPFA_CTRL_TAG_FLATSTART		"FLATSTART"
#define KPFA_CTRL_TAG_TOLERANCE		"CONVERGENCETOLERANCE"
#define KPFA_CTRL_TAG_MAXITER   	"MAXITERATION"
#define KPFA_CTRL_TAG_GSTEP   		"GSTEP"
#define KPFA_CTRL_TAG_EQRMARGIN_L  	"EQRMARGIN_L"
#define KPFA_CTRL_TAG_EQRMARGIN_C  	"EQRMARGIN_C"
#define KPFA_CTRL_TAG_HVDCFREQ   	"HVDCFREQCONTROL"
#define KPFA_CTRL_TAG_INCREMENT   	"INCREMENT"

/**
 * Facts control parameter class
 */
class KpfaFactsParam {

public:
	// Bus ID
	uint32_t m_nId;

	// Name
	string m_rName;

	// Qmin, Qmax
	double m_nQb;
	double m_nQt;

	double m_nQg;

	KpfaFactsParam() {
		m_nId = 0;
		m_rName = "";
		m_nQb = 0;
		m_nQt = 0;
		m_nQg = 0;
	}

	~KpfaFactsParam() {
		//do nothing
	}
};

/**
 * The declaration of the class for Control Data Management
 */
class KpfaCtrlDataMgmt {

public:

	// Adjust tap
	bool_t m_bAdjustTap;

	// Adjust shunt
	bool_t m_bAdjustShunt;

	// Flat start
	bool_t m_bFlatStart;

	// Maximum iteration
	uint32_t m_nMaxIteration;

	// HVDC frequence control
	uint32_t m_nHvdcFreqControl;

	// Tolerance
	double m_nTolerence;

	// EQR margin
	double m_nEqrMarginL;
	double m_nEqrMarginC;

	// G-step
	double m_nGstep;

	// Increment step 0, 1
	uint32_t m_nIncrement0;
	uint32_t m_nIncrement1;

	// Facts Control Parameters
	std::vector<KpfaFactsParam> m_rFactsParamList;

public:

	KpfaCtrlDataMgmt();

	virtual ~KpfaCtrlDataMgmt();

	KpfaError_t ReadCtrlDataFile(const char *pFilePath);

	/**
	 * This function will return theFACTS parameters.
	 *
	 * @return the FACTS parameters
	 */
	inline std::vector<KpfaFactsParam> &GetFactsParamList() {
		return m_rFactsParamList;
	}

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaCtrlDataMgmt *pDataMgmt);
};

#endif /* _KPFA_CTRL_DATA_MGMT_H_ */
