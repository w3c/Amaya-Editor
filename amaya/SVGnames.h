
#ifdef _SVG
/* mapping table of SVG elements */
#include "SVG.h"
static ElemMapping    SVGElemMappingTable[] =
{
   /* This table MUST be in alphabetical order */
   {"XMLPI", SPACE, SVG_EL_XMLPI, L_OtherValue, FALSE},
   {"XMLPI_line", SPACE, SVG_EL_XMLPI_line, L_OtherValue, FALSE},
   {"XMLcomment", SPACE, SVG_EL_XMLcomment, L_OtherValue, FALSE},
   {"XMLcomment_line", SPACE, SVG_EL_XMLcomment_line, L_OtherValue, FALSE},
   {"a", SPACE, SVG_EL_a, L_OtherValue, FALSE},
   {"c", SPACE, SVG_EL_TEXT_UNIT, L_OtherValue, TRUE},
   {"circle", SPACE, SVG_EL_circle, L_OtherValue, FALSE},
   {"defs", SPACE, SVG_EL_defs, L_OtherValue, FALSE},
   {"desc", SPACE, SVG_EL_desc, L_OtherValue, FALSE},
   {"ellipse", SPACE, SVG_EL_ellipse, L_OtherValue, FALSE},
   {"foreignObject", SPACE, SVG_EL_foreignObject, L_OtherValue, FALSE},
   {"g", SPACE, SVG_EL_g, L_OtherValue, FALSE},
   {"image", SPACE, SVG_EL_image, L_OtherValue, FALSE},
   {"line", SPACE, SVG_EL_line_, L_OtherValue, FALSE},
   {"metadata", SPACE, SVG_EL_metadata, L_OtherValue, FALSE},
   {"path", SPACE, SVG_EL_path, L_OtherValue, FALSE},
   {"polygon", SPACE, SVG_EL_polygon, L_OtherValue, FALSE},
   {"polyline", SPACE, SVG_EL_polyline, L_OtherValue, FALSE},
   {"rect", SPACE, SVG_EL_rect, L_OtherValue, FALSE},
   {"script", SPACE, SVG_EL_script, L_OtherValue, FALSE},
   {"style", SPACE, SVG_EL_style__, L_OtherValue, FALSE},
   {"svg", SPACE, SVG_EL_SVG, L_OtherValue, FALSE},
   {"switch", SPACE, SVG_EL_switch, L_OtherValue, FALSE},
   {"symbol", SPACE, SVG_EL_symbol_, L_OtherValue, FALSE},
   {"text", SPACE, SVG_EL_text_, L_OtherValue, FALSE},
   {"title", SPACE, SVG_EL_title, L_OtherValue, FALSE},
   {"tspan", SPACE, SVG_EL_tspan, L_OtherValue, FALSE},
   {"use", SPACE, SVG_EL_use_, L_OtherValue, FALSE},
   {"", SPACE, 0, L_OtherValue, FALSE} 	/* Last entry. Mandatory */
};

static AttributeMapping SVGAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {"unknown_attr", "", 'A', SVG_ATTR_Unknown_attribute, L_OtherValue},

   {"class", "", 'A', SVG_ATTR_class, L_OtherValue},
   {"cx", "", 'A', SVG_ATTR_cx, L_OtherValue},
   {"cy", "", 'A', SVG_ATTR_cy, L_OtherValue},
   {"d", "", 'A', SVG_ATTR_d, L_OtherValue},
   {"dx", "", 'A', SVG_ATTR_dx, L_OtherValue},
   {"dy", "", 'A', SVG_ATTR_dy, L_OtherValue},
   {"externalResourcesRequired", "", 'A', SVG_ATTR_externalResourcesRequired, L_OtherValue},
   {"fill", "", 'A', SVG_ATTR_fill, L_OtherValue},
   {"font-family", "", 'A', SVG_ATTR_font_family, L_OtherValue},
   {"font-size", "", 'A', SVG_ATTR_font_size, L_OtherValue},
   {"font-style", "", 'A', SVG_ATTR_font_style, L_OtherValue},
   {"font-variant", "", 'A', SVG_ATTR_font_variant, L_OtherValue},
   {"font-weight", "", 'A', SVG_ATTR_font_weight, L_OtherValue},
   {"height", "", 'A', SVG_ATTR_height_, L_OtherValue},
   {"id", "", 'A', SVG_ATTR_id, L_OtherValue},
   {"media", "", 'A', SVG_ATTR_media, L_OtherValue},
   {"pathLength", "", 'A', SVG_ATTR_pathLength, L_OtherValue},
   {"points", "", 'A', SVG_ATTR_points, L_OtherValue},
   {"preserveAspectRatio", "", 'A', SVG_ATTR_preserveAspectRatio, L_OtherValue},
   {"r", "", 'A', SVG_ATTR_r, L_OtherValue},
   {"requiredExtensions", "", 'A', SVG_ATTR_requiredExtensions, L_OtherValue},
   {"requiredFeatures", "", 'A', SVG_ATTR_requiredFeatures, L_OtherValue},
   {"rx", "", 'A', SVG_ATTR_rx, L_OtherValue},
   {"ry", "", 'A', SVG_ATTR_ry, L_OtherValue},
   {"stroke", "", 'A', SVG_ATTR_stroke, L_OtherValue},
   {"stroke-width", "", 'A', SVG_ATTR_stroke_width, L_OtherValue},
   {"style", "", 'A', SVG_ATTR_style_, L_OtherValue},
   {"systemLanguage", "", 'A', SVG_ATTR_systemLanguage, L_OtherValue},
   {"target", "", 'A', SVG_ATTR_target_, L_OtherValue},
   {"title", "", 'A', SVG_ATTR_title_, L_OtherValue},
   {"transform", "", 'A', SVG_ATTR_transform, L_OtherValue},
   {"type", "", 'A', SVG_ATTR_type, L_OtherValue},
   {"viewBox", "", 'A', SVG_ATTR_viewBox, L_OtherValue},
   {"width", "", 'A', SVG_ATTR_width_, L_OtherValue},
   {"x", "", 'A', SVG_ATTR_x, L_OtherValue},
   {"x1", "", 'A', SVG_ATTR_x1, L_OtherValue},
   {"x2", "", 'A', SVG_ATTR_x2, L_OtherValue},
   {"xlink:href", "", 'A', SVG_ATTR_xlink_href, L_OtherValue},
   {"xml:space", "", 'A', SVG_ATTR_xml_space, L_OtherValue},
   {"y", "", 'A', SVG_ATTR_y, L_OtherValue},
   {"y1", "", 'A', SVG_ATTR_y1, L_OtherValue},
   {"y2", "", 'A', SVG_ATTR_y2, L_OtherValue},

   {"zzghost", "", 'A', SVG_ATTR_Ghost_restruct, L_OtherValue},
   {"", "", EOS, 0, L_OtherValue}	  /* Last entry. Mandatory */
};
#else /* _SVG */
/* there is no mapping table of SVG elements */

static ElemMapping *SVGElemMappingTable = NULL;
static AttributeMapping* SVGAttributeMappingTable = NULL;
#endif /* _SVG */
