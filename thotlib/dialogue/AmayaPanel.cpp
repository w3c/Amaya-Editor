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

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "attrmenu_f.h"
#include "frame_tv.h"

#include "message_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"


#include "AmayaPanel.h"
#include "AmayaSubPanel.h"
#include "AmayaSubPanelManager.h"
#include "AmayaXHTMLPanel.h"
#include "AmayaAttributePanel.h"
#include "AmayaColorsPanel.h"
#include "AmayaCharStylePanel.h"
#include "AmayaFormatPanel.h"
#include "AmayaNormalWindow.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaPanel, wxPanel)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  AmayaPanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaPanel::AmayaPanel( wxWindow *      p_parent_window
			,AmayaNormalWindow * p_parent_nwindow
			,wxWindowID     id
			,const wxPoint& pos
			,const wxSize&  size
			,long style
			)
  :  wxPanel( wxDynamicCast( p_parent_window, wxWindow ),
	      id, pos, size, style )
     ,m_pPanel_xhtml(NULL)
     ,m_pPanel_attribute(NULL)
     ,m_pParentNWindow(p_parent_nwindow)
{
  wxLogDebug( _T("AmayaPanel::AmayaPanel") );

  // init the panel list
  memset( m_aPanelList, 0, WXAMAYA_PANEL_TYPE_NB * sizeof(AmayaSubPanel *));

  // load title area
  m_pTitlePanel = wxXmlResource::Get()->LoadPanel(this, _T("wxID_TITLEPANEL"));
  m_pScrolledWindow = new wxScrolledWindow( this );
  m_pScrolledWindow->SetScrollRate( 5, 5 );

  // load static sub-panels  
  m_pPanel_xhtml     = new AmayaXHTMLPanel(     m_pScrolledWindow, p_parent_nwindow );
  m_pPanel_attribute = new AmayaAttributePanel( m_pScrolledWindow, p_parent_nwindow );
  m_pPanel_colors    = new AmayaColorsPanel(    m_pScrolledWindow, p_parent_nwindow );
  m_pPanel_charstyle = new AmayaCharStylePanel( m_pScrolledWindow, p_parent_nwindow );
  m_pPanel_format    = new AmayaFormatPanel(    m_pScrolledWindow, p_parent_nwindow );
  m_aPanelList[WXAMAYA_PANEL_XHTML]     = m_pPanel_xhtml;
  m_aPanelList[WXAMAYA_PANEL_ATTRIBUTE] = m_pPanel_attribute;
  m_aPanelList[WXAMAYA_PANEL_COLORS]    = m_pPanel_colors;
  m_aPanelList[WXAMAYA_PANEL_CHARSTYLE] = m_pPanel_charstyle;
  m_aPanelList[WXAMAYA_PANEL_FORMAT]    = m_pPanel_format;

  // attach subpanels & title to the panel
  wxBoxSizer * p_TopSizer = new wxBoxSizer ( wxVERTICAL );
  SetSizer(p_TopSizer);
  p_TopSizer->Add( m_pTitlePanel,       0, wxALL | wxEXPAND, 5 );
  p_TopSizer->Add( m_pScrolledWindow,   1, wxALL | wxEXPAND, 5 );

  wxBoxSizer * p_PanelSizer = new wxBoxSizer ( wxVERTICAL );
  m_pScrolledWindow->SetSizer(p_PanelSizer);
  p_PanelSizer->Add( m_pPanel_xhtml,      0, wxBOTTOM | wxEXPAND, 5 );
  p_PanelSizer->Add( m_pPanel_attribute,  0, wxBOTTOM | wxEXPAND, 5 );
  p_PanelSizer->Add( m_pPanel_colors,     0, wxBOTTOM | wxEXPAND, 5 );
  p_PanelSizer->Add( m_pPanel_charstyle,  0, wxBOTTOM | wxEXPAND, 5 );
  p_PanelSizer->Add( m_pPanel_format,     0, wxBOTTOM | wxEXPAND, 5 );
  
  // setup labels
  XRCCTRL(*this, "wxID_LABEL_TOOLS", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_TOOLS)));
  XRCCTRL(*this, "wxID_BUTTON_CLOSE", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DONE)));

  Layout();
  SetAutoLayout(TRUE);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  ~AmayaPanel
 * Description:  destructor
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaPanel::~AmayaPanel()
{
  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  HideWhenUnsplit
 * Description:  must hide the panel & his childs
 *--------------------------------------------------------------------------------------
 */
void AmayaPanel::ShowWhenUnsplit( bool show )
{
  wxLogDebug( _T("AmayaPanel::ShowWhenUnsplit") );
  if (!show)
    {
      Hide();
      m_pPanel_xhtml->Hide();
   }
  else
    {
      Show();
      m_pPanel_xhtml->Show();
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnClose
 * Description:  this method is called when the button for quick split is pushed
 *--------------------------------------------------------------------------------------
 */
void AmayaPanel::OnClose( wxCommandEvent& event )
{
  if ( event.GetId() != XRCCTRL(*this, "wxID_BUTTON_CLOSE", wxBitmapButton)->GetId() )
    {
      event.Skip();
      return;
    }
  m_pParentNWindow->ClosePanel();  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  GetXHTMLPanel
 * Description:  
 *--------------------------------------------------------------------------------------
 */
AmayaXHTMLPanel * AmayaPanel::GetXHTMLPanel() const
{
  return m_pPanel_xhtml;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  GetAttributePanel
 * Description:  
 *--------------------------------------------------------------------------------------
 */
AmayaAttributePanel * AmayaPanel::GetAttributePanel() const
{
  return m_pPanel_attribute;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
 *--------------------------------------------------------------------------------------
 */
void AmayaPanel::RefreshToolTips()
{
  m_pPanel_xhtml->RefreshToolTips();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OpenSubPanel
 * Description:  expand the subpanel
 *--------------------------------------------------------------------------------------
 */
void AmayaPanel::OpenSubPanel( int panel_type )
{
  if (panel_type >= WXAMAYA_PANEL_TYPE_NB || panel_type < 0 || m_aPanelList[panel_type] == NULL )
    return;  
  AmayaSubPanelManager::GetInstance()->Expand( m_aPanelList[panel_type] );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  CloseSubPanel
 * Description:  unexpand the subpanel
 *--------------------------------------------------------------------------------------
 */
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
