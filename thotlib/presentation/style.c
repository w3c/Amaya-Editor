/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Set of functions to style Thot documents: it's the style API.
 *
 * Author: I. Vatton (INRIA)
 *
 */

#include "thot_sys.h"
#include "thot_gui.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "constprs.h"
#include "typeprs.h"
#include "document.h"
#include "pschema.h"
#include "application.h"
#include "style.h"
#include "HTMLstyleColor.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "thotcolor_tv.h"

#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "context_f.h"
#include "exceptions_f.h"
#include "inites_f.h"
#include "memory_f.h"
#include "schemas_f.h"
#include "style_f.h"
#include "tree_f.h"

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
unsigned int TtaHexaVal (char c)
{
   if (c >= '0' && c <= '9')
      return (c - '0');
   if (c >= 'a' && c <= 'f')
      return (c - 'a' + 10);
   if (c >= 'A' && c <= 'F')
      return (c - 'A' + 10);
   return (0);
}


/*----------------------------------------------------------------------
  Getan Amaya color
 ----------------------------------------------------------------------*/
static ThotBool ThotGiveRGB (char *colname, unsigned short *red,
			     unsigned short *green, unsigned short *blue)
{
  int                  i;
  ThotBool             failed = TRUE;

  /* Lookup the color name in the application color name database */
  for (i = 0; i < NColors && failed; i++)
    if (!strcasecmp (ColorName (i), colname))
      {
	failed = FALSE;
	*red   = RGB_Table[i].red;
	*green = RGB_Table[i].green;
	*blue  = RGB_Table[i].blue;
      }
  return (failed);
}

/*----------------------------------------------------------------------
  TtaGiveRGB
  Returns the RGB of the color and the pointer to the following text in
  value if the parsing was finished.
 ----------------------------------------------------------------------*/
char *TtaGiveRGB (char *value, unsigned short *red, unsigned short *green,
		    unsigned short *blue)
{
  char              colname[100];
  char             *ptr, *sptr;
  int               i, len;
  int               r, g, b;
  ThotBool          failed;

  ptr = value;
  failed = TRUE;
  *red = -1;
  *green = 0;
  *blue = 0;
  if (*ptr == '#')
    {
      /* we expect an hexa encoding like F0F or FF00FF */
      if (isxdigit (ptr[1]) && isxdigit (ptr[2]) && isxdigit (ptr[3]))
	{
	  if (!isxdigit (ptr[4]))
	    {
	      /* encoded on 3 digits #F0F  */
	      *red = TtaHexaVal (ptr[1]) * 16 + TtaHexaVal (ptr[1]);
	      *green = TtaHexaVal (ptr[2]) * 16 + TtaHexaVal (ptr[2]);
	      *blue = TtaHexaVal (ptr[3]) * 16 + TtaHexaVal (ptr[3]);
	      ptr = &ptr[4];
	      failed = FALSE;
	    }
	  else if (isxdigit (ptr[5]) && isxdigit (ptr[6]))
	    {
	      /* encoded on 6 digits #FF00FF */
	      *red = TtaHexaVal (ptr[1]) * 16 + TtaHexaVal (ptr[2]);
	      *green = TtaHexaVal (ptr[3]) * 16 + TtaHexaVal (ptr[4]);
	      *blue = TtaHexaVal (ptr[5]) * 16 + TtaHexaVal (ptr[6]);
	      ptr = &ptr[7];
	      failed = FALSE;
	    }
	}
    }
  else if (!strncasecmp (ptr, "rgb", 3))
    {
      ptr = &ptr[3];
      ptr = TtaSkipBlanks (ptr);
      if (*ptr == '(')
	{
	  ptr++;
	  ptr = TtaSkipBlanks (ptr);
	  failed = FALSE;
	  /* encoded as rgb(red, green, blue) or rgb(red%, green%, blue%) */
	  sscanf (ptr, "%d", &r);
	  while (*ptr != EOS && *ptr != ',' && *ptr != '%')
	    ptr++;
	  if (*ptr == '%')
	    {
	      *red = (unsigned short)(r * 255 / 100);
	      while (*ptr != EOS && *ptr != ',')
		ptr++;
	    }
	  else
	    *red = (unsigned short)r;
	  ptr++;
	  sscanf (ptr, "%d", &g);
	  while (*ptr != EOS && *ptr != ',' && *ptr != '%')
	    ptr++;
	  if (*ptr == '%')
	    {
	      *green = (unsigned short)(g * 255 / 100);
	      while (*ptr != EOS && *ptr != ',')
		ptr++;
	    }
	  else
	    *green = (unsigned short)g;
	  ptr++;
	  sscanf (ptr, "%d", &b);
	  while (*ptr != EOS && *ptr != ')' && *ptr != '%')
	    ptr++;
	  if (*ptr == '%')
	    {
	      *blue = (unsigned short)(b * 255 / 100);
	      while (*ptr != EOS && *ptr != ')')
		ptr++;
	    }
	  else
	    *blue = (unsigned short)b;
	  /* search the rgb end */
	  if (*ptr == ')')
	    ptr++;
	}
    }
  else if (isalpha (*ptr) || isxdigit (*ptr))
    {
      /* we expect a color name like "red". Store it in colname */
      len = (sizeof (colname) / sizeof (char)) - 1;
      sptr = ptr;
      for (i = 0; i < len && isalnum (*ptr); i++)
	{
	  colname[i] = *ptr;
	  ptr++;
	}
      colname[i] = EOS;
      
      /* Look for the color name in our own color name database */
      for (i = 0; i < (int)NBCOLORNAME && failed; i++)
	if (!strcasecmp (ColornameTable[i].name, colname))
	  {
	    *red = ColornameTable[i].red;
	    *green = ColornameTable[i].green;
	    *blue = ColornameTable[i].blue;
	    failed = FALSE;
	  }
      if (failed)
	failed = ThotGiveRGB (colname, red, green, blue);
      if (failed)
	/* it may be a sequence of 3 or 6 hex. digits with a missing
           leading '#' */
	{
	  ptr = sptr;
	  if (isxdigit (ptr[0]) && isxdigit (ptr[1]) && isxdigit (ptr[2]))
	  /* we expect an hexa encoding like F0F or FF00FF */
	    {
	      if (!isxdigit (ptr[3]))
		{
		  /* encoded on 3 digits #F0F  */
		  *red = TtaHexaVal (ptr[0]) * 16 + TtaHexaVal (ptr[0]);
		  *green = TtaHexaVal (ptr[1]) * 16 + TtaHexaVal (ptr[1]);
		  *blue = TtaHexaVal (ptr[2]) * 16 + TtaHexaVal (ptr[2]);
		  ptr = &ptr[3];
		  failed = FALSE;
		}
	      else if (isxdigit (ptr[4]) && isxdigit (ptr[5]))
		{
		  /* encoded on 6 digits #FF00FF */
		  *red = TtaHexaVal (ptr[0]) * 16 + TtaHexaVal (ptr[1]);
		  *green = TtaHexaVal (ptr[2]) * 16 + TtaHexaVal (ptr[3]);
		  *blue = TtaHexaVal (ptr[4]) * 16 + TtaHexaVal (ptr[5]);
		  ptr = &ptr[6];
		  failed = FALSE;
		}
	    }
	}
    }
  if (failed)
    return (value);
  else
    return (ptr);
}

/*----------------------------------------------------------------------
  BuildBoxName : generate an unique name encoding for the given context.
  Assume the ancestor list has been sorted.
  ----------------------------------------------------------------------*/
static void BuildBoxName (GenericContext ctxt, Name *boxname)
{
  int               i;
  int               len;
  char              buffer[100];

  buffer[0] = 0;
  len = 0;
  for (i = 0; i < MAX_ANCESTORS && ctxt->name[i]; i++)
    {
      if (ctxt->names_nb[i] > 1)
	sprintf (&buffer[len], "%d:%d/", ctxt->name[i], ctxt->names_nb[i]);
      else
	sprintf (&buffer[len], "%d/", ctxt->name[i]);
      len = strlen (buffer);
    }
  if (ctxt->type)
    sprintf (&buffer[len], "%d,", ctxt->type);
  len = strlen (buffer);
  if (ctxt->attrType[0])
    sprintf (&buffer[len], "%d.%s,", ctxt->attrType[0], ctxt->attrText[0]);
  len = strlen (buffer);
  strncpy (*boxname, buffer, sizeof (Name));
}

/*----------------------------------------------------------------------
 BoxRuleSearch : look in the array of boxes for an entry
        corresponding to the current context.
  ----------------------------------------------------------------------*/
static PtrPRule BoxRuleSearch (PtrPSchema tsch, GenericContext ctxt)
{
  int                 i;
  Name                boxname;

  BuildBoxName (ctxt, &boxname);
  
  /* search for the BOXE in the Presentation Schema */
  for (i = 1; i <= tsch->PsNPresentBoxes; i++)
    {
      if (!strcmp (ctxt->attrText[0], tsch->PsPresentBox->PresBox[i - 1]->PbName))
	{
	  ctxt->box = i;
	  return (tsch->PsPresentBox->PresBox[i - 1]->PbFirstPRule);
	}
    }

  ctxt->box = 0;
  return (NULL);
}

/*----------------------------------------------------------------------
   Function used to to search all specific presentation rules
   for a given type of rule associated to an element.
  ----------------------------------------------------------------------*/
static PtrPRule SearchElementPRule (PtrElement el, PRuleType type, unsigned int extra)
{
  PtrPRule      cur;
  ThotBool      found;
    
  cur = el->ElFirstPRule;
  found = FALSE;
  while (cur != NULL && !found)
    {
      /* shortcut : rules are sorted by type and view number */
      if (cur->PrType > type ||
	  (cur->PrType == type && cur->PrViewNum > 1) ||
	  (cur->PrType == type && type == PRFunction &&
	   cur->PrPresFunction > (FunctionType) extra))
	/* not found */
	cur = NULL;
      else if (type == PRFunction)
	{
	  /* check for extra specification in case of function rule */
	  if (cur->PrPresFunction == (FunctionType) extra)
	    found = TRUE;
	  else
	    cur = cur->PrNextPRule;
	}
      else if (type == cur->PrType)
	found = TRUE;
      else
	/* jump to next and keep track of previous */
	cur = cur->PrNextPRule;
    }
  return (cur);
}

/*----------------------------------------------------------------------
   Function used to to add a specific presentation rule
   for a given type of rule associated to an element.
   Parameter specificity is the specificity od the corresponding CSS rule.
  ----------------------------------------------------------------------*/
