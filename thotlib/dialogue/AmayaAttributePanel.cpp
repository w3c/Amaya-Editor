#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/spinctrl.h"

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "selection.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "frame.h"
#include "view.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "attrmenu_f.h"
#include "frame_tv.h"
#include "views_f.h"

#include "message_wx.h"
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"

#include "AmayaAttributePanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFrame.h"
#include "AmayaFloatingPanel.h"
#include "AmayaSubPanelManager.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaAttributePanel, AmayaSubPanel)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaAttributePanel
 *      Method:  AmayaAttributePanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaAttributePanel::AmayaAttributePanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_ATTRIBUTE") )
  ,m_NbAttr(0)
  ,m_NbAttr_evt(0)
  ,m_pRBEnum(NULL)
{
  m_pVPanelParent       = XRCCTRL(*m_pPanel, "wxID_PANEL_ATTRVALUE", wxPanel);
  m_pVPanelSizer  = m_pVPanelParent->GetSizer();

  m_pAttrList     = XRCCTRL(*m_pPanel, "wxID_CLIST_ATTR", wxCheckListBox);      
  m_pAutoRefresh  = XRCCTRL(*m_pPanel, "wxID_CHECK_AUTOREF", wxCheckBox);
  m_pPanel_Lang   = XRCCTRL(*m_pVPanelParent, "wxID_ATTRIBUTE_LANG", wxPanel);
  m_pPanel_Text   = XRCCTRL(*m_pVPanelParent, "wxID_ATTRIBUTE_TEXT", wxPanel);
  m_pPanel_Enum   = XRCCTRL(*m_pVPanelParent, "wxID_ATTRIBUTE_ENUM", wxPanel);
  m_pPanel_Num    = XRCCTRL(*m_pVPanelParent, "wxID_ATTRIBUTE_NUM", wxPanel);
  m_pPanel_ApplyArea = XRCCTRL(*m_pVPanelParent, "wxID_PANEL_APPLY_AREA", wxPanel);

  // setup labels
  XRCCTRL(*m_pVPanelParent, "wxID_OK", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  XRCCTRL(*m_pVPanelParent, "wxID_BUTTON_DEL_ATTR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DEL)));
  m_pTitleText->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ATTR)));
  m_pAutoRefresh->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_REFRESH)));
  m_pPanel_Num->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_VALUE_OF_ATTR)));
  m_pPanel_Enum->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_VALUE_OF_ATTR)));
  m_pPanel_Text->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_VALUE_OF_ATTR)));
  m_pPanel_Lang->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_VALUE_OF_ATTR)));

  // init value panels visibility
  ShowAttributValue( wxATTR_TYPE_NONE );

  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaAttributePanel
 *      Method:  ~AmayaAttributePanel
 * Description:  destructor
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaAttributePanel::~AmayaAttributePanel()
{
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaAttributePanel
 *      Method:  GetPanelType
 * Description:  
 *--------------------------------------------------------------------------------------
 */
int AmayaAttributePanel::GetPanelType()
{
  return WXAMAYA_PANEL_ATTRIBUTE;
}


/*----------------------------------------------------------------------
  IsFreezed check if the attribute list should be updated or not
  params:
  returns:
  ----------------------------------------------------------------------*/
bool AmayaAttributePanel::IsFreezed()
{
  return !m_pAutoRefresh->IsChecked();
}

/*----------------------------------------------------------------------
  SendDataToPanel refresh the attribute list or show the value panels
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SendDataToPanel( AmayaParams& p )
{
  int action_id = (int)p.param1;
  switch ( action_id )
    {
    case wxATTR_ACTION_LISTUPDATE:
      ShowAttributValue( wxATTR_TYPE_NONE );
      SetupListValue( (const char *)p.param2, (int)p.param3, (const int *)p.param4, (const char *)p.param5, (int)p.param6, (const int *)p.param7 );
      break;
    case wxATTR_ACTION_SETUPLANG:
      m_CurrentAttType = wxATTR_TYPE_LANG;
      ShowAttributValue( wxATTR_TYPE_LANG );
      SetMandatoryState( p.param2 != NULL );
      SetupLangValue( (const char *)p.param3,(const char *)p.param4, (const char *)p.param5, (int)p.param6, (int)p.param7 );
      break;
    case wxATTR_ACTION_SETUPTEXT:
      m_CurrentAttType = wxATTR_TYPE_TEXT;
      ShowAttributValue( wxATTR_TYPE_TEXT );
      SetMandatoryState( p.param2 != NULL );
      SetupTextValue( (const char *)p.param3 );
      break;
    case wxATTR_ACTION_SETUPENUM:
      m_CurrentAttType = wxATTR_TYPE_ENUM;
      ShowAttributValue( wxATTR_TYPE_ENUM );
      SetMandatoryState( p.param2 != NULL );
      SetupEnumValue( (const char *)p.param3, (int)p.param4, (int)p.param5 );
      break;
    case wxATTR_ACTION_SETUPNUM:
      m_CurrentAttType = wxATTR_TYPE_NUM;
      ShowAttributValue( wxATTR_TYPE_NUM );
      SetMandatoryState( p.param2 != NULL );
      SetupNumValue( (int)p.param3 );
      break;
    }
}

/*----------------------------------------------------------------------
  OnListSelectItem
  called when the user select one item is the list
  should update the right panel type only if this item is checked
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::OnListSelectItem( wxCommandEvent& event )
{
  SelectAttribute( event.GetSelection() );
}

/*----------------------------------------------------------------------
  SelectAttribute
  selecte an attribut to the given position
  params:
  bool force_checked : true if this function must ignore the checked item state
                       (default is false)
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SelectAttribute( int position, bool force_checked )
{
  // force the selection in the attribute list
  m_pAttrList->SetSelection(position);

  if (m_pAttrList->IsChecked(position) || force_checked)
    {
      // call the callback to show the right attribute value panel
      // there is two case because of old "event" attributs menu (GTK version)
      // TODO: simplify the code => remove this special case for event menu...
      int item_num = 0;
      if ( position >= m_NbAttr )
	{
	  item_num = position - m_NbAttr;
	  CallbackAttrMenu( -1, item_num, TtaGiveActiveFrame() );
	}
      else
	{
	  item_num = position;
	  CallbackAttrMenu( -2, item_num, TtaGiveActiveFrame() );
	}
    }
  else
    ShowAttributValue( wxATTR_TYPE_NONE );
}

/*----------------------------------------------------------------------
  OnListCheckItem
  called when the user check one item is the list
  should show or hide the panel detail depending on the ckeckbox state
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::OnListCheckItem( wxCommandEvent& event )
{
  if (!m_pAttrList->IsChecked(event.GetSelection()))
    {
      SelectAttribute( event.GetSelection(), true );

      if (!m_CurrentAttMandatory)
	{
	  /* remove attribute */
	  RemoveCurrentAttribute();
	}
      else
	{
	  /* this attribut is mandatory, user is not allowed to remove it ! */
	  m_pAttrList->Check(event.GetSelection(), true);
	  TtaDisplaySimpleMessage (INFO, LIB, TMSG_ATTR_MANDATORY);
	}
    }
  else
  {
    /* force the item selection */
    SelectAttribute( event.GetSelection() );
    /* creation of a new attribut with a default value */
    CreateCurrentAttribute();
  }
  event.Skip();
}

