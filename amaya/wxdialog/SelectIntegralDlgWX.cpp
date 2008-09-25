/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"
#include "wx/valgen.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

#include "SelectIntegralDlgWX.h"
static int Waiting = 0;
static int MyRef = 0;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SelectIntegralDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_INSERT"),      SelectIntegralDlgWX::OnInsert )
  EVT_BUTTON( XRCID("wxID_CANCEL"),      SelectIntegralDlgWX::OnCancel )
  EVT_CLOSE( SelectIntegralDlgWX::OnClose )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  SelectIntegralDlgWX create the dialog.
  params:
    + parent : parent window
  returns:
  ----------------------------------------------------------------------*/
SelectIntegralDlgWX::SelectIntegralDlgWX( int ref, wxWindow* parent) :
  AmayaDialog( parent, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SelectIntegralDlgWX"));
  // waiting for a return
  Waiting = 1;
  MyRef = ref;

  // update dialog labels with given ones
  SetTitle( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_SELECT_INTEGRAL_TITLE) ));
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_SELECT_INTEGRAL_LABEL) ));
  XRCCTRL(*this, "wxID_INSERT", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_INSERT) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));

  
  Refresh();
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
SelectIntegralDlgWX::~SelectIntegralDlgWX()
{
}


/*----------------------------------------------------------------------
  Close the window
  ----------------------------------------------------------------------*/
void SelectIntegralDlgWX::OnInsert( wxCommandEvent& event )
{
  wxString      value;
  char          buffer[MAX_LENGTH];
  int           val;

  if (!Waiting)
    return;
  Waiting = 0;

  // return number
  value = XRCCTRL(*this, "wxID_INTEGRAL_NUMBER", wxChoice)->GetStringSelection();
  strcpy(buffer, (const char*)value.mb_str(wxConvUTF8) );
  if (!strcmp (buffer, "double"))
    val = 2;
  else if (!strcmp (buffer, "triple"))
    val = 3;
  else
    val = 1;
  ThotCallback (MathsDialogue + MathIntegralNumber, INTEGER_DATA, (char *)val);

  // return contour
  value = XRCCTRL(*this, "wxID_INTEGRAL_CONTOUR", wxChoice)->GetStringSelection();
  strcpy(buffer, (const char*)value.mb_str(wxConvUTF8) );
  if (!strcmp (buffer, "contour"))
    val = 1;
  else
    val = 0;
  ThotCallback (MathsDialogue + MathIntegralContour, INTEGER_DATA, (char *)val);

  // return open symbol
  value = XRCCTRL(*this, "wxID_INTEGRAL_TYPE", wxChoice)->GetStringSelection();
  strcpy(buffer, (const char*)value.mb_str(wxConvUTF8) );
  if (!strcmp (buffer, "msubsup"))
    val = 1;
  else
    val = 0;
  ThotCallback (MathsDialogue + MathIntegralType, INTEGER_DATA, (char *)val);

  ThotCallback (MyRef, INTEGER_DATA, (char*) 1);
  TtaDestroyDialogue (MyRef);
}

/*----------------------------------------------------------------------
  OnClose
  called when the window manager closes the dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void SelectIntegralDlgWX::OnClose(wxCloseEvent& event)
{
  if (!Waiting)
    return;
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
  TtaRedirectFocus();
  TtaDestroyDialogue (MyRef);
}

/*----------------------------------------------------------------------
  OnCancel
  called when the window manager closes the dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void SelectIntegralDlgWX::OnCancel(wxCommandEvent& event)
{
  if (!Waiting)return;
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
  TtaRedirectFocus();
  TtaDestroyDialogue (MyRef);
}

#endif /* _WX */
