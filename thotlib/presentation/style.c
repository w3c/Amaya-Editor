/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
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

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"

#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "exceptions_f.h"
#include "memory_f.h"
#include "style_f.h"

/*----------------------------------------------------------------------
  BuildBoxName : generate an unique name encoding for the given context.
  Assume the ancestor list has been sorted.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BuildBoxName (GenericContext ctxt, Name *boxname)
#else  /* __STDC__ */
static void         BuildBoxName (ctxt, boxname)
GenericContext      ctxt;
Name               *boxname;
#endif /* !__STDC__ */
{
  int                 i;
  int                 len;
  CHAR_T              buffer[100];

  buffer[0] = 0;
  len = 0;
  for (i = 0; i < MAX_ANCESTORS && ctxt->name[i]; i++)
    {
      if (ctxt->names_nb[i] > 1)
	usprintf (&buffer[len], TEXT("%d:%d/"), ctxt->name[i], ctxt->names_nb[i]);
      else
	usprintf (&buffer[len], TEXT("%d/"), ctxt->name[i]);
      len = ustrlen (buffer);
    }
  if (ctxt->type)
    usprintf (&buffer[len], TEXT("%d,"), ctxt->type);
  len = ustrlen (buffer);
  if (ctxt->attrType[0])
    usprintf (&buffer[len], TEXT("%d.%s,"), ctxt->attrType[0], ctxt->attrText[0]);
  len = ustrlen (buffer);

  ustrncpy ((STRING) boxname, buffer, sizeof (Name));
}

/*----------------------------------------------------------------------
 BoxRuleSearch : look in the array of boxes for an entry
        corresponding to the current context.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule     BoxRuleSearch (PtrPSchema tsch, GenericContext ctxt)
#else  /* __STDC__ */
static PtrPRule     BoxRuleSearch (tsch, ctxt)
PtrPSchema          tsch;
GenericContext      ctxt;
#endif /* !__STDC__ */
{
  int                 i;
  Name                boxname;

  BuildBoxName (ctxt, &boxname);
  
  /* search for the BOXE in the Presentation Schema */
  for (i = 1; i <= tsch->PsNPresentBoxes; i++)
    {
      if (!ustrcmp (ctxt->attrText[0], tsch->PsPresentBox[i - 1].PbName))
	{
	  ctxt->box = i;
	  return (tsch->PsPresentBox[i - 1].PbFirstPRule);
	}
    }

  ctxt->box = 0;
  return (NULL);
}

/*----------------------------------------------------------------------
   Function used to to search all specific presentation rules
   for a given type of rule associated to an element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule SearchElementPRule (PtrElement el, PRuleType type, unsigned int extra)
#else
static PtrPRule SearchElementPRule (el, type, extra)
PtrElement      el;
PRuleType       type;
unsigned int    extra;

#endif
{
    PtrPRule cur;
    
    cur = el->ElFirstPRule;

    while (cur != NULL)
      {
	/* shortcut : rules are sorted by type and view number */
	if (cur->PrType > type ||
	    (cur->PrType == type && cur->PrViewNum > 1) ||
	    (cur->PrType == type && type == PRFunction &&
	     cur->PrPresFunction > (FunctionType) extra))
	  {
	     cur = NULL;
	     break;
	  }
	
	/* check for extra specification in case of function rule */
	if (type == PRFunction && cur->PrPresFunction != (FunctionType) extra)
	  {
	    cur = cur->PrNextPRule;
	    continue;
	  }

	/* check this rule */
	if (type == cur->PrType)
	   break;

	/* jump to next and keep track of previous */
	cur = cur->PrNextPRule;
    }
    return (cur);
}

/*----------------------------------------------------------------------
   Function used to to add a specific presentation rule
   for a given type of rule associated to an element.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule  InsertElementPRule (PtrElement el, PRuleType type, unsigned int extra)
#else
static PtrPRule  InsertElementPRule (el, type, extra)
PtrElement       el;
PRuleType        type;
unsigned int     extra;

#endif
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
	  (cur->PrType == type && type == PtFunction && cur->PrPresFunction > (FunctionType) extra))
	 cur = NULL;
       else
	 {
	   /* last specific rule */
	   prev = cur;
	   if (cur->PrViewNum == 1 && cur->PrType == type &&
	       (type != PRFunction ||
	       /* check for extra specification in case of function rule */
	       (type == PRFunction && cur->PrPresFunction == (FunctionType) extra)))
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
	    stdRule = GlobalSearchRulepEl (el, &pSPR, &pSSR, 0, NULL, 1, type, extra, FALSE, TRUE, &pAttr);
	    if (stdRule != NULL)
	      /* copy the standard rule */
	      *pRule = *stdRule;
	    else
		pRule->PrType = type;
	    pRule->PrCond = NULL;
	    pRule->PrSpecifAttr = 0;
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
#ifdef __STDC__
static void  RemoveElementPRule (PtrElement el, PRuleType type, unsigned int extra)
#else
static void  RemoveElementPRule (el, type, extra)
PtrElement          el;
PRuleType           type;
unsigned int        extra;

#endif
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
	if ((type == PRFunction) && (cur->PrPresFunction != (FunctionType) extra))
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
    FreePresentRule(cur);
    return;
}

