/*
 * KpfaTwoTermData.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_TWO_TERM_DATA_H_
#define _KPFA_TWO_TERM_DATA_H_

#include "KpfaRawData.h"

#define KPFA_NUM_TWO_TERM_DATA_ITEMS_1		12
#define KPFA_NUM_TWO_TERM_DATA_ITEMS_2		17
#define KPFA_NUM_TWO_TERM_DATA_ITEMS_3		18

class KpfaTwoTermData: public KpfaRawData {

public:

public:

	KpfaTwoTermData();

	virtual ~KpfaTwoTermData();

	KpfaError_t ParseInput(string &rInputString, uint32_t nId = 0);

	KpfaError_t TransformUnit(double nSbase);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaTwoTermData *pRawData);
};

#endif /* _KPFA_TWO_TERM_DATA_H_ */
