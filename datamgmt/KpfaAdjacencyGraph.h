/*
 * KpfaAdjacencyGraph.h
 *
 *  Created on: 2015. 5. 15.
 *      Author: Youngsun Han
 */

#ifndef _KPFA_ADJACENCY_GRAPH_H_
#define _KPFA_ADJACENCY_GRAPH_H_

#include "KpfaDebug.h"

typedef set<uint32_t> KpfaVertexSet_t;
typedef map<uint32_t, uint32_t> KpfaVertexTable_t;
typedef map<uint32_t, KpfaVertexSet_t> KpfaVertexSetTable_t;

/**
 * The declaration of the class for Raw Data Management
 */
class KpfaAdjacencyGraph {

private:

    // adjacency lists
    KpfaVertexSetTable_t m_rAdjacencyList;

    // a list of the pairs of vertex ID and subgraph ID
    KpfaVertexTable_t m_rSubgraphTable;

public:

	KpfaAdjacencyGraph();

	virtual ~KpfaAdjacencyGraph();

	/**
	 * This function will be used to insert a new edge into the adjacency table.
	 *
	 * @param nI 1'st vertex ID of the inserted edge
	 * @param nj 2'nd vertex ID of the inserted edge
	 */
	inline void AddEdge(uint32_t nI, uint32_t nJ) {

		m_rAdjacencyList[nI].insert(nJ);
		m_rAdjacencyList[nJ].insert(nI);
	}    

	KpfaVertexTable_t &SplitSubgraphs(uint32_t &nSubgraphCount);

	///////////////////////////////////////////////////////////////////
	// Debugging Functions
	///////////////////////////////////////////////////////////////////

	virtual void Write(ostream &rOut);

	friend ostream &operator << (ostream &rOut, KpfaAdjacencyGraph *pGraph);

private:

    bool TraverseDFS(uint32_t nId, uint32_t nSubgraphId);

};

#endif /* _KPFA_ADJACENCY_GRAPH_H_ */
