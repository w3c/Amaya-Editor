/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
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

#include "AmayaSVGPanel.h"
#include "AmayaNormalWindow.h"

//
//
// AmayaSVGPanel
//
//

static
AMAYA_BEGIN_TOOLBAR_DEF_TABLE(AmayaSVGToolDef)
  AMAYA_TOOLBAR_DEF("wxID_SVG_TEXT", "CreateSVG_Text", LIB, TMSG_SVG_Text)
  AMAYA_TOOLBAR_DEF("wxID_SVG_IMG", "CreateSVG_Image", LIB, TMSG_SVG_Image)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_SELECT", "EditSVG_Select", LIB, TMSG_SEL)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_GROUP", "CreateSVG_Group", LIB, TMSG_SVG_Group)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_SVG_UNGROUP", "TransformSVG_Ungroup", LIB, TMSG_SVG_Ungroup)
  AMAYA_END_TOOLBAR_DEF_TABLE()

  AmayaSVGPanel::AmayaSVGPanel():
  wxPanel()
{
}

/*----------------------------------------------------------------------------
  AmayaSVGPanel
  ----------------------------------------------------------------------------*/
AmayaSVGPanel::AmayaSVGPanel(wxWindow* parent, wxWindowID id,
			     const wxPoint& pos, 
			     const wxSize& size, long style,
			     const wxString& name, wxObject* extra):
  wxPanel()
{
  Create(parent, id, pos, size, style, name, extra);
}

/*----------------------------------------------------------------------------
  ~AmayaSVGPanel
  ----------------------------------------------------------------------------*/
AmayaSVGPanel::~AmayaSVGPanel()
{
}

/*----------------------------------------------------------------------------
  Create the panel
  ----------------------------------------------------------------------------*/
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

  m_tbar1->Add(AmayaSVGToolDef);
  m_tbar1->Realize();
  m_tbar2->Add(AmayaSVGToolDef);
  m_tbar2->Realize();
  m_tbar3->Add(AmayaSVGToolDef);
  m_tbar3->Realize();
  Fit();
  SetAutoLayout(true);
  
  return true;
}

/*----------------------------------------------------------------------------
  SVG constructions.
  ----------------------------------------------------------------------------*/

/* Number maximum of constructs in one directory */
#define MAX_CONSTRUCTS_BY_DIRECTORY 20

/* Structure of an SVG contruct */
typedef struct svg_construct_
{
  const    char *file_name;
  int      msg_id;
  const    char *function_name;
  ThotBool fill;
} svg_construct;

/* Structure of a list of a set of constructs */
typedef struct svg_constructs_
{
  const         char* directory;
  int           length;
  svg_construct list[MAX_CONSTRUCTS_BY_DIRECTORY];
} svg_constructs;

