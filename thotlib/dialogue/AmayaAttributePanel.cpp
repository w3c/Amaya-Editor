/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/spinctrl.h"
#include "wx/tokenzr.h"

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
#include "callback_f.h"
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
#include "structselect_f.h"

#include "AmayaAttributePanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFrame.h"

#define COLOR_MANDATORY   wxColour(128, 0, 0)
#define COLOR_READONLY    wxColour(64, 64, 64)
#define COLOR_NEW         wxColour(0, 128, 0)

//
//
// AmayaAttributeToolPanel
//
//
wxString AmayaAttributeToolPanel::s_subpanelClassNames[wxATTR_PANEID_MAX]=
{
  wxT("AmayaEnumAttributeSubpanel"),      // wxATTR_PANEID_ENUM
  wxT("AmayaStringAttributeSubpanel"),    // wxATTR_PANEID_TEXT
  wxT("AmayaNumAttributeSubpanel"),       // wxATTR_PANEID_NUM
  wxT("AmayaLangAttributeSubpanel")       // wxATTR_PANEID_LANG
};

AmayaAttributeToolPanel::wxATTR_PANEID AmayaAttributeToolPanel::s_subpanelAssoc[restr_content_max][wxATTR_INTTYPE_MAX] = 
{
  // wxATTR_INTTYPE_NUM  wxATTR_INTTYPE_TEXT wxATTR_INTTYPE_REF  wxATTR_INTTYPE_ENUM wxATTR_INTTYPE_LANG
    {wxATTR_PANEID_NUM,  wxATTR_PANEID_TEXT, wxATTR_PANEID_NONE, wxATTR_PANEID_ENUM, wxATTR_PANEID_LANG}, // restr_content_no_restr
    {wxATTR_PANEID_NUM,  wxATTR_PANEID_NUM,  wxATTR_PANEID_NONE, wxATTR_PANEID_ENUM, wxATTR_PANEID_NONE}, // restr_content_number
    {wxATTR_PANEID_NONE, wxATTR_PANEID_TEXT, wxATTR_PANEID_NONE, wxATTR_PANEID_ENUM, wxATTR_PANEID_LANG}, // restr_content_string
    {wxATTR_PANEID_NONE, wxATTR_PANEID_NONE, wxATTR_PANEID_NONE, wxATTR_PANEID_NONE, wxATTR_PANEID_NONE}, // restr_content_list
    {wxATTR_PANEID_NONE, wxATTR_PANEID_LANG, wxATTR_PANEID_NONE, wxATTR_PANEID_LANG, wxATTR_PANEID_LANG}  // restr_content_lang
};

IMPLEMENT_DYNAMIC_CLASS(AmayaAttributeToolPanel, AmayaToolPanel)

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
AmayaAttributeToolPanel::AmayaAttributeToolPanel():
  AmayaToolPanel()
,m_attrList(NULL)
,m_currentAttElem(NULL)
,m_firstSel(NULL)
,m_lastSel(NULL)
,m_firstChar(0)
,m_lastChar(0)
,m_NbAttr(0)
,m_NbAttr_evt(0)
,m_currentPane(wxATTR_PANEID_NONE)
,m_pCurrentlyEditedControl(NULL)
,m_disactiveCount(0)
{
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
AmayaAttributeToolPanel::~AmayaAttributeToolPanel()
{
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
bool AmayaAttributeToolPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  int sz = 0;

  if (!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_ATTRIBUTE")))
    return false;
  // initialize the col width
  TtaGetEnvInt ("ATTR_COL_0", &sz);
  if (sz && sz > 20)
    m_sz0 = sz;
  else
    m_sz0 = 50;
  TtaGetEnvInt ("ATTR_COL_1", &sz);
  if (sz && sz > 20)
    m_sz1 = sz;
  else
    m_sz1 = 150;

#ifdef _WINDOWS
  SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#endif /* _WINDOWS */
  m_pVPanelParent = XRCCTRL(*this, "wxID_PANEL_ATTRVALUE", wxPanel);
  m_pVPanelSizer = m_pVPanelParent->GetSizer();

  m_pAttrList = XRCCTRL(*this, "wxID_CLIST_ATTR", wxListCtrl);
  m_pPanel_NewAttr = XRCCTRL(*m_pVPanelParent, "wxID_PANEL_CHOOSE_NEW_ATTRIBUTE", wxPanel);
  m_pNewAttrChoice = XRCCTRL(*m_pPanel_NewAttr, "wxID_CHOOSE_NEW_ATTRIBUTE", wxChoice);

  // Setup list
  m_pAttrList->InsertColumn(0, TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ATTRIBUTE_NAME)));
  m_pAttrList->InsertColumn(1, TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ATTRIBUTE_VALUE)));
  
  // setup labels
  XRCCTRL(*m_pVPanelParent, "wxID_BUTTON_DEL_ATTR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DEL)));
  XRCCTRL(*m_pPanel_NewAttr, "wxID_ATTR_LABEL_NEW_ATTR", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_INSERT)));
  
  /* Retrieve the sizer for subpanels. */
  wxPanel* panelSubs = XRCCTRL(*m_pVPanelParent, "wxID_PANEL_SUBPANELS", wxPanel);
  if (panelSubs)
    {
      m_pSubpanelSizer = panelSubs->GetSizer();
      if (m_pSubpanelSizer)
        {
          /* Init the array of embed subpanels */
          int i;
          for (i = 0; i < wxATTR_PANEID_MAX; i++)
            {
              wxClassInfo* ci = wxClassInfo::FindClass(s_subpanelClassNames[i]);
              if (ci)
                m_subpanels[i] = wxDynamicCast(ci->CreateObject(), AmayaAttributeSubpanel);
              if (m_subpanels[i])
                {
                  m_subpanels[i]->Create(panelSubs, -1);
                  m_subpanels[i]->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_VALUE_OF_ATTR)));
                  m_pSubpanelSizer->Add(m_subpanels[i], 1, wxEXPAND)->Show(false);
                }
            }
          m_pSubpanelSizer->Layout();
        }
    }
  m_pAttrList->SetColumnWidth(0, m_sz0);
  m_pAttrList->SetColumnWidth(1, m_sz1);
  SetupListValue(NULL);
  return true;
}