/*----------------------------------------------------------------------
  BoxRuleInsert looks in the array of boxes for an entry corresponding
  to the current context. If not found we add a new one to the array.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule    *BoxRuleInsert (PtrPSchema tsch, GenericContext ctxt)
#else  /* __STDC__ */
static PtrPRule    *BoxRuleInsert (tsch, ctxt)
PtrPSchema          tsch;
GenericContext      ctxt;
#endif /* !__STDC__ */
{
  PresentationBox    *box;
  int                 i;
  Name                boxname;

  BuildBoxName (ctxt, &boxname);

  /* search for the BOXE in the Presentation Schema */
  for (i = 1; i <= tsch->PsNPresentBoxes; i++)
    {
      if (!ustrcmp (boxname, tsch->PsPresentBox[i - 1].PbName))
	{
	  ctxt->box = i;
	  return (&tsch->PsPresentBox[i - 1].PbFirstPRule);
	}
    }

  if (tsch->PsNPresentBoxes >= MAX_PRES_BOX)
    {
      fprintf (stderr, "BoxRuleInsert: PsNPresentBoxes >= MAX_PRES_BOX (%d)\n", MAX_PRES_BOX);
      ctxt->box = 0;
      return (NULL);
    }
  
  /* allocate and initialize the new BOX */
  tsch->PsNPresentBoxes++;
  ctxt->box = tsch->PsNPresentBoxes;
  box = &tsch->PsPresentBox[tsch->PsNPresentBoxes - 1];
  ustrncpy (box->PbName, boxname, sizeof (box->PbName));
  box->PbFirstPRule = NULL;
  box->PbAcceptPageBreak = TRUE;
  box->PbAcceptLineBreak = TRUE;
  box->PbBuildAll = TRUE;
  box->PbPageFooter = FALSE;
  box->PbPageHeader = FALSE;
  box->PbPageBox = FALSE;
  box->PbFooterHeight = 0;
  box->PbHeaderHeight = 0;
  box->PbPageCounter = 0;
  box->PbContent = FreeContent;
  box->PbContElem = 0;
  box->PbContRefElem = 0;

  return (&tsch->PsPresentBox[tsch->PsNPresentBoxes - 1].PbFirstPRule);
}

/*----------------------------------------------------------------------
  PresConstInsert : add a constant to the constant array of a
  Presentation Schema and returns the associated index.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          PresConstInsert (PSchema tcsh, STRING value)
#else  /* __STDC__ */
static int          PresConstInsert (doc, value)
PSchema             tcsh;
STRING              value;
#endif /* !__STDC__ */
{
  PtrPSchema pSchemaPrs = (PtrPSchema) tcsh;
  int i;

  if (pSchemaPrs == NULL || value == NULL)
    return (-1);

  /* lookup the existing constants, searching for a corresponding entry */
  for (i = 0; i < pSchemaPrs->PsNConstants; i++)
    {
      if (pSchemaPrs->PsConstant[i].PdType == CharString &&
	  !ustrncmp (value, pSchemaPrs->PsConstant[i].PdString, MAX_PRES_CONST_LEN))
	return (i+1);
    }

  /* if not found, try to add it at the end */
  if (pSchemaPrs->PsNConstants >= MAX_PRES_CONST)
    return (-1);
  i = pSchemaPrs->PsNConstants;
  pSchemaPrs->PsConstant[i].PdType = CharString;
  pSchemaPrs->PsConstant[i].PdAlphabet = 'L';
  ustrncpy (&pSchemaPrs->PsConstant[i].PdString[0], value, MAX_PRES_CONST_LEN);
  pSchemaPrs->PsNConstants++;
  return(i+1);
}

/*----------------------------------------------------------------------
  CompareCond : defines an absolute order on conditions.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          CompareCond (PtrCondition c1, PtrCondition c2)
#else  /* __STDC__ */
static int          CompareCond (c1, c2)
PtrCondition        c1;
PtrCondition        c2;
#endif /* !__STDC__ */
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
    case PcAttribute:
      if (c1->CoTypeElAttr < c2->CoTypeElAttr)
	return (-1);
      if (c1->CoTypeElAttr > c2->CoTypeElAttr)
	return (+1);
      if (c1->CoTypeElAttr == 0)
	return (+1);
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
#ifdef __STDC__
static void         AddCond (PtrCondition * base, PtrCondition cond)
#else  /* __STDC__ */
static void         AddCond (base, cond)
PtrCondition       *base;
PtrCondition        cond;
#endif /* !__STDC__ */
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
   while (next != NULL)
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
#ifdef __STDC__
static void         PresRuleAddAncestorCond (PtrPRule rule, int type, int nr)
#else  /* __STDC__ */
static void         PresRuleAddAncestorCond (rule, type, nr)
PtrPRule            rule;
int                 type;
int                 nr;
#endif /* !__STDC__ */
{
   PtrCondition        cond = NULL;

   GetPresentRuleCond (&cond);
   memset (cond, 0, sizeof (Condition));
   if (cond == NULL)
     {
	TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
	return;
     }
   cond->CoCondition = PcWithin;
   cond->CoTarget = FALSE;
   cond->CoNotNegative = TRUE;
   /* as it's greater we register the number of ancestors - 1 */
   cond->CoRelation = nr - 1;
   cond->CoTypeAncestor = type;
   cond->CoImmediate = FALSE;
   cond->CoAncestorRel = CondGreater;
   cond->CoAncestorName[0] = EOS;
   cond->CoSSchemaName[0] = EOS;
   AddCond (&rule->PrCond, cond);
}

/*----------------------------------------------------------------------
  PresRuleAddAttrCond : add a Attr condition to a presentation rule.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PresRuleAddAttrCond (PtrPRule rule, int type)
#else  /* __STDC__ */
static void         PresRuleAddAttrCond (rule, type)
PtrPRule            rule;
int                 type;
#endif /* !__STDC__ */
{
   PtrCondition        cond = NULL;

   GetPresentRuleCond (&cond);
   memset (cond, 0, sizeof (Condition));
   if (cond == NULL)
     {
	TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
	return;
     }
   cond->CoCondition = PcElemType;
   cond->CoNotNegative = TRUE;
   cond->CoTarget = FALSE;
   cond->CoTypeElAttr = type;
   AddCond (&rule->PrCond, cond);
}

