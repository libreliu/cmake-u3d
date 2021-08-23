//***************************************************************************
//
//  Copyright (c) 2005 - 2006 Intel Corporation
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

/**
	@file	TTFontExtract.cpp

			This file contains functionality for creating glyphs from 
			from TrueType strings.

	@note	Some of the TrueType processing/parsing/extraction was derived 
			from Win32* API documentation.
*/


#include <windows.h>
#include <stdlib.h>
#include "CIFXGeom2D.h"
#include "IFXGlyph2DCommands.h"
#include "IFXGlyphCommandList.h"
#include "TTFontExtract.h"
#include "IFXArray.h"

//***************************************************************************
//  Defines
//***************************************************************************


#define _DEFAULT_QUALITY	5


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


/**
	This class serves an intermediate layer between FIXED and F32 types
	and provides easy two-sided conversion.
*/
class CFloat
{
public:
	CFloat( const F32 init ) { m_data = init; }
	CFloat( const FIXED init ) { long* lData = (long*)&init; m_data = (*lData)/(F32)65536.0; }
	operator F32() { return m_data; };
	operator FIXED() { 	long lData = (long)(m_data*65536L);	return *(FIXED *)&lData; }
private:
	F32 m_data;
};

/**
	This class contains TrueType font extraction functionality.
*/
class TTFontExtract
{
public:
	typedef IFXArray<SIFXPoint2d> VertexSet;
	typedef IFXArray<VertexSet> CharSet;

	TTFontExtract() : m_curveSteps( _DEFAULT_QUALITY ), m_lineSteps( _DEFAULT_QUALITY ) {}
	TTFontExtract( U32 cs, U32 ls ) : m_curveSteps(cs), m_lineSteps(ls) {}

	/// This function gets vertex coordinates for text message pString drawn with font pFont.
	void GetGlyphString(char* pString, IFXCHAR* pFont, U32 fontSize, CharSet* pCharSet);
	/// This function gets vertex coordinates for character represented by pPolyHeader.
	void GetCharacter(LPTTPOLYGONHEADER pPolyHeader, DWORD hdrSize, CharSet* pCS);
	/// Line partition into sublines.
	void GetLine(SIFXPoint2d start, SIFXPoint2d end, SIFXPoint2d* pPoints);
	/// Spline interpolation
	void GetCurve(LPPOINTFX spline, SIFXPoint2d* pPoint);

	U32 GetPathFlag( U32 j ) { return pathFlags[j]; }
	SIFXPoint2d& GetShift( U32 j ) {return charShift[j]; }
private:
	U32 m_curveSteps;			///< Quality of curve interpolation
	U32 m_lineSteps;			///< Quality of line interpolation
	IFXArray<U32> pathFlags;	///< In multi-path characters defines path/glyph point
	VertexSet charShift;
};


//***************************************************************************
//  Public methods
//***************************************************************************


void TTFontExtract::GetGlyphString(char* pString, IFXCHAR* pFont, U32 fontSize, CharSet* pCharSet)
{
	HWND hWnd = GetDesktopWindow();
	HDC hdc = GetDC(hWnd);
	HFONT hFont, hOldFont;
	if( hdc )
		 hFont = CreateFont( fontSize, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, pFont );
	if( hFont )
		hOldFont = (HFONT)SelectObject( hdc, hFont );
	if( !hOldFont || hOldFont == HGDI_ERROR ) return;

	GLYPHMETRICS gm;
	MAT2 tm; //transformation matrix for character
	tm.eM11 = CFloat(1.0);
	tm.eM12 = CFloat(0.0);
	tm.eM21 = CFloat(0.0);
	tm.eM22 = CFloat(1.0);	
	U32 f = charShift.GetNumberElements();
	U32 i;
	for ( i = 0; i < strlen( pString ); i++ )
	{
		FillMemory( &gm, sizeof(GLYPHMETRICS), 0 );
		//create GlyphMetrics for character
		long charMem = GetGlyphOutline( hdc, pString[i], GGO_NATIVE , &gm, 0, NULL, &tm );
		if ( charMem > 0 )	
		{
			LPSTR charBitmap = new char[charMem];
			//create CharSet for character
			if ( charBitmap && GetGlyphOutline(hdc, pString[i], GGO_NATIVE, &gm, charMem, charBitmap, &tm) != GDI_ERROR)
				if ( ((TTPOLYGONHEADER *)charBitmap)->dwType == TT_POLYGON_TYPE )
					GetCharacter( (TTPOLYGONHEADER *)charBitmap, charMem, pCharSet );
			if( charBitmap )
				delete charBitmap;
		}
		charShift.CreateNewElement() = SIFXPoint2d( gm.gmCellIncX, gm.gmCellIncY );
	}
	SelectObject( hdc, hOldFont );
	DeleteObject( hFont );
	ReleaseDC(hWnd, hdc);
}