wxString AmayaAttributeToolPanel::GetToolPanelName()const
{
  return TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ATTR));
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAttributeToolPanel
 *      Method:  GetDefaultAUIConfig
 * Description:  Return a default AUI config for the panel.
 -----------------------------------------------------------------------*/
wxString AmayaAttributeToolPanel::GetDefaultAUIConfig()
{
  return wxT("dir=4;layer=0;row=0;pos=1");
}



/*----------------------------------------------------------------------
 *       Class:  AmayaAttributeToolPanel
 *      Method:  RedirectFocusToEditableControl
 * Description:  Redirect the focus to the currently edited control.
 -----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::RedirectFocusToEditableControl()
{
  if (m_pCurrentlyEditedControl)
    m_pCurrentlyEditedControl->SetFocus();
  else
    TtaRedirectFocus ();
}

/*----------------------------------------------------------------------
  UpdateListColumnWidth
  Recompute width of list columns
  ----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::UpdateListColumnWidth()
{
  // Resize columns.
  int sz0, sz1;

  m_pAttrList->Freeze();
  // m_pAttrList->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
  sz0 = m_pAttrList->GetColumnWidth(0);
  if (sz0 < 20)
    sz0 = 20;
  if (sz0 != m_sz0)
    {
      m_sz0 = sz0;
      TtaSetEnvInt ("ATTR_COL_0", m_sz0, TRUE);
    }
  sz1 = m_pAttrList->GetColumnWidth(1);
  if (sz1 < 20)
    sz1 = 20;
  if (sz1 != m_sz1)
    {
      m_sz1 = sz1;
      TtaSetEnvInt ("ATTR_COL_1", m_sz1, TRUE);
    }
  m_pAttrList->Thaw();
}

/*----------------------------------------------------------------------
  Check if the current attribute (if any) is mandatory or
  if can be modified
  returns: true if mandatory
  ----------------------------------------------------------------------*/
bool AmayaAttributeToolPanel::IsMandatory()const
{
  return (!m_currentAttElem||
          AttrListElem_IsMandatory(m_currentAttElem));
}

/*----------------------------------------------------------------------
  Check if the current attribute (if any) is read-only
  returns: true if read-only
  ----------------------------------------------------------------------*/
bool AmayaAttributeToolPanel::IsReadOnly()const
{
  return (!m_currentAttElem ||
          AttrListElem_IsReadOnly (m_currentAttElem));
}


/*----------------------------------------------------------------------
 * Show or hide the attribute bar, the bar where is shwon
 *  the attribute subpanel if any.
  ----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::ShowAttributeBar(bool bShow)
{
  if (m_pVPanelSizer)
    {
      m_pVPanelSizer->Show((size_t)0, bShow);
    }
}

/*----------------------------------------------------------------------
  SendDataToPanel refresh the attribute list or show the value panels
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::SendDataToPanel( AmayaParams& p )
{
  if (IsShown())
    {
      if ( (int)(p.param1) == wxATTR_ACTION_LISTUPDATE)
        {
          DesactivatePanel();
          m_firstSel  = (PtrElement)(p.param5);
          m_lastSel   = (PtrElement)(p.param6);
          m_firstChar = p.param7;
          m_lastChar  = p.param8;
          ShowAttributValue( wxATTR_PANEID_NONE );
          SetupListValue ((DLList)(p.param2));
          ActivePanel();
        }
    }
}

/*----------------------------------------------------------------------
  Analyse elem param to find correct internal type.
  ----------------------------------------------------------------------*/
AmayaAttributeToolPanel::wxATTR_INTTYPE AmayaAttributeToolPanel::GetInternalTypeFromAttrElem(PtrAttrListElem elem)
{
  PtrTtAttribute  pAttr = AttrListElem_GetTtAttribute(elem);
  if (elem && pAttr)
    {
      switch(pAttr->AttrType)
      {
        case AtReferenceAttr:
          return wxATTR_INTTYPE_REF;
        case AtEnumAttr:
          return wxATTR_INTTYPE_ENUM;
        case AtNumAttr:
          return wxATTR_INTTYPE_NUM;
        case AtTextAttr:
          if (elem->num == 1)
            return wxATTR_INTTYPE_LANG;
          else
            return wxATTR_INTTYPE_TEXT;
        default:
          break;
      }
    }
  return wxATTR_INTTYPE_NONE;
}

