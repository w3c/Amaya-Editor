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

#include "AmayaApplyClassPanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFloatingPanel.h"
#include "AmayaSubPanelManager.h"

IMPLEMENT_DYNAMIC_CLASS(AmayaApplyClassPanel, AmayaSubPanel)

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  AmayaApplyClassPanel
 * Description:  construct a panel (bookmarks, elements, attributes, colors ...)
 *--------------------------------------------------------------------------------------
 */
AmayaApplyClassPanel::AmayaApplyClassPanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_APPLYCLASS") )
    ,m_ApplyClassRef(0)
{
  // setup labels
  RefreshToolTips();
  m_pTitleText->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY_CLASS)));

  m_pClassList = XRCCTRL(*m_pPanelContentDetach,"wxID_LIST_APPLYCLASS",wxListBox);

#if 0
  // setup labels
  RefreshToolTips();
  m_pTitleText->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_FORMAT)));

  //TtaGetMessage (LIB, TMSG_ALIGN)
  XRCCTRL(*m_pPanelContentDetach, "wxID_LABEL_FORMATINDENT", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_INDENT_PTS)));
  //TtaGetMessage (LIB, TMSG_INDENT)
  XRCCTRL(*m_pPanelContentDetach, "wxID_LABEL_FORMATLINESPACE", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_LINE_SPACING_PTS)));
  //TtaGetMessage (LIB, TMSG_LINE_SPACING)

  m_OffColour = XRCCTRL(*m_pPanelContentDetach, "wxID_BMPBUTTON_FORMATLEFT", wxBitmapButton)->GetBackgroundColour();
  m_OnColour  = wxColour(250, 200, 200);
#endif /* 0 */

  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );

  RefreshApplyClassPanel();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  ~AmayaApplyClassPanel
 * Description:  destructor
 *--------------------------------------------------------------------------------------
 */
AmayaApplyClassPanel::~AmayaApplyClassPanel()
{  
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  GetPanelType
 * Description:  
 *--------------------------------------------------------------------------------------
 */
int AmayaApplyClassPanel::GetPanelType()
{
  return WXAMAYA_PANEL_APPLYCLASS;
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  RefreshToolTips
 * Description:  reassign the tooltips values
 *--------------------------------------------------------------------------------------
 */
void AmayaApplyClassPanel::RefreshToolTips()
{  
  XRCCTRL(*m_pPanelContentDetach,"wxID_LIST_APPLYCLASS",wxListBox)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_SEL_CLASS)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_REFRESH",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_REFRESH)));
  XRCCTRL(*m_pPanelContentDetach,"wxID_APPLY",wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_APPLY)));
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
 *--------------------------------------------------------------------------------------
 */
void AmayaApplyClassPanel::SendDataToPanel( AmayaParams& p )
{
  int nb_class              = (long int)p.param1;
  const char * listBuffer   = (char *)p.param2;
  const char * currentClass = (char *)p.param3;
  int ref                   = (int)p.param4;;
  
  m_ApplyClassRef = ref;
  
  /* fill the list */
  m_pClassList->Clear();
  int i = 0;
  int index = 0;
  while (i < nb_class && listBuffer[index] != EOS)
    {
      m_pClassList->Append( TtaConvMessageToWX( &listBuffer[index] ) );
      index += strlen (&listBuffer[index]) + 1; /* one entry length */
      i++;
    }

  /* select the wanted item */
  m_pClassList->SetStringSelection(TtaConvMessageToWX(currentClass));

  /* recalculate layout */
  GetParent()->GetParent()->Layout();
  GetParent()->Layout();
  Layout();
  m_pPanelContentDetach->Layout();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
 *--------------------------------------------------------------------------------------
 */
void AmayaApplyClassPanel::DoUpdate()
{
  AmayaSubPanel::DoUpdate();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  IsActive
 * Description:  
 *--------------------------------------------------------------------------------------
 */
bool AmayaApplyClassPanel::IsActive()
{
  return AmayaSubPanel::IsActive();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  OnApply
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaApplyClassPanel::OnApply( wxCommandEvent& event )
{
  ThotCallback(m_ApplyClassRef, INTEGER_DATA, (char*) 1);
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  OnSelected
 * Description:  
 *--------------------------------------------------------------------------------------
 */
void AmayaApplyClassPanel::OnSelected( wxCommandEvent& event )
{
  wxString s_selected = XRCCTRL(*this, "wxID_LIST_APPLYCLASS", wxListBox)->GetStringSelection();
  
  // allocate a temporary buffer
  char buffer[512];
  wxASSERT( s_selected.Len() < 512 );
  strcpy( buffer, (const char*)s_selected.mb_str(wxConvUTF8) );
  
  // call the callback : send the selected string 
  ThotCallback(m_ApplyClassRef+1, STRING_DATA, buffer);
}



/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  OnRefresh
 * Description:  refresh the panel from current selection
 *--------------------------------------------------------------------------------------
 */
void AmayaApplyClassPanel::OnRefresh( wxCommandEvent& event )
{
  RefreshApplyClassPanel();
}

/*
 *--------------------------------------------------------------------------------------
 *       Class:  AmayaApplyClassPanel
 *      Method:  RefreshApplyClassPanel
 * Description:  refresh the panel from current selection
 *--------------------------------------------------------------------------------------
 */
void AmayaApplyClassPanel::RefreshApplyClassPanel()
{
  Document doc;
  View view;

  TtaGiveActiveView( &doc, &view );
  /* force the refresh */
  TtaExecuteMenuAction ("ApplyClass", doc, view, TRUE);
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaApplyClassPanel, AmayaSubPanel)
  EVT_BUTTON( XRCID("wxID_APPLY"), AmayaApplyClassPanel::OnApply )
  EVT_BUTTON( XRCID("wxID_REFRESH"), AmayaApplyClassPanel::OnRefresh )
  EVT_LISTBOX( XRCID("wxID_LIST_APPLYCLASS"), AmayaApplyClassPanel::OnSelected ) 
  EVT_LISTBOX_DCLICK( XRCID("wxID_LIST_APPLYCLASS"), AmayaApplyClassPanel::OnApply )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
