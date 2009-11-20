/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/valtext.h"

#include "AmayaApp.h"
#include "FontDlgWX.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(FontDlgWX, wxDialog)
  EVT_BUTTON(     XRCID("wxID_OK"),           FontDlgWX::OnOk )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),       FontDlgWX::OnCancel )
END_EVENT_TABLE()


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
FontDlgWX::FontDlgWX(wxWindow* parent, const wxString& title):
  wxDialog()
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("FontDlgWX"));
  
  SetTitle(title);
  XRCCTRL(*this,"wxID_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_CHOOSE_FONT) ));
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_LIB_CONFIRM) ));
   XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel( TtaConvMessageToWX( TtaGetMessage(LIB,TMSG_CANCEL) ));
 
  wxTextValidator valid(wxFILTER_NUMERIC);
  XRCCTRL(*this,"wxID_COMBO_SIZE", wxComboBox)->SetValidator(valid);
  // give focus to ...
  XRCCTRL(*this, "wxID_CHOICE_FAMILY", wxChoice)->SetFocus();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int FontDlgWX::GetFontFamily()const
{
  return XRCCTRL(*this, "wxID_CHOICE_FAMILY", wxChoice)->GetSelection();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
int FontDlgWX::GetFontSize()const
{
  long l;

  if(!XRCCTRL(*this, "wxID_COMBO_SIZE", wxComboBox)->GetValue().ToLong(&l))
    l = -1;
  return l;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void FontDlgWX::SetFontFamily(int family)
{
  XRCCTRL(*this, "wxID_CHOICE_FAMILY", wxChoice)->SetSelection(family);  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void FontDlgWX::SetFontSize(int size)
{
  wxString str;

  str.Printf(wxT("%d"), size);
  XRCCTRL(*this, "wxID_COMBO_SIZE", wxComboBox)->SetValue(str);
}

/*----------------------------------------------------------------------
  OnCancel called when the user clicks on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void FontDlgWX::OnCancel( wxCommandEvent& event )
{
  EndModal (wxID_CANCEL);
  TtaRedirectFocus();
}


/*----------------------------------------------------------------------
  OnOk called when the user clicks on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void FontDlgWX::OnOk( wxCommandEvent& event )
{
  EndModal (wxID_OK);
  TtaRedirectFocus();
}


#endif /* _WX */
