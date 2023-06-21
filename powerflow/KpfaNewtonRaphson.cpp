/*
 * KpfaNewtonRaphson.cpp
 *
 *  Created on: 2014. 6. 2.
 *      Author: Youngsun Han
 */

#include "KpfaNewtonRaphson.h"

KpfaNewtonRaphson::KpfaNewtonRaphson(KpfaRawDataMgmt *pDataMgmt, KpfaNtrapParam_t *pParam) {

	KPFA_ASSERT(pDataMgmt != NULL, "KpfaJacobi: pDataMgmt must not be NULL.");

	m_pDataMgmt = pDataMgmt;

	if(pParam == NULL) {
		m_pParam = new KpfaNtrapParam_t();
		m_pParam->rVoltage = KpfaComplex_t(1.0, 0);
		m_pParam->nTolerance = 0.00001;
		m_pParam->nMaxIteration = 100;
	}
	else {
		m_pParam = pParam;
	}

	m_nMaxTolerance = 0.0;
	m_nMaxPbusId = 0;
	m_nMaxQbusId = 0;

	// Initialize S, V matrices
	uint32_t nbus = pDataMgmt->GetBusCount();

	m_rSmat.resize(nbus, false);
	m_rVmat.resize(nbus, false);
}

KpfaNewtonRaphson::~KpfaNewtonRaphson() {
	// do nothing
}

/**
 * This function will build the initial V matrix using the bus data and 
 * calculate the pmsize and qmsize.
 *
 * @return error information
 */
KpfaError_t 
KpfaNewtonRaphson::BuildVMatrix() {

	// Initialize V matrix
	KpfaComplexVector_t &vmat = m_rVmat;
	vmat.clear();

	// Set voltage, angle
	uint32_t i = 0;
	KpfaRawDataList_t::iterator iter;
	KpfaRawDataList_t &dataList = m_pDataMgmt->GetBusDataList();

	for(iter = dataList.begin(); iter != dataList.end(); iter++) {
		KpfaBusData *bus = (KpfaBusData *)*iter;
		KPFA_CHECK(bus->m_nIdx == i, KPFA_ERROR_INVALID_BUS_INDEX);
#if 1
		vmat(i++) = KpfaComplex_t(bus->m_nVm, bus->m_nVa);
#else
		vmat(i++) = KpfaComplex_t(1.0, 0.0);
#endif
	}

	return KPFA_SUCCESS;
}

/** 
 * This function will be used to update the V matrix using the given delta V matrix.
 *
 * @param rDeltaVmat delta V matrix
 * @return error information
 */
KpfaError_t
KpfaNewtonRaphson::UpdateVMatrix(KpfaDoubleVector_t &rDeltaVmat) {

	uint32_t k, j = 0;

	// V matrix
	KpfaComplexVector_t &vmat = m_rVmat;

	// P, Q bus lists
	KpfaBusIndexMap_t::iterator iter;
	KpfaBusIndexMap_t &pbmap = m_rPbusMap;
	KpfaBusIndexMap_t &qbmap = m_rQbusMap;

	uint32_t pmsize = (uint32_t)pbmap.size();

	// Update voltage angle
	for(iter = pbmap.begin(), j = 0; iter != pbmap.end(); iter++, j++) {
		KpfaComplex_t v_k = vmat((k = iter->first));
		v_k.imag(v_k.imag() + rDeltaVmat(j));
		vmat(k) = v_k;
	}

	// Update voltage magnitude
	for(iter = qbmap.begin(), j = pmsize; iter != qbmap.end(); iter++, j++) {
		KpfaComplex_t v_k = vmat((k = iter->first));
		v_k.real(v_k.real() + rDeltaVmat(j));
		vmat(k) = v_k;
	}

	//cout << vmat << endl;

	return KPFA_SUCCESS;
}

/**
 * This function will build the initial S matrix with the given Y matrix.
 *
 * @param pYmat input Y matrix
 * @return error information
 */

