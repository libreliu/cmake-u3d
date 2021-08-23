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
#ifndef ____VQAPI
#define ____VQAPI

#include "u3ddefs.h"

// the function makes a list of tristrps from triangles using 
// adjacency structure and a simple search
// the format of pOutInd array is the following:
// (1 byte) - size of the strip (number of indices). != 0
// strip data - number of elements - (unsigned int*sizeof strip)
// then - size of next strip
// all of the single triangles are stored at the end of the array after
// key byte = 0
// the size of pOutInd should be no smaller than nInd*sizeof(unsigned int)
bool makeTriStrips(unsigned int* pInd, unsigned int* pMem, unsigned int nInd, 
                   void* pOutInd, unsigned int* pSize, 
                   unsigned int* pNStrip, unsigned int* pNInd);

#endif // ____VQAPI
