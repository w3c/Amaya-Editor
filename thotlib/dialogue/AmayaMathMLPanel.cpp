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
#include "editcommands_f.h"

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "panel_tv.h"

#include "AmayaMathMLPanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFloatingPanel.h"
#include "AmayaSubPanelManager.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaMathMLPanel, AmayaSubPanel)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  AmayaMathMLPanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaMathMLPanel::AmayaMathMLPanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_MATHML") )
{
  // setup labels
  RefreshToolTips();
  m_pTitleText->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_MATHML)));

  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  ~AmayaMathMLPanel
 * Description:  destructor
 *               TODO
 *--------------------------------------------------------------------------------------
 */
AmayaMathMLPanel::~AmayaMathMLPanel()
{
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  GetPanelType
 * Description:  
 *--------------------------------------------------------------------------------------
 */
int AmayaMathMLPanel::GetPanelType()
{
  return WXAMAYA_PANEL_MATHML;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::RefreshToolTips()
{  
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaPanel
 *      Method:  OnButton
 * Description:  this method is called when the user click on a tool
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::OnButton( wxCommandEvent& event )
{
  Document doc;
  View view;
  TtaGetActiveView( &doc, &view );

  int id       = event.GetId();
  int amaya_id = -1;
  CloseTextInsertion ();
  if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_BMATH")) )
    TtaExecuteMenuAction ("CreateMath", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FENCE")) )
    TtaExecuteMenuAction ("CreateMROW", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SQRT")) )
    TtaExecuteMenuAction ("CreateMSQRT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_ROOT")) )
    TtaExecuteMenuAction ("CreateMROOT", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_FRAC")) )
    TtaExecuteMenuAction ("CreateMFRAC", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MATRIX")) )
    TtaExecuteMenuAction ("CreateMTABLE", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_UNDER")) )
    TtaExecuteMenuAction ("CreateMUNDER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVER")) )
    TtaExecuteMenuAction ("CreateMOVER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_OVERUNDER")) )
    TtaExecuteMenuAction ("CreateMUNDEROVER", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_MSCRIPT")) )
    TtaExecuteMenuAction ("CreateMMULTISCRIPTS", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUB")) )
    TtaExecuteMenuAction ("CreateMSUB", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUP")) )
    TtaExecuteMenuAction ("CreateMSUP", doc, view, FALSE);
  else if ( id == wxXmlResource::GetXRCID(_T("wxID_PANEL_MATH_SUBSUP")) )
    TtaExecuteMenuAction ("CreateMSUBSUP", doc, view, FALSE);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::SendDataToPanel( AmayaParams& p )
{
  int action = (int)p.param1;
  if (action == wxMATHML_ACTION_INIT)
    {
    }
  else if (action == wxMATHML_ACTION_REFRESH)
    {
    }
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
 *--------------------------------------------------------------------------------------
 */
void AmayaMathMLPanel::DoUpdate()
{
  AmayaSubPanel::DoUpdate();
}


/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaMathMLPanel
 *      Method:  IsActive
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaMathMLPanel::IsActive()
{
  return AmayaSubPanel::IsActive();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaMathMLPanel, AmayaSubPanel)
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_BMATH"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FENCE"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SQRT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_ROOT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_FRAC"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MATRIX"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_UNDER"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVER"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_OVERUNDER"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_MSCRIPT"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUB"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUP"), AmayaMathMLPanel::OnButton ) 
    EVT_BUTTON( XRCID("wxID_PANEL_MATH_SUBSUP"), AmayaMathMLPanel::OnButton ) 
END_EVENT_TABLE()

#endif /* #ifdef _WX */