/*----------------------------------------------------------------------
  SelectAttribute
  select an attribut to the given position
  ----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::SelectAttribute(int position)
{
  wxATTR_INTTYPE  inttype;
  if (position!=wxID_ANY)
    {
      m_currentAttElem = (PtrAttrListElem)m_pAttrList->GetItemData(position);
      
      inttype = GetInternalTypeFromAttrElem(m_currentAttElem);
      
      if (inttype==wxATTR_INTTYPE_REF)
        {
          // Reference has a special behavior
          CallbackEditRefAttribute(m_currentAttElem, TtaGiveActiveFrame());
        }
      else if (inttype==wxATTR_INTTYPE_LANG)
        {
          // Lang is always lang
          SetupAttr(m_currentAttElem, wxATTR_PANEID_LANG);
          return;
        }
      else if (inttype==wxATTR_INTTYPE_ENUM)
        {
          // Enum is always enum
          SetupAttr(m_currentAttElem, wxATTR_PANEID_ENUM);
          return;          
        }
      else if (inttype!=wxATTR_INTTYPE_NONE)
        {
          if (AttrListElem_IsEnum(m_currentAttElem))
	    {
              /* all type restricted to enum. */
              SetupAttr(m_currentAttElem, wxATTR_PANEID_ENUM);
	      return;          
	    }
          else if (s_subpanelAssoc[m_currentAttElem->restr.RestrType][inttype]!=wxATTR_PANEID_NONE)
            {
              SetupAttr(m_currentAttElem, s_subpanelAssoc[m_currentAttElem->restr.RestrType][inttype]);
              return;          
            }
        }
    }
  m_currentAttElem = NULL;
  ShowAttributValue( wxATTR_PANEID_NONE );
}

/*----------------------------------------------------------------------
  SetupAttr
  Set the correct attribute type panel for an attribute
  ----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::SetupAttr(PtrAttrListElem elem, wxATTR_PANEID type)
{
  ShowAttributValue(type);
  m_subpanels[type]->SetSelectionPosition(m_firstSel, m_lastSel, 
                                                  m_firstChar, m_lastChar);
  m_subpanels[type]->SetAttrListElem(elem);
  m_currentPane = type;
}


/*----------------------------------------------------------------------
  QueryRemoveCurrentAttribute
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::QueryRemoveCurrentAttribute()
{
  if (!IsMandatory())
    RemoveCurrentAttribute();
  else
    /* this attribut is mandatory, user is not allowed to remove it ! */
    TtaDisplaySimpleMessage (INFO, LIB, TMSG_ATTR_MANDATORY);
}

/*----------------------------------------------------------------------
  RemoveCurrentAttribut
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::RemoveCurrentAttribute()
{
  PtrElement      pEl;
  Attribute       attr;
  AttributeType   attrType;
  NotifyAttribute notifyAttr;
  int             kind;

  Document     doc = TtaGetDocument((Element)m_firstSel);
  DisplayMode  mode = TtaGetDisplayMode(doc);
  
  DesactivatePanel();

  if (m_currentPane != wxATTR_PANEID_NONE && m_firstSel &&
      m_currentAttElem && m_currentAttElem->val)
    {
      TtaSetDisplayMode(doc, DeferredDisplay);
      TtaOpenUndoSequence(doc, (Element)m_firstSel, (Element)m_lastSel,
                          m_firstChar, m_lastChar);

      /* first selected element */
      pEl = (PtrElement)m_firstSel;
      attr = (Attribute)m_currentAttElem->val;
      TtaGiveAttributeType (attr, &attrType, &kind);
      /* prepare event AttrDelete to be sent to the application */
      notifyAttr.event = TteAttrDelete;
      notifyAttr.document = doc;
      notifyAttr.info = 0; /* not sent by undo */
      notifyAttr.attributeType = attrType;
      while (pEl != NULL)
        {
          if (attr)
            {
              notifyAttr.element = (Element) pEl;
              notifyAttr.attribute = attr;
              if (!CallEventAttribute (&notifyAttr, TRUE))
                /* application accepts */
                {
                  TtaRegisterAttributeDelete(attr, (Element)pEl, doc);
                  TtaRemoveAttribute((Element)pEl, attr, doc);
                  TtaSetDocumentModified(doc);
                  /* send event Attribute deleted */
                  notifyAttr.attribute = NULL;
                  CallEventAttribute (&notifyAttr, FALSE);
                }
            }
          /* next element in the selection */
          pEl = NextInSelection (pEl, (PtrElement)m_lastSel);
          attr = TtaGetAttribute ((Element)pEl, attrType);
        }

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
void AmayaAttributeToolPanel::CreateCurrentAttribute()
{
  wxString        name;
  long            index;
  PtrTtAttribute  pAttr;
  PtrAttrListElem elem;

  if (m_pNewAttrChoice->GetSelection()!=wxNOT_FOUND)
    {
      elem = (PtrAttrListElem)
            m_pNewAttrChoice->GetClientData(m_pNewAttrChoice->GetSelection());
      if (elem)
        {
          pAttr = AttrListElem_GetTtAttribute(elem);
          if (pAttr && pAttr->AttrType == AtEnumAttr &&
              pAttr->AttrNEnumValues == 1)
            {
              SetAttrValueToRange (elem, (intptr_t)1);
              ForceAttributeUpdate();
            }
          else
            {
              index = m_pAttrList->InsertItem(m_pAttrList->GetItemCount(),
                                              TtaConvMessageToWX(AttrListElem_GetName(elem)));
              elem->restr.RestrFlags |= attr_new;
              m_pAttrList->SetItemData(index, (long)elem);
              m_pAttrList->SetItemTextColour(index, COLOR_NEW);
              m_pAttrList->SetItemState(index,
                                        wxLIST_STATE_FOCUSED|wxLIST_STATE_SELECTED,
                                        wxLIST_STATE_FOCUSED|wxLIST_STATE_SELECTED);
              m_pAttrList->EnsureVisible(index);
              SelectAttribute(index);
            }
        }
    }
  RedirectFocusToEditableControl();
}


