#ifdef _WX

#include "wx/wx.h"
#include "wx/string.h"
#include "wx/xrc/xmlres.h"
#include "wx/settings.h"

// Thotlib includes
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
#include "logdebug.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "appdialogue_f.h"
#include "displayview_f.h"
#include "editcommands_f.h"
#include "message_wx.h"
#include "AmayaParams.h"
#include "appdialogue_wx_f.h"
#include "AmayaToolBar.h"
#include "AmayaWindow.h"
#include "AmayaFrame.h"


//
//
// AmayaBaseToolBar
//
//

IMPLEMENT_DYNAMIC_CLASS(AmayaBaseToolBar, wxToolBar)
BEGIN_EVENT_TABLE(AmayaBaseToolBar, wxToolBar)
  EVT_TOOL(wxID_ANY, AmayaBaseToolBar::OnTool)
  EVT_UPDATE_UI(wxID_ANY, AmayaBaseToolBar::OnUpdate)
END_EVENT_TABLE()


AmayaBaseToolBar::AmayaBaseToolBar():
wxToolBar()
{
  
}

bool AmayaBaseToolBar::Create( wxWindow *parent, wxWindowID id,
     const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
  return wxToolBar::Create(parent, id, pos, size, style, name);
}

AmayaBaseToolBar::~AmayaBaseToolBar()
{
}

void AmayaBaseToolBar::Add(AmayaToolBarToolDef* def)
{
  while(def->idname!=NULL)
    {
      if(def->action!=NULL && def->action[0]!=0)
        def->actionid = FindMenuAction(def->action);
      else
        def->actionid = -1;
      m_map[wxXmlResource::GetXRCID(wxString(def->idname, wxConvUTF8))] = def;
      def++;
    }  
}

bool AmayaBaseToolBar::Realize()
{
  AmayaToolBarToolDefHashMap::iterator it;
  // For each registered tool
  for( it = m_map.begin(); it != m_map.end(); ++it )
  {
    // set the tooltip content
    if(it->second->tooltip_categ!=wxID_ANY && it->second->tooltip_msg!=wxID_ANY)
      SetToolShortHelp(it->first, TtaConvMessageToWX(
        TtaGetMessage(it->second->tooltip_categ,it->second->tooltip_msg)));
  }
  return wxToolBar::Realize();
}

void AmayaBaseToolBar::OnTool(wxCommandEvent& event)
{
  AmayaToolBarToolDef* def = m_map[event.GetId()];
  if(def && def->action!=NULL && def->action[0]!=0)
    {
      Document doc;
      View view;
      FrameToView (TtaGiveActiveFrame(), &doc, &view);
      TtaExecuteMenuAction (def->action, doc, view, FALSE);
    }
}

void AmayaBaseToolBar::OnUpdate(wxUpdateUIEvent& event)
{
  AmayaToolBarToolDef* def = m_map[event.GetId()];
  if(def && def->actionid!=-1)
    {
      Document doc;
      View view;
      FrameToView (TtaGiveActiveFrame(), &doc, &view);
      event.Enable(MenuActionList[def->actionid].ActionActive[doc]);
    }
  else
    event.Enable(true);
}



//
//
// AmayaToolBarEditing
//
//


static
AMAYA_BEGIN_TOOLBAR_DEF_TABLE(AmayaToolBarEditingToolDef)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_NEW",        "NewXHTML",       LIB, TMSG_BUTTON_NEW)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_OPEN",       "OpenDoc",        LIB, TMSG_BUTTON_OPEN)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_SAVE",       "SaveDocument",   LIB, TMSG_BUTTON_SAVE)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_SAVE_ALL",   "SaveAll",        LIB, TMSG_BUTTON_SAVE_ALL)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_PRINT",      "SetupAndPrint",  LIB, TMSG_BUTTON_PRINT)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_UNDO",       "",               LIB, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_REDO",       "",               LIB, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_CUT",        "",               LIB, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_COPY",       "",               LIB, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_PASTE",      "",               LIB, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_SPELLCHECK", "",               LIB, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_FIND",       "",               LIB, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_CSS",        "SetCSSStyle",    LIB, TMSG_CSSStyle)
AMAYA_END_TOOLBAR_DEF_TABLE()


IMPLEMENT_DYNAMIC_CLASS(AmayaToolBarEditing, AmayaBaseToolBar)

AmayaToolBarEditing::AmayaToolBarEditing():
  AmayaBaseToolBar()
{
  Add(AmayaToolBarEditingToolDef);
}


//
//
// AmayaToolBarBrowsing
//
//

static
AMAYA_BEGIN_TOOLBAR_DEF_TABLE(AmayaToolBarBrowsingToolDef)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_BACK",    "GotoPreviousHTML", LIB, TMSG_BUTTON_PREVIOUS)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_FORWARD", "GotoNextHTML",     LIB, TMSG_BUTTON_NEXT)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_RELOAD",  "Reload",           LIB, TMSG_BUTTON_RELOAD)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_STOP",    "StopTransfer",     LIB, TMSG_BUTTON_INTERRUPT)
  AMAYA_TOOLBAR_DEF("wxID_TOOL_HOME",    "GoToHome",         LIB, TMSG_BUTTON_HOME)
AMAYA_END_TOOLBAR_DEF_TABLE()


IMPLEMENT_DYNAMIC_CLASS(AmayaToolBarBrowsing, AmayaBaseToolBar)

AmayaToolBarBrowsing::AmayaToolBarBrowsing():
  AmayaBaseToolBar()
{
  Add(AmayaToolBarBrowsingToolDef);
}


#endif /* #ifdef _WX */
