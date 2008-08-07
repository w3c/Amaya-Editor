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

/***************************************************************************
  List of all SVG constructions.
  ****************************************************************************/
#define MAX_BY_DIRECTORY 14

typedef struct svg_element_
  {
    const char *file_name;
    int  msg_id;
    const char *function_name;
  } svg_element;

typedef struct svg_elements_
  {
    const char* directory;
    int length;
    svg_element list[MAX_BY_DIRECTORY];
  } svg_elements;

static svg_elements svg_templates[] =
  {
    /*    {"basic_shapes", 2,
     {
       {"square", TMSG_SVG_Square, "CreateSVG_Square"},
       {"rounded_square", TMSG_SVG_Square, "CreateSVG_RoundedSquare"}
     }
     },*/

    {"3d", 6,
     {
       {"cone", TMSG_SVG_Cone, NULL},
       {"cube", TMSG_SVG_Cube, NULL},
       {"cylinder", TMSG_SVG_Cylinder, NULL},
       {"octahedron", TMSG_SVG_Octahedron, NULL},
       {"parallelepiped", TMSG_SVG_Parallelepiped, NULL},
       {"pyramid", TMSG_SVG_Pyramid, NULL}
     }
    },

    {"balloons", 5,
     {
       {"rectangular", TMSG_SVG_Rectangular, NULL},
       {"round", TMSG_SVG_Round, NULL},
       {"rounded_rectangular", TMSG_SVG_Rounded_rectangular, NULL},
       {"scream", TMSG_SVG_Scream, NULL},
       {"thought", TMSG_SVG_Thought, NULL}
     }
    },

    {"chemistry", 7,
     {
       {"beaker", TMSG_SVG_Beaker, NULL},
       {"boiling_flask", TMSG_SVG_Boiling_flask, NULL},
       {"buchner_flask", TMSG_SVG_Buchner_flask, NULL},
       {"burette", TMSG_SVG_Burette, NULL},
       {"erlenmeyer_flask", TMSG_SVG_Erlenmeyer_flask, NULL},
       {"pipette", TMSG_SVG_Pipette, NULL},
       {"test_tube", TMSG_SVG_Test_tube, NULL}
     }
    },

    {"circuit_diagram", 14,
     {
       {"current_source", TMSG_SVG_Current_source, NULL},
       {"voltage_source", TMSG_SVG_Voltage_source, NULL},
       {"ground_point", TMSG_SVG_Ground_point, NULL},
       {"resistor", TMSG_SVG_Resistor, NULL},
       {"resistor2", TMSG_SVG_Resistor, NULL},
       {"switch", TMSG_SVG_Switch, NULL},
       {"transistor", TMSG_SVG_Transistor, NULL},
       {"capacitor", TMSG_SVG_Capacitor, NULL},
       {"inductor", TMSG_SVG_Inductor, NULL},
       {"op-amp", TMSG_SVG_Op_amp, NULL},
       {"op-amp2", TMSG_SVG_Op_amp, NULL},
       {"led", TMSG_SVG_Led, NULL},
       {"diode", TMSG_SVG_Diode, NULL},
       {"zener_diode", TMSG_SVG_Zener_diode, NULL}
     }
    },

    {"polygons_and_stars", 7,
     {
       {"pentagon", TMSG_SVG_Pentagon, NULL},
       {"hexagon", TMSG_SVG_Hexagon, NULL},
       {"heptagon", TMSG_SVG_Heptagon, NULL},
       {"octogon", TMSG_SVG_Octogon, NULL},
       {"star4", TMSG_SVG_Star4, NULL},
       {"star5", TMSG_SVG_Star5, NULL},
       {"star6", TMSG_SVG_Star6, NULL}
     }
    },

    {"", 3,
     {
       {"cross", TMSG_SVG_Cross, NULL},
       {"frame", TMSG_SVG_Frame, NULL},
       {"ring", TMSG_SVG_Ring, NULL}
     }
    }
    
  };

/***************************************************************************
  DisplayMenu
    Display the i-th popup menu
  ****************************************************************************/