KpfaError_t 
KpfaNewtonRaphson::BuildSMatrix(KpfaYMatrix *pYmat) {

	uint32_t k, j;

    // S, V, Y matrices
	KpfaComplexMatrix_t &ymat = pYmat->GetPolarMatrix();
	KpfaComplexVector_t &vmat = m_rVmat;
	KpfaComplexVector_t &smat = m_rSmat;

	// Initialize S matrix
	smat.clear();

    // Calculate S_kj 
	KpfaComplexMatrix_t::iterator1 riter;
	KpfaComplexMatrix_t::iterator2 citer;

	for(riter = ymat.begin1(); riter != ymat.end1(); riter++) {
        k = riter.index1();
        for(citer = riter.begin(); citer != riter.end(); citer++) {
            j = citer.index2();

            KpfaComplex_t v_k  = vmat(k);        
            KpfaComplex_t v_j  = vmat(j);
            KpfaComplex_t y_kj = ymat(k, j);

			double mag = v_k.real() * v_j.real() * y_kj.real();
			double ang = v_k.imag() - v_j.imag() - y_kj.imag();

        	// Calculate P_k, Q_k	
            double p_k = mag * cos(ang);
            double q_k = mag * sin(ang);
            
        	// Set S_k to the sum of P_k + j Q_k
            smat(k) += KpfaComplex_t(p_k, q_k);
	    }

        // PVTEST in KU Fortran
#ifdef KPFA_APPLY_CONVERGE_HEURISTIC		
        // YOUNGSUN - CHKME 
		KpfaBusData *bus = m_pDataMgmt->GetBusDataAt(k);
		KpfaGenData *gen = m_pDataMgmt->GetGenData(bus->m_nI);
		
		double tmpQ = smat(k).imag() + bus->m_nQl;
				
		switch(bus->m_nIde) {
			case KPFA_GEN_BUS: {
				if(tmpQ < gen->m_nQb) {
					bus->m_nIde = KPFA_LOAD_BUS;
					bus->m_nQg = gen->m_nQb;
				}
				else if(tmpQ > gen->m_nQt) {
					bus->m_nIde = KPFA_LOAD_BUS;
					bus->m_nQg = gen->m_nQt;
				}
				break;
			}
			// YOUNGSUN - CHKME
			// A load bus is able to be changed to a generator bus 
			// if the bus was a generator bus before. 
			case KPFA_LOAD_BUS: {
				if(gen != NULL && tmpQ > gen->m_nQb && tmpQ < gen->m_nQt) {
					bus->m_nIde = KPFA_GEN_BUS;
				}
				break;
			}
			default: break;
		}
#endif
	}

	return KPFA_SUCCESS;
}

/** 
 * This function will be used to update the S matrix using the given delta S matrix.
 *
 * @param rDeltaVmat delta S matrix
 * @return error information
 */
KpfaError_t
KpfaNewtonRaphson::UpdateSMatrix(KpfaDoubleVector_t &rDeltaSmat) {

	uint32_t k, j;

	// S matrix
	KpfaComplexVector_t &smat = m_rSmat;

	// P, Q bus lists
	KpfaBusIndexMap_t::iterator iter;
	KpfaBusIndexMap_t &pbmap = m_rPbusMap;
	KpfaBusIndexMap_t &qbmap = m_rQbusMap;

	uint32_t pmsize = (uint32_t)pbmap.size();

	// Update P
	for(iter = pbmap.begin(), j = 0; iter != pbmap.end(); iter++, j++) {
		KpfaComplex_t s_k = smat((k = iter->first));
		s_k.real(s_k.real() + rDeltaSmat(j));
		smat(k) = s_k;
	}

	// Update Q
	for(iter = qbmap.begin(), j = pmsize; iter != qbmap.end(); iter++, j++) {
		KpfaComplex_t s_k = smat((k = iter->first));
		s_k.imag(s_k.imag() + rDeltaSmat(j));
		smat(k) = s_k;
	}

	return KPFA_SUCCESS;
}

/**
 * This function will build P, Q bus index maps, respectively.
 *
 * @return error information
 */
KpfaError_t
KpfaNewtonRaphson::BuildPqBusIndexMaps() {

	// Build P, Q bus index maps 
	KpfaBusIndexMap_t &pbmap = m_rPbusMap;
	KpfaBusIndexMap_t &qbmap = m_rQbusMap;

	pbmap.clear();
	qbmap.clear();    

	uint32_t k = 0, npb = 0, nqb = 0;

	KpfaRawDataList_t::iterator iter;
	KpfaRawDataList_t &dataList = m_pDataMgmt->GetBusDataList();

	for(iter = dataList.begin(); iter != dataList.end(); iter++, k++) {
		KpfaBusData *bus = (KpfaBusData *)*iter;

		switch(bus->m_nIde) {
			case KPFA_GEN_BUS:
				// Keep to only P matrix
                pbmap[k] = npb++;
				break;
			case KPFA_LOAD_BUS:
				// Keep to both P, Q matrices
                pbmap[k] = npb++;
                qbmap[k] = nqb++;
				break;
			default:
				break;
		}
	}

	return KPFA_SUCCESS;
}

