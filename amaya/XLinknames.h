#include "XLink.h"

static AttributeMapping XLinkAttributeMappingTable[] =
{
   /* The first entry MUST be unknown_attr */
   /* The rest of this table MUST be in alphabetical order */
   {TEXT("unknown_attr"), TEXT(""), 'A', XLink_ATTR_Invalid_attribute, L_Other},
   {TEXT("actuate"), TEXT(""), 'A', XLink_ATTR_actuate, L_Other},
   {TEXT("from"), TEXT(""), 'A', XLink_ATTR_from, L_Other},
   {TEXT("href"), TEXT(""), 'A', XLink_ATTR_href_, L_Other},
   {TEXT("role"), TEXT(""), 'A', XLink_ATTR_role, L_Other},
   {TEXT("show"), TEXT(""), 'A', XLink_ATTR_show, L_Other},
   {TEXT("title"), TEXT(""), 'A', XLink_ATTR_title, L_Other},
   {TEXT("to"), TEXT(""), 'A', XLink_ATTR_to, L_Other},
   {TEXT("type"), TEXT(""), 'A', XLink_ATTR_type, L_Other},
   {TEXT("zzghost"), TEXT(""), 'A', XLink_ATTR_Ghost_restruct, L_Other},

   {TEXT(""), TEXT(""), EOS, 0, L_Other}		/* Last entry. Mandatory */
};
