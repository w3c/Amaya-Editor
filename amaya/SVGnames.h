
#ifdef _SVG
/* mapping table of SVG elements */
#include "SVG.h"
static ElemMapping    SVGElemMappingTable[] =
{
   /* This table MUST be in alphabetical order  and in lower case */
   {"a", SPACE, SVG_EL_a, L_OtherValue, FALSE},
   {"c", SPACE, SVG_EL_TEXT_UNIT, L_OtherValue, TRUE},
   {"cdata", SPACE, SVG_EL_CDATA, L_OtherValue, FALSE},
   {"cdata_line", SPACE, SVG_EL_CDATA_line, L_OtherValue, FALSE},
   {"circle", SPACE, SVG_EL_circle, L_OtherValue, FALSE},
   {"defs", SPACE, SVG_EL_defs, L_OtherValue, FALSE},
   {"desc", SPACE, SVG_EL_desc, L_OtherValue, FALSE},
   {"doctype", SPACE, SVG_EL_DOCTYPE, L_OtherValue, FALSE},
   {"doctype_line", SPACE, SVG_EL_DOCTYPE_line, L_OtherValue, FALSE},
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
   {"script", SPACE, SVG_EL_script_, L_OtherValue, FALSE},
   {"style", SPACE, SVG_EL_style__, L_OtherValue, FALSE},
   {"svg", SPACE, SVG_EL_SVG, L_OtherValue, FALSE},
   {"switch", SPACE, SVG_EL_switch, L_OtherValue, FALSE},
   {"symbol", SPACE, SVG_EL_symbol_, L_OtherValue, FALSE},
   {"text", SPACE, SVG_EL_text_, L_OtherValue, FALSE},
   {"title", SPACE, SVG_EL_title, L_OtherValue, FALSE},
   {"tspan", SPACE, SVG_EL_tspan, L_OtherValue, FALSE},
   {"use", SPACE, SVG_EL_use_, L_OtherValue, FALSE},
   {"xmlcomment", SPACE, SVG_EL_XMLcomment, L_OtherValue, FALSE},
   {"xmlcomment_line", SPACE, SVG_EL_XMLcomment_line, L_OtherValue, FALSE},
   {"xmlpi", SPACE, SVG_EL_XMLPI, L_OtherValue, FALSE},
   {"xmlpi_line", SPACE, SVG_EL_XMLPI_line, L_OtherValue, FALSE},
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
   {"direction", "", 'A', SVG_ATTR_direction_, L_OtherValue},
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
   {"lang", "", 'A', SVG_ATTR_Language, L_OtherValue},
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
   {"text-decoration", "", 'A', SVG_ATTR_text_decoration, L_OtherValue},
   {"title", "", 'A', SVG_ATTR_title_, L_OtherValue},
   {"transform", "", 'A', SVG_ATTR_transform, L_OtherValue},
   {"type", "", 'A', SVG_ATTR_type, L_OtherValue},
   {"unicode-bidi", "", 'A', SVG_ATTR_unicode_bidi, L_OtherValue},
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

/* mapping table of attribute values */
static AttrValueMapping SVGAttrValueMappingTable[] =
{ 
   {SVG_ATTR_xml_space, "default", SVG_ATTR_xml_space_VAL_xml_space_default},
   {SVG_ATTR_xml_space, "preserve", SVG_ATTR_xml_space_VAL_xml_space_preserve},

   {SVG_ATTR_externalResourcesRequired, "false", SVG_ATTR_externalResourcesRequired_VAL_false},
   {SVG_ATTR_externalResourcesRequired, "true", SVG_ATTR_externalResourcesRequired_VAL_true},

   {SVG_ATTR_font_style, "inherit", SVG_ATTR_font_style_VAL_inherit},
   {SVG_ATTR_font_style, "italic", SVG_ATTR_font_style_VAL_italic},
   {SVG_ATTR_font_style, "normal", SVG_ATTR_font_style_VAL_normal_},
   {SVG_ATTR_font_style, "oblique", SVG_ATTR_font_style_VAL_oblique_},

   {SVG_ATTR_font_variant, "inherit", SVG_ATTR_font_variant_VAL_inherit},
   {SVG_ATTR_font_variant, "normal", SVG_ATTR_font_variant_VAL_normal_},
   {SVG_ATTR_font_variant, "small-caps", SVG_ATTR_font_variant_VAL_small_caps},

   {SVG_ATTR_font_weight, "100", SVG_ATTR_font_weight_VAL_w100},
   {SVG_ATTR_font_weight, "200", SVG_ATTR_font_weight_VAL_w200},
   {SVG_ATTR_font_weight, "300", SVG_ATTR_font_weight_VAL_w300},
   {SVG_ATTR_font_weight, "400", SVG_ATTR_font_weight_VAL_w400},
   {SVG_ATTR_font_weight, "500", SVG_ATTR_font_weight_VAL_w500},
   {SVG_ATTR_font_weight, "600", SVG_ATTR_font_weight_VAL_w600},
   {SVG_ATTR_font_weight, "700", SVG_ATTR_font_weight_VAL_w700},
   {SVG_ATTR_font_weight, "800", SVG_ATTR_font_weight_VAL_w800},
   {SVG_ATTR_font_weight, "900", SVG_ATTR_font_weight_VAL_w900},
   {SVG_ATTR_font_weight, "bold", SVG_ATTR_font_weight_VAL_bold_},
   {SVG_ATTR_font_weight, "bolder", SVG_ATTR_font_weight_VAL_bolder},
   {SVG_ATTR_font_weight, "inherit", SVG_ATTR_font_weight_VAL_inherit},
   {SVG_ATTR_font_weight, "lighter", SVG_ATTR_font_weight_VAL_lighter},
   {SVG_ATTR_font_weight, "normal", SVG_ATTR_font_weight_VAL_normal_},

   {SVG_ATTR_direction_, "ltr", SVG_ATTR_direction__VAL_ltr_},
   {SVG_ATTR_direction_, "rtl", SVG_ATTR_direction__VAL_rtl_},
   {SVG_ATTR_direction_, "inherit", SVG_ATTR_direction__VAL_inherit},

   {SVG_ATTR_unicode_bidi, "normal", SVG_ATTR_unicode_bidi_VAL_normal_},
   {SVG_ATTR_unicode_bidi, "embed", SVG_ATTR_unicode_bidi_VAL_embed_},
   {SVG_ATTR_unicode_bidi, "bidi-override", SVG_ATTR_unicode_bidi_VAL_bidi_override},
   {SVG_ATTR_unicode_bidi, "inherit", SVG_ATTR_unicode_bidi_VAL_inherit},

   {0, "", 0}			/* Last entry. Mandatory */
};
#else /* _SVG */
/* there is no mapping table of SVG elements */

static ElemMapping *SVGElemMappingTable = NULL;
static AttributeMapping* SVGAttributeMappingTable = NULL;
#endif /* _SVG */