/**
 * This function will build the delta S matrix with the original S matrix.
 *
 * @return error information
 */
 
KpfaError_t
KpfaNewtonRaphson::CalculateDeltaSMatrix(KpfaDoubleVector_t &rDeltaSmat) {

	// S, Delta S matrices
	KpfaComplexVector_t &smat = m_rSmat;

	uint32_t pmsize = m_rPbusMap.size();
	uint32_t qmsize = m_rQbusMap.size();

	rDeltaSmat.resize(pmsize + qmsize, false);
	rDeltaSmat.clear();

	// Reset the maximum Tolerance
	double maxtol = 0;
	uint32_t maxpid = 0;
	uint32_t maxqid = 0;

	uint32_t i = 0, k;
	
	KpfaRawDataList_t::iterator iter;
	KpfaRawDataList_t &dataList = m_pDataMgmt->GetBusDataList();

	// P matrix (Both generator and load)
	for(iter = dataList.begin(), k = 0; iter != dataList.end(); iter++, k++) {

		KpfaBusData *bus = (KpfaBusData *)*iter;
		if(bus->m_nIde == KPFA_SWING_BUS) continue;

		KpfaComplex_t s_k = smat(k);

		double pval = bus->m_nPg - bus->m_nPl - s_k.real();

		if(pval != 0) {
			rDeltaSmat(i) = pval;
		}
		i++;
		
		// Update the maximum tolerance info.
		////////////////////////////////////////////////////////////
		if(fabs(pval) > maxtol) {
			maxtol = fabs(pval);
			maxpid = bus->m_nI;
			maxqid = 0;
		}
	}

	// Q matrix (Only load)
	for(iter = dataList.begin(), k = 0; iter != dataList.end(); iter++, k++) {

		KpfaBusData *bus = (KpfaBusData *)*iter;
		if(bus->m_nIde == KPFA_SWING_BUS || bus->m_nIde == KPFA_GEN_BUS) continue;

		KpfaComplex_t s_k = smat(k);

		double qval = bus->m_nQg - bus->m_nQl - s_k.imag();

		if(qval != 0) {
			rDeltaSmat(i) = qval;
		}
		i++;

		// Update the maximum tolerance info.
		////////////////////////////////////////////////////////////
		if(fabs(qval) > maxtol) {
			maxtol = fabs(qval);
			maxpid = 0;
			maxqid = bus->m_nI;
		}
	}

	m_nMaxTolerance = maxtol;
	m_nMaxPbusId = maxpid;
	m_nMaxQbusId = maxqid;
	
	return KPFA_SUCCESS;
}

/**
 * This function will build the delta V matrix with the delta S and Jacobian matrices.
 *
 * @param rJmat Jacobian matrix
 * @param rDeltaSmat delta S matrix
 * @param rDeltaVmat output delta V matrix
 * @return error information
 */
 
KpfaError_t
KpfaNewtonRaphson::CalculateDeltaVMatrix(KpfaDoubleMatrix_t &rJmat, 
                                         KpfaDoubleVector_t &rDeltaSmat,
                                         KpfaDoubleVector_t &rDeltaVmat) {
	KpfaLinearSystem jls;

	rDeltaVmat.resize(rDeltaSmat.size(), false);
	rDeltaVmat.clear();

	// Calculate the delta V matrix
	KpfaError_t error = jls.Solve(rJmat, rDeltaSmat, rDeltaVmat);

	if(error != KPFA_SUCCESS) {
		KPFA_ERROR("The linear system for delta V matrix is not solved.");
	}
	return error;
}

/**
 * This function will build a Jacobian matrix using the given S, Y matrices.
 * Info) We could reduce more than 50% execution time employing the following
 * implementation against the original one.
 *
 * @param pYmat Y matrix
 * @param rJmat Jacobian matrix
 * @return error information
 */

