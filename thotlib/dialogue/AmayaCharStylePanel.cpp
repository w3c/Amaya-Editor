#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/spinctrl.h"

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
#include "font_f.h"

#include "AmayaCharStylePanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFloatingPanel.h"
#include "AmayaSubPanelManager.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaCharStylePanel, AmayaSubPanel)

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  AmayaCharStylePanel
 * Description:  construct a panel (bookmarks, elements, attributes, colors ...)
  -----------------------------------------------------------------------*/
AmayaCharStylePanel::AmayaCharStylePanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_CHARSTYLE") )
{
 // setup labels
  RefreshToolTips();
  m_pTitleText->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CHAR)));
  //  XRCCTRL(*m_pPanelContentDetach, "wxID_APPLY", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  //  XRCCTRL(*m_pPanelContentDetach, "wxID_REFRESH", wxButton)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_REFRESH)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_LABEL_FONTFAMILY", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_FONT_FAMILY)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_LABEL_UNDERLINE", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_LINE)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_LABEL_BODYSIZE", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BODY_SIZE_PTS)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_LABEL_CHARSTYLE", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_STYLE)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_LABEL_BOLDNESS", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BOLDNESS)));

  // fill choice selectors
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_FONTFAMILY", wxChoice)->Clear();
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_FONTFAMILY", wxChoice)->Append(TtaConvMessageToWX("Serif"));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_FONTFAMILY", wxChoice)->Append(TtaConvMessageToWX("Sans-serif"));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_FONTFAMILY", wxChoice)->Append(TtaConvMessageToWX("Monospace"));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_FONTFAMILY", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNCHANGED)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_FONTFAMILY", wxChoice)->SetStringSelection(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNCHANGED)));

  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_CHARSTYLE", wxChoice)->Clear();
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_CHARSTYLE", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ROMAN)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_CHARSTYLE", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_ITALIC)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_CHARSTYLE", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_OBLIQUE)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_CHARSTYLE", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNCHANGED)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_CHARSTYLE", wxChoice)->SetStringSelection(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNCHANGED)));

  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_UNDERLINE", wxChoice)->Clear();
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_UNDERLINE", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_NORMAL)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_UNDERLINE", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNDERLINE)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_UNDERLINE", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_OVERLINE)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_UNDERLINE", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CROSS_OUT)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_UNDERLINE", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNCHANGED)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_UNDERLINE", wxChoice)->SetStringSelection(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNCHANGED)));

  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_BOLDNESS", wxChoice)->Clear();
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_BOLDNESS", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_NOT_BOLD)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_BOLDNESS", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BOLD)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_BOLDNESS", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNCHANGED)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_BOLDNESS", wxChoice)->SetStringSelection(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNCHANGED)));
  
  // setup range of size
  XRCCTRL(*m_pPanelContentDetach, "wxID_SPIN_BODYSIZE", wxSpinCtrl)->SetValue( 0 );
  ResetPresentMenus();

  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  ~AmayaCharStylePanel
 * Description:  destructor
  -----------------------------------------------------------------------*/
