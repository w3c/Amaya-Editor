#ifdef _WX

#include "wx/wx.h"
#include "wx/memory.h"
#ifndef _GLPRINT
  #include "wx/xrc/xmlres.h"          // XRC XML resouces
#endif /* _GLPRINT */

#define THOT_EXPORT extern
#include "amaya.h"

#include "AmayaApp.h"

#include "wxAmayaSocketEventLoop.h"
#include "wxAmayaSocketEvent.h"

IMPLEMENT_APP(AmayaApp)

// Static attribut used to convert text from unicode to ISO-8859-1
// or from ISO-8859-1 to unicode
wxCSConv AmayaApp::conv_ascii(_T("ISO-8859-1"));

#ifndef _GLPRINT
// defined into EDITORAPP.c
extern int amaya_main (int argc, char** argv);
#else /* _GLPRINT */
// defined into print.c
extern int main (int argc, char** argv);
#endif /* #ifndef _GLPRINT */

bool AmayaApp::OnInit()
{
  // for debug : the output is stderr
  delete wxLog::SetActiveTarget( new wxLogStderr );
  
  // just convert arguments format (unicode to iso-8859-1) before passing it to amaya_main
  InitAmayaArgs();

#ifndef _GLPRINT
  /* initialize the Registry */
  TtaInitializeAppRegistry (amaya_argv[0]);

  // Initialize all the XRC handlers. Always required (unless you feel like
  // going through and initializing a handler of each control type you will
  // be using (ie initialize the spinctrl handler, initialize the textctrl
  // handler). However, if you are only using a few control types, it will
  // save some space to only initialize the ones you will be using. See
  // wxXRC docs for details.
  wxXmlResource::Get()->InitAllHandlers();    
    
  // Load all of the XRC files that will be used. You can put everything
  // into one giant XRC file if you wanted, but then they become more 
  // diffcult to manage, and harder to reuse in later projects.   

  // this is the amaya directory (need to be called after amaya_main or
  // TtaGetEnvString will return bad strings)
  wxString amaya_directory( TtaGetEnvString ("THOTDIR"), conv_ascii );

  // Now it's possible to load all the dialogs
  wxXmlResource::Get()->Load( amaya_directory+_T("/resources/xrc/InitConfirmDlgWX.xrc") );
  // TODO: rajouter ici toutes les autres ressources a charger
  
  // just call amaya main from EDITORAPP.c
  amaya_main( amaya_argc, amaya_argv );
#endif /* #ifndef _GLPRINT */

#ifdef _GLPRINT
  main( amaya_argc, amaya_argv );
#endif /* #ifndef _GLPRINT */

  /* setup the socket event loop */
  /* when a socket is active, check every 100 ms if something happend on the socket */
  m_SocketEventLoop = new wxAmayaSocketEventLoop( 100 );
  wxAmayaSocketEvent::InitSocketEvent( m_SocketEventLoop );

  return true;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  OnExit
 * Description:  free all allocated memory
 *--------------------------------------------------------------------------------------
 */
int AmayaApp::OnExit()
{
  m_SocketEventLoop->Stop();
  delete m_SocketEventLoop;
  m_SocketEventLoop = NULL;

  // free arguments
  ClearAmayaArgs();

  // free internal amaya ressources
  TtaQuit();

  return 0;
}
  
/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  InitAmayaArgs
 * Description:  this methode convert wxApp::argc and wxApp::argv to amaya format
 *               amaya_argv must be free when closing application
 *--------------------------------------------------------------------------------------
 */
void AmayaApp::InitAmayaArgs()
{
  // convert argc and argv in order to be compatible with amaya
  amaya_argc = wxApp::argc;
  amaya_argv = new char*[amaya_argc];

  //  wxCSConv conv_ascii(_T("ISO-8859-1")); // to convert string in ASCII (ISO-8859-1)
  for ( int i = 0; i < amaya_argc; i++ )
  {
    // unicode to ascii convertion of every arguments
    wxString     amaya_arg( wxApp::argv[i] );
    wxASSERT_MSG( amaya_arg.IsAscii(), _T("arguments (argv) must contain only ascii char in order to be converted in ascii") );
    amaya_argv[i] = new char[amaya_arg.Length()+1];
    sprintf(amaya_argv[i],"%s", (const char*) amaya_arg.mb_str(conv_ascii));
  }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApp
 *      Method:  ClearAmayaArgs
 * Description:  free arguments -> must be called when appli exit
 *--------------------------------------------------------------------------------------
 */
void AmayaApp::ClearAmayaArgs()
{
  for ( int i = 0; i < amaya_argc; i++ )
  {
    delete [] amaya_argv[i];
  }  
  delete [] amaya_argv;
}

void AmayaApp::OnIdle( wxIdleEvent& event )
{
  //  wxLogDebug( _T("AmayaApp::OnIdle") );

  event.Skip();
}

BEGIN_EVENT_TABLE(AmayaApp, wxApp)
  EVT_IDLE( AmayaApp::OnIdle ) // Process a wxEVT_IDLE event  
END_EVENT_TABLE()


#endif /* #ifdef _WX */
