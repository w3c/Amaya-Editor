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

AmayaExplorerToolPanel::AmayaExplorerToolPanel():
  AmayaToolPanel()
{
}

AmayaExplorerToolPanel::~AmayaExplorerToolPanel()
{
}

bool AmayaExplorerToolPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  return wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_EXPLORER"));
}

wxString AmayaExplorerToolPanel::GetToolPanelName()const
{
  return TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_EXPLORE));
}


#endif /* #ifdef _WX */
