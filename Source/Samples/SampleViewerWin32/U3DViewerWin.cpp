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

#include "U3DViewer.h"
#include "U3DCameraNavigation.h"

#include "IFXAutoRelease.h"
#include "IFXCheckX.h"
#include "IFXExportingCIDs.h"
#include "IFXModifierChain.h"
#include "IFXNotificationManager.h"
#include "IFXPalette.h"
#include "IFXPickObject.h"
#include "IFXScheduler.h"
#include "IFXWriteBuffer.h"
#include "IFXWriteManager.h"
#include "IFXRenderingCIDs.h"
#include "IFXSchedulingCIDs.h"

#include <dlgs.h>
#include <commctrl.h>
#include <windowsx.h>

#include "resource.h"


#define U3D_DEFAULT_WIN_WIDTH 640
#define U3D_DEFAULT_WIN_HEIGHT 480
#define THE_LAST_MENU_ID ID_OPTIONS_NEXTVIEW + 100

#define CXEDIT		30
#define CYEDIT		22
#define CXUPDOWN	8

#define SB_PROGRESSBAR_WIDTH 200
#define SB_FRAMERATE_WIDTH 80

#define _MAXIMUM_STRING_LENGTH		512
#define _MAXIMUM_ELAPSED_TIME_MS	1.5f

#define _GENERATED_VIEW_NAME_IF_NONE_AVAILABLE	L"GeneratedViewSinceFileHadNone"

static U3DViewer*	gs_pU3DView				= NULL;
#define WINCLASSNAME L"SGViewer"
#define APPNAME	L"SamplePlayer "


void U3DViewer::SetMenuStates()
{
	//Loading mode
	CheckMenuItem( m_hMenu, ID_OPTIONS_ATOMICLOADING,
		m_bLoadingAtomic ? MF_CHECKED : MF_UNCHECKED );

	//Picking mode
	CheckMenuItem( m_hMenu, ID_OPTIONS_MULTIPICKING,
		m_bMultiPicking ? MF_CHECKED : MF_UNCHECKED );

	//Target framerate
	CheckMenuItem( m_hMenu, ID_OPTIONS_USETARGETFRAMERATE,
		m_UseTargetFramerate ? MF_CHECKED : MF_UNCHECKED );

	//Rendering mode
	CheckMenuItem( m_hMenu, ID_OPTIONS_OPENGL,
		m_uRendererID == U3D_RENDERER_OPENGL ? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( m_hMenu, ID_OPTIONS_DIRECTX7,
		m_uRendererID == U3D_RENDERER_DIRECTX7 ? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( m_hMenu, ID_OPTIONS_DIRECTX8,
		m_uRendererID == U3D_RENDERER_DIRECTX8 ? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( m_hMenu, ID_OPTIONS_NULL,
		m_uRendererID == U3D_RENDERER_NULL ? MF_CHECKED : MF_UNCHECKED );

	//Culling Mode
	CheckMenuItem( m_hMenu, ID_OPTIONS_SCENEGRAPHCULLER,
		m_uCullerID == U3D_SCENEGRAPH_CULLER ? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( m_hMenu, ID_OPTIONS_OCTREECULLER,
		m_uCullerID == U3D_OCTREE_CULLER ? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( m_hMenu, ID_OPTIONS_USEVIEWPORT,
		m_bPreserveViewSettings ? MF_CHECKED : MF_UNCHECKED );
}

DLGPROC DialogProcHelper(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return (DLGPROC)gs_pU3DView->RemoteLoadingDialogProc(hDlg, msg, wParam, lParam);
}

INT_PTR U3DViewer::RemoteLoadingDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HWND hwndCtrl;
	UNREFERENCED_PARAMETER( lParam );

	switch(msg)
	{
	case WM_COMMAND:
		{
		switch(LOWORD(wParam))
		{
		case IDREMOTEOK:
			hwndCtrl = GetDlgItem(hDlg, IDC_REMOTESTREAMCOMBO);
			GetWindowText(hwndCtrl, m_strU3DFilename, 256);
			EndDialog(hDlg, IDREMOTEOK);
			break;

		case IDREMOTECANCEL:
			m_strU3DFilename[0] = NULL;
			EndDialog(hDlg, IDREMOTECANCEL);
			break;
		}

		return TRUE;
		}
	default:
		return FALSE;
	}
}

/**
	Message routing callback function for the viewer  window.

@param    HWND hWnd     Handle to window receiving messages
@param    UINT msg    Message identifier
@param    WPARAM wParam   See Win32 SDK
@param    LPARAM lParam   See Win32 SDK

@return LRESULT     True if the message that this thread processes is given, otherwise
the message is sent to a default handler.
*/

LRESULT CALLBACK SGWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	return gs_pU3DView->SGViewProc( hWnd, message, wParam, lParam );
}

/**
	Find Local matrix linked with given Instance

@param		IFXNode* pNode		pointer to Node which local matrix is finding
@param		U32 instance		instance of node

@return		matrix linked with given instance

@note		this function may be moved to IFXNode component
*/
U32 MatrixNumber(IFXNode* pNode, U32 instance)
{
	U32 result = 0;

	if (instance != 0)
	{
		U32 numInstances, numParents;
		IFXCHECKX(pNode->GetNumberOfInstances(&numInstances));
		IFXCHECKX(instance >= numInstances);
		numParents = pNode->GetNumberOfParents();
		if (numInstances == numParents)
			result = instance;
		else
		{
			U32 i;
			numInstances = 0;
			for (i = 0; i < numParents; i++)
			{
				IFXNode *pParentNode = pNode->GetParentNR(i);
				IFXCHECKX(NULL == pParentNode);
				U32 numParentInstances = 0;
				IFXCHECKX(pParentNode->GetNumberOfInstances(&numParentInstances));
				numInstances += numParentInstances;
				if (instance > numInstances-1)
					result++;
				else
					break;
			}
		}
	}

	return result;
}

IFXRESULT U3DViewer::SGViewProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	IFXRESULT result = IFX_OK;

	static POINT  MousePoint;
	static BOOL   bMouseDown = FALSE;
	static BOOL   bRMouseDown = FALSE;
	static BOOL   bSpaceDown = FALSE;
	static BOOL   bYKeyDown = FALSE;
	static BOOL   bFirstDrag = TRUE;
	static BOOL   bZeroX = FALSE;
	static BOOL   bZeroY = FALSE;

	HWND hViewerWindow = (HWND)m_ViewerWindow.GetWindowPtr();

	switch ( message )
	{
	case WM_ERASEBKGND:
		if (m_bRendererInitialized)
			return 1;
		else
			return DefWindowProc(hWnd, message, wParam, lParam);

	case WM_KEYDOWN:
		switch ( wParam )
		{
		case VK_ESCAPE:
			if (m_pSGUtils && !m_pSGUtils->IsU3DFileLoaded()) 
			{
				m_pSGUtils->CancelLoading();
				SendMessage(m_hStatusBar, SB_SETTEXT, 0, (LPARAM)L"Canceled");
			} 
			else 
			{
				PostMessage( hWnd, WM_CLOSE, 0, 0 );
			}
			break;
		case VK_SPACE:
			bSpaceDown = TRUE;
			break;
		case 'p':
		case 'P':
			if (m_pSGUtils) 
			{
				BOOL bSimulationState = m_pSGUtils->GetSimulationStateX();
				m_pSGUtils->SetSimulationStateX(!bSimulationState, FALSE);
				IFXTRACE_GENERIC(L"[SamplePlayer] switch SimulationState\n");
			}
			break;
		case 'v':
		case 'V':
			PostMessage( hWnd, WM_COMMAND, ID_OPTIONS_USEVIEWPORT, 0 );
			break;
		case 'n':
		case 'N':
			PostMessage( hWnd, WM_COMMAND, ID_OPTIONS_NEXTVIEW, 0 );
			break;
		case 'm':
		case 'M':
			PostMessage( hWnd, WM_COMMAND, ID_OPTIONS_NEXTVIEWINSTANCE, 0 );
			break;
		default:
			bSpaceDown = FALSE;
		}
		break;

	case WM_DESTROY:
		{
			if (m_hFPSEdit)
			{
				DestroyWindow(m_hFPSEdit);
				m_hFPSEdit = 0;
			}
			if (m_hCLODLevelEdit)
			{
				DestroyWindow(m_hCLODLevelEdit);
				m_hCLODLevelEdit = 0;
			}

			if (!DestroyMenu(m_hMenu))
				result = IFX_E_SUBSYSTEM_FAILURE;

			PostQuitMessage( 0 );
		}
		break;

	case WM_MOUSEMOVE:
		{
			BOOL bCtrlDown  = wParam & MK_CONTROL;
			BOOL bShiftDown = wParam & MK_SHIFT;
			POINTS pt = MAKEPOINTS( lParam );
			double dx = pt.x - (short)MousePoint.x;
			double dy = pt.y - (short)MousePoint.y;
			if ( 0 == dx && 0 == dy ) break;

			if ( bFirstDrag )
			{
				bFirstDrag = FALSE;
				if ( bShiftDown )
				{
					bZeroX = fabs(dx) < fabs(dy);
					bZeroY = !bZeroX;
				}
			}
			if ( bMouseDown && m_pSGUtils )
			{
				IFXDECLARELOCAL( IFXNode, pNode );

				if( NULL != m_pNodePalette )
				{
					U32 viewId = m_pViewIndeces[m_uActiveViewIndex];
					result = m_pNodePalette->GetResourcePtr( viewId, IID_IFXNode, (void**) &pNode );
				}
				else
					result = IFX_E_NOT_INITIALIZED;

				if( IFXSUCCESS( result ) )
				{
					U32 numMatrix = MatrixNumber(pNode, m_uActiveViewInstance);
					IFXMatrix4x4& viewMatrix = (IFXMatrix4x4&)pNode->GetMatrix(numMatrix);

					if ( bSpaceDown ) 
					{
						if (bZeroX)
							dx = 0;
						if (bZeroY)
							dy = 0;

						IFXCHECKX(PanView( (F32)dx, (F32)dy, &viewMatrix ));
					} 
					else
						if ( bCtrlDown )
							IFXCHECKX(ZoomView( (F32)dx, (F32)dy, bShiftDown, &viewMatrix ));
						else
							IFXCHECKX(RotateView( (F32)dx, (F32)dy, bShiftDown, bYKeyDown, m_pivotPoint, &viewMatrix));

					IFXCHECKX(pNode->SetMatrix(numMatrix, &viewMatrix));
				}
			}

			MousePoint.x = LOWORD( lParam );
			MousePoint.y = HIWORD( lParam );
		}
		break;

	case WM_LBUTTONDOWN:
		MousePoint.x = LOWORD( lParam );
		MousePoint.y = HIWORD( lParam );
		bMouseDown = TRUE;
		bFirstDrag = TRUE;
		bZeroX = FALSE;
		bZeroY = FALSE;
		SetCapture(hWnd);
		break;

	case WM_RBUTTONDOWN:
		{
			m_pPickedModel = NULL;
			MousePoint.x = LOWORD( lParam );
			MousePoint.y = HIWORD( lParam );
			bRMouseDown = TRUE;
			bFirstDrag = TRUE;
			bZeroX = FALSE;
			bZeroY = FALSE;
			SetCapture(hWnd);

			if (m_pSGUtils) 
			{
				IFXUnknown* pUnknown = NULL;
				POINTS pt = MAKEPOINTS( lParam );
				IFXString ModelName;
				U32 uRID;

				IFXRect clientRect = m_ViewerWindow.GetWindowSize();

				if (!m_bMultiPicking) 
				{
					IFXDECLARELOCAL( IFXView, pView );

					U32 viewId = m_pViewIndeces[m_uActiveViewIndex];
					result = m_pNodePalette->GetResourcePtr( viewId, IID_IFXView, (void**) &pView );

					if (IFXSUCCESS(result))
					{
						pView->Pick( pt.x - (F32)clientRect.m_X, 
									 pt.y - (F32)clientRect.m_Y,
									 m_uActiveViewInstance, 
									 &m_pPickedModel, &m_PickedInstance );
					}

					if (NULL != m_pPickedModel) 
					{
						result = m_pNodePalette->FindByResourcePtr(m_pPickedModel, &uRID);

						if (IFXSUCCESS(result))
							result = m_pNodePalette->GetName(uRID, &ModelName);

						if (IFXSUCCESS(result)) 
						{
							IFXTRACE_GENERIC(L"[SamplePlayer] %ls (%i) was picked\n", ModelName.Raw(), m_PickedInstance);
							SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)ModelName.Raw());
						}

						if (IFXSUCCESS(result)) 
						{
							IFXRESULT qiresult = IFX_E_CANNOT_FIND;
							U32 ModifierCount, i;
							IFXDECLARELOCAL(IFXModifierChain, pModifierChain);
							IFXDECLARELOCAL(IFXModifier, pMod);
							result = m_pPickedModel->GetModifierChain(&pModifierChain);
							IFXRELEASE(m_pCLODModifier);

							if (IFXSUCCESS(result))
								result = pModifierChain->GetModifierCount(ModifierCount);

							for (i = 1; i < ModifierCount && IFXSUCCESS(result); i++) 
							{
								result = pModifierChain->GetModifier( i, pMod );

								if (IFXSUCCESS(result))
									qiresult = pMod->QueryInterface(IID_IFXCLODModifier, (void**)&m_pCLODModifier);

								if (IFXSUCCESS(qiresult))
									break;
							}

							if (m_hCLODLevelEdit)
							{
								if (IFXSUCCESS(result) && IFXSUCCESS(qiresult)) 
								{
									F32 CLODLevel = 0.0f;
									EnableWindow(m_hCLODLevelEdit, TRUE);
									result = m_pCLODModifier->GetCLODLevel(&CLODLevel);
									if (IFXSUCCESS(result)) 
									{
										SetDlgItemInt(m_hToolBar, IDM_CLODLEVELEDIT, (U32)(CLODLevel*100), FALSE);
									}
								} 
								else 
								{
									SetDlgItemInt(m_hToolBar, IDM_CLODLEVELEDIT,	0, FALSE);
									EnableWindow(m_hCLODLevelEdit, FALSE);
								}
							}
						}
					} 
					else 
					{
						IFXTRACE_GENERIC(L"[SamplePlayer] nothing was picked\n");
						SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)L"whitespace");
					}
				} 
				else 
				{
					IFXSimpleList* pPickList = NULL;
					IFXPickObject* pPickObject = NULL;

					IFXDECLARELOCAL( IFXView, pView );

					U32 viewId = m_pViewIndeces[m_uActiveViewIndex];
					result = m_pNodePalette->GetResourcePtr( viewId, IID_IFXView, (void**) &pView );

					if (IFXSUCCESS(result))
					{
						result = pView->PickMultiple(	pt.x - (F32)clientRect.m_X, 
														pt.y - (F32)clientRect.m_Y, 
														m_uActiveViewInstance, 
														&pPickList);
					}

					// --- Process the pick
					if (pPickList && IFXSUCCESS(result)) 
					{
						U32 PickCount;
						result = pPickList->GetCount(&PickCount);
						// Get the pick data for the objects

						U32 i;
						for (i = 0; i < PickCount; i++) 
						{
							if (IFXSUCCESS(result))
							{
								result = pPickList->Get(i, &pUnknown);
							}

							if (pUnknown && IFXSUCCESS(result) )
							{
								result = pUnknown->QueryInterface(IID_IFXPickObject, (void **)&pPickObject);
							}

							IFXRELEASE(pUnknown);

							// Get the picked model
							if (pPickObject && IFXSUCCESS(result)) 
							{
								m_pPickedModel = pPickObject->GetModel();  // Does not result in a reference
								m_PickedInstance = pPickObject->GetInstance();

								if (!m_pPickedModel)
									result = IFX_E_INVALID_POINTER;
							}

							if (m_pNodePalette && IFXSUCCESS(result))
								result = m_pNodePalette->FindByResourcePtr(m_pPickedModel, &uRID);

							if (m_pNodePalette && IFXSUCCESS(result))
								result = m_pNodePalette->GetName(uRID, &ModelName);
							if (IFXSUCCESS(result)) 
							{
								IFXTRACE_GENERIC(L"[SamplePlayer] %i %ls (%i) was picked\n", i, ModelName.Raw(), m_PickedInstance);
								SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)ModelName.Raw());
							}
						}
					} 
					else 
					{
						IFXTRACE_GENERIC(L"[SamplePlayer] nothing was picked\n");
						SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)L"whitespace");
					}
				}
			}
		}
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		bMouseDown = FALSE;
		bRMouseDown = FALSE;
		ReleaseCapture();
		m_pPickedModel = NULL;
		m_PickedInstance = 0;
		break;

	case WM_SIZE:
		if (hViewerWindow != NULL && SIZE_MINIMIZED != wParam ) 
		{
			RECT rcWindowClient, rcToolBar;
			GetWindowRect(GetDlgItem(hViewerWindow, ID_TOOLBAR), &rcToolBar);
			GetClientRect(hViewerWindow, &rcWindowClient);
			rcWindowClient.top += rcToolBar.bottom - rcToolBar.top;
			if(TRUE == m_bRendererInitialized &&
				(rcWindowClient.bottom - rcWindowClient.top) > 0 &&
				(rcWindowClient.right - rcWindowClient.left) > 0 &&
				m_bRenderIt) 
			{
				IFXCHECKX(SetWindowSize());
				IFXCHECKX(SetViewport());
				m_ZeroWindow = FALSE;
			}
			else 
			{
				m_ZeroWindow = TRUE;
			}
		}
		break;
	case WM_COMMAND:
