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

#include "AmayaApp.h"
#include "MakeIdDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "thot_sys.h"
static int      MyRef = 0;


//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(MakeIdDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_ADD"),              MakeIdDlgWX::OnAddButton )
  EVT_BUTTON(     XRCID("wxID_REMOVE"),           MakeIdDlgWX::OnRemoveButton )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),           MakeIdDlgWX::OnCancelButton )
  EVT_TEXT_ENTER( XRCID("wxID_NAME"),             MakeIdDlgWX::OnAddButton )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  MakeIdDlgWX create the dialog used to Add/Remove Ids
  params:
    + parent : parent window
  returns:
  ----------------------------------------------------------------------*/
MakeIdDlgWX::MakeIdDlgWX( int ref, wxWindow* parent) :
  AmayaDialog( parent, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("MakeIdDlgWX"));
  MyRef = ref;

  // update dialog labels with given ones
  SetTitle( TtaConvMessageToWX( TtaGetMessage(AMAYA, ADD_REMOVE_ID)));
  XRCCTRL(*this, "wxID_LABEL_NAME", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, ENTER_ELEMENT_NAME)));
  XRCCTRL(*this, "wxID_ADD", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA, ADD_ID)));
  XRCCTRL(*this, "wxID_REMOVE", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA, REMOVE_ID)));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_CANCEL)));

  XRCCTRL(*this, "wxID_LABEL_RADIO", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA, APPLY_OPERATION)));
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(0, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_IN_WHOLE_DOC)));
  XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->SetString(1, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_WITHIN_SEL)));
  XRCCTRL(*this, "wxID_NAME", wxTextCtrl)->SetSelection(0, -1);
  XRCCTRL(*this, "wxID_STATUS", wxStaticText)->SetLabel(TtaConvMessageToWX(""));

  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor. (Empty, as I don't need anything special done when destructing).
  ----------------------------------------------------------------------*/
MakeIdDlgWX::~MakeIdDlgWX()
{
}

/*----------------------------------------------------------------------
  OnAddButton called when the user validate his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void MakeIdDlgWX::OnAddButton( wxCommandEvent& event )
{
  // get the "where to open" indicator
  int where_id = XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->GetSelection();
  ThotCallback (BaseDialog + mIdUseSelection, INTEGER_DATA, (char*)where_id);

  // get the combobox current url
  wxString url = XRCCTRL(*this, "wxID_NAME", wxTextCtrl)->GetValue( );
  // allocate a temporary buffer to copy the 'const char *' url buffer 
  char buffer[512];
  wxASSERT( url.Len() < 512 );
  strcpy( buffer, (const char*)url.mb_str(wxConvUTF8) );
  // give the new url to amaya (to do url completion)
  ThotCallback (BaseDialog + mElemName,  STRING_DATA, (char *)buffer );

  // create or load the new document
  ThotCallback (MyRef, INTEGER_DATA, (char*)1);

  /* keep the dialog to display the result */
  //TtaDestroyDialogue (MyRef);
  XRCCTRL(*this, "wxID_STATUS", wxStaticText)->SetLabel(TtaConvMessageToWX(IdStatus));
}

/*----------------------------------------------------------------------
  OnRemoveButton called when the user wants to clear each fields
  params:
  returns:
  ----------------------------------------------------------------------*/
void MakeIdDlgWX::OnRemoveButton( wxCommandEvent& event )
{
  // get the "where to open" indicator
  int where_id = XRCCTRL(*this, "wxID_RADIOBOX", wxRadioBox)->GetSelection();
  ThotCallback (BaseDialog + mIdUseSelection, INTEGER_DATA, (char*)where_id);

  // get the combobox current url
  wxString url = XRCCTRL(*this, "wxID_NAME", wxTextCtrl)->GetValue( );
  // allocate a temporary buffer to copy the 'const char *' url buffer 
  char buffer[512];
  wxASSERT( url.Len() < 512 );
  strcpy( buffer, (const char*)url.mb_str(wxConvUTF8) );
  // give the new url to amaya (to do url completion)
  ThotCallback (BaseDialog + mElemName,  STRING_DATA, (char *)buffer );

  // create or load the new document
  ThotCallback (MyRef, INTEGER_DATA, (char*)2);

  /* The dialogue is no longer destroyed in the callback to prevent a crash on Mac */
  TtaDestroyDialogue (MyRef);
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  OnCancelButton called when the user click on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void MakeIdDlgWX::OnCancelButton( wxCommandEvent& event )
{
  TtaDestroyDialogue (MyRef);
  TtaRedirectFocus();
}

#endif /* _WX */
