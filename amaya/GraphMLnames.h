
#ifdef GRAPHML
/* mapping table of GraphML elements */
#include "GraphML.h"
static ElemMapping    GraphMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {"XMLPI", SPACE, GraphML_EL_XMLPI, L_OtherValue, FALSE},
   {"XMLPI_line", SPACE, GraphML_EL_XMLPI_line, L_OtherValue, FALSE},
   {"XMLcomment", SPACE, GraphML_EL_XMLcomment, L_OtherValue, FALSE},
   {"XMLcomment_line", SPACE, GraphML_EL_XMLcomment_line, L_OtherValue, FALSE},
   {"a", SPACE, GraphML_EL_a, L_OtherValue, FALSE},
   {"c", SPACE, GraphML_EL_TEXT_UNIT, L_OtherValue, TRUE},
   {"circle", SPACE, GraphML_EL_circle, L_OtherValue, FALSE},
   {"defs", SPACE, GraphML_EL_defs, L_OtherValue, FALSE},
   {"desc", SPACE, GraphML_EL_desc, L_OtherValue, FALSE},
   {"ellipse", SPACE, GraphML_EL_ellipse, L_OtherValue, FALSE},
   {"foreignObject", SPACE, GraphML_EL_foreignObject, L_OtherValue, FALSE},
   {"g", SPACE, GraphML_EL_g, L_OtherValue, FALSE},
   {"image", SPACE, GraphML_EL_image, L_OtherValue, FALSE},
   {"line", SPACE, GraphML_EL_line_, L_OtherValue, FALSE},
   {"metadata", SPACE, GraphML_EL_metadata, L_OtherValue, FALSE},
   {"path", SPACE, GraphML_EL_path, L_OtherValue, FALSE},
   {"polygon", SPACE, GraphML_EL_polygon, L_OtherValue, FALSE},
   {"polyline", SPACE, GraphML_EL_polyline, L_OtherValue, FALSE},
   {"rect", SPACE, GraphML_EL_rect, L_OtherValue, FALSE},
   {"script", SPACE, GraphML_EL_script, L_OtherValue, FALSE},
   {"style", SPACE, GraphML_EL_style__, L_OtherValue, FALSE},
   {"svg", SPACE, GraphML_EL_GraphML, L_OtherValue, FALSE},
   {"symbol", SPACE, GraphML_EL_symbol_, L_OtherValue, FALSE},
   {"text", SPACE, GraphML_EL_text_, L_OtherValue, FALSE},
   {"title", SPACE, GraphML_EL_title, L_OtherValue, FALSE},
   {"tspan", SPACE, GraphML_EL_tspan, L_OtherValue, FALSE},
   {"use", SPACE, GraphML_EL_use_, L_OtherValue, FALSE},
   {"", SPACE, 0, L_OtherValue, FALSE} 	/* Last entry. Mandatory */
};

static AttributeMapping GraphMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {"unknown_attr", "", 'A', GraphML_ATTR_Invalid_attribute, L_OtherValue},

   {"class", "", 'A', GraphML_ATTR_class, L_OtherValue},
   {"cx", "", 'A', GraphML_ATTR_cx, L_OtherValue},
   {"cy", "", 'A', GraphML_ATTR_cy, L_OtherValue},
   {"d", "", 'A', GraphML_ATTR_d, L_OtherValue},
   {"dx", "", 'A', GraphML_ATTR_dx, L_OtherValue},
   {"dy", "", 'A', GraphML_ATTR_dy, L_OtherValue},
   {"externalResourcesRequired", "", 'A', GraphML_ATTR_externalResourcesRequired, L_OtherValue},
   {"fill", "", 'A', GraphML_ATTR_fill, L_OtherValue},
   {"height", "", 'A', GraphML_ATTR_height_, L_OtherValue},
   {"id", "", 'A', GraphML_ATTR_id, L_OtherValue},
   {"media", "", 'A', GraphML_ATTR_media, L_OtherValue},
   {"pathLength", "", 'A', GraphML_ATTR_pathLength, L_OtherValue},
   {"points", "", 'A', GraphML_ATTR_points, L_OtherValue},
   {"preserveAspectRatio", "", 'A', GraphML_ATTR_preserveAspectRatio, L_OtherValue},
   {"r", "", 'A', GraphML_ATTR_r, L_OtherValue},
   {"rx", "", 'A', GraphML_ATTR_rx, L_OtherValue},
   {"ry", "", 'A', GraphML_ATTR_ry, L_OtherValue},
   {"stroke", "", 'A', GraphML_ATTR_stroke, L_OtherValue},
   {"stroke-width", "", 'A', GraphML_ATTR_stroke_width, L_OtherValue},
   {"style", "", 'A', GraphML_ATTR_style_, L_OtherValue},
   {"target", "", 'A', GraphML_ATTR_target_, L_OtherValue},
   {"title", "", 'A', GraphML_ATTR_title_, L_OtherValue},
   {"transform", "", 'A', GraphML_ATTR_transform, L_OtherValue},
   {"type", "", 'A', GraphML_ATTR_type, L_OtherValue},
   {"viewBox", "", 'A', GraphML_ATTR_viewBox, L_OtherValue},
   {"width", "", 'A', GraphML_ATTR_width_, L_OtherValue},
   {"x", "", 'A', GraphML_ATTR_x, L_OtherValue},
   {"x1", "", 'A', GraphML_ATTR_x1, L_OtherValue},
   {"x2", "", 'A', GraphML_ATTR_x2, L_OtherValue},
   {"xlink:href", "", 'A', GraphML_ATTR_xlink_href, L_OtherValue},
   {"xml:space", "", 'A', GraphML_ATTR_xml_space, L_OtherValue},
   {"y", "", 'A', GraphML_ATTR_y, L_OtherValue},
   {"y1", "", 'A', GraphML_ATTR_y1, L_OtherValue},
   {"y2", "", 'A', GraphML_ATTR_y2, L_OtherValue},

   {"zzghost", "", 'A', GraphML_ATTR_Ghost_restruct, L_OtherValue},
   {"", "", EOS, 0, L_OtherValue}	  /* Last entry. Mandatory */
};
#else /* GRAPHML */
/* there is no mapping table of GraphML elements */

static ElemMapping *GraphMLElemMappingTable = NULL;
static AttributeMapping* GraphMLAttributeMappingTable = NULL;
#endif /* GRAPHML */
