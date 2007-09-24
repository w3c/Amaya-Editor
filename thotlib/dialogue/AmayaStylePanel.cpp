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

#include "AmayaStylePanel.h"
#include "AmayaNormalWindow.h"


//
//
// AmayaStyleToolPanel
//
//

static
AMAYA_BEGIN_TOOLBAR_DEF_TABLE(AmayaStyleToolDef)
AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_LEFT",      "TextAlignLeft", wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_CENTER",  "TextAlignCenter",  wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_RIGHT",   "TextAlignRight",  wxID_ANY, wxID_ANY )
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_JUSTIFY", "TextAlignJustify",  wxID_ANY, wxID_ANY )
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_S_LINE",  "LineSapcingSingle",   wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_H_LINE",  "LineSapcingHalf",  wxID_ANY, wxID_ANY )
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_D_LINE",  "LineSapcingDouble",  wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_INDENT",  "MarginLeftIncrease",  wxID_ANY, wxID_ANY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_NOINDENT","MarginLeftDecrease",  wxID_ANY, wxID_ANY)

AMAYA_END_TOOLBAR_DEF_TABLE()

IMPLEMENT_DYNAMIC_CLASS(AmayaStyleToolPanel, AmayaToolPanel)

AmayaStyleToolPanel::AmayaStyleToolPanel():
  AmayaToolPanel()
{
}

AmayaStyleToolPanel::~AmayaStyleToolPanel()
{
}

bool AmayaStyleToolPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_STYLE")))
    return false;
  
  m_tbar1 = XRCCTRL(*this,"wxID_TOOLBAR_CSS_1", AmayaBaseToolBar);
  m_tbar2 = XRCCTRL(*this,"wxID_TOOLBAR_CSS_2", AmayaBaseToolBar);

  m_tbar1->Add(AmayaStyleToolDef);
  m_tbar1->Realize();
  m_tbar2->Add(AmayaStyleToolDef);
  m_tbar2->Realize();
  SetAutoLayout(true);
  
  return true;
}

wxString AmayaStyleToolPanel::GetToolPanelName()const
{
  return TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CSSStyle));
}


#endif /* #ifdef _WX */
