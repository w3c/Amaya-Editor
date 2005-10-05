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
#include "MENUconf.h"
#include "MENUconf_f.h"

#include "PreferenceDlgWX.h"

bool PreferenceDlgWX::m_OnApplyLock = FALSE;

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PreferenceDlgWX, AmayaDialog)

  EVT_NOTEBOOK_PAGE_CHANGED( XRCID("wxID_NOTEBOOK"), PreferenceDlgWX::OnPageChanged )

  EVT_BUTTON(     XRCID("wxID_OK"),           PreferenceDlgWX::OnOk )
  EVT_BUTTON(     XRCID("wxID_DEFAULT"),      PreferenceDlgWX::OnDefault )
  EVT_BUTTON(     XRCID("wxID_CANCEL"),       PreferenceDlgWX::OnCancel )

  // Clear url list callback
  EVT_BUTTON(     XRCID("wxID_BUTTON_CLEARURL"), PreferenceDlgWX::OnClearUrlList )

  // Cache tab callbacks
  EVT_BUTTON(     XRCID("wxID_BUTTON_EMPTYCACHE"), PreferenceDlgWX::OnEmptyCache )
  
  // Color tab callbacks
  EVT_BUTTON(     XRCID("wxID_BUTTON_TEXTCOLOR"),    PreferenceDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_BACKCOLOR"),    PreferenceDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_SELCOLOR"),     PreferenceDlgWX::OnColorPalette )
  EVT_BUTTON(     XRCID("wxID_BUTTON_SELBACKCOLOR"), PreferenceDlgWX::OnColorPalette )
  EVT_COMBOBOX( XRCID("wxID_COMBO_SELBACKCOLOR"),    PreferenceDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_SELCOLOR"),        PreferenceDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_BACKCOLOR"),       PreferenceDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_TEXTCOLOR"),       PreferenceDlgWX::OnColorChanged )
  EVT_TEXT( XRCID("wxID_COMBO_SELBACKCOLOR"),    PreferenceDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_SELCOLOR"),        PreferenceDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_BACKCOLOR"),       PreferenceDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_TEXTCOLOR"),       PreferenceDlgWX::OnColorTextChanged )

  // Geometry tab callbacks
  EVT_BUTTON( XRCID("wxID_BUTTON_GEOMSAVE"),    PreferenceDlgWX::OnGeomSave )
  EVT_BUTTON( XRCID("wxID_BUTTON_GEOMRESTOR"),  PreferenceDlgWX::OnGeomRestor )
  

  EVT_CLOSE( PreferenceDlgWX::OnClose )
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
  AmayaDialog( parent, ref )
  ,m_IsInitialized(false) // this flag is used to know when events can be proceed
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("PreferenceDlgWX"));
  m_UrlList = url_list;

#ifndef DAV
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  // invalid WebDAV Page
  int page_id = GetPagePosFromXMLID( _T("wxID_PAGE_DAV") );
  if (page_id)
    p_notebook->DeletePage(page_id );
#endif /* DAV */

  // setup dialog title
  SetTitle( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DLGPREFERENCE_TITLE)));

  // send labels to dialog (labels depends on language)
  SetupLabelDialog_General();
  SetupLabelDialog_Browse();
  SetupLabelDialog_Publish();
  SetupLabelDialog_Cache();
  SetupLabelDialog_Proxy();
  SetupLabelDialog_Color();
  SetupLabelDialog_Geometry();
#ifdef ANNOTATIONS
  SetupLabelDialog_Annot();
#endif /* ANNOTATIONS */
#ifdef DAV
  SetupLabelDialog_DAV();
#endif /* DAV */

  XRCCTRL(*this, "wxID_OK",      wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_APPLY_BUTTON)));
  XRCCTRL(*this, "wxID_CANCEL",  wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DONE)));
  XRCCTRL(*this, "wxID_DEFAULT", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DEFAULT_BUTTON)));

  // load current values and send it to the dialog
  SetupDialog_General( GetProp_General() );
  SetupDialog_Browse( GetProp_Browse() );
  SetupDialog_Publish( GetProp_Publish() );
  SetupDialog_Cache( GetProp_Cache() );
  SetupDialog_Proxy( GetProp_Proxy() );
  SetupDialog_Color( GetProp_Color() );
#ifdef ANNOTATIONS
  SetupDialog_Annot( GetProp_Annot() );
#endif /* ANNOTATIONS */
#ifdef DAV
  SetupDialog_DAV( GetProp_DAV() );
#endif /* DAV */

  // give focus to ...
  //  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->SetFocus();

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
PreferenceDlgWX::~PreferenceDlgWX()
{
  /* do not call this one because it cancel the link creation */
  /*  ThotCallback (m_Ref, INTEGER_DATA, (char*) 0);*/
}

/*----------------------------------------------------------------------
  Return the @ of corresponding widget from the given string id.
  ----------------------------------------------------------------------*/
