/*
 * KpfaRawDataReader.cpp
 *
 *  Created on: 2015. 5. 19.
 *      Author: Youngsun Han
 */

#include "KpfaRawDataReader.h"
#include "KpfaAdjacencyGraph.h"

/**
 * This function will be used to initialize all the data structures for raw data.
 */
KpfaRawDataReader::KpfaRawDataReader() {

	m_bFlat = false;
	m_nSysBase = 0.0;
	m_nMaxBusId = 0;
}

/**
 * This function will be used to finalize all the data structures for raw data.
 */
KpfaRawDataReader::~KpfaRawDataReader() {

	m_rBusDataList.clear();
	m_rBusDataTable.clear();
	m_rGenDataList.clear();
	m_rLoadDataList.clear();
	m_rBranchDataList.clear();
#if KPFA_RAW_DATA_VERSION == 33
	m_rFixedShuntDataList.clear();
#endif
	m_rTransformerDataList.clear();
	m_rAreaDataList.clear();
	m_rTwoTermDataList.clear();
	m_rVscDataList.clear();
	m_rSwitchedShuntDataList.clear();
	m_rFactsDataList.clear();
}

/**
 * This function will read the case ID data from the raw data file.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadCaseData(ifstream &rRawFile) {

	string linebuf;

	if(getline(rRawFile, linebuf).fail()) {
		return KPFA_ERROR_CASE_READ;
	}

	KpfaError_t error = m_rCaseData.ParseInput(linebuf);

	if(error != KPFA_SUCCESS) {
		KPFA_ERROR("ReadCaseData - Error to parse the case ID data.");
		return error;
	}

	// Skip the following two lines of the case ID data.
	getline(rRawFile, linebuf);
	getline(rRawFile, linebuf);

	// System base
	m_nSysBase = m_rCaseData.m_nSbase;

	return error;
}

/**
 * This function will read the bus data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadBusData(ifstream &rRawFile) {

	string linebuf;
	
	m_rBusDataList.clear();
	m_rBusDataTable.clear();

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the bus data
#if 0
		if(linebuf.find("0 / END OF BUS DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}

		KpfaBusData *busData = new KpfaBusData();

		if(busData == NULL) {
			KPFA_ERROR("ReadBusData - Error to allocate memory.");
			return KPFA_ERROR_MEMORY_ALLOC;
		}

		KpfaError_t error = busData->ParseInput(linebuf);

		if(error != KPFA_SUCCESS) {
			delete busData;
			return error;
		}

		// Ignore isolated buses
		if(busData->m_nIde == KPFA_ISOLATED_BUS) {
			delete busData;
			continue;
		}
		else if(m_bFlat == true) {
			// reset vm, va if the bus is flat
			busData->m_nVm = 1.0;
			busData->m_nVa = 0;
		}

		// Transform the unit to p.u.
		error = busData->TransformUnit(m_nSysBase);
		KPFA_CHECK(error == KPFA_SUCCESS, KPFA_ERROR_INVALID_SBASE);

		// To keep the original type of the bus
		busData->m_nOrigIde = busData->m_nIde;
		busData->m_nOrigVm  = busData->m_nVm;

		// Insert the new bus data into the bus data table and list
		InsertBusData(busData);

        // Find the maximum bus ID
        if(m_nMaxBusId < busData->m_nI) {
            m_nMaxBusId = busData->m_nI;
        }
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the load data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadLoadData(ifstream &rRawFile) {

	string linebuf;

	KpfaLoadData *loadData = NULL;
	KpfaLoadData *prevLoadData = NULL;

	m_rLoadDataList.clear();

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the load data
#if 0
		if(linebuf.find("0 / END OF LOAD DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}

		loadData = new KpfaLoadData();

		if(loadData == NULL) {
			KPFA_ERROR("ReadLoadData - Error to allocate memory.");
			return KPFA_ERROR_MEMORY_ALLOC;
		}


		KpfaError_t error = loadData->ParseInput(linebuf);

		if(error != KPFA_SUCCESS) {
			delete loadData;
			return error;
		}

		// Check the load status
		if(loadData->m_bStatus == false) {
			delete loadData;
			continue;
		}

		// Check the load bus
		KpfaBusData *loadBus = GetBusData(loadData->m_nI);

		// YOUNGSUN - CHKME
#if 1
		if(loadBus == NULL) {
#else
		if(loadBus == NULL || loadBus->m_nIde != KPFA_LOAD_BUS) {
#endif
			delete loadData;
			continue;
		}

		if(loadData->m_nPl == 0 && loadData->m_nQl == 0) {
			delete loadData;
			continue;
		}

		// YOUNGSUN - CHKME
		// HVDC CHECK
#if 0
		switch(loadData->m_nI) {
			case KPFA_HVDC_1_BID:
				loadData->m_nHvdcI = 1;
				break;
			case KPFA_HVDC_2_BID:
				loadData->m_nHvdcI = 2;
				break;
			case KPFA_HVDC_1_BID_2nd:
			case KPFA_HVDC_2_BID_2nd:
				prevLoadData->m_nPl += loadData->m_nPl;
				prevLoadData->m_nQl += loadData->m_nQl;
				continue;
			default:
				loadData->m_nHvdcI = 0;
				break;
		}
#endif

		// Transform the unit to p.u.
		error = loadData->TransformUnit(m_nSysBase);
		KPFA_CHECK(error == KPFA_SUCCESS, KPFA_ERROR_INVALID_SBASE);

		// YOUNGSUN - CHKME
		loadBus->m_nPl = (loadData->m_nPl + loadData->m_nIp + loadData->m_nYp);
		loadBus->m_nQl = (loadData->m_nQl + loadData->m_nIq - loadData->m_nYq);

		// Insert the new load data into both the load data table and list
		InsertLoadData(loadData);

		// Keep the previous load data for controlling HVDC
		prevLoadData = loadData;
	}

	return KPFA_SUCCESS;
}

#if KPFA_RAW_DATA_VERSION == 33
/**
 * This function will read the fixed shunt data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadFixedShuntData(ifstream &rRawFile) {

	string linebuf;

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the fixed shunt data
#if 0
		if(linebuf.find("0 / END OF FIXED SHUNT DATA") != string::npos) {
#else
		if(linebuf.find("0") == 0) {
#endif
			return KPFA_SUCCESS;
		}

		KpfaFixedShuntData *shuntData = new KpfaFixedShuntData();

		if(shuntData == NULL) {
			KPFA_ERROR("ReadFixedShuntData - Error to allocate memory.");
			return KPFA_ERROR_MEMORY_ALLOC;
		}

		KpfaError_t error = shuntData->ParseInput(linebuf);

		if(error != KPFA_SUCCESS) {
			delete shuntData;
			return error;
		}

		// Transform the unit to p.u.
		error = shuntData->TransformUnit(m_nSysBase);
		KPFA_CHECK(error == KPFA_SUCCESS, KPFA_ERROR_INVALID_SBASE);

		// Insert the new generator data into the generator data list.
		m_rFixedShuntDataList.push_back(shuntData);
	}

	return KPFA_SUCCESS;
}
#endif

/**
 * This function will read the generator data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadGenData(ifstream &rRawFile) {

	string linebuf;
	m_rGenDataList.clear();

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the load data
#if 0
		if(linebuf.find("0 / END OF GENERATOR DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}

		KpfaGenData *genData = new KpfaGenData();

		if(genData == NULL) {
			KPFA_ERROR("ReadGenData - Error to allocate memory.");
			return KPFA_ERROR_MEMORY_ALLOC;
		}

		KpfaError_t error = genData->ParseInput(linebuf);

		if(error != KPFA_SUCCESS) {
			delete genData;
			return error;
		}

		// Exception Handling
		KpfaBusData *genBus = GetBusData(genData->m_nI);

		// YOUNGSUN - CHKME
#if 1
		if(genBus == NULL) {
#else
		if(genBus == NULL || genBus->m_nIde != KPFA_GEN_BUS) {
#endif
			delete genData;
			continue;
		}

		// Change the generator bus to a load bus if it it now working
		if(genData->m_bStat == FALSE) {
			genBus->m_nIde = KPFA_LOAD_BUS;
			delete genData;
			continue;
		}

		// Transform the unit to p.u.
		error = genData->TransformUnit(m_nSysBase);
		KPFA_CHECK(error == KPFA_SUCCESS, KPFA_ERROR_INVALID_SBASE);

		// YOUNGSUN - CHKME
#if 1
		// Merge the generators with the same bus ID
		KpfaGenData *prevGenData = GetGenData(genData->m_nI);

		if(prevGenData != NULL) {
			prevGenData->MergeWith(genData);
			delete genData;
			continue;
		}
#endif
		// YOUNGSUN - CHKME
		genBus->m_nPg = genData->m_nPg;
		genBus->m_nQg = genData->m_nQg;
		genBus->m_nQt = genData->m_nQt;
		genBus->m_nQb = genData->m_nQb;
		genBus->m_nVm = genData->m_nVs;

		if(m_bFlat == true) {
			genData->m_nQg = 0;
		}

		// Insert the new generator data into both the generator data table and list
		InsertGenData(genData);
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the branch data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadBranchData(ifstream &rRawFile) {

	string linebuf;

	m_rBranchDataList.clear();

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the load data
#if 0
		if(linebuf.find("0 / END OF BRANCH DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}

		KpfaBranchData *branchData = new KpfaBranchData();

		if(branchData == NULL) {
			KPFA_ERROR("ReadBranchData - Error to allocate memory.");
			return KPFA_ERROR_MEMORY_ALLOC;
		}

		KpfaError_t error = branchData->ParseInput(linebuf);

		if(error != KPFA_SUCCESS) {
			delete branchData;
			return error;
		}

		// Exception Handling
		KpfaBusData *branchBus1 = GetBusData(branchData->m_nI);
		KpfaBusData *branchBus2 = GetBusData(branchData->m_nJ);

		if(!branchData->m_bSt || branchBus1 == NULL || branchBus2 == NULL) {
			delete branchData;
			continue;
		}

		if(branchData->m_nR == 0 && branchData->m_nX == 0) {
			branchData->m_nX = 0.0001;
		}

		branchBus1->m_nBl += branchData->m_nBi;
		branchBus2->m_nBl += branchData->m_nBj;

		// YOUNGSUN - CHKME
		branchData->m_nTap = 1.0;

		// Insert the new branch data into the branch data list.
		m_rBranchDataList.push_back(branchData);
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the transformer data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadTransformerData(ifstream &rRawFile) {

	string linebuf;

	m_rTransformerDataList.clear();

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the transformer data
#if 0
		if(linebuf.find("0 / END OF TRANSFORMER DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}

		KpfaTransformerData *transData = new KpfaTransformerData();

		if(transData == NULL) {
			KPFA_ERROR("ReadTransformerData - Error to allocate memory.");
			return KPFA_ERROR_MEMORY_ALLOC;
		}

		// Line #1
		KpfaError_t error = transData->ParseInput(linebuf);

		if(error != KPFA_SUCCESS) {
			delete transData;
			return error;
		}

		// Line #2
		getline(rRawFile, linebuf);
		error = transData->ParseInput(linebuf, 1);

		if(error != KPFA_SUCCESS) {
			delete transData;
			return error;
		}

		uint32_t nWindingCount = transData->m_nWindingCount;

		for(uint32_t i = 0; i < nWindingCount; i++) {

			// Line #3 (#4, #5)
			getline(rRawFile, linebuf);
			error = transData->ParseInput(linebuf, 2 + i);

			if(error != KPFA_SUCCESS) {
				delete transData;
				return error;
			}
		}

		if(nWindingCount == 1) {

			// Line #4
			getline(rRawFile, linebuf);
			error = transData->ParseInput(linebuf, 3);

			if(error != KPFA_SUCCESS) {
				delete transData;
				return error;
			}
		}

		// Exception Handling
		KpfaBusData *transBus1 = GetBusData(transData->m_nI);
		KpfaBusData *transBus2 = GetBusData(transData->m_nJ);
		KpfaBusData *transBus3 = GetBusData(transData->m_nK);

		if(transData->m_nStat == 0) {
			delete transData;
			continue;
		}
		
		if(transBus1 == NULL || transBus2 == NULL ||
		  (transBus3 == NULL && transData->m_nK != 0)) {
			delete transData;
			continue;
		}

		m_rTransformerDataList.push_back(transData);

		// 3 winding transformers
		if(transBus3 != NULL) {

			KpfaBusData *newBus = new KpfaBusData();

            newBus->m_nI = ++m_nMaxBusId;
			newBus->m_rName = transData->m_rName;
			newBus->m_nIde = KPFA_LOAD_BUS;	// 1 (YOUNGSUN - CHKME)
			newBus->m_nVm = transData->m_nVmstar;
			newBus->m_nVa = transData->m_nAnstar;

			newBus->TransformUnit(m_nSysBase);
			
			InsertBusData(newBus);

			// three branches for 3 winding transformers
			KpfaBranchData *branch1 = new KpfaBranchData();
			KpfaBranchData *branch2 = new KpfaBranchData();
			KpfaBranchData *branch3 = new KpfaBranchData();

			branch1->m_nCkt = transData->m_nCkt;
			branch2->m_nCkt = transData->m_nCkt;
			branch3->m_nCkt = transData->m_nCkt;

			// Branch from
			branch1->m_nI = transData->m_nI;
			branch2->m_nI = transData->m_nJ;
			branch3->m_nI = transData->m_nK;

			// Branch to
			branch1->m_nJ = newBus->m_nI;
			branch2->m_nJ = newBus->m_nI;
			branch3->m_nJ = newBus->m_nI;

			// Resistance
			branch1->m_nR = (transData->m_nR[2] + transData->m_nR[0] - transData->m_nR[1]) / 2.0;
			branch2->m_nR = (transData->m_nR[0] + transData->m_nR[1] - transData->m_nR[2]) / 2.0;
			branch3->m_nR = (transData->m_nR[1] + transData->m_nR[2] - transData->m_nR[0]) / 2.0;

			// Reactance
			branch1->m_nX = (transData->m_nX[2] + transData->m_nX[0] - transData->m_nX[1]) / 2.0;
			branch2->m_nX = (transData->m_nX[0] + transData->m_nX[1] - transData->m_nX[2]) / 2.0;
			branch3->m_nX = (transData->m_nX[1] + transData->m_nX[2] - transData->m_nX[0]) / 2.0;

			if(branch1->m_nX == 0)	branch1->m_nX = 0.0001;
			if(branch2->m_nX == 0)	branch2->m_nX = 0.0001;
			if(branch3->m_nX == 0)	branch3->m_nX = 0.0001;

			branch1->m_nTap = transData->m_nWindv[0];
			branch2->m_nTap = transData->m_nWindv[1];
			branch3->m_nTap = transData->m_nWindv[2];

			m_rBranchDataList.push_back(branch1);
			m_rBranchDataList.push_back(branch2);
			m_rBranchDataList.push_back(branch3);
		}
		// 2 winding transformers
		else {

			KpfaBranchData *branch1 = new KpfaBranchData();

			branch1->m_nCkt = transData->m_nCkt;
			branch1->m_nI = transData->m_nI;
			branch1->m_nJ = transData->m_nJ;
			branch1->m_nR = transData->m_nR[0];
			branch1->m_nX = transData->m_nX[0];

			if(branch1->m_nX == 0) {
				branch1->m_nX = 0.0001;
			}

			branch1->m_nTap = transData->m_nWindv[0];
			double tmp = transData->m_nWindv[1];

			if(tmp != 1.0) {

				branch1->m_nTap /= tmp;

				//tmp = pow(tmp, 2);
				tmp *= tmp;

				branch1->m_nR *= tmp;
				branch1->m_nX *= tmp;
			}

			m_rBranchDataList.push_back(branch1);
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the area data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadAreaData(ifstream &rRawFile) {

	string linebuf;

	m_rAreaDataList.clear();

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the area data
#if 0
		if(linebuf.find("0 / END OF AREA DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}

		KpfaAreaData *areaData = new KpfaAreaData();

		if(areaData == NULL) {
			KPFA_ERROR("ReadAreaData - Error to allocate memory.");
			return KPFA_ERROR_MEMORY_ALLOC;
		}

		KpfaError_t error = areaData->ParseInput(linebuf);

		if(error != KPFA_SUCCESS) {
			delete areaData;
			return error;
		}

		m_rAreaDataList.push_back(areaData);
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the two terminal DC data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadTwoTermData(ifstream &rRawFile) {

	string linebuf;

	m_rTwoTermDataList.clear();

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the two term DC data
#if 0
		if(linebuf.find("0 / END OF TWO-TERMINAL DC LINE DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}

#if 0
		KpfaTwoTermData *twoTermData = new KpfaTwoTermData();

		if(twoTermData == NULL) {
			KPFA_ERROR("ReadTwoTermData - Error to allocate memory.");
			return KPFA_ERROR_MEMORY_ALLOC;
		}

		// Parse two terminal DC data
		KpfaError_t error = twoTermData->ParseInput(linebuf);

		if(error != KPFA_SUCCESS) {
			delete twoTermData;
			return error;
		}

		m_rTwoTermDataList.push_back(twoTermData);
#endif
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the VSC DC data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadVscData(ifstream &rRawFile) {

	string linebuf;

	m_rVscDataList.clear();

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the VSC DC data
#if 0
		if(linebuf.find("0 / END OF VSC DC LINE DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}

#if 0
		KpfaVscData *vscData = new KpfaVscData();

		if(vscData == NULL) {
			KPFA_ERROR("ReadVscData - Error to allocate memory.");
			return KPFA_ERROR_MEMORY_ALLOC;
		}

		// Parse VSC data
		KpfaError_t error = vscData->ParseInput(linebuf);

		if(error != KPFA_SUCCESS) {
			delete vscData;
			return error;
		}

		m_rVscDataList.push_back(vscData);
#endif
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the switched shunt data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadSwitchedShuntData(ifstream &rRawFile) {

	string linebuf;

	m_rSwitchedShuntDataList.clear();

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the load data
#if 0
		if(linebuf.find("0 / END OF SWITCHED SHUNT DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}

		KpfaSwitchedShuntData *shuntData = new KpfaSwitchedShuntData();

		if(shuntData == NULL) {
			KPFA_ERROR("ReadBranchData - Error to allocate memory.");
			return KPFA_ERROR_MEMORY_ALLOC;
		}

		KpfaError_t error = shuntData->ParseInput(linebuf);

		if(error != KPFA_SUCCESS) {
			delete shuntData;
			return error;
		}

		// Exception Handling
		KpfaBusData *shuntBus = GetBusData(shuntData->m_nI);

		if(shuntBus == NULL) {
			delete shuntBus;
			continue;
		}

		// Transform the unit to p.u.
		error = shuntData->TransformUnit(m_nSysBase);
		KPFA_CHECK(error == KPFA_SUCCESS, KPFA_ERROR_INVALID_SBASE);

		shuntBus->m_nBl -= shuntData->m_nBinit;

		// Insert the new switchedShunt data into the switched shunt data list.
		m_rSwitchedShuntDataList.push_back(shuntData);
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the impedance correction data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadImpCorrectData(ifstream &rRawFile) {

	string linebuf;

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the impedance correction data
#if 0
		if(linebuf.find("0 / END OF IMPEDANCE CORRECTION TABLE DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the multi-terminal data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadMultiTermData(ifstream &rRawFile) {

	string linebuf;

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the multi-terminal dc line data
#if 0
		if(linebuf.find("0 / END OF MULTI-TERMINAL DC LINE DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the multi-section data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadMultiSectData(ifstream &rRawFile) {

	string linebuf;

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the multi-section line data
#if 0
		if(linebuf.find("0 / END OF MULTI-SECTION LINE DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the zone data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadZoneData(ifstream &rRawFile) {

	string linebuf;

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the zone data
#if 0
		if(linebuf.find("0 / END OF ZONE DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the inter-area data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadInterAreaData(ifstream &rRawFile) {

	string linebuf;

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the inter-area transfer data
#if 0
		if(linebuf.find("0 / END OF INTER-AREA TRANSFER DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the owner data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadOwnerData(ifstream &rRawFile) {

	string linebuf;

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the owner data
#if 0
		if(linebuf.find("0 / END OF OWNER DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will read the FACTS data from the raw data file and parse them.
 *
 * @param rRawFile input stream for raw file
 * @return error information
 */
