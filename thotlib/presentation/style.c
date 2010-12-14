/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Set of functions to style Thot documents: it's the style API.
 *
 * Author: I. Vatton (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
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

struct unit_def CSSUnitNames[] =
  {
    {"pt", UNIT_PT},
    {"pc", UNIT_PC},
    {"in", UNIT_IN},
    {"cm", UNIT_CM},
    {"mm", UNIT_MM},
    {"em", UNIT_EM},
    {"px", UNIT_PX},
    {"ex", UNIT_XHEIGHT},
    {"%", UNIT_PERCENT},
    {NULL, 0}
  };


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
  Get an Amaya color
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
  *red = 32767;
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
      ptr = (char*)TtaSkipBlanks (ptr);
      if (*ptr == '(')
        {
          ptr++;
          ptr = (char*)TtaSkipBlanks (ptr);
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
#ifdef IV
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
#endif /* _IV */
    }
  if (failed)
    return (value);
  else
    return (ptr);
}

/*----------------------------------------------------------------------
  PresBoxInsert
  Add a new presentation box to schema tsch.
  ----------------------------------------------------------------------*/
static void PresBoxInsert (PtrPSchema tsch, GenericContext ctxt)
{
  PtrPresentationBox  box;
  int                 i, size;
  PtrPSchema          pSP;
  PtrPRule            pPRule, pPRuleCopy, pPrevCopy;

  if (tsch->PsPresentBox)
    /* there is already a presentation box for pseudo-elements */
    {
      ctxt->box = 1;
      return;
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
        ctxt->box = 0;
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
    /* can't allocate a new box */
    return;
  tsch->PsPresentBox->PresBox[tsch->PsNPresentBoxes] = box;
  memset (box, 0, sizeof (PresentationBox));
  tsch->PsNPresentBoxes++;
  ctxt->box = tsch->PsNPresentBoxes;
  strcpy (box->PbName, "Pseudo-element");
  box->PbFirstPRule = NULL;
  box->PbPageFooter = FALSE;
  box->PbPageHeader = FALSE;
  box->PbPageBox = FALSE;
  box->PbFooterHeight = 0;
  box->PbHeaderHeight = 0;
  box->PbPageCounter = 0;
  box->PbContent = FreeContent;
  box->PbContVariable = 0;

  if (!tsch->PsFirstDefaultPRule)
    /* copy the default presentation rules from the main PSchema, to
       make sure that the presentation box has at least one rule for each
       property. */
    {
      pSP = PresentationSchema (tsch->PsSSchema, LoadedDocument[ctxt->doc-1]);
      if (pSP)
        {
          pPRule = pSP->PsFirstDefaultPRule;
          pPrevCopy = NULL;
          while (pPRule)
            {
              GetPresentRule (&pPRuleCopy);
              *pPRuleCopy = *pPRule;

              /* inheritance from the parent element in the default PSchema
                 is changed into inheritance from the creating element */
              if (pPRuleCopy->PrPresMode == PresInherit)
                if (pPRuleCopy->PrInheritMode == InheritParent)
                  pPRuleCopy->PrInheritMode = InheritCreator;

              pPRuleCopy->PrNextPRule = NULL;
              if (!pPrevCopy)
                tsch->PsFirstDefaultPRule = pPRuleCopy;
              else
                pPrevCopy->PrNextPRule = pPRuleCopy;
              pPrevCopy = pPRuleCopy;
              pPRule = pPRule->PrNextPRule;
            }
        }
    }
}

/*----------------------------------------------------------------------
  SearchElementPRule is used to to search all specific presentation rules
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
  return cur;
}

/*----------------------------------------------------------------------
  Function used to to add a specific presentation rule
  for a given type of rule associated to an element.
  Parameter specificity is the specificity od the corresponding CSS rule.
  ----------------------------------------------------------------------*/
static PtrPRule InsertElementPRule (PtrElement el, PtrDocument pDoc,
                                    PRuleType type, unsigned int extra,
                                    int specificity, int lineNum)
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
          stdRule = GlobalSearchRulepEl (el, pDoc, &pSPR, &pSSR, FALSE, 0,
                                         NULL, 1, type, (FunctionType)extra, FALSE, TRUE, &pAttr);
          if (stdRule != NULL)
            /* copy the standard rule */
            *pRule = *stdRule;
          else
            pRule->PrType = type;
          pRule->PrCond = NULL;
          pRule->PrSpecifAttr = 0;
          pRule->PrSpecificity = specificity;
          pRule->PrCSSLine = lineNum;
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
  if (doc)
    ApplyASpecificStyleRule (cur, el, LoadedDocument[doc -1], TRUE);

  /* Free the PRule */
  FreePresentRule(cur, el->ElStructSchema);
  return;
}

/*----------------------------------------------------------------------
  PresConstInsert : add a constant to the constant array of a
  Presentation Schema and returns the associated index.
  ----------------------------------------------------------------------*/
static int PresConstInsert (PSchema tcsh, char *value, BasicType constType)
{
  PtrPSchema pSchemaPrs = (PtrPSchema) tcsh;
  int i;

  if (pSchemaPrs == NULL || value == NULL)
    return (-1);

  /* lookup the existing constants, searching for a corresponding entry */
  for (i = 0; i < pSchemaPrs->PsNConstants; i++)
    {
      if (pSchemaPrs->PsConstant[i].PdType == constType &&
          pSchemaPrs->PsConstant[i].PdString &&
          !strcmp (value, pSchemaPrs->PsConstant[i].PdString))
        {
	  return (i+1);
        }
    }

  /* if not found, try to add it at the end */
  if (pSchemaPrs->PsNConstants >= MAX_PRES_CONST)
    return (-1);
  i = pSchemaPrs->PsNConstants;
  pSchemaPrs->PsConstant[i].PdType = constType;
  pSchemaPrs->PsConstant[i].PdScript = 'L';
  pSchemaPrs->PsConstant[i].PdString = TtaStrdup (value);
  pSchemaPrs->PsNConstants++;
  return(i+1);
}

/*----------------------------------------------------------------------
  AddCond : add a new condition in a presentation rule, respecting
  the order of the list.
  ----------------------------------------------------------------------*/
static void AddCond (PtrCondition *base, PtrCondition cond, SSchema sch)
{
  PtrCondition        cour = *base;
  PtrCondition        next;

  if (cour == NULL)
    {
      *base = cond;
      cond->CoNextCondition = NULL;
      return;
    }
  next = cour->CoNextCondition;
  while (next)
    {
      cour = next;
      next = cour->CoNextCondition;
    }
  cour->CoNextCondition = cond;
  cond->CoNextCondition = NULL;
}

/*----------------------------------------------------------------------
  AddElemCond : add an element condition to a presentation rule.
  ----------------------------------------------------------------------*/
static void AddElemCond (PtrCondition *base, GenericContext ctxt, int level,
                                 ThotBool firstChild)
{
  PtrCondition        cond = NULL;

  GetPresentRuleCond (&cond);
  if (cond == NULL)
    {
      TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
      return;
    }
  memset (cond, 0, sizeof (Condition));
  if (firstChild)
    {
      cond->CoCondition = PcFirst;
      cond->CoNotNegative = TRUE;
    }
  else if (ctxt->rel[level] == RelVoid)
    {
      /* the current element type must be ... */
      cond->CoCondition = PcElemType;
      cond->CoChangeElem = FALSE;
      cond->CoNotNegative = TRUE;
      cond->CoTarget = FALSE;
      cond->CoTypeElem = ctxt->name[level];
    }
  else
    {
      cond->CoImmediate = FALSE;
      if (ctxt->rel[level] == RelParent)
        {
          cond->CoChangeElem = TRUE;
          cond->CoCondition = PcWithin;
          cond->CoImmediate = TRUE;
        }
      else if (ctxt->rel[level] == RelAncestor)
        {
          cond->CoChangeElem = TRUE;
          cond->CoCondition = PcWithin;
          cond->CoImmediate = FALSE;
        }
      else if (ctxt->rel[level] == RelPrevious)
        {
          cond->CoChangeElem = TRUE;
          cond->CoCondition = PcSibling;
          cond->CoImmediate = TRUE;
        }
      else
        cond->CoChangeElem = FALSE;
      cond->CoTarget = FALSE;
      cond->CoNotNegative = TRUE;
      cond->CoRelation = 0;
      cond->CoTypeAncestor = ctxt->name[level];
      cond->CoAncestorRel = CondGreater;
      cond->CoAncestorName = NULL;
      cond->CoSSchemaName[0] = EOS;
    }
  AddCond (base, cond, ctxt->schema);
}

/*----------------------------------------------------------------------
  AddAttrCond : add a Attr condition to a presentation rule.
  ----------------------------------------------------------------------*/