int PreferenceDlgWX::GetPagePosFromXMLID( const wxString & xml_id )
{
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  wxPanel *    p_page     = (wxPanel *)FindWindow(wxXmlResource::GetXRCID(xml_id));
  int          page_id    = 0;
  bool         found      = false;

  while (!found && page_id < (int) p_notebook->GetPageCount() )
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
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnPageChanged( wxNotebookEvent& event )
{
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  wxPanel *    p_new_page = (wxPanel *)((event.GetSelection()>=0 && p_notebook)?p_notebook->GetPage(event.GetSelection()):NULL);

  if(!m_IsInitialized || !p_new_page || !XRCCTRL(*this,"wxID_OK",wxButton) || !XRCCTRL(*this,"wxID_DEFAULT",wxButton))
  {
    event.Skip();
    return;
  }

  int page_id_geom = wxXmlResource::GetXRCID(_T("wxID_PAGE_GEOMETRY"));

  if ( p_new_page->GetId() == page_id_geom )
    {
      // the new page is Geometry => hide the bottom buttons (ok, default)
      XRCCTRL(*this, "wxID_OK",      wxButton)->Hide();
      XRCCTRL(*this, "wxID_DEFAULT", wxButton)->Hide();
    }
  else
    {
      XRCCTRL(*this, "wxID_OK",      wxButton)->Show();
      XRCCTRL(*this, "wxID_DEFAULT", wxButton)->Show();
    }

  event.Skip();
}

/************************************************************************/
/* General tab                                                          */
/************************************************************************/

/*----------------------------------------------------------------------
  SetupLabelDialog_General init labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupLabelDialog_General()
{
  // Setup notebook tab names :
  int page_id;
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  page_id = GetPagePosFromXMLID( _T("wxID_PAGE_GENERAL") );
  if (page_id >= 0)
    p_notebook->SetPageText( page_id, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_GENERAL_MENU)) );

  // update dialog General tab labels with given ones
  XRCCTRL(*this, "wxID_LABEL_HOMEPAGE", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_HOME_PAGE)) );
  XRCCTRL(*this, "wxID_LABEL_CHARZOOM", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_ZOOM)) );
  XRCCTRL(*this, "wxID_LABEL_LG", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DIALOGUE_LANGUAGE)) );

  XRCCTRL(*this, "wxID_CHECK_CCLINE", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_PASTE_LINE_BY_LINE)) );
  XRCCTRL(*this, "wxID_CHECK_BACKUP", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_AUTO_SAVE)) );
  XRCCTRL(*this, "wxID_CHECK_SAVEGEO", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SAVE_GEOMETRY_ON_EXIT)) );
  XRCCTRL(*this, "wxID_CHECK_NOALIASING", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_NOALIASING)) );
  XRCCTRL(*this, "wxID_CHECK_DATE", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DATE)) );
  XRCCTRL(*this, "wxID_CHECK_SHOWTARGETS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SHOW_TARGETS)) );
  XRCCTRL(*this, "wxID_CHECK_SHOWSHORTCUTS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_SHOWSHORTCUTS)));
  XRCCTRL(*this, "wxID_CHECK_SHOWTEMPLATES", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_SHOW_TEMPLATES)));

  XRCCTRL(*this, "wxID_RADIO_QUICKAXX", wxRadioBox)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ACCESSKEY)) );
  XRCCTRL(*this, "wxID_RADIO_QUICKAXX", wxRadioBox)->SetString(2,TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_NONE)) );

  // setup range of zoom
  XRCCTRL(*this, "wxID_CHARZOOM_VALUE", wxSpinCtrl)->SetRange( 10, 1000 );

  // fill the combobox with url list
  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->Append(m_UrlList);
}

/*----------------------------------------------------------------------
  SetupDialog_General send init value to dialog 
  params:
    + const PropTab_General & prop : the values to setup into the dialog
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_General( const Prop_General & prop )
{
  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->SetValue( TtaConvMessageToWX(prop.HomePage) );

  XRCCTRL(*this, "wxID_CHARZOOM_VALUE", wxSpinCtrl)->SetValue( prop.Zoom );

  XRCCTRL(*this, "wxID_CHECK_CCLINE", wxCheckBox)->SetValue( prop.PasteLineByLine );
  XRCCTRL(*this, "wxID_CHECK_BACKUP", wxCheckBox)->SetValue( prop.S_AutoSave );
  XRCCTRL(*this, "wxID_CHECK_SAVEGEO", wxCheckBox)->SetValue( prop.S_Geometry );
  XRCCTRL(*this, "wxID_CHECK_NOALIASING", wxCheckBox)->SetValue( prop.S_NoAliasing );
  XRCCTRL(*this, "wxID_CHECK_DATE", wxCheckBox)->SetValue( prop.S_DATE );
  XRCCTRL(*this, "wxID_CHECK_SHOWTARGETS", wxCheckBox)->SetValue( prop.S_Targets );
  XRCCTRL(*this, "wxID_CHECK_SHOWSHORTCUTS", wxCheckBox)->SetValue( prop.S_Shortcuts );
  XRCCTRL(*this, "wxID_CHECK_SHOWTEMPLATES", wxCheckBox)->SetValue( prop.S_Templates );

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
  wxString        value;
  Prop_General    prop;
  memset( &prop, 0, sizeof(Prop_General) );

  value = XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->GetValue();
  strcpy( prop.HomePage, (const char*)value.mb_str(wxConvUTF8) );

  prop.Zoom = XRCCTRL(*this, "wxID_CHARZOOM_VALUE",     wxSpinCtrl)->GetValue();
  prop.PasteLineByLine = XRCCTRL(*this, "wxID_CHECK_CCLINE", wxCheckBox)->GetValue();
  prop.S_AutoSave = XRCCTRL(*this, "wxID_CHECK_BACKUP", wxCheckBox)->GetValue();
  prop.S_Geometry = XRCCTRL(*this, "wxID_CHECK_SAVEGEO", wxCheckBox)->GetValue();
  prop.S_NoAliasing = XRCCTRL(*this, "wxID_CHECK_NOALIASING", wxCheckBox)->GetValue();
  prop.S_DATE = XRCCTRL(*this, "wxID_CHECK_DATE", wxCheckBox)->GetValue();
  prop.S_Targets = XRCCTRL(*this, "wxID_CHECK_SHOWTARGETS", wxCheckBox)->GetValue();
  prop.S_Shortcuts = XRCCTRL(*this, "wxID_CHECK_SHOWSHORTCUTS", wxCheckBox)->GetValue();
  prop.S_Templates = XRCCTRL(*this, "wxID_CHECK_SHOWTEMPLATES", wxCheckBox)->GetValue();
  prop.AccesskeyMod = XRCCTRL(*this, "wxID_RADIO_QUICKAXX", wxRadioBox)->GetSelection();

  value = XRCCTRL(*this, "wxID_CHOICE_LG", wxChoice)->GetStringSelection();
  strcpy( prop.DialogueLang, (const char*)value.mb_str(wxConvUTF8) );

  return prop;
}

/************************************************************************/
/* Browse tab                                                           */
/************************************************************************/

/*----------------------------------------------------------------------
  SetupLabelDialog_Browse init labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupLabelDialog_Browse()
{
  // Setup notebook tab names :
  int page_id;
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  page_id = GetPagePosFromXMLID( _T("wxID_PAGE_BROWSE") );
  if (page_id >= 0)
    p_notebook->SetPageText( page_id, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_BROWSE_MENU)) );

  // update dialog General tab labels with given ones
  XRCCTRL(*this, "wxID_RADIO_OPENLOC", wxRadioBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_OPENLOC)) );
  XRCCTRL(*this, "wxID_RADIO_OPENLOC", wxRadioBox)->SetString(0, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_REPLACECURRENT)));
#ifdef _MACOS
  XRCCTRL(*this, "wxID_RADIO_OPENLOC", wxRadioBox)->SetString(1, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_INNEWTAB_MACOS)));
#else /* _MACOS */
  XRCCTRL(*this, "wxID_RADIO_OPENLOC", wxRadioBox)->SetString(1, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_INNEWTAB)));