static PtrPRule InsertElementPRule (PtrElement el, PtrDocument pDoc,
				    PRuleType type, unsigned int extra,
				    int specificity)
{
   PtrPSchema          pSPR;
   PtrSSchema          pSSR;
   PtrAttribute        pAttr;
   PtrPRule            cur, prev, pRule, stdRule;

   cur = el->ElFirstPRule;
   stdRule = NULL;
   pRule = NULL;
   prev = NULL;
   while (cur != NULL)
     {
       /* shortcut : rules are sorted by type and view number */
       if (cur->PrType > type ||
	  (cur->PrType == type && type == PtFunction &&
	   cur->PrPresFunction > (FunctionType) extra))
	 cur = NULL;
       else
	 {
	   /* last specific rule */
	   prev = cur;
	   if (cur->PrViewNum == 1 && cur->PrType == type &&
	       (type != PRFunction ||
	       /* check for extra specification in case of function rule */
	       (type == PRFunction &&
		cur->PrPresFunction == (FunctionType) extra)))
	     {
	       /* this specific rule already exists */
	       pRule = cur;
	       cur = NULL;
	     }
	   else 
	     cur = cur->PrNextPRule;
	 }
     }

    if (pRule == NULL &&
	(type != PRFunction ||
	 extra != FnShowBox ||
	 !TypeHasException (ExcNoShowBox, el->ElTypeNumber, el->ElStructSchema)))
      {
	/* not found, allocate it, fill it and insert it */
	GetPresentRule (&pRule);
	if (pRule != NULL)
	  {
	    stdRule = GlobalSearchRulepEl (el, pDoc, &pSPR, &pSSR, 0, NULL, 1,
					   type, extra, FALSE, TRUE, &pAttr);
	    if (stdRule != NULL)
	      /* copy the standard rule */
	      *pRule = *stdRule;
	    else
		pRule->PrType = type;
	    pRule->PrCond = NULL;
	    pRule->PrSpecifAttr = 0;
	    pRule->PrSpecificity = specificity;
	    pRule->PrSpecifAttrSSchema = NULL;
	    /* set it specific to view 1 */
	    pRule->PrViewNum = 1;

	    /* Add the order / conditions .... */
	    /* chain in the rule */
	    if (prev == NULL)
	      {
		pRule->PrNextPRule = el->ElFirstPRule;
		el->ElFirstPRule = pRule;
	      }
	    else
	      {
		pRule->PrNextPRule = prev->PrNextPRule;
		prev->PrNextPRule = pRule;
	      }
	  }
      }
    return (pRule);
}

/*----------------------------------------------------------------------
   Function used to to remove a specific presentation rule
   for a given type of rule associated to an element.
  ----------------------------------------------------------------------*/
static void RemoveElementPRule (PtrElement el, PRuleType type, unsigned int extra)
{
    PtrPRule cur, prev;
    Document doc;
    
    prev = NULL;
    cur = el->ElFirstPRule;

    while (cur != NULL)
      {
	/* shortcut : rules are sorted by type and view number */
	if (cur->PrType > type ||
	    (cur->PrType == type && cur->PrViewNum > 1) ||
	    ((cur->PrType == type && type == PRFunction &&
	      cur->PrPresFunction > (FunctionType) extra)))
	  {
	    cur = NULL;
	    break;
	  }
	
	/* check for extra specification in case of function rule */
	if ((type == PRFunction) &&
	    (cur->PrPresFunction != (FunctionType) extra))
	  {
	    prev = cur;
	    cur = cur->PrNextPRule;
	    continue;
	  }

	/* check this rule */
	if (type == cur->PrType)
	  break;

	/* jump to next and keep track of previous */
	prev = cur;
	cur = cur->PrNextPRule;
      }
    if (cur == NULL)
      return;

    /* remove the rule from the chain */
    if (prev == NULL)
      el->ElFirstPRule = cur->PrNextPRule;
    else
      prev->PrNextPRule = cur->PrNextPRule;
    cur->PrNextPRule = NULL;
    
    /* update the presentation */
    doc = TtaGetDocument ((Element) el);
    ApplyASpecificStyleRule (cur, el, LoadedDocument[doc -1], TRUE);

    /* Free the PRule */
    FreePresentRule(cur, el->ElStructSchema);
    return;
}

/*----------------------------------------------------------------------
  BoxRuleInsert looks in the array of boxes for an entry corresponding
  to the current context. If not found we add a new one to the array.
  ----------------------------------------------------------------------*/
static PtrPRule *BoxRuleInsert (PtrPSchema tsch, GenericContext ctxt)
{
  PtrPresentationBox  box;
  int                 i, size;
  Name                boxname;

  BuildBoxName (ctxt, &boxname);

  /* search for the BOX in the Presentation Schema */
  for (i = 1; i <= tsch->PsNPresentBoxes; i++)
    {
      if (!strcmp (boxname, tsch->PsPresentBox->PresBox[i - 1]->PbName))
	{
	  ctxt->box = i;
	  return (&tsch->PsPresentBox->PresBox[i - 1]->PbFirstPRule);
	}
    }

  if (tsch->PsNPresentBoxes >= tsch->PsPresentBoxTableSize)
    /* the presentation box table is full. Extend it */
    {
      /* add 10 new entries */
      size = tsch->PsNPresentBoxes + 10;
      i = size * sizeof (PtrPresentationBox);
      if (!tsch->PsPresentBox)
	tsch->PsPresentBox = (PresBoxTable*) malloc (i);
      else
	tsch->PsPresentBox = (PresBoxTable*) realloc (tsch->PsPresentBox, i);
      if (!tsch->PsPresentBox)
	{
	  ctxt->box = 0;
	  return (NULL);
	}
      else
	{
	  tsch->PsPresentBoxTableSize = size;
	  for (i = tsch->PsNPresentBoxes; i < size; i++)
	    tsch->PsPresentBox->PresBox[i] = NULL;
	}
    }
  /* allocate and initialize the new presentation box */
  box = (PtrPresentationBox) malloc (sizeof (PresentationBox));
  if (box == NULL)
    {
      ctxt->box = 0;
      return (NULL);
    }
  tsch->PsPresentBox->PresBox[tsch->PsNPresentBoxes] = box;
  memset (box, 0, sizeof (PresentationBox));
  tsch->PsNPresentBoxes++;
  ctxt->box = tsch->PsNPresentBoxes;
  strncpy (box->PbName, boxname, sizeof (box->PbName));
  box->PbFirstPRule = NULL;
  box->PbPageFooter = FALSE;
  box->PbPageHeader = FALSE;
  box->PbPageBox = FALSE;
  box->PbFooterHeight = 0;
  box->PbHeaderHeight = 0;
  box->PbPageCounter = 0;
  box->PbContent = FreeContent;
  box->PbContVariable = 0;
  return (&tsch->PsPresentBox->PresBox[tsch->PsNPresentBoxes - 1]->PbFirstPRule);
}

/*----------------------------------------------------------------------
  PresConstInsert : add a constant to the constant array of a
  Presentation Schema and returns the associated index.
  ----------------------------------------------------------------------*/
static int          PresConstInsert (PSchema tcsh, char *value)
{
  PtrPSchema pSchemaPrs = (PtrPSchema) tcsh;
  int i;

  if (pSchemaPrs == NULL || value == NULL)
    return (-1);

  /* lookup the existing constants, searching for a corresponding entry */
  for (i = 0; i < pSchemaPrs->PsNConstants; i++)
    {
      if (pSchemaPrs->PsConstant[i].PdType == CharString &&
	  !strncmp (value, pSchemaPrs->PsConstant[i].PdString, MAX_PRES_CONST_LEN))
	return (i+1);
    }

  /* if not found, try to add it at the end */
  if (pSchemaPrs->PsNConstants >= MAX_PRES_CONST)
    return (-1);
  i = pSchemaPrs->PsNConstants;
  pSchemaPrs->PsConstant[i].PdType = CharString;
  pSchemaPrs->PsConstant[i].PdScript = 'L';
  strncpy (&pSchemaPrs->PsConstant[i].PdString[0], value, MAX_PRES_CONST_LEN);
  pSchemaPrs->PsNConstants++;
  return(i+1);
}

/*----------------------------------------------------------------------
  CompareCond : defines an absolute order on conditions.
  ----------------------------------------------------------------------*/
static int CompareCond (PtrCondition c1, PtrCondition c2)
{
  if (c1 == c2)
    return (0);
  if (c1 == NULL)
    return (-1);
  if (c2 == NULL)
    return (+1);

  /* Force PcElemType to be at the head of the condition list */
  if (c1->CoCondition == PcElemType && c2->CoCondition != PcElemType)
    return (-1);
  if (c1->CoCondition != PcElemType && c2->CoCondition == PcElemType)
    return (+1);

  /* otherwise apply natural order by nature */
  if (c1->CoCondition < c2->CoCondition)
    return (-1);
  if (c1->CoCondition > c2->CoCondition)
    return (+1);
  switch (c1->CoCondition)
    {
    case PcInterval:
    case PcEven:
    case PcOdd:
    case PcOne:
      if (c1->CoCounter < c2->CoCounter)
	return (-1);
      if (c1->CoCounter > c2->CoCounter)
	return (+1);
      else
	return (0);
    case PcWithin:
      if (c1->CoTypeAncestor < c2->CoTypeAncestor)
	return (-1);
      if (c1->CoTypeAncestor > c2->CoTypeAncestor)
	return (+1);
      if (c1->CoTypeAncestor == 0)
	return (+1);
      if (c1->CoRelation < c2->CoRelation)
	return (-1);
      if (c1->CoRelation > c2->CoRelation)
	return (+1);
      return (0);
    case PcElemType:
      if (c1->CoTypeElem < c2->CoTypeElem)
	return (-1);
      if (c1->CoTypeElem > c2->CoTypeElem)
	return (+1);
      if (c1->CoTypeElem == 0)
	return (+1);
      return (0);
    case PcAttribute:
    case PcInheritAttribute:
      if (c1->CoTypeAttr < c2->CoTypeAttr)
	return (-1);
      if (c1->CoTypeAttr > c2->CoTypeAttr)
	return (+1);
      if (c1->CoTypeAttr == 0 && c2->CoTypeAttr != 0)
	return (+1);
      if (!c1->CoTestAttrValue && c2->CoTestAttrValue)
	return (-1);
      if (c1->CoTestAttrValue && !c2->CoTestAttrValue)
	return (+1);
      if (c1->CoTestAttrValue && c2->CoTestAttrValue)
	{
	  if (c1->CoTextMatch < c2->CoTextMatch)
	    return (-1);
	  if (c1->CoTextMatch > c2->CoTextMatch)
	    return (+1);
	  return (strcasecmp (c1->CoAttrTextValue, c2->CoAttrTextValue));
	}
      return (0);
    default:
      return (+1);
    }
  return (+1);
}

/*----------------------------------------------------------------------
  AddCond : add a new condition in a presentation rule, respecting
  the order of the list.
  ----------------------------------------------------------------------*/
static void AddCond (PtrCondition *base, PtrCondition cond)
{
   PtrCondition        cour = *base;
   PtrCondition        next;

   if (cour == NULL)
     {
	*base = cond;
	cond->CoNextCondition = NULL;
	return;
     }
   if (CompareCond (cond, cour) <= 0)
     {
	*base = cond;
	cond->CoNextCondition = cour;
	return;
     }
   next = cour->CoNextCondition;
   while (next)
     {
	if (CompareCond (cond, next) <= 0)
	  {
	     cond->CoNextCondition = next;
	     cour->CoNextCondition = cond;
	     return;
	  }

	/* skip to next */
	cour = next;
	next = cour->CoNextCondition;
     }
   cour->CoNextCondition = cond;
   cond->CoNextCondition = NULL;
}

/*----------------------------------------------------------------------
  PresRuleAddAncestorCond : add an ancestor condition to a presentation rule.
  ----------------------------------------------------------------------*/
static void PresRuleAddAncestorCond (PtrPRule rule, int type, int nr)
{
   PtrCondition        cond = NULL;

   GetPresentRuleCond (&cond);
   memset (cond, 0, sizeof (Condition));
   if (cond == NULL)
     {
	TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
	return;
     }
   if (nr == 0)
     {
       /* the current element type must be ... */
       cond->CoCondition = PcElemType;
       cond->CoNotNegative = TRUE;
       cond->CoTarget = FALSE;
       cond->CoTypeElem = type;
     }
   else
     {
       cond->CoCondition = PcWithin;
       cond->CoTarget = FALSE;
       cond->CoNotNegative = TRUE;
       /* as it's greater we register the number of ancestors - 1 */
       cond->CoRelation = nr - 1;
       cond->CoTypeAncestor = type;
       cond->CoImmediate = FALSE;
       cond->CoAncestorRel = CondGreater;
       cond->CoAncestorName = NULL;
       cond->CoSSchemaName[0] = EOS;
     }
   AddCond (&rule->PrCond, cond);
}

