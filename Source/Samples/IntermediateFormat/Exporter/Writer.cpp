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

//------------- INCLUDES ----------------------------------------------------

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "Writer.h"
#include "Tokens.h"

//------------- PUBLIC METHODS ----------------------------------------------

// contructor
IFXWriter::IFXWriter(): m_indent(0), m_plain(false)
{
}

// constructor
IFXWriter::IFXWriter( const TCHAR* fileName ):
m_indent(0),
m_fileName(fileName),
m_plain(false),
m_precision(DEFAULT_PRECISION)
{
}

// destructor
IFXWriter::~IFXWriter()
{
}

// open output file	
bool IFXWriter::open()
{
	m_pFileHandle = ::_tfopen( m_fileName, "w" );
	if( m_pFileHandle == 0 )
		return false;

	return true;
}

// close output file
bool IFXWriter::close()
{
	if( m_pFileHandle != 0 )
	{
		::_ftprintf( m_pFileHandle, "\n" );
		::fclose( m_pFileHandle );
		m_pFileHandle = NULL;
	}
	return true;
}

// begin block
void IFXWriter::begin( const IFXCHAR* in )
{
	indent();
	if( wcscmp( in, L"" ) != 0 )
		::fwprintf( m_pFileHandle, L"%ls ", in );
	::_ftprintf( m_pFileHandle, "{" );
	incIndent();
}

void IFXWriter::begin( const IFXCHAR* in, const int par )
{
	indent();
	if( wcscmp( in, L"" ) != 0 )
		::fwprintf( m_pFileHandle, L"%ls ", in );
	::_ftprintf( m_pFileHandle, "%d ", par );
	::_ftprintf( m_pFileHandle, "{" );
	incIndent();
}

void IFXWriter::begin( const IFXCHAR* in, const TCHAR* par, bool quote )
{
	indent();
	if( wcscmp( in, L"" ) != 0 )
		::fwprintf( m_pFileHandle, L"%ls ", in );
	if( _tcscmp( par, "" ) != 0 )
		if( quote )
			::_ftprintf( m_pFileHandle, "\"%s\" ", par );
		else
			::_ftprintf( m_pFileHandle, "%s ", par );
	::_ftprintf( m_pFileHandle, "{" );
	incIndent();
}

void IFXWriter::begin( const IFXCHAR* in, const IFXCHAR* par, bool quote )
{
	indent();
	if( wcscmp( in, L"" ) != 0 )
		::fwprintf( m_pFileHandle, L"%ls ", in );
	if( wcscmp( par, L"" ) != 0 )
		if( quote )
			::fwprintf( m_pFileHandle, L"\"%ls\" ", par );
		else
			::fwprintf( m_pFileHandle, L"%ls ", par );
	::_ftprintf( m_pFileHandle, "{" );
	incIndent();
}

// end block
void IFXWriter::end()
{
	decIndent();
	indent();
	::_ftprintf( m_pFileHandle, "}" );
}

// output string param
void IFXWriter::output( const IFXCHAR* id, const TCHAR* param, bool isIndent )
{
	if( !m_plain && isIndent == true ) indent();
	::fwprintf( m_pFileHandle, L"%ls", id );
	::_ftprintf( m_pFileHandle, " \"%s\"", param );
}

// output string param
void IFXWriter::output( const IFXCHAR* id, const IFXCHAR* param, bool isIndent )
{
	if( !m_plain && isIndent == true ) indent();
	::fwprintf( m_pFileHandle, L"%ls \"%ls\"", id, param );
}

// output float param
void IFXWriter::output( const IFXCHAR* id, const float param, bool isIndent )
{
	if( !m_plain && isIndent == true ) indent();
	if( wcscmp( id, L"" ) != 0 ) ::fwprintf( m_pFileHandle, L"%ls ", id );
	output( param );
}

// output int param
void IFXWriter::output( const IFXCHAR* id, const int param, bool isIndent )
{
	if( !m_plain && isIndent == true ) indent();
	::fwprintf( m_pFileHandle, L"%ls ", id );
	output( param );
}

// output unsigned int param
void IFXWriter::output( const IFXCHAR* id, const unsigned int param, bool isIndent )
{
	if( !m_plain && isIndent == true ) indent();
	::fwprintf( m_pFileHandle, L"%ls ", id );
	output( param );
}

// output color param
void IFXWriter::output( const IFXCHAR* id, const Color* p )
{
	output( id, true );
	output( p->r );
	output( p->g );
	output( p->b );
}

// output point3 param
void IFXWriter::output( const IFXCHAR* id, const Point3* p, bool isIndent )
{
	if( !m_plain && isIndent == true ) indent();
	if( wcscmp( id, L"" ) != 0 )
		::fwprintf( m_pFileHandle, L"%ls ", id );
	output( p->x );
	output( p->y );
	output( p->z );
}

// output int3 param
void IFXWriter::output( const IFXCHAR* id, const U32* p )
{
	if( !m_plain )
		indent();
	if( wcscmp( id, L"" ) != 0 )
		::fwprintf( m_pFileHandle, L"%ls ", id );
	output( p[0] );
	output( p[1] );
	output( p[2] );
}

// Print out a transformation matrix in different ways.
// Apart from exporting the full matrix we also decompose
// the matrix and export the components.
void IFXWriter::output( const IFXCHAR* id, Matrix3* m )
{
	Point3 row;

	begin( id );

	// Dump the whole Matrix
	row = m->GetRow(0);
	indent();
	output(&row);
	output((float)0.0);

	row = m->GetRow(1);
	indent();
	output(&row);
	output((float)0.0);

	row = m->GetRow(2);
	indent();
	output(&row);
	output((float)0.0);

	row = m->GetRow(3);
	indent();
	output(&row);
	output((float)1.0);

	end();
}

void IFXWriter::output( const IFXCHAR* id, Quat* q )
{
	output( id, q->x );
	output( L"", q->y, false );
	output( L"", q->z, false );
	output( L"", q->w, false );

}

//------------- PROTECTED METHODS -------------------------------------------

// output point3
void IFXWriter::output( const Point3* p )
{
	output( p->x );
	output( p->y );
	output( p->z );
}

// output color
void IFXWriter::output( const Color* p )
{
	output( p->r );
	output( p->g );
	output( p->b );
}

// output Int
void IFXWriter::output( const int in ) const
{
	::_ftprintf( m_pFileHandle, "%d ", in );
}

// output unsigned Int
void IFXWriter::output( const unsigned int in ) const
{
	::_ftprintf( m_pFileHandle, "%u ", in );
}

// output Float
void IFXWriter::output( const float in ) const
{
	TCHAR format[MAX_TOKEN_LEN];
	TCHAR prec[MAX_TOKEN_LEN];

	_itoa( m_precision, prec, 10 );
#ifdef F32_EXPONENTIAL
	strcpy( format, "%g " );
#else
	strcpy( format, "%." );
	strcat( format, prec );
	strcat( format, "f " );
#endif
	::_ftprintf( m_pFileHandle, format, in );
}

// output String
void IFXWriter::output( const IFXCHAR* in, bool isIndent )
{
	if( isIndent == true ) indent();
	::fwprintf( m_pFileHandle, L"%ls ", in );
}

// output Indent
void IFXWriter::indent() const
{
	::_ftprintf( m_pFileHandle, "\n" );
	for( int i = 0; i < m_indent; ++i )
	{
		::_ftprintf( m_pFileHandle, "\t" );
	}
}

//------------- PRIVATE METHODS ---------------------------------------------