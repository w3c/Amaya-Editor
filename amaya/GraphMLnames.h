
#ifdef GRAPHML
/* mapping table of GraphML elements */
#include "GraphML.h"
static ElemMapping    GraphMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {TEXT("XMLcomment"), SPACE, GraphML_EL_XMLcomment, L_Other},
   {TEXT("XMLcomment_line"), SPACE, GraphML_EL_XMLcomment_line, L_Other},
   {TEXT("circle"), SPACE, GraphML_EL_Circle, L_Other},
   {TEXT("closedspline"), SPACE, GraphML_EL_ClosedSpline, L_Other},
   {TEXT("desc"), SPACE, GraphML_EL_desc, L_Other},
   {TEXT("group"), SPACE, GraphML_EL_Group, L_Other},
   {TEXT("label"), 'X', GraphML_EL_Label, L_Other}, /* see function GraphMLGetDTDName */
   {TEXT("line"), 'E', GraphML_EL_Line_, L_Other},
   {TEXT("math"), 'X', GraphML_EL_Math, L_Other},   /* see function GraphMLGetDTDName */
   {TEXT("metadata"), SPACE, GraphML_EL_metadata, L_Other},
   {TEXT("oval"), SPACE, GraphML_EL_Oval, L_Other},
   {TEXT("polygon"), SPACE, GraphML_EL_Polygon, L_Other},
   {TEXT("polyline"), 'E', GraphML_EL_Polyline, L_Other},
   {TEXT("rect"), SPACE, GraphML_EL_Rectangle, L_Other},
   {TEXT("roundrect"), SPACE, GraphML_EL_RoundRect, L_Other},
   {TEXT("spline"), 'E', GraphML_EL_Spline, L_Other},
   {TEXT("svg"), SPACE, GraphML_EL_GraphML, L_Other},
   {TEXT("text"), 'X', GraphML_EL_Text_, L_Other},  /* see function GraphMLGetDTDName */
   {TEXT("title"), SPACE, GraphML_EL_title, L_Other},
   {TEXT("xmlgraphics"), SPACE, GraphML_EL_GraphML, L_Other},
   {TEXT(""), SPACE, 0, L_Other}	/* Last entry. Mandatory */
};

static AttributeMapping GraphMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {TEXT("unknown_attr"), TEXT(""), 'A', GraphML_ATTR_Invalid_attribute, L_Other},

   {TEXT("align"), TEXT(""), 'A', GraphML_ATTR_align, L_Other},
   {TEXT("arrowhead"), TEXT(""), 'A', GraphML_ATTR_arrowhead, L_Other},
   {TEXT("class"), TEXT(""), 'A', GraphML_ATTR_class, L_Other},
   {TEXT("depth"), TEXT(""), 'A', GraphML_ATTR_depth_, L_Other},
   {TEXT("direction"), TEXT(""), 'A', GraphML_ATTR_direction, L_Other},
   {TEXT("fill"), TEXT(""), 'A', GraphML_ATTR_fill, L_Other},
   {TEXT("height"), TEXT(""), 'A', GraphML_ATTR_height_, L_Other},
   {TEXT("hspace"), TEXT(""), 'A', GraphML_ATTR_hspace, L_Other},
   {TEXT("id"), TEXT(""), 'A', GraphML_ATTR_id, L_Other},
   {TEXT("linestyle"), TEXT(""), 'A', GraphML_ATTR_linestyle_, L_Other},
   {TEXT("linewidth"), TEXT(""), 'A', GraphML_ATTR_linewidth, L_Other},
   {TEXT("position"), TEXT(""), 'A', GraphML_ATTR_position, L_Other},
   {TEXT("points"), TEXT(""), 'A', GraphML_ATTR_points, L_Other},
   {TEXT("stroke"), TEXT(""), 'A', GraphML_ATTR_stroke, L_Other},
   {TEXT("stroke-width"), TEXT(""), 'A', GraphML_ATTR_stroke_width, L_Other},
   {TEXT("style"), TEXT(""), 'A', GraphML_ATTR_style_, L_Other},
   {TEXT("valign"), TEXT(""), 'A', GraphML_ATTR_valign, L_Other},
   {TEXT("vspace"), TEXT(""), 'A', GraphML_ATTR_vspace, L_Other},
   {TEXT("width"), TEXT(""), 'A', GraphML_ATTR_width_, L_Other},
   {TEXT("x"), TEXT(""), 'A', GraphML_ATTR_x, L_Other},
   {TEXT("y"), TEXT(""), 'A', GraphML_ATTR_y, L_Other},
   {TEXT("zzghost"), TEXT(""), 'A', GraphML_ATTR_Ghost_restruct, L_Other},
   {TEXT(""), TEXT(""), EOS, 0, L_Other}		/* Last entry. Mandatory */
};
#else /* GRAPHML */
/* there is no mapping table of GraphML elements */

static ElemMapping *GraphMLElemMappingTable = NULL;
static AttributeMapping* GraphMLAttributeMappingTable = NULL;
#endif /* GRAPHML */
