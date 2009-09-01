/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/string.h"
#include "wx/dynarray.h"

#include "AmayaApp.h"
#include "StyleListToolPanel.h"

#define THOT_EXPORT extern
#include "amaya.h"
#include "init_f.h"
#include "appdialogue_wx.h"
#include "message_wx.h"

#include "css.h"
#include "UIcss_f.h"

static const char       *DisplayCategory[]={
  "[x] " /*CSS_Unknown*/,
  "[U] " /*CSS_USER_STYLE*/,
  "[S] " /*CSS_DOCUMENT_STYLE*/,
  "[E] " /*CSS_EXTERNAL_STYLE*/,
  "[I] " /*CSS_IMPORT*/,
  "[ ] " /*CSS_EMBED*/
};

//
//
// StyleListToolPanel
//
//

IMPLEMENT_DYNAMIC_CLASS(StyleListToolPanel, AmayaToolPanel)

BEGIN_EVENT_TABLE(StyleListToolPanel, AmayaToolPanel)
  EVT_CHECKLISTBOX(XRCID("wxID_CHECKLIST_CSS"), StyleListToolPanel::OnCheckSheet)

  EVT_BUTTON(XRCID("wxID_BUTTON_ADD_CSS"), StyleListToolPanel::OnAddSheet)
  EVT_BUTTON(XRCID("wxID_BUTTON_DEL_CSS"), StyleListToolPanel::OnRemSheet)
  EVT_BUTTON(XRCID("wxID_BUTTON_ACTIVE_CSS"), StyleListToolPanel::OnActivateSheet)
  EVT_BUTTON(XRCID("wxID_BUTTON_DESACTIVE_CSS"), StyleListToolPanel::OnDesactivateSheet)
  EVT_BUTTON(XRCID("wxID_BUTTON_VIEW_CSS"), StyleListToolPanel::OnShowSheet)

  EVT_UPDATE_UI(XRCID("wxID_BUTTON_ADD_CSS"), StyleListToolPanel::OnUpdateAddSheet)
  EVT_UPDATE_UI(XRCID("wxID_BUTTON_DEL_CSS"), StyleListToolPanel::OnUpdateRemSheet)
  EVT_UPDATE_UI(XRCID("wxID_BUTTON_ACTIVE_CSS"), StyleListToolPanel::OnUpdateActivateSheet)
  EVT_UPDATE_UI(XRCID("wxID_BUTTON_DESACTIVE_CSS"), StyleListToolPanel::OnUpdateDesactivateSheet)
  EVT_UPDATE_UI(XRCID("wxID_BUTTON_VIEW_CSS"), StyleListToolPanel::OnUpdateShowSheet)
  EVT_LISTBOX_DCLICK( XRCID("wxID_CHECKLIST_CSS"), StyleListToolPanel::OnOpenSheet)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
StyleListToolPanel::StyleListToolPanel():
  AmayaToolPanel()
{
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
StyleListToolPanel::~StyleListToolPanel()
{
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
bool StyleListToolPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("StyleListPanel")))
    return false;

#ifdef _WINDOWS
  SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#endif /* _WINDOWS */
  m_list = XRCCTRL(*this,"wxID_CHECKLIST_CSS",wxCheckListBox);
  
  XRCCTRL(*this,"wxID_BUTTON_ADD_CSS",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_PANEL_CSS_ADD)));
  XRCCTRL(*this,"wxID_BUTTON_DEL_CSS",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_PANEL_CSS_REM)));
  XRCCTRL(*this,"wxID_BUTTON_ACTIVE_CSS",wxBitmapButton)->Hide();
  XRCCTRL(*this,"wxID_BUTTON_DESACTIVE_CSS",wxBitmapButton)->Hide();
  //XRCCTRL(*this,"wxID_BUTTON_ACTIVE_CSS",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_PANEL_CSS_ACTIVE)));
  //XRCCTRL(*this,"wxID_BUTTON_DESACTIVE_CSS",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_PANEL_CSS_DISACTIVE)));
  XRCCTRL(*this,"wxID_BUTTON_VIEW_CSS",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(AMAYA,AM_PANEL_CSS_OPEN)));
  
  return true;
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
wxString StyleListToolPanel::GetToolPanelName()const
{
  return TtaConvMessageToWX(TtaGetMessage(AMAYA, AM_PANEL_STYLE_LIST));
}

/*----------------------------------------------------------------------
 *       Class:  StyleListToolPanel
 *      Method:  GetDefaultAUIConfig
 * Description:  Return a default AUI config for the panel.
 -----------------------------------------------------------------------*/
wxString StyleListToolPanel::GetDefaultAUIConfig()
{
  return wxT("");
}

/*----------------------------------------------------------------------
  SendDataToPanel refresh the attribute list or show the value panels
  params:
  returns:
  ----------------------------------------------------------------------*/
void StyleListToolPanel::SendDataToPanel( AmayaParams& params )
{
  Update((Document)params.param1);
}

/*----------------------------------------------------------------------
  Update
  ----------------------------------------------------------------------*/
