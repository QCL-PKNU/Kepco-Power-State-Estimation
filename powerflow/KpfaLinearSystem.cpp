/*
 * KpfaLinearSystem.cpp
 *
 *  Created on: Apr 23, 2015
 *      Author: youngsun
 */

#include "KpfaLinearSystem.h"
#if KPFA_LINEAR_SYSTEM_SOLVER == KPFA_PCG_SOLVER
#include "pcg.hpp"
#endif

KpfaLinearSystem::KpfaLinearSystem() {
	// do nothing
}

KpfaLinearSystem::~KpfaLinearSystem() {
	// do nothing
}

/**
 * This function will solve the linear system (A*X=B) using one of the following methods.
 * 1. LU factorization
 * 2. Conjugate Gradient with Cholesky Preconditioner
 * 3. MA28 library
 *
 * @param rAmat A matrix
 * @param rBmat B matrix
 * @param rXmat X matrix to be solved
 * @return error information
 */
KpfaError_t
KpfaLinearSystem::Solve(KpfaDoubleMatrix_t &rAmat,
						KpfaDoubleVector_t &rBmat,
						KpfaDoubleVector_t &rXmat) {

	KpfaError_t error;

#if KPFA_LINEAR_SYSTEM_SOLVER == KPFA_LU_SOLVER
	error = Solve_LU(rAmat, rBmat, rXmat);
#elif KPFA_LINEAR_SYSTEM_SOLVER == KPFA_PCG_SOLVER
	error = Solve_PCG(rAmat, rBmat, rXmat);
#elif KPFA_LINEAR_SYSTEM_SOLVER == KPFA_MA28_SOLVER
	error = Solve_MA28(rAmat, rBmat, rXmat);
#endif

	return error;
}

///////////////////////////////////////////////////////////////////
// Linear System Solvers
///////////////////////////////////////////////////////////////////

#if KPFA_LINEAR_SYSTEM_SOLVER == KPFA_LU_SOLVER
/**
 * This function will solve the linear system (A*X=B) using LU factorization.
 *
 * @param rAmat A matrix
 * @param rBmat B matrix
 * @param rXmat X matrix to be solved
 * @return error information
 */
template <class M, class V>
KpfaError_t
KpfaLinearSystem::Solve_LU(M &rAmat, V &rBmat, V &rXmat) {

	uint32_t msize = rAmat.size1();

	// create identity matrix of "inverse"
	KpfaDoubleMatrix_t vmat(msize, msize);
	vmat.assign(identity_matrix<double>(msize));

	// create a permutation matrix for the LU-factorization
	permutation_matrix<std::size_t> pmat(msize);

	// perform LU-factorization
	if(lu_factorize(rAmat, pmat) != 0) {
		return KPFA_ERROR_LU_FACTORIZE;
	}

	// back-substitute to get the inverse
	lu_substitute(rAmat, pmat, vmat);

	// X = A_inv * B
	rXmat = block_prod<V, 64>(vmat, rBmat);

	return KPFA_SUCCESS;
}

#elif KPFA_LINEAR_SYSTEM_SOLVER == KPFA_PCG_SOLVER
/**
 * This function will solve the linear system (A*X=B)
 * using conjugate gradient method with Cholesky preconditioner.
 *
 * @param rAmat A matrix
 * @param rBmat B matrix
 * @param rXmat X matrix to be solved
 * @return error information
 */
template <class M, class V>
KpfaError_t
KpfaLinearSystem::Solve_PCG(M &rAmat, V &rBmat, V &rXmat) {

	CholeskyPreconditioner<M> precond(rAmat);

	// Initial Guess
	rXmat = rBmat;

	// Solve preconditioned conjugate gradient
	size_t niter = pcg_solve(rAmat, rXmat, rBmat, precond,
							 KPFA_MAX_PCG_ITERATION);

	if(niter == KPFA_MAX_PCG_ITERATION) {
		return KPFA_ERROR_PCG_MAX_ITERATION;
	}

	return KPFA_SUCCESS;
}
#elif KPFA_LINEAR_SYSTEM_SOLVER == KPFA_MA28_SOLVER

