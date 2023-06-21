/*
 * KpfaBranchData.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_BRANCH_DATA_H_
#define _KPFA_BRANCH_DATA_H_

#include "KpfaRawData.h"

#define KPFA_MIN_BRANCH_RESISTANCE		0.00010

#define KPFA_MIN_NUM_BRANCH_DATA_ITEMS	17
#define KPFA_MAX_NUM_BRANCH_DATA_ITEMS	23

class KpfaBranchData: public KpfaRawData {

public:

	// Branch from bus number
	uint32_t m_nI;

	// Branch to bus number
	uint32_t m_nJ;

	// Branch circuit ID
	uint32_t m_nCkt;

	// Branch resistance
	double m_nR;

	// Branch reactance
	double m_nX;

	// Total branch charging susceptance
	double m_nB;

	// First, second, and third ratings
	double m_nRatea;
	double m_nRateb;
	double m_nRatec;

	// Complex admittance of the line shunt at the bus I end of the branch
	double m_nGi;
	double m_nBi;

	// Complex admittance of the line shunt at the bus J end of the branch
	double m_nGj;
	double m_nBj;

	// Branch status (0: out-of-service, 1: in-service)
	bool_t m_bSt;

	// Line length
	double m_nLen;

	// The number of owners
	uint32_t m_nOwnerCount;

	// Owner number (1 ~ 9999).
	uint16_t m_nO[KPFA_MAX_NUM_OWNERS];

	// Fraction of total ownership assigned to owner Qi
	double m_nF[KPFA_MAX_NUM_OWNERS];

	// Additional Info.
	double m_nTap;

	double m_nPflow;
	double m_nQflow;

	double m_nPflow_JI;
	double m_nQflow_JI;

public:

	KpfaBranchData();

	virtual ~KpfaBranchData();

	KpfaError_t ParseInput(string &rInputString, uint32_t nId = 0);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaBranchData *pRawData);
};

#endif /* _KPFA_BRANCH_DATA_H_ */
