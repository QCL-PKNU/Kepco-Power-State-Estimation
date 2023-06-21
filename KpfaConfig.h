/*
 * KpfaConfig.h
 *
 *  Created on: 2014. 5. 14.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_CONFIG_H_
#define _KPFA_CONFIG_H_

// C++ STL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <time.h>

// C++ STL library
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <complex>
#include <string>
#include <vector>
#include <map>
#include <set>

// UBLAS library
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/operation_blocked.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>

using namespace boost::numeric::ublas;
using namespace std;

// Return result data
#define KPFA_RESULT_SUPPORT

// Dump matrix and vector
//#define KPFA_ENABLE_DUMP_COMPLEX_MATRIX
//#define KPFA_ENABLE_DUMP_COMPLEX_VECTOR
//#define KPFA_ENABLE_DUMP_DOUBLE_MATRIX
//#define KPFA_ENABLE_DUMP_DOUBLE_VECTOR

// Print-out time consumption information
#define KPFA_ENABLE_TIMER

// Print-out all matrices for debugging
//#define KPFA_DUMP_ALL_MATRIX

// Version of the raw data
#define KPFA_RAW_DATA_VERSION 	30

// Flag for applying heuristic method to improve the convergence rate.
#define KPFA_APPLY_CONVERGE_HEURISTIC

// Maximum buffer length
#define MAX_BUF_LEN				256

// Zero complex
#define ZERO_COMPLEX 			KpfaComplex_t(0, 0)

// User-defined data types
#ifndef _bool_t_defined_
#define _bool_t_defined_
typedef unsigned char bool_t;
#endif

#ifndef _uint8_t_defined_
#define _uint8_t_defined_
typedef unsigned char uint8_t;
#endif

#ifndef _int8_t_defined_
#define _int8_t_defined_
typedef signed char int8_t;
#endif

#ifndef _uint16_t_defined_
#define _uint16_t_defined_
typedef unsigned short uint16_t;
#endif

#ifndef _int16_t_defined_
#define _int16_t_defined_
typedef signed short int16_t;
#endif

#ifndef _uint32_t_defined_
#define _uint32_t_defined_
typedef unsigned int uint32_t;
#endif

#ifndef _int32_t_defined_
#define _int32_t_defined_
typedef signed int int32_t;
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef _kpfa_handle_t_defined_
#define _kpfa_handle_t_defined_
typedef void * KpfaHandle_t;
#endif

#ifndef _kpfa_complex_t_defined_
#define _kpfa_complex_t_defined_
typedef complex<double> KpfaComplex_t;
#endif

/**
 * Additional Data Type Declaration based on Boost Library
 */

typedef compressed_matrix<double> KpfaDoubleMatrix_t;

typedef compressed_matrix<KpfaComplex_t> KpfaComplexMatrix_t;

typedef boost::numeric::ublas::vector<double> KpfaDoubleVector_t;

typedef boost::numeric::ublas::vector<KpfaComplex_t> KpfaComplexVector_t;

#endif
