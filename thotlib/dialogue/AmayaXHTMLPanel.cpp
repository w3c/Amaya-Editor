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

#define THOT_EXPORT extern
#include "frame_tv.h"

#include "AmayaXHTMLPanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFrame.h"


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaXHTMLPanel
 *      Method:  AmayaXHTMLPanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaXHTMLPanel::AmayaXHTMLPanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_XHTML") )
{
  wxLogDebug( _T("AmayaXHTMLPanel::AmayaXHTMLPanel") );

}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaXHTMLPanel
 *      Method:  ~AmayaXHTMLPanel
 * Description:  destructor
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaXHTMLPanel::~AmayaXHTMLPanel()
{
  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaXHTMLPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
 *--------------------------------------------------------------------------------------
 */
void AmayaXHTMLPanel::RefreshToolTips()
{  
  wxASSERT(m_pParentNWindow);
  const char ** p_tooltip_array = WindowTable[m_pParentNWindow->GetWindowId()].Tooltip_Panel_XHTML;
  XRCCTRL(*this, "wxID_PANEL_XHTML_STRONG", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_STRONG]));
  XRCCTRL(*this, "wxID_PANEL_XHTML_EMPH",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_EMPH])); 
  XRCCTRL(*this, "wxID_PANEL_XHTML_CODE",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_CODE]));
  XRCCTRL(*this, "wxID_PANEL_XHTML_H1",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_H1]));
  XRCCTRL(*this, "wxID_PANEL_XHTML_H2",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_H2]));
  XRCCTRL(*this, "wxID_PANEL_XHTML_H3",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_H3]));
  XRCCTRL(*this, "wxID_PANEL_XHTML_BULLET", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_BULLET]));
  XRCCTRL(*this, "wxID_PANEL_XHTML_NL",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_NL]));
  XRCCTRL(*this, "wxID_PANEL_XHTML_DL",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_DL]));
  XRCCTRL(*this, "wxID_PANEL_XHTML_IMG",    wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_IMG]));
  XRCCTRL(*this, "wxID_PANEL_XHTML_LINK",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_LINK]));
  XRCCTRL(*this, "wxID_PANEL_XHTML_TABLE",  wxBitmapButton)->SetToolTip(TtaConvMessageToWX(p_tooltip_array[WXAMAYA_PANEL_XHTML_TABLE]));
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButton
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaXHTMLPanel::OnButton( wxCommandEvent& event )
{
  wxLogDebug( _T("AmayaXHTMLPanel::OnButton") );
  
  int id       = event.GetId();
  int amaya_id = -1;

  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_STRONG")) )
    amaya_id = WXAMAYA_PANEL_XHTML_STRONG;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_EMPH")) )
    amaya_id = WXAMAYA_PANEL_XHTML_EMPH;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_CODE")) )
    amaya_id = WXAMAYA_PANEL_XHTML_CODE;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_H1")) )
    amaya_id = WXAMAYA_PANEL_XHTML_H1;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_H2")) )
    amaya_id = WXAMAYA_PANEL_XHTML_H2;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_H3")) )
    amaya_id = WXAMAYA_PANEL_XHTML_H3;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_BULLET")) )
    amaya_id = WXAMAYA_PANEL_XHTML_BULLET;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_NL")) )
    amaya_id = WXAMAYA_PANEL_XHTML_NL;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_DL")) )
    amaya_id = WXAMAYA_PANEL_XHTML_DL;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_IMG")) )
    amaya_id = WXAMAYA_PANEL_XHTML_IMG;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_LINK")) )
    amaya_id = WXAMAYA_PANEL_XHTML_LINK;
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_TABLE")) )
    amaya_id = WXAMAYA_PANEL_XHTML_TABLE;
  
  if (amaya_id != -1)
    APP_Callback_PanelButtonActivate ( WXAMAYA_PANEL_XHTML,
				       m_pParentNWindow->GetActiveFrame()->GetFrameId(),
				       amaya_id );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaXHTMLPanel
 *      Method:  RefreshCheckButtonState
 * Description:  refresh the button widgets of the frame's panel
 *--------------------------------------------------------------------------------------
 */
void AmayaXHTMLPanel::RefreshCheckButtonState( bool * p_checked_array )
{
  wxColour color_off = wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND);
  wxColour color_on  = wxColour(250, 200, 200);

  if (p_checked_array[WXAMAYA_PANEL_XHTML_STRONG])
    XRCCTRL(*this, "wxID_PANEL_XHTML_STRONG", wxBitmapButton)->SetBackgroundColour( color_on );
  else
    XRCCTRL(*this, "wxID_PANEL_XHTML_STRONG", wxBitmapButton)->SetBackgroundColour( color_off );
  
  if (p_checked_array[WXAMAYA_PANEL_XHTML_EMPH])
    XRCCTRL(*this, "wxID_PANEL_XHTML_EMPH", wxBitmapButton)->SetBackgroundColour( color_on );
  else
    XRCCTRL(*this, "wxID_PANEL_XHTML_EMPH", wxBitmapButton)->SetBackgroundColour( color_off );
  
  if (p_checked_array[WXAMAYA_PANEL_XHTML_CODE])
    XRCCTRL(*this, "wxID_PANEL_XHTML_CODE", wxBitmapButton)->SetBackgroundColour( color_on );
  else
    XRCCTRL(*this, "wxID_PANEL_XHTML_CODE", wxBitmapButton)->SetBackgroundColour( color_off );

  Refresh();
  Layout();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaXHTMLPanel, AmayaSubPanel)
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_STRONG"), AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_EMPH"),   AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_CODE"),   AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_H1"),     AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_H2"),     AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_H3"),     AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_BULLET"), AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_NL"),     AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_DL"),     AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_IMG"),    AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_LINK"),   AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_TABLE"),  AmayaXHTMLPanel::OnButton ) 
END_EVENT_TABLE()

#endif /* #ifdef _WX */
