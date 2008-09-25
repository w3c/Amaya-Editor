/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "frame_tv.h"
#include "views_f.h"
#include "applicationapi_f.h"

#include "AmayaClassicNotebook.h"
#include "AmayaPage.h"
#include "AmayaWindow.h"
#include "AmayaNormalWindow.h"
#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaApp.h"
#include "AmayaConfirmCloseTab.h"

IMPLEMENT_CLASS(AmayaClassicNotebook, wxNotebook)

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaClassicNotebook::AmayaClassicNotebook(wxWindow * window , wxWindowID id)
  :  wxNotebook( window, id,
                 wxDefaultPosition, wxDefaultSize,
                 wxTAB_TRAVERSAL |
                 wxCLIP_CHILDREN |
                 wxFULL_REPAINT_ON_RESIZE |
                 wxNB_MULTILINE /* only windows */,
                 wxT("AmayaClassicNotebook") )
{
  m_MContextFrameId = 0;
  if (WindowBColor == -1)
    {
      // Initialize the window background colour
    wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
      WindowBColor = TtaGetThotColor (col.Red(), col.Green(), col.Blue());
    }
  SetImageList( AmayaApp::GetDocumentIconList() );
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaClassicNotebook::~AmayaClassicNotebook()
{
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaClassicNotebook::InsertPage(size_t index, AmayaPage* page, const wxString& text, bool select, int imageId)
{
  return wxNotebook::InsertPage(index, page, text, select, imageId);
}

/*----------------------------------------------------------------------
  Class:  AmayaClassicNotebook
  Method:  ClosePage
  Description: Close a page.
  Return: false if cant close it.
 -----------------------------------------------------------------------*/
bool AmayaClassicNotebook::ClosePage(int page_id)
{
  AmayaPage *page = (AmayaPage*)GetPage( page_id );
  bool result = true;

  page->Hide();
  if(GetPageCount()==1 &&
     AmayaNormalWindow::GetNormalWindowCount()==1)
	{
      TtaExecuteMenuAction("NewTab", 1, 1, FALSE);
	  result = false;
	}
  
  if(page->Close())
  {
    DeletePage(page_id);
    /** \todo Update selection with old selection page. */
    UpdatePageId();    
    return result;
  }
  else
  {
    page->Show();
    return false;
  }
}


/*----------------------------------------------------------------------
 *       Class:  AmayaClassicNotebook
 *      Method:  CloseAllButPage
 * Description:  Close all notebook pages but not one.
 -----------------------------------------------------------------------*/
bool AmayaClassicNotebook::CloseAllButPage(int position)
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
 *       Class:  AmayaClassicNotebook
 *      Method:  CleanUp
 * Description:  check that there is no empty pages
 -----------------------------------------------------------------------*/
void AmayaClassicNotebook::CleanUp()
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
 *       Class:  AmayaClassicNotebook
 *      Method:  OnClose
 * Description:  Intercept the CLOSE event and prevent it if ncecessary.
  -----------------------------------------------------------------------*/
void AmayaClassicNotebook::OnClose(wxCloseEvent& event)
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
  Class:  AmayaClassicNotebook
  Method:  UpdatePageId
  Description:  this function is called to update the page id and document's pageid
  when a page has been removed or moved ... 
 -----------------------------------------------------------------------*/
void AmayaClassicNotebook::UpdatePageId()
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
 *       Class:  AmayaClassicNotebook
 *      Method:  OnPageChanging
 * Description:  The page selection is about to be changed.
 *               Processes a wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING event.
 *               This event can be vetoed.
 -----------------------------------------------------------------------*/
#ifdef __WXDEBUG__
void AmayaClassicNotebook::OnPageChanging(wxNotebookEvent& event)
{
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaClassicNotebook::OnPageChanging : old=%d, new=%d"),
                 event.GetOldSelection(),
                 event.GetSelection() );
  event.Skip();
}
#endif /* __WXDEBUG__ */

/*----------------------------------------------------------------------
 *       Class:  AmayaClassicNotebook
 *      Method:  OnPageChanged
 * Description:  called when a new page has been selected
 -----------------------------------------------------------------------*/
void AmayaClassicNotebook::OnPageChanged(wxNotebookEvent& event)
{
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaClassicNotebook::OnPageChanged : old=%d, new=%d"),
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
  Class:  AmayaClassicNotebook
  Method:  GetPageId
  Description:  used to get the page Id when only the page @ is known
 -----------------------------------------------------------------------*/
int AmayaClassicNotebook::GetPageId( const AmayaPage * p_page )
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
AmayaWindow * AmayaClassicNotebook::GetAmayaWindow()
{
  return wxDynamicCast(wxGetTopLevelParent(this), AmayaWindow);
} 

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void AmayaClassicNotebook::OnContextMenu( wxContextMenuEvent & event )
{
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaClassicNotebook::OnContextMenu - (x,y)=(%d,%d)"),
                 event.GetPosition().x,
                 event.GetPosition().y );

  int window_id = GetAmayaWindow()->GetWindowId();
  long flags    = 0;
  int page_id   = 0;
  wxPoint point = event.GetPosition();
  wxPoint origin = GetClientAreaOrigin();
#ifdef _MACOS_26
  point = ScreenToClient(point);
  point.y += origin.y; 
  page_id   = HitTest(point, &flags);
#else /* _MACOS */
  page_id   = HitTest(ScreenToClient(point), &flags);
#endif /* _MACOS */
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaClassicNotebook::OnContextMenu - page_id=%d, flags=%d"), page_id, flags );

  // store the aimed frame, it's possible that it is not the current active one
  if (page_id >= 0)
    {
      m_MContextFrameId = TtaGetFrameId ( window_id, page_id, 1 );
      wxMenu * p_menu = TtaGetContextMenu ( window_id );
#ifdef _MACOS_26
      PopupMenu (p_menu, point);
#else /* _MACOS */
      PopupMenu (p_menu, ScreenToClient(point));
#endif /* _MACOS */
    }

  //  event.Skip();
}

/*----------------------------------------------------------------------
  Class:  AmayaClassicNotebook
  Method:  OnMouseMiddleUp
  Description:  Handle Mouse middle clic on a tab
 -----------------------------------------------------------------------*/
void AmayaClassicNotebook::OnMouseMiddleUp(wxMouseEvent& event)
{
  int page_id   = wxID_ANY;
  long flags    = 0;
  page_id   = HitTest(event.GetPosition(), &flags);
  if (page_id != wxID_ANY)
    {
      AmayaPage* page = wxDynamicCast(GetPage(page_id), AmayaPage);
      if(page)
        {
          PtrDocument         pDoc;
          int                 view;
          GetDocAndView (page->GetActiveFrame()->GetFrameId(), &pDoc, &view);
          TtaExecuteMenuAction("AmayaCloseTab", (Document)IdentDocument(pDoc), view, FALSE);
        }
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaClassicNotebook, wxNotebook)
  EVT_CLOSE( AmayaClassicNotebook::OnClose )
  EVT_NOTEBOOK_PAGE_CHANGED(  -1, AmayaClassicNotebook::OnPageChanged )
#ifdef __WXDEBUG__  
  EVT_NOTEBOOK_PAGE_CHANGING( -1, AmayaClassicNotebook::OnPageChanging )
#endif /* __WXDEBUG__ */
  EVT_CONTEXT_MENU(               AmayaClassicNotebook::OnContextMenu )
  EVT_MIDDLE_UP(AmayaClassicNotebook::OnMouseMiddleUp)
END_EVENT_TABLE()
  
#endif /* #ifdef _WX */
