
#ifdef GRAPHML
/* mapping table of GraphML elements */
#include "GraphML.h"
static ElemMapping    GraphMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {TEXT("XMLPI"), SPACE, GraphML_EL_XMLPI, L_Other, FALSE},
   {TEXT("XMLPI_line"), SPACE, GraphML_EL_XMLPI_line, L_Other, FALSE},
   {TEXT("XMLcomment"), SPACE, GraphML_EL_XMLcomment, L_Other, FALSE},
   {TEXT("XMLcomment_line"), SPACE, GraphML_EL_XMLcomment_line, L_Other, FALSE},
   {TEXT("circle"), SPACE, GraphML_EL_circle, L_Other, FALSE},
   {TEXT("closedspline"), SPACE, GraphML_EL_ClosedSpline, L_Other, FALSE},
   {TEXT("desc"), SPACE, GraphML_EL_desc, L_Other, FALSE},
   {TEXT("ellipse"), SPACE, GraphML_EL_ellipse, L_Other, FALSE},
   {TEXT("foreignObject"), SPACE, GraphML_EL_foreignObject, L_Other, FALSE},
   {TEXT("g"), SPACE, GraphML_EL_g, L_Other, FALSE},
   {TEXT("image"), SPACE, GraphML_EL_image, L_Other, FALSE},
   {TEXT("line"), SPACE, GraphML_EL_line_, L_Other, FALSE},
   {TEXT("metadata"), SPACE, GraphML_EL_metadata, L_Other, FALSE},
   {TEXT("polygon"), SPACE, GraphML_EL_polygon, L_Other, FALSE},
   {TEXT("polyline"), SPACE, GraphML_EL_polyline, L_Other, FALSE},
   {TEXT("rect"), SPACE, GraphML_EL_rect, L_Other, FALSE},
   {TEXT("spline"), SPACE, GraphML_EL_Spline, L_Other, FALSE},
   {TEXT("svg"), SPACE, GraphML_EL_GraphML, L_Other, FALSE},
   {TEXT("text"), SPACE, GraphML_EL_text_, L_Other, FALSE},
   {TEXT("title"), SPACE, GraphML_EL_title, L_Other, FALSE},
   {TEXT("tspan"), SPACE, GraphML_EL_tspan, L_Other, FALSE},
   {TEXT(""), SPACE, 0, L_Other, FALSE} 	/* Last entry. Mandatory */
};

static AttributeMapping GraphMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {TEXT("unknown_attr"), TEXT(""), 'A', GraphML_ATTR_Invalid_attribute, L_Other},

   {TEXT("arrowhead"), TEXT(""), 'A', GraphML_ATTR_arrowhead, L_Other},
   {TEXT("class"), TEXT(""), 'A', GraphML_ATTR_class, L_Other},
   {TEXT("cx"), TEXT(""), 'A', GraphML_ATTR_cx, L_Other},
   {TEXT("cy"), TEXT(""), 'A', GraphML_ATTR_cy, L_Other},
   {TEXT("dx"), TEXT(""), 'A', GraphML_ATTR_dx, L_Other},
   {TEXT("dy"), TEXT(""), 'A', GraphML_ATTR_dy, L_Other},
   {TEXT("fill"), TEXT(""), 'A', GraphML_ATTR_fill, L_Other},
   {TEXT("height"), TEXT(""), 'A', GraphML_ATTR_height_, L_Other},
   {TEXT("id"), TEXT(""), 'A', GraphML_ATTR_id, L_Other},
   {TEXT("linestyle"), TEXT(""), 'A', GraphML_ATTR_linestyle_, L_Other},
   {TEXT("points"), TEXT(""), 'A', GraphML_ATTR_points, L_Other},
   {TEXT("r"), TEXT(""), 'A', GraphML_ATTR_r, L_Other},
   {TEXT("rx"), TEXT(""), 'A', GraphML_ATTR_rx, L_Other},
   {TEXT("ry"), TEXT(""), 'A', GraphML_ATTR_ry, L_Other},
   {TEXT("stroke"), TEXT(""), 'A', GraphML_ATTR_stroke, L_Other},
   {TEXT("stroke-width"), TEXT(""), 'A', GraphML_ATTR_stroke_width, L_Other},
   {TEXT("style"), TEXT(""), 'A', GraphML_ATTR_style_, L_Other},
   {TEXT("transform"), TEXT(""), 'A', GraphML_ATTR_transform, L_Other},
   {TEXT("viewBox"), TEXT(""), 'A', GraphML_ATTR_viewBox, L_Other},
   {TEXT("width"), TEXT(""), 'A', GraphML_ATTR_width_, L_Other},
   {TEXT("x"), TEXT(""), 'A', GraphML_ATTR_x, L_Other},
   {TEXT("x1"), TEXT(""), 'A', GraphML_ATTR_x1, L_Other},
   {TEXT("x2"), TEXT(""), 'A', GraphML_ATTR_x2, L_Other},
   {TEXT("xlink:href"), TEXT("image"), 'A', GraphML_ATTR_xlink_href, L_Other},
   {TEXT("xml:space"), TEXT(""), 'A', GraphML_ATTR_xml_space, L_Other},
   {TEXT("y"), TEXT(""), 'A', GraphML_ATTR_y, L_Other},
   {TEXT("y1"), TEXT(""), 'A', GraphML_ATTR_y1, L_Other},
   {TEXT("y2"), TEXT(""), 'A', GraphML_ATTR_y2, L_Other},

   {TEXT("zzghost"), TEXT(""), 'A', GraphML_ATTR_Ghost_restruct, L_Other},
   {TEXT(""), TEXT(""), EOS, 0, L_Other}	  /* Last entry. Mandatory */
};
#else /* GRAPHML */
/* there is no mapping table of GraphML elements */

static ElemMapping *GraphMLElemMappingTable = NULL;
static AttributeMapping* GraphMLAttributeMappingTable = NULL;
#endif /* GRAPHML */
