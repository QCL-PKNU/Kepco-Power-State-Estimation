/*
 * KpfaVscData.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_VSC_DATA_H_
#define _KPFA_VSC_DATA_H_

#include "KpfaRawData.h"

#define KPFA_NUM_VSC_DATA_ITEMS		18

class KpfaVscData: public KpfaRawData {

public:

public:

	KpfaVscData();

	virtual ~KpfaVscData();

	KpfaError_t ParseInput(string &rInputString, uint32_t nId = 0);

	KpfaError_t TransformUnit(double nSbase);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaVscData *pRawData);
};

#endif /* _KPFA_VSC_DATA_H_ */
