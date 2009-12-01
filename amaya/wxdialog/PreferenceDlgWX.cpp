/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
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
#include "wx/listbook.h"
#include "wx/colordlg.h"
#include "wx/listbox.h"
#include "wx/dynarray.h"
#include "AmayaApp.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "appdialogue_wx.h"
#include "message_wx.h"
#include "MENUconf.h"
#include "MENUconf_f.h"
#include "registry_wx.h"
#include "PreferenceDlgWX.h"
#include "templates.h"
#include "Elemlist.h"
#include "templates_f.h"
#include "HTMLhistory_f.h"
#ifdef DAV
#include "davlibUI_f.h"
#endif /* DAV */
bool PreferenceDlgWX::m_OnApplyLock = FALSE;
extern bool WarnRestart;
static int MyRef = 0;


//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(PreferenceDlgWX, AmayaDialog)

  EVT_LISTBOOK_PAGE_CHANGED( XRCID("wxID_NOTEBOOK"), PreferenceDlgWX::OnPageChanged )
  EVT_BUTTON( XRCID("wxID_OK"),           PreferenceDlgWX::OnOk )
  EVT_BUTTON( XRCID("wxID_DEFAULT"),      PreferenceDlgWX::OnDefault )
  EVT_BUTTON( XRCID("wxID_CANCEL"),       PreferenceDlgWX::OnCancel )
  // Clear url list callback
  EVT_BUTTON( XRCID("wxID_BUTTON_CLEARURL"), PreferenceDlgWX::OnClearUrlList )
  // Cache tab callbacks
  EVT_BUTTON( XRCID("wxID_BUTTON_EMPTYCACHE"),   PreferenceDlgWX::OnEmptyCache )
  // Color tab callbacks
  EVT_BUTTON( XRCID("wxID_BUTTON_TEXTCOLOR"),    PreferenceDlgWX::OnColorPalette )
  EVT_BUTTON( XRCID("wxID_BUTTON_BACKCOLOR"),    PreferenceDlgWX::OnColorPalette )
  EVT_BUTTON( XRCID("wxID_BUTTON_SELCOLOR"),     PreferenceDlgWX::OnColorPalette )
  EVT_BUTTON( XRCID("wxID_BUTTON_SELBACKCOLOR"), PreferenceDlgWX::OnColorPalette )
  EVT_COMBOBOX( XRCID("wxID_COMBO_SELBACKCOLOR"),PreferenceDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_SELCOLOR"),    PreferenceDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_BACKCOLOR"),   PreferenceDlgWX::OnColorChanged )
  EVT_COMBOBOX( XRCID("wxID_COMBO_TEXTCOLOR"),   PreferenceDlgWX::OnColorChanged )
  EVT_TEXT( XRCID("wxID_COMBO_SELBACKCOLOR"),    PreferenceDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_SELCOLOR"),        PreferenceDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_BACKCOLOR"),       PreferenceDlgWX::OnColorTextChanged )
  EVT_TEXT( XRCID("wxID_COMBO_TEXTCOLOR"),       PreferenceDlgWX::OnColorTextChanged )
  // Geometry tab callbacks
  EVT_CHECKBOX( XRCID("wxID_CHECK_SAVEGEO"),     PreferenceDlgWX::OnGeomToggle )
  EVT_BUTTON( XRCID("wxID_BUTTON_GEOMSAVE"),     PreferenceDlgWX::OnGeomSave )
  EVT_BUTTON( XRCID("wxID_BUTTON_GEOMRESTOR"),   PreferenceDlgWX::OnGeomRestor )
  EVT_CLOSE( PreferenceDlgWX::OnClose )
  // Templates tab callbacks
#ifdef TEMPLATES
  EVT_BUTTON( XRCID("wxID_BUTTON_DELETE_TEMPLATE"),    PreferenceDlgWX::OnTemplateDel )
  EVT_BUTTON( XRCID("wxID_BUTTON_MOVEUP_TEMPLATE"),    PreferenceDlgWX::OnTemplateMoveUp )
  EVT_BUTTON( XRCID("wxID_BUTTON_MOVEDOWN_TEMPLATE"),  PreferenceDlgWX::OnTemplateMoveDown )
  EVT_BUTTON( XRCID("wxID_BUTTON_CHOOSE_TEMPLATE"),    PreferenceDlgWX::OnTemplateChoose )
  EVT_BUTTON( XRCID("wxID_BUTTON_ADD_TEMPLATE"),       PreferenceDlgWX::OnTemplateAdd )
  //EVT_TEXT_ENTER( XRCID("wxID_TEXT_NEW_TEMPLATE"),     PreferenceDlgWX::OnTemplateAdd )
  EVT_UPDATE_UI( XRCID("wxID_BUTTON_DELETE_TEMPLATE"), PreferenceDlgWX::OnUpdateTemplateDel )
  EVT_UPDATE_UI( XRCID("wxID_BUTTON_MOVEUP_TEMPLATE"), PreferenceDlgWX::OnUpdateTemplateMoveUp )
  EVT_UPDATE_UI( XRCID("wxID_BUTTON_MOVEDOWN_TEMPLATE"), PreferenceDlgWX::OnUpdateTemplateMoveDown )
  EVT_UPDATE_UI( XRCID("wxID_BUTTON_ADD_TEMPLATE"),     PreferenceDlgWX::OnUpdateTemplateAdd )
  EVT_TEXT(XRCID("wxID_LIST_TEMPLATE_REPOSITORIES"), PreferenceDlgWX::OnTemplateKey)
  EVT_LISTBOX(XRCID("wxID_LIST_TEMPLATE_REPOSITORIES"), PreferenceDlgWX::OnTemplateSelected)
#endif /* TEMPLATES */
#ifdef DAV
  EVT_BUTTON( XRCID("wxID_BUTTON_DELETE_DAV"),    PreferenceDlgWX::OnDAVDel )
  EVT_TEXT_ENTER( XRCID("wxID_VALUE_DAV_RESOURCES"),    PreferenceDlgWX::OnDAVUpdate )
#endif /* DAV */
  // Passwords tab callbacks
  EVT_LISTBOX(XRCID("wxID_LIST_PASSWORDS"),         PreferenceDlgWX::OnPasswordSelected)
  EVT_BUTTON( XRCID("wxID_BUTTON_DELETE_PASSWORD"), PreferenceDlgWX::OnPasswordDeleted )
  EVT_BUTTON( XRCID("wxID_BUTTON_EMPTY_PASSWORDS"), PreferenceDlgWX::OnEmptyPasswords )
  // RDFa tab callbacks
  EVT_BUTTON( XRCID("wxID_BUTTON_DELETE_NS"),       PreferenceDlgWX::OnNSDelete )
  EVT_BUTTON( XRCID("wxID_BUTTON_ADD_NS"),          PreferenceDlgWX::OnNSAdd )
  EVT_LISTBOX(XRCID("wxID_LIST_NS"),                PreferenceDlgWX::OnNSSelected)

