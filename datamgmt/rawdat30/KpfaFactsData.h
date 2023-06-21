/*
 * KpfaFactsData.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_FACTS_DATA_H_
#define _KPFA_FACTS_DATA_H_

#include "KpfaRawData.h"

#define KPFA_NUM_FACTS_DATA_ITEMS	19

class KpfaFactsData: public KpfaRawData {

public:

	// Facts ID
	uint32_t m_nN;

	// Bus number (I)
	uint32_t m_nI;

	// Bus number (J)
	uint32_t m_nJ;

	// FACTS mode
	uint32_t m_nMode;

	// Desired P
	double m_nPdes;

	// Desired Q
	double m_nQdes;

	// V set
	double m_nVset;

	// Shunt max
	double m_nShmx;

	double m_nTrmx;

	// Maximum Vt
	double m_nVtmn;

	// Minimum Vt
	double m_nVtmx;

	double m_nImx;

	double m_nLinx;

	// Percent of contributed reactive power (100.0 by default)
	double m_nRmpct;

	// Owner to which the facts is assigned (1 ~ 9999)
	uint32_t m_nOwner;

	double m_nSet1;

	double m_nSet2;

	double m_nVsref;

	double m_nQout;

	// addition information
	double m_nRequiredQ;

	double m_nSensitivity;

public:

	KpfaFactsData();

	virtual ~KpfaFactsData();

	KpfaError_t ParseInput(string &rInputString, uint32_t nId = 0);

	KpfaError_t TransformUnit(double nSbase);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaFactsData *pRawData);
};

#endif /* _KPFA_AREA_DATA_H_ */
