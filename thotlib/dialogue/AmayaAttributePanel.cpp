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

#include "undo.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "attrmenu.h"
#include "attrmenu_f.h"
#include "tree_f.h"
#include "containers.h"
#include "frame_tv.h"
#include "views_f.h"

#include "content_f.h"
#include "language_f.h"

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

  /*----------------------------------------------------------------------
   *       Class:  AmayaAttributePanel
   *      Method:  AmayaAttributePanel
   * Description:  construct a panel (bookmarks, elements, attributes ...)
   *               TODO
   -----------------------------------------------------------------------*/
  AmayaAttributePanel::AmayaAttributePanel( wxWindow * p_parent_window,
                                            AmayaNormalWindow * p_parent_nwindow )
    : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_ATTRIBUTE") )
    ,m_attrList(NULL)
    ,m_currentAttElem(NULL)
    ,m_firstSel(NULL)
    ,m_lastSel(NULL)
    ,m_firstChar(0)
    ,m_lastChar(0)
    ,m_NbAttr(0)
    ,m_NbAttr_evt(0)
    ,m_disactiveCount(0)
{
  m_pVPanelParent       = XRCCTRL(*this, "wxID_PANEL_ATTRVALUE", wxPanel);
  m_pVPanelSizer  = m_pVPanelParent->GetSizer();

  m_pAttrList     = XRCCTRL(*this, "wxID_CLIST_ATTR", wxListCtrl);    
  m_pPanel_Lang   = XRCCTRL(*m_pVPanelParent, "wxID_ATTRIBUTE_LANG", wxPanel);
  m_pPanel_Text   = XRCCTRL(*m_pVPanelParent, "wxID_ATTRIBUTE_TEXT", wxPanel);
  m_pPanel_Enum   = XRCCTRL(*m_pVPanelParent, "wxID_ATTRIBUTE_ENUM", wxPanel);
  m_pPanel_Num    = XRCCTRL(*m_pVPanelParent, "wxID_ATTRIBUTE_NUM", wxPanel);
  m_pPanel_NewAttr = XRCCTRL(*m_pVPanelParent, "wxID_PANEL_CHOOSE_NEW_ATTRIBUTE", wxPanel);
  m_pNewAttrChoice = XRCCTRL(*m_pPanel_NewAttr, "wxID_CHOOSE_NEW_ATTRIBUTE", wxChoice);
  m_pChoiceEnum    = XRCCTRL(*m_pPanel_Enum, "wxID_ATTR_CHOICE_ENUM", wxChoice);

  // Setup list
  m_pAttrList->InsertColumn(0, TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ATTRIBUTE_NAME)));
  m_pAttrList->InsertColumn(1, TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ATTRIBUTE_VALUE)));
  
  // setup labels
//  XRCCTRL(*m_pPanel_Lang, "wxID_OK", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
//  XRCCTRL(*m_pPanel_Text, "wxID_OK", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
//  XRCCTRL(*m_pPanel_Enum, "wxID_OK", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
//  XRCCTRL(*m_pPanel_Num, "wxID_OK", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  XRCCTRL(*m_pPanel_Lang, "wxID_BUTTON_DEL_ATTR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DEL)));
  XRCCTRL(*m_pPanel_Text, "wxID_BUTTON_DEL_ATTR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DEL)));
  XRCCTRL(*m_pPanel_Enum, "wxID_BUTTON_DEL_ATTR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DEL)));
  XRCCTRL(*m_pPanel_Num, "wxID_BUTTON_DEL_ATTR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DEL)));

//  XRCCTRL(*m_pPanel_NewAttr, "wxID_BUTTON_NEW_ATTRIBUTE", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_INSERT)));
  XRCCTRL(*m_pPanel_NewAttr, "wxID_ATTR_LABEL_NEW_ATTR", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_INSERT)));
  
  m_pTitleText->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ATTR)));
  m_pPanel_Num->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_VALUE_OF_ATTR)));
  m_pPanel_Enum->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_VALUE_OF_ATTR)));
  m_pPanel_Text->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_VALUE_OF_ATTR)));
  m_pPanel_Lang->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_VALUE_OF_ATTR)));

  // init value panels visibility
  ShowAttributValue( wxATTR_TYPE_NONE );

  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );
  
  SetupListValue(NULL);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAttributePanel
 *      Method:  ~AmayaAttributePanel
 * Description:  destructor
 *               TODO
 -----------------------------------------------------------------------*/
