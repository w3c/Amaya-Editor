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
#include "displayview_f.h"
#include "registry_wx.h"

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "paneltypes_wx.h"

#include "AmayaXHTMLPanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFloatingPanel.h"
#include "AmayaSubPanelManager.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaXHTMLPanel, AmayaSubPanel)

/*----------------------------------------------------------------------
 *       Class:  AmayaXHTMLPanel
 *      Method:  AmayaXHTMLPanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 *               TODO
  -----------------------------------------------------------------------*/
AmayaXHTMLPanel::AmayaXHTMLPanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_XHTML") )
{
  // setup labels
  RefreshToolTips();
  m_pTitleText->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_XHTML)));
  
  m_OffColour = XRCCTRL(*m_pPanelContentDetach, "wxID_PANEL_XHTML_STRONG", wxBitmapButton)->GetBackgroundColour();
  m_OnColour  = wxColour(250, 200, 200);

  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaXHTMLPanel
 *      Method:  ~AmayaXHTMLPanel
 * Description:  destructor
 *               TODO
  -----------------------------------------------------------------------*/
AmayaXHTMLPanel::~AmayaXHTMLPanel()
{
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );  
}

/*----------------------------------------------------------------------
 *       Class:  AmayaXHTMLPanel
 *      Method:  GetPanelType
 * Description:  
  -----------------------------------------------------------------------*/
int AmayaXHTMLPanel::GetPanelType()
{
  return WXAMAYA_PANEL_XHTML;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaXHTMLPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
  -----------------------------------------------------------------------*/
void AmayaXHTMLPanel::RefreshToolTips()
{
  XRCCTRL(*this,"wxID_PANEL_XHTML_DIV",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_DIV)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_H1",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_H1)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_H2",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_H2)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_H3",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_H3)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_TABLE",  wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_TABLE)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_TARGET",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_TARGET)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_LINK",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_LINK)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_BULLET", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_UL)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_NL",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_OL)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_DL",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_DL)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_DT",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_DT)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_DD",     wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_DD)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_IMG",    wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_IMG)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_OBJ",    wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_OBJECT)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_STRONG", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_BOLD)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_EMPH",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_ITALICS))); 
  XRCCTRL(*this,"wxID_PANEL_XHTML_CODE",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_BUTTON_CODE)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_INS", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_INSERTION)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_DEL",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_DELETION))); 
  XRCCTRL(*this,"wxID_PANEL_XHTML_SUB",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_SUB)));
  XRCCTRL(*this,"wxID_PANEL_XHTML_SUP",   wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_SUP)));
}

/*----------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButton
 * Description:  this method is called when the user click on a tool
  -----------------------------------------------------------------------*/
