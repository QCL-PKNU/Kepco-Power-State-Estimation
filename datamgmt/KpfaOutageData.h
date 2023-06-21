/*
 * KpfaOutageData.h
 *
 * The file will contains the functions to read and parse a outage data.
 *
 *  Created on: 2015. 5. 25.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_OUTAGE_DATA_H_
#define _KPFA_OUTAGE_DATA_H_

#include "KpfaDebug.h"
#include "KpfaConfig.h"
#include "KpfaUtility.h"

/**
 * Outage data types
 */
typedef enum {

	KPFA_OUTAGE_BUS = 0,
	KPFA_OUTAGE_GEN,
	KPFA_OUTAGE_BRANCH,
	KPFA_OUTAGE_TRANSFORMER,
	KPFA_OUTAGE_WIND,
	KPFA_OUTAGE_HVDC,

	// The number of outage types
	KPFA_NUM_OF_OUTAGE_TYPES,

} KpfaOutageDataType_t;

/**
 * Outgae tag
 */
#define KPFA_OTG_TAG_BUS    "bus"
#define KPFA_OTG_TAG_BRANCH "branch"
#define KPFA_OTG_TAG_GEN    "generator"
#define KPFA_OTG_TAG_TRANS  "transformer"
#define KPFA_OTG_TAG_WIND   "wind"
#define KPFA_OTG_TAG_HVDC   "hvdc"

/**
 * Outage data class
 */
class KpfaOutageData {

private:

	// Outage Data Type
	KpfaOutageDataType_t m_nDataType;

public:
        
    // Bus number (I, J, K)
    uint32_t m_nI;
    uint32_t m_nJ;
    uint32_t m_nK;

    // Circuit ID
    uint32_t m_nCkt;

    // Wind rate
    double m_nWindRate;

    // HVDC type
    uint32_t m_nHvdcType;

    // Original state for restoration
    uint32_t m_nOrigState;

	double m_nOrigPg;
	double m_nOrigQg;

	double m_nOrigPl;
	double m_nOrigQl;
    
public:
            
	KpfaOutageData();

	virtual ~KpfaOutageData();

	/**
	 * This function will return the data type of the outage data.
	 *
	 * @return the outage data type
	 */
	inline KpfaOutageDataType_t GetDataType() {
		return m_nDataType;
	}

	/**
	 * This function will parse a string including outage data.
	 *
	 * @param rInputString a string with outage data
	 * @return error information
	 */
	KpfaError_t ParseOutageData(string &rInputString);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaOutageData *pOutageData);

private:

    KpfaError_t ParseOutageBus(string &rInputString);

    KpfaError_t ParseOutageBranch(string &rInputString);

    KpfaError_t ParseOutageGenerator(string &rInputString);

    KpfaError_t ParseOutageTransformer(string &rInputString);

    KpfaError_t ParseOutageWind(string &rInputString);

    KpfaError_t ParseOutageHvdc(string &rInputString);    
    
};

#endif /* _KPFA_OUTAGE_DATA_H_ */
