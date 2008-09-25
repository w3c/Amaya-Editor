/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/valgen.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

#include "SelectOperatorDlgWX.h"
static int Waiting = 0;
static int MyRef = 0;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SelectOperatorDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_OPERATOR0"), SelectOperatorDlgWX::OnOperator0 )
  EVT_BUTTON( XRCID("wxID_OPERATOR1"), SelectOperatorDlgWX::OnOperator1 )
  EVT_BUTTON( XRCID("wxID_OPERATOR2"), SelectOperatorDlgWX::OnOperator2 )
  EVT_CLOSE(SelectOperatorDlgWX::OnClose )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  SelectOperatorDlgWX create the dialog.
  params:
    + parent : parent window
    + title : dialog title
    + label : the message to show at dialog center
  returns:
  ----------------------------------------------------------------------*/
SelectOperatorDlgWX::SelectOperatorDlgWX( int ref, wxWindow* parent) :
  AmayaDialog( parent, ref )
{

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SelectOperatorDlgWX"));
  // waiting for a return
  Waiting = 1;
  MyRef = ref;

  // update dialog labels with given ones
  SetTitle(  TtaConvMessageToWX(TtaGetMessage (AMAYA, AM_SELECT_OPERATOR_TITLE)) );
  XRCCTRL(*this, "wxID_Operator", wxStaticText)-> SetLabel( TtaConvMessageToWX(TtaGetMessage (AMAYA, AM_SELECT_OPERATOR_LABEL)) );
  // fill initial value
  Fit();
  Refresh();
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
SelectOperatorDlgWX::~SelectOperatorDlgWX()
{
}

/*----------------------------------------------------------------------
  Return the number of button clicked on
  ----------------------------------------------------------------------*/
void SelectOperatorDlgWX::OnOperator0( wxCommandEvent& event )
{
  if (!Waiting)
    return;
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char*) 1);
  TtaRedirectFocus();
  TtaDestroyDialogue (MyRef);
}

/*----------------------------------------------------------------------
  Return the number of button clicked on
  ----------------------------------------------------------------------*/
void SelectOperatorDlgWX::OnOperator1( wxCommandEvent& event )
{
  if (!Waiting)
    return;
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char*) 2);
  TtaDestroyDialogue (MyRef);
}

/*----------------------------------------------------------------------
  Return the number of button clicked on
  ----------------------------------------------------------------------*/
void SelectOperatorDlgWX::OnOperator2( wxCommandEvent& event )
{
  if (!Waiting)
    return;
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char*) 3);
  TtaDestroyDialogue (MyRef);
}

/*----------------------------------------------------------------------
  OnClose
  called when the window manager closes the dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void SelectOperatorDlgWX::OnClose(wxCloseEvent& event)
{
  if (!Waiting)
    return;
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0); 
  TtaDestroyDialogue (MyRef);
}

#endif /* _WX */
