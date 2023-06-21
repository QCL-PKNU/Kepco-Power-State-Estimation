/*
 * KpfaResult.h
 *
 *  Created on: 2015. 10. 19.
 *      Author: Youngsun Han
 */
#ifndef _KPFA_RESULT_H_
#define _KPFA_RESULT_H_

#include "KpfaDebug.h"
#include "KpfaConfig.h"
#include "KpfaPowerflow.h"
#include "KpfaInterface.h"
#include "KpfaRawDataMgmt.h"

////////////////////////////////////////////////////////
// Global variable declaration
////////////////////////////////////////////////////////

extern KpfaResultData_t *g_pResultData;

////////////////////////////////////////////////////////
// Function declaration
////////////////////////////////////////////////////////

KpfaError_t KpfaAllocResultData(KpfaRawDataMgmt *pRawDataMgmt,
								KpfaCtgDataMgmt *pCtgDataMgmt);

KpfaError_t KpfaGatherResultData(KpfaRawDataMgmt *pRawDataMgmt,	
								 KpfaPowerflow *pPowerflow,
								 int nCtgIndex, 
								 int nPfIndex);

KpfaError_t KpfaGatherFactsData(KpfaRawDataMgmt *pRawDataMgmt,
								int nCtgIndex);

void KpfaPrintResult(char *pFilePath);

#endif