END_EVENT_TABLE()


  /*----------------------------------------------------------------------
    PreferenceDlgWX create the dialog used to change amaya preferences
    params:
    + parent : parent window
    + title : dialog title
    + ...
    returns:
    ----------------------------------------------------------------------*/
  PreferenceDlgWX::PreferenceDlgWX (int ref, wxWindow* parent,
                                    const wxArrayString & url_list,
				    const wxArrayString & rdfa_list) :
    AmayaDialog( parent, ref ),
    m_IsInitialized(false) // this flag is used to know when events can be proceed
{
  wxXmlResource::Get()->LoadDialog(this, parent, wxT("PreferenceDlgWX"));
  m_UrlList = url_list;
  m_RDFaNSList = rdfa_list;
  MyRef = ref;
  // no warn requested by default
  WarnRestart = false;
  m_book = new wxListBoxBook(this, XRCID("wxID_NOTEBOOK"));
  GetSizer()->Prepend(m_book, 1, wxEXPAND);
  
  m_book->InsertPage(0, wxXmlResource::Get()->LoadPanel(m_book, wxT("wxID_PAGE_GENERAL")),
      TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_GENERAL_MENU)), true);
  m_book->InsertPage(1, wxXmlResource::Get()->LoadPanel(m_book, wxT("wxID_PAGE_BROWSE")),
      TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_BROWSE_MENU)), false);
  m_book->InsertPage(2, wxXmlResource::Get()->LoadPanel(m_book, wxT("wxID_PAGE_PUBLISH")),
      TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_PUBLISH_MENU)), false);
  m_book->InsertPage(3, wxXmlResource::Get()->LoadPanel(m_book, wxT("wxID_PAGE_CACHE")),
      TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_CACHE_MENU)), false);
  m_book->InsertPage(4, wxXmlResource::Get()->LoadPanel(m_book, wxT("wxID_PAGE_PASSWORDS")),
      TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_PASSWORDS)), false);
  m_book->InsertPage(5, wxXmlResource::Get()->LoadPanel(m_book, wxT("wxID_PAGE_PROXY")),
      TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_PROXY_MENU)), false);
  m_book->InsertPage(6, wxXmlResource::Get()->LoadPanel(m_book, wxT("wxID_PAGE_COLOR")),
      TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_COLORS)), false);
  m_book->InsertPage(7, wxXmlResource::Get()->LoadPanel(m_book, wxT("wxID_PAGE_GEOMETRY")),
      TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_GEOMETRY_MENU)), false);
  m_book->InsertPage(8, wxXmlResource::Get()->LoadPanel(m_book, wxT("wxID_PAGE_ANNOT")),
      TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_ANNOT_CONF_MENU)), false);
  m_book->InsertPage(9, wxXmlResource::Get()->LoadPanel(m_book, wxT("wxID_PAGE_DAV")),
      TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DAV_PREFERENCES)), false);
  m_book->InsertPage(10, wxXmlResource::Get()->LoadPanel(m_book, wxT("wxID_PAGE_TEMPLATES")),
      TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_TEMPLATES)), false);
  m_book->InsertPage(11, wxXmlResource::Get()->LoadPanel(m_book, wxT("wxID_PAGE_EMAILS")),
      TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS)), false);
  m_book->InsertPage(12, wxXmlResource::Get()->LoadPanel(m_book, wxT("wxID_PAGE_RDFa")),
                     TtaConvMessageToWX("RDFa"), false);
  
#ifndef DAV
  // invalid WebDAV Page
  int page_id = GetPagePosFromXMLID( _T("wxID_PAGE_DAV") );
  if (page_id)
    m_book->DeletePage(page_id );
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
#ifdef TEMPLATES
  SetupLabelDialog_Templates();
#else /* TEMPLATES */
  // invalid templates Page
  int templates_page_id = GetPagePosFromXMLID( _T("wxID_PAGE_TEMPLATES") );
  if (templates_page_id)
    m_book->DeletePage(templates_page_id );  
#endif /* TEMPLATES */
  SetupLabelDialog_Emails();
  SetupLabelDialog_Passwords();
  SetupLabelDialog_RDFa();

  XRCCTRL(*this, "wxID_OK", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_APPLY_BUTTON)));
  XRCCTRL(*this, "wxID_CANCEL", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CANCEL)));
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
#ifdef TEMPLATES
  SetupDialog_Templates();
#endif /* TEMPLATES */
  SetupDialog_Emails( GetProp_Emails() );
  SetupDialog_Passwords( GetProp_Passwords() );
  SetupDialog_RDFa( GetProp_RDFa() );

  // on windows, the color selector dialog must be complete.
  colour_data.SetChooseFull(true);
  Fit();
  SetSize(720, -1);

  CentreOnScreen();
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
  TtaDestroyDialogue (MyRef);
}

/*----------------------------------------------------------------------
  Return the @ of corresponding widget from the given string id.
  ----------------------------------------------------------------------*/
