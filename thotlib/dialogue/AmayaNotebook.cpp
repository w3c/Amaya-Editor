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
#include "logdebug.h"

#include "appdialogue_wx_f.h"

#include "AmayaNotebook.h"
#include "AmayaPage.h"
#include "AmayaWindow.h"
#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaApp.h"
#include "AmayaConfirmCloseTab.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaNotebook, wxNotebook)

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaNotebook::AmayaNotebook( wxWindow * p_parent_window,
                              AmayaWindow * p_amaya_window )
  :  wxNotebook( wxDynamicCast(p_parent_window, wxWindow),
                 -1,
                 wxDefaultPosition, wxDefaultSize,
                 wxTAB_TRAVERSAL |
                 wxCLIP_CHILDREN |
                 wxFULL_REPAINT_ON_RESIZE |
                 wxNB_MULTILINE /* only windows */,
                 wxT("AmayaNotebook") )
     ,m_pAmayaWindow( p_amaya_window )
     ,m_MContextFrameId(0)
{
  SetImageList( AmayaApp::GetDocumentIconList() );
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaNotebook::~AmayaNotebook()
{
}

/*----------------------------------------------------------------------
  Class:  AmayaNotebook
  Method:  ClosePage
  Description: Close a page.
  Return: false if cant close it.
 -----------------------------------------------------------------------*/
bool AmayaNotebook::ClosePage(int page_id)
{
  AmayaPage *page = (AmayaPage*)GetPage( page_id );
  page->Hide();
  
  if(page->Close())
  {
    DeletePage(page_id);
    /** \todo Update selection with old selection page. */
    UpdatePageId();    
    return true;
  }
  else
  {
    page->Show();
    return false;
  }
}


/*----------------------------------------------------------------------
 *       Class:  AmayaNotebook
 *      Method:  CloseAllButPage
 * Description:  Close all notebook pages but not one.
 -----------------------------------------------------------------------*/
bool AmayaNotebook::CloseAllButPage(int position)
{
  int pos;
  AmayaPage* sel = (AmayaPage*) GetPage(position);
  
  // Set the selection to avoid focus change.
  SetSelection(position);
  
  for(pos = GetPageCount()-1; pos>=0; pos--)
  {
    AmayaPage* page = (AmayaPage*) GetPage(pos);
    if(page!=sel)
    {
      if(page->Close())
        DeletePage(pos);
    }    
  }
  UpdatePageId();
  sel->SetSelected( TRUE );
  return true;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaNotebook
 *      Method:  CleanUp
 * Description:  check that there is no empty pages
 -----------------------------------------------------------------------*/
void AmayaNotebook::CleanUp()
{
  int pos;
  
  for(pos=GetPageCount()-1; pos>0; pos--)
  {
    AmayaPage *page = (AmayaPage*) GetPage(pos);
    if(page->CleanUp())
    {
      page->Close(true);
      DeletePage(pos);
    }
  }
  UpdatePageId();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNotebook
 *      Method:  OnClose
 * Description:  Intercept the CLOSE event and prevent it if ncecessary.
  -----------------------------------------------------------------------*/
void AmayaNotebook::OnClose(wxCloseEvent& event)
{
  /* show a warning if there is more than one tab */
  if (GetPageCount() > 1 && AmayaConfirmCloseTab::DoesUserWantToShowMe())
  {
    AmayaConfirmCloseTab dlg(this, GetPageCount());
    if ( dlg.ShowModal() != wxID_OK)
    {
      event.Veto();
      return;
    }        
  }

  int page_id = 0;
  for(page_id=GetPageCount()-1; page_id>=0; page_id--)
  {
    AmayaPage * page = (AmayaPage *)GetPage(page_id);
    if(page->Close())
    {
      DeletePage(page_id);
    }
  }
  if(GetPageCount() > 0)
  {
    UpdatePageId();
    event.Veto();
  }
}

/*----------------------------------------------------------------------
  Class:  AmayaNotebook
  Method:  UpdatePageId
  Description:  this function is called to update the page id and document's pageid
  when a page has been removed or moved ... 
 -----------------------------------------------------------------------*/
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
          wxASSERT_MSG(FALSE, _T("y a un problem, on essaye de mettre a jour une page qui est deja fermee") );
        }
      page_id++;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaNotebook
 *      Method:  OnPageChanging
 * Description:  The page selection is about to be changed.
 *               Processes a wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING event.
 *               This event can be vetoed.
 -----------------------------------------------------------------------*/
#ifdef __WXDEBUG__
void AmayaNotebook::OnPageChanging(wxNotebookEvent& event)
{
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaNotebook::OnPageChanging : old=%d, new=%d"),
                 event.GetOldSelection(),
                 event.GetSelection() );
  event.Skip();
}
#endif /* __WXDEBUG__ */

/*----------------------------------------------------------------------
 *       Class:  AmayaNotebook
 *      Method:  OnPageChanged
 * Description:  called when a new page has been selected
 -----------------------------------------------------------------------*/
void AmayaNotebook::OnPageChanged(wxNotebookEvent& event)
{
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaNotebook::OnPageChanged : old=%d, new=%d"),
                 event.GetOldSelection(),
                 event.GetSelection() );

  // do not change the page if this is the same as old one
  // this case can occure when notebook's pages are deleted ...
  if ( event.GetOldSelection() == event.GetSelection() )
    {
      event.Skip();
      return;
    }

  // Get the selected page
  if(event.GetSelection() < (int)GetPageCount() && event.GetSelection() >= 0)
  {
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
  }    
  event.Skip();
}

