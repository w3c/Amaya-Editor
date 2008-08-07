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
  AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_SELECT", "SVG_Select", LIB, TMSG_SEL)
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

/***************************************************************************
  AmayaSVGPanel
*****************************************************************************/
AmayaSVGPanel::AmayaSVGPanel(wxWindow* parent, wxWindowID id,
			     const wxPoint& pos, 
			     const wxSize& size, long style,
			     const wxString& name, wxObject* extra):
  wxPanel()
{
  Create(parent, id, pos, size, style, name, extra);
}

/***************************************************************************
  ~AmayaSVGPanel
*****************************************************************************/
AmayaSVGPanel::~AmayaSVGPanel()
{
}

/***************************************************************************
  Create the panel
*****************************************************************************/
bool AmayaSVGPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
			   const wxSize& size, long style,
			   const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this,
				      parent, wxT("wxID_TOOLPANEL_SVG")))
    return false;
  
  m_tbar1 = XRCCTRL(*this,"wxID_TOOLBAR_SVG_1", AmayaBaseToolBar);
  m_tbar2 = XRCCTRL(*this,"wxID_TOOLBAR_SVG_2", AmayaBaseToolBar);
  m_tbar3 = XRCCTRL(*this,"wxID_TOOLBAR_SVG_3", AmayaBaseToolBar);
  m_tbar4 = XRCCTRL(*this,"wxID_TOOLBAR_SVG_4", AmayaBaseToolBar);

  m_tbar1->Add(AmayaSVGToolDef);
  m_tbar1->Realize();
  m_tbar2->Add(AmayaSVGToolDef);
  m_tbar2->Realize();
  m_tbar3->Add(AmayaSVGToolDef);
  m_tbar3->Realize();
  m_tbar4->Add(AmayaSVGToolDef);
  m_tbar4->Realize();
  Fit();
  SetAutoLayout(true);
  
  return true;
}

/***************************************************************************
  List of all SVG constructions.
****************************************************************************/
#define MAX_CONSTRUCTS_BY_DIRECTORY 14

/* Structure of an SVG contruct */
typedef struct svg_construct_
{
  const char *file_name;
  int  msg_id;
  const char *function_name;
} svg_construct;

typedef struct svg_constructs_
{
  const char* directory;
  int length;
  svg_construct list[MAX_CONSTRUCTS_BY_DIRECTORY];
} svg_constructs;

