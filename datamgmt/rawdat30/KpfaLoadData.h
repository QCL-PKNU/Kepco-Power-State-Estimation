/*
 * KpfaLoadData.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_LOAD_DATA_H_
#define _KPFA_LOAD_DATA_H_

#include "KpfaRawData.h"

#define KPFA_HVDC_1_BID		121
#define KPFA_HVDC_2_BID		251
#define KPFA_HVDC_1_BID_2nd	122
#define KPFA_HVDC_2_BID_2nd	252

#define KPFA_NUM_LOAD_DATA_ITEMS	12

class KpfaLoadData: public KpfaRawData {

public:

	// Bus number
	uint32_t m_nI;

	// Sub Load ID to distinguish among multiple loads at a bus.
	string m_rId;

	// Load status (0: out-of-service, 1: in-service)
	bool_t m_bStatus;

	// Area to which the load is assigned (1 ~ 9999)
	uint16_t m_nArea;

	// Zone to which the load is assigned (1 ~ 9999)
	uint16_t m_nZone;

	// Active power of MVA load
	double m_nPl;

	// Reactive power of MVA load
	double m_nQl;

	// Active power of current load
	double m_nIp;

	// Reactive power of current load
	double m_nIq;

	// Active power of admittance load
	double m_nYp;

	// Reactive power of admittance load
	double m_nYq;

	// Owner to which the load is assigned (1 ~ 9999)
	uint16_t m_nOwner;

	// HVDC
	uint32_t m_nHvdcI;

public:

	KpfaLoadData();

	virtual ~KpfaLoadData();

	KpfaError_t ParseInput(string &rInputString, uint32_t nId = 0);

	KpfaError_t TransformUnit(double nSbase);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaLoadData *pRawData);
};

#endif /* _KPFA_LOAD_DATA_H_ */
