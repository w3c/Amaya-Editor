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
#include "view.h"
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"

#include "message_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"


#include "AmayaPanel.h"
#include "AmayaSubPanel.h"
#include "AmayaSubPanelManager.h"
#include "AmayaAttributePanel.h"
#include "AmayaApplyClassPanel.h"
#include "AmayaMathMLPanel.h"
#include "AmayaXHTMLPanel.h"
#include "AmayaExplorerPanel.h"
#include "AmayaXMLPanel.h"
#include "AmayaSpeCharPanel.h"
#include "AmayaNormalWindow.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaPanel, wxPanel)

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  AmayaPanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 *               TODO
  -----------------------------------------------------------------------*/
AmayaPanel::AmayaPanel( wxWindow *      p_parent_window
			,AmayaNormalWindow * p_parent_nwindow
			,wxWindowID     id
			,const wxPoint& pos
			,const wxSize&  size
			,long style
			)
  :  wxPanel(/* wxDynamicCast( p_parent_window, wxWindow ),
                id, pos, size, style*/ )
     ,m_pParentNWindow(p_parent_nwindow)
{
  TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaPanel::AmayaPanel") );

  // init the panel list
  memset( m_aPanelList, 0, WXAMAYA_PANEL_TYPE_NB * sizeof(AmayaSubPanel *));

  // load resource
  wxXmlResource::Get()->LoadPanel(this, p_parent_window, _T("wxID_PANEL"));
  // get reference of usefull child
  m_pScrolledWindow = XRCCTRL(*this, "wxID_PANEL_SWIN", wxScrolledWindow);
  m_pScrolledWindow->SetScrollRate( 5, 5 );

  // load static sub-panels
  m_aPanelList[WXAMAYA_PANEL_EXPLORER]   = new AmayaExplorerPanel(     m_pScrolledWindow, p_parent_nwindow );
  m_aPanelList[WXAMAYA_PANEL_XHTML]      = new AmayaXHTMLPanel(     m_pScrolledWindow, p_parent_nwindow );
  m_aPanelList[WXAMAYA_PANEL_ATTRIBUTE]  = new AmayaAttributePanel( m_pScrolledWindow, p_parent_nwindow );
  m_aPanelList[WXAMAYA_PANEL_APPLYCLASS] = new AmayaApplyClassPanel( m_pScrolledWindow, p_parent_nwindow );
  m_aPanelList[WXAMAYA_PANEL_MATHML]     = new AmayaMathMLPanel( m_pScrolledWindow, p_parent_nwindow );
  m_aPanelList[WXAMAYA_PANEL_SPECHAR]    = new AmayaSpeCharPanel( m_pScrolledWindow, p_parent_nwindow );
  m_aPanelList[WXAMAYA_PANEL_XML]        = new AmayaXMLPanel( m_pScrolledWindow, p_parent_nwindow );

  wxBoxSizer * p_PanelSizer = new wxBoxSizer ( wxVERTICAL );
  m_pScrolledWindow->SetSizer(p_PanelSizer);
  p_PanelSizer->Add( m_aPanelList[WXAMAYA_PANEL_EXPLORER],   0, wxBOTTOM | wxEXPAND, 5 );
  p_PanelSizer->Add( m_aPanelList[WXAMAYA_PANEL_XHTML],      0, wxBOTTOM | wxEXPAND, 5 );
  p_PanelSizer->Add( m_aPanelList[WXAMAYA_PANEL_ATTRIBUTE],  0, wxBOTTOM | wxEXPAND, 5 );
  p_PanelSizer->Add( m_aPanelList[WXAMAYA_PANEL_APPLYCLASS], 0, wxBOTTOM | wxEXPAND, 5 );
  p_PanelSizer->Add( m_aPanelList[WXAMAYA_PANEL_MATHML],     0, wxBOTTOM | wxEXPAND, 5 );
  p_PanelSizer->Add( m_aPanelList[WXAMAYA_PANEL_SPECHAR],    0, wxBOTTOM | wxEXPAND, 5 );
  p_PanelSizer->Add( m_aPanelList[WXAMAYA_PANEL_XML],        0, wxBOTTOM | wxEXPAND, 5 );
  
  // setup labels
  XRCCTRL(*this, "wxID_LABEL_TOOLS", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_TOOLS)));
  XRCCTRL(*this, "wxID_BUTTON_CLOSE", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DONE)));

  Layout();
  SetAutoLayout(TRUE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  ~AmayaPanel
 * Description:  destructor
 *               TODO
  -----------------------------------------------------------------------*/
AmayaPanel::~AmayaPanel()
{
  
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  HideWhenUnsplit
 * Description:  must hide the panel & his childs
  -----------------------------------------------------------------------*/
void AmayaPanel::ShowWhenUnsplit( bool show )
{
  TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaPanel::ShowWhenUnsplit"));
  if (!show)
    {
      Hide();
   }
  else
    {
      Show();
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnClose
 * Description:  this method is called when the button for quick split is pushed
  -----------------------------------------------------------------------*/
void AmayaPanel::OnClose( wxCommandEvent& event )
{
  if ( event.GetId() != XRCCTRL(*this, "wxID_BUTTON_CLOSE", wxBitmapButton)->GetId() )
    {
      event.Skip();
      return;
    }
  m_pParentNWindow->ClosePanel();  
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  GetXHTMLPanel
 * Description:  
  -----------------------------------------------------------------------*/
AmayaXHTMLPanel * AmayaPanel::GetXHTMLPanel() const
{
  return (AmayaXHTMLPanel *)m_aPanelList[WXAMAYA_PANEL_XHTML];
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  GetAttributePanel
 * Description:  
  -----------------------------------------------------------------------*/
AmayaAttributePanel * AmayaPanel::GetAttributePanel() const
{
  return (AmayaAttributePanel *)m_aPanelList[WXAMAYA_PANEL_ATTRIBUTE];
}
/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  GetApplyClassPanel
 * Description:  
  -----------------------------------------------------------------------*/
AmayaApplyClassPanel * AmayaPanel::GetApplyClassPanel() const
{
  return (AmayaApplyClassPanel *)m_aPanelList[WXAMAYA_PANEL_APPLYCLASS];
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
  -----------------------------------------------------------------------*/
void AmayaPanel::RefreshToolTips()
{
  m_aPanelList[WXAMAYA_PANEL_XHTML]->RefreshToolTips();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OpenSubPanel
 * Description:  expand the subpanel
  -----------------------------------------------------------------------*/
void AmayaPanel::OpenSubPanel( int panel_type )
{
  if (panel_type >= WXAMAYA_PANEL_TYPE_NB || panel_type < 0 || m_aPanelList[panel_type] == NULL )
    return;
  AmayaSubPanelManager::GetInstance()->Expand( m_aPanelList[panel_type] );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  CloseSubPanel
 * Description:  unexpand the subpanel
  -----------------------------------------------------------------------*/
void AmayaPanel::CloseSubPanel( int panel_type )
{
  if (panel_type >= WXAMAYA_PANEL_TYPE_NB || panel_type < 0 || m_aPanelList[panel_type] == NULL )
    return;  
  AmayaSubPanelManager::GetInstance()->UnExpand( m_aPanelList[panel_type] );
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaPanel, wxPanel)
  EVT_BUTTON( -1, AmayaPanel::OnClose)
END_EVENT_TABLE()

#endif /* #ifdef _WX */