int PreferenceDlgWX::GetPagePosFromXMLID( const wxString & xml_id )
{
  wxPanel *    p_page     = (wxPanel *)FindWindow(wxXmlResource::GetXRCID(xml_id));
  int          page_id    = 0;
  bool         found      = false;

  while (!found && page_id < (int) m_book->GetPageCount() )
    {
      if ( p_page == m_book->GetPage(page_id))
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
void PreferenceDlgWX::OnPageChanged( wxListbookEvent& event )
{
  wxPanel *p_new_page = (wxPanel *)((event.GetSelection()>=0 && m_book)?m_book->GetPage(event.GetSelection()):NULL);

  if(!m_IsInitialized || !p_new_page || !XRCCTRL(*this,"wxID_OK",wxButton) ||
     !XRCCTRL(*this,"wxID_DEFAULT",wxButton))
    {
      event.Skip();
      return;
    }
  int page_id = p_new_page->GetId();
  if (page_id == wxXmlResource::GetXRCID(_T("wxID_PAGE_GEOMETRY")))
    {
      // the new page is Geometry => hide the bottom buttons (ok, default)
      XRCCTRL(*this, "wxID_OK",      wxButton)->Hide();
      XRCCTRL(*this, "wxID_DEFAULT", wxButton)->Hide();
    }
  else
    {
      XRCCTRL(*this, "wxID_OK",      wxButton)->Show();
      XRCCTRL(*this, "wxID_DEFAULT", wxButton)->Show();
      if (page_id == wxXmlResource::GetXRCID(_T("wxID_PAGE_GENERAL")))
	XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->SetFocus();
      else if (page_id == wxXmlResource::GetXRCID(_T("wxID_PAGE_BROWSE")))
	XRCCTRL(*this, "wxID_VALUE_LANNEGLISTLG", wxTextCtrl)->SetFocus();
      else if (page_id == wxXmlResource::GetXRCID(_T("wxID_PAGE_PUBLISH")))
	XRCCTRL(*this, "wxID_VALUE_DEFAULTNAME", wxTextCtrl)->SetFocus();
      else if (page_id == wxXmlResource::GetXRCID(_T("wxID_PAGE_CACHE")))
	XRCCTRL(*this, "wxID_VALUE_CACHEDIR", wxTextCtrl)->SetFocus();
       else if (page_id == wxXmlResource::GetXRCID(_T("wxID_PAGE_PROXY")))
	XRCCTRL(*this, "wxID_VALUE_PROXYHTTP", wxTextCtrl)->SetFocus();
      else if (page_id == wxXmlResource::GetXRCID(_T("wxID_PAGE_COLOR")))
	XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox)->SetFocus();
      else if (page_id == wxXmlResource::GetXRCID(_T("wxID_PAGE_ANNOT")))
	XRCCTRL(*this, "wxID_ANNOT_USER", wxTextCtrl)->SetFocus();
      else if (page_id == wxXmlResource::GetXRCID(_T("wxID_PAGE_DAV")))
	XRCCTRL(*this, "wxID_VALUE_DAV_USER", wxTextCtrl)->SetFocus();
      else if (page_id == wxXmlResource::GetXRCID(_T("wxID_PAGE_TEMPLATES")))
	XRCCTRL(*this, "wxID_TEXT_NEW_TEMPLATE", wxTextCtrl)->SetFocus();
      else if (page_id == wxXmlResource::GetXRCID(_T("wxID_PAGE_EMAILS")))
	XRCCTRL(*this, "wxID_VALUE_EMAIL_SERVER_ADDRESS", wxTextCtrl)->SetFocus();
      else if (page_id == wxXmlResource::GetXRCID(_T("wxID_PAGE_RDFa")))
	XRCCTRL(*this, "wxID_COMBOBOX_NEW_NS", wxComboBox)->SetFocus();
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
  // update dialog General tab labels with given ones
  XRCCTRL(*this, "wxID_LABEL_HOMEPAGE", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_HOME_PAGE)) );
  XRCCTRL(*this, "wxID_LABEL_CHARZOOM", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_ZOOM)) );
  XRCCTRL(*this, "wxID_LABEL_LG", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_DIALOGUE_LANGUAGE)) );

  XRCCTRL(*this, "wxID_XML_EDIT", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_XML_EDIT)) );
  XRCCTRL(*this, "wxID_CHECK_CCLINE", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_PASTE_LINE_BY_LINE)) );
  XRCCTRL(*this, "wxID_CHECK_BACKUP", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_AUTO_SAVE)) );
  XRCCTRL(*this, "wxID_CHECK_INSERT_NBSP", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_INSERT_NBSP)) );
  XRCCTRL(*this, "wxID_CHECK_NOALIASING", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_NOALIASING)) );
  XRCCTRL(*this, "wxID_CHECK_DATE", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DATE)) );
  XRCCTRL(*this, "wxID_CHECK_SHOWTARGETS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SHOW_TARGETS)) );
  XRCCTRL(*this, "wxID_CHECK_SHOWSHORTCUTS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_SHOWSHORTCUTS)));
  XRCCTRL(*this, "wxID_CHECK_WARNCTAB", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_WARNCTAB)) );
  XRCCTRL(*this, "wxID_CHECK_TIPOFTHEDAY", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SHOW_TIPS_STARTUP)) );

  XRCCTRL(*this, "wxID_RADIO_QUICKAXX", wxRadioBox)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ACCESSKEY)) );
  XRCCTRL(*this, "wxID_RADIO_QUICKAXX", wxRadioBox)->SetString(2,TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_NONE)) );

  XRCCTRL(*this, "wxID_LABEL_TOOLPANEL_ALIGN", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_TOOLPANEL_ALIGN)) );
  XRCCTRL(*this, "wxID_LABEL_PROFILE", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_PROFILE)) );
  XRCCTRL(*this, "wxID_CHOICE_LG", wxChoice)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_PROFILE_CHANGE) ));
  XRCCTRL(*this, "wxID_CHOICE_PROFILE", wxChoice)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_PROFILE_CHANGE) ));
  XRCCTRL(*this, "wxID_CHOICE_TOOLPANEL_ALIGN", wxChoice)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_PROFILE_CHANGE) ));
 
  /* tooltip of color buttons */
  XRCCTRL(*this, "wxID_BUTTON_TEXTCOLOR", wxBitmapButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_FG_SEL_COLOR) ));
  XRCCTRL(*this, "wxID_BUTTON_BACKCOLOR", wxBitmapButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_FG_SEL_COLOR) ));
  XRCCTRL(*this, "wxID_BUTTON_SELCOLOR", wxBitmapButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_FG_SEL_COLOR) ));
  XRCCTRL(*this, "wxID_BUTTON_SELBACKCOLOR", wxBitmapButton)->SetToolTip( TtaConvMessageToWX( TtaGetMessage(AMAYA, AM_FG_SEL_COLOR) ));

  // setup range of zoom
  XRCCTRL(*this, "wxID_CHARZOOM_VALUE", wxSpinCtrl)->SetRange( 10, 1000 );

  // fill the combobox with url list
  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->Append(m_UrlList);
#if defined(_WINDOWS)
  // select the string
  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->SetSelection(0, -1);