void AmayaSVGPanel::DisplayMenu(int i)
{
  svg_elements e = svg_templates[i];
  int j;
  char name[MAX_LENGTH];
  wxBitmap icon;
  wxString path;
  wxMenu menu;
  wxMenuItem *item;

  /* Register the number of last popup menu */
  last_menu = i;

  for(j = 0; j < e.length; j++)
    {
      /* Create a new menu item */
      item = new wxMenuItem(&menu, j,
                      TtaConvMessageToWX(TtaGetMessage(LIB, e.list[j].msg_id)));

      /* Add an icon if the png file exists  */
      sprintf(name, "%s/%s.png", e.directory, e.list[j].file_name);
      path = TtaGetResourcePathWX(WX_RESOURCES_SVG, name);
      if(icon.LoadFile(path, wxBITMAP_TYPE_PNG))
	item->SetBitmap(icon);

      /* Add the item in the menu */
      menu.Append(item);
      
    }

  /* Display the menu */
  PopupMenu(&menu);
}

/***************************************************************************
  InsertElement
    i = menu number
    j = submenu number
  ****************************************************************************/
void AmayaSVGPanel::InsertElement(int i, int j)
{
  char name[MAX_LENGTH];
  wxString path;

  wxMenu menu;
  svg_elements e = svg_templates[i];

  Document doc;
  View view;
  TtaGiveActiveView( &doc, &view );

  if(doc > 0 && j >= 0 && j < e.length)
    {

      if(e.list[j].function_name == NULL)
	{
	  /* No function name is given: call the creation of an svg template */
	  sprintf(name, "%s/%s.svg", e.directory, e.list[j].file_name);
	  path = TtaGetResourcePathWX(WX_RESOURCES_SVG, name);
	  LastSVGelement = TtaStrdup(path.mb_str(wxConvUTF8));
	  TtaExecuteMenuAction ("CreateSVG_Template", doc, view, TRUE);
	}
      else
	/* Call the function */
	TtaExecuteMenuAction (e.list[j].function_name, doc, view, TRUE);
    }
}

/***************************************************************************
  ****************************************************************************/
void AmayaSVGPanel::OnMenu0(wxCommandEvent& event)
{
  DisplayMenu(0);
}

void AmayaSVGPanel::OnMenu1(wxCommandEvent& event)
{
  DisplayMenu(1);
}

void AmayaSVGPanel::OnMenu2(wxCommandEvent& event)
{
  DisplayMenu(2);
}

void AmayaSVGPanel::OnMenu3(wxCommandEvent& event)
{
  DisplayMenu(3);
}

void AmayaSVGPanel::OnMenu4(wxCommandEvent& event)
{
  DisplayMenu(4);
}

void AmayaSVGPanel::OnMenu5(wxCommandEvent& event)
{
  DisplayMenu(5);
}

void AmayaSVGPanel::OnInsertElement(wxCommandEvent& event)
{
  InsertElement(last_menu, event.GetId());
}

BEGIN_EVENT_TABLE(AmayaSVGPanel, wxPanel)
  EVT_TOOL(XRCID("wxID_MENU_SVG_3D"), AmayaSVGPanel::OnMenu0)
  EVT_TOOL(XRCID("wxID_MENU_SVG_BALLOONS"), AmayaSVGPanel::OnMenu1)
  EVT_TOOL(XRCID("wxID_MENU_SVG_CHEMISTRY"), AmayaSVGPanel::OnMenu2)
  EVT_TOOL(XRCID("wxID_MENU_SVG_CIRCUIT_DIAGRAM"), AmayaSVGPanel::OnMenu3)
  EVT_TOOL(XRCID("wxID_MENU_SVG_POLYGONS_AND_STARS"), AmayaSVGPanel::OnMenu4)
  EVT_TOOL(XRCID("wxID_MENU_SVG_OTHERS"), AmayaSVGPanel::OnMenu5)

  EVT_MENU_RANGE(0, MAX_BY_DIRECTORY - 1, AmayaSVGPanel::OnInsertElement)
END_EVENT_TABLE()

#endif /* #ifdef _WX */
