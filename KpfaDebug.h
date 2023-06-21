/*
 * KpfaDebug.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_DEBUG_H_
#define _KPFA_DEBUG_H_

#include "KpfaConfig.h"

//////////////////////////////////////////////////
// PFA Error Types
//////////////////////////////////////////////////

typedef enum {

	KPFA_SUCCESS = 0,

	KPFA_ERROR_INVALID_ARGUMENT,
	KPFA_ERROR_MEMORY_ALLOC,
	KPFA_ERROR_FILE_OPEN,
	KPFA_ERROR_INVALID_RESULT,

	KPFA_ERROR_CASE_PARSE,
	KPFA_ERROR_BUS_PARSE,
	KPFA_ERROR_BUS_UNKNOWN_TYPE,
	KPFA_ERROR_LOAD_PARSE,
	KPFA_ERROR_BRANCH_PARSE,
	KPFA_ERROR_GENERATOR_PARSE,
	KPFA_ERROR_FIXED_SHUNT_PARSE,
	KPFA_ERROR_TRANSFORMER_PARSE,
	KPFA_ERROR_AREA_PARSE,
	KPFA_ERROR_TWO_TERM_PARSE,
	KPFA_ERROR_VSC_PARSE,
	KPFA_ERROR_SWITCHED_SHUNT_PARSE,
	KPFA_ERROR_FACTS_PARSE,
	KPFA_ERROR_OUTAGE_PARSE,
	KPFA_ERROR_OUTAGE_BUS_PARSE,
	KPFA_ERROR_OUTAGE_GEN_PARSE,
	KPFA_ERROR_OUTAGE_TRANS_PARSE,
	KPFA_ERROR_OUTAGE_BRANCH_PARSE,
	KPFA_ERROR_OUTAGE_WIND_PARSE,
	KPFA_ERROR_OUTAGE_HVDC_PARSE,

	KPFA_ERROR_CONTROL_PARAM_PARSE,
	KPFA_ERROR_CONTROL_UNKNOWN_PARAM,

	KPFA_ERROR_CASE_READ,
	KPFA_ERROR_BUS_READ,
	KPFA_ERROR_LOAD_READ,
	KPFA_ERROR_BRANCH_READ,
	KPFA_ERROR_GENERATOR_READ,
	KPFA_ERROR_FIXED_SHUNT_READ,
	KPFA_ERROR_TRANSFORMER_READ,
	KPFA_ERROR_AREA_READ,
	KPFA_ERROR_TWO_TERM_READ,
	KPFA_ERROR_VSC_READ,
	KPFA_ERROR_SWITCHED_SHUNT_READ,
	KPFA_ERROR_CTG_NAME_READ,
	KPFA_ERROR_CTG_STATUS_READ,
	KPFA_ERROR_CTG_OUTAGE_READ,
	KPFA_ERROR_SPLIT_SUB_SYS,

	KPFA_ERROR_INVALID_SBASE,
	KPFA_ERROR_YMATRIX_BUILD,
	KPFA_ERROR_JACOBI_CALCULATE,
	KPFA_ERROR_KRON_REDUCTION,
	KPFA_ERROR_LU_FACTORIZE,
	KPFA_ERROR_WRONG_MATSIZE,
	KPFA_ERROR_PCG_MAX_ITERATION,
	KPFA_ERROR_WRONG_NNZ,
	KPFA_ERROR_MA28AD_FAIL,
	KPFA_ERROR_MA28CD_FAIL,

    KPFA_ERROR_INVALID_BUS_ID,
	KPFA_ERROR_INVALID_BUS_TYPE,
	KPFA_ERROR_INVALID_BUS_INDEX,
	KPFA_ERROR_INVALID_GEN_DATA,
	KPFA_ERROR_INVALID_LOAD_DATA,
	KPFA_ERROR_INVALID_FACTS_DATA,
	KPFA_ERROR_INVALID_SSHUNT_DATA,
	KPFA_ERROR_INVALID_HVDC_ID,
	KPFA_ERROR_NOT_CONVERGED,

	KPFA_ERROR_FACTS_MODULE,
	KPFA_ERROR_EQR_MODULE,
	KPFA_ERROR_GV_MODULE,

	KPFA_ERROR_BMATRIX_BUILD,
	KPFA_ERROR_WFACTOR_CALCULATE,
	KPFA_ERROR_CQR_CALCULATE,
	KPFA_ERROR_EQR_CALCULATE,
	KPFA_ERROR_EQR_RECALCULATE,
	KPFA_ERROR_EQR_INVALID_MARGIN,
	KPFA_ERROR_GV_REDUCE_GSTEP,

	KPFA_ERROR_FACTS_LIMITED,
	KPFA_ERROR_INVALID_HVDC_P,

	KPFA_FAILURE,

} KpfaError_t;

/////////////////////////////////////////////////////////////////////////
// PFA Debug Function Declaration
/////////////////////////////////////////////////////////////////////////

void KpfaDebugPrintln(const char *pTag, const char *pFmt,...);

/**
 * Utility functions for printing out non-zero elements of a matrix
 */
