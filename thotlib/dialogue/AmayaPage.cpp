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
#include "appdialogue_f.h"
#include "boxparams_f.h"

#include "AmayaFrame.h"
#include "AmayaPage.h"
#include "wx/log.h"


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
	:  wxPanel( p_parent_window, -1 )
	  ,m_SlashRatio( 0.5 )
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

  if (p_frame == NULL)
    return NULL;

  /* p_frame is the new top frame */  
  m_pTopFrame = p_frame;

  /* the frame needs a new parent ! */
  m_pTopFrame->Reparent( m_pSplitterWindow );
  
  if (oldframe != NULL)
    m_pSplitterWindow->ReplaceWindow( oldframe, m_pTopFrame );
  else
    m_pSplitterWindow->Initialize( m_pTopFrame );

  SetAutoLayout(TRUE);
  
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

  if (p_frame == NULL)
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

  SetAutoLayout(TRUE);
  
  /* return the old bottomframe : needs to be manualy deleted .. */
  return oldframe;
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  DetachTopFrame
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
AmayaFrame * AmayaPage::DetachTopFrame( AmayaFrame * p_frame )
{
  return NULL;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPage
 *      Method:  DetachBottomFrame
 * Description:  TODO
 *--------------------------------------------------------------------------------------
 */
AmayaFrame * AmayaPage::DetachBottomFrame( AmayaFrame * p_frame )
{
  return NULL;
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
  wxLogDebug( _T("AmayaPage::OnClose topframe=%d bottomframe=%d\n"),
		m_pTopFrame ? m_pTopFrame->GetFrameId() : -1,
		m_pBottomFrame ? m_pBottomFrame->GetFrameId() : -1 );
    
  // map this callback to generic one : really kill amaya frames
  
  // Kill top frame
  if ( m_pTopFrame )
    KillFrameCallback( m_pTopFrame->GetFrameId() );

  // Kill bottom frame
  if ( m_pBottomFrame )
    KillFrameCallback( m_pBottomFrame->GetFrameId() );
  
  event.Skip();
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
END_EVENT_TABLE()

#endif /* #ifdef _WX */ 
