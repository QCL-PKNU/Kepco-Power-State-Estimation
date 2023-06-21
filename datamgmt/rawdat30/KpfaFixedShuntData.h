/*
 * KpfaFixedShuntData.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_FIXED_SHUNT_DATA_H_
#define _KPFA_FIXED_SHUNT_DATA_H_

#include "KpfaRawData.h"

#define KPFA_NUM_FIXED_SHUNT_DATA_ITEMS 5

class KpfaFixedShuntData: public KpfaRawData {

public:

	// Bus number
	uint32_t m_nI;

	// Shunt ID
	string m_rId;

	// Shunt status (1: in-service, 0: out-of-service)
	bool_t m_bStatus;

	// Active component of shunt admittance to ground
	double m_nGl;

	// Reactive component of shunt admittance to ground
	double m_nBl;

public:

	KpfaFixedShuntData();

	virtual ~KpfaFixedShuntData();

	KpfaError_t ParseInput(string &rInputString, uint32_t nId = 0);

	KpfaError_t TransformUnit(double nSbase);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaFixedShuntData *pRawData);
};

#endif /* _KPFA_FIXED_SHUNT_DATA_H_ */
