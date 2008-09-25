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
#include "wx/arrstr.h"
#include "wx/spinctrl.h"
#include "wx/notebook.h"
#include "wx/colordlg.h"
#include "AmayaApp.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "StyleDlgWX.h"
#include "AHTURLTools_f.h"
#include "EDITstyle_f.h"
#include "wxdialogapi_f.h"

static char *Buffer = NULL; // parsed buffer
static char *Extra = NULL; // not parsed buffer
static int  Index = 0;
static int  BG_repeat = 0;
static char End_rule[10];
static int  CurrentDoc = 0;

bool StyleDlgWX::m_OnApplyLock = FALSE;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(StyleDlgWX, AmayaDialog)

  EVT_BUTTON(     XRCID("wxID_OK"),           StyleDlgWX::OnOk )
  EVT_BUTTON(     XRCID("wxID_DEFAULT"),      StyleDlgWX::OnDefault )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),       StyleDlgWX::OnCancel )

  EVT_BUTTON(     XRCID("wxID_BUTTON_TEXTCOLOR"),    StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_BACKCOLOR"),    StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_T_COLOR"),      StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_B_COLOR"),      StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_L_COLOR"),      StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_R_COLOR"),      StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_BORDER_COLOR"), StyleDlgWX::OnColorPalette )
  EVT_COMBOBOX( XRCID("wxID_COMBO_TEXTCOLOR"),   StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_BACKCOLOR"),   StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_T_COLOR"),     StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_B_COLOR"),     StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_L_COLOR"),     StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_R_COLOR"),     StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_BORDER_COLOR"),StyleDlgWX::OnColorChanged )
  EVT_TEXT( XRCID("wxID_COMBO_TEXTCOLOR"),       StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_BACKCOLOR"),       StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_T_COLOR"),         StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_B_COLOR"),         StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_L_COLOR"),         StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_R_COLOR"),         StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_BORDER_COLOR"),    StyleDlgWX::OnColorTextChanged )

  EVT_BUTTON( XRCID("wxID_BUTTON_BGIMAGE"),      StyleDlgWX::OnBrowseButton )
  EVT_BUTTON( XRCID("wxID_BUTTON_LISTIMAGE"),    StyleDlgWX::OnBrowseButton )
  EVT_TOOL( XRCID("wxID_NOREPEAT"),     StyleDlgWX::OnButton ) 
  EVT_TOOL( XRCID("wxID_REPEAT"),       StyleDlgWX::OnButton ) 
  EVT_TOOL( XRCID("wxID_XREPEAT"),      StyleDlgWX::OnButton ) 
  EVT_TOOL( XRCID("wxID_YREPEAT"),      StyleDlgWX::OnButton )

  EVT_TEXT( XRCID("wxID_COMBO_SIZE"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_LINE"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_INDENT"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_VALIGN"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_WORD"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_LETTER"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_BGHPOS"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_BGVPOS"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_BTOP"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_BBOTTOM"),  StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_BLEFT"),    StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_BRIGHT"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_B"),        StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_MTOP"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_MBOTTOM"),  StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_MLEFT"),    StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_MRIGHT"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_PTOP"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_PBOTTOM"),  StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_PLEFT"),    StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_PRIGHT"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_MARGIN"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_PADDING"),  StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_WIDTH"),    StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_HEIGHT"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_TOP"),      StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_BOTTOM"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_LEFT"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_COMBO_RIGHT"),    StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_ZINDEX"),         StyleDlgWX::OnValueChanged ) 
  END_EVENT_TABLE()


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void SetComboValue (wxComboBox *combo, const char *value)
{
  if (value == NULL)
    return;
  wxString str = TtaConvMessageToWX(value);
  str.Trim();
  str = str.BeforeFirst(wxT(';'));
  
  combo->SetStringSelection (str);
  if(combo->GetValue()!=str)
    {
      combo->Append (str);
      combo->SetValue (str);      
    }

}

/*----------------------------------------------------------------------
  property is TRUE when the value includes the property
  ----------------------------------------------------------------------*/
static void SetChoiceValue (wxChoice *choice, const char *value)
{
  if (value == NULL)
    return;
  wxString str = TtaConvMessageToWX(value);
  str.Trim();
  choice->SetStringSelection(str.BeforeFirst(wxT(';')));
}

/*----------------------------------------------------------------------
  SetColorTextChanged gets the combobox values and converts the string
  to a wxColour object
  ----------------------------------------------------------------------*/
