//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************
#include "tristrip.h"
#include "vqapi.h"
#include <malloc.h>
#include <string.h>


unsigned int CAdj::getNumOfNeighbors()
{
    return (unsigned int)(m_at[0].size() + m_at[1].size() + m_at[2].size());
}

CAdj* CAdj::getAdjTriWithMinNeighbors(unsigned int adjacentEdge, unsigned int* pNewEdge)
{
    unsigned int min[3] = {0, 0, 0};
    unsigned int totalmin = ((unsigned int)-1), minedge = ((unsigned int)-1);
    CAdj* pNewTri, *pMinTri = 0;

    if(adjacentEdge == -1) {
        for (unsigned int i = 0; i < 3; i++) {
            min[i] = getAdjTriWithMinNeighborsByEdge(i, &pNewTri);
            if (min[i] < totalmin) {
                totalmin = min[i];
                minedge = i;
                pMinTri = pNewTri;
            }
        }
    } else {
            totalmin = getAdjTriWithMinNeighborsByEdge(adjacentEdge, &pNewTri);
            pMinTri = pNewTri;
            minedge  = adjacentEdge;
    } /* if */

    if (totalmin == ((unsigned int)-1) || minedge == ((unsigned int)-1)) {
        // the triangle doesn't have neighbors at all
        pMinTri     = 0;
        *pNewEdge   = (unsigned int)-1;
    } /* if */
    *pNewEdge = minedge;
    return pMinTri;
}

// internal service function
unsigned int CAdj::getAdjTriWithMinNeighborsByEdge(unsigned int nEdge, CAdj** pMin)
{
    *pMin = 0;
    if (!m_at[nEdge].size()) {
        return ((unsigned int)-1);
    }

    unsigned int min = ((unsigned int)-1);
    unsigned int cur;
    *pMin = 0;

    for (unsigned int i = 0; i < m_at[nEdge].m_nTotal; i++) {
        if (m_at[nEdge][i]) {
            cur = m_at[nEdge][i]->getNumOfNeighbors();
            if (min > cur) {
                *pMin = m_at[nEdge][i];
                min = cur;
            }
        }
    }

    return min;
}

unsigned int CAdj::getThirdVertex(unsigned int edge)
{
    switch (edge) {
    case 0:
        return m_v[2];
    case 1:
        return m_v[0];
    case 2:
        return m_v[1];
    default:
        return ((unsigned int)-1);  // error
    }
}

unsigned int CAdj::getEdgeVertex1(unsigned int edge)
{
    if (edge < 3)
        return m_v[edge];
    else
        return ((unsigned int)-1);  // error
}

unsigned int CAdj::getEdgeVertex2(unsigned int edge)
{
    switch (edge) {
    case 0:
    case 1:
        return m_v[edge+1];
    case 2:
        return m_v[0];
    default:
        return ((unsigned int)-1);  // error
    }
}

unsigned int CAdj::getEdge(unsigned int vertex1, unsigned int vertex2, bool isRev)
{
    if (isRev) {
        if (vertex1 == m_v[1] && vertex2 == m_v[0])
            return 0;   // AB edge
        else if (vertex1 == m_v[2] && vertex2 == m_v[1])
            return 1;   // BC edge
        else if (vertex1 == m_v[0] && vertex2 == m_v[2])
            return 2;   // CA edge
        else
            return ((unsigned int)-1);  // error
    } else {
        if (vertex1 == m_v[0] && vertex2 == m_v[1])
            return 0;   // AB edge
        else if (vertex1 == m_v[1] && vertex2 == m_v[2])
            return 1;   // BC edge
        else if (vertex1 == m_v[2] && vertex2 == m_v[0])
            return 2;   // CA edge
        else
            return ((unsigned int)-1);  // error
    }
}

void CAdj::removeTri(CAdj* pTri)
{
    for (unsigned int i = 0; i < 3; i++) {
        for (unsigned int j = 0; j < m_at[i].m_nTotal;) {
            if (m_at[i][j] == pTri) {
                m_at[i].erase(j);
            } else {
                j++;
            }
        }
    }
}

