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


#include "AmayaWindow.h"
#include "AmayaDocument.h"
//#include "AmayaSplitterWindow.h"
#include "AmayaPanel.h"
//#include "AmayaNotebook.h"
#include "AmayaPage.h"

//#include "AmayaMenuItem.h"

//#include "AmayaSetting.h"
//#include "AmayaGLCanvas.h"
//#include "DnDFileTarget.h"
//#include "TestUnicodeDialogue.h"

//#include "appdialogue.h"

// a deplacer dans un .h
#define DEFAULT_TOOGLE_FULLSCREEN    false
#define DEFAULT_TOOGLE_TOOLTIP       true


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  AmayaWindow
 * Description:  create a new AmayaWindow
 *               AmayaWindow
 *               [
 *                   Menu TODO
 *                   m_pToolBar TODO
 *                   wxSplitterWindow
 *                   [
 *                       AmayaPanel
 *                       AmayaPage
 *                   ]
 *                   m_pStatusBar
 *               ]
 *--------------------------------------------------------------------------------------
 */
AmayaWindow::AmayaWindow (  int            window_id
      			   ,wxWindow *     p_parent_window
                           ,const wxPoint& pos
	                   ,const wxSize&  size
		       ) : 
  wxFrame( p_parent_window, -1, _T(""), pos, size ),
  menuCallback(),
  toolbarCallback(),
  m_WindowId( window_id ),
  m_IsFullScreenEnable( DEFAULT_TOOGLE_FULLSCREEN ),
  m_IsToolTipEnable( DEFAULT_TOOGLE_TOOLTIP ),
  m_SlashRatio( 0.20 )
{
  // Creation of the toolbar
  CreateToolBar( wxHORIZONTAL|wxTB_DOCKABLE|wxTB_FLAT );
  m_pToolBar = GetToolBar();
  m_pToolBar->SetMargins( 2, 2 );
 
  // Creation of the statusbar
  CreateStatusBar( 1 );
  m_pStatusBar = GetStatusBar();

  // Creation of differents frame areas
  wxBoxSizer *	p_SizerTop = NULL;

  // Insert a forground sizer into the frame
  p_SizerTop = new wxBoxSizer ( wxHORIZONTAL );
  SetSizer( p_SizerTop );

  // Create a splitted vertical window
  m_pSplitterWindow = new wxSplitterWindow( this, -1,
                      		wxDefaultPosition, wxDefaultSize,
                     		wxSP_3D | wxSP_NOBORDER | wxSP_PERMIT_UNSPLIT );
  m_pSplitterWindow->SetMinimumPaneSize( 50 );
  p_SizerTop->Add( m_pSplitterWindow, 1, wxGROW, 0 );

  m_pCurrentPanel = new AmayaPanel( m_pSplitterWindow );
  m_pPage         = new AmayaPage( m_pSplitterWindow );
  m_pSplitterWindow->SplitVertically(
      m_pCurrentPanel,
      m_pPage,
      (int)(m_SlashRatio*((float)GetSize().GetWidth())) );
  
  SetAutoLayout(TRUE);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  ~AmayaWindow
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
AmayaWindow::~AmayaWindow()
{
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  AttachFrame
 * Description:  attach a frame to the window
 *      params:
 *        + AmayaFrame * p_frame : the frame to attach
 *        TODO rajouter un param pour indiquer la position top ou bottom de la frame a inserer
 *--------------------------------------------------------------------------------------
 */
bool AmayaWindow::AttachFrame( AmayaFrame * p_frame )
{
  if ( p_frame )
  {
    m_pPage->AttachTopFrame( p_frame );
    SetAutoLayout(TRUE);
  }
  else
  {
    return false;
  }

  return true;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  DetachFrame
 * Description:  detach a frame to the window
 *      params:
 *        + AmayaFrame * p_frame : the frame to detach
 *        TODO
 *--------------------------------------------------------------------------------------
 */
bool AmayaWindow::DetachFrame( AmayaFrame * p_frame )
{

  return true;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  AppendMenu
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::AppendMenu ( wxMenu * p_menu, const wxString & label )
{
   GetMenuBar()->Append( p_menu,
		         label );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  AppendMenuItem
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::AppendMenuItem ( 
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

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  OnClose
 * Description:  just close the contained AmayaPage
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::OnClose(wxCloseEvent& event)
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
  // event.Skip() is called or not by m_pPage->OnClose
  if (m_pPage)
    m_pPage->OnClose( event );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  OnToolBarTool
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::OnToolBarTool( wxCommandEvent& event )
{
//  printf ( "OnToolBarTool : id = %d\t toolid = %d\n", event.GetId(), event.GetId()-AmayaWindow::TOOLBAR_TOOL_START );
//  ToolBarActionCallback( event.GetId()-AmayaWindow::TOOLBAR_TOOL_START, m_FrameId );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  OnMenuItem
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::OnMenuItem( wxCommandEvent& event )
{
  wxMenu * p_menu = (wxMenu *)event.GetEventObject();
  long     id     = event.GetId()-AmayaWindow::MENU_ITEM_START;

  printf ( "OnMenuItem : p_menu = 0x%x\tmenuid = %d\n", p_menu, id );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  SetURL
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::SetURL ( const wxString & new_url )
{
  m_pURLBar->SetValue( new_url );

  // Just select url
  m_pURLBar->SetSelection( 0, new_url.Length() );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  AppendURL
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::AppendURL ( const wxString & new_url )
{
  m_pURLBar->Append( new_url );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  OnSplitterUnsplit
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::OnSplitterUnsplit( wxSplitterEvent& event )
{
  wxPrintf( _T("EVT_SPLITTER_UNSPLIT\n") );
  
  event.Skip();  
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaWindow, wxFrame)

  // when a menu item is pressed  
  EVT_MENU_RANGE( AmayaWindow::MENU_ITEM_START, AmayaWindow::MENU_ITEM_END, AmayaWindow::OnMenuItem ) 
  
  // when a toolbar button is pressed  
  EVT_TOOL_RANGE( AmayaWindow::TOOLBAR_TOOL_START, AmayaWindow::TOOLBAR_TOOL_END, AmayaWindow::OnToolBarTool ) 
  
  EVT_CLOSE( AmayaWindow::OnClose )


//  EVT_SPLITTER_SASH_POS_CHANGED( -1, AmayaWindow::OnSplitterPosChanged )
//  EVT_SPLITTER_DCLICK( -1, AmayaWindow::OnSplitterDClick )
  EVT_SPLITTER_UNSPLIT( -1, AmayaWindow::OnSplitterUnsplit )
  
//  EVT_SIZE( AmayaWindow::OnSize )

END_EVENT_TABLE()

#endif /* #ifdef _WX */