/*----------------------------------------------------------------------
  ShowAttributValue
  show the right panel type and initialize it
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::ShowAttributValue( wxATTR_PANEID type )
{
  m_currentPane = type;

  int i;
  for (i = 0; i < wxATTR_PANEID_MAX; i++)
    m_pSubpanelSizer->Show(m_subpanels[i], false );
  
  if (type >= 0 && type < wxATTR_PANEID_MAX)
    {
      m_pSubpanelSizer->Show( type, true );
      m_pCurrentlyEditedControl = m_subpanels[type]->GetEditionControl();
      m_subpanels[type]->Refresh();
      m_pSubpanelSizer->Layout();
      ShowAttributeBar(true);
    }
  else if (m_pCurrentlyEditedControl)
    {
      m_pCurrentlyEditedControl = NULL;
      ShowAttributeBar(false);
    }
  Layout();
  
  RedirectFocusToEditableControl();
}


/*----------------------------------------------------------------------
  SetupListValue
  init the attribut list
  ----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::SetupListValue(DLList attrList)
{
  ForwardIterator iter;
  DLListNode      node;
  PtrAttrListElem elem;
  TtAttribute    *pAttr;
  long            index;
  char            buffer[MAX_TXT_LEN];
  int             size;
  AttributeType   type;

  m_pAttrList->DeleteAllItems();
  m_pNewAttrChoice->Clear();
  
  m_pVPanelSizer->Hide((size_t)0);
  
  if (m_attrList)
    DLList_Destroy(m_attrList);
  m_attrList = attrList;
  if (attrList)
    {
      iter = DLList_GetForwardIterator(attrList);
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
                    m_pAttrList->SetItem(index, 1, TtaConvMessageToWX(buffer));
                    break;
                  case AtEnumAttr:
                    type.AttrSSchema = (int*) elem->pSS;
                    type.AttrTypeNum = elem->num;
                    pAttr = AttrListElem_GetTtAttribute(elem);
                    if (pAttr->AttrNEnumValues == 1 &&
                        !strcasecmp (pAttr->AttrEnumValue[0], "yes"))
                      // this is a boolean value
                      m_pAttrList->SetItem(index, 1, TtaConvMessageToWX(pAttr->AttrName));
                     else
                     m_pAttrList->SetItem(index, 1, TtaConvMessageToWX(
                            TtaGetAttributeValueName(type, 
                                TtaGetAttributeValue((Attribute)elem->val))));
                    break;
                  case AtReferenceAttr:
                  default:
                    break;
                }
                if (AttrListElem_IsNew(elem))
                  m_pAttrList->SetItemTextColour(index, COLOR_NEW);
                else if (AttrListElem_IsReadOnly(elem))
                  m_pAttrList->SetItemTextColour(index, COLOR_READONLY);
                else if (AttrListElem_IsMandatory(elem))
                  m_pAttrList->SetItemTextColour(index, COLOR_MANDATORY);
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
  UpdateListColumnWidth();
}


/*----------------------------------------------------------------------
  OnDelAttr called when the user want to remove an existing attribut (the selected one)
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::OnDelAttr( wxCommandEvent& event )
{
  RemoveCurrentAttribute();
}

/*----------------------------------------------------------------------
  Update the value of an attribute in the list.
  Modify the content of the list without update it completely.
  ----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::ModifyListAttrValue(const wxString& attrName,
                                                       const wxString& attrVal)
{
  long index = m_pAttrList->FindItem(wxID_ANY, attrName);
  if (index!=wxNOT_FOUND)
    {
      m_pAttrList->SetItem(index, 1, attrVal);
      UpdateListColumnWidth();
    }
}

/*----------------------------------------------------------------------
  Retrieve the name of the currently selected attribute in the list.
  ----------------------------------------------------------------------*/
