#ifdef _WX
#include "wx/wx.h"
#include "wx/string.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"
#include "registry.h"
#include "appdialogue.h"
#include "message.h"
#include "dialogapi.h"
#include "application.h"
#include "appdialogue_wx.h"
#include "windowtypes_wx.h"
#include "registry_wx.h"

#include "appdialogue_wx_f.h"

#include "AmayaNotebook.h"
#include "AmayaPage.h"
#include "AmayaWindow.h"
#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaApp.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaNotebook, wxNotebook)

AmayaNotebook::AmayaNotebook( wxWindow * p_parent_window,
			      AmayaWindow * p_amaya_window )
	:  wxNotebook( wxDynamicCast(p_parent_window, wxWindow),
		       -1,
		       wxDefaultPosition, wxDefaultSize,
			   wxTAB_TRAVERSAL |
			   wxCLIP_CHILDREN |
			   wxFULL_REPAINT_ON_RESIZE |
			   wxNB_MULTILINE /* only windows */ )
	   ,m_pAmayaWindow( p_amaya_window )
	   ,m_ShouldLostFocus( FALSE )
{
  SetImageList( AmayaApp::GetDocumentIconList() );
}

AmayaNotebook::~AmayaNotebook()
{
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNotebook
 *      Method:  OnClose
 * Description:  called when the AmayaNotebook is closed.
 *               just forward close event to each AmayaPage
 *--------------------------------------------------------------------------------------
 */
void AmayaNotebook::OnClose(wxCloseEvent& event)
{
  wxLogDebug( _T("AmayaNotebook::OnClose") );

  /* if this boolean is set to false, the window must not be closed */
  bool close_window = true; 

  unsigned int page_id = 0;
  while ( page_id < GetPageCount() )
  {
    AmayaPage * p_page = (AmayaPage *)GetPage(page_id);
    p_page->OnClose(event);
    close_window &= p_page->IsClosed();

    /* really delete the page if the document has been closed */
    if (p_page->IsClosed())
      {
	//RemovePage(page_id);
	DeletePage(page_id);

	// maybe something has been removed so update the ids
	UpdatePageId();

	/* wait for pending events :
	   if a page is deleted, it throws notebookevent */
	while ( wxTheApp->Pending() )
	  wxTheApp->Dispatch();
      }
    else
      page_id++;
  }
  
  /* there is still open pages ? */
  if (close_window)
    {
      m_ShouldLostFocus = FALSE;
      event.Skip(); /* everything is closed => close the window */
    }
  else
      event.Veto(); /* still an opened page => keep the window open */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNotebook
 *      Method:  UpdatePageId
 * Description:  this function is called to update the page id and document's pageid
 *               when a page has been removed or moved ... 
 *--------------------------------------------------------------------------------------
 */
void AmayaNotebook::UpdatePageId()
{
  /* update page_id for each page */
  unsigned int page_id = 0;
  while ( page_id < GetPageCount() )
    {
      AmayaPage * p_page = (AmayaPage *)GetPage(page_id);
      if (!p_page->IsClosed())
	{
	  p_page->SetPageId( page_id );
	}
      else
	{
	  wxLogDebug( _T("y a un problem, on essaye de mettre a jour une page qui est deja fermee") );
	}
      page_id++;
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNotebook
 *      Method:  OnPageChanging
 * Description:  The page selection is about to be changed.
 *               Processes a wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING event.
 *               This event can be vetoed.
 *--------------------------------------------------------------------------------------
 */
void AmayaNotebook::OnPageChanging(wxNotebookEvent& event)
{
  wxLogDebug( _T("AmayaNotebook::OnPageChanging : old=%d, new=%d"),
              event.GetOldSelection(),
              event.GetSelection() );
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNotebook
 *      Method:  OnPageChanged
 * Description:  called when a new page has been selected
 *--------------------------------------------------------------------------------------
 */
void AmayaNotebook::OnPageChanged(wxNotebookEvent& event)
{
  wxLogDebug( _T("AmayaNotebook::OnPageChanged : old=%d, new=%d"),
              event.GetOldSelection(),
              event.GetSelection() );

  // do not change the page if this is the same as old one
  // this case can occure when notebook's pages are deleted ...
  if ( event.GetOldSelection() == event.GetSelection() || m_pAmayaWindow->IsClosing() )
    {
      event.Skip();
      return;
    }

  // Get the selected page
  AmayaPage * p_selected_page = (AmayaPage *)GetPage(event.GetSelection());

  // Update every page
  // important work is done when SetSelected is called :
  // + window title is updated in order to match current selected page
  unsigned int page_id = 0;
  while ( page_id < GetPageCount() )
  {
    AmayaPage * p_page = (AmayaPage *)GetPage(page_id);
    if ( !p_page->IsClosed() && p_page == p_selected_page )
    {
      p_page->SetSelected( TRUE );
    }
    else
    {
      p_page->SetSelected( FALSE );
    }
    page_id++;
  }
  
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNotebook
 *      Method:  GetPageId
 * Description:  used to get the page Id when only the page @ is known
 *--------------------------------------------------------------------------------------
 */
int AmayaNotebook::GetPageId( const AmayaPage * p_page )
{
  unsigned int page_id = 0;
  bool found = false;
  while ( !found && page_id < GetPageCount() )
  {
    found = ( GetPage(page_id) == wxDynamicCast(p_page,wxWindow) );
    page_id++;
  }

  if (found)
    return page_id-1;
  else
    return -1;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNotebook
 *      Method:  OnSetFocus
 * Description:  this callback is called when a the notebook get focus.
 *               notebook sould never get focus so when this callback is called,
 *               amaya ask active frame to distribute focus to corresponding active canvas
 *--------------------------------------------------------------------------------------
 */
void AmayaNotebook::OnSetFocus( wxFocusEvent & event )
{
  wxLogDebug( _T("AmayaNotebook::OnSetFocus") );

  m_ShouldLostFocus = TRUE;

  event.Skip();
}

void AmayaNotebook::OnIdle( wxIdleEvent& event )
{
  if ( m_ShouldLostFocus )
    {
      m_ShouldLostFocus = FALSE;
      // when notebook receive focus, the page is warned and can redistribute focus to the right widgets
      // in order to handle characteres.
      AmayaPage * p_selected_page = (AmayaPage *)GetPage(GetSelection());
      if (p_selected_page)
	p_selected_page->SetSelected( TRUE );	
    }
  
  event.Skip();
}

void AmayaNotebook::OnContextMenu( wxContextMenuEvent & event )
{
  wxLogDebug( _T("AmayaNotebook::OnContextMenu - (x,y)=(%d,%d)"),
	      event.GetPosition().x,
	      event.GetPosition().y );

  long flags  = 0;
  int tab_pos = HitTest(ScreenToClient(event.GetPosition()), &flags);
  wxLogDebug( _T("AmayaNotebook::OnContextMenu - tab_pos=%d, flags=%d"), tab_pos, flags);

  wxMenu * p_menu = TtaGetContextMenu( m_pAmayaWindow->GetWindowId(), tab_pos, -1 );
  PopupMenu(p_menu, ScreenToClient(event.GetPosition()));

  event.Skip();
}

BEGIN_EVENT_TABLE(AmayaNotebook, wxNotebook)
  EVT_CLOSE(	                 AmayaNotebook::OnClose )
  EVT_NOTEBOOK_PAGE_CHANGED(  -1, AmayaNotebook::OnPageChanged )
  EVT_NOTEBOOK_PAGE_CHANGING( -1, AmayaNotebook::OnPageChanging )

  EVT_SET_FOCUS(        AmayaNotebook::OnSetFocus )
  EVT_IDLE(             AmayaNotebook::OnIdle) // Process a wxEVT_IDLE event

  EVT_CONTEXT_MENU(              AmayaNotebook::OnContextMenu )
END_EVENT_TABLE()
  
#endif /* #ifdef _WX */