AmayaAttributePanel::~AmayaAttributePanel()
{
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );
}


/*----------------------------------------------------------------------
 *       Class:  AmayaAttributePanel
 *      Method:  GetPanelType
 * Description:  
 -----------------------------------------------------------------------*/
int AmayaAttributePanel::GetPanelType()
{
  return WXAMAYA_PANEL_ATTRIBUTE;
}


/*----------------------------------------------------------------------
  Check if the current attribute (if any) is mandatory or
  if can be modified
  returns: true if mandatory
  ----------------------------------------------------------------------*/
bool AmayaAttributePanel::IsMandatory()const
{
  return (!m_currentAttElem)||(m_currentAttElem&&m_currentAttElem->oblig);
}

/*----------------------------------------------------------------------
  SendDataToPanel refresh the attribute list or show the value panels
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SendDataToPanel( AmayaParams& p )
{
  DesactivatePanel();
  
  switch ( (int)p.param1 )
    {
    case wxATTR_ACTION_LISTUPDATE:
      m_firstSel  = (PtrElement)p.param5;
      m_lastSel   = (PtrElement)p.param6;
      m_firstChar = p.param7;
      m_lastChar  = p.param8;
      ShowAttributValue( wxATTR_TYPE_NONE );
      SetupListValue((DLList)p.param2);
      break;
    }
  ActivePanel();
}


/*----------------------------------------------------------------------
  SelectAttribute
  selecte an attribut to the given position
  params:
  bool force_checked : true if this function must ignore the checked item state
  (default is false)
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SelectAttribute( int position)
{
  PtrTtAttribute  pAttr;
  if (position!=wxID_ANY)
    {
      m_currentAttElem = (PtrAttrListElem)m_pAttrList->GetItemData(position);
      if (m_currentAttElem)
        {
          pAttr = AttrListElem_GetTtAttribute(m_currentAttElem);
          if(pAttr)
            {
              if(pAttr->AttrType==AtReferenceAttr)
                CallbackEditRefAttribute(m_currentAttElem, TtaGiveActiveFrame());
              else if(m_currentAttElem->num == 1)
                {
                  ShowAttributValue(wxATTR_TYPE_LANG);
                  SetupLangAttr(m_currentAttElem);
                  return;
                }
              else
                {
                  switch(pAttr->AttrType)
                  {
                    case AtEnumAttr:
                      ShowAttributValue(wxATTR_TYPE_ENUM);
                      SetupEnumAttr(m_currentAttElem);
                      return;
                    case AtTextAttr:
                      ShowAttributValue(wxATTR_TYPE_TEXT);
                      SetupTextAttr(m_currentAttElem);
                      return;
                    case AtNumAttr:
                      ShowAttributValue(wxATTR_TYPE_NUM);
                      SetupNumAttr(m_currentAttElem);
                      return;
                    default:
                      break;
                  }
                }
            }
        }
    }
  m_currentAttElem = NULL;
  ShowAttributValue( wxATTR_TYPE_NONE );
}


/*----------------------------------------------------------------------
  QueryRemoveCurrentAttribute
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::QueryRemoveCurrentAttribute()
{

    if (!IsMandatory())
      {
        RemoveCurrentAttribute();
      }
    else
      {
        /* this attribut is mandatory, user is not allowed to remove it ! */
        TtaDisplaySimpleMessage (INFO, LIB, TMSG_ATTR_MANDATORY);
      }
}

