#ifdef _WX

#include "wx/wx.h"

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
#include "appdialogue_wx_f.h"
#include "appdialogue_f.h"
#include "boxparams_f.h"



#include "wx/log.h"

#include "AmayaWindow.h"
#include "AmayaFrame.h"
#include "AmayaPage.h"
#include "AmayaNotebook.h"
#include "AmayaCanvas.h"



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
AmayaPage::AmayaPage( wxWindow * p_parent_window )
  :  wxPanel( wxDynamicCast(p_parent_window, wxWindow), -1 )
     ,m_SlashRatio( 0.5 )
     ,m_IsClosed( FALSE )
     ,m_pNoteBookParent( NULL )
     ,m_pWindowParent( NULL )
     ,m_PageId(-1)
     ,m_ActiveFrame(0)
{
  // Insert a forground sizer
  wxBoxSizer * p_SizerTop = new wxBoxSizer ( wxVERTICAL );
  SetSizer( p_SizerTop );
 
  // Insert a windows splitter 
  m_pSplitterWindow = new wxSplitterWindow( this, -1,
                      wxDefaultPosition, wxDefaultSize,
                      wxSP_FULLSASH | wxSP_3DSASH | wxSP_NOBORDER | wxSP_PERMIT_UNSPLIT );
  p_SizerTop->Add( m_pSplitterWindow, 1, wxGROW, 0 );
 
  /// Insert to area : Top / bottom
  m_pTopFrame     = NULL;
  m_pBottomFrame  = NULL;
  m_pSplitterWindow->SetMinimumPaneSize( 50 );

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
 *      Method: AttachTopFrame
 * Description: attache a AmayaFrame to the page (top or bottom)
 *      params:
 *        + AmayaFrame * p_frame : the frame to attach
 *        + TODO ... : the position identifier - top or bottom
 *      return:
 *        + AmayaFrame * : the old frame or NULL if there was no old frame at this place
 *--------------------------------------------------------------------------------------
 */
AmayaFrame * AmayaPage::AttachTopFrame( AmayaFrame * p_frame )
{
  AmayaFrame * oldframe = m_pTopFrame;

  if (p_frame == NULL || p_frame == oldframe )
    return NULL;

  /* p_frame is the new top frame */  
  m_pTopFrame = p_frame;

  /* the frame needs a new parent ! */
  m_pTopFrame->Reparent( m_pSplitterWindow );

  if (oldframe != NULL)
    m_pSplitterWindow->ReplaceWindow( oldframe, m_pTopFrame );
  else if (m_pBottomFrame == NULL)
    m_pSplitterWindow->Initialize( m_pTopFrame );
  else
    m_pSplitterWindow->SplitHorizontally( m_pTopFrame, m_pBottomFrame );
  
  m_pTopFrame->Show();

  SetAutoLayout(TRUE);

  // update old and new AmayaFrame parents
  if (oldframe)
    oldframe->SetPageParent( NULL ); // no more parent
  if (p_frame)
    p_frame->SetPageParent( this ); // I'm your parent
 
  // update the page title (same as bottom frame)
  if (p_frame)
    p_frame->SetPageTitle(p_frame->GetPageTitle());
  
  // update the window menubar with the current frame
  if (p_frame && GetWindowParent())
    GetWindowParent()->SetMenuBar( p_frame->GetMenuBar() );

  /* return the old topframe : needs to be manualy deleted .. */
  return oldframe;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class: AmayaPage 
 *      Method: AttachBottomFrame
 * Description: cf. above -> AttachTopFrame
 *--------------------------------------------------------------------------------------
 */
AmayaFrame * AmayaPage::AttachBottomFrame( AmayaFrame * p_frame )
{
  AmayaFrame * oldframe = m_pBottomFrame;

  if (p_frame == NULL || p_frame == oldframe)
    return NULL;

  /* p_frame is the new top frame */  
  m_pBottomFrame = p_frame;

  /* the frame needs a new parent ! */
  m_pBottomFrame->Reparent( m_pSplitterWindow );
  
  if (oldframe != NULL)
    m_pSplitterWindow->ReplaceWindow( oldframe, m_pBottomFrame );
  else if (m_pTopFrame == NULL)
    m_pSplitterWindow->Initialize( m_pBottomFrame );
  else
    m_pSplitterWindow->SplitHorizontally( m_pTopFrame, m_pBottomFrame );

  m_pBottomFrame->Show();

  SetAutoLayout(TRUE);
  Layout();
 
  // update old and new AmayaFrame parents
  if (oldframe)
    oldframe->SetPageParent( NULL ); // no more parent
  if (p_frame)
    p_frame->SetPageParent( this ); // I'm your parent

  // update the page title (same as top frame)
  if (p_frame)
    p_frame->SetPageTitle(p_frame->GetPageTitle());
  
  /* return the old bottomframe : needs to be manualy deleted .. */
  return oldframe;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  DetachTopFrame
 * Description:  detache the top frame (hide it but don't delete it)
 *--------------------------------------------------------------------------------------
 */
AmayaFrame * AmayaPage::DetachTopFrame()
{
  AmayaFrame * oldframe = m_pTopFrame;

  if (oldframe == NULL)
    return NULL;

  m_pSplitterWindow->Unsplit( oldframe );

  // update old and new AmayaFrame parents
  if (oldframe)
    oldframe->SetPageParent( NULL ); // no more parent

  m_pTopFrame = NULL;

  return oldframe;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  DetachBottomFrame
 * Description:  detache the bottom frame (hide it but don't delete it)
 *--------------------------------------------------------------------------------------
 */
AmayaFrame * AmayaPage::DetachBottomFrame()
{
  AmayaFrame * oldframe = m_pBottomFrame;

  if (oldframe == NULL)
    return NULL;

  m_pSplitterWindow->Unsplit( oldframe );

  // update old and new AmayaFrame parents
  if (oldframe)
    oldframe->SetPageParent( NULL ); // no more parent

  m_pBottomFrame = NULL;

  return oldframe;
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
    wxLogDebug( _T("AmayaPage::OnSplitterPosChanged now = %d\n"), event.GetSashPosition() );

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
    
    event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  OnSplitterDClick
 * Description:  called when a double click is done on the splitbar
 *               TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::OnSplitterDClick( wxSplitterEvent& event )
{
  wxLogDebug( _T("AmayaPage::OnSplitterDClick\n") );
  
  event.Skip();  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  OnSplitterDClick
 * Description:  called when the splitbar is unsplited
 *               TODO
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::OnSplitterUnsplit( wxSplitterEvent& event )
{
  wxLogDebug( _T("AmayaPage::OnSplitterUnsplit\n") );
  
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
  
  wxLogDebug( _T("AmayaPage::OnSize w=%d h=%d \n"),
		event.GetSize().GetWidth(),
		event.GetSize().GetHeight() );
  
  // force the splitter position (depending of window size)
  float new_height = event.GetSize().GetHeight();
  float new_width = event.GetSize().GetWidth();
  float new_slash_pos = 0;
  int split_mode = m_pSplitterWindow->GetSplitMode();
  if ( split_mode == 1 ) // horizontally
    new_slash_pos = m_SlashRatio * new_height;
  else if ( split_mode == 2 ) // vertically
    new_slash_pos = m_SlashRatio * new_width;
  if ( new_slash_pos > 0 )
    m_pSplitterWindow->SetSashPosition( (int)new_slash_pos );

  event.Skip(); 
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
  wxLogDebug( _T("AmayaPage::OnClose topframe=%d bottomframe=%d"),
		m_pTopFrame ? m_pTopFrame->GetFrameId() : -1,
		m_pBottomFrame ? m_pBottomFrame->GetFrameId() : -1 );

  // Detach the window menu bar to avoid  probleme when
  // AmayaWindow will be deleted.
  // (because the menu bar is owned by AmayaFrame)
  GetWindowParent()->DesactivateMenuBar();

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
      DetachTopFrame();
      p_AmayaFrame->OnClose( event );
      if ( FrameTable[frame_id].WdFrame != 0)
	{
	  // if the frame didnt die, just re-attach it
	  AttachTopFrame(p_AmayaFrame);
	  m_IsClosed = FALSE;
	}
    }

  // Kill bottom frame
  if ( m_pBottomFrame )
    { 
      p_AmayaFrame = m_pBottomFrame;
      frame_id     = m_pBottomFrame->GetFrameId();
      DetachBottomFrame();
      p_AmayaFrame->OnClose( event );
      if (FrameTable[frame_id].WdFrame != 0)
	{
	  // if the frame didnt die, just re-attach it
	  AttachBottomFrame(p_AmayaFrame);
	  m_IsClosed = FALSE;
	}
    }

  // Reactivate the menu bar (nothing is done if the window is goind to die)
  GetWindowParent()->ActivateMenuBar();
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
  wxLogDebug( _T("AmayaPage::OnPaint") );
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
  Reparent( p_notebook );

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
  if (m_pNoteBookParent)
  {
    int page_id = m_pNoteBookParent->GetPageId( this );
    return (page_id == m_pNoteBookParent->GetSelection());
  }
  else
    return FALSE;
//  return m_IsSelected;
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
  if (isSelected)
  {
    for ( int frame_pos = 1; frame_pos<=2; frame_pos++ )
      {
	if (GetFrame(frame_pos))
	  {
	    // post a size event to force frame refresh
	    // to canvas
	    wxSizeEvent event_canvas( GetFrame(frame_pos)->GetCanvas()->GetSize() );
	    wxPostEvent( GetFrame(frame_pos)->GetCanvas(), event_canvas );
	    // to page
	    wxSizeEvent event_page( GetSize() );
	    wxPostEvent( this, event_page );
	  }
      }
    if ( GetActiveFrame() )
      {
	GetActiveFrame()->SetActive( TRUE );

	// update the page title
	//	GetActiveFrame()->SetWindowTitle(m_pTopFrame->GetWindowTitle());
      }
  }
}

void AmayaPage::SetPageId( int page_id )
{
  m_PageId = page_id;
  /*
  if (m_pTopFrame)
    m_pTopFrame->SetPageId( page_id );

  if (m_pBottomFrame)
  m_pBottomFrame->SetPageId( page_id );*/

  // update the document's page id
  int frame_id = 0;
  if (m_pTopFrame)
    {
      frame_id = m_pTopFrame->GetFrameId();
      TtaSetPageId( FrameTable[frame_id].FrDoc, page_id );
    }
  if (m_pBottomFrame)
    {
      frame_id = m_pBottomFrame->GetFrameId();
      TtaSetPageId( FrameTable[frame_id].FrDoc, page_id );
    }
}
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
 *      Method:  DeletedFrame
 * Description:  force the frame to be detached from the page when the frame
 *               is deleted by itself
 *--------------------------------------------------------------------------------------
 */
void AmayaPage::DeletedFrame( AmayaFrame * p_frame )
{
  if ( p_frame == GetFrame(1) )
    {
      DetachTopFrame();
    }
  if ( p_frame == GetFrame(2) )
    {
      DetachBottomFrame();
    }
}

void AmayaPage::SetActiveFrame( const AmayaFrame * p_frame )
{
  if ( p_frame == GetFrame(1) )
    m_ActiveFrame = 1;
  else if ( p_frame == GetFrame(2) )
    m_ActiveFrame = 2;
  else
    m_ActiveFrame = 0;
}

AmayaFrame * AmayaPage::GetActiveFrame() const
{
  return GetFrame( m_ActiveFrame );
}


/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaPage, wxPanel)
  EVT_SPLITTER_SASH_POS_CHANGED( -1, 	AmayaPage::OnSplitterPosChanged )
  EVT_SPLITTER_DCLICK( -1, 		AmayaPage::OnSplitterDClick )
  EVT_SPLITTER_UNSPLIT( -1, 		AmayaPage::OnSplitterUnsplit )
  
  EVT_SIZE( 				AmayaPage::OnSize )
  EVT_CLOSE( 				AmayaPage::OnClose )
  //  EVT_PAINT(                            AmayaPage::OnPaint )  
END_EVENT_TABLE()

#endif /* #ifdef _WX */ 