void TTFontExtract::GetCharacter(LPTTPOLYGONHEADER pPolyHeader, DWORD hdrSize, CharSet *pCS)
{
	LPTTPOLYGONHEADER pHeaderStart = pPolyHeader;
	LPTTPOLYCURVE pCurve;
	VertexSet vertices;
	POINTFX curPt, spline[3];
	U32 pathFlag = 0, i = 0, k, pointfxNum;
	//go through TrueType glyphmetrics
	while ((DWORD)pPolyHeader < (DWORD)(((LPSTR)pHeaderStart) + hdrSize))
	{
		curPt = pPolyHeader->pfxStart;
		vertices.Clear();
		vertices.CreateNewElement() = SIFXPoint2d( CFloat(curPt.x), CFloat(curPt.y) );
		pCurve = (LPTTPOLYCURVE) (pPolyHeader + 1);//first path

		while ((DWORD)pCurve < (DWORD)(((LPSTR)pPolyHeader) + pPolyHeader->cb))
		{
			//collect lines and curves and convert them into a set of points
			pointfxNum=(U32)pCurve->cpfx;
			if( pCurve->wType == TT_PRIM_LINE )
			{
				vertices.CreateNewElement() = SIFXPoint2d( CFloat(curPt.x), CFloat(curPt.y) );
				for (i = 0; i<pointfxNum; i++)
				{
					SIFXPoint2d* points = new SIFXPoint2d[m_lineSteps];
					GetLine( SIFXPoint2d( CFloat(curPt.x), CFloat(curPt.y) ),
						SIFXPoint2d( CFloat(pCurve->apfx[i].x), CFloat(pCurve->apfx[i].y) ),points);
					for( k=0; k<m_lineSteps; k++ )
						vertices.CreateNewElement() = points[k];
					vertices.CreateNewElement() = SIFXPoint2d( CFloat(pCurve->apfx[i].x), CFloat(pCurve->apfx[i].y) );
					curPt=pCurve->apfx[i];
					delete points;
				}
			}

			if( pCurve->wType == TT_PRIM_QSPLINE )
			{
				spline[0]=curPt;
				for (i = 0; i<pointfxNum-1; i++)
				{	
					spline[1] = pCurve->apfx[i];
					if( i<pointfxNum-2 ) {
						*(long*)&spline[2].x = (*(long*)&spline[1].x+*(long*)&pCurve->apfx[i+1].x)/2;
						*(long*)&spline[2].y = (*(long*)&spline[1].y+*(long*)&pCurve->apfx[i+1].y)/2;
					}
					else
						spline[2]=pCurve->apfx[i+1];

					SIFXPoint2d* points = new SIFXPoint2d[m_curveSteps];
					GetCurve(spline, points);
					for( k=0; k<m_curveSteps; k++ )
						vertices.CreateNewElement() = points[k];
					spline[0]=spline[2];
					delete points;
				}
				curPt=spline[2];
				vertices.CreateNewElement() = SIFXPoint2d( CFloat(curPt.x), CFloat(curPt.y) );
			}
			pCurve = (LPTTPOLYCURVE)&(pCurve->apfx[pointfxNum]);//next command
		}
		if( pCS )
			pCS->CreateNewElement() = vertices;
		pathFlags.CreateNewElement() = pathFlag;
		pathFlag = 1;
		pPolyHeader = (LPTTPOLYGONHEADER)(((LPSTR)pPolyHeader) + pPolyHeader->cb);//next path
	}
}