#if(WINVER >= 0x0500)
		if (LOWORD(wParam) >= THE_LAST_MENU_ID) 
		{
			U32 tmp = LOWORD(wParam);
			U32 tmp1 = THE_LAST_MENU_ID;
			if (m_pSGUtils) 
			{
				m_uActiveViewIndex = tmp - tmp1;
				IFXCHECKX(SetViewport());
			}
		} 
		else
#endif
			switch (LOWORD(wParam)) 
			{
			case IDM_CLODLEVELEDIT:
				if (HIWORD(wParam) == EN_CHANGE && m_pCLODModifier)
				{
					BOOL booResult;
					U32 CLODLevel = GetDlgItemInt(m_hToolBar, IDM_CLODLEVELEDIT, 
												  &booResult, FALSE);
					if (booResult && CLODLevel <= 100)
						result = m_pCLODModifier->SetCLODLevel((F32)CLODLevel/100);
				}
				break;
			case ID_OPTIONS_USEVIEWPORT:
				m_bPreserveViewSettings = !m_bPreserveViewSettings;
				if (m_pSGUtils)
					IFXCHECKX(SetViewport());
				break;
			case ID_OPTIONS_ATOMICLOADING:
				{
					TBBUTTONINFO info = {0};
					info.dwMask = TBIF_STATE | TBIF_LPARAM ;
					info.cbSize = sizeof(info);
					SendMessage(m_hToolBar, TB_GETBUTTONINFO, IDM_LOADTYPE, (LPARAM)&info);
					if (m_bLoadingAtomic) {
						m_bLoadingAtomic = FALSE;
						info.fsState &= ~TBSTATE_CHECKED;
						IFXTRACE_GENERIC(L"[SamplePlayer] asynchronous loading\n");
					} else {
						m_bLoadingAtomic = TRUE;
						info.fsState |= TBSTATE_CHECKED;
						IFXTRACE_GENERIC(L"[SamplePlayer] atomic loading\n");
					}
					SendMessage(m_hToolBar, TB_SETBUTTONINFO, IDM_LOADTYPE, (LPARAM)&info);
				}
				break;
			case ID_OPTIONS_MULTIPICKING:
				if (m_bMultiPicking) {
					m_bMultiPicking = FALSE;
					IFXTRACE_GENERIC(L"[SamplePlayer] single picking\n");
				} else {
					m_bMultiPicking = TRUE;
					IFXTRACE_GENERIC(L"[SamplePlayer] multi picking\n");
				}
				break;
			case ID_OPTIONS_USETARGETFRAMERATE:
				if (m_UseTargetFramerate) 
				{
					m_UseTargetFramerate = FALSE;
					IFXTRACE_GENERIC(L"[SamplePlayer] Use Target Framerate\n");
				} 
				else 
				{
					m_UseTargetFramerate = TRUE;
					IFXTRACE_GENERIC(L"[SamplePlayer] Don't Use Target Framerate\n");
					if (m_pDevice)
						IFXCHECKX(m_pDevice->GetTargetFramerate(&m_TargetFrameRate));
				}
				EnableWindow(m_hFPSEdit, m_UseTargetFramerate);
				if (m_pDevice)
					m_pDevice->SetUseTargetFramerate(m_UseTargetFramerate);
				break;
			case ID_EXIT:
				// Recieved key/menu command to exit app
				SendMessage( hWnd, WM_CLOSE, 0, 0 );
				return 0;
			case ID_FILE_LOADFROMURL:
				DialogBox(NULL, MAKEINTRESOURCE(IDD_REMOTEDIALOG), hViewerWindow, (DLGPROC)DialogProcHelper );
				if (NULL != m_strU3DFilename[0]) 
				{
					result = Initialize(TRUE);
				}
				break;
			case IDM_LOADTYPE:
				{
					TBBUTTONINFO info = {0};
					info.dwMask = TBIF_STATE | TBIF_LPARAM ;
					info.cbSize = sizeof(info);
					if ( SendMessage( m_hToolBar, TB_GETBUTTONINFO, IDM_LOADTYPE, (LPARAM)&info ) >= 0 ) 
					{
						if ( (TBSTATE_CHECKED  & info.fsState) )
							m_bLoadingAtomic = TRUE;
						else
							m_bLoadingAtomic = FALSE;
					}
				}
				break;
			case IDM_OPEN:
			case ID_OPEN:
				m_strU3DFilename[0] = 0;
				ofn.lpstrFilter = L".u3d Files (.u3d)\0*.u3d\0All Files (*.*)\0*.*\0\0";
				ofn.lpstrTitle = L"Open U3D File";
				ofn.hwndOwner = hViewerWindow;
				if(TRUE == GetOpenFileName( &ofn )) 
				{
					result = Initialize();
					if (IFXSUCCESS(result)) 
					{
						EnableMenuItem(m_hMenu, ID_FILE_APPEND, MF_ENABLED);
					} 
					else 
					{
						IFXCHAR	pMessage[ _MAXIMUM_STRING_LENGTH ];

						if ( result == IFX_E_UNSUPPORTED )
							swprintf(
							pMessage,
							L"Received the unsupported return code 0x%08lX\n",
							result );
						else if ( result == IFX_E_INVALID_FILE )
							swprintf(
							pMessage,
							L"Received the invalid file return code 0x%08lX\n",
							result );
						else
							swprintf(
							pMessage,
							L"Received a failure with return code 0x%08lX\n",
							result );

						MessageBox( 
							hViewerWindow, 
							pMessage,
							APPNAME, 
							MB_ICONWARNING | MB_OK );

						result = IFX_OK;

						EnableMenuItem(m_hMenu, ID_FILE_APPEND, MF_DISABLED);				
					}
				}
				else
				{
					IFXCHAR	pMessage[ _MAXIMUM_STRING_LENGTH ];
					DWORD error = CommDlgExtendedError();
					if (error != 0)
					{
						swprintf(
							pMessage,
							L"Received an exception with error return code %X",
							error);
						MessageBox( 
							hViewerWindow, 
							pMessage,
							APPNAME, 
							MB_ICONWARNING | MB_OK );
					}
				}
				break;
			case ID_FILE_APPEND:
				result = IFX_E_ABORTED;
				ofn.lpstrFilter = L".u3d Files (.u3d)\0*.u3d\0All Files (*.*)\0*.*\0\0";
				ofn.lpstrTitle = L"Append U3D File";
				ofn.hwndOwner = hViewerWindow;
				if(TRUE == GetOpenFileName( &ofn )) {
					try {
						m_pSGUtils->LoadU3DFileInit(m_strU3DFilename, m_bLoadingAtomic);
						m_AppName.Concatenate(" + ");
						m_AppName.Concatenate(m_strU3DFilename);
						SetWindowText(hViewerWindow, (LPCTSTR)m_AppName);
						CreateProgressBar();
						m_bFindView = TRUE;

						result = IFX_OK;
					}
					catch(IFXException& e) {
						result = e.GetIFXResult();

						IFXCHAR	pMessage[ _MAXIMUM_STRING_LENGTH ];

						swprintf(
							pMessage,
							L"Received an exception with error return code "
							L"0x%08lX with description\n\"%ls\"\n",
							result,
							e.GetText().Raw() );

						MessageBox( 
							hViewerWindow, 
							pMessage,
							APPNAME, 
							MB_ICONWARNING | MB_OK );

						DestroyProgressBar();
					}
					catch( ... ) 
					{
						MessageBox( 
							hViewerWindow, 
							L"Received an unknown exception", 
							APPNAME, 
							MB_ICONWARNING | MB_OK );

						result = IFX_E_UNDEFINED;
					}
				}
				break;
			case ID_OPTIONS_SCENEGRAPHCULLER:
			{
				IFXDECLARELOCAL( IFXView, pView );

				U32 viewId = m_pViewIndeces[m_uActiveViewIndex];

				if( NULL != m_pNodePalette )
				{
					result = m_pNodePalette->GetResourcePtr( viewId, IID_IFXView, (void**) &pView );

					if (IFXSUCCESS(result))
					{
						IFXDECLARELOCAL(IFXSpatialSetQuery, pCullingSubsystem);
						IFXCHECKX(IFXCreateComponent(CID_IFXSceneGraphCuller, IID_IFXSpatialSetQuery,(void**)&pCullingSubsystem));
						IFXCHECKX(pView->SetCullingSubsystem(*pCullingSubsystem ));
					}
					m_uCullerID = U3D_SCENEGRAPH_CULLER;
				}
				break;
			}
			case ID_OPTIONS_OCTREECULLER:
			{
				IFXDECLARELOCAL( IFXView, pView );

				U32 viewId = m_pViewIndeces[m_uActiveViewIndex];

				if( NULL != m_pNodePalette )
				{
					result = m_pNodePalette->GetResourcePtr( viewId, IID_IFXView, (void**) &pView );

					if (IFXSUCCESS(result))
					{
						IFXDECLARELOCAL(IFXSpatialSetQuery, pCullingSubsystem);
						IFXCHECKX(IFXCreateComponent(CID_IFXOctreeCuller, IID_IFXSpatialSetQuery,(void**)&pCullingSubsystem));	
						IFXCHECKX(pView->SetCullingSubsystem(*pCullingSubsystem ));
					}
					m_uCullerID = U3D_OCTREE_CULLER;
				}
				break;
			}
			case ID_OPTIONS_OPENGL:
				if (m_bRendererInitialized)
					IFXCHECKX(SetRenderer(U3D_RENDERER_OPENGL));
				else
					m_uRendererID = U3D_RENDERER_OPENGL;
				break;
			case ID_OPTIONS_DIRECTX7:
				if (m_bRendererInitialized)
					IFXCHECKX(SetRenderer(U3D_RENDERER_DIRECTX7));
				else
					m_uRendererID = U3D_RENDERER_DIRECTX7;
				break;
			case ID_OPTIONS_DIRECTX8:
				if (m_bRendererInitialized)
					IFXCHECKX(SetRenderer(U3D_RENDERER_DIRECTX8));
				else
					m_uRendererID = U3D_RENDERER_DIRECTX8;
				break;
			case ID_OPTIONS_NULL:
				if (m_bRendererInitialized)
					IFXCHECKX(SetRenderer(U3D_RENDERER_NULL));
				else
					m_uRendererID = U3D_RENDERER_NULL;
				break;
			case ID_OPTIONS_NEXTVIEW:
			case ID_OPTIONS_NEXTVIEWINSTANCE:
				if (m_pSGUtils) 
				{
					U32 index, numInst;
					IFXString NodeName, NumParentsChar, OutString, ParentIndex;

					IFXDECLARELOCAL( IFXView, pView );

					U32 viewId = m_pViewIndeces[m_uActiveViewIndex];
					result = m_pNodePalette->GetResourcePtr( viewId, IID_IFXView, (void**) &pView );

					if (IFXSUCCESS(result))
						IFXCHECKX(pView->GetNumberOfInstances(&numInst));

					if (LOWORD(wParam) == ID_OPTIONS_NEXTVIEW) 
					{
						if (m_uActiveViewIndex+1 < m_uViewCounter)
							m_uActiveViewIndex++;
						else 
						{
							m_uActiveViewIndex = 0;
						}
						m_uActiveViewInstance = 0;
					} 
					else 
					{
						if (m_uActiveViewInstance+1 < numInst)
							m_uActiveViewInstance++;
						else
							m_uActiveViewInstance = 0;
					}

					IFXCHECKX(SetViewport());
					IFXCHECKX(m_pNodePalette->FindByResourcePtr(pView, &index));
					IFXCHECKX(NumParentsChar.ToString(numInst, 10));
					IFXCHECKX(ParentIndex.ToString(m_uActiveViewInstance, 10));
					IFXCHECKX(m_pNodePalette->GetName(index, &NodeName));
					OutString = NodeName + L" " + ParentIndex + L" (" + NumParentsChar + L")";
					SendMessage(m_hStatusBar, SB_SETTEXT, 2, (LPARAM)OutString.Raw());
				}
				break;
			case ID_OPTIONS_CHANGECLEARCOLOR:
				IFXDECLARELOCAL( IFXViewResource, pViewResource );
				// Obtain the current view's main view resource 
				// interface before proceeding.
				if ( m_pSGUtils && m_uActiveViewIndex < m_uViewCounter )
				{
					IFXDECLARELOCAL( IFXView, pView );

					U32 viewId = m_pViewIndeces[m_uActiveViewIndex];
					result = m_pNodePalette->GetResourcePtr( viewId, IID_IFXView, (void**) &pView );

					if (IFXSUCCESS(result))
						pViewResource = pView->GetViewResource();
				}

				if ( pViewResource )
				{
					// Get this view resource's clear color.
					IFXVector3	vectorRGB	= pViewResource->GetRenderClear().GetColorValue();

					#define _CUSTOM_COLORS_MAXIMUM	16

					// This static color array will hold user 
					// specified custom colors defined using this 
					// color picker dialog box through out the 
					// entire execution of the application.
					static COLORREF	s_pColors[ _CUSTOM_COLORS_MAXIMUM ] = { 0 };

					// Fill out the necessary dialog box information.
					CHOOSECOLOR		colorInfo;
					ZeroMemory( &colorInfo, sizeof( colorInfo ) );
					colorInfo.lStructSize	= sizeof( colorInfo );
					colorInfo.hwndOwner		= hWnd;
					colorInfo.rgbResult		= RGB( vectorRGB.X() * 255, 
												vectorRGB.Y() * 255, 
												vectorRGB.Z() * 255 );
					colorInfo.lpCustColors	= s_pColors;
					colorInfo.Flags			= CC_ANYCOLOR |
											CC_FULLOPEN |
											CC_RGBINIT;

					// Let the user pick a background clear color to 
					// use with the current view's view resource.  
					// If things went well and the user clicked on 
					// OK, the return value will be non-zero.
					if ( ChooseColor( &colorInfo ) )
					{
						vectorRGB.Set( float( GetRValue( colorInfo.rgbResult ) ) / 255, 
									float( GetGValue( colorInfo.rgbResult ) ) / 255, 
									float( GetBValue( colorInfo.rgbResult ) ) / 255 );

						// Set this view resource's clear color with 
						// the resulting color.
						pViewResource->GetRenderClear().SetColorValue( vectorRGB );
					}

					IFXRELEASE( pViewResource );
				}
				break;
			}
			SetMenuStates();
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}


	return result;
}