void StyleListToolPanel::Update(Document doc)
{
  m_doc = doc;
  if(m_list)
    {
      m_list->Clear();
      m_map.clear();
      
      CSSInfoPtr          css;
      PInfoPtr            pInfo;
      char               *ptr, *localname = TtaGetMessage (AMAYA, AM_LOCAL_CSS);
      
      css = CSSList;
      while (css)
        {
          pInfo = css->infos[doc];
          while (pInfo)
            {
              if (pInfo)
                {
                  if (pInfo->PiCategory == CSS_DOCUMENT_STYLE)
                    ptr = localname;
                  else if (css->url == NULL && css->localName)
                    ptr = css->localName;
                  else
                    ptr = css->url;

                  wxString str = TtaConvMessageToWX(DisplayCategory[pInfo->PiCategory]) +
                  TtaConvMessageToWX(ptr);
                  
                  int item = m_list->Append(str);
                  m_map[item] = pInfo;
                  if(pInfo->PiEnabled)
                    m_list->Check(item);
                  
                }
              pInfo = pInfo->PiNext;
            }
          css = css->NextCSS;
        }

    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleListToolPanel::OnCheckSheet(wxCommandEvent& event)
{
  int item = event.GetSelection();
  if (item != wxNOT_FOUND)
    {
      PInfoPtr pInfo = m_map[item];
      if (pInfo)
        {
          if (m_list->IsChecked(item))
            MakeEnableCSS (m_doc, pInfo);
          else
            MakeDisableCSS (m_doc, pInfo);
        }
    }  
}

/*----------------------------------------------------------------------
  OnOpenSheet called when the user double click
  ----------------------------------------------------------------------*/
void StyleListToolPanel::OnOpenSheet( wxCommandEvent& event )
{
  Document   doc;
  int        view;

  int item = event.GetSelection();
  if (item != wxNOT_FOUND)
    {
      TtaGetActiveView (&doc, &view);
      PInfoPtr pInfo = m_map[item];
      if (pInfo &&
          (DocumentTypes[doc] != docSource || pInfo->PiCategory != CSS_DOCUMENT_STYLE))
        MakeOpenCSS(m_doc, pInfo);
    }
  TtaRedirectFocus();
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleListToolPanel::OnAddSheet(wxCommandEvent& event)
{
  Document   doc;
  int        view;

  TtaGetActiveView (&doc, &view);
  if (doc == 0 ||
      DocumentTypes[doc] == docSource || DocumentTypes[doc] == docCSS ||
      DocumentTypes[doc] == docText)
    return;
  LinkCSS(m_doc, 1);
  Update(m_doc);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleListToolPanel::OnRemSheet(wxCommandEvent& event)
{
  wxArrayPtrVoid infos;
  wxArrayInt     selections;
  Document       doc;
  int            view;

  TtaGetActiveView (&doc, &view);
  if (doc == 0 ||
      DocumentTypes[doc] == docSource || DocumentTypes[doc] == docCSS ||
      DocumentTypes[doc] == docText)
    return;

  int i, nb = m_list->GetSelections(selections);
  for (i = 0; i < nb; i++)
    {
      int item = selections[i];
      if (item != wxNOT_FOUND)
        {
          PInfoPtr pInfo = m_map[item];
          if (pInfo && pInfo->PiCategory != CSS_DOCUMENT_STYLE)
            infos.Add(pInfo);
        }
    }
  for (i = 0; i < (int)infos.GetCount(); i++)
    {
      /* register this element in the editing history */
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
      MakeRemoveCSS(m_doc, (PInfoPtr)infos[i]);
      TtaCloseUndoSequence (doc);
    }
  Update (m_doc);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleListToolPanel::OnActivateSheet(wxCommandEvent& event)
{
  wxArrayInt selections;
  Document   doc;
  int        view;

  TtaGetActiveView (&doc, &view);
  if (doc == 0 ||
      DocumentTypes[doc] == docSource || DocumentTypes[doc] == docCSS ||
      DocumentTypes[doc] == docText)
    return;
  int i, nb = m_list->GetSelections(selections);
  for (i = 0; i < nb; i++)
    {
      int item = selections[i];
      if (item != wxNOT_FOUND)
        {
          PInfoPtr pInfo = m_map[item];
          if (pInfo)
            MakeEnableCSS(m_doc, pInfo);
        }
    }
  Update(m_doc);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleListToolPanel::OnDesactivateSheet(wxCommandEvent& event)
{
  wxArrayInt selections;
  Document   doc;
  int        view;

  TtaGetActiveView (&doc, &view);
  if (doc == 0 ||
      DocumentTypes[doc] == docSource || DocumentTypes[doc] == docCSS ||
      DocumentTypes[doc] == docText)
    return;
  int i, nb = m_list->GetSelections(selections);
  for (i = 0; i < nb; i++)
    {
      int item = selections[i];
      if (item != wxNOT_FOUND)
        {
          PInfoPtr pInfo = m_map[item];
          if (pInfo)
            MakeDisableCSS (m_doc, pInfo);
        }
    }
  Update(m_doc);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleListToolPanel::OnShowSheet(wxCommandEvent& event)
{
  wxArrayPtrVoid infos;
  wxArrayInt     selections;
  Document       doc;
  int            view;

  TtaGetActiveView (&doc, &view);

  int i, nb = m_list->GetSelections(selections);
  for (i = 0; i < nb; i++)
    {
      int item = selections[i];
      if (item != wxNOT_FOUND)
        {
          PInfoPtr pInfo = m_map[item];
          if (pInfo &&
              (DocumentTypes[doc] != docSource || pInfo->PiCategory != CSS_DOCUMENT_STYLE))
            infos.Add (pInfo);
        }
    }
  for (i = 0; i < (int)infos.GetCount(); i++)
    MakeOpenCSS(m_doc, (PInfoPtr)infos[i]);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleListToolPanel::OnUpdateAddSheet(wxUpdateUIEvent& event)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleListToolPanel::OnUpdateRemSheet(wxUpdateUIEvent& event)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleListToolPanel:: OnUpdateActivateSheet(wxUpdateUIEvent& event)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleListToolPanel:: OnUpdateDesactivateSheet(wxUpdateUIEvent& event)
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void StyleListToolPanel::OnUpdateShowSheet(wxUpdateUIEvent& event)
{
}


#endif /* _WX */

