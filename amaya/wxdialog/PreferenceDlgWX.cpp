#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"
#include "wx/arrstr.h"
#include "wx/spinctrl.h"
#include "wx/notebook.h"
#include "wx/xrc/xmlres.h"
#include "AmayaApp.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "MENUconf.h"
#include "MENUconf_f.h"

#include "PreferenceDlgWX.h"

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PreferenceDlgWX, AmayaDialog)
  EVT_BUTTON(     XRCID("wxID_OK"),           PreferenceDlgWX::OnOk )
  EVT_BUTTON(     XRCID("wxID_DEFAULT"),      PreferenceDlgWX::OnDefault )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),       PreferenceDlgWX::OnCancel )
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  PreferenceDlgWX create the dialog used to 
    - Change amaya preferences
  params:
    + parent : parent window
    + title : dialog title
    + ...
  returns:
  ----------------------------------------------------------------------*/
PreferenceDlgWX::PreferenceDlgWX( int ref,
				  wxWindow* parent,
				  const wxArrayString & url_list ) :
  AmayaDialog( NULL, ref )
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("PreferenceDlgWX"));

  wxLogDebug( _T("PreferenceDlgWX::PreferenceDlgWX") );

  // setup dialog title
  SetTitle( TtaConvMessageToWX("TODO (ex: Preferences)") );

  // Setup notebook tab names :
  int page_id;
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  page_id = GetPagePosFromXMLID( _T("wxID_PAGE_GENERAL") );
  if (page_id >= 0)
    p_notebook->SetPageText( page_id, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_GENERAL_MENU)) );


  // update dialog General tab labels with given ones
  XRCCTRL(*this, "wxID_LABEL_HOMEPAGE", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_HOME_PAGE)) );
  XRCCTRL(*this, "wxID_LABEL_CHARMENUSIZE", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_MENU_FONT_SIZE)) );
  XRCCTRL(*this, "wxID_LABEL_CHARZOOM", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_ZOOM)) );
  XRCCTRL(*this, "wxID_LABEL_LG", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DIALOGUE_LANGUAGE)) );

  XRCCTRL(*this, "wxID_CHECK_CCLINE", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_PASTE_LINE_BY_LINE)) );
  XRCCTRL(*this, "wxID_CHECK_BACKUP", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_AUTO_SAVE)) );
  XRCCTRL(*this, "wxID_CHECK_SAVEGEO", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SAVE_GEOMETRY_ON_EXIT)) );
  XRCCTRL(*this, "wxID_CHECK_SHOWBUTTONS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SHOW_BUTTONBAR)) );
  XRCCTRL(*this, "wxID_CHECK_SHOWADDR", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SHOW_TEXTZONE)) );
  XRCCTRL(*this, "wxID_CHECK_SHOWTARGETS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SHOW_TARGETS)) );
  XRCCTRL(*this, "wxID_CHECK_NBSECTIONS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SECTION_NUMBER)) );

  XRCCTRL(*this, "wxID_RADIO_QUICKAXX", wxRadioBox)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ACCESSKEY)) );
  XRCCTRL(*this, "wxID_RADIO_QUICKAXX", wxRadioBox)->SetString(2,TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_NONE)) );

  XRCCTRL(*this, "wxID_OK",      wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage (AMAYA, AM_APPLY_BUTTON)));
  XRCCTRL(*this, "wxID_CANCEL",  wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CANCEL)) );
  XRCCTRL(*this, "wxID_DEFAULT", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON)));

  // setup range of zoom
  XRCCTRL(*this, "wxID_CHARZOOM_VALUE",  wxSlider)->SetRange( 10, 1000 );
  // setup menu font size range
  XRCCTRL(*this, "wxID_CHARMENUSIZE_VALUE",  wxSlider)->SetRange( 8, 20 );

  // fill the combobox with url list
  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE",     wxComboBox)->Append(url_list);
  // initialize it
  //  XRCCTRL(*this, "wxID_COMBOBOX",     wxComboBox)->SetValue(wx_init_value);

  // load current values and send it to the dialog
  GetGeneralConf ();
  SetupDialog_General( GetProp_General() );

  // give focus to ...
  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE",     wxComboBox)->SetFocus();

  SetAutoLayout( TRUE );
}

/*----------------------------------------------------------------------
  Destructor.
  ----------------------------------------------------------------------*/
PreferenceDlgWX::~PreferenceDlgWX()
{
  /* do not call this one because it cancel the link creation */
  /*  ThotCallback (m_Ref, INTEGER_DATA, (char*) 0);*/
}

int PreferenceDlgWX::GetPagePosFromXMLID( const wxString & xml_id )
{
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  wxPanel *    p_page     = (wxPanel *)FindWindow(wxXmlResource::GetXRCID(xml_id));
  int          page_id    = 0;
  bool         found      = false;

  while (!found && page_id < p_notebook->GetPageCount() )
    {
      if ( p_page == p_notebook->GetPage(page_id))
	found = true;
      else
	page_id++;
    }

  if (found)
    return page_id;
  else
    return -1;
}