#else /* _WINDOWS */
  // set te cursor to the end
  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->SetInsertionPointEnd();
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  SetupDialog_General send init value to dialog 
  params:
  + const PropTab_General & prop : the values to setup into the dialog
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_General( const Prop_General & prop )
{
  wxString        value;
  char           *prof;

  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->SetValue( TtaConvMessageToWX(prop.HomePage) );
  XRCCTRL(*this, "wxID_CHARZOOM_VALUE", wxSpinCtrl)->SetValue( prop.Zoom );

  XRCCTRL(*this, "wxID_XML_EDIT", wxCheckBox)->SetValue( prop.XMLEdit );
  XRCCTRL(*this, "wxID_CHECK_CCLINE", wxCheckBox)->SetValue( prop.PasteLineByLine );
  XRCCTRL(*this, "wxID_CHECK_BACKUP", wxCheckBox)->SetValue( prop.S_AutoSave );
  XRCCTRL(*this, "wxID_CHECK_INSERT_NBSP", wxCheckBox)->SetValue( prop.S_NBSP );
  XRCCTRL(*this, "wxID_CHECK_NOALIASING", wxCheckBox)->SetValue( prop.S_NoAliasing );
  XRCCTRL(*this, "wxID_CHECK_DATE", wxCheckBox)->SetValue( prop.S_DATE );
  XRCCTRL(*this, "wxID_CHECK_SHOWTARGETS", wxCheckBox)->SetValue( prop.S_Targets );
  XRCCTRL(*this, "wxID_CHECK_SHOWSHORTCUTS", wxCheckBox)->SetValue( prop.S_Shortcuts );
  XRCCTRL(*this, "wxID_CHECK_WARNCTAB", wxCheckBox)->SetValue( prop.WarnCTab );
  XRCCTRL(*this, "wxID_CHECK_TIPOFTHEDAY", wxCheckBox)->SetValue( prop.ShowTipsStartup );

  XRCCTRL(*this, "wxID_RADIO_QUICKAXX", wxRadioBox)->SetSelection( prop.AccesskeyMod );
  if (!strcmp (prop.DialogueLang, "de"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (German)");
  else if (!strcmp (prop.DialogueLang, "en"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (English)");
  else if (!strcmp (prop.DialogueLang, "es"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (Spanish)");
  else if (!strcmp (prop.DialogueLang, "fi"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (Finnish)");
  else if (!strcmp (prop.DialogueLang, "fr"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (French)");
  else if (!strcmp (prop.DialogueLang, "hu"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (Hungarian)");
  else if (!strcmp (prop.DialogueLang, "it"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (Italian)");
  else if (!strcmp (prop.DialogueLang, "ja"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (Japanese)");
  else if (!strcmp (prop.DialogueLang, "ka"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (Georgian)");
  else if (!strcmp (prop.DialogueLang, "nl"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (Dutch)");
  else if (!strcmp (prop.DialogueLang, "no"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (Norwegian)");
  else if (!strcmp (prop.DialogueLang, "pt"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (Portuguese)");
  else if (!strcmp (prop.DialogueLang, "ru"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (Russian)");
  else if (!strcmp (prop.DialogueLang, "tr"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (Turkish)");
  else if (!strcmp (prop.DialogueLang, "uk"))
    value = TtaConvMessageToWX(prop.DialogueLang)+_T(" (Ukrainian)");
  else if (!strcmp (prop.DialogueLang, "cn"))
    value = _T("zh-")+TtaConvMessageToWX(prop.DialogueLang)+_T(" (Simplified Chinese)");
  else if (!strcmp (prop.DialogueLang, "tw"))
    value = _T("zh-")+TtaConvMessageToWX(prop.DialogueLang)+_T(" (Traditional Chinese)");
  else
    value = TtaConvMessageToWX(prop.DialogueLang);

  prof = TtaGetEnvString ("CURRENT_PROFILE");
  if (prof)
    XRCCTRL(*this, "wxID_CHOICE_PROFILE", wxChoice)->SetStringSelection(TtaConvMessageToWX(prof));
  else
    XRCCTRL(*this, "wxID_CHOICE_PROFILE", wxChoice)->SetStringSelection(TtaConvMessageToWX("Advanced"));

  XRCCTRL(*this, "wxID_CHOICE_LG", wxChoice)->SetStringSelection( value );
#if defined(_WINDOWS)
  // select the string
  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->SetSelection(0, -1);
#else /* _WINDOWS */
  // set te cursor to the end
  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->SetInsertionPointEnd();
#endif /* _WINDOWS */
  XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->SetFocus();
  
  XRCCTRL(*this, "wxID_CHOICE_TOOLPANEL_ALIGN", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_FORMATLEFT)) );
  XRCCTRL(*this, "wxID_CHOICE_TOOLPANEL_ALIGN", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_FORMATRIGHT)) );
  XRCCTRL(*this, "wxID_CHOICE_TOOLPANEL_ALIGN", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_AUI_FREE_PANEL_POS)) );
  XRCCTRL(*this, "wxID_CHOICE_TOOLPANEL_ALIGN", wxChoice)->SetSelection(prop.ToolPanelLayout);
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
  char           *prof;
  char            buffer[MAX_LENGTH];

  memset( &prop, 0, sizeof(Prop_General) );
  value = XRCCTRL(*this, "wxID_COMBOBOX_HOMEPAGE", wxComboBox)->GetValue();
  strncpy( prop.HomePage, (const char*)value.mb_str(wxConvUTF8), MAX_LENGTH - 1);
  prop.HomePage[MAX_LENGTH - 1] = EOS;
  prop.Zoom = XRCCTRL(*this, "wxID_CHARZOOM_VALUE",     wxSpinCtrl)->GetValue();
  prop.XMLEdit = XRCCTRL(*this, "wxID_XML_EDIT", wxCheckBox)->GetValue();
  prop.PasteLineByLine = XRCCTRL(*this, "wxID_CHECK_CCLINE", wxCheckBox)->GetValue();
  prop.S_AutoSave = XRCCTRL(*this, "wxID_CHECK_BACKUP", wxCheckBox)->GetValue();
  prop.S_NBSP = XRCCTRL(*this, "wxID_CHECK_INSERT_NBSP", wxCheckBox)->GetValue();
  prop.S_NoAliasing = XRCCTRL(*this, "wxID_CHECK_NOALIASING", wxCheckBox)->GetValue();
  prop.S_DATE = XRCCTRL(*this, "wxID_CHECK_DATE", wxCheckBox)->GetValue();
  prop.S_Targets = XRCCTRL(*this, "wxID_CHECK_SHOWTARGETS", wxCheckBox)->GetValue();
  prop.S_Shortcuts = XRCCTRL(*this, "wxID_CHECK_SHOWSHORTCUTS", wxCheckBox)->GetValue();
  prop.WarnCTab = XRCCTRL(*this, "wxID_CHECK_WARNCTAB", wxCheckBox)->GetValue();
  prop.ShowTipsStartup = XRCCTRL(*this, "wxID_CHECK_TIPOFTHEDAY", wxCheckBox)->GetValue();
  prop.AccesskeyMod = XRCCTRL(*this, "wxID_RADIO_QUICKAXX", wxRadioBox)->GetSelection();

  value = XRCCTRL(*this, "wxID_CHOICE_LG", wxChoice)->GetStringSelection();
  strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
  if (!strncmp (buffer, "zh", 2))
    strncpy ( prop.DialogueLang, &buffer[3], 2 );
  else
    strncpy ( prop.DialogueLang, buffer, 2 );
  prop.DialogueLang[2] = EOS;
  prop.ToolPanelLayout = XRCCTRL(*this, "wxID_CHOICE_TOOLPANEL_ALIGN", wxChoice)->GetSelection();

  // detect if the Amaya profile changes
  buffer[0] = EOS;
  value = XRCCTRL(*this, "wxID_CHOICE_PROFILE", wxChoice)->GetStringSelection();
  if (!value.IsEmpty())
    {
      strcpy (buffer, (const char*)value.mb_str(wxConvUTF8) );
      prof = TtaGetEnvString ("CURRENT_PROFILE");
      if (prof == NULL || strcmp (prof, buffer))
        {
          // change the Amaya profile
          WarnRestart = true;
          TtaSetEnvString ("CURRENT_PROFILE", buffer, TRUE);
          // Avoid to save previous "AUI_DECORATION"
          SavePANEL_PREFERENCES = 0;
        }
    }
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
  // update dialog General tab labels with given ones
  XRCCTRL(*this, "wxID_RADIO_OPENLOC", wxRadioBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_OPENLOC)) );
  XRCCTRL(*this, "wxID_RADIO_OPENLOC", wxRadioBox)->SetString(0, TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_REPLACECURRENT)));
  XRCCTRL(*this, "wxID_RADIO_OPENLOC", wxRadioBox)->SetString(1, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_INNEWTAB)));
  XRCCTRL(*this, "wxID_RADIO_OPENLOC", wxRadioBox)->SetString(2, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_INNEWWINDOW)));
  XRCCTRL(*this, "wxID_LABEL_SCREEN", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_SCREEN_TYPE)) );
  XRCCTRL(*this, "wxID_CHECK_LOADIMG", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_LOAD_IMAGES)) );
  XRCCTRL(*this, "wxID_CHECK_LOADOBJ", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_LOAD_OBJECTS)) );
  XRCCTRL(*this, "wxID_CHECK_SHOWBACKGROUND", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SHOW_BG_IMAGES)) );
  XRCCTRL(*this, "wxID_CHECK_APPLYCSS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_LOAD_CSS)) );
  XRCCTRL(*this, "wxID_CHECK_LINKDBCLICK", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_ENABLE_DOUBLECLICK)) );
  XRCCTRL(*this, "wxID_LABEL_LANNEGLISTLG", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_LANG_NEGOTIATION)) );
  XRCCTRL(*this, "wxID_CHECK_WARNIDS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_WARNIDS)) );
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
  XRCCTRL(*this, "wxID_CHECK_WARNIDS", wxCheckBox)->SetValue( prop.WarnIDs );
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
  prop.WarnIDs = XRCCTRL(*this, "wxID_CHECK_WARNIDS", wxCheckBox)->GetValue();
  
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
  XRCCTRL(*this, "wxID_LABEL_CHARSET", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DEFAULT_CHARSET)) );
  XRCCTRL(*this, "wxID_LABEL_DEFAULTNAME", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DEFAULT_NAME)) );
  XRCCTRL(*this, "wxID_LABEL_REDIRECT", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SAFE_PUT_REDIRECT)) );

  XRCCTRL(*this, "wxID_CHECK_XHTML", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_USE_XHTML_MIMETYPE)) );
  XRCCTRL(*this, "wxID_CHECK_ETAGS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_USE_ETAGS)) );
  XRCCTRL(*this, "wxID_CHECK_PUTGET", wxCheckBox)->Hide();
  //XRCCTRL(*this, "wxID_CHECK_PUTGET", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_VERIFY_PUT)) );
  XRCCTRL(*this, "wxID_CHECK_CRLF", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_EXPORT_CRLF)) );
  XRCCTRL(*this, "wxID_CHECK_MATHPI", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_MATHPI)) );
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
  XRCCTRL(*this, "wxID_CHECK_MATHPI", wxCheckBox)->SetValue( prop.GenerateMathPI );
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
  prop.GenerateMathPI   = XRCCTRL(*this, "wxID_CHECK_MATHPI", wxCheckBox)->GetValue();
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
  XRCCTRL(*this, "wxID_LABEL_TEXTCOLOR", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DOC_FG_COLOR)) );
  XRCCTRL(*this, "wxID_LABEL_BACKCOLOR", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_DOC_BG_COLOR)) );
  XRCCTRL(*this, "wxID_LABEL_SELCOLOR", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_FG_SEL_COLOR)) );
  XRCCTRL(*this, "wxID_LABEL_SELBACKCOLOR", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_BG_SEL_COLOR)) );

  XRCCTRL(*this, "wxID_LABEL_COLORGEOCHG", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_GEOMETRY_CHANGE)) );
  XRCCTRL(*this, "wxID_COLOR_SOURCE", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_COLOR_SOURCE)) );

  // setup combobox choices
  int    id_color   = 0;
  const char * color_name = NULL;
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
  XRCCTRL(*this, "wxID_COLOR_SOURCE", wxCheckBox)->SetValue( prop.ColorSource );

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

  prop.ColorSource = XRCCTRL(*this, "wxID_COLOR_SOURCE", wxCheckBox)->GetValue();

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
  else if (id == selcolor_id)
    value = XRCCTRL(*this, "wxID_COMBO_SELCOLOR", wxComboBox)->GetValue();
  else if (id == selbackcolor_id)
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
      
      char color_string[20];
      sprintf( color_string, "#%02x%02x%02x", col.Red(), col.Green(), col.Blue() );
      wxComboBox *     p_combo  = NULL;
      if (id == textcolor_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_TEXTCOLOR", wxComboBox);
      else if (id == backcolor_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_BACKCOLOR", wxComboBox);
      else if (id == selcolor_id)
        p_combo = XRCCTRL(*this, "wxID_COMBO_SELCOLOR", wxComboBox);
      else if (id == selbackcolor_id)
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
  ThotBool val;

  XRCCTRL(*this, "wxID_CHECK_SAVEGEO", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SAVE_GEOMETRY_ON_EXIT)) );
  XRCCTRL(*this, "wxID_LABEL_GEOMCHG", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_GEOMETRY_CHANGE)) );
  XRCCTRL(*this, "wxID_BUTTON_GEOMSAVE", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_SAVE_GEOMETRY)) );
  XRCCTRL(*this, "wxID_BUTTON_GEOMRESTOR", wxButton)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_RESTORE_GEOMETRY)) );
  val = GetProp_Geometry();
  XRCCTRL(*this, "wxID_CHECK_SAVEGEO", wxCheckBox)->SetValue( val );
}

