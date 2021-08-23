//***************************************************************************
//
//  Copyright (c) 2004 - 2006 Intel Corporation
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

#include "afxwin.h"
#include <process.h>

using namespace std;

#define IDC_NEWTEST 100
#define IDC_CATSUITE 101
#define IDC_MYLIST 103
#define IDC_DIR 200
#define IDC_TIMEOUT 201
#define IDC_EXEC 202
#define IDC_NEWCAT 203
#define IDC_ADDNEWCAT 203
#define IDC_OPEN 204
#define IDC_DOWNL 205
#define IDC_UPL 206
#define IDC_CONSOLE 207
#define IDC_WINDOW 208

#define MAXLINE 1024
#define MAXCAT 100

class CMainWnd;

//console/window subsystem radiobutton
class CSubSys: public CButton
{
public:
  void SetButtonParent( CMainWnd* p ) { m_pParent = p; }
private:
  CMainWnd* m_pParent;
};

//upload suite on the BM
class CUpl: public CButton
{
public:
  afx_msg void OnLButtonDown(UINT, CPoint);
  void SetButtonParent( CMainWnd* p ) { m_pParent = p; }
private:
  DECLARE_MESSAGE_MAP();
  CMainWnd* m_pParent;
};

BEGIN_MESSAGE_MAP(CUpl, CButton)
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//download all tests from the BM
class CDownl: public CButton
{
public:
  afx_msg void OnLButtonDown(UINT, CPoint);
  void SetButtonParent( CMainWnd* p ) { m_pParent = p; }
private:
  DECLARE_MESSAGE_MAP();
  CMainWnd* m_pParent;
};

BEGIN_MESSAGE_MAP(CDownl, CButton)
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//edit downloaded file in Notepad
class COpen: public CButton
{
public:
  afx_msg void OnLButtonDown(UINT, CPoint);
  void SetButtonParent( CMainWnd* p ) { m_pParent = p; }
private:
  DECLARE_MESSAGE_MAP();
  CMainWnd* m_pParent;
};

BEGIN_MESSAGE_MAP(COpen, CButton)
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//create suite for certain category
class CCatSuite: public CButton
{
public:
  afx_msg void OnLButtonDown(UINT, CPoint);
  void SetButtonParent( CMainWnd* p ) { m_pParent = p; }
private:
  DECLARE_MESSAGE_MAP();
  CMainWnd* m_pParent;
};

BEGIN_MESSAGE_MAP(CCatSuite, CButton)
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//add new category
class CNewCat: public CButton
{
public:
  afx_msg void OnLButtonDown(UINT, CPoint);
  void SetButtonParent( CMainWnd* p ) { m_pParent = p; }
private:
  DECLARE_MESSAGE_MAP();
  CMainWnd* m_pParent;
};

BEGIN_MESSAGE_MAP(CNewCat, CButton)
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

//create new test
class CNewTest: public CButton
{
public:
  afx_msg void OnLButtonDown(UINT, CPoint);
  void SetButtonParent( CMainWnd* p ) { m_pParent = p; }
private:
  DECLARE_MESSAGE_MAP();
  CMainWnd* m_pParent;
};

BEGIN_MESSAGE_MAP(CNewTest, CButton)
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


class CMainWnd : public CFrameWnd
{
  friend CNewTest;
  friend CNewCat;
  friend CCatSuite;
  friend COpen;
  friend CDownl;
  friend CUpl;
  friend CSubSys;
public:
  CMainWnd();
  afx_msg int OnCreate( LPCREATESTRUCT );
  afx_msg void OnActivate( UINT, CWnd*, BOOL );
~CMainWnd();
private:
  CStatic* m_pDirL;
  CStatic* m_pTestL;
  CStatic* m_pTimeoutL;
  CStatic* m_pExecL;
  CEdit* m_pDirE;
  CEdit* m_pTestE;
  CEdit* m_pTimeoutE;
  CEdit* m_pExecE;
  CNewTest* m_pNewTest;
  CCatSuite* m_pCatSuite;
  CListBox* m_pCatsList;
  CEdit* m_pNewCat;
  CNewCat* m_pAddNewCat;
  COpen* m_pOpen;
  CDownl* m_pDownl;
  CUpl* m_pUpl;
  CSubSys* m_pSubSysConsole;
  CSubSys* m_pSubSysWindow;
  CEdit* m_pBase;
  char** m_ppCats;
  DECLARE_MESSAGE_MAP();
};

BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
  ON_WM_CREATE()
  ON_WM_ACTIVATE()
END_MESSAGE_MAP()

void run( char* name )
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
  BOOL res = CreateProcess( NULL,name,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi );
  WaitForMultipleObjects(1,&pi.hProcess,TRUE,INFINITE);
}

void CUpl::OnLButtonDown(UINT, CPoint)
{
  char cmdline[MAXLINE];
  char file[MAXLINE];
  char drive[MAXLINE];
  char dir[MAXLINE];
  char name[MAXLINE];
  char ext[MAXLINE];
  strcpy( cmdline, "confout.bat " );
  m_pParent->m_pBase->GetLine( 1, file );
  strcat( cmdline, file );
  strcat( cmdline, " -i " );
  _splitpath( file, drive, dir, name, ext );
  strcat( cmdline, name );
  strcat( cmdline, ext );
  run( cmdline );
}

void CDownl::OnLButtonDown(UINT, CPoint)
{
  char cmdline[MAXLINE];
  char file[MAXLINE];
  strcpy( cmdline, "perl dbin.pl " );
  m_pParent->m_pBase->GetLine( 1, file );
  strcat( cmdline, file );
  run( cmdline );
}

void COpen::OnLButtonDown(UINT, CPoint)
{
  char cmdline[MAXLINE];
  char file[MAXLINE];
  strcpy( cmdline, "notepad " );
  m_pParent->m_pBase->GetLine( 1, file );
  strcat( cmdline, file );
  run( cmdline );
}
void CCatSuite::OnLButtonDown(UINT, CPoint)
{
  char cmdline[MAXLINE];
  int cnt = m_pParent->m_pCatsList->GetSelCount();
    if( cnt != 1 )
  {
    MessageBox( "Please select one category" );
    return;
  }
  int sel[1];
  m_pParent->m_pCatsList->GetSelItems( 1, sel );
  char line[MAXLINE];
  m_pParent->m_pCatsList->GetText( sel[0], line );
  strcpy( cmdline, "perl gen_suite.pl " );
  strcat( cmdline, line );
  run( cmdline );
}

void CNewCat::OnLButtonDown(UINT, CPoint)
{
  char line[MAXLINE];
  m_pParent->m_pNewCat->GetLine( 1, line );
  m_pParent->m_pNewCat->SetSel( 0, -1 );
  m_pParent->m_pNewCat->Clear();
  m_pParent->m_pCatsList->AddString( line );
  for( int i = 0; i < MAXCAT && m_pParent->m_ppCats[i][0] != 0; i++ );
  strcpy( m_pParent->m_ppCats[i], line );
}

void CNewTest::OnLButtonDown(UINT, CPoint)
{
  char cmdline[MAXLINE];
  char dir[MAXLINE];
  char test[MAXLINE];
  char timeouts[MAXLINE];
  char execs[MAXLINE];
  int timeout;
  int exec;
  int cnt;
  char subsys[9];
  strcpy( subsys, "console " );
  m_pParent->m_pDirE->GetLine( 1, dir );
  m_pParent->m_pTestE->GetLine( 1, test );
  m_pParent->m_pTimeoutE->GetLine( 1, timeouts );
  m_pParent->m_pExecE->GetLine( 1, execs );

  if( m_pParent->m_pSubSysWindow->GetCheck() )
    strcpy( subsys, "window " );
  if( !( timeout = atoi( timeouts ) ) )
    MessageBox( "Bad timeout value" );
  else
  if( !( exec = atoi( execs ) ) )
    MessageBox( "Bad execution number value" );
  else
  if( cnt = m_pParent->m_pCatsList->GetSelCount() )
  {
    strcpy( cmdline, "perl utmanage.pl " );
    strcat( cmdline, subsys );
    strcat( cmdline, dir );
    strcat( cmdline, " " );
    strcat( cmdline, test );
    strcat( cmdline, " " );
    strcat( cmdline, timeouts );
    strcat( cmdline, " " );
    strcat( cmdline, execs );

    int sel[MAXCAT];
    m_pParent->m_pCatsList->GetSelItems( cnt, sel );
    for( int i = 0; i < cnt; i++ )
    {
      m_pParent->m_pCatsList->GetText( sel[i], test );
      strcat( cmdline, " " );
      strcat( cmdline, test );
    }
    run( cmdline );
  }
  else
    MessageBox( "Please select one or more categories" );
}