wxString AmayaAttributeToolPanel::GetCurrentSelectedAttrName()const
{
  long index = m_pAttrList->GetNextItem(wxID_ANY, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  if (index != wxNOT_FOUND)
    return m_pAttrList->GetItemText(index);
  else
    return wxT("");
}

/*----------------------------------------------------------------------
  OnApply called when the user want to change the attribute value
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::OnApply( wxCommandEvent& event )
{
  PtrTtAttribute  pAttr;
  char            buffer[MAX_LENGTH];
  Document        doc;
  wxString        value;

  if (m_currentAttElem && m_firstSel)
    {
      doc = TtaGetDocument((Element)m_firstSel);
      pAttr = AttrListElem_GetTtAttribute(m_currentAttElem);
      if (pAttr->AttrType==AtEnumAttr || pAttr->AttrType==AtNumAttr)
        {
          int test = m_subpanels[m_currentPane]->GetIntValue();
          value = m_subpanels[m_currentPane]->GetStringValue();
          SetAttrValueToRange(m_currentAttElem, (intptr_t)test);
        }
      else if (pAttr->AttrType==AtTextAttr)
        {
          value = m_subpanels[m_currentPane]->GetStringValue();
          strncpy (buffer, (const char*)value.mb_str(wxConvUTF8), MAX_LENGTH-1);
          SetAttrValueToRange(m_currentAttElem, (intptr_t)buffer);
        }
      ModifyListAttrValue(GetCurrentSelectedAttrName(), value);
    }
  m_currentAttElem = NULL;
  
  RedirectFocusToEditableControl();
}

/*----------------------------------------------------------------------
  ForceAttributeUpdate force the current document to refresh the attribute list
  => UpdateAttrMenu is called => AmayaAttributeToolPanel::UpdateAttributeList is finaly called
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::ForceAttributeUpdate()
{
  /* do the update */
  PtrDocument pDoc;
  int         view;

  GetDocAndView (TtaGiveActiveFrame(), &pDoc, &view);
  if (pDoc)
    UpdateAttrMenu (pDoc, TRUE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaAttributeToolPanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
 -----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::DoUpdate()
{
  AmayaToolPanel::DoUpdate();
  ForceAttributeUpdate();  
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::OnListItemSelected(wxListEvent& event)
{
  if (IsPanelActive())
    SelectAttribute(event.GetIndex());
  event.Skip();
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::OnListItemDeselected(wxListEvent& event)
{
  if (IsPanelActive())
    SelectAttribute(wxID_ANY);
  event.Skip();
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::OnInsert( wxCommandEvent& WXUNUSED(event))
{
  CreateCurrentAttribute();
}

/*----------------------------------------------------------------------
 -----------------------------------------------------------------------*/
void AmayaAttributeToolPanel::OnUpdateDeleteButton(wxUpdateUIEvent& event)
{
  
  if (IsPanelActive() && m_currentAttElem && m_currentPane!=wxATTR_PANEID_NONE)
    event.Enable(!IsMandatory());
  else
    event.Enable(false);
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaAttributeToolPanel, AmayaToolPanel)
  EVT_LIST_ITEM_SELECTED(XRCID("wxID_CLIST_ATTR"), AmayaAttributeToolPanel::OnListItemSelected)
  EVT_LIST_ITEM_DESELECTED(XRCID("wxID_CLIST_ATTR"), AmayaAttributeToolPanel::OnListItemDeselected)
  
  EVT_TEXT_ENTER( XRCID("wxID_ATTR_TEXT_VALUE"),      AmayaAttributeToolPanel::OnApply )
  EVT_TEXT_ENTER( XRCID("wxID_ATTR_NUM_VALUE"),       AmayaAttributeToolPanel::OnApply )

  EVT_CHOICE(XRCID("wxID_ATTR_COMBO_LANG_LIST"), AmayaAttributeToolPanel::OnApply)
  EVT_CHOICE(XRCID("wxID_ATTR_CHOICE_ENUM"), AmayaAttributeToolPanel::OnApply)

  EVT_BUTTON(     XRCID("wxID_OK"),              AmayaAttributeToolPanel::OnApply )
  
  EVT_BUTTON(     XRCID("wxID_BUTTON_DEL_ATTR"), AmayaAttributeToolPanel::OnDelAttr )
  EVT_CHOICE(XRCID("wxID_CHOOSE_NEW_ATTRIBUTE"), AmayaAttributeToolPanel::OnInsert)

  EVT_UPDATE_UI(  XRCID("wxID_BUTTON_DEL_ATTR"), AmayaAttributeToolPanel::OnUpdateDeleteButton)
END_EVENT_TABLE()



/************************************************************************
 ************************************************************************
 * AmayaAttributeSubpanel
 ************************************************************************
 ************************************************************************/

IMPLEMENT_ABSTRACT_CLASS(AmayaAttributeSubpanel, wxPanel)
BEGIN_EVENT_TABLE(AmayaAttributeSubpanel, wxPanel)
END_EVENT_TABLE()

AmayaAttributeSubpanel::AmayaAttributeSubpanel():
wxPanel()
{
}

AmayaAttributeSubpanel::~AmayaAttributeSubpanel()
{
}

bool AmayaAttributeSubpanel::SetSelectionPosition(PtrElement firstSel, PtrElement lastSel,
                                  int firstChar, int lastChar)
{
  m_firstSel  = firstSel;
  m_lastSel   = lastSel;
  m_firstChar = firstChar;
  m_lastChar  = lastChar;
  return true;
}

void AmayaAttributeSubpanel::SendApplyInfoToParent(wxCommandEvent& event)
{
  wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK);
  ProcessEvent(evt);
}

wxString AmayaAttributeSubpanel::getAttributeStringValue(PtrAttrListElem elem)
{
  char              buffer[MAX_TXT_LEN];
  int               i = MAX_TXT_LEN-1;
  char             *tmp;

  wxString str;
  
   if (elem && elem->val && elem->val->AeAttrText)
    {
      i = CopyBuffer2MBs (elem->val->AeAttrText, 0, (unsigned char*)buffer, i);
      tmp = (char *)TtaConvertMbsToByte ((unsigned char *)buffer, TtaGetDefaultCharset ());
      str = TtaConvMessageToWX( tmp );
      TtaFreeMemory (tmp);
      return str;
    }
   return wxT("");
}

int AmayaAttributeSubpanel::getAttributeNumericValue(PtrAttrListElem elem)
{
  if (elem && elem->val)
      return elem->val->AeAttrValue;
  return 0;
}

void AmayaAttributeSubpanel::getAttributeEnumValues(PtrAttrListElem elem, wxArrayString& arr)
{
  TtAttribute      *pAttr = AttrListElem_GetTtAttribute(elem);
  int val;
  if (elem && elem->val)
    {
      if (pAttr->AttrNEnumValues == 1 &&
          !strcasecmp (pAttr->AttrEnumValue[0], "yes"))
        {
          // this is a boolean value
          arr.Add(wxString(pAttr->AttrName, wxConvUTF8));
        }
      else
        for (val = 0; val < pAttr->AttrNEnumValues; val++)
          arr.Add(wxString(pAttr->AttrEnumValue[val], wxConvUTF8));
    }
}


/************************************************************************
 ************************************************************************
 * AmayaEnumAttributeSubpanel
 ************************************************************************
 ************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(AmayaEnumAttributeSubpanel, AmayaAttributeSubpanel)

BEGIN_EVENT_TABLE(AmayaEnumAttributeSubpanel, AmayaAttributeSubpanel)
  EVT_CHOICE(XRCID("wxID_ATTR_CHOICE_ENUM"), AmayaAttributeSubpanel::SendApplyInfoToParent)
END_EVENT_TABLE()


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaEnumAttributeSubpanel::AmayaEnumAttributeSubpanel():
AmayaAttributeSubpanel(),
m_pChoice(NULL)
{  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaEnumAttributeSubpanel::~AmayaEnumAttributeSubpanel()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaEnumAttributeSubpanel::Create(wxWindow* parent, wxWindowID id)
{
  wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent,
                                          wxT("wxID_SUBPANEL_ATTRIBUTE_ENUM"));
  m_pChoice = XRCCTRL(*this, "wxID_ATTR_CHOICE_ENUM", wxChoice);
  return true;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaEnumAttributeSubpanel::SetAttrListElem(PtrAttrListElem elem)
{
  TtAttribute      *pAttr;
  wxStringTokenizer tkz;
  wxArrayString     arr;
  wxString          str, def;
  int               index, val;

  m_pChoice->Clear();
  if (elem)
    {
      m_type = (AmayaAttributeToolPanel::wxATTR_INTTYPE)AttrListElem_GetType(elem);

      if (AttrListElem_GetType(elem)==AtEnumAttr)
        {
          pAttr = AttrListElem_GetTtAttribute(elem);
          // Std enumeration
          if (pAttr->AttrNEnumValues == 1 &&
              !strcasecmp (pAttr->AttrEnumValue[0], "yes"))
            {
              // this is a boolean value
              m_pChoice->Append(TtaConvMessageToWX(pAttr->AttrName));
              m_pChoice->SetSelection(0);
            }
          else
            {
              for (val = 0; val < pAttr->AttrNEnumValues; val++)
                {
                  index = wxNOT_FOUND;
                  if (AttrListElem_IsEnum(elem) && elem->restr.RestrEnumVal)
                    {
                      if (strstr(elem->restr.RestrEnumVal, pAttr->AttrEnumValue[val]))
                          index = m_pChoice->Append(TtaConvMessageToWX(pAttr->AttrEnumValue[val]));
                    }
                  else
                      index = m_pChoice->Append(TtaConvMessageToWX(pAttr->AttrEnumValue[val]));
                  if (index!=wxNOT_FOUND)
                    m_pChoice->SetClientData(index, (void*)val);
                }
              /* current value */
              if (elem->val && elem->val->AeAttrValue > 0)
                m_pChoice->SetSelection(elem->val->AeAttrValue-1);
              else
                m_pChoice->SetStringSelection(TtaConvMessageToWX(elem->restr.RestrDefVal));
            }
        }
      else if (AttrListElem_IsEnum(elem) && elem->restr.RestrEnumVal)
        {
          // Restricted enumeration
          tkz.SetString(TtaConvMessageToWX(elem->restr.RestrEnumVal), wxT(" "));
          while (tkz.HasMoreTokens())
              m_pChoice->Append(tkz.GetNextToken());
          
          if (elem->val)
            {
              if (AttrListElem_GetType(elem)==AtNumAttr)
                {
                  index = AmayaAttributeSubpanel::getAttributeNumericValue(elem);
                  m_pChoice->SetStringSelection(wxString::Format(wxT("%d"), index));
                }
              else if (AttrListElem_GetType(elem)==AtTextAttr)
                {
                  str = AmayaAttributeSubpanel::getAttributeStringValue(elem);
                  m_pChoice->SetStringSelection(str);
                }
            }
          else
            m_pChoice->SetStringSelection(TtaConvMessageToWX(elem->restr.RestrDefVal));
        }
      return true;
    }
  else
    {
      m_type = AmayaAttributeToolPanel::wxATTR_INTTYPE_MAX;
      return false;
    }
}

