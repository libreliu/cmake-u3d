//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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


#include "LineSetInitialize.h"

IFXRESULT Init_Simplest_AuthorLineSet(IFXAuthorLineSet* pLineSet)
{

#define _NUMLINES      1
#define _NUMPOS        2
#define _NUMNORM         2
#define _NUMDIFFCOLORS   1
#define _NUMSPECCOLORS   1
#define _NUMTEXCOORD     0
#define _NUMMATERIAL     1

#define START_TEX_COORD  0.1f
#define SHIFT_TEX_COORD  0.13f

#define EPS 1.E-5

  IFXRESULT ir = IFX_OK;

  if (!pLineSet) return IFX_E_INVALID_POINTER;

  IFXAuthorLineSetDesc desc;
    desc.m_numLines			= _NUMLINES;
    desc.m_numPositions		= _NUMPOS;
    desc.m_numNormals			= _NUMNORM;
    desc.m_numDiffuseColors	= _NUMDIFFCOLORS;
    desc.m_numSpecularColors	= _NUMSPECCOLORS;
    desc.m_numTexCoords		= _NUMTEXCOORD;
    desc.m_numMaterials		= _NUMMATERIAL;

    pLineSet->Allocate(&desc);
    pLineSet->SetLineSetDesc(&desc);
    U32 i;
    int num = 0;
    int numpos = pLineSet->GetMaxLineSetDesc()->m_numPositions;

	IFXVector3  buff;
	IFXVector3* pVector = &buff;

    // 1 - POSITIONS
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(0,pVector);
    pVector->X() = 10.0f; pVector->Y() = 0.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(1,pVector);

    // 1.A - LINES
    IFXU32Line Line;
    Line.Set(0,1);  pLineSet->SetPositionLine(0, &Line);

    // 2 - NORMALS
    pVector->X() = 0.0f;  pVector->Y() = 0.5f;  pVector->Z() = 0.5f;  pLineSet->SetNormal(0,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.2f;  pVector->Z() = 0.8f;    pLineSet->SetNormal(1,pVector);


    // 3 - DIFFUSE
	IFXVector4 diff_color( 0.0f, 0.0f, 0.0f, 1.0f );
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numDiffuseColors; i++) {
      pLineSet->SetDiffuseColor( i, &diff_color );
	  if( i < 3 )
		  diff_color[2-i] = 1.f;
    }

    //  NORMAL and DIFFUSE COLOR at LINE

    IFXU32Line LineC, LineN;
    int size = pLineSet->GetLineSetDesc()->m_numLines;
    for (int k=0; k < size; k++) {
      
	    LineN.Set(1,0); // 3,3
        LineC.Set(1,0); // 0,0

		pLineSet->SetNormalLine(k,&LineN);
		pLineSet->SetDiffuseLine(k, &LineC);

    }

    // 4 - SPECULAR
	IFXVector4 spec_color( 0.0f, 1.0f, 0.0f, 0.0f );
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numSpecularColors; i++) {
      pLineSet->SetSpecularColor( i, &spec_color );
    }

    // SPECULAR LINE
    Line.Set(0,0);  // for simplicity
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numLines; i++) {
      pLineSet->SetSpecularLine(i, &Line);
    }

    // 5 - TEXTURECOORDS
    F32 tcoord;
	IFXVector4  buffText;
    IFXVector4* pVectorText = &buffText;
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numTexCoords; i++) {
      tcoord = (float)START_TEX_COORD;
		
	  pVectorText->Set(tcoord+i, tcoord+1+i, tcoord+2+i, tcoord+3+i);
      pLineSet->SetTexCoord( i, pVectorText );
    }

        // 6 - MATERIALS
    IFXAuthorMaterial material;
      material.m_uNumTextureLayers  = 0;
      material.m_uOriginalMaterialID  = 1;
      material.m_uDiffuseColors   = TRUE;
      material.m_uSpecularColors    = FALSE;
      material.m_uNormals       = TRUE;

    for (i=0; i<material.m_uNumTextureLayers; i++){
      material.m_uTexCoordDimensions[i] = 0;
    }

    for (i=0; i < pLineSet->GetLineSetDesc()->m_numMaterials; i++) {

        pLineSet->SetMaterial( i, &material );
    }

    // 7 - LINE MATERIALS
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numLines; i++) 
	{
		pLineSet->SetLineMaterial(i,0);
    }


  return ir;
}// Init_Simplest_AuthorLineSet()