/*----------------------------------------------------------------------
  RemoveCurrentAttribut
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::RemoveCurrentAttribute()
{
  /* remove attribute */
  switch (m_CurrentAttType)
    {
    case wxATTR_TYPE_TEXT:
    case wxATTR_TYPE_ENUM:
    case wxATTR_TYPE_NUM:
      CallbackValAttrMenu (NumMenuAttr, 2, NULL);
      break;
    case wxATTR_TYPE_LANG:
      CallbackLanguageMenu(NumFormLanguage, 2, NULL);
      break;
    }
  
  /* try to redirect focus to canvas */
  TtaRedirectFocus();  
}

/*----------------------------------------------------------------------
  CreateCurrentAttribut
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::CreateCurrentAttribute()
{
  switch (m_CurrentAttType)
    {
    case wxATTR_TYPE_TEXT:
      {
	/* default value is a empty buffer */
	CallbackValAttrMenu (NumMenuAttrText, -1, "");
	CallbackValAttrMenu (NumMenuAttr, 1, NULL);
      }
      break;
    case wxATTR_TYPE_ENUM:
      {
	CallbackValAttrMenu (NumMenuAttrEnum, 0, NULL);
	CallbackValAttrMenu (NumMenuAttr, 1, NULL);
      }
      break;
    case wxATTR_TYPE_NUM:
      {
	CallbackValAttrMenu (NumMenuAttrNumber, 0, NULL);
	CallbackValAttrMenu (NumMenuAttr, 1, NULL);
      }
      break;
    case wxATTR_TYPE_LANG:
      {
	CallbackLanguageMenu(NumSelectLanguage, -1, "");
	CallbackLanguageMenu(NumFormLanguage, 1, NULL);
      }
      break;
    }
}