wxString AmayaEnumAttributeSubpanel::GetStringValue()
{
  return m_pChoice->GetStringSelection();
}

intptr_t AmayaEnumAttributeSubpanel::GetIntValue()
{
  if (m_type==AmayaAttributeToolPanel::wxATTR_INTTYPE_ENUM)
    {
      return (intptr_t)m_pChoice->GetClientData(m_pChoice->GetSelection())+1;
    }
  else if (m_type==AmayaAttributeToolPanel::wxATTR_INTTYPE_NUM)
    {
      long i;
      m_pChoice->GetStringSelection().ToLong(&i);
      return i;
    }
  else
    return 0;
}


/************************************************************************
 ************************************************************************
 * AmayaStringAttributeSubpanel
 ************************************************************************
 ************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(AmayaStringAttributeSubpanel, AmayaAttributeSubpanel)

BEGIN_EVENT_TABLE(AmayaStringAttributeSubpanel, AmayaAttributeSubpanel)
  EVT_TEXT_ENTER(XRCID("wxID_ATTR_TEXT_VALUE"), AmayaAttributeSubpanel::SendApplyInfoToParent)
  EVT_TEXT_ENTER(XRCID("wxID_ATTR_COMBO_VALUE"), AmayaAttributeSubpanel::SendApplyInfoToParent)
END_EVENT_TABLE()


AmayaStringAttributeSubpanel::AmayaStringAttributeSubpanel():
AmayaAttributeSubpanel(),
m_hasDefaults(false),
m_pText(NULL),
m_pCombo(NULL)
{
}

AmayaStringAttributeSubpanel::~AmayaStringAttributeSubpanel()
{
}

bool AmayaStringAttributeSubpanel::Create(wxWindow* parent, wxWindowID id)
{
  wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent,
                                        wxT("wxID_SUBPANEL_ATTRIBUTE_STRING"));
  m_pText  = XRCCTRL(*this, "wxID_ATTR_TEXT_VALUE", wxTextCtrl);
  m_pCombo = XRCCTRL(*this, "wxID_ATTR_COMBO_VALUE", wxComboBox);
  
  XRCCTRL(*this, "wxID_OK", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  return true;
}

bool AmayaStringAttributeSubpanel::SetAttrListElem(PtrAttrListElem elem)
{
  wxString value;
  
  if (elem)
    {
      if (AttrListElem_IsNew(elem) && elem->restr.RestrDefVal)
        value = TtaConvMessageToWX(elem->restr.RestrDefVal);
      else if (elem->val && elem->val->AeAttrText)
        value = AmayaAttributeSubpanel::getAttributeStringValue(elem);

      if (elem->restr.RestrEnumVal!=NULL && elem->restr.RestrEnumVal[0]!=EOS)
        {
          m_hasDefaults = true;
          m_pText->Hide();
          m_pCombo->Show();
          m_pCombo->Clear();
          m_pCombo->Append(wxStringTokenize(TtaConvMessageToWX(elem->restr.RestrEnumVal)));
          m_pCombo->SetValue(value);
          m_pCombo->SetInsertionPointEnd();
          GetSizer()->Show(m_pText, false);
          GetSizer()->Show(m_pCombo, true);
        }
      else
        {
          m_hasDefaults = false;
          m_pText->Show();
          m_pCombo->Hide();
          m_pText->SetValue(value);
          m_pText->SetInsertionPointEnd();
          GetSizer()->Show(m_pText, true);
          GetSizer()->Show(m_pCombo, false);
        }
      GetSizer()->Layout();
    }
  return true;
}

wxString AmayaStringAttributeSubpanel::GetStringValue()
{
  return m_hasDefaults?m_pCombo->GetValue():m_pText->GetValue();
}




/************************************************************************
 ************************************************************************
 * AmayaNumAttributeSubpanel
 ************************************************************************
 ************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(AmayaNumAttributeSubpanel, AmayaAttributeSubpanel)

BEGIN_EVENT_TABLE(AmayaNumAttributeSubpanel, AmayaAttributeSubpanel)
  EVT_TEXT_ENTER(XRCID("wxID_ATTR_NUM_VALUE"), AmayaAttributeSubpanel::SendApplyInfoToParent)
END_EVENT_TABLE()


AmayaNumAttributeSubpanel::AmayaNumAttributeSubpanel():
AmayaAttributeSubpanel(),
m_pSpin(NULL)
{  
}

AmayaNumAttributeSubpanel::~AmayaNumAttributeSubpanel()
{
}

bool AmayaNumAttributeSubpanel::Create(wxWindow* parent, wxWindowID id)
{
  wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent,
                                          wxT("wxID_SUBPANEL_ATTRIBUTE_NUM"));
  m_pSpin = XRCCTRL(*this, "wxID_ATTR_NUM_VALUE", wxSpinCtrl);
  XRCCTRL(*this, "wxID_OK", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  return true;
}

bool AmayaNumAttributeSubpanel::SetAttrListElem(PtrAttrListElem elem)
{
  long               i=0, begin=0, end=1000;
  char*             title;
  TtAttribute      *pAttr = AttrListElem_GetTtAttribute(elem);
  wxString          str;

  if (elem)
    {
      if (AttrListElem_IsNew(elem) && elem->restr.RestrDefVal)
        {
          str = TtaConvMessageToWX(elem->restr.RestrDefVal);
          str.ToLong(&i);
        }

      if (elem->val)
          i     = AmayaAttributeSubpanel::getAttributeNumericValue(elem);
      if (pAttr)
        {
          title = pAttr->AttrName;
          if (!strcmp (elem->pSS->SsName, "HTML") &&
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
        }
    }
  
  m_pSpin->SetRange(begin, end);
  m_pSpin->SetValue(i);
  return true;
}

wxString AmayaNumAttributeSubpanel::GetStringValue()
{
  return wxString::Format(wxT("%d"), m_pSpin->GetValue());
}


intptr_t AmayaNumAttributeSubpanel::GetIntValue()
{
  return m_pSpin->GetValue();
}

/************************************************************************
 ************************************************************************
 * AmayaLangAttributeSubpanel
 ************************************************************************
 ************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(AmayaLangAttributeSubpanel, AmayaAttributeSubpanel)

BEGIN_EVENT_TABLE(AmayaLangAttributeSubpanel, AmayaAttributeSubpanel)
  EVT_TEXT_ENTER(XRCID("wxID_ATTR_COMBO_LANG_LIST"), AmayaAttributeSubpanel::SendApplyInfoToParent)
END_EVENT_TABLE()

wxArrayString AmayaLangAttributeSubpanel::s_arrLangs;

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaLangAttributeSubpanel::AmayaLangAttributeSubpanel():
AmayaAttributeSubpanel(),
m_pCombo(NULL),
m_pText(NULL)
{  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaLangAttributeSubpanel::~AmayaLangAttributeSubpanel()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaLangAttributeSubpanel::Create(wxWindow* parent, wxWindowID id)
{
  wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, 
                                        wxT("wxID_SUBPANEL_ATTRIBUTE_LANG"));
  m_pCombo = XRCCTRL(*this, "wxID_ATTR_COMBO_LANG_LIST", wxChoice);
  m_pText = XRCCTRL(*this, "wxID_ATTR_LABEL_LANG_INHER", wxStaticText);
  return true;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaLangAttributeSubpanel::Initialize()
{
  char                lang_list[MAX_TXT_LEN];
  int                 nbItem, defItem, i;

  if (s_arrLangs.IsEmpty())
    {
      // Fill the static array of languages.
      GetListOfLanguages (lang_list, MAX_TXT_LEN, NULL, &nbItem, &defItem);
      i = 0;
      while (nbItem > 0)
        {
          s_arrLangs.Add(TtaConvMessageToWX(lang_list+i));
          i += strlen(lang_list+i)+1;
          nbItem--;
        }
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaLangAttributeSubpanel::SetAttrListElem(PtrAttrListElem elem)
{
  Language            language;
  PtrAttribute        pHeritAttr;
  PtrElement          pElAttr;
  char                languageCode[MAX_TXT_LEN];
  int                 i, max;
  wxString            filter, code, lang, herit, str;
  
  
  Initialize();
  
  if (elem)
    {
      /* Fill the wxChoice. */
      m_pCombo->Clear();
      if (AttrListElem_IsEnum(elem))
        {
          filter = TtaConvMessageToWX(elem->restr.RestrEnumVal);
          max = (int)s_arrLangs.GetCount();
          for (i = 0; i < max; i++)
            {
              if (filter.Find(s_arrLangs[i]) != wxNOT_FOUND)
                m_pCombo->Append(s_arrLangs[i]);
            }
        }
      else
          m_pCombo->Append(s_arrLangs);
      
      /* Herited language. */
      /* look for the inherited attribute value Language */
      str = TtaConvMessageToWX(TtaGetMessage (LIB, TMSG_INHERITED_LANG));
      pHeritAttr = GetTypedAttrAncestor (m_firstSel, 1, NULL, &pElAttr);
      if (pHeritAttr && pHeritAttr->AeAttrText)
        {
          /* the attribute value is a RFC-1766 code. Convert it into */
          /* a language name */
          CopyBuffer2MBs (pHeritAttr->AeAttrText, 0,
                          (unsigned char*)languageCode, MAX_TXT_LEN);
          language = TtaGetLanguageIdFromName (languageCode);
          herit = TtaConvMessageToWX(TtaGetLanguageName(language));
        }
      m_pText->SetLabel(str+herit);
      
      /* Current language. */
      if (elem->val && elem->val->AeAttrText)
        {
          CopyBuffer2MBs (elem->val->AeAttrText, 0, (unsigned char*)languageCode,
                        MAX_TXT_LEN);
          lang = TtaConvMessageToWX(TtaGetLanguageNameFromCode(languageCode));
        }
      else if (elem->restr.RestrDefVal)
        lang = TtaConvMessageToWX(TtaGetLanguageNameFromCode(elem->restr.RestrDefVal));
      else
        lang = herit;
      m_pCombo->SetStringSelection(herit);
    }
  return true;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxString AmayaLangAttributeSubpanel::GetStringValue()
{
  return TtaConvMessageToWX(TtaGetLanguageCodeFromName((char*)
            (const char*)m_pCombo->GetStringSelection().mb_str(wxConvUTF8)));
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
intptr_t AmayaLangAttributeSubpanel::GetIntValue()
{
  return (int)TtaGetLanguageIdFromName((char*)
            (const char*)m_pCombo->GetStringSelection().mb_str(wxConvUTF8));
}

#endif /* #ifdef _WX */
