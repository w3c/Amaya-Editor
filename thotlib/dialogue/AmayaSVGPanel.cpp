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

#include "AmayaSVGPanel.h"
#include "AmayaNormalWindow.h"


//
//
// AmayaSVGPanel
//
//

static
AMAYA_BEGIN_TOOLBAR_DEF_TABLE(AmayaSVGToolDef)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_P",       "CreateParagraph",      LIB, TMSG_BUTTON_P)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_H1",      "CreateHeading1",       LIB, TMSG_BUTTON_H1)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_H2",      "CreateHeading2",       LIB, TMSG_BUTTON_H2)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_H3",      "CreateHeading3",       LIB, TMSG_BUTTON_H3)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_H4",      "CreateHeading4",       LIB, TMSG_BUTTON_H4)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_DIV",     "CreateDivision",       LIB, TMSG_BUTTON_DIV)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_TABLE",   "DoCreateTable",        LIB, TMSG_BUTTON_TABLE)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_TARGET",  "CreateTarget",         LIB, TMSG_BUTTON_TARGET)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_LINK",    "CreateOrChangeLink",   LIB, TMSG_BUTTON_LINK)

AMAYA_END_TOOLBAR_DEF_TABLE()

AmayaSVGPanel::AmayaSVGPanel():
  wxPanel()
{
}

AmayaSVGPanel::AmayaSVGPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
    const wxSize& size, long style, const wxString& name, wxObject* extra):
  wxPanel()
{
  Create(parent, id, pos, size, style, name, extra);
}

AmayaSVGPanel::~AmayaSVGPanel()
{
}

bool AmayaSVGPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_SVG")))
    return false;
  
  m_tbar1 = XRCCTRL(*this,"wxID_TOOLBAR_SVG_1", AmayaBaseToolBar);

  m_tbar1->Add(AmayaSVGToolDef);
  m_tbar1->Realize();
  Fit();
  SetAutoLayout(true);
  
  return true;
}


#endif /* #ifdef _WX */