/*----------------------------------------------------------------------
  OnGeomToggle is called when the user click on the save geom button
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnGeomToggle( wxCommandEvent& event )
{
  ThotCallback (GetPrefGeometryBase() + mToggleGeom, INTEGER_DATA, (char*) 0);
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
  ThotBool val = TRUE;

  XRCCTRL(*this, "wxID_CHECK_SAVEGEO", wxCheckBox)->SetValue( val );
  SetProp_Geometry (val);
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
}

/*----------------------------------------------------------------------
  SetupDialog_DAV init labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_DAV( const Prop_DAV & prop)
{
  int            val;
  wxComboBox    *box = XRCCTRL(*this, "wxID_VALUE_DAV_RESOURCES", wxComboBox);
  Prop_DAV_Path *path = DAV_Paths;
  if (path)
    {
      box->SetValue(TtaConvMessageToWX(path->Path));
      path = path->NextPath;
      while (path)
        {
          box->Append(TtaConvMessageToWX(path->Path));
          path = path->NextPath;
        }
    }
  XRCCTRL(*this, "wxID_VALUE_DAV_USER", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.textUserReference) );
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
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnDAVDel(wxCommandEvent& event)
{
  wxString       value;
  wxComboBox    *box = XRCCTRL(*this, "wxID_VALUE_DAV_RESOURCES", wxComboBox);
  Prop_DAV_Path *path;

  value = box->GetValue();
  if (value.Len() > 0)
    {
      RemovePathInDAVList ((const char*)value.mb_str(wxConvUTF8));
      // update the combobox
      path = DAV_Paths;
      if (path)
        {
          box->SetValue(TtaConvMessageToWX(path->Path));
          path = path->NextPath;
          while (path)
            {
              box->Append(TtaConvMessageToWX(path->Path));
              path = path->NextPath;
            }
        }
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnDAVUpdate( wxCommandEvent& event )
{
  wxString       value;

  value = XRCCTRL(*this, "wxID_VALUE_DAV_RESOURCES", wxComboBox)->GetValue();
  if (value.Len() > 0)
    {
      AddPathInDAVList ((const char*)value.mb_str(wxConvUTF8));
      XRCCTRL(*this, "wxID_VALUE_DAV_RESOURCES", wxComboBox)->Append(value);
    }
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
  return prop;
}
#endif /* DAV */