void AmayaXHTMLPanel::OnButton( wxCommandEvent& event )
{
  int id = event.GetId();

  Document doc;
  View view;
  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_STRONG")) )
    TtaExecuteMenuAction ("SetOnOffStrong", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_EMPH")) )
    TtaExecuteMenuAction ("SetOnOffEmphasis", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_CODE")) )
    TtaExecuteMenuAction ("SetOnOffCode", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_INS")) )
    TtaExecuteMenuAction ("SetOnOffINS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_DEL")) )
    TtaExecuteMenuAction ("SetOnOffDEL", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_SUB")) )
    TtaExecuteMenuAction ("SetOnOffSub", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_SUP")) )
    TtaExecuteMenuAction ("SetOnOffSup", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_DIV")) )
    TtaExecuteMenuAction ("CreateDivision", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_H1")) )
    TtaExecuteMenuAction ("CreateHeading1", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_H2")) )
    TtaExecuteMenuAction ("CreateHeading2", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_H3")) )
    TtaExecuteMenuAction ("CreateHeading3", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_BULLET")) )
    TtaExecuteMenuAction ("CreateList", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_NL")) )
    TtaExecuteMenuAction ("CreateNumberedList", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_DL")) )
    TtaExecuteMenuAction ("CreateDefinitionList", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_DT")) )
    TtaExecuteMenuAction ("CreateDefinitionTerm", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_DD")) )
    TtaExecuteMenuAction ("CreateDefinitionDef", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_IMG")) )
    TtaExecuteMenuAction ("CreateImage", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_OBJ")) )
    TtaExecuteMenuAction ("CreateObject", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_ADDRESS")) )
    TtaExecuteMenuAction ("CreateAddress", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_LINK")) )
    TtaExecuteMenuAction ("CreateOrChangeLink", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_TABLE")) )
    TtaExecuteMenuAction ("CreateTable", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_XHTML_TARGET")) )
    TtaExecuteMenuAction ("CreateTarget", doc, view, FALSE);
}

/*----------------------------------------------------------------------
 *       Class:  AmayaXHTMLPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
  -----------------------------------------------------------------------*/
void AmayaXHTMLPanel::SendDataToPanel( AmayaParams& p )
{
  bool * p_checked_array = (bool *)p.param2;

  // load bitmaps
  m_Bitmap_StrongOn  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_B_sel.png" ), wxBITMAP_TYPE_PNG);
  m_Bitmap_EmphOn  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_I_sel.png" ), wxBITMAP_TYPE_PNG);
  m_Bitmap_CodeOn  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_T_sel.png" ), wxBITMAP_TYPE_PNG);
  m_Bitmap_InsOn  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_ins_sel.png" ), wxBITMAP_TYPE_PNG);
  m_Bitmap_DelOn  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_del_sel.png" ), wxBITMAP_TYPE_PNG);
  m_Bitmap_SubOn  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_sub_sel.png" ), wxBITMAP_TYPE_PNG);
  m_Bitmap_SupOn  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_sup_sel.png" ), wxBITMAP_TYPE_PNG);

  m_Bitmap_StrongOff  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_B.png" ), wxBITMAP_TYPE_PNG);
  m_Bitmap_EmphOff  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_I.png" ), wxBITMAP_TYPE_PNG);
  m_Bitmap_CodeOff  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_T.png" ), wxBITMAP_TYPE_PNG);
  m_Bitmap_InsOff  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_ins.png" ), wxBITMAP_TYPE_PNG);
  m_Bitmap_DelOff  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_del.png" ), wxBITMAP_TYPE_PNG);
  m_Bitmap_SubOff  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_sub.png" ), wxBITMAP_TYPE_PNG);
  m_Bitmap_SupOff  = wxBitmap( TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, "XHTML_sup.png" ), wxBITMAP_TYPE_PNG);

  if (p_checked_array[WXAMAYA_PANEL_XHTML_STRONG])
    XRCCTRL(*this, "wxID_PANEL_XHTML_STRONG", wxBitmapButton)->SetBitmapLabel( m_Bitmap_StrongOn );
  else
    XRCCTRL(*this, "wxID_PANEL_XHTML_STRONG", wxBitmapButton)->SetBitmapLabel( m_Bitmap_StrongOff );
  
  if (p_checked_array[WXAMAYA_PANEL_XHTML_EMPH])
    XRCCTRL(*this, "wxID_PANEL_XHTML_EMPH", wxBitmapButton)->SetBitmapLabel( m_Bitmap_EmphOn );
  else
    XRCCTRL(*this, "wxID_PANEL_XHTML_EMPH", wxBitmapButton)->SetBitmapLabel( m_Bitmap_EmphOff );
  
  if (p_checked_array[WXAMAYA_PANEL_XHTML_CODE])
    XRCCTRL(*this, "wxID_PANEL_XHTML_CODE", wxBitmapButton)->SetBitmapLabel( m_Bitmap_CodeOn );
  else
    XRCCTRL(*this, "wxID_PANEL_XHTML_CODE", wxBitmapButton)->SetBitmapLabel( m_Bitmap_CodeOff );

  if (p_checked_array[WXAMAYA_PANEL_XHTML_INS])
    XRCCTRL(*this, "wxID_PANEL_XHTML_INS", wxBitmapButton)->SetBitmapLabel( m_Bitmap_InsOn );
  else
    XRCCTRL(*this, "wxID_PANEL_XHTML_INS", wxBitmapButton)->SetBitmapLabel( m_Bitmap_InsOff );

  if (p_checked_array[WXAMAYA_PANEL_XHTML_DEL])
    XRCCTRL(*this, "wxID_PANEL_XHTML_DEL", wxBitmapButton)->SetBitmapLabel( m_Bitmap_DelOn );
  else
    XRCCTRL(*this, "wxID_PANEL_XHTML_DEL", wxBitmapButton)->SetBitmapLabel( m_Bitmap_DelOff );
  
  if (p_checked_array[WXAMAYA_PANEL_XHTML_SUB])
    XRCCTRL(*this, "wxID_PANEL_XHTML_SUB", wxBitmapButton)->SetBitmapLabel( m_Bitmap_SubOn );
  else
    XRCCTRL(*this, "wxID_PANEL_XHTML_SUB", wxBitmapButton)->SetBitmapLabel( m_Bitmap_SubOff );

  if (p_checked_array[WXAMAYA_PANEL_XHTML_SUP])
    XRCCTRL(*this, "wxID_PANEL_XHTML_SUP", wxBitmapButton)->SetBitmapLabel( m_Bitmap_SupOn );
  else
    XRCCTRL(*this, "wxID_PANEL_XHTML_SUP", wxBitmapButton)->SetBitmapLabel( m_Bitmap_SupOff );

  Refresh();
  Layout();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaXHTMLPanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
  -----------------------------------------------------------------------*/
void AmayaXHTMLPanel::DoUpdate()
{
  AmayaSubPanel::DoUpdate();
  
  // force to refresh the strong, emphasis... button states
  //Document doc;
  //View view;
  //TtaGetActiveView( &doc, &view );
  //TtaRefreshPanelButton( doc, view, WXAMAYA_PANEL_XHTML );
}


/*----------------------------------------------------------------------
 *       Class:  AmayaXHTMLPanel
 *      Method:  IsActive
 * Description:  
  -----------------------------------------------------------------------*/
bool AmayaXHTMLPanel::IsActive()
{
  return AmayaSubPanel::IsActive();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaXHTMLPanel, AmayaSubPanel)
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_STRONG"), AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_EMPH"),   AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_CODE"),   AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_INS"),    AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_DEL"),    AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_SUB"),    AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_SUP"),    AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_H1"),     AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_H2"),     AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_H3"),     AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_BULLET"), AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_NL"),     AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_DL"),     AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_DT"),     AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_DD"),     AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_IMG"),    AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_OBJ"),    AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_LINK"),   AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_TABLE"),  AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_DIV"),    AmayaXHTMLPanel::OnButton ) 
  EVT_BUTTON( XRCID("wxID_PANEL_XHTML_TARGET"), AmayaXHTMLPanel::OnButton ) 
END_EVENT_TABLE()

#endif /* #ifdef _WX */
