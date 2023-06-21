/*
 * KpfaDebug.cpp
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#include "KpfaDebug.h"

//////////////////////////////////////////////////
// Debugging Function Definition
//////////////////////////////////////////////////

/**
 * This function will be used to print out the debugging message.
 */

void
KpfaDebugPrintln(const char *pTag, const char *pFmt, ...) {

	char buf[MAX_BUF_LEN];

	va_list args;
	va_start(args, pFmt);
	vsprintf(buf, pFmt, args);
	va_end(args);

	printf("%s]: %s\n", pTag, buf);
}

//////////////////////////////////////////////////
// Global Clock Definition for estimating time consumption
//////////////////////////////////////////////////

clock_t g_KpfaGlobalClock;

//////////////////////////////////////////////////
// Matrix & Vector Debugging Functions
//////////////////////////////////////////////////

/**
 * This function will be used to print out the non-zero elements of the given matrix.
 *
 * @param rOut output stream
 * @param rMat a matrix to be printed
 */
void KpfaPrintDoubleMatrix(ostream &rOut, KpfaDoubleMatrix_t &rMat) {

	KpfaDoubleMatrix_t::iterator1 riter;
	KpfaDoubleMatrix_t::iterator2 citer;

	uint32_t nz = 0;

	for(riter = rMat.begin1(); riter != rMat.end1(); riter++) {
		for(citer = riter.begin(); citer != riter.end(); citer++) {

			rOut << std::fixed;
			rOut << (citer.index1() + 1) << ",";
			rOut << (citer.index2() + 1) << ",";
			rOut << std::scientific;
			rOut << *citer << endl;

			nz++;
		}
	}

	rOut << "# of non-zero elements: " << nz << endl;
}

/**
 * This function will be used to print out the non-zero elements of the given vector.
 *
 * @param rOut output stream
 * @param rVec a vector to be printed
 */
void KpfaPrintDoubleVector(ostream &rOut, KpfaDoubleVector_t &rVec) {

	KpfaDoubleVector_t::iterator iter;

	uint32_t nz = 0;

	for(iter = rVec.begin(); iter != rVec.end(); iter++) {

		rOut << std::fixed;
		rOut << (iter.index() + 1) << ",";
		rOut << std::scientific;
		rOut << *iter << endl;

		nz++;
	}

	rOut << "# of non-zero elements: " << nz << endl;
}


/**
 * This function will be used to print out the non-zero elements of the given matrix.
 *
 * @param rOut output stream
 * @param rMat a matrix to be printed
 */
void KpfaPrintComplexMatrix(ostream &rOut, KpfaComplexMatrix_t &rMat) {

	KpfaComplexMatrix_t::iterator1 riter;
	KpfaComplexMatrix_t::iterator2 citer;

	for(riter = rMat.begin1(); riter != rMat.end1(); riter++) {
		for(citer = riter.begin(); citer != riter.end(); citer++) {

			KpfaComplex_t tmp = *citer;

			rOut << std::fixed;
			rOut << (citer.index1() + 1) << ",";
			rOut << (citer.index2() + 1) << ",";
			rOut << std::scientific;
			rOut << tmp.real() << "," << tmp.imag() << endl;
		}
	}
}

/**
 * This function will be used to print out the non-zero elements of the given vector.
 *
 * @param rOut output stream
 * @param rVec a vector to be printed
 */
void KpfaPrintComplexVector(ostream &rOut, KpfaComplexVector_t &rVec) {

	KpfaComplexVector_t::iterator iter;

	uint32_t nz = 0;

	for(iter = rVec.begin(); iter != rVec.end(); iter++) {

		KpfaComplex_t tmp = *iter;

		rOut << std::fixed;
		rOut << (iter.index() + 1) << ",";
		rOut << std::scientific;
		rOut << tmp.real() << "," << tmp.imag() << endl;

		nz++;
	}

	rOut << "# of non-zero elements: " << nz << endl;
}

