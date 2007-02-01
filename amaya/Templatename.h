
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
    {"option", SPACE, Template_EL_option, L_OtherValue, FALSE},
    {"repeat", SPACE, Template_EL_repeat, L_OtherValue, FALSE},
    {"union", SPACE, Template_EL_union, L_OtherValue, FALSE},
    {"use", SPACE, Template_EL_useEl, L_OtherValue, FALSE},   
    {"", SPACE, 0, L_OtherValue, FALSE} 	/* Last entry. Mandatory */
};

static AttributeMapping TemplateAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {"unknown_attr", "", 'A', Template_ATTR_Unknown_attribute, L_OtherValue},
   {"currentOccurs", "", 'A', Template_ATTR_currentOccurs, L_OtherValue},
   {"currentType", "", 'A', Template_ATTR_currentType, L_OtherValue},   
   {"default", "", 'A', Template_ATTR_defaultAt, L_OtherValue},   
   {"exclude", "", 'A', Template_ATTR_exclude, L_OtherValue},
   {"fixed", "", 'A', Template_ATTR_fixed, L_OtherValue},
   {"include", "", 'A', Template_ATTR_includeAt, L_OtherValue},
   {"maxOccurs", "", 'A', Template_ATTR_maxOccurs, L_OtherValue},
   {"minOccurs", "", 'A', Template_ATTR_minOccurs, L_OtherValue}, 
   {"name", "attribute", 'A', Template_ATTR_ref_name, L_OtherValue},   
   {"name", "component", 'A', Template_ATTR_name, L_OtherValue},   
   {"name", "union", 'A', Template_ATTR_name, L_OtherValue},
   {"name", "", 'A', Template_ATTR_ref, L_OtherValue},
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
   {Template_ATTR_type, "integer", Template_ATTR_type_VAL_integerVal},
   {Template_ATTR_type, "decimal", Template_ATTR_type_VAL_decimal},
   {Template_ATTR_type, "string", Template_ATTR_type_VAL_string},
   {Template_ATTR_type, "list", Template_ATTR_type_VAL_listVal},
   {0, "", 0}			/* Last entry. Mandatory */
};
#else /* TEMPLATES */
/* there is no mapping table of Template elements */

static ElemMapping *TemplateElemMappingTable = NULL;
//static AttributeMapping* TemplateAttributeMappingTable = NULL;
#endif /* TEMPLATES */
