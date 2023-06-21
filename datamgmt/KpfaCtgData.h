/*
 * KpfaCtgData.h
 *
 *  Created on: 2015. 5. 24.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_CTG_DATA_H_
#define _KPFA_CTG_DATA_H_

#include "KpfaDebug.h"
#include "KpfaConfig.h"
#include "KpfaOutageData.h"

/**
 * Data type for a list of outage data
 */
typedef std::vector<KpfaOutageData *> KpfaOutageDataList_t;

/**
 * The declaration of the class for Contingency Data Management
 */
class KpfaCtgData {

private:

	// Contingency index
	uint32_t m_nIdx;

    // Contingency name
    string m_rName;

    // Contingency status
    bool_t m_bStatus;

    // if there is a generator outage
    bool_t m_bHasOutageGen;

    // if there is a HVDC outage
    bool_t m_bHasOutageHvdc;

    // HVDC bus ID if an outage occurs
    uint32_t m_nOutageHvdcId;

    // Outage data list
    KpfaOutageDataList_t m_rOutageDataList;

public:

	KpfaCtgData(uint32_t nIdx = 0);

	virtual ~KpfaCtgData();

	/**
	 * This function will return the status of this contingency.
	 *
	 * @return the status of the contingency
	 */
	inline bool_t GetStatus() {
		return m_bStatus;
	}

	/**
	 * This function will return the name of this contingency.
	 *
	 * @return the name of the contingency
	 */
	inline string GetName() {
		return m_rName;
	}

	/**
	 * This function will return the index of this contingency.
	 *
	 * @return the index of the contingency
	 */
	inline uint32_t GetIndex() {
		return m_nIdx;
	}

	/**
	 * This function will notify if there is a generator outage.
	 *
	 * @return generator outage existence
	 */
	inline bool_t &HasOutageGen() {
		return m_bHasOutageGen;
	}

	/**
	 * This function will notify if there is a HVDC outage.
	 *
	 * @return HVDC outage existence
	 */
	inline bool_t &HasOutageHvdc() {
		return m_bHasOutageHvdc;
	}

	/**
	 * This function return the HVDC bus Id.
	 *
	 * @return HVDC bus Id
	 */
	inline uint32_t GetOutageHvdcId() {
		return m_nOutageHvdcId;
	}


    /**
     * This function will return the list of outage data of the current contingency.
     *
     * @return the list of outage data
     */
    inline KpfaOutageDataList_t &GetOutageDataList() {
        return m_rOutageDataList;
    }

	KpfaError_t ReadCtgData(ifstream &rCtgFile, bool_t &bFinalCtg);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaCtgData *pDataMgmt);

private:

    KpfaError_t ReadCtgHeader(ifstream &rCtgFile);
};

#endif /* _KPFA_CTG_DATA_H_ */
