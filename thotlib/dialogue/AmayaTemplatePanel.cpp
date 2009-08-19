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
#define THOT_EXPORT extern
#include "frame_tv.h"
#include "paneltypes_wx.h"

#include "AmayaTemplatePanel.h"
#include "AmayaNormalWindow.h"


//
//
// AmayaTemplatePanel
//
//

static
AMAYA_BEGIN_TOOLBAR_DEF_TABLE(AmayaTemplateToolDef)
AMAYA_TOOLBAR_DEF("wxID_TEMPLATE_CREATE", "CreateTemplateFromDocument", LIB, TemplateFromDocument)
AMAYA_TOOLBAR_DEF("wxID_LIBRARY_CREATE", "NewLibrary", LIB, TemplateLibrary)
AMAYA_TOOLBAR_DEF("wxID_TEMPLATE_TEXT_BOX", "TemplateCreateTextBox", LIB, TemplateTextBox)
AMAYA_TOOLBAR_DEF("wxID_TEMPLATE_USE_BOX", "TemplateCreateUseBox", LIB, TemplateUseBox)
AMAYA_TOOLBAR_DEF("wxID_TEMPLATE_USECOMP_BOX", "TemplateCreateUseCompBox", LIB, TemplateUseCompBox)
AMAYA_TOOLBAR_DEF("wxID_TEMPLATE_REPEAT_SECTION", "TemplateCreateRepeat", LIB, TemplateRepeat)
AMAYA_TOOLBAR_DEF("wxID_TEMPLATE_REPEATCOMP_SECTION", "TemplateCreateRepeatComp", LIB, TemplateRepeatComp)
AMAYA_TOOLBAR_DEF("wxID_TEMPLATE_FREE_BOX", "TemplateCreateFreeBox", LIB, TemplateFreeBox)
AMAYA_TOOLBAR_DEF("wxID_TEMPLATE_UNION", "TemplateCreateUnion", LIB, TemplateUnion)
AMAYA_TOOLBAR_DEF("wxID_TEMPLATE_IMPORT", "TemplateCreateImport", LIB, TemplateImport)
AMAYA_END_TOOLBAR_DEF_TABLE()

/*----------------------------------------------------------------------------
  ----------------------------------------------------------------------------*/
AmayaTemplatePanel::AmayaTemplatePanel():
  wxPanel()
{
}

/*----------------------------------------------------------------------------
  ----------------------------------------------------------------------------*/
AmayaTemplatePanel::AmayaTemplatePanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
    const wxSize& size, long style, const wxString& name, wxObject* extra):
  wxPanel()
{
  Create(parent, id, pos, size, style, name, extra);
}

/*----------------------------------------------------------------------------
  ----------------------------------------------------------------------------*/
AmayaTemplatePanel::~AmayaTemplatePanel()
{
}

/*----------------------------------------------------------------------------
  ----------------------------------------------------------------------------*/
bool AmayaTemplatePanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_TEMPLATE")))
    return false;
  
  m_tbar1 = XRCCTRL(*this,"wxID_TOOLBAR_TEMPLATE_1", AmayaBaseToolBar);
  m_tbar2 = XRCCTRL(*this,"wxID_TOOLBAR_TEMPLATE_2", AmayaBaseToolBar);

  m_tbar1->Add(AmayaTemplateToolDef);
  m_tbar1->Realize();
  m_tbar2->Add(AmayaTemplateToolDef);
  m_tbar2->Realize();
  Fit();
  SetAutoLayout(true);
  
  return true;
}


#endif /* #ifdef _WX */
