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

  // Cache tab callbacks
  EVT_BUTTON(     XRCID("wxID_BUTTON_EMPTYCACHE"), PreferenceDlgWX::OnEmptyCache )

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

  m_UrlList = url_list;

  // setup dialog title
  SetTitle( TtaConvMessageToWX("TODO (ex: Preferences)") );

  // send labels to dialog (labels depends on language)
  SetupLabelDialog_General();
  SetupLabelDialog_Browse();
  SetupLabelDialog_Publish();
  SetupLabelDialog_Cache();

  XRCCTRL(*this, "wxID_OK",      wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage (AMAYA, AM_APPLY_BUTTON)));
  XRCCTRL(*this, "wxID_CANCEL",  wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CANCEL)) );
  XRCCTRL(*this, "wxID_DEFAULT", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage (AMAYA, AM_DEFAULT_BUTTON)));

  // load current values and send it to the dialog
  SetupDialog_General( GetProp_General() );
  SetupDialog_Browse( GetProp_Browse() );
  SetupDialog_Publish( GetProp_Publish() );
  SetupDialog_Cache( GetProp_Cache() );

  // give focus to ...
  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->SetFocus();

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

/*----------------------------------------------------------------------
  Return the @ of corresponding widget from the given string id.
  ----------------------------------------------------------------------*/
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
  wxLogDebug( _T("PreferenceDlgWX::SetupLabelDialog_General") );

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

  // setup range of zoom
  XRCCTRL(*this, "wxID_CHARZOOM_VALUE", wxSlider)->SetRange( 10, 1000 );
  // setup menu font size range
  XRCCTRL(*this, "wxID_CHARMENUSIZE_VALUE", wxSlider)->SetRange( 8, 20 );

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
  wxString        value;
  Prop_General    prop;
  memset( &prop, 0, sizeof(Prop_General) );

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
  wxLogDebug( _T("PreferenceDlgWX::SetupLabelDialog_Browse") );

  // Setup notebook tab names :
  int page_id;
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  page_id = GetPagePosFromXMLID( _T("wxID_PAGE_BROWSE") );
  if (page_id >= 0)
    p_notebook->SetPageText( page_id, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_BROWSE_MENU)) );

  // update dialog General tab labels with given ones
  XRCCTRL(*this, "wxID_LABEL_SCREEN", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_SCREEN_TYPE)) );
  XRCCTRL(*this, "wxID_CHECK_LOADIMG", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_LOAD_IMAGES)) );
  XRCCTRL(*this, "wxID_CHECK_LOADOBJ", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_LOAD_OBJECTS)) );
  XRCCTRL(*this, "wxID_CHECK_SHOWBACKGROUND", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SHOW_BG_IMAGES)) );
  XRCCTRL(*this, "wxID_CHECK_APPLYCSS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_LOAD_CSS)) );
  XRCCTRL(*this, "wxID_CHECK_LINKDBCLICK", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ENABLE_DOUBLECLICK)) );
  XRCCTRL(*this, "wxID_CHECK_ENABLEFTP", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ENABLE_FTP)) );
}