#endif /*  _MACOS */
  XRCCTRL(*this, "wxID_RADIO_OPENLOC", wxRadioBox)->SetString(2, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_INNEWWINDOW)));
  XRCCTRL(*this, "wxID_LABEL_SCREEN", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_SCREEN_TYPE)) );
  XRCCTRL(*this, "wxID_CHECK_LOADIMG", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_LOAD_IMAGES)) );
  XRCCTRL(*this, "wxID_CHECK_LOADOBJ", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_LOAD_OBJECTS)) );
  XRCCTRL(*this, "wxID_CHECK_SHOWBACKGROUND", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SHOW_BG_IMAGES)) );
  XRCCTRL(*this, "wxID_CHECK_APPLYCSS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_LOAD_CSS)) );
  XRCCTRL(*this, "wxID_CHECK_LINKDBCLICK", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ENABLE_DOUBLECLICK)) );
  XRCCTRL(*this, "wxID_CHECK_ENABLEFTP", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ENABLE_FTP)) );
  XRCCTRL(*this, "wxID_LABEL_LANNEGLISTLG", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_LANG_NEGOTIATION)) );
  XRCCTRL(*this, "wxID_CHECK_WARNCTAB", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_WARNCTAB)) );
  XRCCTRL(*this, "wxID_LABEL_MAXURL", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_MAXURLLIST)) );
  XRCCTRL(*this, "wxID_BUTTON_CLEARURL", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_CLEARURLLIST)) );
}

/*----------------------------------------------------------------------
  SetupDialog_Browse send init value to dialog 
  params:
    + const PropTab_Browse & prop : the values to setup into the dialog
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_Browse( const Prop_Browse & prop )
{
  XRCCTRL(*this, "wxID_RADIO_OPENLOC", wxRadioBox)->SetSelection( prop.OpeningLocation );
  XRCCTRL(*this, "wxID_CHECK_LOADIMG", wxCheckBox)->SetValue( prop.LoadImages );
  XRCCTRL(*this, "wxID_CHECK_LOADOBJ", wxCheckBox)->SetValue( prop.LoadObjects );
  XRCCTRL(*this, "wxID_CHECK_SHOWBACKGROUND", wxCheckBox)->SetValue( prop.BgImages );
  XRCCTRL(*this, "wxID_CHECK_APPLYCSS", wxCheckBox)->SetValue( prop.LoadCss );
  XRCCTRL(*this, "wxID_CHECK_LINKDBCLICK", wxCheckBox)->SetValue( prop.DoubleClick );
  XRCCTRL(*this, "wxID_CHECK_ENABLEFTP", wxCheckBox)->SetValue( prop.EnableFTP );
  XRCCTRL(*this, "wxID_CHECK_WARNCTAB", wxCheckBox)->SetValue( prop.WarnCTab );
  XRCCTRL(*this, "wxID_CHOICE_SCREEN", wxChoice)->SetStringSelection( TtaConvMessageToWX(prop.ScreenType) );
  XRCCTRL(*this, "wxID_VALUE_LANNEGLISTLG", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.LanNeg) );
  XRCCTRL(*this, "wxID_VALUE_MAXURL", wxSpinCtrl)->SetValue( prop.MaxURL );
}

/*----------------------------------------------------------------------
  GetValueDialog_Browse get dialog values
  params:
  returns:
    + PropTab_Browse prop : the dialog values
  ----------------------------------------------------------------------*/
Prop_Browse PreferenceDlgWX::GetValueDialog_Browse()
{
  wxString        value;
  Prop_Browse     prop;
  memset( &prop, 0, sizeof(Prop_Browse) );

  prop.OpeningLocation = XRCCTRL(*this, "wxID_RADIO_OPENLOC", wxRadioBox)->GetSelection();
  prop.LoadImages  = XRCCTRL(*this, "wxID_CHECK_LOADIMG", wxCheckBox)->GetValue();
  prop.LoadObjects = XRCCTRL(*this, "wxID_CHECK_LOADOBJ", wxCheckBox)->GetValue();
  prop.BgImages = XRCCTRL(*this, "wxID_CHECK_SHOWBACKGROUND", wxCheckBox)->GetValue();
  prop.LoadCss = XRCCTRL(*this, "wxID_CHECK_APPLYCSS", wxCheckBox)->GetValue();
  prop.DoubleClick = XRCCTRL(*this, "wxID_CHECK_LINKDBCLICK", wxCheckBox)->GetValue();
  prop.EnableFTP = XRCCTRL(*this, "wxID_CHECK_ENABLEFTP", wxCheckBox)->GetValue();
  prop.WarnCTab = XRCCTRL(*this, "wxID_CHECK_WARNCTAB", wxCheckBox)->GetValue();
  
  value = XRCCTRL(*this, "wxID_CHOICE_SCREEN", wxChoice)->GetStringSelection();
  strcpy( prop.ScreenType, (const char*)value.mb_str(wxConvUTF8) );

  value = XRCCTRL(*this, "wxID_VALUE_LANNEGLISTLG",  wxTextCtrl)->GetValue();
  strcpy( prop.LanNeg, (const char*)value.mb_str(wxConvUTF8) );

  prop.MaxURL = XRCCTRL(*this, "wxID_VALUE_MAXURL", wxSpinCtrl)->GetValue();

  return prop;
}

/************************************************************************/
/* Publish tab                                                          */
/************************************************************************/

/*----------------------------------------------------------------------
  SetupLabelDialog_Publish init labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupLabelDialog_Publish()
{
  // Setup notebook tab names :
  int page_id;
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  page_id = GetPagePosFromXMLID( _T("wxID_PAGE_PUBLISH") );
  if (page_id >= 0)
    p_notebook->SetPageText( page_id, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_PUBLISH_MENU)) );

  XRCCTRL(*this, "wxID_LABEL_CHARSET", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DEFAULT_CHARSET)) );
  XRCCTRL(*this, "wxID_LABEL_DEFAULTNAME", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DEFAULT_NAME)) );
  XRCCTRL(*this, "wxID_LABEL_REDIRECT", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SAFE_PUT_REDIRECT)) );

  XRCCTRL(*this, "wxID_CHECK_XHTML", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_USE_XHTML_MIMETYPE)) );
  XRCCTRL(*this, "wxID_CHECK_ETAGS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_USE_ETAGS)) );
  XRCCTRL(*this, "wxID_CHECK_PUTGET", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_VERIFY_PUT)) );
  XRCCTRL(*this, "wxID_CHECK_CRLF", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_EXPORT_CRLF)) );
  XRCCTRL(*this, "wxID_LABEL_EXPORTLENGTH", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_EXPORT_LENGTH)) );
  XRCCTRL(*this, "wxID_EXPORTLENGTH", wxSpinCtrl)->SetRange( 0, 1000 );
}

/*----------------------------------------------------------------------
  SetupDialog_Publish send init value to dialog 
  params:
    + const PropTab_Publish & prop : the values to setup into the dialog
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_Publish( const Prop_Publish & prop )
{
  XRCCTRL(*this, "wxID_CHOICE_CHARSET", wxChoice)->SetStringSelection( TtaConvMessageToWX(prop.CharsetType) );

  XRCCTRL(*this, "wxID_CHECK_XHTML", wxCheckBox)->SetValue( prop.UseXHTMLMimeType );
  XRCCTRL(*this, "wxID_CHECK_ETAGS", wxCheckBox)->SetValue( prop.LostUpdateCheck );
  XRCCTRL(*this, "wxID_CHECK_PUTGET", wxCheckBox)->SetValue( prop.VerifyPublish );
  XRCCTRL(*this, "wxID_CHECK_CRLF", wxCheckBox)->SetValue( prop.ExportCRLF );
  XRCCTRL(*this, "wxID_EXPORTLENGTH", wxSpinCtrl)->SetValue( prop.ExportLength );

  XRCCTRL(*this, "wxID_VALUE_DEFAULTNAME", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.DefaultName) );
  XRCCTRL(*this, "wxID_VALUE_REDIRECT", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.SafePutRedirect) );
}

/*----------------------------------------------------------------------
  GetValueDialog_Publish get dialog values
  params:
  returns:
    + Prop_Publish prop : the dialog values
  ----------------------------------------------------------------------*/