/************************************************************************/
/* Templates tab                                                        */
/************************************************************************/
#ifdef TEMPLATES

/*----------------------------------------------------------------------
  SetupLabelDialog_Templates inits template labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupLabelDialog_Templates()
{
  XRCCTRL(*this, "wxID_BUTTON_CHOOSE_TEMPLATE", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_SEL)));
  XRCCTRL(*this, "wxID_BUTTON_ADD_TEMPLATE", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_APPLY_BUTTON)));
  wxStaticBoxSizer *sz = (wxStaticBoxSizer*)XRCCTRL(*this, "wxID_PAGE_TEMPLATES", wxPanel)->GetSizer()->GetItem((size_t)0)->GetSizer();
  sz->GetStaticBox()->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_TEMPLATES)));
}

/*----------------------------------------------------------------------
  SetupDialog_Templates inits template dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_Templates()
{
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_TEMPLATE_REPOSITORIES", wxListBox);
  Prop_Templates_Path* path = TemplateRepositoryPaths;
  while (path)
    {
      box->Append(TtaConvMessageToWX(path->Path));
      path = path->NextPath;
    }
}

/*----------------------------------------------------------------------
  UpdateTemplateList updates the current list of templates
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::UpdateTemplateList()
{
  wxString             value;
  Prop_Templates_Path *element = NULL;
  wxListBox           *box;
  int                  i, max;

  FreeTemplateRepositoryList();
  box = XRCCTRL(*this, "wxID_LIST_TEMPLATE_REPOSITORIES", wxListBox);
  max = (int)box->GetCount();
  for (i = 0; i < max; i++)
    {
    element = (Prop_Templates_Path*) AllocTemplateRepositoryListElement( (const char*) box->GetString(i).mb_str(*wxConvCurrent), element);
    if (i == 0)
       TemplateRepositoryPaths = element;
    }
  SaveTemplateRepositoryList ();
}

/*----------------------------------------------------------------------
  GetValueDialog_Templates gets the show template indicator
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::GetValueDialog_Templates()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnTemplateChoose(wxCommandEvent& event)
{
  static const wxString home = TtaGetHomeDir();
  wxFileDialog  *p_dlg;
  
  wxString path = XRCCTRL(*this, "wxID_TEXT_NEW_TEMPLATE", wxTextCtrl)->GetValue();
  if (!path.IsEmpty())
    path.Replace(wxT("~"), home);
  
  p_dlg = new wxFileDialog(this,
                           TtaConvMessageToWX( TtaGetMessage (AMAYA, AM_OPEN_URL) ),
                           _T(""), _T(""), _T("Templates (*.xtd)|*.xtd"),
                           wxOPEN | wxCHANGE_DIR);
  if (p_dlg->ShowModal() == wxID_OK)
    {
      path = p_dlg->GetPath();
      XRCCTRL(*this, "wxID_TEXT_NEW_TEMPLATE", wxTextCtrl)->SetValue(path);
    }
  p_dlg->Destroy();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnTemplateAdd(wxCommandEvent& event)
{
  wxString   path = XRCCTRL(*this, "wxID_TEXT_NEW_TEMPLATE", wxTextCtrl)->GetValue();
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_TEMPLATE_REPOSITORIES", wxListBox);
  int        i, max;
  
  if (path.IsEmpty())
    return;
  max = (int)box->GetCount();
  for (i = 0; i < max; i++)
    {
      box->GetString(i).mb_str(*wxConvCurrent);
      if (!strcmp (path.mb_str(wxConvUTF8), box->GetString(i).mb_str(wxConvUTF8)))
        {
          // this entry already exists
          box->SetSelection(max-1);
          return;
        }
    }
  box->Append(path);
  box->SetSelection(max-1);

  // Update the list of templates
  UpdateTemplateList();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnUpdateTemplateAdd(wxUpdateUIEvent& event)
{
  event.Enable(!XRCCTRL(*this, "wxID_TEXT_NEW_TEMPLATE", wxTextCtrl)->GetValue().IsEmpty());
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnTemplateDel(wxCommandEvent& event)
{
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_TEMPLATE_REPOSITORIES", wxListBox);
  int sel = box->GetSelection();
  if (sel != wxNOT_FOUND)
  {
    box->Delete(sel);
    // clear the input area
    XRCCTRL(*this, "wxID_TEXT_NEW_TEMPLATE", wxTextCtrl)->SetValue( TtaConvMessageToWX(""));
    // Update the list of templates
    UpdateTemplateList();
  }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnUpdateTemplateDel(wxUpdateUIEvent& event)
{
  event.Enable(XRCCTRL(*this, "wxID_LIST_TEMPLATE_REPOSITORIES",
                       wxListBox)->GetSelection() != wxNOT_FOUND);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnTemplateMoveUp(wxCommandEvent& event)
{
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_TEMPLATE_REPOSITORIES", wxListBox);
  int sel = box->GetSelection();
  if (sel != wxNOT_FOUND && sel > 0)
  {
    wxString str = box->GetString(sel);
    box->Delete(sel);
    box->Insert(str, sel-1);
    box->SetSelection(sel-1);
    // Update the list of templates
    UpdateTemplateList();
  }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnUpdateTemplateMoveUp(wxUpdateUIEvent& event)
{
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_TEMPLATE_REPOSITORIES", wxListBox);
  event.Enable (box->GetSelection() != wxNOT_FOUND && box->GetSelection() !=0 );
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnTemplateMoveDown(wxCommandEvent& event)
{
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_TEMPLATE_REPOSITORIES", wxListBox);
  int sel = box->GetSelection();
  if (sel != wxNOT_FOUND && sel<(int)box->GetCount()-2)
  {
    wxString str = box->GetString(sel);
    box->Delete(sel);
    box->Insert(str, sel+1);
    box->SetSelection(sel+1);
    // Update the list of templates
    UpdateTemplateList();
  }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnUpdateTemplateMoveDown(wxUpdateUIEvent& event)
{
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_TEMPLATE_REPOSITORIES", wxListBox);
  event.Enable (box->GetSelection() != wxNOT_FOUND && box->GetSelection() != (int)box->GetCount()-1);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnTemplateKey(wxCommandEvent& event)
{
  // useful to accept up/down keys
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnTemplateSelected(wxCommandEvent& event)
{
  if (event.IsSelection())
    XRCCTRL(*this, "wxID_TEXT_NEW_TEMPLATE", wxTextCtrl)->SetValue(event.GetString());
}
#endif /* Templates */


