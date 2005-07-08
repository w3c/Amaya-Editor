/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1998-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Templatebuilder
 *
 * Authors: V. Quint
 *          I. Vatton
 *          P. Cheyrou-Lagreze
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "HTML.h"
#include "parser.h"
#include "registry.h"
#include "style.h"
#include "XLink.h"

#define MaxMsgLength 200

#include "anim_f.h"
#include "animbuilder_f.h"
#include "css_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "HTMLactions_f.h"
#include "styleparser_f.h"
#include "Xml2thot_f.h"

/*----------------------------------------------------------------------
   TemplateGetDTDName
   Return in DTDname the name of the DTD to be used for parsing the
   content of element named elementName.
   This element type appear with an 'X' in the ElemMappingTable.
  ----------------------------------------------------------------------*/
void      TemplateGetDTDName (char *DTDname, char *elementName)
{
  strcpy (DTDname, "");
}

/*----------------------------------------------------------------------
   MapTemplateAttribute
   Search in the Attribute Mapping Table the entry for the
   attribute of name Attr and returns the corresponding Thot attribute type.
  ----------------------------------------------------------------------*/
void MapTemplateAttribute (char *attrName, AttributeType *attrType,
			   char* elementName, ThotBool *level, Document doc)
{
  attrType->AttrSSchema = GetTemplateSSchema (doc);
  MapXMLAttribute (Template_TYPE, attrName, elementName, level, doc,
		   &(attrType->AttrTypeNum));
}

/*----------------------------------------------------------------------
   MapTemplateAttributeValue
   Search in the Attribute Value Mapping Table the entry for the attribute
   ThotAtt and its value attVal. Returns the corresponding Thot value.
  ----------------------------------------------------------------------*/
void MapTemplateAttributeValue (char* attVal, const AttributeType * attrType, int *value)
{
  MapXMLAttributeValue (Template_TYPE, attVal, attrType, value);
}

/*----------------------------------------------------------------------
   MapTemplateEntity
   Search that entity in the entity table and return the corresponding value.
  ----------------------------------------------------------------------*/
void   MapTemplateEntity (char *entityName, char *entityValue, char *script)
{
  entityValue[0] = EOS;
  *script = EOS;
}

/*----------------------------------------------------------------------
   TemplateEntityCreated
   A Template entity has been created by the XML parser.
  ----------------------------------------------------------------------*/
void    TemplateEntityCreated (unsigned char *entityValue, Language lang,
			      char *entityName, Document doc)
{
}



/*----------------------------------------------------------------------
   TemplateElementComplete
   Check the Thot structure of the Template element el.
  ----------------------------------------------------------------------*/
void TemplateElementComplete (ParserData *context, Element el, int *error)
{

}

/*----------------------------------------------------------------------
   UnknownTemplateNameSpace
   The element doesn't belong to a supported namespace
  ----------------------------------------------------------------------*/
void               UnknownTemplateNameSpace (ParserData *context,
					Element *unknownEl,
					char* content)
{
}



/*----------------------------------------------------------------------
   GetNumber
   Parse an integer or floating point number and skip to the next token.
   Return the value of that number in number and moves ptr to the next
   token to be parsed.
   If the string to be parsed is not a valid number, set error to TRUE.
  ----------------------------------------------------------------------*/
static char *GetNumber (char *ptr, int* number, ThotBool *error)
{
  int      integer, nbdecimal, exponent, i;
  char     *decimal;
  ThotBool negative, negativeExp;

  *number = 0;
  *error = FALSE;
  integer = 0;
  nbdecimal = 0;
  decimal = NULL;
  negative = FALSE;
  /* read the sign */
  if (*ptr == '+')
    ptr++;
  else if (*ptr == '-')
    {
      ptr++;
      negative = TRUE;
    }

  if (*ptr < '0' || *ptr > '9')
    {
      *error = TRUE;
      ptr++;
      return (ptr);
    }
  /* read the integer part */
  while (*ptr != EOS && *ptr >= '0' && *ptr <= '9')
    {
      integer *= 10;
      integer += *ptr - '0';
      ptr++;
    }
  if (*ptr == '.')
    /* there is a decimal part */
    {
      ptr++;
      decimal = ptr;
      while (*ptr != EOS &&  *ptr >= '0' && *ptr <= '9')
	{
	  nbdecimal++;
	  ptr++;
	}
    }

  if (*ptr != 'e' && *ptr != 'E')
    /* no exponent */
    {
      if (nbdecimal > 0)
	/* there are some digits after the decimal point */
	{
	  if (*decimal >= '5' && *decimal <= '9')
	    /* the first digit after the point is 5 of greater
	       round up the value to the next integer */
	    integer++;
	}
    }
  else
    /* there is an exponent part, parse it */
    {
      ptr++;
      negativeExp = FALSE;
      /* read the sign of the exponent */
      if (*ptr == '+')
	ptr++;
      else if (*ptr == '-')
	{
	  ptr++;
	  negativeExp = TRUE;
	}
      exponent = 0;

      if (*ptr < '0' || *ptr > '9')
	{
	  *error = TRUE;
	  ptr++;
	  return (ptr);
	}

      while (*ptr != EOS &&  *ptr >= '0' && *ptr <= '9')
	{
	  exponent *= 10;
	  exponent += *ptr - '0';
	  ptr++;
	}
      if (exponent > 0)
	{
	  if (negativeExp)
	    {
	      for (i = 0; i < exponent; i++)
		integer /= 10;
	    }
	  else
	    {
	      for (i = 0; i < exponent; i++)
		{
		  integer *= 10;
		  if (i < nbdecimal)
		    {
		      integer += *decimal - '0';
		      decimal++;
		    }
		}
	    }
	}
    }

  if (negative)
    *number = - integer;
  else
    *number = integer;

  /* skip the following spaces */
  while (*ptr != EOS &&
         (*ptr == ',' || *ptr == SPACE || *ptr == BSPACE ||
	  *ptr == EOL    || *ptr == TAB   || *ptr == CR))
    ptr++;
  return (ptr);
}



/*----------------------------------------------------------------------
   TemplateAttributeComplete
   The XML parser has read attribute attr for element el in document doc.
  ----------------------------------------------------------------------*/
void TemplateAttributeComplete (Attribute attr, Element el, Document doc)
{
   AttributeType	attrType;
   int                  attrKind;
   TtaGiveAttributeType (attr, &attrType, &attrKind);
   switch (attrType.AttrTypeNum)
     {
     default:
       break;
     }
}