Prop_Publish PreferenceDlgWX::GetValueDialog_Publish()
{
  wxString        value;
  Prop_Publish     prop;
  memset( &prop, 0, sizeof(Prop_Publish) );

  value = XRCCTRL(*this, "wxID_CHOICE_CHARSET", wxChoice)->GetStringSelection();
  strcpy( prop.CharsetType, (const char*)value.mb_str(wxConvUTF8) );

  prop.UseXHTMLMimeType = XRCCTRL(*this, "wxID_CHECK_XHTML", wxCheckBox)->GetValue();
  prop.LostUpdateCheck  = XRCCTRL(*this, "wxID_CHECK_ETAGS", wxCheckBox)->GetValue();
  prop.VerifyPublish    = XRCCTRL(*this, "wxID_CHECK_PUTGET", wxCheckBox)->GetValue();
  prop.ExportCRLF       = XRCCTRL(*this, "wxID_CHECK_CRLF", wxCheckBox)->GetValue();
  prop.ExportLength     = XRCCTRL(*this, "wxID_EXPORTLENGTH", wxSpinCtrl)->GetValue();

  value = XRCCTRL(*this, "wxID_VALUE_DEFAULTNAME", wxTextCtrl)->GetValue();
  strcpy( prop.DefaultName, (const char*)value.mb_str(wxConvUTF8) );

  value = XRCCTRL(*this, "wxID_VALUE_REDIRECT", wxTextCtrl)->GetValue();
  strcpy( prop.SafePutRedirect, (const char*)value.mb_str(wxConvUTF8) );

  return prop;
}


/************************************************************************/
/* Cache tab                                                            */
/************************************************************************/

/*----------------------------------------------------------------------
  SetupLabelDialog_Cache init labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupLabelDialog_Cache()
{
  // Setup notebook tab names :
  int page_id;
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  page_id = GetPagePosFromXMLID( _T("wxID_PAGE_CACHE") );
  if (page_id >= 0)
    p_notebook->SetPageText( page_id, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_CACHE_MENU)) );

  XRCCTRL(*this, "wxID_LABEL_CACHEDIR", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_CACHE_DIR)) );
  XRCCTRL(*this, "wxID_LABEL_CACHESIZE", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_CACHE_SIZE)) );
  XRCCTRL(*this, "wxID_LABEL_MAXSIZEITEM", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_CACHE_ENTRY_SIZE)) );

  XRCCTRL(*this, "wxID_CHECK_ENABLECACHE", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ENABLE_CACHE)) );
  XRCCTRL(*this, "wxID_CHECK_PROTECTEDDOC", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_CACHE_PROT_DOCS)) );
  XRCCTRL(*this, "wxID_CHECK_DISCO", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DISCONNECTED_MODE)) );
  XRCCTRL(*this, "wxID_CHECK_EXPIGNORE", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_IGNORE_EXPIRES)) );

  XRCCTRL(*this, "wxID_BUTTON_EMPTYCACHE", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_FLUSH_CACHE_BUTTON)) );

  XRCCTRL(*this, "wxID_CACHESIZE_VALUE", wxSpinCtrl)->SetRange( 1, 100 );
  XRCCTRL(*this, "wxID_MAXSIZEITEM_VALUE", wxSpinCtrl)->SetRange( 1, 5 );
}

/*----------------------------------------------------------------------
  SetupDialog_Cache send init value to dialog 
  params:
    + const Prop_Cache & prop : the values to setup into the dialog
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_Cache( const Prop_Cache & prop )
{
  XRCCTRL(*this, "wxID_CHECK_ENABLECACHE",  wxCheckBox)->SetValue( prop.EnableCache );
  XRCCTRL(*this, "wxID_CHECK_PROTECTEDDOC", wxCheckBox)->SetValue( prop.CacheProtectedDocs );
  XRCCTRL(*this, "wxID_CHECK_DISCO", wxCheckBox)->SetValue( prop.CacheDisconnectMode );
  XRCCTRL(*this, "wxID_CHECK_EXPIGNORE", wxCheckBox)->SetValue( prop.CacheExpireIgnore );

  XRCCTRL(*this, "wxID_VALUE_CACHEDIR", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.CacheDirectory) );

  XRCCTRL(*this, "wxID_CACHESIZE_VALUE",  wxSpinCtrl)->SetValue( prop.CacheSize );
  XRCCTRL(*this, "wxID_MAXSIZEITEM_VALUE",  wxSpinCtrl)->SetValue( prop.MaxCacheFile );
}

/*----------------------------------------------------------------------
  GetValueDialog_Cache get dialog values
  params:
  returns:
    + Prop_Cache prop : the dialog values
  ----------------------------------------------------------------------*/
Prop_Cache PreferenceDlgWX::GetValueDialog_Cache()
{
  wxString        value;
  Prop_Cache      prop;
  memset( &prop, 0, sizeof(Prop_Cache) );

  prop.EnableCache = XRCCTRL(*this, "wxID_CHECK_ENABLECACHE", wxCheckBox)->GetValue();
  prop.CacheProtectedDocs  = XRCCTRL(*this, "wxID_CHECK_PROTECTEDDOC", wxCheckBox)->GetValue();
  prop.CacheDisconnectMode = XRCCTRL(*this, "wxID_CHECK_DISCO", wxCheckBox)->GetValue();
  prop.CacheExpireIgnore = XRCCTRL(*this, "wxID_CHECK_EXPIGNORE", wxCheckBox)->GetValue();

  value = XRCCTRL(*this, "wxID_VALUE_CACHEDIR", wxTextCtrl)->GetValue();
  strcpy( prop.CacheDirectory, (const char*)value.mb_str(wxConvUTF8) );

  prop.CacheSize = XRCCTRL(*this, "wxID_CACHESIZE_VALUE",  wxSpinCtrl)->GetValue();
  prop.MaxCacheFile = XRCCTRL(*this, "wxID_MAXSIZEITEM_VALUE",  wxSpinCtrl)->GetValue();

  return prop;
}