/************************************************************************/
/* Emails tab                                                           */
/************************************************************************/

/*----------------------------------------------------------------------
  SetupLabelDialog_Emails init labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupLabelDialog_Emails()
{
  XRCCTRL(*this, "wxID_LABEL_EMAIL", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_PROPERTIES)));
  XRCCTRL(*this, "wxID_LABEL_EMAIL_SERVER_ADDRESS", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SERVER_ADDRESS)));
  XRCCTRL(*this, "wxID_LABEL_EMAIL_SERVER_PORT", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_EMAILS_SERVER_PORT)));
  XRCCTRL(*this, "wxID_LABEL_EMAIL_DEFAULT_SERVER_PORT", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_SERVER_DEFPORT)));
  XRCCTRL(*this, "wxID_LABEL_EMAIL_FROM_ADDRESS", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_EMAILS_FROM_)));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_Emails( const Prop_Emails & prop )
{
  XRCCTRL(*this, "wxID_VALUE_EMAIL_SERVER_ADDRESS", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.serverAddress) );
  XRCCTRL(*this, "wxID_SPIN_EMAIL_SERVER_PORT", wxSpinCtrl)->SetValue( prop.serverPort );  
  XRCCTRL(*this, "wxID_VALUE_EMAIL_FROM_ADDRESS", wxTextCtrl)->SetValue( TtaConvMessageToWX(prop.fromAddress) );
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
Prop_Emails PreferenceDlgWX::GetValueDialog_Emails()
{
  Prop_Emails prop;
  wxString    value;
  
  memset( &prop, 0, sizeof(Prop_Emails) );
  value = XRCCTRL(*this, "wxID_VALUE_EMAIL_SERVER_ADDRESS", wxTextCtrl)->GetValue();
  strcpy( prop.serverAddress, (const char*)value.mb_str(wxConvUTF8) );
  prop.serverPort = XRCCTRL(*this, "wxID_SPIN_EMAIL_SERVER_PORT", wxSpinCtrl)->GetValue();
  value = XRCCTRL(*this, "wxID_VALUE_EMAIL_FROM_ADDRESS", wxTextCtrl)->GetValue();
  strcpy( prop.fromAddress, (const char*)value.mb_str(wxConvUTF8) );

  return prop;
}


/************************************************************************/
/* Passwords tab                                                        */
/************************************************************************/

/*----------------------------------------------------------------------
  SetupLabelDialog_Passwords inits Passwords labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupLabelDialog_Passwords()
{
  // Setup notebook tab names :
  int page_id;
  page_id = GetPagePosFromXMLID( _T("wxID_PAGE_PASSWORDS") );
  if (page_id >= 0)
    m_book->SetPageText( page_id, TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_PASSWORDS)));

  XRCCTRL(*this, "wxID_CHECK_DEF_PASSWORDS", wxCheckBox)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_PASSWORDS_SAVE_OPTION)));
  XRCCTRL(*this, "wxID_BUTTON_EMPTY_PASSWORDS", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_PASSWORDS_EMPTY)));
  XRCCTRL(*this, "wxID_BUTTON_DELETE_PASSWORD", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_DEL)));

  wxStaticBoxSizer *sz = (wxStaticBoxSizer*)XRCCTRL(*this, "wxID_PAGE_PASSWORDS", wxPanel)->GetSizer()->GetItem((size_t)0)->GetSizer();
  sz->GetStaticBox()->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_PASSWORDS_LIST)));
}

/*----------------------------------------------------------------------
  SetupDialog_Passwordss inits password dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_Passwords( const Prop_Passwords & prop)
{
  Prop_Passwords_Site* site;

  XRCCTRL(*this, "wxID_CHECK_DEF_PASSWORDS", wxCheckBox)->SetValue( prop.S_Passwords );
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_PASSWORDS", wxListBox);
  box->Clear();
  /* Build the list of server/realm pair from the internal table */
  LoadPasswordsSiteList();
  site = GetFirtsPasswordsSite();
  while (site)
    {
      box->Append(TtaConvMessageToWX(site->Site));
      site = site->NextSite;
    }
}

/*----------------------------------------------------------------------
  OnEmptyPasswords is called when the user click on flush password button
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnEmptyPasswords( wxCommandEvent& event )
{
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_PASSWORDS", wxListBox);
  box->Clear();
  ThotCallback (GetPrefPasswordsBase() + PasswordsMenu, INTEGER_DATA, (char*) 3);
}

/*----------------------------------------------------------------------
  GetValueDialog_Passwords gets the save passwords indicator
  params:
  returns:
  ----------------------------------------------------------------------*/
