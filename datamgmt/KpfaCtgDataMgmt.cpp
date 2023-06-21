/*
 * KpfaCtgDataMgmt.cpp
 *
 *  Created on: 2015. 5. 24.
 *      Author: Youngsun Han
 */

#include "KpfaCtgDataMgmt.h"
#include "KpfaUtility.h"

/**
* This function will be used to read a file including contingency data on the given path.
*
* @param pFilePath file path
* @return error information
*/

KpfaError_t
KpfaCtgDataMgmt::ReadCtgDataFile(const char *pFilePath) {

    bool_t ctgend;
	string linebuf;
	KpfaError_t error;

    // open the ctg file with the given path
	ifstream ctgfile(pFilePath);

	if(!ctgfile.is_open()) {
		cerr << "File Not Open: " << pFilePath << endl;
		return KPFA_ERROR_FILE_OPEN;
	}

	for(uint32_t i = 0; ; i++) {
        // read contingency data from the ctg file stream
        KpfaCtgData *ctgData = new KpfaCtgData(i);
        error = ctgData->ReadCtgData(ctgfile, ctgend);
        KPFA_CHECK(error == KPFA_SUCCESS, error);

        if(ctgend == TRUE) {
            delete ctgData;
            break;
        }

        // insert the contingency data into the last of the ctg list
        m_rCtgDataList.push_back(ctgData);
    }

	ctgfile.close();
	return KPFA_SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaCtgDataMgmt::Write(ostream &rOut) {

    uint32_t i;
    KpfaCtgDataList_t::iterator iter;
    KpfaCtgDataList_t &dataList = m_rCtgDataList;

    for(iter = dataList.begin(), i = 0; iter != dataList.end(); iter++, i++) {
        rOut << i << ") " << *iter << endl;
    }
}

ostream &operator << (ostream &rOut, KpfaCtgDataMgmt *pDataMgmt) {
	pDataMgmt->Write(rOut);
	return rOut;
}