/*----------------------------------------------------------------------
  PresRuleAddAttrCond : add a Attr condition to a presentation rule.
  ----------------------------------------------------------------------*/
static void PresRuleAddAttrCond (PtrPRule rule, int type, int level,
				 char* value, CondMatch match){

   PtrCondition        cond = NULL;

   GetPresentRuleCond (&cond);
   memset (cond, 0, sizeof (Condition));
   if (cond == NULL)
     {
	TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
	return;
     }
   if (level == 0)
     cond->CoCondition = PcAttribute;
   else
      cond->CoCondition = PcInheritAttribute;
   cond->CoNotNegative = TRUE;
   cond->CoTarget = FALSE;
   cond->CoTypeAttr = type;
   cond->CoTestAttrValue = (value != NULL);
   if (value)
     {
       cond->CoAttrTextValue = TtaStrdup (value);
       cond->CoTextMatch = match;
     }
   else
     cond->CoAttrTextValue = NULL;
   AddCond (&rule->PrCond, cond);
}

static char ListOfValues[MAX_LENGTH];
/*----------------------------------------------------------------------
  TtaGetStyledAttributeValues returns the list of values of the attribute
  attrType which have an entry in the presentation schema tsch.
  ----------------------------------------------------------------------*/
char *TtaGetStyledAttributeValues (PSchema tsch, int attrType)
{
  AttributePres      *attrs;
  int                 i, len;

  i = 0;
  ListOfValues[i] = EOS;
  attrs = ((PtrPSchema) tsch)->PsAttrPRule->AttrPres[attrType - 1];
  while (attrs)
    {
      if (attrs->ApString)
	{
	  /* add that new value */
	  len = strlen (attrs->ApString) + 1;
	  if (i + len <= MAX_LENGTH)
	    strncpy (&ListOfValues[i], attrs->ApString, len);
	  i += len;
	}
      attrs = attrs->ApNextAttrPres;
    }
  return ListOfValues;
}

/*----------------------------------------------------------------------
  FirstPresAttrRuleSearch: look in the array of Attribute presentation
  blocks, for a block and a rule corresponding to the current context.
  When the rule is not found, attrblock points to the last block.
  ----------------------------------------------------------------------*/
static PtrPRule *FirstPresAttrRuleSearch (PtrPSchema tsch, int attrType,
					  GenericContext ctxt, int att,
					  AttributePres **attrblock)
{
  PtrPRule           *ppRule;
  PtrSSchema          pSS;
  AttributePres      *attrs;
  char               *attrVal;
  unsigned int        elementType;
  int                 nbrules;
  int                 i, j, val;
  unsigned int        match;
  

  /* select the right attribute */
  attrs = tsch->PsAttrPRule->AttrPres[attrType - 1];
  *attrblock = NULL;
  nbrules = tsch->PsNAttrPRule->Num[attrType - 1];
  pSS = (PtrSSchema) ctxt->schema;
  ppRule = NULL;
  attrVal = ctxt->attrText[att];
  elementType = ctxt->name[att];
  match = ctxt->attrMatch[att];
  for (i = 0; i < nbrules && !ppRule && attrs; i++)
    {
      if ((att > 0 && attrs->ApElemType != (int)(ctxt->type)) ||
	  (att == 0 && attrs->ApElemType != 0))
	{
	  if (ctxt->type == 0)
	    /* this new rule is less specific and should be added before */
	    attrs = NULL;
	  else
	    {
	      *attrblock = attrs;
	      attrs = attrs->ApNextAttrPres;
	    }
	}
      else
	{
	  switch (pSS->SsAttribute->TtAttr[attrType - 1]->AttrType)
	    {
	    case AtNumAttr:
	      if (attrVal)
		sscanf (attrVal, "%d", &val);
	      else
		val = 0;
	      if (val)
		{
		for (j = 0; j < attrs->ApNCases && !ppRule ; j++)
		  if (attrs->ApCase[j].CaComparType == ComparConstant &&
		      attrs->ApCase[j].CaLowerBound == val &&
		      attrs->ApCase[j].CaUpperBound == val)
		    ppRule = &(attrs->ApCase[j].CaFirstPRule);
		  else
		    for (j = 0; j < attrs->ApNCases && !ppRule ; j++)
		      if (attrs->ApCase[j].CaComparType == ComparConstant &&
			  attrs->ApCase[j].CaLowerBound < -MAX_INT_ATTR_VAL &&
			  attrs->ApCase[j].CaUpperBound > MAX_INT_ATTR_VAL)
			ppRule = &(attrs->ApCase[j].CaFirstPRule);
		}
	      break;
	    case AtTextAttr:
	      if (attrVal && attrs->ApString &&
		  !strcmp (attrs->ApString, attrVal) &&
		  attrs->ApMatch == match)
		ppRule = &(attrs->ApTextFirstPRule);
	      else if (attrVal == NULL && 
		       (attrs->ApString == NULL || attrs->ApString[0] == EOS))
		ppRule = &(attrs->ApTextFirstPRule);
	      break;
	    case AtReferenceAttr:
	      ppRule = &(attrs->ApRefFirstPRule);
	      break;
	    case AtEnumAttr:
	      val = (int) attrVal;
	      if (val)
		ppRule = &(attrs->ApEnumFirstPRule[val - 1]);
	      else
		ppRule = &(attrs->ApEnumFirstPRule[0]);
	      break;
	    }
	  *attrblock = attrs;
	  attrs = attrs->ApNextAttrPres;
	}
    }
  return (ppRule);
}

/*----------------------------------------------------------------------
  PresAttrChainInsert : look in the array of Attribute presentation
  blocks, for a block corresponding to the current context.
  if not found we add a new one to the array.
  ----------------------------------------------------------------------*/
static PtrPRule *PresAttrChainInsert (PtrPSchema tsch, int attrType,
				      GenericContext ctxt, int att)
{
  AttributePres      *attrs, *new;
  PtrSSchema          pSS;
  PtrPSchema          pSP;
  PtrPRule           *ppRule;
  char               *attrVal;
  int                 nbrules, val, match;

  pSS = (PtrSSchema) ctxt->schema;
  ppRule = FirstPresAttrRuleSearch (tsch, attrType, ctxt, att, &attrs);
  /* If no attribute presentation rule is found, create and initialize it */
  if (!ppRule)
    {
      /* select the last entry */
      nbrules = tsch->PsNAttrPRule->Num[attrType - 1] + 1;
      /* add the new entry */
      GetAttributePres (&new);
      tsch->PsNAttrPRule->Num[attrType - 1] = nbrules;
      if (att > 0 && ctxt->type)
	{
	  new->ApElemType = ctxt->type;
	  pSP = PresentationSchema (pSS, LoadedDocument[ctxt->doc-1]);
	  if (pSP)
	    {
	      pSP->PsNInheritedAttrs->Num[ctxt->type - 1]++;
	      pSP->PsNHeirElems->Num[attrType - 1]++;
	    }
	}
      if (attrs)
	{
	  new->ApNextAttrPres = attrs->ApNextAttrPres;
	  attrs->ApNextAttrPres = new;
	}
      else
	{
	  new->ApNextAttrPres = tsch->PsAttrPRule->AttrPres[attrType - 1];
	  tsch->PsAttrPRule->AttrPres[attrType - 1] = new;
	}

      attrVal = ctxt->attrText[att];
      match = ctxt->attrMatch[att];
      switch (pSS->SsAttribute->TtAttr[attrType - 1]->AttrType)
	{
	case AtNumAttr:
	  new->ApNCases = 1;
	  if (attrVal)
	    sscanf (attrVal, "%d", &val);
	  else
	    val = 0;
	  if (val)
	    {
	      new->ApCase[0].CaLowerBound = val;
	      new->ApCase[0].CaUpperBound = val;
	    }
	  else
	    {   
	      new->ApCase[0].CaLowerBound = -MAX_INT_ATTR_VAL - 1;
	      new->ApCase[0].CaUpperBound = MAX_INT_ATTR_VAL + 1;
	    }
	  new->ApCase[0].CaComparType = ComparConstant;
	  new->ApCase[0].CaFirstPRule = NULL;
	  return (&(new->ApCase[0].CaFirstPRule));
	  break;
	case AtTextAttr:
	  new->ApMatch = match;
	  if (attrVal)
	    new->ApString = TtaStrdup (attrVal);
	  else
	    new->ApString = NULL;
	  new->ApTextFirstPRule = NULL;
	  return (&(new->ApTextFirstPRule));
	  break;
	case AtReferenceAttr:
	  new->ApRefFirstPRule = NULL;
	  return (&(new->ApRefFirstPRule));
	  break;
	case AtEnumAttr:
	  /* get the attribute value */
	  val = (int) attrVal;
	  if (val)
	    {
	      new->ApEnumFirstPRule[val - 1] = NULL;
	      return (&(new->ApEnumFirstPRule[val - 1]));
	    }
	  else
	    {
	      new->ApEnumFirstPRule[0] = NULL;
	      return (&(new->ApEnumFirstPRule[0]));
	    }
	  break;
	}
    }

  return (ppRule);
}

/*----------------------------------------------------------------------
  TstRuleContext : test if a presentation rule correponds to the
  context given in argument and the correct presentation rule type.
  All the rules in a rule list are sorted :
  *      first by type,
  *      second by View,
  *      and Last by conditions
  If pres is zero, we don't test on the kind of rule ...
  If att is less than MAX_ANCESTORS we're testing conditions of an
  attribute rule, otherwise we're testing conditions for element rules.
  Returns:
  * 0 if the rule has all needed condititons
  * -1 if the rule has less conditions than neeeded
  * 1 if the rule has more conditions than neeeded
  ----------------------------------------------------------------------*/