/*----------------------------------------------------------------------
  SetupDialog_General send init value to dialog 
  params:
    + const PropTab_General & prop : the values to setup into the dialog
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_General( const Prop_General & prop )
{
  wxLogDebug( _T("PreferenceDlgWX::SetupDialog_General") );

  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE",  wxComboBox)->SetValue( TtaConvMessageToWX(prop.HomePage) );

  XRCCTRL(*this, "wxID_CHARZOOM_VALUE",     wxSlider)->SetValue( prop.Zoom );
  XRCCTRL(*this, "wxID_CHARMENUSIZE_VALUE", wxSlider)->SetValue( prop.FontMenuSize );

  XRCCTRL(*this, "wxID_CHECK_CCLINE",      wxCheckBox)->SetValue( prop.PasteLineByLine );
  XRCCTRL(*this, "wxID_CHECK_BACKUP",      wxCheckBox)->SetValue( prop.S_AutoSave );
  XRCCTRL(*this, "wxID_CHECK_SAVEGEO",     wxCheckBox)->SetValue( prop.S_Geometry );
  XRCCTRL(*this, "wxID_CHECK_SHOWBUTTONS", wxCheckBox)->SetValue( prop.S_Buttons );
  XRCCTRL(*this, "wxID_CHECK_SHOWADDR",    wxCheckBox)->SetValue( prop.S_Address );
  XRCCTRL(*this, "wxID_CHECK_SHOWTARGETS", wxCheckBox)->SetValue( prop.S_Targets );
  XRCCTRL(*this, "wxID_CHECK_NBSECTIONS",  wxCheckBox)->SetValue( prop.S_Numbers );

  XRCCTRL(*this, "wxID_RADIO_QUICKAXX",    wxRadioBox)->SetSelection( prop.AccesskeyMod );
  
  XRCCTRL(*this, "wxID_CHOICE_LG", wxChoice)->SetStringSelection( TtaConvMessageToWX(prop.DialogueLang) );
}

/*----------------------------------------------------------------------
  GetValueDialog_General get dialog values
  params:
  returns:
    + PropTab_General prop : the dialog values
  ----------------------------------------------------------------------*/
Prop_General PreferenceDlgWX::GetValueDialog_General()
{
  char            buffer[512];
  wxString        value;
  Prop_General prop;

  memset( &prop, 0, sizeof(Prop_General) );
  memset( &buffer, 0, 512 );

  wxLogDebug( _T("PreferenceDlgWX::GetValueDialog_General") );

  value = XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE",  wxComboBox)->GetValue();
  strcpy( prop.HomePage, (const char*)value.mb_str(wxConvUTF8) );

  prop.Zoom = XRCCTRL(*this, "wxID_CHARZOOM_VALUE",     wxSlider)->GetValue();
  prop.FontMenuSize = XRCCTRL(*this, "wxID_CHARMENUSIZE_VALUE", wxSlider)->GetValue();

  prop.PasteLineByLine = XRCCTRL(*this, "wxID_CHECK_CCLINE",      wxCheckBox)->GetValue();
  prop.S_AutoSave      = XRCCTRL(*this, "wxID_CHECK_BACKUP",      wxCheckBox)->GetValue();
  prop.S_Geometry      = XRCCTRL(*this, "wxID_CHECK_SAVEGEO",     wxCheckBox)->GetValue();
  prop.S_Buttons       = XRCCTRL(*this, "wxID_CHECK_SHOWBUTTONS", wxCheckBox)->GetValue();
  prop.S_Address       = XRCCTRL(*this, "wxID_CHECK_SHOWADDR",    wxCheckBox)->GetValue();
  prop.S_Targets       = XRCCTRL(*this, "wxID_CHECK_SHOWTARGETS", wxCheckBox)->GetValue();
  prop.S_Numbers       = XRCCTRL(*this, "wxID_CHECK_NBSECTIONS",  wxCheckBox)->GetValue();

  prop.AccesskeyMod    = XRCCTRL(*this, "wxID_RADIO_QUICKAXX",    wxRadioBox)->GetSelection();

  value = XRCCTRL(*this, "wxID_CHOICE_LG", wxChoice)->GetStringSelection();
  strcpy( prop.DialogueLang, (const char*)value.mb_str(wxConvUTF8) );

  return prop;
}

/*----------------------------------------------------------------------
  OnOk called when the user validates his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnOk( wxCommandEvent& event )
{
  SetProp_General(GetValueDialog_General());
  ValidateGeneralConf ();
  SetGeneralConf();
  Close();
}

/*----------------------------------------------------------------------
  OnDefault reset to default value the current configuration page
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnDefault( wxCommandEvent& event )
{
  GetDefaultGeneralConf();
  SetupDialog_General( GetProp_General() );
}

/*----------------------------------------------------------------------
  OnCancel called when the user clicks on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnCancel( wxCommandEvent& event )
{
  Close();
}

#endif /* _WX */
