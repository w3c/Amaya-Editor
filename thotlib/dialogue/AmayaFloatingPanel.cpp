#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "logdebug.h"

#include "AmayaFloatingPanel.h"
#include "AmayaSubPanel.h"
#include "AmayaSubPanelManager.h"
#include "AmayaAttributePanel.h"
#include "AmayaColorsPanel.h"
#include "AmayaCharStylePanel.h"
#include "AmayaFormatPanel.h"
#include "AmayaApplyClassPanel.h"
#include "AmayaMathMLPanel.h"
#include "AmayaXHTMLPanel.h"
#include "AmayaXMLPanel.h"
#include "AmayaSpeCharPanel.h"

/*----------------------------------------------------------------------
 *       Class:  AmayaFloatingPanel
 *      Method:  AmayaFloatingPanel
 * Description:  construct a floating panel (bookmarks, elements, attributes ...)
  -----------------------------------------------------------------------*/
AmayaFloatingPanel::AmayaFloatingPanel( wxWindow * p_parent
					,AmayaSubPanel * p_subpanel
					,wxWindowID     id
					,const wxPoint& pos
					,const wxSize&  size
					,long style
					)
  : wxDialog( p_parent, id, _T("AmayaFloatingPanel"), pos, size, style )
  //  : wxFrame( p_parent, id, _T("AmayaFloatingPanel"), pos, size, style )
    ,m_pParentSubPanel(p_subpanel)
{
  
  // load the right panel depending on sub-panel parent type
  m_pPanel = NULL;
  switch (m_pParentSubPanel->GetPanelType())
    {
    case WXAMAYA_PANEL_XHTML:
      TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaFloatingPanel::AmayaFloatingPanel - WXAMAYA_PANEL_XHTML"));
      m_pPanel = new AmayaXHTMLPanel( this );
      break;
    case WXAMAYA_PANEL_ATTRIBUTE:
      TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaFloatingPanel::AmayaFloatingPanel - WXAMAYA_PANEL_ATTRIBUTE"));
      m_pPanel = new AmayaAttributePanel( this );
      break;
    case WXAMAYA_PANEL_COLORS:
      TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaFloatingPanel::AmayaFloatingPanel - WXAMAYA_PANEL_COLORS"));
      m_pPanel = new AmayaColorsPanel( this );
      break;
    case WXAMAYA_PANEL_CHARSTYLE:
      TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaFloatingPanel::AmayaFloatingPanel - WXAMAYA_PANEL_CHARSTYLE"));
      m_pPanel = new AmayaCharStylePanel( this );
      break;
    case WXAMAYA_PANEL_FORMAT:
      TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaFloatingPanel::AmayaFloatingPanel - WXAMAYA_PANEL_FORMAT"));
      m_pPanel = new AmayaFormatPanel( this );
      break;
    case WXAMAYA_PANEL_APPLYCLASS:
      TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaFloatingPanel::AmayaFloatingPanel - WXAMAYA_PANEL_APPLYCLASS"));
      m_pPanel = new AmayaApplyClassPanel( this );
      break;
    case WXAMAYA_PANEL_MATHML:
      TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaFloatingPanel::AmayaFloatingPanel - WXAMAYA_PANEL_MATHML"));
      m_pPanel = new AmayaMathMLPanel( this );
      break;
    case WXAMAYA_PANEL_XML:
      TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaFloatingPanel::AmayaFloatingPanel - WXAMAYA_PANEL_XML"));
      m_pPanel = new AmayaXMLPanel( this );
      break;
    case WXAMAYA_PANEL_SPECHAR:
      TTALOGDEBUG_0( TTA_LOG_PANELS, _T("AmayaFloatingPanel::AmayaFloatingPanel - WXAMAYA_PANEL_SPECHAR"));
      m_pPanel = new AmayaSpeCharPanel( this );
      break;
    }

  // hide the title bar
  wxPanel * p_panel_title = (wxPanel *)wxWindow::FindWindowById(wxXmlResource::GetXRCID(_T("wxID_PANEL_TITLE")), m_pPanel);
  wxSizer * p_sizer_title = p_panel_title->GetContainingSizer();
  p_sizer_title->Show(p_panel_title, false);
  p_sizer_title->Layout();

  // remove black borders
  m_pPanelContentDetach = XRCCTRL(*this, "wxID_PANEL_CONTENT_DETACH", wxPanel);
  m_pPanelContentDetach->SetWindowStyle( m_pPanelContentDetach->GetWindowStyle() & ~wxSIMPLE_BORDER );
  
  // set the frame title
  wxStaticText * p_title_widget = (wxStaticText *)wxWindow::FindWindowById(wxXmlResource::GetXRCID(_T("wxID_LABEL_TITLE")), m_pPanel);
  SetTitle(p_title_widget->GetLabel());

  // insert the subpanel into the frame
  m_pTopSizer = new wxBoxSizer( wxVERTICAL );
  SetSizer(m_pTopSizer);
  m_pTopSizer->Add( m_pPanel /*m_pPanelContentDetach*/, 1, wxALL | wxEXPAND , 2 );
  m_pTopSizer->Fit(this);
  
  Layout();
  SetPosition(wxGetMousePosition());
}

/*----------------------------------------------------------------------
 *       Class:  AmayaFloatingPanel
 *      Method:  ~AmayaFloatingPanel
 * Description:  destructor
 *               TODO
  -----------------------------------------------------------------------*/
AmayaFloatingPanel::~AmayaFloatingPanel()
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaFloatingPanel
 *      Method:  OnClose
 * Description:  reattach the subpanel to the sidepanel, reparent it
  -----------------------------------------------------------------------*/
void AmayaFloatingPanel::OnClose( wxCloseEvent& event )
{
  AmayaSubPanelManager::GetInstance()->DoUnfloat( m_pParentSubPanel );

  event.Skip();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaFloatingPanel
 *      Method:  Raise
 * Description:  override the top wxWindows::Raise methode. just show and raise the panel
  -----------------------------------------------------------------------*/
void AmayaFloatingPanel::Raise()
{
  Show();
  wxWindow::Raise();
}


/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaFloatingPanel, wxDialog)
  EVT_CLOSE( AmayaFloatingPanel::OnClose )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
