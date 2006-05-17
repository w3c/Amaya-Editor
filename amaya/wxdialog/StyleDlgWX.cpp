#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"
#include "wx/arrstr.h"
#include "wx/spinctrl.h"
#include "wx/notebook.h"
#include "wx/xrc/xmlres.h"
#include "wx/colordlg.h"
#include "AmayaApp.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "StyleDlgWX.h"
#include "AHTURLTools_f.h"

static char *Buffer;
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
  EVT_BUTTON( XRCID("wxID_BUTTON_NOREPEAT"),     StyleDlgWX::OnButton ) 
  EVT_BUTTON( XRCID("wxID_BUTTON_REPEAT"),       StyleDlgWX::OnButton ) 
  EVT_BUTTON( XRCID("wxID_BUTTON_XREPEAT"),      StyleDlgWX::OnButton ) 
  EVT_BUTTON( XRCID("wxID_BUTTON_YREPEAT"),      StyleDlgWX::OnButton )

  EVT_TEXT( XRCID("wxID_CHOICE_SIZE"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_LINE"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_INDENT"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_VALIGN"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_WORD"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_LETTER"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_BGHPOS"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_BGVPOS"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_BTOP"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_BBOTTOM"),  StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_BLEFT"),    StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_BRIGHT"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_B"),        StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_MTOP"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_MBOTTOM"),  StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_MLEFT"),    StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_MRIGHT"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_PTOP"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_PBOTTOM"),  StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_PLEFT"),    StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_PRIGHT"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_MARGIN"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_PADDING"),  StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_WIDTH"),    StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_HEIGHT"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_TOP"),      StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_BOTTOM"),   StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_LEFT"),     StyleDlgWX::OnValueChanged ) 
  EVT_TEXT( XRCID("wxID_CHOICE_RIGHT"),    StyleDlgWX::OnValueChanged ) 
  END_EVENT_TABLE()


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleDlgWX::InitValues ()
{
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

 XRCCTRL(*this, "wxID_CHOICE_SIZE", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_WEIGHT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_VARIANT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_LINE", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_INDENT", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_VALIGN", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_ALIGN", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_DECORATION", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_TRANSFORM", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_WORD", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_LETTER", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_BGATTACH", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_BGHPOS", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_BGVPOS", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_BTOP", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_T_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_BBOTTOM", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_B_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_BLEFT", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_L_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_BRIGHT", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_R_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_B", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_BORDER_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_MTOP", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_MBOTTOM", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_MLEFT", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_MRIGHT", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_PTOP", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_PBOTTOM", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_PLEFT", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_PRIGHT", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_MARGIN", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_PADDING", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_WIDTH", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_HEIGHT", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_TOP", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_BOTTOM", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_LEFT", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_RIGHT", wxComboBox)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_DISPLAY", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_VISIBILITY", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_FLOAT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_CLEAR", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_LISTSTYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_LISTPOSITION", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 BG_repeat = 0;
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
  wxString ptr = TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_APPLY_BUTTON));
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel(ptr);
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_DONE)));
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


/*----------------------------------------------------------------------
  Class:  StyleDlgWX
  Method:  OnButton
  Description:  this method is called when the user click on a tool
  -----------------------------------------------------------------------*/
void StyleDlgWX::OnButton( wxCommandEvent& event )
{
  int id = event.GetId();
  BG_repeat = 0;
  if ( id == wxXmlResource::GetXRCID(_T("wxID_BUTTON_NOREPEAT")) )
    BG_repeat = 1;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_BUTTON_XREPEAT")) )
    BG_repeat = 2;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_BUTTON_YREPEAT")) )
    BG_repeat = 3;
  else // repeat
    BG_repeat = 4;
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
        *ptr = *(++ptr);
      while (*ptr != EOS);
      ptr = buffer;
    }

  if (*ptr == '-' && !negative)
    {
      ret = FALSE;
      // set positive value
      do
        *ptr = *(++ptr);
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
            *pos = *(++pos);
          while (*pos != EOS);
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
  else if (!string)
    {
      buffer[0] = EOS;
      return FALSE;
    }
  else
    return ret;
}


/*----------------------------------------------------------------------
  Class:  StyleDlgWX
  Method:  OnValueButton
  Description:  
  -----------------------------------------------------------------------*/
