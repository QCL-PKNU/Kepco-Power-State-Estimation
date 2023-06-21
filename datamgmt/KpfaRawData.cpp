/*
 * KpfaRawData.cpp
 *
 *  Created on: 2014. 5. 21.
 *      Author: Youngsun Han
 */

#include "KpfaRawData.h"

/**
 * Raw data type strings
 */
string g_KpfaRawDataTypeString[KPFA_NUM_OF_RAW_DATA_TYPES] = {

	"CASE",
	"BUS",
	"LOAD",
	"FIXED SHUNT",
	"GENERATOR",
	"BRANCH",
	"TRANSFORMER",
	"AREA",
	"TWO-TERMINAL DC",
	"VSC DC LINE",
	"IMPEDANCE CORRECTION",
	"MUTLI-TERMINAL DC",
	"MULTI-SECTION LINE",
	"ZONE",
	"INTER-AREA TRANSFER",
	"OWNER",
	"FACTS DEVICE",
	"SWITCHED SHUNT",
	"GNE",
	"INDUCTION MACHINE",
	"Q RECORD"
};

KpfaRawData::KpfaRawData(KpfaRawDataType_t nDataType) {
	m_nDataType = nDataType;
}

KpfaRawData::~KpfaRawData() {
	// Do nothing
}

KpfaError_t
KpfaRawData::TransformUnit(double nSbase) {
	// Do nothing
	return KPFA_SUCCESS;
}

ostream &operator << (ostream &rOut, KpfaRawData *pRawData) {
	pRawData->Write(rOut);
	return rOut;
}
