/*
 * KpfaRawDataMgmt.cpp
 *
 *  Created on: 2014. 5. 19.
 *      Author: Youngsun Han
 */

#include "KpfaRawDataMgmt.h"

/**
 * HVDC table entries
 */

// HVDC Table #1
static KpfaHvdcEntry_t g_HvdcTable1[] = {
	{20, 12.0f, 0.0f, -12.0f}, {40, 24.0f, 0.0f, -24.0f}, {60, 36.0f, 27.5f, -8.6f},
	{80, 48.0f, 27.5f, -20.6f}, {100, 60.0f, 55.0f, -5.0f},	{120, 72.0f, 55.0f, -8.5f},
	{140, 84.0f, 82.5f, -1.6f}, {160, 96.0f, 82.5f, -13.6f}, {180, 108.0f, 82.5f, -25.6f},
	{200, 120.0f, 110.0f, -10.0f}, {220, 132.0f, 110.0f, -22.0f}, {240, 144.0f, 137.5f, -6.6f},
	{260, 156.0f, 137.5f, -18.6f}, {280, 168.0f, 165.0f, -3.0f}, {300, 180.0f, 165.0f, -15.0f},
	{-1, 0.0f, 0.0f, 0.0f},
};

// HVDC Table #2
static KpfaHvdcEntry_t g_HvdcTable2[] = {
	{0, 0.0f, 42.0f, 42.0f}, {40, 18.5f, 84.0f, 65.5f}, {60, 28.48f, 84.0f, 55.51f},
	{80, 38.92f, 84.0f, 45.08f}, {100, 42.39f, 84.0f, 34.21f}, {120, 61.06f, 84.0f, 64.94f},
	{140, 72.72f, 126.0f, 53.28f}, {150, 78.67f, 126.0f, 47.33f}, {160, 84.78f, 126.0f, 41.22f},
	{180, 97.16f, 126.0f, 28.83f}, {200, 109.94f, 126.0f, 58.05f}, {220, 123.02f, 168.0f, 44.98f},
	{240, 136.42f, 168.0f, 31.59f}, {250, 143.25f, 168.0f, 24.75f}, {260, 150.18f, 168.0f, 17.81f},
	{280, 164.22f, 168.0f, 45.78f}, {300, 178.64f, 210.0f, 31.36f}, {320, 193.28f, 210.0f, 16.71f},
	{340, 208.24f, 210.0f, 1.76f}, {360, 223.56f, 252.0f, 28.44f}, {380, 239.1f, 252.0f, 12.89f},
	{400, 255.02f, 252.0f, -3.02f}, {-1, 0.0f, 0.0f, 0.0f},
};

/**
 * This function will be used to initialize all the data structures for raw data.
 */
KpfaRawDataMgmt::KpfaRawDataMgmt(KpfaCaseData *pCaseData) {

    m_pCaseData = pCaseData;

	// Bus Data
	m_rBusDataList.clear();
	m_rBusDataTable.clear();

	// Generator Data
	m_rGenDataList.clear();
	m_rGenDataTable.clear();

	// Load Data
	m_rLoadDataList.clear();
	m_rLoadDataTable.clear();

#if KPFA_RAW_DATA_VERSION == 33
	// Fixed Shunt Data
	m_rFixedShuntDataList.clear();
#endif

	// Branch Data
	m_rBranchDataList.clear();

	// Transformer Data
	m_rTransformerDataList.clear();

	// Area Data
	m_rAreaDataList.clear();

	// Two Terminal DC Data
	m_rTwoTermDataList.clear();

	// VSC DC Data
	m_rVscDataList.clear();

	// Switched Shunt
	m_rSwitchedShuntDataList.clear();

	// FACTS
	m_rFactsDataList.clear();

	// Swing bus ID
	m_nSwingBusId = 0;

    // Clear the applied contingency data
    m_pCtgData = NULL;
}

/**
 * This function will be used to finalize all the data structures for raw data.
 */
