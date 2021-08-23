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

#include "PointSetInitialize.h"

IFXRESULT InitAuthorPointSet(IFXAuthorPointSet* pPointSet)
{

#define NUMPOINTS       18
#define NUMPOS          18
#define NUMNORM         18
#define NUMDIFFCOLORS   5
#define NUMSPECCOLORS   5
#define NUMTEXCOORD     0
#define NUMMATERIAL     2

#define START_TEX_COORD  0.1f
#define SHIFT_TEX_COORD  0.13f

#define EPS 1.E-5

  IFXRESULT ir = IFX_OK;

  if (!pPointSet) return IFX_E_INVALID_POINTER;

  IFXAuthorPointSetDesc desc;
  
    desc.m_numPoints     = NUMPOINTS;
    desc.m_numPositions    = NUMPOS;
    desc.m_numNormals        = NUMNORM;
    desc.m_numDiffuseColors  = NUMDIFFCOLORS;
    desc.m_numSpecularColors = NUMSPECCOLORS;
    desc.m_numTexCoords    = NUMTEXCOORD;
    desc.m_numMaterials    = NUMMATERIAL;


    pPointSet->Allocate(&desc);	
    pPointSet->SetPointSetDesc(&desc);
    U32 i;
    int num = 0;
    int numpos = pPointSet->GetMaxPointSetDesc()->m_numPositions;


    // 1 - POSITIONS
    // fill Points as square polyPoints
	IFXVector3	buff;
    IFXVector3* pVector = &buff;
	float a = 2.0f;
	pVector->X() = 0.0f + a;  pVector->Y() = 0.0f + a;  pVector->Z() = a;  pPointSet->SetPosition(0,pVector);
    pVector->X() = 0.0f + a;  pVector->Y() = 0.0f + a;  pVector->Z() = a;  pPointSet->SetPosition(0,pVector);
    pVector->X() = 10.0f + a; pVector->Y() = 0.0f + a;  pVector->Z() = a;    pPointSet->SetPosition(1,pVector);
    pVector->X() = 10.0f + a; pVector->Y() = -10.0f + a;  pVector->Z() = a;  pPointSet->SetPosition(2,pVector);
    pVector->X() = 0.0f + a;  pVector->Y() = -10.0f + a;  pVector->Z() = a;  pPointSet->SetPosition(3,pVector);
    pVector->X() = -10.0f + a;  pVector->Y() = -10.0f + a;  pVector->Z() = a ;  pPointSet->SetPosition(4,pVector);
    pVector->X() = -10.0f + a;  pVector->Y() = 0.0f + a;  pVector->Z() =  a;  pPointSet->SetPosition(5,pVector);
    pVector->X() = -10.0f + a;  pVector->Y() = 10.0f + a; pVector->Z() = a;  pPointSet->SetPosition(6,pVector);
    pVector->X() = 0.0f + a;  pVector->Y() = 10.0f + a; pVector->Z() = a;  pPointSet->SetPosition(7,pVector);
    pVector->X() = 10.0f + a; pVector->Y() = 10.0f + a; pVector->Z() = a;  pPointSet->SetPosition(8,pVector);
    float b = 13.3f;
    pVector->X() = 0.0f + b ; pVector->Y() = 0.0f+ b;   pVector->Z() = 10.0f + a;   pPointSet->SetPosition(9,pVector);
    pVector->X() = 10.0f + b; pVector->Y() = 0.0f+ b;   pVector->Z() = 10.0f + a;   pPointSet->SetPosition(10,pVector);
    pVector->X() = 10.0f + b; pVector->Y() = -10.0f+ b; pVector->Z() = 10.0f + a; pPointSet->SetPosition(11,pVector);
    pVector->X() = 0.0f + b;  pVector->Y() = -10.0f+ b; pVector->Z() = 10.0f + a; pPointSet->SetPosition(12,pVector);
    pVector->X() = -10.0f + b;  pVector->Y() = -10.0f+ b; pVector->Z() = 10.0f + a; pPointSet->SetPosition(13,pVector);
    pVector->X() = -10.0f + b;  pVector->Y() = 0.0f+ b;   pVector->Z() = 10.0f + a; pPointSet->SetPosition(14,pVector);
    pVector->X() = -10.0f + b;  pVector->Y() = 10.0f+ b;  pVector->Z() = 10.0f + a; pPointSet->SetPosition(15,pVector);
    pVector->X() = 0.0f + b;  pVector->Y() = 10.0f+ b;  pVector->Z() = 10.0f + a; pPointSet->SetPosition(16,pVector);
    pVector->X() = 10.0f + b; pVector->Y() = 10.0f+ b;  pVector->Z() = 10.0f + a; pPointSet->SetPosition(17,pVector);

    // 1.A - points
    U32 point;
    point  = 0; pPointSet->SetPositionPoint(0, &point);
    point  = 1; pPointSet->SetPositionPoint(1, &point);
    point  = 2; pPointSet->SetPositionPoint(2, &point);
    point  = 3; pPointSet->SetPositionPoint(3, &point);
    point  = 4; pPointSet->SetPositionPoint(4, &point);
    point  = 5; pPointSet->SetPositionPoint(5, &point);
    point  = 6; pPointSet->SetPositionPoint(6, &point);
    point  = 7; pPointSet->SetPositionPoint(7, &point);
    point  = 8; pPointSet->SetPositionPoint(8, &point);
    point  = 9; pPointSet->SetPositionPoint(9, &point);
    point  = 10; pPointSet->SetPositionPoint(10, &point);
    point  = 11; pPointSet->SetPositionPoint(11, &point);
    point  = 12; pPointSet->SetPositionPoint(12, &point);
    point  = 13; pPointSet->SetPositionPoint(13, &point);
    point  = 14; pPointSet->SetPositionPoint(14, &point);
    point  = 15; pPointSet->SetPositionPoint(15, &point);
    point  = 16; pPointSet->SetPositionPoint(16, &point);
    point  = 17; pPointSet->SetPositionPoint(17, &point);

     // 2 - NORMALS
	pVector->X() = 0.0f;  pVector->Y() = 0.5f;  pVector->Z() = 0.5f;  pPointSet->SetNormal(0,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.2f;  pVector->Z() = 0.8f;  pPointSet->SetNormal(1,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.01f; pVector->Z() = 0.99f; pPointSet->SetNormal(2,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pPointSet->SetNormal(3,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pPointSet->SetNormal(4,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pPointSet->SetNormal(5,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pPointSet->SetNormal(6,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pPointSet->SetNormal(7,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pPointSet->SetNormal(8,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pPointSet->SetNormal(9,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pPointSet->SetNormal(10,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = -1.0f; pPointSet->SetNormal(11,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.5f;  pVector->Z() = 0.5f;  pPointSet->SetNormal(12,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.2f;  pVector->Z() = 0.8f;  pPointSet->SetNormal(13,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.01f; pVector->Z() = 0.99f; pPointSet->SetNormal(14,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pPointSet->SetNormal(15,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pPointSet->SetNormal(16,pVector);
	pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  pPointSet->SetNormal(17,pVector);

    U32 p = 1;
    for (i=0; i< pPointSet->GetPointSetDesc()->m_numPoints; i++) {
      pPointSet->SetNormalPoint(i,&p);
    }
    	
    // 3 - DIFFUSE
	IFXVector4 diff_color( 0.0f, 0.0f, 0.0f, 1.0f );
    for (i=0; i < pPointSet->GetPointSetDesc()->m_numDiffuseColors; i++) {
      pPointSet->SetDiffuseColor( i, &diff_color );
    }
    //  DIFFUSE POINTS
    p = 0;  // for simplicity
    for (i=0; i < pPointSet->GetPointSetDesc()->m_numPoints; i++) {
      pPointSet->SetDiffusePoint(i, &p);
    }

    // 4 - SPECULAR
	IFXVector4 spec_color( 0.0f, 1.0f, 0.0f, 0.0f );
    for (i=0; i < pPointSet->GetPointSetDesc()->m_numSpecularColors; i++) {
      pPointSet->SetSpecularColor( i, &spec_color );
    }

    //  SPECULAR LINE
    p = 0;  // for simplicity
    for (i=0; i < pPointSet->GetPointSetDesc()->m_numPoints; i++) {
      pPointSet->SetSpecularPoint(i, &p);
    }

    // 5 - TEXTURECOORDS
    F32 tcoord;
    for (i=0; i < pPointSet->GetPointSetDesc()->m_numTexCoords; i++) {
      tcoord = (float)START_TEX_COORD;
      pPointSet->SetTexCoord( i, new IFXVector4( tcoord+i, tcoord+1+i, tcoord+2+i, tcoord+3+i ) );
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

    for (i=0; i < pPointSet->GetPointSetDesc()->m_numMaterials; i++) {

      if( i==0 )
        pPointSet->SetMaterial( i, &material );
      else
        pPointSet->SetMaterial( i, &material_1 );

    }

    // 7 - Point MATERIALS
	for (i=0; i < pPointSet->GetPointSetDesc()->m_numPoints; i++) {

		pPointSet->SetPointMaterial(i,0);

    }

  return ir;
}// InitAuthorPointSet()

IFXRESULT InitGrid256AuthorPointSet(IFXAuthorPointSet* pPointSet)
{

#define GRIDPOINTS		16
#define NUMPOINTS_2     (GRIDPOINTS)*(GRIDPOINTS) 
#define NUMPOS_2		(GRIDPOINTS)*(GRIDPOINTS)
#define NUMNORM_2       (GRIDPOINTS)*(GRIDPOINTS)
#define NUMDIFFCOLORS_2   2
#define NUMSPECCOLORS_2   2
#define NUMTEXCOORD_2     0
#define NUMMATERIAL_2     2


#define EPS 1.E-5

  IFXRESULT ir = IFX_OK;

	if (!pPointSet) return IFX_E_INVALID_POINTER;

	IFXAuthorPointSetDesc desc;
    desc.m_numPoints			= NUMPOINTS_2;
    desc.m_numPositions		= NUMPOS_2;
    desc.m_numNormals			= NUMNORM_2;
    desc.m_numDiffuseColors	= NUMDIFFCOLORS_2;
    desc.m_numSpecularColors	= NUMSPECCOLORS_2;
    desc.m_numTexCoords		= NUMTEXCOORD_2;
    desc.m_numMaterials		= NUMMATERIAL_2;

	ir = pPointSet->Allocate(&desc);
	if (IFXSUCCESS(ir)){
		ir = pPointSet->SetPointSetDesc(&desc);
	}
	
    U32 iPos, iPoint, i;
    int num = 0;
    int numpos = pPointSet->GetMaxPointSetDesc()->m_numPositions;
	IFXASSERT(numpos > 0 );
	
	// 1 - POSITIONS
    IFXVector3  buff(0.f, 0.f, 0.f);
	for (iPos = 0; iPos < NUMPOS_2; iPos++ ) {
		ir = pPointSet->SetPosition(iPos, &buff);
		if (IFXSUCCESS(ir)){
			if ( iPos!=0 && ((iPos+1)%GRIDPOINTS) == 0)  {
				buff.X() = 0.f;
				buff.Y() = buff.Y() + 1.f;
			} else {
				buff.X() = buff.X() + 1.f;
			}
		}
	}

    // 2 - PointS
	int ii;
	U32 point;
	for (ii = 0; ii < NUMPOINTS_2; ii ++ ) {
		point = ii;
		if(IFXSUCCESS(ir))
			ir = pPointSet->SetPositionPoint(ii, &point );
	}


	// 2 - NORMALS
    IFXVector3* pVector = &buff;
	pVector->X() = 1.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;  
	for(i=0; i < pPointSet->GetPointSetDesc()->m_numNormals; i++) {
		if(IFXSUCCESS(ir))
			ir = pPointSet->SetNormal(i,pVector);				
	}

	U32 p = 1;
    for (i=0; i< pPointSet->GetPointSetDesc()->m_numNormals; i++) {
		if(IFXSUCCESS(ir))
			ir = pPointSet->SetNormalPoint(i,&p);
    }

    // 3 - DIFFUSE
	IFXVector4 diff_color( 0.0f, 0.0f, 0.0f, 1.0f );
    for (i=0; i < pPointSet->GetPointSetDesc()->m_numDiffuseColors; i++) {
      if(IFXSUCCESS(ir))
			ir = pPointSet->SetDiffuseColor( i, &diff_color );
    }
   
	p = 0;  
    for (i=0; i < pPointSet->GetPointSetDesc()->m_numPoints; i++) {
		if(IFXSUCCESS(ir))
			ir = pPointSet->SetDiffusePoint(i, &p);
    }


	// 4 - SPECULAR
	IFXVector4 spec_color( 0.0f, 1.0f, 0.0f, 0.0f );
    for (i=0; i < pPointSet->GetPointSetDesc()->m_numSpecularColors; i++) {
      pPointSet->SetSpecularColor( i, &spec_color );
    }

    p = 0;  
	for (i=0; i < pPointSet->GetPointSetDesc()->m_numPoints; i++) {
		if(IFXSUCCESS(ir))
			ir = pPointSet->SetSpecularPoint(i, &p);
    }

    // 5 - TEXTURECOORDS
	
    // 6 - MATERIALS
    IFXAuthorMaterial material_0;
      material_0.m_uNumTextureLayers	= 0;
      material_0.m_uOriginalMaterialID	= 0;
      material_0.m_uDiffuseColors		= TRUE;
      material_0.m_uSpecularColors		= FALSE;
      material_0.m_uNormals				= TRUE;

    for (i=0; i < pPointSet->GetPointSetDesc()->m_numMaterials; i++) {

		if( 0 == i ){
			if(IFXSUCCESS(ir))
				ir = pPointSet->SetMaterial( i, &material_0 );
		}
      
	}// for

    // 7 - Point MATERIALS
    for (iPoint = 0; iPoint < pPointSet->GetPointSetDesc()->m_numPoints; iPoint++) {
		if(IFXSUCCESS(ir))
			ir = pPointSet->SetPointMaterial(iPoint, 0);
    }

	return ir;
}// InitGrid256AuthorPointSet()



