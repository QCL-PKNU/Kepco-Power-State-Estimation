/*
 * KpfaMain.cpp
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#include <stdio.h>
#include <memory.h>
#include "KpfaInterface.h"

/**
 * Main function
 */
int main(int argc, char **argv) {

	// Input parameters configuration
	KpfaInputParam_t param;
	param.pCtgFilePath  = "../../../Test/2027_input.ctg";
	//param.pRawFilePath  = "./Test/input.raw";//Jeju_mod_by_Kang_ver30.raw";//2027_map_ver30.raw";
	param.pRawFilePath  = "../../../Test/2027_map_ver30.raw";
	param.pCtrlFilePath = "../../../Test/input2.conx";

	// Result data initialization
	KpfaResultData_t result;
	memset(&result, 0, sizeof(KpfaResultData_t));

	// Performs powerflow analysis
	int error = KpfaDoAnalysis(&param, &result);

	if(error != 0) {
		printf("Stability Analysis Error: %d\n", error);
		return -1;
    }

	KpfaPrintResult("test.out", &result);

	// Release the result data
	KpfaReleaseResult(&result);

	return 0;
} 