static int TstRuleContext (PtrPRule rule, GenericContext ctxt,
			   PRuleType pres, unsigned int att)
{
  PtrCondition        firstCond, cond;
  int                 nbcond, nbCtxtCond, prevAttr;
  unsigned            i;

  /* test the number and type of the rule */
  if (rule->PrViewNum != 1)
    return (-1);
  if (pres && rule->PrType != pres)
    return (-1);

  /* scan all the conditions associated to a rule */
  firstCond = rule->PrCond;
  /* check the number of condititons */
  cond = firstCond;
  nbcond = 0;
  while (cond)
    {
      cond = cond->CoNextCondition;
      nbcond++;
    }
  nbCtxtCond = 0;
  prevAttr = ctxt->attrType[0];
  if (att < MAX_ANCESTORS)
    /* the rule is associated to an attribute */
    {
      /* count the number of conditions in the context */
      for (i = 1; i < MAX_ANCESTORS; i++)
	{
	  if (ctxt->name[i])
	    nbCtxtCond++;
	  if (ctxt->attrType[i])
	    {
	    if (prevAttr)
	      nbCtxtCond++;
	    else
	      prevAttr = ctxt->attrType[i];
	    }
	}
    }
  else
    /* the rule is associated with an element */
    {
      /* count the number of conditions in the context */
      for (i = 1; i < MAX_ANCESTORS; i++)
	if (ctxt->name[i])
	  nbCtxtCond++;
    }

  if (nbCtxtCond < nbcond)
    return (1);
  else if (nbCtxtCond > nbcond)
    return (-1);

  /* same number of conditions */
  /* check if all ancestors are within the rule conditions */
  i = 1;
  while (i < MAX_ANCESTORS)
     {
       if (ctxt->names_nb[i] > 0)
	 {
	   cond = firstCond;
	   while (cond &&
		  (cond->CoCondition != PcWithin ||
		   cond->CoTypeAncestor != ctxt->name[i] ||
		   cond->CoRelation != ctxt->names_nb[i] - 1))
	     cond = cond->CoNextCondition;
	   if (cond == NULL)
	     /* the ancestor is not found */
	     return (1);
	 }
       if (ctxt->attrType[i] && i != att)
	 {
	   cond = firstCond;
	   while (cond &&
		  ((cond->CoCondition != PcInheritAttribute &&
		    ctxt->attrLevel[i] != 0) ||
		   (cond->CoCondition != PcAttribute &&
		    ctxt->attrLevel[i] == 0) ||
		   cond->CoTypeAttr != ctxt->attrType[i] ||
		   cond->CoTestAttrValue != (ctxt->attrText != NULL) ||
		   (cond->CoTestAttrValue &&
		    (cond->CoTextMatch != ctxt->attrMatch[i] ||
		    (cond->CoAttrTextValue &&
		     strcasecmp (cond->CoAttrTextValue, ctxt->attrText[i]))))))
	     cond = cond->CoNextCondition;
	   if (cond == NULL)
	     /* conditions are different */
	     return (1);
	 }
       i++;
     }

  /* all conditions are the same */
  return (0);
}

/*----------------------------------------------------------------------
  PresRuleSearch : search a presentation rule for a given view
  in an attribute chain or an element chain.
  ----------------------------------------------------------------------*/
static PtrPRule PresRuleSearch (PtrPSchema tsch, GenericContext ctxt,
				PRuleType pres, unsigned int extra,
				PtrPRule **chain)
{
  PtrPRule            pRule;
  unsigned int        attrType, att;
  int                 condCheck;
  ThotBool            found;

  *chain = NULL;
  /* by default the rule doesn't concern any attribute */
  attrType = 0;
  att = MAX_ANCESTORS;

  /* select the good starting point depending on the context */
  if (ctxt->box != 0)
    *chain = BoxRuleInsert (tsch, ctxt);
  else
    {
      /*
	detect whether there is an attribute in the selector to generate
	an attribute rule with conditions on the current element or
	ancestor elements.
      */
      att = 0;
      while (att < MAX_ANCESTORS && ctxt->attrType[att] == 0)
	att++;
      if (att < MAX_ANCESTORS)
	{
	  attrType = ctxt->attrType[att];
	  *chain = PresAttrChainInsert (tsch, attrType, ctxt, att);
	}
      else if (ctxt->type)
	/* we are now sure that only elements are concerned */
	*chain = &tsch->PsElemPRule->ElemPres[ctxt->type - 1];
      else
	return (NULL);
    }

  /*
   * scan the chain of presentation rules looking for an existing
   * rule for this context and kind of presentation attribute.
   */
  pRule = **chain;
  found = FALSE;
  while (!found && pRule != NULL)
    {
      /* shortcut : rules are sorted by type and view number and
	 Functions rules are sorted by number */
      if (pRule->PrType > pres ||
	  (pRule->PrType == pres && pRule->PrViewNum > 1) ||
	  (pRule->PrType == pres && pres == PtFunction &&
	   pRule->PrPresFunction >  (FunctionType) extra))
	  pRule = NULL;
      else if (pRule->PrType != pres ||
	       (pres == PtFunction &&
		pRule->PrPresFunction != (FunctionType) extra))
	/* check for extra specification in case of function rule */
	{
	  *chain = &(pRule->PrNextPRule);
	  pRule = pRule->PrNextPRule;
	}
      else
	{
	  condCheck = TstRuleContext (pRule, ctxt, pres, att);
	  if (condCheck == 0)
	    /* this rule already exists */
	    found = TRUE;
	  else if (condCheck > 0)
	    {
	      /* the new rule should be added after */
	      *chain = &(pRule->PrNextPRule);
	      /* jump to next */
	      pRule = pRule->PrNextPRule;
	    }
	  else
	      /* the new rule should be added before */
	    pRule = NULL;
	}
    }
   return (pRule);
}

/*----------------------------------------------------------------------
  PresRuleInsert : insert a new presentation rule for a given type
  in a chain. If it already exists, return the current block.
  In a chain all the rules are sorted by type and also by view.
  ----------------------------------------------------------------------*/
static PtrPRule PresRuleInsert (PtrPSchema tsch, GenericContext ctxt,
				PRuleType pres, unsigned int extra)
{
  PtrPRule           *chain;
  PtrPRule            cur, pRule = NULL;
  int                 i, att;

  /* Search presentation rule */
  cur = PresRuleSearch (tsch, ctxt, pres, extra, &chain);
  if (cur != NULL)
    return (cur);
  else if (pres != PRFunction ||
	   extra != FnShowBox ||
	   !TypeHasException (ExcNoShowBox, ctxt->type, (PtrSSchema) ctxt->schema))
    {
      /* not found, allocate it, fill it and insert it */
      GetPresentRule (&pRule);

      if (pRule != NULL)
	{
	  pRule->PrType = pres;
	  pRule->PrCond = NULL;
	  pRule->PrViewNum = 1;
	  pRule->PrSpecifAttr = 0;
	  pRule->PrImportant = ctxt->important;
	  /* store the rule priority */
	  pRule->PrSpecificity = ctxt->cssSpecificity;
	  pRule->PrSpecifAttrSSchema = NULL;
      
	  /* In case of an attribute rule, add the Attr condition */
	  att = 0;
	  while (att < MAX_ANCESTORS && ctxt->attrType[att] == 0)
	  att++;
	  if (att == 0 && ctxt->type)
	    /* the attribute is attached to that element like a selector "a#id" */
	    PresRuleAddAncestorCond (pRule, ctxt->type, 0);
	  /* add other conditions ... */
	  i = 0;
	  while (i < MAX_ANCESTORS)
	    {
	      if (i != 0 && ctxt->name[i] && ctxt->names_nb[i] > 0)
		/* it's an ancestor like a selector "li a" */
		PresRuleAddAncestorCond (pRule, ctxt->name[i], ctxt->names_nb[i]);
	      if (ctxt->attrType[i]  && i != att)
		/* it's another attribute */
		PresRuleAddAttrCond (pRule, ctxt->attrType[i], ctxt->attrLevel[i],
				     ctxt->attrText[i], ctxt->attrMatch[i]);
	      i++;
	    }

	  /* Add the order / conditions .... */
	  /* chain in the rule */
	  if (chain != NULL)
	    {
	      pRule->PrNextPRule = *chain;
	      *chain = pRule;
	    }
	}
    }
  return (pRule);
}

/*----------------------------------------------------------------------
  PresRuleRemove : remove an existing presentation rule for a given type
  in a chain if it exists.
  ----------------------------------------------------------------------*/
static void PresRuleRemove (PtrPSchema tsch, GenericContext ctxt,
			    PRuleType pres, unsigned int extra)
{
  PtrPRule         *chain;
  PtrPRule          cur;
  Document          doc;
  PtrSSchema        pSS;
  int               elType;
  int               attrType;
  int               presBox;

  /* Search presentation rule */
  cur = PresRuleSearch (tsch, ctxt, pres, extra, &chain);
  if (cur != NULL)
    {
      if (chain != NULL)
	/* found, remove it from the chain */
	*chain = cur->PrNextPRule;

      cur->PrNextPRule = NULL;
      /* update the rendering */
      doc = ctxt->doc;
      /* remove that specific rule */
      presBox = ctxt->box;
      attrType = 0;
      elType = 0;
      pSS = (PtrSSchema) ctxt->schema;
      ApplyAGenericStyleRule (doc, pSS, elType, attrType, presBox, cur, TRUE);
      /* Free the PRule */
      FreePresentRule(cur, pSS);
    }
}

/*----------------------------------------------------------------------
 PresentationValueToPRule : set up an internal Presentation Rule according
 to a Presentation Value for a given type of presentation property.
 funcType is an extra parameter needed when using a Function rule
 or a position rule (VertPos, HorizPos).
 ----------------------------------------------------------------------*/
