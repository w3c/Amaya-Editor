#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces

#include "AmayaSubPanel.h"
#include "AmayaFloatingPanel.h"
#include "AmayaNormalWindow.h"

#include "registry_wx.h"

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  AmayaSubPanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaSubPanel::AmayaSubPanel( wxWindow *      p_parent_window
			      ,AmayaNormalWindow * p_parent_nwindow
			      ,const wxString& panel_xrcid
			      ,wxWindowID     id
			      ,const wxPoint& pos
			      ,const wxSize&  size
			      ,long style
			      )
  :  wxPanel( p_parent_window, id, pos, size, style )
     ,m_pPanel(NULL)
     ,m_IsExpanded(true)
     ,m_IsFloating(false)
     ,m_DoUnstick_Lock(false)
     ,m_pParentNWindow(p_parent_nwindow)
{
  wxLogDebug( _T("AmayaSubPanel::AmayaSubPanel: ")+panel_xrcid );

  // load resource
  m_pPanel = wxXmlResource::Get()->LoadPanel(this, panel_xrcid);
  m_pTopSizer = new wxBoxSizer( wxVERTICAL );
  SetSizer(m_pTopSizer);
  m_pTopSizer->Add( m_pPanel, 1, wxALL | wxEXPAND , 0 );
  m_pTopSizer->Fit(this);
  GetParent()->Layout();

  // remember the size of the content part .. used to size the expand/unexpand stats
  m_pPanelContent = XRCCTRL(*this, "wxID_PANEL_CONTENT", wxPanel);
  m_pPanelTitle   = XRCCTRL(*this, "wxID_PANEL_TITLE", wxPanel);
  m_ContentSize = m_pPanelContent->GetSize();
  m_TitleSize   = m_pPanelTitle->GetSize();

  // load bitmaps
  m_Bitmap_DetachOn  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON, "detach_floating.gif" ) );
  m_Bitmap_DetachOff = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON, "detach.gif" ) );
  m_Bitmap_ExpandOn  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON, "expand_on.gif" ) );
  m_Bitmap_ExpandOff = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON, "expand_off.gif" ) );

  DebugPanelSize(_T("AmayaSubPanel()"));
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  ~AmayaSubPanel
 * Description:  destructor
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaSubPanel::~AmayaSubPanel()
{
  
}

