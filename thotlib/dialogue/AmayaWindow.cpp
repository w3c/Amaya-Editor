#ifdef _WX

#include "wx/wx.h"
#include "wx/tglbtn.h"
#include "wx/string.h"


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
#include "frame.h"

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
#include "AmayaURLBar.h"
#include "AmayaToolBar.h"
#include "AmayaPanel.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaWindow, wxFrame)

/* contains the last activated window id. */
int AmayaWindow::m_ActiveWindowId = -1;

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
			    ,int kind
			    ,long style
		       ) : 
  wxFrame( wxDynamicCast(p_parent_window, wxWindow),
	   -1, _T(""), pos, size, style ),
  m_WindowId( window_id ),
  m_IsClosing( FALSE ),
  m_Kind( kind ),
  m_ShouldCleanUp( false ),
  m_ActiveFrameId( 0 )
{
  wxLogDebug( _T("AmayaWindow::AmayaWindow: window_id=%d"), m_WindowId );
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
  wxLogDebug( _T("AmayaWindow::~AmayaWindow: window_id=%d"), m_WindowId );
  // empty the current window entry
  memset(&WindowTable[m_WindowId], 0, sizeof(Window_Ctl));
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  SetupURLBar
 * Description:  create and add the url to the toolbar if it's not allready done
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::SetupURLBar()
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
  return NULL;
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
  return false;
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
 *      Method:  ClosePage
 * Description:  ferme une page
 *--------------------------------------------------------------------------------------
 */
bool AmayaWindow::ClosePage( int position )
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
AmayaPage * AmayaWindow::GetPage( int position ) const
{
  return NULL;
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
  return 0;
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
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  GetActivePage
 * Description:  return the current selected page
 *--------------------------------------------------------------------------------------
 */
AmayaPage * AmayaWindow::GetActivePage() const
{
  wxLogDebug( _T("AmayaWindow::GetActivePage") );
  return NULL;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  GetActiveFrame
 * Description:  return the current selected frame
 *--------------------------------------------------------------------------------------
 */
AmayaFrame * AmayaWindow::GetActiveFrame() const
{
  wxLogDebug( _T("AmayaWindow::GetActiveFrame") );
  return NULL;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  SetURL
 * Description:  set the current url value
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::SetURL ( const wxString & new_url )
{
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  GetURL
 * Description:  get the current url value
 *--------------------------------------------------------------------------------------
 */
wxString AmayaWindow::GetURL( )
{
  return wxString(_T(""));
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  SetWindowEnableURL
 * Description:  set the current url status (enable or disable)
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::SetEnableURL( bool urlenabled )
{
  return;
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
  return;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  EmptyURLBar
 * Description:  remove all items in the url bar
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::EmptyURLBar()
{
  return;
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
 *      Method:  SetMenuBar
 * Description:  override the wxFrame::SetMenuBar methode and check if the menu bar is NULL
 *               if NULL then replace the menubar with a dummy menu bar to avoid ugly effects when closing a frame
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::SetMenuBar( wxMenuBar * p_menu_bar )
{
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
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  OnSize
 * Description:  the window is resized, we must recalculate by hand the new urlbar size
 *               (wxWidgets is not able to do that itself ...)
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::OnSize( wxSizeEvent& event )
{
  wxLogDebug( _T("AmayaWindow::OnSize - ")+
	      wxString(_T(" w=%d h=%d")),
	      event.GetSize().GetWidth(),
	      event.GetSize().GetHeight() );

  // save the new window size
  WindowTable[GetWindowId()].FrWidth  = event.GetSize().GetWidth();
  WindowTable[GetWindowId()].FrHeight = event.GetSize().GetHeight();

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  GetAmayaToolBar
 * Description:  return the current toolbar
 *--------------------------------------------------------------------------------------
 */
AmayaToolBar * AmayaWindow::GetAmayaToolBar()
{
  return NULL;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  AttachFrame
 * Description:  for a SIMPLE window type, attach directly a frame to the window
 *--------------------------------------------------------------------------------------
 */
bool AmayaWindow::AttachFrame( AmayaFrame * p_frame )
{
  return false;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  DetachFrame
 * Description:  for a SIMPLE window type, detach directly a frame from the window
 *--------------------------------------------------------------------------------------
 */
AmayaFrame * AmayaWindow::DetachFrame()
{
  return NULL;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  CleanUp
 * Description:  check that there is no empty pages
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::CleanUp()
{
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  OnIdle
 * Description:  not used
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::OnIdle( wxIdleEvent& event )
{
  if (m_ShouldCleanUp)
    {
      m_ShouldCleanUp = false;
      /* now check that there is no empty pages */
      TtaCleanUpWindow( GetWindowId() );
    }

  event.Skip();
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  OnActivate
 * Description:  this is used to keep udated a internal window_id to know what is the
 *               last activated window.
 *               activated = highlighted
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::OnActivate( wxActivateEvent & event )
{  
  if (event.GetActive())
    {
      wxLogDebug( _T("AmayaWindow::OnActivate - window_id=%d"), m_WindowId );
      m_ActiveWindowId = GetWindowId();
    }

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  GetAmayaPanel
 * Description:  return the window's panel (exist only on AmayaNormalWindow)
 *--------------------------------------------------------------------------------------
 */
AmayaPanel * AmayaWindow::GetAmayaPanel() const
{
  return NULL;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  ClosePanel
 * Description:  close the side panel
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::ClosePanel()
{
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaWindow
 *      Method:  OpenPanel
 * Description:  open the side panel
 *--------------------------------------------------------------------------------------
 */
void AmayaWindow::OpenPanel()
{
}


/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaWindow, wxFrame)
  EVT_SIZE(      AmayaWindow::OnSize )
  EVT_IDLE(      AmayaWindow::OnIdle ) // Process a wxEVT_IDLE event  
  EVT_ACTIVATE(  AmayaWindow::OnActivate )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
