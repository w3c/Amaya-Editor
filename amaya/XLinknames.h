#include "XLink.h"

static AttributeMapping XLinkAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {"unknown_attr", "", 'A', XLink_ATTR_Unknown_attribute, L_OtherValue},
   {"actuate", "", 'A', XLink_ATTR_actuate, L_OtherValue},
   {"arcrole", "", 'A', XLink_ATTR_arcrole, L_OtherValue},
   {"from", "", 'A', XLink_ATTR_from, L_OtherValue},
   {"href", "", 'A', XLink_ATTR_href_, L_OtherValue},
   {"role", "", 'A', XLink_ATTR_role, L_OtherValue},
   {"show", "", 'A', XLink_ATTR_show, L_OtherValue},
   {"title", "", 'A', XLink_ATTR_title, L_OtherValue},
   {"to", "", 'A', XLink_ATTR_to, L_OtherValue},
   {"type", "", 'A', XLink_ATTR_type, L_OtherValue},
   {"zzghost", "", 'A', XLink_ATTR_Ghost_restruct, L_OtherValue},

   {"", "", EOS, 0, L_OtherValue}		/* Last entry. Mandatory */
};