void TTFontExtract::GetLine(SIFXPoint2d start, SIFXPoint2d end, SIFXPoint2d *pPoints)
{
	F64 dx = (end.x - start.x)/m_lineSteps;
	F64 dy = (end.y - start.y)/m_lineSteps;
	U32 i;
	if( pPoints )
	{
		for( i = 0; i < m_lineSteps; i++ )
		{
			pPoints[i].x = start.x + i*dx;
			pPoints[i].y = start.y + i*dy;
		}
	}
}

void TTFontExtract::GetCurve(LPPOINTFX spline, SIFXPoint2d *pPoint)
{
	long *spl = reinterpret_cast<long *>(spline);
	F64 stepSize = 1.0/m_curveSteps;
	F64 t = 0.0;
	U32 i;
	for ( i = 0; i<m_curveSteps; i++ )
	{
		t += stepSize;
		pPoint[i].x = (((spl[0] - 2*spl[2] + spl[4]) * t + 2*spl[2] - 2*spl[0]) * t + spl[0])/65536.0;
		pPoint[i].y = (((spl[1] - 2*spl[3] + spl[5]) * t + 2*spl[3] - 2*spl[1]) * t + spl[1])/65536.0;
	}
}


//***************************************************************************
//  Global functions
//***************************************************************************


IFXRESULT CreateGlyphString(IFXGlyphCommandList* pComList, IFXCHAR* pString, IFXCHAR* pFont, U32 size)
{
	IFXRESULT result = IFX_OK;
	char* pMBString;
	TTFontExtract::CharSet charSet;
	TTFontExtract ex( 5,2 );

	if( !pComList || !pString || !pFont )
		result = IFX_E_INVALID_POINTER;
	U32 len = wcstombs(NULL, pString, 0);
	if( IFXSUCCESS( result ) )
		pMBString = new char[len+1];
	if( !pMBString )
		result = IFX_E_OUT_OF_MEMORY;
	if( IFXSUCCESS( result ) )
	{
		wcstombs(pMBString, pString, len);
		pMBString[len]='\0';
	}

	ex.GetGlyphString(pMBString, pFont, size, &charSet);
	if( pMBString )
		delete pMBString;
	U32 j, ch, i;
	for( j = 0, ch = 0; j < charSet.GetNumberElements() && IFXSUCCESS( result ); j++)
	{
		if( ex.GetPathFlag(j) == 0 )
			result = pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTGLYPH );
		if( IFXSUCCESS( result ) )
			result = pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_STARTPATH );
		if( IFXSUCCESS( result ) )
			result = pComList->AddMoveToBlock(floor(charSet[j][0].x+.5), floor(charSet[j][0].y+.5));
		for( i = 0; i<charSet[j].GetNumberElements()-1 && IFXSUCCESS( result ); i++ )
			result = pComList->AddLineToBlock( charSet[j][i+1].x, charSet[j][i+1].y );
		if( IFXSUCCESS( result ) )
			result = pComList->AddLineToBlock(charSet[j][0].x, charSet[j][0].y);
		if( IFXSUCCESS( result ) )
			result = pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDPATH );
		if( ( j < charSet.GetNumberElements()-1 && ex.GetPathFlag(j+1) == 0 ) || j == charSet.GetNumberElements()-1 )
			if( IFXSUCCESS( result ) )
			{
				result = pComList->AddTagBlock( IFXGlyph2DCommands::IGG_TYPE_ENDGLYPH, ex.GetShift( ch ).x, ex.GetShift( ch ).y );
				ch++;
			}
	}
	for( j=0; j<charSet.GetNumberElements(); j++ )
		charSet[j].Clear();
	charSet.Clear();
	return result;
}