/* list of all constructions */
static svg_constructs list_of_constructs[] =
  {
    {"", 4,
     {
       {"document_svg", TMSG_SVG_SvgCanvas, "CreateSVG_Svg", FALSE},
       //{"SVG_text", TMSG_SVG_Text, "CreateSVG_Text", FALSE},
       {"SVG_foreign_object", TMSG_SVG_ForeigObject, "CreateSVG_ForeignObject", FALSE},
       //{"document_image", TMSG_SVG_Image, "CreateSVG_Image", FALSE},
       {NULL, TMSG_SVG_Information, "EditSVG_Information", FALSE},
       {NULL, TMSG_SVG_GenerateDescription, "EditSVG_GenerateDescription", FALSE}
     }
    },

    {"", 4,
     {
       {"SVG_line", TMSG_SVG_Line, "CreateSVG_Line", FALSE},
       {"SVG_simple_arrow", TMSG_SVG_SimpleArrow, "CreateSVG_StartArrow", FALSE},
       {"SVG_simple_earrow", TMSG_SVG_SimpleArrow, "CreateSVG_EndArrow", FALSE},
       {"SVG_double_arrow", TMSG_SVG_DoubleArrow, "CreateSVG_DoubleArrow", FALSE},
     }
    },

    {"", 4,
     {
       {"SVG_polyline", TMSG_SVG_Polyline, "CreateSVG_Polyline", FALSE},
       {"SVG_polygon", TMSG_SVG_Polygon, "CreateSVG_Polygon", TRUE},
       {"SVG_bezier", TMSG_SVG_Bezier, "CreateSVG_Spline", FALSE},
       {"SVG_closed_bezier", TMSG_SVG_ClosedBezier, "CreateSVG_Closed", TRUE}
     }
    },

    {"", 12,
     {
       {"SVG_square", TMSG_SVG_Square, "CreateSVG_Square", TRUE},
       {"SVG_rounded_square",
        TMSG_SVG_RoundedSquare, "CreateSVG_RoundedSquare", TRUE},
       {"SVG_rectangle", TMSG_SVG_Rectangle, "CreateSVG_Rectangle", TRUE},
       {"SVG_rounded_rectangle",
        TMSG_SVG_RoundedRectangle, "CreateSVG_RoundedRectangle", TRUE},
       {"SVG_diamond", TMSG_SVG_Diamond, "CreateSVG_Diamond", TRUE},
       {"SVG_parallelogram", TMSG_SVG_Parallelogram, "CreateSVG_Parallelogram", TRUE},
       {"SVG_trapezium", TMSG_SVG_Trapezium, "CreateSVG_Trapezium", TRUE},
       {"SVG_equilateral_triangle",
        TMSG_SVG_EquilateralTriangle, "CreateSVG_EquilateralTriangle", TRUE},
       {"SVG_isosceles_triangle",
        TMSG_SVG_IsoscelesTriangle, "CreateSVG_IsoscelesTriangle", TRUE},
       {"SVG_right_triangle",
        TMSG_SVG_RightTriangle, "CreateSVG_RectangleTriangle", TRUE},
       {"SVG_circle", TMSG_SVG_Circle, "CreateSVG_Circle", TRUE},
       {"SVG_ellipse", TMSG_SVG_Ellipse, "CreateSVG_Ellipse", TRUE}
     }
    },

    {"", 4,
     {
       {"SVG_send_to_back", TMSG_SVG_SendToBack, "TransformSVG_SendToBack", FALSE},
       {"SVG_send_backward",
        TMSG_SVG_SendBackward, "TransformSVG_SendBackward", FALSE},
       {"SVG_bring_forward",
        TMSG_SVG_BringForward, "TransformSVG_BringForward", FALSE},
       {"SVG_bring_to_front", TMSG_SVG_BringToFront, "TransformSVG_BringToFront", FALSE}
     }
    },

    {"", 8,
     {
       {"SVG_flip_vertically",
        TMSG_SVG_FlipVertically, "TransformSVG_FlipVertically", FALSE},
       {"SVG_flip_horizontally",
        TMSG_SVG_FlipHorizontally, "TransformSVG_FlipHorizontally", FALSE},
       {"SVG_rotate_anticlockwise",
        TMSG_SVG_RotateAntiClockwise, "TransformSVG_AntiClockWise", FALSE},
       {"SVG_rotate_clockwise",
        TMSG_SVG_RotateClockwise, "TransformSVG_ClockWise", FALSE},
       {"SVG_rotate", TMSG_SVG_Rotate, "TransformSVG_Rotate", FALSE},
       {"SVG_scale", TMSG_SVG_Scale, "TransformSVG_Scale", FALSE},
       {"SVG_skew", TMSG_SVG_Skew, "TransformSVG_Skew", FALSE},
       {"SVG_translate", TMSG_SVG_Translate, "TransformSVG_Translate", FALSE}
     }
    },

    {"", 6,
     {
       {"SVG_align_left", TMSG_SVG_AlignLeft, "TransformSVG_AlignLeft", FALSE},
       {"SVG_align_center", TMSG_SVG_AlignCenter, "TransformSVG_AlignCenter", FALSE},
       {"SVG_align_right", TMSG_SVG_AlignRight, "TransformSVG_AlignRight", FALSE},
       {"SVG_align_top", TMSG_SVG_AlignTop, "TransformSVG_AlignTop", FALSE},
       {"SVG_align_middle", TMSG_SVG_AlignMiddle, "TransformSVG_AlignMiddle", FALSE},
       {"SVG_align_bottom", TMSG_SVG_AlignBottom, "TransformSVG_AlignBottom", FALSE}
     }
    },

    {"", 8,
     {
       {"SVG_distribute_left",
	TMSG_SVG_DistributeLeft, "TransformSVG_DistributeLeft", FALSE},
       {"SVG_distribute_center",
	TMSG_SVG_DistributeCenter, "TransformSVG_DistributeCenter", FALSE},
       {"SVG_distribute_right",
	TMSG_SVG_DistributeRight, "TransformSVG_DistributeRight", FALSE},
       {"SVG_distribute_top",
	TMSG_SVG_DistributeTop, "TransformSVG_DistributeTop", FALSE},
       {"SVG_distribute_middle",
	TMSG_SVG_DistributeMiddle, "TransformSVG_DistributeMiddle", FALSE},
       {"SVG_distribute_bottom",
	TMSG_SVG_DistributeBottom, "TransformSVG_DistributeBottom", FALSE},
       {"SVG_distribute_horizontal_spacing",
	TMSG_SVG_DistributeHSpacing, "TransformSVG_DistributeHSpacing", FALSE},
       {"SVG_distribute_vertical_spacing",
	TMSG_SVG_DistributeVSpacing, "TransformSVG_DistributeVSpacing", FALSE}
     }
    },
   
    {"3d", 6,
     {
       {"cone", TMSG_SVG_Cone, NULL, TRUE},
       {"cube", TMSG_SVG_Cube, NULL, TRUE},
       {"cylinder", TMSG_SVG_Cylinder, NULL, TRUE},
       {"octahedron", TMSG_SVG_Octahedron, NULL, TRUE},
       {"parallelepiped", TMSG_SVG_Parallelepiped, NULL, TRUE},
       {"pyramid", TMSG_SVG_Pyramid, NULL, TRUE}
     }
    },

    {"balloons", 6,
     {
       {"rectangular", TMSG_SVG_Rectangular, NULL, TRUE},
       {"round", TMSG_SVG_Round, NULL, TRUE},
       {"rounded_rectangular", TMSG_SVG_Rounded_rectangular, NULL, TRUE},
       {"scream", TMSG_SVG_Scream, NULL, TRUE},
       {"thought", TMSG_SVG_Thought, NULL, TRUE},
       {"cloud", TMSG_SVG_Cloud, NULL, TRUE}
     }
    },

    {"block_arrows", 20,
     {
       {"simple_arrow", TMSG_SVG_Simple_Arrow, NULL, TRUE},
       {"double_arrow", TMSG_SVG_Double_Arrow, NULL, TRUE},
       {"3-way_arrow", TMSG_SVG_3_Way_Arrow, NULL, TRUE},
       {"4-way_arrow", TMSG_SVG_4_Way_Arrow, NULL, TRUE},
       {"corner_arrow", TMSG_SVG_Corner_Arrow, NULL, TRUE},
       {"corner_double_arrow", TMSG_SVG_Corner_Double_Arrow, NULL, TRUE},

       {"simple_arrow_callout", TMSG_SVG_Simple_Arrow_Callout, NULL, TRUE},
       {"double_arrow_callout", TMSG_SVG_Double_Arrow_Callout, NULL, TRUE},
       {"3-way_arrow_callout", TMSG_SVG_3_Way_Arrow_Callout, NULL, TRUE},
       {"4-way_arrow_callout", TMSG_SVG_4_Way_Arrow_Callout, NULL, TRUE},
       {"corner_double_arrow_callout",
        TMSG_SVG_Corner_Double_Arrow_Callout, NULL, TRUE},

       {"S-shape_arrow", TMSG_SVG_S_Shape_Arrow, NULL, TRUE},
       {"rounded_arrow", TMSG_SVG_Rounded_Arrow, NULL, TRUE},
       {"circular_arrow", TMSG_SVG_Circular_Arrow, NULL, TRUE},
       {"2-way_rounded_arrow", TMSG_SVG_2_Way_Rounded_Arrow, NULL, TRUE},

       {"split_arrow", TMSG_SVG_Split_Arrow, NULL, TRUE},
       {"pentagon", TMSG_SVG_Pentagon_Arrow, NULL, TRUE},
       {"chevron", TMSG_SVG_Chevron, NULL, TRUE},
       {"striped_arrow", TMSG_SVG_Striped_Arrow, NULL, TRUE},
       {"notched_arrow", TMSG_SVG_Notched_Arrow, NULL, TRUE}
     }
    },

    {"chemistry", 9,
     {
       {"beaker", TMSG_SVG_Beaker, NULL, TRUE},
       {"boiling_flask", TMSG_SVG_Boiling_flask, NULL, TRUE},
       {"buchner_flask", TMSG_SVG_Buchner_flask, NULL, TRUE},
       {"burette", TMSG_SVG_Burette, NULL, TRUE},
       {"erlenmeyer_flask", TMSG_SVG_Erlenmeyer_flask, NULL, TRUE},
       {"pipette", TMSG_SVG_Pipette, NULL, FALSE},
       {"test_tube", TMSG_SVG_Test_tube, NULL, TRUE},
       {"funnel", TMSG_SVG_Funnel, NULL, FALSE},
       {"bunsen_burner", TMSG_SVG_Bunsen_burner, NULL, TRUE}
     }
    },

    {"circuit_diagram", 16,
     {
       {"current_source", TMSG_SVG_Current_source, NULL, FALSE},
       {"current_source2", TMSG_SVG_Current_source, NULL, FALSE},
       {"voltage_source", TMSG_SVG_Voltage_source, NULL, FALSE},
       {"voltage_source2", TMSG_SVG_Voltage_source, NULL, FALSE},
       {"ground_point", TMSG_SVG_Ground_point, NULL, FALSE},
       {"resistor", TMSG_SVG_Resistor, NULL, FALSE},
       {"resistor2", TMSG_SVG_Resistor, NULL, FALSE},
       {"switch", TMSG_SVG_Switch, NULL, FALSE},
       {"transistor", TMSG_SVG_Transistor, NULL, FALSE},
       {"capacitor", TMSG_SVG_Capacitor, NULL, FALSE},
       {"inductor", TMSG_SVG_Inductor, NULL, FALSE},
       {"op-amp", TMSG_SVG_Op_amp, NULL, FALSE},
       {"op-amp2", TMSG_SVG_Op_amp, NULL, FALSE},
       {"led", TMSG_SVG_Led, NULL, FALSE},
       {"diode", TMSG_SVG_Diode, NULL, FALSE},
       {"zener_diode", TMSG_SVG_Zener_diode, NULL, FALSE}
     }
    },

    {"polygons_and_stars", 7,
     {
       {"pentagon", TMSG_SVG_Pentagon, NULL, TRUE},
       {"hexagon", TMSG_SVG_Hexagon, NULL, TRUE},
       {"heptagon", TMSG_SVG_Heptagon, NULL, TRUE},
       {"octogon", TMSG_SVG_Octogon, NULL, TRUE},
       {"star4", TMSG_SVG_Star4, NULL, TRUE},
       {"star5", TMSG_SVG_Star5, NULL, TRUE},
       {"star6", TMSG_SVG_Star6, NULL, TRUE}
     }
    },

    {"logic_gates", 7,
     {
       {"and", TMSG_SVG_AND, NULL, FALSE},
       {"nand", TMSG_SVG_NAND, NULL, FALSE},
       {"or", TMSG_SVG_OR, NULL, FALSE},
       {"nor", TMSG_SVG_NOR, NULL, FALSE},
       {"xor", TMSG_SVG_XOR, NULL, FALSE},
       {"xnor", TMSG_SVG_XNOR, NULL, FALSE},
       {"not", TMSG_SVG_NOT, NULL, FALSE}
     }
    },

    {"", 12,
     {
       {"scroll", TMSG_SVG_Scroll, NULL, TRUE},
       {"sun", TMSG_SVG_Sun, NULL, TRUE},
       {"cloud", TMSG_SVG_Cloud, NULL, TRUE},
       {"lightning_bolt", TMSG_SVG_Lightning_Bolt, NULL, TRUE},
       {"moon", TMSG_SVG_Moon, NULL, TRUE},
       {"smiley", TMSG_SVG_Smiley, NULL, TRUE},
       {"heart", TMSG_SVG_Heart, NULL, TRUE},
       {"flower", TMSG_SVG_Flower, NULL, TRUE},
       {"cross", TMSG_SVG_Cross, NULL, TRUE},
       {"frame", TMSG_SVG_Frame, NULL, TRUE},
       {"ring", TMSG_SVG_Ring, NULL, TRUE},
       {"prohibited", TMSG_SVG_Prohibited, NULL, TRUE}
     }
    }
    
  };