Prop_Passwords PreferenceDlgWX::GetValueDialog_Passwords()
{
  Prop_Passwords       prop = GetProp_Passwords();

  prop.S_Passwords = XRCCTRL(*this, "wxID_CHECK_DEF_PASSWORDS", wxCheckBox)->GetValue();
  return prop;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnPasswordSelected(wxCommandEvent& event)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnPasswordDeleted(wxCommandEvent& event)
{
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_PASSWORDS", wxListBox);
  int sel = box->GetSelection();
  if (sel != wxNOT_FOUND)
  {
    char buffer[MAX_LENGTH];
    strcpy (buffer, (const char*)box->GetString(sel).mb_str(*wxConvCurrent) );
    box->Delete(sel);
    // Update the password table
    sel++;
    UpdatePasswordsSiteList(sel, &buffer[0]);
  }
}

/************************************************************************/
/* RDFa tab                                                             */
/************************************************************************/

/*----------------------------------------------------------------------
  SetupLabelDialog_RDFa inits RDFa labels
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupLabelDialog_RDFa()
{
  XRCCTRL(*this, "wxID_BUTTON_ADD_NS", wxBitmapButton)->SetToolTip(TtaConvMessageToWX("Add"));
  wxStaticBoxSizer *sz = (wxStaticBoxSizer*)XRCCTRL(*this, "wxID_PAGE_RDFa", wxPanel)->GetSizer()->GetItem((size_t)0)->GetSizer();
  sz->GetStaticBox()->SetLabel(TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_RDFa)));
  XRCCTRL(*this, "wxID_LABEL_NEW_NS", wxStaticText)->SetLabel( TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_RDFa_NEW_LABEL)) );

  // fill the combobox with ns list
  XRCCTRL(*this, "wxID_COMBOBOX_NEW_NS", wxComboBox)->Append(m_RDFaNSList);
#if defined(_WINDOWS)
  // select the string
  XRCCTRL(*this, "wxID_COMBOBOX_NEW_NS", wxComboBox)->SetSelection(0, -1);
#else /* _WINDOWS */
  // set te cursor to the end
  XRCCTRL(*this, "wxID_COMBOBOX_NEW_NS", wxComboBox)->SetInsertionPointEnd();
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  SetupDialog_RDFa inits RDFa dialog
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::SetupDialog_RDFa( const Prop_RDFa & prop)
{
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_NS", wxListBox);
  box->Clear()
;
  Prop_RDFa_Path* path = prop.FirstPath;
  while (path)
    {
      box->Append(TtaConvMessageToWX(path->Path));
      path = path->NextPath;
    }
#if defined(_WINDOWS)
  // select the string
  XRCCTRL(*this, "wxID_COMBOBOX_NEW_NS", wxComboBox)->SetSelection(0, -1);
#else /* _WINDOWS */
  // set te cursor to the end
  XRCCTRL(*this, "wxID_COMBOBOX_NEW_NS", wxComboBox)->SetInsertionPointEnd();
#endif /* _WINDOWS */
  //XRCCTRL(*this, "wxID_COMBOBOX_NEW_NS", wxComboBox)->SetFocus();
}

/*----------------------------------------------------------------------
  GetValueDialog_NS gets the show NS indicator
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::GetValueDialog_RDFa()
{
  wxString        value;

  value = XRCCTRL(*this, "wxID_COMBOBOX_NEW_NS", wxComboBox)->GetValue();
}

/*----------------------------------------------------------------------
  UpdateNSList updates the current list of NS
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::UpdateRDFaNsList()
{
  wxString             value;
  Prop_RDFa            prop = GetProp_RDFa();
  Prop_RDFa_Path      *element = NULL;
  wxListBox           *box;
  int                  i, max;

  FreeRDFaNSList(&(prop.FirstPath));
  box = XRCCTRL(*this, "wxID_LIST_NS", wxListBox);
  max = (int)box->GetCount();
  for (i = 0; i < max; i++)
    {
      element = (Prop_RDFa_Path*) AllocRDFaNsListElement( (const char*) box->GetString(i).mb_str(*wxConvCurrent), element);
    if (i == 0)
       prop.FirstPath = element;
    }
  SetRDFaNsList ((const Prop_RDFa_Path**)&(prop.FirstPath));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnNSAdd(wxCommandEvent& event)
{
  wxString   path = XRCCTRL(*this, "wxID_COMBOBOX_NEW_NS", wxComboBox)->GetValue();
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_NS", wxListBox);
  int        i, max;
  
  if (path.IsEmpty())
    return;
  max = (int)box->GetCount();
  for (i = 0; i < max; i++)
    {
      box->GetString(i).mb_str(*wxConvCurrent);
      if (!strcmp (path.mb_str(wxConvUTF8), box->GetString(i).mb_str(wxConvUTF8)))
        {
          // this entry already exists
          box->SetSelection(max-1);
          return;
        }
    }
  box->Append(path);
  box->SetSelection(max-1);
  XRCCTRL(*this, "wxID_COMBOBOX_NEW_NS", wxComboBox)->SetValue( TtaConvMessageToWX("") );
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnNSDelete(wxCommandEvent& event)
{
  wxListBox *box = XRCCTRL(*this, "wxID_LIST_NS", wxListBox);
  int sel = box->GetSelection();
  if (sel != wxNOT_FOUND)
  {
    box->Delete(sel);
  }
  XRCCTRL(*this, "wxID_COMBOBOX_NEW_NS", wxComboBox)->SetValue( TtaConvMessageToWX("") );
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnNSSelected(wxCommandEvent& event)
{
  if (event.IsSelection())
    XRCCTRL(*this, "wxID_COMBOBOX_NEW_NS", wxComboBox)->SetValue(event.GetString());
}


/************************************************************************/
/* General events                                                       */
/************************************************************************/
/*----------------------------------------------------------------------
  OnOk called when the user validates his selection
  params:
  returns:
  ----------------------------------------------------------------------*/
void PreferenceDlgWX::OnOk( wxCommandEvent& event )
{
  if (m_OnApplyLock)
    return;

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

#ifdef TEMPLATES
  // not necessary to update the list
  OnTemplateAdd (event);
  GetValueDialog_Templates();
#endif /* TEMPLATES */

  Prop_Emails prop_emails = GetValueDialog_Emails();
  SetProp_Emails( &prop_emails );
  ThotCallback (GetPrefEmailsBase() + EmailsMenu, INTEGER_DATA, (char*) 1);

  Prop_Passwords prop_passwords = GetValueDialog_Passwords();
  SetProp_Passwords( &prop_passwords );
  ThotCallback (GetPrefPasswordsBase() + PasswordsMenu, INTEGER_DATA, (char*) 1);

  // update the ns list from the listbox
  OnNSAdd(event);
  GetValueDialog_RDFa();
  UpdateRDFaNsList();
  ThotCallback (GetPrefRDFaBase() + RDFaMenu, INTEGER_DATA, (char*) 1);

  ThotCallback (MyRef, INTEGER_DATA, (char*) 1);
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
  wxWindow * p_page = (wxWindow *) (m_book->GetSelection() != -1) ? m_book->GetPage(m_book->GetSelection()) : NULL;

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
      // SetupDialog_DAV( GetProp_DAV() );
    }
#endif /* DAV */
#ifdef TEMPLATES
  else if ( p_page->GetId() == wxXmlResource::GetXRCID(_T("wxID_PAGE_TEMPLATES")) )
    SetupDialog_Templates();
#endif /* TEMPLATES */
  else if ( p_page->GetId() == wxXmlResource::GetXRCID(_T("wxID_PAGE_EMAILS")) )
    {
      ThotCallback (GetPrefEmailsBase() + EmailsMenu, INTEGER_DATA, (char*) 2);
      SetupDialog_Emails( GetProp_Emails() );
    }
  else if ( p_page->GetId() == wxXmlResource::GetXRCID(_T("wxID_PAGE_PASSWORDS")) )
    {
      ThotCallback (GetPrefPasswordsBase() + PasswordsMenu, INTEGER_DATA, (char*) 2);
      SetupDialog_Passwords( GetProp_Passwords() );
    }
  else if ( p_page->GetId() == wxXmlResource::GetXRCID(_T("wxID_PAGE_RDFa")) )
    {
      ThotCallback (GetPrefRDFaBase() + RDFaMenu, INTEGER_DATA, (char*) 2);
      SetupDialog_RDFa( GetProp_RDFaDef() );
    }

  ThotCallback (MyRef, INTEGER_DATA, (char*) 2);
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

  m_OnApplyLock = TRUE;
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
  ThotCallback (GetPrefEmailsBase() + EmailsMenu, INTEGER_DATA, (char*) 0);
  ThotCallback (GetPrefPasswordsBase() + PasswordsMenu, INTEGER_DATA, (char*) 0);
  ThotCallback (GetPrefRDFaBase() + RDFaMenu, INTEGER_DATA, (char*) 0);

  ThotCallback (MyRef, INTEGER_DATA, (char*) 0);
  TtaRedirectFocus();
  m_OnApplyLock = FALSE;
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
    {
      event.Skip(); // let wxWidgets close the dialog
      TtaRedirectFocus();
    }
}

#endif /* _WX */
