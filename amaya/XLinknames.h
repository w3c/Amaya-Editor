#include "XLink.h"

static AttributeMapping XLinkAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {"unknown_attr", "", 'A', XLink_ATTR_Invalid_attribute, L_Other},
   {"actuate", "", 'A', XLink_ATTR_actuate, L_Other},
   {"from", "", 'A', XLink_ATTR_from, L_Other},
   {"href", "", 'A', XLink_ATTR_href_, L_Other},
   {"role", "", 'A', XLink_ATTR_role, L_Other},
   {"show", "", 'A', XLink_ATTR_show, L_Other},
   {"title", "", 'A', XLink_ATTR_title, L_Other},
   {"to", "", 'A', XLink_ATTR_to, L_Other},
   {"type", "", 'A', XLink_ATTR_type, L_Other},
   {"zzghost", "", 'A', XLink_ATTR_Ghost_restruct, L_Other},

   {"", "", EOS, 0, L_Other}		/* Last entry. Mandatory */
};