/*----------------------------------------------------------------------
  ShowAttributValue
  show the right panel type and initialize it
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::ShowAttributValue( wxATTR_TYPE type )
{
  if(IsFreezed())
    return;

  m_pVPanelSizer->Show( m_pPanel_Text, false );
  m_pVPanelSizer->Show( m_pPanel_Lang, false );
  m_pVPanelSizer->Show( m_pPanel_Enum, false );
  m_pVPanelSizer->Show( m_pPanel_Num, false );
  m_pVPanelSizer->Show( m_pPanel_ApplyArea, false );

  switch(type)
    {
    case wxATTR_TYPE_ENUM:
      {
	m_pVPanelSizer->Show( m_pPanel_Enum, true );
	m_pVPanelSizer->Show( m_pPanel_ApplyArea, true );
	m_pPanel_Enum->Refresh();
      }
      break;
    case wxATTR_TYPE_TEXT:
      {
	m_pVPanelSizer->Show( m_pPanel_Text, true );
	m_pVPanelSizer->Show( m_pPanel_ApplyArea, true );
	m_pPanel_Text->Refresh();
      }
      break;
    case wxATTR_TYPE_LANG:
      {
	m_pVPanelSizer->Show( m_pPanel_Lang, true );
	m_pVPanelSizer->Show( m_pPanel_ApplyArea, true );
	m_pPanel_Lang->Refresh();
      }
      break;
    case wxATTR_TYPE_NUM:
      {
	m_pVPanelSizer->Show( m_pPanel_Num, true );
	m_pVPanelSizer->Show( m_pPanel_ApplyArea, true );
	m_pPanel_Num->Refresh();
      }
      break;
    default:
      break;
    }

  GetParent()->GetParent()->Layout();
  m_pVPanelSizer->Layout();
  GetParent()->Layout();
  Layout();
  m_pPanelContentDetach->Layout();
}

/*----------------------------------------------------------------------
  SetupListValue
  init the attribut list
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SetupListValue( const char * p_attr_list, int nb_attr, const int * p_active_attr,
					  const char * p_attr_evt_list, int nb_attr_evt, const int * p_active_attr_evt )
{
  /* remember the selected entry */
  wxString last_entry = m_pAttrList->GetStringSelection();

  bool listHasFocus = (wxWindow::FindFocus() == m_pAttrList);

  m_NbAttr     = nb_attr;
  m_NbAttr_evt = nb_attr_evt;

  /* ----------------------------------------------------- */
  /* build the item list */
  /* --> clear the list */
  m_pAttrList->Clear();
  /* --> build the attr list */
  int index = 0;
  int i_item = 0;
  int ignore_attr_item = 0;
  int ignore_attr_evt_item = 0;
  while ( i_item < m_NbAttr )
    {
      /* just add T (toggle items) */
      if (p_attr_list[index] == 'T')
	m_pAttrList->Append( TtaConvMessageToWX( &p_attr_list[index+1] ) );
      else
	ignore_attr_item++;
      index += strlen (&p_attr_list[index]) + 1; /* one entry length */
      i_item++;
    }
  /* --> build the attr event list */
  index = 0;
  i_item = 0;
  while ( i_item < m_NbAttr_evt )
    {
      /* just add T (toggle items) */
      if (p_attr_evt_list[index] == 'T')
	m_pAttrList->Append( TtaConvMessageToWX( &p_attr_evt_list[index+1] ) );
      else
	ignore_attr_evt_item++;
      index += strlen (&p_attr_evt_list[index]) + 1; /* one entry length */
      i_item++;
    }
  /* ----------------------------------------------------- */


  /* ----------------------------------------------------- */
  /* check the items */
  /* --> attr list */
  i_item = 0;
  while ( i_item < m_NbAttr - ignore_attr_item )
    {
      m_pAttrList->Check(i_item, (p_active_attr[i_item]==1) );
      i_item++;
    }
  /* --> attr event list */
  i_item = 0;
  while ( i_item < m_NbAttr_evt - ignore_attr_evt_item )
    {
      m_pAttrList->Check(i_item+m_NbAttr-ignore_attr_item, (p_active_attr_evt[i_item]==1) );
      i_item++;
    }
  /* ----------------------------------------------------- */

  m_NbAttr     -= ignore_attr_item;
  m_NbAttr_evt -= ignore_attr_evt_item;
  
  // workaround for a wx bug : the list lost the focus when it is updated...
  if (listHasFocus)
    m_pAttrList->SetFocus();

  /* restore the last selected entry because 
   * UpdateAttrMenu rebuild the list and clear the selection */
  int item_to_select;
  if ( (item_to_select = m_pAttrList->FindString(last_entry)) != wxNOT_FOUND)
    SelectAttribute(item_to_select);
}


