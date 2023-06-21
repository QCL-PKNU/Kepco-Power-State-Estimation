/*
 * KpfaInterface.h
 *
 *  Created on: Oct 28, 2015
 *      Author: youngsun
 */

#ifndef _KPFA_INTERFACE_H_
#define _KPFA_INTERFACE_H_

#define KPFA_MAX_STR_LEN	32

////////////////////////////////////////////////////////
// FACTS
////////////////////////////////////////////////////////

typedef struct {

	// Bus ID
	int nBusId;

	// Voltage
	float nVoltage;

	// QGen
	float nQGen;

} KpfaFacts_t;

typedef struct {

	// FACTS list
	KpfaFacts_t *pPrevList;
	KpfaFacts_t *pNextList;

	// # of FACTS
	int nSize;

	// Contingency name
	char rName[KPFA_MAX_STR_LEN];

	// Status
	int nStatus;

} KpfaFactsCtg_t;

typedef struct {

	// CTG list
	KpfaFactsCtg_t *pList;

	// Final result
	KpfaFacts_t hFinal;

	// # of CTG
	int nSize;

} KpfaFactsList_t;

////////////////////////////////////////////////////////
// G-V
////////////////////////////////////////////////////////

typedef struct {

	// Gen Parameter
	float nGenParam;

	// Voltage
	float nVoltage;

} KpfaGv_t;

typedef struct {

	// G-V list
	KpfaGv_t *pList;

	// # of G-V points
	int nSize;

	// Contingency name
	char rName[KPFA_MAX_STR_LEN];

	// Margine
	float nMargin;

} KpfaGvCtg_t;

typedef struct {

	// CTG list
	KpfaGvCtg_t *pList;

	// # of CTG
	int nSize;

} KpfaGvList_t;

////////////////////////////////////////////////////////
// EQR
////////////////////////////////////////////////////////

typedef struct {

	// Bus ID
	int nBusId;

	// EQR value
	float nEqrValueL;
	float nEqrValueC;

} KpfaEqr_t;

typedef struct {

	// EQR list
	KpfaEqr_t *pList;

	// # of EQR
	int nSize;

	// CQR value
	float nCqrValueL;
	float nCqrValueC;

	// Contingency name
	char rName[KPFA_MAX_STR_LEN];

	// Margin
	float nEqrMarginL;
	float nEqrMarginC;

} KpfaEqrCtg_t;

typedef struct {

	// CTG list
	KpfaEqrCtg_t *pPrevList;
	KpfaEqrCtg_t *pNextList;

	// # of CTG
	int nSize;

} KpfaEqrList_t;

////////////////////////////////////////////////////////
// OPF
////////////////////////////////////////////////////////

typedef struct {

	float nLossBefore;
	float nLossAfter;

} KpfaOpf_t;

////////////////////////////////////////////////////////
// Power Flow
////////////////////////////////////////////////////////

// Bus
typedef struct {

	// Bus ID
	int nBusId;

	// Voltage
	float nVoltage;

} KpfaBus_t;

typedef struct {

	// Bus
	KpfaBus_t *pList;

	// # of bus
	int nSize;

	// Contingency name
	char rName[KPFA_MAX_STR_LEN];

} KpfaBusCtg_t;

typedef struct {

	// Bus info for each of the CTGs
	KpfaBusCtg_t *pList;

	// # of bus
	int nSize;

} KpfaBusList_t;

// Branch
typedef struct {

	// Branch ID
	int nCkt;

	// Start Branch ID
	int nStartId;

	// End Branch ID
	int nEndId;

	// Branch P, Q1, Q2 Flow
	float nFlowValueP;
	float nFlowValueQ1;
	float nFlowValueQ2;

} KpfaBranch_t;

typedef struct {

	// Branch list
	KpfaBranch_t *pList;

	// # of branch
	int nSize;

	// Contingency name
	char rName[KPFA_MAX_STR_LEN];

} KpfaBranchCtg_t;

typedef struct {

	// Branch ctg list
	KpfaBranchCtg_t *pList;

	// # of branch
	int nSize;

} KpfaBranchList_t;

// Generator
typedef struct {

	// Bus ID
	int nBusId;

	// Q gen
	float nQGen;

} KpfaGenerator_t;

typedef struct {

	// Generator ctg list
	KpfaGenerator_t *pList;

	// # of gen
	int nSize;

	// Contingency name
	char rName[KPFA_MAX_STR_LEN];

} KpfaGeneratorCtg_t;

typedef struct {

	// Generator list
	KpfaGeneratorCtg_t *pList;

	// # of gen
	int nSize;

} KpfaGeneratorList_t;

// Powerflow (Bus + Branch + Generator)
typedef struct {

	KpfaBusList_t hBusList[2];

	KpfaBranchList_t hBranchList[2];

	KpfaGeneratorList_t hGeneratorList[2];

} KpfaPowerFlow_t;

////////////////////////////////////////////////////////
// Result
////////////////////////////////////////////////////////

#define KPFA_STATUS_0			0
#define KPFA_STATUS_1			1
#define KPFA_STATUS_2			2
#define KPFA_STATUS_3			3
#define KPFA_STATUS_4			4

typedef struct {

	// FACTS
	KpfaFactsList_t hFactsList;

	// Final Operating Point of FACTS
	KpfaFactsCtg_t *pFinalOpFacts;

	// GV
	KpfaGvList_t hGvList;

	// EQR
	KpfaEqrList_t hEqrList;

	// OPF
	KpfaOpf_t hOpf;

	// PF
	KpfaPowerFlow_t hPowerflow;

} KpfaResultData_t;

////////////////////////////////////////////////////////
// KPFA External Input Interface
////////////////////////////////////////////////////////

typedef struct {

	const char *pRawFilePath;
	const char *pCtgFilePath;
	const char *pCtrlFilePath;

} KpfaInputParam_t;

////////////////////////////////////////////////////////
// KPFA External Function Interface
////////////////////////////////////////////////////////

#ifdef KPFA_DLL_SUPPORT

extern "C" {

__declspec(dllimport) int KpfaDoAnalysis(KpfaInputParam_t *pParam, KpfaResultData_t *pResultData);

__declspec(dllimport) void KpfaPrintResult(char *pFilePath, KpfaResultData_t *pResultData);

__declspec(dllimport) void KpfaReleaseResult(KpfaResultData_t *pResultData);

};

#else

int KpfaDoAnalysis(KpfaInputParam_t *pParam, KpfaResultData_t *pResultData);

void KpfaPrintResult(char *pFilePath, KpfaResultData_t *pResultData);

void KpfaReleaseResult(KpfaResultData_t *pResultData);


#endif

#endif /* _KPFA_INTERFACE_H_ */
