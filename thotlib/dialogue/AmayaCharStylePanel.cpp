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
#include "registry_wx.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "panel_tv.h"
#include "colors_f.h"
#include "inites_f.h"
#include "presentmenu_f.h"

#include "AmayaCharStylePanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFloatingPanel.h"
#include "AmayaSubPanelManager.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaCharStylePanel, AmayaSubPanel)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  AmayaCharStylePanel
 * Description:  construct a panel (bookmarks, elements, attributes, colors ...)
 *--------------------------------------------------------------------------------------
 */
AmayaCharStylePanel::AmayaCharStylePanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_CHARSTYLE") )
{
  wxLogDebug( _T("AmayaCharStylePanel::AmayaCharStylePanel") );

#if 0
  //  m_pPanelContentDetach = XRCCTRL(*this, "wxID_PANEL_CONTENT_DETACH", wxPanel);

  // setup labels
  RefreshToolTips();
  m_pTitleText->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_COLORS)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_MODIFYCOLOR", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_GETCOLOR", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CPGETBUTTON)));

  m_Bitmap_Empty        = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON, "empty.gif" ) );
  m_Bitmap_DefaultColor = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON, "default_color.gif" ) );
  m_Color_ButtonBG      = XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_FGCOLOR", wxBitmapButton)->GetBackgroundColour();

  // on windows, the color selector dialog must be complete.
  m_ColourData.SetChooseFull(true);
#endif /* 0 */

  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  ~AmayaCharStylePanel
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
AmayaCharStylePanel::~AmayaCharStylePanel()
{  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  GetPanelType
 * Description:  
 *--------------------------------------------------------------------------------------
 */
int AmayaCharStylePanel::GetPanelType()
{
  return WXAMAYA_PANEL_CHARSTYLE;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
 *--------------------------------------------------------------------------------------
 */
void AmayaCharStylePanel::RefreshToolTips()
{  
#if 0
  XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_DEFAULTCOLORS", wxButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_STD_COLORS)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_SWITCHCOLORS", wxButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CPCOLORSWITCH)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_BGCOLOR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CPCOLORBG)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_FGCOLOR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CPCOLORFG)));
#endif /* 0 */

}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
 *--------------------------------------------------------------------------------------
 */
void AmayaCharStylePanel::SendDataToPanel( AmayaPanelParams& p )
{
#if 0
  // update button background colors
  m_ThotBGColor = (int)p.param1;
  if (m_ThotBGColor >= 0)
    {
      wxColour * p_bg_colour = ColorPixel(m_ThotBGColor);
      XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_BGCOLOR", wxBitmapButton)->SetBackgroundColour( *p_bg_colour );
      XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_BGCOLOR", wxBitmapButton)->SetBitmapLabel(m_Bitmap_Empty);
    }
  else
    {
      // default bg color is ? white ?
      XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_BGCOLOR", wxBitmapButton)->SetBackgroundColour( m_Color_ButtonBG );
      XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_BGCOLOR", wxBitmapButton)->SetBitmapLabel(m_Bitmap_DefaultColor);
    }

  m_ThotFGColor = (int)p.param2;
  if (m_ThotFGColor >= 0)
    {
      wxColour * p_fg_colour = ColorPixel(m_ThotFGColor);
      XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_FGCOLOR", wxBitmapButton)->SetBackgroundColour( *p_fg_colour );
      XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_FGCOLOR", wxBitmapButton)->SetBitmapLabel(m_Bitmap_Empty);
    }
  else
    {
      // default fg color is ? black ?
      XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_FGCOLOR", wxBitmapButton)->SetBackgroundColour( m_Color_ButtonBG );
      XRCCTRL(*m_pPanelContentDetach, "wxID_BUTTON_FGCOLOR", wxBitmapButton)->SetBitmapLabel(m_Bitmap_DefaultColor);
    }
#endif /* 0 */
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
 *--------------------------------------------------------------------------------------
 */
void AmayaCharStylePanel::DoUpdate()
{
  wxLogDebug( _T("AmayaCharStylePanel::DoUpdate") );
  AmayaSubPanel::DoUpdate();
  //  ThotUpdatePalette();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  IsActive
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaCharStylePanel::IsActive()
{
  return AmayaSubPanel::IsActive();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaCharStylePanel, AmayaSubPanel)
  /*
  EVT_BUTTON( XRCID("wxID_MODIFYCOLOR"), AmayaCharStylePanel::OnModifyColor ) 
  EVT_BUTTON( XRCID("wxID_GETCOLOR"),    AmayaCharStylePanel::OnGetColor ) 
  EVT_BUTTON( XRCID("wxID_BUTTON_DEFAULTCOLORS"), AmayaCharStylePanel::OnDefaultColors ) 
  EVT_BUTTON( XRCID("wxID_BUTTON_SWITCHCOLORS"),  AmayaCharStylePanel::OnSwitchColors ) 
  EVT_BUTTON( XRCID("wxID_BUTTON_FGCOLOR"),  AmayaCharStylePanel::OnChooseFGColor ) 
  EVT_BUTTON( XRCID("wxID_BUTTON_BGCOLOR"),  AmayaCharStylePanel::OnChooseBGColor ) 
  */
END_EVENT_TABLE()

#endif /* #ifdef _WX */