/*----------------------------------------------------------------------
  SetupLangValue
  send langage's panel values
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SetupLangValue( const char * selected_lang, 
					  const char * inherited_lang,
					  const char * lang_list,
					  int lang_list_nb,
					  int default_lang_id )
{
  /* setup the inherited langage label */
  wxStaticText * p_stext = XRCCTRL(*m_pPanel_Lang, "wxID_ATTR_LABEL_LANG_INHER", wxStaticText);
  p_stext->SetLabel( TtaConvMessageToWX( inherited_lang ) );

  /* setup the langage list */
  wxComboBox * p_combo = XRCCTRL(*m_pPanel_Lang, "wxID_ATTR_COMBO_LANG_LIST", wxComboBox);
  p_combo->Clear();
  int i_item = 0;
  int index = 0;
  while ( i_item < lang_list_nb )
    {
      p_combo->Append( TtaConvMessageToWX( &lang_list[index] ) );
      index += strlen(&lang_list[index])+1; /* one entry length */
      i_item++;
    }

  /* setup the selected language value */
  wxString wx_selected_lang = TtaConvMessageToWX(selected_lang);
  if ( wx_selected_lang != _T("") )
    p_combo->SetStringSelection(wx_selected_lang);
  else
  {
#ifndef _WINDOWS
    p_combo->SetValue(_T(""));
#endif /* _WINDOWS */
  }

  m_pPanel_Lang->Refresh();
}