U3DViewer::U3DViewer() :
IFXDEFINEMEMBER(m_pCoreServices),
IFXDEFINEMEMBER(m_pGeneratorPalette),
IFXDEFINEMEMBER(m_pNodePalette),
IFXDEFINEMEMBER(m_pSceneGraph),
IFXDEFINEMEMBER(m_pDevice),
IFXDEFINEMEMBER(m_pRenderServices),
IFXDEFINEMEMBER(m_pITimer)
{
	m_pSGUtils = NULL;
	gs_pU3DView = this;

	// IFX scene resources
	m_pViewIndeces.push_back( 0 );

	m_uViewCounter = 0;

	m_uActiveViewIndex = 0;
	m_uActiveViewInstance = 0;
	m_pDevice = NULL;
	m_bRendererInitialized = FALSE;
	m_hProgress = NULL;
	m_bLoadingAtomic = TRUE;
	m_bMultiPicking = FALSE;
	m_UseTargetFramerate = FALSE;
	m_bPreserveViewSettings = FALSE;

	m_pivotPoint.Set(0.0f, 0.0f, 0.0f);
	m_hInstance = GetModuleHandle(NULL);
	m_hMenu = NULL;

	m_strU3DFilename[0] = 0;

	// Display the OpenFileName dialog. Then, try to load the specified file

	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = (HWND)m_ViewerWindow.GetWindowPtr();
	ofn.hInstance         = (HINSTANCE)m_hInstance;
	ofn.lpstrFilter     = L".u3d Files (.u3d)\0*.u3d\0\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = 0;
	ofn.nFilterIndex      = 0;
	ofn.lpstrFile         = m_strU3DFilename;
	ofn.nMaxFile          = 512;
	ofn.lpstrFileTitle    = NULL;
	ofn.nMaxFileTitle     = 0;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = L"Open U3D File";
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = L"u3d";
	ofn.lCustData         = NULL;
	ofn.lpfnHook       = NULL;
	ofn.lpTemplateName    = NULL;
	ofn.Flags             = OFN_FILEMUSTEXIST | OFN_EXPLORER;

	m_uRendererID = U3D_DEFAULT_RENDERER;
	m_uCullerID = U3D_DEFAULT_CULLER;

	m_pITimer = NULL;
	m_TimeOut = 0;

	m_pCLODModifier = NULL;

	m_bRenderIt = FALSE;
	m_bFindView = TRUE;
	m_startToFindCameras = TRUE;

	m_ViewID = 0;
	m_uViewCounter = 0;

	m_ZeroWindow = TRUE;
}


