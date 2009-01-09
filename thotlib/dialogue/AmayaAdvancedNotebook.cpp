/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/string.h"
#include "wx/aui/auibook.h"
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


#include "AmayaAdvancedNotebook.h"
#include "AmayaPage.h"
#include "AmayaWindow.h"
#include "AmayaNormalWindow.h"
#include "AmayaHelpWindow.h"
#include "AmayaFrame.h"
#include "AmayaCanvas.h"
#include "AmayaApp.h"
#include "AmayaConfirmCloseTab.h"

IMPLEMENT_CLASS(AmayaAdvancedNotebook, wxAuiNotebook)

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaAdvancedNotebook::AmayaAdvancedNotebook(wxWindow * window , wxWindowID id)
  :  wxAuiNotebook( window, id,
                 wxDefaultPosition, wxDefaultSize,
                 wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE |  
                 wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ALL_TABS |
                 wxAUI_NB_WINDOWLIST_BUTTON),
     m_imageList(NULL)
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
AmayaAdvancedNotebook::~AmayaAdvancedNotebook()
{
}

/*----------------------------------------------------------------------
  Class:  AmayaAdvancedNotebook
  Method:  InsertPage
  Description: Insert a page.
 -----------------------------------------------------------------------*/
bool AmayaAdvancedNotebook::InsertPage(size_t index, AmayaPage* page,
				       const wxString& text, bool select, int imageId)
{
#ifndef _MACOS
  if (imageId >= 0)
    {
      wxBitmap bmp(m_imageList->GetIcon(imageId));
      return wxAuiNotebook::InsertPage(index, page, text, select, bmp);
    }
  else
#endif /* _MACOS */
    return wxAuiNotebook::InsertPage(index, page, text, select, wxNullBitmap);
}

/*----------------------------------------------------------------------
  Class:  AmayaAdvancedNotebook
  Method:  SetPageImage
  Description: Close a page.
  Return: false if cant close it.
 -----------------------------------------------------------------------*/
bool AmayaAdvancedNotebook::SetPageImage(size_t page, int image)
{
#ifndef _MACOS
  if(m_imageList && image!=wxID_ANY && image<m_imageList->GetImageCount())
    {
      wxBitmap bmp(m_imageList->GetIcon(image));
      SetPageBitmap(page, bmp);
      return true;
    }
#endif /* _MACOS */
  return false;
}

/*----------------------------------------------------------------------
  Class:  AmayaAdvancedNotebook
  Method:  ClosePage
  Description: Close a page.
  Return: false if cannot close it.
 -----------------------------------------------------------------------*/
