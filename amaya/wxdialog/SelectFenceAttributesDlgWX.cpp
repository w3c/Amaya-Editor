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

#include "SelectFenceAttributesDlgWX.h"
static int Waiting = 0;
static int MyRef = 0;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(SelectFenceAttributesDlgWX, AmayaDialog)
  EVT_BUTTON( XRCID("wxID_INSERT"),      SelectFenceAttributesDlgWX::OnInsert )
  EVT_BUTTON( XRCID("wxID_CANCEL"),  SelectFenceAttributesDlgWX::OnCancel )
  EVT_CLOSE( SelectFenceAttributesDlgWX::OnClose )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  SelectFenceAttributesDlgWX create the dialog.
  params:
    + parent : parent window
  returns:
  ----------------------------------------------------------------------*/
SelectFenceAttributesDlgWX::SelectFenceAttributesDlgWX( int ref, wxWindow* parent) :
  AmayaDialog( parent, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("SelectFenceAttributesDlgWX"));
  // waiting for a return
  Waiting = 1;
  MyRef = ref;

  // update dialog labels with given ones
  SetTitle( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_SELECT_FENCE_TITLE) ));
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_SELECT_FENCE_LABEL) ));
  XRCCTRL(*this, "wxID_INSERT", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_INSERT) ));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX( TtaGetMessage(LIB, TMSG_CANCEL) ));

  Refresh();
  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
SelectFenceAttributesDlgWX::~SelectFenceAttributesDlgWX()
{
}


/*----------------------------------------------------------------------
  Close the window
  ----------------------------------------------------------------------*/
void SelectFenceAttributesDlgWX::OnInsert( wxCommandEvent& event )
{
  wxString      string;
  int           value;
  if (!Waiting)
    return;
  Waiting = 0;

  // return open symbol
  string = XRCCTRL(*this, "wxID_ATTRIBUTE_OPEN", wxChoice)->GetStringSelection( );
  if (string.Len() > 0)
    {
      value = (int)string.GetChar(0);
      ThotCallback (MathsDialogue + MathAttributeOpen, STRING_DATA, (char *)value);
    }

  // return open separators
  string = XRCCTRL(*this, "wxID_ATTRIBUTE_SEPARATORS", wxChoice)->GetStringSelection( );
  if (string.Len() > 0)
    {
      value = (int)string.GetChar(0);
      ThotCallback (MathsDialogue + MathAttributeSeparators, STRING_DATA, (char *)value);
    }

  // return open symbol
  string = XRCCTRL(*this, "wxID_ATTRIBUTE_CLOSE", wxChoice)->GetStringSelection( );
  if (string.Len() > 0)
    {
      value = (int)string.GetChar(0);
      ThotCallback (MathsDialogue + MathAttributeClose, STRING_DATA, (char *)value);
    }

  ThotCallback (MyRef, INTEGER_DATA, (char*) 1);
  TtaDestroyDialogue (MyRef);
}

/*----------------------------------------------------------------------
  OnClose
  called when the window manager closes the dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void SelectFenceAttributesDlgWX::OnClose(wxCloseEvent& event)
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
void SelectFenceAttributesDlgWX::OnCancel(wxCommandEvent& event)
{
  if (!Waiting)return;
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
  TtaRedirectFocus();
  TtaDestroyDialogue (MyRef);
}

#endif /* _WX */