/*----------------------------------------------------------------------
  OnEmptyCache is called when the user click on emptycache button (Cache tab)
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnEmptyCache( wxCommandEvent& event )
{
  ThotCallback (GetPrefCacheBase() + CacheMenu, INTEGER_DATA, (char*) 3);
}

/*----------------------------------------------------------------------
  OnClearUrlList is called when the user click on clear url button (Browse tab)
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnClearUrlList( wxCommandEvent& event )
{
  ThotCallback (GetPrefBrowseBase() + BrowseMenu, INTEGER_DATA, (char*) 3);
}


/************************************************************************/
/* Proxy tab                                                            */
/************************************************************************/

/*----------------------------------------------------------------------
  SetupLabelDialog_Proxy init labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupLabelDialog_Proxy()
{
  // Setup notebook tab names :
  int page_id;
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  page_id = GetPagePosFromXMLID( _T("wxID_PAGE_PROXY") );
  if (page_id >= 0)
    p_notebook->SetPageText( page_id, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_PROXY_MENU)) );

  XRCCTRL(*this, "wxID_LABEL_PROXYHTTP", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_HTTP_PROXY)) );
  XRCCTRL(*this, "wxID_LABEL_PROXYDOM", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_PROXY_DOMAIN)) );
  XRCCTRL(*this, "wxID_LABEL_PROXYSPACE", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_PROXY_DOMAIN_INFO)) );

  XRCCTRL(*this, "wxID_RADIOBOX_NOTUSEPROXY", wxRadioButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DONT_PROXY_DOMAIN)) );
  XRCCTRL(*this, "wxID_RADIOBOX_USEPROXY", wxRadioButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_ONLY_PROXY_DOMAIN)) );
}

/*----------------------------------------------------------------------
  SetupDialog_Proxy send init value to dialog 
  params:
    + const Prop_Proxy & prop : the values to setup into the dialog
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_Proxy( const Prop_Proxy & prop )
{
  XRCCTRL(*this, "wxID_VALUE_PROXYHTTP", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.HttpProxy) );
  XRCCTRL(*this, "wxID_VALUE_PROXYDOM", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.ProxyDomain) );

  XRCCTRL(*this, "wxID_RADIOBOX_USEPROXY", wxRadioButton)->SetValue(prop.ProxyDomainIsOnlyProxy);
  XRCCTRL(*this, "wxID_RADIOBOX_NOTUSEPROXY", wxRadioButton)->SetValue(!prop.ProxyDomainIsOnlyProxy);
}

/*----------------------------------------------------------------------
  GetValueDialog_Proxy get dialog values
  params:
  returns:
    + Prop_Proxy prop : the dialog values
  ----------------------------------------------------------------------*/
Prop_Proxy PreferenceDlgWX::GetValueDialog_Proxy()
{
  wxString        value;
  Prop_Proxy      prop;
  memset( &prop, 0, sizeof(Prop_Proxy) );

  value = XRCCTRL(*this, "wxID_VALUE_PROXYHTTP", wxTextCtrl)->GetValue();
  strcpy( prop.HttpProxy, (const char*)value.mb_str(wxConvUTF8) );

  value = XRCCTRL(*this, "wxID_VALUE_PROXYDOM", wxTextCtrl)->GetValue();
  strcpy( prop.ProxyDomain, (const char*)value.mb_str(wxConvUTF8) );

  prop.ProxyDomainIsOnlyProxy = XRCCTRL(*this, "wxID_RADIOBOX_USEPROXY", wxRadioButton)->GetValue();

  return prop;
}


/************************************************************************/
/* Color tab                                                            */
/************************************************************************/

/*----------------------------------------------------------------------
  SetupLabelDialog_Color init labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupLabelDialog_Color()
{
  // Setup notebook tab names :
  int page_id;
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  page_id = GetPagePosFromXMLID( _T("wxID_PAGE_COLOR") );
  if (page_id >= 0)
    p_notebook->SetPageText( page_id, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_COLOR_MENU)) );

  XRCCTRL(*this, "wxID_LABEL_TEXTCOLOR", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DOC_FG_COLOR)) );
  XRCCTRL(*this, "wxID_LABEL_BACKCOLOR", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DOC_BG_COLOR)) );
  XRCCTRL(*this, "wxID_LABEL_SELCOLOR", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_FG_SEL_COLOR)) );
  XRCCTRL(*this, "wxID_LABEL_SELBACKCOLOR", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_BG_SEL_COLOR)) );

  XRCCTRL(*this, "wxID_LABEL_COLORGEOCHG", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_GEOMETRY_CHANGE)) );

  // setup combobox choices
  int    id_color   = 0;
  char * color_name = NULL;
  while (id_color < NumberOfColors())
    {
      color_name = ColorName(id_color);
      XRCCTRL(*this, "wxID_COMBO_SELBACKCOLOR", wxComboBox)->Append( TtaConvMessageToWX(color_name) );
      XRCCTRL(*this, "wxID_COMBO_SELCOLOR", wxComboBox)->Append( TtaConvMessageToWX(color_name) );
      XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox)->Append( TtaConvMessageToWX(color_name) );
      XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox)->Append( TtaConvMessageToWX(color_name) );
      id_color++;
    }

}

/*----------------------------------------------------------------------
  SetupDialog_Color send init value to dialog 
  params:
    + const Prop_Color & prop : the values to setup into the dialog
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_Color( const Prop_Color & prop )
{
  XRCCTRL(*this, "wxID_COMBO_SELBACKCOLOR",  wxComboBox)->Append( TtaConvMessageToWX(prop.BgSelColor) );
  XRCCTRL(*this, "wxID_COMBO_SELCOLOR", wxComboBox)->Append( TtaConvMessageToWX(prop.FgSelColor) );
  XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox)->Append( TtaConvMessageToWX(prop.BgColor) );
  XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox)->Append( TtaConvMessageToWX(prop.FgColor) );
  XRCCTRL(*this, "wxID_COMBO_SELBACKCOLOR", wxComboBox)->SetValue( TtaConvMessageToWX(prop.BgSelColor) );
  XRCCTRL(*this, "wxID_COMBO_SELCOLOR", wxComboBox)->SetValue( TtaConvMessageToWX(prop.FgSelColor) );
  XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox)->SetValue( TtaConvMessageToWX(prop.BgColor) );
  XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox)->SetValue( TtaConvMessageToWX(prop.FgColor) );

  // setup background colours
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  TtaGiveRGB ((char *)prop.BgSelColor, &red, &green, &blue);
  XRCCTRL(*this, "wxID_BUTTON_SELBACKCOLOR", wxBitmapButton)->SetBackgroundColour( wxColour(red, green, blue) );
  TtaGiveRGB ((char *)prop.FgSelColor, &red, &green, &blue);
  XRCCTRL(*this, "wxID_BUTTON_SELCOLOR", wxBitmapButton)->SetBackgroundColour( wxColour(red, green, blue) );
  TtaGiveRGB ((char *)prop.BgColor, &red, &green, &blue);
  XRCCTRL(*this, "wxID_BUTTON_BACKCOLOR", wxBitmapButton)->SetBackgroundColour( wxColour(red, green, blue) );
  TtaGiveRGB ((char *)prop.FgColor, &red, &green, &blue);
  XRCCTRL(*this, "wxID_BUTTON_TEXTCOLOR", wxBitmapButton)->SetBackgroundColour( wxColour(red, green, blue) );
}

/*----------------------------------------------------------------------
  GetValueDialog_Color get dialog values
  params:
  returns:
    + Prop_Color prop : the dialog values
  ----------------------------------------------------------------------*/
