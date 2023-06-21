/*
 * KpfaFactsModule.h
 *
 *  Created on: Oct 13, 2015
 *      Author: youngsun
 */

#ifndef _KPFA_FACTS_MODULE_H_
#define _KPFA_FACTS_MODULE_H_

#include "KpfaDebug.h"
#include "KpfaConfig.h"
#include "KpfaRawDataMgmt.h"
#include "KpfaCtrlDataMgmt.h"

#define KPFA_FACTS_VS_STEP		(double)0.005
#define KPFA_FACTS_VS_MARGIN	(double)0.02

/**
 * The declaration of the class for FACTS algorithm module
 */
class KpfaFactsModule {

private:

	// FACTS vs step
	double m_nVsStep;

	// FACTS operating direction
	bool_t m_bIncrement;

	// Control data management
	KpfaCtrlDataMgmt *m_pCtrlDataMgmt;

public:

	KpfaFactsModule(KpfaCtrlDataMgmt *pCtrlDataMgmt = NULL) {
		m_pCtrlDataMgmt = pCtrlDataMgmt;

		m_nVsStep = KPFA_FACTS_VS_STEP;
	}

	virtual ~KpfaFactsModule() {
		//do nothing
	}

	/**
	 * This function will be used to update the VS step.
	 *
	 * @param nVsStep new VS step value
	 */
	inline void SetVsStep(double nVsStep) {
		m_nVsStep = nVsStep;
	}

	/**
	 * This function will be used to update the FACT operating direction.
	 *
	 * @param bIncrement new operating direction
	 */
	inline void SetIncrement(bool_t bIncrement) {
		m_bIncrement = bIncrement;
	}

	KpfaError_t Execute(KpfaRawDataMgmt *pRawDataMgmt);

	bool_t Execute(KpfaRawDataMgmt *pRawDataMgmt, bool_t bIncrement);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaFactsModule *pModule);
};

#endif /* _KPFA_FACTS_MODULE_H_ */
