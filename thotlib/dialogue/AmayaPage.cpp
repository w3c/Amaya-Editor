#ifdef _WX

#include "wx/wx.h"
#include "wx/string.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "AmayaCanvas.h"

#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "frame.h"
#include "appdialogue_wx.h"
#include "registry_wx.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "font_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "font_f.h"
#include "appli_f.h"
#include "profiles_f.h"
#include "appdialogue_wx_f.h"
#include "appdialogue_f.h"
#include "boxparams_f.h"
#include "displayview_f.h"

#include "wx/log.h"

#include "AmayaNormalWindow.h"
#include "AmayaFrame.h"
#include "AmayaPage.h"
#include "AmayaNotebook.h"
#include "AmayaQuickSplitButton.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaPage, wxPanel)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  AmayaPage
 * Description:  constructor of the page :
 *   wxBoxSizer
 *   [
 *       wxSplitterWindow
 *       [
 *           AmayaFrame
 *           AmayaFrame
 *       ]
 *   ]
 *--------------------------------------------------------------------------------------
 */
AmayaPage::AmayaPage( wxWindow * p_parent_window, AmayaWindow * p_amaya_parent_window )
  :  wxPanel( p_parent_window, -1 )
     ,m_SlashRatio( 0.5 )
     ,m_IsClosed( FALSE )
     ,m_pNoteBookParent( NULL )
     ,m_pWindowParent( p_amaya_parent_window )
     ,m_PageId(-1)
     ,m_ActiveFrame(1) // by default, frame 1 is selected
     ,m_IsSelected(false)
     ,m_MasterFrameId(-1)
{
  // Insert a forground sizer
  m_pSizerTop = new wxBoxSizer ( wxVERTICAL );
  SetSizer( m_pSizerTop );
 
  // Insert a windows splitter 
  m_pSplitterWindow = new wxSplitterWindow( this, -1
					    ,wxDefaultPosition
					    ,wxDefaultSize
#if 0
					    ,/*wxSP_FULLSASH |*/
					    wxSP_3DSASH |
					    wxSP_BORDER |
					    wxSP_3D 
					    /* | wxSP_NOBORDER*/
					    /*| wxSP_PERMIT_UNSPLIT*/
#endif /* 0 */
					    );
  m_pSizerTop->Add( m_pSplitterWindow, 1, wxEXPAND, 0 );


  // Split button creation
  // this button is used to quickly split the page 
  m_pSplitButtonBottom = new AmayaQuickSplitButton( this, AmayaQuickSplitButton::wxAMAYA_QS_HORIZONTAL, 4 );
  m_pSizerTop->Add( m_pSplitButtonBottom, 0, wxEXPAND, 0 );
  m_pSplitButtonBottom->ShowQuickSplitButton( true );

  /// Insert to area : Top / bottom
  m_pTopFrame     = NULL;
  m_pBottomFrame  = NULL;
  m_pSplitterWindow->SetMinimumPaneSize( 50 );

  // Create a dummy panel to initilize the splitter window with something
  m_DummyPanel = new wxPanel( m_pSplitterWindow );
  m_pSplitterWindow->Initialize( m_DummyPanel );

  // refresh the correspondig menu item state
  RefreshSplitToggleMenu();

  // initialize the last open view name
  strcpy(m_LastOpenViewName, "Formatted_view");

  SetAutoLayout(TRUE);
}


/*
 *--------------------------------------------------------------------------------------
 *       Class: AmayaPage 
 *      Method: ~AmayaPage 
 * Description: destructor of AmayaPage (do nothing)
 *--------------------------------------------------------------------------------------
 */
AmayaPage::~AmayaPage()
{
}

/*
 *--------------------------------------------------------------------------------------
 *       Class: AmayaPage 
 *      Method: AttachFrame
 * Description: attache a AmayaFrame to the page (top or bottom)
 *      params:
 *        + AmayaFrame * p_frame : the frame to attach
 *        + int position : the position identifier - top (1) or bottom (2)
 *      return:
 *        + AmayaFrame * : the old frame or NULL if there was no old frame at this place
 *--------------------------------------------------------------------------------------
 */
