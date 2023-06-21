/*
 * KpfaRawDataMgmt.h
 *
 *  Created on: 2014. 5. 15.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_RAW_DATA_MGMT_H_
#define _KPFA_RAW_DATA_MGMT_H_

#include "KpfaAreaData.h"
#include "KpfaBranchData.h"
#include "KpfaBusData.h"
#include "KpfaCaseData.h"
#include "KpfaFixedShuntData.h"
#include "KpfaGenData.h"
#include "KpfaLoadData.h"
#include "KpfaSwitchedShuntData.h"
#include "KpfaTransformerData.h"
#include "KpfaTwoTermData.h"
#include "KpfaVscData.h"
#include "KpfaFactsData.h"
#include "KpfaRawData.h"
#include "KpfaCtgDataMgmt.h"

#define KPFA_MAX_HVDC_1_P	300
#define KPFA_MAX_HVDC_2_P	400

// HVDC data entry
typedef struct {
	double nPower;		// Valid Power
	double nRequired;	// Q
	double nSupply;		// Filter
	double nError;		// Excess or Deficiency
} KpfaHvdcEntry_t;

// Raw Data List
typedef std::vector<KpfaRawData *> KpfaRawDataList_t;

// Raw Data Table (Bus ID, Raw Data)
typedef std::map<uint32_t, KpfaRawData *> KpfaRawDataTable_t;

/**
 * The declaration of the class for Raw Data Management
 */
class KpfaRawDataMgmt {

public: 

	// System base
	double m_nSysBase;

private:

	// Case ID
	KpfaCaseData *m_pCaseData;

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

	// Facts
	KpfaRawDataList_t m_rFactsDataList;

	// Swing bus ID
	uint32_t m_nSwingBusId;

    // Applied contingency data
    KpfaCtgData *m_pCtgData;

public:

    KpfaRawDataMgmt(KpfaCaseData *pCaseData = NULL);

	virtual ~KpfaRawDataMgmt();

	/**
	 * This function will return the index of the currently applied contingency.
	 *
	 * @return contingency index
	 */
	inline int32_t GetCtgIndex() {
		return (m_pCtgData != NULL) ? m_pCtgData->GetIndex() : -1;
	}

	/**
	 * This function will return the matrix index with the bus ID.
	 *
	 * @param nBusId	bus ID
	 * @return matrix index 
	 */ 
	inline uint32_t GetBusIndex(uint32_t nBusId) {
		KpfaBusData *bus = GetBusData(nBusId);
		assert(bus != NULL);
		return bus->m_nIdx;
	}

	/**
	 * This function will return the number of buses.
	 *
	 * @return bus number
	 */
	inline uint32_t GetBusCount() {
		return (uint32_t)m_rBusDataList.size();
	}

	/**
	 * This function will return the swing bus ID.
	 *
	 * @return the swing bus ID
	 */
	inline uint32_t GetSwingBusId() {
		return m_nSwingBusId;
	}

	/**
	 * This function will return the case data.
	 *
	 * @return Case data
	 */
	inline KpfaCaseData *GetCaseData() {
		return m_pCaseData;
	}

	/**
	 * This function will set the case data with the given case data.
	 *
	 * @param rCaseData the given case data
	 */
	inline void SetCaseData(KpfaCaseData *pCaseData) {
	    m_pCaseData = pCaseData;
	}    

