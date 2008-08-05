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
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_SVG",       "CreateSVG_Svg", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_LINE",       "CreateSVG_Line", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_RECTANGLE",       "CreateSVG_Rectangle", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ROUNDED_RECTANGLE",       "CreateSVG_RoundedRectangle", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_CIRCLE",       "CreateSVG_Circle", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ELLIPSE",       "CreateSVG_Ellipse", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_POLYLINE",       "CreateSVG_Polyline", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_POLYGON",       "CreateSVG_Polygon", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_SPLINE",       "CreateSVG_Spline", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_CLOSED",       "CreateSVG_Closed", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_FOREIGN_OBJECT",       "CreateSVG_ForeignObject", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_TEXT", "CreateSVG_Text", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_GROUP", "CreateSVG_Group", LIB, TMSG_GROUP)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_SIMPLE_ARROW", "CreateSVG_SimpleArrow", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_DOUBLE_ARROW", "CreateSVG_DoubleArrow", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ZIGZAG", "CreateSVG_Zigzag", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_SQUARE", "CreateSVG_Square", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ROUNDED_SQUARE", "CreateSVG_RoundedSquare", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_DIAMOND", "CreateSVG_Diamond", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_TRAPEZIUM", "CreateSVG_Trapezium", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_PARALLELOGRAM", "CreateSVG_Parallelogram", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_EQUILATERAL_TRIANGLE", "CreateSVG_EquilateralTriangle", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ISOSCELES_TRIANGLE", "CreateSVG_IsoscelesTriangle", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_RECTANGLE_TRIANGLE", "CreateSVG_RectangleTriangle", wxID_ANY, wxID_ANY)
/*AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_CUBE", "CreateSVG_Cube", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_PARALLELEPIPED", "CreateSVG_Parallelepiped", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_CYLINDER", "CreateSVG_Cylinder", wxID_ANY, wxID_ANY)*/
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_UNGROUP", "TransformSVG_Ungroup", LIB, TMSG_UNGROUP)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_SELECT", "SVG_Select", LIB, TMSG_SEL)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_FLIP_VERTICALLY", "TransformSVG_FlipVertically", LIB, TMSG_VREVERSE)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_FLIP_HORIZONTALLY", "TransformSVG_FlipHorizontally", LIB, TMSG_HREVERSE)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_BRING_TO_FRONT", "TransformSVG_BringToFront", LIB, TMSG_TOFRONT)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_BRING_FORWARD", "TransformSVG_BringForward", LIB, TMSG_UP)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_SEND_BACKWARD", "TransformSVG_SendBackward", LIB, TMSG_DOWN)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_SEND_TO_BACK", "TransformSVG_SendToBack", LIB, TMSG_TOBACK)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ROTATE_ANTICLOCKWISE", "TransformSVG_AntiClockWise", LIB, TMSG_TURNLEFT)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ROTATE_CLOCKWISE", "TransformSVG_ClockWise", LIB, TMSG_TURNRIGHT)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ALIGN_LEFT", "TransformSVG_AlignLeft", LIB, TMSG_FORMATLEFT)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ALIGN_CENTER", "TransformSVG_AlignCenter", LIB, TMSG_FORMATCENTER)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ALIGN_RIGHT", "TransformSVG_AlignRight", LIB, TMSG_FORMATRIGHT)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ALIGN_BOTTOM", "TransformSVG_AlignBottom", LIB, TMSG_FORMATBOTTOM)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ALIGN_MIDDLE", "TransformSVG_AlignMiddle", LIB, TMSG_FORMATMIDDLE)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ALIGN_TOP", "TransformSVG_AlignTop", LIB, TMSG_FORMATTOP)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_ROTATE", "TransformSVG_Rotate", LIB, TMSG_ROTATE)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_IMAGE", "CreateSVG_Image", LIB, TMSG_BUTTON_IMG)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_SKEW", "TransformSVG_Skew", LIB, TMSG_SKEW)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_SCALE", "TransformSVG_Scale", LIB, TMSG_SCALE)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_TRANSLATE", "TransformSVG_Translate", LIB, TMSG_TRANSLATE)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_DISTRIBUTE_LEFT", "TransformSVG_DistributeLeft", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_DISTRIBUTE_CENTER", "TransformSVG_DistributeCenter", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_DISTRIBUTE_RIGHT", "TransformSVG_DistributeRight", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_DISTRIBUTE_TOP", "TransformSVG_DistributeTop", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_DISTRIBUTE_MIDDLE", "TransformSVG_DistributeMiddle", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_DISTRIBUTE_CENTER", "TransformSVG_DistributeCenter", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_DISTRIBUTE_HSPACING", "TransformSVG_DistributeHSpacing", wxID_ANY, wxID_ANY)
AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_DISTRIBUTE_VSPACING", "TransformSVG_DistributeVSpacing", wxID_ANY, wxID_ANY)

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
  m_tbar2 = XRCCTRL(*this,"wxID_TOOLBAR_SVG_2", AmayaBaseToolBar);
  m_tbar3 = XRCCTRL(*this,"wxID_TOOLBAR_SVG_3", AmayaBaseToolBar);
  m_tbar4 = XRCCTRL(*this,"wxID_TOOLBAR_SVG_4", AmayaBaseToolBar);
  m_tbar5 = XRCCTRL(*this,"wxID_TOOLBAR_SVG_5", AmayaBaseToolBar);
  m_tbar6 = XRCCTRL(*this,"wxID_TOOLBAR_SVG_6", AmayaBaseToolBar);
  m_tbar7 = XRCCTRL(*this,"wxID_TOOLBAR_SVG_7", AmayaBaseToolBar);
  m_tbar8 = XRCCTRL(*this,"wxID_TOOLBAR_SVG_8", AmayaBaseToolBar);

  m_tbar1->Add(AmayaSVGToolDef);
  m_tbar1->Realize();
  m_tbar2->Add(AmayaSVGToolDef);
  m_tbar2->Realize();
  m_tbar3->Add(AmayaSVGToolDef);
  m_tbar3->Realize();
  m_tbar4->Add(AmayaSVGToolDef);
  m_tbar4->Realize();
  m_tbar5->Add(AmayaSVGToolDef);
  m_tbar5->Realize();
  m_tbar6->Add(AmayaSVGToolDef);
  m_tbar6->Realize();
  m_tbar7->Add(AmayaSVGToolDef);
  m_tbar7->Realize();
  m_tbar8->Add(AmayaSVGToolDef);
  m_tbar8->Realize();
  Fit();
  SetAutoLayout(true);
  
  return true;
}

