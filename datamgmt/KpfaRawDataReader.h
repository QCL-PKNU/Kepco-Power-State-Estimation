/*
 * KpfaRawDataReader.h
 *
 *  Created on: 2015. 5. 15.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_RAW_DATA_READER_H_
#define _KPFA_RAW_DATA_READER_H_

#include "KpfaRawDataMgmt.h"

// Raw Data Mgmt List
typedef std::vector<KpfaRawDataMgmt*> KpfaRawDataMgmtList_t;

/**
 * The declaration of the class for Raw Data Management
 */
class KpfaRawDataReader {

private:

	// Flat
	bool m_bFlat;

	// System base
	double m_nSysBase;

    // Maximum bus ID
    uint32_t m_nMaxBusId;

	// Case ID
	KpfaCaseData m_rCaseData;

	// Bus Data
	KpfaRawDataList_t m_rBusDataList;
	KpfaRawDataTable_t m_rBusDataTable;

	// Generator Data
	KpfaRawDataList_t m_rGenDataList;
	KpfaRawDataTable_t m_rGenDataTable;

	// Load Data
	KpfaRawDataList_t m_rLoadDataList;
	KpfaRawDataTable_t m_rLoadDataTable;

#if KPFA_RAW_DATA_VERSION == 33
	// Fixed Shunt Data
	KpfaRawDataList_t m_rFixedShuntDataList;
#endif

	// Branch Data
	KpfaRawDataList_t m_rBranchDataList;

	// Transformer Data
	KpfaRawDataList_t m_rTransformerDataList;

	// Area Data
	KpfaRawDataList_t m_rAreaDataList;

	// Two Terminal DC Data
	KpfaRawDataList_t m_rTwoTermDataList;

	// VSC DC Data
	KpfaRawDataList_t m_rVscDataList;

	// Switched Shunt
	KpfaRawDataList_t m_rSwitchedShuntDataList;

	// FACTS
	KpfaRawDataList_t m_rFactsDataList;

    // Raw Data Mgmt List
    KpfaRawDataMgmtList_t m_rRawDataMgmtList;

public:

	KpfaRawDataReader();

	virtual ~KpfaRawDataReader();

	/**
	 * This function will be used to add a new bus data into both bus table and list
	 *
	 * @param pBusData a new bus data
	 */
	inline void InsertBusData(KpfaBusData *pBusData) {
	
		// <key, value> := <i, the bus data>
		m_rBusDataTable[pBusData->m_nI] = pBusData;
		
		// Insert the new bus data into the bus data list
		m_rBusDataList.push_back(pBusData);	
	}

	/**
	 * This function will be used to add a new generator data into both generator table and list
	 *
	 * @param pGenData a new generator data
	 */
	inline void InsertGenData(KpfaGenData *pGenData) {

		// <key, value> := <i, the generator data>
		m_rGenDataTable[pGenData->m_nI] = pGenData;
		
		// Insert the new generator data into the generator data list
		m_rGenDataList.push_back(pGenData);	
	}	
	
	/**
	 * This function will be used to add a new load data into both load table and list
	 *
	 * @param pLoadData a new load data
	 */
	inline void InsertLoadData(KpfaLoadData *pLoadData) {
	
		// <key, value> := <i, the load data>
		m_rLoadDataTable[pLoadData->m_nI] = pLoadData;
		
		// Insert the new load data into the load data list
		m_rLoadDataList.push_back(pLoadData);	
	}

	/**
	 * This function will return the raw bus data indicated by the given bus ID.
	 *
	 * @param nBusId Bus ID
	 * @return Raw bus data with the bus ID, 'nBusId'
	 */
	inline KpfaBusData *GetBusData(uint32_t nBusId) {
		if(m_rBusDataTable.count(nBusId) != 0) {
			return (KpfaBusData *)m_rBusDataTable[nBusId];
		}
		return NULL;
	}

	/**
	 * This function will return the raw generator data indicated by the given bus ID.
	 *
	 * @param nBusId Bus ID
	 * @return Raw generator data with the bus ID, 'nBusId'
	 */
	inline KpfaGenData *GetGenData(uint32_t nBusId) {
		if(m_rGenDataTable.count(nBusId) != 0) {
			return (KpfaGenData *)m_rGenDataTable[nBusId];
		}
		return NULL;
	}

	KpfaError_t ReadRawDataFile(const char *pFilePath);

	KpfaRawDataMgmtList_t &GetRawDataMgmts();

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaRawDataReader *pRawDataReader);

private:

	KpfaError_t ReadCaseData(ifstream &rRawFile);

	KpfaError_t ReadBusData(ifstream &rRawFile);

	KpfaError_t ReadGenData(ifstream &rRawFile);

	KpfaError_t ReadLoadData(ifstream &rRawFile);

	KpfaError_t ReadBranchData(ifstream &rRawFile);

	KpfaError_t ReadTransformerData(ifstream &rRawFile);

	KpfaError_t ReadAreaData(ifstream &rRawFile);

	KpfaError_t ReadTwoTermData(ifstream &rRawFile);

	KpfaError_t ReadVscData(ifstream &rRawFile);

	KpfaError_t	ReadSwitchedShuntData(ifstream &rRawFile);

	KpfaError_t ReadImpCorrectData(ifstream &rRawFile);

	KpfaError_t ReadMultiTermData(ifstream &rRawFile);

	KpfaError_t ReadMultiSectData(ifstream &rRawFile);

	KpfaError_t ReadZoneData(ifstream &rRawFile);

	KpfaError_t ReadInterAreaData(ifstream &rRawFile);

	KpfaError_t ReadOwnerData(ifstream &rRawFile);

	KpfaError_t ReadFactsData(ifstream &rRawFile);
};

#endif /* _KPFA_RAW_DATA_READER_H_ */
