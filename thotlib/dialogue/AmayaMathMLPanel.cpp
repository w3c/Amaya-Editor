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

IMPLEMENT_DYNAMIC_CLASS(AmayaMathMLPanel, AmayaSubPanel)


/*
typedef struct _XmlEntity
{
  char         *charName;      
  int           charCode;      
} XmlEntity;
extern XmlEntity MathEntityTable[];
*/

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



  wxComboBox * m_pList = XRCCTRL(*this,"wxID_PANEL_MATH_LIST",wxComboBox);
  
  /*
  int entity_id = 0;
  while (MathEntityTable[entity_id].charCode != -1)
    {
      wxString wx_entity = TtaConvMessageToWX(MathEntityTable[entity_id].charName);
      wx_entity += _T(" : ");
      wxCSConv conv_utf16(wxT("UTF16"));
      //      wx_entity += wxString::Format(_T("/u%x"), );
      wx_entity += wxString((wxChar)MathEntityTable[entity_id].charCode);
      m_pList->Append(wx_entity);    
      entity_id++;
    }
  */


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

#if 0  
  int id       = event.GetId();
  int amaya_id = -1;

  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_STRONG")) )
    amaya_id = WXAMAYA_PANEL_XHTML_STRONG;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_EMPH")) )
    amaya_id = WXAMAYA_PANEL_XHTML_EMPH;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_CODE")) )
    amaya_id = WXAMAYA_PANEL_XHTML_CODE;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_H1")) )
    amaya_id = WXAMAYA_PANEL_XHTML_H1;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_H2")) )
    amaya_id = WXAMAYA_PANEL_XHTML_H2;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_H3")) )
    amaya_id = WXAMAYA_PANEL_XHTML_H3;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_BULLET")) )
    amaya_id = WXAMAYA_PANEL_XHTML_BULLET;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_NL")) )
    amaya_id = WXAMAYA_PANEL_XHTML_NL;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_DL")) )
    amaya_id = WXAMAYA_PANEL_XHTML_DL;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_IMG")) )
    amaya_id = WXAMAYA_PANEL_XHTML_IMG;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_LINK")) )
    amaya_id = WXAMAYA_PANEL_XHTML_LINK;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_TABLE")) )
    amaya_id = WXAMAYA_PANEL_XHTML_TABLE;
  
  if (amaya_id != -1)
    APP_Callback_PanelButtonActivate ( WXAMAYA_PANEL_XHTML,
				       TtaGiveActiveFrame(),
				       amaya_id );
#endif /* 0 */
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
  /*
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_STRONG"), AmayaMathMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_EMPH"),   AmayaMathMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_CODE"),   AmayaMathMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_H1"),     AmayaMathMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_H2"),     AmayaMathMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_H3"),     AmayaMathMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_BULLET"), AmayaMathMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_NL"),     AmayaMathMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_DL"),     AmayaMathMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_IMG"),    AmayaMathMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_LINK"),   AmayaMathMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_TABLE"),  AmayaMathMLPanel::OnButton ) 
  */
END_EVENT_TABLE()

#endif /* #ifdef _WX */