/*----------------------------------------------------------------------
  UnExpand the wxID_PANEL_CONTENT area
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaSubPanel::UnExpand()
{
  wxLogDebug( _T("AmayaSubPanel::UnExpand") );

  // do nothing if the content is allready unexpanded
  if (!m_IsExpanded && !m_IsFloating)
    return;

  // setup bitmaps
  wxBitmapButton* p_button = XRCCTRL(*this, "wxID_BUTTON_EXPAND", wxBitmapButton);
  p_button->SetBitmapLabel(    m_Bitmap_ExpandOff );
  p_button->SetBitmapSelected( wxBitmap() );
  p_button->SetBitmapFocus(    wxBitmap() );
  Refresh();

  // setup content parnel size
  wxSizer * p_sizer = m_pPanelContent->GetContainingSizer();
  p_sizer->SetItemMinSize( m_pPanelContent, wxSize( m_pPanelContent->GetSize().GetWidth(), 0 ) );
  p_sizer->Show(m_pPanelContent,false);
  p_sizer->Layout();

  //  m_pPanelContent->SetClientSize( wxSize(-1, 0) );
  DebugPanelSize( _T("UnExpand") );
  
  m_IsExpanded = false;
}

/*----------------------------------------------------------------------
  Expand the wxID_PANEL_CONTENT area
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaSubPanel::Expand()
{
  wxLogDebug( _T("AmayaSubPanel::Expand") );

  // do nothing if the content is allready expanded
  if (m_IsExpanded && !m_IsFloating)
    return;

  // setup bitmaps
  wxBitmapButton* p_button = XRCCTRL(*this, "wxID_BUTTON_EXPAND", wxBitmapButton);
  p_button->SetBitmapLabel(    m_Bitmap_ExpandOn );
  p_button->SetBitmapSelected( wxBitmap() );
  p_button->SetBitmapFocus(    wxBitmap() );
  Refresh();

  // setup content panel size
  wxSizer * p_sizer = m_pPanelContent->GetContainingSizer();
  p_sizer->SetItemMinSize( m_pPanelContent, wxSize( m_pPanelContent->GetSize().GetWidth(), -1 ) );
  p_sizer->Show(m_pPanelContent,true);
  p_sizer->Layout();

  //m_pPanelContent->SetClientSize( wxSize(-1, m_ContentSize.y) );
  DebugPanelSize( _T("Expand") );

  m_IsExpanded = true;
}
/*----------------------------------------------------------------------
  OnExpand is called when the user click on the expand button
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaSubPanel::OnExpand( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaSubPanel::OnExpand") );

  // TODO : faire fonctionner le systeme de expand/unexpand
  // switch state ...
  if (m_IsExpanded)
    UnExpand();
  else
    Expand();

  //  Fit();
  //  GetParent()->Layout();
  //m_pTopSizer->SetSizeHints( this );
  Layout();

  DebugPanelSize( _T("After-OnExpand") );
}

/*----------------------------------------------------------------------
  OnDetach is called when the user click on the detach button
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaSubPanel::OnDetach( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaSubPanel::OnDetach") );

  if (m_IsFloating)
    DoUnstick();
  else
    DoStick();
}

/*----------------------------------------------------------------------
  only for debug purpose
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaSubPanel::DebugPanelSize( const wxString & prefix )
{
  wxLogDebug( _T("AmayaSubPanel::DebugPanelSize[")+prefix+_T("] this=(%d,%d)"), GetSize().x, GetSize().y );
  wxLogDebug( _T("AmayaSubPanel::DebugPanelSize[")+prefix+_T("] title=(%d,%d)"), m_pPanelTitle->GetSize().x, m_pPanelTitle->GetSize().y );
  wxLogDebug( _T("AmayaSubPanel::DebugPanelSize[")+prefix+_T("] content=(%d,%d)"), m_pPanelContent->GetSize().x, m_pPanelContent->GetSize().y );
}

/*----------------------------------------------------------------------
  DoStick detach the subpanel from the AmayaWindow
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaSubPanel::DoStick()
{
  // setup bitmaps
  wxBitmapButton* p_button = XRCCTRL(*this, "wxID_BUTTON_DETACH", wxBitmapButton);
  p_button->SetBitmapLabel(    m_Bitmap_DetachOn );
//  p_button->SetBitmapSelected( wxBitmap() );
//  p_button->SetBitmapFocus(    wxBitmap() );

  // setup panel style
  wxPanel* p_panel_detach = XRCCTRL(*this, "wxID_PANEL_CONTENT_DETACH", wxPanel);
  p_panel_detach->SetWindowStyle( p_panel_detach->GetWindowStyle() & ~wxSIMPLE_BORDER );

  // open a new floating window
  m_pFloatingPanel = new AmayaFloatingPanel( this, -1, wxGetMousePosition() );
  m_pFloatingPanel->Show();
  m_IsFloating = true;

  Refresh();
  GetParent()->Layout();
}

/*----------------------------------------------------------------------
  DoUnstick attach the subpanel to AmayaWindow
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaSubPanel::DoUnstick()
{
  if (m_DoUnstick_Lock)
    return;
  m_DoUnstick_Lock = true;

  // setup bitmaps
  wxBitmapButton* p_button = XRCCTRL(*this, "wxID_BUTTON_DETACH", wxBitmapButton);
  p_button->SetBitmapLabel(    m_Bitmap_DetachOff );
//  p_button->SetBitmapSelected( wxBitmap() );
//  p_button->SetBitmapFocus(    wxBitmap() );

  // close the floating window
  m_pFloatingPanel->Close();
  m_pFloatingPanel = NULL;
  m_IsFloating = false;

  // setup panel style
  wxPanel* p_panel_detach = XRCCTRL(*this, "wxID_PANEL_CONTENT_DETACH", wxPanel);
  p_panel_detach->SetWindowStyle( p_panel_detach->GetWindowStyle() | wxSIMPLE_BORDER );

  GetParent()->Layout();
  Refresh();

  m_DoUnstick_Lock = false;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  SetTopAmayaWindow
 * Description:  setup the top AmayaWindow parent of this panel, used to know the 
 *               current active frame in order to actiavte the right callbacks 
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanel::SetTopAmayaWindow( AmayaNormalWindow * p_parent_window )
{
  m_pParentNWindow = p_parent_window;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  RefreshCheckButtonState
 * Description:  refresh the button widgets of the frame's panel
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanel::RefreshCheckButtonState( bool * p_checked_array )
{
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanel::RefreshToolTips()
{
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaSubPanel, wxPanel)
  EVT_BUTTON( XRCID("wxID_BUTTON_EXPAND"), AmayaSubPanel::OnExpand ) 
  EVT_BUTTON( XRCID("wxID_BUTTON_DETACH"), AmayaSubPanel::OnDetach )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