U3DViewer::~U3DViewer()
{
	// Clean up Windows objects
	UnregisterClass( WINCLASSNAME, HINSTANCE( m_hInstance ) );

	// release all COM objects
	IFXRELEASE(m_pCLODModifier);

	m_pViewIndeces.clear();

	IFXRELEASE(m_pNodePalette);
	IFXRELEASE(m_pGeneratorPalette);

	IFXDELETE(m_pSGUtils);
}

/**
	Parses CommandLine and fills UserOptions structure in U3DSamplePlayer

	@param    const char *cmdline   The application's CommandLine

	@return BOOL      Returns TRUE if the filename is proper.
*/
IFXRESULT U3DViewer::ParseArguments(const IFXCHAR *cmdline)
{
	IFXRESULT result = IFX_OK;
	IFXCHAR *pdest;
	U32 length;

	m_UserOptions.m_bLoadingAtomic = m_bLoadingAtomic;
	wcscpy(m_UserOptions.m_Filename, m_strU3DFilename);
	m_UserOptions.m_Renderer = m_uRendererID;
	m_UserOptions.m_Culler = m_uCullerID;
	m_UserOptions.m_TimeOut = m_TimeOut;

	if (cmdline[0]) {
		for (;;) {
			while (cmdline[0]==' ') cmdline++;
			if (cmdline[0]!='-') break;
			switch (cmdline[1]) {
		case 'r': //Renderer mode: DX7, DX8 or OGL
			if (!wcsncmp(cmdline,L"-rogl",5)) {
				m_UserOptions.m_Renderer = U3D_RENDERER_OPENGL;
			} else if (!wcsncmp(cmdline,L"-rdx7",5)) {
				m_UserOptions.m_Renderer = U3D_RENDERER_DIRECTX7;
			} else if (!wcsncmp(cmdline,L"-rdx8",5)) {
				m_UserOptions.m_Renderer = U3D_RENDERER_DIRECTX8;
			} else {
				IFXTRACE_GENERIC(L"[SamplePlayer] Unknown renderer. Switched to default one\n");
			}
			cmdline+=5;
			break;
		case 'c': //Culling mode: SG, OC
			if (!wcsncmp(cmdline,L"-csgr",5)) {
				m_UserOptions.m_Culler = U3D_SCENEGRAPH_CULLER;
			} else if (!wcsncmp(cmdline,L"-coct",5)) {
				m_UserOptions.m_Culler = U3D_OCTREE_CULLER;
			} else {
				IFXTRACE_GENERIC(L"[SamplePlayer] Unknown culler. Switched to default one\n");
			}
			cmdline+=5;
			break;
		case 'l': //Loading mode: non-atomic is default, you can switch it to atomic mode by -latom
			{
				TBBUTTONINFO info = {0};
				info.dwMask = TBIF_STATE | TBIF_LPARAM ;
				info.cbSize = sizeof(info);
				if (m_hToolBar)
					SendMessage(m_hToolBar, TB_GETBUTTONINFO, IDM_LOADTYPE, (LPARAM)&info);
				if (!wcsncmp(cmdline,L"-latom",6)) {
					m_UserOptions.m_bLoadingAtomic = TRUE;
					info.fsState |= TBSTATE_CHECKED;
					IFXTRACE_GENERIC(L"[SamplePlayer] atomic loading\n");
					cmdline+=4;
				} else if (!wcsncmp(cmdline,L"-lnatom",7)) {
					m_UserOptions.m_bLoadingAtomic = FALSE;
					info.fsState &= ~TBSTATE_CHECKED;
					IFXTRACE_GENERIC(L"[SamplePlayer] asynchronous loading\n");
					cmdline+=5;
				} else {
					IFXTRACE_GENERIC(L"[SamplePlayer] Unknown loading mode. Switched to default one\n");
				}
				if (m_hToolBar)
					SendMessage(m_hToolBar, TB_SETBUTTONINFO, IDM_LOADTYPE, (LPARAM)&info);
				cmdline+=2;
			}
			break;
		case 'i': //Input file: -ifilename.ext
			cmdline+=2;
			pdest = (IFXCHAR*)wcschr( cmdline, L' ' );
			if (!pdest) {
				pdest = (IFXCHAR*)wcschr( cmdline, NULL );
			}
			if (pdest) {
				length = (U32)(pdest - cmdline);
				wcsncpy(m_UserOptions.m_Filename, cmdline, length);
				m_UserOptions.m_Filename[length] = NULL;

				cmdline+=length;
			} else {
				IFXTRACE_GENERIC(L"[SamplePlayer] Unknown filename format.\n");
			}
			break;
		case 't': //Timeout in seconds
			cmdline+=2;
			pdest = (IFXCHAR*)wcschr( cmdline, L' ' );
			if (!pdest) {
				pdest = (IFXCHAR*)wcschr( cmdline, NULL );
			}
			if (pdest) {
				length = (U32)(pdest - cmdline);
				m_UserOptions.m_TimeOut = _wtoi(cmdline);
				cmdline+=length;
			} else {
				IFXTRACE_GENERIC(L"[SamplePlayer] Unknown filename format.\n");
			}
			break;
		default:
			IFXTRACE_GENERIC(L"[SamplePlayer] Unknown option: %ls\n", cmdline);
			}
		}
		m_bLoadingAtomic = m_UserOptions.m_bLoadingAtomic;
		m_uRendererID = m_UserOptions.m_Renderer;
		m_uCullerID = m_UserOptions.m_Culler;
		if (m_UserOptions.m_Filename[0] != 0) {
			wcscpy(m_strU3DFilename, m_UserOptions.m_Filename);
			m_TimeOut = m_UserOptions.m_TimeOut;

			result = Initialize();
			if (IFXSUCCESS(result)) {
				EnableMenuItem(m_hMenu, ID_FILE_APPEND, MF_ENABLED);
			} else {
				IFXCHAR pMessage[ _MAXIMUM_STRING_LENGTH ];

				swprintf(pMessage,
					L"File you wanted to open has unsupported version number or can't be opened for another reason.\nChoose another file.\n");

				MessageBox( 
					(HWND)m_ViewerWindow.GetWindowPtr(), 
					pMessage,
					APPNAME, 
					MB_ICONWARNING | MB_OK );

				result = IFX_OK;

				EnableMenuItem(m_hMenu, ID_FILE_APPEND, MF_DISABLED);				
			}
		}
		SetMenuStates();
	}

	return result;
}

/**
	Initializes U3DViewer.

	@return IFXRESULT         The return of this status.
*/
IFXRESULT U3DViewer::Initialize(const BOOL RemoteLoading)
{
	m_bRenderIt = FALSE;
	m_bFindView = TRUE;
	m_AppName = APPNAME;
	m_pPickedModel = NULL;
	m_PickedInstance = 0;
	return InitializeSG( NULL, RemoteLoading );
}

