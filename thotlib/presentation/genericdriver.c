/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * genericdriver.c : presentation driver used to manipulate presentation  *
 *           using the generic presentation description of a document.	  *
 *                                                                        *
 *  This kind of routines are used to change presentation attributes of   *
 *  ALL ELEMENTS of a document type. Any document displayed using the     *
 *  Thot editing kernel is associated to one (or more) presentation       *
 *  scheme describing the way each kind of element is to be displayed     *
 *  depending on its type, its environment and specific rules.		  *
 *  This driver works by modifying directly the in-memory representation  *
 *  of these rules.							  *
 *                                                                        *
 *  In the case for HTML structure schema :				  *
 *  Here is the list of places where specific presentation rules are to   *
 *  be installed depending on the context of the rule :			  *
 *  									  *
 *                   | Type of Elem | Int attr | String attr | Ancestors  *
 *  H1 { ... }       |      X       |          |             |            *
 *  EM { ... }       |              |     X    |             |            *
 *  .pink { ... }    |              |          |      X      |            *
 *  UL LI { ... }    |      X       |          |             |      X     *
 *  H1 EM { ... }    |              |     X    |             |      X     *
 *  H1.pink { ... }  |              |          |      X      |      X     *
 *  EM.pink { ... }  |              |   ????   |    ????     |            *
 *                                                                        *
 *  NOTE : The driver need conditions applying to a presentation rule to  *
 *  be sorted. The function SortConds and AddCond apply the needed order  *
 *  PcElemType rule have to be first then all rules are sorted by the	  *
 *  value of the CoCondition field, and others fields available for rules *
 *  of teh same kind. The function TstRuleContext rely on this order.	  *
 *
 * Author: D. Veillard (INRIA)
           I. Vatton (INRIA) extensions
 *
 */

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "typeint.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "pschema.h"

#undef THOT_EXPORT
#define THOT_EXPORT
#include "edit_tv.h"

#include "changepresent_f.h"
#include "genericdriver.h"
#include "memory_f.h"
#include "presentdriver_f.h"


/*----------------------------------------------------------------------
  CleanGenericContext : clean up Generic context by removing all context.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CleanGenericContext (GenericContext ctxt)
#else  /* __STDC__ */
static void         CleanGenericContext (ctxt)
GenericContext      ctxt;
#endif /* __STDC__ */
{
  int                 i;
  
  if (ctxt == NULL)
    return;
  if (ctxt->drv != &GenericStrategy)
    return;
  ctxt->box = 0;
  ctxt->type = 0;
  ctxt->attr = 0;
  ctxt->attrval = 0;
  ctxt->class = NULL;
  ctxt->classattr = 0;
  ctxt->attrelem = 0;
  ctxt->magic1 = 0;
  ctxt->magic2 = 0;
  for (i = 0; i < MAX_ANCESTORS; i++)
    {
      ctxt->ancestors[i] = 0;
      ctxt->ancestors_nb[i] = 0;
    }
}

/*----------------------------------------------------------------------
  GetGenericContext : user level function needed to allocate and
  initialize a GenericContext.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
GenericContext      GetGenericContext (Document doc)
#else  /* __STDC__ */
GenericContext      GetGenericContext (doc)
Document            doc;
#endif /* __STDC__ */
{
   GenericContext      ctxt;

   ctxt = (GenericContext) TtaGetMemory (sizeof (GenericContextBlock));
   if (ctxt == NULL)
      return (NULL);
   ctxt->drv = &GenericStrategy;
   ctxt->doc = doc;
   ctxt->schema = TtaGetDocumentSSchema (doc);
   ctxt->destroy = 0;
   CleanGenericContext (ctxt);
   return (ctxt);
}

/*----------------------------------------------------------------------
  FreeGenericContext : user level function needed to deallocate
  a GenericContext.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FreeGenericContext (GenericContext ctxt)
#else  /* __STDC__ */
void                FreeGenericContext (ctxt)
GenericContext      ctxt;
#endif /* __STDC__ */
{
   if (ctxt == NULL)
      return;
   if (ctxt->drv != &GenericStrategy)
      return;
   TtaFreeMemory ( ctxt);
}


/*----------------------------------------------------------------------
  BuildBoxName : generate an unique name encoding for the given context.
  Assume the ancestor list has been sorted.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BuildBoxName (GenericContext ctxt, Name * boxname)
#else  /* __STDC__ */
static void         BuildBoxName (ctxt, boxname)
GenericContext      ctxt;
Name               *boxname;
#endif /* !__STDC__ */
{
   int                 i;
   int                 len;
   char                buffer[100];

   buffer[0] = 0;
   len = 0;
   for (i = 0; i < MAX_ANCESTORS; i++)
     {
	if (ctxt->ancestors[i] == 0)
	   break;
	if (ctxt->ancestors_nb[i] > 1)
	   sprintf (&buffer[len], "%d:%d/",
		    ctxt->ancestors[i], ctxt->ancestors_nb[i]);
	else
	   sprintf (&buffer[len], "%d/", ctxt->ancestors[i]);
	len = strlen (buffer);
     }
   if (ctxt->type)
      sprintf (&buffer[len], "%d,", ctxt->type);
   len = strlen (buffer);
   if (ctxt->attr)
      sprintf (&buffer[len], "%d:%d,", ctxt->attr, ctxt->attrval);
   len = strlen (buffer);
   if (ctxt->class)
      sprintf (&buffer[len], "%d.%s,", ctxt->classattr, ctxt->class);
   len = strlen (buffer);

   strncpy ((char *) boxname, buffer, sizeof (Name));
}

/*----------------------------------------------------------------------
 BoxRuleSearch : look in the array of boxes for an entry
        corresponding to the current context.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule     BoxRuleSearch (PSchema tsch, GenericContext ctxt)
#else  /* __STDC__ */
static PtrPRule     BoxRuleSearch (tsch, ctxt)
PSchema             tsch;
GenericContext      ctxt;
#endif /* !__STDC__ */
{
   PtrPSchema          pSchemaPrs = (PtrPSchema) tsch;
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
   for (i = 1; i <= pSchemaPrs->PsNPresentBoxes; i++)
     {
	if (!strcmp (ctxt->class, pSchemaPrs->PsPresentBox[i - 1].PbName))
	  {
	     ctxt->box = i;
	     return (pSchemaPrs->PsPresentBox[i - 1].PbFirstPRule);
	  }
     }

   ctxt->box = 0;
   return (NULL);
}

