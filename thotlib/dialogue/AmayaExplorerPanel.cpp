/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

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
#include "paneltypes_wx.h"

#include "AmayaPanel.h"
#include "AmayaExplorerPanel.h"
#include "AmayaNormalWindow.h"

#include <wx/dirctrl.h>
#include <wx/treectrl.h>

void OpenNewDocFromArgv( char * url );

//
//
// AmayaExplorerToolPanel
//
//

IMPLEMENT_DYNAMIC_CLASS(AmayaExplorerToolPanel, AmayaToolPanel)

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
AmayaExplorerToolPanel::AmayaExplorerToolPanel():
  AmayaToolPanel()
{
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
AmayaExplorerToolPanel::~AmayaExplorerToolPanel()
{
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
bool AmayaExplorerToolPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_EXPLORER")))
    return false;
  
  char    *s;
  wxString path;
#ifdef _WINDOWS
  SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#endif /* _WINDOWS */
  wxGenericDirCtrl* dirCtrl = XRCCTRL(*this, "wxID_DIRCTRL_EXPLORER", wxGenericDirCtrl);
  
  // Initial selection in the set of folders
  s = TtaGetEnvString ("EXPLORER_PATH");
  if (s)
    path = TtaConvMessageToWX(s);
  else
    path = TtaGetHomeDir();
  
  dirCtrl->ExpandPath(path);
  //dirCtrl->SetFilter(APPFILENAMEFILTER);
  //dirCtrl->GetFilterListCtrl()->FillFilterList(APPFILENAMEFILTER, 0);
  dirCtrl->DoResize();
  
  return true;
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
wxString AmayaExplorerToolPanel::GetToolPanelName()const
{
  return TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_EXPLORE));
}

/*----------------------------------------------------------------------
 *       Class:  AmayaExplorerToolPanel
 *      Method:  GetDefaultAUIConfig
 * Description:  Return a default AUI config for the panel.
 -----------------------------------------------------------------------*/
wxString AmayaExplorerToolPanel::GetDefaultAUIConfig()
{
  return wxT("dir=4;layer=0;row=0;pos=0");
}

/*----------------------------------------------------------------------
 *       Class:  AmayaExplorerToolPanel
 *      Method:  OnDirTreeItemActivate
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaExplorerToolPanel::OnDirTreeItemActivate(wxTreeEvent& event)
{
  wxGenericDirCtrl* dirCtrl = XRCCTRL(*this, "wxID_DIRCTRL_EXPLORER", wxGenericDirCtrl);
  if (!dirCtrl->GetFilePath().IsEmpty())
  {
    char buffer[MAX_TXT_LEN];
    strcpy(buffer, dirCtrl->GetFilePath().mb_str(wxConvUTF8));
    TtaSetEnvString ("EXPLORER_PATH", buffer, TRUE);
    OpenNewDocFromArgv(buffer);
  }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaExplorerToolPanel
 *      Method:  OnClose
 * Description:  
  -----------------------------------------------------------------------*/
void AmayaExplorerToolPanel::OnClose(wxCloseEvent& event)
{
  wxGenericDirCtrl* dirCtrl = XRCCTRL(*this, "wxID_DIRCTRL_EXPLORER", wxGenericDirCtrl);
  if (!dirCtrl->GetFilePath().IsEmpty())
  {
    char buffer[MAX_TXT_LEN];
    strcpy(buffer, dirCtrl->GetFilePath().mb_str(wxConvUTF8));
    TtaSetEnvString ("EXPLORER_PATH", buffer, TRUE);
  }
  
  event.Skip();
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaExplorerToolPanel, AmayaToolPanel)
  EVT_TREE_ITEM_ACTIVATED( wxID_ANY, AmayaExplorerToolPanel::OnDirTreeItemActivate)
  EVT_CLOSE(AmayaExplorerToolPanel::OnClose)
END_EVENT_TABLE()

#endif /* #ifdef _WX */