static void PresentationValueToPRule (PresentationValue val, int type,
				      PtrPRule rule, int funcType,
				      ThotBool absolute, ThotBool generic,
				      ThotBool minValue)
{
  TypeUnit            int_unit;
  int                 value;
  int                 unit;
  ThotBool            real;

  value = val.typed_data.value;
  unit = val.typed_data.unit;
  real = val.typed_data.real;
  /* The drivers affect only the main "WYSIWYG" view */
  rule->PrViewNum = 1;
  /*
   * normalize the unit to fit the Thot internal ones.
   * The driver interface accept floats with up to 3 digits
   * after the dot, e.g. 1.115 , coded as :
   *      real = TRUE
   *      val = 1115
   */
  int_unit = UnRelative;
  switch (unit)
    {
    case UNIT_REL:
      int_unit = UnRelative;
      if (type == PtHeight ||            type == PtWidth ||
	  type == PtVertPos ||           type == PtHorizPos ||
	  type == PtMarginTop ||         type == PtMarginRight ||
	  type == PtMarginBottom ||      type == PtMarginLeft ||
	  type == PtPaddingTop ||        type == PtPaddingRight ||
	  type == PtPaddingBottom ||     type == PtPaddingLeft ||
	  type == PtBorderTopWidth ||    type == PtBorderRightWidth ||
	  type == PtBorderBottomWidth || type == PtBorderLeftWidth ||
          type == PtLineWeight ||        type == PtThickness ||
	  type == PtIndent ||            type == PtLineSpacing ||
          type == PtBreak1 ||            type == PtBreak2 ||
          type == PtXRadius ||           type == PtYRadius)
	value *= 10;
      break;
    case UNIT_EM:
      int_unit = UnRelative;
      value *= 10;
      break;
    case UNIT_PT:
      int_unit = UnPoint;
      break;
    case UNIT_PC:
      int_unit = UnPoint;
      value *= 12;
      break;
    case UNIT_IN:
      int_unit = UnPoint;
      value *= 72;
      break;
    case UNIT_CM:
      int_unit = UnPoint;
      value *= 28;
      break;
    case UNIT_MM:
      int_unit = UnPoint;
      value *= 28;
      value /= 10;
      break;
    case UNIT_PX:
      int_unit = UnPixel;
      break;
    case UNIT_PERCENT:
      int_unit = UnPercent;
      break;
    case UNIT_XHEIGHT:
      int_unit = UnXHeight;
      value *= 10;
      break;
    case UNIT_BOX:
      break;
    case VALUE_AUTO:
      int_unit = UnAuto;
      break;
    default:
      break;
    }

    if (real)
      value = (value + 500) / 1000;
  
  /* now, set-up the value */
  switch (type)
    {
    case PtVisibility:
    case PtDepth:
    case PtFillPattern:
    case PtBackground:
    case PtForeground:
    case PtBorderTopColor:
    case PtBorderRightColor:
    case PtBorderBottomColor:
    case PtBorderLeftColor:
    case PtStrokeOpacity:
    case PtFillOpacity:
    case PtOpacity:
      rule->PrPresMode = PresImmediate;
      rule->PrAttrValue = FALSE;
      rule->PrIntValue = value;
      break;
    case PtFont:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case FontHelvetica:
	  rule->PrChrValue = 'H';
	  break;
	case FontTimes:
	  rule->PrChrValue = 'T';
	  break;
	case FontCourier:
	  rule->PrChrValue = 'C';
	  break;
#ifdef _WINDOWS
#ifdef _I18N_
	case FontOther:
	  rule->PrChrValue = -1;
	  break;
#endif /* _I18N_ */
#endif /* _WINDOWS */
	}
      break;
    case PtStyle:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case StyleRoman:
	  rule->PrChrValue = 'R';
	  break;
	case StyleItalics:
	  rule->PrChrValue = 'I';
	  break;
	case StyleOblique:
	  rule->PrChrValue = 'O';
	  break;
	default:
	  rule->PrChrValue = 'R';
	  break;	   
	}
      break;
    case PtWeight:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case WeightNormal:
	  rule->PrChrValue = 'N';
	  break;
	case WeightBold:
	  rule->PrChrValue = 'B';
	  break;
	default:
	  rule->PrChrValue = 'N';
	  break;	   
	}
      break;
    case PtUnderline:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case Underline:
	  rule->PrChrValue = 'U';
	  break;
	case Overline:
	  rule->PrChrValue = 'O';
	  break;
	case CrossOut:
	  rule->PrChrValue = 'C';
	  break;
	}
      break;
    case PtThickness:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case ThinUnderline:
	  rule->PrChrValue = 'N';
	  break;
	case ThickUnderline:
	  rule->PrChrValue = 'T';
	  break;
	}
      break;
    case PtLineStyle:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case SolidLine:
	  rule->PrChrValue = 'S';
	  break;
	case DashedLine:
	  rule->PrChrValue = '-';
	  break;
	case DottedLine:
	  rule->PrChrValue = '.';
	  break;
	}
      break;
    case PtDisplay:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case Undefined:
	  rule->PrChrValue = 'U';
	  break;
	case Inline:
	  rule->PrChrValue = 'I';
	  break;
	case Block:
	  rule->PrChrValue = 'B';
	  break;
	case ListItem:
	  rule->PrChrValue = 'L';
	  break;
	case RunIn:
	  rule->PrChrValue = 'R';
	  break;
	case Compact:
	  rule->PrChrValue = 'C';
	  break;
	case Marker:
	  rule->PrChrValue = 'M';
	  break;
	}
      break;
    case PtFloat:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case FloatLeft:
	  rule->PrChrValue = 'L';
	  break;
	case FloatRight:
	  rule->PrChrValue = 'R';
	  break;
	default:
	  rule->PrChrValue = 'N';
	  break;
	}
      break;
    case PtClear:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case ClearLeft:
	  rule->PrChrValue = 'L';
	  break;
	case ClearRight:
	  rule->PrChrValue = 'R';
	  break;
	case ClearBoth:
	  rule->PrChrValue = 'B';
	  break;
	default:
	  rule->PrChrValue = 'N';
	  break;
	}
      break;
    case PtBorderTopStyle:
    case PtBorderRightStyle:
    case PtBorderBottomStyle:
    case PtBorderLeftStyle:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case BorderStyleNone:
	  rule->PrChrValue = '0';
	  break;
	case BorderStyleHidden:
	  rule->PrChrValue = 'H';
	  break;
	case BorderStyleDotted:
	  rule->PrChrValue = '.';
	  break;
	case BorderStyleDashed:
	  rule->PrChrValue = '-';
	  break;
	case BorderStyleSolid:
	  rule->PrChrValue = 'S';
	  break;
	case BorderStyleDouble:
	  rule->PrChrValue = 'D';
	  break;
	case BorderStyleGroove:
	  rule->PrChrValue = 'G';
	  break;
	case BorderStyleRidge:
	  rule->PrChrValue = 'R';
	  break;
	case BorderStyleInset:
	  rule->PrChrValue = 'I';
	  break;
	case BorderStyleOutset:
	  rule->PrChrValue = 'O';
	  break;
	}
      break;
    case PtBreak1:
    case PtBreak2:
    case PtIndent:
    case PtLineSpacing:
    case PtLineWeight:
    case PtBorderTopWidth:
    case PtBorderLeftWidth:
    case PtBorderBottomWidth:
    case PtBorderRightWidth:
    case PtPaddingTop:
    case PtPaddingLeft:
    case PtPaddingBottom:
    case PtPaddingRight:
    case PtXRadius:
    case PtYRadius:
      rule->PrPresMode = PresImmediate;
      rule->PrMinUnit = int_unit;
      rule->PrMinValue = value;
      rule->PrMinAttr = FALSE;
      break;
    case PtMarginTop:
    case PtMarginLeft:
    case PtMarginBottom:
    case PtMarginRight:
      rule->PrPresMode = PresImmediate;
      if (val.typed_data.unit == VALUE_AUTO)
	{
	  rule->PrMinUnit = UnAuto;
	  rule->PrMinValue = 0;
	}
      else
	{
	  rule->PrMinUnit = int_unit;
	  rule->PrMinValue = value;
	}
      rule->PrMinAttr = FALSE;
      break;
    case PtSize:
      if (int_unit == UnPercent)
	{
        rule->PrPresMode = PresInherit;
        rule->PrInheritMode = InheritParent;
        rule->PrInhPercent = True;
        rule->PrInhAttr = False;
        rule->PrInhDelta = value;
        rule->PrMinMaxAttr = False;
        rule->PrInhMinOrMax = 0;
        rule->PrInhUnit = UnRelative;
	}
      else
	{
        rule->PrPresMode = PresImmediate;
        rule->PrMinUnit = int_unit;
        rule->PrMinValue = value;
        rule->PrMinAttr = FALSE;
        }
      break;
    case PtAdjust:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case AdjustLeft:
	  rule->PrAdjust = AlignLeft;
	  break;
	case AdjustRight:
	  rule->PrAdjust = AlignRight;
	  break;
	case Centered:
	  rule->PrAdjust = AlignCenter;
	  break;
	case LeftWithDots:
	  rule->PrAdjust = AlignLeftDots;
	  break;
	case Justify:
	  rule->PrAdjust = AlignJustify;
	  break;
	default:
	  rule->PrAdjust = AlignLeft;
	  break;
	}
      break;
    case PtDirection:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case LeftToRight:
	  rule->PrChrValue = 'L';
	  break;
	case RightToLeft:
	  rule->PrChrValue = 'R';
	  break;
	default:
	  rule->PrChrValue = 'L';
	  break;	  
	}
      break;
    case PtUnicodeBidi:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case Normal:
	  rule->PrChrValue = 'N';
	  break;
	case Embed:
	  rule->PrChrValue = 'E';
	  break;
	case Override:
	  rule->PrChrValue = 'O';
	  break;
	default:
	  rule->PrChrValue = 'N';
	  break;	  
	}
      break;
    case PtHyphenate:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case Hyphenation:
	  rule->PrBoolValue = TRUE;
	  break;
	case NoHyphenation:
	  rule->PrBoolValue = FALSE;
	  break;
	}
      break;
    case PtVertRef:
    case PtHorizRef:
      rule->PrPresMode = PresImmediate;
      rule->PrPosRule.PoDistUnit = int_unit;
      rule->PrPosRule.PoDistance = value;
      break;
    case PtVertPos:
      rule->PrPresMode = PresImmediate;
      if (funcType == 0)
	{
	  rule->PrPosRule.PoDistUnit = int_unit;
	  rule->PrPosRule.PoDistance = value;
	}
      else
	/* funcType represents the axis used for positionning */
	switch (funcType)
	  {
	  case PositionTop:
	    rule->PrPosRule.PoPosDef = Top;
	    break;
	  case PositionBottom:
	    rule->PrPosRule.PoPosDef = Bottom;
	    break;
	  case PositionHorizRef:
	    rule->PrPosRule.PoPosDef = HorizRef;
	    break;
	  case PositionHorizMiddle:
	    rule->PrPosRule.PoPosDef = HorizMiddle;
	    break;
	  default:
	    break;
	  }
      if (generic)
	{
	  /* generate a complete rule Top=Previous AnyElem.Bottom+value */
	  rule->PrPosRule.PoPosDef = Top;
	  rule->PrPosRule.PoPosRef = Bottom;
	  rule->PrPosRule.PoRelation = RlPrevious;
	  rule->PrPosRule.PoNotRel = FALSE;
	  rule->PrPosRule.PoRefKind = RkAnyElem;
	  rule->PrPosRule.PoUserSpecified = FALSE;
	  rule->PrPosRule.PoRefIdent = 0;	  
	}
      break;
    case PtHorizPos:
      rule->PrPresMode = PresImmediate;
      if (funcType == 0)
	{
	  rule->PrPosRule.PoDistUnit = int_unit;
	  rule->PrPosRule.PoDistance = value;
	}
      else
	/* funcType represents the axis used for positionning */
	switch (funcType)
	  {
	  case PositionLeft:
	    rule->PrPosRule.PoPosDef = Left;
	    break;
	  case PositionRight:
	    rule->PrPosRule.PoPosDef = Right;
	    break;
	  case PositionVertRef:
	    rule->PrPosRule.PoPosDef = VertRef;
	    break;
	  case PositionVertMiddle:
	    rule->PrPosRule.PoPosDef = VertMiddle;
	    break;
	  default:
	    break;
	  }
      if (generic)
	{
	  /* generate a complete rule Left=Enclosing.left+value */
	  rule->PrPosRule.PoPosDef = Left;
	  rule->PrPosRule.PoPosRef = Left;
	  rule->PrPosRule.PoRelation = RlEnclosing;
	  rule->PrPosRule.PoNotRel = FALSE;
	  rule->PrPosRule.PoRefKind = RkElType;
	  rule->PrPosRule.PoUserSpecified = FALSE;
	  rule->PrPosRule.PoRefIdent = 0;	  
	}
      break;
    case PtHeight:
      rule->PrPresMode = PresImmediate;
      if (generic)
	{
	  /* generate a complete rule Height=Enclosed.Height */
	  rule->PrDimRule.DrPosition = FALSE;
	  rule->PrDimRule.DrAbsolute = FALSE;
	  rule->PrDimRule.DrSameDimens = TRUE;
	  rule->PrDimRule.DrUnit = int_unit;
	  rule->PrDimRule.DrAttr = FALSE;
	  rule->PrDimRule.DrMin = minValue;
	  rule->PrDimRule.DrUserSpecified = FALSE;
	  rule->PrDimRule.DrRelation = RlEnclosed;
	  rule->PrDimRule.DrNotRelat = FALSE;
	  rule->PrDimRule.DrRefKind = RkElType;
	  rule->PrDimRule.DrRefIdent = 0;	  
	}
      if (rule->PrDimRule.DrPosition)
          {
	  rule->PrDimRule.DrPosRule.PoDistUnit = int_unit;
	  rule->PrDimRule.DrPosRule.PoDistAttr = FALSE;
	  rule->PrDimRule.DrPosRule.PoDistance = value;
	  }
      else
	{
	  rule->PrDimRule.DrUnit = int_unit;
	  rule->PrDimRule.DrAttr = FALSE;
	  if (absolute)
	    {
	      if (val.typed_data.unit == VALUE_AUTO)
		/* it means "height: auto" */
		{
		  rule->PrDimRule.DrPosition = FALSE;
		  rule->PrDimRule.DrAbsolute = FALSE;
		  rule->PrDimRule.DrRelation = RlEnclosed;
		}
	      else
		{
		  rule->PrDimRule.DrAbsolute = TRUE;
		  rule->PrDimRule.DrValue = value;
		}
	    }
	  else
	    rule->PrDimRule.DrValue = 0;
	}
      break;
    case PtWidth:
      rule->PrPresMode = PresImmediate;
      if (generic)
	{
	  /* generate a complete rule Width=Enclosing.Width+value */
	  rule->PrDimRule.DrPosition = FALSE;
	  rule->PrDimRule.DrAbsolute = FALSE;
	  rule->PrDimRule.DrSameDimens = TRUE;
	  rule->PrDimRule.DrUnit = int_unit;
	  rule->PrDimRule.DrAttr = FALSE;
	  rule->PrDimRule.DrMin = minValue;
	  rule->PrDimRule.DrUserSpecified = FALSE;
	  rule->PrDimRule.DrRelation = RlEnclosing;
	  rule->PrDimRule.DrNotRelat = FALSE;
	  rule->PrDimRule.DrRefKind = RkElType;
	  rule->PrDimRule.DrRefIdent = 0;
	}
      if (rule->PrDimRule.DrPosition)
          {
	  rule->PrDimRule.DrPosRule.PoDistUnit = int_unit;
	  rule->PrDimRule.DrPosRule.PoDistAttr = FALSE;
	  rule->PrDimRule.DrPosRule.PoDistance = value;
	  }
      else
	{
	  rule->PrDimRule.DrUnit = int_unit;
	  rule->PrDimRule.DrAttr = FALSE;
	  if (absolute)
	    {
	      if (val.typed_data.unit == VALUE_AUTO)
		/* it means "width: auto" */
		{
		  rule->PrDimRule.DrPosition = FALSE;
		  rule->PrDimRule.DrAbsolute = FALSE;
		  rule->PrDimRule.DrRelation = RlEnclosed;
		}
	      else
		{
		  rule->PrDimRule.DrAbsolute = TRUE;
		  rule->PrDimRule.DrValue = value;
		}
	    }
	  else if (int_unit == UnPercent)
	    rule->PrDimRule.DrValue = 100 - value;
	  else
	    rule->PrDimRule.DrValue = -value;
	}
      break;
    case PtPictInfo:
      break;
    case PtVertOverflow:
    case PtHorizOverflow:
      rule->PrPresMode = PresImmediate;
      rule->PrBoolValue = TRUE;
      break;
    case PtFunction:
      rule->PrPresMode = PresFunction;
      switch (funcType)
	{
	case FnCreateBefore:
	case FnCreateWith:
	case FnCreateFirst:
	case FnCreateLast:
	case FnCreateAfter:
	case FnCreateEnclosing:
	  rule->PrPresFunction = (FunctionType) funcType;
	  rule->PrNPresBoxes = 0;
	  rule->PrElement = TRUE;
	  break;
	case FnShowBox:
	  rule->PrPresFunction = (FunctionType) funcType;
	  rule->PrNPresBoxes = value;
	  break;
	case FnBackgroundPicture:
	  rule->PrPresFunction = (FunctionType) funcType;
	  rule->PrNPresBoxes = 1;
	  rule->PrPresBox[0] = value;
	  break;
	case FnPictureMode:
	  rule->PrPresFunction = (FunctionType) funcType;
	  rule->PrNPresBoxes = 1;
	  switch (value)
	    {
	    case REALSIZE:
	      rule->PrPresBox[0] = RealSize;
	      break;
	    case SCALE:
	      rule->PrPresBox[0] = ReScale;
	      break;
	    case REPEAT:
	      rule->PrPresBox[0] = FillFrame;
	      break;
	    case HREPEAT:
	      rule->PrPresBox[0] = XRepeat;
	      break;
	    case VREPEAT:
	      rule->PrPresBox[0] = YRepeat;
	      break;
	    default:
	      rule->PrPresBox[0] = RealSize;
	    }
	  break;
	case FnPage:
	  rule->PrPresFunction = (FunctionType) funcType;
	  rule->PrPresBoxRepeat = FALSE;
	  rule->PrExternal = FALSE;
	  rule->PrElement = FALSE;
	  rule->PrNPresBoxes = 0;
	  rule->PrPresBox[0] = 0;
	  rule->PrPresBoxName[0] = EOS;
	  break;
	}
      break;
    }
}

