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


#include "ClodInitialize.h"

#define _NUMFACES     1
#define _NUMPOS       3
#define _NUMNORM      3
#define _NUMDIFFCOLORS    3
#define _NUMSPECCOLORS    3
#define _NUMTEXCOORD    0
#define _NUMMATERIAL    1
#define _NUMBASEVERTICES  3

#define START_TEX_COORD  0.1f
#define SHIFT_TEX_COORD  0.13f
#define EPS 1.E-5

IFXRESULT InitAuthorClod(IFXAuthorCLODMesh* pClodMesh)
{

	IFXRESULT ir = IFX_OK;

	if (!pClodMesh) return IFX_E_INVALID_POINTER;

	IFXAuthorMeshDesc		desc;
	desc.NumBaseVertices	= 32+9;
	desc.NumPositions		= 32+9;
	desc.NumNormals		= 1;
	desc.NumFaces			= 26+8+2;
	desc.NumSpecularColors = 26+8+2;
	desc.NumTexCoords		= 0;
	desc.NumMaterials		= 9;
	desc.NumDiffuseColors	= 36*3;//26+8+2;
	pClodMesh->Allocate(&desc);
	pClodMesh->SetMeshDesc(&desc);
	U32 i;
	int num = 0;
	int numpos = pClodMesh->GetMaxMeshDesc()->NumPositions;


	// 1 - POSITIONS
	IFXVector3  buff;
	IFXVector3* pVector = &buff;
	pVector->X() = 7.0f;  pVector->Y() = 16.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(1,pVector);
	pVector->X() = 10.0f; pVector->Y() = 9.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(2,pVector);
	pVector->X() = 1.0f;  pVector->Y() = 11.0f;  pVector->Z() = 0.0f;  pClodMesh->SetPosition(3,pVector);

	pVector->X() = 8.0f;  pVector->Y() = 11.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(4,pVector);
	pVector->X() = 6.0f; pVector->Y() = 14.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(5,pVector);
	pVector->X() = 4.0f;  pVector->Y() = 12.0f;  pVector->Z() = 0.0f;  pClodMesh->SetPosition(6,pVector);

	pVector->X() = 8.0f;  pVector->Y() = 9.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(7,pVector);
	pVector->X() = 8.0f; pVector->Y() = 6.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(8,pVector);
	pVector->X() = 5.0f;  pVector->Y() = 3.0f;  pVector->Z() = 0.0f;  pClodMesh->SetPosition(9,pVector);

	pVector->X() = 1.0f;  pVector->Y() = 5.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(10,pVector);
	pVector->X() = 6.0f; pVector->Y() = 2.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(11,pVector);
	pVector->X() = 6.0f;  pVector->Y() = 0.0f;  pVector->Z() = 0.0f;  pClodMesh->SetPosition(12,pVector);

	pVector->X() = 2.0f;  pVector->Y() = 1.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(13,pVector);

	pVector->X() = 8.0f; pVector->Y() = 11.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(14,pVector);
	pVector->X() = 6.0f;  pVector->Y() = 14.0f;  pVector->Z() = 0.0f;  pClodMesh->SetPosition(15,pVector);
	pVector->X() = 4.0f; pVector->Y() = 12.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(0,pVector);




	pVector->X() = -7.0f;  pVector->Y() = 16.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(17,pVector);
	pVector->X() = -10.0f; pVector->Y() = 9.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(18,pVector);
	pVector->X() = -1.0f;  pVector->Y() = 11.0f;  pVector->Z() = 0.0f;  pClodMesh->SetPosition(19,pVector);

	pVector->X() = -8.0f;  pVector->Y() = 11.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(20,pVector);
	pVector->X() = -6.0f; pVector->Y() = 14.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(21,pVector);
	pVector->X() = -4.0f;  pVector->Y() = 12.0f;  pVector->Z() = 0.0f;  pClodMesh->SetPosition(22,pVector);

	pVector->X() = -8.0f;  pVector->Y() = 9.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(23,pVector);
	pVector->X() = -8.0f;  pVector->Y() = 6.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(24,pVector);
	pVector->X() = -5.0f;  pVector->Y() = 3.0f;  pVector->Z() = 0.0f;  pClodMesh->SetPosition(25,pVector);

	pVector->X() = -1.0f;  pVector->Y() = 5.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(26,pVector);
	pVector->X() = -6.0f; pVector->Y() = 2.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(27,pVector);
	pVector->X() = -6.0f;  pVector->Y() = 0.0f;  pVector->Z() = 0.0f;  pClodMesh->SetPosition(28,pVector);

	pVector->X() = -2.0f;  pVector->Y() = 1.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(29,pVector);

	pVector->X() = -8.0f; pVector->Y() = 11.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(30,pVector);
	pVector->X() = -6.0f;  pVector->Y() = 14.0f;  pVector->Z() = 0.0f;  pClodMesh->SetPosition(31,pVector);
	pVector->X() = -4.0f; pVector->Y() = 12.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(16,pVector);


	pVector->X() = 0.0f; pVector->Y() = 13.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(32,pVector);
	pVector->X() = 0.0f; pVector->Y() = 6.5f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(33,pVector);
	pVector->X() = 0.0f; pVector->Y() = 2.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(34,pVector);

	pVector->X() = 1.0f; pVector->Y() = 14.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(35,pVector);
	pVector->X() = 3.0f; pVector->Y() = 17.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(36,pVector);

	pVector->X() = -1.0f; pVector->Y() = 14.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(37,pVector);
	pVector->X() = -3.0f; pVector->Y() = 17.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(38,pVector);

	pVector->X() = 1.1f; pVector->Y() = 14.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(39,pVector);
	pVector->X() = -1.1f; pVector->Y() = 14.0f;   pVector->Z() = 0.0f;  pClodMesh->SetPosition(40,pVector);


	// 1.A - TRIANGLES(FACES)
	IFXAuthorFace Face;
	Face.Set(1,4,2); pClodMesh->SetPositionFace(0,&Face);
	Face.Set(1,4,5); pClodMesh->SetPositionFace(1,&Face);
	Face.Set(3,4,2); pClodMesh->SetPositionFace(2,&Face);
	Face.Set(6,4,3); pClodMesh->SetPositionFace(3,&Face);
	Face.Set(3,5,6); pClodMesh->SetPositionFace(4,&Face);
	Face.Set(3,5,1); pClodMesh->SetPositionFace(5,&Face);
	Face.Set(0,14,15); pClodMesh->SetPositionFace(6,&Face);
	Face.Set(3,2,7); pClodMesh->SetPositionFace(7,&Face);
	Face.Set(3,7,8); pClodMesh->SetPositionFace(8,&Face);
	Face.Set(3,8,9); pClodMesh->SetPositionFace(9,&Face);
	Face.Set(3,9,10); pClodMesh->SetPositionFace(10,&Face);
	Face.Set(10,11,13); pClodMesh->SetPositionFace(11,&Face);
	Face.Set(11,12,13); pClodMesh->SetPositionFace(12,&Face);

	Face.Set(16+1,16+4,16+2); pClodMesh->SetPositionFace(16+0-3,&Face);
	Face.Set(16+1,16+4,16+5); pClodMesh->SetPositionFace(16+1-3,&Face);
	Face.Set(16+3,16+4,16+2); pClodMesh->SetPositionFace(16+2-3,&Face);
	Face.Set(16+6,16+4,16+3); pClodMesh->SetPositionFace(16+3-3,&Face);
	Face.Set(16+3,16+5,16+6); pClodMesh->SetPositionFace(16+4-3,&Face);
	Face.Set(16+3,16+5,16+1); pClodMesh->SetPositionFace(16+5-3,&Face);
	Face.Set(16+0,16+14,16+15); pClodMesh->SetPositionFace(16+6-3,&Face);
	Face.Set(16+3,16+2,16+7); pClodMesh->SetPositionFace(16+7-3,&Face);
	Face.Set(16+3,16+7,16+8); pClodMesh->SetPositionFace(16+8-3,&Face);
	Face.Set(16+3,16+8,16+9); pClodMesh->SetPositionFace(16+9-3,&Face);
	Face.Set(16+3,16+9,16+10); pClodMesh->SetPositionFace(16+10-3,&Face);
	Face.Set(16+10,16+11,16+13); pClodMesh->SetPositionFace(16+11-3,&Face);
	Face.Set(16+11,16+12,16+13); pClodMesh->SetPositionFace(16+12-3,&Face);

	Face.Set(3,32,33); pClodMesh->SetPositionFace(26,&Face);
	Face.Set(33,10,34); pClodMesh->SetPositionFace(27,&Face);
	Face.Set(32,35,39); pClodMesh->SetPositionFace(28,&Face);
	Face.Set(35,36,39); pClodMesh->SetPositionFace(29,&Face);

	Face.Set(19,32,33); pClodMesh->SetPositionFace(30,&Face);
	Face.Set(33,26,34); pClodMesh->SetPositionFace(31,&Face);
	Face.Set(32,37,40); pClodMesh->SetPositionFace(32,&Face);
	Face.Set(37,38,40); pClodMesh->SetPositionFace(33,&Face);

	Face.Set(10,9,11); pClodMesh->SetPositionFace(34,&Face);
	Face.Set(26,25,27); pClodMesh->SetPositionFace(35,&Face);


	// 2 - NORMALS
	pVector->X() = 0.0f;  pVector->Y() = 0.0f;  pVector->Z() = 1.0f;
	pClodMesh->SetNormal(0,pVector);

	// 3 - DIFFUSE
	IFXVector4 diff_color( 0.0f, 1.0f, 0.0f, 1.0f );
	IFXVector4 diff_color1( 0.5f, 1.0f, 0.5f, 1.0f );
	IFXVector4 diff_color2( 0.5f, 0.5f, 0.0f, 1.0f );

	for (i=0; i < pClodMesh->GetMeshDesc()->NumDiffuseColors; i++)
	{
		pClodMesh->SetDiffuseColor( i++,	&diff_color  );
		pClodMesh->SetDiffuseColor( i++,	&diff_color1 );
		pClodMesh->SetDiffuseColor( i,	&diff_color2 );
	}


	//  NORMAL and DIFFUSE COLOR at FACE

	IFXAuthorFace FaceC, FaceN;
	int size = pClodMesh->GetMeshDesc()->NumFaces;
	for (int k=0; k < size; k++)
	{
		FaceN.Set(0,0,0);
		FaceC.Set(k*3,k*3+1,k*3+2);
		pClodMesh->SetNormalFace(k, &FaceN);
		pClodMesh->SetDiffuseFace(k, &FaceC);
	}

	// 4 - SPECULAR
	IFXVector4 spec_color( 0.0f, 1.0f, 0.0f, 1.0f );
	IFXVector4 spec_color1( 0.5f, 1.0f, 0.5f, 1.0f );
	IFXVector4 spec_color2( 0.5f, 0.5f, 0.0f, 1.0f );
	for (i=0; i < pClodMesh->GetMeshDesc()->NumSpecularColors; i++) {
		if( i%3 == 0 )
			pClodMesh->SetSpecularColor( i, &spec_color );
		else if( i%3 == 1 )
			pClodMesh->SetSpecularColor( i, &spec_color1 );
		else
			pClodMesh->SetSpecularColor( i, &spec_color2 );
	}

	// SPECULAR Face
	Face.Set(0,1,2);  // for simplicity
	for (i=0; i < pClodMesh->GetMeshDesc()->NumFaces; i++) {
		pClodMesh->SetSpecularFace(i, &Face);
	}

	// 5 - TEXTURECOORDS
	F32 tcoord;
	for (i=0; i < pClodMesh->GetMeshDesc()->NumTexCoords; i++) {
		tcoord = (float)START_TEX_COORD;
		pClodMesh->SetTexCoord( i, &(IFXVector4( tcoord+i, tcoord+1+i, tcoord+2+i, tcoord+3+i )) );
	}

	// 6 - MATERIALS

	IFXAuthorMaterial material_0;
	material_0.m_uNumTextureLayers    = 0;
	material_0.m_uOriginalMaterialID  = 0;
	material_0.m_uDiffuseColors       = TRUE;
	material_0.m_uSpecularColors      = FALSE;
	material_0.m_uNormals             = TRUE;

	IFXAuthorMaterial material_1;
	material_1.m_uNumTextureLayers    = 0;
	material_1.m_uOriginalMaterialID  = 1;
	material_1.m_uDiffuseColors       = FALSE;
	material_1.m_uSpecularColors      = TRUE;
	material_1.m_uNormals             = TRUE;

	IFXAuthorMaterial material_2;
	material_2.m_uNumTextureLayers    = 0;
	material_2.m_uOriginalMaterialID  = 2;
	material_2.m_uDiffuseColors       = FALSE;
	material_2.m_uSpecularColors      = TRUE;
	material_2.m_uNormals             = TRUE;

	IFXAuthorMaterial material_3;
	material_3.m_uNumTextureLayers    = 0;
	material_3.m_uOriginalMaterialID  = 3;
	material_3.m_uDiffuseColors       = FALSE;
	material_3.m_uSpecularColors      = TRUE;
	material_3.m_uNormals             = TRUE;

	IFXAuthorMaterial material_4;
	material_4.m_uNumTextureLayers    = 0;
	material_4.m_uOriginalMaterialID  = 4;
	material_4.m_uDiffuseColors       = TRUE;
	material_4.m_uSpecularColors      = FALSE;
	material_4.m_uNormals             = TRUE;


	IFXAuthorMaterial material_5;
	material_5.m_uNumTextureLayers    = 0;
	material_5.m_uOriginalMaterialID  = 5;
	material_5.m_uDiffuseColors       = TRUE;
	material_5.m_uSpecularColors      = TRUE;
	material_5.m_uNormals             = TRUE;

	IFXAuthorMaterial material_6;
	material_6.m_uNumTextureLayers    = 0;
	material_6.m_uOriginalMaterialID  = 6;
	material_6.m_uDiffuseColors       = TRUE;
	material_6.m_uSpecularColors      = FALSE;
	material_6.m_uNormals             = TRUE;

	IFXAuthorMaterial material_7;
	material_7.m_uNumTextureLayers    = 0;
	material_7.m_uOriginalMaterialID  = 7;
	material_7.m_uDiffuseColors       = TRUE;
	material_7.m_uSpecularColors      = TRUE;
	material_7.m_uNormals             = TRUE;

	IFXAuthorMaterial material_8;
	material_8.m_uNumTextureLayers    = 0;
	material_8.m_uOriginalMaterialID  = 8;
	material_8.m_uDiffuseColors       = TRUE;
	material_8.m_uSpecularColors      = FALSE;
	material_8.m_uNormals             = TRUE;

	// materials width ID  1, 2, 3,  are rendered very strange, probably not correct
	for (i=0; i < pClodMesh->GetMeshDesc()->NumMaterials; i++) {
		if( i==0 )
			pClodMesh->SetMaterial( i, &material_0 );

		if( i==1 )
			pClodMesh->SetMaterial( i, &material_1 ); // 1

		if( i==2 )
			pClodMesh->SetMaterial( i, &material_2 ); // 2

		if( i==3 )
			pClodMesh->SetMaterial( i, &material_3 ); // 3

		if( i==4 )
			pClodMesh->SetMaterial( i, &material_4 );

		if( i==5 )
			pClodMesh->SetMaterial( i, &material_5 );

		if( i==6 )
			pClodMesh->SetMaterial( i, &material_6 );

		if( i==7 )
			pClodMesh->SetMaterial( i, &material_7 );

		if( i==8 )
			pClodMesh->SetMaterial( i, &material_8 );
	}// for

	// 7 - FACE MATERIALS
	for (i=0; i < pClodMesh->GetMeshDesc()->NumFaces; i++) {

		pClodMesh->SetFaceMaterial(i,i%3);
		//                pClodMesh->SetFaceMaterial(i,i%(pClodMesh->GetMeshDesc()->NumMaterials));
	}

	return IFX_OK;
}