IFXRESULT U3DViewer::InitializeSG( U3DSceneUtilities* pSGUtils, const BOOL RemoteLoading )
{
	IFXRESULT result = IFX_OK;
	U8 uLoad = 0;

	if (m_pSGUtils) 
	{
		IFXRELEASE(m_pCoreServices);
		IFXRELEASE(m_pGeneratorPalette);
		IFXRELEASE(m_pNodePalette);
		IFXRELEASE(m_pSceneGraph);

		if (m_hCLODLevelEdit)
		{
			EnableWindow(m_hCLODLevelEdit, FALSE);
		}
		IFXRELEASE(m_pCLODModifier);

		IFXDELETE(m_pSGUtils);
		m_uActiveViewInstance = 0;

		m_startToFindCameras = TRUE;
		m_ViewID = 0;
#if(WINVER >= 0x0500)
		for (U32 i = 0; i < m_uViewCounter; i++)
			DeleteMenu(m_hMenu, THE_LAST_MENU_ID + i, MF_BYCOMMAND);
#endif
		m_uViewCounter = 0;
	}

	/*
		Reset device if another file is loaded to delete all textures
		from rendering system.
	*/
	if(NULL != m_pDevice) 
	{
		m_pDevice->Release();
		m_pDevice = NULL;
		m_bRendererInitialized = FALSE;
	}

	if ( IFXSUCCESS(result) ) 
	{
		if( !pSGUtils )
		{
			pSGUtils = new U3DSceneUtilities;
			result = pSGUtils->InitializeScene();
			uLoad = 1;
		}
	}
	if ( !pSGUtils ||
		!pSGUtils->m_pCoreServices ||
		!pSGUtils->m_pMaterialPalette ||
		!pSGUtils->m_pGeneratorPalette ||
		!pSGUtils->m_pNodePalette ||
		!pSGUtils->m_pSceneGraph ||
		!pSGUtils->m_pShaderPalette ||
		!pSGUtils->m_pTexturePalette )
		result = IFX_E_INVALID_POINTER;

	// Create and setup some callback tasks to monitor failures that occur 
	// during scheduler processing activities.
	if ( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXTaskCallback, pTaskCallback );

		result = IFXCreateComponent( 
			CID_IFXTaskCallback, 
			IID_IFXTaskCallback, 
			( void** ) &pTaskCallback );

		if ( IFXSUCCESS( result ) )
		{
			pTaskCallback->SetFunction( HandleNotifications );

			IFXDECLARELOCAL( IFXNotificationManager, pNotificationManager );

			result = pSGUtils->m_pCoreServices->GetNotificationManager( 
				IID_IFXNotificationManager, 
				( void** ) &pNotificationManager );

			if ( IFXSUCCESS( result ) )
			{
				IFXTaskHandle	taskHandle;

				// Rather than receiving all notifications, register to 
				// receive only the ones we are interested in monitoring.
				/// @todo	On IFXNotificationType_Idle, call Sleep(0) once in a while to improve UI responsiveness.
				result = pNotificationManager->RegisterTask( 
					pTaskCallback, 
					IFXNotificationType_Error, 
					IFXNotificationId_Nil, 
					this, 
					NULL, 
					&taskHandle );
				IFXASSERT( IFXSUCCESS( result ) );
				result = pNotificationManager->RegisterTask( 
					pTaskCallback, 
					IFXNotificationType_System, 
					IFXNotificationId_UnknownBlockSkipped, 
					this, 
					NULL, 
					&taskHandle );
				IFXASSERT( IFXSUCCESS( result ) );

				IFXRELEASE( pNotificationManager );
			}
		}
	}

	// Set up pointers to IFX scene resources and add references that
	// indicate our use of them.
	if ( pSGUtils && IFXSUCCESS(result) )
	{
		IFXASSERT(NULL==m_pCoreServices);
		m_pCoreServices = pSGUtils->m_pCoreServices;
		m_pCoreServices->AddRef();

		m_pGeneratorPalette = pSGUtils->m_pGeneratorPalette;
		m_pGeneratorPalette->AddRef();

		m_pNodePalette = pSGUtils->m_pNodePalette;
		m_pNodePalette->AddRef();

		if (NULL==m_pSceneGraph) {
			m_pSceneGraph = pSGUtils->m_pSceneGraph;
			m_pSceneGraph->AddRef();
		}

		//LOAD FILE
		if( uLoad )
		{
			try {
				pSGUtils->LoadU3DFileInit(m_strU3DFilename, m_bLoadingAtomic, RemoteLoading);
				m_AppName.Concatenate(m_strU3DFilename);
				SetWindowText((HWND)m_ViewerWindow.GetWindowPtr(), (LPCTSTR)m_AppName);
			}
			catch(IFXException& e) {
				result = e.GetIFXResult();
				IFXRELEASE(m_pCoreServices);
				IFXRELEASE(m_pGeneratorPalette);
				IFXRELEASE(m_pNodePalette);
				IFXRELEASE(m_pSceneGraph);

				IFXDELETE(m_pSGUtils);
				m_uActiveViewInstance = 0;
			}
			catch ( ... )
			{
				result = IFX_E_UNDEFINED;
			}
		}
	}

	if ( IFXSUCCESS(result) ) {
		CreateProgressBar();
		m_pSGUtils = pSGUtils;
	}

	//Initialize the Renderer.
	if ( IFXSUCCESS(result) && !m_bRendererInitialized ) {
		result = InitializeRenderer();
	}
	m_uViewCounter = 0;

	return result;
}

/**
	Set up the viewer window for rendering in accordance with
	the user's preferences.

	@return IFXRESULT         The return of this status.
*/
IFXRESULT U3DViewer::SetupWindows()
{
	IFXRESULT result = IFX_OK;
	HWND    hWnd = NULL;
	ATOM    wndResult = NULL;
	RECT rc;

	WNDCLASS wc = { 0, (WNDPROC)SGWndProc, 0, 0, (HINSTANCE)m_hInstance,
		LoadIcon( (HINSTANCE)m_hInstance, MAKEINTRESOURCE(IDI_ICON1) ),
		LoadCursor( NULL, IDC_ARROW ),
		(HBRUSH)GetStockObject(WHITE_BRUSH),
		NULL, WINCLASSNAME };

	// Create the master viewer window.
	if ( IFXSUCCESS(result) )
	{
		wndResult = RegisterClass( &wc );

		if ( !wndResult )
		{
			result = IFX_E_UNDEFINED;
		}
		m_dwWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
			WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_VISIBLE;
	}

	m_hMenu = LoadMenu( (HINSTANCE)m_hInstance, MAKEINTRESOURCE(IDR_MENU1) );

	if (IFXSUCCESS(result))
	{
		SetRect(&rc, 0, 0, U3D_DEFAULT_WIN_WIDTH, U3D_DEFAULT_WIN_HEIGHT);
		AdjustWindowRect(&rc, m_dwWindowStyle, TRUE);

		hWnd = CreateWindow( WINCLASSNAME, APPNAME,
			m_dwWindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, (rc.right-rc.left), (rc.bottom-rc.top),
			NULL, m_hMenu, (HINSTANCE)m_hInstance, NULL );

		if ( !hWnd )
			result = IFX_E_INVALID_HANDLE;
	}
	m_dwWindowStyle = GetWindowLong((HWND)m_ViewerWindow.GetWindowPtr(), GWL_STYLE);

	// If all is OK, show the window
	if ( IFXSUCCESS(result) )
	{
		ShowWindow( hWnd, SW_SHOW );
		UpdateWindow(hWnd);
		//m_ViewerWindow = hWnd;
		m_ViewerWindow.SetWindowPtr(hWnd);
	}

	//
	//Create Status Bar window
	//
	U32 OneCellWidth = (rc.right-rc.left-SB_FRAMERATE_WIDTH-SB_PROGRESSBAR_WIDTH)/3;
	int statwidths[5] = {SB_PROGRESSBAR_WIDTH, 
		SB_PROGRESSBAR_WIDTH+OneCellWidth, 
		SB_PROGRESSBAR_WIDTH+OneCellWidth*2, 
		SB_PROGRESSBAR_WIDTH+OneCellWidth*3, 
		-1}; //Define the size of each status bar part
	//Dynamically Create the Statusbar
	m_hStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL,
		WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | WS_BORDER,
		0, 0, 0, 0,
		(HWND)m_ViewerWindow.GetWindowPtr(),
		NULL, GetModuleHandle(NULL), NULL);
	//Set the size of the statusbar's parts
	SendMessage(m_hStatusBar, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);

	if ( IFXSUCCESS(result) && m_hMenu)
		result = CreateToolBar();

	if ( IFXSUCCESS(result) ) {
		SetMenuStates();
	}

	return result;
}