/*----------------------------------------------------------------------
  PRuleToPresentationValue : return the PresentationValue corresponding to
  a given rule.
  ----------------------------------------------------------------------*/
static PresentationValue PRuleToPresentationValue (PtrPRule rule)
{
  PresentationValue   val;
  TypeUnit            int_unit = -1;
  int                 value = 0;
  int                 unit = -1;
  int                 type;
  ThotBool            real = FALSE;

  /* read the value */
  switch (rule->PrType)
    {
    case PtVisibility:
    case PtDepth:
    case PtFillPattern:
    case PtBackground:
    case PtForeground:
    case PtBorderTopColor:
    case PtBorderRightColor:
    case PtBorderBottomColor:
    case PtBorderLeftColor:
      value = rule->PrIntValue;
      break;
    case PtOpacity:
    case PtFillOpacity:
    case PtStrokeOpacity:
      real = TRUE;
      value = rule->PrIntValue;
      break;
    case PtFont:
      switch (rule->PrChrValue)
	{
	case 'H':
	  value = FontHelvetica;
	  break;
	case 'T':
	  value = FontTimes;
	  break;
	case 'C':
	  value = FontCourier;
	  break;
#ifdef _WINDOWS
#ifdef _I18N_
	case -1:
	  value = FontOther;
	  break;
#endif /* _I18N_ */
#endif /* _WINDOWS */
	}
      break;

    case PtStyle:
      switch (rule->PrChrValue)
	{
	case 'R':
	  value = StyleRoman;
	  break;
	case 'I':
	  value = StyleItalics;
	  break;
	case 'O':
	  value = StyleOblique;
	  break;
	default:
	  value = StyleRoman;
	  break;
	}
      break;

    case PtWeight:
      switch (rule->PrChrValue)
	{
	case 'B':
	  value = WeightBold;
	  break;
	case 'N':
	  value = WeightNormal;
	  break;
	default:
	  value = WeightNormal;
	  break;
	}
      break;

    case PtUnderline:
      switch (rule->PrChrValue)
	{
	case 'N':
	  value = NoUnderline;
	  break;
	case 'U':
	  value = Underline;
	  break;
	case 'O':
	  value = Overline;
	  break;
	case 'C':
	  value = CrossOut;
	  break;
	}
      break;

    case PtThickness:
      switch (rule->PrChrValue)
	{
	case 'N':
	  value = ThinUnderline;
	  break;
	case 'T':
	  value = ThickUnderline;
	  break;
	}
      break;

    case PtDirection:
      switch (rule->PrChrValue)
	{
	case 'R':
	  value = RightToLeft;
	  break;
	case 'L':
	  value = LeftToRight;
	  break;
	}
      break;

    case PtUnicodeBidi:
      switch (rule->PrChrValue)
	{
	case 'N':
	  value = Normal;
	  break;
	case 'E':
	  value = Embed;
	  break;
	case 'O':
	  value = Override;
	  break;
	}
      break;

    case PtLineStyle:
      switch (rule->PrChrValue)
	{
	case 'S':
	  value = SolidLine;
	  break;
	case '-':
	  value = DashedLine;
	  break;
	case '.':
	  value = DottedLine;
	  break;
	}
      break;

    case PtDisplay:
      switch (rule->PrChrValue)
	{
	case 'U':
	  value = Undefined;
	  break;
	case 'I':
	  value = Inline;
	  break;
	case 'B':
	  value = Block;
	  break;
	case 'L':
	  value = ListItem;
	  break;
	case 'R':
	  value = RunIn;
	  break;
	case 'C':
	  value = Compact;
	  break;
	case 'M':
	  value = Marker;
	  break;
	}
      break;

    case PtFloat:
       switch (rule->PrChrValue)
	{
	case 'L':
	  value = FloatLeft;
	  break;
	case 'R':
	  value = FloatRight;
	  break;
	default:
	  value = FloatNone;
	  break;
	}
      break;
    case PtClear:
      switch (rule->PrChrValue)
	{
	case 'L':
	  value = ClearLeft;
	  break;
	case 'R':
	  value = ClearRight;
	  break;
	case 'B':
	  value = ClearBoth;
	  break;
	default:
	  value = ClearNone;
	  break;
	}
      break;
 
    case PtBorderTopStyle:
    case PtBorderRightStyle:
    case PtBorderBottomStyle:
    case PtBorderLeftStyle:
      switch (rule->PrChrValue)
	{
	case '0':
	  value = BorderStyleNone;
	  break;
	case 'H':
	  value = BorderStyleHidden;
	  break;
	case '.':
	  value = BorderStyleDotted;
	  break;
	case '-':
	  value = BorderStyleDashed;
	  break;
	case 'S':
	  value = BorderStyleSolid;
	  break;
	case 'D':
	  value = BorderStyleDouble;
	  break;
	case 'G':
	  value = BorderStyleGroove;
	  break;
	case 'R':
	  value = BorderStyleRidge;
	  break;
	case 'I':
	  value = BorderStyleInset;
	  break;
	case 'O':
	  value = BorderStyleOutset;
	  break;
	}
      break;

    case PtBreak1:
    case PtBreak2:
    case PtIndent:
    case PtLineSpacing:
    case PtLineWeight:
    case PtMarginTop:
    case PtMarginRight:
    case PtMarginBottom:
    case PtMarginLeft:
    case PtPaddingTop:
    case PtPaddingRight:
    case PtPaddingBottom:
    case PtPaddingLeft:
    case PtBorderTopWidth:
    case PtBorderRightWidth:
    case PtBorderBottomWidth:
    case PtBorderLeftWidth:
    case PtXRadius:
    case PtYRadius:
      int_unit = rule->PrMinUnit;
      value = rule->PrMinValue;
      break;

    case PtSize:
      if (rule->PrPresMode == PresInherit)
	{
        int_unit = UnPercent;
        value = rule->PrInhDelta;
	}
      else
	{
        int_unit = rule->PrMinUnit;
        value = rule->PrMinValue;
	}
      break;

    case PtVertRef:
    case PtHorizRef:
    case PtVertPos:
    case PtHorizPos:
      int_unit = rule->PrPosRule.PoDistUnit;
      value = rule->PrPosRule.PoDistance;
      break;

    case PtHeight:
    case PtWidth:
      int_unit = rule->PrDimRule.DrUnit;
      if (int_unit == 0)
	int_unit = UnPixel;

      value = rule->PrDimRule.DrValue;
      break;

    case PtHyphenate:
      if (rule->PrBoolValue)
	value = Hyphenation;
      else
	value = NoHyphenation;
      break;

    case PtPictInfo:
    case PtVertOverflow:
    case PtHorizOverflow:
    case PtGather:
    case PtPageBreak:
    case PtLineBreak:
      break;

    case PtAdjust:
      switch (rule->PrAdjust)
	{
	case AlignLeft:
	  value = AdjustLeft;
	  break;
	case AlignRight:
	  value = AdjustRight;
	  break;
	case AlignCenter:
	  value = Centered;
	  break;
	case AlignLeftDots:
	  value = LeftWithDots;
	  break;
	case AlignJustify:
	  value = Justify;
	  break;
	default:
	  value = AdjustLeft;
	  break;
	}
      break;

    case PtFunction:
      switch (rule->PrPresFunction)
	{
	case FnCreateBefore:
	case FnCreateWith:
	case FnCreateFirst:
	case FnCreateLast:
	case FnCreateAfter:
	case FnCreateEnclosing:
	  value = rule->PrNPresBoxes;
	  unit = UNIT_BOX;
	  break;
	case FnShowBox:
	  value = rule->PrNPresBoxes;
	  unit = UNIT_REL;
	  break;
	case FnBackgroundPicture:
	  value = rule->PrPresBox[0];
	  unit = UNIT_REL;
	  break;
	case FnPictureMode:
	  unit = UNIT_REL;
	  value = REALSIZE;
	  switch (rule->PrPresBox[0])
	    {
	    case RealSize:
	      value = REALSIZE;
	      break;
	    case ReScale:
	      value = SCALE;
	      break;
	    case FillFrame:
	      value = REPEAT;
	      break;
	    case XRepeat:
	      value = HREPEAT;
	      break;
	    case YRepeat:
	      value = VREPEAT;
	      break;
	    default:
	      unit = UNIT_INVALID;
	      value = 0;
	    }
	case FnPage:
	  value = PageAlways;
	  unit = UNIT_REL;
	  break;
	default:
	  break;
	}
      break;
    }

  /* translate to external units */
  switch (int_unit)
    {
    case UnRelative:
      type = rule->PrType;
      if (type == PtBreak1 ||
	  type == PtBreak2 ||
	  type == PtIndent ||
	  type == PtLineSpacing ||
	  type == PtLineWeight)
	unit = UNIT_REL;
      else
	unit = UNIT_EM;
      value /= 10;
      break;
    case UnXHeight:
      value /= 10;
      unit = UNIT_XHEIGHT;
      break;
    case UnPoint:
      unit = UNIT_PT;
      break;
    case UnPixel:
      unit = UNIT_PX;
      break;
    case UnPercent:
      unit = UNIT_PERCENT;
      break;
    default:
      unit = UNIT_INVALID;
      break;
    }

  if (value % 10 && unit != UNIT_INVALID)
    {
      real = TRUE;
      value *= 1000;
    }

  val.typed_data.value = value;
  val.typed_data.unit = unit;
  val.typed_data.real = real;
  return (val);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void TypeToPresentation (unsigned int type, PRuleType *intRule,
				unsigned int *func, ThotBool *absolute)
{
  *func = 0;
  *absolute = FALSE;
  switch (type)
    {
    case PRVisibility:
      *intRule = PtVisibility;
      break;
    case PRShowBox:
      *intRule = PtFunction;
      *func = FnShowBox;
      break;
    case PRPictureMode:
      *intRule = PtFunction;
      *func = FnPictureMode;
      break;
    case PRBackgroundPicture:
      *intRule = PtFunction;
      *func = FnBackgroundPicture;
      break;
    case PRCreateEnclosing:
      *intRule = PtFunction;
      *func = FnCreateEnclosing;
      break;
    case PRPageBefore:
      *intRule = PtFunction;
      *func = FnPage;
      break;
    case PRVertOverflow:
      *intRule = PtVertOverflow;
      break;
    case PRHorizOverflow:
      *intRule = PtHorizOverflow;
      break;
    case PtVertRef:
      *intRule = PtVertRef;
      break;
    case PtHorizRef:
      *intRule = PtHorizRef;
      break;
    case PRHeight:
      *intRule = PtHeight;
      *absolute = TRUE;
      break;
    case PRWidth:
      *intRule = PtWidth;
      *absolute = TRUE;
      break;
    case PRVertPos:
      *intRule = PtVertPos;
      break;
    case PRHorizPos:
      *intRule = PtHorizPos;
      break;
    case PRMarginTop:
      *intRule = PtMarginTop;
      break;
    case PRMarginRight:
      *intRule = PtMarginRight;
      break;
    case PRMarginBottom:
      *intRule = PtMarginBottom;
      break;
    case PRMarginLeft:
      *intRule = PtMarginLeft;
      break;
    case PRPaddingTop:
      *intRule = PtPaddingTop;
      break;
    case PRPaddingRight:
      *intRule = PtPaddingRight;
      break;
    case PRPaddingBottom:
      *intRule = PtPaddingBottom;
      break;
    case PRPaddingLeft:
      *intRule = PtPaddingLeft;
      break;
    case PRBorderTopWidth:
      *intRule = PtBorderTopWidth;
      break;
    case PRBorderRightWidth:
      *intRule = PtBorderRightWidth;
      break;
    case PRBorderBottomWidth:
      *intRule = PtBorderBottomWidth;
      break;
    case PRBorderLeftWidth:
      *intRule = PtBorderLeftWidth;
      break;      
    case PRBorderTopColor:
      *intRule = PtBorderTopColor;
      break;
    case PRBorderRightColor:
      *intRule = PtBorderRightColor;
      break;
    case PRBorderBottomColor:
      *intRule = PtBorderBottomColor;
      break;
    case PRBorderLeftColor:
      *intRule = PtBorderLeftColor;
      break;
    case PRBorderTopStyle:
      *intRule = PtBorderTopStyle;
      break;
    case PRBorderRightStyle:
      *intRule = PtBorderRightStyle;
      break;
    case PRBorderBottomStyle:
      *intRule = PtBorderBottomStyle;
      break;
    case PRBorderLeftStyle:
      *intRule = PtBorderLeftStyle;
      break;
    case PRSize:
      *intRule = PtSize;
      break;
    case PRStyle:
      *intRule = PtStyle;
      break;
    case PRWeight:
      *intRule = PtWeight;
      break;
    case PRFont:
      *intRule = PtFont;
      break;
    case PRUnderline:
      *intRule = PtUnderline;
      break;
    case PRIndent:
      *intRule = PtIndent;
      break;
    case PRLineSpacing:
      *intRule = PtLineSpacing;
      break;
    case PRDepth:
      *intRule = PtDepth;
      break;
    case PRAdjust:
      *intRule = PtAdjust;
      break;
    case PRDirection:
      *intRule = PtDirection;
      break;
    case PRUnicodeBidi:
      *intRule = PtUnicodeBidi;
      break;
    case PRLineWeight:
      *intRule = PtLineWeight;
      break;
    case PRFillPattern:
      *intRule = PtFillPattern;
      break;
    case PROpacity:
      *intRule = PtOpacity;
      break;
    case PRFillOpacity:
      *intRule = PtFillOpacity;
      break;
    case PRStrokeOpacity:
      *intRule = PtStrokeOpacity;
      break;
    case PRBackground:
      *intRule = PtBackground;
      break;
    case PRForeground:
      *intRule = PtForeground;
      break;
    case PRHyphenate:
      *intRule = PtHyphenate;
      break;
    case PRPageInside:
      *intRule = PtPageBreak;
      break;
    case PRXRadius:
      *intRule = PtXRadius;
      *absolute = TRUE;
      break;
    case PRYRadius:
      *intRule = PtYRadius;
      *absolute = TRUE;
      break;
    case PRDisplay:
      *intRule = PtDisplay;
      break;
    case PRFloat:
      *intRule = PtFloat;
      break;
    case PRClear:
      *intRule = PtClear;
      break;
    default:
      *intRule = PtFunction;
    }
}

/*----------------------------------------------------------------------
  TtaSetStylePresentation attaches a presentation rule to an element or to an
  extended presentation schema.
  type: type of the presentation rule
  el: element to which the presentation rule must be attached. If NULL,
      the rule is not attached to an element but included in an extended
      P schema (generic rule).
  tsch: extended P schema to which the rule will be attached. If NULL,
        the rule is attached to element el (which must not be NULL).
  c: context of the rule. For a position rule (type = PRVertPos or PRHorizPos)
     ctxt->type indicates the edge whose position is set; ctxt->type = 0
     indicates that only the distance is changed, not the edge.
  v: value for the rule. For a position rule (type = PRVertPos or PRHorizPos)
     v indicates the distance; it is taken into account only if ctxt->type = 0.
  ----------------------------------------------------------------------*/
int TtaSetStylePresentation (unsigned int type, Element el, PSchema tsch,
			     PresentationContext c, PresentationValue v)
{
  GenericContext    ctxt = (GenericContext) c;
  PtrPRule          pRule;
  PRuleType         intRule;
  ElementType       elType;
  unsigned int      func = 0;
  int               cst = 0;
  int               i;
  int               attrType;
  int               doc = c->doc;
  ThotBool          absolute, generic, minValue;

  TypeToPresentation (type, &intRule, &func, &absolute);
  generic = (el == NULL);
  if (type == PRHeight)
    {
      /* check if the height is a minimum value or not */
      if (!generic)
	{
	  elType = TtaGetElementType (el);
	  i = elType.ElTypeNum;
	}
      else
	i = ctxt->type;
      /* apply a min value for all compound element */
      minValue = (i == 0 || i >= PageBreak);
    }
  else
    minValue = FALSE;

  if (c->destroy)
    {
      if (generic)
	PresRuleRemove ((PtrPSchema) tsch, ctxt, intRule, func);
      else
	RemoveElementPRule ((PtrElement) el, intRule, func);
    }
  else
    {
      attrType = 0;
      if (generic)
	pRule = PresRuleInsert ((PtrPSchema) tsch, ctxt, intRule, func);
      else
	pRule = InsertElementPRule ((PtrElement) el, LoadedDocument[doc - 1],
				    intRule, func, c->cssSpecificity);
      if (pRule)
	{
	  if (type == PRBackgroundPicture)
	    {
	      if (!generic)
		tsch = (PSchema) PresentationSchema (LoadedDocument[doc - 1]->DocSSchema, LoadedDocument[doc - 1]);
	      cst = PresConstInsert (tsch, v.pointer);
	      v.typed_data.unit = UNIT_REL;
	      v.typed_data.value = cst;
	      v.typed_data.real = FALSE;
	    }
	  /* avoid to override an important rule by a non-important rule */
	  if (ctxt->important || !pRule->PrImportant)
	    {
	      if (ctxt->type > 0 &&
	          (type == PRVertPos || type == PRHorizPos))
		func = ctxt->type;
	      PresentationValueToPRule (v, intRule, pRule, func, absolute,
					generic, minValue);
	    }
	  if (generic)
	    {
	      pRule->PrViewNum = 1;
	      if (ctxt->box && intRule == PtFunction)
		BuildBoxName (ctxt, &pRule->PrPresBoxName);
	      /*  select the good starting point depending on the context */
	      if (ctxt->box)
		pRule = BoxRuleSearch ((PtrPSchema) tsch, ctxt);
	      if (pRule)
		{
		  i = 0;
		  while (attrType == 0 && i < MAX_ANCESTORS)
		    attrType = ctxt->attrType[i++];
		  ApplyAGenericStyleRule (doc, (PtrSSchema) ctxt->schema,
					  ctxt->type, attrType, ctxt->box, pRule, FALSE);
		}
	    }
	  else
	    ApplyASpecificStyleRule (pRule, (PtrElement) el,
				     LoadedDocument[doc - 1], FALSE);
	}
    }
  return (0);
}

/*----------------------------------------------------------------------
  TtaGetStylePresentation returns the style rule attached to an element
  or to an extended presentation schema.
  ----------------------------------------------------------------------*/
int TtaGetStylePresentation (unsigned int type, Element el, PSchema tsch,
			     PresentationContext c, PresentationValue *v)
{
  PtrPRule          rule, *chain;
  PRuleType         intRule;
  unsigned int      func;
  int               cst;
  ThotBool          absolute, generic;

  TypeToPresentation (type, &intRule, &func, &absolute);
  generic = (el == NULL);
  if (generic)
    rule = PresRuleSearch ((PtrPSchema) tsch, (GenericContext) c, intRule,
			   func, &chain);
  else
    rule = SearchElementPRule ((PtrElement) el, intRule, func);
  if (rule == NULL)
    return (-1);

  *v = PRuleToPresentationValue (rule);
  if (type == PRBackgroundPicture)
    {
      cst = v->typed_data.unit;
      v->pointer = &((PtrPSchema) tsch)->PsConstant[cst-1].PdString[0];
    }
  return (0);
}

/*----------------------------------------------------------------------
  PRuleToPresentationSetting : Translate the internal values stored
  in a PRule to a valid PresentationSetting.
  ----------------------------------------------------------------------*/
static void PRuleToPresentationSetting (PtrPRule rule, PresentationSetting setting, int extra)
{
  /* first decoding step : analyze the type of the rule */
  switch (rule->PrType)
    {
    case PtVisibility:
      setting->type = PRVisibility;
      break;
    case PtDepth:
      setting->type = PRDepth;
      break;
    case PtFillPattern:
      setting->type = PRFillPattern;
      break;
    case PtOpacity:
      setting->type = PROpacity;
      break;
    case PtStrokeOpacity:
      setting->type = PRStrokeOpacity;
      break;
    case PtFillOpacity:
      setting->type = PRFillOpacity;
      break;
    case PtBackground:
      setting->type = PRBackground;
      break;
    case PtForeground:
      setting->type = PRForeground;
      break;
    case PtBorderTopColor:
      setting->type = PRBorderTopColor;
      break;
    case PtBorderRightColor:
      setting->type = PRBorderRightColor;
      break;
    case PtBorderBottomColor:
      setting->type = PRBorderBottomColor;
      break;
    case PtBorderLeftColor:
      setting->type = PRBorderLeftColor;
      break;
    case PtBorderTopStyle:
      setting->type = PRBorderTopStyle;
      break;
    case PtBorderRightStyle:
      setting->type = PRBorderRightStyle;
      break;
    case PtBorderBottomStyle:
      setting->type = PRBorderBottomStyle;
      break;
    case PtBorderLeftStyle:
      setting->type = PRBorderLeftStyle;
      break;
    case PtFont:
      setting->type = PRFont;
      break;
    case PtStyle:
      setting->type = PRStyle;
      break;
    case PtWeight:
      setting->type = PRWeight;
      break;
    case PtUnderline:
      setting->type = PRUnderline;
      break;
    case PtIndent:
      setting->type = PRIndent;
      break;
    case PtDisplay:
      setting->type = PRDisplay;
      break;
    case PtFloat:
      setting->type = PRFloat;
      break;
    case PtClear:
      setting->type = PRClear;
      break;
    case PtSize:
      setting->type = PRSize;
      break;
    case PtLineSpacing:
      setting->type = PRLineSpacing;
      break;
    case PtMarginTop:
      setting->type = PRMarginTop;
      break;
    case PtMarginRight:
      setting->type = PRMarginRight;
      break;
    case PtMarginBottom:
      setting->type = PRMarginBottom;
      break;
    case PtMarginLeft:
      setting->type = PRMarginLeft;
      break;
    case PtPaddingTop:
      setting->type = PRPaddingTop;
      break;
    case PtPaddingRight:
      setting->type = PRPaddingRight;
      break;
    case PtPaddingBottom:
      setting->type = PRPaddingBottom;
      break;
    case PtPaddingLeft:
      setting->type = PRPaddingLeft;
      break;
    case PtBorderTopWidth:
      setting->type = PRBorderTopWidth;
      break;
    case PtBorderRightWidth:
      setting->type = PRBorderRightWidth;
      break;
    case PtBorderBottomWidth:
      setting->type = PRBorderBottomWidth;
      break;
    case PtBorderLeftWidth:
      setting->type = PRBorderLeftWidth;
      break;
    case PtXRadius:
      setting->type = PRXRadius;
      break;
    case PtYRadius:
      setting->type = PRYRadius;
      break;
    case PtHeight:
      setting->type = PRHeight;
      break;
    case PtWidth:
      setting->type = PRWidth;
      break;
    case PtHyphenate:
      setting->type = PRHyphenate;
      break;
    case PtAdjust:
      setting->type = PRAdjust;
      break;
    case PtFunction:
      switch (extra)
	{
	case FnShowBox:
	  setting->type = PRShowBox;
	  break;
	case FnBackgroundPicture:
	  setting->type = PRBackgroundPicture;
	  break;
	case FnPictureMode:
	  setting->type = PRPictureMode;
	  break;
	case FnPage:
	  setting->type = PRPageBefore;
	  break;
	default:
	  /* not yet supported by the driver */
	  setting->type = PRNone;
	  return;
	}
      break;
    default:
      /* not yet supported by the driver */
      setting->type = PRNone;
      return;
    }
  
  /* second decoding step : read the value contained */
  setting->value = PRuleToPresentationValue (rule);
}

/*----------------------------------------------------------------------
  GetGenericStyleContext : user level function needed to allocate and
  initialize a GenericContext.
  ----------------------------------------------------------------------*/
GenericContext TtaGetGenericStyleContext (Document doc)
{
  GenericContext      ctxt;
  int                 i;

  ctxt = (GenericContext) TtaGetMemory (sizeof (GenericContextBlock));
  if (ctxt == NULL)
    return (NULL);
  ctxt->doc = doc;
  ctxt->type = 0;
  ctxt->schema = TtaGetDocumentSSchema (doc);
  ctxt->destroy = 0;
  ctxt->box = 0;
  for (i = 0; i < MAX_ANCESTORS; i++)
    {
      ctxt->name[i] = 0;
      ctxt->names_nb[i] = 0;
      ctxt->attrType[i] = 0;
      ctxt->attrText[i] = NULL;
    }
   return (ctxt);
}


/*----------------------------------------------------------------------
  TtaGetSpecificStyleContext : user level function needed to allocate and
  initialize a SpecificContext.
  ----------------------------------------------------------------------*/
PresentationContext TtaGetSpecificStyleContext (Document doc)
{
   PresentationContext     ctxt;

   ctxt = (PresentationContext) TtaGetMemory (sizeof (PresentationContextBlock));
   if (ctxt == NULL)
      return (NULL);
   ctxt->doc = doc;
   ctxt->schema = TtaGetDocumentSSchema (doc);
   ctxt->type = 0;
   ctxt->cssSpecificity = 0;
   ctxt->important = FALSE;
   ctxt->destroy = 0;
   return (ctxt);
}

/*----------------------------------------------------------------------
  TtaCleanElementPresentation
  Remove all specific presentation rules attached to element el
  in document doc
  ----------------------------------------------------------------------*/
void TtaCleanElementPresentation (Element el, Document doc)
{
  PRule               rule, nextRule;
  DisplayMode         dispMode;

  if (el == NULL || doc == 0 || LoadedDocument[doc - 1] == NULL)
    return;

  /* avoid too many redisplay */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);
  rule = (PRule) ((PtrElement) el)->ElFirstPRule;
  while (rule)
    {
      nextRule = rule;
      TtaNextPRule (el, &nextRule);
      if (TtaIsCSSPRule (rule))
	TtaRemovePRule (el, rule, doc);
      rule = nextRule;
    }
  /* restore the display mode */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
}

