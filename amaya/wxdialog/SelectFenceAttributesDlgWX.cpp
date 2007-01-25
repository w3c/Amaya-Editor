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
  EVT_COMBOBOX( XRCID("wxID_ATTRIBUTE_OPEN"),       SelectFenceAttributesDlgWX::OnOpenSelected )
  EVT_COMBOBOX( XRCID("wxID_ATTRIBUTE_SEPARATORS"),   SelectFenceAttributesDlgWX::OnSeparatorsSelected )
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
  XRCCTRL(*this, "wxID_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_SELECT_FENCE_LABEL) ));
  XRCCTRL(*this, "wxID_INSERT", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_INSERT) ));

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
  if (Waiting)
    {
      Waiting = 0;
      ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
    }
  else
    // clean up the dialog context
    TtaDestroyDialogue( MyRef );
}

/*----------------------------------------------------------------------
  OnSelected new value selected
  ----------------------------------------------------------------------*/
void SelectFenceAttributesDlgWX::OnOpenSelected( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_ATTRIBUTE_OPEN", wxComboBox)->SetInsertionPointEnd();
}

/*----------------------------------------------------------------------
  OnSeparatorsSelected
  ----------------------------------------------------------------------*/
void SelectFenceAttributesDlgWX::OnSeparatorsSelected( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_ATTRIBUTE_SEPARATORS", wxComboBox)->SetInsertionPointEnd();
}

/*----------------------------------------------------------------------
  OnCloseSelected
  ----------------------------------------------------------------------*/
void SelectFenceAttributesDlgWX::OnCloseSelected( wxCommandEvent& event )
{
  XRCCTRL(*this, "wxID_ATTRIBUTE_CLOSE", wxComboBox)->SetInsertionPointEnd();
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
  string = XRCCTRL(*this, "wxID_ATTRIBUTE_OPEN", wxComboBox)->GetValue( );
  if (string.Len() > 0)
    {
      value = (int)string.GetChar(0);
      ThotCallback (MathsDialogue + MathAttributeOpen, STRING_DATA, (char *)value);
    }

  // return open separators
  string = XRCCTRL(*this, "wxID_ATTRIBUTE_SEPARATORS", wxComboBox)->GetValue( );
  if (string.Len() > 0)
    {
      value = (int)string.GetChar(0);
      ThotCallback (MathsDialogue + MathAttributeSeparators, STRING_DATA, (char *)value);
    }

  // return open symbol
  string = XRCCTRL(*this, "wxID_ATTRIBUTE_CLOSE", wxComboBox)->GetValue( );
  if (string.Len() > 0)
    {
      value = (int)string.GetChar(0);
      ThotCallback (MathsDialogue + MathAttributeClose, STRING_DATA, (char *)value);
    }

  ThotCallback (MyRef, INTEGER_DATA, (char*) 1);
  TtaDestroyDialogue (MyRef);
}

#endif /* _WX */
