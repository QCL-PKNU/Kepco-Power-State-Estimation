/*
 * KpfaLinearSystem.h
 *
 *  Created on: Apr 23, 2015
 *      Author: youngsun
 */

#ifndef _KPFA_LINEAR_SYSTEM_H_
#define _KPFA_LINEAR_SYSTEM_H_

#include "KpfaDebug.h"
#include "KpfaConfig.h"

// Linear system solver types
#define KPFA_LU_SOLVER		0
#define KPFA_PCG_SOLVER	 	1
#define KPFA_MA28_SOLVER	2

// Sparsity types for vectors
#define KPFA_DENSE_VECTOR	0
#define KPFA_SPARSE_VECTOR	1

// For using dense vector for B, X
#define KPFA_VECTOR_SPARSITY		KPFA_DENSE_VECTOR

// For indicating the employed solver
#define KPFA_LINEAR_SYSTEM_SOLVER	KPFA_MA28_SOLVER

// Max iteration for PCG
#define KPFA_MAX_PCG_ITERATION 		1000

class KpfaLinearSystem {

public:

	KpfaLinearSystem();

	virtual ~KpfaLinearSystem();

	// Solve A * X = B to get X
	KpfaError_t Solve(KpfaDoubleMatrix_t &rAmat,
	                  KpfaDoubleVector_t &rBmat,
					  KpfaDoubleVector_t &rXmat);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaLinearSystem *pLinearSystem);

private:

#if KPFA_LINEAR_SYSTEM_SOLVER == KPFA_LU_SOLVER

	// Use LU factorization
	template <class M, class V>
	KpfaError_t Solve_LU(M &rAmat, V &rBmat, V &rXmat);

#elif KPFA_LINEAR_SYSTEM_SOLVER == KPFA_PCG_SOLVER

	// Use preconditioned conjugate gradient
	template <class M, class V>
	KpfaError_t Solve_PCG(M &rAmat, V &rBmat, V &rXmat);

#elif KPFA_LINEAR_SYSTEM_SOLVER == KPFA_MA28_SOLVER

	// Use MA28 library
	template <class M, class V>
	KpfaError_t Solve_MA28(M &rAmat, V &rBmat, V &rXmat);

#endif
};

#endif /* _KPFA_LINEAR_SYSTEM_H_ */
