#ifdef _WX

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"

#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "font_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "font_f.h"
#include "appli_f.h"
#include "profiles_f.h"
#include "appdialogue_f.h"
#include "boxparams_f.h"


#include "AmayaFrame.h"
#include "AmayaDocument.h"
//#include "AmayaMenuItem.h"

//#include "AmayaSetting.h"
//#include "AmayaGLCanvas.h"
//#include "DnDFileTarget.h"
//#include "TestUnicodeDialogue.h"

//#include "appdialogue.h"

// a deplacer dans un .h
#define DEFAULT_TOOGLE_FULLSCREEN    false
#define DEFAULT_TOOGLE_TOOLTIP       true

AmayaFrame::AmayaFrame ( wxFrame * frame
			 ,const wxPoint& pos
			 ,const wxSize& size
			 ,int frameid			 
			 //,wxLocale& locale
		       ) : 
  wxFrame( frame, -1, _T(""), pos, size ),
  menuCallback(),
  toolbarCallback(),
//  m_UserData ( user_data ),
//  m_Locale( locale ),
  m_FrameId(frameid),
  m_IsFullScreenEnable( DEFAULT_TOOGLE_FULLSCREEN ),
  m_IsToolTipEnable( DEFAULT_TOOGLE_TOOLTIP )
{
  /* Creation fo the toolbar */
  CreateToolBar( /*wxNO_BORDER|*/wxHORIZONTAL|wxTB_DOCKABLE|wxTB_FLAT/*|wxTB_TEXT*/ ); 
  GetToolBar()->SetMargins( 2, 2 );
  m_pToolBar = GetToolBar();
 
  CreateStatusBar( 1 );
  m_pStatusBar = GetStatusBar();



  wxBoxSizer *	p_SizerTop;
  wxBoxSizer *	p_SizerV;
  wxBoxSizer *	p_SizerH;

  wxPanel *		p_PanelTop;
  wxWindow *		p_Parent;
  p_Parent = this;

  // Insert a forground panel into the frame
  p_SizerTop = new wxBoxSizer ( wxVERTICAL );
  p_PanelTop = new wxPanel ( p_Parent );
  p_SizerTop->SetMinSize( 800, 600 );
  p_SizerTop->Add( p_PanelTop, 1, wxGROW|wxALL, 0 );
  SetSizer( p_SizerTop );
  p_Parent = p_PanelTop;

  // Create 2 sizer, and place it into the panel
  p_SizerV = new wxBoxSizer( wxVERTICAL );
  p_SizerH = new wxBoxSizer( wxHORIZONTAL );
  p_Parent->SetSizer( p_SizerV );
  p_SizerV->Add( p_SizerH, 0, wxGROW|wxALL, 0 );


  // Add URL bar
  m_pURLBar = new wxComboBox( p_Parent, -1,  _T(""), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
//  m_pURLBar->SetToolTip( _T("Enter the URL here") );
  p_SizerH->Add( m_pURLBar, 1, wxGROW|wxALL|wxALIGN_CENTER_VERTICAL, 5 );

  
  // add a blank document
  AmayaDocument * p_AmayaDoc = new AmayaDocument( p_Parent );
  p_SizerV->Add ( p_AmayaDoc, 1, wxALL|wxGROW, 0 );
  
  /*
  wxScrolledWindow * p_ScrolledWindow = new wxScrolledWindow ( p_Parent );
  wxBoxSizer * p_SizerTmp = new wxBoxSizer ( wxVERTICAL );
  p_ScrolledWindow->SetSizer( p_SizerTmp );
  //	p_ScrolledWindow->EnableScrolling(TRUE, TRUE);
  p_ScrolledWindow->SetScrollbars(20, 20, 30, 30);
  p_SizerV->Add( p_ScrolledWindow, 1, wxALL|wxGROW, 0 );
*/
  // Add a GLCanvas
/*	m_pGLCanvas = new AmayaGLCanvas( p_ScrolledWindow, -1, wxDefaultPosition, wxSize(1000,1000) );
  //	p_SizerV->Add( m_pGLCanvas, 1, wxALL|wxGROW, 2 );
  m_pGLCanvas->SetFocus();
  p_SizerTmp->Add( m_pGLCanvas, 1, wxALL|wxGROW, 0 );
*/
  // Add the amaya logo to the H.Sizer
/*  p_SizerH->Add(
	  new wxStaticBitmap( 
		  p_Parent,
		  wxID_STATIC,
		  wxBitmap(_T("icons/logo.xpm"), wxBITMAP_TYPE_XPM) ),
	  0,
	  wxALIGN_LEFT|wxALL,
	  2 );
*/
  
  
  SetAutoLayout(TRUE);


  
//TODO   return (frame);
#if 0
	wxBoxSizer *	p_SizerTop;
	wxBoxSizer *	p_SizerV;
	wxBoxSizer *	p_SizerH;

	wxPanel *		p_PanelTop;
	wxWindow *		p_Parent;
	p_Parent = this;

	// Insert a forground panel into the frame
	p_SizerTop = new wxBoxSizer ( wxVERTICAL );
	p_PanelTop = new wxPanel ( p_Parent );
	p_SizerTop->SetMinSize( 800, 600 );
	p_SizerTop->Add( p_PanelTop, 1, wxGROW|wxALL, 0 );
	SetSizer( p_SizerTop );
	p_Parent = p_PanelTop;

	// Create 2 sizer, and place it into the panel
	p_SizerV = new wxBoxSizer( wxVERTICAL );
	p_SizerH = new wxBoxSizer( wxHORIZONTAL );
	p_Parent->SetSizer( p_SizerV );
	p_SizerV->Add( p_SizerH, 0, wxGROW|wxALL, 0 );


	// Add a scrolled window
	wxScrolledWindow * p_ScrolledWindow = new wxScrolledWindow ( p_Parent );
	wxBoxSizer * p_SizerTmp = new wxBoxSizer ( wxVERTICAL );
	p_ScrolledWindow->SetSizer( p_SizerTmp );
	//	p_ScrolledWindow->EnableScrolling(TRUE, TRUE);
	p_ScrolledWindow->SetScrollbars(20, 20, 30, 30);
	p_SizerV->Add( p_ScrolledWindow, 1, wxALL|wxGROW, 0 );

	// Add a GLCanvas
/*	m_pGLCanvas = new AmayaGLCanvas( p_ScrolledWindow, -1, wxDefaultPosition, wxSize(1000,1000) );
	//	p_SizerV->Add( m_pGLCanvas, 1, wxALL|wxGROW, 2 );
	m_pGLCanvas->SetFocus();
	p_SizerTmp->Add( m_pGLCanvas, 1, wxALL|wxGROW, 0 );
*/
	// Add the amaya logo to the H.Sizer
	p_SizerH->Add(
		new wxStaticBitmap( 
			p_Parent,
			wxID_STATIC,
			wxBitmap(_T("icons/logo.xpm"), wxBITMAP_TYPE_XPM) ),
		0,
		wxALIGN_LEFT|wxALL,
		2 );

	// Add URL bar
	m_pURLBar = new wxComboBox( p_Parent, -1,  _T(""), wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
	m_pURLBar->SetToolTip( _T("Enter the URL here") );
	p_SizerH->Add( m_pURLBar, 1, wxGROW|wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	// ----------------------------------------------------------- //
	// Build the menu/statusbar/toolbars
	m_pMenuBar = new wxMenuBar();
  
	// File menu
	m_pFileMenu = new wxMenu();
	m_pFileMenu->Append(MENU_FILE_OPEN, _("&Open\tCtrl-O"), _("Opens an existing file"));
	m_pFileMenu->Append(MENU_FILE_SAVE, _("&Save\tCtrl-S"), _("Save the content"));
	m_pFileMenu->AppendSeparator();
	m_pFileMenu->Append(MENU_FILE_QUIT, _("&Quit\tCtrl-Q"), _("Quit the application"));
	m_pMenuBar->Append(m_pFileMenu, _("&File"));
  
	// View menu
	m_pViewMenu = new wxMenu();

	m_pMenuItemToggleFullScreen = new wxMenuItem( m_pViewMenu,
						      MENU_VIEW_FULLSCREEN,
						      _("Toogle &Full Screen\tCtrl-F"),
						      _("Toggle full screen"),
						      wxITEM_CHECK );
	m_pMenuItemToggleToolTip = new wxMenuItem( m_pViewMenu,
						   MENU_VIEW_TOOLTIP,
						   _("&Toggle ToolTip\tCtrl-T"),
						   _("Toggle ToolTip"),
						   wxITEM_CHECK );

	m_pViewMenu->Append( m_pMenuItemToggleFullScreen );
	m_pViewMenu->Append( m_pMenuItemToggleToolTip );
	m_pMenuBar->Append(m_pViewMenu, _("&View"));

	// About menu
	m_pInfoMenu = new wxMenu();
	m_pInfoMenu->Append(MENU_INFO_TESTUNICODE, _("&Test Unicode\tCtrl-U"), _("Unicode demo"));
	m_pFileMenu->AppendSeparator();
	m_pInfoMenu->Append(MENU_INFO_ABOUT, _("&About\tCtrl-A"), _("Shows information about the application"));
	m_pMenuBar->Append(m_pInfoMenu, _("&Info"));
  
	SetMenuBar(m_pMenuBar);
  
	// Create status bar with 3 fields
	CreateStatusBar(3);
	SetStatusText(_("Ready"), 0);
	m_pStatusBar = GetStatusBar();

	// Create the toolbar
	CreateToolBar( /*wxNO_BORDER|*/wxHORIZONTAL|wxTB_DOCKABLE|wxTB_FLAT/*|wxTB_TEXT*/ ); 
	GetToolBar()->SetMargins( 2, 2 );
	InitToolbar(GetToolBar());
	m_pToolBar = GetToolBar();
	// ----------------------------------------------------------- //

	SetAutoLayout(TRUE);
	p_SizerTop->Fit(this);

	// This frame is able to receive droped files
	// Create a customized DnD target
//	new DnDFileTarget( this );

//	m_pTestUnicodeDialogue = new TestUnicodeDialogue( this );
#endif
}

AmayaFrame::~AmayaFrame()
{
}

void AmayaFrame::appendMenu ( wxMenu * p_menu, const wxString & label )
{
   GetMenuBar()->Append( p_menu,
		         label );
}

void AmayaFrame::appendMenuItem ( 
    wxMenu * 		p_menu_parent,
    long 		id,
    const wxString & 	label,
    const wxString & 	help,
    wxItemKind 		kind,
    const AmayaCParam & callback )
{
  if ( kind != wxITEM_SEPARATOR )
  {
    wxASSERT( id+MENU_ITEM_START < MENU_ITEM_END );
    id += MENU_ITEM_START;
  }
  
  p_menu_parent->Append(
     id,
     label,
     help,
     kind );

  // TODO : call callbacks
  if ( kind == wxITEM_SEPARATOR )
  {
    // do not call callback if it's a separator
  }
}


void AmayaFrame::OnClose(wxCloseEvent& event)
{
/*  
    if ( event.CanVeto() && (gs_nFrames > 0) )
    {
        wxString msg;
        msg.Printf(_T("%d windows still open, close anyhow?"), gs_nFrames);
        if ( wxMessageBox(msg, _T("Please confirm"),
                          wxICON_QUESTION | wxYES_NO) != wxYES )
        {
            event.Veto();

            return;
        }
    }
*/
    // map this callback to generic one : really kill amaya frame
    KillFrameCallback( m_FrameId );
    
    event.Skip();
}

void AmayaFrame::OnToolBarTool( wxCommandEvent& event )
{
//  printf ( "OnToolBarTool : id = %d\t toolid = %d\n", event.GetId(), event.GetId()-AmayaFrame::TOOLBAR_TOOL_START );
//  ToolBarActionCallback( event.GetId()-AmayaFrame::TOOLBAR_TOOL_START, m_FrameId );
}

void AmayaFrame::OnMenuItem( wxCommandEvent& event )
{
  wxMenu * p_menu = (wxMenu *)event.GetEventObject();
  long     id     = event.GetId()-AmayaFrame::MENU_ITEM_START;

  printf ( "OnMenuItem : p_menu = 0x%x\tmenuid = %d\n", p_menu, id );
}

void AmayaFrame::SetURL ( const wxString & new_url )
{
  m_pURLBar->SetValue( new_url );

  // Just select url
  m_pURLBar->SetSelection( 0, new_url.Length() );
}

void AmayaFrame::AppendURL ( const wxString & new_url )
{
  m_pURLBar->Append( new_url );
}

BEGIN_EVENT_TABLE(AmayaFrame, wxFrame)

  // when a menu item is pressed  
  EVT_MENU_RANGE( AmayaFrame::MENU_ITEM_START, AmayaFrame::MENU_ITEM_END, AmayaFrame::OnMenuItem ) 
  
  // when a toolbar button is pressed  
  EVT_TOOL_RANGE( AmayaFrame::TOOLBAR_TOOL_START, AmayaFrame::TOOLBAR_TOOL_END, AmayaFrame::OnToolBarTool ) 
  
  EVT_CLOSE( AmayaFrame::OnClose )

END_EVENT_TABLE()

#endif /* #ifdef _WX */