void StyleDlgWX::SetColorTextChanged (int id)
{
  wxString            value, svalue;
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  char                buffer[512];
  int textcolor_id = wxXmlResource::GetXRCID(_T("wxID_COMBO_TEXTCOLOR"));
  int backcolor_id = wxXmlResource::GetXRCID(_T("wxID_COMBO_BACKCOLOR"));
  int tcolor_id = wxXmlResource::GetXRCID(_T("wxID_COMBO_T_COLOR"));
  int bcolor_id = wxXmlResource::GetXRCID(_T("wxID_COMBO_B_COLOR"));
  int lcolor_id = wxXmlResource::GetXRCID(_T("wxID_COMBO_L_COLOR"));
  int rcolor_id = wxXmlResource::GetXRCID(_T("wxID_COMBO_R_COLOR"));
  int borderc_id = wxXmlResource::GetXRCID(_T("wxID_COMBO_BORDER_COLOR"));

  if (id == textcolor_id)
    {
      value = XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox)->GetValue();
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      if (buffer[0] == EOS || !strcmp (buffer, "transparent"))
        XRCCTRL(*this, "wxID_BUTTON_TEXTCOLOR", wxBitmapButton)->SetBackgroundColour(m_OffColour);
      else
        {
          TtaGiveRGB (buffer, &red, &green, &blue);
          wxColour col( red, green, blue );
          XRCCTRL(*this, "wxID_BUTTON_TEXTCOLOR", wxBitmapButton)->SetBackgroundColour( col );
        }
    }
  else if (id == backcolor_id)
    {
      value = XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox)->GetValue();
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      if (buffer[0] == EOS || !strcmp (buffer, "transparent"))
        XRCCTRL(*this, "wxID_BUTTON_BACKCOLOR", wxBitmapButton)->SetBackgroundColour(m_OffColour);
      else
        {
          TtaGiveRGB (buffer, &red, &green, &blue);
          wxColour col( red, green, blue );
          XRCCTRL(*this, "wxID_BUTTON_BACKCOLOR", wxBitmapButton)->SetBackgroundColour( col );
        }
    }
  else if (id == tcolor_id)
    {
      value = XRCCTRL(*this, "wxID_COMBO_T_COLOR", wxComboBox)->GetValue();
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      TtaGiveRGB (buffer, &red, &green, &blue);
      if (buffer[0] == EOS || !strcmp (buffer, "transparent"))
        XRCCTRL(*this, "wxID_BUTTON_T_COLOR", wxBitmapButton)->SetBackgroundColour(m_OffColour);
      else
        {
          TtaGiveRGB (buffer, &red, &green, &blue);
          wxColour col( red, green, blue );
          XRCCTRL(*this, "wxID_BUTTON_T_COLOR", wxBitmapButton)->SetBackgroundColour( col );
          // initialize other values
          svalue = XRCCTRL(*this, "wxID_CHOICE_T_STYLE", wxChoice)->GetStringSelection();
          if (svalue.Len() == 0)
            XRCCTRL(*this, "wxID_CHOICE_T_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX("solid"));
          value = XRCCTRL(*this, "wxID_COMBO_BTOP", wxComboBox)->GetValue();
          if (value.Len() == 0)
            XRCCTRL(*this, "wxID_COMBO_BTOP", wxComboBox)->SetValue(TtaConvMessageToWX( "medium" ));
        }
    }
  else if (id == bcolor_id)
    {
      value = XRCCTRL(*this, "wxID_COMBO_B_COLOR", wxComboBox)->GetValue();
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      if (buffer[0] == EOS || !strcmp (buffer, "transparent"))
        XRCCTRL(*this, "wxID_BUTTON_B_COLOR", wxBitmapButton)->SetBackgroundColour(m_OffColour);
      else
        {
          TtaGiveRGB (buffer, &red, &green, &blue);
          wxColour col( red, green, blue );
          XRCCTRL(*this, "wxID_BUTTON_B_COLOR", wxBitmapButton)->SetBackgroundColour( col );
          // initialize other values
          svalue = XRCCTRL(*this, "wxID_CHOICE_B_STYLE", wxChoice)->GetStringSelection();
          if (svalue.Len() == 0)
            XRCCTRL(*this, "wxID_CHOICE_B_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX("solid"));
          value = XRCCTRL(*this, "wxID_COMBO_BBOTTOM", wxComboBox)->GetValue();
          if (value.Len() == 0)
            XRCCTRL(*this, "wxID_COMBO_BBOTTOM", wxComboBox)->SetValue(TtaConvMessageToWX( "medium" ));
        }
    }
  else if (id == lcolor_id)
    {
      value = XRCCTRL(*this, "wxID_COMBO_L_COLOR", wxComboBox)->GetValue();
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      if (buffer[0] == EOS || !strcmp (buffer, "transparent"))
        XRCCTRL(*this, "wxID_BUTTON_L_COLOR", wxBitmapButton)->SetBackgroundColour(m_OffColour);
      else
        {
          TtaGiveRGB (buffer, &red, &green, &blue);
          wxColour col( red, green, blue );
          XRCCTRL(*this, "wxID_BUTTON_L_COLOR", wxBitmapButton)->SetBackgroundColour( col );
          // initialize other values
          svalue = XRCCTRL(*this, "wxID_CHOICE_L_STYLE", wxChoice)->GetStringSelection();
          if (svalue.Len() == 0)
            XRCCTRL(*this, "wxID_CHOICE_L_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX("solid"));
          value = XRCCTRL(*this, "wxID_COMBO_BLEFT", wxComboBox)->GetValue();
          if (value.Len() == 0)
            XRCCTRL(*this, "wxID_COMBO_BLEFT", wxComboBox)->SetValue(TtaConvMessageToWX( "medium" ));
        }
    }
  else if (id == rcolor_id)
    {
      value = XRCCTRL(*this, "wxID_COMBO_R_COLOR", wxComboBox)->GetValue();
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      if (buffer[0] == EOS || !strcmp (buffer, "transparent"))
        XRCCTRL(*this, "wxID_BUTTON_R_COLOR", wxBitmapButton)->SetBackgroundColour(m_OffColour);
      else
        {
          TtaGiveRGB (buffer, &red, &green, &blue);
          wxColour col( red, green, blue );
          XRCCTRL(*this, "wxID_BUTTON_R_COLOR", wxBitmapButton)->SetBackgroundColour( col );
          // initialize other values
          svalue = XRCCTRL(*this, "wxID_CHOICE_R_STYLE", wxChoice)->GetStringSelection();
          if (svalue.Len() == 0)
            XRCCTRL(*this, "wxID_CHOICE_R_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX("solid"));
          value = XRCCTRL(*this, "wxID_COMBO_BRIGHT", wxComboBox)->GetValue();
          if (value.Len() == 0)
            XRCCTRL(*this, "wxID_COMBO_BRIGHT", wxComboBox)->SetValue(TtaConvMessageToWX( "medium" ));
        }
    }
  else if (id == borderc_id)
    {
      value = XRCCTRL(*this, "wxID_COMBO_BORDER_COLOR", wxComboBox)->GetValue();
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      if (buffer[0] == EOS || !strcmp (buffer, "transparent"))
        XRCCTRL(*this, "wxID_BUTTON_BORDER_COLOR", wxBitmapButton)->SetBackgroundColour(m_OffColour);
      else
        {
          TtaGiveRGB (buffer, &red, &green, &blue);
          wxColour col( red, green, blue );
          XRCCTRL(*this, "wxID_BUTTON_BORDER_COLOR", wxBitmapButton)->SetBackgroundColour( col );
          // initialize other values
          svalue = XRCCTRL(*this, "wxID_CHOICE_BORDER_STYLE", wxChoice)->GetStringSelection();
          if (svalue.Len() == 0)
            XRCCTRL(*this, "wxID_CHOICE_BORDER_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX("solid"));
          value = XRCCTRL(*this, "wxID_COMBO_B", wxComboBox)->GetValue();
          if (value.Len() == 0)
            XRCCTRL(*this, "wxID_COMBO_B", wxComboBox)->SetValue(TtaConvMessageToWX( "medium" ));
        }
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleDlgWX::SetValue (const char *property, const char *value)
{
  wxToolBar* tb = XRCCTRL(*this, "wxID_REPEAT_TOOL", wxToolBar);
  wxCommandEvent  event;
  int             id;
  ThotBool        notfound = FALSE;

  if (*property == 'b')
    {
      if (!strncmp (property, "background-color", 16))
        {
          SetComboValue (XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox), value);
          id = wxXmlResource::GetXRCID(_T("wxID_COMBO_BACKCOLOR"));
          SetColorTextChanged (id);
        }
      else if (!strncmp (property, "background-image", 16))
        SetComboValue (XRCCTRL(*this, "wxID_BGIMAGE", wxComboBox), value);
      else if (!strncmp (property, "background-repeat", 17))
        {
          BG_repeat = 0;
          if (!strncmp (value, "no-repeat", 9))
            {
              BG_repeat = 1;
              tb->ToggleTool(XRCID("wxID_NOREPEAT"), true);
              tb->ToggleTool(XRCID("wxID_REPEAT"), false);
              tb->ToggleTool(XRCID("wxID_XREPEAT"), false);
              tb->ToggleTool(XRCID("wxID_YREPEAT"), false);
            }
          else if (!strncmp (value, "repeat-x", 8))
            {
              BG_repeat = 2;
              tb->ToggleTool(XRCID("wxID_NOREPEAT"), false);
              tb->ToggleTool(XRCID("wxID_REPEAT"), false);
              tb->ToggleTool(XRCID("wxID_XREPEAT"), true);
              tb->ToggleTool(XRCID("wxID_YREPEAT"), false);
            }
          else if (!strncmp (value, "repeat-y", 8))
            {
              BG_repeat = 3;
              tb->ToggleTool(XRCID("wxID_NOREPEAT"), false);
              tb->ToggleTool(XRCID("wxID_REPEAT"), false);
              tb->ToggleTool(XRCID("wxID_XREPEAT"), false);
              tb->ToggleTool(XRCID("wxID_YREPEAT"), true);
            }
          else if (!strncmp (value, "repeat", 6))
            {
              BG_repeat = 4;
              tb->ToggleTool(XRCID("wxID_NOREPEAT"), false);
              tb->ToggleTool(XRCID("wxID_REPEAT"), true);
              tb->ToggleTool(XRCID("wxID_XREPEAT"), false);
              tb->ToggleTool(XRCID("wxID_YREPEAT"), false);
            }
        }
      else if (!strncmp (property, "background-attachment", 21))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_BGATTACH", wxChoice), value);
      else if (!strncmp (property, "background-positionH", 20))
          SetChoiceValue (XRCCTRL(*this, "wxID_COMBO_BGHPOS", wxChoice), value);
      else if (!strncmp (property, "background-positionV", 20))
          SetChoiceValue (XRCCTRL(*this, "wxID_COMBO_BGVPOS", wxChoice), value);
      else if (!strncmp (property, "border-top-width", 16))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_BTOP", wxComboBox), value);
      else if (!strncmp (property, "border-right-width", 18))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_BRIGHT", wxComboBox), value);
      else if (!strncmp (property, "border-bottom-width", 19))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_BBOTTOM", wxComboBox), value);
      else if (!strncmp (property, "border-left-width", 17))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_BLEFT", wxComboBox), value);
      else if (!strncmp (property, "border-width", 12))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_B", wxComboBox), value);
      else if (!strncmp (property, "border-top-color", 16))
        {
          id = wxXmlResource::GetXRCID(_T("wxID_COMBO_T_COLOR"));
          SetComboValue (XRCCTRL(*this, "wxID_COMBO_T_COLOR", wxComboBox), value);
          SetColorTextChanged (id);
        }
      else if (!strncmp (property, "border-right-color", 18))
        {
          id = wxXmlResource::GetXRCID(_T("wxID_COMBO_R_COLOR"));
          SetComboValue (XRCCTRL(*this, "wxID_COMBO_R_COLOR", wxComboBox), value);
          SetColorTextChanged (id);
        }
      else if (!strncmp (property, "border-bottom-color", 19))
        {
          id = wxXmlResource::GetXRCID(_T("wxID_COMBO_B_COLOR"));
          SetComboValue (XRCCTRL(*this, "wxID_COMBO_B_COLOR", wxComboBox), value);
          SetColorTextChanged (id);
        }
      else if (!strncmp (property, "border-left-color", 17))
        {
          id = wxXmlResource::GetXRCID(_T("wxID_COMBO_L_COLOR"));
          SetComboValue (XRCCTRL(*this, "wxID_COMBO_L_COLOR", wxComboBox), value);
          SetColorTextChanged (id);
        }
      else if (!strncmp (property, "border-color", 12))
        {
          id = wxXmlResource::GetXRCID(_T("wxID_COMBO_BORDER_COLOR"));
          SetComboValue (XRCCTRL(*this, "wxID_COMBO_BORDER_COLOR", wxComboBox), value);
          SetColorTextChanged (id);
        }
      else if (!strncmp (property, "border-top-style", 16))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_T_STYLE", wxChoice), value);
      else if (!strncmp (property, "border-right-style", 18))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_R_STYLE", wxChoice), value);
      else if (!strncmp (property, "border-bottom-style", 19))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_B_STYLE", wxChoice), value);
      else if (!strncmp (property, "border-left-style", 17))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_L_STYLE", wxChoice), value);
      else if (!strncmp (property, "border-style", 12))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_BORDER_STYLE", wxChoice), value);
      else if (!strncmp (property, "bottom", 6))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_BOTTOM", wxComboBox), value);
      else
        notfound = TRUE;
    }
  else if (*property == 'c')
    {
      if (!strncmp (property, "clear", 5))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_CLEAR", wxChoice), value);
      else if (!strncmp (property, "color", 5 ))
        {
          id = wxXmlResource::GetXRCID(_T("wxID_COMBO_TEXTCOLOR"));
          SetComboValue (XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox), value);
          SetColorTextChanged (id);
        }
      else
        notfound = TRUE;
    }
  else if (*property == 'd')
    {
      if (!strncmp (property, "display", 7))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_DISPLAY", wxChoice), value);
      else
        notfound = TRUE;
    }
  else if (*property == 'f')
    {
      if (!strncmp (property, "float", 5))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_FLOAT", wxChoice), value);
      else if (!strncmp (property, "font-family", 11))
        SetComboValue (XRCCTRL(*this, "wxID_COMBOBOX_FAMILY", wxComboBox), value);
      else if (!strncmp (property, "font-style", 10))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_STYLE", wxChoice), value);
      else if (!strncmp (property, "font-variant", 12))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_VARIANT", wxChoice), value);
      else if (!strncmp (property, "font-weight", 11))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_WEIGHT", wxChoice), value);
      else if (!strncmp (property, "font-size", 9))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_SIZE", wxComboBox), value);
      else
        notfound = TRUE;
    }
  else if (*property == 'h')
    {
      if (!strncmp (property, "height", 6))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_HEIGHT", wxComboBox), value);
      else
        notfound = TRUE;
    }
  else if (*property == 'l')
    {
      if (!strncmp (property, "left", 4))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_LEFT", wxComboBox), value);
      else if (!strncmp (property, "letter-spacing", 14))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_LETTER", wxComboBox), value);
      else if (!strncmp (property, "line-height", 11))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_LINE", wxComboBox), value);
      else if (!strncmp (property, "list-style-type", 15))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_LISTSTYLE", wxChoice), value);
      else if (!strncmp (property, "list-style-image", 16))
        SetComboValue (XRCCTRL(*this, "wxID_LIST_IMAGE", wxComboBox), value);
      else if (!strncmp (property, "list-style-position", 19))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_LISTPOSITION", wxChoice), value);
      else
        notfound = TRUE;
    }
  else if (*property == 'm')
    {
      if (!strncmp (property, "margin-bottom", 13))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_MBOTTOM", wxComboBox), value);
      else if (!strncmp (property, "margin-top", 10))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_MTOP", wxComboBox), value);
      else if (!strncmp (property, "margin-right", 12))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_MRIGHT", wxComboBox), value);
      else if (!strncmp (property, "margin-left", 11))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_MLEFT", wxComboBox), value);
      else if (!strncmp (property, "margin", 6))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_MARGIN", wxComboBox), value);
      else
        notfound = TRUE;
    }
  else if (*property == 'p')
    {
      if (!strncmp (property, "padding-top", 11))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_PTOP", wxComboBox), value);
      else if (!strncmp (property, "padding-right", 13))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_PRIGHT", wxComboBox), value);
      else if (!strncmp (property, "padding-bottom", 14))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_PBOTTOM", wxComboBox), value);
      else if (!strncmp (property, "padding-left", 12))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_PLEFT", wxComboBox), value);
      else if (!strncmp (property, "padding", 7))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_PADDING", wxComboBox), value);
      else if (!strncmp (property, "position", 8))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_POSITION", wxChoice), value);
      else
        notfound = TRUE;
    }
  else if (*property == 'r')
    {
      if (!strncmp (property, "right", 5))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_RIGHT", wxComboBox), value);
      else
        notfound = TRUE;
    }
  else if (*property == 't')
    {
      if (!strncmp (property, "text-align", 10))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_ALIGN", wxChoice), value);
      else if (!strncmp (property, "text-indent", 11))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_INDENT", wxComboBox), value);
      else if (!strncmp (property, "text-decoration", 15))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_DECORATION", wxChoice), value);
      else if (!strncmp (property, "text-transform", 14))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_TRANSFORM", wxChoice), value);
      else if (!strncmp (property, "top", 3))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_TOP", wxComboBox), value);
      else
        notfound = TRUE;
    }
  else if (*property == 'v')
    {
      if (!strncmp (property, "vertical-align", 14))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_VALIGN", wxComboBox), value);
      else if (!strncmp (property, "visibility", 10))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_VISIBILITY", wxChoice), value);
      else
        notfound = TRUE;
    }
  else if (*property == 'w')
    {
      if (!strncmp (property, "white-space", 11))
        SetChoiceValue (XRCCTRL(*this, "wxID_CHOICE_WHITESPACE", wxChoice), value);
      else if (!strncmp (property, "width", 5))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_WIDTH", wxComboBox), value);
      else if (!strncmp (property, "word-spacing", 12))
        SetComboValue (XRCCTRL(*this, "wxID_COMBO_WORD", wxComboBox), value);
      else
        notfound = TRUE;
    }
  else if (*property == 'z')
    {
      if (!strncmp (property, "z-index", 7))
        SetComboValue (XRCCTRL(*this, "wxID_ZINDEX", wxComboBox), value);
      else
        notfound = TRUE;
    }
  else
    notfound = TRUE;
  if (notfound)
    {
      // keep the current substring
      strcat (Extra, property);
      strcat (Extra, ": ");
      strcat (Extra, value);
      strcat (Extra, "; ");
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleDlgWX::InitValues ()
{
  wxToolBar* tb = XRCCTRL(*this, "wxID_REPEAT_TOOL", wxToolBar);
  XRCCTRL(*this, "wxID_NO_SELECTION", wxStaticText)->SetLabel(TtaConvMessageToWX(""));

  XRCCTRL(*this, "wxID_COMBOBOX_FAMILY", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_BGIMAGE", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_LIST_IMAGE", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_T_COLOR", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_B_COLOR", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_L_COLOR", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_R_COLOR", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_BORDER_COLOR", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_SIZE", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_LINE", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_INDENT", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_VALIGN", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_WORD", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_LETTER", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_BGHPOS", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_BGVPOS", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_BTOP", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_BBOTTOM", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_BLEFT", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_BRIGHT", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_B", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_MTOP", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_MBOTTOM", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_MLEFT", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_MRIGHT", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_PTOP", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_PBOTTOM", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_PLEFT", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_PRIGHT", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_MARGIN", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_PADDING", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_WIDTH", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_HEIGHT", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_TOP", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_BOTTOM", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_LEFT", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_COMBO_RIGHT", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_ZINDEX", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
  // ------------------
  XRCCTRL(*this, "wxID_CHOICE_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_WEIGHT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_VARIANT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_ALIGN", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_DECORATION", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_TRANSFORM", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_BGATTACH", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_T_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_B_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_L_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_R_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_BORDER_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_DISPLAY", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_VISIBILITY", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_FLOAT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_CLEAR", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_LISTSTYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  XRCCTRL(*this, "wxID_CHOICE_LISTPOSITION", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
  BG_repeat = 0;
  tb->ToggleTool(XRCID("wxID_NOREPEAT"), false);
  tb->ToggleTool(XRCID("wxID_REPEAT"), false);
  tb->ToggleTool(XRCID("wxID_XREPEAT"), false);
  tb->ToggleTool(XRCID("wxID_YREPEAT"), false);
  tb->SetToolShortHelp(XRCID("wxID_NOREPEAT"), TtaConvMessageToWX( "no-repeat" ));
  tb->SetToolShortHelp(XRCID("wxID_REPEAT"), TtaConvMessageToWX( "repeat" ));
  tb->SetToolShortHelp(XRCID("wxID_XREPEAT"), TtaConvMessageToWX( "repeat-x" ));
  tb->SetToolShortHelp(XRCID("wxID_YREPEAT"), TtaConvMessageToWX( "repeat-y" ));
  XRCCTRL(*this, "wxID_BUTTON_TEXTCOLOR", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*this, "wxID_BUTTON_BACKCOLOR", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*this, "wxID_BUTTON_T_COLOR", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*this, "wxID_BUTTON_B_COLOR", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*this, "wxID_BUTTON_L_COLOR", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*this, "wxID_BUTTON_R_COLOR", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  XRCCTRL(*this, "wxID_BUTTON_BORDER_COLOR", wxBitmapButton)->SetBackgroundColour( m_OffColour );

  // get the initial style values
  if (m_OnApplyLock)
    return;  // cannot perform the action
  m_OnApplyLock = TRUE;
  Buffer = GetCurrentStyle ();
  if (Buffer)
    {
      // prepare the buffer that will include unparsed substrings
      Extra = (char *)TtaGetMemory (strlen (Buffer));
      Extra[0] = EOS;
      ParseStyleDlgValues (this, Buffer);
      TtaFreeMemory (Buffer);
      Buffer = NULL;
      if (Extra[0] == EOS)
        {
          // all the initial string is now parsed
          TtaFreeMemory (Extra);
          Extra = NULL;
        }
    }
  m_OnApplyLock = FALSE;
}

/*----------------------------------------------------------------------
  StyleDlgWX create the dialog used to 
  - Change amaya Styles
  params:
  + parent : parent window
  + title : dialog title
  + ...
  returns:
  ----------------------------------------------------------------------*/
StyleDlgWX::StyleDlgWX( int ref, wxWindow* parent ) :
  AmayaDialog( parent, ref )
{
  ThotBool check;

  wxXmlResource::Get()->LoadDialog(this, parent, wxT("StyleDlgWX"));
  m_ref = ref;
  SetTitle (TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CSSStyle)));
  wxString ptr = TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_APPLY_BUTTON));
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel(ptr);
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CANCEL)));
  XRCCTRL(*this, "wxID_DEFAULT", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DEFAULT_BUTTON)));
  XRCCTRL(*this, "wxID_NO_SELECTION", wxStaticText)->SetLabel(TtaConvMessageToWX(""));
  XRCCTRL(*this, "wxID_LABEL_CLOSE", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_CLOSE_WHEN_APPLY)));
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  p_notebook->SetPageText( 0, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CHAR)) );
  p_notebook->SetPageText( 1, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_COLORS)) );
  p_notebook->SetPageText( 2, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_LIB_BOXES)) );
  p_notebook->SetPageText( 3, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_FORMAT)) );

  /* tooltip of color buttons */
  ptr = TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_FG_SEL_COLOR));
  XRCCTRL(*this, "wxID_BUTTON_TEXTCOLOR", wxBitmapButton)->SetToolTip( ptr);
  XRCCTRL(*this, "wxID_BUTTON_BACKCOLOR", wxBitmapButton)->SetToolTip( ptr);
  XRCCTRL(*this, "wxID_BUTTON_T_COLOR", wxBitmapButton)->SetToolTip( ptr);
  XRCCTRL(*this, "wxID_BUTTON_B_COLOR", wxBitmapButton)->SetToolTip( ptr);
  XRCCTRL(*this, "wxID_BUTTON_L_COLOR", wxBitmapButton)->SetToolTip( ptr);
  XRCCTRL(*this, "wxID_BUTTON_R_COLOR", wxBitmapButton)->SetToolTip( ptr);
  XRCCTRL(*this, "wxID_BUTTON_BORDER_COLOR", wxBitmapButton)->SetToolTip( ptr);

  /* tooltip of browse buttons */
  XRCCTRL(*this, "wxID_BUTTON_BGIMAGE", wxBitmapButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BROWSE) ));
  XRCCTRL(*this, "wxID_BUTTON_LISTIMAGE", wxBitmapButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_BROWSE) ));

  // init the close check entry
  TtaGetEnvBoolean ("CLOSE_WHEN_APPLY", &check);
  XRCCTRL(*this, "wx_CHECK_CLOSE", wxCheckBox)->SetValue(check);
  m_OffColour = XRCCTRL(*this, "wxID_BUTTON_TEXTCOLOR", wxBitmapButton)->GetBackgroundColour();
  InitValues ();

  // on windows, the color selector dialog must be complete.
  colour_data.SetChooseFull(true);

  SetAutoLayout( TRUE );
  // this flag is used to know when events can be proceed
  // for example : when resources are loaded it produces "Page changed" events
}