void StyleDlgWX::OnValueChanged( wxCommandEvent& event )
{
  int             id = event.GetId();
  wxString        value;
  char            buffer[50];

  if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_SIZE")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_SIZE", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_SIZE", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_LINE")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_LINE", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, TRUE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_LINE", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_INDENT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_INDENT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, FALSE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_INDENT", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_VALIGN")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_VALIGN", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, FALSE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_VALIGN", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_WORD")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_WORD", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_WORD", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_LETTER")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_LETTER", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_LETTER", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_BGHPOS")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_BGHPOS", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_BGHPOS", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_BGVPOS")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_BGVPOS", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_BGVPOS", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_BTOP")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_BTOP", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_BTOP", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_BBOTTOM")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_BBOTTOM", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_BBOTTOM", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_BLEFT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_BLEFT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_BLEFT", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_BRIGHT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_BRIGHT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_BRIGHT", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_B")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_B", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_B", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_MTOP")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_MTOP", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_MTOP", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_PTOP")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_PTOP", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, FALSE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_PTOP", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_MBOTTOM")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_MBOTTOM", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_MBOTTOM", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_PBOTTOM")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_PBOTTOM", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, FALSE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_PBOTTOM", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_MLEFT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_MLEFT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_MLEFT", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_PLEFT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_PLEFT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, FALSE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_PLEFT", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_MRIGHT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_MRIGHT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_MRIGHT", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_PRIGHT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_PRIGHT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, FALSE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_PRIGHT", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_MARGIN")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_MARGIN", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_MARGIN", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_PADDING")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_PADDING", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_PADDING", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_WIDTH")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_WIDTH", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_WIDTH", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_HEIGHT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_HEIGHT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, FALSE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_HEIGHT", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_TOP")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_TOP", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_TOP", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_BOTTOM")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_BOTTOM", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_BOTTOM", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_LEFT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_LEFT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_LEFT", wxComboBox)->SetValue(value);
            }
        }
    }
  else if (id == wxXmlResource::GetXRCID(_T("wxID_CHOICE_RIGHT")))
    {
      // check values
      value = XRCCTRL(*this, "wxID_CHOICE_RIGHT", wxComboBox)->GetValue();
      if (value.Len() > 0)
        {
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), 50);
          if (!CheckValue (buffer, TRUE, TRUE, TRUE, FALSE, TRUE))
            {
              value = TtaConvMessageToWX(buffer);
              XRCCTRL(*this, "wxID_CHOICE_RIGHT", wxComboBox)->SetValue(value);
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

  value = XRCCTRL(*this, "wxID_CHOICE_SIZE", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "font-size: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
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

  value = XRCCTRL(*this, "wxID_CHOICE_LINE", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "line-height: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_INDENT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "text-indent: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_VALIGN", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "vertical-align: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
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

  value = XRCCTRL(*this, "wxID_CHOICE_WORD", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "word-spacing: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_LETTER", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "letter-spacing: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
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
          strcpy (&Buffer[Index], "background-attachement: ");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }

      value = XRCCTRL(*this, "wxID_CHOICE_BGHPOS", wxComboBox)->GetValue();
      svalue = XRCCTRL(*this, "wxID_CHOICE_BGVPOS", wxComboBox)->GetValue();
      if (value.Len() > 0 || svalue.Len() > 0)
        {
          strcpy (&Buffer[Index], "background-position: ");
          if (value.Len() > 0)
            strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
          if (value.Len() > 0 && svalue.Len() > 0)
            strcat (&Buffer[Index], " ");
          if (svalue.Len() > 0)
            strcat (&Buffer[Index], (const char*)svalue.mb_str(wxConvUTF8));
           strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
      else
        {
          if (value.Len() > 0)
            {
              strcpy (&Buffer[Index], "background-position: ");
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
              strcat (&Buffer[Index], End_rule);
              Index += strlen (&Buffer[Index]);
            }
        }
    }

  // Border top
  value = XRCCTRL(*this, "wxID_CHOICE_BTOP", wxComboBox)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_T_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_T_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 || svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-top: ");
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      if (svalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)svalue.mb_str(wxConvUTF8));
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
  value = XRCCTRL(*this, "wxID_CHOICE_BBOTTOM", wxComboBox)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_B_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_B_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 || svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-bottom: ");
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      if (svalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)svalue.mb_str(wxConvUTF8));
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
  value = XRCCTRL(*this, "wxID_CHOICE_BLEFT", wxComboBox)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_L_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_L_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 || svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-left: ");
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      if (svalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)svalue.mb_str(wxConvUTF8));
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
  value = XRCCTRL(*this, "wxID_CHOICE_BRIGHT", wxComboBox)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_R_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_R_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 ||
      svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-right: ");
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      if (svalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)svalue.mb_str(wxConvUTF8));
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
  value = XRCCTRL(*this, "wxID_CHOICE_B", wxComboBox)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_BORDER_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_BORDER_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 || svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border: ");
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      if (svalue.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)svalue.mb_str(wxConvUTF8));
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
  // get the combobox values and convert the string to a wxColour object
  wxString            value;
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  char                buffer[512];
  int                 id = event.GetId();
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
      TtaGiveRGB (buffer, &red, &green, &blue);
      wxColour col( red, green, blue );
      XRCCTRL(*this, "wxID_BUTTON_TEXTCOLOR", wxBitmapButton)->SetBackgroundColour( col );
    }
  else if (id == backcolor_id)
    {
      value = XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox)->GetValue();
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      TtaGiveRGB (buffer, &red, &green, &blue);
      wxColour col( red, green, blue );
      XRCCTRL(*this, "wxID_BUTTON_BACKCOLOR", wxBitmapButton)->SetBackgroundColour( col );
    }
  else if (id == tcolor_id)
    {
      value = XRCCTRL(*this, "wxID_COMBO_T_COLOR", wxComboBox)->GetValue();
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      TtaGiveRGB (buffer, &red, &green, &blue);
      wxColour col( red, green, blue );
      XRCCTRL(*this, "wxID_BUTTON_T_COLOR", wxBitmapButton)->SetBackgroundColour( col );
    }
  else if (id == bcolor_id)
    {
      value = XRCCTRL(*this, "wxID_COMBO_B_COLOR", wxComboBox)->GetValue();
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      TtaGiveRGB (buffer, &red, &green, &blue);
      wxColour col( red, green, blue );
      XRCCTRL(*this, "wxID_BUTTON_B_COLOR", wxBitmapButton)->SetBackgroundColour( col );
    }
  else if (id == lcolor_id)
    {
      value = XRCCTRL(*this, "wxID_COMBO_L_COLOR", wxComboBox)->GetValue();
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      TtaGiveRGB (buffer, &red, &green, &blue);
      wxColour col( red, green, blue );
      XRCCTRL(*this, "wxID_BUTTON_L_COLOR", wxBitmapButton)->SetBackgroundColour( col );
    }
  else if (id == rcolor_id)
    {
      value = XRCCTRL(*this, "wxID_COMBO_R_COLOR", wxComboBox)->GetValue();
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      TtaGiveRGB (buffer, &red, &green, &blue);
      wxColour col( red, green, blue );
      XRCCTRL(*this, "wxID_BUTTON_R_COLOR", wxBitmapButton)->SetBackgroundColour( col );
    }
  else if (id == borderc_id)
    {
      value = XRCCTRL(*this, "wxID_COMBO_BORDER_COLOR", wxComboBox)->GetValue();
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      TtaGiveRGB (buffer, &red, &green, &blue);
      wxColour col( red, green, blue );
      XRCCTRL(*this, "wxID_BUTTON_BORDER_COLOR", wxBitmapButton)->SetBackgroundColour( col );
    }

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
  value = XRCCTRL(*this, "wxID_CHOICE_MTOP", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin-top-width: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Margin bottom
  value = XRCCTRL(*this, "wxID_CHOICE_MBOTTOM", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin-bottom: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Margin left
  value = XRCCTRL(*this, "wxID_CHOICE_MLEFT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin-left: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Margin right
  value = XRCCTRL(*this, "wxID_CHOICE_MRIGHT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin-right: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Padding top
  value = XRCCTRL(*this, "wxID_CHOICE_PTOP", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding-top: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Padding bottom
  value = XRCCTRL(*this, "wxID_CHOICE_PBOTTOM", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding-bottom: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Padding left
  value = XRCCTRL(*this, "wxID_CHOICE_PLEFT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding-left: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Padding right
  value = XRCCTRL(*this, "wxID_CHOICE_PRIGHT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding-right: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Margin
  value = XRCCTRL(*this, "wxID_CHOICE_MARGIN", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Padding
  value = XRCCTRL(*this, "wxID_CHOICE_PADDING", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Width
  value = XRCCTRL(*this, "wxID_CHOICE_WIDTH", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "width: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Height
  value = XRCCTRL(*this, "wxID_CHOICE_HEIGHT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "height: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
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
      strcpy (&Buffer[Index], "list-style: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //List-position
  value = XRCCTRL(*this, "wxID_CHOICE_LISTPOSITION", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "list-position: ");
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
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Top
  value = XRCCTRL(*this, "wxID_CHOICE_TOP", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "top: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Bottom
  value = XRCCTRL(*this, "wxID_CHOICE_BOTTOM", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "bottom: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Left
  value = XRCCTRL(*this, "wxID_CHOICE_LEFT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "left: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Right
  value = XRCCTRL(*this, "wxID_CHOICE_RIGHT", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "right: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
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
    return;
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
  m_OnApplyLock = FALSE;
}

#endif /* _WX */
