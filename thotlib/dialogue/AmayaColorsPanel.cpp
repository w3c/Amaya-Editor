#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/tglbtn.h"

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

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "panel_tv.h"
#include "colors_f.h"
#include "inites_f.h"
#include "presentmenu_f.h"

#include "AmayaColorsPanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFloatingPanel.h"
#include "AmayaSubPanelManager.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaColorsPanel, AmayaSubPanel)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaColorsPanel
 *      Method:  AmayaColorsPanel
 * Description:  construct a panel (bookmarks, elements, attributes, colors ...)
 *--------------------------------------------------------------------------------------
 */
AmayaColorsPanel::AmayaColorsPanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_COLORS") )
    ,m_ThotBGColor(-1)
    ,m_ThotFGColor(-1)
{
  wxLogDebug( _T("AmayaColorsPanel::AmayaColorsPanel") );

  m_pPanelContentDetach = XRCCTRL(*this, "wxID_PANEL_CONTENT_DETACH", wxPanel);

  RefreshToolTips();
  
  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaColorsPanel
 *      Method:  ~AmayaColorsPanel
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
AmayaColorsPanel::~AmayaColorsPanel()
{  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaColorsPanel
 *      Method:  GetPanelType
 * Description:  
 *--------------------------------------------------------------------------------------
 */
int AmayaColorsPanel::GetPanelType()
{
  return WXAMAYA_PANEL_COLORS;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaColorsPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
 *--------------------------------------------------------------------------------------
 */
void AmayaColorsPanel::RefreshToolTips()
{  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaColorPanel
 *      Method:  OnModifyColor
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaColorsPanel::OnModifyColor( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaColorsPanel::OnModifyColor") );
  ModifyColor (m_ThotFGColor, FALSE);
  ModifyColor (m_ThotBGColor, TRUE);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaColorPanel
 *      Method:  OnGetColor
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaColorsPanel::OnGetColor( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaColorsPanel::OnGetColor") );
  ThotGetSelectedElementColor();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaColorPanel
 *      Method:  OnDefaultColors
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaColorsPanel::OnDefaultColors( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaColorsPanel::OnDefaultColors") );
  ThotSelectPalette (-1, -1);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaColorPanel
 *      Method:  OnSwitchColors
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaColorsPanel::OnSwitchColors( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaColorsPanel::OnSwitchColors") );
  ThotSelectPalette (m_ThotFGColor, m_ThotBGColor);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaColorsPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
 *--------------------------------------------------------------------------------------
 */
void AmayaColorsPanel::SendDataToPanel( AmayaPanelParams& p )
{
  // update button background colors
  m_ThotBGColor = (int)p.param1;
  if (m_ThotBGColor >= 0)
    {
      wxColour * p_bg_colour = ColorPixel(m_ThotBGColor);
      XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_BGCOLOR", wxButton)->SetBackgroundColour( *p_bg_colour );
    }
  else
    {
      // default bg color is ? white ?
      XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_BGCOLOR", wxButton)->SetBackgroundColour( wxColour(_T("white")) );
    }

  m_ThotFGColor = (int)p.param2;
  if (m_ThotFGColor >= 0)
    {
      wxColour * p_fg_colour = ColorPixel(m_ThotFGColor);
      XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_FGCOLOR", wxButton)->SetBackgroundColour( *p_fg_colour );
    }
  else
    {
      // default fg color is ? black ?
      XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_FGCOLOR", wxButton)->SetBackgroundColour( wxColour(_T("black")) );
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaColorsPanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
 *--------------------------------------------------------------------------------------
 */
void AmayaColorsPanel::DoUpdate()
{
  wxLogDebug( _T("AmayaColorsPanel::DoUpdate") );
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
 *       Class:  AmayaColorsPanel
 *      Method:  IsActive
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaColorsPanel::IsActive()
{
  return AmayaSubPanel::IsActive();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaColorsPanel, AmayaSubPanel)
  EVT_BUTTON( XRCID("wxID_MODIFYCOLOR"), AmayaColorsPanel::OnModifyColor ) 
  EVT_BUTTON( XRCID("wxID_GETCOLOR"),    AmayaColorsPanel::OnGetColor ) 
  EVT_BUTTON( XRCID("wxID_BUTTON_DEFAULTCOLORS"), AmayaColorsPanel::OnDefaultColors ) 
  EVT_BUTTON( XRCID("wxID_BUTTON_SWITCHCOLORS"),  AmayaColorsPanel::OnSwitchColors ) 
END_EVENT_TABLE()

#endif /* #ifdef _WX */