/*----------------------------------------------------------------------
  FirstPresAttrRuleSearch: look in the array of Attribute presentation
  blocks, for a block and a rule corresponding to the current context.
  When the rule is not found, attrblock points to the last block.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule    *FirstPresAttrRuleSearch (PtrPSchema tsch, int attrType, GenericContext ctxt, int att, AttributePres **attrblock)
#else  /* __STDC__ */
static PtrPRule    *FirstPresAttrRuleSearch (tsch, attrType, ctxt, att, attrblock)
PtrPSchema          tsch;
int                 attrType;
GenericContext      ctxt;
int                 att;
AttributePres     **attrblock;
#endif /* !__STDC__ */
{
  PtrPRule           *ppRule;
  PtrSSchema          pSS;
  AttributePres      *attrs;
  STRING              attrVal;
  unsigned int        elementType;
  int                 nbrules;
  int                 i, j, val;

  /* select the right attribute */
  attrs = tsch->PsAttrPRule[attrType - 1];
  *attrblock = NULL;
  nbrules = tsch->PsNAttrPRule[attrType - 1];
  pSS = (PtrSSchema) ctxt->schema;
  ppRule = NULL;
  attrVal = ctxt->attrText[att];
  elementType = ctxt->name[att];
  for (i = 0; i < nbrules && !ppRule && attrs; i++)
    {
      if ((att > 0 && attrs->ApElemType != ctxt->type) ||
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
	  switch (pSS->SsAttribute[attrType].AttrType)
	    {
	    case AtNumAttr:
	      if (attrVal)
		usscanf (attrVal, TEXT("%d"), &val);
	      else
		val = 0;
	      if (val)
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
	      break;
	    case AtTextAttr:
	      if (attrVal && !ustrcmp (attrs->ApString, attrVal))
		ppRule = &(attrs->ApTextFirstPRule);
	      else if (!attrVal && attrs->ApString[0] == EOS)
		ppRule = &(attrs->ApTextFirstPRule);
	      break;
	    case AtReferenceAttr:
	      ppRule = &(attrs->ApRefFirstPRule);
	      break;
	    case AtEnumAttr:
	      if (attrVal)
		usscanf (attrVal, TEXT("%d"), &val);
	      else
		val = 0;
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
#ifdef __STDC__
static PtrPRule    *PresAttrChainInsert (PtrPSchema tsch, int attrType, GenericContext ctxt, int att)
#else  /* __STDC__ */
static PtrPRule    *PresAttrChainInsert (tsch, attrType, ctxt, att)
PtrPSchema          tsch;
int                 attrType;
GenericContext      ctxt;
int                 att;
#endif /* !__STDC__ */
{
  AttributePres      *attrs, *new;
  PtrSSchema          pSS;
  PtrPRule           *ppRule;
  STRING              attrVal;
  int                 nbrules, val;

  pSS = (PtrSSchema) ctxt->schema;
  ppRule = FirstPresAttrRuleSearch (tsch, attrType, ctxt, att, &attrs);
  /* If no attribute presentation rule is found, create and initialize it */
  if (!ppRule)
    {
      /* select the last entry */
      nbrules = tsch->PsNAttrPRule[attrType - 1] + 1;

      /* add the new entry */
      GetAttributePres (&new);
      tsch->PsNAttrPRule[attrType - 1] = nbrules;
      if (att > 0 && ctxt->type)
	{
	new->ApElemType = ctxt->type;
	pSS->SsPSchema->PsNHeirElems[attrType - 1] += 1;
	}
      if (attrs)
	{
	  new->ApNextAttrPres = attrs->ApNextAttrPres;
	  attrs->ApNextAttrPres = new;
	}
      else
	{
	  new->ApNextAttrPres = tsch->PsAttrPRule[attrType - 1];
	  tsch->PsAttrPRule[attrType - 1] = new;
	}

      attrVal = ctxt->attrText[att];
      switch (pSS->SsAttribute[attrType].AttrType)
	{
	case AtNumAttr:
	  new->ApNCases = 1;
	  if (attrVal)
	    usscanf (attrVal, TEXT("%d"), &val);
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
	  if (attrVal)
	    ustrcpy (new->ApString, attrVal);
	  else
	    attrs->ApString[0] = EOS;
	  new->ApTextFirstPRule = NULL;
	  return (&(new->ApTextFirstPRule));
	  break;
	case AtReferenceAttr:
	  new->ApRefFirstPRule = NULL;
	  return (&(new->ApRefFirstPRule));
	  break;
	case AtEnumAttr:
	  if (attrVal)
	    usscanf (attrVal, TEXT("%d"), &val);
	  else
	    val = 0;
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
  TstRuleContext : test if a presentation rule correpond to the
  context given in argument and the correct presentation rule type.
  All the rules in a rule' list are sorted :
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
#ifdef __STDC__
static int      TstRuleContext (PtrPRule rule, GenericContext ctxt, PRuleType pres, unsigned int att)
#else  /* __STDC__ */
static int      TstRuleContext (rule, ctxt, pres, att)
PtrPRule        rule;
GenericContext  ctxt;
PRuleType       pres;
unsigned int    att;
#endif /* !__STDC__ */
{
  PtrCondition        firstCond, cond;
  int                 i, nbcond;

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

   if (att < MAX_ANCESTORS)
     {
       /* the rule is associated to an attribute */
       /* test if the element type is within the rule conditions */
       if (ctxt->name[att] &&
	   (nbcond != 1 ||
	    firstCond->CoCondition != PcElemType ||
	    firstCond->CoTypeElAttr != (int) ctxt->name[att] ||
	    (att == 0 && firstCond->CoTypeElAttr != (int) ctxt->type)))
	 /* it's not the right rule */
	 return (1);
       else if (nbcond != 0)
	 /* it's not the right rule: there are too many conditions in the rule */
	 return (-1);
     }
   /* check if all ancestors are within the rule conditions */
   i = 0;
   while (i < MAX_ANCESTORS && ctxt->name[i] != 0)
     {
       if (ctxt->names_nb[i] > 0 && i != att)
	 {
	   cond = firstCond;
	   while (cond &&
		  (cond->CoCondition != PcWithin ||
		   cond->CoTypeAncestor != ctxt->names_nb[i] ||
		   cond->CoRelation != ctxt->names_nb[i] - 1))
	     cond = cond->CoNextCondition;
	   if (cond == NULL)
	     /* the ancestor is not found */
	     return (1);
	 }
       i++;
     }
   if (i != nbcond + 1)
     /* it's not the right rule: there are too many conditions in the rule */
     return (-1);
   return (0);
}

/*----------------------------------------------------------------------
  PresRuleSearch : search a presentation rule for a given view
  in an attribute chain or an element chain.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule     PresRuleSearch (PtrPSchema tsch, GenericContext ctxt, PRuleType pres, unsigned int extra, PtrPRule **chain)
#else  /* __STDC__ */
static PtrPRule     PresRuleSearch (tsch, ctxt, pres, extra, chain)
PtrPSchema          tsch;
GenericContext      ctxt;
PRuleType           pres;
unsigned int        extra;
PtrPRule          **chain;
#endif /* !__STDC__ */
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
	*chain = &tsch->PsElemPRule[ctxt->type - 1];
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
	       (pres == PtFunction && pRule->PrPresFunction !=  (FunctionType) extra))
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
#ifdef __STDC__
static PtrPRule     PresRuleInsert (PtrPSchema tsch, GenericContext ctxt, PRuleType pres, unsigned int extra)
#else  /* __STDC__ */
static PtrPRule     PresRuleInsert (tsch, ctxt, pres, extra)
PtrPSchema          tsch;
GenericContext      ctxt;
PRuleType           pres;
unsigned int        extra;
#endif /* !__STDC__ */
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
	  pRule->PrSpecifAttrSSchema = NULL;
      
	  /* In case of an attribute rule, add the Attr condition */
	  att = 0;
	  while (att < MAX_ANCESTORS && ctxt->attrType[att] == 0)
	    att++;
	  if (att == 0 && ctxt->type)
	    /* the attribute should be attached to that element */
	    PresRuleAddAttrCond (pRule, ctxt->type);
	  else if (att < MAX_ANCESTORS && ctxt->name[att])
	    /* the attribute should be attached to that element */
	    PresRuleAddAttrCond (pRule, ctxt->name[att]);
	  /* add the ancesters conditions ... */
	  i = 0;
	  while (i < MAX_ANCESTORS && ctxt->name[i] != 0)
	    {
	      if (ctxt->names_nb[i] > 0 && i != att)
		PresRuleAddAncestorCond (pRule, ctxt->name[i], ctxt->names_nb[i]);
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
#ifdef __STDC__
static void     PresRuleRemove (PtrPSchema tsch, GenericContext ctxt, PRuleType pres, unsigned int extra)
#else  /* __STDC__ */
static void     PresRuleRemove (tsch, ctxt, pres, extra)
PtrPSchema          tsch;
GenericContext      ctxt;
PRuleType           pres;
unsigned int        extra;
#endif /* !__STDC__ */
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
      FreePresentRule(cur);
    }
}

/*----------------------------------------------------------------------
 PresentationValueToPRule : set up an internal Presentation Rule accordingly
 to a Presentation Value for a given type of presentation attribute.
 funcType is an extra parameter needed when using a Function rule.
 ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PresentationValueToPRule (PresentationValue val, int type, PtrPRule rule, int funcType, ThotBool absolute, ThotBool generic)
#else
static void         PresentationValueToPRule (val, type, rule, funcType, absolute, generic)
PresentationValue   val;
int                 type;
PtrPRule            rule;
int                 funcType;
ThotBool            absolute;
ThotBool            generic;
#endif
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
  switch (unit)
    {
    case STYLE_UNIT_REL:
      int_unit = UnRelative;
      if (type == PtBreak1 ||
	  type == PtBreak2 ||
	  type == PtIndent ||
	  type == PtLineSpacing ||
	  type == PtLineWeight)
	value *= 10;
      break;
    case STYLE_UNIT_EM:
      int_unit = UnRelative;
      value *= 10;
      break;
    case STYLE_UNIT_PT:
      int_unit = UnPoint;
      break;
    case STYLE_UNIT_PC:
      int_unit = UnPoint;
      value *= 12;
      break;
    case STYLE_UNIT_IN:
      int_unit = UnPoint;
      value *= 72;
      break;
    case STYLE_UNIT_CM:
      int_unit = UnPoint;
      value *= 28;
      break;
    case STYLE_UNIT_MM:
      int_unit = UnPoint;
      value *= 28;
      value /= 10;
      break;
    case STYLE_UNIT_PX:
      int_unit = UnPixel;
      break;
    case STYLE_UNIT_PERCENT:
      int_unit = UnPercent;
      break;
    case STYLE_UNIT_XHEIGHT:
      int_unit = UnXHeight;
      value *= 10;
      break;
    case STYLE_UNIT_BOX:
      int_unit = UnRelative;	/* unused */
      break;
    default:
      int_unit = UnRelative;
      break;
    }

  if (real)
    value /= 1000;

  /* now, set-up the value */
  switch (type)
    {
    case PtVisibility:
    case PtDepth:
    case PtFillPattern:
    case PtBackground:
    case PtForeground:
      rule->PrPresMode = PresImmediate;
      rule->PrAttrValue = 0;
      rule->PrIntValue = value;
      break;
    case PtFont:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case STYLE_FONT_HELVETICA:
	  rule->PrChrValue = 'H';
	  break;
	case STYLE_FONT_TIMES:
	  rule->PrChrValue = 'T';
	  break;
	case STYLE_FONT_COURIER:
	  rule->PrChrValue = 'C';
	  break;
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
    case PtLineStyle:
      break;
    case PtBreak1:
    case PtBreak2:
    case PtIndent:
    case PtLineSpacing:
    case PtLineWeight:
      rule->PrPresMode = PresImmediate;
      rule->PrMinUnit = int_unit;
      rule->PrMinValue = value;
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
    case PtJustify:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case STYLE_JUSTIFIED:
	  rule->PrJustify = TRUE;
	  break;
	case STYLE_NOTJUSTIFIED:
	  rule->PrJustify = FALSE;
	  break;
	}
      break;
    case PtAdjust:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case STYLE_ADJUSTLEFT:
	  rule->PrAdjust = AlignLeft;
	  break;
	case STYLE_ADJUSTRIGHT:
	  rule->PrAdjust = AlignRight;
	  break;
	case STYLE_ADJUSTCENTERED:
	  rule->PrAdjust = AlignCenter;
	  break;
	case STYLE_ADJUSTLEFTWITHDOTS:
	  rule->PrAdjust = AlignLeftDots;
	  break;
	default:
	  rule->PrAdjust = AlignLeft;
	  break;
	}
      break;
    case PtHyphenate:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case STYLE_HYPHENATE:
	  rule->PrJustify = TRUE;
	  break;
	case STYLE_NOHYPHENATE:
	  rule->PrJustify = FALSE;
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
      rule->PrPosRule.PoDistUnit = int_unit;
      rule->PrPosRule.PoDistance = value;
      if (generic)
	{
	  /* generate a complete rule Top=Previous AnyElem.Bottom+value */
	  rule->PrPosRule.PoPosDef = Top;
	  rule->PrPosRule.PoPosRef = Bottom;
	  rule->PrPosRule.PoRelation = RlPrevious;
	  rule->PrPosRule.PoNotRel = FALSE;
	  rule->PrPosRule.PoRefKind = RkElType;
	  rule->PrPosRule.PoUserSpecified = FALSE;
	  rule->PrPosRule.PoRefIdent = MAX_RULES_SSCHEMA + 1;	  
	}
      break;
    case PtHorizPos:
      rule->PrPresMode = PresImmediate;
      rule->PrPosRule.PoDistUnit = int_unit;
      rule->PrPosRule.PoDistance = value;
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
      rule->PrDimRule.DrUnit = int_unit;
      rule->PrDimRule.DrAttr = FALSE;
      if (generic)
	{
	  /* generate a complete rule Height=Enclosed.Height */
	  rule->PrDimRule.DrPosition = FALSE;
	  rule->PrDimRule.DrAbsolute = FALSE;
	  rule->PrDimRule.DrSameDimens = TRUE;
	  rule->PrDimRule.DrRelation = RlEnclosed;
	  rule->PrDimRule.DrNotRelat = FALSE;
	  rule->PrDimRule.DrRefKind = RkElType;
	  rule->PrDimRule.DrUserSpecified = FALSE;
	  rule->PrDimRule.DrRefIdent = 0;	  
	}
      if (absolute)
	{
	  rule->PrDimRule.DrAbsolute = TRUE;
	  rule->PrDimRule.DrValue = value;
	}
      else
	rule->PrDimRule.DrValue = 0;
      break;
    case PtWidth:
      rule->PrPresMode = PresImmediate;
      rule->PrDimRule.DrUnit = int_unit;
      rule->PrDimRule.DrAttr = FALSE;
      if (generic)
	{
	  /* generate a complete rule Width=Enclosing.Width+value */
	  rule->PrDimRule.DrPosition = FALSE;
	  rule->PrDimRule.DrAbsolute = FALSE;
	  rule->PrDimRule.DrSameDimens = TRUE;
	  rule->PrDimRule.DrRelation = RlEnclosing;
	  rule->PrDimRule.DrNotRelat = FALSE;
	  rule->PrDimRule.DrRefKind = RkElType;
	  rule->PrDimRule.DrUserSpecified = FALSE;
	  rule->PrDimRule.DrRefIdent = 0;	  
	}
      if (absolute)
	{
	rule->PrDimRule.DrAbsolute = TRUE;
	rule->PrDimRule.DrValue = value;
	}
      else if (int_unit == UnPercent)
	rule->PrDimRule.DrValue = 100 - value;
      else
	rule->PrDimRule.DrValue = -value;
      break;
    case PtPictInfo:
      break;
    case PtVertOverflow:
    case PtHorizOverflow:
      rule->PrPresMode = PresImmediate;
      rule->PrJustify = TRUE;
      break;
    case PtFunction:
      rule->PrPresMode = PresFunction;
      switch (funcType)
	{
	case FnLine:
	  if (value == STYLE_NOTINLINE)
	    rule->PrPresFunction = FnNoLine;
	  else
	    rule->PrPresFunction = FnLine;
	  rule->PrNPresBoxes = 0;
	  break;
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
	    case STYLE_REALSIZE:
	      rule->PrPresBox[0] = RealSize;
	      break;
	    case STYLE_SCALE:
	      rule->PrPresBox[0] = ReScale;
	      break;
	    case STYLE_REPEAT:
	      rule->PrPresBox[0] = FillFrame;
	      break;
	    case STYLE_HREPEAT:
	      rule->PrPresBox[0] = XRepeat; break;
	    case STYLE_VREPEAT:
	      rule->PrPresBox[0] = YRepeat; break;
	    default:
	      rule->PrPresBox[0] = RealSize;
	    }
	  break;
	}
      break;
    }
}