/*----------------------------------------------------------------------
  SetupDialog_Browse send init value to dialog 
  params:
    + const PropTab_Browse & prop : the values to setup into the dialog
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_Browse( const Prop_Browse & prop )
{
  wxLogDebug( _T("PreferenceDlgWX::SetupDialog_Browse") );

  XRCCTRL(*this, "wxID_CHECK_LOADIMG", wxCheckBox)->SetValue( prop.LoadImages );
  XRCCTRL(*this, "wxID_CHECK_LOADOBJ", wxCheckBox)->SetValue( prop.LoadObjects );
  XRCCTRL(*this, "wxID_CHECK_SHOWBACKGROUND", wxCheckBox)->SetValue( prop.BgImages );
  XRCCTRL(*this, "wxID_CHECK_APPLYCSS", wxCheckBox)->SetValue( prop.LoadCss );
  XRCCTRL(*this, "wxID_CHECK_LINKDBCLICK", wxCheckBox)->SetValue( prop.DoubleClick );
  XRCCTRL(*this, "wxID_CHECK_ENABLEFTP", wxCheckBox)->SetValue( prop.EnableFTP );
  
  XRCCTRL(*this, "wxID_CHOICE_SCREEN", wxChoice)->SetStringSelection( TtaConvMessageToWX(prop.ScreenType) );
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

  wxLogDebug( _T("PreferenceDlgWX::GetValueDialog_Browse") );

  prop.LoadImages  = XRCCTRL(*this, "wxID_CHECK_LOADIMG", wxCheckBox)->GetValue();
  prop.LoadObjects = XRCCTRL(*this, "wxID_CHECK_LOADOBJ", wxCheckBox)->GetValue();
  prop.BgImages    = XRCCTRL(*this, "wxID_CHECK_SHOWBACKGROUND", wxCheckBox)->GetValue();
  prop.LoadCss     = XRCCTRL(*this, "wxID_CHECK_APPLYCSS", wxCheckBox)->GetValue();
  prop.DoubleClick = XRCCTRL(*this, "wxID_CHECK_LINKDBCLICK", wxCheckBox)->GetValue();
  prop.EnableFTP   = XRCCTRL(*this, "wxID_CHECK_ENABLEFTP", wxCheckBox)->GetValue();
  
  value = XRCCTRL(*this, "wxID_CHOICE_SCREEN", wxChoice)->GetStringSelection();
  strcpy( prop.ScreenType, (const char*)value.mb_str(wxConvUTF8) );

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
  wxLogDebug( _T("PreferenceDlgWX::SetupLabelDialog_Publish") );

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
}

/*----------------------------------------------------------------------
  SetupDialog_Publish send init value to dialog 
  params:
    + const PropTab_Publish & prop : the values to setup into the dialog
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_Publish( const Prop_Publish & prop )
{
  wxLogDebug( _T("PreferenceDlgWX::SetupDialog_Publish") );

  XRCCTRL(*this, "wxID_CHOICE_CHARSET", wxChoice)->SetStringSelection( TtaConvMessageToWX(prop.CharsetType) );

  XRCCTRL(*this, "wxID_CHECK_XHTML", wxCheckBox)->SetValue( prop.UseXHTMLMimeType );
  XRCCTRL(*this, "wxID_CHECK_ETAGS", wxCheckBox)->SetValue( prop.LostUpdateCheck );
  XRCCTRL(*this, "wxID_CHECK_PUTGET", wxCheckBox)->SetValue( prop.VerifyPublish );
  
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

  wxLogDebug( _T("PreferenceDlgWX::GetValueDialog_Publish") );

  value = XRCCTRL(*this, "wxID_CHOICE_CHARSET", wxChoice)->GetStringSelection();
  strcpy( prop.CharsetType, (const char*)value.mb_str(wxConvUTF8) );

  prop.UseXHTMLMimeType = XRCCTRL(*this, "wxID_CHECK_XHTML", wxCheckBox)->GetValue();
  prop.LostUpdateCheck  = XRCCTRL(*this, "wxID_CHECK_ETAGS", wxCheckBox)->GetValue();
  prop.VerifyPublish    = XRCCTRL(*this, "wxID_CHECK_PUTGET", wxCheckBox)->GetValue();

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
  wxLogDebug( _T("PreferenceDlgWX::SetupLabelDialog_Cache") );

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

  XRCCTRL(*this, "wxID_CACHESIZE_VALUE",  wxSlider)->SetRange( 1, 100 );
  XRCCTRL(*this, "wxID_MAXSIZEITEM_VALUE",  wxSlider)->SetRange( 1, 5 );
}

/*----------------------------------------------------------------------
  SetupDialog_Cache send init value to dialog 
  params:
    + const Prop_Cache & prop : the values to setup into the dialog
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_Cache( const Prop_Cache & prop )
{
  wxLogDebug( _T("PreferenceDlgWX::SetupDialog_Cache") );

  XRCCTRL(*this, "wxID_CHECK_ENABLECACHE", wxCheckBox)->SetValue( prop.EnableCache );
  XRCCTRL(*this, "wxID_CHECK_PROTECTEDDOC", wxCheckBox)->SetValue( prop.CacheProtectedDocs );
  XRCCTRL(*this, "wxID_CHECK_DISCO", wxCheckBox)->SetValue( prop.CacheDisconnectMode );
  XRCCTRL(*this, "wxID_CHECK_EXPIGNORE", wxCheckBox)->SetValue( prop.CacheExpireIgnore );

  XRCCTRL(*this, "wxID_VALUE_CACHEDIR", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.CacheDirectory) );

  XRCCTRL(*this, "wxID_CACHESIZE_VALUE",  wxSlider)->SetValue( prop.CacheSize );
  XRCCTRL(*this, "wxID_MAXSIZEITEM_VALUE",  wxSlider)->SetValue( prop.MaxCacheFile );
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

  prop.EnableCache         = XRCCTRL(*this, "wxID_CHECK_ENABLECACHE", wxCheckBox)->GetValue();
  prop.CacheProtectedDocs  = XRCCTRL(*this, "wxID_CHECK_PROTECTEDDOC", wxCheckBox)->GetValue();
  prop.CacheDisconnectMode = XRCCTRL(*this, "wxID_CHECK_DISCO", wxCheckBox)->GetValue();
  prop.CacheExpireIgnore   = XRCCTRL(*this, "wxID_CHECK_EXPIGNORE", wxCheckBox)->GetValue();

  value = XRCCTRL(*this, "wxID_VALUE_CACHEDIR", wxTextCtrl)->GetValue();
  strcpy( prop.CacheDirectory, (const char*)value.mb_str(wxConvUTF8) );

  prop.CacheSize    = XRCCTRL(*this, "wxID_CACHESIZE_VALUE",  wxSlider)->GetValue();
  prop.MaxCacheFile = XRCCTRL(*this, "wxID_MAXSIZEITEM_VALUE",  wxSlider)->GetValue();

  return prop;
}

/*----------------------------------------------------------------------
  OnEmptyCache is called when the user click on emptycache button (Cache tab)
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnEmptyCache( wxCommandEvent& event )
{
  wxLogDebug( _T("PreferenceDlgWX::OnEmptyCache") );
  ThotCallback (GetPrefCacheBase() + CacheMenu, INTEGER_DATA, (char*) 3);
}

/*----------------------------------------------------------------------
  OnOk called when the user validates his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnOk( wxCommandEvent& event )
{
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

  ThotCallback (m_Ref, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
  OnDefault reset to default value the current configuration page
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnDefault( wxCommandEvent& event )
{
  wxNotebook * p_notebook = XRCCTRL(*this, "wxID_NOTEBOOK", wxNotebook);
  wxWindow *    p_page    = (wxWindow *) (p_notebook->GetSelection() != -1) ? p_notebook->GetPage(p_notebook->GetSelection()) : NULL;

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

  ThotCallback (m_Ref, INTEGER_DATA, (char*) 2);
}

/*----------------------------------------------------------------------
  OnCancel called when the user clicks on cancel button
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnCancel( wxCommandEvent& event )
{
  ThotCallback (GetPrefGeneralBase() + GeneralMenu, INTEGER_DATA, (char*) 0);
  ThotCallback (GetPrefBrowseBase() + BrowseMenu, INTEGER_DATA, (char*) 0);
  ThotCallback (GetPrefPublishBase() + PublishMenu, INTEGER_DATA, (char*) 0);
  ThotCallback (GetPrefCacheBase() + CacheMenu, INTEGER_DATA, (char*) 0);
  ThotCallback (m_Ref, INTEGER_DATA, (char*) 0);
}

#endif /* _WX */
