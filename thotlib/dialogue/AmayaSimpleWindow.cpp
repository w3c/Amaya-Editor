/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/tglbtn.h"
#include "wx/string.h"
#include "wx/splitter.h"

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
#include "appdialogue_wx.h"
#include "message_wx.h"

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

#include "AmayaSimpleWindow.h"
#include "AmayaFrame.h"

IMPLEMENT_CLASS(AmayaSimpleWindow, AmayaWindow)

/*----------------------------------------------------------------------
 *       Class:  AmayaSimpleWindow
 *      Method:  AmayaSimpleWindow
 * Description:  create a new AmayaSimpleWindow
  -----------------------------------------------------------------------*/
  AmayaSimpleWindow::AmayaSimpleWindow (  wxWindow * parent, wxWindowID id,
                                         const wxPoint& pos,const wxSize&  size) : 
  AmayaWindow( parent, id, pos, size, WXAMAYAWINDOW_SIMPLE, wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT ),
  m_pFrame( NULL )
{
  // Creation of the top sizer to contain simple frame
  m_pTopSizer = new wxBoxSizer ( wxVERTICAL );
  SetSizer(m_pTopSizer);

#if 0
  // Creation of a close button
  wxButton * p_close_button = new wxButton( this, -1, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CANCEL)) );
  m_pTopSizer->Add( p_close_button, 0, wxEXPAND );
#endif /* 0 */

  SetAutoLayout(TRUE);
  CenterOnParent();
  // NOTICE : the menu bar is created for each AmayaFrame, 
  //          the menu bar is not managed by the window
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSimpleWindow
 *      Method:  ~AmayaSimpleWindow
 * Description:  destructor
  -----------------------------------------------------------------------*/
AmayaSimpleWindow::~AmayaSimpleWindow()
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSimpleWindow
 *      Method:  OnClose
 * Description:  Intercept the CLOSE event and prevent it if ncecessary.
  -----------------------------------------------------------------------*/
void AmayaSimpleWindow::OnClose(wxCloseEvent& event)
{
  if (m_pFrame)
  {
    int  frame_id = m_pFrame->GetFrameId();
    bool bClose;
    
    // Query close contained frame
    bClose   = m_pFrame->Close();
    if ( !TtaFrameIsClosed (frame_id) || !bClose)
    {
      // if the frame didnt die, just re-attach it
      AttachFrame( m_pFrame );
      event.Veto();
      return;
    }
    else
    {
      m_pFrame = NULL;
    }
  }
  else
    Destroy();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSimpleWindow
 *      Method:  GetActiveFrame
 * Description:  return the current selected frame
  -----------------------------------------------------------------------*/
AmayaFrame * AmayaSimpleWindow::GetActiveFrame() const
{
  return m_pFrame;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSimpleWindow
 *      Method:  AttachFrame
 * Description:  for a SIMPLE window type, attach directly a frame to the window
  -----------------------------------------------------------------------*/
bool AmayaSimpleWindow::AttachFrame( AmayaFrame * p_frame )
{
  wxLogDebug( _T("AmayaSimpleWindow::AttachFrame window_id=%d"), m_WindowId );

  // do not attach the frame if it is allready attached
  if ( p_frame == m_pFrame )
    return false;

  m_pFrame = p_frame;
  m_pFrame->SetActive(TRUE);
  m_pFrame->SetPageParent( NULL ); // no page parent
  m_pFrame->Show();
 
  // now I am your parent.
  //  m_pFrame->Reparent( this ); /* do not reparent because on MacOSX it is not implemented */
 
  // update the frame's window parent title
  if (m_pFrame)
    m_pFrame->SetFrameTitle(m_pFrame->GetFrameTitle());
  
  // insert the frame into the window
  m_pTopSizer->Insert( 0, m_pFrame, 1, wxEXPAND );
  Layout();

  SetAutoLayout(TRUE);

  return true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSimpleWindow
 *      Method:  DetachFrame
 * Description:  for a SIMPLE window type, detach directly a frame from the window
  -----------------------------------------------------------------------*/
AmayaFrame * AmayaSimpleWindow::DetachFrame()
{
  wxLogDebug( _T("AmayaSimpleWindow::DetachFrame window_id=%d"), m_WindowId );
  AmayaFrame * p_frame = m_pFrame;
  if (m_pFrame)
    {
      m_pFrame->Hide();
      m_pTopSizer->Detach(0);
      Layout();
      //      p_frame->Reparent( NULL );
      m_pFrame = NULL;
    }

  Close();

  return p_frame;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSimpleWindow
 *      Method:  CleanUp
 * Description:  check that this is not an empty window
  -----------------------------------------------------------------------*/
void AmayaSimpleWindow::CleanUp()
{
  if (!m_pFrame)
      Close();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaSimpleWindow, AmayaWindow)
  EVT_CLOSE(AmayaSimpleWindow::OnClose )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