/*----------------------------------------------------------------------
  Destructor.
  ----------------------------------------------------------------------*/
StyleDlgWX::~StyleDlgWX()
{
  /* do not call this one because it cancel the link creation */
}

static bool Exclude_Button = false;
/*----------------------------------------------------------------------
  Class:  StyleDlgWX
  Method:  OnButton
  Description:  this method is called when the user click on a tool
  -----------------------------------------------------------------------*/
void StyleDlgWX::OnButton( wxCommandEvent& event )
{
  if (Exclude_Button)
    return;
  Exclude_Button = true;
  wxToolBar* tb = XRCCTRL(*this, "wxID_REPEAT_TOOL", wxToolBar);
  int id = event.GetId();
  if ( id == wxXmlResource::GetXRCID(_T("wxID_NOREPEAT")) )
    {
      if (BG_repeat == 1)
        {
          BG_repeat = 0;
          //tb->ToggleTool(XRCID("wxID_NOREPEAT"), false);
        }
      else
        {
          BG_repeat = 1;
          tb->ToggleTool(XRCID("wxID_NOREPEAT"), true);
          tb->ToggleTool(XRCID("wxID_REPEAT"), false);
          tb->ToggleTool(XRCID("wxID_XREPEAT"), false);
          tb->ToggleTool(XRCID("wxID_YREPEAT"), false);
        }
    }
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_XREPEAT")) )
    {
      if (BG_repeat == 2)
        {
          BG_repeat = 0;
          //tb->ToggleTool(XRCID("wxID_XREPEAT"), false);
        }
      else
        {
          BG_repeat = 2;
          tb->ToggleTool(XRCID("wxID_NOREPEAT"), false);
          tb->ToggleTool(XRCID("wxID_REPEAT"), false);
          tb->ToggleTool(XRCID("wxID_XREPEAT"), true);
          tb->ToggleTool(XRCID("wxID_YREPEAT"), false);
        }
    }
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_YREPEAT")) )
    {
     if (BG_repeat == 3)
        {
          BG_repeat = 0;
          //tb->ToggleTool(XRCID("wxID_YREPEAT"), false);
        }
      else
        {
          BG_repeat = 3;
          tb->ToggleTool(XRCID("wxID_NOREPEAT"), false);
          tb->ToggleTool(XRCID("wxID_REPEAT"), false);
          tb->ToggleTool(XRCID("wxID_XREPEAT"), false);
          tb->ToggleTool(XRCID("wxID_YREPEAT"), true);
        }
    }
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_REPEAT")) )
    {
     if (BG_repeat == 4)
        {
          BG_repeat = 0;
          //tb->ToggleTool(XRCID("wxID_REPEAT"), false);
        }
      else
        {
          BG_repeat = 4;
          tb->ToggleTool(XRCID("wxID_NOREPEAT"), false);
          tb->ToggleTool(XRCID("wxID_REPEAT"), true);
          tb->ToggleTool(XRCID("wxID_XREPEAT"), false);
          tb->ToggleTool(XRCID("wxID_YREPEAT"), false);
        }
    }
  Exclude_Button = false;
}