AmayaCharStylePanel::~AmayaCharStylePanel()
{  
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  GetPanelType
 * Description:  
  -----------------------------------------------------------------------*/
int AmayaCharStylePanel::GetPanelType()
{
  return WXAMAYA_PANEL_CHARSTYLE;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
  -----------------------------------------------------------------------*/
void AmayaCharStylePanel::RefreshToolTips()
{
  XRCCTRL(*m_pPanelContentDetach,"wxID_REFRESH",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_REFRESH)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_APPLY",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_APPLY_FONTFAMILY",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_APPLY_UNDERLINE",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_APPLY_BODYSIZE",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_APPLY_CHARSTYLE",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_APPLY_BOLDNESS",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
  -----------------------------------------------------------------------*/
void AmayaCharStylePanel::SendDataToPanel( AmayaParams& p )
{
  int font_family    = p.param1;
  int font_underline = p.param7;
  int font_style     = p.param8;
  int font_weight    = p.param9;
  int font_size      = p.param10;

  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_FONTFAMILY", wxChoice)->SetSelection(font_family);
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_UNDERLINE", wxChoice)->SetSelection(font_underline);
  XRCCTRL(*m_pPanelContentDetach, "wxID_SPIN_BODYSIZE", wxSpinCtrl)->SetValue( font_size );
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_CHARSTYLE", wxChoice)->SetSelection(font_style);
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_BOLDNESS", wxChoice)->SetSelection(font_weight);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
  -----------------------------------------------------------------------*/
void AmayaCharStylePanel::DoUpdate()
{
  AmayaSubPanel::DoUpdate();

  // do not refresh the panel from the current selection because the user must ask for that
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  IsActive
 * Description:  
  -----------------------------------------------------------------------*/
bool AmayaCharStylePanel::IsActive()
{
  return AmayaSubPanel::IsActive();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  OnApply
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaCharStylePanel::OnApply( wxCommandEvent& event )
{
  int font_family = XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_FONTFAMILY",wxChoice)->GetSelection();
  int font_style = XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_CHARSTYLE",wxChoice)->GetSelection();
  int font_weight = XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_BOLDNESS",wxChoice)->GetSelection();
  int font_underline = XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_UNDERLINE",wxChoice)->GetSelection();
  int font_size = XRCCTRL(*m_pPanelContentDetach, "wxID_SPIN_BODYSIZE", wxSpinCtrl)->GetValue();

  ThotCallback (NumMenuCharFamily, INTEGER_DATA, (char*)font_family);
  ThotCallback (NumMenuCharFontStyle, INTEGER_DATA, (char*)font_style);
  ThotCallback (NumMenuCharFontWeight, INTEGER_DATA, (char*)font_weight);
  ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (char*)font_underline);
  ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*)font_size);
  ThotCallback (NumFormPresChar, INTEGER_DATA, (char*) 1);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  OnApplyFontFamily
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaCharStylePanel::OnApplyFontFamily( wxCommandEvent& event )
{
  int font_family    = XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_FONTFAMILY",wxChoice)->GetSelection();
  ThotCallback (NumMenuCharFamily, INTEGER_DATA, (char*)font_family); /* default=4 */
  ThotCallback (NumFormPresChar, INTEGER_DATA, (char*) 2);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  OnApplyUnderline
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaCharStylePanel::OnApplyUnderline( wxCommandEvent& event )
{
  int font_underline = XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_UNDERLINE",wxChoice)->GetSelection();
  ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (char*)font_underline); /* default=4 */
  ThotCallback (NumFormPresChar, INTEGER_DATA, (char*) 3);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  OnApplyBodySize
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaCharStylePanel::OnApplyBodySize( wxCommandEvent& event )
{
  int font_size = XRCCTRL(*m_pPanelContentDetach, "wxID_SPIN_BODYSIZE", wxSpinCtrl)->GetValue( );
  if (font_size != 0)
    ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*)font_size);
  ThotCallback (NumFormPresChar, INTEGER_DATA, (char*) 4);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  OnApplyCharStyle
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaCharStylePanel::OnApplyCharStyle( wxCommandEvent& event )
{
  int font_style     = XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_CHARSTYLE",wxChoice)->GetSelection();
  ThotCallback (NumMenuCharFontStyle, INTEGER_DATA, (char*)font_style); /* default=3 */
  ThotCallback (NumFormPresChar, INTEGER_DATA, (char*) 5);
}


/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  OnApplyBoldness
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaCharStylePanel::OnApplyBoldness( wxCommandEvent& event )
{
  int font_weight    = XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_BOLDNESS",wxChoice)->GetSelection();
  ThotCallback (NumMenuCharFontWeight, INTEGER_DATA, (char*)font_weight); /* default=2 */
  ThotCallback (NumFormPresChar, INTEGER_DATA, (char*) 6);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  OnRefresh
 * Description:  refresh the panel from current selection
  -----------------------------------------------------------------------*/
void AmayaCharStylePanel::OnRefresh( wxCommandEvent& event )
{
  RefreshCharStylePanel();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  RefreshCharStylePanel
 * Description:  refresh the panel from current selection
  -----------------------------------------------------------------------*/
void AmayaCharStylePanel::RefreshCharStylePanel()
{
  Document doc;
  View view;

  TtaGetActiveView( &doc, &view );
  /* force the refresh */
  TtaExecuteMenuAction ("TtcChangeCharacters", doc, view, TRUE);
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaCharStylePanel, AmayaSubPanel)
  EVT_BUTTON( XRCID("wxID_APPLY_FONTFAMILY"), AmayaCharStylePanel::OnApplyFontFamily ) 
  EVT_BUTTON( XRCID("wxID_APPLY_UNDERLINE"),  AmayaCharStylePanel::OnApplyUnderline ) 
  EVT_BUTTON( XRCID("wxID_APPLY_BODYSIZE"),   AmayaCharStylePanel::OnApplyBodySize ) 
  EVT_BUTTON( XRCID("wxID_APPLY_CHARSTYLE"),  AmayaCharStylePanel::OnApplyCharStyle ) 
  EVT_BUTTON( XRCID("wxID_APPLY_BOLDNESS"),   AmayaCharStylePanel::OnApplyBoldness ) 

  EVT_BUTTON( XRCID("wxID_APPLY"),   AmayaCharStylePanel::OnApply ) 
  EVT_BUTTON( XRCID("wxID_REFRESH"), AmayaCharStylePanel::OnRefresh )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