/*----------------------------------------------------------------------
  BoxRuleInsert : look in the array of boxes for an entry
  corresponding to the current context.
  if not found we add a new one to the array.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule    *BoxRuleInsert (PSchema tsch, GenericContext ctxt)
#else  /* __STDC__ */
static PtrPRule    *BoxRuleInsert (tsch, ctxt)
PSchema             tsch;
GenericContext      ctxt;
#endif /* !__STDC__ */
{
   PtrPSchema          pSchemaPrs = (PtrPSchema) tsch;
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
   for (i = 1; i <= pSchemaPrs->PsNPresentBoxes; i++)
     {
	if (!strcmp (boxname, pSchemaPrs->PsPresentBox[i - 1].PbName))
	  {
	     ctxt->box = i;
	     return (&pSchemaPrs->PsPresentBox[i - 1].PbFirstPRule);
	  }
     }

   if (pSchemaPrs->PsNPresentBoxes >= MAX_PRES_BOX)
     {
	fprintf (stderr, "BoxRuleInsert : PsNPresentBoxes >= MAX_PRES_BOX (%d)\n",
		 MAX_PRES_BOX);
	ctxt->box = 0;
	return (NULL);
     }

   /* allocate and initialize the new BOX */
   pSchemaPrs->PsNPresentBoxes++;
   ctxt->box = pSchemaPrs->PsNPresentBoxes;
   box = &pSchemaPrs->PsPresentBox[pSchemaPrs->PsNPresentBoxes - 1];
   strncpy (box->PbName, boxname, sizeof (box->PbName));
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

   return (&pSchemaPrs->PsPresentBox[pSchemaPrs->PsNPresentBoxes -
				     1].PbFirstPRule);
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

int                 NbAddCond = 0;

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

   NbAddCond++;

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
  CmpRulesForDisplay : the Thot rendering engine expects rules
  stored in a list to be sorted by type then by view.
  this accelerate the drawing but doesn't fit with access by
  context.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          CmpRulesForDisplay (PtrPRule r1, PtrPRule r2)
#else  /* __STDC__ */
static int          CmpRulesForDisplay (r1, r2)
PtrPRule            r1;
PtrPRule            r2;
#endif /* !__STDC__ */
{
   if (r1->PrType < r2->PrType)
      return (-1);
   if (r1->PrType > r2->PrType)
      return (+1);
   if (r1->PrType == r2->PrType && r1->PrType == PtFunction)
     {
       if (r1->PrPresFunction > r2->PrPresFunction) 
           return (-1);
       if (r1->PrPresFunction < r2->PrPresFunction) 
           return (+1);
     }
   if (r1->PrViewNum < r2->PrViewNum)
      return (-1);
   if (r1->PrViewNum > r2->PrViewNum)
      return (+1);
   return (0);
}

/*----------------------------------------------------------------------
  CmpRulesForAccess : the driver API sometimes need to sort
  rules stored in a list in a different order. Rules are
  to be sorted by identical context i.e. by conditions
  applying to the rules and by view.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          CmpRulesForAccess (PtrPRule r1, PtrPRule r2)
#else  /* __STDC__ */
static int          CmpRulesForAccess (r1, r2)
PtrPRule            r1;
PtrPRule            r2;
#endif /* !__STDC__ */
{
   if (r1->PrViewNum < r2->PrViewNum)
      return (-1);
   if (r1->PrViewNum > r2->PrViewNum)
      return (+1);
   return (CompareCondLists (r1->PrCond, r2->PrCond));
}

typedef int         (*CmpRulesFunc) (PtrPRule r1, PtrPRule r2);

/*----------------------------------------------------------------------
  SortRules : basic function used to sort rules stored in a list.
  This is an uggly Bubble-Sort but should be fast enought.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SortRules (PtrPRule * rules, CmpRulesFunc cmp)
#else  /* __STDC__ */
static void         SortRules (rules, cmp)
PtrPRule           *rules;
CmpRulesFunc        cmp;
#endif /* !__STDC__ */
{
   PtrPRule            cour;
   PtrPRule            next;

   while (*rules != NULL)
     {
	cour = *rules;
	next = cour->PrNextPRule;
	while ((cour != NULL) && (next != NULL))
	  {
	     if (cmp (cour, next) < 0)
	       {

		  /*
		   * insert next at the base of the current rule list.
		   */
		  cour->PrNextPRule = next->PrNextPRule;
		  next->PrNextPRule = *rules;
		  *rules = next;
		  next = cour->PrNextPRule;
	       }
	     else
	       {
		  cour = next;
		  next = cour->PrNextPRule;
	       }
	  }
	rules = &((*rules)->PrNextPRule);
     }
}

#define SortRulesForDisplay(rules) SortRules((rules), CmpRulesForDisplay)
#define SortRulesForAccess(rules) SortRules((rules), CmpRulesForAccess)

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
   if ((pres) &&
       (rule->PrType != pres))
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
	while ((cond != NULL) && (cond->CoCondition != PcElemType))
	   cond = cond->CoNextCondition;
	if ((ctxt->attrelem != 0) && (cond == NULL))
	   return (0);
	if ((ctxt->attrelem == 0) && (cond != NULL))
	   return (0);
	if ((ctxt->attrelem != 0) &&
	    (cond->CoTypeElAttr != ctxt->attrelem))
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
   if ((cond != NULL) && (cond->CoCondition == PcWithin))
      return (0);
   return (1);
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
   cond->CoTarget = 0;
   cond->CoNotNegative = 1;
   cond->CoRelation = nr;
   cond->CoTypeAncestor = type;
   cond->CoImmediate = 0;
   cond->CoAncestorRel = CondGreater;
   cond->CoAncestorName[0] = 0;
   cond->CoSSchemaName[0] = 0;
   AddCond (&rule->PrCond, cond);
}

/*----------------------------------------------------------------------
  PresRuleAddSurElemCond : add a SurElem condition to a presentation rule.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PresRuleAddSurElemCond (PtrPRule rule, int type)
#else  /* __STDC__ */
static void         PresRuleAddSurElemCond (rule, type)
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
   cond->CoNotNegative = 1;
   cond->CoTarget = 0;
   cond->CoTypeElAttr = type;
   AddCond (&rule->PrCond, cond);
}

