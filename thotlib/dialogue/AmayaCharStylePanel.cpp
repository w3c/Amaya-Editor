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
#include "font_f.h"

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
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_FONTFAMILY", wxChoice)->Append(TtaConvMessageToWX("Times"));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_FONTFAMILY", wxChoice)->Append(TtaConvMessageToWX("Helvetica"));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_FONTFAMILY", wxChoice)->Append(TtaConvMessageToWX("Courier"));
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
  
  XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_BODYSIZE",wxChoice)->Clear();
  int bodyRelatSize = 0;
  int bodyPointSize = 0;
  int font_size_nb = NumberOfFonts ();
  wxString font_size_unit = TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_TYPOGRAPHIC_POINTS));
  for (bodyRelatSize = 0; bodyRelatSize < font_size_nb; bodyRelatSize++)
    {
      bodyPointSize = ThotFontPointSize(bodyRelatSize);
      XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_BODYSIZE",wxChoice)->Append(wxString::Format(_T("%d "),bodyPointSize)+font_size_unit);
    }
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_BODYSIZE", wxChoice)->Append(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNCHANGED)));
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_BODYSIZE", wxChoice)->SetStringSelection(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_UNCHANGED)));

  ResetPresentMenus();

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
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );
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
  XRCCTRL(*m_pPanelContentDetach,"wxID_REFRESH",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_REFRESH)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_APPLY",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
 *--------------------------------------------------------------------------------------
 */
void AmayaCharStylePanel::SendDataToPanel( AmayaParams& p )
{
  int font_family    = (int)p.param1;
  int font_style     = (int)p.param2;
  int font_weight    = (int)p.param3;
  int font_underline = (int)p.param4;
  int font_size      = (int)p.param5;

  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_FONTFAMILY", wxChoice)->SetSelection(font_family);
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_UNDERLINE", wxChoice)->SetSelection(font_underline);
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_BODYSIZE", wxChoice)->SetSelection(font_size);
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_CHARSTYLE", wxChoice)->SetSelection(font_style);
  XRCCTRL(*m_pPanelContentDetach, "wxID_CHOICE_BOLDNESS", wxChoice)->SetSelection(font_weight);
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

  // do not refresh the panel from the current selection because the user must ask for that
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

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  OnApply
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaCharStylePanel::OnApply( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaCharStylePanel::OnApply") );

  int font_family    = XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_FONTFAMILY",wxChoice)->GetSelection();
  int font_style     = XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_CHARSTYLE",wxChoice)->GetSelection();
  int font_weight    = XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_BOLDNESS",wxChoice)->GetSelection();
  int font_underline = XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_UNDERLINE",wxChoice)->GetSelection();
  int font_size      = XRCCTRL(*m_pPanelContentDetach,"wxID_CHOICE_BODYSIZE",wxChoice)->GetSelection();

  ThotCallback (NumMenuCharFamily, INTEGER_DATA, (char*)font_family);
  ThotCallback (NumMenuCharFontStyle, INTEGER_DATA, (char*)font_style);
  ThotCallback (NumMenuCharFontWeight, INTEGER_DATA, (char*)font_weight);
  ThotCallback (NumMenuUnderlineType, INTEGER_DATA, (char*)font_underline);
  ThotCallback (NumMenuCharFontSize, INTEGER_DATA, (char*)font_size);
  ThotCallback (NumFormPresChar, INTEGER_DATA, (char*) 1);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  OnRefresh
 * Description:  refresh the panel from current selection
 *--------------------------------------------------------------------------------------
 */
void AmayaCharStylePanel::OnRefresh( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaCharStylePanel::OnRefresh") );
  RefreshCharStylePanel();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaCharStylePanel
 *      Method:  RefreshCharStylePanel
 * Description:  refresh the panel from current selection
 *--------------------------------------------------------------------------------------
 */
void AmayaCharStylePanel::RefreshCharStylePanel()
{
  Document doc;
  View view;
  TtaGetActiveView( &doc, &view );
  TtaExecuteMenuAction ("TtcChangeCharacters", doc, view);
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaCharStylePanel, AmayaSubPanel)
  EVT_BUTTON( XRCID("wxID_APPLY"),   AmayaCharStylePanel::OnApply ) 
  EVT_BUTTON( XRCID("wxID_REFRESH"), AmayaCharStylePanel::OnRefresh )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