IFXRESULT U3DViewer::CreateToolBar()
{
	IFXRESULT result = IFX_OK;

	// Toolbar buttons
	TBBUTTON tbButtonsCreate [ ] = 
	{
		{STD_FILEOPEN, IDM_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 
#if defined(_WIN32) | defined(_WIN64)
		{0},
#endif
		0L, 0},
	};
	TBBUTTON tbButtonsAdd [ ] =
	{
		{0, IDM_LOADTYPE, TBSTATE_ENABLED | (m_bLoadingAtomic?TBSTATE_CHECKED:0x00), TBSTYLE_CHECK, 
#if defined(_WIN32) | defined(_WIN64)
		{0},
#endif
		0L, 0},
		{0,            0,        TBSTATE_ENABLED, TBSTYLE_SEP, 
#if defined(_WIN32) | defined(_WIN64)
		{0},
#endif
		0L, 0},
	};

	m_hToolBar = CreateToolbarEx ((HWND)m_ViewerWindow.GetWindowPtr(), 
		WS_CHILD | 
		WS_BORDER | 
		WS_VISIBLE | 
		0, 
		ID_TOOLBAR, 0, HINST_COMMCTRL,
		IDB_STD_SMALL_COLOR, tbButtonsCreate, IFXARRAY_SIZE(tbButtonsCreate), 
		0, 0, 100, 30, 
		sizeof (TBBUTTON));
	if (NULL == m_hToolBar)
		result = IFX_E_INVALID_HANDLE;

	U32 stdidx;
	if (IFXSUCCESS(result))
	{
		TBADDBITMAP tb = {0};
		tb.hInst = GetModuleHandle(NULL);
		tb.nID = IDB_TOOLBITMAP;
		stdidx = SendMessage (m_hToolBar, TB_ADDBITMAP, 1, (LPARAM)&tb);
		tbButtonsAdd[0].iBitmap += stdidx;
		if (FALSE == SendMessage(m_hToolBar, TB_ADDBUTTONS, IFXARRAY_SIZE(tbButtonsAdd), (LPARAM)&tbButtonsAdd))
			result = IFX_E_NOT_DONE;
	}

	SIZE tbSize;
	if (IFXSUCCESS(result))
		if (0 == SendMessage(m_hToolBar, TB_GETMAXSIZE, 0, (LPARAM)&tbSize))
			result = IFX_E_NOT_DONE;
	
	U32 cxOffset = tbSize.cx;
	if (IFXSUCCESS(result))
	{
		m_hCLODLevelEdit = CreateWindowEx(0L, L"Edit", NULL, 
			WS_CHILD | 
			WS_DISABLED |
			WS_BORDER | 
			WS_VISIBLE | 
			ES_RIGHT | 
			ES_NUMBER |
			0, cxOffset, 2, CXEDIT, CYEDIT, 
			(HWND)m_ViewerWindow.GetWindowPtr(), (HMENU)IDM_CLODLEVELEDIT, GetModuleHandle(NULL), 0 );
		if (NULL == m_hCLODLevelEdit)
			result = IFX_E_INVALID_HANDLE;
		cxOffset += CXEDIT;
	}
	if (IFXSUCCESS(result))
	{
		SetParent(m_hCLODLevelEdit, m_hToolBar);
		SetDlgItemInt(m_hToolBar, IDM_CLODLEVELEDIT, 0, FALSE);
		HWND hCLODLevel = CreateUpDownControl(
			WS_CHILD | 
			WS_VISIBLE |
			UDS_SETBUDDYINT |
			0, cxOffset, 2, CXUPDOWN, CYEDIT,
			m_hToolBar,	ID_CLODUPDOWN, GetModuleHandle(NULL), m_hCLODLevelEdit,
			100, 0,	0);
		cxOffset += CXUPDOWN;
	}

	if (IFXSUCCESS(result))
	{
		m_hFPSEdit = CreateWindowEx(0L, L"Edit", NULL, 
			WS_CHILD | 
			WS_DISABLED |
			WS_BORDER | 
			WS_VISIBLE | 
			ES_RIGHT | 
			ES_NUMBER |
			0, cxOffset+16, 2, CXEDIT, CYEDIT, 
			(HWND)m_ViewerWindow.GetWindowPtr(), (HMENU)IDM_FPSEDIT, GetModuleHandle(NULL), 0 );
		if (NULL == m_hFPSEdit)
			result = IFX_E_INVALID_HANDLE;
		cxOffset += CXEDIT+16;
	}
	if (IFXSUCCESS(result))
	{
		SetParent(m_hFPSEdit, m_hToolBar);
		SetDlgItemInt(m_hToolBar, IDM_FPSEDIT, 0, FALSE);
		HWND hCLODLevel = CreateUpDownControl(
			WS_CHILD | 
			WS_VISIBLE |
			UDS_SETBUDDYINT |
			0, cxOffset, 2, CXUPDOWN, CYEDIT,
			m_hToolBar,	ID_FPSUPDOWN, GetModuleHandle(NULL), m_hFPSEdit,
			100, 0,	0);
		cxOffset += CXUPDOWN;
	}

	return result;
}      