/*----------------------------------------------------------------------
  PRuleToPresentationValue : return the PresentationValue corresponding to
  a given rule.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PresentationValue   PRuleToPresentationValue (PtrPRule rule)
#else
static PresentationValue   PRuleToPresentationValue (rule)
PtrPRule                   rule;
#endif
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
      value = rule->PrIntValue;
      break;
    case PtFont:
      switch (rule->PrChrValue)
	{
	case 'H':
	  value = STYLE_FONT_HELVETICA;
	  break;
	case 'T':
	  value = STYLE_FONT_TIMES;
	  break;
	case 'C':
	  value = STYLE_FONT_COURIER;
	  break;
	}
      break;
    case PtStyle:
      switch (rule->PrChrValue)
	{
	case 'R':
	  value = STYLE_FONT_ROMAN;
	  break;
	case 'I':
	  value = STYLE_FONT_ITALICS;
	  break;
	case 'O':
	  value = STYLE_FONT_OBLIQUE;
	  break;
	default:
	  value = STYLE_FONT_ROMAN;
	  break;
	}
      break;
    case PtWeight:
      switch (rule->PrChrValue)
	{
	case 'B':
	  value = STYLE_WEIGHT_BOLD;
	  break;
	case 'N':
	  value = STYLE_WEIGHT_NORMAL;
	  break;
	default:
	  value = STYLE_WEIGHT_NORMAL;
	  break;
	}
      break;
    case PtUnderline:
      switch (rule->PrChrValue)
	{
	case 'U':
	  value = STYLE_UNDERLINE;
	  break;
	case 'O':
	  value = STYLE_OVERLINE;
	  break;
	case 'C':
	  value = STYLE_CROSSOUT;
	  break;
	}
      break;
    case PtThickness:
    case PtLineStyle:
      break;
    case PtBreak1:
    case PtBreak2:
    case PtIndent:
    case PtLineSpacing:
    case PtLineWeight:
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
    case PtJustify:
      if (rule->PrJustify)
	value = STYLE_JUSTIFIED;
      else
	value = STYLE_NOTJUSTIFIED;
      break;
    case PtAdjust:
      switch (rule->PrAdjust)
	{
	case AlignLeft:
	  value = STYLE_ADJUSTLEFT;
	  break;
	case AlignRight:
	  value = STYLE_ADJUSTRIGHT;
	  break;
	case AlignCenter:
	  value = STYLE_ADJUSTCENTERED;
	  break;
	case AlignLeftDots:
	  value = STYLE_ADJUSTLEFTWITHDOTS;
	  break;
	default:
	  value = STYLE_ADJUSTLEFT;
	  break;
	}
      break;
    case PtHyphenate:
      if (rule->PrJustify)
	value = STYLE_HYPHENATE;
      else
	value = STYLE_NOHYPHENATE;
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
    case PtPictInfo:
    case PtVertOverflow:
    case PtHorizOverflow:
      break;
    case PtFunction:
      switch (rule->PrPresFunction)
	{
	case FnLine:
	  value = STYLE_INLINE;
	  break;
	case FnNoLine:
	  value = STYLE_NOTINLINE;
	  break;
	case FnCreateBefore:
	case FnCreateWith:
	case FnCreateFirst:
	case FnCreateLast:
	case FnCreateAfter:
	case FnCreateEnclosing:
	  value = rule->PrNPresBoxes;
	  unit = STYLE_UNIT_BOX;
	  break;
	case FnShowBox:
	  value = rule->PrNPresBoxes;
	  unit = STYLE_UNIT_REL;
	  break;
	case FnBackgroundPicture:
	  value = rule->PrPresBox[0];
	  unit = STYLE_UNIT_REL;
	  break;
	case FnPictureMode:
	  unit = STYLE_UNIT_REL;
	  value = STYLE_REALSIZE;
	  switch (rule->PrPresBox[0])
	    {
	    case RealSize:
	      value = STYLE_REALSIZE; break;
	    case ReScale:
	      value = STYLE_SCALE; break;
	    case FillFrame:
	      value = STYLE_REPEAT; break;
	    case XRepeat:
	      value = STYLE_HREPEAT; break;
	    case YRepeat:
	      value = STYLE_VREPEAT; break;
	    default:
	      unit = STYLE_UNIT_INVALID;
	      value = 0;
	    }
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
	unit = STYLE_UNIT_REL;
      else
	unit = STYLE_UNIT_EM;
      value /= 10;
      break;
    case UnXHeight:
      value /= 10;
      unit = STYLE_UNIT_XHEIGHT;
      break;
    case UnPoint:
      unit = STYLE_UNIT_PT;
      break;
    case UnPixel:
      unit = STYLE_UNIT_PX;
      break;
    case UnPercent:
      unit = STYLE_UNIT_PERCENT;
      break;
    default:
      unit = STYLE_UNIT_INVALID;
      break;
    }

  if (value % 10 && unit != STYLE_UNIT_INVALID)
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
#ifdef __STDC__
static void         TypeToPresentation (unsigned int type, PRuleType *intRule, unsigned int *func, ThotBool *absolute)
#else
static void         TypeToPresentation (type, intRule, func, absolute)
unsiged int         type;
PRuleType          *intRule;
unsiged int        *func;
ThotBool           *absolute;
#endif
{
  *func = 0;
  *absolute = FALSE;
  switch (type)
    {
    case PRVisibility:
      *intRule = PtVisibility;
      break;
    case PRForeground:
      *intRule = PtForeground;
      break;
    case PRBackground:
      *intRule = PtBackground;
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
    case PRAdjust:
      *intRule = PtAdjust;
      break;
    case PRIndent:
      *intRule = PtIndent;
      break;
    case PRJustify:
      *intRule = PtJustify;
      break;
    case PRHyphenate:
      *intRule = PtHyphenate;
      break;
    case PRUnderline:
      *intRule = PtUnderline;
      break;
    case PRFillPattern:
      *intRule = PtFillPattern;
      break;
    case PRFont:
      *intRule = PtFont;
      break;
    case PRLineSpacing:
      *intRule = PtLineSpacing;
      break;
    case PRVertPos:
      *intRule = PtVertPos;
      break;
    case PRHorizPos:
      *intRule = PtHorizPos;
      break;
    case PRWidth:
      *intRule = PtWidth;
      *absolute = TRUE;
      break;
    case PRHeight:
      *intRule = PtHeight;
      *absolute = TRUE;
      break;
    case PRTMargin:
      *intRule = PtVertPos;
      break;
    case PRLMargin:
      *intRule = PtHorizPos;
      break;
    case PRBMargin:
      *intRule = PtHeight;
      break;
    case PRRMargin:
      *intRule = PtWidth;
      break;
    case PRHorizOverflow:
      *intRule = PtHorizOverflow;
      break;
    case PRVertOverflow:
      *intRule = PtVertOverflow;
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
    case PRLine:
      *intRule = PtFunction;
      *func = FnLine;
      break;
    default:
      *intRule = PtFunction;
    }
}

/*----------------------------------------------------------------------
  TtaSetStylePresentation attachs a style rule to an element or to an
  extended presentation schema.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaSetStylePresentation (unsigned int type, Element el, PSchema tsch, PresentationContext c, PresentationValue v)
#else
int                 TtaSetStylePresentation (type, el, tsch, c, v)
unsiged int         type;
Element             el;
PSchema             tsch;
PresentationContext c;
PresentationValue   v;
#endif
{
  GenericContext    ctxt = (GenericContext) c;
  PtrPRule          pRule;
  PRuleType         intRule;
  unsigned int      func = 0;
  int               cst = 0;
  int               i;
  int               attrType;
  int               doc = c->doc;
  ThotBool          absolute, generic;

  TypeToPresentation (type, &intRule, &func, &absolute);
  generic = (el == NULL);
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
	pRule = InsertElementPRule ((PtrElement) el, intRule, func);

      if (pRule)
	{
	  if (type == PRBackgroundPicture)
	    {
	      if (!generic)
		tsch = (PSchema) (LoadedDocument[doc - 1]->DocSSchema->SsPSchema);
	      cst = PresConstInsert (tsch, v.pointer);
	      v.typed_data.unit = STYLE_UNIT_REL;
	      v.typed_data.value = cst;
	      v.typed_data.real = FALSE;
	    }
	  PresentationValueToPRule (v, intRule, pRule, func, absolute, generic);
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
		  ApplyAGenericStyleRule (doc, (PtrSSchema) ctxt->schema, ctxt->type, attrType, ctxt->box, pRule, FALSE);
		}
	    }
	  else
	    ApplyASpecificStyleRule (pRule, (PtrElement) el, LoadedDocument[doc - 1], FALSE);
	}
    }
  return (0);
}

/*----------------------------------------------------------------------
  TtaGetStylePresentation returns the style rule attached to an element
  or to an extended presentation schema.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetStylePresentation (unsigned int type, Element el, PSchema tsch, PresentationContext c, PresentationValue *v)
#else
int                 TtaGetStylePresentation (type, el, tsch, c, v)
unsigned int        type;
Element             el;
PSchema             tsch;
PresentationContext c;
PresentationValue  *v;
#endif
{
  PtrPRule          rule, *chain;
  PRuleType         intRule;
  unsigned int      func;
  int               cst;
  ThotBool          absolute, generic;

  TypeToPresentation (type, &intRule, &func, &absolute);
  generic = (el == NULL);
  if (generic)
    rule = PresRuleSearch ((PtrPSchema) tsch, (GenericContext) c, intRule, func, &chain);
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
#ifdef __STDC__
static void         PRuleToPresentationSetting (PtrPRule rule, PresentationSetting setting, int extra)
#else
static void         PRuleToPresentationSetting (rule, setting, extra)
PtrPRule            rule;
PresentationSetting setting;
int                 extra;
#endif
{
  /* first decoding step : analyze the type of the rule */
  switch (rule->PrType)
    {
    case PtVisibility:
      setting->type = PRVisibility;
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
    case PtSize:
      setting->type = PRSize;
      break;
    case PtUnderline:
      setting->type = PRUnderline;
      break;
    case PtIndent:
      setting->type = PRIndent;
      break;
    case PtLineSpacing:
      setting->type = PRLineSpacing;
      break;
    case PtJustify:
      setting->type = PRJustify;
      break;
    case PtAdjust:
      setting->type = PRAdjust;
      break;
    case PtHyphenate:
      setting->type = PRHyphenate;
      break;
    case PtFillPattern:
      setting->type = PRFillPattern;
      break;
    case PtBackground:
      setting->type = PRBackground;
      break;
    case PtForeground:
      setting->type = PRForeground;
      break;
    case PtVertPos:
      setting->type = PRTMargin;
      break;
    case PtHorizPos:
      setting->type = PRLMargin;
      break;
    case PtHeight:
      setting->type = PRHeight;
      break;
    case PtWidth:
      setting->type = PRWidth;
      break;
    case PtFunction:
      switch (extra)
	{
	case FnLine:
	  setting->type = PRLine;
	  break;
	case FnShowBox:
	  setting->type = PRShowBox;
	  break;
	case FnBackgroundPicture:
	  setting->type = PRBackgroundPicture;
	  break;
	case FnPictureMode:
	  setting->type = PRPictureMode;
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
#ifdef __STDC__
GenericContext      TtaGetGenericStyleContext (Document doc)
#else  /* __STDC__ */
GenericContext      TtaGetGenericStyleContext (doc)
Document            doc;
#endif /* __STDC__ */
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
  GetSpecificStyleContext : user level function needed to allocate and
  initialize a SpecificContext.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PresentationContext     TtaGetSpecificStyleContext (Document doc)
#else  /* __STDC__ */
PresentationContext     TtaGetSpecificStyleContext (doc)
Document                doc;
#endif /* __STDC__ */
{
   PresentationContext     ctxt;

   ctxt = (PresentationContext) TtaGetMemory (sizeof (PresentationContextBlock));
   if (ctxt == NULL)
      return (NULL);
   ctxt->doc = doc;
   ctxt->schema = TtaGetDocumentSSchema (doc);
   ctxt->destroy = 0;
   return (ctxt);
}

/*----------------------------------------------------------------------
  Function used to remove all presentation for a given element or an
  extended presentation schema
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaCleanStylePresentation (Element el, PSchema tsch, Document doc)
#else
void                TtaCleanStylePresentation (el, tsch, doc)
Element             el;
PSchema             tsch;
Document            doc;
#endif
{
  PRule               rule;
  PtrPRule            pRule = NULL;
  PtrSSchema	      pSS;
  AttributePres      *attrs;
  DisplayMode         dispMode;
  int                 nbrules, i, j;
  unsigned int        elType, attrType, max;

  if (doc == 0)
    return;

  /* avoid too many redisplay */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  if (el != NULL)
    {
      do
	{
	  rule = (PRule) ((PtrElement) el)->ElFirstPRule;
	  while (rule)
	    {
	      TtaRemovePRule (el, rule, doc);
	      TtaNextPRule (el, &rule);
	    }
	}
      while (rule != NULL);
    }
  else
    {
      pSS = LoadedDocument[doc - 1]->DocSSchema;
      /* remove all element rules */
      max = (unsigned int) pSS->SsNRules;
      for (elType = 0; elType < max; elType++)
	{
	  pRule = ((PtrPSchema) tsch)->PsElemPRule[elType];
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
	  attrs = ((PtrPSchema) tsch)->PsAttrPRule[attrType];
	  nbrules = ((PtrPSchema) tsch)->PsNAttrPRule[attrType];
	  for (i = 0; i < nbrules; i++)
	    {
	      switch (pSS->SsAttribute[attrType].AttrType)
		{
		case AtNumAttr:
		  for (j = 0; j < attrs->ApNCases; j++)
		    {
		      pRule = attrs->ApCase[j].CaFirstPRule;
		      while (pRule != NULL)
			{
			  ApplyAGenericStyleRule (doc, pSS, 0, attrType+1, 0, pRule, TRUE);
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
		  for (j = 0; j < pSS->SsAttribute[attrType].AttrNEnumValues; j++)
		    {
		      pRule = attrs->ApEnumFirstPRule[j];
		      while (pRule != NULL)
			{
			  ApplyAGenericStyleRule (doc, pSS, 0, attrType, 0, pRule, TRUE);
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
#ifdef __STDC__
void                 TtaApplyAllSpecificSettings (Element el, Document doc, SettingsApplyHandler handler, void *param)
#else  /* __STDC__ */
void                 TtaApplyAllSpecificSettings (el, doc, handler, param)
Element              el;
Document             doc;
SettingsApplyHandler handler;
void                *param;

#endif /* __STDC__ */
{
  PtrPRule                 rule;
  PresentationSettingBlock setting;
  PtrPSchema               pSc1;
  int                      cst;

  if (el == NULL)
    return;
  rule = ((PtrElement) el)->ElFirstPRule;
  /*
   * for each rule corresponding to the same context i.e. identical
   * conditions, create the corresponding PresentationSetting and
   * call the user handler.
   */
  while (rule != NULL)
    {
      /* fill in the PresentationSetting and call the handler */
      if (rule->PrPresMode == PresFunction)
	PRuleToPresentationSetting (rule, &setting, rule->PrPresFunction);
      else
	PRuleToPresentationSetting (rule, &setting, 0);

      /* need to do some tweaking in the case of BackgroudPicture */
      if (setting.type == PRBackgroundPicture)
	{
	  cst = setting.value.typed_data.value;
	  pSc1 = LoadedDocument[doc - 1]->DocSSchema->SsPSchema;
	  setting.value.pointer = &pSc1->PsConstant[cst-1].PdString[0];
	}

	handler (el, doc, &setting, param);
	rule = rule->PrNextPRule;
     }
}
