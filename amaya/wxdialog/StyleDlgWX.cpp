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
  EVT_COMBOBOX( XRCID("wxID_COMBO_TEXTCOLOR"),    StyleDlgWX::OnColorChanged )
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

  EVT_SPINCTRL( XRCID("wxID_SPIN_SIZE"),     StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_LINE"),     StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_INDENT"),   StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_VALIGN"),   StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_WORD"),     StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_BGHPOS"),   StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_BGVPOS"),   StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_LETTER"),   StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_BTOP"),     StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_BBOTTOM"),  StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_BLEFT"),    StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_BRIGHT"),   StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_B"),        StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_MTOP"),     StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_MBOTTOM"),  StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_MLEFT"),    StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_MRIGHT"),   StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_PTOP"),     StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_PBOTTOM"),  StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_PLEFT"),    StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_PRIGHT"),   StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_MARGIN"),   StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_PADDING"),  StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_WIDTH"),    StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_HEIGHT"),   StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_TOP"),      StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_BOTTOM"),   StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_LEFT"),     StyleDlgWX::OnValueChanged ) 
  EVT_SPINCTRL( XRCID("wxID_SPIN_RIGHT"),    StyleDlgWX::OnValueChanged ) 
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

 XRCCTRL(*this, "wxID_CHOICE_SIZE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_SIZE_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_WEIGHT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_VARIANT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_LINE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_LINE_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_INDENT_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_VALIGN", wxChoice)->SetStringSelection(TtaConvMessageToWX( "%" ));
 XRCCTRL(*this, "wxID_VALIGN_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_ALIGN", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_DECORATION", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_TRANSFORM", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_WORD", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_WORD_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_LETTER", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_LETTER_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_BGATTACH", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_BG_HPOSITION", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_BGHPOS_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "%" ));
 XRCCTRL(*this, "wxID_CHOICE_BG_VPOSITION", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_BGVPOS_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "%" ));
 XRCCTRL(*this, "wxID_CHOICE_BTOP", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_T_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_BTOP_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_BBOTTOM", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_B_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_BBOTTOM_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_BLEFT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_L_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_BLEFT_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_BRIGHT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_R_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_BRIGHT_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_B", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_BORDER_STYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_B_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_MTOP", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_MTOP_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_MBOTTOM", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_MBOTTOM_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_MLEFT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_MLEFT_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_MRIGHT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_MRIGHT_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_PTOP", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_PTOP_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_PBOTTOM", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_PBOTTOM_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_PLEFT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_PLEFT_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_PRIGHT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_PRIGHT_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_MARGIN", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_MARGIN_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_PADDING", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_PADDING_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_WIDTH", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_WIDTH_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_HEIGHT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_HEIGHT_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_CHOICE_DISPLAY", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_VISIBILITY", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_FLOAT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_CLEAR", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_LISTSTYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_CHOICE_LISTPOSITION", wxChoice)->SetStringSelection(TtaConvMessageToWX( "" ));
 XRCCTRL(*this, "wxID_TOP_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_BOTTOM_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_LEFT_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));
 XRCCTRL(*this, "wxID_RIGHT_UNIT", wxChoice)->SetStringSelection(TtaConvMessageToWX( "px" ));

 XRCCTRL(*this, "wx_SIZE_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_LINE_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_INDENT_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_VALIGN_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_WORD_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_LETTER_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_BGHPOS_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_BGVPOS_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_BTOP_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_BBOTTOM_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_BLEFT_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_BRIGHT_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_BORDER_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_MTOP_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_PTOP_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_MBOTTOM_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_PBOTTOM_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_MLEFT_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_PLEFT_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_MRIGHT_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_PRIGHT_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_MARGIN_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_PADDING_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_WIDTH_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_HEIGHT_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_TOP_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_BOTTOM_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_LEFT_DONE", wxCheckBox)->SetValue(FALSE);
 XRCCTRL(*this, "wx_RIGHT_DONE", wxCheckBox)->SetValue(FALSE);

 XRCCTRL(*this, "wxID_SPIN_SIZE", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_SIZE", wxSpinCtrl)->SetRange(0, 400);
 XRCCTRL(*this, "wxID_SPIN_LINE", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_LINE", wxSpinCtrl)->SetRange(0, 400);
 XRCCTRL(*this, "wxID_SPIN_INDENT", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_INDENT", wxSpinCtrl)->SetRange(-400, 400);
 XRCCTRL(*this, "wxID_SPIN_VALIGN", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_WORD", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_LETTER", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_BGHPOS", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_BGHPOS", wxSpinCtrl)->SetRange(0, 400);
 XRCCTRL(*this, "wxID_SPIN_BGVPOS", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_BGVPOS", wxSpinCtrl)->SetRange(0, 400);
 XRCCTRL(*this, "wxID_SPIN_BTOP", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_BBOTTOM", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_BLEFT", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_BRIGHT", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_B", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_MTOP", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_MTOP", wxSpinCtrl)->SetRange(-400, 400);
 XRCCTRL(*this, "wxID_SPIN_PTOP", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_MBOTTOM", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_MBOTTOM", wxSpinCtrl)->SetRange(-400, 400);
 XRCCTRL(*this, "wxID_SPIN_PBOTTOM", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_MLEFT", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_MLEFT", wxSpinCtrl)->SetRange(-400, 400);
 XRCCTRL(*this, "wxID_SPIN_PLEFT", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_MRIGHT", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_MRIGHT", wxSpinCtrl)->SetRange(-400, 400);
 XRCCTRL(*this, "wxID_SPIN_PRIGHT", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_MARGIN", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_MARGIN", wxSpinCtrl)->SetRange(-400, 400);
 XRCCTRL(*this, "wxID_SPIN_PADDING", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_WIDTH", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_WIDTH", wxSpinCtrl)->SetRange(0, 2000);
 XRCCTRL(*this, "wxID_SPIN_HEIGHT", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_HEIGHT", wxSpinCtrl)->SetRange(0, 2000);
 XRCCTRL(*this, "wxID_SPIN_TOP", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_TOP", wxSpinCtrl)->SetRange(-400, 400);
 XRCCTRL(*this, "wxID_SPIN_BOTTOM", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_BOTTOM", wxSpinCtrl)->SetRange(-400, 400);
 XRCCTRL(*this, "wxID_SPIN_LEFT", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_LEFT", wxSpinCtrl)->SetRange(-400, 400);
 XRCCTRL(*this, "wxID_SPIN_RIGHT", wxSpinCtrl)->SetValue(0);
 XRCCTRL(*this, "wxID_SPIN_RIGHT", wxSpinCtrl)->SetRange(-400, 400);
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

  /* tooltip of checked values */
  XRCCTRL(*this, "wx_SIZE_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_LINE_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_INDENT_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_VALIGN_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_WORD_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_LETTER_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_BGHPOS_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_BGVPOS_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_BTOP_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_BBOTTOM_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_BLEFT_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_BRIGHT_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_BORDER_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_MTOP_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_PTOP_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_MBOTTOM_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_PBOTTOM_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_MLEFT_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_PLEFT_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_MRIGHT_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_PRIGHT_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_MARGIN_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_PADDING_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_WIDTH_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_HEIGHT_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_TOP_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_BOTTOM_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_LEFT_DONE", wxCheckBox)->SetToolTip( ptr);
  XRCCTRL(*this, "wx_RIGHT_DONE", wxCheckBox)->SetToolTip( ptr);

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
  Class:  StyleDlgWX
  Method:  OnValueButton
  Description:  
  -----------------------------------------------------------------------*/
void StyleDlgWX::OnValueChanged( wxSpinEvent& event )
{
  int id = event.GetId();

  if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_SIZE")))
    XRCCTRL(*this, "wx_SIZE_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_LINE")))
    XRCCTRL(*this, "wx_LINE_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_INDENT")))
    XRCCTRL(*this, "wx_INDENT_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_VALIGN")))
    XRCCTRL(*this, "wx_VALIGN_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_WORD")))
    XRCCTRL(*this, "wx_WORD_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_LETTER")))
    XRCCTRL(*this, "wx_LETTER_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_BGHPOS")))
    XRCCTRL(*this, "wx_BGHPOS_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_BGVPOS")))
    XRCCTRL(*this, "wx_BGVPOS_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_BTOP")))
    XRCCTRL(*this, "wx_BTOP_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_BBOTTOM")))
    XRCCTRL(*this, "wx_BBOTTOM_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_BLEFT")))
    XRCCTRL(*this, "wx_BLEFT_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_BRIGHT")))
    XRCCTRL(*this, "wx_BRIGHT_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_B")))
    XRCCTRL(*this, "wx_BORDER_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_MTOP")))
    XRCCTRL(*this, "wx_MTOP_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_MBOTTOM")))
    XRCCTRL(*this, "wx_MBOTTOM_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_MLEFT")))
    XRCCTRL(*this, "wx_MLEFT_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_MRIGHT")))
    XRCCTRL(*this, "wx_MRIGHT_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_PTOP")))
    XRCCTRL(*this, "wx_PTOP_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_PBOTTOM")))
    XRCCTRL(*this, "wx_PBOTTOM_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_PLEFT")))
    XRCCTRL(*this, "wx_PLEFT_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_PRIGHT")))
    XRCCTRL(*this, "wx_PRIGHT_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_MARGIN")))
    XRCCTRL(*this, "wx_MARGIN_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_PADDING")))
    XRCCTRL(*this, "wx_PADDING_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_WIDTH")))
    XRCCTRL(*this, "wx_WIDTH_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_HEIGHT")))
    XRCCTRL(*this, "wx_HEIGHT_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_TOP")))
    XRCCTRL(*this, "wx_TOP_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_BOTTOM")))
    XRCCTRL(*this, "wx_BOTTOM_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_LEFT")))
    XRCCTRL(*this, "wx_LEFT_DONE", wxCheckBox)->SetValue(TRUE);
  else if (id == wxXmlResource::GetXRCID(_T("wxID_SPIN_RIGHT")))
    XRCCTRL(*this, "wx_RIGHT_DONE", wxCheckBox)->SetValue(TRUE);    
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
      strcpy (&Buffer[Index], "font-family: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_SIZE", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "font-size: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_SIZE", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_SIZE_DONE", wxCheckBox)->IsChecked() && i >= 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "font-size: ");
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

  value = XRCCTRL(*this, "wxID_CHOICE_LINE", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "line-height: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_LINE", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_LINE_DONE", wxCheckBox)->IsChecked() && i >= 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "line-height: ");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_LINE_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  i = XRCCTRL(*this, "wxID_SPIN_INDENT", wxSpinCtrl)->GetValue();
  if (XRCCTRL(*this, "wx_INDENT_DONE", wxCheckBox)->IsChecked())
    {
      sprintf (text, "%d", i);
      strcpy (&Buffer[Index], "text-indent: ");
      strcat (&Buffer[Index], text);
      value = XRCCTRL(*this, "wxID_INDENT_UNIT", wxChoice)->GetStringSelection();
      if (value.Len() > 0)
        strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }

  value = XRCCTRL(*this, "wxID_CHOICE_VALIGN", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "vertical-align: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_VALIGN", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_VALIGN_DONE", wxCheckBox)->IsChecked())
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "vertical-align: ");
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

  value = XRCCTRL(*this, "wxID_CHOICE_WORD", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "word-spacing: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_WORD", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_WORD_DONE", wxCheckBox)->IsChecked() && i >= 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "word-spacing: ");
          strcat (&Buffer[Index], text);
          value = XRCCTRL(*this, "wxID_WORD_UNIT", wxChoice)->GetStringSelection();
          if (value.Len() > 0)
            strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
          strcat (&Buffer[Index], End_rule);
          Index += strlen (&Buffer[Index]);
        }
    }

  value = XRCCTRL(*this, "wxID_CHOICE_LETTER", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "letter-spacing: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_LETTER", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_LETTER_DONE", wxCheckBox)->IsChecked()  && i >= 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "letter-spacing: ");
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
  wxString        value, svalue, cvalue;
  char            text[10];
  char            pathimage[MAX_LENGTH], *s;
  int             i, j;
  ThotBool        hpos, vpos;

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

      value = XRCCTRL(*this, "wxID_CHOICE_BG_HPOSITION", wxChoice)->GetStringSelection();
      svalue = XRCCTRL(*this, "wxID_CHOICE_BG_VPOSITION", wxChoice)->GetStringSelection();
      i = XRCCTRL(*this, "wxID_SPIN_BGHPOS", wxSpinCtrl)->GetValue();
      j = XRCCTRL(*this, "wxID_SPIN_BGVPOS", wxSpinCtrl)->GetValue();
      hpos = XRCCTRL(*this, "wx_BGHPOS_DONE", wxCheckBox)->IsChecked();
      vpos = XRCCTRL(*this, "wx_BGVPOS_DONE", wxCheckBox)->IsChecked();
      if (value.Len() > 0 || hpos || svalue.Len() > 0 || vpos)
        {
          strcpy (&Buffer[Index], "background-position: ");
          if (value.Len() > 0)
            strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
          else if (hpos)
            {
              sprintf (text, "%d", i);
              strcat (&Buffer[Index], text);
              cvalue = XRCCTRL(*this, "wxID_BGHPOS_UNIT", wxChoice)->GetStringSelection();
              if (cvalue.Len() > 0)
                strcat (&Buffer[Index], (const char*)cvalue.mb_str(wxConvUTF8));
            }
          if ((value.Len() > 0 || hpos) && (svalue.Len() > 0 || vpos))
            strcat (&Buffer[Index], " ");
          if (svalue.Len() > 0)
            strcat (&Buffer[Index], (const char*)svalue.mb_str(wxConvUTF8));
          else if (vpos)
            {
              sprintf (text, "%d", j);
              strcat (&Buffer[Index], text);
              cvalue = XRCCTRL(*this, "wxID_BGVPOS_UNIT", wxChoice)->GetStringSelection();
              if (cvalue.Len() > 0)
                strcat (&Buffer[Index], (const char*)cvalue.mb_str(wxConvUTF8));
            }
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
  value = XRCCTRL(*this, "wxID_CHOICE_BTOP", wxChoice)->GetStringSelection();
  i = XRCCTRL(*this, "wxID_SPIN_BTOP", wxSpinCtrl)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_T_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_T_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 ||
      (XRCCTRL(*this, "wx_BTOP_DONE", wxCheckBox)->IsChecked()  && i >= 0) ||
      svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-top: ");
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      else
        {
          // spin value ?
          if (XRCCTRL(*this, "wx_BTOP_DONE", wxCheckBox)->IsChecked()  && i >= 0)
            {
              sprintf (text, "%d", i);
              strcat (&Buffer[Index], " ");
              strcat (&Buffer[Index], text);
              value = XRCCTRL(*this, "wxID_BTOP_UNIT", wxChoice)->GetStringSelection();
              if (value.Len() > 0)
                strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
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
  value = XRCCTRL(*this, "wxID_CHOICE_BBOTTOM", wxChoice)->GetStringSelection();
  i = XRCCTRL(*this, "wxID_SPIN_BBOTTOM", wxSpinCtrl)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_B_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_B_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 ||
      (XRCCTRL(*this, "wx_BBOTTOM_DONE", wxCheckBox)->IsChecked()  && i >= 0)  ||
      svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-bottom: ");
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      else
        {
          // spin value ?
          if (XRCCTRL(*this, "wx_BBOTTOM_DONE", wxCheckBox)->IsChecked()  && i >= 0)
            {
              sprintf (text, "%d", i);
              strcat (&Buffer[Index], " ");
              strcat (&Buffer[Index], text);
              value = XRCCTRL(*this, "wxID_BBOTTOM_UNIT", wxChoice)->GetStringSelection();
              if (value.Len() > 0)
                strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
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
  value = XRCCTRL(*this, "wxID_CHOICE_BLEFT", wxChoice)->GetStringSelection();
  i = XRCCTRL(*this, "wxID_SPIN_BLEFT", wxSpinCtrl)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_L_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_L_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 ||
      (XRCCTRL(*this, "wx_BLEFT_DONE", wxCheckBox)->IsChecked()  && i >= 0)  ||
      svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-left: ");
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      else
        {
          // spin value ?
          if (XRCCTRL(*this, "wx_BLEFT_DONE", wxCheckBox)->IsChecked()  && i >= 0)
            {
              sprintf (text, "%d", i);
              strcat (&Buffer[Index], " ");
              strcat (&Buffer[Index], text);
              value = XRCCTRL(*this, "wxID_BLEFT_UNIT", wxChoice)->GetStringSelection();
              if (value.Len() > 0)
                strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
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
  value = XRCCTRL(*this, "wxID_CHOICE_BRIGHT", wxChoice)->GetStringSelection();
  i = XRCCTRL(*this, "wxID_SPIN_BRIGHT", wxSpinCtrl)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_R_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_R_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 ||
      (XRCCTRL(*this, "wx_BRIGHT_DONE", wxCheckBox)->IsChecked()  && i >= 0)  ||
      svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border-right: ");
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      else
        {
          // spin value ?
          if (XRCCTRL(*this, "wx_BRIGHT_DONE", wxCheckBox)->IsChecked()  && i >= 0)
            {
              sprintf (text, "%d", i);
              strcat (&Buffer[Index], " ");
              strcat (&Buffer[Index], text);
              value = XRCCTRL(*this, "wxID_BRIGHT_UNIT", wxChoice)->GetStringSelection();
              if (value.Len() > 0)
                  strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
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
  value = XRCCTRL(*this, "wxID_CHOICE_B", wxChoice)->GetStringSelection();
  i = XRCCTRL(*this, "wxID_SPIN_B", wxSpinCtrl)->GetValue();
  svalue = XRCCTRL(*this, "wxID_CHOICE_BORDER_STYLE", wxChoice)->GetStringSelection();
  cvalue = XRCCTRL(*this, "wxID_COMBO_BORDER_COLOR", wxComboBox)->GetValue();
  if (value.Len() > 0 ||
      (XRCCTRL(*this, "wx_BORDER_DONE", wxCheckBox)->IsChecked()  && i >= 0)  ||
      svalue.Len() > 0 || cvalue.Len() > 0)
    {
      strcpy (&Buffer[Index], "border: ");
      if (value.Len() > 0)
        {
          strcat (&Buffer[Index], " ");
          strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
        }
      else
        {
          // spin value ?
          if (XRCCTRL(*this, "wx_BORDER_DONE", wxCheckBox)->IsChecked()  && i >= 0)
            {
              sprintf (text, "%d", i);
              strcat (&Buffer[Index], " ");
              strcat (&Buffer[Index], text);
              value = XRCCTRL(*this, "wxID_B_UNIT", wxChoice)->GetStringSelection();
              if (value.Len() > 0)
                  strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
            }
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
  char            text[10];
  int             i;

  //Margin top
  value = XRCCTRL(*this, "wxID_CHOICE_MTOP", wxChoice)->GetStringSelection();
  if (value.Len() > 0)
    {
      strcpy (&Buffer[Index], "margin-top-width: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_MTOP", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_MTOP_DONE", wxCheckBox)->IsChecked())
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "margin-top: ");
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
      strcpy (&Buffer[Index], "margin-bottom: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_MBOTTOM", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_MBOTTOM_DONE", wxCheckBox)->IsChecked())
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "margin-bottom: ");
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
      strcpy (&Buffer[Index], "margin-left: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_MLEFT", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_MLEFT_DONE", wxCheckBox)->IsChecked())
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "margin-left: ");
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
      strcpy (&Buffer[Index], "margin-right: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_MRIGHT", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_MRIGHT_DONE", wxCheckBox)->IsChecked())
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "margin-right: ");
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
      strcpy (&Buffer[Index], "padding-top: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_PTOP", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_PTOP_DONE", wxCheckBox)->IsChecked()  && i >= 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "padding-top: ");
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
      strcpy (&Buffer[Index], "padding-bottom: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_PBOTTOM", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_PBOTTOM_DONE", wxCheckBox)->IsChecked()  && i >= 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "padding-bottom: ");
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
      strcpy (&Buffer[Index], "padding-left: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_PLEFT", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_PLEFT_DONE", wxCheckBox)->IsChecked()  && i >= 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "padding-left: ");
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
      strcpy (&Buffer[Index], "padding-right: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_PRIGHT", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_PRIGHT_DONE", wxCheckBox)->IsChecked()  && i >= 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "padding-right: ");
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
      strcpy (&Buffer[Index], "margin: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_MARGIN", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_MARGIN_DONE", wxCheckBox)->IsChecked())
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "margin: ");
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
      strcpy (&Buffer[Index], "padding: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_PADDING", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_PADDING_DONE", wxCheckBox)->IsChecked()  && i >= 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "padding: ");
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
      strcpy (&Buffer[Index], "width: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_WIDTH", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_WIDTH_DONE", wxCheckBox)->IsChecked()  && i >= 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "width: ");
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
      strcpy (&Buffer[Index], "height: ");
      strcat (&Buffer[Index], (const char*)value.mb_str(wxConvUTF8));
      strcat (&Buffer[Index], End_rule);
      Index += strlen (&Buffer[Index]);
    }
  else
    {
      // spin value ?
      i = XRCCTRL(*this, "wxID_SPIN_HEIGHT", wxSpinCtrl)->GetValue();
      if (XRCCTRL(*this, "wx_HEIGHT_DONE", wxCheckBox)->IsChecked()  && i >= 0)
        {
          sprintf (text, "%d", i);
          strcpy (&Buffer[Index], "height: ");
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
  char            pathimage[MAX_LENGTH], *s;
  int             i;

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
  i = XRCCTRL(*this, "wxID_SPIN_TOP", wxSpinCtrl)->GetValue();
  if (XRCCTRL(*this, "wx_TOP_DONE", wxCheckBox)->IsChecked())
    {
      sprintf (text, "%d", i);
      strcpy (&Buffer[Index], "top: ");
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
  if (XRCCTRL(*this, "wx_BOTTOM_DONE", wxCheckBox)->IsChecked())
    {
      sprintf (text, "%d", i);
      strcpy (&Buffer[Index], "bottom: ");
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
  if (XRCCTRL(*this, "wx_LEFT_DONE", wxCheckBox)->IsChecked())
    {
      sprintf (text, "%d", i);
      strcpy (&Buffer[Index], "left: ");
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
  if (XRCCTRL(*this, "wx_RIGHT_DONE", wxCheckBox)->IsChecked())
    {
      sprintf (text, "%d", i);
      strcpy (&Buffer[Index], "right: ");
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
