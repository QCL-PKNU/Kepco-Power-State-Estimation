/*
 * KpfaOutageData.cpp
 *
 *  Created on: 2015. 5. 25.
 *      Author: Youngsun Han
 */

#include "KpfaOutageData.h"

#define KPFA_CTG_TAG_OUTAGE "Outage"

#define KPFA_NUM_OTG_BUS_DATA_ITEMS         1
#define KPFA_NUM_OTG_GEN_DATA_ITEMS         2
#define KPFA_NUM_OTG_BRANCH_DATA_ITEMS      3
#define KPFA_NUM_OTG_TRANS_DATA_ITEMS_1     4 
#define KPFA_NUM_OTG_TRANS_DATA_ITEMS_2     3 
#define KPFA_NUM_OTG_WIND_DATA_ITEMS        2
#define KPFA_NUM_OTG_HVDC_DATA_ITEMS        2

/**
 * Outage data type strings
 */
string g_KpfaOutageDataTypeString[KPFA_NUM_OF_OUTAGE_TYPES] = {

	"bus",
	"generator",
	"branch",
	"transformer",
	"wind",
	"hvdc"
};

KpfaOutageData::KpfaOutageData() {
	// Do nothing
}

KpfaOutageData::~KpfaOutageData() {
	// Do nothing
}

#define _FOUND(STR)     (rInputString.find(STR) != string::npos)

KpfaError_t 
KpfaOutageData::ParseOutageData(string &rInputString) {

    KpfaError_t error;
    KpfaStringList_t tokens;

    // get the outage substring after '=' character
    string otgstr = rInputString.substr(rInputString.find("=") + 1);

    // check the type of outage
    if(_FOUND(KPFA_OTG_TAG_BUS)) {
        error = ParseOutageBus(otgstr);
        KPFA_CHECK(error == KPFA_SUCCESS, error);
    }
    else if(_FOUND(KPFA_OTG_TAG_GEN)) {
        error = ParseOutageGenerator(otgstr);
        KPFA_CHECK(error == KPFA_SUCCESS, error);
    }
    else if(_FOUND(KPFA_OTG_TAG_BRANCH)) {
        error = ParseOutageBranch(otgstr);
        KPFA_CHECK(error == KPFA_SUCCESS, error);
    }
    else if(_FOUND(KPFA_OTG_TAG_TRANS)) {
        error = ParseOutageTransformer(otgstr);
        KPFA_CHECK(error == KPFA_SUCCESS, error);
    }
    else if(_FOUND(KPFA_OTG_TAG_WIND)) {
        error = ParseOutageWind(otgstr);
        KPFA_CHECK(error == KPFA_SUCCESS, error);
    }
    else if(_FOUND(KPFA_OTG_TAG_HVDC)) {
        error = ParseOutageHvdc(otgstr);
        KPFA_CHECK(error == KPFA_SUCCESS, error);
    }
    else {
        KPFA_ERROR("Invalid Outage Data: %s\n", rInputString.c_str());
        return KPFA_ERROR_OUTAGE_PARSE;
    }     

    return KPFA_SUCCESS;
}

#undef _FOUND

KpfaError_t 
KpfaOutageData::ParseOutageBus(string &rInputString) {

    KpfaStringList_t tokens;
    uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

    if(nTokens != KPFA_NUM_OTG_BUS_DATA_ITEMS) {
        return KPFA_ERROR_OUTAGE_BUS_PARSE;
    }

    m_nDataType = KPFA_OUTAGE_BUS;

    // Bus ID
    m_nI = (uint32_t)atoi(tokens[0].c_str());
        
    return KPFA_SUCCESS;
}

KpfaError_t 
KpfaOutageData::ParseOutageBranch(string &rInputString) {

    uint32_t i = 0;
    KpfaStringList_t tokens;
    uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

    if(nTokens != KPFA_NUM_OTG_BRANCH_DATA_ITEMS) {
        return KPFA_ERROR_OUTAGE_BRANCH_PARSE;
    }

    m_nDataType = KPFA_OUTAGE_BRANCH;

    // Branch from bus ID
    m_nI = (uint32_t)atoi(tokens[i++].c_str());

    // Branch to bus ID
    m_nJ = (uint32_t)atoi(tokens[i++].c_str());

    // Branch circuit ID
    m_nCkt = (uint32_t)atoi(KpfaStripQuotes(tokens[i]).c_str());
    
    return KPFA_SUCCESS;
}

