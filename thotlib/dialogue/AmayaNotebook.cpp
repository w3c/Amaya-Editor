#ifdef _WX

#include "AmayaNotebook.h"
#include "AmayaPage.h"
#include "AmayaWindow.h"
#include "AmayaFrame.h"
#include "AmayaCanvas.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaNotebook, wxNotebook)

AmayaNotebook::AmayaNotebook( wxWindow * p_parent_window,
			      AmayaWindow * p_amaya_window )
	:  wxNotebook( wxDynamicCast(p_parent_window, wxWindow),
		       -1,
		       wxDefaultPosition, wxDefaultSize,
		       wxNB_FIXEDWIDTH )
          ,m_pAmayaWindow( p_amaya_window )
{
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

  int page_id = 0;
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
      event.Skip(); /* everything is closed => close the window */
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
  int page_id = 0;
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
  int page_id = 0;
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
  int page_id = 0;
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
 *      Method:  OnChar
 * Description:  this callback is called when a key is pressed.
 *               this event is then forwarded to current active AmayaCanvas.
 *               the reason why this event is not directly catch into AmayaCanvas is that
 *               on old GTK version (debian woody), the AmayaNotebook always keep the focus !
 *               So events are not throw to the canvas because it doesn't have focus.
 *--------------------------------------------------------------------------------------
 */
void AmayaNotebook::OnChar( wxKeyEvent& event )
{
  // get the selected page into the current notebook
  AmayaPage * p_page = m_pAmayaWindow->GetActivePage();
  if ( !p_page )
    {
      event.Skip();
      return;
    }
  
  // get the current active frame into the selected page
  AmayaFrame * p_frame = p_page->GetActiveFrame();
  if ( !p_frame )
    {
      event.Skip();
      return;
    }

  // get the frame's canvas
  AmayaCanvas * p_canvas = p_frame->GetCanvas();
  if ( !p_canvas )
    {
      event.Skip();
      return;
    }
  
  wxLogDebug( _T("AmayaNotebook::OnChar : frame=%d char=%x (skip)"),
	      p_frame->GetFrameId(),
	      event.GetKeyCode() );
  
  // forward the key event to current active canvas
  p_canvas->OnChar( event );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaNotebook
 *      Method:  OnKeydown
 * Description:  this callback is called when a key is pressed.
 *               this event is then forwarded to current active AmayaCanvas.
 *               the reason why this event is not directly catch into AmayaCanvas is that
 *               on old GTK version (debian woody), the AmayaNotebook always keep the focus !
 *               So events are not throw to the canvas because it doesn't have focus.
 *--------------------------------------------------------------------------------------
 */
void AmayaNotebook::OnKeyDown( wxKeyEvent& event )
{
  // get the selected page into the current notebook
  AmayaPage * p_page = m_pAmayaWindow->GetActivePage();
  if ( !p_page )
    {
      event.Skip();
      return;
    }
  
  // get the current active frame into the selected page
  AmayaFrame * p_frame = p_page->GetActiveFrame();
  if ( !p_frame )
    {
      event.Skip();
      return;
    }

  // get the frame's canvas
  AmayaCanvas * p_canvas = p_frame->GetCanvas();
  if ( !p_canvas )
    {
      event.Skip();
      return;
    }
  
  wxLogDebug( _T("AmayaNotebook::OnKeyDown : frame=%d key=%x (skip)"),
	      p_frame->GetFrameId(),
	      event.GetKeyCode() );
  
  // forward the key event to current active canvas
  p_canvas->OnKeyDown( event );
}

BEGIN_EVENT_TABLE(AmayaNotebook, wxNotebook)
  EVT_CLOSE(	                 AmayaNotebook::OnClose )
  EVT_NOTEBOOK_PAGE_CHANGED( -1, AmayaNotebook::OnPageChanged )

  EVT_KEY_DOWN(		AmayaNotebook::OnKeyDown) // Process a wxEVT_KEY_DOWN event (any key has been pressed). 
  EVT_CHAR(		AmayaNotebook::OnChar) // Process a wxEVT_CHAR event. 

END_EVENT_TABLE()
  
#endif /* #ifdef _WX */
