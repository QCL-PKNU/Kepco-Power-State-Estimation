/*
 * KpfaGenData.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_GEN_DATA_H_
#define _KPFA_GEN_DATA_H_

#include "KpfaRawData.h"

#if KPFA_RAW_DATA_VERSION < 35
	#define KPFA_MIN_NUM_GENERATOR_DATA_ITEMS		20
	#define KPFA_MAX_NUM_GENERATOR_DATA_ITEMS		26
#else
	#define KPFA_MIN_NUM_GENERATOR_DATA_ITEMS		21
	#define KPFA_MAX_NUM_GENERATOR_DATA_ITEMS		27
#endif

class KpfaGenData: public KpfaRawData {

public:

	// Bus number
	uint32_t m_nI;

	// Machine identifier used to distinguish among multiple machines at bus I
	string m_rId;

	// Generator active power output
	double m_nPg;

	// Generator reactive power output
	double m_nQg;

	// Maximum generator reactive power output
	double m_nQt;

	// Minimum generator reactive power output
	double m_nQb;

	// Regulated voltage set-point
	double m_nVs;

	// Sub bus number
	uint32_t m_nIreg;

	// Total MVA base of the units represented by this machine
	double m_nMbase;

	// Complex machine impedance
	double m_nZr;
	double m_nZx;

	// Step-up transformer impedance
	double m_nRt;
	double m_nXt;

	// Step-up transformer off-nominal turns ratio
	double m_nGtap;

	// Machine status (1: in-service, 0: out-of-service)
	bool_t m_bStat;

	// Percent of the total Mvar required
	double m_nRmpct;

	// Maximum generator active power output
	double m_nPt;

	// Minimum generator active power output
	double m_nPb;

	// The number of owners
	uint32_t m_nOwnerCount;

	// Owner number (1 ~ 9999).
	uint16_t m_nO[KPFA_MAX_NUM_OWNERS];

	// Fraction of total ownership assigned to owner Qi
	double m_nF[KPFA_MAX_NUM_OWNERS];

	// Wind machine control mode
	uint8_t m_nWmod;

	// Qt overflow (EXTENDED)
	bool_t m_bQtx;

	// Qb underflow (EXTENDED)
	bool_t m_bQbx;

	// FACTS on/off
	bool_t m_bFacts;

	// Weighting factor
	double m_nWFactor[2];

public:

	KpfaGenData();

	virtual ~KpfaGenData();

	KpfaError_t ParseInput(string &rInputString, uint32_t nId = 0);

	KpfaError_t TransformUnit(double nSbase);

	void MergeWith(KpfaGenData *pGenData);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaGenData *pRawData);
};

#endif /* _KPFA_GENERATOR_DATA_H_ */