static svg_constructs list_of_constructs[] =
  {
    {"special_objects", 4,
     {
       {"svg_canvas", TMSG_SVG_SvgCanvas, "CreateSVG_Svg"},
       {"text", TMSG_SVG_Text, "CreateSVG_Text"},
       {"foreign_object", TMSG_SVG_ForeigObject, "CreateSVG_ForeignObject"},
       {"image", TMSG_SVG_Image, "CreateSVG_Image"}
     }
    },

    {"connectors", 4,
     {
       {"line", TMSG_SVG_Line, "CreateSVG_Line"},
       {"simple_arrow", TMSG_SVG_SimpleArrow, "CreateSVG_SimpleArrow"},
       {"double_arrow", TMSG_SVG_DoubleArrow, "CreateSVG_DoubleArrow"},
       {"zigzag", TMSG_SVG_Zigzag, "CreateSVG_Zigzag"}
     }
    },

    {"basic_shapes", 12,
     {
       {"square", TMSG_SVG_Square, "CreateSVG_Square"},
       {"rounded_square", TMSG_SVG_RoundedSquare, "CreateSVG_RoundedSquare"},
       {"rectangle", TMSG_SVG_Rectangle, "CreateSVG_Rectangle"},
       {"rounded_rectangle", TMSG_SVG_RoundedRectangle, "CreateSVG_RoundedRectangle"},
       {"diamond", TMSG_SVG_Diamond, "CreateSVG_Diamond"},
       {"parallelogram", TMSG_SVG_Parallelogram, "CreateSVG_Parallelogram"},
       {"trapezium", TMSG_SVG_Trapezium, "CreateSVG_Trapezium"},
       {"equilateral_triangle", TMSG_SVG_EquilateralTriangle, "CreateSVG_EquilateralTriangle"},
       {"isosceles_triangle", TMSG_SVG_IsoscelesTriangle, "CreateSVG_IsoscelesTriangle"},
       {"right_triangle", TMSG_SVG_RightTriangle, "CreateSVG_RightTriangle"},
       {"circle", TMSG_SVG_Circle, "CreateSVG_Circle"},
       {"ellipse", TMSG_SVG_Ellipse, "CreateSVG_Ellipse"}
     }
    },

    {"curves", 4,
     {
       {"polyline", TMSG_SVG_Polyline, "CreateSVG_Polyline"},
       {"polygon", TMSG_SVG_Polygon, "CreateSVG_Polygon"},
       {"closed_bezier", TMSG_SVG_ClosedBezier, "CreateSVG_Closed"},
       {"bezier", TMSG_SVG_Bezier, "CreateSVG_Spline"}
     }
    },

    {"layers", 6,
     {
       {"group", TMSG_SVG_Group, "CreateSVG_Group"},
       {"ungroup", TMSG_SVG_Ungroup, "TransformSVG_Ungroup"},
       {"send_to_back", TMSG_SVG_SendToBack, "TransformSVG_SendToBack"},
       {"send_backward", TMSG_SVG_SendBackward, "TransformSVG_SendBackward"},
       {"bring_forward", TMSG_SVG_BringForward, "TransformSVG_BringForward"},
       {"bring_to_front", TMSG_SVG_BringToFront, "TransformSVG_BringToFront"}
     }
    },

    {"transform", 8,
     {
       {"flip_vertically", TMSG_SVG_FlipVertically, "TransformSVG_FlipVertically"},
       {"flip_horizontally", TMSG_SVG_FlipHorizontally, "TransformSVG_FlipHorizontally"},
       {"rotate_anticlockwise", TMSG_SVG_RotateAntiClockwise, "TransformSVG_RotateAntiClockwise"},
       {"rotate_clockwise", TMSG_SVG_RotateClockwise, "TransformSVG_RotateClockwise"},
       {"rotate", TMSG_SVG_Rotate, "TransformSVG_Rotate"},
       {"scale", TMSG_SVG_Scale, "TransformSVG_Scale"},
       {"skew", TMSG_SVG_Skew, "TransformSVG_Skew"},
       {"translate", TMSG_SVG_Translate, "TransformSVG_Translate"}
     }
    },

    {"align", 6,
     {
       {"left", TMSG_SVG_AlignLeft, "TransformSVG_AlignLeft"},
       {"center", TMSG_SVG_AlignCenter, "TransformSVG_AlignCenter"},
       {"right", TMSG_SVG_AlignRight, "TransformSVG_AlignRight"},
       {"top", TMSG_SVG_AlignTop, "TransformSVG_AlignTop"},
       {"middle", TMSG_SVG_AlignMiddle, "TransformSVG_AlignMiddle"},
       {"bottom", TMSG_SVG_AlignBottom, "TransformSVG_AlignBottom"}
     }
    },
   
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
  Event table and functions
****************************************************************************/
BEGIN_EVENT_TABLE(AmayaSVGPanel, wxPanel)
EVT_TOOL(XRCID("wxID_MENU_SVG_SPECIAL_OBJECTS"), AmayaSVGPanel::OnMenu0)
EVT_TOOL(XRCID("wxID_MENU_SVG_CONNECTORS"), AmayaSVGPanel::OnMenu1)
EVT_TOOL(XRCID("wxID_MENU_SVG_BASIC_SHAPES"), AmayaSVGPanel::OnMenu2)
EVT_TOOL(XRCID("wxID_MENU_SVG_CURVES"), AmayaSVGPanel::OnMenu3)
EVT_TOOL(XRCID("wxID_MENU_SVG_LAYERS"), AmayaSVGPanel::OnMenu4)
EVT_TOOL(XRCID("wxID_MENU_SVG_TRANSFORM"), AmayaSVGPanel::OnMenu5)
EVT_TOOL(XRCID("wxID_MENU_SVG_ALIGN"), AmayaSVGPanel::OnMenu6)
EVT_TOOL(XRCID("wxID_MENU_SVG_3D"), AmayaSVGPanel::OnMenu7)
EVT_TOOL(XRCID("wxID_MENU_SVG_BALLOONS"), AmayaSVGPanel::OnMenu8)
EVT_TOOL(XRCID("wxID_MENU_SVG_CHEMISTRY"), AmayaSVGPanel::OnMenu9)
EVT_TOOL(XRCID("wxID_MENU_SVG_CIRCUIT_DIAGRAM"), AmayaSVGPanel::OnMenu10)
EVT_TOOL(XRCID("wxID_MENU_SVG_POLYGONS_AND_STARS"), AmayaSVGPanel::OnMenu11)
EVT_TOOL(XRCID("wxID_MENU_SVG_OTHERS"), AmayaSVGPanel::OnMenu12)

EVT_MENU_RANGE(0, MAX_CONSTRUCTS_BY_DIRECTORY - 1,
	       AmayaSVGPanel::OnInsertElement)
END_EVENT_TABLE()

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

void AmayaSVGPanel::OnMenu6(wxCommandEvent& event)
{
  DisplayMenu(6);
}

void AmayaSVGPanel::OnMenu7(wxCommandEvent& event)
{
  DisplayMenu(7);
}

void AmayaSVGPanel::OnMenu8(wxCommandEvent& event)
{
  DisplayMenu(8);
}

void AmayaSVGPanel::OnMenu9(wxCommandEvent& event)
{
  DisplayMenu(9);
}

void AmayaSVGPanel::OnMenu10(wxCommandEvent& event)
{
  DisplayMenu(10);
}

void AmayaSVGPanel::OnMenu11(wxCommandEvent& event)
{
  DisplayMenu(11);
}

void AmayaSVGPanel::OnMenu12(wxCommandEvent& event)
{
  DisplayMenu(12);
}

void AmayaSVGPanel::OnMenu13(wxCommandEvent& event)
{
  DisplayMenu(13);
}

void AmayaSVGPanel::OnInsertElement(wxCommandEvent& event)
{
  InsertElement(last_menu, event.GetId());
}

/***************************************************************************
  DisplayMenu
    Display the i-th popup menu
****************************************************************************/
void AmayaSVGPanel::DisplayMenu(int i)
{
  svg_constructs e = list_of_constructs[i];
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
      if(e.list[j].file_name != NULL)
	{
	  sprintf(name, "%s/%s.png", e.directory, e.list[j].file_name);
	  path = TtaGetResourcePathWX(WX_RESOURCES_SVG, name);
	  if(icon.LoadFile(path, wxBITMAP_TYPE_PNG))
	    item->SetBitmap(icon);
	}

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
  svg_constructs e = list_of_constructs[i];

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
#endif /* #ifdef _WX */
