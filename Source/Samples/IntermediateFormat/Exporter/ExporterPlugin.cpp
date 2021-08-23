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

/**
	@file	ExporterPlugin.cpp

			This module contains the IFXExporterPlugin class.

	@note	As a starting point, portions of this module were based on the 
			Discreet* 3ds max* SDK sample exporters and may contain 
			materials owned by Autodesk*, Inc.  Copyright 1994 - 2004.

			* Other names and brands may be claimed as the property of 
			  others.
*/

//------------- INCLUDES ----------------------------------------------------

#include "Common.h"
#include "max.h"
#include "resource.h"

#include "ExporterPlugin.h"
#include "Exporter.h"

//------------- PUBLIC METHODS ----------------------------------------------

/**
*  @return Returns the number of file name extensions supported by the plug-in.
*/
int IFXExporterPlugin::ExtCount()
{
	return 1;
}

/**
*  @return Returns the 'n-th' file name extension (i.e. "IDTF").
*/
const TCHAR * IFXExporterPlugin::Ext( int n )
{
	switch(n)
	{
	case 0:
		return (_T(EXPORTER_EXT));
	}
	return _T("");
}

/**
*  @return Returns a long ASCII description of the file type being exported
*  (i.e. "CAD 3D Working Group File").
*/
const TCHAR * IFXExporterPlugin::LongDesc()
{
	return GetString( IDS_LONGDESC );
}

/**
*  Returns a short ASCII description of the file type being exported
*  (i.e. "IDTF").
*/
const TCHAR * IFXExporterPlugin::ShortDesc()
{
	return GetString( IDS_SHORTDESC );
}

/**
*  @return Returns the ASCII Author name.
*/
const TCHAR * IFXExporterPlugin::AuthorName()
{
	return GetString( IDS_AUTHOR );
}

/**
*  @return Returns the ASCII Copyright message for the plug-in.
*/
const TCHAR * IFXExporterPlugin::CopyrightMessage()
{
	return GetString( IDS_COPYRIGHT );
}

/**
*  @return Returns the first message string that is displayed.
*/
const TCHAR * IFXExporterPlugin::OtherMessage1()
{
	return GetString( IDS_OTHER1 );
}

/**
*  @return Returns the second message string that is displayed.
*/
const TCHAR * IFXExporterPlugin::OtherMessage2()
{
	return GetString( IDS_OTHER2 );
}


/**
*  @return Returns the version number of the export plug-in.
*  The format is the version number * 100 (i.e. v3.01 = 301).
*/
unsigned int IFXExporterPlugin::Version()
{
	return EXPORTER_VER;
}


/**
*  About box Dialog handler
*/
static BOOL CALLBACK AboutDlgProc( HWND hWnd, UINT msg,
								  WPARAM wParam, LPARAM lParam )
{

	IFXExporterPlugin* exp =
		(IFXExporterPlugin*)GetWindowLong( hWnd, GWL_USERDATA );

	if( !exp && msg != WM_INITDIALOG )
		return FALSE;

	switch( msg )
	{
	case WM_INITDIALOG:
		// Update class pointer
		exp = (IFXExporterPlugin*)lParam;
		SetWindowLong( hWnd, GWL_USERDATA, lParam );
		break;

	case WM_DESTROY:
		break;

	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDOK:
			EndDialog( hWnd, 1 );
			break;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

/**
*  Show the aboutbox
*/
void IFXExporterPlugin::ShowAbout( HWND hWnd )
{
	DialogBoxParam( g_hInstance,
		MAKEINTRESOURCE(IDD_ABOUT),
		hWnd,
		AboutDlgProc,
		(LPARAM)this );
}

/**
*  This method is called for the plug-in to perform its file export.
*/
int IFXExporterPlugin::DoExport(
							 const TCHAR*  name,
							 ExpInterface* pEi,
							 Interface*    pI,
							 BOOL          suppressPrompts,
							 DWORD         options )
{
	//Exporter* theExp = new Exporter( name );
	IFXExporter theExp( name );

	//if( theExp == 0 ) return FALSE;

	if( theExp.initialize( pEi, pI ) == FALSE)
	{
		char buf[256];
		::sprintf(buf, GetString( IDS_OPENFILEERR ), name );
		::MessageBox( pI->GetMAXHWnd(), buf,
			GetString( IDS_EXPCAPTION), MB_OK | MB_ICONSTOP );
		return FALSE; // fail
	}

	theExp.export();

	theExp.finalize(); // close stream

	//delete theExp;

	return TRUE;
}


/**
*  This method is called by MAX to determine if one or more export options
*  are supported by a plug-in for a given extension.
*  It should return TRUE if all option bits set are supported for this
*  extension; otherwise FALSE.
*/
BOOL IFXExporterPlugin::SupportsOptions( int ext, DWORD options )
{
	// (from the SDK helpfile)
	// "This parameter [options] specifies which options are being queried,
	// and may have more than one option specified.  At present,
	// the only export option is SCENE_EXPORT_SELECTED, but this may
	// change in the future.  If more than one option is specified in
	// this field, the plugin should only return TRUE if all of the
	// options are supported.  If one or more of the options are not
	// supported, the plugin should return FALSE."

	if( options & SCENE_EXPORT_SELECTED )
		return TRUE;
	else
		return FALSE;

}


