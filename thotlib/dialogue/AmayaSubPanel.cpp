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
#include "registry_wx.h"
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"


#include "AmayaSubPanel.h"
#include "AmayaFloatingPanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaSubPanelManager.h"

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
     ,m_State( (wxAMAYA_SPANEL_EXPANDED & ~wxAMAYA_SPANEL_FLOATING) )
     ,m_DoUnfloat_Lock(false)
     ,m_pParentNWindow(p_parent_nwindow)
{
  wxLogDebug( _T("AmayaSubPanel::AmayaSubPanel: ")+panel_xrcid);

  // keep a reference on my manager
  m_pManager = AmayaSubPanelManager::GetInstance();

  // load resource
  m_pPanel = wxXmlResource::Get()->LoadPanel(this, panel_xrcid);
  m_pTopSizer = new wxBoxSizer( wxVERTICAL );
  SetSizer(m_pTopSizer);
  m_pTopSizer->Add( m_pPanel, 1, wxALL | wxEXPAND , 0 );
  m_pTopSizer->Fit(this);
  GetParent()->Layout();
  
  // get reference of these usefull childs
  m_pPanelContent       = XRCCTRL(*this, "wxID_PANEL_CONTENT", wxPanel);
  m_pPanelContentDetach = XRCCTRL(*this, "wxID_PANEL_CONTENT_DETACH", wxPanel);
  
  // load bitmaps
  m_Bitmap_DetachOn  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON, "detach_floating.gif" ) );
  m_Bitmap_DetachOff = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON, "detach.gif" ) );
  m_Bitmap_ExpandOn  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON, "expand_on.gif" ) );
  m_Bitmap_ExpandOff = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON, "expand_off.gif" ) );

  // create a presistant floating panel, it will be shown or hidden depanding on user choice
  m_pFloatingPanel = NULL;
  //  m_pFloatingPanel = new AmayaFloatingPanel( this, -1 );
  //  m_pFloatingPanel->Hide(); // not floating by default

  // setup labels
  XRCCTRL(*this, "wxID_BUTTON_DETACH", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ATTACHDETACH)));
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
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );

  // destroy manualy the floating panel because it seems that it stay alive
  if ( m_pFloatingPanel )
    m_pFloatingPanel->Destroy();
  m_pFloatingPanel = NULL;
}

/*----------------------------------------------------------------------
  UnExpand the wxID_PANEL_CONTENT area
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaSubPanel::UnExpand()
{
  wxLogDebug( _T("AmayaSubPanel::UnExpand") );

  // setup content panel size
  wxSizer * p_sizer = m_pPanelContent->GetContainingSizer();
  p_sizer->Show(m_pPanelContent,false);
  GetParent()->Layout();

  // call the update callback only if it has been requested
  if (m_ShouldBeUpdated)
    DoUpdate();
}

/*----------------------------------------------------------------------
  Expand the wxID_PANEL_CONTENT area
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaSubPanel::Expand()
{
  wxLogDebug( _T("AmayaSubPanel::Expand") );

  // setup content panel size
  wxSizer * p_sizer = m_pPanelContent->GetContainingSizer();
  p_sizer->Show(m_pPanelContent,true);
  GetParent()->Layout();

  // call the update callback only if it has been requested
  if (m_ShouldBeUpdated)
    DoUpdate();
}

/*----------------------------------------------------------------------
  DoFloat detach the subpanel from the AmayaWindow
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaSubPanel::DoFloat()
{
  wxLogDebug( _T("AmayaSubPanel::DoFloat") );

  // setup panel style
  /*  wxPanel* p_panel_detach = XRCCTRL(*this, "wxID_PANEL_CONTENT_DETACH", wxPanel);
  p_panel_detach->SetWindowStyle( p_panel_detach->GetWindowStyle() & ~wxSIMPLE_BORDER );
  */

  // open the floating panel
  if (!m_pFloatingPanel)
    m_pFloatingPanel = new AmayaFloatingPanel( m_pParentNWindow, this );
  m_pFloatingPanel->SetPosition(wxGetMousePosition());
  m_pFloatingPanel->Raise();
  //  m_pPanel_xhtml     = new AmayaXHTMLPanel( this, p_parent_nwindow );

  Refresh();
  GetParent()->Layout();

  // call the update callback only if it has been requested
  if (m_ShouldBeUpdated)
    DoUpdate();
}