void U3DViewer::CreateProgressBar()
{
	if (!m_hProgress)
		m_hProgress = CreateWindowEx(0, PROGRESS_CLASS, NULL,
		WS_CHILD | WS_VISIBLE,
		1, 3, SB_PROGRESSBAR_WIDTH, 17,
		m_hStatusBar,
		NULL, GetModuleHandle(NULL), NULL);
	SendMessage(m_hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
}

void U3DViewer::DestroyProgressBar()
{
	DestroyWindow(m_hProgress);
	m_hProgress = NULL;
}


IFXRESULT U3DViewer::SetRenderer(U3DRenderer Renderer)
{
	IFXRESULT result = IFX_OK;

	if (NULL != m_pRenderServices && NULL != m_pDevice) {
		if (m_pRenderServices->GetSupported(Renderer))
			m_uRendererID = Renderer;
		else
			if (m_pRenderServices->GetSupported(U3D_DEFAULT_RENDERER))
				m_uRendererID = U3D_DEFAULT_RENDERER;
			else
				result = IFX_E_UNSUPPORTED;

		if ( IFXSUCCESS(result) ) {
			result = m_pDevice->SetRenderer( m_uRendererID );
			if (m_pITimer) {
				m_pITimer->StopTimer(1);
				m_pITimer->StartTimer(1);
				m_pITimer->GetElapsed(0, StartTime);
			}
			m_Frames = 0;
		}

		if (m_ViewerWindow.GetWindowPtr() != NULL ) {
			IFXCHECKX(SetWindowSize());
			IFXCHECKX(SetViewport());
			m_ZeroWindow = FALSE;
		}

	} else {
		result = IFX_E_NOT_INITIALIZED;
	}

	return result;
}


/**
	Sets up to render 3D scene representation

	@return IFXRESULT     The return status of this method.
*/
IFXRESULT U3DViewer::InitializeRenderer( )
{
	IFXRESULT result = IFX_OK;
	U32     ViewID = NULL;

	if (!m_pRenderServices) {
		// Initialize the renderer services
		if ( IFXSUCCESS(result) )
		{
			result = IFXCreateComponent(  CID_IFXRenderServices,
				IID_IFXRenderServices,
				(void**)&m_pRenderServices);
		}

		if ( m_pRenderServices )
		{
			if ( IFXSUCCESS(result) )
			{
				result = m_pRenderServices->RegisterRenderer(
					U3D_RENDERER_NULL,
					CID_IFXRenderNULL,
					CID_IFXRenderDeviceNULL );
			}
			if ( IFXSUCCESS(result) )
				result = m_pRenderServices->RegisterRenderer(
					U3D_RENDERER_OPENGL,
					CID_IFXRenderOGL,
					CID_IFXRenderDeviceOGL );

			if ( IFXSUCCESS(result) )
			{
				result = m_pRenderServices->RegisterRenderer(
					U3D_RENDERER_DIRECTX7,
					CID_IFXRenderDX7,
					CID_IFXRenderDeviceDX7 );
			}

			if ( IFXSUCCESS(result) )
			{
				result = m_pRenderServices->RegisterRenderer(
					U3D_RENDERER_DIRECTX8,
					CID_IFXRenderDX8,
					CID_IFXRenderDeviceDX8 );
			}
		}
	}

	// if there is no timer, create one and start it
	if (!m_pITimer)
		result = IFXCreateComponent (CID_IFXPerformanceTimer, IID_IFXPerformanceTimer,
		(void**) &m_pITimer);

	// if there is no device, create one and initialize it
	/// @todo: What happens if there is already a device?  Is there any setup that need to be repeated?
	if (!m_pDevice)
	{
		result = IFXCreateComponent( CID_IFXDevice, IID_IFXDevice, (void**)&m_pDevice );

		if( m_pDevice && IFXSUCCESS(result) )
		{
			result = m_pDevice->Initialize( );

			// Set the renderer to use
			if ( IFXSUCCESS(result) )
			{
				result = SetRenderer(m_uRendererID);
			}
			if ( IFXSUCCESS(result) )
			{
				result = SetWindowSize();
			}
			if ( IFXSUCCESS(result) )
			{
				m_ZeroWindow = FALSE;
			}

			// Get initial state of UseTargetFrameRate flag and TargetFrameRateValue
			// and set appropriate Edit control
			if ( IFXSUCCESS(result) )
				result = m_pDevice->GetUseTargetFramerate(&m_UseTargetFramerate);
			if ( IFXSUCCESS(result) )
				result = m_pDevice->GetTargetFramerate(&m_TargetFrameRate);
			if ( IFXSUCCESS(result) )
			{
				char buffer[10];
				IFXCHAR framerate[10];
				_gcvt( m_TargetFrameRate, 6, buffer );
				U32 s = mbstowcs(NULL, buffer, 0);
				if (NULL != framerate) {
					mbstowcs(framerate, buffer, s);
					framerate[s]=L'\0';
					SendMessage(m_hFPSEdit, WM_SETTEXT, 0, (LPARAM)framerate);
				}
			}
		}
	}

	m_bRendererInitialized = TRUE;

	return result;
}

IFXRESULT U3DViewer::SetWindowSize()
{
	IFXRESULT result = IFX_OK;
	RECT rcWindowClient, 
		rcStatusBar, 
		rcToolBar;
	int   x, 
		y, 
		cx, 
		cy;
		
	HWND hViewerWindow = (HWND)m_ViewerWindow.GetWindowPtr();

	if (hViewerWindow && m_hStatusBar) {
		GetClientRect(hViewerWindow, &rcWindowClient);
		GetClientRect(m_hStatusBar, &rcStatusBar);

		SetWindowPos(m_hStatusBar, 0, rcWindowClient.left,
			rcWindowClient.bottom - rcStatusBar.bottom,
			rcWindowClient.right,
			rcStatusBar.bottom, SWP_NOZORDER);
		ShowWindow(m_hStatusBar,  SW_SHOW);

		//resize ToolBar
		if (m_hToolBar) {
			GetWindowRect(GetDlgItem(hViewerWindow, ID_TOOLBAR), &rcToolBar);
			x = 0;
			y = 0;
			cx = rcWindowClient.right - rcWindowClient.left;
			cy = rcWindowClient.bottom - rcWindowClient.top;
			MoveWindow( GetDlgItem(hViewerWindow, ID_TOOLBAR), x, y, cx, cy, TRUE);
			rcWindowClient.top += rcToolBar.bottom - rcToolBar.top;
		}

		U32 OneCellWidth = (rcWindowClient.right-rcWindowClient.left-SB_FRAMERATE_WIDTH-SB_PROGRESSBAR_WIDTH)/3;
		int statwidths[5] = {SB_PROGRESSBAR_WIDTH, 
			SB_PROGRESSBAR_WIDTH+OneCellWidth, 
			SB_PROGRESSBAR_WIDTH+OneCellWidth*2, 
			SB_PROGRESSBAR_WIDTH+OneCellWidth*3, 
			-1}; //Define the size of each status bar part
		//Set the size of the statusbar's parts
		SendMessage(m_hStatusBar, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);

		rcWindowClient.bottom -= rcStatusBar.bottom;
		//m_pClientRect.Set( rcWindowClient.left, rcWindowClient.top, rcWindowClient.right - rcWindowClient.left,
		//	rcWindowClient.bottom - rcWindowClient.top );
	} else
		result = IFX_E_INVALID_POINTER;

	if (rcWindowClient.right - rcWindowClient.left > 0 && rcWindowClient.bottom - rcWindowClient.top > 0)
	{
		if (IFXSUCCESS(result))
			result = m_ViewerWindow.SetWindowSize(IFXRect(rcWindowClient.left, 
			rcWindowClient.top, 
			rcWindowClient.right - rcWindowClient.left,
			rcWindowClient.bottom - rcWindowClient.top ));
		if (IFXSUCCESS(result))
			m_ZeroWindow = FALSE;

		if (IFXSUCCESS(result) && NULL != m_pDevice)
			m_pDevice->SetWindow(m_ViewerWindow);
	}

	return result;
}

IFXRESULT U3DViewer::FindView()
{
	IFXRESULT result = IFX_OK;
	U32	LastViewID, 
		FirstViewID;
	U32 uViewCounter = m_uViewCounter;

#if(WINVER >= 0x0500)
	MENUITEMINFO mii;
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING | MIIM_ID;
	mii.fType = MFT_STRING;
	mii.fState = MFS_ENABLED;
#endif
	if (m_pNodePalette) 
	{
		if (IFXSUCCESS(result))
			result = m_pNodePalette->Last(&LastViewID);

		if (m_startToFindCameras && IFXSUCCESS(result))
		{
			result = m_pNodePalette->First(&FirstViewID);
			m_ViewID = FirstViewID;
			m_startToFindCameras = FALSE;
		}

		// Get the view resource associated with this ID.
		if (IFXSUCCESS(result)) 
		{
			do 
			{
				if (!m_startToFindCameras)
					result = m_pNodePalette->Next(&m_ViewID);

				IFXDECLARELOCAL(IFXView,pView);

				if (IFXSUCCESS(result))
				{
					result = m_pNodePalette->GetResourcePtr( m_ViewID, IID_IFXView, (void**) &pView );
				}

				if (IFXSUCCESS(result)) 
				{
					m_pViewIndeces[m_uViewCounter] = m_ViewID;
#if(WINVER >= 0x0500)
					IFXString nodeName;
					result = m_pNodePalette->GetName(m_ViewID, &nodeName);
					mii.wID = THE_LAST_MENU_ID + m_uViewCounter;
					mii.dwTypeData = (LPWSTR)nodeName.Raw();
					InsertMenuItem(m_hMenu, ID_OPTIONS_NEXTVIEW, FALSE, &mii);
#endif
					m_uViewCounter++;
				}
			} 
			while (m_ViewID != LastViewID);

			if (uViewCounter == m_uViewCounter)
				result = IFX_E_CANNOT_FIND;

			if (m_uActiveViewIndex >= m_uViewCounter) 
			{
				m_uActiveViewIndex = 0;
				m_uActiveViewInstance = 0;
			}
		}
	} 
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

/**
This function clears the back buffer so that garbage pixels are not 
drawn to the screen when the used viewports do not cover the entire 
surface.

@note	For optimal performance, the device's back buffer is not 
automatically cleared when rendering.  Currently, there is no 
direct way to explictly clear the back buffer.  Normally, 
viewport(s) cover the entire renderable surface and since they 
can clear the portions used, this functionality is generally not 
necessary.  However, if the full area is not covered, the 
ability to do this is needed.  That's why this function is 
included and used.

@note	This function clears the back buffer using the U3D API by 
setting the viewport to be the entire window and rendering the scene.
Rather than creating a View Resource and View Node for this purpose,
it uses the current view and only performs viewport change temporarily.

@todo	Add a new U3D API function that exposes a more straight forward 
technique to clear the back buffer.
*/
IFXRESULT U3DViewer::ClearBackBuffer()
{
	const F32 ENTIRE_DEVICE = 1.0f;
	IFXRESULT result = IFX_OK;

	if (!(m_pDevice && m_pNodePalette)) 
		result = IFX_E_NOT_INITIALIZED;

	IFXF32Rect	viewRect;
	IFXDECLARELOCAL( IFXView, pView );

	U32 viewId = m_pViewIndeces[m_uActiveViewIndex];
	if (IFXSUCCESS(result))
		result = m_pNodePalette->GetResourcePtr( viewId, IID_IFXView, (void**) &pView );

	//Get the current values so they can be restored before exiting.
	if (IFXSUCCESS(result))
		result = pView->GetViewport(viewRect);

	//set the view to use entire device and set
	//far clip in front of near clip
	if (IFXSUCCESS(result))
	{
		IFXF32Rect	clearRect( 0.0f, 0.0f, ENTIRE_DEVICE, ENTIRE_DEVICE );

		result = pView->SetViewport(clearRect);
	}

	//render the scene (same call as in the main render loop
	//only with the viewport changes).
	if (IFXSUCCESS(result))		
		result = m_pDevice->Render();

	//restore the values to the view
	if (IFXSUCCESS(result))
		result = pView->SetViewport(viewRect);

	return result;
}

IFXRESULT U3DViewer::SetViewport()
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL(IFXSpatialSetQuery, pCullingSubsystem);

	IFXRect clientRect = m_ViewerWindow.GetWindowSize();
	if(TRUE == m_bRendererInitialized &&
		clientRect.m_Height > 0 &&
		clientRect.m_Width > 0 /*&&
		m_bRenderIt*/) 
	{
		if (m_pNodePalette && m_pDevice)
		{
			IFXDECLARELOCAL( IFXView, pView );

			U32 viewId = m_pViewIndeces[m_uActiveViewIndex];
			result = m_pNodePalette->GetResourcePtr( viewId, IID_IFXView, (void**) &pView );

			// Point the device to this view
			if (IFXSUCCESS(result))
			{
				IFXRESULT temporaryResult	= IFX_OK;

				// First remove all previous views associated with the device.
				while ( IFXSUCCESS( temporaryResult ) )
					temporaryResult = m_pDevice->RemoveLastView();

				// Then associate this view with the device.
				result = m_pDevice->AppendView( pView, m_uActiveViewInstance );
			}

			if (IFXSUCCESS(result))
			{
				if (m_uCullerID == U3D_SCENEGRAPH_CULLER)
					result = IFXCreateComponent(CID_IFXSceneGraphCuller, IID_IFXSpatialSetQuery,(void**)&pCullingSubsystem);
				else
					result = IFXCreateComponent(CID_IFXOctreeCuller, IID_IFXSpatialSetQuery,(void**)&pCullingSubsystem);
			}

			if (IFXSUCCESS(result))
				result = pView->SetCullingSubsystem(*pCullingSubsystem);

			if (IFXSUCCESS(result) && !m_bPreserveViewSettings)
			{
				IFXF32Rect ViewRect;
				ViewRect.Set(0, 0, 1, 1);
				result = pView->SetViewport(ViewRect);
				pView->SetAttributes(pView->GetAttributes()|1);
			}

			// Whenever the device's window and viewport are changed, do a one 
			// time clearing of the entire window.  This is only performed just 
			// in case the viewport is modified such that it does not occupy the 
			// entire drawable area of the device's window so that no visual 
			// garbage remains from previous rendered frames.  If your 
			// application will always have viewport(s) covering the entire 
			// window, then it is not necessary to perform the logic in the 
			// ClearWindow function.
			if(IFXSUCCESS(result))
				result = ClearBackBuffer();
		}
		else
			result = IFX_E_INVALID_POINTER;
	}

	return result;
}

/**
	Render frames until the user decides to quit or some other
	terminal event occurs.

	@return IFXRESULT     The return status of this method.
*/
IFXRESULT U3DViewer::EventLoop()
{
	IFXRESULT   result = IFX_OK;
	IFXTaskHandle idleHandle = 0;
	IFXTaskHandle statusHandle = 0;
	HWND hViewerWindow = (HWND)m_ViewerWindow.GetWindowPtr();

	// Process messages from the window
	if (IFXSUCCESS(result) )
	{
		MSG       message;
		BOOL      bGotMsg = FALSE;
		BOOL      bJobExecuted = FALSE;

		bGotMsg = PeekMessage( &message, NULL, 0U, 0U, PM_REMOVE );

		// Free rendering mode - run until the user pushes cancel
		while ( WM_QUIT != message.message )
		{
			bGotMsg = PeekMessage( &message, NULL, 0U, 0U, PM_REMOVE );

			if ( bGotMsg )
			{
				TranslateMessage( &message );
				DispatchMessage( &message ); // dispatch message to window procedure
			}
			else
			{
				result = IFX_OK;
				// Render a frame during idle time (no messages are waiting)
				if ( NULL == m_pSGUtils )
					WaitMessage();
				else
				{
					try
					{
						U32 numParents = 0;
						IFXString NodeName, NumParentsChar, OutString, ParentIndex;

						m_pSGUtils->ProcessScheduler();

						///	@todo	Improve sample code to better demonstrate view handling logic for this whole module.
						if (m_bFindView) 
						{
							result = FindView();

							// Create a view node and view resource to use 
							// for rendering if the file loaded does not 
							// contain any.
							if ( IFX_E_CANNOT_FIND == result && m_pSGUtils->IsU3DFileLoaded() )
							{
								SGNodeHeader	nodeHeader;

								wcscpy( nodeHeader.Name, _GENERATED_VIEW_NAME_IF_NONE_AVAILABLE );
								wcscpy( nodeHeader.ParentName, PALETTE_DEFAULT_GROUP );
								nodeHeader.NodeTm.MakeIdentity();
								nodeHeader.NumberOfParents	= 0;

								IFXDECLARELOCAL( IFXView, pViewNode );

								result = m_pSGUtils->CreateCameraNode( &nodeHeader, &pViewNode, FALSE );

								IFXRELEASE( pViewNode );

								// Get view information now that there's one.
								///	@todo	Return more data from CreateCameraNode so a call to FindView isn't needed.
								if ( IFXSUCCESS( result ) )
									result = FindView();
							}

							// If an actual view node is available now (for 
							// either atomic or non-atomic loading mode), 
							// setup our rendering device and viewport size 
							// if necessary, display the view node name in 
							// the status bar, and indicate that rendering 
							// can begin.  Note that this may be true even 
							// if the result code is IFX_E_CANNOT_FIND.
							if ( IFX_E_CANNOT_FIND != result )
							{
								IFXDECLARELOCAL( IFXView, pView );

								U32 viewId = m_pViewIndeces[m_uActiveViewIndex];
								result = m_pNodePalette->GetResourcePtr( viewId, IID_IFXView, (void**) &pView );

								result = SetViewport();

								numParents = pView->GetNumberOfParents();
								IFXCHECKX(NumParentsChar.ToString(numParents, 10));
								IFXCHECKX(ParentIndex.ToString(m_uActiveViewInstance, 10));
								IFXCHECKX(m_pNodePalette->GetName(viewId, &NodeName));
								OutString = NodeName + L" " + ParentIndex + L" (" + NumParentsChar + L")";
								SendMessage(m_hStatusBar, SB_SETTEXT, 2, (LPARAM)OutString.Raw());

								m_bRenderIt = TRUE;

								SendMessage(hViewerWindow, WM_SIZE, 0, 0);
							}

							if (IFX_E_CANNOT_FIND == result)
								result = IFX_OK;
						}

						if (!m_pSGUtils->IsU3DFileLoaded() && IFXSUCCESS(result) )
						{
							SendMessage(m_hProgress, PBM_SETPOS, (WPARAM)(int)(m_pSGUtils->LoadingProgress()), 0);
							SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)L"To cancel loading press Esc");
						}
						else 
						{
							if (IFXSUCCESS(result))
								m_bFindView = FALSE;
							if (m_hProgress && IFXSUCCESS(result))
							{
								DestroyProgressBar();
								SendMessage(m_hStatusBar, SB_SETTEXT, 1, (LPARAM)L"");

								if (m_pSGUtils->IsU3DFileLoaded() && !m_pSGUtils->IsLoadingCanceled())
								{
									SendMessage(m_hStatusBar, SB_SETTEXT, 0, (LPARAM)L"Ready");

									if (IFXSUCCESS(result)) 
									{
										result = m_pSGUtils->KeepTextureDecompressed();

										if (IFXSUCCESS(result) && m_bRenderIt)
											result = SetViewport();

										if (IFXSUCCESS(result) && m_bRenderIt) 
										{
											IFXDECLARELOCAL( IFXView, pView );

											U32 viewId = m_pViewIndeces[m_uActiveViewIndex];
											result = m_pNodePalette->GetResourcePtr( viewId, IID_IFXView, (void**) &pView );

											numParents = pView->GetNumberOfParents();

											IFXCHECKX(NumParentsChar.ToString(numParents, 10));
											IFXCHECKX(ParentIndex.ToString(m_uActiveViewInstance, 10));
											IFXCHECKX(m_pNodePalette->GetName(viewId, &NodeName));
											OutString = NodeName + L" " + ParentIndex + L" (" + NumParentsChar + L")";
											SendMessage(m_hStatusBar, SB_SETTEXT, 2, (LPARAM)OutString.Raw());
										}
									}
								}
							}
						}

						// All set to go - render!!!
						if ( m_pDevice && IFXSUCCESS(result) && m_bRenderIt && !m_ZeroWindow)
						{
							result = m_pDevice->Render();

							if (result == IFX_E_DX7SURFACELOST)
							{
								//This error appears when system came to Lock mode (Ctrl-Alt-Del)
								MessageBox(
									hViewerWindow,
									L"This is the DX7 specific failure.\n" 
									L"Lock method returned with D3D_OK \n" 
									L"but pointer to the VertexBuffer is Zero.\n" 
									L"\nThe application will now exit.\n",
									APPNAME,
									MB_ICONWARNING | MB_OK );
								PostMessage( hViewerWindow, WM_CLOSE, 0, 0 );
							}

							if(m_pITimer)
							{
								IFXCHAR buffer[349];
								F32 FrameRate;
								U32 currentTime;
								F32 NewTime;

								static F32 LastTime = 0.0f;

								m_pITimer->GetElapsed(0, currentTime);
								NewTime = (F32)currentTime/1000;

								++m_Frames;

								if ( ( NewTime - LastTime ) > _MAXIMUM_ELAPSED_TIME_MS )
								{
									FrameRate = m_Frames / (NewTime - LastTime);
									LastTime = NewTime;
									m_Frames  = 0;

									_itow( (U32)FrameRate, buffer, 10);
									wcscat(buffer, L" FPS");
									SendMessage(m_hStatusBar, SB_SETTEXT, 4, (LPARAM)buffer);
								}

								currentTime -= StartTime;
								if (m_TimeOut)
								{
									_itow((m_TimeOut * 1000 - currentTime)/1000, buffer, 10);
									SendMessage(m_hStatusBar, SB_SETTEXT, 3, (LPARAM)buffer);
									if (currentTime >= m_TimeOut * 1000)
										PostQuitMessage(0);
								}
							}
							IFXASSERT( IFXSUCCESS( result ) );
						}
					}
					catch (IFXException& e)
					{
						result = e.GetIFXResult();

						IFXCHAR	pMessage[ _MAXIMUM_STRING_LENGTH ];

						swprintf(
							pMessage,
							L"Received an exception with error return code "
							L"0x%08lX and description\n\"%ls\"\n" 
							L"The application will now exit.\n",
							result,
							e.GetText().Raw() );

						MessageBox( 
							hViewerWindow, 
							pMessage,
							APPNAME, 
							MB_ICONWARNING | MB_OK );

						PostMessage( hViewerWindow, WM_CLOSE, 0, 0 );
					}
					catch( ... )
					{
						MessageBox(
							hViewerWindow,
							L"Received an unknown exception.  The application\n" 
							L"will now exit.\n",
							APPNAME,
							MB_ICONWARNING | MB_OK );

						result = IFX_E_UNDEFINED;

						PostMessage( hViewerWindow, WM_CLOSE, 0, 0 );
					}
				}
			}
		}
	}

	return result;
}

