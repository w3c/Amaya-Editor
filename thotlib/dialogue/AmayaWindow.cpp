#ifdef _WX

#include "wx/wx.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "selection.h"
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
#include "dialogapi_f.h"
#include "callback_f.h"
#include "appdialogue_wx_f.h"

#include "AmayaWindow.h"
#include "AmayaPanel.h"
#include "AmayaNotebook.h"
#include "AmayaPage.h"
#include "AmayaFrame.h"
#include "AmayaCallback.h"

// TODO : a deplacer dans un .h
#define DEFAULT_TOOGLE_FULLSCREEN    false
#define DEFAULT_TOOGLE_TOOLTIP       true

// Static attribut used to convert text from unicode to ISO-8859-1
// or from ISO-8859-1 to unicode
wxCSConv AmayaWindow::conv_ascii(_T("ISO-8859-1"));


IMPLEMENT_DYNAMIC_CLASS(AmayaWindow, wxFrame)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  AmayaWindow
 * Description:  create a new AmayaWindow
 *--------------------------------------------------------------------------------------
 */
AmayaWindow::AmayaWindow (  int            window_id
      			   ,wxWindow *     p_parent_window
                           ,const wxPoint& pos
	                   ,const wxSize&  size
		       ) : 
  wxFrame( wxDynamicCast(p_parent_window, wxWindow),
	   -1, _T(""), pos, size ),