Prop_Color PreferenceDlgWX::GetValueDialog_Color()
{
  wxString        value;
  Prop_Color      prop;
  memset( &prop, 0, sizeof(Prop_Color) );

  value = XRCCTRL(*this, "wxID_COMBO_SELBACKCOLOR", wxComboBox)->GetValue();
  strcpy( prop.BgSelColor, (const char*)value.mb_str(wxConvUTF8) );

  value = XRCCTRL(*this, "wxID_COMBO_SELCOLOR", wxComboBox)->GetValue();
  strcpy( prop.FgSelColor, (const char*)value.mb_str(wxConvUTF8) );

  value = XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox)->GetValue();
  strcpy( prop.BgColor, (const char*)value.mb_str(wxConvUTF8) );

  value = XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox)->GetValue();
  strcpy( prop.FgColor, (const char*)value.mb_str(wxConvUTF8) );

  return prop;
}

/*----------------------------------------------------------------------
  OnColorPalette is called when the user click on the color palette button
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnColorPalette( wxCommandEvent& event )
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
void PreferenceDlgWX::OnColorChanged( wxCommandEvent& event )
{
  event.Skip();
}

/*----------------------------------------------------------------------
  OnColorTextChanged is called when a new color is selected
  then the background color of wxBitmapButton is updated to the right color
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnColorTextChanged( wxCommandEvent& event )
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

/************************************************************************/
/* Geometry tab                                                         */
/************************************************************************/

/*----------------------------------------------------------------------
  SetupLabelDialog_Geometry init labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupLabelDialog_Geometry()
{
  int page_id;

  // Setup notebook tab names :
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  page_id = GetPagePosFromXMLID( _T("wxID_PAGE_GEOMETRY") );
  if (page_id >= 0)
    p_notebook->SetPageText( page_id, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_GEOMETRY_MENU)) );

  XRCCTRL(*this, "wxID_LABEL_GEOMCHG", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_GEOMETRY_CHANGE)) );
  XRCCTRL(*this, "wxID_BUTTON_GEOMSAVE", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SAVE_GEOMETRY)) );
  XRCCTRL(*this, "wxID_BUTTON_GEOMRESTOR", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_RESTORE_GEOMETRY)) );
  
}

/*----------------------------------------------------------------------
  OnGeomSave is called when the user click on the save geom button
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnGeomSave( wxCommandEvent& event )
{
  ThotCallback (GetPrefGeometryBase() + GeometryMenu, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnGeomSave is called when the user click on the geom restor button
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnGeomRestor( wxCommandEvent& event )
{
  ThotCallback (GetPrefGeometryBase() + GeometryMenu, INTEGER_DATA, (char*) 2);
}


/************************************************************************/
/* Annotation tab                                                           */
/************************************************************************/
#ifdef ANNOTATIONS
/*----------------------------------------------------------------------
  SetupLabelDialog_Annot init labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupLabelDialog_Annot()
{
  // Setup notebook tab names :
  int page_id;
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  page_id = GetPagePosFromXMLID( _T("wxID_PAGE_ANNOT") );
  if (page_id >= 0)
    p_notebook->SetPageText( page_id, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_ANNOT_CONF_MENU)) );

  XRCCTRL(*this, "wxID_LABEL_ANNOT_USER", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_ANNOT_USER)) );
  XRCCTRL(*this, "wxID_LABEL_POST_SERVER", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_ANNOT_POST_SERVER)) );
  XRCCTRL(*this, "wxID_LABEL_SERVERS", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_ANNOT_SERVERS)) );

  XRCCTRL(*this, "wxID_CHECK_LOCAL_LOAD", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_ANNOT_LAUTOLOAD)) );
  XRCCTRL(*this, "wxID_CHECK_REMOTE_LOAD", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_ANNOT_RAUTOLOAD)) );
  XRCCTRL(*this, "wxID_CHECK_DISABLELOAD", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_ANNOT_RAUTOLOAD_RST)) );
}

/*----------------------------------------------------------------------
  SetupDialog_Annot send init value to dialog 
  params:
    + const Prop_Annot & prop : the values to setup into the dialog
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_Annot( const Prop_Annot & prop )
{
  XRCCTRL(*this, "wxID_ANNOT_USER", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.AnnotUser) );
  XRCCTRL(*this, "wxID_POST_SERVER", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.AnnotPostServer) );
  XRCCTRL(*this, "wxID_ANNOT_SERVERS", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.AnnotServers) );

  XRCCTRL(*this, "wxID_CHECK_LOCAL_LOAD", wxCheckBox)->SetValue( prop.AnnotLAutoLoad );
  XRCCTRL(*this, "wxID_CHECK_REMOTE_LOAD", wxCheckBox)->SetValue( prop.AnnotRAutoLoad );
  XRCCTRL(*this, "wxID_CHECK_DISABLELOAD", wxCheckBox)->SetValue( prop.AnnotRAutoLoadRst );
}

/*----------------------------------------------------------------------
  GetValueDialog_Annot get dialog values
  params:
  returns:
    + Prop_Annot prop : the dialog values
  ----------------------------------------------------------------------*/
Prop_Annot PreferenceDlgWX::GetValueDialog_Annot()
{
  wxString        value;
  Prop_Annot     prop;
  memset( &prop, 0, sizeof(Prop_Annot) );

  value = XRCCTRL(*this, "wxID_ANNOT_USER", wxTextCtrl)->GetValue();
  strcpy( prop.AnnotUser, (const char*)value.mb_str(wxConvUTF8) );
  value = XRCCTRL(*this, "wxID_POST_SERVER", wxTextCtrl)->GetValue();
  strcpy( prop.AnnotPostServer, (const char*)value.mb_str(wxConvUTF8) );
  value = XRCCTRL(*this, "wxID_ANNOT_SERVERS", wxTextCtrl)->GetValue();
  strcpy( prop.AnnotServers, (const char*)value.mb_str(wxConvUTF8) );

  prop.AnnotLAutoLoad = XRCCTRL(*this, "wxID_CHECK_LOCAL_LOAD", wxCheckBox)->GetValue();
  prop.AnnotRAutoLoad = XRCCTRL(*this, "wxID_CHECK_REMOTE_LOAD", wxCheckBox)->GetValue();
  prop.AnnotRAutoLoadRst = XRCCTRL(*this, "wxID_CHECK_DISABLELOAD", wxCheckBox)->GetValue();

  return prop;
}
#endif /*ANNOTATIONS */

