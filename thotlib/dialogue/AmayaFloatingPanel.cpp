#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaFloatingPanel.h"
#include "AmayaSubPanel.h"
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
  wxLogDebug( _T("AmayaFloatingPanel::AmayaFloatingPanel") );

  m_pPanelContent = (wxPanel *)wxWindow::FindWindowById(wxXmlResource::GetXRCID(_T("wxID_PANEL_CONTENT_DETACH")), m_pPanel);
  m_pPanelContentParent = m_pPanelContent->GetParent();
  m_pPanelContent->Reparent(this);
  
  // set the frame title
  wxStaticText * p_title_widget = (wxStaticText *)wxWindow::FindWindowById(wxXmlResource::GetXRCID(_T("wxID_LABEL_TITLE")), m_pPanel);
  SetTitle(p_title_widget->GetLabel());

  // insert the subpanel into the frame
  m_pTopSizer = new wxBoxSizer( wxVERTICAL );
  SetSizer(m_pTopSizer);
  m_pTopSizer->Add( m_pPanelContent, 1, wxALL | wxEXPAND , 5 );
  m_pTopSizer->Fit(this);
  
  Layout();
  SetAutoLayout(true);
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

void AmayaFloatingPanel::OnClose( wxCloseEvent& event )
{
  wxLogDebug(_T("AmayaFloatingPanel - OnClose") );

  m_pPanelContent->Reparent(m_pPanelContentParent);
  m_pPanel->DoUnstick();

  event.Skip();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaFloatingPanel, wxFrame)
  //  EVT_BUTTON( XRCID("wxID_BUTTON_DETACH"), AmayaFloatingPanel::OnDetach )
  EVT_CLOSE( AmayaFloatingPanel::OnClose )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
