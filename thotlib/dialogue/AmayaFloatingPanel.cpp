#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaFloatingPanel.h"
#include "AmayaSubPanel.h"
#include "AmayaSubPanelManager.h"

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFloatingPanel
 *      Method:  AmayaFloatingPanel
 * Description:  construct a floating panel (bookmarks, elements, attributes ...)
 *--------------------------------------------------------------------------------------
 */
AmayaFloatingPanel::AmayaFloatingPanel( AmayaSubPanel * p_subpanel
					,wxWindowID     id
					,const wxPoint& pos
					,const wxSize&  size
					,long style
					)
  : wxFrame( p_subpanel, id, _T("AmayaFloatingPanel"), pos, size, style )
    ,m_pPanel(p_subpanel)
{
  wxString xrc_id = m_pPanel->GetPanelType();
  wxLogDebug( _T("AmayaFloatingPanel::AmayaFloatingPanel - id=")+xrc_id );

  // load resource, and take only the interesting part
  wxPanel * p_panel_copy = wxXmlResource::Get()->LoadPanel(this, xrc_id);
  m_pPanelContentDetach = (wxPanel *)wxWindow::FindWindowById(wxXmlResource::GetXRCID(_T("wxID_PANEL_CONTENT_DETACH")), p_panel_copy);
  //  m_pPanelContentParent = m_pPanelContent->GetParent();
  m_pPanelContentDetach->Reparent(this);
  p_panel_copy->Destroy();
  
  // set the frame title
  wxStaticText * p_title_widget = (wxStaticText *)wxWindow::FindWindowById(wxXmlResource::GetXRCID(_T("wxID_LABEL_TITLE")), m_pPanel);
  SetTitle(p_title_widget->GetLabel());

  // insert the subpanel into the frame
  m_pTopSizer = new wxBoxSizer( wxVERTICAL );
  SetSizer(m_pTopSizer);
  m_pTopSizer->Add( m_pPanelContentDetach, 1, wxALL | wxEXPAND , 0 );
  m_pTopSizer->Fit(this);
  
  Layout();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFloatingPanel
 *      Method:  ~AmayaFloatingPanel
 * Description:  destructor
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaFloatingPanel::~AmayaFloatingPanel()
{
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFloatingPanel
 *      Method:  OnClose
 * Description:  reattach the subpanel to the sidepanel, reparent it
 *--------------------------------------------------------------------------------------
 */
void AmayaFloatingPanel::OnClose( wxCloseEvent& event )
{
  wxLogDebug(_T("AmayaFloatingPanel - OnClose") );

  AmayaSubPanelManager::GetInstance()->DoUnfloat( m_pPanel );
  Hide();

  // do not skip this event, we want to keep this floating window in order to update it
  //  event.Skip();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFloatingPanel
 *      Method:  Raise
 * Description:  override the top wxWindows::Raise methode. just show and raise the panel
 *--------------------------------------------------------------------------------------
 */
void AmayaFloatingPanel::Raise()
{
  Show();
  wxWindow::Raise();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaFloatingPanel
 *      Method:  GetPanelContent
 * Description:  returns the panel content detachable, this content depends on owner panel type
 *--------------------------------------------------------------------------------------
 */
wxPanel * AmayaFloatingPanel::GetPanelContentDetach()
{
  return m_pPanelContentDetach;
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaFloatingPanel, wxFrame)
  EVT_CLOSE( AmayaFloatingPanel::OnClose )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
