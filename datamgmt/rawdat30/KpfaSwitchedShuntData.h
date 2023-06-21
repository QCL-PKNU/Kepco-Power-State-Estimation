/*
 * KpfaSwitchedShuntData.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_SWITCHED_SHUNT_DATA_H_
#define _KPFA_SWITCHED_SHUNT_DATA_H_

#include "KpfaRawData.h"

#define KPFA_SHUNT_MODE		1
#define KPFA_REACTOR_MODE	2
#define KPFA_MIXED_MODE		3

#define KPFA_NUM_SWITCHED_SHUNT_BLOCKS				8
#define KPFA_MIN_NUM_SWITCHED_SHUNT_DATA_ITEMS		8
#define KPFA_MAX_NUM_SWITCHED_SHUNT_DATA_ITEMS		24

class KpfaSwitchedShuntData: public KpfaRawData {

public:

	// Bus number
	uint32_t m_nI;

	// Control mode
	uint16_t m_nModsw;

	// Desired voltage upper limit (1.0 by default)
	double m_nVswhi;

	// Desired voltage lower limit (1.0 by default)
	double m_nVswlo;

	// Remote bus number
	uint32_t m_nSwrem;

	// Percent of contributed reactive power (100.0 by default)
	double m_nRmpct;

	// VSC DC line name (MODSW = 4) or FACTS device name (MODSW = 6)
	string m_rRmidnt;

	// Switched shunt admittance
	double m_nBinit;

	// Number of steps for each block
	uint32_t m_nN[KPFA_NUM_SWITCHED_SHUNT_BLOCKS];

	// Admittance increment per step for each block
	double m_nB[KPFA_NUM_SWITCHED_SHUNT_BLOCKS];

	// Number of blocks
	uint32_t m_nBlockNum;

	// Maximum shunt value
	double m_nMaxShunt;

	// Minimum shunt value
	double m_nMinShunt;

	// Bank Shunt
	std::vector<double> m_rBankShunt;

	// Shunt mode
	uint16_t m_nShuntMode;

public:

	KpfaSwitchedShuntData();

	virtual ~KpfaSwitchedShuntData();

	KpfaError_t ParseInput(string &rInputString, uint32_t nId = 0);

	KpfaError_t TransformUnit(double nSbase);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaSwitchedShuntData *pRawData);
};

#endif /* _KPFA_SWITCHED_SHUNT_DATA_H_ */
