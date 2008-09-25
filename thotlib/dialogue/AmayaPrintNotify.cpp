/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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
#include "message_wx.h"

#include "AmayaPrintNotify.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaPrintNotify, wxDialog)

/* this function is the callback for cancel button : implementation is into print.c */
extern void wx_print_cancel();

/*----------------------------------------------------------------------
 *       Class:  AmayaPrintNotify
 *      Method:  AmayaPrintNotify
 * Description:  create a new AmayaPrintNotify
  -----------------------------------------------------------------------*/
AmayaPrintNotify::AmayaPrintNotify() : 
  wxDialog( NULL, -1,
	    _T(""),
	    wxDefaultPosition,
	    wxSize(500,500),
	    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCAPTION )
{
  // setup the title
  SetTitle( TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_LIB_PRINT)) );

  // create the label
  m_pLabel = new wxStaticText( this, -1, _T("                                                                           "),
                               wxDefaultPosition,
                               wxSize(200, -1)/*wxDefaultSize*/,
                               wxALIGN_CENTRE );

  // create the button
  m_pCancelButton = new wxButton(this, -1, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CANCEL)));

  // create a top sizer to contains the label and cancel button
  m_pTopSizer = new wxBoxSizer ( wxVERTICAL );
  m_pTopSizer->Add( m_pLabel, 1, wxEXPAND | wxALL, 5 );
  m_pTopSizer->Add( m_pCancelButton, 0, wxEXPAND | wxALL, 5 );
  SetSizer(m_pTopSizer);
  m_pTopSizer->Fit(this);
  m_pTopSizer->Layout();
  
  SetAutoLayout(TRUE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPrintNotify
 *      Method:  ~AmayaPrintNotify
 * Description:  destructor
  -----------------------------------------------------------------------*/
AmayaPrintNotify::~AmayaPrintNotify()
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPrintNotify
 *      Method:  OnButton
 * Description:  called when the cancel button is pressed
  -----------------------------------------------------------------------*/
void AmayaPrintNotify::OnButton(wxCommandEvent & event)
{
  Close();
  event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPrintNotify
 *      Method:  SetMessage
 * Description:  used to change the label of the dialog
  -----------------------------------------------------------------------*/
void AmayaPrintNotify::SetMessage( const char * p_message )
{
  m_pLabel->SetLabel( TtaConvMessageToWX(p_message) );
  m_pTopSizer->Fit(this);
  m_pTopSizer->Layout();
  Layout();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPrintNotify
 *      Method:  OnClose
 * Description:  called when closing the dialog
  -----------------------------------------------------------------------*/
void AmayaPrintNotify::OnClose(wxCloseEvent & event)
{
  wx_print_cancel();
  event.Skip();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaPrintNotify, wxDialog)
  EVT_BUTTON( -1, AmayaPrintNotify::OnButton )
  EVT_CLOSE(      AmayaPrintNotify::OnClose )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
