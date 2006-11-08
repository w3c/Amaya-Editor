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
  : AmayaSubPanel( p_parent_window, p_parent_nwindow, _T("wxID_PANEL_EXPLORER") ),
  m_dirCtrl(NULL)
{
  // setup labels
  RefreshToolTips();
  m_pTitleText->SetLabel(TtaConvMessageToWX(/*TtaGetMessage(LIB,TMSG_XHTML)*/"Explorer"));
  
  wxPanel *panel = XRCCTRL(*this, "wxID_PANEL_CONTENT_DETACH", wxPanel);
  
  wxSize sz = panel->GetSize();
  m_dirCtrl = new wxGenericDirCtrl(panel, wxID_ANY, wxDirDialogDefaultFolderStr,
                                   wxDefaultPosition, sz/*wxDefaultSize*/,
                                wxDIRCTRL_3D_INTERNAL|wxDIRCTRL_SELECT_FIRST|wxSUNKEN_BORDER|wxDIRCTRL_SHOW_FILTERS,
                                APPFILENAMEFILTER);
  wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(m_dirCtrl, 1, wxEXPAND);
  panel->SetSizer(sizer);
  sizer->Fit(this);
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
    if(!m_dirCtrl->GetFilePath().IsEmpty())
    {
        char buffer[2048];
        strcpy(buffer, m_dirCtrl->GetFilePath().mb_str(wxConvUTF8));
        OpenNewDocFromArgv(buffer);
    }    
}


/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaExplorerPanel, AmayaSubPanel)
  EVT_TREE_ITEM_ACTIVATED(wxID_ANY, AmayaExplorerPanel::OnDirTreeItemActivate)
END_EVENT_TABLE()

#endif /* #ifdef _WX */
