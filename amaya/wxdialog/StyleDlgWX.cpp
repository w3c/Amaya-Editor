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

static char Buffer[2000];
static int  Index = 0;
static int  BG_repeat = 0;
static char End_rule[3];

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(StyleDlgWX, AmayaDialog)

  EVT_NOTEBOOK_PAGE_CHANGED( XRCID("wxID_NOTEBOOK"), StyleDlgWX::OnPageChanged )

  EVT_BUTTON(     XRCID("wxID_OK"),           StyleDlgWX::OnOk )
  EVT_BUTTON(     XRCID("wxID_DEFAULT"),      StyleDlgWX::OnDefault )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),       StyleDlgWX::OnCancel )

  EVT_BUTTON(     XRCID("wxID_BUTTON_TEXTCOLOR"), StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_BACKCOLOR"), StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_T_COLOR"),   StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_B_COLOR"),   StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_L_COLOR"),   StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_R_COLOR"),   StyleDlgWX::OnColorPalette )
  EVT_COMBOBOX( XRCID("wxID_COMBO_TEXTCOLOR"),    StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_BACKCOLOR"),   StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_T_COLOR"),     StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_B_COLOR"),     StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_L_COLOR"),     StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_R_COLOR"),     StyleDlgWX::OnColorChanged )
  EVT_TEXT( XRCID("wxID_COMBO_TEXTCOLOR"),       StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_BACKCOLOR"),       StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_T_COLOR"),         StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_B_COLOR"),         StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_L_COLOR"),         StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_R_COLOR"),         StyleDlgWX::OnColorTextChanged )
  EVT_BUTTON( XRCID("wxID_BUTTON_NOREPEAT"),     StyleDlgWX::OnButton ) 
  EVT_BUTTON( XRCID("wxID_BUTTON_REPEAT"),       StyleDlgWX::OnButton ) 
  EVT_BUTTON( XRCID("wxID_BUTTON_XREPEAT"),      StyleDlgWX::OnButton ) 
  EVT_BUTTON( XRCID("wxID_BUTTON_YREPEAT"),      StyleDlgWX::OnButton ) 
  END_EVENT_TABLE()

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
    ,m_IsInitialized(false) // this flag is used to know when events can be proceed
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("StyleDlgWX"));
  m_ref = ref;
  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_APPLY_BUTTON)));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DONE)));
  XRCCTRL(*this, "wxID_DEFAULT", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DEFAULT_BUTTON)));

  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  p_notebook->SetPageText( 0, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CHAR)) );
  p_notebook->SetPageText( 1, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_COLORS)) );
  p_notebook->SetPageText( 2, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_LIB_BOXES)) );
  p_notebook->SetPageText( 3, TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_FORMAT)) );

  // give focus to ...
  //  XRCCTRL(*this, "wxID_COMBOBOX_FAMILY", wxComboBox)->SetFocus();

  // on windows, the color selector dialog must be complete.
  colour_data.SetChooseFull(true);

  SetAutoLayout( TRUE );
  // this flag is used to know when events can be proceed
  // for example : when resources are loaded it produces "Page changed" events
  m_IsInitialized = true;
}

/*----------------------------------------------------------------------
  Destructor.
  ----------------------------------------------------------------------*/
StyleDlgWX::~StyleDlgWX()
{
  /* do not call this one because it cancel the link creation */
}


