/*
 * KpfaAreaData.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_AREA_DATA_H_
#define _KPFA_AREA_DATA_H_

#include "KpfaRawData.h"

#define KPFA_NUM_AREA_DATA_ITEMS	5

class KpfaAreaData: public KpfaRawData {

public:

	// Bus number
	uint32_t m_nI;

	// Area slack bus number
	uint32_t m_nIsw;

	// Desired MW area interchange
	double m_nPdes;

	// Desired MW interchange tolerance
	double m_nPtol;

	// Area name
	string m_rName;

public:

	KpfaAreaData();

	virtual ~KpfaAreaData();

	KpfaError_t ParseInput(string &rInputString, uint32_t nId = 0);

	KpfaError_t TransformUnit(double nSbase);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaAreaData *pRawData);
};

#endif /* _KPFA_AREA_DATA_H_ */
