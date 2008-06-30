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

#include "AmayaTemplatePanel.h"
#include "AmayaNormalWindow.h"


//
//
// AmayaSVGPanel
//
//

static
AMAYA_BEGIN_TOOLBAR_DEF_TABLE(AmayaTemplateToolDef)
AMAYA_TOOLBAR_DEF("wxID_TEMPLATE_TEXT_BOX",       "TemplateCreateTextBox", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_TEMPLATE_USE_BOX",        "TemplateCreateUseBox", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_TEMPLATE_FREE_BOX",       "TemplateCreateFreeBox", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_TEMPLATE_REPEAT_SECTION", "TemplateCreateRepeat", wxID_ANY, wxID_ANY)
AMAYA_END_TOOLBAR_DEF_TABLE()

AmayaTemplatePanel::AmayaTemplatePanel():
  wxPanel()
{
}

AmayaTemplatePanel::AmayaTemplatePanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
    const wxSize& size, long style, const wxString& name, wxObject* extra):
  wxPanel()
{
  Create(parent, id, pos, size, style, name, extra);
}

AmayaTemplatePanel::~AmayaTemplatePanel()
{
}

bool AmayaTemplatePanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_TEMPLATE")))
    return false;
  
  m_tbar1 = XRCCTRL(*this,"wxID_TOOLBAR_TEMPLATE_1", AmayaBaseToolBar);

  m_tbar1->Add(AmayaTemplateToolDef);
  m_tbar1->Realize();
  Fit();
  SetAutoLayout(true);
  
  return true;
}


#endif /* #ifdef _WX */