extern void KpfaPrintDoubleMatrix(ostream &rOut, KpfaDoubleMatrix_t &rMat);

extern void KpfaPrintComplexMatrix(ostream &rOut, KpfaComplexMatrix_t &rMat);

/**
 * Utility functions for printing out non-zero elements of a vector
 */
extern void KpfaPrintDoubleVector(ostream &rOut, KpfaDoubleVector_t &rVec);

extern void KpfaPrintComplexVector(ostream &rOut, KpfaComplexVector_t &rVec);

/////////////////////////////////////////////////////////////////////////
// Debug Interface Macros for PFA Debugging Functions
/////////////////////////////////////////////////////////////////////////

#define KPFA_ENABLE_DEBUG
#define KPFA_ENABLE_ERROR
#define KPFA_ENABLE_ASSERT
#define KPFA_ENABLE_CHECK

#ifdef KPFA_ENABLE_DEBUG
#define KPFA_DEBUG(TAG, FMT, ...)	KpfaDebugPrintln(TAG, FMT, ##__VA_ARGS__)
#else
#define KPFA_DEBUG(TAG, FMT, ...)	//
#endif

#ifdef KPFA_ENABLE_DEBUG
#define KPFA_ERROR(FMT, ...)		KpfaDebugPrintln("ERROR", FMT, ##__VA_ARGS__)
#else
#define KPFA_ERROR(FMT, ...)		//
#endif

#ifdef KPFA_ENABLE_CHECK
#define KPFA_CHECK(EXPR, ERROR)		\
do{									\
	if(!(EXPR)) {					\
		return ERROR;				\
	}								\
} while(0)
#else
#define KPFA_CHECK(EXPR, ERROR)		//
#endif

#ifdef KPFA_ENABLE_ASSERT
#define KPFA_ASSERT(EXPR, MSG)		\
do{									\
	if(!(EXPR)) {					\
		KPFA_ERROR("%s", MSG);		\
		assert(EXPR);				\
	}								\
} while(0)
#else
#define KPFA_ASSERT(EXPR, MSG)		//
#endif

//////////////////////////////////////////////////
// Global variable declaration and macros for time estimation
//////////////////////////////////////////////////

extern clock_t g_KpfaGlobalClock;

#ifdef KPFA_ENABLE_TIMER
#define KPFA_START_TIMER()      g_KpfaGlobalClock = clock()
#else
#define KPFA_START_TIMER()      //
#endif

#ifdef KPFA_ENABLE_TIMER
#define KPFA_PRINT_TIMER(MSG)   printf("%s: %f\n", MSG, ((clock() - g_KpfaGlobalClock) / (double)CLOCKS_PER_SEC))
#else
#define KPFA_PRINT_TIMER(MSG)   //
#endif

#endif /* _KPFA_DEBUG_H_ */