/*----------------------------------------------------------------------
  SetupTextValue
  send text's panel values
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SetupTextValue( const char * text )
{
  wxTextCtrl * p_text_ctrl = XRCCTRL(*m_pPanel_Text, "wxID_ATTR_TEXT_VALUE", wxTextCtrl);
  p_text_ctrl->SetValue( TtaConvMessageToWX( text ) );
  m_pPanel_Text->Refresh();
}

/*----------------------------------------------------------------------
  SetupEnumValue
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SetupEnumValue( const char * enums, int nb_enum, int selected )
{
  /* test if the attribut type is boolean */
  if (nb_enum <= 1)
    {
      /* do not show any dialog for boolean type */
      ShowAttributValue( wxATTR_TYPE_NONE );
      return;
    }

  wxSizer * p_sizer = m_pPanel_Enum->GetSizer();
  /* remove the old enum field */
  if (m_pRBEnum)
    delete m_pRBEnum;

  /* create the new enum field */
  wxArrayString choices;
  int i_item = 0;
  int index = 0;
  while ( i_item < nb_enum )
    {
      choices.Add( TtaConvMessageToWX( &enums[index] ) );
      index += strlen(&enums[index])+1; /* one entry length */
      i_item++;
    }
  m_pRBEnum = new wxRadioBox( m_pPanel_Enum, -1, _T(""), wxDefaultPosition, wxDefaultSize,
			      choices, 1, wxRA_SPECIFY_COLS );
  if (selected != -1)
    m_pRBEnum->SetSelection(selected);
  p_sizer->Prepend(m_pRBEnum, 0, wxALL|wxEXPAND, 0);
  p_sizer->Layout();

  m_pPanel_Enum->Refresh();
}

/*----------------------------------------------------------------------
  SetupNumValue
  send num's panel values
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SetupNumValue( int num )
{
  wxSpinCtrl * p_spin_ctrl = XRCCTRL(*m_pPanel_Num, "wxID_ATTR_NUM_VALUE", wxSpinCtrl);
  p_spin_ctrl->SetValue( num );
  m_pPanel_Num->Refresh();
}

/*----------------------------------------------------------------------
  OnDelAttr called when the user want to remove an existing attribut (the selected one)
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::OnDelAttr( wxCommandEvent& event )
{
  RemoveCurrentAttribute();
}

/*----------------------------------------------------------------------
  OnApply called when the user want to change the attribute value
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::OnApply( wxCommandEvent& event )
{
  if (!IsFreezed())
    if (m_pAttrList->GetSelection() < 0 || !m_pAttrList->IsChecked(m_pAttrList->GetSelection()))
      return;

  switch (m_CurrentAttType)
    {
    case wxATTR_TYPE_TEXT:
      {
	wxTextCtrl * p_text_ctrl = XRCCTRL(*m_pPanel_Text, "wxID_ATTR_TEXT_VALUE", wxTextCtrl);
	wxString value = p_text_ctrl->GetValue();
	char buffer[MAX_LENGTH];
	strcpy( buffer, (const char*)value.mb_str(wxConvUTF8) );
	CallbackValAttrMenu (NumMenuAttrText, -1, buffer);
	/* create/modify attribute */
	CallbackValAttrMenu (NumMenuAttr, 1, NULL);

	/* try to redirect focus to canvas */
	TtaRedirectFocus();  
      }
      break;
    case wxATTR_TYPE_ENUM:
      {
	CallbackValAttrMenu (NumMenuAttrEnum, m_pRBEnum->GetSelection(), NULL);
	/* create/modify attribute */
	CallbackValAttrMenu (NumMenuAttr, 1, NULL);

	/* try to redirect focus to canvas */
	TtaRedirectFocus();  
      }
      break;
    case wxATTR_TYPE_NUM:
      {
	wxSpinCtrl * p_spin_ctrl = XRCCTRL(*m_pPanel_Num, "wxID_ATTR_NUM_VALUE", wxSpinCtrl);
	int value = p_spin_ctrl->GetValue();
	CallbackValAttrMenu (NumMenuAttrNumber, value, NULL);
	/* create/modify attribute */
	CallbackValAttrMenu (NumMenuAttr, 1, NULL);

	/* try to redirect focus to canvas */
	TtaRedirectFocus();  
      }
      break;
    case wxATTR_TYPE_LANG:
      {
	wxComboBox * p_cb = XRCCTRL(*m_pPanel_Lang, "wxID_ATTR_COMBO_LANG_LIST", wxComboBox);
	wxString value = p_cb->GetValue();
	char buffer[MAX_LENGTH];
	strcpy( buffer, (const char*)value.mb_str(wxConvUTF8) );
	CallbackLanguageMenu(NumSelectLanguage, -1, buffer);
	/* create/modify attribute */
	CallbackLanguageMenu(NumFormLanguage, 1, NULL);

	/* try to redirect focus to canvas */
	TtaRedirectFocus();  
      }
      break;
    }
}