KpfaRawDataMgmt::~KpfaRawDataMgmt() {

	// Bus Data
	m_rBusDataList.clear();
	m_rBusDataTable.clear();

	// Generator Data
	m_rGenDataList.clear();
	m_rGenDataTable.clear();

	// Load Data
	m_rLoadDataList.clear();
	m_rLoadDataTable.clear();

#if KPFA_RAW_DATA_VERSION == 33
	// Fixed Shunt Data
	m_rFixedShuntDataList.clear();
#endif

	// Branch Data
	m_rBranchDataList.clear();

	// Transformer Data
	m_rTransformerDataList.clear();

	// Area Data
	m_rAreaDataList.clear();

	// Two Terminal DC Data
	m_rTwoTermDataList.clear();

	// VSC DC Data
	m_rVscDataList.clear();

	// Switched Shunt
	m_rSwitchedShuntDataList.clear();

	// FACTS
	m_rFactsDataList.clear();
}

/**
 * This function will return the HVDC entry indicated by the given parameters.
 *
 * @param nTableId the HVDC table ID
 * @param nIncrement if the HVDC will be controlled to increment
 * @param nPower the HVDC power
 * @return the selected HVDC entry
 */
KpfaHvdcEntry_t *
KpfaRawDataMgmt::GetHvdcEntry(uint32_t nTableId, uint32_t nIncrement, double nPower) {

	KpfaHvdcEntry_t *hvdcTable = NULL;

	switch(nTableId) {
		case 0: hvdcTable = (KpfaHvdcEntry_t *)g_HvdcTable1; break;
		case 1: hvdcTable = (KpfaHvdcEntry_t *)g_HvdcTable2; break;
		default: return NULL;
	}

	int i;

	nPower = fabs(nPower);

	// exception handling
	if(nTableId == 0 && nPower > KPFA_MAX_HVDC_1_P) {
		return NULL;
	}
	else if(nTableId == 1 && nPower > KPFA_MAX_HVDC_2_P) {
		return NULL;
	}

	for(i = 0; ; i++) {

		if(hvdcTable[i].nPower == -1) {
			return NULL;
		}

		if(nPower <= hvdcTable[i].nPower) {
			break;
		}
	}

	// Increment
	if(nIncrement && nPower != hvdcTable[i].nPower) {

		if(i > 0) {
			return &hvdcTable[i-1];
		}
		else {
			return NULL;
		}
	}

	// Decrement
	return &hvdcTable[i];
}

/**
 * This function will be used to apply the given contingency data to 
 * this raw data management object. 
 *
 * @param pCtgData contingency data
 * @return error information
 */
