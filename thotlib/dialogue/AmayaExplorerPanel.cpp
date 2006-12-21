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
#include "file_filters.h"

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "panel_tv.h"

#include "AmayaSubPanel.h"
#include "AmayaExplorerPanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaFloatingPanel.h"
#include "AmayaSubPanelManager.h"

#include <wx/dirctrl.h>
#include <wx/treectrl.h>

void OpenNewDocFromArgv( char * url );


IMPLEMENT_DYNAMIC_CLASS(AmayaExplorerPanel, AmayaSubPanel)

/*----------------------------------------------------------------------
 *       Class:  AmayaExplorerPanel
 *      Method:  AmayaExplorerPanel
 * Description:  construct a panel (bookmarks, elements, attributes ...)
 *               TODO
  -----------------------------------------------------------------------*/
AmayaExplorerPanel::AmayaExplorerPanel( wxWindow * p_parent_window, AmayaNormalWindow * p_parent_nwindow )
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_EXPLORER") )
{
  char *s;

  // setup labels
  RefreshToolTips();
  m_pTitleText->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_EXPLORE)));
  
  wxGenericDirCtrl* dirCtrl = XRCCTRL(*this, "wxID_DIRCTRL_EXPLORER", wxGenericDirCtrl);
  
  // Initial selection in the set of folders
  wxString path;
  s = TtaGetEnvString ("EXPLORER_PATH");
  if (s)
  {
    path = TtaConvMessageToWX(s);
  }
  else
  {
    path = TtaGetHomeDir();
  }
  
  dirCtrl->DoResize();
  
  // register myself to the manager, so I will be avertised that another panel is floating ...
  m_pManager->RegisterSubPanel( this );
}

/*----------------------------------------------------------------------
 *       Class:  AmayaExplorerPanel
 *      Method:  ~AmayaExplorerPanel
 * Description:  destructor
 *               TODO
  -----------------------------------------------------------------------*/
AmayaExplorerPanel::~AmayaExplorerPanel()
{
  // unregister myself to the manager, so nothing should be asked to me in future
  m_pManager->UnregisterSubPanel( this );  
}

/*----------------------------------------------------------------------
 *       Class:  AmayaExplorerPanel
 *      Method:  GetPanelType
 * Description:  
  -----------------------------------------------------------------------*/
int AmayaExplorerPanel::GetPanelType()
{
  return WXAMAYA_PANEL_EXPLORER;
}


/*----------------------------------------------------------------------
 *       Class:  AmayaExplorerPanel
 *      Method:  SendDataToPanel
 * Description:  refresh the button widgets of the frame's panel
  -----------------------------------------------------------------------*/
void AmayaExplorerPanel::SendDataToPanel( AmayaParams& p )
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaExplorerPanel
 *      Method:  DoUpdate
 * Description:  force a refresh when the user expand or detach this panel
  -----------------------------------------------------------------------*/
void AmayaExplorerPanel::DoUpdate()
{
  AmayaSubPanel::DoUpdate();
}


/*----------------------------------------------------------------------
 *       Class:  AmayaExplorerPanel
 *      Method:  IsActive
 * Description:  
  -----------------------------------------------------------------------*/
bool AmayaExplorerPanel::IsActive()
{
  return AmayaSubPanel::IsActive();
}

/*----------------------------------------------------------------------
 *       Class:  AmayaExplorerPanel
 *      Method:  IsActive
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaExplorerPanel::OnDirTreeItemActivate(wxTreeEvent& event)
{
  wxGenericDirCtrl* dirCtrl = XRCCTRL(*this, "wxID_DIRCTRL_EXPLORER", wxGenericDirCtrl);
  if(!dirCtrl->GetFilePath().IsEmpty())
  {
    char buffer[MAX_TXT_LEN];
    strcpy(buffer, dirCtrl->GetFilePath().mb_str(wxConvUTF8));
    TtaSetEnvString ("EXPLORER_PATH", buffer, TRUE);
    OpenNewDocFromArgv(buffer);
  }
}


/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaExplorerPanel, AmayaSubPanel)
  EVT_TREE_ITEM_ACTIVATED( wxID_ANY, AmayaExplorerPanel::OnDirTreeItemActivate)
END_EVENT_TABLE()

#endif /* #ifdef _WX */