#define NB_ELEMENTS 7

static const char* svg_sources_files[] = 
{
  "3d/cone.svg",
  "3d/cube.svg",
  "3d/cylinder.svg",
  "3d/parallelepiped.svg",
  "chemistry/burette.svg",
  "chemistry/erlenmeyer_flask.svg",
  "ring.svg"
};

void AmayaSVGPanel::OnMenuTest(wxCommandEvent& event)
{
  wxMenu menu;
  menu.Append(1, wxT("Cone"));
  menu.Append(2, wxT("Cube"));
  menu.Append(3, wxT("Cylinder"));
  menu.Append(4, wxT("Parallelepiped"));
  menu.Append(5, wxT("Burette"));
  menu.Append(6, wxT("Erlenmeyer"));
  menu.Append(7, wxT("Ring"));
  PopupMenu(&menu);
}

void AmayaSVGPanel::OnInsertElement(wxCommandEvent& event)
{
  Document doc;
  View view;

  TtaGiveActiveView( &doc, &view );

  if(doc > 0 && event.GetId()>=1 && event.GetId()<=NB_ELEMENTS)
    {
      wxString path = TtaGetResourcePathWX(WX_RESOURCES_SVG, svg_sources_files[event.GetId()-1]);
      LastSVGelement = TtaStrdup(path.mb_str(wxConvUTF8));
      TtaExecuteMenuAction ("CreateSVG_Template", doc, view, TRUE);
    }
}



BEGIN_EVENT_TABLE(AmayaSVGPanel, wxPanel)
  EVT_TOOL(XRCID("wxID_MENUTEST"), AmayaSVGPanel::OnMenuTest)
  EVT_MENU_RANGE(1, NB_ELEMENTS, AmayaSVGPanel::OnInsertElement)
END_EVENT_TABLE()

#endif /* #ifdef _WX */