KpfaError_t 
KpfaRawDataMgmt::ApplyContingencyData(KpfaCtgData *pCtgData) {

    if(m_pCtgData == NULL)				 return KPFA_SUCCESS;
	if(m_pCtgData->GetStatus() == FALSE) return KPFA_SUCCESS;

    // Keep the applied contingency data for restoring
    m_pCtgData = pCtgData;

    // Get the iterator of outage list
    KpfaOutageDataList_t::iterator oiter;
    KpfaOutageDataList_t &otgDataList = pCtgData->GetOutageDataList();

    // Apply all the outage of the given contingency
    KpfaRawDataList_t::iterator riter;

    for(oiter = otgDataList.begin(); oiter != otgDataList.end(); oiter++) {

        KpfaOutageData *otg = *oiter;

        switch(otg->GetDataType()) {
            
            // Apply bus outage
            case KPFA_OUTAGE_BUS: {
                KpfaBusData *bus = GetBusData(otg->m_nI);
                KPFA_CHECK(bus != NULL, KPFA_ERROR_INVALID_BUS_ID);
                otg->m_nOrigState = (uint32_t)bus->m_nIde;
                bus->m_nIde = KPFA_ISOLATED_BUS;
                break;
            }

            // Apply branch outage
            case KPFA_OUTAGE_BRANCH: {
                KpfaRawDataList_t &branchDataList = m_rBranchDataList;
                for(riter = branchDataList.begin(); riter != branchDataList.end(); riter++) {
                    KpfaBranchData *branch = (KpfaBranchData *)*riter;
                    if(branch->m_nI == otg->m_nI && branch->m_nJ == otg->m_nJ && branch->m_nCkt == otg->m_nCkt) {
                        otg->m_nOrigState = (uint32_t)branch->m_bSt;
                        branch->m_bSt = FALSE;
                        break;
                    }
                }
                break;
            }

            // Apply transformer outage
            case KPFA_OUTAGE_TRANSFORMER: {
                KpfaRawDataList_t &transDataList = m_rTransformerDataList;
                for(riter = transDataList.begin(); riter != transDataList.end(); riter++) {
                    KpfaTransformerData *trans = (KpfaTransformerData *)*riter;
                    if(trans->m_nI == otg->m_nI && trans->m_nJ == otg->m_nJ && 
                       trans->m_nK == otg->m_nK && trans->m_nCkt == otg->m_nCkt) {
                       otg->m_nOrigState = (uint32_t)trans->m_nStat;
                       trans->m_nStat = 0;
                       break;
                    }
                }
                break;
            }

            // Apply generator outage
            case KPFA_OUTAGE_GEN: {
                KpfaGenData *gen = GetGenData(otg->m_nI);
                KPFA_CHECK(gen != NULL, KPFA_ERROR_INVALID_BUS_ID);
                otg->m_nOrigState = (uint32_t)gen->m_bStat;
                gen->m_bStat = FALSE;

				KpfaBusData *bus = GetBusData(gen->m_nI);
                KPFA_CHECK(bus != NULL, KPFA_ERROR_INVALID_BUS_ID);

				otg->m_nOrigPg = bus->m_nPg;
				otg->m_nOrigQg = bus->m_nQg;

				bus->m_nPg = 0;
				bus->m_nQg = 0;

				// YOUNGSUN - CHKME
				bus->m_nIde = KPFA_LOAD_BUS;
                break;
            }

            // YOUNGSUN - CHKME (HVDC, WIND outage)
          
            // Apply HVDC outage 
            case KPFA_OUTAGE_HVDC: {
#if 0
				KpfaLoadData *load = GetLoadData(otg->m_nI);
                KPFA_CHECK(load != NULL, KPFA_ERROR_INVALID_BUS_ID);

				otg->m_nOrigState = load->m_bStatus;
				otg->m_nOrigPl = load->m_nPl;
				otg->m_nOrigQl = load->m_nQl;

				load->m_nPl = 0;
				load->m_nQl = 0;
#else
				KpfaBusData *bus = GetBusData(otg->m_nI);
                KPFA_CHECK(bus != NULL, KPFA_ERROR_INVALID_BUS_ID);

				otg->m_nOrigPl = bus->m_nPl;
				otg->m_nOrigQl = bus->m_nQl;

				bus->m_nPl = 0;
				bus->m_nQl = 0;
#endif
                break;
            }

            // Apply wind outage 
            case KPFA_OUTAGE_WIND: {
                break;
            }

            default: 
                KPFA_ERROR("Unknown Outage Data Type: %d\n", otg->GetDataType());
                break;
        }
    }
    
    return KPFA_SUCCESS;
}

/**
 * This function will retrieve the original raw data from the modified one by contingency data.  
 * 
 * @return error information
 */