/*----------------------------------------------------------------------------
  AmayaSVGPanel

  Event table and functions
  ----------------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaSVGPanel, wxPanel)
EVT_TOOL(XRCID("wxID_MENU_SVG_SPECIAL_OBJECTS"), AmayaSVGPanel::OnMenu0)
EVT_TOOL(XRCID("wxID_MENU_SVG_CONNECTORS"), AmayaSVGPanel::OnMenu1)
EVT_TOOL(XRCID("wxID_MENU_SVG_CURVES"), AmayaSVGPanel::OnMenu2)
EVT_TOOL(XRCID("wxID_MENU_SVG_BASIC_SHAPES"), AmayaSVGPanel::OnMenu3)
EVT_TOOL(XRCID("wxID_MENU_SVG_LAYERS"), AmayaSVGPanel::OnMenu4)
EVT_TOOL(XRCID("wxID_MENU_SVG_TRANSFORM"), AmayaSVGPanel::OnMenu5)
EVT_TOOL(XRCID("wxID_MENU_SVG_ALIGN"), AmayaSVGPanel::OnMenu6)
EVT_TOOL(XRCID("wxID_MENU_SVG_DISTRIBUTE"), AmayaSVGPanel::OnMenu7)
EVT_TOOL(XRCID("wxID_MENU_SVG_3D"), AmayaSVGPanel::OnMenu8)
EVT_TOOL(XRCID("wxID_MENU_SVG_BALLOONS"), AmayaSVGPanel::OnMenu9)
EVT_TOOL(XRCID("wxID_MENU_SVG_BLOCK_ARROWS"), AmayaSVGPanel::OnMenu10)
EVT_TOOL(XRCID("wxID_MENU_SVG_CHEMISTRY"), AmayaSVGPanel::OnMenu11)
EVT_TOOL(XRCID("wxID_MENU_SVG_CIRCUIT_DIAGRAM"), AmayaSVGPanel::OnMenu12)
EVT_TOOL(XRCID("wxID_MENU_SVG_POLYGONS_AND_STARS"), AmayaSVGPanel::OnMenu13)
EVT_TOOL(XRCID("wxID_MENU_SVG_LOGIC_GATES"), AmayaSVGPanel::OnMenu14)
EVT_TOOL(XRCID("wxID_MENU_SVG_OTHERS"), AmayaSVGPanel::OnMenu15)

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

void AmayaSVGPanel::OnMenu14(wxCommandEvent& event)
{
  DisplayMenu(14);
}

void AmayaSVGPanel::OnMenu15(wxCommandEvent& event)
{
  DisplayMenu(15);
}

void AmayaSVGPanel::OnInsertElement(wxCommandEvent& event)
{
  InsertElement(last_menu, event.GetId());
}

/*----------------------------------------------------------------------------
  DisplayMenu
    Display the i-th popup menu
  ----------------------------------------------------------------------------*/
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

  for (j = 0; j < e.length; j++)
    {
      /* Create a new menu item */
      item = new wxMenuItem(&menu, j,
			    TtaConvMessageToWX(TtaGetMessage(LIB, e.list[j].msg_id)));

      /* Add an icon if the png file exists  */
      if (e.list[j].file_name != NULL)
        {
          sprintf(name, "%s/%s.png", e.directory, e.list[j].file_name);
          if (e.list[j].function_name == NULL)
            /* It's an SVG template: look in svg/ */
            path = TtaGetResourcePathWX(WX_RESOURCES_SVG, name);
          else
            /* It's an SVG command: look in 16x16 */
            path = TtaGetResourcePathWX(WX_RESOURCES_ICON_16X16, name);
          
          if (icon.LoadFile(path, wxBITMAP_TYPE_PNG))
            item->SetBitmap(icon);
        }

      /* Add the item in the menu */
      menu.Append(item);
    }

  /* Display the menu */
  PopupMenu(&menu);
}

