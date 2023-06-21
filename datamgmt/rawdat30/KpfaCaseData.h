/*
 * KpfaCaseData.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_CASE_ID_DATA_H_
#define _KPFA_CASE_ID_DATA_H_

#include "KpfaRawData.h"

#define KPFA_NUM_CASE_DATA_ITEMS 7

class KpfaCaseData: public KpfaRawData {

public:

	// Case flag
	bool_t m_bIc;

	// System MVA base
	double m_nSbase;

	// PSSE revision number
	uint8_t m_nRev;

	// Units of transformer ratings
	double m_nXfrrat;

	// Units of ratings of non-transformer branches
	double m_nNxfrat;

	// System base frequency in Hertz
	double m_nBasfrq;

public:

	KpfaCaseData();

	virtual ~KpfaCaseData();

	KpfaError_t ParseInput(string &rInputString, uint32_t nId = 0);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaCaseData *pRawData);
};

#endif /* _KPFA_CASE_ID_DATA_H_ */