/*----------------------------------------------------------------------
  PresAttrsRuleSearch : look in the array of Attribute presentation
  blocks, for a block corresponding to the current context.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule     PresAttrRuleSearch (PSchema tsch, GenericContext ctxt)
#else  /* __STDC__ */
static PtrPRule     PresAttrRuleSearch (tsch, ctxt)
PSchema             tsch;
GenericContext      ctxt;
#endif /* !__STDC__ */
{
   /* PtrSSchema   pSchemaStr = (PtrSSchema) ctxt->schema; */
   PtrPSchema          pSchemaPrs = (PtrPSchema) tsch;
   AttributePres      *attrs = NULL;
   AttributePres      *found = NULL;
   int                 nbrules = 0;
   int                 i;

   /*
    * select the right attribute.
    */
   if (ctxt->class != NULL)
     {
	attrs = pSchemaPrs->PsAttrPRule[ctxt->classattr - 1];
	nbrules = pSchemaPrs->PsNAttrPRule[ctxt->classattr - 1];
     }
   else if (ctxt->attr != 0)
     {
	attrs = pSchemaPrs->PsAttrPRule[ctxt->attr - 1];
	nbrules = pSchemaPrs->PsNAttrPRule[ctxt->attr - 1];
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
	     if (!strcmp (attrs[i].ApString, ctxt->class))
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
static PtrPRule    *PresAttrRuleInsert (PSchema tsch, GenericContext ctxt)
#else  /* __STDC__ */
static PtrPRule    *PresAttrRuleInsert (tsch, ctxt)
PSchema             tsch;
GenericContext      ctxt;
#endif /* !__STDC__ */
{
   PtrPSchema          pSchemaPrs = (PtrPSchema) tsch;
   AttributePres      *attrs = NULL;
   AttributePres      *found = NULL;
   int                 nbrules = 0;
   int                 i;

   /* select the right attribute */
   if (ctxt->class != NULL)
     {
	attrs = pSchemaPrs->PsAttrPRule[ctxt->classattr - 1];
	nbrules = pSchemaPrs->PsNAttrPRule[ctxt->classattr - 1];
     }
   else if (ctxt->attr != 0)
     {
	attrs = pSchemaPrs->PsAttrPRule[ctxt->attr - 1];
	nbrules = pSchemaPrs->PsNAttrPRule[ctxt->attr - 1];
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
	     if (!strcmp (attrs[i].ApString, ctxt->class))
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
	     TtaFreeMemory ( pSchemaPrs->PsAttrPRule[ctxt->classattr - 1]);
	     attrs = pSchemaPrs->PsAttrPRule[ctxt->classattr - 1] = found;
	     pSchemaPrs->PsNAttrPRule[ctxt->classattr - 1] = nbrules;
	  }
	else if (ctxt->attr)
	  {
	     TtaFreeMemory ( pSchemaPrs->PsAttrPRule[ctxt->attr - 1]);
	     attrs = pSchemaPrs->PsAttrPRule[ctxt->attr - 1] = found;
	     pSchemaPrs->PsNAttrPRule[ctxt->attr - 1] = nbrules;
	  }

	found = &found[nbrules - 1];
	memset (found, 0, sizeof (AttributePres));
	if (ctxt->class != NULL)
	   strcpy (&found->ApString[0], ctxt->class);
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
  PresRuleSearch : search a presentation rule for a given view
  in a chain.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrPRule     PresRuleSearch (PSchema tsch, GenericContext ctxt,
                                    PRuleType pres, int extra, PtrPRule **chain)
#else  /* __STDC__ */
static PtrPRule     PresRuleSearch (tsch, ctxt, pres, extra, chain)
PSchema             tsch;
GenericContext      ctxt;
PRuleType           pres;
int                 extra;
PtrPRule          **chain;
#endif /* !__STDC__ */
{
  PtrPSchema          pSchemaPrs = (PtrPSchema) tsch;
  PtrPRule            cur;
  boolean             found;

  *chain = NULL;
  /* select the good starting point depending on the context */
  if (ctxt->box != 0)
    *chain = BoxRuleInsert (tsch, ctxt);
  else if (ctxt->attr || ctxt->class)
    *chain = PresAttrRuleInsert (tsch, ctxt);
  else if (ctxt->type != 0)
    *chain = &pSchemaPrs->PsElemPRule[ctxt->type - 1];
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
      else if (pres == PtFunction && cur->PrPresFunction != extra)
	/* check for extra specification in case of function rule */
	{
	  *chain = &(cur->PrNextPRule);
	  cur = cur->PrNextPRule;
	}

      if (cur)
	{
	  /* check this rule */
	  if (TstRuleContext (cur, ctxt, pres))
	    found = TRUE;
	  else
	    {
	      /* jump to next and keep track of previous */
	      *chain = &(cur->PrNextPRule);
	      cur = cur->PrNextPRule;
	    }
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
static PtrPRule     PresRuleInsert (PSchema tsch, GenericContext ctxt,
				    PRuleType pres, int extra)
#else  /* __STDC__ */
static PtrPRule     PresRuleInsert (tsch, ctxt, pres, extra)
PSchema             tsch;
GenericContext      ctxt;
PRuleType           pres;
int                 extra;
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
      if (pRule == NULL)
	{
	  TtaDisplaySimpleMessage (FATAL, LIB, TMSG_NO_MEMORY);
	  return (NULL);
	}
      pRule->PrType = pres;
      pRule->PrCond = NULL;
      pRule->PrViewNum = 1;
      pRule->PrSpecifAttr = 0;
      pRule->PrSpecifAttrSSchema = NULL;
      
      /* In case of an attribute rule, add the SurElem condition */
      if ((ctxt->attr || ctxt->class) && ctxt->attrelem != 0)
	PresRuleAddSurElemCond (pRule, ctxt->attrelem);
      /* add the ancesters conditions ... */
      for (i = 0; i < MAX_ANCESTORS; i++)
	{
	  if (ctxt->ancestors[i] != 0)
	    PresRuleAddAncestorCond (pRule, ctxt->ancestors[i], ctxt->ancestors_nb[i]);
	  else
	    break;
	}

      /* Add the order / conditions .... */
      /* chain in the rule */
      if (chain != NULL)
	{
	  pRule->PrNextPRule = *chain;
	  *chain = pRule;
	}
      return (pRule);
    }
}

/*----------------------------------------------------------------------
  PresRuleRemove : remove an existing presentation rule for a given type
  in a chain if it exists.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void     PresRuleRemove (PSchema tsch, GenericContext ctxt,
				    PRuleType pres, int extra)
#else  /* __STDC__ */
static void     PresRuleRemove (tsch, ctxt, pres, extra)
PSchema             tsch;
GenericContext      ctxt;
PRuleType           pres;
int                 extra;
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
   *									*
   *	Function used to remove all generic presentation for a given	*
   *	context.							*
   *									*
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 GenericCleanPresentation (PresentationTarget t, PresentationContext c,
					      PresentationValue v)
#else  /* !__STDC__ */
int                 GenericCleanPresentation (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;

#endif /* !__STDC__ */
{
   PtrPSchema          pSchemaPrs = (PtrPSchema) t;
   GenericContext      ctxt = (GenericContext) c;
   AttributePres      *attrs = NULL;
   AttributePres      *found = NULL;
   PtrPRule           *chain = NULL;
   PtrPRule            cur = NULL;
   PtrPRule            prev = NULL;
   PtrPRule            destroy = NULL;
   int                 nbrules = 0;
   int                 i, j, tmp, nb_ancestors;
   Document doc;
   PtrSSchema pSS;
   int elType = 0;
   int attrType = 0;
   int presBox = 0;

   doc = ctxt->doc;
   pSS = (PtrSSchema) TtaGetDocumentSSchema (doc);

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

   /*
    * select the right attribute in case of a class context.
    */
   if (ctxt->class != NULL)
     {
	attrs = pSchemaPrs->PsAttrPRule[ctxt->classattr - 1];
	nbrules = pSchemaPrs->PsNAttrPRule[ctxt->classattr - 1];
     }
   else if (ctxt->attr != 0)
     {
	attrs = pSchemaPrs->PsAttrPRule[ctxt->attr - 1];
	nbrules = pSchemaPrs->PsNAttrPRule[ctxt->attr - 1];
     }

   /*
    * locate the attribute presentation rule block concerning this
    * attribute.
    */
   for (i = 0; i < nbrules; i++)
     {
	if (ctxt->class != NULL)
	  {
	     if (!strcmp (attrs[i].ApString, ctxt->class))
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

   if (ctxt->class)
     {
	if (found == NULL)
	   return (0);
	attrType = ctxt->classattr;
	chain = &found->ApTextFirstPRule;
     }
   else if (ctxt->attr)
     {
	if (found == NULL)
	   return (0);
	attrType = ctxt->attr;
	chain = &found->ApEnumFirstPRule[ctxt->attrval];
     }
   else
     {
        elType = ctxt->type;
	chain = &pSchemaPrs->PsElemPRule[ctxt->type - 1];
     }

   /*
    * scan the chain of presentation rules looking for an existing
    * rule for this context and kind of presentation attribute.
    */
   prev = NULL;
   cur = *chain;
   while (cur != NULL)
     {
	/* test this rule */
	if (TstRuleContext (cur, ctxt, 0))
	  {
	     /* unchain and free the current presentation rule */
	     if (prev == NULL)
	       {
		  *chain = cur->PrNextPRule;
		  cur->PrNextPRule = destroy;
		  destroy = cur;
		  cur = *chain;
	       }
	     else
	       {
		  prev->PrNextPRule = cur->PrNextPRule;
		  cur->PrNextPRule = destroy;
		  destroy = cur;
		  cur = prev->PrNextPRule;
	       }
	     continue;
	  }

	/* jump to next and keep track of previous */
	prev = cur;
	cur = cur->PrNextPRule;
     }

   /*
    * All the target PRules have been removed from the list,
    * update the presentation.
    */
   if ((elType != 0) || (attrType != 0) || (presBox != 0))
       ApplyPRules (doc, pSS, elType, attrType, presBox, destroy, TRUE);

   /*
    * Free all the destroyed PRules.
    */
   while (destroy != NULL) {
       cur = destroy;
       destroy = destroy->PrNextPRule;
       FreePresentRule (cur);
   }
   return (0);
}

/*----------------------------------------------------------------------
   *									*
   *	Function used to update the drawing given context.		*
   *									*
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 GenericUpdatePresentation (PresentationTarget t, PresentationContext c,
					      PresentationValue v)
#else  /* !__STDC__ */
int                 GenericUpdatePresentation (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;

#endif /* !__STDC__ */
{
   GenericTarget  tsch = (GenericTarget) t;
   GenericContext ctxt = (GenericContext) c;
   PtrPSchema pSchemaPrs = (PtrPSchema) t;
   Document doc;
   PtrSSchema pSS;
   int elType = 0;
   int attrType = 0;
   int presBox = 0;
   PtrPRule pRule;

   if ((tsch == NULL) || (ctxt == NULL))
     return(-1);
   doc = ctxt->doc;
/*    pSS = (PtrSSchema) TtaGetDocumentSSchema (doc); */
   pSS = (PtrSSchema) ctxt->schema;
   /*
    * select the good starting point depending on the context
    */
   if (ctxt->box != 0)
     {
	presBox = ctxt->box;
	pRule = BoxRuleSearch (tsch, ctxt);
     }
   else if (ctxt->type != 0)
     {
        elType = ctxt->type;
	pRule = pSchemaPrs->PsElemPRule[ctxt->type - 1];
     }
   else if ((ctxt->attr) || (ctxt->class))
     {
	if (ctxt->attr)
	    attrType = ctxt->attr;
	else
	    attrType = ctxt->classattr;
	   
	pRule = PresAttrRuleSearch (tsch, ctxt);
     }
   else
     {
	return (-1);
     }
   if (pRule == NULL)
      return (-1);

   ApplyPRules (doc, pSS, elType, attrType, presBox, pRule, FALSE);

   return(0);
}

/*----------------------------------------------------------------------
   *									*
   *	Function used to translate various parameters between external  *
   *	and internal representation of presentation attributes.		*
   *      These function also handle setting or fetching these values     *
   *      from the internal memory representation of presentation rules.  *
   *									*
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         etoi_convert (PtrPRule rule, GenericValue val1,
				  GenericContext ctxt, int specific)
#else  /* !__STDC__ */
static void         etoi_convert (rule, val1, ctxt, specific)
PtrPRule            rule;
GenericValue        val1;
GenericContext      ctxt;
int                 specific;

#endif /* !__STDC__ */
{
   PresentationValueToPRule (val1, rule->PrType, (PRule) rule, specific);
   rule->PrViewNum = 1;
   if ((ctxt->box != 0) && (rule->PrPresMode == PresFunction)) {
      BuildBoxName(ctxt, &rule->PrPresBoxName);
   }
}

#define DRIVERG_CTXT_MAGIC1	((unsigned long) 0x23F45dA9L)

/*----------------------------------------------------------------------
 * StoreConds : store a condition list in a generic context.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         StoreConds (GenericContext ctxt, PtrCondition cond)
#else  /* __STDC__ */
static void         StoreConds (ctxt, cond)
GenericContext      ctxt;
PtrCondition        cond;
#endif /* __STDC__ */
{
  int                 i = 0;

  while (cond != NULL)
    {
      switch (cond->CoCondition)
	{
	case PcElemType:
	  ctxt->attrelem = cond->CoTypeElAttr;
	  break;
	case PcWithin:
	  ctxt->ancestors[i] = cond->CoTypeAncestor;
	  ctxt->ancestors_nb[i] = cond->CoRelation;
	  i++;
	  break;
	  /* We are far from supporting all the range of the P language */
	default:
	  break;
	}
      cond = cond->CoNextCondition;
    }
}

/*----------------------------------------------------------------------
  ApplyAllGenericContext : browse all the PSchema structure,
  creating for each PRules list the corresponding GenericContext 
  structure, and call the given handler for each one.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyAllGenericContext (Document doc, GenericTarget target,
			    GenericContextApplyHandler handler, void *param)
#else  /* __STDC__ */
void                ApplyAllGenericContext (doc, target, handler, param)
Document            doc;
GenericTarget       target;
GenericSettingsApplyHandler handler;
void               *param;
#endif /* __STDC__ */
{
  GenericContext      ctxt;
  PtrSSchema          pSchemaStr;
  PtrPSchema          pSchemaP;
  PtrPRule            rule;
  PtrCondition        prevcond, cond;
  TtAttribute        *pAt1;
  AttributePres      *pRP1;
  int                 El;
  int                 GetAttributeOfElement;
  int                 Val;

  /* set up and check the environment */
  prevcond = 0;
  pSchemaP = (PtrPSchema) target;
  
   if (doc == 0 || LoadedDocument[doc - 1] == NULL)
     return;
   else if (pSchemaP == NULL)
     return;
   else if (handler == NULL)
     return;
   ctxt = GetGenericContext (doc);
   if (ctxt == NULL)
     return;

   /* Locate the structure schema concerned (main structure or nature) */
   pSchemaStr = (PtrSSchema) TtaGetSSchema (pSchemaP->PsStructName, doc);
   if (pSchemaStr == NULL)
     pSchemaStr = (PtrSSchema) TtaGetDocumentSSchema (doc);
   
   /* Browse all rules applying directly to a given kind of element */
   for (El = 1; El <= pSchemaStr->SsNRules; El++)
     {
	rule = pSchemaP->PsElemPRule[El - 1];
	if (rule != NULL)
	  {
	    /*
	     * Ok, there is a list of PRules attached to this Element type.
	     * From this list we can derive at least one Generic context.
	     * We differentiate rules to be on differents contexts if they
	     * don't have equivalent conditions.
	     */
	    SortRulesForAccess (&pSchemaP->PsElemPRule[El - 1]);
	    rule = pSchemaP->PsElemPRule[El - 1];
	    while (rule != NULL)
	      {
		cond = rule->PrCond;
		if ((rule == pSchemaP->PsElemPRule[El - 1]) ||
		    (CompareCondLists (prevcond, rule->PrCond)))
		  {
		    /* this is a new context */
		    prevcond = cond;
		    CleanGenericContext (ctxt);
		    StoreConds (ctxt, cond);
		    ctxt->type = El;
		    ctxt->schema = (SSchema) pSchemaStr;
		    
		    /* call the handler provided by the user */
		    ctxt->magic1 = DRIVERG_CTXT_MAGIC1;
		    ctxt->magic2 = (unsigned long) rule;
		    handler (target, ctxt, param);
		  }
		rule = rule->PrNextPRule;
	      }
	    SortRulesForDisplay (&pSchemaP->PsElemPRule[El - 1]);
	  }
     }

   /* browse all rules applied to element having attributes */
   if (pSchemaStr->SsNAttributes > 0)
     {
       for (GetAttributeOfElement = 1; GetAttributeOfElement <= pSchemaStr->SsNAttributes; GetAttributeOfElement++)
	 {
	   pAt1 = &pSchemaStr->SsAttribute[GetAttributeOfElement - 1];
	   pRP1 = pSchemaP->PsAttrPRule[GetAttributeOfElement - 1];
	   if (pAt1 == NULL)
	     continue;
	   if (pRP1 == NULL)
	     continue;
	   if (pAt1->AttrType == AtTextAttr)
	     {
	       while (pRP1 != NULL)
		 {
		   rule = pRP1->ApTextFirstPRule;
		   if (rule == NULL)
		     {
		       pRP1 = pRP1->ApNextAttrPres;
		       continue;
		     }
		   
		   /*
		    * Ok, there is a list of PRules attached to this
		    * text attribute.
		    */
		   SortRulesForAccess (&pRP1->ApTextFirstPRule);
		   rule = pRP1->ApTextFirstPRule;
		   while (rule != NULL)
		     {
		       cond = rule->PrCond;
		       if ((rule == pRP1->ApTextFirstPRule) ||
			   (CompareCondLists (prevcond, cond)))
			 {			   
			   /* this is a new context */
			   prevcond = cond;
			   CleanGenericContext (ctxt);
			   StoreConds (ctxt, cond);
			   ctxt->class = pRP1->ApString;
			   ctxt->classattr = GetAttributeOfElement;
			   ctxt->schema = (SSchema) pSchemaStr;

			   /* call the handler provided by the user */
			   ctxt->magic1 = DRIVERG_CTXT_MAGIC1;
			   ctxt->magic2 = (unsigned long) rule;
			   handler (target, ctxt, param);
			 }
		       rule = rule->PrNextPRule;
		     }
		   SortRulesForDisplay (&pRP1->ApTextFirstPRule);
		   pRP1 = pRP1->ApNextAttrPres;
		 }
	     }
	   else if (pAt1->AttrType == AtEnumAttr)
	     {
	       for (Val = 1; Val <= pAt1->AttrNEnumValues; Val++)
		 {
		   rule = pRP1->ApEnumFirstPRule[Val];
		   if (rule == NULL)
		     continue;

		   /*
		    * Ok, there is a list of PRules attached to this
		    * value attribute.
		    */
		   SortRulesForAccess (&pRP1->ApEnumFirstPRule[Val]);
		   rule = pRP1->ApEnumFirstPRule[Val];
		   while (rule != NULL)
		     {
		       cond = rule->PrCond;
		       if ((rule == pRP1->ApEnumFirstPRule[Val]) ||
			   (CompareCondLists (prevcond, cond)))
			 {
			   /* this is a new context */
			   prevcond = cond;
			   CleanGenericContext (ctxt);
			   StoreConds (ctxt, cond);
			   ctxt->attr = GetAttributeOfElement;
			   ctxt->attrval = Val;
			   ctxt->schema = (SSchema) pSchemaStr;
			   
			   /* call the handler provided by the user. */
			   ctxt->magic1 = DRIVERG_CTXT_MAGIC1;
			   ctxt->magic2 = (unsigned long) rule;
			   handler (target, ctxt, param);
			 }
		       rule = rule->PrNextPRule;
		     }
		   SortRulesForDisplay (&pRP1->ApEnumFirstPRule[Val]);
		   pRP1 = pRP1->ApNextAttrPres;
		 }
	     }
	 }
     }

   /* Clean up things */
   FreeGenericContext (ctxt);
}

/*----------------------------------------------------------------------
  ApplyAllGenericSettings : browse all the PRules structures,
  associated to the corresponding GenericContext 
  structure, and call the given handler for each one.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ApplyAllGenericSettings (GenericTarget target,
		   GenericContext ctxt, GenericSettingsApplyHandler handler,
					     void *param)
#else  /* __STDC__ */
void                ApplyAllGenericSettings (target, ctxt, handler, param)
GenericTarget       target;
GenericContext      ctxt;
GenericSettingsApplyHandler handler;
void               *param;
#endif /* __STDC__ */
{
   PtrPSchema          pSchemaP;
   PtrPRule            rule = NULL;
   PtrCondition        prevcond, cond;
   PresentationSettingBlock setting;

   /* set up and check the environment */
   prevcond = 0;
   pSchemaP = (PtrPSchema) target;
   if (pSchemaP == NULL)
      return;
   if (ctxt == NULL)
      return;
   if (handler == NULL)
      return;

   /*
    * if magic1 is properly set, this mean that we don't need to
    * search for the first appropriate PRule, since it's given
    * directly in magic2.
    */
   if (ctxt->magic1 == DRIVERG_CTXT_MAGIC1)
     {
	rule = (PtrPRule) ctxt->magic2;
	/*
	 * sanity check on the value contained in magic2.
	 */
	if ((rule != NULL) && (rule->PrType >= PtVisibility) &&
	    (rule->PrType <= PtPictInfo))
	   goto found_first_prule;
     }

   /*
    * Search the first PRule corresponding to the given context.
    */

 found_first_prule:
   /*
    * for each rule corresponding to the same context i.e. identical
    * conditions, create the corresponding PresentationSetting and
    * call the user handler.
    */
   if (rule != NULL)
      prevcond = rule->PrCond;
   while (rule != NULL)
     {
	cond = rule->PrCond;
	if ((cond != prevcond) && (CompareCondLists (prevcond, cond)))
	   break;

	/*
	 * fill in the PresentationSetting and call the handler.
	 */
	if (rule->PrPresMode == PresFunction)
	    PRuleToPresentationSetting ((PRule) rule, &setting, 
	                                rule->PrPresFunction);
	else
	    PRuleToPresentationSetting ((PRule) rule, &setting, 0);

	/*
	 * need to do some tweaking in the case of BackgroudPicture
	 */
	if (setting.type == DRIVERP_BGIMAGE) {
            int cst = setting.value.typed_data.value;

            setting.value.pointer = &pSchemaP->PsConstant[cst-1].PdString[0];
	}

	handler (target, ctxt, &setting, param);
	rule = rule->PrNextPRule;
     }


}

/************************************************************************
 *									*
 *	Macro's used to generate presentations routines			*
 *      These heavily rely on the token-pasting mechanism provided by   *
 *      the C preprocessor. The string a##b is replaced by the string   *
 *      "ab", but this is done after the macro is expanded.             *
 *      This mecanism allows to avoid a lot of typing, errors and keep  *
 *      the code compact at the price of a loss of readability.         *
 *      On old fashionned preprocessor (pre-Ansi) the token pasting was *
 *      a side effect of the preprocessor implementation on empty       *
 *      comments. In this case we use a+slash+star+star+slash+b to      *
 *      produce the same string "ab".					*
 *									*
 ************************************************************************/

#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP) || defined(WWW_MSWINDOWS)

#define GENERIC_FUNCS(genre,name)					\
									\
int GenericSet##name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue v)				\
{									\
    GenericTarget  tsch = (GenericTarget) t;				\
    GenericContext cont = (GenericContext) c;				\
    GenericValue   val = /* (GenericValue) - EGP */ v;			\
    PtrPRule   rule;							\
									\
    if (cont->destroy) {						\
        PresRuleRemove(tsch, cont,Pt##genre,0);				\
	return(0);							\
    }									\
    rule = PresRuleInsert(tsch, cont,Pt##genre,0);			\
    if (rule == NULL) return(-1);					\
    etoi_convert(rule,val,cont,0);					\
    return(0);								\
}									\
									\
int GenericGet##name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue *v)				\
{									\
    GenericTarget  tsch = (GenericTarget) t;				\
    GenericContext cont = (GenericContext) c;				\
    GenericValue   *val = (GenericValue *) v;				\
    PtrPRule        rule, *chain;      					\
									\
    rule = PresRuleSearch(tsch, cont, Pt##genre, 0, &chain);		\
    if (rule == NULL) return(-1);					\
    val->typed_data.value = rule->PrIntValue;				\
    val->typed_data.unit = DRIVERP_UNIT_REL;				\
    return(0);								\
}									\

#define GENERIC_FUNCS2(genre,category,name)				\
									\
int GenericSet##name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue v)				\
{									\
    GenericTarget  tsch = (GenericTarget) t;				\
    GenericContext cont = (GenericContext) c;				\
    GenericValue   val = /* (GenericValue) - EGP */ v;			\
    PtrPRule   rule;							\
									\
    if (cont->destroy) {						\
        PresRuleRemove(tsch, cont,Pt##genre,category);			\
	return(0);							\
    }									\
    rule = PresRuleInsert(tsch, cont,Pt##genre, category);		\
    if (rule == NULL) return(-1);					\
    etoi_convert(rule,val,cont,category);				\
    return(0);								\
}									\
									\
int GenericGet##name(PresentationTarget t, PresentationContext c,	\
                      PresentationValue *v)				\
{									\
    GenericTarget  tsch = (GenericTarget) t;				\
    GenericContext cont = (GenericContext) c;				\
    GenericValue   *val = (GenericValue *) v;				\
    PtrPRule        rule, *chain;					\
									\
    rule = PresRuleSearch(tsch, cont, Pt##genre, category, &chain);	\
    if (rule == NULL) return(-1);					\
    val->typed_data.value = rule->PrIntValue;				\
    val->typed_data.unit = DRIVERP_UNIT_REL;				\
    return(0);								\
}									\

#else  /* ! __STDC__ i.e. token-pasting is made the old way ! */

#define GENERIC_FUNCS(type,name)					\
									\
int GenericSet/**/name(t,c,v)						\
    PresentationTarget t;						\
    PresentationContext c;						\
    PresentationValue v;						\
{									\
    GenericTarget  tsch = (GenericTarget) t;				\
    GenericContext cont = (GenericContext) c;				\
    GenericValue   val = (GenericValue) v;				\
    PtrPRule   rule;							\
									\
    if (cont->destroy) {						\
        PresRuleRemove(tsch, cont,Pt/**/genre,0);			\
	return(0);							\
    }									\
    rule = PresRuleInsert(tsch, cont,Pt/**/genre, 0);			\
    if (rule == NULL) return(-1);					\
    etoi_convert(rule,val,cont,0);					\
    return(0);								\
}									\
									\
int GenericGet/**/name(t,c,v)						\
    PresentationTarget t;						\
    PresentationContext c;						\
    PresentationValue *v;						\
{									\
    GenericTarget  tsch = (GenericTarget) t;				\
    GenericContext cont = (GenericContext) c;				\
    GenericValue   *val = (GenericValue *) v;				\
    PtrPRule        rule, *chain;					\
									\
    rule = PresRuleSearch(tsch, cont, Pt/**/genre, 0, &chain);		\
    if (rule == NULL) return(-1);					\
    val->typed_data.value = rule->PrIntValue;				\
    val->typed_data.unit = DRIVERP_UNIT_REL;				\
    return(0);								\
}									\

#define GENERIC_FUNCS2(type,category,name)				\
									\
int GenericSet/**/name(t,c,v)						\
    PresentationTarget t;						\
    PresentationContext c;						\
    PresentationValue v;						\
{									\
    GenericTarget  tsch = (GenericTarget) t;				\
    GenericContext cont = (GenericContext) c;				\
    GenericValue   val = (GenericValue) v;				\
    PtrPRule   rule;							\
									\
    if (cont->destroy) {						\
        PresRuleRemove(tsch, cont,Pt/**/genre, category);		\
	return(0);							\
    }									\
    rule = PresRuleInsert(tsch, cont,Pt/**/genre, category);		\
    if (rule == NULL) return(-1);					\
    etoi_convert(rule,val,cont,category);				\
    return(0);								\
}									\
									\
int GenericGet/**/name(t,c,v)						\
    PresentationTarget t;						\
    PresentationContext c;						\
    PresentationValue *v;						\
{									\
    GenericTarget  tsch = (GenericTarget) t;				\
    GenericContext cont = (GenericContext) c;				\
    GenericValue   *val = (GenericValue *) v;				\
    PtrPRule        rule, *chain;	       				\
									\
    rule = PresRuleSearch(tsch, cont, Pt/**/genre, category, &chain);	\
    if (rule == NULL) return(-1);					\
    val->typed_data.value = rule->PrIntValue;				\
    val->typed_data.unit = DRIVERP_UNIT_REL;				\
    return(0);								\
}									\

#endif /* __STDC__ */

/*----------------------------------------------------------------------
   *									*
   *	generation of most common presentations routines		*
   *									*
  ----------------------------------------------------------------------*/

GENERIC_FUNCS (Foreground, ForegroundColor)
GENERIC_FUNCS (Background, BackgroundColor)
GENERIC_FUNCS (Size, FontSize)
GENERIC_FUNCS (Style, FontStyle)
GENERIC_FUNCS (Adjust, Alignment)
GENERIC_FUNCS (Indent, Indent)
GENERIC_FUNCS (Justify, Justification)
GENERIC_FUNCS (Hyphenate, Hyphenation)
GENERIC_FUNCS (Underline, TextUnderlining)
GENERIC_FUNCS (FillPattern, FillPattern)
GENERIC_FUNCS (Font, FontFamily)
GENERIC_FUNCS (LineSpacing, LineSpacing)
GENERIC_FUNCS2 (Function, FnLine, InLine)
GENERIC_FUNCS2 (Function, FnShowBox, ShowBox)
GENERIC_FUNCS2 (Function, FnPictureMode, PictureMode)

/*----------------------------------------------------------------------
   *									*
   *	a few presentations routines still need to be hand-coded	*
   *									*
  ----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 GenericSetBox (PresentationTarget t, PresentationContext c,
				   PresentationValue v)
#else
int                 GenericSetBox (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;
#endif
{
   GenericTarget       tsch = (GenericTarget) t;
   GenericContext      ctxt = (GenericContext) c;
   PtrPRule            rule;
   int                 box;

   if (ctxt->destroy) {
       return(0);
   }
   BoxRuleInsert (tsch, ctxt);
   box = ctxt->box;
   ctxt->box = 0;
   rule = PresRuleInsert (tsch, ctxt, PtFunction, FnCreateEnclosing);
   if (rule == NULL)
      return (-1);
   ctxt->box = box;
   etoi_convert (rule, v, ctxt, FnCreateEnclosing);
   return (0);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 GenericGetBox (PresentationTarget t, PresentationContext c,
				   PresentationValue * v)
#else
int                 GenericGetBox (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue  *v;
#endif
{
   /******************
   GenericTarget       tsch = (GenericTarget) t;
   GenericContext      ctxt = (GenericContext) c;

   PtrPRule   rule;

   (void *) BoxRuleSearch (tsch, ctxt);
    ******************/
   return (0);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int      GenericSetBgImage (PresentationTarget t, PresentationContext c,
			    PresentationValue v)
#else
int      GenericSetBgImage (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;
#endif
{
   GenericTarget       tsch = (GenericTarget) t;
   GenericContext      ctxt = (GenericContext) c;
   PtrPRule            rule;
   int                 cst;

   if (ctxt->destroy) {
       PresRuleRemove (tsch, ctxt, PtFunction, FnBackgroundPicture);
       return(0);
   }
   cst = PresConstInsert (tsch, v.pointer);
   rule = PresRuleInsert (tsch, ctxt, PtFunction, FnBackgroundPicture);
   if (rule == NULL)
      return (-1);
   v.typed_data.unit = DRIVERP_UNIT_REL;
   v.typed_data.value = cst;
   etoi_convert (rule, v, ctxt, FnBackgroundPicture);
   return (0);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int      GenericGetBgImage (PresentationTarget t, PresentationContext c,
			    PresentationValue * v)
#else
int      GenericGetBgImage (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue  *v;
#endif
{
   GenericTarget       tsch = (GenericTarget) t;
   PtrPSchema          pSchemaPrs = (PtrPSchema) tsch;
   GenericContext      ctxt = (GenericContext) c;
   PtrPRule            rule, *chain;
   PresentationValue   val;
   int                 cst;

   rule = PresRuleSearch (tsch, ctxt, PtFunction, FnBackgroundPicture, &chain);
   if (rule == NULL)
      return (-1);
   val = PRuleToPresentationValue ((PRule) rule);
   cst = val.typed_data.unit;
   v->pointer = &pSchemaPrs->PsConstant[cst-1].PdString[0];
   return (0);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int      GenericSetWidth (PresentationTarget t, PresentationContext c,
			  PresentationValue v)
#else
int      GenericSetWidth (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;
#endif
{
   return (0);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int      GenericSetVPos (PresentationTarget t, PresentationContext c,
			 PresentationValue v)
#else
int      GenericSetVPos (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;
#endif
{
   GenericTarget       tsch = (GenericTarget) t;
   GenericContext      cont = (GenericContext) c;
   GenericValue        val = /* (GenericValue) - EGP */ v;
   PtrPRule            rule;

   if (cont->destroy) {
       PresRuleRemove (tsch, cont, PtVertPos, 0);
       return(0);
   }
   rule = PresRuleInsert (tsch, cont, PtVertPos, 0);
   if (rule == NULL)
      return (-1);
   etoi_convert (rule, val, cont, 0);
   return (0);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int      GenericGetVPos (PresentationTarget t, PresentationContext c,
			 PresentationValue v)
#else
int      GenericGetVPos (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;
#endif
{
   return (0);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int      GenericGetHPos (PresentationTarget t, PresentationContext c,
			 PresentationValue v)
#else
int      GenericGetHPos (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;
#endif
{
   return (0);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int      GenericSetHPos (PresentationTarget t, PresentationContext c,
			 PresentationValue v)
#else
int      GenericSetHPos (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;
#endif
{
   return (0);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int      GenericGetHeight (PresentationTarget t, PresentationContext c,
			   PresentationValue v)
#else
int      GenericGetHeight (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;
#endif
{
   return (0);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int      GenericSetHeight (PresentationTarget t, PresentationContext c,
			   PresentationValue v)
#else
int      GenericSetHeight (t, c, v)
PresentationTarget  t;
PresentationContext c;
PresentationValue   v;
#endif
{
   return (0);
}

/*----------------------------------------------------------------------
 *	the strategy block for the generic presentation driver
  ----------------------------------------------------------------------*/
PresentationStrategy GenericStrategy =
{
   (PresentationSetFunction) GenericCleanPresentation,
   (PresentationSetFunction) GenericUpdatePresentation,

   (PresentationGetFunction) GenericGetForegroundColor,
   (PresentationSetFunction) GenericSetForegroundColor,

   (PresentationGetFunction) GenericGetBackgroundColor,
   (PresentationSetFunction) GenericSetBackgroundColor,

   (PresentationGetFunction) GenericGetFontSize,
   (PresentationSetFunction) GenericSetFontSize,

   (PresentationGetFunction) GenericGetFontStyle,
   (PresentationSetFunction) GenericSetFontStyle,

   (PresentationGetFunction) GenericGetFontFamily,
   (PresentationSetFunction) GenericSetFontFamily,

   (PresentationGetFunction) GenericGetTextUnderlining,
   (PresentationSetFunction) GenericSetTextUnderlining,

   (PresentationGetFunction) GenericGetAlignment,
   (PresentationSetFunction) GenericSetAlignment,

   (PresentationGetFunction) GenericGetLineSpacing,
   (PresentationSetFunction) GenericSetLineSpacing,

   (PresentationGetFunction) GenericGetIndent,
   (PresentationSetFunction) GenericSetIndent,

   (PresentationGetFunction) GenericGetJustification,
   (PresentationSetFunction) GenericSetJustification,

   (PresentationGetFunction) GenericGetHyphenation,
   (PresentationSetFunction) GenericSetHyphenation,

   (PresentationGetFunction) GenericGetFillPattern,
   (PresentationSetFunction) GenericSetFillPattern,

   (PresentationGetFunction) GenericGetVPos,
   (PresentationSetFunction) GenericSetVPos,

   (PresentationGetFunction) GenericGetHPos,
   (PresentationSetFunction) GenericSetHPos,

   NULL, /* (PresentationGetFunction) GenericGetHeight, */
   NULL, /* (PresentationSetFunction) GenericSetHeight, */

   NULL, /* (PresentationGetFunction) GenericGetRelHeight, */
   NULL, /* (PresentationSetFunction) GenericSetRelHeight, */

   NULL, /* (PresentationGetFunction) GenericGetWidth, */
   NULL, /* (PresentationSetFunction) GenericSetWidth, */

   NULL, /* (PresentationGetFunction) GenericGetRelWidth, */
   NULL, /* (PresentationSetFunction) GenericSetRelWidth, */

   (PresentationGetFunction) GenericGetInLine,
   (PresentationSetFunction) GenericSetInLine,

   NULL, /* (PresentationGetFunction) GenericGetShow, */
   NULL, /* (PresentationSetFunction) GenericSetShow, */

   (PresentationGetFunction) GenericGetBox,
   (PresentationSetFunction) GenericSetBox,

   (PresentationGetFunction) GenericGetShowBox,
   (PresentationSetFunction) GenericSetShowBox,

   (PresentationGetFunction) GenericGetBgImage,
   (PresentationSetFunction) GenericSetBgImage,

   (PresentationGetFunction) GenericGetPictureMode,
   (PresentationSetFunction) GenericSetPictureMode,
};