/*----------------------------------------------------------------------
  Class:  AmayaNotebook
  Method:  GetPageId
  Description:  used to get the page Id when only the page @ is known
 -----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void AmayaNotebook::OnContextMenu( wxContextMenuEvent & event )
{
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaNotebook::OnContextMenu - (x,y)=(%d,%d)"),
                 event.GetPosition().x,
                 event.GetPosition().y );

  int window_id = m_pAmayaWindow->GetWindowId();
  long flags    = 0;
  int page_id   = 0;
  wxPoint point = event.GetPosition();
  wxPoint origin = GetClientAreaOrigin();
#ifdef _MACOS
  point = ScreenToClient(point);
  point.y += origin.y; 
  page_id   = HitTest(point, &flags);
#else /* _MACOS */
  page_id   = HitTest(ScreenToClient(point), &flags);
#endif /* _MACOS */
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaNotebook::OnContextMenu - page_id=%d, flags=%d"), page_id, flags );

  // store the aimed frame, it's possible that it is not the current active one
  if (page_id >= 0)
    {
      m_MContextFrameId = TtaGetFrameId ( window_id, page_id, 1 );
      wxMenu * p_menu = TtaGetContextMenu ( window_id );
#ifdef _MACOS
      PopupMenu (p_menu, point);
#else /* _MACOS */
      PopupMenu (p_menu, ScreenToClient(point));
#endif /* _MACOS */
    }

  //  event.Skip();
}

/*----------------------------------------------------------------------
  Class:  AmayaNotebook
  Method:  GetMContextFrame
  Description:  return the aimed frame by the last context menu
 -----------------------------------------------------------------------*/
int AmayaNotebook::GetMContextFrame()
{
  return m_MContextFrameId;
}



/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaNotebook, wxNotebook)
  EVT_CLOSE( AmayaNotebook::OnClose )
  EVT_NOTEBOOK_PAGE_CHANGED(  -1, AmayaNotebook::OnPageChanged )
#ifdef __WXDEBUG__  
  EVT_NOTEBOOK_PAGE_CHANGING( -1, AmayaNotebook::OnPageChanging )
#endif /* __WXDEBUG__ */
  EVT_CONTEXT_MENU(               AmayaNotebook::OnContextMenu )
END_EVENT_TABLE()
  
#endif /* #ifdef _WX */
