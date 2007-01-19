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
  EVT_TEXT_ENTER( XRCID("wxID_ATTRIBUTE_OPEN"),     SelectFenceAttributesDlgWX::OnOpenOk )
  EVT_COMBOBOX( XRCID("wxID_ATTRIBUTE_OPEN"),       SelectFenceAttributesDlgWX::OnOpenSelected )
  EVT_TEXT_ENTER( XRCID("wxID_ATTRIBUTE_SEPARATORS"), SelectFenceAttributesDlgWX::OnSeparatorsOk )
  EVT_COMBOBOX( XRCID("wxID_ATTRIBUTE_SEPARATORS"),   SelectFenceAttributesDlgWX::OnSeparatorsSelected )
  EVT_TEXT_ENTER( XRCID("wxID_ATTRIBUTE_CLOSE"),     SelectFenceAttributesDlgWX::OnCloseOk )
  EVT_COMBOBOX( XRCID("wxID_ATTRIBUTE_CLOSE"),       SelectFenceAttributesDlgWX::OnCloseSelected )
  EVT_BUTTON( XRCID("wxID_INSERT"), SelectFenceAttributesDlgWX::OnInsert )
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
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)-> SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_SELECT_FENCE_LABEL) ));
  XRCCTRL(*this, "wxID_INSERT", wxButton)-> SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_INSERT) ));

#ifndef _MACOS
  // give focus to ...
  XRCCTRL(*this, "wxID_ATTRIBUTE_OPEN", wxComboBox)->SetFocus();
#endif /* _MACOS */

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
  OnSelected new value selected
  ----------------------------------------------------------------------*/
void SelectFenceAttributesDlgWX::OnOpenSelected( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_ATTRIBUTE_OPEN", wxComboBox)->SetInsertionPointEnd();
}

void SelectFenceAttributesDlgWX::OnSeparatorsSelected( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_ATTRIBUTE_SEPARATORS", wxComboBox)->SetInsertionPointEnd();
}

void SelectFenceAttributesDlgWX::OnCloseSelected( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_ATTRIBUTE_CLOSE", wxComboBox)->SetInsertionPointEnd();
}

/*----------------------------------------------------------------------
  OnOk called when the user validates his selection
  ----------------------------------------------------------------------*/
void SelectFenceAttributesDlgWX::OnOpenOk( wxCommandEvent& event )
{
  wxString url = XRCCTRL(*this, "wxID_ATTRIBUTE_OPEN", wxComboBox)->GetValue( );
  Waiting = 0;
  char buffer[MAX_LENGTH];
  wxASSERT( url.Len() < MAX_LENGTH );
  strcpy( buffer, (const char*)url.mb_str(wxConvUTF8) );
  ThotCallback (MathsDialogue + MathAttributeOpen, STRING_DATA, (char *)buffer);
  ThotCallback (MyRef, INTEGER_DATA, (char*)1);
}
void SelectFenceAttributesDlgWX::OnSeparatorsOk( wxCommandEvent& event )
{
  wxString url = XRCCTRL(*this, "wxID_ATTRIBUTE_SEPARATORS", wxComboBox)->GetValue( );
  Waiting = 0;
  char buffer[MAX_LENGTH];
  wxASSERT( url.Len() < MAX_LENGTH );
  strcpy( buffer, (const char*)url.mb_str(wxConvUTF8) );
  ThotCallback (MathsDialogue + MathAttributeSeparators, STRING_DATA, (char *)buffer);
  ThotCallback (MyRef, INTEGER_DATA, (char*)2);
}
void SelectFenceAttributesDlgWX::OnCloseOk( wxCommandEvent& event )
{
  wxString url = XRCCTRL(*this, "wxID_ATTRIBUTE_CLOSE", wxComboBox)->GetValue( );
  Waiting = 0;
  char buffer[MAX_LENGTH];
  wxASSERT( url.Len() < MAX_LENGTH );
  strcpy( buffer, (const char*)url.mb_str(wxConvUTF8) );
  ThotCallback (MathsDialogue + MathAttributeClose, STRING_DATA, (char *)buffer);
  ThotCallback (MyRef, INTEGER_DATA, (char*)3);
}

/*----------------------------------------------------------------------
  Close the window
  ----------------------------------------------------------------------*/
void SelectFenceAttributesDlgWX::OnInsert( wxCommandEvent& event )
{
  if (!Waiting)
    return;
  Waiting = 0;
  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
  TtaDestroyDialogue (MyRef);
}

#endif /* _WX */
