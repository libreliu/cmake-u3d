<!--Perl: header_info-->

//TODO: include all necessary headers
#include "IFXException.h"
#include "U3DViewer.h"
#include "<!--Perl: test_name-->.h"

class U3DViewWrapper
{
public:
  U3DViewer* m_pViewer;

  U3DViewWrapper();
  ~U3DViewWrapper();
};


int WINAPI WinMain(HINSTANCE hInstance,
           HINSTANCE hPrevInstance,
           LPSTR lpCmdLine,
           int nCmdShow)
{
  U3DViewWrapper u3dapp;
  IFXRESULT result = IFX_OK;
  <!--Perl: test_name--> test;

  try {
    result = IFXCOMInitialize();

    if ( IFXSUCCESS(result)) {
      result = u3dapp.m_pViewer->SetupWindows();
    }

    if ( IFXSUCCESS(result)) {
      U3DSceneUtilities* pSGUtils = new U3DSceneUtilities;
      test.NewLog();
      test.Run( pSGUtils );
      test.Report();
      test.ReleaseLog();
      result = u3dapp.m_pViewer->InitializeSG( pSGUtils );
    }

    // Render until the user is done
    if ( IFXSUCCESS(result)) {
      result = u3dapp.m_pViewer->EventLoop();
    }

    result = IFXCOMUninitialize();
  }
  catch (IFXException& e) {
     result = e.GetIFXResult();
  }
  if( test.GetNumFailed() )
    return 1;
  else
    return result;
}


U3DViewWrapper::U3DViewWrapper()
{
  m_pViewer = new(U3DViewer);

}

U3DViewWrapper::~U3DViewWrapper()
{
  delete(m_pViewer);

}
