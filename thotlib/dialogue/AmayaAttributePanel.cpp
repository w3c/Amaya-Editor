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
  wxLogDebug( _T("AmayaAttributePanel::AmayaAttributePanel") );

  AssignDataPanelReferences();

  m_pVPanelSizer->Show( m_pPanel_Lang, false );
  m_pVPanelSizer->Show( m_pPanel_Text, false );
  m_pVPanelSizer->Show( m_pPanel_Enum, false );
  m_pVPanelSizer->Show( m_pPanel_Num,  false );
  m_pVPanelSizer->Layout(); // recalc layout

  // setup labels
  XRCCTRL(*m_pPanelContentDetach, "wxID_OK", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  XRCCTRL(*m_pPanel, "wxID_LABEL_TITLE", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ATTR)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHECK_AUTOREF", wxCheckBox)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_AUTOREFRESH)));
  m_pPanel_Num->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_VALUE_OF_ATTR)));
  m_pPanel_Enum->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_VALUE_OF_ATTR)));
  m_pPanel_Text->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_VALUE_OF_ATTR)));
  m_pPanel_Lang->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_VALUE_OF_ATTR)));
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
  wxLogDebug( _T("AmayaAttributePanel::~AmayaAttributePanel") );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaAttributePanel
 *      Method:  AssignDataPanelReferences
 * Description:  assign right references depending on floating state
 *--------------------------------------------------------------------------------------
 */
void AmayaAttributePanel::AssignDataPanelReferences()
{
  if (IsFloating())
    {
      m_pPanelContentDetach = XRCCTRL(*m_pFloatingPanel, "wxID_PANEL_CONTENT_DETACH", wxPanel);
      m_pVPanelParent       = XRCCTRL(*m_pFloatingPanel, "wxID_PANEL_ATTRVALUE", wxPanel);
    }
  else
    {
      m_pPanelContentDetach = XRCCTRL(*this, "wxID_PANEL_CONTENT_DETACH", wxPanel);
      m_pVPanelParent       = XRCCTRL(*this, "wxID_PANEL_ATTRVALUE", wxPanel);
    }
  m_pVPanelSizer  = m_pVPanelParent->GetSizer();
  m_pAttrList     = XRCCTRL(*m_pPanelContentDetach, "wxID_CLIST_ATTR", wxCheckListBox);      
  m_pAutoRefresh  = XRCCTRL(*m_pPanelContentDetach, "wxID_CHECK_AUTOREF", wxCheckBox);
  m_pPanel_Lang   = XRCCTRL(*m_pPanelContentDetach, "wxID_ATTRIBUTE_LANG", wxPanel);
  m_pPanel_Text   = XRCCTRL(*m_pPanelContentDetach, "wxID_ATTRIBUTE_TEXT", wxPanel);
  m_pPanel_Enum   = XRCCTRL(*m_pPanelContentDetach, "wxID_ATTRIBUTE_ENUM", wxPanel);
  m_pPanel_Num    = XRCCTRL(*m_pPanelContentDetach, "wxID_ATTRIBUTE_NUM", wxPanel);
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
  SendDataToPanel refresh the attribute list when user selection changes 
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SendDataToPanel( void * param1, void * param2, void * param3, void * param4, void * param5, void * param6 )
{
  const char * p_attr_list      = (const char *)param1;
  int nb_attr                   = (int)param2;
  const int * p_active_attr     = (const int *)param3;
  const char * p_attr_evt_list  = (const char *)param4;
  int nb_attr_evt               = (int)param5;
  const int * p_active_attr_evt = (const int *)param6;

  wxLogDebug(_T("AmayaAttributePanel - UpdateAttributeList") );

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

  // attribute list is updated, nothing should be selected : to be sure that nothing is shown, hide the value fields
  ShowAttributValue( wxATTR_TYPE_NONE );
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
  wxLogDebug( _T("AmayaAttributePanel - OnListSelectItem: isck=%s, issel=%s, sel=%d"),
	      event.IsChecked() ? _T("yes") : _T("no"),
	      event.IsSelection() ? _T("yes") : _T("no"),
	      event.GetSelection() );
  
  // check that this attribut is checked
  if ( !m_pAttrList->IsChecked(event.GetSelection()) )
    {
      ShowAttributValue( wxATTR_TYPE_NONE );
      return;
    }
  
  SelectAttribute( event.GetSelection() );
}

/*----------------------------------------------------------------------
  SelectAttribute
  selecte an attribut to the given position
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SelectAttribute( int position )
{
  // call the callback to show the right attribute value panel
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

/*----------------------------------------------------------------------
  OnListCheckItem
  called when the user check one item is the list
  should show or hide the panel detail depending on the ckeckbox state
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::OnListCheckItem( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaAttributePanel - OnListCheckItem: isck=%s, issel=%s, sel=%d"),
	      event.IsChecked() ? _T("yes") : _T("no"),
	      event.IsSelection() ? _T("yes") : _T("no"),
	      event.GetSelection() );

  if (!m_pAttrList->IsChecked(event.GetSelection()))
    {
      /* before to check an item force the selection */
      SelectAttribute( event.GetSelection() );

      /* remove attribute */
      if (m_CurrentAttType == wxATTR_TYPE_LANG)
	CallbackLanguageMenu(NumFormLanguage, 2, NULL);
      else
	CallbackValAttrMenu (NumMenuAttr, 2, NULL);
    }
  event.Skip();
}