KpfaError_t
KpfaRawDataReader::ReadFactsData(ifstream &rRawFile) {

	string linebuf;

	while(!getline(rRawFile, linebuf).fail()) {

		// Check the end of the FACTS device data
#if 0
		if(linebuf.find("0 / END OF FACTS DEVICE DATA") != string::npos) {
#else
		if(linebuf.find("0 /") != string::npos) {
#endif
			return KPFA_SUCCESS;
		}

		KpfaFactsData *factsData = new KpfaFactsData();

		if(factsData == NULL) {
			KPFA_ERROR("ReadAreaData - Error to allocate memory.");
			return KPFA_ERROR_MEMORY_ALLOC;
		}

		KpfaError_t error = factsData->ParseInput(linebuf);

		if(error != KPFA_SUCCESS) {
			delete factsData;
			return error;
		}

		// Exception Handling
		KpfaGenData *genData = GetGenData(factsData->m_nI);

		if(genData == NULL) {
			delete factsData;
			continue;
		}

		genData->m_bFacts = true;

		m_rFactsDataList.push_back(factsData);
	}

	return KPFA_SUCCESS;
}

/**
* This function will be used to read a file including raw data on the given path.
*
* @param pFilePath file path
* @return error information
*/
KpfaError_t
KpfaRawDataReader::ReadRawDataFile(const char *pFilePath) {

	KpfaError_t error;
	ifstream rawfile(pFilePath);

	if(!rawfile.is_open()) {
		cerr << "File Not Open: " << pFilePath << endl;
		return KPFA_ERROR_FILE_OPEN;
	}

	// Case ID
	error = ReadCaseData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Bus
	m_nMaxBusId = 0;
	error = ReadBusData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Load
	error = ReadLoadData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

 #if KPFA_RAW_DATA_VERSION == 33
	// FixedShunt
	error = ReadFixedShuntData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);
#endif

	// Generator
	error = ReadGenData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Branch
	error = ReadBranchData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Transformer
	error = ReadTransformerData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Area
	error = ReadAreaData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Two Terminal DC
	error = ReadTwoTermData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// VSC DC
	error = ReadVscData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Switched Shunt
	error = ReadSwitchedShuntData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Impedance Correction Table
	error = ReadImpCorrectData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Multi-terminal DC
	error = ReadMultiTermData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Multi-section
	error = ReadMultiSectData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Zone
	error = ReadZoneData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Inter-area transfer
	error = ReadInterAreaData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// Owner
	error = ReadOwnerData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	// FACTS
	error = ReadFactsData(rawfile);
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	rawfile.close();

	return KPFA_SUCCESS;
}

