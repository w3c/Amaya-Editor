#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces


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
#include "message_wx.h"
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"
#include "panel.h"

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "panel_tv.h"

#include "AmayaMathMLPanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFloatingPanel.h"
#include "AmayaSubPanelManager.h"

#include "mathml_filtres.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaMathMLPanel, AmayaSubPanel)

typedef struct _XmlEntity
{
  char         *charName;      
  int           charCode;      
} XmlEntity;

MathMLEntityHash AmayaMathMLPanel::m_MathMLEntityHash;

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  AmayaMathMLPanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaMathMLPanel::AmayaMathMLPanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_MATHML") )
{
  wxLogDebug( _T("AmayaMathMLPanel::AmayaMathMLPanel") );

  //  m_pPanelContentDetach = XRCCTRL(*this, "wxID_PANEL_CONTENT_DETACH", wxPanel);

  // setup labels
  RefreshToolTips();
  m_pTitleText->SetLabel(TtaConvMessageToWX("Maths"/*TtaGetMessage(LIB,TMSG_MATHML)*/));



  m_pList = XRCCTRL(*this,"wxID_PANEL_MATH_LIST",wxComboBox);

  /*
  m_OffColour = XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_STRONG", wxBitmapButton)->GetBackgroundColour();
  m_OnColour  = wxColour(250, 200, 200);
  */

  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  ~AmayaMathMLPanel
 * Description:  destructor
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaMathMLPanel::~AmayaMathMLPanel()
{
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  GetPanelType
 * Description:  
 *--------------------------------------------------------------------------------------
 */
int AmayaMathMLPanel::GetPanelType()
{
  return WXAMAYA_PANEL_MATHML;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::RefreshToolTips()
{  
#if 0
  const char ** p_tooltip_array = PanelTable[WXAMAYA_PANEL_XHTML].Tooltip_Panel;
  XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_STRONG", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_STRONG]));
  XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_EMPH",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_EMPH])); 
  XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_CODE",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_CODE]));
  XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_H1",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_H1]));
  XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_H2",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_H2]));
  XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_H3",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_H3]));
  XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_BULLET", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_BULLET]));
  XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_NL",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_NL]));
  XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_DL",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_DL]));
  XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_IMG",    wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_IMG]));
  XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_LINK",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_LINK]));
  XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_TABLE",  wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_TABLE]));
#endif /* 0 */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButton
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButton( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButton") );

  //  TtaExecuteMenuAction ("CreateMath", doc, view);
  //  TtaExecuteMenuAction ("CreateMENCLOSE", doc, view);

  Document doc;
  View view;
  TtaGetActiveView( &doc, &view );

  int id       = event.GetId();
  int amaya_id = -1;

  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FENCE")) )
    TtaExecuteMenuAction ("CreateMROW", doc, view);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SQRT")) )
    TtaExecuteMenuAction ("CreateMSQRT", doc, view);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ROOT")) )
    TtaExecuteMenuAction ("CreateMROOT", doc, view);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FRAC")) )
    TtaExecuteMenuAction ("CreateMFRAC", doc, view);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MATRIX")) )
    TtaExecuteMenuAction ("CreateMTABLE", doc, view);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNDER")) )
    TtaExecuteMenuAction ("CreateMUNDER", doc, view);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVER")) )
    TtaExecuteMenuAction ("CreateMOVER", doc, view);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERUNDER")) )
    TtaExecuteMenuAction ("CreateMUNDEROVER", doc, view);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MSCRIPT")) )
    TtaExecuteMenuAction ("CreateMMULTISCRIPTS", doc, view);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUB")) )
    TtaExecuteMenuAction ("CreateMSUB", doc, view);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUP")) )
    TtaExecuteMenuAction ("CreateMSUP", doc, view);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUBSUP")) )
    TtaExecuteMenuAction ("CreateMSUBSUP", doc, view);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre1
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre1( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre1") );
  DoFilter( filtre_greek );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre2
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre2( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre2") );
  DoFilter( filtre_greek_maj );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre3
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre3( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre3") );
  DoFilter( filtre_maths );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre4
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre4( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre4") );
  DoFilter( filtre_operateurs );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre5
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre5( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre5") );
  DoFilter( filtre_relations_binaires );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre6
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre6( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre6") );
  DoFilter( filtre_relations_binaires_negation );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButtonFiltre7
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButtonFiltre7( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaMathMLPanel::OnButtonFiltre7") );
  DoFilter( filtre_divers );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  DoFilter
 * Description:  this method is called to refresh the entity list with a given filter
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::DoFilter( int * filtre )
{
  m_pList->Clear();
  int element_id = 0;
  while ( filtre[element_id] != -1 )
    {
      wxString & entity_name = m_MathMLEntityHash[filtre[element_id]];
      m_pList->Append( entity_name
		       + (entity_name.IsEmpty() ? _T("") :_T(" : "))
		       + wxString((wxChar)filtre[element_id]));
      element_id++;
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::SendDataToPanel( AmayaParams& p )
{
  XmlEntity *MathEntityTable = (XmlEntity *)p.param1;

  // initialize entity hashtable
  int entity_id = 0;
  while (MathEntityTable[entity_id].charCode != -1)
    {
      m_MathMLEntityHash[MathEntityTable[entity_id].charCode] = TtaConvMessageToWX(MathEntityTable[entity_id].charName);
      entity_id++;
    }


#if 0
  bool * p_checked_array = (bool *)p.param1;

  wxLogDebug(_T("AmayaMathMLPanel::SendDataToPanel") );

  if (p_checked_array[WXAMAYA_PANEL_XHTML_STRONG])
    XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_STRONG", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  else
    XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_STRONG", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  
  if (p_checked_array[WXAMAYA_PANEL_XHTML_EMPH])
    XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_EMPH", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  else
    XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_EMPH", wxBitmapButton)->SetBackgroundColour( m_OffColour );
  
  if (p_checked_array[WXAMAYA_PANEL_XHTML_CODE])
    XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_CODE", wxBitmapButton)->SetBackgroundColour( m_OnColour );
  else
    XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_CODE", wxBitmapButton)->SetBackgroundColour( m_OffColour );

  Refresh();
  Layout();
#endif /* 0 */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::DoUpdate()
{
  wxLogDebug( _T("AmayaMathMLPanel::DoUpdate") );
  AmayaSubPanel::DoUpdate();

#if 0  
  // force to refresh the strong, emphasis... button states
  Document doc;
  View view;
  TtaGetActiveView( &doc, &view );
  TtaRefreshPanelButton( doc, view, WXAMAYA_PANEL_XHTML );
#endif /* 0 */
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  IsActive
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaMathMLPanel::IsActive()
{
  return AmayaSubPanel::IsActive();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaMathMLPanel, AmayaSubPanel)
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FENCE"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SQRT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ROOT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FRAC"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MATRIX"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNDER"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVER"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERUNDER"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MSCRIPT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUB"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUP"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUBSUP"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F1"), AmayaMathMLPanel::OnButtonFiltre1 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F2"), AmayaMathMLPanel::OnButtonFiltre2 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F3"), AmayaMathMLPanel::OnButtonFiltre3 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F4"), AmayaMathMLPanel::OnButtonFiltre4 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F5"), AmayaMathMLPanel::OnButtonFiltre5 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F6"), AmayaMathMLPanel::OnButtonFiltre6 )
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_F7"), AmayaMathMLPanel::OnButtonFiltre7 )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