/*----------------------------------------------------------------------
  RemoveCurrentAttribut
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::RemoveCurrentAttribute()
{
  Document doc = TtaGetDocument((Element)m_firstSel);
  DisplayMode mode = TtaGetDisplayMode(doc);
  
  DesactivatePanel();

  if(m_CurrentAttType!=wxATTR_TYPE_NONE && m_firstSel
          && m_currentAttElem && m_currentAttElem->val)
    {
      TtaSetDisplayMode(doc, DeferredDisplay);
      TtaOpenUndoSequence(doc, (Element)m_firstSel, (Element)m_lastSel,
                                                      m_firstChar, m_lastChar);
      TtaRegisterAttributeDelete((Attribute)m_currentAttElem->val,
                                                     (Element)m_firstSel, doc);
      TtaRemoveAttribute((Element)m_firstSel, (Attribute)m_currentAttElem->val,
                                                                          doc);
      TtaSetDocumentModified(doc);
      TtaCloseUndoSequence(doc);
      TtaSetDisplayMode(doc, mode);
      ForceAttributeUpdate();
    }

  ActivePanel();

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
  AttributeType attType;
  Attribute     attr;
  Document      doc = TtaGetDocument((Element)m_firstSel);
  DisplayMode   mode = TtaGetDisplayMode(doc);
  wxString      name;
  long          index;

  if(m_pNewAttrChoice->GetSelection()!=wxNOT_FOUND)
    {
      PtrAttrListElem elem = (PtrAttrListElem)
            m_pNewAttrChoice->GetClientData(m_pNewAttrChoice->GetSelection());
      if(elem)
        {
          name = wxString(AttrListElem_GetName(elem), wxConvUTF8);
          attType.AttrSSchema = (SSchema)elem->pSS;
          attType.AttrTypeNum = elem->num;
          attr = TtaNewAttribute(attType);
          if(attr)
            {
              TtaSetDisplayMode(doc, DeferredDisplay);
              TtaOpenUndoSequence(doc, (Element)m_firstSel, (Element)m_lastSel,
                                                      m_firstChar, m_lastChar);
              TtaAttachAttribute((Element)m_firstSel, attr, doc);
              TtaRegisterAttributeCreate(attr, (Element)m_firstSel, doc);
              TtaCloseUndoSequence(doc);
              TtaSetDisplayMode(doc, mode);
              TtaSetDocumentModified(doc);
              ForceAttributeUpdate();
              
              index = m_pAttrList->FindItem(wxID_ANY, name);
              if(index!=wxID_ANY)
                {
                  m_pAttrList->SetItemState(index,
                                   wxLIST_STATE_FOCUSED|wxLIST_STATE_SELECTED,
                                   wxLIST_STATE_FOCUSED|wxLIST_STATE_SELECTED);
                  SelectAttribute(index);
                }
            }
        }
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
  m_CurrentAttType = type;

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
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SetupListValue(DLList attrList)
{
  ForwardIterator iter = DLList_GetForwardIterator(attrList);
  DLListNode      node;
  PtrAttrListElem elem;
  long            index;
  char            buffer[MAX_TXT_LEN];
  int             size;
  AttributeType   type;

  m_pAttrList->DeleteAllItems();
  m_pNewAttrChoice->Clear();
  
  if(m_attrList)
    DLList_Destroy(m_attrList);
  m_attrList = attrList;
  if(attrList)
    {
      ITERATOR_FOREACH(iter, DLListNode, node)
      {
        elem = (PtrAttrListElem)node->elem;
        if (elem)
          {
            if (elem->val)
              {
                index = m_pAttrList->InsertItem(m_pAttrList->GetItemCount(),
                            TtaConvMessageToWX(AttrListElem_GetName(elem)));
                switch(AttrListElem_GetType(elem))
                {
                  case AtNumAttr:
                    m_pAttrList->SetItem(index, 1, 
                        wxString::Format(wxT("%d"),
                            TtaGetAttributeValue((Attribute)elem->val)));
                    break;
                  case AtTextAttr:
                    size = MAX_TXT_LEN;
                    TtaGiveTextAttributeValue((Attribute)elem->val, buffer, &size);
                    m_pAttrList->SetItem(index, 1, wxString(buffer, wxConvUTF8));
                    break;
                  case AtEnumAttr:
                    type.AttrSSchema = (int*) elem->pSS;
                    type.AttrTypeNum = elem->num;
                    m_pAttrList->SetItem(index, 1, wxString(
                            TtaGetAttributeValueName(type, 
                                TtaGetAttributeValue((Attribute)elem->val)), 
                                wxConvUTF8));
                    break;
                  case AtReferenceAttr:
                  default:
                    break;
                }
                if(elem->oblig)
                  m_pAttrList->SetItemTextColour(index, wxColour(128, 0, 0));
                m_pAttrList->SetItemData(index, (long)elem);
              }
            else
              {
                index = m_pNewAttrChoice->Append(
                            TtaConvMessageToWX(AttrListElem_GetName(elem)));
                m_pNewAttrChoice->SetClientData(index, (void*)elem);
              }
            
          }
      }
      TtaFreeMemory(iter);
    }
  
  // Resize columns.
  long sz0, sz1;
  m_pAttrList->Freeze();
  m_pAttrList->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
  m_pAttrList->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
  sz0 = m_pAttrList->GetColumnWidth(0);
  sz1 = m_pAttrList->GetColumnWidth(1);
  m_pAttrList->SetColumnWidth(0, wxLIST_AUTOSIZE);
  m_pAttrList->SetColumnWidth(1, wxLIST_AUTOSIZE);
  if(sz0 > m_pAttrList->GetColumnWidth(0))
    m_pAttrList->SetColumnWidth(0, sz0);
  if(sz1 > m_pAttrList->GetColumnWidth(1))
    m_pAttrList->SetColumnWidth(1, sz1);
    m_pAttrList->Thaw();
}

/*----------------------------------------------------------------------
  SetupLangAttr
  send langage's panel values
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SetupLangAttr(PtrAttrListElem elem)
{
  PtrAttribute        currAttr = elem->val;
  Language            language;
  PtrAttribute        pHeritAttr;
  PtrElement          pElAttr;
  char               *ptr;
  char                languageCode[MAX_TXT_LEN];
  char                label[200];
  int                 defItem, nbItem;
  char                lang_list[MAX_TXT_LEN];
  int                 item = 0;
  int                 index = 0;

  wxArrayString arr;

  wxChoice     *p_combo = XRCCTRL(*m_pPanel_Lang, "wxID_ATTR_COMBO_LANG_LIST", wxChoice);
  wxStaticText *p_stext = XRCCTRL(*m_pPanel_Lang, "wxID_ATTR_LABEL_LANG_INHER", wxStaticText);

  if(elem && currAttr)
    {
      /* Initialize the language selector */
      languageCode[0] = EOS;
      if (currAttr && currAttr->AeAttrText)
        CopyBuffer2MBs (currAttr->AeAttrText, 0, (unsigned char*)languageCode,
                        MAX_TXT_LEN);
      if (languageCode[0] == EOS)
        {
          /* look for the inherited attribute value Language */
          strcpy (label, TtaGetMessage (LIB, TMSG_INHERITED_LANG));
          pHeritAttr = GetTypedAttrAncestor (m_firstSel, 1, NULL, &pElAttr);
          if (pHeritAttr && pHeritAttr->AeAttrText)
            {
              /* the attribute value is a RFC-1766 code. Convert it into */
              /* a language name */
              CopyBuffer2MBs (pHeritAttr->AeAttrText, 0,
                              (unsigned char*)languageCode, MAX_TXT_LEN);
              language = TtaGetLanguageIdFromName (languageCode);
              p_stext->SetLabel( TtaConvMessageToWX( TtaGetLanguageName(language)));
            }
        }
      else
        label[0] = EOS;

      ptr = GetListOfLanguages (lang_list, MAX_TXT_LEN, languageCode, &nbItem, &defItem);

      p_combo->Clear();
      for(item=0, index=0; item<nbItem; item++)
        {
          p_combo->Append( TtaConvMessageToWX( &lang_list[index] ) );
          index += strlen(&lang_list[index])+1; /* one entry length */
        }

      /* setup the selected language value */
      wxString wx_selected_lang = TtaConvMessageToWX(ptr);
      if ( wx_selected_lang != _T("") )
        p_combo->SetStringSelection(wx_selected_lang);
    }
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
  p_text_ctrl->SetInsertionPointEnd();
}