//------------------------------------------------------------------
IFXRESULT U3DViewer::HandleNotifications( 
	IFXTaskData*	pTaskData )
{
	if ( pTaskData )
	{
		IFXDECLARELOCAL( IFXNotificationInfo, pInfo );

		// Obtain a reference to the full notification task data to access 
		// it and unsure it is associated with a notification event.
		if ( IFXSUCCESS( pTaskData->QueryInterface( 
			IID_IFXNotificationInfo, 
			( void** ) &pInfo ) ) )
		{
			IFXRESULT			result;
			IFXNotificationType	type;

			// Get the notification type.
			result = pInfo->GetType( &type );
			IFXASSERT( IFXSUCCESS( result ) );

			IFXNotificationId	id;

			// Get the notification id.
			result = pInfo->GetId( &id );
			IFXASSERT( IFXSUCCESS( result ) );

			U3DViewer	*pViewer	= NULL;

			// Get the user data value that was specified when the task 
			// callback was registered.  It's being used as a pointer to 
			// our U3DViewer instance.

			result = pInfo->GetUserData( ( void** ) &pViewer );
			IFXASSERT( IFXSUCCESS( result ) );

			// Handle recognized notification events.
			if ( type == IFXNotificationType_Error )
			{
				IFXTaskHandle	hErrorTask;

				// Get the handle of the failing task that resulted in this 
				// notification.
				result = pInfo->GetId( ( IFXNotificationId* ) &hErrorTask );
				IFXASSERT( IFXSUCCESS( result ) );

				IFXRESULT	errorResult;

				// Get the error result code returned by the task.
				result = pInfo->GetKeyFilter( ( U32* ) &errorResult );
				IFXASSERT( IFXSUCCESS( result ) );

				IFXDECLARELOCAL( IFXUnknown, pErrorUnknown );

				// Get a reference to the task.
				result = pInfo->GetObjectFilter( &pErrorUnknown );
				IFXASSERT( IFXSUCCESS( result ) );

				// Provide a warning message to the user.

				IFXCHAR	pMessage[ _MAXIMUM_STRING_LENGTH ];

				swprintf(
					pMessage,
					L"Task with handle %ld and IFXUnknown pointer\n" 
					L"0x%08lX failed execution with result\n"
					L"code 0x%08lX.\n", 
					hErrorTask,
					pErrorUnknown,
					errorResult );

				MessageBox( 
					(HWND)pViewer->m_ViewerWindow.GetWindowPtr(),
					pMessage,
					APPNAME, 
					MB_ICONWARNING | MB_OK );

				IFXRELEASE( pErrorUnknown );
			}
			else if ( type == IFXNotificationType_System && 
				id == IFXNotificationId_UnknownBlockSkipped )
			{
				U32	blockType	= 0;

				// Get the block type that resulted in this notification.
				result = pInfo->GetNotificationData( ( void** ) &blockType );
				IFXASSERT( IFXSUCCESS( result ) );

				U32	loadId	= 0;

				// Get the associated load id.
				result = pInfo->GetKeyFilter( &loadId );
				IFXASSERT( IFXSUCCESS( result ) );

				// Provide a warning message to the user.

				IFXCHAR	pMessage[ _MAXIMUM_STRING_LENGTH ];

#define _QUIET_UNKNOWN_BLOCK_SKIPPED_ANNOUNCEMENT
#if defined( _QUIET_UNKNOWN_BLOCK_SKIPPED_ANNOUNCEMENT )
				swprintf(
					pMessage,
					L"Unknown block 0x%08lX skipped", 
					blockType );

				SendMessage( 
					pViewer->m_hStatusBar, 
					SB_SETTEXT, 
					0, 
					LPARAM( pMessage ) );
#else
				swprintf(
					pMessage,
					L"An unrecognized block in the file was\n" 
					L"encountered with type 0x%08lX.\n" 
					L"The load id associated with this file is %ld.\n",
					blockType,
					loadId );

				MessageBox( 
					pViewer->hViewerWindow, 
					pMessage,
					APPNAME, 
					MB_ICONWARNING | MB_OK );
#endif
			}

			IFXRELEASE( pInfo );
		}
		// Note:  No pViewer clean-up is needed.
	}

	// Always return success since we're just monitoring notifications.
	return IFX_OK;
}