/*----------------------------------------------------------------------
  CheckValue checks if the value is compatible with one of possible cases.
  Return TRUE if it's a valid else fix the string.
  -----------------------------------------------------------------------*/
static ThotBool CheckValue (char *buffer, ThotBool negative, ThotBool length,
                            ThotBool percent, ThotBool number, ThotBool string)
{
  char        *ptr = buffer, *pos;
  ThotBool     zero, ret = TRUE;

  while (TtaIsBlank (ptr))
    {
      ret = FALSE;
      // remove initial spaces
      do
        {
          *ptr = ptr[1];
          ptr++;
        }
      while (*ptr != EOS);
      ptr = buffer;
    }

  if (*ptr == '-' && !negative)
    {
      ret = FALSE;
      // set positive value
      do
        {
          *ptr = ptr[1];
          ptr++;
        }
      while (*ptr != EOS);
      ptr = buffer;
    }
  
  if (isdigit (*ptr))
    {
      if (!length && !number && !percent)
        {
          // length, number, and percent not allowed
          buffer[0] = EOS;
          return FALSE;
        }

      // Check the special value 0
      zero = (*ptr == '0');
      ptr++;
      while (isdigit (*ptr))
        {
          // more than one digit
          zero = FALSE;
          ptr++;
        }
      if (*ptr == '.')
        // decimal value
        ptr++;

      while (isdigit (*ptr))
        {
          // more than one digit
          zero = FALSE;
          ptr++;
        }

      while (*ptr == SPACE)
        {
          ret = FALSE;
          pos = ptr;
          do
            {
              *ptr = ptr[1];
              ptr++;
            }
           while (*ptr != EOS);
        }

      if (*ptr == EOS)
        {
          if (!zero && !number)
            {
              // number value not allowed
              if (length)
                strcpy (ptr, "px");
              else if (percent)
                strcpy (ptr, "%");
              return FALSE;
            }
          else
            return ret;
        }
      else if (length &&
               (!strcmp (ptr, "px") ||
                !strcmp (ptr, "pt") ||
                !strcmp (ptr, "pc") ||
                !strcmp (ptr, "in") ||
                !strcmp (ptr, "mm") ||
                !strcmp (ptr, "cm") ||
                !strcmp (ptr, "em") ||
                !strcmp (ptr, "ex")))
        return ret;
      else if (percent && !strcmp (ptr, "%"))
        return ret;
      else
        {
          // number value not allowed
          if (length)
            strcpy (ptr, "px");
          else if (percent)
            strcpy (ptr, "%");
          return FALSE;
        }
    }
  else if (length &&
           (!strcmp (ptr, "px") ||
            !strcmp (ptr, "pt") ||
            !strcmp (ptr, "pc") ||
            !strcmp (ptr, "in") ||
            !strcmp (ptr, "mm") ||
            !strcmp (ptr, "cm") ||
            !strcmp (ptr, "em") ||
            !strcmp (ptr, "ex")))
        return ret;
  else if (percent && !strcmp (ptr, "%"))
    return ret;
  else if (!string)
    {
      buffer[0] = EOS;
      return FALSE;
    }
  else
    return ret;
}


