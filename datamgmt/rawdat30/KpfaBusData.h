/*
 * KpfaBusData.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_BUS_DATA_H_
#define _KPFA_BUS_DATA_H_

#include "KpfaRawData.h"

#define KPFA_NUM_BUS_DATA_ITEMS 11

class KpfaBusData: public KpfaRawData {

public:

	// Bus number (1 ~ 999997)
	uint32_t m_nI;

	// Bus name (up to 12 characters)
	string m_rName;

	// Bus base voltage
	double m_nBaskv;

	// Bus type code
	KpfaBusType_t m_nIde;

	// Gl (Ignored), Bl
	double m_nGl;
	double m_nBl;

	// Area number (1 ~ 9999)
	uint16_t m_nArea;

	// Zone number (1 ~ 9999)
	uint16_t m_nZone;

	// Bus voltage magnitude
	double m_nVm;

	// Bus voltage phase angle
	double m_nVa;

	// Owner number (1 ~ 9999)
	uint16_t m_nOwner;

	// Additional Info.
	///////////////////////////////////////////////

	uint32_t m_nIdx;

	//For load
	double m_nPl;
	double m_nQl;

	//For generator
	double m_nPg;
	double m_nQg;

	double m_nQt;
	double m_nQb;

	//For restoring the original data
	double m_nOrigVm;
	KpfaBusType_t m_nOrigIde;

public:

	KpfaBusData();

	virtual ~KpfaBusData();

	virtual KpfaError_t ParseInput(string &rInputString, uint32_t nId = 0);

	KpfaError_t TransformUnit(double nSbase);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaBusData *pRawData);
};

#endif /* _KPFA_BUS_DATA_H_ */