IFXRESULT InitAuthorLineSet(IFXAuthorLineSet* pLineSet)
{

#define NUMLINES      33
#define NUMPOS        18
#define NUMNORM         33
#define NUMDIFFCOLORS   5
#define NUMSPECCOLORS   5
#define NUMTEXCOORD     0
#define NUMMATERIAL     2

#define START_TEX_COORD  0.1f
#define SHIFT_TEX_COORD  0.13f

#define EPS 1.E-5

  IFXRESULT ir = IFX_OK;

  if (!pLineSet) return IFX_E_INVALID_POINTER;

  IFXAuthorLineSetDesc desc;
    desc.m_numLines      = NUMLINES;
    desc.m_numPositions    = NUMPOS;
    desc.m_numNormals        = NUMNORM;
    desc.m_numDiffuseColors  = NUMDIFFCOLORS;
    desc.m_numSpecularColors = NUMSPECCOLORS;
    desc.m_numTexCoords    = NUMTEXCOORD;
    desc.m_numMaterials    = NUMMATERIAL;

    pLineSet->Allocate(&desc);
    pLineSet->SetLineSetDesc(&desc);
    U32 i;
    int num = 0;
    int numpos = pLineSet->GetMaxLineSetDesc()->m_numPositions;


	IFXVector3  buff;
	IFXVector3* pVector = &buff;

    // 1 - POSITIONS
    // fill lines as square polylines
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(0,pVector);
    pVector->X() = 10.0f; pVector->Y() = 0.0f;  pVector->Z() = 0.0f;    pLineSet->SetPosition(1,pVector);
    pVector->X() = 10.0f; pVector->Y() = -10.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(2,pVector);
    pVector->X() = 0.0f;  pVector->Y() = -10.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(3,pVector);
    pVector->X() = -10.0f;  pVector->Y() = -10.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(4,pVector);
    pVector->X() = -10.0f;  pVector->Y() = 0.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(5,pVector);
    pVector->X() = -10.0f;  pVector->Y() = 10.0f; pVector->Z() = 0.0f;  pLineSet->SetPosition(6,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 10.0f; pVector->Z() = 0.0f;  pLineSet->SetPosition(7,pVector);
    pVector->X() = 10.0f; pVector->Y() = 10.0f; pVector->Z() = 0.0f;  pLineSet->SetPosition(8,pVector);

    float b = 3.3f;
    pVector->X() = 0.0f + b ; pVector->Y() = 0.0f+ b;   pVector->Z() = 10.0f;   pLineSet->SetPosition(9,pVector);
    pVector->X() = 10.0f + b; pVector->Y() = 0.0f+ b;   pVector->Z() = 10.0f;   pLineSet->SetPosition(10,pVector);
    pVector->X() = 10.0f + b; pVector->Y() = -10.0f+ b; pVector->Z() = 10.0f; pLineSet->SetPosition(11,pVector);
    pVector->X() = 0.0f + b;  pVector->Y() = -10.0f+ b; pVector->Z() = 10.0f; pLineSet->SetPosition(12,pVector);
    pVector->X() = -10.0f + b;  pVector->Y() = -10.0f+ b; pVector->Z() = 10.0f; pLineSet->SetPosition(13,pVector);
    pVector->X() = -10.0f + b;  pVector->Y() = 0.0f+ b;   pVector->Z() = 10.0f; pLineSet->SetPosition(14,pVector);
    pVector->X() = -10.0f + b;  pVector->Y() = 10.0f+ b;  pVector->Z() = 10.0f; pLineSet->SetPosition(15,pVector);
    pVector->X() = 0.0f + b;  pVector->Y() = 10.0f+ b;  pVector->Z() = 10.0f; pLineSet->SetPosition(16,pVector);
    pVector->X() = 10.0f + b; pVector->Y() = 10.0f+ b;  pVector->Z() = 10.0f; pLineSet->SetPosition(17,pVector);


/*  NUMLINES == 12   positions  MATERIAL ( 6 items )
    0       0-1     0
    1       1-2     0
    2       2-3     1
    3       3-0     1
    4       3-4     2
    5       4-5     2
    6       5-0     3
    7       5-6     3
    8       6-7     4
    9       7-0     4
    10        7-8     5
    11        8-1     5
*/
    // 1.A - LINES
    IFXU32Line Line;
    Line.Set(0,1);  pLineSet->SetPositionLine(0, &Line);
    Line.Set(1,2);  pLineSet->SetPositionLine(1, &Line);
    Line.Set(2,3);  pLineSet->SetPositionLine(2, &Line);
    Line.Set(3,0);  pLineSet->SetPositionLine(3, &Line);
    Line.Set(3,4);  pLineSet->SetPositionLine(4, &Line);
    Line.Set(4,5);  pLineSet->SetPositionLine(5, &Line);
    Line.Set(5,0);  pLineSet->SetPositionLine(6, &Line);
    Line.Set(5,6);  pLineSet->SetPositionLine(7, &Line);
    Line.Set(6,7);  pLineSet->SetPositionLine(8, &Line);
    Line.Set(7,0);  pLineSet->SetPositionLine(9, &Line);
    Line.Set(7,8);  pLineSet->SetPositionLine(10, &Line);
    Line.Set(8,1);  pLineSet->SetPositionLine(11, &Line);

    int a = 9;
    Line.Set(0 + a,1 + a);  pLineSet->SetPositionLine(12, &Line);
    Line.Set(1 + a,2 + a);  pLineSet->SetPositionLine(13, &Line);
    Line.Set(2 + a,3 + a);  pLineSet->SetPositionLine(14, &Line);
    Line.Set(3 + a,0 + a);  pLineSet->SetPositionLine(15, &Line);
    Line.Set(3 + a,4 + a);  pLineSet->SetPositionLine(16, &Line);
    Line.Set(4 + a,5 + a);  pLineSet->SetPositionLine(17, &Line);
    Line.Set(5 + a,0 + a);  pLineSet->SetPositionLine(18, &Line);
    Line.Set(5 + a,6 + a);  pLineSet->SetPositionLine(19, &Line);
    Line.Set(6 + a,7 + a);  pLineSet->SetPositionLine(20, &Line);
    Line.Set(7 + a,0 + a);  pLineSet->SetPositionLine(21, &Line);
    Line.Set(7 + a,8 + a);  pLineSet->SetPositionLine(22, &Line);
    Line.Set(8 + a,1 + a);  pLineSet->SetPositionLine(23, &Line);


    Line.Set(4,13); pLineSet->SetPositionLine(24, &Line);
    Line.Set(2,11); pLineSet->SetPositionLine(25, &Line);
    Line.Set(8,17); pLineSet->SetPositionLine(26, &Line);
    Line.Set(6,15); pLineSet->SetPositionLine(27, &Line);


    Line.Set(3,12); pLineSet->SetPositionLine(28, &Line);
    Line.Set(7,16); pLineSet->SetPositionLine(29, &Line);
    Line.Set(5,14); pLineSet->SetPositionLine(30, &Line);
    Line.Set(1,10); pLineSet->SetPositionLine(31, &Line);
    Line.Set(0,9);  pLineSet->SetPositionLine(32, &Line);

    // 3 - DIFFUSE
	IFXVector4 diff_color( 0.0f, 0.0f, 0.0f, 1.0f );
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numDiffuseColors; i++) {
      pLineSet->SetDiffuseColor( i, &diff_color );
    }


    // 2 - NORMALS
    pVector->X() = 0.0f;  pVector->Y() = 0.5f;  pVector->Z() = 0.5f;  pLineSet->SetNormal(0,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.2f;  pVector->Z() = 0.8f;    pLineSet->SetNormal(1,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.01f; pVector->Z() = 0.99f; pLineSet->SetNormal(2,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pLineSet->SetNormal(3,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pLineSet->SetNormal(4,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pLineSet->SetNormal(5,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(6,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(7,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(8,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(9,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(10,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(11,pVector);


    //
    pVector->X() = 0.0f;  pVector->Y() = 0.5f;  pVector->Z() = 0.5f;  pLineSet->SetNormal(12,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.2f;  pVector->Z() = 0.8f;    pLineSet->SetNormal(13,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.01f; pVector->Z() = 0.99f; pLineSet->SetNormal(14,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pLineSet->SetNormal(15,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pLineSet->SetNormal(16,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pLineSet->SetNormal(17,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(18,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(19,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(20,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(21,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(22,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(23,pVector);

    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(24,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(25,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(26,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(27,pVector);

    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(28,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(29,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(30,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(31,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(32,pVector);


    //  NORMAL and DIFFUSE COLOR at LINE

    IFXU32Line LineC, LineN;
    int size = pLineSet->GetLineSetDesc()->m_numLines;
    for (int k=0; k < size; k++) {
      switch ( k )
      {
        case 0:
          {
          LineN.Set(0,0);
          LineC.Set(0,0);
          break;
          }
        case 1:
          {
          LineN.Set(1,1);
          LineC.Set(1,1);
          break;
          }
        case 2:
          {
          LineN.Set(2,2);
          LineC.Set(2,2);
          break;
          }

        case 3:
          {
          LineN.Set(3,3);
          LineC.Set(3,3);
          break;
          }

        default:
          {
          LineN.Set(3,0); // 3,3
          LineC.Set(2,0); // 0,0
          }
      }


      pLineSet->SetNormalLine(k,&LineN);
      pLineSet->SetDiffuseLine(k, &LineC);
    }

    // 4 - SPECULAR
	IFXVector4 spec_color( 0.0f, 1.0f, 0.0f, 0.0f );
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numSpecularColors; i++) {
      pLineSet->SetSpecularColor( i, &spec_color );
    }

    // SPECULAR LINE
    Line.Set(0,0);  // for simplicity
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numLines; i++) {
      pLineSet->SetSpecularLine(i, &Line);
    }

    // 5 - TEXTURECOORDS
	IFXVector4  buffText;
	IFXVector4* pVectorText = &buffText;
    F32 tcoord;
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numTexCoords; i++) {
      tcoord = (float)START_TEX_COORD;
	  pVectorText->Set(tcoord+i, tcoord+1+i, tcoord+2+i, tcoord+3+i);
	  pLineSet->SetTexCoord( i,pVectorText);
		  
    }

        // 6 - MATERIALS
    IFXAuthorMaterial material;
      material.m_uNumTextureLayers  = 0;
      material.m_uOriginalMaterialID  = 1;
      material.m_uDiffuseColors   = TRUE;
      material.m_uSpecularColors    = FALSE;
      material.m_uNormals       = TRUE;
    IFXAuthorMaterial material_1;
      material_1.m_uNumTextureLayers  = 0;
      material_1.m_uOriginalMaterialID = 2;
      material_1.m_uDiffuseColors   = FALSE;
      material_1.m_uSpecularColors  = TRUE;
      material_1.m_uNormals     = TRUE;

    for (i=0; i<material.m_uNumTextureLayers; i++){
      material.m_uTexCoordDimensions[i] = 0;
    }

    for (i=0; i < pLineSet->GetLineSetDesc()->m_numMaterials; i++) {

      if( i==0 )
        pLineSet->SetMaterial( i, &material );
      else
        pLineSet->SetMaterial( i, &material_1 );

    }

    // 7 - LINE MATERIALS
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numLines; i++) {

		pLineSet->SetLineMaterial(i,0);

    }
  return ir;
}// InitAuthorLineSet()



IFXRESULT InitGridAuthorLineSet(IFXAuthorLineSet* pLineSet)
{

#define NUMLINES_1			9
#define NUMPOS_1			18
#define NUMNORM_1			9
#define NUMDIFFCOLORS_1		9
#define NUMSPECCOLORS_1		9
#define NUMTEXCOORD_1		0
#define NUMMATERIAL_1		9

#define START_TEX_COORD  0.1f
#define SHIFT_TEX_COORD  0.13f

#define EPS 1.E-5

  IFXRESULT ir = IFX_OK;

  if (!pLineSet) return IFX_E_INVALID_POINTER;

  IFXAuthorLineSetDesc desc;
    desc.m_numLines			= NUMLINES_1;
    desc.m_numPositions		= NUMPOS_1;
    desc.m_numNormals			= NUMNORM_1;
    desc.m_numDiffuseColors	= NUMDIFFCOLORS_1;
    desc.m_numSpecularColors	= NUMSPECCOLORS_1;
    desc.m_numTexCoords		= NUMTEXCOORD_1;
    desc.m_numMaterials		= NUMMATERIAL_1;

    pLineSet->Allocate(&desc);
    pLineSet->SetLineSetDesc(&desc);
    U32 i;
    int num = 0;
    int numpos = pLineSet->GetMaxLineSetDesc()->m_numPositions;


    // 1 - POSITIONS
	IFXVector3  buff;
	IFXVector3*  pVector = &buff;	  

    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(0,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 5.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(1,pVector);
    
	pVector->X() = 1.0f;  pVector->Y() = 0.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(2,pVector);
	pVector->X() = 1.0f;  pVector->Y() = 5.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(3,pVector);

    pVector->X() = 2.0f;  pVector->Y() = 0.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(4,pVector);
    pVector->X() = 2.0f;  pVector->Y() = 5.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(5,pVector);

    pVector->X() = 3.0f;  pVector->Y() = 0.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(6,pVector);
    pVector->X() = 3.0f;  pVector->Y() = 5.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(7,pVector);
    
	
	pVector->X() = 4.0f; pVector->Y() = 0.0f; pVector->Z() = 0.0f; pLineSet->SetPosition(8,pVector);
    pVector->X() = 4.0f; pVector->Y() = 5.0f; pVector->Z() = 0.0f; pLineSet->SetPosition(9,pVector);

	pVector->X() = -1.0f; pVector->Y() = 1.0f; pVector->Z() = 0.0f;   pLineSet->SetPosition(10,pVector);
    pVector->X() =  5.0f; pVector->Y() = 1.0f; pVector->Z() = 0.0f;   pLineSet->SetPosition(11,pVector);

	pVector->X() = -1.0f; pVector->Y() = 2.0f; pVector->Z() = 0.0f;   pLineSet->SetPosition(12,pVector);
    pVector->X() =  5.0f; pVector->Y() = 2.0f; pVector->Z() = 0.0f;   pLineSet->SetPosition(13,pVector);

	pVector->X() = -1.0f; pVector->Y() = 3.0f; pVector->Z() = 0.0f;   pLineSet->SetPosition(14,pVector);
    pVector->X() =  5.0f; pVector->Y() = 3.0f; pVector->Z() = 0.0f;   pLineSet->SetPosition(15,pVector);

	pVector->X() = -1.0f; pVector->Y() = 4.0f; pVector->Z() = 0.0f;   pLineSet->SetPosition(16,pVector);
    pVector->X() =  5.0f; pVector->Y() = 4.0f; pVector->Z() = 0.0f;   pLineSet->SetPosition(17,pVector);


    // 1.A - LINES
    IFXU32Line Line;

	Line.Set(4,1);    pLineSet->SetPositionLine(0, &Line);
	Line.Set(4,3);    pLineSet->SetPositionLine(1, &Line);
	Line.Set(4,5);    pLineSet->SetPositionLine(2, &Line);
	Line.Set(4,7);    pLineSet->SetPositionLine(3, &Line);
	Line.Set(4,9);    pLineSet->SetPositionLine(4, &Line);    
	Line.Set(12,11);  pLineSet->SetPositionLine(5, &Line);
	Line.Set(12,13);  pLineSet->SetPositionLine(6, &Line);
	Line.Set(12,15);  pLineSet->SetPositionLine(7, &Line);
	Line.Set(12,17);  pLineSet->SetPositionLine(8, &Line);



    // 3 - DIFFUSE
	IFXVector4 diff_color( 0.0f, 0.0f, 1.0f, 1.0f );
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numDiffuseColors; i++) {
      pLineSet->SetDiffuseColor( i, &diff_color );
	  if( i < 3 )
		  diff_color[2-i] = 1.f;
    }

    // 2 - NORMALS
    pVector->X() = 0.0f;  pVector->Y() = 0.5f;  pVector->Z() = 0.5f;  pLineSet->SetNormal(0,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.2f;  pVector->Z() = 0.8f;  pLineSet->SetNormal(1,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.01f; pVector->Z() = 0.99f; pLineSet->SetNormal(2,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pLineSet->SetNormal(3,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pLineSet->SetNormal(4,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pLineSet->SetNormal(5,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(6,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(7,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(8,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(9,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(10,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pLineSet->SetNormal(11,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.5f;  pVector->Z() = 0.5f;  pLineSet->SetNormal(12,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.2f;  pVector->Z() = 0.8f;  pLineSet->SetNormal(13,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.01f; pVector->Z() = 0.99f; pLineSet->SetNormal(14,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pLineSet->SetNormal(15,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pLineSet->SetNormal(16,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pLineSet->SetNormal(17,pVector);
    

    //  NORMAL and DIFFUSE COLOR at LINE

    IFXU32Line LineC, LineN;
    int size = pLineSet->GetLineSetDesc()->m_numLines;
    for (int k=0; k < size; k++) {
      switch ( k )
      {
        case 0:
          {
          LineN.Set(0,0);
          LineC.Set(0,0);
          break;
          }
        case 1:
          {
          LineN.Set(1,1);
          LineC.Set(1,1);
          break;
          }
        case 2:
          {
          LineN.Set(2,2);
          LineC.Set(2,2);
          break;
          }

        case 3:
          {
          LineN.Set(3,2);
          LineC.Set(3,2);
          break;
          }
        case 4:
          {
          LineN.Set(1,2);
          LineC.Set(1,2);
          break;
          }
        case 5:
          {
           LineN.Set(0,0);
          LineC.Set(0,0);
          break;
          }
        case 6:
          {
          LineN.Set(1,1);
          LineC.Set(1,1);
          break;
          }
        case 7:
          {
          LineN.Set(2,2);
          LineC.Set(2,2);
          break;
          }
		case 8:
          {
          LineN.Set(3,2);
          LineC.Set(3,2);
          break;
          }
        default:
          {
          LineN.Set(3,0); 
          LineC.Set(2,0); 
          }
      }
      pLineSet->SetNormalLine(k,&LineN);
      pLineSet->SetDiffuseLine(k, &LineC);
    }

    // 4 - SPECULAR
	IFXVector4 spec_color( 0.0f, 1.0f, 0.0f, 1.0f );
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numSpecularColors; i++) {
      pLineSet->SetSpecularColor( i, &spec_color );
    }

    // SPECULAR LINE
    Line.Set(0,0);  // for simplicity
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numLines; i++) {
      pLineSet->SetSpecularLine(i, &Line);
    }

    // 5 - TEXTURECOORDS
    F32 tcoord;
	IFXVector4  buffText;
	IFXVector4* pVectorText = &buffText;
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numTexCoords; i++) {
      tcoord = (float)START_TEX_COORD;
	  pVectorText->Set(tcoord+i, tcoord+1+i, tcoord+2+i, tcoord+3+i);
	  pLineSet->SetTexCoord( i, pVectorText);
    }

        // 6 - MATERIALS
    IFXAuthorMaterial material_0;
      material_0.m_uNumTextureLayers	= 0;
      material_0.m_uOriginalMaterialID	= 0;
      material_0.m_uDiffuseColors		= TRUE;
      material_0.m_uSpecularColors		= FALSE;
      material_0.m_uNormals				= TRUE;

    IFXAuthorMaterial material_1;
      material_1.m_uNumTextureLayers	= 0;
      material_1.m_uOriginalMaterialID	= 1;
      material_1.m_uDiffuseColors		= FALSE;
      material_1.m_uSpecularColors		= TRUE;
      material_1.m_uNormals				= TRUE;

 IFXAuthorMaterial material_2;
      material_2.m_uNumTextureLayers	= 0;
      material_2.m_uOriginalMaterialID	= 2;
      material_2.m_uDiffuseColors		= FALSE;
      material_2.m_uSpecularColors		= TRUE;
      material_2.m_uNormals				= TRUE;

 IFXAuthorMaterial material_3;
      material_3.m_uNumTextureLayers	= 0;
      material_3.m_uOriginalMaterialID	= 3;
      material_3.m_uDiffuseColors		= FALSE;
      material_3.m_uSpecularColors		= TRUE;
      material_3.m_uNormals				= TRUE;

 IFXAuthorMaterial material_4;
      material_4.m_uNumTextureLayers	= 0;
      material_4.m_uOriginalMaterialID	= 4;
      material_4.m_uDiffuseColors		= TRUE;
      material_4.m_uSpecularColors		= FALSE;
      material_4.m_uNormals				= TRUE;


 IFXAuthorMaterial material_5;
      material_5.m_uNumTextureLayers	= 0;
      material_5.m_uOriginalMaterialID	= 5;
      material_5.m_uDiffuseColors		= TRUE;
      material_5.m_uSpecularColors		= TRUE;
      material_5.m_uNormals				= TRUE;

 IFXAuthorMaterial material_6;
      material_6.m_uNumTextureLayers	= 0;
      material_6.m_uOriginalMaterialID	= 6;
      material_6.m_uDiffuseColors		= TRUE;
      material_6.m_uSpecularColors		= FALSE;
      material_6.m_uNormals				= TRUE;
 
IFXAuthorMaterial material_7;
      material_7.m_uNumTextureLayers	= 0;
      material_7.m_uOriginalMaterialID	= 7;
      material_7.m_uDiffuseColors		= TRUE;
      material_7.m_uSpecularColors		= TRUE;
      material_7.m_uNormals				= TRUE;

 IFXAuthorMaterial material_8;
      material_8.m_uNumTextureLayers	= 0;
      material_8.m_uOriginalMaterialID	= 8;
      material_8.m_uDiffuseColors		= TRUE;
      material_8.m_uSpecularColors		= FALSE;
      material_8.m_uNormals				= TRUE;

    for (i=0; i<material_0.m_uNumTextureLayers; i++){
      material_0.m_uTexCoordDimensions[i] = 0;
    }

    for (i=0; i < pLineSet->GetLineSetDesc()->m_numMaterials; i++) {

      if( i==0 )
        pLineSet->SetMaterial( i, &material_0 );
      
	  if( i==1 )
        pLineSet->SetMaterial( i, &material_1 );

	  if( i==2 )
        pLineSet->SetMaterial( i, &material_2 );
      
	  if( i==3 )
        pLineSet->SetMaterial( i, &material_3 );
      
	  if( i==4 )
        pLineSet->SetMaterial( i, &material_4 );
      
	  if( i==5 )
        pLineSet->SetMaterial( i, &material_5 );
      
	  if( i==6 )
        pLineSet->SetMaterial( i, &material_6 );
      
	  if( i==7 )
        pLineSet->SetMaterial( i, &material_7 );

      if( i==8 )
        pLineSet->SetMaterial( i, &material_8 );
    }// for

    // 7 - LINE MATERIALS
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numLines; i++) {
      
		pLineSet->SetLineMaterial(i,i);

    }
  return ir;
}// InitGridAuthorLineSet()

IFXRESULT InitTextureAuthorLineSet(IFXAuthorLineSet* pLineSet)
{

#define NUMLINES_TEXT      1
#define NUMPOS_TEXT        2
#define NUMNORM_TEXT         2
#define NUMDIFFCOLORS_TEXT   1
#define NUMSPECCOLORS_TEXT   1
#define NUMTEXCOORD_TEXT     1
#define NUMMATERIAL_TEXT     1

#define START_TEX_COORD  0.1f
#define SHIFT_TEX_COORD  0.13f

#define EPS 1.E-5

  IFXRESULT ir = IFX_OK;

  if (!pLineSet) return IFX_E_INVALID_POINTER;

  IFXAuthorLineSetDesc desc;
    desc.m_numLines			= NUMLINES_TEXT;
    desc.m_numPositions		= NUMPOS_TEXT;
    desc.m_numNormals			= NUMNORM_TEXT;
    desc.m_numDiffuseColors	= NUMDIFFCOLORS_TEXT;
    desc.m_numSpecularColors	= NUMSPECCOLORS_TEXT;
    desc.m_numTexCoords		= NUMTEXCOORD_TEXT;
    desc.m_numMaterials		= NUMMATERIAL_TEXT;

    pLineSet->Allocate(&desc);
    pLineSet->SetLineSetDesc(&desc);
    U32 i;
    int num = 0;
    int numpos = pLineSet->GetMaxLineSetDesc()->m_numPositions;


    // 1 - POSITIONS
	IFXVector3 buff;
    IFXVector3* pVector = &buff;
    pVector->X() = -10.0f;  pVector->Y() = 0.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(0,pVector);
    pVector->X() = 10.0f; pVector->Y() = 0.0f;  pVector->Z() = 0.0f;  pLineSet->SetPosition(1,pVector);

    // 1.A - LINES
    IFXU32Line Line;
    Line.Set(0,1);  pLineSet->SetPositionLine(0, &Line);

    // 2 - NORMALS
    pVector->X() = 0.0f;  pVector->Y() = 0.5f;  pVector->Z() = 0.5f;  pLineSet->SetNormal(0,pVector);
    pVector->X() = 0.0f;  pVector->Y() = 0.2f;  pVector->Z() = 0.8f;  pLineSet->SetNormal(1,pVector);


    // 3 - DIFFUSE
	IFXVector4 diff_color( 0.0f, 0.0f, 0.0f, 1.0f );
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numDiffuseColors; i++) {
      pLineSet->SetDiffuseColor( i, &diff_color );
	  if( i < 3 )
		  diff_color[2-i] = 1.f;
    }

    //  NORMAL and DIFFUSE COLOR at LINE
    IFXU32Line LineC, LineN, LineD;
    int size = pLineSet->GetLineSetDesc()->m_numLines;
    for (int k=0; k < size; k++) {
      
	    LineN.Set(1,0); // 3,3
        LineC.Set(1,1); // 0,0
		LineD.Set(1,1); // 0,0

		pLineSet->SetNormalLine(k,&LineN);
		pLineSet->SetDiffuseLine(k, &LineC);
		pLineSet->SetDiffuseLine(k, &LineD);

    }

    // 4 - SPECULAR
	IFXVector4 spec_color( 0.0f, 1.0f, 0.0f, 0.0f );
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numSpecularColors; i++) {
      pLineSet->SetSpecularColor( i, &spec_color );
    }

    // SPECULAR LINE
    Line.Set(0,0);  // for simplicity
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numLines; i++) {
      pLineSet->SetSpecularLine(i, &Line);
    }

    // 5 - TEXTURECOORDS 
	
			F32 tcoord;
			F32 t0,t1,t2,t3;
			t0 = t1 = t2 = t3 = 0.0f;
			IFXVector4 buffText;
			IFXVector4* pVectorText = &buffText;

			for (i=0; i < pLineSet->GetLineSetDesc()->m_numTexCoords; i++) {
			tcoord = (float)START_TEX_COORD;
				t0 = t0 + tcoord+i;
				t1 = t1 + tcoord+1+i;
				t2 = t2 + tcoord+2+i;
				t3 = t3 + tcoord+3+i;
				pVectorText->Set( t0, t1, t2, t3 );
				pLineSet->SetTexCoord( i, pVectorText);
			}

			IFXU32Line LineTexture;
			LineTexture.Set(0,1);  pLineSet->SetTexLine(0,0, &LineTexture);
			LineTexture.Set(1,0);  pLineSet->SetTexLine(1,1, &LineTexture);
			LineTexture.Set(1,0);  pLineSet->SetTexLine(2,1, &LineTexture);
			LineTexture.Set(1,0);  pLineSet->SetTexLine(3,1, &LineTexture);

			//IFXRESULT SetTexLine(U32 in_Layer, U32 in_Idx, const IFXU32Line* in_pLine);

        // 6 - MATERIALS
    IFXAuthorMaterial material;
      material.m_uNumTextureLayers  = 0;
      material.m_uOriginalMaterialID  = 1;
      material.m_uDiffuseColors		= TRUE;
      material.m_uSpecularColors    = TRUE;
      material.m_uNormals			= TRUE;

    for (i=0; i<material.m_uNumTextureLayers; i++){
      material.m_uTexCoordDimensions[i] = 0;
    }

    for (i=0; i < pLineSet->GetLineSetDesc()->m_numMaterials; i++) {

        pLineSet->SetMaterial( i, &material );
    }

    // 7 - LINE MATERIALS
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numLines; i++) 
	{
		pLineSet->SetLineMaterial(i,0);
    }

  return ir;

}//InitTextureAuthorLineSet



IFXRESULT InitGrid256AuthorLineSet(IFXAuthorLineSet* pLineSet)
{

#define GRIDLINES		16
#define NUMLINES_2      (GRIDLINES)*(GRIDLINES-1)*2 
#define NUMPOS_2		(GRIDLINES)*(GRIDLINES)
#define NUMNORM_2         2
#define NUMDIFFCOLORS_2   2
#define NUMSPECCOLORS_2   0
#define NUMTEXCOORD_2     0
#define NUMMATERIAL_2     2


#define EPS 1.E-5

  IFXRESULT ir = IFX_OK;

  if (!pLineSet) return IFX_E_INVALID_POINTER;

  IFXAuthorLineSetDesc desc;
    desc.m_numLines			= NUMLINES_2;
    desc.m_numPositions		= NUMPOS_2;
    desc.m_numNormals			= NUMNORM_2;
    desc.m_numDiffuseColors	= NUMDIFFCOLORS_2;
    desc.m_numSpecularColors	= NUMSPECCOLORS_2;
    desc.m_numTexCoords		= NUMTEXCOORD_2;
    desc.m_numMaterials		= NUMMATERIAL_2;

    pLineSet->Allocate(&desc);
    pLineSet->SetLineSetDesc(&desc);
    U32 iPos, kPos, iLine, iP0, iP1, i;
    int num = 0;
    int numpos = pLineSet->GetMaxLineSetDesc()->m_numPositions;


    // 1 - POSITIONS
    IFXVector3  buff(0.f, 0.f, 0.f);
	for (iPos = 0; iPos < NUMPOS_2; iPos++ ) {
		pLineSet->SetPosition(iPos, &buff);
		if ( iPos!=0 && ((iPos+1)%GRIDLINES) == 0)  {
			buff.X() = 0.f;
			buff.Y() = buff.Y() + 1.f;
		} else {
			buff.X() = buff.X() + 1.f;
		}
	}

    // 2 - LINES
    IFXU32Line Line, LineDiff;
	iLine = 0;

	iP0 = 0; iP1=0;
	for (iPos = 0; iPos < GRIDLINES; iPos++ ) {
		for (kPos = 0; kPos < GRIDLINES-1; kPos++ ) {
			iP1 = iP0 + 1;
			Line.Set(iP0, iP1);
			pLineSet->SetPositionLine(iLine, &Line);
			iLine++;
			iP0++;
		}
		iP0++;
	}
	IFXASSERT(iLine == ( (NUMLINES_2)/2 ));

	iP0 = 0; iP1=0;
	for (kPos = 0; kPos < GRIDLINES; kPos++ ) {
		iP0 = kPos;
		for (iPos = 0; iPos < GRIDLINES-1; iPos++ ) {
			iP1 = iP0 + GRIDLINES;
			Line.Set(iP0, iP1);
			pLineSet->SetPositionLine(iLine, &Line);
			iLine++;
			iP0 = iP0 + GRIDLINES;
		}
	}

    // 3 - DIFFUSE
	IFXVector4 diff_color( 0.0f, 0.0f, 1.0f, 1.0f );
    for (i=0; i < pLineSet->GetLineSetDesc()->m_numDiffuseColors; i++) {
      pLineSet->SetDiffuseColor( i, &diff_color );
    }

    // 2 - NORMALS
	buff.X() = 0.f; buff.Y() = 0.5f; buff.Z() = 0.5f; 
	for (i=0; i < pLineSet->GetLineSetDesc()->m_numNormals; i++) {
      pLineSet->SetNormal( i, &buff);
	  if(i == ((pLineSet->GetLineSetDesc()->m_numNormals)/2)-1) {
		  buff.Y() = 0.2f; buff.Z() = 0.8f; 
	  }
    }

    //  NORMAL and DIFFUSE COLOR at LINE
	Line.Set(0, 0);
	LineDiff.Set(0, 0);
	for (iLine = 0; iLine<pLineSet->GetLineSetDesc()->m_numLines; iLine++) {
      pLineSet->SetNormalLine(iLine, &Line);
      pLineSet->SetDiffuseLine(iLine, &LineDiff);
	  if(iLine == ((pLineSet->GetLineSetDesc()->m_numLines)/2)-1) {
		  Line.Set(1, 1);
		  LineDiff.Set(1, 1);
	  }
	}


        // 6 - MATERIALS
    IFXAuthorMaterial material_0;
      material_0.m_uNumTextureLayers	= 0;
      material_0.m_uOriginalMaterialID	= 0;
      material_0.m_uDiffuseColors		= TRUE;
      material_0.m_uSpecularColors		= FALSE;
      material_0.m_uNormals				= TRUE;

    IFXAuthorMaterial material_1;
      material_1.m_uNumTextureLayers	= 0;
      material_1.m_uOriginalMaterialID	= 1;
      material_1.m_uDiffuseColors		= FALSE;
      material_1.m_uSpecularColors		= FALSE;
      material_1.m_uNormals				= TRUE;

    for (i=0; i < pLineSet->GetLineSetDesc()->m_numMaterials; i++) {

      if( i==0 )
        pLineSet->SetMaterial( i, &material_0 );
      
	  if( i==1 )
        pLineSet->SetMaterial( i, &material_1 );

    }// for

    // 7 - LINE MATERIALS
    for (iLine = 0; iLine < pLineSet->GetLineSetDesc()->m_numLines; iLine++) {
      
		pLineSet->SetLineMaterial(iLine, 0);

    }
  
	return ir;

}// InitGrid256AuthorLineSet()