AmayaFrame * AmayaPage::AttachFrame( AmayaFrame * p_frame, int position )
{
  // check if this is the first frame or not
  // the first one will be considered ad the master
  // the master frame is the on which control the urlbar, buttons (TODO : and menus)
  if ( !m_pTopFrame && !m_pBottomFrame )
    m_MasterFrameId = p_frame->GetFrameId();

  // Select the right frame
  AmayaFrame ** pp_frame_container = NULL;
  switch (position)
    {
    case 1:
      pp_frame_container = &m_pTopFrame;
      break;
    case 2:
      pp_frame_container = &m_pBottomFrame;
      break;
    default:
      wxASSERT_MSG(FALSE, _T("AmayaPage::AttachFrame -> Bad position"));
      return NULL;
      break;
    }

  // remember what is the oldframe
  AmayaFrame * oldframe = *pp_frame_container;
  if (p_frame == NULL || p_frame == oldframe )
    {
      if (!m_pSplitterWindow->IsSplit())
      	{
	  // on passe ici qd on essaye de charger un nouveau document dans la meme fenetre
	  // je ne sais pas si c necessaire de forcer le split
	  // de toute facon si on le fait il faut verifier que le top et le bottom ne sont pas null
	  // m_pSplitterWindow->SplitHorizontally( m_pTopFrame, m_pBottomFrame );
      	  AdjustSplitterPos();
      	}
      return NULL;
    }

  /* p_frame is the new top frame */  
  *pp_frame_container = p_frame;

#ifndef _MACOS
 /* do not reparent because on MacOSX it is not implemented */
  /* the frame needs a new parent ! */
  p_frame->Reparent( m_pSplitterWindow );
#endif /* _MACOS */
  bool ok = false;
  if (oldframe != NULL)
    ok = m_pSplitterWindow->ReplaceWindow( oldframe, p_frame );
  else if (m_pBottomFrame == NULL || m_pTopFrame == NULL)
    {
      ok = m_pSplitterWindow->ReplaceWindow( m_DummyPanel, p_frame );
      m_DummyPanel->Destroy();
      m_DummyPanel = NULL;
    }
  else
    {
      ok = m_pSplitterWindow->SplitHorizontally( m_pTopFrame, m_pBottomFrame );
      AdjustSplitterPos();
    }
  wxASSERT_MSG(ok, _T("AmayaPage::AttachFrame -> Impossible d'attacher la frame") );

  // update old and new AmayaFrame parents
  if (oldframe)
    oldframe->SetPageParent( NULL ); // no more parent
  if (p_frame)
    p_frame->SetPageParent( this ); // I'm your parent
 
  // update the page title (same as bottom frame)
  if (p_frame)
    p_frame->SetFrameTitle(p_frame->GetFrameTitle());

  if ( m_pTopFrame && m_pBottomFrame )
    {
      /* hide the split button */
      //      m_pSplitButtonBottom->ShowQuickSplitButton( false );
    }
  
  // try to avoid refresh because it forces a total canvas redraw (it's not very optimized)
  //p_frame->Refresh();

  SetAutoLayout(TRUE);

  // refresh the correspondig menu item state
  RefreshSplitToggleMenu();

  // remember the last open view
  strcpy(m_LastOpenViewName, FrameTable[p_frame->GetFrameId()].FrViewName);

  // return the old frame : needs to be manualy deleted ..
  return oldframe;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  DetachFrame
 * Description:  detache the frame (hide it but don't delete it)
 *      params:
 *        + 1 the top frame
 *        + 2 the bottom frame 
 *      return:
 *        + AmayaFrame* : the detached frame
 *--------------------------------------------------------------------------------------
 */
AmayaFrame * AmayaPage::DetachFrame( int position )
{
  // Select the right frame : top or bottom
  AmayaFrame ** pp_frame_container = NULL;
  AmayaFrame * p_other_frame = NULL;
  switch (position)
    {
    case 1:
      pp_frame_container = &m_pTopFrame;
      p_other_frame      = m_pBottomFrame;
      break;
    case 2:
      pp_frame_container = &m_pBottomFrame;
      p_other_frame      = m_pTopFrame;
      break;
    default:
      return NULL;
      break;
    }

  AmayaFrame * oldframe = *pp_frame_container;

  if (oldframe == NULL)
    return NULL;

  bool isUnsplited = m_pSplitterWindow->Unsplit( oldframe );
  if (!isUnsplited)
    {
      // The frame is alone and can't be unsplit
      // => replace it with a dummy panel
      m_DummyPanel = new wxPanel( m_pSplitterWindow );
      bool isReplaced = m_pSplitterWindow->ReplaceWindow( oldframe, m_DummyPanel );
      wxASSERT_MSG( isReplaced, _T("La frame n'a pas pu etre remplacee") );
    }

  // update old and new AmayaFrame parents
  if (oldframe)
    {
      oldframe->SetActive( FALSE );
      // no more parents
      //      oldframe->Reparent( NULL );
      //      oldframe->SetPageParent( NULL ); 
    }

  *pp_frame_container = NULL;

  // This frame is not anymore active
  // activate the other frame
  int active_frame_position = 1;
  while ( !GetFrame( active_frame_position ) && active_frame_position <= MAX_MULTI_FRAME )
    active_frame_position++;
  if ( active_frame_position <= MAX_MULTI_FRAME )
    {
      // there is another frame
      // so activate it
      AmayaFrame * p_frame = GetFrame( active_frame_position );
      SetActiveFrame( p_frame );
      if ( GetActiveFrame() )
	GetActiveFrame()->SetActive( TRUE );
      else
	wxASSERT(FALSE);
    }
  else
    {
      // there is no more frame
      SetActiveFrame( NULL );
    }

  if ( !(m_pTopFrame && m_pBottomFrame) )
    {
      /* show again the split button */
      //      m_pSplitButtonBottom->ShowQuickSplitButton( true );
    }
  
  // simulate a size event to refresh the canvas ...
  // this is usefull when a document is modified and there is many open views :
  // if nothing is done here when a view is detached from the page, a undraw
  // zone appears into the canvas on the other frame...
  // this is not very clean but it works (maybe try to remove this on further wxWidgets version)
  if (p_other_frame && p_other_frame->GetCanvas())
    {
      wxSizeEvent event( p_other_frame->GetCanvas()->GetSize() );
      wxPostEvent(p_other_frame->GetCanvas(), event );
    }

  // refresh the correspondig menu item state
  RefreshSplitToggleMenu();

  // check if there is no more frame in the page
  // if there is no more frame, the master frame must be erased 
  // the master frame is the on which control the urlbar, buttons (TODO : and menus)
  if ( !m_pTopFrame && !m_pBottomFrame )
    m_MasterFrameId = -1;

  return oldframe;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  OnSplitButton
 * Description:  this method is called when the button for quickly split is pushed
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::OnSplitButton( wxCommandEvent& event )
{
  if ( event.GetId() != m_pSplitButtonBottom->GetId() )
    {
      event.Skip();
      return;
    }

  DoSplitUnsplit();

  //  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  GetSplitterWindow
 * Description:  return the page's splitter window pointer 
 *--------------------------------------------------------------------------------------
 */
wxSplitterWindow * AmayaPage::GetSplitterWindow()
{
  return m_pSplitterWindow;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  DoSplitUnsplit
 * Description:  toggle split/unsplit state
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::DoSplitUnsplit()
{
  if (!m_pSplitterWindow->IsSplit())
    {
      // TODO: montrer la meme vue que la premiere frame
      AmayaFrame * p_frame = GetFrame(1);
      Document document = FrameTable[p_frame->GetFrameId()].FrDoc;
      View view         = FrameTable[p_frame->GetFrameId()].FrView;
      
      if ( !strcmp(m_LastOpenViewName, "Formatted_view") )
	TtaExecuteMenuAction ("ShowSource", document, view, FALSE);
      else if ( !strcmp(m_LastOpenViewName, "Links_view") )
	TtaExecuteMenuAction ("ShowLinks", document, view, FALSE);
      else if ( !strcmp(m_LastOpenViewName, "Alternate_view") )
	TtaExecuteMenuAction ("ShowAlternate", document, view, FALSE);
      else if ( !strcmp(m_LastOpenViewName, "Table_of_contents") )
	TtaExecuteMenuAction ("ShowToC", document, view, FALSE);
      else // if ( !strcmp(m_LastOpenViewName, "Structure_view") )
	TtaExecuteMenuAction ("ShowStructure", document, view, FALSE);

    }
  else
    {
      // unsplit the page
      DetachFrame(2);
    }
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  OnSplitterPosChanging
 * Description:  this method is called when the splitter position start to change its position
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::OnSplitterPosChanging( wxSplitterEvent& event )
{
  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  OnSplitterPosChanged
 * Description:  this method is called when the splitter position has changed
 *               this method calculate the new ratio (m_SlashRatio)
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::OnSplitterPosChanged( wxSplitterEvent& event )
{
    // calculate the new ratio (depending of window size)
    float new_slash_pos = event.GetSashPosition();
    float height = GetSize().GetHeight();
    float width = GetSize().GetWidth();
    int split_mode = m_pSplitterWindow->GetSplitMode();
    if ( split_mode == 1 ) // horizontally
      m_SlashRatio = new_slash_pos / height;
    else if ( split_mode == 2 ) // vertically
      m_SlashRatio = new_slash_pos / width;
    if ( m_SlashRatio <= 0 || m_SlashRatio >= 1 )
      m_SlashRatio = 0.5;
    
    // do not forward this event because the panel/notebook splitter should not receive it
    //    event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  OnSplitterDClick
 * Description:  called when a double click is done on the splitbar
 *               detach the bottom frame
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::OnSplitterDClick( wxSplitterEvent& event )
{
  DetachFrame(2);
  // dont forward this event because childs slitterwindows dont must catch this event (AmayaPanel for exemple)
  //  event.Skip();  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  OnSplitterUnsplit
 * Description:  called when the splitbar is unsplited
 *               TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::OnSplitterUnsplit( wxSplitterEvent& event )
{
  // the frame has been maybe unsplited manualy
  // maybe an update is needed
  int pos = GetFramePosition( wxDynamicCast(event.GetWindowBeingRemoved(), AmayaFrame) );
  switch ( pos )
    {
    case 1:
      m_pTopFrame = NULL;
      break;
    case 2:
      m_pBottomFrame = NULL;
      break;
    }

  // we should show the quick split bar when frames are unsplited
  //  m_pSplitButtonBottom->ShowQuickSplitButton( true );

  event.Skip();  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  OnSize
 * Description:  called when the AmayaPage is resized.
 *               assign a right value to the splitbar position
 *               depending of m_SlashRatio position
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::OnSize( wxSizeEvent& event )
{
  // do not update the size if the page is not selected
  if ( !IsSelected() )
  {
    event.Skip();
    return;
  }
  
  AdjustSplitterPos();

  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  AdjustSplitterPos
 * Description:  adjust the splitbar position,
 *               the split bar pos could be horizontal or vertical
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::AdjustSplitterPos( int height, int width )
{
  if ( height == -1 )
    height = GetSize().GetHeight();
  if ( width == -1 )
    width = GetSize().GetWidth();

  // force the splitter position (depending of window size)
  float new_height    = height;
  float new_width     = width;
  float new_slash_pos = 0;
  int split_mode      = m_pSplitterWindow->GetSplitMode();
  switch (split_mode)
    {
    case 1:
      new_slash_pos = m_SlashRatio * new_height;
      break;
    case 2:
      new_slash_pos = m_SlashRatio * new_width;
      break;
    default:
      wxASSERT(FALSE);
      break;
    }  
  if ( new_slash_pos > 0 && (GetFrame(1) && GetFrame(2)) )
    m_pSplitterWindow->SetSashPosition( (int)new_slash_pos );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  OnClose
 * Description:  called when the AmayaPage is closed.
 *               just call generic callbacks to close top frame and bottom frame
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::OnClose(wxCloseEvent& event)
{
  TTALOGDEBUG_2( TTA_LOG_DIALOG, _T("AmayaPage::OnClose topframe=%d bottomframe=%d"),
		m_pTopFrame ? m_pTopFrame->GetFrameId() : -1,
		m_pBottomFrame ? m_pBottomFrame->GetFrameId() : -1 );

  /* I suppose the page will be closed */
  /* but it can be override to FALSE if the top or bottom frame has been modified */
  m_IsClosed = TRUE;

  int frame_id = 0;
  AmayaFrame * p_AmayaFrame = NULL;
  // Kill top frame
  if ( m_pTopFrame )
    {
      p_AmayaFrame = m_pTopFrame;
      frame_id     = m_pTopFrame->GetFrameId();

      // try to close the frame : the user can choose to close or not with a dialog
      p_AmayaFrame->OnClose( event );
      
      // if the user don't want to close then just reattach the frame
      if ( !TtaFrameIsClosed (frame_id) )
	{
	  // if the frame didnt die, just re-attach it
	  AttachFrame(p_AmayaFrame, 1);
	  m_IsClosed = FALSE;
	}
    }

  // Kill bottom frame
  if ( m_pBottomFrame )
    { 
      p_AmayaFrame = m_pBottomFrame;
      frame_id     = m_pBottomFrame->GetFrameId();
      // try to close the frame : the user can choose to close or not with a dialog
      p_AmayaFrame->OnClose( event );
      
      // if the user don't want to close then just reattach the frame
      if ( !TtaFrameIsClosed (frame_id) )
	{
	  // if the frame didnt die, just re-attach it
	  AttachFrame(p_AmayaFrame, 2);
	  m_IsClosed = FALSE;
	}
      
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  OnPaint
 * Description:  nothing is done
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::OnPaint( wxPaintEvent& event )
{
  event.Skip(FALSE);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  SetNotebookParent
 * Description:  update the notebook page's parent
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::SetNotebookParent( AmayaNotebook * p_notebook )
{
  /* notebook is a new parent for the page
   * warning: AmayaPage original parent must be a wxNotbook */
  //Reparent( p_notebook ); /* do not reparent becaus on MacOSX it is not implemented */

  m_pNoteBookParent = p_notebook;

  if (m_pNoteBookParent->GetWindowParent() != GetWindowParent())
    SetWindowParent( m_pNoteBookParent->GetWindowParent() );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  GetNotebookParent
 * Description:  what is the parent of this page ?
 *--------------------------------------------------------------------------------------
 */
AmayaNotebook * AmayaPage::GetNotebookParent()
{
  return m_pNoteBookParent;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  SetWindowParent / GetWindowParent
 * Description:  set/get the top window parent
 *--------------------------------------------------------------------------------------
 */
AmayaWindow * AmayaPage::GetWindowParent()
{
  return m_pWindowParent;
}
void AmayaPage::SetWindowParent( AmayaWindow * p_window )
{
  m_pWindowParent = p_window;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  IsClosed
 * Description:  to know if this page is going do die
 *--------------------------------------------------------------------------------------
 */
bool AmayaPage::IsClosed()
{
  return m_IsClosed;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  IsSelected
 * Description:  to know if this page is currently selected
 *--------------------------------------------------------------------------------------
 */
bool AmayaPage::IsSelected()
{
#if 0

  if (m_pNoteBookParent)
  {
    int page_id = m_pNoteBookParent->GetPageId( this );
    return (page_id == m_pNoteBookParent->GetSelection());
  }
  else
    return FALSE;

#endif /* 0 */
  return m_IsSelected;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  SetSelected
 * Description:  the notebook has a new selected page then it calls SetSelected(TRUE)
 *               on the selected page and SetSelected(FALSE) on the other one
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::SetSelected( bool isSelected )
{
  m_IsSelected = isSelected;
  if (isSelected)
    {
      for ( int frame_pos = 1; frame_pos<=2; frame_pos++ )
	{
	  if ( GetFrame(frame_pos) )
	    {
	      // post a size event to force frame refresh
	      // to canvas
	      AmayaCanvas * p_canvas = GetFrame(frame_pos)->GetCanvas();
	      if ( p_canvas )
		{
		  wxSizeEvent event_canvas( p_canvas->GetSize() );
		  wxPostEvent( p_canvas, event_canvas );
		}
	      // to page
	      //	      wxSizeEvent event_page( GetSize() );
	      //	      wxPostEvent( this, event_page );
	    }
	}

      // if there is an active frame
      if ( GetActiveFrame() )
	{
	  // activate it : setup the corresponding menu and update internal boolean
	  GetActiveFrame()->SetActive( TRUE );
	}
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  SetPageId
 * Description:  update the page id (page position in the notebook)
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::SetPageId( int page_id )
{
  // update each owned frames
  int frame_id = 1;
  while( frame_id < MAX_FRAME )
    {
      if (!TtaFrameIsClosed(frame_id))
	{
	  // if this frame is owned by this page
	  if (FrameTable[frame_id].FrPageId == m_PageId)
	    {
	      FrameTable[frame_id].FrPageId = page_id;	      
	    }
	}
      frame_id++;
    }

  // update the internal page_id with the new one
  m_PageId = page_id;
}
/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  GetPageId
 * Description:  return the page position relatively to other window pages
 *--------------------------------------------------------------------------------------
 */
int AmayaPage::GetPageId()
{
  return m_PageId;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  GetFrame
 * Description:  return the first or second page frame
 *--------------------------------------------------------------------------------------
 */
AmayaFrame * AmayaPage::GetFrame( int frame_position ) const
{
  if (frame_position == 1)
    {
      return m_pTopFrame;
    }
  else if (frame_position == 2)
    {
      return m_pBottomFrame;
    }
  else
    return NULL;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  GetFramePosition
 * Description:  return the frame position if it exist in the current page
 *--------------------------------------------------------------------------------------
 */
int AmayaPage::GetFramePosition( const AmayaFrame * p_frame ) const
{
  if ( p_frame == GetFrame(1) )
    return 1;
  else if ( p_frame == GetFrame(2) )
    return 2;
  else
    return 0;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  DeletedFrame
 * Description:  force the frame to be detached from the page when the frame
 *               is deleted by itself
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::DeletedFrame( AmayaFrame * p_frame )
{
  if ( p_frame == GetFrame(1) )
    {
      DetachFrame(1);
    }
  if ( p_frame == GetFrame(2) )
    {
      DetachFrame(2);
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::SetActiveFrame( const AmayaFrame * p_frame )
{
  if ( p_frame == GetFrame(1) )
    m_ActiveFrame = 1;
  else if ( p_frame == GetFrame(2) )
    m_ActiveFrame = 2;
  else
    m_ActiveFrame = 0;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  
 * Description:  
 *--------------------------------------------------------------------------------------
 */
AmayaFrame * AmayaPage::GetActiveFrame() const
{
  return GetFrame( m_ActiveFrame );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  RaisePage
 * Description:  raise the page and the window if necessary
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::RaisePage()
{
  AmayaNotebook * p_notebook = GetNotebookParent();
  if (p_notebook)
    {
      // raise the notebook page
      p_notebook->SetSelection(GetPageId());
      
      SetSelected( TRUE );
    }
  // force the window's parent to raises to the top of the window hierarchy if it is a managed window (dialog or frame).
  if ( GetWindowParent() )
    GetWindowParent()->Raise();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::OnContextMenu( wxContextMenuEvent & event )
{
  event.Skip();
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  RefreshSplitToggleMenu
 * Description:  is called to toggle on/off the "split/unsplit view" menu item depeding on
 *               the page split state.
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::RefreshSplitToggleMenu()
{
  int window_id = GetWindowParent()->GetWindowId();
  int page_id   = GetPageId();

  // update menu items of each documents
  int doc_id    = 1;
  int frame_id  = 0;
  int itemID    = WindowTable[window_id].MenuItemSplitViewID;
  int action    = FindMenuActionFromMenuItemID(NULL, itemID);
  ThotBool on   = m_pSplitterWindow->IsSplit();

  while ( doc_id < MAX_DOCUMENTS )
    {
      if (LoadedDocument[doc_id-1])
	{
	  frame_id = LoadedDocument[doc_id-1]->DocViewFrame[0];
	  if (FrameTable[frame_id].FrWindowId == window_id &&
	      FrameTable[frame_id].FrPageId == page_id)
	    {
	      /* toggle the menu item of every documents */
	      MenuActionList[action].ActionToggle[doc_id] = on;
	      TtaRefreshMenuItemStats( doc_id, NULL, itemID );
	    }
	}
      doc_id++;
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  RefreshSplitToggleMenu()
 * Description:  is called to toggle on/off the "Show/Hide panel" menu item depeding on
 *               the panel showing state.
 *               Just forward the request to the parent window.
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::RefreshShowPanelToggleMenu()
{
  wxASSERT(GetWindowParent());

  if (GetWindowParent())
    GetWindowParent()->RefreshShowPanelToggleMenu();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  GetMasterFrameId
 * Description:  the master frame is the on which control the urlbar, buttons (TODO : and menus)
 *--------------------------------------------------------------------------------------
 */
int AmayaPage::GetMasterFrameId()
{
  return m_MasterFrameId;
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaPage, wxPanel)
  EVT_SPLITTER_SASH_POS_CHANGING( -1, 	AmayaPage::OnSplitterPosChanging )
  EVT_SPLITTER_SASH_POS_CHANGED( -1, 	AmayaPage::OnSplitterPosChanged )
  EVT_SPLITTER_DCLICK( -1, 		AmayaPage::OnSplitterDClick )
  EVT_SPLITTER_UNSPLIT( -1, 		AmayaPage::OnSplitterUnsplit )
  
  EVT_SIZE( 				AmayaPage::OnSize )
  EVT_CLOSE( 				AmayaPage::OnClose )
  //  EVT_PAINT(                            AmayaPage::OnPaint )  

  EVT_BUTTON( -1,                       AmayaPage::OnSplitButton)

  //  EVT_CONTEXT_MENU(                     AmayaPage::OnContextMenu )

END_EVENT_TABLE()

#endif /* #ifdef _WX */ 