///////////////////////////////////////////////////////////////////
// C++ to Fortran interface for MA28 library 
///////////////////////////////////////////////////////////////////

typedef int* F_INT;
typedef float* F_REAL;
typedef double* F_DOUBLE;

extern "C" {
void ma28_solve_linear_system_(
	F_INT/*N*/,F_INT/*NZ*/,F_DOUBLE/*A*/,F_INT/*LICN*/,F_INT/*ICN*/,
	F_INT/*LIRN*/,F_INT/*IRN*/,F_DOUBLE/*X*/,F_INT/*ERROR*/);
}

/**
 * This function will solve the linear system (A*X=B)
 * using MA28 library written in Fortran.
 *
 * @param rAmat A matrix
 * @param rBmat B matrix
 * @param rXmat X matrix to be solved
 * @return error information
 */
template <class M, class V>
KpfaError_t
KpfaLinearSystem::Solve_MA28(M &rAmat, V &rBmat, V &rXmat) {

    // error flag for ma28ad
    int error = 0;

    // # of non-zero elements
    int nz = rAmat.nnz();

    // Input matrix size
    int n = rAmat.size1();

    // YOUNGSUN - CHKME
    // Array sizes for using MA28 written in Fortran
    int tnz = nz << 2, tn = n << 0;

    // icn, irn arrays
    int lirn = tnz;
    int licn = tnz;

    int *icn = new int[lirn];
    int *irn = new int[licn];

    // A matrix and X(B) vector
    double *amat = new double[tnz];
    double *xmat = new double[tn ];

    KpfaDoubleVector_t::iterator  viter;
    KpfaDoubleMatrix_t::iterator1 riter;
    KpfaDoubleMatrix_t::iterator2 citer;

    // Fill irn, icn amatays with the non-zero elements of rAmat.
    int i, j, k = 0;

    for(riter = rAmat.begin1(); riter != rAmat.end1(); riter++) {
    	i = riter.index1()+1;
    	for(citer = riter.begin(); citer != riter.end(); citer++) {
        	j = citer.index2()+1;

        	amat[k] = *citer;
            irn[k] = i;
            icn[k] = j;

            k++;
        }
    }

#ifdef KPFA_DUMP_ALL_MATRIX
{
	ofstream o;
	o.open("./output/ma28.out", ios::out);
	for(i = 0; i < k; i++) {
		o << i+1 << ",";
		o << std::scientific << amat[i] << ",";
		o << std::fixed << irn[i] << "," << icn[i] << endl;
	}
	o.close();
}
#endif

    KPFA_CHECK(k == nz, KPFA_ERROR_WRONG_NNZ);

#if KPFA_VECTOR_SPARSITY == KPFA_DENSE_VECTOR
    for(i = 0; i < n; i++) {
    	xmat[i] = rBmat[i];
    }
#elif KPFA_VECTOR_SPARSITY == KPFA_SPARSE_VECTOR
    memset(xmat, 0, sizeof(double)*n);
    for(viter = rBmat.begin(); viter != rBmat.end(); viter++) {
    	xmat[viter.index()] = *viter;
    }
#endif

    // Solve the linear system using MA28 library
    ma28_solve_linear_system_(&n, &nz, amat, &licn, icn, &lirn, irn, xmat, &error);

    // YOUNGSUN - CHKME
#if 1
    if(error < 0) {
        return KPFA_ERROR_MA28AD_FAIL;
    }
#endif

    // Return X matrix
#if KPFA_VECTOR_SPARSITY == KPFA_DENSE_VECTOR
    for(i = 0; i < n; i++) {
    	rXmat(i) = xmat[i];
    }
#elif KPFA_VECTOR_SPARSITY == KPFA_SPARSE_VECTOR
    rXmat.clear();
    for(i = 0; i < n; i++) {
    	double tmp = xmat[i];
    	if(tmp != 0) rXmat(i) = tmp;
    }
#endif

    return KPFA_SUCCESS;
}

#endif

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaLinearSystem::Write(ostream &rOut) {
	// do nothing yet
}

ostream &operator << (ostream &rOut, KpfaLinearSystem *pLinearSystem) {
	pLinearSystem->Write(rOut);
	return rOut;
}
