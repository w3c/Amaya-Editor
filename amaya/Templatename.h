
#ifdef TEMPLATES
#include "Template.h"
/* mapping table of template elements */
static ElemMapping    TemplateElemMappingTable[] =
{
  /* This table MUST be in alphabetical order  and in lower case */
	{"attribute", SPACE, Template_EL_attribute, L_OtherValue, FALSE},
	{"bag", SPACE, Template_EL_bag, L_OtherValue, FALSE},   
  {"component", SPACE, Template_EL_component, L_OtherValue, FALSE},
  {"head", SPACE, Template_EL_head, L_OtherValue, FALSE},   
  {"import", SPACE, Template_EL_import, L_OtherValue, FALSE},
  {"library", SPACE, Template_EL_Template, L_OtherValue, FALSE},
  //  {"option", SPACE, Template_EL_option, L_OtherValue, FALSE},
  {"repeat", SPACE, Template_EL_repeat, L_OtherValue, FALSE},
  {"union", SPACE, Template_EL_union, L_OtherValue, FALSE},
  {"use", SPACE, Template_EL_useEl, L_OtherValue, FALSE},   
  {"xmlcomment", SPACE, Template_EL_Comment_, L_OtherValue, FALSE},
  {"xmlcomment_line", SPACE, Template_EL_Comment_line, L_OtherValue, FALSE},
  {"xmlpi", SPACE, Template_EL_XMLPI, L_OtherValue, FALSE},
  {"xmlpi_line", SPACE, Template_EL_PI_line, L_OtherValue, FALSE},
  {"", SPACE, 0, L_OtherValue, FALSE} 	/* Last entry. Mandatory */
};

static AttributeMapping TemplateAttributeMappingTable[] =
{
  /* The first entry MUST be unknown_attr */
  /* The rest of this table MUST be in alphabetical order */
  {"unknown_attr", "", 'A', Template_ATTR_Unknown_attribute, L_OtherValue},
  {"currentType", "", 'A', Template_ATTR_currentType, L_OtherValue},   
  {"default", "", 'A', Template_ATTR_defaultAt, L_OtherValue},   
  {"exclude", "", 'A', Template_ATTR_exclude, L_OtherValue},
  {"fixed", "", 'A', Template_ATTR_fixed, L_OtherValue},
  {"include", "", 'A', Template_ATTR_includeAt, L_OtherValue},
  {"initial", "", 'A', Template_ATTR_prompt, L_OtherValue},
  {"label", "", 'A', Template_ATTR_title, L_OtherValue},
  {"maxOccurs", "", 'A', Template_ATTR_maxOccurs, L_OtherValue},
  {"minOccurs", "", 'A', Template_ATTR_minOccurs, L_OtherValue}, 
  {"name", "attribute", 'A', Template_ATTR_ref_name, L_OtherValue},   
  {"name", "component", 'A', Template_ATTR_name, L_OtherValue},   
  {"name", "union", 'A', Template_ATTR_name, L_OtherValue},
  {"option", "", 'A', Template_ATTR_option, L_OtherValue},
  {"prompt", "", 'A', Template_ATTR_prompt, L_OtherValue},
  {"src", "", 'A', Template_ATTR_src, L_OtherValue},
  {"templateVersion", "head", 'A', Template_ATTR_templateVersion, L_OtherValue},
  {"type", "", 'A', Template_ATTR_type, L_OtherValue},   
  {"types", "", 'A', Template_ATTR_types, L_OtherValue},
  {"use", "", 'A', Template_ATTR_useAt, L_OtherValue},
  {"version", "head", 'A', Template_ATTR_version, L_OtherValue},
  {"", "", EOS, 0, L_OtherValue}	  /* Last entry. Mandatory */
};

/* mapping table of attribute values */
static AttrValueMapping TemplateAttrValueMappingTable[] =
{
  {Template_ATTR_option, "set", Template_ATTR_option_VAL_option_set},
  {Template_ATTR_option, "unset", Template_ATTR_option_VAL_option_unset},
  {Template_ATTR_prompt, "prompt", Template_ATTR_prompt_VAL_Yes_},
  {Template_ATTR_prompt, "true", Template_ATTR_prompt_VAL_Yes_},
  {Template_ATTR_type, "number", Template_ATTR_type_VAL_number},
  {Template_ATTR_type, "string", Template_ATTR_type_VAL_string},
  {Template_ATTR_type, "list", Template_ATTR_type_VAL_listVal},
  {Template_ATTR_useAt, "required", Template_ATTR_useAt_VAL_required},
  {Template_ATTR_useAt, "optional", Template_ATTR_useAt_VAL_optional},
  {Template_ATTR_useAt, "prohibited", Template_ATTR_useAt_VAL_prohibited},
  {0, "", 0}			/* Last entry. Mandatory */
};

#else /* TEMPLATES */
/* there is no mapping table of Template elements */

static ElemMapping *TemplateElemMappingTable = NULL;
static AttributeMapping* TemplateAttributeMappingTable = NULL;
#endif /* TEMPLATES */
