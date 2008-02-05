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

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
FontDlgWX::FontDlgWX(wxWindow* parent, const wxString& title):
  wxDialog()
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("FontDlgWX"));
  
  SetTitle(title);
  XRCCTRL(*this,"wxID_LABEL", wxStaticText)->SetLabel( TtaConvMessageToWX( TtaGetMessage(AMAYA,AM_CHOOSE_FONT) ));
  
  wxTextValidator valid(wxFILTER_NUMERIC);
  XRCCTRL(*this,"wxID_COMBO_SIZE", wxComboBox)->SetValidator(valid);

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
  return XRCCTRL(*this, "wxID_CHOICE_FAMILY", wxChoice)->SetSelection(family);  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void FontDlgWX::SetFontSize(int size)
{
  wxString str;
  str.Printf(wxT("%d"), size);
  XRCCTRL(*this, "wxID_COMBO_SIZE", wxComboBox)->SetValue(str);
}




#endif /* _WX */
