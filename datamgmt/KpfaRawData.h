/*
 * KpfaRawData.h
 *
 * The file will contains the functions to read and parse the input raw file.
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_RAW_DATA_H_
#define _KPFA_RAW_DATA_H_

#include "KpfaDebug.h"
#include "KpfaConfig.h"
#include "KpfaUtility.h"

#define RAW_DATA_INITIALIZED

#define KPFA_MAX_NUM_OWNERS	4

#if 1
// C/C++
#define KPFA_ANGLE_TO_RADIAN (double)57.29577951
#else
// FORTRAN
#define KPFA_ANGLE_TO_RADIAN (double)57.29578018188476562500
#endif

/**
 * Raw data types
 */
typedef enum {

	KPFA_RAW_CASE_ID = 0,
	KPFA_RAW_BUS,
	KPFA_RAW_GEN,
	KPFA_RAW_LOAD,
	KPFA_RAW_FIXED_SHUNT,
	KPFA_RAW_BRANCH,
	KPFA_RAW_TRANSFORMER,
	KPFA_RAW_AREA_INTERCHANGE,
	KPFA_RAW_TWO_TERM,
	KPFA_RAW_VSC,
	KPFA_RAW_IMP_CORRECT,
	KPFA_RAW_MULTI_TERM,
	KPFA_RAW_MULTI_SECTION,
	KPFA_RAW_ZONE,
	KPFA_RAW_AREA_TRANSFER,
	KPFA_RAW_OWNER,
	KPFA_RAW_FACTS,
	KPFA_RAW_SWITCHED_SHUNT,
	KPFA_RAW_GNE,
	KPFA_RAW_INDUCT_MACHINE,
	KPFA_RAW_Q_RECORD,

	// The number of data types
	KPFA_NUM_OF_RAW_DATA_TYPES,

} KpfaRawDataType_t;

//  Bus type code (1: Load, 2: Gen, 3: Swing, 4: Isolated)
typedef enum {

    KPFA_PV_BUS = -2,
	KPFA_LOAD_BUS = 1,
	KPFA_GEN_BUS = 2,
	KPFA_SWING_BUS = 3,
	KPFA_ISOLATED_BUS = 4

} KpfaBusType_t;

/**
 * Strings for each of raw data types
 */
extern string g_KpfaRawDataTypeString[KPFA_NUM_OF_RAW_DATA_TYPES];

/**
 * String list type
 */
typedef std::vector<string> KpfaStringList_t;

/**
 * Superclass of all the raw data classes
 */
class KpfaRawData {

protected:

	// Raw Data Type
	KpfaRawDataType_t m_nDataType;

public:

	KpfaRawData(KpfaRawDataType_t nDataType);

	virtual ~KpfaRawData();

	/**
	 * This function will return the data type of the raw data.
	 *
	 * @return the raw data type
	 */
	inline KpfaRawDataType_t GetDataType() {
		return m_nDataType;
	}

	/**
	 * This function must be implemented to parse the input string into the internal information.
	 *
	 * @param rInputString a string with the internal information to be parsed
	 * @return error information
	 */
	virtual KpfaError_t ParseInput(string &rInputString, uint32_t nId) = 0;

	/**
	 * This function will be used to transform the units of the values of the raw data.
	 *
	 * @param nSbase System MVA base
	 * @return error information
	 */
	virtual KpfaError_t TransformUnit(double nSbase);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut) = 0;

	friend ostream &operator << (ostream &rOut, KpfaRawData *pRawData);
};

#endif /* _KPFA_RAW_DATA_H_ */
