
#ifdef GRAPHML
/* mapping table of GraphML elements */
#include "GraphML.h"
static ElemMapping    GraphMLElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {"XMLPI", SPACE, GraphML_EL_XMLPI, L_Other, FALSE},
   {"XMLPI_line", SPACE, GraphML_EL_XMLPI_line, L_Other, FALSE},
   {"XMLcomment", SPACE, GraphML_EL_XMLcomment, L_Other, FALSE},
   {"XMLcomment_line", SPACE, GraphML_EL_XMLcomment_line, L_Other, FALSE},
   {"a", SPACE, GraphML_EL_a, L_Other, FALSE},
   {"c", SPACE, GraphML_EL_TEXT_UNIT, L_Other, TRUE},
   {"circle", SPACE, GraphML_EL_circle, L_Other, FALSE},
   {"defs", SPACE, GraphML_EL_defs, L_Other, FALSE},
   {"desc", SPACE, GraphML_EL_desc, L_Other, FALSE},
   {"ellipse", SPACE, GraphML_EL_ellipse, L_Other, FALSE},
   {"foreignObject", SPACE, GraphML_EL_foreignObject, L_Other, FALSE},
   {"g", SPACE, GraphML_EL_g, L_Other, FALSE},
   {"image", SPACE, GraphML_EL_image, L_Other, FALSE},
   {"line", SPACE, GraphML_EL_line_, L_Other, FALSE},
   {"metadata", SPACE, GraphML_EL_metadata, L_Other, FALSE},
   {"path", SPACE, GraphML_EL_path, L_Other, FALSE},
   {"polygon", SPACE, GraphML_EL_polygon, L_Other, FALSE},
   {"polyline", SPACE, GraphML_EL_polyline, L_Other, FALSE},
   {"rect", SPACE, GraphML_EL_rect, L_Other, FALSE},
   {"script", SPACE, GraphML_EL_script, L_Other, FALSE},
   {"style", SPACE, GraphML_EL_style__, L_Other, FALSE},
   {"svg", SPACE, GraphML_EL_GraphML, L_Other, FALSE},
   {"symbol", SPACE, GraphML_EL_symbol_, L_Other, FALSE},
   {"text", SPACE, GraphML_EL_text_, L_Other, FALSE},
   {"title", SPACE, GraphML_EL_title, L_Other, FALSE},
   {"tspan", SPACE, GraphML_EL_tspan, L_Other, FALSE},
   {"use", SPACE, GraphML_EL_use_, L_Other, FALSE},
   {"", SPACE, 0, L_Other, FALSE} 	/* Last entry. Mandatory */
};

static AttributeMapping GraphMLAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {"unknown_attr", "", 'A', GraphML_ATTR_Invalid_attribute, L_Other},

   {"class", "", 'A', GraphML_ATTR_class, L_Other},
   {"cx", "", 'A', GraphML_ATTR_cx, L_Other},
   {"cy", "", 'A', GraphML_ATTR_cy, L_Other},
   {"d", "", 'A', GraphML_ATTR_d, L_Other},
   {"dx", "", 'A', GraphML_ATTR_dx, L_Other},
   {"dy", "", 'A', GraphML_ATTR_dy, L_Other},
   {"externalResourcesRequired", "", 'A', GraphML_ATTR_externalResourcesRequired, L_Other},
   {"fill", "", 'A', GraphML_ATTR_fill, L_Other},
   {"height", "", 'A', GraphML_ATTR_height_, L_Other},
   {"id", "", 'A', GraphML_ATTR_id, L_Other},
   {"media", "", 'A', GraphML_ATTR_media, L_Other},
   {"pathLength", "", 'A', GraphML_ATTR_pathLength, L_Other},
   {"points", "", 'A', GraphML_ATTR_points, L_Other},
   {"preserveAspectRatio", "", 'A', GraphML_ATTR_preserveAspectRatio, L_Other},
   {"r", "", 'A', GraphML_ATTR_r, L_Other},
   {"rx", "", 'A', GraphML_ATTR_rx, L_Other},
   {"ry", "", 'A', GraphML_ATTR_ry, L_Other},
   {"stroke", "", 'A', GraphML_ATTR_stroke, L_Other},
   {"stroke-width", "", 'A', GraphML_ATTR_stroke_width, L_Other},
   {"style", "", 'A', GraphML_ATTR_style_, L_Other},
   {"target", "", 'A', GraphML_ATTR_target_, L_Other},
   {"title", "", 'A', GraphML_ATTR_title_, L_Other},
   {"transform", "", 'A', GraphML_ATTR_transform, L_Other},
   {"type", "", 'A', GraphML_ATTR_type, L_Other},
   {"viewBox", "", 'A', GraphML_ATTR_viewBox, L_Other},
   {"width", "", 'A', GraphML_ATTR_width_, L_Other},
   {"x", "", 'A', GraphML_ATTR_x, L_Other},
   {"x1", "", 'A', GraphML_ATTR_x1, L_Other},
   {"x2", "", 'A', GraphML_ATTR_x2, L_Other},
   {"xlink:href", "", 'A', GraphML_ATTR_xlink_href, L_Other},
   {"xml:space", "", 'A', GraphML_ATTR_xml_space, L_Other},
   {"y", "", 'A', GraphML_ATTR_y, L_Other},
   {"y1", "", 'A', GraphML_ATTR_y1, L_Other},
   {"y2", "", 'A', GraphML_ATTR_y2, L_Other},

   {"zzghost", "", 'A', GraphML_ATTR_Ghost_restruct, L_Other},
   {"", "", EOS, 0, L_Other}	  /* Last entry. Mandatory */
};
#else /* GRAPHML */
/* there is no mapping table of GraphML elements */

static ElemMapping *GraphMLElemMappingTable = NULL;
static AttributeMapping* GraphMLAttributeMappingTable = NULL;
#endif /* GRAPHML */