/*----------------------------------------------------------------------
  TtaCleanStylePresentation
  Remove all presentation for the extended presentation schema tsch associated
  with structure schema sSch in document doc
  ----------------------------------------------------------------------*/
void TtaCleanStylePresentation (PSchema tsch, Document doc, SSchema sSch)
{
  PtrPRule            pRule;
  PtrSSchema	      pSS;
  AttributePres      *attrs;
  DisplayMode         dispMode;
  int                 nbrules, i, j;
  unsigned int        elType, attrType, max;

  if (doc == 0 || LoadedDocument[doc - 1] == NULL)
    return;

  /* avoid too many redisplay */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);
  pRule = NULL;
  pSS = (PtrSSchema) sSch;

  /* remove all element rules */
  max = (unsigned int) pSS->SsNRules;
  for (elType = 0; elType < max; elType++)
    {
      pRule = ((PtrPSchema) tsch)->PsElemPRule->ElemPres[elType];
      while (pRule != NULL)
	{
	  ApplyAGenericStyleRule (doc, pSS, elType+1, 0, 0, pRule, TRUE);
	  pRule = pRule->PrNextPRule;
	}
    }

  /* remove all attribute rules */
  max = (unsigned int) pSS->SsNAttributes;
  for (attrType = 0; attrType < max; attrType++)
    {
      attrs = ((PtrPSchema) tsch)->PsAttrPRule->AttrPres[attrType];
      nbrules = ((PtrPSchema) tsch)->PsNAttrPRule->Num[attrType];
      for (i = 0; i < nbrules; i++)
	{
	  switch (pSS->SsAttribute->TtAttr[attrType]->AttrType)
	    {
	    case AtNumAttr:
	      for (j = 0; j < attrs->ApNCases; j++)
		{
		  pRule = attrs->ApCase[j].CaFirstPRule;
		  while (pRule != NULL)
		    {
		      ApplyAGenericStyleRule (doc, pSS, 0, attrType+1, 0,
					      pRule, TRUE);
		      pRule = pRule->PrNextPRule;
		    }
		}
	      break;
	    case AtTextAttr:
	      pRule = attrs->ApTextFirstPRule;
	      break;
	    case AtReferenceAttr:
	      pRule = attrs->ApRefFirstPRule;
	      break;
	    case AtEnumAttr:
	      for (j = 0; j < pSS->SsAttribute->TtAttr[attrType]->AttrNEnumValues; j++)
		{
		  pRule = attrs->ApEnumFirstPRule[j];
		  while (pRule != NULL)
		    {
		      ApplyAGenericStyleRule (doc, pSS, 0, attrType, 0,
					      pRule, TRUE);
		      pRule = pRule->PrNextPRule;
		    }
		}
	      break;
	    }

	  while (pRule != NULL)
	    {
	      ApplyAGenericStyleRule (doc, pSS, 0, attrType, 0, pRule, TRUE);
	      pRule = pRule->PrNextPRule;
	    }
	  attrs = attrs->ApNextAttrPres;
	}
    }
  /* restore the display mode */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
}

