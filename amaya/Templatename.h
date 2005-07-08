
#ifdef TEMPLATE
/* mapping table of template elements */
#include "Template.h"
static ElemMapping    TemplateElemMappingTable[] =
{
   /* This table MUST be in alphabetical order  and in lower case */
   {"free-content", SPACE, Template_EL_FreeContent, L_OtherValue, FALSE},
   {"free-structure", SPACE, Template_EL_FreeStructure, L_OtherValue, FALSE},
   {"", SPACE, 0, L_OtherValue, FALSE} 	/* Last entry. Mandatory */
};

static AttributeMapping TemplateAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {"unknown_attr", "", 'A', Template_ATTR_Unknown_attribute, L_OtherValue},
   {"", "", EOS, 0, L_OtherValue}	  /* Last entry. Mandatory */
};

/* mapping table of attribute values */
static AttrValueMapping TemplateAttrValueMappingTable[] =
{
   {0, "", 0}			/* Last entry. Mandatory */
};
#else /* TEMPLATE */
/* there is no mapping table of Template elements */

static ElemMapping *TemplateElemMappingTable = NULL;
static AttributeMapping* TemplateAttributeMappingTable = NULL;
#endif /* TEMPLATE */