/**
 * This function will split seperated buses into several groups.
 *
 * @return error information
 */

KpfaRawDataMgmtList_t &
KpfaRawDataReader::GetRawDataMgmts() {

    // split sub-systems with connected buses using adjacency graph
    KpfaAdjacencyGraph graph;

    // add the edges of branches
    KpfaRawDataList_t::iterator iter;
    KpfaRawDataList_t &branchList = m_rBranchDataList;
    for(iter = branchList.begin(); iter != branchList.end(); iter++) {
        KpfaBranchData *branchData = (KpfaBranchData *)*iter;
        graph.AddEdge(branchData->m_nI, branchData->m_nJ);
    }

    // add the edges of transformers
    KpfaRawDataList_t &transList = m_rTransformerDataList;
    for(iter = transList.begin(); iter != transList.end(); iter++) {
        KpfaTransformerData *transData = (KpfaTransformerData *)*iter;
        graph.AddEdge(transData->m_nI, transData->m_nJ);
        if(transData->m_nK != 0) {
            graph.AddEdge(transData->m_nK, transData->m_nI);
            graph.AddEdge(transData->m_nK, transData->m_nJ);
        }
    }

	m_rRawDataMgmtList.clear();

	// find connected components using adjacency graph
    uint32_t i, numSys = 0;
    KpfaVertexTable_t::iterator siter;
    KpfaVertexTable_t &sysTable = graph.SplitSubgraphs(numSys);

    if(numSys == 0) {
    	return m_rRawDataMgmtList;
    }

    // split raw data elements into different sub-systems
    for(i = 0; i < numSys; i++) {

		KpfaRawDataMgmt *rawDataMgmt = new KpfaRawDataMgmt(&m_rCaseData);

		rawDataMgmt->m_nSysBase = m_nSysBase;

        // 0. case
        m_rRawDataMgmtList.push_back(rawDataMgmt);
    }

    // 1. bus
    KpfaRawDataList_t &busList = m_rBusDataList;
    for(iter = busList.begin(); iter != busList.end(); iter++) {
        KpfaBusData *tmp = (KpfaBusData *)*iter;
        m_rRawDataMgmtList[sysTable[tmp->m_nI]]->InsertBusData(tmp);
    }   

    // 2. load
    KpfaRawDataList_t &loadList = m_rLoadDataList;
    for(iter = loadList.begin(); iter != loadList.end(); iter++) {
        KpfaLoadData *tmp = (KpfaLoadData *)*iter;
        m_rRawDataMgmtList[sysTable[tmp->m_nI]]->InsertLoadData(tmp);
    }   

    // 3. generator
    KpfaRawDataList_t &genList = m_rGenDataList;
    for(iter = genList.begin(); iter != genList.end(); iter++) {
        KpfaGenData *tmp = (KpfaGenData *)*iter;
        m_rRawDataMgmtList[sysTable[tmp->m_nI]]->InsertGenData(tmp);
    }  

    // 4. branch
    for(iter = branchList.begin(); iter != branchList.end(); iter++) {
        KpfaBranchData *tmp = (KpfaBranchData *)*iter;
        m_rRawDataMgmtList[sysTable[tmp->m_nI]]->InsertBranchData(tmp);
    }     

    // 5. transformer
    for(iter = transList.begin(); iter != transList.end(); iter++) {
        KpfaTransformerData *tmp = (KpfaTransformerData *)*iter;
        m_rRawDataMgmtList[sysTable[tmp->m_nI]]->InsertTransformerData(tmp);
    } 

    // 6. area
    KpfaRawDataList_t &areaList = m_rAreaDataList;
    for(iter = areaList.begin(); iter != areaList.end(); iter++) {
        KpfaAreaData *tmp = (KpfaAreaData *)*iter;
        m_rRawDataMgmtList[sysTable[tmp->m_nI]]->InsertAreaData(tmp);
    }     

    // YOUNGSUN - CHKME
    // The following raw data types are not implemented yet
#if 0
    // 7. two term
    KpfaRawDataList_t &twoTermList = m_rTwoTermDataList;
    for(iter = twoTermList.begin(); iter != twoTermList.end(); iter++) {
        KpfaTwoTermData *tmp = (KpfaTwoTermData *)*iter;
        m_rRawDataMgmtList[sysTable[tmp->m_nI]]->InsertTwoTermData(tmp);
    } 

    // 8. vsc
    KpfaRawDataList_t &vscList = m_rVscDataList;
    for(iter = vscList.begin(); iter != vscList.end(); iter++) {
        KpfaVscData *tmp = (KpfaVscData *)*iter;
        m_rRawDataMgmtList[sysTable[tmp->m_nI]]->InsertVscData(tmp);
    }      
#endif

    // 9. switched shunt
    KpfaRawDataList_t &switchedShuntList = m_rSwitchedShuntDataList;
    for(iter = switchedShuntList.begin(); iter != switchedShuntList.end(); iter++) {
        KpfaSwitchedShuntData *tmp = (KpfaSwitchedShuntData *)*iter;
        m_rRawDataMgmtList[sysTable[tmp->m_nI]]->InsertSwitchedShuntData(tmp);
    }  
    
    // 10. FACTS
    KpfaRawDataList_t &factsList = m_rFactsDataList;
    for(iter = factsList.begin(); iter != factsList.end(); iter++) {
        KpfaFactsData *tmp = (KpfaFactsData *)*iter;
        m_rRawDataMgmtList[sysTable[tmp->m_nI]]->InsertFactsData(tmp);
    }

	return m_rRawDataMgmtList;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaRawDataReader::Write(ostream &rOut) {

	uint32_t i;

	// Case ID
	rOut << &m_rCaseData << endl;

	// Bus
	for(i = 0; i < m_rBusDataList.size(); i++) {
		rOut << ">> #" << i << " - " << m_rBusDataList[i] << endl;
	}

	// Load
	for(i = 0; i < m_rLoadDataList.size(); i++) {
		rOut << ">> #" << i << " - " << m_rLoadDataList[i] << endl;
	}

#if KPFA_RAW_DATA_VERSION == 33
	// FixedShunt
	for(i = 0; i < m_rFixedShuntDataList.size(); i++) {
		rOut << ">> #" << i << " - " << m_rFixedShuntDataList[i] << endl;
	}
#endif

	// Generator
	for(i = 0; i < m_rGenDataList.size(); i++) {
		rOut << ">> #" << i << " - " << m_rGenDataList[i] << endl;
	}

	// Branch
	for(i = 0; i < m_rBranchDataList.size(); i++) {
		rOut << ">> #" << i << " - " << m_rBranchDataList[i] << endl;
	}

	// Transformer
	for(i = 0; i < m_rTransformerDataList.size(); i++) {
		rOut << ">> #" << i << " - " << m_rTransformerDataList[i] << endl;
	}

	// Area
	for(i = 0; i < m_rAreaDataList.size(); i++) {
		rOut << ">> #" << i << " - " << m_rAreaDataList[i] << endl;
	}

	// Two terminal DC
	for(i = 0; i < m_rTwoTermDataList.size(); i++) {
		rOut << ">> #" << i << " - " << m_rTwoTermDataList[i] << endl;
	}

	// VSC
	for(i = 0; i < m_rVscDataList.size(); i++) {
		rOut << ">> #" << i << " - " << m_rVscDataList[i] << endl;
	}

	// Switched Shunt
	for(i = 0; i < m_rSwitchedShuntDataList.size(); i++) {
		rOut << ">> #" << i << " - " << m_rSwitchedShuntDataList[i] << endl;
	}

	// FACTS
	for(i = 0; i < m_rFactsDataList.size(); i++) {
		rOut << ">> #" << i << " - " << m_rFactsDataList[i] << endl;
	}
}

ostream &operator << (ostream &rOut, KpfaRawDataReader *pRawDataReader) {
	pRawDataReader->Write(rOut);
	return rOut;
}