void CAdjList::init(unsigned int* pInd, unsigned int nInd)
{
    unsigned int i;
    m_nInd = nInd;
    m_pInd = pInd;
    CAdj* newadj;
    for (i = 0; i < m_nInd; i+=3) {
        newadj = new CAdj(pInd[i], pInd[i+1], pInd[i+2], i/3);
        m_adjList.push_back(newadj);
    }

    // make adjacency
    for (i = 0; i < m_adjList.m_nTotal; i++) {
        CAdj* cur = m_adjList[i];

        // fills in adjacency
        for (unsigned int j = i+1; j < m_adjList.m_nTotal; j++) {
            CAdj* test = m_adjList[j];
            unsigned int curedge = ((unsigned int)-1);
            if ((curedge = cur->getEdge(test->m_v[0], test->m_v[1], true)) != ((unsigned int)-1)) {
                cur->m_at[curedge].push_back(test);
                test->m_at[0].push_back(cur);
            } else if ((curedge = cur->getEdge(test->m_v[1], test->m_v[2], true)) != ((unsigned int)-1)) {
                cur->m_at[curedge].push_back(test);
                test->m_at[1].push_back(cur);
            } else if ((curedge = cur->getEdge(test->m_v[2], test->m_v[0], true)) != ((unsigned int)-1)) {
                cur->m_at[curedge].push_back(test);
                test->m_at[2].push_back(cur);
            }
        }
    }
}

void CAdjList::reset()
{
    while(m_adjList.size()) {
        CAdj* pop = m_adjList.pop_back();
        if (pop)
            delete (pop);
    }
}

CAdj* CAdjList::getTriangleWitnMinNeighbors()
{
    if (!m_adjList.size())
        return 0;
    CAdj* tri = 0;
    for (unsigned int i = 0; i < m_adjList.m_nTotal && tri == 0; i++) {
        tri = m_adjList[i];
    }
    unsigned int min = tri->getNumOfNeighbors();

    for (unsigned int i = 0; i < m_adjList.m_nTotal; i++) {
        if (m_adjList[i] && min > m_adjList[i]->getNumOfNeighbors()) {
            min = m_adjList[i]->getNumOfNeighbors();
            tri = m_adjList[i];
        }
    }

    return tri;
}

void CAdjList::removeTri(CAdj* pTri)
{
    // 1) remove all of the links to the tri from adjacency arrays
    for (unsigned int j = 0; j < 3; j++) {
        for (unsigned int i = 0; i < pTri->m_at[j].m_nTotal; i++) {
            if (pTri->m_at[j].m_pData[i])
                pTri->m_at[j].m_pData[i]->removeTri(pTri);
        }
    }
    // 2) remove the triangle itself
    if (pTri->m_index != ((unsigned int)-1)) {
        m_adjList.erase(pTri->m_index);
        delete(pTri);
    }

/*    unsigned int index = ((unsigned int)-1);
    for (unsigned int i = 0; i < m_adjList.size(); i++) {
        if (m_adjList[i] == pTri) {
            index = i;
        } else 
            m_adjList[i]->removeTri(pTri);
    } 
*/
    return;
}

// CAdjVector class member functions definition
CAdjVector::CAdjVector(unsigned int reserved)
{
    m_pData = 0;
    m_nRes = 0;
    m_nCur = 0;
    m_nTotal = 0;
    if (reserved) {
        // allocate memory
        m_pData = (CAdj**)malloc(sizeof(CAdj*)*reserved);
        if (m_pData) {
            m_nRes = reserved;
        }
    }
}

CAdjVector::~CAdjVector()
{
    // free memory
    if (m_pData)
        free (m_pData);
}

CAdj* CAdjVector::operator[](unsigned int index)
{
    if (m_pData && index < m_nTotal)
        return m_pData[index];
    else
        return 0;
}

unsigned int CAdjVector::size()
{
    return m_nCur;
}

bool CAdjVector::push_back(CAdj* newel)
{
    // allocate/reallocate memory if needed
    if (m_pData) {
        if (m_nTotal >= m_nRes) {
            // reallocate memory
            m_pData = (CAdj**)realloc(m_pData, (m_nRes + 10)*sizeof(CAdj*));
            if (m_pData) {
                m_nRes += 10;
            } else {
                return false;   // error
            }
        }
    } else {
        // no memory is allocated yet - allocate memory for 1000 elements
        m_pData = (CAdj**)malloc(sizeof(CAdj*)*10);
        if (m_pData) {
            m_nRes = 10;
        } else {
            return false;   // error
        }
    }

    // save element
    m_pData[m_nTotal++] = newel;
    m_nCur++;
    return true;
}

bool CAdjVector::erase(unsigned int index)
{
    if (!m_pData || index >= m_nTotal)
        return false;   // error

    m_pData[index] = 0;
    m_nCur--;
    return true;
}

CAdj* CAdjVector::pop_back()
{
    if (!m_pData || !m_nTotal)
        return 0;   // error
    else {
        m_nCur--;
        return m_pData[m_nTotal];
    }
}