/*----------------------------------------------------------------------
  DoUnfloat attach the subpanel to AmayaWindow
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaSubPanel::DoUnfloat()
{
  wxLogDebug( _T("AmayaSubPanel::DoUnfloat") );

  // close the floating window
  if (m_pFloatingPanel)
    m_pFloatingPanel->Destroy();
  m_pFloatingPanel = NULL;

  // setup panel style
  /*  wxPanel* p_panel_detach = XRCCTRL(*this, "wxID_PANEL_CONTENT_DETACH", wxPanel);
  p_panel_detach->SetWindowStyle( p_panel_detach->GetWindowStyle() | wxSIMPLE_BORDER );
  */

  GetParent()->Layout();
  Refresh();

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
  if (IsExpanded())
    m_pManager->UnExpand(this);
  else
    m_pManager->Expand(this);
}

/*----------------------------------------------------------------------
  OnDetach is called when the user click on the detach button
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaSubPanel::OnDetach( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaSubPanel::OnDetach") );

  if (IsFloating())
    {
      // the panel is floating, we must reattach it
      m_pManager->DoUnfloat( this );
    }
  else
    {
      // the panel is attached, detach the panel
      m_pManager->DoFloat( this );
    }
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
  return (m_State & wxAMAYA_SPANEL_EXPANDED);
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
  return (m_State & wxAMAYA_SPANEL_FLOATING);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  GetState
 * Description:  
 *--------------------------------------------------------------------------------------
 */
unsigned int AmayaSubPanel::GetState()
{
  return m_State;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  ChangeState
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanel::ChangeState( unsigned int new_state )
{
  m_State = new_state;
  
  // update icons
  wxBitmapButton* p_expand_button = XRCCTRL(*this, "wxID_BUTTON_EXPAND", wxBitmapButton);
  if (IsExpanded())
    {
      p_expand_button->SetBitmapLabel( m_Bitmap_ExpandOn );
      p_expand_button->SetBitmapSelected( wxBitmap() );
      p_expand_button->SetBitmapFocus( wxBitmap() );
    }
  else
    {
      p_expand_button->SetBitmapLabel( m_Bitmap_ExpandOff );
      p_expand_button->SetBitmapSelected( wxBitmap() );
      p_expand_button->SetBitmapFocus( wxBitmap() );
    }

  wxBitmapButton* p_detach_button = XRCCTRL(*this, "wxID_BUTTON_DETACH", wxBitmapButton);
  if (IsFloating())
    {
      // disable the expand button
      p_expand_button->Disable();
      // setup bitmaps
      p_detach_button->SetBitmapLabel( m_Bitmap_DetachOn );
    }
  else
    {
      // enable the expand button
      p_expand_button->Enable();
      // setup bitmaps
      p_detach_button->SetBitmapLabel( m_Bitmap_DetachOff );
    }
  Refresh();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  GetPanelType
 * Description:  
 *--------------------------------------------------------------------------------------
 */
int AmayaSubPanel::GetPanelType()
{
  return WXAMAYA_PANEL_UNKNOWN;
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

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  Raise
 * Description:  override the top wxWindows::Raise methode. this must raise the floating panel if it exists
 *               or raise the current panel window and give focus to this panel
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanel::Raise()
{
  if (IsFloating())
    m_pFloatingPanel->Raise();
  else
    wxWindow::Raise();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  GetPanelContent
 * Description:  returns the panel content detachable, this content depends on owner panel type
 *--------------------------------------------------------------------------------------
 */
wxPanel * AmayaSubPanel::GetPanelContentDetach()
{
  return m_pPanelContentDetach;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaSubPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel, should be override in inherited class
 *--------------------------------------------------------------------------------------
 */
void AmayaSubPanel::SendDataToPanel( void * param1, void * param2, void * param3, void * param4, void * param5, void * param6 )
{
  wxLogDebug( _T("AmayaSubPanel::SendDataToPanel") );
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