bool AmayaAdvancedNotebook::ClosePage(int page_id)
{
  PtrDocument         pDoc;
  AmayaPage          *page = (AmayaPage*)GetPage( page_id );
  AmayaFrame         *frame;
  int                 view;
  bool                result = true;

  if (page == NULL)
    return result;

  if (GetPageCount() == 1)
    {
      frame = page->GetActiveFrame();
      GetDocAndView (frame->GetFrameId(), &pDoc, &view);
      if (pDoc && pDoc->DocDName && !strcmp (pDoc->DocDName, "empty"))
        // don't close a unique empty page
        return false;
      if (!GetAmayaWindow()->IsKindOf(CLASSINFO(AmayaHelpWindow))  &&
          AmayaNormalWindow::GetNormalWindowCount() == 1)
        {
          TtaExecuteMenuAction("NewTab", 1, 1, FALSE);
          result = false;
        }
      else
        {
#ifdef _MACOS
          // Prevent an Amaya crash
          return false;
#endif
        }
    }

  page->Hide();
  page = (AmayaPage*)GetPage( page_id );
  if (page == NULL)
    return result;

  if (page->Close())
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
 *       Class:  AmayaAdvancedNotebook
 *      Method:  CloseAllButPage
 * Description:  Close all notebook pages but not one.
 -----------------------------------------------------------------------*/
bool AmayaAdvancedNotebook::CloseAllButPage(int position)
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
 *       Class:  AmayaAdvancedNotebook
 *      Method:  CleanUp
 * Description:  check that there is no empty pages
 -----------------------------------------------------------------------*/
void AmayaAdvancedNotebook::CleanUp()
{
  int pos, count = GetPageCount();

  if (count > 0)
  {
    for(pos = count-1; pos > 0; pos--)
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
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedNotebook
 *      Method:  OnClose
 * Description:  Intercept the CLOSE event and prevent it if ncecessary.
  -----------------------------------------------------------------------*/
void AmayaAdvancedNotebook::OnClose(wxCloseEvent& event)
{
  int page_id = 0, count = GetPageCount();

  /* show a warning if there is more than one tab */
  if (count > 1 && AmayaConfirmCloseTab::DoesUserWantToShowMe())
  {
    AmayaConfirmCloseTab dlg(this, GetPageCount());
    if ( dlg.ShowModal() != wxID_OK)
    {
      event.Veto();
      return;
    }        
  }

  for (page_id = count-1; page_id >= 0; page_id--)
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
 *       Class:  AmayaAdvancedNotebook
 *      Method:  OnClosePage
 * Description:  Intercept the PAGE CLOSE event and prevent it if ncecessary.
  -----------------------------------------------------------------------*/
void AmayaAdvancedNotebook::OnClosePage(wxAuiNotebookEvent& event)
{
  PtrDocument         pDoc;
  int                 view;
  int                 page_id   = wxID_ANY;
  AmayaFrame         *frame;

  if (event.GetSelection() != wxID_ANY)
    page_id = event.GetSelection();
  else if (GetSelection() != wxID_ANY)
    page_id = GetSelection();

  if (page_id != wxID_ANY)
    {
      AmayaPage* page = wxDynamicCast(GetPage(page_id), AmayaPage);
      if (page)
        {
          frame = page->GetActiveFrame();
          GetDocAndView (frame->GetFrameId(), &pDoc, &view);
          TtaExecuteMenuAction("AmayaCloseTab", (Document)IdentDocument(pDoc), view, FALSE);
        }
    }
}


/*----------------------------------------------------------------------
  Class:  AmayaAdvancedNotebook
  Method:  UpdatePageId
  Description:  this function is called to update the page id and document's pageid
  when a page has been removed or moved ... 
 -----------------------------------------------------------------------*/
void AmayaAdvancedNotebook::UpdatePageId()
{
  /* update page_id for each page */
  unsigned int page_id = 0, count = GetPageCount();

  if (count == 1)
    SetTabCtrlHeight(0);
  else
    SetTabCtrlHeight(-1);
  while (page_id < count)
    {
      AmayaPage * p_page = (AmayaPage *)GetPage(page_id);
      if (!p_page->IsClosed())
        {
          p_page->SetPageId( page_id );
        }
      else
        {
          //wxASSERT_MSG(FALSE, _T("y a un problem, on essaye de mettre a jour une page qui est deja fermee") );
        }
      page_id++;
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedNotebook
 *      Method:  OnPageChanging
 * Description:  The page selection is about to be changed.
 *               Processes a wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING event.
 *               This event can be vetoed.
 -----------------------------------------------------------------------*/
#ifdef __WXDEBUG__
void AmayaAdvancedNotebook::OnPageChanging(wxAuiNotebookEvent& event)
{
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaAdvancedNotebook::OnPageChanging : old=%d, new=%d"),
                 event.GetOldSelection(),
                 event.GetSelection() );
  event.Skip();
}
#endif /* __WXDEBUG__ */

/*----------------------------------------------------------------------
 *       Class:  AmayaAdvancedNotebook
 *      Method:  OnPageChanged
 * Description:  called when a new page has been selected
 -----------------------------------------------------------------------*/
void AmayaAdvancedNotebook::OnPageChanged(wxAuiNotebookEvent& event)
{
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaAdvancedNotebook::OnPageChanged : old=%d, new=%d"),
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
  Class:  AmayaAdvancedNotebook
  Method:  GetPageId
  Description:  used to get the page Id when only the page @ is known
 -----------------------------------------------------------------------*/
int AmayaAdvancedNotebook::GetPageId( const AmayaPage * p_page )
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
AmayaWindow * AmayaAdvancedNotebook::GetAmayaWindow()
{
  return wxDynamicCast(wxGetTopLevelParent(this), AmayaWindow);
} 



void AmayaAdvancedNotebook::OnMouseRightDown(wxAuiNotebookEvent& event)
{
  int page_id   = GetSelection();

  /* Specific wxAuiNotebook protected code : */
  wxWindow* wnd;
  wxPoint pos = ScreenToClient(wxGetMousePosition());
  wxAuiTabCtrl* tab = GetTabCtrlFromPoint(pos);
  if(tab)
    {
      pos = tab->ScreenToClient(wxGetMousePosition());
      if (tab->TabHitTest(pos.x, pos.y, &wnd))
      {
        page_id = GetPageIndex(wnd);
      }
    }
  int window_id = GetAmayaWindow()->GetWindowId();
  wxPoint point = wxGetMousePosition();
  

  // store the aimed frame, it's possible that it is not the current active one
  if (page_id >= 0)
    {
      m_MContextFrameId = TtaGetFrameId ( window_id, page_id, 1 );
      wxMenu * p_menu = TtaGetContextMenu ( window_id );
      if(p_menu)
        PopupMenu (p_menu, ScreenToClient(point));
    }  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaAdvancedNotebook, wxAuiNotebook)
  EVT_AUINOTEBOOK_BUTTON(wxID_ANY, AmayaAdvancedNotebook::OnClosePage)
  
  EVT_CLOSE( AmayaAdvancedNotebook::OnClose )
  EVT_AUINOTEBOOK_PAGE_CHANGED(  -1, AmayaAdvancedNotebook::OnPageChanged )
#ifdef __WXDEBUG__  
  EVT_AUINOTEBOOK_PAGE_CHANGING( -1, AmayaAdvancedNotebook::OnPageChanging )
#endif /* __WXDEBUG__ */
  EVT_AUINOTEBOOK_TAB_RIGHT_DOWN( -1, AmayaAdvancedNotebook::OnMouseRightDown )
END_EVENT_TABLE()
  
#endif /* #ifdef _WX */