void AmayaAttributePanel::SetupTextAttr(PtrAttrListElem elem)
{
  PtrAttribute      currAttr = elem->val;
  char              buffer[MAX_TXT_LEN];
  int               i = MAX_TXT_LEN-1;
  char             *tmp;

  if(elem && currAttr)
    {
      
      if(currAttr->AeAttrText)
        {
          i = CopyBuffer2MBs (currAttr->AeAttrText, 0, (unsigned char*)buffer, i);
          tmp = (char *)TtaConvertMbsToByte ((unsigned char *)buffer,
                                                         TtaGetDefaultCharset ());
          SetupTextValue(tmp);
          TtaFreeMemory (tmp);
        }
      else
        SetupTextValue("");
    }
}

/*----------------------------------------------------------------------
  SetupEnumValue
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SetupEnumValue( wxArrayString& enums, int selected )
{
  /* test if the attribut type is boolean */
  if (enums.GetCount() <= 1)
    {
      /* do not show any dialog for boolean type */
      ShowAttributValue( wxATTR_TYPE_NONE );
      return;
    }

  wxChoice* choice = XRCCTRL(*m_pPanel_Enum, "wxID_ATTR_CHOICE_ENUM", wxChoice);

  if(choice)
    {
      choice->Clear();
      choice->Append(enums);
      choice->SetSelection(selected);
    }
}