KpfaError_t 
KpfaOutageData::ParseOutageGenerator(string &rInputString) {

    KpfaStringList_t tokens;
    uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

    if(nTokens != KPFA_NUM_OTG_GEN_DATA_ITEMS) {
        return KPFA_ERROR_OUTAGE_GEN_PARSE;
    }

    m_nDataType = KPFA_OUTAGE_GEN;

    // Generator bus ID
    m_nI = (uint32_t)atoi(tokens[0].c_str());

    return KPFA_SUCCESS;
}

KpfaError_t 
KpfaOutageData::ParseOutageTransformer(string &rInputString) {

    uint32_t i = 0;
    KpfaStringList_t tokens;
    uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

    if(nTokens != KPFA_NUM_OTG_TRANS_DATA_ITEMS_1 &&
       nTokens != KPFA_NUM_OTG_TRANS_DATA_ITEMS_2) {
        return KPFA_ERROR_OUTAGE_TRANS_PARSE;
    }

    m_nDataType = KPFA_OUTAGE_TRANSFORMER;

    // Transformer 1st bus ID
    m_nI = (uint32_t)atoi(tokens[i++].c_str());

    // Transformer 2nd bus ID
    m_nJ = (uint32_t)atoi(tokens[i++].c_str());

    if(nTokens == KPFA_NUM_OTG_TRANS_DATA_ITEMS_1) {

        // Transformer 3rd bus ID
        m_nK = (uint32_t)atoi(tokens[i++].c_str());
    }

    // Transformer Circuit ID
    m_nCkt = (uint32_t)atoi(KpfaStripQuotes(tokens[i]).c_str());
    
    return KPFA_SUCCESS;
}

KpfaError_t 
KpfaOutageData::ParseOutageWind(string &rInputString) {

    KpfaStringList_t tokens;
    uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

    if(nTokens != KPFA_NUM_OTG_WIND_DATA_ITEMS) {
        return KPFA_ERROR_OUTAGE_WIND_PARSE;
    }

    m_nDataType = KPFA_OUTAGE_WIND;

    // Wind bus ID
    m_nI = (uint32_t)atoi(tokens[0].c_str());

    // Wind rate
    m_nWindRate = atof(tokens[1].c_str());

    return KPFA_SUCCESS;
}

KpfaError_t 
KpfaOutageData::ParseOutageHvdc(string &rInputString) {

    uint32_t i = 0;
    KpfaStringList_t tokens;
    uint32_t nTokens = KpfaTokenize(rInputString, tokens, ",");

    if(nTokens != KPFA_NUM_OTG_HVDC_DATA_ITEMS) {
        return KPFA_ERROR_OUTAGE_HVDC_PARSE;
    }

    m_nDataType = KPFA_OUTAGE_HVDC;

    // HVDC 1st bus ID
    m_nI = (uint32_t)atoi(tokens[i++].c_str());

    // HVDC 2nd bus ID
    m_nJ = (uint32_t)atoi(tokens[i++].c_str());
    
    return KPFA_SUCCESS;
}

void
KpfaOutageData::Write(ostream &rOut) {

    if(m_nDataType < KPFA_OUTAGE_BUS ||  
       m_nDataType >= KPFA_NUM_OF_OUTAGE_TYPES) {
        rOut << "Unknown Outage Type: " << m_nDataType << endl;
        return;
    }

    rOut << "Outage " << g_KpfaOutageDataTypeString[m_nDataType] << endl;

    switch(m_nDataType) { 
        case KPFA_OUTAGE_BUS: 
            rOut << "m_nI: " << m_nI << endl;
            break;
        case KPFA_OUTAGE_GEN:
            rOut << "m_nI: " << m_nI << endl;
            break;
        case KPFA_OUTAGE_BRANCH:
            rOut << "m_nI: " << m_nI << endl;
            rOut << "m_nJ: " << m_nJ << endl;
            rOut << "m_nCkt: " << m_nCkt << endl;
            break;
        case KPFA_OUTAGE_TRANSFORMER:
            rOut << "m_nI: " << m_nI << endl;
            rOut << "m_nJ: " << m_nJ << endl;
            rOut << "m_nK: " << m_nK << endl;
            rOut << "m_nCkt: " << m_nCkt << endl;
            break;
        case KPFA_OUTAGE_WIND:
            rOut << "m_nI: " << m_nI << endl;
            rOut << "m_nWindRate: " << m_nWindRate << endl;
            break;
        case KPFA_OUTAGE_HVDC:
            rOut << "m_nI: " << m_nI << endl;
            rOut << "m_nJ: " << m_nJ << endl;
            break;
        default:
        	break;
    }
}

ostream &operator << (ostream &rOut, KpfaOutageData *pOutageData) {
	pOutageData->Write(rOut);
	return rOut;
}
