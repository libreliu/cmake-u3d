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
#include "vqapi.h"
#include <malloc.h>
#include "tristrip.h"
#include <math.h>

// the macro converts void* into unsigned int* and gets current value
#define CAST_TO_UL (*((unsigned int*)(pv)))
// the macro converts void* into long* and gets current value
#define CAST_TO_L (*((long*)(pv)))
// the macro converts void* into char* 
#define CAST_TO_CHAR (*((unsigned char*)(pv)))
// the macro increases the current void* pointer into 1 unsigned int
#define INC_PUL pv = (unsigned int*)pv+1; (*pSize)+=sizeof(unsigned int);
// the macro increases the current void* pointer into 1 unsigned char
#define INC_PCHAR pv = (unsigned char*)pv+1; (*pSize)+=sizeof(unsigned char);

// the function makes a list of tristrps from triangles using 
// adjacency structure and a simple search procedure
bool makeTriStrips(unsigned int* pInd, unsigned int* pMem, unsigned int nInd, 
                   void* pOutInd, unsigned int* pSize, 
                   unsigned int* pNStrip, unsigned int* pNInd)
{
    CAdjList adj;
    CAdj* curtri = 0, *newtri = 0;
    unsigned int edge = ((unsigned int)-1), newedge = ((unsigned int)-1);
    void* pv = pOutInd;                 // current position in pOutInd array
    unsigned char* ps = 0;              // pointer to current strip length element in pOutIndArray
    *pSize = 0; *pNStrip = 0;
    unsigned int*  pTri = pMem;
    unsigned int   nTri = 0;   // number of single triangles written
    unsigned int   vertex1 = (unsigned int)-1, vertex2 = (unsigned int)-1, vertex0 = (unsigned int)-1;
    bool isRev = false;
    unsigned int    nTriProcessed = 0;  // number of triangles processed

    *pNInd = 0;
    // first step - make adjacency structure
    adj.init(pInd, nInd);
    //find the initial triangle with min number of neighbors
    curtri = adj.getTriangleWitnMinNeighbors();
    unsigned int counter = 0;

    while (adj.m_adjList.size()) {
        if (!curtri) {
            *pNStrip = 0;
            *pSize = 0;
            return false;  // internal error - terminate
        }

        // get triangle with min neighbors, ajacent to current one by specified edge
        newtri = curtri->getAdjTriWithMinNeighbors(edge, &newedge);
        if (!curtri->getNumOfNeighbors() || !newtri) {
            // tristrip ended
            if (edge == ((unsigned int)-1)) {
                // the only one triangle in tristrip, save it to the end of the buffer!
//                printf("new tri\n");
                *pTri = curtri->m_v[0]; pTri++;
                *pTri = curtri->m_v[1]; pTri++;
                *pTri = curtri->m_v[2]; pTri++;
                *pTri = curtri->m_index; pTri++;
                nTri++;
                isRev = true;
            } else {
                // the sequence of triangles in tristrip, save only 1 vertex
                pInd[nTriProcessed++] = curtri->m_index;
                CAST_TO_CHAR  = 1; INC_PCHAR;
                CAST_TO_CHAR  = 1; INC_PCHAR; 
                CAST_TO_UL = vertex2; INC_PUL; (*pNInd)++;
//                printf("%d\n", vertex2);
                // increase number of elements in the strip
                *ps = *ps+1;
                *pNStrip = *pNStrip+1; // increase total number of strips
                isRev= !isRev;
            }
            adj.removeTri(curtri);
            curtri = adj.getTriangleWitnMinNeighbors();
            edge = ((unsigned int)-1);
        } else {
            if (edge == ((unsigned int)-1)) {
                isRev = true;
                // it was the begining of tristrip
                pInd[nTriProcessed++] = curtri->m_index;
//                printf("new strip\n");
                // save size of strip
                CAST_TO_CHAR = 3; ps = (unsigned char*)pv; INC_PCHAR;
                CAST_TO_CHAR  = 1; INC_PCHAR;
                // save all three vertices
                CAST_TO_UL = curtri->getThirdVertex(newedge); INC_PUL; (*pNInd)++;
//                printf("%d\n", curtri->getThirdVertex(newedge));
                vertex1 = curtri->getEdgeVertex1(newedge);
//                printf("%d\n", vertex1);
                vertex2 = curtri->getEdgeVertex2(newedge);
//                printf("%d\n", vertex2);
                CAST_TO_CHAR  = 1; INC_PCHAR;
                CAST_TO_CHAR  = 1; INC_PCHAR;
                CAST_TO_UL = vertex1; INC_PUL; (*pNInd)++;
                CAST_TO_CHAR  = 1; INC_PCHAR;
                CAST_TO_CHAR  = 1; INC_PCHAR;
                CAST_TO_UL = vertex2; INC_PUL; (*pNInd)++;
            } else {
                // tristrip continued
                pInd[nTriProcessed++] = curtri->m_index;
                CAST_TO_CHAR  = 1; INC_PCHAR;
                CAST_TO_CHAR  = 1; INC_PCHAR;
                CAST_TO_UL = vertex2; INC_PUL; (*pNInd)++;
//                printf("%d\n", vertex2);
                // increase number of elements in the strip
                *ps = *ps+1;
                isRev = !isRev;
            }

            vertex0 = vertex1;
            vertex1 = vertex2;
            vertex2 = newtri->getThirdVertex(newtri->getEdge(vertex0, vertex1, isRev));
            edge = newtri->getEdge(vertex1, vertex2, isRev);
            adj.removeTri(curtri);
            curtri = newtri;
          
            if (*ps > 256-3) {
                // terminate the strip, begin the next one
                edge = ((unsigned int)-1);
                *pNStrip = *pNStrip+1; // increase total number of strips
            }
        }
    }

    // move all of the single triangles to the end of stripbuffer
    // save triangles
    pTri = pMem;
    for (unsigned int i = 0; i < nTri*4; i+=4) {
        CAST_TO_CHAR  = ((i)?0:2);
        INC_PCHAR;
        CAST_TO_CHAR  = 0; INC_PCHAR;
        CAST_TO_UL = *pTri; 
        INC_PUL; (*pNInd)++; pTri++;

        CAST_TO_CHAR  = 0;
        INC_PCHAR;
        CAST_TO_CHAR  = 0; INC_PCHAR;
        CAST_TO_UL = *pTri; 
        INC_PUL; (*pNInd)++; pTri++;

        CAST_TO_CHAR  = 0;
        INC_PCHAR;
        CAST_TO_CHAR  = 0; INC_PCHAR;
        CAST_TO_UL = *pTri; 
        INC_PUL; (*pNInd)++; pTri++;

        pInd[nTriProcessed++] = *pTri; pTri++;
    }

    return true;
}