/*----------------------------------------------------------------------
  Replace px, pt, pc, in, mm, cm, em, ex, % by 0
  -----------------------------------------------------------------------*/
static void CopyValueOrZero (char *buffer, const char *ptr)
{
  if (!strcmp (ptr, "px") ||
      !strcmp (ptr, "pt") ||
      !strcmp (ptr, "pc") ||
      !strcmp (ptr, "in") ||
      !strcmp (ptr, "mm") ||
      !strcmp (ptr, "cm") ||
      !strcmp (ptr, "em") ||
      !strcmp (ptr, "ex") ||
      !strcmp (ptr, "%"))
    strcat (buffer, "0");
  else
    strcat (buffer, ptr);
}

/*----------------------------------------------------------------------
  Class:  StyleDlgWX
  Method:  OnValueButton
  Description:  
  -----------------------------------------------------------------------*/
void StyleDlgWX::OnValueChanged( wxCommandEvent& event )
{
  int                 id = event.GetId();
  wxString            value, svalue;
  char                buffer[50];

  if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_SIZE")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_SIZE", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_SIZE", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_SIZE", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_LINE")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_LINE", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, TRUE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_LINE", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_LINE", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_INDENT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_INDENT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, FALSE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_INDENT", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_INDENT", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_VALIGN")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_VALIGN", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, FALSE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_VALIGN", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_VALIGN", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_WORD")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_WORD", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_WORD", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_WORD", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_LETTER")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_LETTER", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_LETTER", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_LETTER", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_BGHPOS")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_BGHPOS", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_BGHPOS", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_BGHPOS", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_BGVPOS")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_BGVPOS", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_BGVPOS", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_BGVPOS", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_BTOP")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_BTOP", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_BTOP", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_BTOP", wxComboBox)->SetInsertionPoint (1);
              // initialize other values
              svalue = XRCCTRL(*this, "wxID_CHOICE_T_STYLE", wxChoice)->GetStringSelection();
              if (svalue.Len() == 0)
                XRCCTRL(*this, "wxID_CHOICE_T_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX("solid"));
            }
        }
      else
        {
          // clear other values
          XRCCTRL(*this, "wxID_CHOICE_T_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX(""));
          XRCCTRL(*this, "wxID_COMBO_T_COLOR", wxComboBox)->SetValue(TtaConvMessageToWX(""));
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_BBOTTOM")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_BBOTTOM", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_BBOTTOM", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_BBOTTOM", wxComboBox)->SetInsertionPoint (1);
              svalue = XRCCTRL(*this, "wxID_CHOICE_B_STYLE", wxChoice)->GetStringSelection();
              if (svalue.Len() == 0)
                XRCCTRL(*this, "wxID_CHOICE_B_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "solid" ));
            }
        }
      else
        {
          XRCCTRL(*this, "wxID_CHOICE_B_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
          XRCCTRL(*this, "wxID_COMBO_B_COLOR", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_BLEFT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_BLEFT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_BLEFT", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_BLEFT", wxComboBox)->SetInsertionPoint (1);
              // initialize other values
              svalue = XRCCTRL(*this, "wxID_CHOICE_L_STYLE", wxChoice)->GetStringSelection();
              if (svalue.Len() == 0)
                XRCCTRL(*this, "wxID_CHOICE_L_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "solid" ));
            }
        }
      else
        {
          XRCCTRL(*this, "wxID_CHOICE_L_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
          XRCCTRL(*this, "wxID_COMBO_L_COLOR", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_BRIGHT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_BRIGHT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_BRIGHT", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_BRIGHT", wxComboBox)->SetInsertionPoint (1);
              // initialize other values
              svalue = XRCCTRL(*this, "wxID_CHOICE_R_STYLE", wxChoice)->GetStringSelection();
              if (svalue.Len() == 0)
                XRCCTRL(*this, "wxID_CHOICE_R_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "solid" ));
            }
        }
      else
        {
          XRCCTRL(*this, "wxID_CHOICE_R_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
          XRCCTRL(*this, "wxID_COMBO_R_COLOR", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_B")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_B", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_B", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_B", wxComboBox)->SetInsertionPoint (1);
              // initialize other values
              svalue = XRCCTRL(*this, "wxID_CHOICE_BORDER_STYLE", wxChoice)->GetStringSelection();
              if (svalue.Len() == 0)
                XRCCTRL(*this, "wxID_CHOICE_BORDER_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "solid" ));
            }
        }
      else
        {
          XRCCTRL(*this, "wxID_CHOICE_BORDER_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
          XRCCTRL(*this, "wxID_COMBO_BORDER_COLOR", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_MTOP")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_MTOP", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_MTOP", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_MTOP", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_PTOP")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_PTOP", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, FALSE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_PTOP", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_PTOP", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_MBOTTOM")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_MBOTTOM", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_MBOTTOM", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_MBOTTOM", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_PBOTTOM")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_PBOTTOM", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, FALSE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_PBOTTOM", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_PBOTTOM", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_MLEFT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_MLEFT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_MLEFT", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_MLEFT", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_PLEFT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_PLEFT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, FALSE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_PLEFT", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_PLEFT", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_MRIGHT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_MRIGHT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_MRIGHT", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_MRIGHT", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_PRIGHT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_PRIGHT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, FALSE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_PRIGHT", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_PRIGHT", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_MARGIN")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_MARGIN", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_MARGIN", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_MARGIN", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_PADDING")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_PADDING", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_PADDING", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_PADDING", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_WIDTH")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_WIDTH", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_WIDTH", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_WIDTH", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_HEIGHT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_HEIGHT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_HEIGHT", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_HEIGHT", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_TOP")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_TOP", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_TOP", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_TOP", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_BOTTOM")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_BOTTOM", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_BOTTOM", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_BOTTOM", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_LEFT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_LEFT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_LEFT", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_LEFT", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_COMBO_RIGHT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_COMBO_RIGHT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_COMBO_RIGHT", wxComboBox)->SetValue(value);
              XRCCTRL(*this, "wxID_COMBO_RIGHT", wxComboBox)->SetInsertionPoint (1);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_ZINDEX")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_ZINDEX", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (strcmp (buffer, "auto") && strcmp (buffer, "inherit") &&
              !CheckValue (buffer, FALSE, FALSE, FALSE, TRUE, FALSE))
            {
              XRCCTRL(*this, "wxID_ZINDEX", wxComboBox)->SetValue(TtaConvMessageToWX( "" ));
              XRCCTRL(*this, "wxID_ZINDEX", wxComboBox)->SetInsertionPoint (0);
            }
        }
    }
}


/************************************************************************/
/* Text tab                                                             */
/************************************************************************/
/*----------------------------------------------------------------------
  GetValueDialog_Text get dialog values
  params:
  returns:
  + PropTab_Text prop : the dialog values
  ----------------------------------------------------------------------*/
void StyleDlgWX::GetValueDialog_Text()
{
  wxString        value;

  value = XRCCTRL(*this, "wxID_COMBOBOX_FAMILY", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "font-family: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_COMBO_SIZE", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "font-size: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_STYLE", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "font-style: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_WEIGHT", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "font-weight:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_VARIANT", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "font-variant: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_COMBO_LINE", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "line-height: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_COMBO_INDENT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "text-indent: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_COMBO_VALIGN", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "vertical-align: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_ALIGN", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "text-align: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_DECORATION", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "text-decoration: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_TRANSFORM", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "text-transform: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_WHITESPACE", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "white-space: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_COMBO_WORD", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "word-spacing: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_COMBO_LETTER", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "letter-spacing: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
}

/************************************************************************/
/* Color tab                                                            */
/************************************************************************/
/*----------------------------------------------------------------------
  GetValueDialog_Color get dialog values
  params:
  returns:
  + Prop_Color prop : the dialog values
  ----------------------------------------------------------------------*/
void StyleDlgWX::GetValueDialog_Color()
{
  wxString        value, svalue, cvalue;
  char            pathimage[MAX_LENGTH], *s;

  value = XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "color: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "background-color: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Background image
  value = XRCCTRL(*this, "wxID_BGIMAGE", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "background-image: ");
      strcpy (pathimage, (const char*)value.mb_str(wxConvUTF8));
      if (!strcmp (pathimage, "none"))
        strcat (&Buffer[Index], pathimage);
      else if ((IsHTTPPath (DocumentURLs[CurrentDoc]) &&
           !strncmp (pathimage, "http:", 5)) || (!IsHTTPPath (DocumentURLs[CurrentDoc]) &&
           (!strncmp (pathimage, "file:", 5) || pathimage[0] == '/' || pathimage[1] == ':')))
        {
          strcat (&Buffer[Index], "url(");
          /* make a relative path */
          s = MakeRelativeURL (pathimage, DocumentURLs[CurrentDoc]);
          if (s)
            {
              strcat (&Buffer[Index], s);
              TtaFreeMemory (s);
            }
          strcat (&Buffer[Index], ")");
        }
      else
        {
          strcat (&Buffer[Index], "url(");
          strcat (&Buffer[Index], pathimage);
          strcat (&Buffer[Index], ")");
        }
      Index += strlen (&Buffer[Index]);
      if (BG_repeat == 1)
        strcpy (&Buffer[Index], "; background-repeat: no-repeat");
      else if (BG_repeat == 2)
        strcpy (&Buffer[Index], "; background-repeat: repeat-x");
      else if (BG_repeat == 3)
        strcpy (&Buffer[Index], "; background-repeat: repeat-y");
      else if (BG_repeat == 4)
        strcpy (&Buffer[Index], "; background-repeat: repeat");
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
      value = XRCCTRL(*this, "wxID_CHOICE_BGATTACH", wxChoice)->GetStringSelection();
      if (value.Len() > 0)
        {
          strcpy (&Buffer[Index], "background-attachment: ");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }

      value = XRCCTRL(*this, "wxID_COMBO_BGHPOS", wxComboBox)->GetValue();
      svalue = XRCCTRL(*this, "wxID_COMBO_BGVPOS", wxComboBox)->GetValue();
      if (value.Len() > 0 || svalue.Len() > 0)
        {
          strcpy (&Buffer[Index], "background-position: ");
          if (value.Len() > 0)
            CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
          if (value.Len() > 0 && svalue.Len() > 0)
            strcat (&Buffer[Index], " ");
          if (svalue.Len() > 0)
            CopyValueOrZero (&Buffer[Index], svalue.mb_str(wxConvUTF8));
           strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
      else
        {
          if (value.Len() > 0)
            {
              strcpy (&Buffer[Index], "background-position: ");
              CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
              strcat (&Buffer[Index], End_rule);
              Index += strlen (&Buffer[Index]);
            }
        }
    }

  // Border top
  value = XRCCTRL(*this, "wxID_COMBO_BTOP", wxComboBox)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_T_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_T_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 || svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-top:");
      if (svalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)svalue.mb_str(wxConvUTF8));
        }
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
        }
      if (cvalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)cvalue.mb_str(wxConvUTF8));
        }
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  // Border bottom
  value = XRCCTRL(*this, "wxID_COMBO_BBOTTOM", wxComboBox)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_B_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_B_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 || svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-bottom:");
      if (svalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)svalue.mb_str(wxConvUTF8));
        }
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
        }
      if (cvalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)cvalue.mb_str(wxConvUTF8));
        }
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  // Border left
  value = XRCCTRL(*this, "wxID_COMBO_BLEFT", wxComboBox)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_L_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_L_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 || svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-left:");
      if (svalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)svalue.mb_str(wxConvUTF8));
        }
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
        }
      if (cvalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)cvalue.mb_str(wxConvUTF8));
        }
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  // Border right
  value = XRCCTRL(*this, "wxID_COMBO_BRIGHT", wxComboBox)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_R_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_R_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 ||
      svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-right:");
      if (svalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)svalue.mb_str(wxConvUTF8));
        }
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
        }
      if (cvalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)cvalue.mb_str(wxConvUTF8));
        }
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  // Border
  value = XRCCTRL(*this, "wxID_COMBO_B", wxComboBox)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_BORDER_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_BORDER_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 || svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border:");
      if (svalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)svalue.mb_str(wxConvUTF8));
        }
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
        }
      if (cvalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)cvalue.mb_str(wxConvUTF8));
        }
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
}