/************************************************************************/
/* WebDAV tab                                                           */
/************************************************************************/
#ifdef DAV
/*----------------------------------------------------------------------
  SetupLabelDialog_DAV init labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupLabelDialog_DAV()
{
  // Setup notebook tab names :
  int page_id;
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  page_id = GetPagePosFromXMLID( _T("wxID_PAGE_DAV") );
  if (page_id >= 0)
   p_notebook->SetPageText( page_id, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_PREFERENCES)) );

  // update dialog WebDAV tab labels with given ones
  XRCCTRL(*this, "wxID_LABEL_USER_REF", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_USER_URL)) );
  XRCCTRL(*this, "wxID_LABEL_RESOURCES", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_USER_RESOURCES)) );

  XRCCTRL(*this, "wxID_RADIO_LOCK_DEPTH", wxRadioBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_DEPTH)) );
  XRCCTRL(*this, "wxID_RADIO_LOCK_DEPTH", wxRadioBox)->SetString(1, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_DEPTH_INFINITY)) );

  XRCCTRL(*this, "wxID_RADIO_SCOPE_DEPTH", wxRadioBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_LOCKSCOPE)) );
  XRCCTRL(*this, "wxID_RADIO_SCOPE_DEPTH", wxRadioBox)->SetString(0, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_LOCKSCOPE_EXCLUSIVE)) );
  XRCCTRL(*this, "wxID_RADIO_SCOPE_DEPTH", wxRadioBox)->SetString(1, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_LOCKSCOPE_SHARED)) );

  XRCCTRL(*this, "wxID_RADIO_TIMEOUT", wxRadioBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_TIMEOUT)) );
  XRCCTRL(*this, "wxID_RADIO_TIMEOUT", wxRadioBox)->SetString(0, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_TIMEOUT_INFINITE)) );
  XRCCTRL(*this, "wxID_RADIO_TIMEOUT", wxRadioBox)->SetString(1, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_TIMEOUT_OTHER)) );

  XRCCTRL(*this, "wxID_CHECK_GENERAL", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_AWARENESS_GENERAL)) );
  XRCCTRL(*this, "wxID_EXIT_AWARENESS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_AWARENESS_ONEXIT)) );
}

/*----------------------------------------------------------------------
  SetupDialog_DAV init labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_DAV( const Prop_DAV & prop)
{
  int          val;

  XRCCTRL(*this, "wxID_VALUE_DAV_USER", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.textUserReference) );
  XRCCTRL(*this, "wxID_VALUE_DAV_RESOURCES", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.textUserResources) );

  if (!strcmp (prop.radioDepth, "infinity"))
    val = 1;
  else
    val = 0;
  XRCCTRL(*this, "wxID_RADIO_LOCK_DEPTH", wxRadioBox)->SetSelection( val);
  if (!strcmp (prop.radioLockScope, "shared")) 
    val = 1;
  else
    val = 0;
  XRCCTRL(*this, "wxID_RADIO_SCOPE_DEPTH", wxRadioBox)->SetSelection( val );
  if (!strcmp (prop.radioTimeout, "Second"))
    val = 1;
  else
    val = 0;
  XRCCTRL(*this, "wxID_RADIO_TIMEOUT", wxRadioBox)->SetSelection( val );
  if (prop.numberTimeout < 300)
  XRCCTRL(*this, "wxID_TIMEOUT_VALUE", wxSpinCtrl)->SetValue( 300 );
  else
    XRCCTRL(*this, "wxID_TIMEOUT_VALUE", wxSpinCtrl)->SetValue( prop.numberTimeout );

  XRCCTRL(*this, "wxID_CHECK_GENERAL", wxCheckBox)->SetValue( prop.toggleAwareness1 );
  XRCCTRL(*this, "wxID_EXIT_AWARENESS", wxCheckBox)->SetValue( prop.toggleAwareness2 );
}

/*----------------------------------------------------------------------
  GetValueDialog_DAV init labels
  params:
  returns:
  ----------------------------------------------------------------------*/
Prop_DAV PreferenceDlgWX::GetValueDialog_DAV()
{
  wxString        value;
  Prop_DAV        prop;
  int             val;

  memset( &prop, 0, sizeof(Prop_General) );
  value = XRCCTRL(*this, "wxID_VALUE_DAV_USER", wxTextCtrl)->GetValue();
  strcpy( prop.textUserReference, (const char*)value.mb_str(wxConvUTF8) );
  value = XRCCTRL(*this, "wxID_VALUE_DAV_RESOURCES", wxTextCtrl)->GetValue();
  strcpy( prop.textUserResources, (const char*)value.mb_str(wxConvUTF8) );

  val = XRCCTRL(*this, "wxID_RADIO_LOCK_DEPTH", wxRadioBox)->GetSelection();
  if (val == 1)
    strcpy (prop.radioDepth, "infinity");
  else
    strcpy (prop.radioDepth, "0");

  val = XRCCTRL(*this, "wxID_RADIO_SCOPE_DEPTH", wxRadioBox)->GetSelection();
  if (val == 1) 
    strcpy (prop.radioLockScope, "shared");
  else
    strcpy (prop.radioLockScope, "exclusive");
  val = XRCCTRL(*this, "wxID_RADIO_TIMEOUT", wxRadioBox)->GetSelection();
  if (val == 1)
    strcpy (prop.radioTimeout, "Second-");
  else
    strcpy (prop.radioTimeout, "Infinite");

  prop.numberTimeout = XRCCTRL(*this, "wxID_TIMEOUT_VALUE",  wxSpinCtrl)->GetValue();

  prop.toggleAwareness1 = XRCCTRL(*this, "wxID_CHECK_GENERAL", wxCheckBox)->GetValue();
  prop.toggleAwareness2 = XRCCTRL(*this, "wxID_EXIT_AWARENESS", wxCheckBox)->GetValue();

  return prop;
}
#endif /* DAV */