static void AddAttrCond (PtrCondition *base, GenericContext ctxt, int att)
{
  AttributeType       attType;
  PtrCondition        cond = NULL;
  int                 kind;

  GetPresentRuleCond (&cond);
  memset (cond, 0, sizeof (Condition));
  if (cond == NULL)
    {
      TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
      return;
    }
  cond->CoCondition = PcAttribute;
  cond->CoChangeElem = FALSE;
  cond->CoNotNegative = TRUE;
  cond->CoTarget = FALSE;
  cond->CoTypeAttr = ctxt->attrType[att];
  cond->CoTestAttrValue = (ctxt->attrText[att] != NULL);
  attType.AttrSSchema = ctxt->schema;
  attType.AttrTypeNum = ctxt->attrType[att];
  kind = TtaGetAttributeKind (attType);
  if (kind == 0 || kind == 1)
    /* enumerated or integer value */
    cond->CoAttrValue = (long int)ctxt->attrText[att];
  else if (kind == 2)
    /* character string value */
    {
      if (ctxt->attrText[att])
        {
          cond->CoAttrTextValue = TtaStrdup (ctxt->attrText[att]);
          cond->CoTextMatch = (CondMatch)ctxt->attrMatch[att];
        }
      else
        cond->CoAttrTextValue = NULL;
    }
  AddCond (base, cond, attType.AttrSSchema);
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
  memset (ListOfValues, 0, MAX_LENGTH);
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

  if(tsch->PsAttrPRule==NULL)
    return NULL;
  
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
                  attrs->ApMatch == (int)match)
                ppRule = &(attrs->ApTextFirstPRule);
              else if (attrVal == NULL &&
                       attrs->ApString == NULL)
                ppRule = &(attrs->ApTextFirstPRule);
              else if (attrVal == NULL)
                /* this new rule is less specific and should be added before */
                return (ppRule);
              break;
            case AtReferenceAttr:
              ppRule = &(attrs->ApRefFirstPRule);
              break;
            case AtEnumAttr:
              val = (long int) attrVal;
              if (val)
                ppRule = &(attrs->ApEnumFirstPRule[val]);
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
  AttributePres      *attrs = NULL, *new_;
  PtrSSchema          pSS;
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
      GetAttributePres (&new_);
      tsch->PsNAttrPRule->Num[attrType - 1] = nbrules;
      if (att > 0 && ctxt->type)
        {
          new_->ApElemType = ctxt->type;
          new_->ApElemInherits = FALSE;
          tsch->PsNInheritedAttrs->Num[ctxt->type - 1]++;
          tsch->PsNHeirElems->Num[attrType - 1]++;
        }
      if (attrs)
        {
          new_->ApNextAttrPres = attrs->ApNextAttrPres;
          attrs->ApNextAttrPres = new_;
        }
      else
        {
          new_->ApNextAttrPres = tsch->PsAttrPRule->AttrPres[attrType - 1];
          tsch->PsAttrPRule->AttrPres[attrType - 1] = new_;
        }

      attrVal = ctxt->attrText[att];
      match = ctxt->attrMatch[att];
      switch (pSS->SsAttribute->TtAttr[attrType - 1]->AttrType)
        {
        case AtNumAttr:
          new_->ApNCases = 1;
          if (attrVal)
            sscanf (attrVal, "%d", &val);
          else
            val = 0;
          if (val)
            {
              new_->ApCase[0].CaLowerBound = val;
              new_->ApCase[0].CaUpperBound = val;
            }
          else
            {   
              new_->ApCase[0].CaLowerBound = -MAX_INT_ATTR_VAL - 1;
              new_->ApCase[0].CaUpperBound = MAX_INT_ATTR_VAL + 1;
            }
          new_->ApCase[0].CaComparType = ComparConstant;
          new_->ApCase[0].CaFirstPRule = NULL;
          return (&(new_->ApCase[0].CaFirstPRule));
          break;
        case AtTextAttr:
          new_->ApMatch = (CondMatch)match;
          if (attrVal)
            new_->ApString = TtaStrdup (attrVal);
          else
            new_->ApString = NULL;
          new_->ApTextFirstPRule = NULL;
          return (&(new_->ApTextFirstPRule));
          break;
        case AtReferenceAttr:
          new_->ApRefFirstPRule = NULL;
          return (&(new_->ApRefFirstPRule));
          break;
        case AtEnumAttr:
          /* get the attribute value */
          val = (long int) attrVal;
          if (val > 0)
            {
              new_->ApEnumFirstPRule[val] = NULL;
              return (&(new_->ApEnumFirstPRule[val]));
            }
          else
            {
              new_->ApEnumFirstPRule[0] = NULL;
              return (&(new_->ApEnumFirstPRule[0]));
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
  Returns:
  * 0 if the rule has all needed condititons
  * -1 if the rule has less conditions than neeeded
  * 1 if the rule has more conditions than neeeded
  ----------------------------------------------------------------------*/
static int TstRuleContext (PtrPRule rule, GenericContext ctxt,
                           PRuleType pres)
{
  PtrCondition        firstCond, cond;
  AttributeType       attType;
  int                 nbcond, nbCtxtCond, prevAttr;
  int                 i, att, kind;

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
  if (prevAttr && ctxt->attrLevel[0] == 0)
    /* the rule is associated to an attribute */
    {
      nbCtxtCond += ctxt->nbElem;
      if (ctxt->name[0] > AnyType + 1 ||
          (firstCond && firstCond->CoCondition == PcElemType &&
           firstCond->CoTypeElem == ctxt->name[0]))
        nbCtxtCond++;
      /* count the number of conditions in the context */
      for (i = 1; i <= MAX_ANCESTORS && ctxt->attrType[i]; i++)
        nbCtxtCond++;
    }
  else
    /* the rule is associated with an element */
    {
      /* count the number of conditions in the context */
      nbCtxtCond += ctxt->nbElem;
      for (i = 0; i <= MAX_ANCESTORS && ctxt->attrType[i]; i++)
        nbCtxtCond++;
    }

  if (nbCtxtCond < nbcond)
    return (1);
  else if (nbCtxtCond > nbcond)
    return (-1);

  /* same number of conditions */
  /* check if all ancestors are within the rule conditions */
  cond = firstCond;
  if (prevAttr && ctxt->attrLevel[0] == 0)
    {
      // associated to an attribute
      i = 0;
      att = 1;
    }
  else
    {
      // associated to an element
      i = 1;
      att = 0;
    }
  while (i <= ctxt->nbElem)
    {
      if (i > 0 || ctxt->name[0] > AnyType + 1)
        {
          if ((cond->CoTypeElem == ctxt->name[i] &&
               cond->CoCondition == PcElemType &&
               ctxt->rel[i] == RelVoid) ||
              (cond->CoTypeAncestor == ctxt->name[i] &&
               cond->CoCondition == PcWithin &&
               cond->CoImmediate &&
               ctxt->rel[i] == RelParent) ||
              (cond->CoTypeAncestor == ctxt->name[i] &&
               cond->CoCondition == PcWithin &&
               !cond->CoImmediate &&
               ctxt->rel[i] == RelAncestor) ||
              (cond->CoTypeAncestor == ctxt->name[i] &&
               cond->CoCondition == PcSibling &&
               cond->CoImmediate &&
               ctxt->rel[i] == RelPrevious) &&
              (cond->CoCondition == PcFirst &&
               ctxt->firstChild[i]))
            {
              // check the next condition
              cond = cond->CoNextCondition;
              while (ctxt->attrType[att] && ctxt->attrLevel[att] == i)
                {
                  attType.AttrSSchema = ctxt->schema;
                  attType.AttrTypeNum = ctxt->attrType[att];
                  kind = TtaGetAttributeKind (attType);
                  if (cond->CoCondition == PcAttribute &&
                      cond->CoTypeAttr == ctxt->attrType[att] &&
                      (((kind == 0 || kind == 1) &&
                       cond->CoAttrValue == (long int)ctxt->attrText[att]) ||
                       (kind == 2 &&
                        cond->CoTextMatch == (CondMatch)ctxt->attrMatch[att] &&
                        !strcmp (cond->CoAttrTextValue, ctxt->attrText[att]))))
                    // check the next condition
                    cond = cond->CoNextCondition;
                   else
                    /* the attribute is not found */
                    return (1);
                  att++;
                }
            }
          else
            /* the ancestor is not found */
            return (1);
        }
  i++;
    }

  /* all conditions are the same. Compare the pseudo-elements */
  if (rule->PrBoxType == BtElement)
    {
      if (ctxt->pseudo != PbNone)
        return (-1);
    }
  else if (rule->PrBoxType == BtBefore)
    {
      if (ctxt->pseudo == PbNone)
        return (1);
      else if (ctxt->pseudo == PbAfter)
        return (-1);
    }
  else if (rule->PrBoxType == BtAfter)
    {
      if (ctxt->pseudo == PbNone || ctxt->pseudo == PbBefore)
        return (1);
    }
  return (0);
}

/*----------------------------------------------------------------------
  PresRuleSearch : search a presentation rule for a given view
  in an attribute chain or an element chain.
  ----------------------------------------------------------------------*/
static PtrPRule PresRuleSearch (PtrPSchema tsch, GenericContext ctxt,
                                PRuleType pres, FunctionType extra,
                                PtrPRule **chain)
{
  PtrPRule            pRule;
  unsigned int        attrType;
  int                 condCheck;
  ThotBool            found;

  *chain = NULL;
  /* by default the rule doesn't concern any attribute */
  attrType = 0;
  /*
    detect whether there is an attribute in the selector to generate
    an attribute rule with conditions on the current element and/or
    ancestor elements.
  */
  if (ctxt->attrLevel[0] == 0 && ctxt->attrType[0])
    {
      attrType = ctxt->attrType[0];
      *chain = PresAttrChainInsert (tsch, attrType, ctxt, 0);
    }
  /* we are now sure that only elements are concerned */
  else if (ctxt->type)
    {
      if (tsch->PsElemPRule)
        *chain = &tsch->PsElemPRule->ElemPres[ctxt->type - 1];
      else
        return (NULL);
    }
  else
    return (NULL);

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
           pRule->PrPresFunction > extra))
        pRule = NULL;
      else if (pRule->PrType != pres ||
               (pres == PtFunction &&
                pRule->PrPresFunction != extra))
        /* check for extra specification in case of function rule */
        {
          *chain = &(pRule->PrNextPRule);
          pRule = pRule->PrNextPRule;
        }
      else
        {
          condCheck = TstRuleContext (pRule, ctxt, pres);
          if (condCheck == 0)
            /* there is already a rule of this type for this context */
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
  AddConditions : generate presentation conditions for the context.
  ----------------------------------------------------------------------*/
static void AddConditions (PtrCondition *base, GenericContext ctxt)
{
  int                 i, att;
          if (ctxt->box == 0)
            /* rules associated to a presentation box do not have conditions */
            {
              /* In case of an attribute rule, add the Attr condition */
              if (ctxt->attrType[0] && ctxt->attrLevel[0] == 0 && ctxt->type)
                /* the attribute is attached to that element like a
                   selector "a#id" */
                AddElemCond (base, ctxt, 0, FALSE);
              /* add other conditions ... */
              i = 0;
              att = 0;
              while (i <= ctxt->nbElem)
                {
                  if (i > 0)
                    /* it's an ancestor like a selector "li a" */
                    AddElemCond (base, ctxt, i, FALSE);
                  if (ctxt->firstChild[i])
                    /* it's a pseudo-class first-child */
                    AddElemCond (base, ctxt, i, TRUE);
                  while (ctxt->attrType[att] && ctxt->attrLevel[att] == i)
                    {
                      /* skip the first attribute if it is at level 0 : it
                         is already used as the anchor of the list of rules */
                      if (att > 0 || ctxt->attrLevel[att] > 0)
                        AddAttrCond (base, ctxt, att);
                      att++;
                    }
                  i++;
                }
            }
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
  PtrPRule            pRule = NULL;

  /* Search presentation rule */
  pRule = PresRuleSearch (tsch, ctxt, pres, (FunctionType) extra, &chain);
  if (pRule == NULL &&
      (pres != PRFunction || extra != FnShowBox ||
       !TypeHasException (ExcNoShowBox, ctxt->type, (PtrSSchema) ctxt->schema)))
    {
      /* not found, allocate it, fill it and insert it */
      GetPresentRule (&pRule);
      if (pRule)
        {
          pRule->PrType = pres;
          if (pres == PRFunction)
            pRule->PrPresFunction = (FunctionType)extra;
          pRule->PrCond = NULL;
          pRule->PrViewNum = 1;
          pRule->PrSpecifAttr = 0;
          pRule->PrSpecifAttrSSchema = NULL;
	  AddConditions (&pRule->PrCond, ctxt);
          /* chain in the rule */
          if (chain)
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
  cur = PresRuleSearch (tsch, ctxt, pres, (FunctionType) extra, &chain);
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

  /* The drivers affect only the main "WYSIWYG" view */
  rule->PrViewNum = 1;
  if (val.typed_data.unit == VALUE_INHERIT)
    /* the CSS value is "inherit". Create a PRule "Enclosing =" */
    {
      rule->PrPresMode = PresInherit;
      rule->PrInheritMode = InheritParent;
      rule->PrInhPercent = False;
      rule->PrInhAttr = False;
      rule->PrInhDelta = 0;
      rule->PrMinMaxAttr = False;
      rule->PrInhMinOrMax = 0;
      rule->PrInhUnit = UnRelative;
      return;
    }
  else if (val.typed_data.unit == VALUE_CURRENT)
    /* the CSS value is "currentColor". Create an equivalent PRule */
    {
      rule->PrPresMode = PresCurrentColor;
      rule->PrInheritMode = InheritParent;
      rule->PrInhPercent = False;
      rule->PrInhAttr = False;
      rule->PrInhDelta = 0;
      rule->PrMinMaxAttr = False;
      rule->PrInhMinOrMax = 0;
      rule->PrInhUnit = UnRelative;
      return;      
    }
  else
    /* in most cases the rule takes an immediate values */
    rule->PrPresMode = PresImmediate;

  value = val.typed_data.value;
  unit = val.typed_data.unit;
  real = val.typed_data.real;

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
          type == PtXRadius ||           type == PtYRadius ||
          type == PtTop ||               type == PtRight ||
          type == PtBottom ||            type == PtLeft ||
          type == PtBackgroundVertPos || type == PtBackgroundHorizPos)
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
    {
      if (value > 0)
        value = (value + 500) / 1000;
      else
        value = (value - 500) / 1000;
    }
  
  /* now, set-up the value */
  switch (type)
    {
    case PtVisibility:
    case PtDepth:
    case PtFillPattern:
    case PtForeground:
    case PtColor:
    case PtStopColor:
    case PtBorderTopColor:
    case PtBorderRightColor:
    case PtBorderBottomColor:
    case PtBorderLeftColor:
    case PtOpacity:
    case PtStrokeOpacity:
    case PtFillOpacity:
    case PtStopOpacity:
      rule->PrValueType = PrNumValue;
      rule->PrIntValue = value;
      break;
    case PtBackground:
    case PtMarker:
    case PtMarkerStart:
    case PtMarkerMid:
    case PtMarkerEnd:
      if (unit == VALUE_URL)
	rule->PrValueType = PrConstStringValue;
      else
	rule->PrValueType = PrNumValue;
      rule->PrIntValue = value;
      break;
    case PtVis:
      switch (value)
        {
        case VsHidden:
          rule->PrChrValue = 'H';
          break;
        case VsVisible:
          rule->PrChrValue = 'V';
          break;
        case VsCollapse:
          rule->PrChrValue = 'C';
          break;
        case VsInherit:
          rule->PrChrValue = 'I';
          break;
        default:
          break;
        }
      break;
    case PtFont:
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
#ifdef _WINGUI
        case FontOther:
          rule->PrChrValue = -1;
          break;
#endif /* _WINGUI */
        }
      break;
    case PtStyle:
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
    case PtVariant:
      switch (value)
        {
        case VariantNormal:
          rule->PrChrValue = 'N';
          break;
        case VariantSmallCaps:
          rule->PrChrValue = 'C';
          break;
        case VariantDoubleStruck:
          rule->PrChrValue = 'D';
          break;
        case VariantFraktur:
          rule->PrChrValue = 'F';
          break;
        case VariantScript:
          rule->PrChrValue = 'S';
          break;
        default:
          rule->PrChrValue = 'N';
          break;	   
        }
      break;
    case PtUnderline:
      switch (value)
        {
        case NoUnderline:
          rule->PrChrValue ='N' ;
          break;
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
      switch (value)
        {
        case Undefined:
          rule->PrChrValue = 'U';
          break;
        case DisplayNone:
          rule->PrChrValue = 'N';
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
        case InlineBlock:
          rule->PrChrValue = 'b';
          break;
        default:
          rule->PrChrValue = 'U';
          break;
        }
      break;
    case PtListStyleType:
      switch (value)
        {
        case Disc:
          rule->PrChrValue = 'D';
          break;
        case Circle:
          rule->PrChrValue = 'C';
          break;
        case Square:
          rule->PrChrValue = 'S';
          break;
        case Decimal:
          rule->PrChrValue = '1';
          break;
        case DecimalLeadingZero:
          rule->PrChrValue = 'Z';
          break;
        case LowerRoman:
          rule->PrChrValue = 'i';
          break;
        case UpperRoman:
          rule->PrChrValue = 'I';
          break;
        case LowerGreek:
          rule->PrChrValue = 'g';
          break;
        case UpperGreek:
          rule->PrChrValue = 'G';
          break;
        case LowerLatin:
          rule->PrChrValue = 'a';
          break;
        case UpperLatin:
          rule->PrChrValue = 'A';
          break;
        case ListStyleTypeNone:
          rule->PrChrValue = 'N';
          break;
        default:
          rule->PrChrValue = 'N';
          break;
        }
      break;
    case PtListStylePosition:
      switch (value)
        {
        case Inside:
          rule->PrChrValue = 'I';
          break;
        case Outside:
          rule->PrChrValue = 'O';
          break;
        default:
          rule->PrChrValue = 'O';
          break;
        }
      break;
    case PtListStyleImage:
      rule->PrIntValue = value;
      break;
    case PtFloat:
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
    case PtPosition:
      switch (value)
        {
        case PositionStatic:
          rule->PrChrValue = 'S';
          break;
        case PositionRelative:
          rule->PrChrValue = 'R';
          break;
        case PositionAbsolute:
          rule->PrChrValue = 'A';
          break;
        case PositionFixed:
          rule->PrChrValue = 'F';
          break;
        default:
          rule->PrChrValue = 'S';
          break;
        }
      break;
    case PtBorderTopStyle:
    case PtBorderRightStyle:
    case PtBorderBottomStyle:
    case PtBorderLeftStyle:
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
    case PtTop:
    case PtRight:
    case PtBottom:
    case PtLeft:
    case PtBackgroundVertPos:
    case PtBackgroundHorizPos:
      rule->PrMinUnit = int_unit;
      rule->PrMinValue = value;
      rule->PrMinAttr = FALSE;
      break;
    case PtMarginTop:
    case PtMarginLeft:
    case PtMarginBottom:
    case PtMarginRight:
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
          rule->PrMinUnit = int_unit;
          rule->PrMinValue = value;
          rule->PrMinAttr = FALSE;
        }
      break;
    case PtAdjust:
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
    case PtFillRule:
      switch (value)
        {
        case NonZero:
          rule->PrChrValue = 'n';
          break;
        case EvenOdd:
          rule->PrChrValue = 'e';
          break;
        }
      break;
    case PtVertRef:
      rule->PrPosRule.PoDistUnit = int_unit;
      rule->PrPosRule.PoDistance = value;
      rule->PrPosRule.PoRelation = RlSelf;
      rule->PrPosRule.PoPosDef = VertRef;
      rule->PrPosRule.PoPosRef = VertRef;
      break;
    case PtHorizRef:
      rule->PrPosRule.PoDistUnit = int_unit;
      rule->PrPosRule.PoDistance = value;
      rule->PrPosRule.PoRelation = RlEnclosed;
      rule->PrPosRule.PoPosDef = HorizRef;
      rule->PrPosRule.PoPosRef = HorizRef;
      break;
    case PtVertPos:
      if (funcType == 0)
        {
          if (val.typed_data.mainValue)
            {
              rule->PrPosRule.PoDistUnit = int_unit;
              rule->PrPosRule.PoDistance = value;
            }
          else
            {
              rule->PrPosRule.PoDeltaUnit = int_unit;
              rule->PrPosRule.PoDistDelta = value;
            }
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
      if (funcType == 0)
        {
          if (val.typed_data.mainValue)
            {
              rule->PrPosRule.PoDistUnit = int_unit;
              rule->PrPosRule.PoDistance = value;
            }
          else
            {
              rule->PrPosRule.PoDeltaUnit = int_unit;
              rule->PrPosRule.PoDistDelta = value;
            }
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
        case FnContent:
        case FnCreateEnclosing:
          rule->PrPresFunction = (FunctionType) funcType;
          rule->PrNPresBoxes = 1;
          rule->PrPresBox[0] = value;
          rule->PrElement = FALSE;
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
        case FnBackgroundRepeat:
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
            case XREPEAT:
              rule->PrPresBox[0] = XRepeat;
              break;
            case YREPEAT:
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
  TypeUnit            int_unit = (TypeUnit)-1;
  int                 value = 0;
  int                 unit = -1;
  int                 type;
  ThotBool            real = FALSE;
  ThotBool            mainVal = TRUE;

  /* read the value */
  switch (rule->PrType)
    {
    case PtVisibility:
    case PtDepth:
    case PtFillPattern:
    case PtBackground:
    case PtForeground:
    case PtColor:
    case PtStopColor:
    case PtBorderTopColor:
    case PtBorderRightColor:
    case PtBorderBottomColor:
    case PtBorderLeftColor:
    case PtMarker:
    case PtMarkerStart:
    case PtMarkerMid:
    case PtMarkerEnd:
      value = rule->PrIntValue;
      break;
    case PtVis:
      switch (rule->PrChrValue)
        {
        case 'H':
          value = VsHidden;
          break;
        case 'V':
          value = VsVisible;
          break;
        case 'C':
          value = VsCollapse;
          break;
        default:
          value = VsInherit;
          break;
        }
        break;
    case PtOpacity:
    case PtFillOpacity:
    case PtStrokeOpacity:
    case PtStopOpacity:
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
#ifdef _WINGUI
        case -1:
          value = FontOther;
          break;
#endif /* _WINGUI */
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

    case PtVariant:
      switch (rule->PrChrValue)
        {
        case 'N':
          value = VariantNormal;
          break;
        case 'C':
          value = VariantSmallCaps;
          break;
        case 'D':
          value = VariantDoubleStruck;
          break;
        case 'F':
          value = VariantFraktur;
          break;
        case 'S':
          value = VariantScript;
          break;
        default:
          value = VariantNormal;
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
        case 'N':
          value = DisplayNone;
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
        case 'b':
          value = InlineBlock;
          break;
        default:
          value = Undefined;
          break;
        }
      break;
    case PtListStyleType:
      switch (rule->PrChrValue)
        {
        case 'D':
          value = Disc;
          break;
        case 'C':
          value = Circle;
          break;
        case 'S':
          value = Square;
          break;
        case '1':
          value = Decimal;
          break;
        case 'Z':
          value = DecimalLeadingZero;
          break;
        case 'i':
          value = LowerRoman;
          break;
        case 'I':
          value = UpperRoman;
          break;
        case 'g':
          value = LowerGreek;
          break;
        case 'G':
          value = UpperGreek;
          break;
        case 'a':
          value = LowerLatin;
          break;
        case 'A':
          value = UpperLatin;
          break;
        case 'N':
          value = ListStyleTypeNone;
          break;
        default:
          value = ListStyleTypeNone;
          break;
        }
      break;
    case PtListStylePosition:
      switch (rule->PrChrValue)
        {
        case 'I':
          value = Inside;
          break;
        case 'O':
          value = Outside;
          break;
        default:
          value = Outside;
          break;
        }
      break;
    case PtListStyleImage:
      value = rule->PrIntValue;
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
    case PtPosition:
      switch (rule->PrChrValue)
        {
        case 'S':
          value = PnStatic;
          break;
        case 'R':
          value = PnRelative;
          break;
        case 'A':
          value = PnAbsolute;
          break;
        case 'F':
          value = PnFixed;
          break;
        case 'I':
          value = PnInherit;
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
    case PtTop:
    case PtRight:
    case PtBottom:
    case PtLeft:
    case PtBackgroundVertPos:
    case PtBackgroundHorizPos:
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
      if (int_unit == -1)
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

    case PtFillRule:
      switch (rule->PrChrValue)
        {
        case 'n':
          value = NonZero;
          break;
        case 'e':
          value = EvenOdd;
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
        case FnBackgroundRepeat:
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
              value = XREPEAT;
              break;
            case YRepeat:
              value = YREPEAT;
              break;
            default:
              unit = UNIT_INVALID;
              value = 0;
            }
          break;
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
      break;
    case UnXHeight:
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
    case UnAuto:
      unit = VALUE_AUTO;
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
  if (unit == UNIT_REL || unit == UNIT_EM || unit == UNIT_XHEIGHT)
    value /= 10;
  val.typed_data.value = value;
  val.typed_data.unit = unit;
  val.typed_data.real = real;
  val.typed_data.mainValue = mainVal;
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
    case PRVis:
      *intRule = PtVis;
      break;
    case PRFunction:
      *intRule = PtFunction;
      break;
    case PRShowBox:
      *intRule = PtFunction;
      *func = FnShowBox;
      break;
    case PRBackgroundRepeat:
      *intRule = PtFunction;
      *func = FnBackgroundRepeat;
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
    case PRVariant:
      *intRule = PtVariant;
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
    case PRStopOpacity:
      *intRule = PtStopOpacity;
      break;
    case PRMarker:
      *intRule = PtMarker;
      break;
    case PRMarkerStart:
      *intRule = PtMarkerStart;
      break;
    case PRMarkerMid:
      *intRule = PtMarkerMid;
      break;
    case PRMarkerEnd:
      *intRule = PtMarkerEnd;
      break;
    case PRFillRule:
      *intRule = PtFillRule;
      break;
    case PRBackground:
      *intRule = PtBackground;
      break;
    case PRForeground:
      *intRule = PtForeground;
      break;
    case PRColor:
      *intRule = PtColor;
      break;
    case PRStopColor:
      *intRule = PtStopColor;
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
    case PRTop:
      *intRule = PtTop;
      *absolute = TRUE;
      break;
    case PRRight:
      *intRule = PtRight;
      *absolute = TRUE;
      break;
    case PRBottom:
      *intRule = PtBottom;
      *absolute = TRUE;
      break;
    case PRLeft:
      *intRule = PtLeft;
      *absolute = TRUE;
      break;
    case PRBackgroundHorizPos:
      *intRule = PtBackgroundHorizPos;
      *absolute = TRUE;
      break;
    case PRBackgroundVertPos:
      *intRule = PtBackgroundVertPos;
      *absolute = TRUE;
      break;
    case PRDisplay:
      *intRule = PtDisplay;
      break;
    case PRListStyleType:
      *intRule = PtListStyleType;
      break;
    case PRListStyleImage:
      *intRule = PtListStyleImage;
      break;
    case PRListStylePosition:
      *intRule = PtListStylePosition;
      break;
    case PRFloat:
      *intRule = PtFloat;
      break;
    case PRClear:
      *intRule = PtClear;
      break;
    case PRPosition:
      *intRule = PtPosition;
      break;
    case PRContent:
      *intRule = PtFunction;
      *func = FnContent;
      break;
    default:
      *intRule = PtFunction;
    }
}

/*----------------------------------------------------------------------
  GetCounter
  In presentation schema SchemaPrs, get the counter whose name is the
  cst constant in this schema. If such a counter does not exist, create one.
  Return the num of the counter, or 0 if a new counter cannot be created.
  ----------------------------------------------------------------------*/
static int GetCounter (PtrPSchema pSchemaPrs, int cst)
{
  int         cntr;
  Counter    *pCntr;

  cntr = 1;
  while (cntr <= pSchemaPrs->PsNCounters &&
	 pSchemaPrs->PsCounter[cntr - 1].CnNameIndx != cst)
    cntr++;
  pCntr = &pSchemaPrs->PsCounter[cntr - 1];
  if (pCntr->CnNameIndx != cst)
    /* this counter does not exist. Create it */
    {
      if (pSchemaPrs->PsNCounters >= MAX_PRES_COUNTER)
	/* table is full */
	cntr = 0;
      else
	{
	  cntr = pSchemaPrs->PsNCounters;
	  pCntr = &pSchemaPrs->PsCounter[cntr];
	  pCntr->CnNameIndx = cst;
	  pCntr->CnNItems = 0;
	  pCntr->CnNTransmAttrs = 0;
	  pCntr->CnNPresBoxes = 0;
	  pCntr->CnNCreators = 0;
	  pCntr->CnNCreatedBoxes = 0;
	  pSchemaPrs->PsNCounters++;
	  cntr = pSchemaPrs->PsNCounters;
	}
    }
  return cntr;
}

/*----------------------------------------------------------------------
  SetVariableItem
  Add a new item to a Thot presentation variable.
  ----------------------------------------------------------------------*/
static void SetVariableItem (unsigned int type, PSchema tsch,
                             PresentationContext c, PresentationValue v)
{
  GenericContext     ctxt = (GenericContext) c;
  PtrPSchema         pSchemaPrs = (PtrPSchema) tsch;
  PtrPresVariable    pVar;
  Counter           *pCntr;
  int                cst, cntr;

  if (c->destroy || ctxt->var <= 0)
    return;
  pVar = pSchemaPrs->PsVariable->PresVar[ctxt->var - 1];
  if (type == PRContentCounterStyle)
    /* set the style of the latest counter used in the current variable */
    {
      switch (v.typed_data.value)
        {
        case Disc:
          pVar->PvItem[pVar->PvNItems - 1].ViStyle = CntDisc;
          break;
        case Circle:
          pVar->PvItem[pVar->PvNItems - 1].ViStyle = CntCircle;
          break;
        case Square:
          pVar->PvItem[pVar->PvNItems - 1].ViStyle = CntSquare;
          break;
        case Decimal:
          pVar->PvItem[pVar->PvNItems - 1].ViStyle = CntDecimal;
          break;
        case DecimalLeadingZero:
          pVar->PvItem[pVar->PvNItems - 1].ViStyle = CntZLDecimal;
          break;
        case LowerRoman:
          pVar->PvItem[pVar->PvNItems - 1].ViStyle = CntLRoman;
          break;
        case UpperRoman:
          pVar->PvItem[pVar->PvNItems - 1].ViStyle = CntURoman;
          break;
        case LowerGreek:
          pVar->PvItem[pVar->PvNItems - 1].ViStyle = CntLGreek;
          break;
        case UpperGreek:
          pVar->PvItem[pVar->PvNItems - 1].ViStyle = CntUGreek;
          break;
        case LowerLatin:
          pVar->PvItem[pVar->PvNItems - 1].ViStyle = CntLowercase;
          break;
        case UpperLatin:
          pVar->PvItem[pVar->PvNItems - 1].ViStyle = CntUppercase;
          break;
        case ListStyleTypeNone:
          pVar->PvItem[pVar->PvNItems - 1].ViStyle = CntNone;
          break;
        default:
          pVar->PvItem[pVar->PvNItems - 1].ViStyle = CntDecimal;
          break;
        }
      return;
    }
  cst = -1;
  if (type == PRContentString || type == PRContentAttr ||
      type == PRContentCounter)
    cst = PresConstInsert (tsch, (char *)v.pointer, CharString);
  else if (type == PRContentURL)
    cst = PresConstInsert (tsch, (char *)v.pointer, tt_Picture);
  if (cst >= 0 && pVar->PvNItems < MAX_PRES_VAR_ITEM)
    {
      if (type == PRContentCounter)
	{
	  /* look for the named counter */
	  cntr = GetCounter (pSchemaPrs, cst);
	  /* Add a counter item to the variable */
	  if (cntr > 0)
	    {
	      pVar->PvItem[pVar->PvNItems].ViType = VarCounter;
	      pVar->PvItem[pVar->PvNItems].ViStyle = CntDecimal;
	      pVar->PvItem[pVar->PvNItems].ViCounter = cntr;
	      pVar->PvItem[pVar->PvNItems].ViCounterVal = CntCurVal;
	      pVar->PvNItems ++;
	      /* indicate that this counter is used by the presentation box
		 that uses the variable as its content */
	      pCntr = &pSchemaPrs->PsCounter[cntr - 1];
	      pCntr->CnPresBox[pCntr->CnNPresBoxes] = ctxt->box;
	      pCntr->CnNPresBoxes++;
	    }
	}
      else if (type == PRContentString || type == PRContentURL ||
	       type == PRContentAttr)
	{
	  if (type == PRContentAttr)
	    pVar->PvItem[pVar->PvNItems].ViType = VarNamedAttrValue;
	  else
	    pVar->PvItem[pVar->PvNItems].ViType = VarText;
	  pVar->PvItem[pVar->PvNItems].ViConstant = cst;
	  pVar->PvNItems ++;
	}
    }
}

/*----------------------------------------------------------------------
  SetCounterOp
  Add a new operation to a Thot counter.
  ----------------------------------------------------------------------*/
static void SetCounterOp (unsigned int type, PSchema tsch,
			  PresentationContext c, PresentationValue v)
{
  GenericContext     ctxt = (GenericContext) c;
  PtrPSchema         pSchemaPrs = (PtrPSchema) tsch;
  AttributeType      attrType;
  Counter           *pCntr;
  int                cst, cntr, kind;

  if (c->destroy)
    return;
  /* look for the named counter */
  cst = PresConstInsert (tsch, (char *)v.pointer, CharString);
  cntr = GetCounter (pSchemaPrs, cst);
  if (cntr > 0 && (type == PRCounterIncrement || type == PRCounterReset))
    {
      pCntr = &pSchemaPrs->PsCounter[cntr - 1];
      if (pCntr->CnNItems < MAX_PRES_COUNT_ITEM)
	{
	  if (type == PRCounterIncrement)
	    {
	      pCntr->CnItem[pCntr->CnNItems].CiCntrOp = CntrAdd;
	      pCntr->CnItem[pCntr->CnNItems].CiParamValue = v.data;
	    }
	  else
	    {
	      pCntr->CnItem[pCntr->CnNItems].CiCntrOp = CntrSet;
	      pCntr->CnItem[pCntr->CnNItems].CiParamValue = v.data;
	    }
	  pCntr->CnItem[pCntr->CnNItems].CiElemType = ctxt->type;
	  pCntr->CnItem[pCntr->CnNItems].CiCond = NULL;
	  pCntr->CnItem[pCntr->CnNItems].CiCSSURL = ctxt->cssURL;
	  pCntr->CnItem[pCntr->CnNItems].CiCSSLine = ctxt->cssLine;
	  pCntr->CnItem[pCntr->CnNItems].CiAscendLevel = 0;
	  pCntr->CnItem[pCntr->CnNItems].CiInitAttr = 0;
	  pCntr->CnItem[pCntr->CnNItems].CiReinitAttr = 0;
	  /* add conditions based on the context */
	  AddConditions (&pCntr->CnItem[pCntr->CnNItems].CiCond, ctxt);

	  if (ctxt->attrLevel[0] == 0 && ctxt->attrType[0])
	    /* there is an attribute at the first level of the CSS selector */
	    {
	      pCntr->CnItem[pCntr->CnNItems].CiCondAttr = ctxt->attrType[0];
	      pCntr->CnItem[pCntr->CnNItems].CiCondAttrPresent = TRUE;
	      pCntr->CnItem[pCntr->CnNItems].CiCondAttrTextValue = NULL;
	      pCntr->CnItem[pCntr->CnNItems].CiCondAttrIntValue = 0;
              attrType.AttrSSchema = ctxt->schema;
              attrType.AttrTypeNum = ctxt->attrType[0];
              kind = TtaGetAttributeKind (attrType);
	      if (kind == 0 || kind == 1)
		/* enumerated or integer value */
		  pCntr->CnItem[pCntr->CnNItems].CiCondAttrIntValue = (long int)ctxt->attrText[0];
	      else if (kind == 2)
		/* character string value */
		  pCntr->CnItem[pCntr->CnNItems].CiCondAttrTextValue = TtaStrdup(ctxt->attrText[0]);
	    }
	  else
	    pCntr->CnItem[pCntr->CnNItems].CiCondAttr = 0;
	  pCntr->CnNItems++;
        }
    }
}

/*----------------------------------------------------------------------
  VariableInsert
  Add a new variable to a presentation schema.
  ----------------------------------------------------------------------*/
static void VariableInsert (PtrPSchema tsch, GenericContext c)
{
  GenericContext     ctxt = (GenericContext) c;
  PtrPresVariable    var;
  int                i, size;

  if (c->destroy)
    return;
  if (tsch->PsNVariables >= tsch->PsVariableTableSize)
    /* the variable table is full. Extend it */
    {
      /* add 10 new entries */
      size = tsch->PsNVariables + 10;
      i = size * sizeof (PtrPresVariable);
      if (!tsch->PsVariable)
        tsch->PsVariable = (PresVarTable*) malloc (i);
      else
        tsch->PsVariable = (PresVarTable*) realloc (tsch->PsVariable, i);
      if (!tsch->PsVariable)
        {
          ctxt->var = 0;
          return;
        }
      else
        {
          tsch->PsVariableTableSize = size;
          for (i = tsch->PsNVariables; i < size; i++)
            tsch->PsVariable->PresVar[i] = NULL;
        }
    }
  /* allocate and initialize a new variable */
  var = (PtrPresVariable) malloc (sizeof (PresVariable));
  if (var == NUL)
    /* can't allocate a new variable */
    return;
  memset (var, 0, sizeof (PresVariable));
  tsch->PsVariable->PresVar[tsch->PsNVariables] = var;
  tsch->PsNVariables++;
  ctxt->var = tsch->PsNVariables;
  var->PvNItems = 0;
  tsch->PsPresentBox->PresBox[ctxt->box-1]->PbContent = ContVariable;
  tsch->PsPresentBox->PresBox[ctxt->box-1]->PbContVariable = tsch->PsNVariables;
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
  GenericContext     ctxt = (GenericContext) c;
  PtrPRule           pRule;
  PRuleType          intRule;
  ElementType        elType;
  unsigned int       func = 0;
  int                cst;
  int                i;
  int                attrType;
  int                doc = c->doc;
  ThotBool           absolute, generic, minValue;

  if (type == PRContentString || type == PRContentURL ||
      type == PRContentAttr || type == PRContentCounter ||
      type == PRContentCounterStyle)
    /* it is a value in a CSS content rule. Generate the corresponding
       Thot presentation variable item */
    SetVariableItem (type, tsch, c, v);
  else if (type == PRCounterIncrement || type == PRCounterReset)
    SetCounterOp (type, tsch, c, v);
  else
    {
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
          pRule = NULL;
          if (generic)
            pRule = PresRuleInsert ((PtrPSchema) tsch, ctxt, intRule, func);
          else
            pRule = InsertElementPRule ((PtrElement) el, LoadedDocument[doc-1],
                                        intRule, func, c->cssSpecificity,
                                        c->cssLine);
          if (pRule)
            {
              if (generic && ctxt->pseudo == PbBefore)
                pRule->PrBoxType = BtBefore;
              else if (generic && ctxt->pseudo == PbAfter)
                pRule->PrBoxType = BtAfter;
              cst = 0;
              if (type == PRBackgroundPicture ||
                  (type == PRListStyleImage &&
                   v.typed_data.value != 0 &&
                   v.typed_data.unit != VALUE_INHERIT) ||
		  ((type == PRBackground ||
		    type == PRMarker || type == PRMarkerStart ||
		    type == PRMarkerMid || type == PRMarkerEnd) &&
		   v.typed_data.unit == VALUE_URL))
                {
                  if (!generic)
                    tsch = (PSchema) PresentationSchema
                    (((PtrElement)el)->ElStructSchema, LoadedDocument[doc - 1]);
		  if (type == PRBackground ||
		      type == PRMarker || type == PRMarkerStart ||
		      type == PRMarkerMid || type == PRMarkerEnd)
		    cst = PresConstInsert (tsch, (char *)v.pointer, CharString);
		  else
		    {
		      cst = PresConstInsert (tsch, (char *)v.pointer, tt_Picture);
		      v.typed_data.unit = UNIT_REL;
		    }
                  v.typed_data.value = cst;
                  v.typed_data.real = FALSE;
                  v.typed_data.mainValue = TRUE;
                }
              /* avoid to override an important rule by a non-important rule */
              if (ctxt->important || !pRule->PrImportant)
                {
                  pRule->PrImportant = ctxt->important;
                  /* store the rule priority */
                  pRule->PrSpecificity = ctxt->cssSpecificity;
                  /* origin of the CSS rule */
                  pRule->PrCSSLine = ctxt->cssLine;
                  pRule->PrCSSURL = ctxt->cssURL;

                  if (type == PRVertPos || type == PRHorizPos)
                    {
                      if (ctxt->type > 0)
                        func = ctxt->type;
                    }
                  else if (type == PRContent)
                    /* create a presentation variable */
                    {
                      PresBoxInsert ((PtrPSchema)tsch, ctxt);
                      VariableInsert ((PtrPSchema)tsch, ctxt);
                      v.typed_data.value = ctxt->var;
                    }
                  PresentationValueToPRule (v, intRule, pRule, func, absolute,
                                            generic, minValue);
                }
              if (generic)
                {
                  pRule->PrViewNum = 1;
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
                           (FunctionType) func, &chain);
  else
    rule = SearchElementPRule ((PtrElement) el, intRule, func);
  if (rule == NULL)
    return (-1);

  *v = PRuleToPresentationValue (rule);
  if (type == PRBackgroundPicture)
    {
      cst = v->typed_data.unit;
      v->pointer = ((PtrPSchema) tsch)->PsConstant[cst-1].PdString;
    }
  return (0);
}

/*----------------------------------------------------------------------
  PRuleToPresentationSetting : Translate the internal values stored
  in a PRule to a valid PresentationSetting.
  ----------------------------------------------------------------------*/
void PRuleToPresentationSetting (PtrPRule rule, PresentationSetting setting,
                                 PtrPSchema pPS)
{
  int                      cst;

  /* first decoding step : analyze the type of the rule */
  switch (rule->PrType)
    {
    case PtVisibility:
      setting->type = PRVisibility;
      break;
    case PtVis:
      setting->type = PRVis;
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
    case PtStopOpacity:
      setting->type = PRStopOpacity;
      break;
    case PtMarker:
      setting->type = PRMarker;
      break;
    case PtMarkerStart:
      setting->type = PRMarkerStart;
      break;
    case PtMarkerMid:
      setting->type = PRMarkerMid;
      break;
    case PtMarkerEnd:
      setting->type = PRMarkerEnd;
      break;
    case PtFillRule:
      setting->type = PRFillRule;
      break;
    case PtBackground:
      setting->type = PRBackground;
      break;
    case PtForeground:
      setting->type = PRForeground;
      break;
    case PtColor:
      setting->type = PRColor;
      break;
    case PtStopColor:
      setting->type = PRStopColor;
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
    case PtVariant:
      setting->type = PRVariant;
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
    case PtListStyleType:
      setting->type = PRListStyleType;
      break;
    case PtListStyleImage:
      setting->type = PRListStyleImage;
      break;
    case PtListStylePosition:
      setting->type = PRListStylePosition;
      break;
    case PtFloat:
      setting->type = PRFloat;
      break;
    case PtClear:
      setting->type = PRClear;
      break;
    case PtPosition:
      setting->type = PRPosition;
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
    case PtTop:
      setting->type = PRTop;
      break;
    case PtRight:
      setting->type = PRRight;
      break;
    case PtBottom:
      setting->type = PRBottom;
      break;
    case PtLeft:
      setting->type = PRLeft;
      break;
    case PtBackgroundHorizPos:
      setting->type = PRBackgroundHorizPos;
      break;
    case PtBackgroundVertPos:
      setting->type = PRBackgroundVertPos;
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
    case PtHorizRef:
      setting->type = PRHorizRef;
      break;
    case PtFunction:
      switch (rule->PrPresFunction)
        {
        case FnShowBox:
          setting->type = PRShowBox;
          break;
        case FnBackgroundPicture:
          setting->type = PRBackgroundPicture;
          break;
        case FnBackgroundRepeat:
          setting->type = PRBackgroundRepeat;
          break;
        case FnPage:
          setting->type = PRPageBefore;
          break;
        case FnContent:
          setting->type = PRContent;
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
  if (setting->type == PRBackgroundPicture ||
      setting->type == PRListStyleImage)
    {
      /* get the string instead of the box index */
      cst = setting->value.typed_data.value;
      if (cst <= 0)
        setting->value.pointer = NULL;
      else
        setting->value.pointer = pPS->PsConstant[cst-1].PdString;
    }
}

/*----------------------------------------------------------------------
  GetGenericStyleContext : user level function needed to allocate and
  initialize a GenericContext.
  ----------------------------------------------------------------------*/
GenericContext TtaGetGenericStyleContext (Document doc)
{
  GenericContext      ctxt;

  ctxt = (GenericContext) TtaGetMemory (sizeof (GenericContextBlock));
  if (ctxt == NULL)
    return (NULL);
  memset (ctxt, 0, sizeof (GenericContextBlock));
  if (doc)
    {
      ctxt->doc = doc;
      ctxt->schema = TtaGetDocumentSSchema (doc);
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
  memset (ctxt, 0, sizeof (PresentationContextBlock));
  ctxt->doc = doc;
  ctxt->schema = TtaGetDocumentSSchema (doc);
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
  TtaApplyAllSpecificSettings browses all the PRules structures,
  associated to the corresponding Specific Context 
  structure, and calls the given handler for each one.
  ----------------------------------------------------------------------*/
void TtaApplyAllSpecificSettings (Element el, Document doc,
                                  SettingsApplyHandler handler, void *param)
{
  PtrPRule                 rule;
  PresentationSettingBlock setting;
  PtrPSchema               pPS;

  if (el == NULL)
    return;
  pPS = PresentationSchema (((PtrElement) el)->ElStructSchema,
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
          PRuleToPresentationSetting (rule, &setting, pPS);
          handler (el, doc, &setting, param);
        }
      rule = rule->PrNextPRule;
    }
}

/*----------------------------------------------------------------------
  AddBorderStyleValue
  -----------------------------------------------------------------------*/
static void AddBorderStyleValue (char *buffer, int value)
{
  switch (value)
    {
    case BorderStyleNone:
      strcat (buffer, "none");
      break;
    case BorderStyleHidden:
      strcat (buffer, "hidden");
      break;
    case BorderStyleDotted:
      strcat (buffer, "dotted");
      break;
    case BorderStyleDashed:
      strcat (buffer, "dashed");
      break;
    case BorderStyleSolid:
      strcat (buffer, "solid");
      break;
    case BorderStyleDouble:
      strcat (buffer, "double");
      break;
    case BorderStyleGroove:
      strcat (buffer, "groove");
      break;
    case BorderStyleRidge:
      strcat (buffer, "ridge");
      break;
    case BorderStyleInset:
      strcat (buffer, "inset");
      break;
    case BorderStyleOutset:
      strcat (buffer, "outset");
      break;
    }
}

/*----------------------------------------------------------------------
  TtaPToCss:  translate a PresentationSetting to the
  equivalent CSS string, and add it to the buffer given as the
  argument. It is used when extracting the CSS string from actual
  presentation.
  el is the element for which the style rule is generated
 
  All the possible values returned by the presentation drivers are
  described in thotlib/include/presentation.h
  -----------------------------------------------------------------------*/
void TtaPToCss (PresentationSetting settings, char *buffer, int len,
                Element el, void* pSchP)
{
  ElementType         elType;
  float               fval = 0;
  unsigned short      red, green, blue;
  int                 add_unit = 0, val;
  unsigned int        unit, i;
  ThotBool            real = FALSE;
  PresConstant	      *pConst;

  buffer[0] = EOS;
  if (len < 40)
    return;

  unit = settings->value.typed_data.unit;
  if (settings->value.typed_data.real)
    {
      real = TRUE;
      fval = (float) settings->value.typed_data.value;
      fval /= 1000.;
    }

  switch (settings->type)
    {
    case PRVisibility:
      break;
    case PRVis:
      switch (settings->value.typed_data.value)
        {
        case VsHidden:
          strcpy (buffer, "visibility: hidden");
          break;
        case VsVisible:
          strcpy (buffer, "visibility: visible");
          break;
        case VsCollapse:
          strcpy (buffer, "visibility: collapse");
          break;
        case VsInherit:
          strcpy (buffer, "visibility: inherit");
          break;
        default:
          break;
        }
      break;
    case PRListStyleType:
      switch (settings->value.typed_data.value)
        {
        case Disc:
          strcpy (buffer, "list-style-type: disc");
          break;
        case Circle:
          strcpy (buffer, "list-style-type: circle");
          break;
        case Square:
          strcpy (buffer, "list-style-type: square");
          break;
        case Decimal:
          strcpy (buffer, "list-style-type: decimal");
          break;
        case DecimalLeadingZero:
          strcpy (buffer, "list-style-type: decimal-leading-zero");
          break;
        case LowerRoman:
          strcpy (buffer, "list-style-type: lower-roman");
          break;
        case UpperRoman:
          strcpy (buffer, "list-style-type: upper-roman");
          break;
        case LowerGreek:
          strcpy (buffer, "list-style-type: lower-greek");
          break;
        case UpperGreek:
          strcpy (buffer, "list-style-type: upper-greek");
          break;
        case LowerLatin:
          strcpy (buffer, "list-style-type: lower-latin");
          break;
        case UpperLatin:
          strcpy (buffer, "list-style-type: upper-latin");
          break;
        case ListStyleTypeNone:
          strcpy (buffer, "list-style-type: none");
          break;
        default:
          break;
        }
      break;
    case PRListStyleImage:
      if (settings->value.pointer != NULL)
        sprintf (buffer, "list-style-image: url(%s)",
                 (char*)(settings->value.pointer));
      else
        strcpy (buffer, "list-style-image: none");
      break;
    case PRListStylePosition:
      switch (settings->value.typed_data.value)
        {
        case Inside:
          strcpy (buffer, "list-style-position: inside");
          break;
        case Outside:
          strcpy (buffer, "list-style-position: outside");
          break;
        default:
          break;
        }
      break;
    case PRVertOverflow:
    case PRHorizOverflow:
    case PRVertRef:
      break;
    case PRHorizRef:
      strcpy (buffer, "vertical-align: ");
      if (real)
        sprintf (buffer, "vertical-align: %g", fval);
      else
        sprintf (buffer, "vertical-align: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRHeight:
      if (unit == VALUE_AUTO)
        strcpy (buffer, "height: ");
      else if (real)
        sprintf (buffer, "height: %g", fval);
      else
        sprintf (buffer, "height: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRWidth:
      if (unit == VALUE_AUTO)
        strcpy (buffer, "width: ");
      else if (real)
        sprintf (buffer, "width: %g", fval);
      else
        sprintf (buffer, "width: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRVertPos:
    case PRHorizPos:
      break;
    case PRMarginTop:
      if (unit == VALUE_AUTO)
        strcpy (buffer, "margin-top: ");
      else if (real)
        sprintf (buffer, "margin-top: %g", fval);
      else
        sprintf (buffer, "margin-top: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRMarginRight:
      if (unit == VALUE_AUTO)
        strcpy (buffer, "margin-right: ");
      else if (real)
        sprintf (buffer, "margin-right: %g", fval);
      else
        sprintf (buffer, "margin-right: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRMarginBottom:
      if (unit == VALUE_AUTO)
        strcpy (buffer, "margin-bottom: ");
      else if (real)
        sprintf (buffer, "margin-bottom: %g", fval);
      else
        sprintf (buffer, "margin-bottom: %d",
                 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRMarginLeft:
      if (unit == VALUE_AUTO)
        strcpy (buffer, "margin-left: ");
      else if (real)
        sprintf (buffer, "margin-left: %g", fval);
      else
        sprintf (buffer, "margin-left: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRPaddingTop:
      if (real)
        sprintf (buffer, "padding-top: %g", fval);
      else
        sprintf (buffer, "padding-top: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRPaddingRight:
      if (real)
        sprintf (buffer, "padding-right: %g", fval);
      else
        sprintf (buffer, "padding-right: %d",
                 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRPaddingBottom:
      if (real)
        sprintf (buffer, "padding-bottom: %g", fval);
      else
        sprintf (buffer, "padding-bottom: %d",
                 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRPaddingLeft:
      if (real)
        sprintf (buffer, "padding-left: %g", fval);
      else
        sprintf (buffer, "padding-left: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRBorderTopWidth:
      if (real)
        sprintf (buffer, "border-top-width: %g", fval);
      else
        sprintf (buffer, "border-top-width: %d",
                 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRBorderRightWidth:
      if (real)
        sprintf (buffer, "border-right-width: %g", fval);
      else
        sprintf (buffer, "border-right-width: %d",
                 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRBorderBottomWidth:
      if (real)
        sprintf (buffer, "border-bottom-width: %g", fval);
      else
        sprintf (buffer, "border-bottom-width: %d",
                 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRBorderLeftWidth:
      if (real)
        sprintf (buffer, "border-left-width: %g", fval);
      else
        sprintf (buffer, "border-left-width: %d",
                 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRBorderTopColor:
      val = settings->value.typed_data.value;
      if (val == -2)
        strcpy (buffer, "border-top-color: transparent");
      else if (val > 0)
        {
          TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
          sprintf (buffer, "border-top-color: #%02X%02X%02X", red, green, blue);
        }
      break;
    case PRBorderRightColor:
      val = settings->value.typed_data.value;
      if (val == -2)
        strcpy (buffer, "border-right-color: transparent");
      else if (val > 0)
        {
          TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
          sprintf (buffer, "border-right-color: #%02X%02X%02X", red, green, blue);
        }
      break;
    case PRBorderBottomColor:
      val = settings->value.typed_data.value;
      if (val == -2)
        strcpy (buffer, "border-bottom-color: transparent");
      else if (val > 0)
        {
          TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
          sprintf (buffer, "border-bottom-color: #%02X%02X%02X", red, green, blue);
        }
      break;
    case PRBorderLeftColor:
      val = settings->value.typed_data.value;
      if (val == -2)
        strcpy (buffer, "border-left-color: transparent");
      else if (val > 0)
        {
          TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
          sprintf (buffer, "border-left-color: #%02X%02X%02X", red, green, blue);
        }
      break;
    case PRBorderTopStyle:
      strcpy (buffer, "border-top-style: ");
      AddBorderStyleValue (buffer, settings->value.typed_data.value);
      break;
    case PRBorderRightStyle:
      strcpy (buffer, "border-right-style: ");
      AddBorderStyleValue (buffer, settings->value.typed_data.value);
      break;
    case PRBorderBottomStyle:
      strcpy (buffer, "border-bottom-style: ");
      AddBorderStyleValue (buffer, settings->value.typed_data.value);
      break;
    case PRBorderLeftStyle:
      strcpy (buffer, "border-left-style: ");
      AddBorderStyleValue (buffer, settings->value.typed_data.value);
      break;
    case PRSize:
      if (unit == UNIT_REL)
        {
          if (real)
            {
              sprintf (buffer, "font-size: %g", fval);
              add_unit = 1;
            }
          else
            switch (settings->value.typed_data.value)
              {
              case 1:
                strcpy (buffer, "font-size: xx-small");
                break;
              case 2:
                strcpy (buffer, "font-size: x-small");
                break;
              case 3:
                strcpy (buffer, "font-size: small");
                break;
              case 4:
                strcpy (buffer, "font-size: medium");
                break;
              case 5:
                strcpy (buffer, "font-size: large");
                break;
              case 6:
                strcpy (buffer, "font-size: x-large");
                break;
              case 7:
              case 8:
              case 9:
              case 10:
              case 11:
              case 12:
                strcpy (buffer, "font-size: xx-large");
                break;
              }
        }
      else
        {
          if (real)
            sprintf (buffer, "font-size: %g", fval);
          else
            sprintf (buffer, "font-size: %d",
                     settings->value.typed_data.value);
          add_unit = 1;
        }
      break;
    case PRStyle:
      switch (settings->value.typed_data.value)
        {
        case StyleRoman:
          strcpy (buffer, "font-style: normal");
          break;
        case StyleItalics:
          strcpy (buffer, "font-style: italic");
          break;
        case StyleOblique:
          strcpy (buffer, "font-style: oblique");
          break;
        }
      break;
    case PRWeight:
      switch (settings->value.typed_data.value)
        {
        case WeightBold:
          strcpy (buffer, "font-weight: bold");
          break;
        case WeightNormal:
          strcpy (buffer, "font-weight: normal");
          break;
        }
      break;
    case PRVariant:
      switch (settings->value.typed_data.value)
        {
        case VariantNormal:
          strcpy (buffer, "font-variant: normal");
          break;
        case VariantSmallCaps:
          strcpy (buffer, "font-variant: small-caps");
          break;
        case VariantDoubleStruck:
          strcpy (buffer, "font-variant: double-struck");
          break;
        case VariantFraktur:
          strcpy (buffer, "font-variant: fraktur");
          break;
        case VariantScript:
          strcpy (buffer, "font-variant: script");
          break;
        }
      break;
    case PRFont:
      switch (settings->value.typed_data.value)
        {
        case FontHelvetica:
          strcpy (buffer, "font-family: sans-serif");
          break;
        case FontTimes:
          strcpy (buffer, "font-family: serif");
          break;
        case FontCourier:
          strcpy (buffer, "font-family: monospace");
          break;
        }
      break;
    case PRUnderline:
      switch (settings->value.typed_data.value)
        {
        case NoUnderline:
          strcpy (buffer, "text-decoration: none");
          break;
        case Underline:
          strcpy (buffer, "text-decoration: underline");
          break;
        case Overline:
          strcpy (buffer, "text-decoration: overline");
          break;
        case CrossOut:
          strcpy (buffer, "text-decoration: line-through");
          break;
        }
      break;
    case PRThickness:
      break;
    case PRIndent:
      if (real)
        sprintf (buffer, "text-indent: %g", fval);
      else
        sprintf (buffer, "text-indent: %d",
                 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRLineSpacing:
      if (real)
        sprintf (buffer, "line-height: %g", fval);
      else
        sprintf (buffer, "line-height: %d",
                 settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRDepth:
      if (settings->value.typed_data.unit == VALUE_INHERIT)
        sprintf (buffer, "z-index: inherit");
      else if (settings->value.typed_data.unit == VALUE_AUTO)
        sprintf (buffer, "z-index: auto");
      else
        sprintf (buffer, "z-index: %d", - settings->value.typed_data.value);
      break;
      break;
    case PRAdjust:
      elType = TtaGetElementType(el);
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
        {
          switch (settings->value.typed_data.value)
            {
            case AdjustLeft:
              strcpy (buffer, "text-anchor: start");
              break;
            case AdjustRight:
              strcpy (buffer, "text-anchor: end");
              break;
            case Centered:
              strcpy (buffer, "text-anchor: middle");
              break;
            }
        }
      else
        switch (settings->value.typed_data.value)
          {
          case AdjustLeft:
            strcpy (buffer, "text-align: left");
            break;
          case AdjustRight:
            strcpy (buffer, "text-align: right");
            break;
          case Centered:
            strcpy (buffer, "text-align: center");
            break;
          case LeftWithDots:
            strcpy (buffer, "text-align: left");
            break;
          case Justify:
            strcpy (buffer, "text-align: justify");
            break;
          }
      break;
    case PRDirection:
      switch (settings->value.typed_data.value)
        {
        case LeftToRight:
          strcpy (buffer, "direction: ltr");
          break;
        case RightToLeft:
          strcpy (buffer, "direction: rtl");
          break;
        }
      break;
    case PRUnicodeBidi:
      switch (settings->value.typed_data.value)
        {
        case Normal:
          strcpy (buffer, "unicode-bidi: normal");
          break;
        case Embed:
          strcpy (buffer, "unicode-bidi: embed");
          break;
        case Override:
          strcpy (buffer, "unicode-bidi: bidi-override");
          break;
        }
      break;
    case PRLineStyle:
      break;
    case PRLineWeight:
      elType = TtaGetElementType(el);
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
        {
          if (real)
            sprintf (buffer, "stroke-width: %g", fval);
          else
            sprintf (buffer, "stroke-width: %d",
                     settings->value.typed_data.value);
        }
      add_unit = 1;
      break;
    case PRFillPattern:
      break;
    case PRBackground:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      elType = TtaGetElementType(el);
      if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG") == 0)
        sprintf (buffer, "fill: #%02X%02X%02X", red, green, blue);
      else
        sprintf (buffer, "background-color: #%02X%02X%02X", red, green,
                 blue);
      break;
    case PRForeground:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      elType = TtaGetElementType(el);
      if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG") == 0)
        sprintf (buffer, "stroke: #%02X%02X%02X", red, green, blue);
      else
        sprintf (buffer, "color: #%02X%02X%02X", red, green, blue);
      break;
    case PRColor:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      sprintf (buffer, "color: #%02X%02X%02X", red, green, blue);
      break;
    case PRStopColor:
      TtaGiveThotRGB (settings->value.typed_data.value, &red, &green, &blue);
      sprintf (buffer, "stop-color: #%02X%02X%02X", red, green, blue);
      break;
    case PROpacity:
      sprintf (buffer, "opacity: %g", fval);
      break;
    case PRFillOpacity:
      sprintf (buffer, "fill-opacity: %g", fval);
      break;
    case PRStrokeOpacity:
      sprintf (buffer, "stroke-opacity: %g", fval);
      break;
    case PRStopOpacity:
      sprintf (buffer, "stop-opacity: %g", fval);
      break;
    case PRMarker:
      if (settings->value.typed_data.unit == VALUE_INHERIT)
        sprintf (buffer, "marker: inherit");
      else if (settings->value.typed_data.value == 0)
        sprintf (buffer, "marker: none");
      else
	{
	  pConst = &((PtrPSchema)pSchP)->PsConstant[settings->value.typed_data.value-1];
	  if (pConst->PdString)
	    sprintf (buffer, "marker: url(%s)", pConst->PdString);
	}
      break;
    case PRMarkerStart:
      if (settings->value.typed_data.unit == VALUE_INHERIT)
        sprintf (buffer, "marker-start: inherit");
      else if (settings->value.typed_data.value == 0)
        sprintf (buffer, "marker-start: none");
      else
	{
	  pConst = &((PtrPSchema)pSchP)->PsConstant[settings->value.typed_data.value-1];
	  if (pConst->PdString)
	    sprintf (buffer, "marker-start: url(%s)", pConst->PdString);
	}
      break;
    case PRMarkerMid:
      if (settings->value.typed_data.unit == VALUE_INHERIT)
        sprintf (buffer, "marker-mid: inherit");
      else if (settings->value.typed_data.value == 0)
        sprintf (buffer, "marker-mid: none");
      else
	{
	  pConst = &((PtrPSchema)pSchP)->PsConstant[settings->value.typed_data.value-1];
	  if (pConst->PdString)
	    sprintf (buffer, "marker-mid: url(%s)", pConst->PdString);
	}
      break;
    case PRMarkerEnd:
      if (settings->value.typed_data.unit == VALUE_INHERIT)
        sprintf (buffer, "marker-end: inherit");
      else if (settings->value.typed_data.value == 0)
        sprintf (buffer, "marker-end: none");
      else
	{
	  pConst = &((PtrPSchema)pSchP)->PsConstant[settings->value.typed_data.value-1];
	  if (pConst->PdString)
	    sprintf (buffer, "marker-end: url(%s)", pConst->PdString);
	}
      break;
    case PRFillRule:
      if (settings->value.typed_data.unit == VALUE_INHERIT)
        sprintf (buffer, "fill-rule: inherit");
      else if (settings->value.typed_data.value == NonZero)
        sprintf (buffer, "fill-rule: nonzero");
      else if (settings->value.typed_data.value == EvenOdd)
        sprintf (buffer, "fill-rule: evenodd");
      break;
    case PRHyphenate:
      /*
        case PRPageBreak:
        case PRLineBreak:
        case PRGather: */
    case PRXRadius:
    case PRYRadius:
      break;
    case PRPosition:
      switch (settings->value.typed_data.value)
        {
        case PnStatic:
          strcpy (buffer, "position: static");
          break;
        case PnRelative:
          strcpy (buffer, "position: relative");
          break;
        case PnAbsolute:
          strcpy (buffer, "position: absolute");
          break;
        case PnFixed:
          strcpy (buffer, "position: fixed");
          break;
        case PnInherit:
          strcpy (buffer, "position: inherit");
          break;
        default:
          break;
        }
      break;
    case PRTop:
      if (unit == VALUE_AUTO)
        sprintf (buffer, "top: ");
      else if (real)
        sprintf (buffer, "top: %g", fval);
      else
        sprintf (buffer, "top: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRRight:
      if (unit == VALUE_AUTO)
        sprintf (buffer, "right: ");
      else if (real)
        sprintf (buffer, "right: %g", fval);
      else
        sprintf (buffer, "right: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRBottom:
      if (unit == VALUE_AUTO)
        sprintf (buffer, "bottom: ");
      else if (real)
        sprintf (buffer, "bottom: %g", fval);
      else
        sprintf (buffer, "bottom: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRLeft:
      if (unit == VALUE_AUTO)
        sprintf (buffer, "left: ");
      else if (real)
        sprintf (buffer, "left: %g", fval);
      else
        sprintf (buffer, "left: %d", settings->value.typed_data.value);
      add_unit = 1;
      break;
    case PRBackgroundHorizPos:
      if (unit == UNIT_PERCENT)
        {
          if (settings->value.typed_data.value == 0)
            sprintf (buffer, "background-position: left");
          else if (settings->value.typed_data.value == 100)
            sprintf (buffer, "background-position: right");
          else if (settings->value.typed_data.value == 50)
            sprintf (buffer, "background-position: center");
          else
            if (real)
              sprintf (buffer, "background-position: %g%%", fval);
            else
              sprintf (buffer, "background-position: %d%%",
                       settings->value.typed_data.value);
        }
      else
        {
          if (real)
            sprintf (buffer, "background-position: %g", fval);
          else
            sprintf (buffer, "background-position: %d",
                     settings->value.typed_data.value);
          add_unit = 1;
        }
      break;
    case PRBackgroundVertPos:
      if (unit == UNIT_PERCENT)
        {
          if (settings->value.typed_data.value == 0)
            sprintf (buffer, "background-position: top");
          else if (settings->value.typed_data.value == 100)
            sprintf (buffer, "background-position: bottom");
          else if (settings->value.typed_data.value == 50)
            sprintf (buffer, "background-position: center");
          else
            if (real)
              sprintf (buffer, "background-position: %g%%", fval);
            else
              sprintf (buffer, "background-position: %d%%",
                       settings->value.typed_data.value);
        }
      else
        {
          if (real)
            sprintf (buffer, "background-position: %g", fval);
          else
            sprintf (buffer, "background-position: %d",
                     settings->value.typed_data.value);
          add_unit = 1;
        }
      break;
    case PRFloat:
      switch (settings->value.typed_data.value)
        {
        case FloatNone:
          strcpy (buffer, "float: none");
          break;
        case FloatLeft:
          strcpy (buffer, "float: left");
          break;
        case FloatRight:
          strcpy (buffer, "float: right");
          break;
        default:
          break;
        }
      break;
    case PRClear:
      switch (settings->value.typed_data.value)
        {
        case ClearNone:
          strcpy (buffer, "clear: none");
          break;
        case ClearLeft:
          strcpy (buffer, "clear: left");
          break;
        case ClearRight:
          strcpy (buffer, "clear: right");
          break;
        case ClearBoth:
          strcpy (buffer, "clear: both");
          break;
        default:
          break;
        }
      break;
    case PRDisplay:
      switch (settings->value.typed_data.value)
        {
        case Inline:
          strcpy (buffer, "display: inline");
          break;
        case Block:
          strcpy (buffer, "display: block");
          break;
        case ListItem:
          strcpy (buffer, "display: list-item");
          break;
        case RunIn:
          strcpy (buffer, "display: runin");
          break;
        case InlineBlock:
          strcpy (buffer, "display: inlineblock");
          break;
        case DisplayNone:
          strcpy (buffer, "display: none");
          break;
        default:
          break;
        }
      break;
      /*
        case PRBreak1:
        case PRBreak2:
        case PRCreateEnclosing:
        case PRShowBox:
        break;
      */
    case PRBackgroundPicture:
      if (settings->value.pointer != NULL)
        sprintf (buffer, "background-image: url(%s)",
                 (char*)(settings->value.pointer));
      else
        sprintf (buffer, "background-image: none");
      break;
    case PRBackgroundRepeat:
      switch (settings->value.typed_data.value)
        {
        case REALSIZE:
          sprintf (buffer, "background-repeat: no-repeat");
          break;
        case REPEAT:
          sprintf (buffer, "background-repeat: repeat");
          break;
        case YREPEAT:
          sprintf (buffer, "background-repeat: repeat-y");
          break;
        case XREPEAT:
          sprintf (buffer, "background-repeat: repeat-x");
          break;
        }
      break;
    case PRNotInLine:
    case PRNone:
    case PRPageBefore:
    case PRPageAfter:
    case PRPageInside:
      break;
    case PRContentString:
    case PRContentURL:
    case PRContentAttr:
    case PRContentCounter:
    case PRContentCounterStyle:
      break;
    case PRContent:
      sprintf (buffer, "content:");
      break;
    default:
      break;
    }

  if (add_unit)
    {
      if (unit == VALUE_AUTO)
        strcat (buffer, "auto");
      else
        {
          /* add the unit string to the CSS string */
          i = 0;
          while (CSSUnitNames[i].sign)
            {
              if (CSSUnitNames[i].unit == unit)
                {
                  strcat (buffer, CSSUnitNames[i].sign);
                  break;
                }
              else
                i++;
            }
        }
    }
}
