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
#ifndef ____TRISTRIP
#define ____TRISTRIP

// Adjacency structure is defined here
class CAdj;
class CAdjVector {
public:
    CAdjVector(unsigned int reserved = 0);
    ~CAdjVector();

    CAdj* operator[](unsigned int index);

    bool erase(unsigned int index);
    bool push_back(CAdj* newel);
    CAdj* pop_back();
    unsigned int size();
    unsigned int   m_nTotal;   // maximum amount of elements somewhere exists in the array
    CAdj**          m_pData;   // data array
    unsigned int   m_nRes;     // total amount of elements (reserved)
    unsigned int   m_nCur;     // current amount of elements in the array
};

class CAdj {
public:
    CAdj(unsigned int A = ((unsigned int)-1), unsigned int B = ((unsigned int)-1), 
        unsigned int C = ((unsigned int)-1), unsigned int index = ((unsigned int)-1)) 
    { m_v[0] = A; m_v[1] = B, m_v[2] = C; m_index = index;};
    ~CAdj() {};

    unsigned int    m_v[3];     // triangle vertices 0 - A, 1 - B, 2 - C
    CAdjVector      m_at[3];    // arrays of adjacent triangles by edges (0) - AB, (1) - BC, (2) - CA
    unsigned int    m_index;    // index of the triangle

    // the function returns the number of neighbors for the triangle
    unsigned int getNumOfNeighbors();

    // the function returns triangle with min neighbors (but not 0) adjacent not by passed excludeEdge
    // new edge that was the coonecting between to adjacent triangles returned
    // if excludeEdge equals to ((unsigned int)-1) the function checks all three edges
    CAdj* getAdjTriWithMinNeighbors(unsigned int excludeEdge, unsigned int* pNewEdge);
    // these function operates with triangle information:
    // the function gets the vertex of the triangle that doesn't lie on passed edge
    unsigned int getThirdVertex(unsigned int edge);
    // the function returns first edge vertex
    unsigned int getEdgeVertex1(unsigned int edge);
    // the function returns second edge vertex
    unsigned int getEdgeVertex2(unsigned int edge);
    // the function gets edge defined by a pair of vertices.
    unsigned int getEdge(unsigned int vertex1, unsigned int vertex2, bool isRev);

    void removeTri(CAdj* pTri);
    void removeMyself();
private:
    // service functions
    unsigned int getAdjTriWithMinNeighborsByEdge(unsigned int, CAdj**);
};

class CAdjList {
public:
    CAdjList() {};
    ~CAdjList() { reset();};

    CAdjVector      m_adjList;
    unsigned int*  m_pInd;
    unsigned int   m_nInd;
    CAdj*           m_pMem;

    void init(unsigned int* pInd, unsigned int nInd);
    void reset();
    CAdj* getTriangleWitnMinNeighbors();
    void removeTri(CAdj* pTri);
};

#endif // ____TRISTRIP