/*----------------------------------------------------------------------------
  InsertElement
    i = menu number
    j = submenu number
  ----------------------------------------------------------------------------*/
void AmayaSVGPanel::InsertElement(int i, int j)
{
  char name[MAX_LENGTH];
  wxString path;

  wxMenu menu;
  svg_constructs e = list_of_constructs[i];

  Document doc;
  View view;
  TtaGiveActiveView( &doc, &view );

  if (doc > 0 && j >= 0 && j < e.length)
    {
      LastSVGelementIsFilled = e.list[j].fill;
      if (e.list[j].function_name == NULL)
        {
          /* No function name is given: call the creation of an svg template */
          sprintf(name, "%s/%s.svg", e.directory, e.list[j].file_name);
          path = TtaGetResourcePathWX(WX_RESOURCES_SVG, name);
          LastSVGelement = TtaStrdup(path.mb_str(wxConvUTF8));
          LastSVGelementTitle = TtaStrdup(TtaGetMessage(LIB, e.list[j].msg_id));
          TtaExecuteMenuAction ("CreateSVG_Template", doc, view, TRUE);
        }
      else
        {
          /* Call the function */
          TtaExecuteMenuAction (e.list[j].function_name, doc, view, TRUE);
        }
    }
}
#endif /* #ifdef _WX */
