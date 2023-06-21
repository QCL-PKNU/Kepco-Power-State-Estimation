/*
 * KpfaTransformerData.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_TRANSFORMER_DATA_H_
#define _KPFA_TRANSFORMER_DATA_H_

#include "KpfaRawData.h"

#define KPFA_MAX_NUM_WINDINGS	3

#define KPFA_WINDING_1_TO_2		0
#define KPFA_WINDING_2_TO_3		1
#define KPFA_WINDING_3_TO_1		2

#define KPFA_MAX_NUM_TRANSFORMER_DATA_ITEMS_1	20
#define KPFA_MIN_NUM_TRANSFORMER_DATA_ITEMS_1	14
#define KPFA_MAX_NUM_TRANSFORMER_DATA_ITEMS_2	11
#define KPFA_MIN_NUM_TRANSFORMER_DATA_ITEMS_2	3
#define KPFA_NUM_TRANSFORMER_DATA_ITEMS_3_1		16
#define KPFA_NUM_TRANSFORMER_DATA_ITEMS_3_2		6
#define KPFA_NUM_TRANSFORMER_DATA_ITEMS_4		2

class KpfaTransformerData: public KpfaRawData {

public:

	// Bus number
	uint32_t m_nI;

	// Bus number of Winding 2
	uint32_t m_nJ;

	// Bus number of Winding 3
	uint32_t m_nK;

	// Transformer Circuit ID
	uint32_t m_nCkt;

	// Winding data I/O code
	uint8_t m_nCw;

	// Impedance data I/O code
	uint8_t m_nCz;

	// Magnetizing admittance I/O code
	uint8_t m_nCm;

	// Transformer magnetizing admittance connected to ground at bus I
	double m_nMag1;
	double m_nMag2;

	// Nonmetered end code of either 1 (for the Winding 1 bus) or 2 (for the Winding 2 bus)
	uint8_t m_nNmetr;

	// Transformer ID
	string m_rName;

	// Transformer status
	// 0: out-of-service
	// 1: in-service and
	// 2: only Winding 2 out-of-service
	// 3: only Winding 3 out-of-service
	// 4: only Winding 1 out-of-service
	uint8_t m_nStat;

	// The number of windings
	uint32_t m_nWindingCount;

	// The number of owners
	uint32_t m_nOwnerCount;

	// Owner number (1 ~ 9999).
	uint16_t m_nO[KPFA_MAX_NUM_OWNERS];

	// Fraction of total ownership assigned to owner Qi
	double m_nF[KPFA_MAX_NUM_OWNERS];

	// Measured impedance of the transformer between the buses of the winding
	double m_nR[KPFA_MAX_NUM_WINDINGS];
	double m_nX[KPFA_MAX_NUM_WINDINGS];

	// System base for the winding
	double m_nSbase[KPFA_MAX_NUM_WINDINGS];

	// Voltage magnitude at the hidden star point bus
	double m_nVmstar;

	// Voltage phase angle at the hidden star point bus
	double m_nAnstar;

	// Winding 1 off-nominal turns ratio in pu of Winding 1 bus base voltage
	double m_nWindv[KPFA_MAX_NUM_WINDINGS];

	// Nominal (rated) Winding 1 voltage base in kV, or zero
	double m_nNomv[KPFA_MAX_NUM_WINDINGS];

	// Winding one phase shift angle in degrees
	double m_nAng[KPFA_MAX_NUM_WINDINGS];

	// Winding's three three-phase ratings
	double m_nRata[KPFA_MAX_NUM_WINDINGS];
	double m_nRatb[KPFA_MAX_NUM_WINDINGS];
	double m_nRatc[KPFA_MAX_NUM_WINDINGS];

	// Transformer control mode
	uint8_t m_nCod[KPFA_MAX_NUM_WINDINGS];

    // YOUNGSUN - CHKME
	// Ex bus number to be regulated (***)
	uint32_t m_nCont[KPFA_MAX_NUM_WINDINGS];

	// The upper and lower limits
	double m_nRma[KPFA_MAX_NUM_WINDINGS];
	double m_nRmi[KPFA_MAX_NUM_WINDINGS];
	double m_nVma[KPFA_MAX_NUM_WINDINGS];
	double m_nVmi[KPFA_MAX_NUM_WINDINGS];

	// The number of tap positions available
	uint32_t m_nNtp[KPFA_MAX_NUM_WINDINGS];

	// The number of a transformer impedance correction table
	uint32_t m_nTap[KPFA_MAX_NUM_WINDINGS];

	// The load drop compensation impedance
	double m_nCr[KPFA_MAX_NUM_WINDINGS];
	double m_nCx[KPFA_MAX_NUM_WINDINGS];

public:

	KpfaTransformerData();

	virtual ~KpfaTransformerData();

	KpfaError_t ParseInput(string &rInputString, uint32_t nId = 0);

	KpfaError_t TransformUnit(double nSbase);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaTransformerData *pRawData);

private:

	KpfaError_t ParseRawDataLineType1(string &rInputString);

	KpfaError_t ParseRawDataLineType2(string &rInputString);

	KpfaError_t ParseRawDataLineType3(string &rInputString, uint32_t nIdx = 0);

	KpfaError_t ParseRawDataLineType4(string &rInputString);
};

#endif /* _KPFA_TRANSFORMER_DATA_H_ */