	/**
	 * This function will be used to add a new bus data into both bus table and list
	 *
	 * @param pBusData a new bus data
	 */
	inline void InsertBusData(KpfaBusData *pBusData) {

		pBusData->m_nIdx = GetBusCount();
	
		// <key, value> := <i, the bus data>
		m_rBusDataTable[pBusData->m_nI] = pBusData;
		
		// Insert the new bus data into the bus data list
		m_rBusDataList.push_back(pBusData);	

        // Set the swing bus ID if the given bus is a swing bus
        if(pBusData->m_nIde == KPFA_SWING_BUS) {
            m_nSwingBusId = pBusData->m_nI;
        }
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
	 * This function will be used to add a new branch data into branch list
	 *
	 * @param pBranchData a new branch data
	 */
	inline void InsertBranchData(KpfaBranchData *pBranchData) {
	
		// Insert the new branch data into the branch data list
		m_rBranchDataList.push_back(pBranchData);	
	}    

	/**
	 * This function will be used to add a new transformer data into transformer list
	 *
	 * @param pTransformerData a new transformer data
	 */
	inline void InsertTransformerData(KpfaTransformerData *pTransformerData) {
	
		// Insert the new transformer data into the transformer data list
		m_rTransformerDataList.push_back(pTransformerData);	
	}      

	/**
	 * This function will be used to add a new area data into area list
	 *
	 * @param pAreaData a new area data
	 */
	inline void InsertAreaData(KpfaAreaData *pAreaData) {
	
		// Insert the new area data into the area data list
		m_rAreaDataList.push_back(pAreaData);	
	}  

	/**
	 * This function will be used to add a new two terminal data into two terminal list
	 *
	 * @param pTwoTermData a new two terminal data
	 */
	inline void InsertTwoTermData(KpfaTwoTermData *pTwoTermData) {
	
		// Insert the new two terminal data into the two terminal data list
		m_rTwoTermDataList.push_back(pTwoTermData);	
	}      

    /**
     * This function will be used to add a new vsc data into vsc list
     *
     * @param pVscData a new vsc data
     */
    inline void InsertVscData(KpfaVscData *pVscData) {
    
        // Insert the new vsc data into the vsc data list
        m_rVscDataList.push_back(pVscData);   
    }  
    
    /**
     * This function will be used to add a new switched shunt data into switched shunt list
     *
     * @param pSwitchedShuntData a new switched shunt data
     */
    inline void InsertSwitchedShuntData(KpfaSwitchedShuntData *pSwitchedShuntData) {
    
        // Insert the new switched shunt data into the switched shunt data list
        m_rSwitchedShuntDataList.push_back(pSwitchedShuntData);   
    }  

    /**
     * This function will be used to add a new FACTS data into FACTS list
     *
     * @param pFactsData a new FACTS data
     */
    inline void InsertFactsData(KpfaFactsData *pFactsData) {

        // Insert the new switched shunt data into the switched shunt data list
        m_rFactsDataList.push_back(pFactsData);
    }

	/**
	 * This function will return the list of the bus data.
	 *
	 * @return the bus data list
	 */
	inline KpfaRawDataList_t &GetBusDataList() {
		return m_rBusDataList;
	}

	/**
	 * This function will return the raw bus data indexed by the given index from the bus list.
	 *
	 * @param nIndex the index of the bus list
	 * @return Raw bus data indexed by 'nIndex'
	 */
	inline KpfaBusData *GetBusDataAt(uint32_t nIndex) {
		return (KpfaBusData *)m_rBusDataList[nIndex];
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
	 * This function will return the list of the load data.
	 *
	 * @return the load data list
	 */
	inline KpfaRawDataList_t &GetLoadDataList() {
		return m_rLoadDataList;
	}

	/**
	 * This function will return the raw load data indicated by the given bus ID.
	 *
	 * @param nBusId Bus ID
	 * @return Raw load data with the bus ID, 'nBusId'
	 */
	inline KpfaLoadData *GetLoadData(uint32_t nBusId) {
		if(m_rLoadDataTable.count(nBusId) != 0) {
			return (KpfaLoadData *)m_rLoadDataTable[nBusId];
		}
		return NULL;
	}

	/**
	 * This function will return the list of the branch data.
	 *
	 * @return the branch data list
	 */
	inline KpfaRawDataList_t &GetBranchDataList() {
		return m_rBranchDataList;
	}

#if KPFA_RAW_DATA_VERSION == 33
	/**
	 * This function will return the list of the fixed shunt data.
	 *
	 * @return the fixed shunt data list
	 */
	inline KpfaRawDataList_t &GetFixedShuntDataList() {
		return m_rFixedShuntDataList;
	}
#endif

	/**
	 * This function will return the list of the generator data.
	 *
	 * @return the generator data list
	 */
	inline KpfaRawDataList_t &GetGenDataList() {
		return m_rGenDataList;
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

	/**
	 * This function will return the list of the transformer data.
	 *
	 * @return the transformer data list
	 */
	inline KpfaRawDataList_t &GetTransformerDataList() {
		return m_rTransformerDataList;
	}

	/**
	 * This function will return the list of the area data.
	 *
	 * @return the area data list
	 */
	inline KpfaRawDataList_t &GetAreaDataList() {
		return m_rAreaDataList;
	}

	/**
	 * This function will return the list of the two terminal DC data.
	 *
	 * @return the area data list
	 */
	inline KpfaRawDataList_t &GetTwoTermDataList() {
		return m_rTwoTermDataList;
	}

	/**
	 * This function will return the list of the vsc data.
	 *
	 * @return the area data list
	 */
	inline KpfaRawDataList_t &GetVscDataList() {
		return m_rVscDataList;
	}

	/**
	 * This function will return the list of the switched shunt data.
	 *
	 * @return the area data list
	 */
	inline KpfaRawDataList_t &GetSwitchedShuntDataList() {
		return m_rSwitchedShuntDataList;
	}

	/**
	 * This function will return the list of the facts data.
	 *
	 * @return the facts data list
	 */
	inline KpfaRawDataList_t &GetFactsDataList() {
		return m_rFactsDataList;
	}

	KpfaHvdcEntry_t *GetHvdcEntry(uint32_t nTableId, uint32_t nIncrement, double nPower);

	/**
	 * This function will return the error value of the indicated HVDC entry.
	 *
	 * @param nTableId the HVDC table ID
	 * @param nIncrement if the HVDC will be controlled to increment
	 * @param nPower the HVDC power
	 * @return HVDC error value
	 */
	inline double GetHvdcError(uint32_t nTableId, uint32_t nIncrement, double nPower) {
		KpfaHvdcEntry_t *entry = GetHvdcEntry(nTableId, nIncrement, nPower);

		if(entry == NULL) {
			return -1;
		}

		return entry->nError;
	}

	/**
	 * This function will return the support value of the indicated HVDC entry.
	 *
	 * @param nTableId the HVDC table ID
	 * @param nIncrement if the HVDC will be controlled to increment
	 * @param nPower the HVDC power
	 * @return HVDC supply value
	 */
	inline double GetHvdcFilter(uint32_t nTableId, uint32_t nIncrement, double nPower) {
		KpfaHvdcEntry_t *entry = GetHvdcEntry(nTableId, nIncrement, nPower);

		if(entry == NULL) {
			return -1;
		}

		return (entry->nSupply / m_nSysBase);
	}

	KpfaError_t ReadRawDataFile(const char *pFilePath);

    KpfaError_t ApplyContingencyData(KpfaCtgData *pCtgData);

    KpfaError_t RetrieveFromContingency();

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaRawDataMgmt *pDataMgmt);
};

#endif /* _KPFA_RAW_DATA_MGMT_H_ */
