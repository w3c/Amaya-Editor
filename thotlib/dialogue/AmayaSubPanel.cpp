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
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"


#include "AmayaSubPanel.h"
#include "AmayaFloatingPanel.h"
#include "AmayaNormalWindow.h"

#include "registry_wx.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaSubPanel, wxPanel)

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

  // setup labels
  XRCCTRL(*this, "wxID_BUTTON_DETACH", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ATTACHDETACH)));

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

  // do nothing if the content is allready unexpanded or if it is floating
  if (!m_IsExpanded || m_IsFloating)
    return;

  // setup bitmaps
  wxBitmapButton* p_button = XRCCTRL(*this, "wxID_BUTTON_EXPAND", wxBitmapButton);
  p_button->SetBitmapLabel(    m_Bitmap_ExpandOff );
  p_button->SetBitmapSelected( wxBitmap() );
  p_button->SetBitmapFocus(    wxBitmap() );
  Refresh();

  // setup content parnel size
  wxSizer * p_sizer = m_pPanelContent->GetContainingSizer();
  p_sizer->Show(m_pPanelContent,false);
  GetParent()->Layout();

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

  // do nothing if the content is allready expanded or if it is floating
  if (m_IsExpanded || m_IsFloating)
    return;

  // setup bitmaps
  wxBitmapButton* p_button = XRCCTRL(*this, "wxID_BUTTON_EXPAND", wxBitmapButton);
  p_button->SetBitmapLabel(    m_Bitmap_ExpandOn );
  p_button->SetBitmapSelected( wxBitmap() );
  p_button->SetBitmapFocus(    wxBitmap() );
  Refresh();

  // setup content panel size
  wxSizer * p_sizer = m_pPanelContent->GetContainingSizer();
  p_sizer->Show(m_pPanelContent,true);
  GetParent()->Layout();

  //m_pPanelContent->SetClientSize( wxSize(-1, m_ContentSize.y) );
  DebugPanelSize( _T("Expand") );

  m_IsExpanded = true;
  
  // call the update callback only if it has been requested
  if (m_ShouldBeUpdated)
    DoUpdate();
}
/*----------------------------------------------------------------------
  OnExpand is called when the user click on the expand button
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaSubPanel::OnExpand( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaSubPanel::OnExpand") );

  // switch the expand state
  if (m_IsExpanded)
    UnExpand();
  else
    Expand();

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
    {
      // the panel is floating, we must reattach it
      DoUnstick();

      // restore the old expand state
      if (m_IsExpBeforeDetach)
	Expand();
      else
	UnExpand();
    }
  else
    {
      // the panel is attached

      // save the expand state to be able to restore it when reattaching the panel
      m_IsExpBeforeDetach = m_IsExpanded;
      // force the panel to unexpand
      UnExpand();

      // detach the panel
      DoStick();
    }
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
  // disable the expand button
  wxBitmapButton* p_button_exp = XRCCTRL(*this, "wxID_BUTTON_EXPAND", wxBitmapButton);
  p_button_exp->Disable();

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

  // call the update callback only if it has been requested
  if (m_ShouldBeUpdated)
    DoUpdate();
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

  // enable the expand button
  wxBitmapButton* p_button_exp = XRCCTRL(*this, "wxID_BUTTON_EXPAND", wxBitmapButton);
  p_button_exp->Enable();

  // setup bitmaps
  wxBitmapButton* p_button = XRCCTRL(*this, "wxID_BUTTON_DETACH", wxBitmapButton);
  p_button->SetBitmapLabel(    m_Bitmap_DetachOff );
//  p_button->SetBitmapSelected( wxBitmap() );
//  p_button->SetBitmapFocus(    wxBitmap() );

  // close the floating window
  if (m_pFloatingPanel)
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


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  IsExpanded
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaSubPanel::IsExpanded()
{
  return m_IsExpanded;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  IsFloating
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaSubPanel::IsFloating()
{
  return m_IsFloating;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  IsVisible
 * Description:  used to know if the panel should be updated or not
 *--------------------------------------------------------------------------------------
 */
bool AmayaSubPanel::IsVisible()
{
  return (IsFloating() || IsExpanded());
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  ShouldBeUpdated
 * Description:  call this method to setup a flag used to force DoUpdate call when the
 *               sub panel is ready to be updated (when it is visible)
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanel::ShouldBeUpdated( bool should_update )
{
  m_ShouldBeUpdated = should_update;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  DoUpdate
 * Description:  this method is called when the sub-panel is ready to be updated
 *               it should be redefined into inherited classes but do not forget to call
 *               AmayaSubPanel::DoUpdate() in order to update the flags
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanel::DoUpdate()
{
  wxLogDebug( _T("AmayaSubPanel::DoUpdate") );
  m_ShouldBeUpdated = false;
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