/*----------------------------------------------------------------------
  OnOk called when the user validates his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnOk( wxCommandEvent& event )
{
  m_OnApplyLock = TRUE;
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->Disable();
  
  Prop_General prop_gen = GetValueDialog_General();
  SetProp_General( &prop_gen );
  ThotCallback (GetPrefGeneralBase() + GeneralMenu, INTEGER_DATA, (char*) 1);

  Prop_Browse prop_brw = GetValueDialog_Browse();
  SetProp_Browse( &prop_brw );
  ThotCallback (GetPrefBrowseBase() + BrowseMenu, INTEGER_DATA, (char*) 1);

  Prop_Publish prop_pub = GetValueDialog_Publish();
  SetProp_Publish( &prop_pub );
  ThotCallback (GetPrefPublishBase() + PublishMenu, INTEGER_DATA, (char*) 1);

  Prop_Cache prop_cache = GetValueDialog_Cache();
  SetProp_Cache( &prop_cache );
  ThotCallback (GetPrefCacheBase() + CacheMenu, INTEGER_DATA, (char*) 1);

  Prop_Proxy prop_px = GetValueDialog_Proxy();
  SetProp_Proxy( &prop_px );
  ThotCallback (GetPrefProxyBase() + ProxyMenu, INTEGER_DATA, (char*) 1);

  Prop_Color prop_color = GetValueDialog_Color();
  SetProp_Color( &prop_color );
  ThotCallback (GetPrefColorBase() + ColorMenu, INTEGER_DATA, (char*) 1);

#ifdef ANNOTATIONS
  Prop_Annot prop_annot = GetValueDialog_Annot();
  SetProp_Annot( &prop_annot );
  ThotCallback (GetPrefAnnotBase() + AnnotMenu, INTEGER_DATA, (char*) 1);
#endif /* ANNOTATIONS */

#ifdef DAV
  Prop_DAV prop_dav = GetValueDialog_DAV();
  SetProp_DAV( &prop_dav );
  ThotCallback (GetPrefDAVBase() + DAVMenu, INTEGER_DATA, (char*) 1);
#endif /* DAV */

  ThotCallback (m_Ref, INTEGER_DATA, (char*) 1);

  XRCCTRL(*this, "wxID_CANCEL", wxButton)->Enable();
  m_OnApplyLock = FALSE;

  // then just close the dialog
  OnCancel(event);
}

/*----------------------------------------------------------------------
  OnDefault reset to default value the current configuration page
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnDefault( wxCommandEvent& event )
{
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  wxWindow * p_page = (wxWindow *) (p_notebook->GetSelection() != -1) ? p_notebook->GetPage(p_notebook->GetSelection()) : NULL;

  if (!p_page)
    return;

  /* Update only the current page */
  if ( p_page->GetId() == wxXmlResource::GetXRCID(_T("wxID_PAGE_GENERAL")) )
    {
      ThotCallback (GetPrefGeneralBase() + GeneralMenu, INTEGER_DATA, (char*) 2);
      SetupDialog_General( GetProp_General() );
    }
  else if ( p_page->GetId() == wxXmlResource::GetXRCID(_T("wxID_PAGE_BROWSE")) )
    {
      ThotCallback (GetPrefBrowseBase() + BrowseMenu, INTEGER_DATA, (char*) 2);
      SetupDialog_Browse( GetProp_Browse() );
    }
  else if ( p_page->GetId() == wxXmlResource::GetXRCID(_T("wxID_PAGE_PUBLISH")) )
    {
      ThotCallback (GetPrefPublishBase() + PublishMenu, INTEGER_DATA, (char*) 2);
      SetupDialog_Publish( GetProp_Publish() );
    }
  else if ( p_page->GetId() == wxXmlResource::GetXRCID(_T("wxID_PAGE_CACHE")) )
    {
      ThotCallback (GetPrefCacheBase() + CacheMenu, INTEGER_DATA, (char*) 2);
      SetupDialog_Cache( GetProp_Cache() );
    }
  else if ( p_page->GetId() == wxXmlResource::GetXRCID(_T("wxID_PAGE_PROXY")) )
    {
      ThotCallback (GetPrefProxyBase() + ProxyMenu, INTEGER_DATA, (char*) 2);
      SetupDialog_Proxy( GetProp_Proxy() );
    }
  else if ( p_page->GetId() == wxXmlResource::GetXRCID(_T("wxID_PAGE_COLOR")) )
    {
      ThotCallback (GetPrefColorBase() + ColorMenu, INTEGER_DATA, (char*) 2);
      SetupDialog_Color( GetProp_Color() );
    }
  else if ( p_page->GetId() == wxXmlResource::GetXRCID(_T("wxID_PAGE_GEOMETRY")) )
    {
    }
#ifdef ANNOTATIONS
  else if ( p_page->GetId() == wxXmlResource::GetXRCID(_T("wxID_PAGE_ANNOT")) )
    {
      ThotCallback (GetPrefAnnotBase() + AnnotMenu, INTEGER_DATA, (char*) 2);
      SetupDialog_Annot( GetProp_Annot() );
    }
#endif /* ANNOTATIONS */
#ifdef DAV
  else if ( p_page->GetId() == wxXmlResource::GetXRCID(_T("wxID_PAGE_DAV")) )
    {
      ThotCallback (GetPrefDAVBase() + DAVMenu, INTEGER_DATA, (char*) 2);
      SetupDialog_DAV( GetProp_DAV() );
    }
#endif /* DAV */

  ThotCallback (m_Ref, INTEGER_DATA, (char*) 2);
}

/*----------------------------------------------------------------------
  OnCancel called when the user clicks on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnCancel( wxCommandEvent& event )
{
  if (m_OnApplyLock)
    return;

  ThotCallback (GetPrefGeneralBase() + GeneralMenu, INTEGER_DATA, (char*) 0);
  ThotCallback (GetPrefBrowseBase() + BrowseMenu, INTEGER_DATA, (char*) 0);
  ThotCallback (GetPrefPublishBase() + PublishMenu, INTEGER_DATA, (char*) 0);
  ThotCallback (GetPrefCacheBase() + CacheMenu, INTEGER_DATA, (char*) 0);
  ThotCallback (GetPrefProxyBase() + ProxyMenu, INTEGER_DATA, (char*) 0);
  ThotCallback (GetPrefColorBase() + ColorMenu, INTEGER_DATA, (char*) 0);
  ThotCallback (GetPrefGeometryBase() + GeometryMenu, INTEGER_DATA, (char*) 0);
#ifdef ANNOTATIONS
  ThotCallback (GetPrefAnnotBase() + AnnotMenu, INTEGER_DATA, (char*) 0);
#endif /* ANNOTATIONS */
#ifdef DAV
  ThotCallback (GetPrefDAVBase() + DAVMenu, INTEGER_DATA, (char*) 0);
#endif /* DAV */
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 0);
}

/*----------------------------------------------------------------------
  OnClose
  called when the window manager closes the dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnClose(wxCloseEvent& event)
{
  if (!m_OnApplyLock)
    event.Skip(); // let wxWidgets close the dialog
}

#endif /* _WX */