KpfaError_t 
KpfaNewtonRaphson::CalculateJacobiMatrix(KpfaYMatrix *pYmat, KpfaDoubleMatrix_t &rJmat) {

	// Y, V matrix
	KpfaComplexVector_t &vmat = m_rVmat;
	KpfaComplexMatrix_t &ymat = pYmat->GetPolarMatrix();

    KpfaBusIndexMap_t &pbmap = m_rPbusMap;
    KpfaBusIndexMap_t &qbmap = m_rQbusMap;

	uint32_t pmsize = m_rPbusMap.size();
	uint32_t qmsize = m_rQbusMap.size();
    uint32_t jmsize = pmsize + qmsize;

 	rJmat.resize(jmsize, jmsize, false);
	rJmat.clear();

  	// Build J1, J2, J3, J4 matrices simultaneously 
	//////////////////////////////////////////////////////////////  

	// Bus index
	uint32_t k, j, i;
    
    // Jacobian index
    uint32_t jm1_k, jm2_k, jm3_k, jm4_k;
    uint32_t jm1_j, jm2_j, jm3_j, jm4_j;

    // Jacobian matrix flag
	uint32_t jm_flag;

    // Bus iterators
    KpfaComplexMatrix_t::iterator1 iter_k;
    KpfaComplexMatrix_t::iterator2 iter_j;
    KpfaComplexMatrix_t::iterator2 iter_i;

    for(iter_k = ymat.begin1(); iter_k != ymat.end1(); iter_k++) {
        KpfaBusData *bus_k = m_pDataMgmt->GetBusDataAt(k = iter_k.index1());
        if(bus_k->m_nIde == KPFA_SWING_BUS) continue;

    	// Jacobian matrix k index
        jm1_k = jm2_k = pbmap[k];
        if(qbmap.count(k)) jm3_k = jm4_k = qbmap[k] + pmsize;

        // y_kk, v_k
        KpfaComplex_t y_kk = ymat(k, k);
        KpfaComplex_t v_k  = vmat(k);

        double tmp2_kk =  v_k.real() * y_kk.real() * cos(y_kk.imag());
        double tmp4_kk = -v_k.real() * y_kk.real() * sin(y_kk.imag());

        for(iter_j = iter_k.begin(); iter_j != iter_k.end(); iter_j++) {
            KpfaBusData *bus_j = m_pDataMgmt->GetBusDataAt(j = iter_j.index2());

            // set jm_flag to indicate which sub-matrices will be updated
            if(bus_k->m_nIde == KPFA_GEN_BUS && bus_j->m_nIde == KPFA_GEN_BUS)        jm_flag = 0x01; // J1
            else if(bus_k->m_nIde == KPFA_GEN_BUS && bus_j->m_nIde == KPFA_LOAD_BUS)  jm_flag = 0x03; // J1,2
            else if(bus_k->m_nIde == KPFA_LOAD_BUS && bus_j->m_nIde == KPFA_GEN_BUS)  jm_flag = 0x05; // J1,3
            else if(bus_k->m_nIde == KPFA_LOAD_BUS && bus_j->m_nIde == KPFA_LOAD_BUS) jm_flag = 0x0F; // J1,2,3,4
            else continue;

        	// Jacobian matrix j index
        	jm1_j = jm3_j = pbmap[j];
            if(qbmap.count(j)) jm2_j = jm4_j = qbmap[j] + pmsize;

            // y_kj, v_j
            KpfaComplex_t y_kj = ymat(k, j);
            KpfaComplex_t v_j  = vmat(j);       

            // off-diagonal
            if(k != j) {
                double tmp = 0;
    			double mag1 = v_k.real() * y_kj.real();
                double mag2 = v_j.real() * mag1;
    			double ang1 = v_k.imag() - v_j.imag() - y_kj.imag();

                // update each of sub-jacobian matrices
                if((jm_flag & 0x01) && (tmp = mag2 * sin(ang1))) rJmat(jm1_k, jm1_j) =  tmp; // J1
                if((jm_flag & 0x02) && (tmp = mag1 * cos(ang1))) rJmat(jm2_k, jm2_j) =  tmp; // J2
                if((jm_flag & 0x04) && (tmp = mag2 * cos(ang1))) rJmat(jm3_k, jm3_j) = -tmp; // J3
                if((jm_flag & 0x08) && (tmp = mag1 * sin(ang1))) rJmat(jm4_k, jm4_j) =  tmp; // J4                   
            }
            // diagonal 
            else {
                double tmp1 = 0;
                double tmp2 = tmp2_kk;
                double tmp3 = 0;
                double tmp4 = tmp4_kk;

                for(iter_i = iter_k.begin(); iter_i != iter_k.end(); iter_i++) {
                  
                    // y_ki, v_i
                    KpfaComplex_t v_i  = vmat(i = iter_i.index2());
                    KpfaComplex_t y_ki = ymat(k, i);

                    double mag1 = v_i.real() * y_ki.real();
                    double mag2 = v_k.real() * mag1;
                    double ang1 = v_k.imag() - v_i.imag() - y_ki.imag();

                    // YOUNGSUN - CHKME
                    if(k != i) {
                        if(jm_flag & 0x01) tmp1 -= mag2 * sin(ang1);
                        if(jm_flag & 0x04) tmp3 += mag2 * cos(ang1);
                    }
                    if(jm_flag & 0x02) tmp2 += mag1 * cos(ang1);
                    if(jm_flag & 0x08) tmp4 += mag1 * sin(ang1);
                }
                
                // update each of sub-jacobian matrices
                if(jm_flag & 0x01) rJmat(jm1_k, jm1_j) = tmp1; // J1
                if(jm_flag & 0x02) rJmat(jm2_k, jm2_j) = tmp2; // J2
                if(jm_flag & 0x04) rJmat(jm3_k, jm3_j) = tmp3; // J3
                if(jm_flag & 0x08) rJmat(jm4_k, jm4_j) = tmp4; // J4
            }
        }
    }

    return KPFA_SUCCESS;
}