KpfaError_t
KpfaRawDataMgmt::RetrieveFromContingency() {

    // If there is no registered contingency data, just return.
    // Otherwise, retrieve original raw data and clear the applied contigency data information.
    if(m_pCtgData == NULL)				 return KPFA_SUCCESS;
	if(m_pCtgData->GetStatus() == FALSE) return KPFA_SUCCESS;

    // Get the iterator of outage list
    KpfaOutageDataList_t::iterator oiter;
    KpfaOutageDataList_t &otgDataList = m_pCtgData->GetOutageDataList();

    // Retrieve from all the outages
    KpfaRawDataList_t::iterator riter;
	KpfaRawDataList_t &busDataList = GetBusDataList(); 

	for(riter = busDataList.begin(); riter != busDataList.end(); riter++) {

		KpfaBusData *bus = (KpfaBusData *)*riter;
		bus->m_nVm = bus->m_nOrigVm;
	}
        
    for(oiter = otgDataList.begin(); oiter != otgDataList.end(); oiter++) {

        KpfaOutageData *otg = *oiter;

        switch(otg->GetDataType()) {
            
            case KPFA_OUTAGE_BUS: {
                KpfaBusData *bus = GetBusData(otg->m_nI);
                KPFA_CHECK(bus != NULL, KPFA_ERROR_INVALID_BUS_ID);
                bus->m_nIde = (KpfaBusType_t)otg->m_nOrigState;
                break;
            }

            case KPFA_OUTAGE_BRANCH: {
                KpfaRawDataList_t &branchDataList = m_rBranchDataList;
                for(riter = branchDataList.begin(); riter != branchDataList.end(); riter++) {
                    KpfaBranchData *branch = (KpfaBranchData *)*riter;
                    if(branch->m_nI == otg->m_nI && branch->m_nJ == otg->m_nJ && 
                       branch->m_nCkt == otg->m_nCkt) {
                        branch->m_bSt = (bool_t)otg->m_nOrigState;
                        break;
                    }
                }
                break;
            }

            case KPFA_OUTAGE_TRANSFORMER: {
                KpfaRawDataList_t &transDataList = m_rTransformerDataList;
                for(riter = transDataList.begin(); riter != transDataList.end(); riter++) {
                    KpfaTransformerData *trans = (KpfaTransformerData *)*riter;
                    if(trans->m_nI == otg->m_nI && trans->m_nJ == otg->m_nJ && 
                       trans->m_nK == otg->m_nK && trans->m_nCkt == otg->m_nCkt) {
                       trans->m_nStat = (uint8_t)otg->m_nOrigState;
                       break;
                    }
                }
                break;
            }

            case KPFA_OUTAGE_GEN: {
                KpfaGenData *gen = GetGenData(otg->m_nI);
                KPFA_CHECK(gen != NULL, KPFA_ERROR_INVALID_BUS_ID);
                gen->m_bStat = (bool_t)otg->m_nOrigState;

				KpfaBusData *bus = GetBusData(gen->m_nI);
                KPFA_CHECK(bus != NULL, KPFA_ERROR_INVALID_BUS_ID);

				bus->m_nIde = bus->m_nOrigIde;
				bus->m_nPg = otg->m_nOrigPg;
				bus->m_nQg = otg->m_nOrigQg;
                break;
            }

            case KPFA_OUTAGE_HVDC: {
#if 0
				KpfaLoadData *load = GetLoadData(otg->m_nI);
                KPFA_CHECK(load != NULL, KPFA_ERROR_INVALID_BUS_ID);

				load->m_bStatus = otg->m_nOrigState;

				load->m_nPl = otg->m_nOrigPl;
				load->m_nQl = otg->m_nOrigQl;
#else
				KpfaBusData *bus = GetBusData(otg->m_nI);
                KPFA_CHECK(bus != NULL, KPFA_ERROR_INVALID_BUS_ID);

				bus->m_nPl = otg->m_nOrigPl;
				bus->m_nQl = otg->m_nOrigQl;
#endif
                break;
            }

            case KPFA_OUTAGE_WIND: {
                break;
            }

            default: 
                KPFA_ERROR("Unknown Outage Data Type: %d\n", otg->GetDataType());
                break;
        }
    }    

    m_pCtgData = NULL;
    
    return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaRawDataMgmt::Write(ostream &rOut) {

	uint32_t i;

	// Case ID
	rOut << m_pCaseData << endl;

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

ostream &operator << (ostream &rOut, KpfaRawDataMgmt *pDataMgmt) {
	pDataMgmt->Write(rOut);
	return rOut;
}
