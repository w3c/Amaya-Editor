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

#include "ustring.h"
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

#include "presentdriver.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"

#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "memory_f.h"
#include "presentdriver_f.h"

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
  CHAR                buffer[100];

  buffer[0] = 0;
  len = 0;
  for (i = 0; i < MAX_ANCESTORS; i++)
    {
      if (ctxt->ancestors[i] == 0)
	break;
      if (ctxt->ancestors_nb[i] > 1)
	sprintf (&buffer[len], "%d:%d/", ctxt->ancestors[i], ctxt->ancestors_nb[i]);
      else
	sprintf (&buffer[len], "%d/", ctxt->ancestors[i]);
      len = ustrlen (buffer);
    }
  if (ctxt->type)
    sprintf (&buffer[len], "%d,", ctxt->type);
  len = ustrlen (buffer);
  if (ctxt->attr)
    sprintf (&buffer[len], "%d:%d,", ctxt->attr, ctxt->attrval);
  len = ustrlen (buffer);
  if (ctxt->class)
    sprintf (&buffer[len], "%d.%s,", ctxt->classattr, ctxt->class);
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
  int                 i, j, tmp, nb_ancestors;
  Name                boxname;

  /* first sort the ancestors list */
  for (i = 0; i < MAX_ANCESTORS; i++)
    if (ctxt->ancestors[i] == 0)
      break;
  nb_ancestors = i;
  for (i = 0; i < nb_ancestors; i++)
    for (j = i + 1; j < nb_ancestors; j++)
      if (ctxt->ancestors[i] > ctxt->ancestors[j])
	{
	  tmp = ctxt->ancestors[i];
	  ctxt->ancestors[i] = ctxt->ancestors[j];
	  ctxt->ancestors[j] = tmp;
	  tmp = ctxt->ancestors_nb[i];
	  ctxt->ancestors_nb[i] = ctxt->ancestors_nb[j];
	  ctxt->ancestors_nb[j] = tmp;
	}
  
  BuildBoxName (ctxt, &boxname);
  
  /* search for the BOXE in the Presentation Schema */
  for (i = 1; i <= tsch->PsNPresentBoxes; i++)
    {
      if (!ustrcmp (ctxt->class, tsch->PsPresentBox[i - 1].PbName))
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

    if (pRule == NULL)
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

    while (cur != NULL) {
	/* shortcut : rules are sorted by type and view number */
	if ((cur->PrType > type) ||
	    ((cur->PrType == type) && (cur->PrViewNum > 1)) ||
	    (((cur->PrType == type) && (type == PRFunction) &&
	     (cur->PrPresFunction > (FunctionType) extra))))
	  {
	     cur = NULL;
	     break;
	  }
	
	/* check for extra specification in case of function rule */
	if ((type == PRFunction) && (cur->PrPresFunction != (FunctionType) extra)) {
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
    doc = TtaGetDocument ((Element)el);
    ApplyPRulesElement (cur, el, LoadedDocument[doc -1], TRUE);

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
  int                 i, j, tmp, nb_ancestors;
  Name                boxname;

  /* first sort the ancestors list */
  for (i = 0; i < MAX_ANCESTORS; i++)
    if (ctxt->ancestors[i] == 0)
      break;
  nb_ancestors = i;
  for (i = 0; i < nb_ancestors; i++)
    for (j = i + 1; j < nb_ancestors; j++)
      if (ctxt->ancestors[i] > ctxt->ancestors[j])
	{
	  tmp = ctxt->ancestors[i];
	  ctxt->ancestors[i] = ctxt->ancestors[j];
	  ctxt->ancestors[j] = tmp;
	  tmp = ctxt->ancestors_nb[i];
	  ctxt->ancestors_nb[i] = ctxt->ancestors_nb[j];
	  ctxt->ancestors_nb[j] = tmp;
	}
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
static int          PresConstInsert (PSchema tcsh, char *value)
#else  /* __STDC__ */
static int          PresConstInsert (doc, value)
PSchema             tcsh;
char               *value;
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
	  !strncmp (value, pSchemaPrs->PsConstant[i].PdString, MAX_PRES_CONST_LEN))
	return (i+1);
    }

  /* if not found, try to add it at the end */
  if (pSchemaPrs->PsNConstants >= MAX_PRES_CONST)
    return (-1);
  i = pSchemaPrs->PsNConstants;
  pSchemaPrs->PsConstant[i].PdType = CharString;
  pSchemaPrs->PsConstant[i].PdAlphabet = 'L';
  strncpy (&pSchemaPrs->PsConstant[i].PdString[0], value, MAX_PRES_CONST_LEN);
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
  CompareCondLists : Compare lists of conditions, we expect these
  lists to be sorted.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          CompareCondLists (PtrCondition c1, PtrCondition c2)
#else  /* __STDC__ */
static int          CompareCondLists (c1, c2)
PtrCondition        c1;
PtrCondition        c2;
#endif /* !__STDC__ */
{
  int                 res;

  do
    {
      if (c1 == c2)
	return (0);
      if (c1 == NULL)
	return (-1);
      if (c2 == NULL)
	return (+1);
      res = CompareCond (c1, c2);
      if (res != 0)
	return (res);
      c1 = c1->CoNextCondition;
      c2 = c2->CoNextCondition;
    }
  while (1);
  /* NOTREACHED */
  return (0);
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
  SortConds : sort the conditions rules in a presentation rule.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SortConds (PtrPRule rule)
#else  /* __STDC__ */
static void         SortConds (rule)
PtrPRule            rule;
#endif /* !__STDC__ */
{
   PtrCondition        cour = rule->PrCond;
   PtrCondition        next;

   if (cour == NULL)
     {
	return;
     }
   next = cour->CoNextCondition;
   while (next != NULL)
     {
	if (CompareCond (cour, next) > 0)
	  {
	     /* unlink next, and reinsert it onto the queue */
	     cour->CoNextCondition = next->CoNextCondition;
	     next->CoNextCondition = NULL;
	     AddCond (&rule->PrCond, next);
	     next = cour->CoNextCondition;
	     if (next == NULL)
		return;
	  }

	/* skip to next */
	cour = next;
	next = cour->CoNextCondition;
     }
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
   cond->CoRelation = nr;
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
  PresAttrsRuleSearch : look in the array of Attribute presentation
  blocks, for a block corresponding to the current context.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule     PresAttrRuleSearch (PtrPSchema tsch, GenericContext ctxt)
#else  /* __STDC__ */
static PtrPRule     PresAttrRuleSearch (tsch, ctxt)
PtrPSchema          tsch;
GenericContext      ctxt;
#endif /* !__STDC__ */
{
   AttributePres      *attrs = NULL;
   AttributePres      *found = NULL;
   int                 nbrules = 0;
   int                 i;

   /*
    * select the right attribute.
    */
   if (ctxt->class != NULL)
     {
	attrs = tsch->PsAttrPRule[ctxt->classattr - 1];
	nbrules = tsch->PsNAttrPRule[ctxt->classattr - 1];
     }
   else if (ctxt->attr != 0)
     {
	attrs = tsch->PsAttrPRule[ctxt->attr - 1];
	nbrules = tsch->PsNAttrPRule[ctxt->attr - 1];
     }
   else
     {
	fprintf (stderr, "Internal : PresAttrRuleSearch invalid context\n");
	return (NULL);
     }

   /*
    * first locate the attribute presentation rule block concerning this
    * attribute.
    */
   for (i = 0; i < nbrules; i++)
     {
	if (ctxt->class != NULL)
	  {
	     if (!ustrcmp (attrs[i].ApString, ctxt->class))
	       {
		  found = &attrs[i];
		  break;
	       }
	  }
	else if (ctxt->attr)
	  {
	     if (1)
	       {		/* Test sur attrval !!!!!!!!!!!!!!! */
		  found = &attrs[i];
		  break;
	       }
	  }
     }

   if (found == NULL)
      return (NULL);

   if (ctxt->class)
     return (found->ApTextFirstPRule);
   else if (ctxt->attr)
     return (found->ApEnumFirstPRule[ctxt->attrval]);
   else
     return (NULL);
}

/*----------------------------------------------------------------------
  PresAttrsRuleInsert : look in the array of Attribute presentation
  blocks, for a block corresponding to the current context.
  if not found we add a new one to the array.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule    *PresAttrRuleInsert (PtrPSchema tsch, GenericContext ctxt)
#else  /* __STDC__ */
static PtrPRule    *PresAttrRuleInsert (tsch, ctxt)
PtrPSchema          tsch;
GenericContext      ctxt;
#endif /* !__STDC__ */
{
   AttributePres      *attrs = NULL;
   AttributePres      *found = NULL;
   int                 nbrules = 0;
   int                 i;

   /* select the right attribute */
   if (ctxt->class != NULL)
     {
	attrs = tsch->PsAttrPRule[ctxt->classattr - 1];
	nbrules = tsch->PsNAttrPRule[ctxt->classattr - 1];
     }
   else if (ctxt->attr != 0)
     {
	attrs = tsch->PsAttrPRule[ctxt->attr - 1];
	nbrules = tsch->PsNAttrPRule[ctxt->attr - 1];
     }
   else
     {
	fprintf (stderr, "Internal : PresAttrRuleInsert invalid context\n");
	return (NULL);
     }

   /*locate the attribute presentation rule block concerning this attribute */
   for (i = 0; i < nbrules; i++)
     {
	if (ctxt->class)
	  {
	     if (!ustrcmp (attrs[i].ApString, ctxt->class))
	       {
		  found = &attrs[i];
		  break;
	       }
	  }
	else if (ctxt->attr)
	  {
	     if (1)
	       {		/* Test sur attrval !!!!!!!!!!!!!!! */
		  found = &attrs[i];
		  break;
	       }
	  }
     }

   /*
    * If no attribute presentation rule if found for the class, create a
    * new one and initialize it !
    * For the moment this means reallocating a complete new block of rules,
    * copying the old ones, freeing them, initializing the last one and
    * rebuilding the whole chain :-(
    */
   if (found == NULL)
     {
	nbrules++;

	found = (AttributePres *) TtaGetMemory (nbrules * sizeof (AttributePres));
	memcpy (found, attrs, (size_t) (sizeof (AttributePres) * (nbrules - 1)));

	if (ctxt->class)
	  {
	     TtaFreeMemory ( tsch->PsAttrPRule[ctxt->classattr - 1]);
	     attrs = tsch->PsAttrPRule[ctxt->classattr - 1] = found;
	     tsch->PsNAttrPRule[ctxt->classattr - 1] = nbrules;
	  }
	else if (ctxt->attr)
	  {
	     TtaFreeMemory ( tsch->PsAttrPRule[ctxt->attr - 1]);
	     attrs = tsch->PsAttrPRule[ctxt->attr - 1] = found;
	     tsch->PsNAttrPRule[ctxt->attr - 1] = nbrules;
	  }

	found = &found[nbrules - 1];
	memset (found, 0, sizeof (AttributePres));
	if (ctxt->class != NULL)
	   ustrcpy (&found->ApString[0], ctxt->class);
	else
	   found->ApString[0] = 0;
	found->ApTextFirstPRule = NULL;
	found->ApElemType = 0;
	found->ApNextAttrPres = NULL;

	for (i = 1; i < nbrules; i++)
	   attrs[i - 1].ApNextAttrPres = &attrs[i];
     }

   if (ctxt->class)
     return (&found->ApTextFirstPRule);
   else if (ctxt->attr)
     return (&found->ApEnumFirstPRule[ctxt->attrval]);
   else
     return (NULL);
}

/*----------------------------------------------------------------------
  TstRuleContext : test if a presentation rule correpond to the
  context given in argument and the correct presentation rule type.
  All the rules in a rule' list are sorted :
  *      first by type,
  *      second by View,
  *      and Last by conditions
  If pres is zero, we don't test on the kind of rule ...
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          TstRuleContext (PtrPRule rule, GenericContext ctxt,
                                    PRuleType pres)
#else  /* __STDC__ */
static int          TstRuleContext (rule, ctxt, pres)
PtrPRule            rule;
GenericContext      ctxt;
PRuleType           pres;

#endif /* !__STDC__ */
{
   PtrCondition        cond;
   int                 i, j, tmp;
   int                 nb_ancestors;

   /* first sort the rule condition list ... */
   SortConds (rule);
   cond = rule->PrCond;
   /* short test on the vue number and type of pres rule */
   if (rule->PrViewNum != 1)
      return (0);
   if (pres && rule->PrType != pres)
      return (0);

   /* first sort the ancestors list */
   for (i = 0; i < MAX_ANCESTORS; i++)
      if (ctxt->ancestors[i] == 0)
	 break;
   nb_ancestors = i;
   for (i = 0; i < nb_ancestors; i++)
      for (j = i + 1; j < nb_ancestors; j++)
	 if (ctxt->ancestors[i] > ctxt->ancestors[j])
	   {
	      tmp = ctxt->ancestors[i];
	      ctxt->ancestors[i] = ctxt->ancestors[j];
	      ctxt->ancestors[j] = tmp;
	      tmp = ctxt->ancestors_nb[i];
	      ctxt->ancestors_nb[i] = ctxt->ancestors_nb[j];
	      ctxt->ancestors_nb[j] = tmp;
	   }

   /* scan all the conditions associated to a rule */
   if (ctxt->classattr != 0)
     {
	/* should be at the beginning as effect of sorting but ... */
	while (cond != NULL && cond->CoCondition != PcElemType)
	   cond = cond->CoNextCondition;
	if (ctxt->attrelem != 0 && cond == NULL)
	   return (0);
	if (ctxt->attrelem == 0 && cond != NULL)
	   return (0);
	if (ctxt->attrelem != 0 &&
	    cond->CoTypeElAttr != ctxt->attrelem)
	   return (0);
	cond = rule->PrCond;
     }

   /* scan all the ancestors conditions associated to a rule */
   while ((cond != NULL) && (cond->CoCondition < PcWithin))
      cond = cond->CoNextCondition;

   for (i = 0; i < nb_ancestors; i++)
     {
	if (cond == NULL)
	   return (0);
	if (cond->CoCondition != PcWithin)
	   return (0);
	if (ctxt->ancestors[i] != cond->CoTypeAncestor)
	   return (0);
	if (ctxt->ancestors_nb[i] != cond->CoRelation)
	   return (0);
	cond = cond->CoNextCondition;
     }
   if (cond != NULL && cond->CoCondition == PcWithin)
      return (0);
   return (1);
}

/*----------------------------------------------------------------------
  PresRuleSearch : search a presentation rule for a given view
  in a chain.
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
  PtrPRule            cur;
  boolean             found;

  *chain = NULL;
  /* select the good starting point depending on the context */
  if (ctxt->box != 0)
    *chain = BoxRuleInsert (tsch, ctxt);
  else if (ctxt->attr || ctxt->class)
    *chain = PresAttrRuleInsert (tsch, ctxt);
  else if (ctxt->type != 0)
    *chain = &tsch->PsElemPRule[ctxt->type - 1];
  else
    {
      fprintf (stderr, "Internal : invalid Generic Context\n");
      return (NULL);
    }

  /*
   * scan the chain of presentation rules looking for an existing
   * rule for this context and kind of presentation attribute.
   */
  cur = **chain;
  found = FALSE;
  while (!found && cur != NULL)
    {
      /* shortcut : rules are sorted by type and view number and
	 Functions rules are sorted by number */
      if (cur->PrType > pres ||
	  (cur->PrType == pres && cur->PrViewNum > 1) ||
	  (cur->PrType == pres && pres == PtFunction && cur->PrPresFunction > extra))
	  cur = NULL;
      else if (pres != PtFunction ||
	       (pres == PtFunction && cur->PrPresFunction != extra))
	/* check for extra specification in case of function rule */
	{
	  *chain = &(cur->PrNextPRule);
	  cur = cur->PrNextPRule;
	}
      else if (TstRuleContext (cur, ctxt, pres))
	/* this rule already exists */
	found = TRUE;
      else
	{
	  /* jump to next and keep track of previous */
	  *chain = &(cur->PrNextPRule);
	  cur = cur->PrNextPRule;
	}
    }
   return (cur);
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
  int                 i, j, tmp, nb_ancestors;

  /* first sort the ancestors list */
  for (i = 0; i < MAX_ANCESTORS; i++)
    if (ctxt->ancestors[i] == 0)
      break;

  nb_ancestors = i;
  for (i = 0; i < nb_ancestors; i++)
    for (j = i + 1; j < nb_ancestors; j++)
      if (ctxt->ancestors[i] > ctxt->ancestors[j])
	{
	  tmp = ctxt->ancestors[i];
	  ctxt->ancestors[i] = ctxt->ancestors[j];
	  ctxt->ancestors[j] = tmp;
	  tmp = ctxt->ancestors_nb[i];
	  ctxt->ancestors_nb[i] = ctxt->ancestors_nb[j];
	  ctxt->ancestors_nb[j] = tmp;
	}

  /* Search presentation rule */
  cur = PresRuleSearch (tsch, ctxt, pres, extra, &chain);
  if (cur != NULL)
    return (cur);
  else
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
	  if ((ctxt->attr || ctxt->class) && ctxt->attrelem != 0)
	    PresRuleAddAttrCond (pRule, ctxt->attrelem);
	  /* add the ancesters conditions ... */
	  i = 0;
	  while (i < MAX_ANCESTORS && ctxt->ancestors[i] != 0)
	    {
	      PresRuleAddAncestorCond (pRule, ctxt->ancestors[i], ctxt->ancestors_nb[i]);
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
      return (pRule);
    }
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
  int               i, j, tmp, nb_ancestors;
  int               elType = 0;
  int               attrType = 0;
  int               presBox = 0;

  /* first sort the ancestors list */
  for (i = 0; i < MAX_ANCESTORS; i++)
    if (ctxt->ancestors[i] == 0)
      break;
  nb_ancestors = i;
  for (i = 0; i < nb_ancestors; i++)
    for (j = i + 1; j < nb_ancestors; j++)
      if (ctxt->ancestors[i] > ctxt->ancestors[j])
	{
	  tmp = ctxt->ancestors[i];
	  ctxt->ancestors[i] = ctxt->ancestors[j];
	  ctxt->ancestors[j] = tmp;
	  tmp = ctxt->ancestors_nb[i];
	  ctxt->ancestors_nb[i] = ctxt->ancestors_nb[j];
	  ctxt->ancestors_nb[j] = tmp;
	}

  /* Search presentation rule */
  presBox = ctxt->box;
  cur = PresRuleSearch (tsch, ctxt, pres, extra, &chain);
  if (cur != NULL)
    {
      if (chain != NULL)
	/* found, remove it from the chain */
	*chain = cur->PrNextPRule;

      cur->PrNextPRule = NULL;
      /* update the rendering */
      doc = ctxt->doc;
      pSS = (PtrSSchema) ctxt->schema;
      ApplyPRules (doc, pSS, elType, attrType, presBox, cur, TRUE);
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
static void         PresentationValueToPRule (PresentationValue val, int type, PtrPRule rule, int funcType, boolean absolute, boolean generic)
#else
static void         PresentationValueToPRule (val, type, rule, funcType, absolute, generic)
PresentationValue   val;
int                 type;
PtrPRule            rule;
int                 funcType;
boolean             absolute;
boolean             generic;
#endif
{
  TypeUnit            int_unit;
  int                 value;
  int                 unit;
  boolean             real;

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
    case DRIVERP_UNIT_REL:
      int_unit = UnRelative;
      if (type == PtBreak1 ||
	  type == PtBreak2 ||
	  type == PtIndent ||
	  type == PtLineSpacing ||
	  type == PtLineWeight)
	value *= 10;
      break;
    case DRIVERP_UNIT_EM:
      int_unit = UnRelative;
      value *= 10;
      break;
    case DRIVERP_UNIT_PT:
      int_unit = UnPoint;
      break;
    case DRIVERP_UNIT_PC:
      int_unit = UnPoint;
      value *= 12;
      break;
    case DRIVERP_UNIT_IN:
      int_unit = UnPoint;
      value *= 72;
      break;
    case DRIVERP_UNIT_CM:
      int_unit = UnPoint;
      value *= 28;
      break;
    case DRIVERP_UNIT_MM:
      int_unit = UnPoint;
      value *= 28;
      value /= 10;
      break;
    case DRIVERP_UNIT_PX:
      int_unit = UnPixel;
      break;
    case DRIVERP_UNIT_PERCENT:
      int_unit = UnPercent;
      break;
    case DRIVERP_UNIT_XHEIGHT:
      int_unit = UnXHeight;
      value *= 10;
      break;
    case DRIVERP_UNIT_BOX:
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
	case DRIVERP_FONT_HELVETICA:
	  rule->PrChrValue = 'H';
	  break;
	case DRIVERP_FONT_TIMES:
	  rule->PrChrValue = 'T';
	  break;
	case DRIVERP_FONT_COURIER:
	  rule->PrChrValue = 'C';
	  break;
	}
      break;
    case PtStyle:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case StyleBold:
	  rule->PrChrValue = 'B';
	  break;
	case StyleRoman:
	  rule->PrChrValue = 'R';
	  break;
	case StyleItalics:
	  rule->PrChrValue = 'I';
	  break;
	case StyleBoldItalics:
	  rule->PrChrValue = 'G';
	  break;
	case StyleOblique:
	  rule->PrChrValue = 'O';
	  break;
	case StyleBoldOblique:
	  rule->PrChrValue = 'Q';
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
    case PtSize:
    case PtLineSpacing:
    case PtLineWeight:
      rule->PrPresMode = PresImmediate;
      rule->PrMinUnit = int_unit;
      rule->PrMinValue = value;
      rule->PrMinAttr = FALSE;
      break;
    case PtJustify:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case DRIVERP_JUSTIFIED:
	  rule->PrJustify = TRUE;
	  break;
	case DRIVERP_NOTJUSTIFIED:
	  rule->PrJustify = FALSE;
	  break;
	}
      break;
    case PtAdjust:
      rule->PrPresMode = PresImmediate;
      switch (value)
	{
	case DRIVERP_ADJUSTLEFT:
	  rule->PrAdjust = AlignLeft;
	  break;
	case DRIVERP_ADJUSTRIGHT:
	  rule->PrAdjust = AlignRight;
	  break;
	case DRIVERP_ADJUSTCENTERED:
	  rule->PrAdjust = AlignCenter;
	  break;
	case DRIVERP_ADJUSTLEFTWITHDOTS:
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
	case DRIVERP_HYPHENATE:
	  rule->PrJustify = TRUE;
	  break;
	case DRIVERP_NOHYPHENATE:
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
      if (generic)
	{
	  /* generate a complete rule Height=Enclosed.Height */
	  rule->PrDimRule.DrPosition = FALSE;
	  rule->PrDimRule.DrAbsolute = FALSE;
	  rule->PrDimRule.DrSameDimens = TRUE;
	  rule->PrDimRule.DrAttr = FALSE;
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
	  if (value == DRIVERP_NOTINLINE)
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
	    case DRIVERP_REALSIZE:
	      rule->PrPresBox[0] = RealSize;
	      break;
	    case DRIVERP_SCALE:
	      rule->PrPresBox[0] = ReScale;
	      break;
	    case DRIVERP_REPEAT:
	      rule->PrPresBox[0] = FillFrame;
	      break;
	    case DRIVERP_HREPEAT:
	      rule->PrPresBox[0] = XRepeat; break;
	    case DRIVERP_VREPEAT:
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
  boolean             real = FALSE;

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
	  value = DRIVERP_FONT_HELVETICA;
	  break;
	case 'T':
	  value = DRIVERP_FONT_TIMES;
	  break;
	case 'C':
	  value = DRIVERP_FONT_COURIER;
	  break;
	}
      break;
    case PtStyle:
      switch (rule->PrChrValue)
	{
	case 'B':
	  value = DRIVERP_FONT_BOLD;
	  break;
	case 'R':
	  value = DRIVERP_FONT_ROMAN;
	  break;
	case 'I':
	  value = DRIVERP_FONT_ITALICS;
	  break;
	case 'G':
	  value = DRIVERP_FONT_BOLDITALICS;
	  break;
	case 'O':
	  value = DRIVERP_FONT_OBLIQUE;
	  break;
	case 'Q':
	  value = DRIVERP_FONT_BOLDOBLIQUE;
	  break;
	}
      break;
    case PtUnderline:
      switch (rule->PrChrValue)
	{
	case 'U':
	  value = DRIVERP_UNDERLINE;
	  break;
	case 'O':
	  value = DRIVERP_OVERLINE;
	  break;
	case 'C':
	  value = DRIVERP_CROSSOUT;
	  break;
	}
      break;
    case PtThickness:
    case PtLineStyle:
      break;
    case PtBreak1:
    case PtBreak2:
    case PtIndent:
    case PtSize:
    case PtLineSpacing:
    case PtLineWeight:
      int_unit = rule->PrMinUnit;
      value = rule->PrMinValue;
      break;
    case PtJustify:
      if (rule->PrJustify)
	value = DRIVERP_JUSTIFIED;
      else
	value = DRIVERP_NOTJUSTIFIED;
      break;
    case PtAdjust:
      switch (rule->PrAdjust)
	{
	case AlignLeft:
	  value = DRIVERP_ADJUSTLEFT;
	  break;
	case AlignRight:
	  value = DRIVERP_ADJUSTRIGHT;
	  break;
	case AlignCenter:
	  value = DRIVERP_ADJUSTCENTERED;
	  break;
	case AlignLeftDots:
	  value = DRIVERP_ADJUSTLEFTWITHDOTS;
	  break;
	default:
	  value = DRIVERP_ADJUSTLEFT;
	  break;
	}
      break;
    case PtHyphenate:
      if (rule->PrJustify)
	value = DRIVERP_HYPHENATE;
      else
	value = DRIVERP_NOHYPHENATE;
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
	  value = DRIVERP_INLINE;
	  break;
	case FnNoLine:
	  value = DRIVERP_NOTINLINE;
	  break;
	case FnCreateBefore:
	case FnCreateWith:
	case FnCreateFirst:
	case FnCreateLast:
	case FnCreateAfter:
	case FnCreateEnclosing:
	  value = rule->PrNPresBoxes;
	  unit = DRIVERP_UNIT_BOX;
	  break;
	case FnShowBox:
	  value = rule->PrNPresBoxes;
	  unit = DRIVERP_UNIT_REL;
	  break;
	case FnBackgroundPicture:
	  value = rule->PrPresBox[0];
	  unit = DRIVERP_UNIT_REL;
	  break;
	case FnPictureMode:
	  unit = DRIVERP_UNIT_REL;
	  value = DRIVERP_REALSIZE;
	  switch (rule->PrPresBox[0])
	    {
	    case RealSize:
	      value = DRIVERP_REALSIZE; break;
	    case ReScale:
	      value = DRIVERP_SCALE; break;
	    case FillFrame:
	      value = DRIVERP_REPEAT; break;
	    case XRepeat:
	      value = DRIVERP_HREPEAT; break;
	    case YRepeat:
	      value = DRIVERP_VREPEAT; break;
	    default:
	      unit = DRIVERP_UNIT_INVALID;
	      value = 0;
	    }
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
	unit = DRIVERP_UNIT_REL;
      else
	unit = DRIVERP_UNIT_EM;
      value /= 10;
      break;
    case UnXHeight:
      value /= 10;
      unit = DRIVERP_UNIT_XHEIGHT;
      if (value % 10)
	{
	  real = TRUE;
	  value *= 1000;
	}
      else
      break;
    case UnPoint:
      unit = DRIVERP_UNIT_PT;
      break;
    case UnPixel:
      unit = DRIVERP_UNIT_PX;
      break;
    case UnPercent:
      unit = DRIVERP_UNIT_PERCENT;
      break;
    default:
      unit = DRIVERP_UNIT_INVALID;
      break;
    }

  if (value % 10)
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
static void         TypeToPresentation (unsigned int type, PRuleType *intRule, unsigned int *func, boolean *absolute)
#else
static void         TypeToPresentation (type, intRule, func, absolute)
unsiged int         type;
PRuleType          *intRule;
unsiged int        *func;
boolean            *absolute;
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
  PtrPRule          rule;
  PRuleType         intRule;
  unsigned int      func = 0;
  int               cst = 0;
  boolean           absolute, generic;

  TypeToPresentation (type, &intRule, &func, &absolute);
  generic = (el == NULL);
  if (c->destroy)
    {
      if (generic)
	PresRuleRemove ((PtrPSchema) tsch, (GenericContext) c, intRule, func);
      else
	RemoveElementPRule ((PtrElement) el, intRule, func);
    }
  else
    {
      if (generic)
	rule = PresRuleInsert ((PtrPSchema) tsch, (GenericContext) c, intRule, func);
      else
	rule = InsertElementPRule ((PtrElement) el, intRule, func);
      if (rule == NULL)
	return (-1);

      if (type == PRBackgroundPicture)
	{
	  cst = PresConstInsert (tsch, v.pointer);
	  v.typed_data.unit = DRIVERP_UNIT_REL;
	  v.typed_data.value = cst;
	  v.typed_data.real = FALSE;
	}
      PresentationValueToPRule (v, intRule, rule, func, absolute, generic);
      if (generic)
	{
	  rule->PrViewNum = 1;
	  if (((GenericContext) c)->box != 0 && intRule == PtFunction)
	    BuildBoxName ((GenericContext) c, &rule->PrPresBoxName);
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
  boolean           absolute, generic;

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
      setting->type = DRIVERP_SHOW;
      break;
    case PtFont:
      setting->type = DRIVERP_FONT_FAMILY;
      break;
    case PtStyle:
      setting->type = DRIVERP_FONT_STYLE;
      break;
    case PtUnderline:
      setting->type = DRIVERP_TEXT_UNDERLINING;
      break;
    case PtIndent:
      setting->type = DRIVERP_INDENT;
      break;
    case PtSize:
      setting->type = DRIVERP_FONT_SIZE;
      break;
    case PtLineSpacing:
      setting->type = DRIVERP_LINE_SPACING;
      break;
    case PtJustify:
      setting->type = DRIVERP_JUSTIFICATION;
      break;
    case PtAdjust:
      setting->type = DRIVERP_ALIGNMENT;
      break;
    case PtHyphenate:
      setting->type = DRIVERP_HYPHENATION;
      break;
    case PtFillPattern:
      setting->type = DRIVERP_FILL_PATTERN;
      break;
    case PtBackground:
      setting->type = DRIVERP_BACKGROUND_COLOR;
      break;
    case PtForeground:
      setting->type = DRIVERP_FOREGROUND_COLOR;
      break;
    case PtVertPos:
      setting->type = DRIVERP_VERTICAL_POSITION;
      break;
    case PtHorizPos:
      setting->type = DRIVERP_HORIZONTAL_POSITION;
      break;
    case PtHeight:
      setting->type = DRIVERP_HEIGHT;
      break;
    case PtWidth:
      setting->type = DRIVERP_WIDTH;
      break;
    case PtFunction:
      switch (extra)
	{
	case FnLine:
	  setting->type = DRIVERP_IN_LINE;
	  break;
	case FnShowBox:
	  setting->type = DRIVERP_SHOWBOX;
	  break;
	case FnBackgroundPicture:
	  setting->type = DRIVERP_BGIMAGE;
	  break;
	case FnPictureMode:
	  setting->type = DRIVERP_PICTUREMODE;
	  break;
	default:
	  /* not yet supported by the driver */
	  setting->type = DRIVERP_NONE;
	  return;
	}
      break;
    default:
      /* not yet supported by the driver */
      setting->type = DRIVERP_NONE;
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
  ctxt->schema = TtaGetDocumentSSchema (doc);
  ctxt->destroy = 0;
  ctxt->box = 0;
  ctxt->type = 0;
  ctxt->attr = 0;
  ctxt->attrval = 0;
  ctxt->class = NULL;
  ctxt->classattr = 0;
  ctxt->attrelem = 0;
  for (i = 0; i < MAX_ANCESTORS; i++)
    {
      ctxt->ancestors[i] = 0;
      ctxt->ancestors_nb[i] = 0;
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

  if (el != NULL)
    {
      do
	{
	  rule = NULL;
	  TtaNextPRule (el, &rule);
	  if (rule)
	    TtaRemovePRule (el, rule, doc);
	}
      while (rule != NULL);
    }
}

/*----------------------------------------------------------------------
  Function used to update the drawing after styling an element or a
  generic type.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaUpdateStylePresentation (Element el, PSchema tsch, PresentationContext c)
#else  /* !__STDC__ */
void                TtaUpdateStylePresentation (el, tsch, c)
Element             el;
PSchema             tsch;
PresentationContext c;
#endif /* !__STDC__ */
{
   GenericContext ctxt = (GenericContext) c;
   Document       doc;
   PtrSSchema     pSS;
   int            elType = 0;
   int            attrType = 0;
   int            presBox = 0;
   PtrPRule       pRule;

   if (c == NULL || (el == NULL && tsch == NULL))
     return;

   doc = c->doc;
   if (el != NULL)
     {
       pRule = ((PtrElement)el)->ElFirstPRule;
       if (pRule == NULL)
	 return;
       doc = ctxt->doc;
       ApplyPRulesElement (pRule, (PtrElement) el, LoadedDocument[doc - 1], (boolean)c->destroy);
     }
   else
     {
       pSS = (PtrSSchema) c->schema;
       /*  select the good starting point depending on the context */
       if (ctxt->box != 0)
	 {
	   presBox = ctxt->box;
	   pRule = BoxRuleSearch ((PtrPSchema) tsch, ctxt);
	 }
       else if (c->type != 0)
	 {
	   elType = c->type;
	   pRule = ((PtrPSchema) tsch)->PsElemPRule[elType - 1];
	 }
       else if (ctxt->attr || ctxt->class)
	 {
	   if (ctxt->attr)
	     attrType = ctxt->attr;
	   else
	     attrType = ctxt->classattr;
	   pRule = PresAttrRuleSearch ((PtrPSchema) tsch, ctxt);
	 }
       else
	 return;

       if (pRule == NULL)
	 return;
       ApplyPRules (doc, pSS, elType, attrType, presBox, pRule, FALSE);
     }
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
      if (setting.type == DRIVERP_BGIMAGE)
	{
	  cst = setting.value.typed_data.value;
	  pSc1 = LoadedDocument[doc - 1]->DocSSchema->SsPSchema;
	  setting.value.pointer = &pSc1->PsConstant[cst-1].PdString[0];
	}

	handler (el, doc, &setting, param);
	rule = rule->PrNextPRule;
     }
}