/**
 * This function will be used to the validity of the result of newton-raphson method.
 *
 * @param rYmat input Y matrix
 * @param rVmat input V matrix
 * @return error information
 */
KpfaError_t 
KpfaNewtonRaphson::CheckValidity(KpfaComplexMatrix_t &rYmat, KpfaComplexVector_t &rVmat) {

	int k, j;
	bool_t flag;
	KpfaRawDataList_t::iterator riter;
	KpfaRawDataList_t::iterator citer;
	KpfaRawDataList_t &busList = m_pDataMgmt->GetBusDataList();

	// k
	for(k = 0, riter = busList.begin(); riter != busList.end(); riter++, k++) {

		KpfaBusData *bus_k = (KpfaBusData *)*riter;

		if(bus_k->m_nIde != KPFA_GEN_BUS) {
			continue;
		}

		flag = FALSE;

		KpfaGenData *gen_k = m_pDataMgmt->GetGenData(bus_k->m_nI);
		KPFA_CHECK(gen_k != NULL, KPFA_ERROR_INVALID_BUS_ID);

		KpfaComplex_t v_k = rVmat(k);
		
		double vm_k = bus_k->m_nVm;
		double va_k = v_k.imag();

		double q_k = 0;
		double p_k = 0;

		for(j = 0, citer = busList.begin(); citer != busList.end(); citer++, j++) {

			KpfaComplex_t y_kj = rYmat(k, j);
			KpfaComplex_t v_j  = rVmat(j);
			
			q_k += abs(y_kj) * v_j.real() * sin(va_k - v_j.imag() - arg(y_kj));  
		}

		q_k *= vm_k;

		if(q_k < gen_k->m_nQb) {
			q_k = gen_k->m_nQb;
			flag = TRUE;
		}
		else if(q_k > gen_k->m_nQt) {
			q_k = gen_k->m_nQt;
			flag = TRUE;
		}

		bus_k->m_nQg = q_k;

		// update Vm
		/////////////////////////////////////////////////////////////////////////
		if(flag == FALSE) continue;

		p_k = gen_k->m_nPg;

		// S_k
		KpfaComplex_t tmp(p_k, -q_k);
	
		// S_k / V_k
		tmp /= KpfaComplex_t(vm_k * cos(-va_k), vm_k * sin(-va_k));

		// {S_k / V_k} - Sum of Y_kj * Vj
		for(j = 0, citer = busList.begin(); citer != busList.end(); citer++, j++) {

			if(k == j) continue;

			KpfaComplex_t y_kj = rYmat(k, j);
			KpfaComplex_t v_j  = rVmat(j);

			double vm_j = v_j.real();
			double va_j = v_j.imag();

			tmp -= y_kj * KpfaComplex_t(vm_j * cos(va_j), vm_j * sin(va_j));  
		}

		// {{S_k / V_k} - Sum of Y_kj * Vj} / Y_kk
#if 0
		tmp /= rYmat(k, k);
#endif

		// new Vm 
		bus_k->m_nVm = abs(tmp);
	}

	return KPFA_SUCCESS;
}

