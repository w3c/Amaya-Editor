#ifdef _WX

#include "AmayaApp.h"
//#include "AmayaFrame.h"

//#include "appdialogue.h"

IMPLEMENT_APP(AmayaApp)

// defined into EDITORAPP.c
extern int amaya_main (int argc, char** argv);

bool AmayaApp::OnInit()
{
  /*  const wxString langs[] =
    {
      _T("(System default)"),
      _T("French"),
      _T("Russian"),
      _T("English"),
      _T("English (U.S.)")
    };
  
  SetExitOnFrameDelete(FALSE);
  int lng = wxGetSingleChoiceIndex(_T("Please choose language:"), _T("Language"), 
                                   WXSIZEOF(langs), langs);
  SetExitOnFrameDelete(TRUE);
  
  // Initialize local system with the given language
  switch (lng)
    {
    case 0 : _locale.Init(wxLANGUAGE_DEFAULT); break;
    case 1 : _locale.Init(wxLANGUAGE_FRENCH); break;
    case 2 : _locale.Init(wxLANGUAGE_GERMAN); break;
    case 3 : _locale.Init(wxLANGUAGE_RUSSIAN); break;
    case 4 : _locale.Init(wxLANGUAGE_ENGLISH); break;
    case -1:
    case 5 : _locale.Init(wxLANGUAGE_ENGLISH_US); break;
    }
  
  // Add a catalogue : allow application to find translations (into '/language/catalog.mo' here : '/fr/amayaui.mo' for the french translation)
  _locale.AddCatalog(_T("amayaui"));
  */
  
  // Launche thot init functions
//  InitThot ();

  /*
  AmayaFrame *frame = new AmayaFrame( (wxFrame *) NULL,
				      _("Amaya [International] [OpenGL]"),
				      wxPoint(100, 100), wxSize(800, 600),
				      _locale);

  frame->Show(TRUE);


  SetTopWindow(frame);
  */

/*  // Insert a log window for debug
  wxTextCtrl * p_log_win = new wxTextCtrl( this, -1, _T("This is the log window.\n"),
                            wxPoint(5,260), wxSize(630,100),
                            wxTE_MULTILINE | wxTE_READONLY );
 */
  
  // for debug : the output is stderr
  delete wxLog::SetActiveTarget( new wxLogStderr( ) );
  
  // just convert arguments format (unicode to iso-8859-1) before passing it to amaya_main
  InitAmayaArgs();

  // just call amaya main from EDITORAPP.c
  amaya_main( amaya_argc, amaya_argv );

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
  amaya_argv = new (char*)[amaya_argc];

  wxCSConv toascii(_T("ISO-8859-1")); // to convert string in ASCII (ISO-8859-1)
  for ( int i = 0; i < amaya_argc; i++ )
  {
    // unicode to ascii convertion of every arguments
    wxString     argument( wxApp::argv[i] );
    wxASSERT_MSG( argument.IsAscii(), _("arguments (argv) must contain only ascii char in order to be converted in ascii") );
    amaya_argv[i] = new char[argument.Length()+1];
    sprintf(amaya_argv[i],"%s", (const char*) argument.mb_str(toascii));
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