/*----------------------------------------------------------------------
  OnAutoRefresh called when the user want to freeze/unfreeze the attribut panel
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::OnAutoRefresh( wxCommandEvent& event )
{
  if (m_pAutoRefresh->IsChecked())
    {
      m_pAttrList->Enable();
      DoUpdate();
      TtaRedirectFocus();
    }
  else
    {
      m_pAttrList->Disable();
    }
}

/*----------------------------------------------------------------------
  ForceAttributeUpdate force the current document to refresh the attribute list
  => UpdateAttrMenu is called => AmayaAttributePanel::UpdateAttributeList is finaly called
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::ForceAttributeUpdate()
{
  /* do the update */
  PtrDocument pDoc;
  int         view;
  GetDocAndView( TtaGiveActiveFrame(), &pDoc, &view );
  if (pDoc)
    UpdateAttrMenu( pDoc );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaAttributePanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
 *--------------------------------------------------------------------------------------
 */
void AmayaAttributePanel::DoUpdate()
{
  AmayaSubPanel::DoUpdate();
  ForceAttributeUpdate();  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaAttributePanel
 *      Method:  IsActive
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaAttributePanel::IsActive()
{
  return (AmayaSubPanel::IsActive() && !IsFreezed());
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaAttributePanel
 *      Method:  SetMandatoryState
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaAttributePanel::SetMandatoryState( bool is_mandatory )
{
  m_CurrentAttMandatory = is_mandatory;
  if (is_mandatory)
    XRCCTRL(*m_pPanel, "wxID_BUTTON_DEL_ATTR", wxBitmapButton)->Disable();
  else
    XRCCTRL(*m_pPanel, "wxID_BUTTON_DEL_ATTR", wxBitmapButton)->Enable();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaAttributePanel, AmayaSubPanel)
  EVT_LISTBOX(      XRCID("wxID_CLIST_ATTR"),      AmayaAttributePanel::OnListSelectItem )
  EVT_CHECKLISTBOX( XRCID("wxID_CLIST_ATTR"),      AmayaAttributePanel::OnListCheckItem )
  EVT_TEXT_ENTER(   XRCID("wxID_ATTR_COMBO_LANG_LIST"), AmayaAttributePanel::OnApply )
  EVT_TEXT_ENTER(   XRCID("wxID_ATTR_TEXT_VALUE"),      AmayaAttributePanel::OnApply )
  EVT_TEXT_ENTER(   XRCID("wxID_ATTR_NUM_VALUE"),       AmayaAttributePanel::OnApply )
  EVT_BUTTON(       XRCID("wxID_OK"),              AmayaAttributePanel::OnApply )
  EVT_BUTTON(       XRCID("wxID_BUTTON_DEL_ATTR"), AmayaAttributePanel::OnDelAttr )
  EVT_CHECKBOX(     XRCID("wxID_CHECK_AUTOREF"),   AmayaAttributePanel::OnAutoRefresh )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
