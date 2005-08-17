
#ifdef TEMPLATES
#include "Template.h"
/* mapping table of template elements */
static ElemMapping    TemplateElemMappingTable[] =
{
   /* This table MUST be in alphabetical order  and in lower case */
   {"free_content", SPACE, Template_EL_FREE_CONTENT, L_OtherValue, FALSE},
   {"free_struct", SPACE, Template_EL_FREE_STRUCT, L_OtherValue, FALSE},
   {"", SPACE, 0, L_OtherValue, FALSE} 	/* Last entry. Mandatory */
};

static AttributeMapping TemplateAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {"unknown_attr", "", 'A', Template_ATTR_Unknown_attribute, L_OtherValue},

   {"xml:id", "", 'A', Template_ATTR_xmlid, L_OtherValue},

   {"", "", EOS, 0, L_OtherValue}	  /* Last entry. Mandatory */
};

/* mapping table of attribute values */
static AttrValueMapping TemplateAttrValueMappingTable[] =
{
   {0, "", 0}			/* Last entry. Mandatory */
};
#else /* TEMPLATES */
/* there is no mapping table of Template elements */

static ElemMapping *TemplateElemMappingTable = NULL;
static AttributeMapping* TemplateAttributeMappingTable = NULL;
#endif /* TEMPLATES */
