/*
 * KpfaGvModule.h
 *
 *  Created on: Oct 13, 2015
 *      Author: youngsun
 */

#ifndef _KPFA_GV_MODULE_H_
#define _KPFA_GV_MODULE_H_

#include "KpfaDebug.h"
#include "KpfaConfig.h"
#include "KpfaRawDataMgmt.h"
#include "KpfaCtrlDataMgmt.h"

/**
 * GV result item
 */
typedef struct {
	// generator parameter
	double nGenParam;
	// voltage
	double nVoltage;
} KpfaGvData_t;

typedef std::vector<KpfaGvData_t *> KpfaGvDataList_t;

/**
 * The declaration of the class for GV algorithm module
 */
class KpfaGvModule {

private:

	double m_nMaxMargin;

	// system required Q
	double m_nSysReqQ;

	KpfaGvDataList_t m_rGvDataList;

	KpfaCtrlDataMgmt *m_pCtrlDataMgmt;

public:

	KpfaGvModule(KpfaCtrlDataMgmt *pDataMgmt = NULL) {

		m_rGvDataList.clear();

		if(pDataMgmt == NULL) {
			m_pCtrlDataMgmt = new KpfaCtrlDataMgmt();
		}
		else {
			m_pCtrlDataMgmt = pDataMgmt;
		}

		m_nMaxMargin = 1.0;

		m_nSysReqQ = 0.0;
	}

	virtual ~KpfaGvModule() {

		m_rGvDataList.clear();
	}

	/**
	 * This function will be used to update the max margin.
	 *
	 * @param nMaxMargin new max margin
	 */
	inline void SetMaxMargin(double nMaxMargin) {
		m_nMaxMargin = nMaxMargin;
	}

	/**
	 * This function will return the value of the max margin.
	 *
	 * @return the max margin
	 */
	inline double GetMaxMargin() {
		return m_nMaxMargin;
	}

	/**
	 * This function will return the value of the required Q.
	 */
	inline double GetSysReqQ() {
		return m_nSysReqQ;
	}

	KpfaError_t Execute(KpfaRawDataMgmt *pRawDataMgmt, KpfaCtgData *pCtgData);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaGvModule *pModule);
};

#endif /* _KPFA_GV_MODULE_H_ */