//  menuCallback(),
//  toolbarCallback(),
  m_WindowId( window_id ),
  m_IsFullScreenEnable( DEFAULT_TOOGLE_FULLSCREEN ),
  m_IsToolTipEnable( DEFAULT_TOOGLE_TOOLTIP ),
  m_SlashRatio( 0.20 ),
  m_IsClosing( FALSE )
{
  // Create a splitted vertical window
  m_pSplitterWindow = new wxSplitterWindow( this, -1,
                      		            wxDefaultPosition, wxDefaultSize,
                     		            wxSP_3D | wxSP_NOBORDER | wxSP_PERMIT_UNSPLIT );
  m_pSplitterWindow->SetMinimumPaneSize( 50 );
  
  // Create a background panel to contains the notebook
  wxPanel * p_NotebookPanel = new wxPanel( m_pSplitterWindow, -1, wxDefaultPosition, wxDefaultSize,
        wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_BORDER);

  // Create the notebook with its special sizer
  m_pNotebook                              = new AmayaNotebook( p_NotebookPanel, this );
  wxNotebookSizer * p_SpecialNotebookSizer = new wxNotebookSizer( m_pNotebook );

  // Create a sizer to layout the notebook in the panel
  wxBoxSizer * p_NotebookSizer             = new wxBoxSizer ( wxHORIZONTAL );
  p_NotebookSizer->Add(p_SpecialNotebookSizer, 1, wxEXPAND | wxALL, 0);
  p_NotebookPanel->SetSizer(p_NotebookSizer);
  p_NotebookPanel->Layout();
  
  // Create a AmayaPanel to contains commands shortcut
  m_pCurrentPanel = new AmayaPanel( m_pSplitterWindow );

  // Split the Notebook and the AmayaPanel
  m_pSplitterWindow->SplitVertically(
      m_pCurrentPanel,
      p_NotebookPanel,
      (int)(m_SlashRatio*((float)GetSize().GetWidth())) );  
  
  // Creation of frame sizer to contains differents frame areas
  wxBoxSizer * p_SizerFrame = new wxBoxSizer ( wxHORIZONTAL );
  p_SizerFrame->Add( m_pSplitterWindow, 1, wxEXPAND );

  SetSizer(p_SizerFrame);
  SetAutoLayout(TRUE);
  p_SizerFrame->Fit(this);

  // Creation of the toolbar
  //  CreateToolBar( wxHORIZONTAL|wxTB_DOCKABLE|wxTB_FLAT );
  //  GetToolBar()->SetMargins( 2, 2 );
 
  // Creation of the statusbar
  CreateStatusBar( 1 );

  // NOTICE : the menu bar is created for each AmayaFrame, 
  //          the menu bar is not managed by the window
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
 *      Method:  CreatePage
 * Description:  create a new AmayaPage, the notebook will be the parent page
 *               it's possible to attach automaticaly this page to the window or not
 *--------------------------------------------------------------------------------------
 */
AmayaPage * AmayaWindow::CreatePage( bool attach, int position )
{
  AmayaPage * p_page = new AmayaPage( m_pNotebook );
  
  if (attach)
    AttachPage( position, p_page );
  
  return p_page;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  AttachPage
 * Description:  really attach a page to the current window
 *--------------------------------------------------------------------------------------
 */
bool AmayaWindow::AttachPage( int position, AmayaPage * p_page )
{
  bool ret;
  if (!m_pNotebook)
    ret = false;
  else
  {
    /* notebook is a new parent for the page
     * warning: AmayaPage original parent must be a wxNotbook */
//    p_page->Reparent( m_pNotebook );
    p_page->SetNotebookParent( m_pNotebook );
    
    /* insert the page in the notebook */
    ret = m_pNotebook->InsertPage( position,
       	                           p_page,
                                   _T("") ); /* this is the page name */
    //ret = m_pNotebook->AddPage( p_page, _T("Nom de la Page") );

    // update the pages ids
    m_pNotebook->UpdatePageId();

    SetAutoLayout(TRUE);
  }
  return ret;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  DetachPage
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaWindow::DetachPage( int position )
{
  return false;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  GetPage
 * Description:  search the page at given position
 *--------------------------------------------------------------------------------------
 */
AmayaPage * AmayaWindow::GetPage( int position )
{
  if (!m_pNotebook)
    return NULL;
  if (GetPageCount() <= position)
    return NULL;
  return (AmayaPage *)m_pNotebook->GetPage(position);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  GetPage
 * Description:  how many page into the window
 *--------------------------------------------------------------------------------------
 */
int AmayaWindow::GetPageCount() const
{
  if (!m_pNotebook)
    return 0;
  return (int)m_pNotebook->GetPageCount();
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
  // remove the menu when the window is going to die
  DesactivateMenuBar();

  m_IsClosing = TRUE;

  // Ask the notebook to close its pages
  if (m_pNotebook)
    m_pNotebook->OnClose( event );

  // simulate a idle event to force the windows to be closed
  // (maybe a bug in wxWindow)
  wxIdleEvent idle_event;
  wxPostEvent(this, idle_event);

  m_IsClosing = FALSE;
  
  // reactivate the menubar if the windows is still alive
  ActivateMenuBar();
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
  long     id     = event.GetId();

  wxMenuItem * p_menu_item = NULL;
  if (GetMenuBar())
  {
	  p_menu_item = GetMenuBar()->FindItem(id);
	  if (!p_menu_item)
	  {
		  wxASSERT_MSG(FALSE,_T("Menu item doesnt existe"));
		  return;
	  }
  }
/*  wxLogDebug( _T("AmayaWindow::OnMenuItem : p_menu = 0x%x\tmenuid = %d"), p_menu, id );
  wxLogDebug( _T("\tp_menu  IsKindOf wxMenuItem %s"), p_menu->IsKindOf(CLASSINFO(wxMenuItem)) ? _T("yes") : _T("no") );
  wxLogDebug( _T("\tp_menu  IsKindOf wxMenu %s"), p_menu->IsKindOf(CLASSINFO(wxMenu)) ? _T("yes") : _T("no") );

  wxMenuItem * p_menu_item = p_menu->FindItem( event.GetId() );
  wxLogDebug( _T("\tp_menu_item = 0x%x"), p_menu_item );
  wxLogDebug( _T("\tp_menu_item  IsKindOf wxMenuItem %s"), p_menu_item->IsKindOf(CLASSINFO(wxMenuItem)) ? _T("yes") : _T("no") );
*/
  AmayaContext * p_context = (AmayaContext *)p_menu_item->GetRefData();
  if (p_context)
    {
      void * p_data = p_context->GetData();
      
      // call the generic callback
      CallMenuWX( (ThotWidget)p_menu_item, p_data );
    }
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

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  IsClosing
 * Description:  return true if the window is going to be closed
 *--------------------------------------------------------------------------------------
 */
bool AmayaWindow::IsClosing()
{
  return m_IsClosing;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  DesactivateMenuBar
 * Description:  desactivate the current window menu bar
 *               (used when the windows is going to be closed)
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::DesactivateMenuBar()
{
  SetMenuBar( NULL );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  ActivateMenuBar
 * Description:  activate the menubar : look what is the current active frame and get
 *               its menu bar.
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::ActivateMenuBar()
{
  // TODO : aller chercher la bonne bar de menu
  
  // If the current windows is goind to die, do not activate the menubar
  if (m_IsClosing)
    return;

  Document doc;
  View     view;
  TtaGetActiveView( &doc, &view );
  
  if ( doc < 1 || doc > MAX_DOCUMENTS )
    return;

  int window_id;
  int page_id;
  int page_position;
  window_id = TtaGetDocumentWindowId( doc, view );
  TtaGetDocumentPageId( doc, view, &page_id, &page_position );

  AmayaWindow * p_AmayaWindow = WindowsTable[window_id];

  if ( p_AmayaWindow != this )
    {
      // the active page is not into the current window
      // so exit
      return;
    }

  AmayaPage * p_AmayaPage = p_AmayaWindow->GetPage( page_id );

  if (!p_AmayaPage)
    return;

  AmayaFrame * p_AmayaFrame = p_AmayaPage->GetFrame( 1 /* TODO : indiquer la frame top ou bottom en fonction de la vue (view) */ );

  if (!p_AmayaFrame)
    return;

  SetMenuBar( p_AmayaFrame->GetMenuBar() );
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaWindow, wxFrame)

  // when a menu item is pressed  
  EVT_MENU_RANGE( AmayaWindow::MENU_ITEM_START, AmayaWindow::MENU_ITEM_END, AmayaWindow::OnMenuItem ) 
  EVT_MENU( -1, AmayaWindow::OnMenuItem )

  // when a toolbar button is pressed  
  EVT_TOOL_RANGE( AmayaWindow::TOOLBAR_TOOL_START, AmayaWindow::TOOLBAR_TOOL_END, AmayaWindow::OnToolBarTool ) 
  
  EVT_CLOSE( AmayaWindow::OnClose )


//  EVT_SPLITTER_SASH_POS_CHANGED( -1, AmayaWindow::OnSplitterPosChanged )
//  EVT_SPLITTER_DCLICK( -1, AmayaWindow::OnSplitterDClick )
  EVT_SPLITTER_UNSPLIT( -1, AmayaWindow::OnSplitterUnsplit )
  
//  EVT_SIZE( AmayaWindow::OnSize )

END_EVENT_TABLE()

#endif /* #ifdef _WX */
