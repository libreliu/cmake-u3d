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
#ifndef __IFXWRITER_H__
#define __IFXWRITER_H__

//------------- INCLUDES ----------------------------------------------------

#include "max.h" // color, matrix3, point3
#include "IFXDataTypes.h"
#include "Common.h"

//------------- CLASS, STRUCTURE AND TYPES ----------------------------------

/**
*  Interface for the IFXWriter class.
*/
class IFXWriter
{
public:
	~IFXWriter();
	IFXWriter();
	IFXWriter( const TCHAR* );
	IFXWriter( const IFXWriter& ) {};
	IFXWriter& operator= ( const IFXWriter& ) {};

	bool open();
	bool close();

	void begin( const IFXCHAR* );
	void begin( const IFXCHAR*, const int );
	void begin( const IFXCHAR*, const TCHAR*, bool = false );
	void begin( const IFXCHAR*, const IFXCHAR*, bool = false );
	void end();

	void output( const IFXCHAR*, const TCHAR*, bool = true );
	void output( const IFXCHAR*, const IFXCHAR*, bool = true );
	void output( const IFXCHAR*, const float, bool = true );
	void output( const IFXCHAR*, const int, bool = true );
	void output( const IFXCHAR*, const unsigned int, bool = true );
	void output( const IFXCHAR*, const Color* );
	void output( const IFXCHAR*, const Point3*, bool = true );
	void output( const IFXCHAR*, const U32* );
	void output( const IFXCHAR*, Matrix3* );
	void output( const IFXCHAR*, Quat* );

	void setPlain( bool plain ) { m_plain = plain; }
	void setPrecision( U32 prec ) { m_precision = prec; }
	void restorePrecision() { m_precision = DEFAULT_PRECISION; }

protected:
	void indent() const;
	void incIndent() { m_indent++; }
	void decIndent() { m_indent--; }

	void output( const Point3* );
	void output( const Color* );

	void output( const IFXCHAR*, bool = true );
	void output( const int ) const;
	void output( const unsigned int ) const;
	void output( const float ) const;

private:

	TSTR m_fileName;
	FILE* m_pFileHandle;
	int m_indent;
	bool m_plain;
	U32 m_precision;
};

#endif