/**
 * This function will be used to calculate the result of the powerflow analysis
 * by applying the Newton-Raphson method to the given Y matrix.
 *
 * @param pYmat input Y matrix, already reduced
 * @return error information
 */

KpfaError_t
KpfaNewtonRaphson::Calculate(KpfaYMatrix *pYmat) {

	uint32_t i;
	KpfaError_t error;

    KpfaDoubleMatrix_t &jmat = m_rJmat;
    KpfaDoubleVector_t &dsmat = m_rDeltaSmat;
    KpfaDoubleVector_t &dvmat = m_rDeltaVmat;

	double tolerance = m_pParam->nTolerance;
	uint32_t maxiter = m_pParam->nMaxIteration;

    // Dump Y matrix
	KPFA_DUMP_COMPLEX_MATRIX("./output/ymat.out", pYmat->GetMatrix());
    
	// Build an initial V matrix
	error = BuildVMatrix();
	KPFA_CHECK(error == KPFA_SUCCESS, error);

	KPFA_DUMP_COMPLEX_MATRIX("./output/vmat.out", pYmat->GetMatrix());

	for(i = 1; i < maxiter;i++) {

		// Build an initial S matrix with P, Q
		error = BuildSMatrix(pYmat);
		KPFA_CHECK(error == KPFA_SUCCESS, error);

        KPFA_DUMP_COMPLEX_VECTOR("./output/smat.out", m_rSmat);

    	// Build P, Q bus index maps first
    	error = BuildPqBusIndexMaps();
    	KPFA_CHECK(error == KPFA_SUCCESS, error);

		// Calculate the delta S matrix
		error = CalculateDeltaSMatrix(dsmat);
		KPFA_CHECK(error == KPFA_SUCCESS, error);

		KPFA_DUMP_DOUBLE_VECTOR("./output/dsmat.out", m_rDeltaSmat);

		// Calculate the Jacobian matrix
		//KPFA_START_TIMER();
        error = CalculateJacobiMatrix(pYmat, jmat);
		KPFA_CHECK(error == KPFA_SUCCESS, error);
		//KPFA_PRINT_TIMER("CalculateJacobiMatrix");
        
		KPFA_DUMP_DOUBLE_MATRIX("./output/jmat.out", m_rJmat);

		// Calculate the delta V matrix
		error = CalculateDeltaVMatrix(jmat, dsmat, dvmat);
		KPFA_CHECK(error == KPFA_SUCCESS, error);		

        KPFA_DUMP_DOUBLE_VECTOR("./output/dvmat.out", m_rDeltaVmat);

		// Update the V matrix using the given delta V matrix		
		error = UpdateVMatrix(dvmat);
		KPFA_CHECK(error == KPFA_SUCCESS, error);

        KPFA_DUMP_COMPLEX_VECTOR("./output/vmat.out", m_rVmat);

		// Check validity
		//error = CheckValidity(pYmat->GetMatrix(), m_rVmat);
		//KPFA_CHECK(error == KPFA_SUCCESS, error);

		// For debugging
		/////////////////////////////////////////////////////////////
#if 1
		KPFA_DEBUG("NewtonRaphson", "Iteration number: %d", i);
		KPFA_DEBUG("NewtonRaphson", "Maximum mismatch: %f, P: %d, Q: %d", 
					m_nMaxTolerance, m_nMaxPbusId, m_nMaxQbusId);
#endif
		/////////////////////////////////////////////////////////////

		// Check if the solution is converged
		if(m_nMaxTolerance < tolerance) {
			break;
		}
	}

	if(i == maxiter) {
		KPFA_ERROR("Powerflow solution is not converged");
		return KPFA_ERROR_NOT_CONVERGED;
	}

	return KPFA_SUCCESS;
}

void
KpfaNewtonRaphson::Write(ostream &rOut) {
    // do nothing
}

ostream &operator << (ostream &rOut, KpfaNewtonRaphson *pNrap) {
	pNrap->Write(rOut);
	return rOut;
}
