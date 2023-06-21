/*
 * KpfaAdjacencyGraph.cpp
 *
 *  Created on: 2015. 5. 19.
 *      Author: Youngsun Han
 */

#include "KpfaAdjacencyGraph.h"

/**
 * This function will be used to initialize all the data structures for adjacency graph.
 */
KpfaAdjacencyGraph::KpfaAdjacencyGraph() {
    m_rAdjacencyList.clear();
    m_rSubgraphTable.clear();
}

/**
 * This function will be used to finalize all the data structures for adjacency graph.
 */
KpfaAdjacencyGraph::~KpfaAdjacencyGraph() {
    m_rAdjacencyList.clear();
    m_rSubgraphTable.clear();
}

/**
 * This function will traverse all the vertices and assign different ID numbers
 * to each of different groups with connected components.
 *
 * @param nSubgraphCount the number of subgraphs
 * @return a table with the entries of vertex ID and subgraph ID
 */
KpfaVertexTable_t &
KpfaAdjacencyGraph::SplitSubgraphs(uint32_t &nSubgraphCount) {

	nSubgraphCount = 0;
	m_rSubgraphTable.clear();
	KpfaVertexSetTable_t::iterator iter;

	// Traverse each vertex with DFS and split subgraphs
	for(iter = m_rAdjacencyList.begin(); iter != m_rAdjacencyList.end(); iter++) {
		if(m_rSubgraphTable.count(iter->first) == 0) {
			TraverseDFS(iter->first, nSubgraphCount++);
		}
	}

	return m_rSubgraphTable;
}

/**
 * This function will traverse the vertex with the given ID and its connected vertices
 * and assign the given subgraph ID to all of them.
 *
 * @param nId the current vertex ID
 * @param nSubgraphId the curret subgraph ID
 * @return if any vertex is newly included into the current subgraph
 */
bool 
KpfaAdjacencyGraph::TraverseDFS(uint32_t nId, uint32_t nSubgraphId) {

  uint32_t ntraversed = 0;

  // If there is no vertex with the given ID, finish to traverse.
  if(m_rAdjacencyList.count(nId) == 0)	return false;

  // If the vertex with the given ID is already traversed, finish to traverse.
  // Otherwise traverse the vertex and assign the current subgraph ID.
  if(m_rSubgraphTable.count(nId) == 0)	m_rSubgraphTable[nId] = nSubgraphId;
  else return false;

  // For all of the sub-vertices,
  KpfaVertexSet_t::iterator iter;
  KpfaVertexSet_t &vset = m_rAdjacencyList[nId];

  for(iter = vset.begin(); iter != vset.end(); iter++) {
    // Traverse sub-vertices recursively
    if(TraverseDFS(*iter, nSubgraphId)) ntraversed++;
  }

  return (ntraversed> 0);
}

///////////////////////////////////////////////////////////////////
// Debugging Functions
///////////////////////////////////////////////////////////////////

void
KpfaAdjacencyGraph::Write(ostream &rOut) {

  KpfaVertexSet_t::iterator viter;
  KpfaVertexSetTable_t::iterator siter;

  for(siter = m_rAdjacencyList.begin(); siter != m_rAdjacencyList.end(); siter++) {

    KpfaVertexSet_t &vset = siter->second;

    rOut << "Adjacent Table Entry[" << m_rSubgraphTable[siter->first];
    rOut << ", " << siter->first << "]: ";

    for(viter = vset.begin(); viter != vset.end(); viter++) {
      rOut << *viter << "/";
    }

    rOut << endl;
  }
}

ostream &operator << (ostream &rOut, KpfaAdjacencyGraph *pGraph) {
  pGraph->Write(rOut);
  return rOut;
}