/*----------------------------------------------------------------------
  ApplyAllSpecificSettings browses all the PRules structures,
  associated to the corresponding Specific Context 
  structure, and calls the given handler for each one.
  ----------------------------------------------------------------------*/
void TtaApplyAllSpecificSettings (Element el, Document doc,
				  SettingsApplyHandler handler,
				  void *param)
{
  PtrPRule                 rule;
  PresentationSettingBlock setting;
  PtrPSchema               pPS;
  int                      cst;

  if (el == NULL)
    return;
  pPS = PresentationSchema (LoadedDocument[doc - 1]->DocSSchema,
			    LoadedDocument[doc - 1]);
  rule = ((PtrElement) el)->ElFirstPRule;
  /*
   * for each rule corresponding to the same context i.e. identical
   * conditions, create the corresponding PresentationSetting and
   * call the user handler.
   */
  while (rule != NULL)
    {
      if (rule->PrSpecificity >= 100)
	{
	  /* the rule is the translation of a style attribute */
	  /* fill in the PresentationSetting and call the handler */
	  if (rule->PrType == PtFunction)
	    PRuleToPresentationSetting (rule, &setting, rule->PrPresFunction);
	  else
	    PRuleToPresentationSetting (rule, &setting, 0);
	  
	  /* need to do some tweaking in the case of BackgroudPicture */
	  if (setting.type == PRBackgroundPicture)
	    {
	      cst = setting.value.typed_data.value;
	      setting.value.pointer = &pPS->PsConstant[cst-1].PdString[0];
	    }
	  handler (el, doc, &setting, param);
	}
      rule = rule->PrNextPRule;
    }
}
