/*
 * KpfaUtility.h
 *
 *  Created on: 2014. 5. 23.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_UTILITY_H_
#define _KPFA_UTILITY_H_

#include "KpfaConfig.h"

typedef std::vector<string> KpfaStringList_t;

/**
 * Macros for convert the given string into a number.
 */
#define KPFA_CONVERTi(TYPE, STRING)	(TYPE)atoi(STRING.c_str())
#define KPFA_CONVERTf(TYPE, STRING)	(TYPE)atof(STRING.c_str())

/**
 * Macros for dump matrix and vector
 */
#ifdef KPFA_ENABLE_DUMP_COMPLEX_MATRIX
#define KPFA_DUMP_COMPLEX_MATRIX(PATH, MAT) \
do {                                        \
    ofstream o;                             \
    o.open(PATH, ios::out);                 \
    KpfaPrintComplexMatrix(o, MAT);         \
    o.close();                              \
} while(0)
#else
#define KPFA_DUMP_COMPLEX_MATRIX(PATH, MAT) //
#endif

#ifdef KPFA_ENABLE_DUMP_COMPLEX_VECTOR
#define KPFA_DUMP_COMPLEX_VECTOR(PATH, VEC) \
do {                                        \
    ofstream o;                             \
    o.open(PATH, ios::out);                 \
    KpfaPrintComplexVector(o, VEC);          \
    o.close();                              \
} while(0)
#else
#define KPFA_DUMP_COMPLEX_VECTOR(PATH, VEC)	//
#endif

#ifdef KPFA_ENABLE_DUMP_DOUBLE_MATRIX
#define KPFA_DUMP_DOUBLE_MATRIX(PATH, MAT)  \
do {                                        \
    ofstream o;                             \
    o.open(PATH, ios::out);                 \
    KpfaPrintDoubleMatrix(o, MAT);          \
    o.close();                              \
} while(0)
#else
#define KPFA_DUMP_DOUBLE_MATRIX(PATH, MAT)	//
#endif

#ifdef KPFA_ENABLE_DUMP_DOUBLE_VECTOR
#define KPFA_DUMP_DOUBLE_VECTOR(PATH, VEC)  \
do {                                        \
    ofstream o;                             \
    o.open(PATH, ios::out);                 \
    KpfaPrintDoubleVector(o, VEC);          \
    o.close();                              \
} while(0)
#else
#define KPFA_DUMP_DOUBLE_VECTOR(PATH, VEC)	//
#endif

/**
 * Utility function for parsing the raw data
 */
extern uint32_t KpfaTokenize(const string &rString, KpfaStringList_t &rTokens, const string &rDelimiters);

/**
 * Utility function for removing the specified character from the given string
 */ 
extern string& KpfaStripQuotes(string &rString);

#endif