void AmayaAttributePanel::SetupEnumAttr(PtrAttrListElem elem)
{
  PtrAttribute      currAttr = elem->val;
  int               i, val;
  TtAttribute      *pAttr = AttrListElem_GetTtAttribute(elem);

  wxArrayString     arr;
  
  
  if(elem && currAttr && pAttr)
    {
      for (val=0; val < pAttr->AttrNEnumValues; val++)
          arr.Add(wxString(pAttr->AttrEnumValue[val], wxConvUTF8));
      /* current value */
      i = 0;
      if (currAttr != NULL && currAttr->AeAttrValue > 0)
        i = currAttr->AeAttrValue - 1;
    }
  SetupEnumValue(arr, i);
}

/*----------------------------------------------------------------------
  SetupNumValue
  send num's panel values
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::SetupNumValue( int num, int begin, int end )
{
  wxSpinCtrl * p_spin_ctrl = XRCCTRL(*m_pPanel_Num, "wxID_ATTR_NUM_VALUE", wxSpinCtrl);
  p_spin_ctrl->SetRange(begin, end);
  p_spin_ctrl->SetValue( num );
  m_pPanel_Num->Refresh();
}

void AmayaAttributePanel::SetupNumAttr(PtrAttrListElem elem)
{
  PtrAttribute      currAttr = elem->val;
  int               i, begin, end;
  char*             title;
  TtAttribute      *pAttr = AttrListElem_GetTtAttribute(elem);

  if(elem && currAttr && pAttr)
    {
      i     = currAttr->AeAttrValue;
      title = pAttr->AttrName;
      if(!strcmp (elem->pSS->SsName, "HTML") &&
          (!strcmp (title, "rowspan") ||
           !strcmp (title, "colspan") ||
           !strcmp (title, "rows") ||
           !strcmp (title, "cols")))
        {
          if (i < 1)
            i = 1;
          begin = 1;
          end = 1000;
        }
      else
        {
          begin = 0;
          end = 1000;
        }
      SetupNumValue(i, begin, end);
    }
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


void AmayaAttributePanel::ModifyListAttrValue(const wxString& attrName,
                                                       const wxString& attrVal)
{
  long index = m_pAttrList->FindItem(wxID_ANY, attrName);
  if(index!=wxNOT_FOUND)
    {
      m_pAttrList->SetItem(index, 1, attrVal);
    }
}

wxString AmayaAttributePanel::GetCurrentSelectedAttrName()const
{
  long index = m_pAttrList->GetNextItem(wxID_ANY, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if(index!=wxNOT_FOUND)
    return m_pAttrList->GetItemText(index);
  else
    return wxT("");
}

/*----------------------------------------------------------------------
  OnApply called when the user want to change the attribute value
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributePanel::OnApply( wxCommandEvent& event )
{
  DisplayMode   mode;
  char          buffer[MAX_LENGTH];
  Document      doc;
  wxString      value;

  if(m_currentAttElem && m_currentAttElem->val && m_firstSel)
    {
      doc = TtaGetDocument((Element)m_firstSel);
      
      mode = TtaGetDisplayMode(doc);
      TtaSetDisplayMode(doc, DeferredDisplay);
      TtaOpenUndoSequence(doc, (Element)m_firstSel, (Element)m_lastSel,
                                                      m_firstChar, m_lastChar);
      TtaRegisterAttributeReplace((Attribute)m_currentAttElem->val,
                                                     (Element)m_firstSel, doc);

      switch (m_CurrentAttType)
        {
        case wxATTR_TYPE_TEXT:
          {

            wxTextCtrl * p_text_ctrl = XRCCTRL(*m_pPanel_Text, "wxID_ATTR_TEXT_VALUE", wxTextCtrl);
            value = p_text_ctrl->GetValue();
            strncpy(buffer, (const char*)value.mb_str(wxConvUTF8), MAX_LENGTH-1);
            TtaSetAttributeText((Attribute)m_currentAttElem->val,
                                buffer,
                                (Element)m_firstSel, doc);
            /* try to redirect focus to canvas */
            TtaRedirectFocus();  
          }
          break;
        case wxATTR_TYPE_ENUM:
          {
            TtaSetAttributeValue((Attribute)m_currentAttElem->val,
                m_pChoiceEnum->GetSelection()+1,
                (Element)m_firstSel, doc);
            value = m_pChoiceEnum->GetStringSelection();
            /* try to redirect focus to canvas */
            TtaRedirectFocus();  
          }
          break;
        case wxATTR_TYPE_NUM:
          {
            wxSpinCtrl * p_spin_ctrl = XRCCTRL(*m_pPanel_Num, "wxID_ATTR_NUM_VALUE", wxSpinCtrl);
            TtaSetAttributeValue((Attribute)m_currentAttElem->val,
                p_spin_ctrl->GetValue(),
                (Element)m_firstSel, doc);
            value.Printf(wxT("%d"), p_spin_ctrl->GetValue());
            /* try to redirect focus to canvas */
            TtaRedirectFocus();  
          }
          break;
        case wxATTR_TYPE_LANG:
          {
            wxChoice * p_cb = XRCCTRL(*m_pPanel_Lang, "wxID_ATTR_COMBO_LANG_LIST", wxChoice);
            value = p_cb->GetStringSelection();
            strcpy( buffer, (const char*)value.mb_str(wxConvUTF8) );
            strcpy( buffer, TtaGetLanguageCodeFromName (buffer));
            
            TtaSetAttributeText((Attribute)m_currentAttElem->val,
                                buffer,
                                (Element)m_firstSel, doc);
            /* try to redirect focus to canvas */
            TtaRedirectFocus();  
          }
          break;
        case wxATTR_TYPE_NONE:
          wxASSERT(FALSE); /* should not append ? */
          break;
        }

      ModifyListAttrValue(GetCurrentSelectedAttrName(), value);
      TtaSetDocumentModified(doc);
      TtaCloseUndoSequence(doc);
      TtaSetDisplayMode(doc, mode);
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
  GetDocAndView (TtaGiveActiveFrame(), &pDoc, &view);
  if (pDoc)
    UpdateAttrMenu (pDoc, TRUE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAttributePanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
 -----------------------------------------------------------------------*/
void AmayaAttributePanel::DoUpdate()
{
  AmayaSubPanel::DoUpdate();
  ForceAttributeUpdate();  
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAttributePanel
 *      Method:  IsActive
 * Description:  
 -----------------------------------------------------------------------*/
bool AmayaAttributePanel::IsActive()
{
  return (AmayaSubPanel::IsActive());
}



void AmayaAttributePanel::OnListItemSelected(wxListEvent& event)
{
  if(IsPanelActive())
    SelectAttribute(event.GetIndex());
  event.Skip();
}

void AmayaAttributePanel::OnListItemDeselected(wxListEvent& event)
{
  if(IsPanelActive())
    SelectAttribute(wxID_ANY);
  event.Skip();
}

void AmayaAttributePanel::OnInsert( wxCommandEvent& WXUNUSED(event))
{
  CreateCurrentAttribute();
}

void AmayaAttributePanel::OnUpdateDeleteButton(wxUpdateUIEvent& event)
{
  event.Enable(!IsMandatory());
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaAttributePanel, AmayaSubPanel)
  EVT_LIST_ITEM_SELECTED(XRCID("wxID_CLIST_ATTR"), AmayaAttributePanel::OnListItemSelected)
  EVT_LIST_ITEM_DESELECTED(XRCID("wxID_CLIST_ATTR"), AmayaAttributePanel::OnListItemDeselected)
  
  EVT_TEXT_ENTER( XRCID("wxID_ATTR_TEXT_VALUE"),      AmayaAttributePanel::OnApply )
  EVT_TEXT_ENTER( XRCID("wxID_ATTR_NUM_VALUE"),       AmayaAttributePanel::OnApply )

  EVT_CHOICE(XRCID("wxID_ATTR_COMBO_LANG_LIST"), AmayaAttributePanel::OnApply)
  EVT_CHOICE(XRCID("wxID_ATTR_CHOICE_ENUM"), AmayaAttributePanel::OnApply)

  EVT_BUTTON(     XRCID("wxID_OK"),              AmayaAttributePanel::OnApply )
  
  EVT_BUTTON(     XRCID("wxID_BUTTON_DEL_ATTR"), AmayaAttributePanel::OnDelAttr )
  EVT_CHOICE(XRCID("wxID_CHOOSE_NEW_ATTRIBUTE"), AmayaAttributePanel::OnInsert)

  EVT_UPDATE_UI(  XRCID("wxID_BUTTON_DEL_ATTR"), AmayaAttributePanel::OnUpdateDeleteButton)
END_EVENT_TABLE()

#endif /* #ifdef _WX */