afx_msg int CMainWnd::OnCreate( LPCREATESTRUCT )
{
  FILE* f;
  char cmdline[MAXLINE];
  char dir[MAXLINE];
  char line[MAXLINE];
  int i = 0;
  GetCurrentDirectory( MAXLINE, dir );
  strcpy( cmdline, "perl " );
  strcat( cmdline, "confin.pl " );
  run( cmdline );

  strcpy( cmdline, dir );
  strcat( cmdline, "/test.tdb" );
  f = fopen( cmdline,"r" );
  if( !f )
    return 0;
  while( !feof( f ) )
  {
    fgets( line, MAXLINE, f );
    if( !feof( f ) )
    {
      line[strlen( line )-1] = 0;
      strcpy( m_ppCats[i++], line );
    }
  }
    fclose( f );


  return 0;
}

void CMainWnd::OnActivate( UINT, CWnd*, BOOL )
{
  m_pNewTest->SetButtonParent( this );
  m_pAddNewCat->SetButtonParent( this );
  m_pCatSuite->SetButtonParent( this );
  m_pOpen->SetButtonParent( this );
  m_pDownl->SetButtonParent( this );
  m_pUpl->SetButtonParent( this );
  m_pCatsList->ResetContent();
  for( int i = 0; i < MAXCAT && m_ppCats[i][0] != 0; i++ )
      m_pCatsList->AddString( m_ppCats[i] );
}

