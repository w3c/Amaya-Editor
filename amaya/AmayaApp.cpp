#ifdef _WX

#include "wx/wx.h"
#include "wx/memory.h"
#ifndef _GLPRINT
  #include "wx/xrc/xmlres.h"          // XRC XML resouces
#endif /* _GLPRINT */

#define THOT_EXPORT extern
#include "amaya.h"


#include "AmayaApp.h"


//#include "AmayaFrame.h"

//#include "appdialogue.h"


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

/*
// Generique Frame creation
// Wraping function between wxWindows and Amaya kernel
AmayaFrame * AmayaApp::CreateAmayaFrame ( int frame, const char * frame_name, int x, int y, int w, int h )
{
  AmayaFrame * pFrame = new AmayaFrame( (void *) frame,
					(wxFrame *) NULL,
					wxString( frame_name, wxConvUTF8 ),
					wxPoint( x, y ),
					wxSize( w, h ),
					_locale );
  if ( !pFrame ) return pFrame;

  // Load the user customized workspace corresponding to user profil
  LoadWorkspaceParameters( pFrame );

  // Register the new frame into global table
  FrameTable[frame].WdFrame = pFrame;
  FrameTable[frame].pMenu = pFrame->GetMenuBar();
  FrameTable[frame].pToolBar = pFrame->GetToolBar();
  FrameTable[frame].pStatusBar = pFrame->GetStatusBar();


  
  pFrame->Show(TRUE);
  SetTopWindow(pFrame);
}

void AmayaApp::LoadWorkspaceParameters( AmayaFrame * pFrame )
{
  // TODO : il faut charger les valeurs a partir d'un fichier de config

  // load full screen param
  pFrame->m_IsFullScreenEnable = false;

  // load tool tips param
  pFrame->m_IsToolTipEnable = true;

  // load history
  pFrame->AppendURL ( _T("http://www.w3.org/Amaya/") );
  pFrame->AppendURL ( _T("http://www.google.fr/") );
  pFrame->AppendURL ( _T("http://www.yahoo.fr/") );
  pFrame->AppendURL ( _T("http://www.tf1.fr/") );

  // Send an event to toogle fullscreen if needed
  {
    pFrame->m_IsFullScreenEnable = !pFrame->m_IsFullScreenEnable;
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, AmayaFrame::MENU_VIEW_FULLSCREEN );
    wxPostEvent( pFrame, event );
  }

  // Send an event to toogle tooltips if needed
  {
    pFrame->m_IsToolTipEnable = !pFrame->m_IsToolTipEnable;
    wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, AmayaFrame::MENU_VIEW_TOOLTIP );
    wxPostEvent( pFrame, event );
  }
}

void AmayaApp::SaveWorkspaceParameters( AmayaFrame * pFrame ) 
{

}
*/

#endif /* #ifdef _WX */
