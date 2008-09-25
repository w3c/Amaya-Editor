/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
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

#include "AmayaXHTMLPanel.h"
#include "AmayaNormalWindow.h"


//
//
// AmayaXHTMLPanel
//
//

static
AMAYA_BEGIN_TOOLBAR_DEF_TABLE(AmayaXHTMLToolDef)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_P",       "CreateParagraph",      LIB, TMSG_BUTTON_P)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_H1",      "CreateHeading1",       LIB, TMSG_BUTTON_H1)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_H2",      "CreateHeading2",       LIB, TMSG_BUTTON_H2)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_H3",      "CreateHeading3",       LIB, TMSG_BUTTON_H3)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_H4",      "CreateHeading4",       LIB, TMSG_BUTTON_H4)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_DIV",     "CreateDivision",       LIB, TMSG_BUTTON_DIV)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_TABLE",   "DoCreateTable",        LIB, TMSG_BUTTON_TABLE)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_TARGET",  "CreateTarget",         LIB, TMSG_BUTTON_TARGET)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_LINK",    "CreateOrChangeLink",   LIB, TMSG_BUTTON_LINK)

  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_BULLET",  "CreateList",           LIB, TMSG_BUTTON_UL)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_NL",      "CreateNumberedList",   LIB, TMSG_BUTTON_OL)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_DL",      "CreateDefinitionList", LIB, TMSG_BUTTON_DL)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_DT",      "CreateDefinitionTerm", LIB, TMSG_BUTTON_DT)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_DD",      "CreateDefinitionDef",  LIB, TMSG_BUTTON_DD)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_IMG",     "CreateImage",          LIB, TMSG_BUTTON_IMG)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_OBJ",     "CreateObject",         LIB, TMSG_OBJECT)

  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_STRONG",  "SetOnOffStrong",       LIB, TMSG_BUTTON_BOLD)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_EMPH",    "SetOnOffEmphasis",     LIB, TMSG_BUTTON_ITALICS)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_CODE",    "SetOnOffCode",         LIB, TMSG_BUTTON_CODE)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_INS",     "SetOnOffINS",          LIB, TMSG_INSERTION)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_DEL",     "SetOnOffDEL",          LIB, TMSG_DELETION)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_SUB",     "SetOnOffSub",          LIB, TMSG_SUB)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_XHTML_SUP",     "SetOnOffSup",          LIB, TMSG_SUP)

  AMAYA_TOOLBAR_DEF("wxID_PANEL_TABLE_MERGE",     "MergeSelectedCells",   LIB, CellMerge)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_TABLE_SPLIT_H",   "CellVertShrink",       LIB, CellVShrink)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_TABLE_SPLIT_V",   "CellHorizShrink",      LIB, CellHShrink)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_TABLE_INSERT_ROW","CreateRowBefore",      LIB, CreateRowB)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_TABLE_APPEND_ROW","CreateRowAfter",       LIB, CreateRowA)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_TABLE_INSERT_COL","CreateColumnBefore",   LIB, CreateColumnB)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_TABLE_APPEND_COL","CreateColumnAfter",    LIB, CreateColumnA)
AMAYA_END_TOOLBAR_DEF_TABLE()

AmayaXHTMLPanel::AmayaXHTMLPanel():
  wxPanel()
{
}

AmayaXHTMLPanel::AmayaXHTMLPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
    const wxSize& size, long style, const wxString& name, wxObject* extra):
  wxPanel()
{
  Create(parent, id, pos, size, style, name, extra);
}

AmayaXHTMLPanel::~AmayaXHTMLPanel()
{
}

bool AmayaXHTMLPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_XHTML")))
    return false;
  
  m_tbar1 = XRCCTRL(*this,"wxID_TOOLBAR_XHTML_1", AmayaBaseToolBar);
  m_tbar2 = XRCCTRL(*this,"wxID_TOOLBAR_XHTML_2", AmayaBaseToolBar);
  m_tbar3 = XRCCTRL(*this,"wxID_TOOLBAR_XHTML_3", AmayaBaseToolBar);
  m_tbar4 = XRCCTRL(*this,"wxID_TOOLBAR_XHTML_4", AmayaBaseToolBar);
  m_tbar5 = XRCCTRL(*this,"wxID_TOOLBAR_XHTML_5", AmayaBaseToolBar);

  m_tbar1->Add(AmayaXHTMLToolDef);
  m_tbar1->Realize();
  m_tbar2->Add(AmayaXHTMLToolDef);
  m_tbar2->Realize();
  m_tbar3->Add(AmayaXHTMLToolDef);
  m_tbar3->Realize();
  m_tbar4->Add(AmayaXHTMLToolDef);
  m_tbar4->Realize();
  m_tbar5->Add(AmayaXHTMLToolDef);
  m_tbar5->Realize();
  Fit();
  SetAutoLayout(true);
  
  return true;
}


#endif /* #ifdef _WX */