CMainWnd::CMainWnd()
{
  m_ppCats = new char*[MAXCAT];
  for( int i = 0; i < MAXCAT; i++ )
  {
    m_ppCats[i] = new char[MAXLINE];
    m_ppCats[i][0] = 0;
  }
  int top = 10;
  int left = 10;
  int lw = 90;
  int lh = 20;
  int dist = 5;
  int hdist = 10;

  Create(NULL,"Unit testing",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,CRect(100,100,640,450),NULL,NULL);
  m_pDirL = new CStatic();
  if (m_pDirL!=NULL) m_pDirL->Create("Directory",WS_CHILD|WS_VISIBLE|SS_CENTER,
    CRect(left,top,left+lw,top+lh),this);
  m_pDirE = new CEdit();
  if (m_pDirE!=NULL) m_pDirE->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP,
    CRect(left+lw+hdist,top,left+lw+hdist+200,top+lh),this,IDC_DIR);
  m_pTestL = new CStatic();
  if (m_pTestL!=NULL) m_pTestL->Create("Test name",WS_CHILD|WS_VISIBLE|SS_CENTER,
    CRect(left,top+lh+dist,left+lw,top+2*lh+dist),this);
  m_pTestE = new CEdit();
  if (m_pTestE!=NULL) m_pTestE->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_TABSTOP,
    CRect(left+lw+hdist,top+lh+dist,left+lw+hdist+200,top+2*lh+dist),this,IDC_DIR);
  m_pTimeoutL = new CStatic();
  if (m_pTimeoutL!=NULL) m_pTimeoutL->Create("Timeout",WS_CHILD|WS_VISIBLE|SS_CENTER,
    CRect(left,top+2*(lh+dist),left+lw,top+3*lh+2*dist),this);
  m_pTimeoutE = new CEdit();
  if (m_pTimeoutE!=NULL) m_pTimeoutE->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_NUMBER|ES_AUTOHSCROLL|WS_TABSTOP,
    CRect(left+lw+hdist,top+2*(lh+dist),left+lw+hdist+30,top+3*lh+2*dist),this,IDC_TIMEOUT);
  m_pExecL = new CStatic();
  if (m_pExecL!=NULL) m_pExecL->Create("Executions",WS_CHILD|WS_VISIBLE|SS_CENTER,
    CRect(left,top+3*(lh+dist),left+lw,top+4*lh+3*dist),this);
  m_pExecE = new CEdit();
  if (m_pExecE!=NULL) m_pExecE->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_NUMBER|ES_AUTOHSCROLL|WS_TABSTOP,
    CRect(left+lw+hdist,top+3*(lh+dist),left+lw+hdist+30,top+4*lh+3*dist),this,IDC_EXEC);
  m_pNewTest = new CNewTest();
  if (m_pNewTest!=NULL) m_pNewTest->Create("New test",WS_CHILD|WS_VISIBLE|SS_CENTER,
    CRect(10,120,100,160),this,IDC_NEWTEST);
  m_pCatsList = new CListBox();
  if (m_pCatsList!=NULL) m_pCatsList->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|LBS_EXTENDEDSEL|WS_VSCROLL,
    CRect(360,40,510,240),this,IDC_MYLIST);
  m_pCatSuite = new CCatSuite();
  if (m_pCatSuite!=NULL) m_pCatSuite->Create("Category suite",WS_CHILD|WS_VISIBLE|SS_CENTER,
    CRect(360,250,510,290),this,IDC_CATSUITE);
  m_pAddNewCat = new CNewCat();
  if (m_pAddNewCat!=NULL) m_pAddNewCat->Create("Add",WS_CHILD|WS_VISIBLE|SS_CENTER,
    CRect(470,10,510,30),this,IDC_ADDNEWCAT);
  m_pNewCat = new CEdit();
  if (m_pNewCat!=NULL) m_pNewCat->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL,
    CRect(360,10,460,30),this,IDC_NEWCAT);
  m_pDownl = new CDownl();
  if (m_pDownl!=NULL) m_pDownl->Create("Download",WS_CHILD|WS_VISIBLE|SS_CENTER,
    CRect(10,250,100,280),this,IDC_DOWNL);
  m_pOpen = new COpen();
  if (m_pOpen!=NULL) m_pOpen->Create("Edit",WS_CHILD|WS_VISIBLE|SS_CENTER,
    CRect(110,250,200,280),this,IDC_OPEN);
  m_pUpl = new CUpl();
  if (m_pUpl!=NULL) m_pUpl->Create("Upload",WS_CHILD|WS_VISIBLE|SS_CENTER,
    CRect(210,250,300,280),this,IDC_UPL);
  m_pBase = new CEdit();
  if (m_pBase!=NULL) m_pBase->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL,
    CRect(10,220,300,240),this,IDC_NEWCAT);
  m_pSubSysConsole = new CSubSys();
  if (m_pSubSysConsole!=NULL) m_pSubSysConsole->Create("Console",WS_CHILD|WS_VISIBLE|SS_CENTER|BS_AUTORADIOBUTTON,
    CRect(210,120,300,140),this,IDC_CONSOLE);
  m_pSubSysWindow = new CSubSys();
  if (m_pSubSysWindow!=NULL) m_pSubSysWindow->Create("Window",WS_CHILD|WS_VISIBLE|SS_CENTER|BS_AUTORADIOBUTTON,
    CRect(210,140,300,160),this,IDC_WINDOW);
  m_pSubSysConsole->SetCheck( 1 );
}

CMainWnd::~CMainWnd()
{
  delete m_ppCats;
  if( m_pDirL != NULL ) delete m_pDirL;
  if( m_pDirE != NULL ) delete m_pDirE;
  if( m_pTimeoutL != NULL ) delete m_pTimeoutL;
  if( m_pTimeoutE != NULL ) delete m_pTimeoutE;
  if( m_pExecL != NULL ) delete m_pExecL;
  if( m_pExecE != NULL ) delete m_pExecE;
  if( m_pNewTest != NULL ) delete m_pNewTest;
  if( m_pCatSuite != NULL ) delete m_pCatSuite;
  if( m_pCatsList != NULL ) delete m_pCatsList;
  if( m_pNewCat != NULL ) delete m_pNewCat;
  if( m_pOpen != NULL ) delete m_pOpen;
  if( m_pDownl != NULL ) delete m_pDownl;
  if( m_pUpl != NULL ) delete m_pUpl;
  if( m_pBase != NULL ) delete m_pBase;
  if( m_pSubSysConsole != NULL ) delete m_pSubSysConsole;
  if( m_pSubSysWindow != NULL ) delete m_pSubSysWindow;
}

class CMyApp : public CWinApp
{
public:
  CMyApp();
  virtual BOOL InitInstance();
};

CMyApp::CMyApp()
{}

BOOL CMyApp::InitInstance()
{
  m_pMainWnd=new CMainWnd();
  ASSERT(m_pMainWnd);
  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();
  return TRUE;
};

CMyApp theApp;