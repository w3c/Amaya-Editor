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

static char Buffer[100];

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(StyleDlgWX, AmayaDialog)

  EVT_NOTEBOOK_PAGE_CHANGED( XRCID("wxID_NOTEBOOK"), StyleDlgWX::OnPageChanged )

  EVT_BUTTON(     XRCID("wxID_OK"),           StyleDlgWX::OnOk )
  EVT_BUTTON(     XRCID("wxID_DEFAULT"),      StyleDlgWX::OnDefault )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),       StyleDlgWX::OnCancel )

  EVT_BUTTON(     XRCID("wxID_BUTTON_TEXTCOLOR"),    StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_BACKCOLOR"),    StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_SELCOLOR"),     StyleDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_SELBACKCOLOR"), StyleDlgWX::OnColorPalette )
  EVT_COMBOBOX( XRCID("wxID_COMBO_SELBACKCOLOR"),    StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_SELCOLOR"),        StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_BACKCOLOR"),       StyleDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_TEXTCOLOR"),       StyleDlgWX::OnColorChanged )
  EVT_TEXT( XRCID("wxID_COMBO_SELBACKCOLOR"),    StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_SELCOLOR"),        StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_BACKCOLOR"),       StyleDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_TEXTCOLOR"),       StyleDlgWX::OnColorTextChanged )
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
  value = XRCCTRL(*this, "wxID_COMBOBOX_FAMILY", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      strcpy (Buffer, "font-family:");
      strcpy (Buffer, (const char*)value.mb_str(wxConvUTF8));
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
  value = XRCCTRL(*this, "wxID_COMBO_SELBACKCOLOR", wxComboBox)->GetValue();
  strcpy( Buffer, (const char*)value.mb_str(wxConvUTF8) );

  value = XRCCTRL(*this, "wxID_COMBO_SELCOLOR", wxComboBox)->GetValue();
  strcpy( Buffer, (const char*)value.mb_str(wxConvUTF8) );

  value = XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox)->GetValue();
  strcpy( Buffer, (const char*)value.mb_str(wxConvUTF8) );

  value = XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox)->GetValue();
  strcpy( Buffer, (const char*)value.mb_str(wxConvUTF8) );
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
  int selcolor_id = wxXmlResource::GetXRCID(_T("wxID_BUTTON_SELCOLOR"));
  int selbackcolor_id = wxXmlResource::GetXRCID(_T("wxID_BUTTON_SELBACKCOLOR"));

  // First of all setup the dialog with the combobox color
  wxString value;
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  char buffer[512];
  if (event.GetId() == textcolor_id)
    value = XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox)->GetValue();
  else if (event.GetId() == backcolor_id)
    value = XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox)->GetValue();
  else if (event.GetId() == selcolor_id)
    value = XRCCTRL(*this, "wxID_COMBO_SELCOLOR", wxComboBox)->GetValue();
  else if (event.GetId() == selbackcolor_id)
    value = XRCCTRL(*this, "wxID_COMBO_SELBACKCOLOR", wxComboBox)->GetValue();
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
      if (event.GetId() == textcolor_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox);
      else if (event.GetId() == backcolor_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox);
      else if (event.GetId() == selcolor_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_SELCOLOR", wxComboBox);
      else if (event.GetId() == selbackcolor_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_SELBACKCOLOR", wxComboBox);    
      if (p_combo)
        {
          p_combo->Append( TtaConvMessageToWX(color_string) );
          p_combo->SetValue( TtaConvMessageToWX(color_string) );
#ifdef _WINDOWS
          // on windows, the TextChanged event is not generated automaticaly when SetValue is called
          // just simulate it.
          wxCommandEvent event;
          OnColorTextChanged(event);
#endif /* _WINDOWS */
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
  wxString value1;
  wxString value2;
  wxString value3;
  wxString value4;
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  char buffer[512];
  value1 = XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox)->GetValue();
  value2 = XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox)->GetValue();
  value3 = XRCCTRL(*this, "wxID_COMBO_SELCOLOR", wxComboBox)->GetValue();
  value4 = XRCCTRL(*this, "wxID_COMBO_SELBACKCOLOR", wxComboBox)->GetValue();
  strcpy(buffer, (const char*)value1.mb_str(wxConvUTF8) );
  TtaGiveRGB (buffer, &red, &green, &blue);
  wxColour col1( red, green, blue );
  strcpy(buffer, (const char*)value2.mb_str(wxConvUTF8) );
  TtaGiveRGB (buffer, &red, &green, &blue);
  wxColour col2( red, green, blue );
  strcpy(buffer, (const char*)value3.mb_str(wxConvUTF8) );
  TtaGiveRGB (buffer, &red, &green, &blue);
  wxColour col3( red, green, blue );
  strcpy(buffer, (const char*)value4.mb_str(wxConvUTF8) );
  TtaGiveRGB (buffer, &red, &green, &blue);
  wxColour col4( red, green, blue );

  // setup background colours
  XRCCTRL(*this, "wxID_BUTTON_SELBACKCOLOR", wxBitmapButton)->SetBackgroundColour( col4 );
  XRCCTRL(*this, "wxID_BUTTON_SELCOLOR", wxBitmapButton)->SetBackgroundColour( col3 );
  XRCCTRL(*this, "wxID_BUTTON_BACKCOLOR", wxBitmapButton)->SetBackgroundColour( col2 );
  XRCCTRL(*this, "wxID_BUTTON_TEXTCOLOR", wxBitmapButton)->SetBackgroundColour( col1 );

  event.Skip();
}

/*----------------------------------------------------------------------
  GetValueDialog_Box get dialog values
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleDlgWX::GetValueDialog_Box()
{
  wxString        value;
  value = XRCCTRL(*this, "wxID_COMBOBOX_FAMILY", wxComboBox)->GetValue();
  strcpy( Buffer, (const char*)value.mb_str(wxConvUTF8) );
}

/************************************************************************/
/* Box tab                                                              */
/************************************************************************/


/*----------------------------------------------------------------------
  OnOk called when the user validates his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleDlgWX::OnOk( wxCommandEvent& event )
{
  Buffer[0] = EOS;
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