/*----------------------------------------------------------------------
  ShowAttributValue
  show the right panel type and initialize it
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::ShowAttributValue( wxATTR_TYPE type )
{
  wxLogDebug(_T("AmayaAttributePanel - ShowAttributValue") );

  m_pVPanelSizer->Show( m_pPanel_Text, false );
  m_pVPanelSizer->Show( m_pPanel_Lang, false );
  m_pVPanelSizer->Show( m_pPanel_Enum, false );
  m_pVPanelSizer->Show( m_pPanel_Num, false );

  switch(type)
    {
    case wxATTR_TYPE_ENUM:
      {
	m_pVPanelSizer->Show( m_pPanel_Enum, true );
	m_pPanel_Enum->Refresh();
      }
      break;
    case wxATTR_TYPE_TEXT:
      {
	m_pVPanelSizer->Show( m_pPanel_Text, true );
	m_pPanel_Text->Refresh();
      }
      break;
    case wxATTR_TYPE_LANG:
      {
	m_pVPanelSizer->Show( m_pPanel_Lang, true );
	m_pPanel_Lang->Refresh();
      }
      break;
    case wxATTR_TYPE_NUM:
      {
	m_pVPanelSizer->Show( m_pPanel_Num, true );
	m_pPanel_Num->Refresh();
      }
      break;
    }

  m_pVPanelSizer->Layout();
  GetParent()->Layout();
  Layout();
  m_pPanelContentDetach->Layout();
  
  // remember the current attribut type, used to know what callback must be called
  m_CurrentAttType = type;
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
  p_combo->SetStringSelection(TtaConvMessageToWX(selected_lang));

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
			      choices, 0, wxRA_SPECIFY_COLS | wxNO_BORDER );
  if (selected != -1)
    m_pRBEnum->SetSelection(selected);
  p_sizer->Add(m_pRBEnum, 0, wxALL|wxEXPAND, 0);
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
      }
      break;
    case wxATTR_TYPE_ENUM:
      {
	CallbackValAttrMenu (NumMenuAttrEnum, m_pRBEnum->GetSelection(), NULL);
	/* create/modify attribute */
	CallbackValAttrMenu (NumMenuAttr, 1, NULL);
      }
      break;
    case wxATTR_TYPE_NUM:
      {
	wxSpinCtrl * p_spin_ctrl = XRCCTRL(*m_pPanel_Num, "wxID_ATTR_NUM_VALUE", wxSpinCtrl);
	int value = p_spin_ctrl->GetValue();
	CallbackValAttrMenu (NumMenuAttrNumber, value, NULL);
	/* create/modify attribute */
	CallbackValAttrMenu (NumMenuAttr, 1, NULL);
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
      m_pVPanelParent->Enable();
      DoUpdate();
    }
  else
    {
      m_pAttrList->Disable();
      m_pVPanelParent->Disable();
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

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaAttributePanel, AmayaSubPanel)
  EVT_LISTBOX( XRCID("wxID_CLIST_ATTR"),      AmayaAttributePanel::OnListSelectItem )
  EVT_CHECKLISTBOX( XRCID("wxID_CLIST_ATTR"), AmayaAttributePanel::OnListCheckItem )
  EVT_BUTTON( XRCID("wxID_OK"),               AmayaAttributePanel::OnApply )
  EVT_CHECKBOX( XRCID("wxID_CHECK_AUTOREF"),  AmayaAttributePanel::OnAutoRefresh )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