/*----------------------------------------------------------------------
  Class:  Style
  Method:  OnButton
  *Description:  this method is called when the user click on a tool
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
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleDlgWX::OnPageChanged( wxNotebookEvent& event )
{
  event.Skip();
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
  char            text[10];
  int             i;

  value = XRCCTRL(*this, "wxID_COMBOBOX_FAMILY", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "font-family:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_SIZE", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "font-size:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_SIZE", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "font-size:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_SIZE_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  value = XRCCTRL(*this, "wxID_CHOICE_STYLE", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "font-style:");
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
      strcpy (&Buffer[Index], "font-variant:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_LINE", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "line-height:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_LINE", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "line-height:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_LINE_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  i = XRCCTRL(*this, "wxID_SPIN_INDENT", wxSpinCtrl)->GetValue();
  if (i > 0)
    {
      sprintf (text, "%d", i);
      strcpy (&Buffer[Index], "text-indent:");
      strcat (&Buffer[Index], text);
      value = XRCCTRL(*this, "wxID_INDENT_UNIT", wxChoice)->GetStringSelection();
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_VALIGN", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "vertical-align:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_VALIGN", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "vertical-align:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_VALIGN_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  value = XRCCTRL(*this, "wxID_CHOICE_ALIGN", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "text-align:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_DECORATION", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "text-decoration:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_WORD", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "word-spacing:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_WORD", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "word-spacing:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_WORD_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  value = XRCCTRL(*this, "wxID_CHOICE_LETTER", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "letter-spacing:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_LETTER", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "letter-spacing:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_LETTER_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
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
  wxString        value;
  char            text[10];
  int             i;

  value = XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "color:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "background-color:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Background image
  value = XRCCTRL(*this, "wxID_COMBOBOX_BGIMAGE", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "background-image:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
      if (BG_repeat == 1)
        strcpy (&Buffer[Index], "background-repeat: no-repeat");
      else if (BG_repeat == 2)
        strcpy (&Buffer[Index], "background-repeat: repeat-x");
      else if (BG_repeat == 3)
        strcpy (&Buffer[Index], "background-repeat: repeat-y");
      else
        strcpy (&Buffer[Index], "background-repeat: repeat");
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);

      value = XRCCTRL(*this, "wxID_CHOICE_BGATTACH", wxChoice)->GetStringSelection();
      if (value.Len() > 0)
        {
          strcpy (&Buffer[Index], "background-attachement:");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }

      value = XRCCTRL(*this, "wxID_CHOICE_BGVPOSITION", wxChoice)->GetStringSelection();
      if (value.Len() > 0)
        {
          strcpy (&Buffer[Index], "background-position:");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
          value = XRCCTRL(*this, "wxID_CHOICE_BGHPOSITION", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcpy (&Buffer[Index], " ");
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
      else
        {
          value = XRCCTRL(*this, "wxID_CHOICE_BGHPOSITION", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcpy (&Buffer[Index], "background-position:");
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
              strcat (&Buffer[Index], End_rule);
              Index += strlen (&Buffer[Index]);
            }
        }
    }

  //Border top
  value = XRCCTRL(*this, "wxID_CHOICE_BTOP", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-top-width:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_BTOP", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "border-top-width:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_BTOP_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }
  value = XRCCTRL(*this, "wxID_CHOICE_T_STYLE", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-top-style:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  value = XRCCTRL(*this, "wxID_COMBO_T_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-top-color:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Border bottom
  value = XRCCTRL(*this, "wxID_CHOICE_BBOTTOM", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-bottom-width:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_BBOTTOM", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "border-bottom-width:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_BBOTTOM_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }
  value = XRCCTRL(*this, "wxID_CHOICE_B_STYLE", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-bottom-style:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  value = XRCCTRL(*this, "wxID_COMBO_B_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-bottom-color:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Border left
  value = XRCCTRL(*this, "wxID_CHOICE_BLEFT", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-left-width:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_BLEFT", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "border-left-width:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_BLEFT_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }
  value = XRCCTRL(*this, "wxID_CHOICE_L_STYLE", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-left-style:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  value = XRCCTRL(*this, "wxID_COMBO_L_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-left-color:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Border right
  value = XRCCTRL(*this, "wxID_CHOICE_BRIGHT", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-right-width:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_BRIGHT", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "border-right-width:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_BRIGHT_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }
  value = XRCCTRL(*this, "wxID_CHOICE_R_STYLE", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-right-style:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  value = XRCCTRL(*this, "wxID_COMBO_R_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-right-color:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
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
  strcpy(buffer, (const char*)value.mb_str(wxConvUTF8) );
  TtaGiveRGB (buffer, &red, &green, &blue);
  colour_data.SetColour( wxColour( red, green, blue ) );
  
  // open the color dialog and ask user to select a color.
  wxColourDialog dialog(this, &colour_data);
  if (dialog.ShowModal() == wxID_OK)
    {
      colour_data = dialog.GetColourData();
      wxColour col = colour_data.GetColour();
      
      char color_string[7];
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

  event.Skip();
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
  char            text[10];
  int             i;

  //Margin top
  value = XRCCTRL(*this, "wxID_CHOICE_MTOP", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin-top-width:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_MTOP", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "margin-top:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_MTOP_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  //Margin bottom
  value = XRCCTRL(*this, "wxID_CHOICE_MBOTTOM", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin-bottom:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_MBOTTOM", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "margin-bottom:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_MBOTTOM_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  //Margin left
  value = XRCCTRL(*this, "wxID_CHOICE_MLEFT", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin-left:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_MLEFT", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "margin-left:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_MLEFT_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  //Margin right
  value = XRCCTRL(*this, "wxID_CHOICE_MRIGHT", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin-right:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_MRIGHT", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "margin-right:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_MRIGHT_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  //Padding top
  value = XRCCTRL(*this, "wxID_CHOICE_PTOP", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding-top:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_PTOP", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "padding-top:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_PTOP_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  //Padding bottom
  value = XRCCTRL(*this, "wxID_CHOICE_PBOTTOM", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding-bottom:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_PBOTTOM", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "padding-bottom:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_PBOTTOM_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  //Padding left
  value = XRCCTRL(*this, "wxID_CHOICE_PLEFT", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding-left:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_PLEFT", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "padding-left:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_PLEFT_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  //Padding right
  value = XRCCTRL(*this, "wxID_CHOICE_PRIGHT", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding-right:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_PRIGHT", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "padding-right:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_PRIGHT_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  //Margin
  value = XRCCTRL(*this, "wxID_CHOICE_MARGIN", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_MARGIN", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "margin:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_MARGIN_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  //Padding
  value = XRCCTRL(*this, "wxID_CHOICE_PADDING", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "padding:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_PADDING", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "padding:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_PADDING_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  //Width
  value = XRCCTRL(*this, "wxID_CHOICE_WIDTH", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "width:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_WIDTH", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "width:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_WIDTH_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  //Height
  value = XRCCTRL(*this, "wxID_CHOICE_HEIGHT", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "height:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_HEIGHT", wxSpinCtrl)->GetValue();
      if (i > 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "height:");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_HEIGHT_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            {
              strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
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
  char            text[10];
  int             i;

  //Display
  value = XRCCTRL(*this, "wxID_CHOICE_DISPLAY", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "display:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Visibility
  value = XRCCTRL(*this, "wxID_CHOICE_VISIBILITY", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "visibility:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Float
  value = XRCCTRL(*this, "wxID_CHOICE_FLOAT", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "float:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Clear
  value = XRCCTRL(*this, "wxID_CHOICE_CLEAR", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "clear:");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Top
  i = XRCCTRL(*this, "wxID_SPIN_TOP", wxSpinCtrl)->GetValue();
  if (i > 0)
    {
      sprintf (text, "%d", i);
      strcpy (&Buffer[Index], "top:");
      strcat (&Buffer[Index], text);
      value = XRCCTRL(*this, "wxID_TOP_UNIT", wxChoice)->GetStringSelection();
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Bottom
  i = XRCCTRL(*this, "wxID_SPIN_BOTTOM", wxSpinCtrl)->GetValue();
  if (i > 0)
    {
      sprintf (text, "%d", i);
      strcpy (&Buffer[Index], "bottom:");
      strcat (&Buffer[Index], text);
      value = XRCCTRL(*this, "wxID_BOTTOM_UNIT", wxChoice)->GetStringSelection();
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Left
  i = XRCCTRL(*this, "wxID_SPIN_LEFT", wxSpinCtrl)->GetValue();
  if (i > 0)
    {
      sprintf (text, "%d", i);
      strcpy (&Buffer[Index], "left:");
      strcat (&Buffer[Index], text);
      value = XRCCTRL(*this, "wxID_LEFT_UNIT", wxChoice)->GetStringSelection();
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  //Right
  i = XRCCTRL(*this, "wxID_SPIN_RIGHT", wxSpinCtrl)->GetValue();
  if (i > 0)
    {
      sprintf (text, "%d", i);
      strcpy (&Buffer[Index], "right:");
      strcat (&Buffer[Index], text);
      value = XRCCTRL(*this, "wxID_RIGHT_UNIT", wxChoice)->GetStringSelection();
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
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
  Buffer[0] = EOS;
  Index = 0;
  strcpy (End_rule, ";\n");
  GetValueDialog_Text();
  GetValueDialog_Color();
  GetValueDialog_Box();
  GetValueDialog_Format();
  ThotCallback (m_ref, STRING_DATA, Buffer);
}

/*----------------------------------------------------------------------
  OnDefault reset to default value the current configuration page
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleDlgWX::OnDefault( wxCommandEvent& event )
{
}

/*----------------------------------------------------------------------
  OnCancel called when the user clicks on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleDlgWX::OnCancel( wxCommandEvent& event )
{
  ThotCallback (m_ref, STRING_DATA, NULL);
}

#endif /* _WX */