/*----------------------------------------------------------------------
  OnColorPalette is called when the user click on the color palette button
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleDlgWX::OnColorPalette( wxCommandEvent& event )
{
  int textcolor_id = wxXmlResource::GetXRCID(_T("wxID_BUTTON_TEXTCOLOR"));
  int backcolor_id = wxXmlResource::GetXRCID(_T("wxID_BUTTON_BACKCOLOR"));
  int tcolor_id = wxXmlResource::GetXRCID(_T("wxID_BUTTON_T_COLOR"));
  int bcolor_id = wxXmlResource::GetXRCID(_T("wxID_BUTTON_B_COLOR"));
  int lcolor_id = wxXmlResource::GetXRCID(_T("wxID_BUTTON_L_COLOR"));
  int rcolor_id = wxXmlResource::GetXRCID(_T("wxID_BUTTON_R_COLOR"));
  int borderc_id = wxXmlResource::GetXRCID(_T("wxID_BUTTON_BORDER_COLOR"));
  int id = event.GetId();
  // First of all setup the dialog with the combobox color
  wxString value;
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  char buffer[512];

  if (id == textcolor_id)
    value = XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox)->GetValue();
  else if (id == backcolor_id)
    value = XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox)->GetValue();
  else if (id == tcolor_id)
    value = XRCCTRL(*this, "wxID_COMBO_T_COLOR", wxComboBox)->GetValue();
  else if (id == bcolor_id)
    value = XRCCTRL(*this, "wxID_COMBO_B_COLOR", wxComboBox)->GetValue();
  else if (id == lcolor_id)
    value = XRCCTRL(*this, "wxID_COMBO_L_COLOR", wxComboBox)->GetValue();
  else if (id == rcolor_id)
    value = XRCCTRL(*this, "wxID_COMBO_R_COLOR", wxComboBox)->GetValue();
  else if (id == borderc_id)
    value = XRCCTRL(*this, "wxID_COMBO_BORDER_COLOR", wxComboBox)->GetValue();
  strcpy(buffer, (const char*)value.mb_str(wxConvUTF8) );
  TtaGiveRGB (buffer, &red, &green, &blue);
  colour_data.SetColour( wxColour( red, green, blue ) );
  
  // open the color dialog and ask user to select a color.
  wxColourDialog dialog(this, &colour_data);
  if (dialog.ShowModal() == wxID_OK)
    {
      colour_data = dialog.GetColourData();
      wxColour col = colour_data.GetColour();
      
      char color_string[20];
      sprintf( color_string, "#%02x%02x%02x", col.Red(), col.Green(), col.Blue() );
      wxComboBox *     p_combo  = NULL;
      if (id == textcolor_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox);
      else if (id == backcolor_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox);
      else if (id == tcolor_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_T_COLOR", wxComboBox);
      else if (id == bcolor_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_B_COLOR", wxComboBox);    
      else if (id == lcolor_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_L_COLOR", wxComboBox);
      else if (id == rcolor_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_R_COLOR", wxComboBox);    
      else if (id == borderc_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_BORDER_COLOR", wxComboBox);    
      if (p_combo)
        {
          p_combo->Append( TtaConvMessageToWX(color_string) );
          p_combo->SetValue( TtaConvMessageToWX(color_string) );

          if (id == textcolor_id)
            XRCCTRL(*this, "wxID_BUTTON_TEXTCOLOR", wxBitmapButton)->SetBackgroundColour( col );
          else if (id == backcolor_id)
            XRCCTRL(*this, "wxID_BUTTON_BACKCOLOR", wxBitmapButton)->SetBackgroundColour( col );
          else if (id == tcolor_id)
            XRCCTRL(*this, "wxID_BUTTON_T_COLOR", wxBitmapButton)->SetBackgroundColour( col );
          else if (id == bcolor_id)
            XRCCTRL(*this, "wxID_BUTTON_B_COLOR", wxBitmapButton)->SetBackgroundColour( col );
          else if (id == lcolor_id)
            XRCCTRL(*this, "wxID_BUTTON_L_COLOR", wxBitmapButton)->SetBackgroundColour( col );
          else if (id == rcolor_id)
            XRCCTRL(*this, "wxID_BUTTON_R_COLOR", wxBitmapButton)->SetBackgroundColour( col );
          else if (id == borderc_id)
            XRCCTRL(*this, "wxID_BUTTON_BORDER_COLOR", wxBitmapButton)->SetBackgroundColour( col );
        }
    }
}

/*----------------------------------------------------------------------
  OnColorChanged is called when the used select something is the popup list
  (not used)
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleDlgWX::OnColorChanged( wxCommandEvent& event )
{
  event.Skip();
}

/*----------------------------------------------------------------------
  OnColorTextChanged is called when a new color is selected
  then the background color of wxBitmapButton is updated to the right color
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleDlgWX::OnColorTextChanged( wxCommandEvent& event )
{
  int                 id = event.GetId();

  SetColorTextChanged (id);
  event.Skip();
}

/*----------------------------------------------------------------------
  OnBrowseButton called when the user wants to search for a local file
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleDlgWX::OnBrowseButton( wxCommandEvent& event )
{
  int            id = event.GetId();
  int            list_id = wxXmlResource::GetXRCID(_T("wxID_BUTTON_LISTIMAGE"));
  // Create a generic filedialog
  wxFileDialog  *p_dlg = new wxFileDialog
    (
     this,
     TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OPEN_URL) ),
     _T(""),
     _T(""), 
     _T("*.*"),
     wxOPEN | wxCHANGE_DIR // wxCHANGE_DIR -> remember the last directory used.
     );

  // do not force the directory, let wxWidgets choose for the current one
  // p_dlg->SetDirectory(wxGetHomeDir());
  
  if (p_dlg->ShowModal() == wxID_OK)
    {
      if (id == list_id)
        XRCCTRL(*this, "wxID_LIST_IMAGE", wxComboBox)->SetValue( p_dlg->GetPath() );
      else
        XRCCTRL(*this, "wxID_BGIMAGE", wxComboBox)->SetValue( p_dlg->GetPath() );
      // destroy the dlg before calling thotcallback because it's a child of this
      // dialog and thotcallback will delete the dialog...
      // so if I do not delete it manualy here it will be deleted twice
      p_dlg->Destroy();
    }
  else
    {
      p_dlg->Destroy();
    }
}

/************************************************************************/
/* Box tab                                                            */
/************************************************************************/
/*----------------------------------------------------------------------
  GetValueDialog_Box get dialog values
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleDlgWX::GetValueDialog_Box()
{
  wxString        value;

  //Margin top
  value = XRCCTRL(*this, "wxID_COMBO_MTOP", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin-top: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Margin bottom
  value = XRCCTRL(*this, "wxID_COMBO_MBOTTOM", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin-bottom: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Margin left
  value = XRCCTRL(*this, "wxID_COMBO_MLEFT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin-left: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Margin right
  value = XRCCTRL(*this, "wxID_COMBO_MRIGHT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin-right: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Padding top
  value = XRCCTRL(*this, "wxID_COMBO_PTOP", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding-top: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Padding bottom
  value = XRCCTRL(*this, "wxID_COMBO_PBOTTOM", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding-bottom: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Padding left
  value = XRCCTRL(*this, "wxID_COMBO_PLEFT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding-left: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Padding right
  value = XRCCTRL(*this, "wxID_COMBO_PRIGHT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding-right: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Margin
  value = XRCCTRL(*this, "wxID_COMBO_MARGIN", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Padding
  value = XRCCTRL(*this, "wxID_COMBO_PADDING", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Width
  value = XRCCTRL(*this, "wxID_COMBO_WIDTH", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "width: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Height
  value = XRCCTRL(*this, "wxID_COMBO_HEIGHT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "height: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
}

/************************************************************************/
/* Format tab                                                            */
/************************************************************************/
/*----------------------------------------------------------------------
  GetValueDialog_Format get dialog values
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleDlgWX::GetValueDialog_Format()
{
  wxString        value;
  char            pathimage[MAX_LENGTH], *s;

  //Display
  value = XRCCTRL(*this, "wxID_CHOICE_DISPLAY", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "display: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Visibility
  value = XRCCTRL(*this, "wxID_CHOICE_VISIBILITY", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "visibility: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Z-Index
  value = XRCCTRL(*this, "wxID_ZINDEX", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "z-index: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Float
  value = XRCCTRL(*this, "wxID_CHOICE_FLOAT", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "float: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Clear
  value = XRCCTRL(*this, "wxID_CHOICE_CLEAR", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "clear: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //List-style
  value = XRCCTRL(*this, "wxID_CHOICE_LISTSTYLE", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "list-style-type: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //List-position
  value = XRCCTRL(*this, "wxID_CHOICE_LISTPOSITION", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "list-style-position: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Background image
  value = XRCCTRL(*this, "wxID_LIST_IMAGE", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "list-style-image: ");
      strcpy (pathimage, (const char*)value.mb_str(wxConvUTF8));
      if (!strcmp (pathimage, "none"))
        strcat (&Buffer[Index], pathimage);
      else if ((IsHTTPPath (DocumentURLs[CurrentDoc]) &&
           !strncmp (pathimage, "http:", 5)) || (!IsHTTPPath (DocumentURLs[CurrentDoc]) &&
           (!strncmp (pathimage, "file:", 5) || pathimage[0] == '/' || pathimage[1] == ':')))
        {
          strcat (&Buffer[Index], "url(");
          /* make a relative path */
          s = MakeRelativeURL (pathimage, DocumentURLs[CurrentDoc]);
          if (s)
            {
              strcat (&Buffer[Index], s);
              TtaFreeMemory (s);
            }
          strcat (&Buffer[Index], ")");
        }
      else
        {
          strcat (&Buffer[Index], "url(");
          strcat (&Buffer[Index], pathimage);
          strcat (&Buffer[Index], ")");
        }
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }


  //Position
  value = XRCCTRL(*this, "wxID_CHOICE_POSITION", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "position: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Top
  value = XRCCTRL(*this, "wxID_COMBO_TOP", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "top: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Bottom
  value = XRCCTRL(*this, "wxID_COMBO_BOTTOM", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "bottom: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Left
  value = XRCCTRL(*this, "wxID_COMBO_LEFT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "left: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Right
  value = XRCCTRL(*this, "wxID_COMBO_RIGHT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "right: ");
      CopyValueOrZero (&Buffer[Index], value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
}


/*----------------------------------------------------------------------
  OnOk called when the user validates his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleDlgWX::OnOk( wxCommandEvent& event )
{
  ThotBool check;

  if (m_OnApplyLock)
    return;

  m_OnApplyLock = TRUE;
  CurrentDoc = TtaGetSelectedDocument();
  if (CurrentDoc &&
      DocumentTypes[CurrentDoc] != docLog &&
      DocumentTypes[CurrentDoc] != docLibrary &&
      DocumentTypes[CurrentDoc] != docBookmark)
    {
      Buffer = (char *)TtaGetMemory (2500);
      if (Extra)
        {
          strcpy (Buffer, Extra);
          // unparsed rules are now restored
          TtaFreeMemory (Extra);
          Extra = NULL;
        }
      if (DocumentTypes[CurrentDoc] == docText ||
       DocumentTypes[CurrentDoc] == docCSS ||
       DocumentTypes[CurrentDoc] == docSource)
        {
          strcpy (Buffer, "  ");
          Index = 2;
          strcpy (End_rule, ";\n  ");
        }
      else
        {
          Buffer[0] = EOS;
          Index = 0;
          strcpy (End_rule, "; ");
        }
      GetValueDialog_Text();
      GetValueDialog_Color();
      GetValueDialog_Box();
      GetValueDialog_Format();
      // remove extra spaces
      while (Index > 0 && Buffer[Index - 1] != ';')
        Index--;
      Buffer[Index] = EOS;
      //if (Buffer[0] != EOS)
        // not an empty style
        ThotCallback (m_ref, STRING_DATA, Buffer);
      TtaFreeMemory (Buffer);

      // test if the form should be closed when apply
      TtaGetEnvBoolean ("CLOSE_WHEN_APPLY", &check);
      if (XRCCTRL(*this, "wx_CHECK_CLOSE", wxCheckBox)->IsChecked())
        {
          if (!check)
            TtaSetEnvBoolean ("CLOSE_WHEN_APPLY", TRUE, TRUE);
          // yes -> close
          ThotCallback (m_ref, STRING_DATA, NULL);
        }
      else if (check)
        TtaSetEnvBoolean ("CLOSE_WHEN_APPLY", FALSE, TRUE);
      // redirect focus to the canvas because when an action is done 
      // it's more probable that the user wants to type some characteres after executing the action
      TtaRedirectFocus();
    }
  else
    XRCCTRL(*this, "wxID_NO_SELECTION", wxStaticText)->SetLabel(TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_NO_INSERT_POINT)));
  m_OnApplyLock = FALSE;
}

/*----------------------------------------------------------------------
  OnDefault reset to default value the current configuration page
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleDlgWX::OnDefault( wxCommandEvent& event )
{
  InitValues ();
}

/*----------------------------------------------------------------------
  OnCancel called when the user clicks on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleDlgWX::OnCancel( wxCommandEvent& event )
{
  ThotBool check;

  if (m_OnApplyLock)
    return; // cannot perform the action
  m_OnApplyLock = TRUE;

  // test if the form should be closed when apply
  TtaGetEnvBoolean ("CLOSE_WHEN_APPLY", &check);
  if (XRCCTRL(*this, "wx_CHECK_CLOSE", wxCheckBox)->IsChecked())
    {
      if (!check)
        TtaSetEnvBoolean ("CLOSE_WHEN_APPLY", TRUE, TRUE);
    }
  else if (check)
    TtaSetEnvBoolean ("CLOSE_WHEN_APPLY", FALSE, TRUE);

  ThotCallback (m_ref, STRING_DATA, NULL);
  TtaRedirectFocus();
  m_OnApplyLock = FALSE;
}

#endif /* _WX */
