
/* -- Copyright (c) 1996 Inria -- All rights reserved -- */

/*

   Some functions for handling structure rules

   IV : Septembre 92 adaptation Tool Kit

*/

#include "thot_sys.h"
#include "libmsg.h"
#include "message.h"
#include "constmedia.h"
#include "functions.h"
#include "typemedia.h"
#include "view.h"
#include "document.h"
#include "appdialogue.h"

#define EXPORT extern
#include "edit.var"
#include "environ.var"
#include "appdialogue.var"

#include "arbabs.f"
#include "environ.f"
#include "except.f"
#include "memory.f"
#include "schemastr.f"
#include "structure.f"

/* ---------------------------------------------------------------------- */
/* |    CreateDocument acquiert et initialise un contexte de document.    | */
/* |            Au retour, pDoc contient un pointeur sur le contexte de | */
/* |            document.                                               | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                CreateDocument (PtrDocument * pDoc)

#else  /* __STDC__ */
void                CreateDocument (pDoc)
PtrDocument        *pDoc;

#endif /* __STDC__ */
{
   int                 doc;

   /* cherche un pointeur de descripteur de document libre */
   doc = 0;
   while (TabDocuments[doc] != NULL && doc < MAX_DOCUMENTS-1)
      doc++;
   if (TabDocuments[doc] != NULL)
     {
	TtaDisplaySimpleMessage (LIB, INFO, LIB_TOO_MANY_DOCS);
	*pDoc = NULL;
     }
   else
     {
	/* acquiert un descripteur de document */
	GetDocument (&TabDocuments[doc]);
	*pDoc = TabDocuments[doc];
	/* initialise le mode d'affichage */
	documentDisplayMode[doc] = DisplayImmediately;
	(*pDoc)->DocBackUpInterval = CurSaveInterval;
     }
}


/* ---------------------------------------------------------------------- */
/* |    GetSRuleFromName cherche dans le schema de structure pSS (et dans les    | */
/* |            extensions et schemas de structures utilises comme      | */
/* |            natures par ce schema), une regle de nom typeName       | */
/* |            Retourne dans pSS le schema contenant la                | */
/* |            regle trouvee et dans typeNum le numero de la regle     | */
/* |            trouvee dans ce schema. typeNum vaut zero si le nom     | */
/* |            n'est pas trouve'.                                      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                GetSRuleFromName (int * typeNum, PtrSSchema * pSS, Name typeName)

#else  /* __STDC__ */
void                GetSRuleFromName (typeNum, pSS, typeName)
int        *typeNum;
PtrSSchema       *pSS;
Name                 typeName;

#endif /* __STDC__ */

{
   int         ruleNum;
   PtrSSchema        pSSch;

   /* on n'a pas encore trouve' */
   *typeNum = 0;
   /* pointeur sur le schema ou l'on cherche */
   pSSch = *pSS;
   ruleNum = 0;
   if (pSSch != NULL)
     {
	do
	   /* cherche dans les regles de structure du schema */
	  {
	     /* si c'est une regle de changement de nature, on prendra la
		regle racine de la nature */
	     if (strcmp (typeName, pSSch->SsRule[ruleNum].SrName) == 0
		 && pSSch->SsRule[ruleNum].SrConstruct != CsNatureSchema)
		/* trouve' */
	       {
		  *typeNum = ruleNum+1;
		  *pSS = pSSch;
	       }
	     else if (pSSch->SsRule[ruleNum].SrConstruct == CsNatureSchema)
		/* une nature, cherche dans son schema de structure */
	       {
		  *pSS = pSSch->SsRule[ruleNum].SrSSchemaNat;
		  GetSRuleFromName (typeNum, pSS, typeName);
	       }
	     ruleNum++;
	  }
	while (*typeNum == 0 && ruleNum < pSSch->SsNRules);
	if (*typeNum == 0)
	   /* on n'a pas trouve' dans le schema ni dans ses natures, on cherche */
	   /* dans les extensions du schema */
	   if (pSSch->SsNextExtens != NULL)
	     {
		*pSS = pSSch->SsNextExtens;
		GetSRuleFromName (typeNum, pSS, typeName);
	     }
     }
}


/* ---------------------------------------------------------------------- */
/* |	GetTypeNumIdentity						| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int         GetTypeNumIdentity (int typeNum, PtrSSchema pSS)

#else  /* __STDC__ */
int         GetTypeNumIdentity (typeNum, pSS)
int         typeNum;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   while (pSS->SsRule[typeNum - 1].SrConstruct == CsIdentity)
      typeNum = pSS->SsRule[typeNum - 1].SrIdentRule;
   return typeNum;
}


/* ---------------------------------------------------------------------- */
/* |	SameSRules teste si les regles de numero typeNum1 et		| */
/* |	typeNum2 dans les schemas de structure pSS1 et pSS2 sont egales	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             SameSRules (int typeNum1, PtrSSchema pSS1, int typeNum2, PtrSSchema pSS2)

#else  /* __STDC__ */
boolean             SameSRules (typeNum1, pSS1, typeNum2, pSS2)
int         typeNum1;
PtrSSchema        pSS1;
int         typeNum2;
PtrSSchema        pSS2;

#endif /* __STDC__ */

{
   boolean             ret;

   ret = False;
   if (typeNum1 == typeNum2)
      if (typeNum2 <= MAX_BASIC_TYPE)
	 /* les types de base sont les memes dans tous les schemas */
	 ret = True;
      else if (pSS1->SsCode == pSS2->SsCode)
	 if (typeNum1 < pSS1->SsFirstDynNature || pSS1->SsFirstDynNature == 0)
	    /* ce n'est pas une regle ajoutee par l'editeur pour une */
	    /* nature chargee dynamiquement */
	    ret = True;
   return ret;
}

/* ---------------------------------------------------------------------- */
/* | EquivalentSRules retourne vrai si la regle 1 (numero typeNum1 dans le	| */
/* |	schema de structure pointe par pSS1) peut conduire a la regle 2	| */
/* |	(numero typeNum2 dans le schema de structure pointe par pSS2)	| */
/* |	uniquement par des regles d'CsIdentity, de nature et de CsChoice.	| */
/* |	pEl est l'element d'arbre abstrait pour lequel on travaille.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             EquivalentSRules (int typeNum1, PtrSSchema pSS1, int typeNum2, PtrSSchema pSS2, PtrElement pEl)

#else  /* __STDC__ */
boolean             EquivalentSRules (typeNum1, pSS1, typeNum2, pSS2, pEl)
int         typeNum1;
PtrSSchema        pSS1;
int         typeNum2;
PtrSSchema        pSS2;
PtrElement          pEl;

#endif /* __STDC__ */

{
   boolean             ret;
   int                 i;
   boolean             test;
   Name                 name;
   SRule              *pSRule;
   PtrElement          pAsc, pPrev;
   boolean             ok;
   PtrSSchema        pSSch;


   if (typeNum1 == 0 || typeNum2 == 0 || pSS1 == NULL || pSS2 == NULL)
      ret = False;
   else if (SameSRules (typeNum1, pSS1, typeNum2, pSS2))
      ret = True;
   else
     {
	ret = False;
	test = True;
	pSRule = &pSS1->SsRule[typeNum1 - 1];
	if (pSRule->SrAssocElem)
	   test = False;
	if (pSRule->SrRecursive)
	   if (pSRule->SrRecursDone)
	      test = False;
	   else
	      pSRule->SrRecursDone = True;
	if (test)
	   switch (pSRule->SrConstruct)
		 {
		    case CsIdentity:
		       if (pSRule->SrIdentRule > MAX_BASIC_TYPE)
			  ret = EquivalentSRules (pSRule->SrIdentRule, pSS1, typeNum2, pSS2, pEl);
		       break;
		    case CsNatureSchema:
		       if (pSRule->SrSSchemaNat == NULL)
			  /* structure schema of nature is not loaded. Load it */
			 {
			    name[0] = '\0';
			    LoadNat (pSS1, name, typeNum1);
			 }
		       if (pSRule->SrSSchemaNat == NULL)
			  /* structure schema loading failed */
			  ret = False;
		       else
			  ret = SameSRules (pSRule->SrSSchemaNat->SsRootElem,
					       pSRule->SrSSchemaNat, typeNum2, pSS2);
		       break;
		    case CsChoice:
		       if (pSRule->SrNChoices == -1)
			  /* equivalent only if external nature */
			  if (typeNum2 == pSS2->SsRootElem)
			     ret = True;
			  else
			     ret = False;
		       else if (pSRule->SrNChoices == 0)
			  /* UNIT rule */
			  /* equivalent only if basic type or exported unit */
			 {
			    if (typeNum2 <= MAX_BASIC_TYPE)
			       /* basic type */
			       ret = True;
			    else if (pSS2->SsRule[typeNum2 - 1].SrUnitElem)
			       /* exported unit */
			       ret = True;
			    else
			       /* searches all units in the structure schema
				  and in all its extensions */
			      {
				 pSSch = pSS1;
				 /* if it's an extension, return to initial
				    schema for handling all extensions */
				 while (pSSch->SsPrevExtens != NULL)
				    pSSch = pSSch->SsPrevExtens;
				 /* process all extensions */
				 do
				   {
				      /* search units defined in this schema */
				      i = 0;
				      do
					 if (pSSch->SsRule[i++].SrUnitElem)
					    ret = EquivalentSRules (i, pSSch, typeNum2, pSS2, pEl);
				      while (!ret && i < pSSch->SsNRules);
				      if (!ret)
					 pSSch = pSSch->SsNextExtens;
				   }
				 while (!ret && pSSch != NULL);
			      }
			    if (!ret && pEl != NULL)
			       /* not found. Is it an unit defined in the */
			       /* schema of an ancestor element? */
			      {
				 pAsc = pEl;
				 pPrev = NULL;
				 while (pAsc != NULL && !ret)
				   {
				      if (pPrev == NULL)
					 ok = True;
				      else
					 ok = pAsc->ElSructSchema != pPrev->ElSructSchema;
				      if (ok)
					 /* the ancestor belongs to a different
					    structure schema */
					{
					   pSSch = pAsc->ElSructSchema;
					   /* if it's an extension, return
					     to the initial schema for handling
					     all other extensions */
					   while (pSSch->SsPrevExtens != NULL)
					      pSSch = pSSch->SsPrevExtens;
					   do
					     {
						for (i = 0; i < pSSch->SsNRules && !ret; i++)
						   if (pSSch->SsRule[i].SrUnitElem)
						      ret = EquivalentSRules (i+1, pSSch, typeNum2, pSS2, pAsc);
						if (!ret)
						   pSSch = pSSch->SsNextExtens;
					     }
					   while (!ret && pSSch != NULL);
					}
				      /* next ancestor */
				      pPrev = pAsc;
				      pAsc = pAsc->ElParent;
				   }
			      }
			 }
		       else
			  /* choice. Check all options */
			 {
			    i = 0;
			    do
			      {
				 if (pSS1->SsRule[pSRule->SrChoice[i] - 1].SrConstruct
				     == CsNatureSchema ||
				 /* current option is a nature */
				     pSS1->SsRule[pSRule->SrChoice[i] - 1].SrConstruct
				     == CsChoice)
				    /* current option is a choice */
				    ret = EquivalentSRules (pSRule->SrChoice[i], pSS1,
						      typeNum2, pSS2, pEl);
				 else
				    ret = SameSRules (pSRule->SrChoice[i], pSS1,
							 typeNum2, pSS2);
				 i++;
			      }
			    while (!ret && i < pSRule->SrNChoices);
			 }
		       break;
		    default:
		       break;
		 }
	if (pSRule->SrRecursive && pSRule->SrRecursDone && test)
	   pSRule->SrRecursDone = False;
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    ListRuleOfElem  si l'element de type (typeNum, pSS) peut	| */
/* |            etre un fils d'un element CsList, retourne le numero de  | */
/* |            la regle definissant cet element CsList.                 | */
/* |            Retourne 0 sinon.                                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int         ListRuleOfElem (int typeNum, PtrSSchema pSS)

#else  /* __STDC__ */
int         ListRuleOfElem (typeNum, pSS)
int         typeNum;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   int         rule, ret;

   ret = 0;
   for (rule = 0; ret == 0 && rule < pSS->SsNRules; rule++)
      if (pSS->SsRule[rule].SrConstruct == CsList)
	 if (EquivalentSRules (pSS->SsRule[rule].SrListItem, pSS, typeNum, pSS,
			 NULL))
	    ret = rule+1;
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    AggregateRuleOfElem    si l'element de type (typeNum, pSS) peut	| */
/* |            etre un fils d'un element CsAggregate, retourne le numero   | */
/* |            de la regle definissant cet element CsAggregate.            | */
/* |            Retourne 0 sinon.                                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int         AggregateRuleOfElem (int typeNum, PtrSSchema pSS)

#else  /* __STDC__ */
int         AggregateRuleOfElem (typeNum, pSS)
int         typeNum;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   int         rule, comp, ret;
   SRule              *pRegle;

   ret = 0;
   for (rule = 0; ret == 0 && rule < pSS->SsNRules; rule++)
     {
	pRegle = &pSS->SsRule[rule];
	if (pRegle->SrConstruct == CsAggregate || pRegle->SrConstruct == CsUnorderedAggregate)
	   for (comp = 0; ret == 0 && comp < pRegle->SrNComponents; comp++)
	      if (EquivalentSRules (pRegle->SrComponent[comp], pSS, typeNum, pSS, NULL))
		 ret = rule+1;
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |	ExcludedType verifie si l'element pointe par pEl et ses ascendants| */
/* |	excluent le type d'element de numero typeNum defini dans le	| */
/* |	schema de structure pSS.					| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             ExcludedType (PtrElement pEl, int typeNum, PtrSSchema pSS)

#else  /* __STDC__ */
boolean             ExcludedType (pEl, typeNum, pSS)
PtrElement          pEl;
int                 typeNum;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   SRule              *pRule;
   int                 i;
   PtrSSchema        pSSasc, pSSExt;
   boolean             StrSchOK;
   boolean             ret;

   /* a priori ce type n'est pas exclus */
   ret = False;
   /* s'il s'agit d'un type de base, peu importe le schema de */
   /* structure auquel il appartient */
   if (typeNum <= MAX_BASIC_TYPE)
      pSS = NULL;
   /* examine la regle de structure de l'element et de ses ascendants */
   while (pEl != NULL && !ret)
     {
	/* accede a la regle de structure de l'element */
	pSSasc = pEl->ElSructSchema;
	pRule = &pSSasc->SsRule[pEl->ElTypeNumber - 1];
	pSSExt = NULL;
	do
	  {
	     if (pRule != NULL)
	       {
		  if (pSS == NULL)
		     /* n'importe quel schema de structure convient */
		     StrSchOK = True;
		  else
		     /* On compare les codes identifiant les schemas de structure */
		     StrSchOK = pSSasc->SsCode == pSS->SsCode;
		  if (StrSchOK)
		     /* les schemas de structure correspondent */
		    {

		       /* examine toutes les exclusions definies dans la regle */
		       for (i = 0; i < pRule->SrNExclusions; i++)
			  if (pRule->SrExclusion[i] == typeNum)
			     /* le type demande' fait partie des exclusions */
			     ret = True;
			  else
			     /* le 2eme element d'une paire est exclus si le */
			     /* 1er est exclus */
			     if (pRule->SrExclusion[i] + 1 == typeNum)
			       /* le type precedent est exclus */
			       if (pSS->SsRule[typeNum - 1].SrConstruct == CsPairedElement)
				  /* l'element est un membre de paire */
				  if (!pSS->SsRule[typeNum - 1].SrFirstOfPair)
				    /* c'est le 2eme membre de la paire */
				    ret = True;
		    }
	       }
	     /* passe a l'extension de schema suivante */
	     if (pSSExt == NULL)
		pSSExt = pSSasc->SsNextExtens;
	     else
		pSSExt = pSSExt->SsNextExtens;
	     if (pSSExt != NULL)
		/* il y a encore un schema d'extension */
	       {
		  pSSasc = pSSExt;
		  /* cherche dans ce schema la regle d'extension pour l'ascendant */
		  pRule = ExtensionRule (pEl->ElSructSchema, pEl->ElTypeNumber, pSSExt);
	       }
	  }
	while (pSSExt != NULL && !ret);
	pEl = pEl->ElParent;	/* passe a l'ascendant */
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    AllowedIncludedElem evalue si un element de type typeNum (defini | */
/* |    dans le schema de structure pSS) peut etre une inclusion dans	| */
/* |    la descendance de l'element pEl.				| */
/* |    Retourne "Vrai" si l'inclusion est autorisee.                   | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             AllowedIncludedElem (PtrElement pEl, int typeNum, PtrSSchema pSS)

#else  /* __STDC__ */
boolean             AllowedIncludedElem (pEl, typeNum, pSS)
PtrElement          pEl;
int                 typeNum;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   boolean             ret;
   int                 i;
   SRule              *pRule;
   PtrElement          pAsc;
   PtrSSchema        pSSrule, pSSExt;

   ret = False;
   if (pSS->SsRule[typeNum - 1].SrConstruct == CsPairedElement)
      if (!pSS->SsRule[typeNum - 1].SrFirstOfPair)
	 /* c'est un element de fin de paire, on fait comme si */
	 /* c'etait l'element de debut de paire */
	 typeNum--;
   pAsc = pEl;
   /* examine les elements ascendants */
   while (pAsc != NULL && (!ret))
     {
	/* regle de structure de l'ascendant courant */
	pSSrule = pAsc->ElSructSchema;
	pRule = &pSSrule->SsRule[pAsc->ElTypeNumber - 1];
	pSSExt = NULL;
	do
	  {
	     if (pRule != NULL)
	       {
		  /* parcourt la liste de ses extensions */
		  for (i = 0; i < pRule->SrNInclusions && (!ret); i++)
		     if (EquivalentSRules (pRule->SrInclusion[i], pSSrule,
				     typeNum, pSS, pAsc))
			/* l'element est compatible avec l'extension */
			if (!ExcludedType (pEl, pRule->SrInclusion[i],
					 pSSrule))
			   /* cette extension n'est pas une exclusion */
			   ret = True;
	       }
	     /* passe a l'extension de schema suivante */
	     if (pSSExt == NULL)
		pSSExt = pSSrule->SsNextExtens;
	     else
		pSSExt = pSSExt->SsNextExtens;
	     if (pSSExt != NULL)
		/* il y a encore un schema d'extension */
	       {
		  pSSrule = pSSExt;
		  /* cherche dans ce schema la regle d'extension pour l'ascendant */
		  pRule = ExtensionRule (pAsc->ElSructSchema, pAsc->ElTypeNumber, pSSExt);
	       }
	  }
	while (pSSExt != NULL && (!ret));
	/* passe a l'element ascendant */
	pAsc = pAsc->ElParent;
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |	ListOrAggregateRule   Cherche si l'element pEl qui fait partie d'un	| */
/* |	arbre abstrait peut etre obtenu a partir de la regle typeNum du	| */
/* |	schema de structure pSS, en passant par une regle CsList ou	| */
/* |	CsAggregate. Si oui, retourne dans pSS le pointeur sur le schema de	| */
/* |	structure contenant la regle CsList ou CsAggregate et dans typeNum	| */
/* |	le numero de cette regle.					| */
/* |	Si non retourne 0 dans typeNum et NULL dans pSS.		| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ListOrAggregateRule (PtrElement pEl, int * typeNum, PtrSSchema * pSS)

#else  /* __STDC__ */
void                ListOrAggregateRule (pEl, typeNum, pSS)
PtrElement          pEl;
int        *typeNum;
PtrSSchema       *pSS;

#endif /* __STDC__ */

{
   int                 c;
   boolean             test, equiv;
   SRule              *pSRule;

   equiv = False;
   if (*typeNum != 0 && *pSS != NULL)
     {
	pSRule = &(*pSS)->SsRule[*typeNum - 1];
	test = True;
	if (pSRule->SrRecursive)
	   if (pSRule->SrRecursDone)
	     {
		test = False;
		*typeNum = 0;
		*pSS = NULL;
	     }
	   else
	      pSRule->SrRecursDone = True;
	if (test)
	   switch (pSRule->SrConstruct)
		 {
		    case CsList:
		       if (!EquivalentSRules (pSRule->SrListItem, *pSS, pEl->ElTypeNumber, pEl->ElSructSchema, pEl))
			  /* ce n'est pas le type prevu des elements de la liste */
			  /* c'est peut-etre une inclusion */
			  if (!AllowedIncludedElem (pEl->ElParent, pEl->ElTypeNumber, pEl->ElSructSchema))
			    {
			       *typeNum = 0;
			       *pSS = NULL;
			    }
		       break;
		    case CsUnorderedAggregate:
		    case CsAggregate:
		       for (c = 0; c < pSRule->SrNComponents; c++)
			 {

			    equiv = EquivalentSRules (pSRule->SrComponent[c], *pSS, pEl->ElTypeNumber, pEl->ElSructSchema, pEl);

			    if (equiv)
			       break;
			 }
		       if (!equiv)
			  /* ce n'est pas un composant de l'agregat, c'est
			     peut-etre une inclusion */
			  equiv = AllowedIncludedElem (pEl->ElParent, pEl->ElTypeNumber, pEl->ElSructSchema);
		       if (!equiv)
			 {
			    *typeNum = 0;
			    *pSS = NULL;
			 }
		       break;
		    case CsIdentity:
		       *typeNum = pSRule->SrIdentRule;
		       ListOrAggregateRule (pEl, typeNum, pSS);
		       break;
		    default:
		       *typeNum = 0;
		       *pSS = NULL;
		       break;
		 }
	if (pSRule->SrRecursive && pSRule->SrRecursDone && test)
	   pSRule->SrRecursDone = False;
     }
}


/* ----------------------------------------------------------------------- */
/* |    Rend le type de constructeur d'un element                        | */
/* ----------------------------------------------------------------------- */

#ifdef __STDC__
RConstruct        GetElementConstruct (PtrElement pEl)

#else  /* __STDC__ */
RConstruct        GetElementConstruct (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   RConstruct        constr;
   int                 typ;

   constr = CsNatureSchema;
   if (pEl != NULL)
     {
	constr = pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct;
	typ = pEl->ElTypeNumber;
	while (constr == CsIdentity)
	  {
	     typ = pEl->ElSructSchema->SsRule[typ - 1].SrIdentRule;
	     constr = pEl->ElSructSchema->SsRule[typ - 1].SrConstruct;
	  }
     }
   return constr;
}


/* ---------------------------------------------------------------------- */
/* |    Teste si un element a un ancetre de type liste, mais sans       | */
/* |    agregat entre l'element et cet ancetre. Rend cet ancetre ou	| */
/* |	NULL si ce n'est pas le cas.					| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrElement          AncestorList (PtrElement pEl)

#else  /* __STDC__ */
PtrElement          AncestorList (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   boolean             stop;
   RConstruct        constr;

   stop = False;
   do
      if (pEl == NULL)
	 stop = True;
      else
	{
	   constr = GetElementConstruct (pEl);
	   if (constr == CsAggregate || constr == CsUnorderedAggregate)
	     {
		pEl = NULL;
		stop = True;
	     }
	   else if (constr == CsList)
	      stop = True;
	   else
	      pEl = pEl->ElParent;
	}
   while (!stop);
   return pEl;
}


/* ---------------------------------------------------------------------- */
/* |    Teste si les cardinalites minimales et maximales de la liste	| */
/* |	pEl permettent d'ajouter ou retrancher delta elements.		| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             CanChangeNumberOfElem (PtrElement pEl, int delta)

#else  /* __STDC__ */
boolean             CanChangeNumberOfElem (pEl, delta)
PtrElement          pEl;
int                 delta;

#endif /* __STDC__ */

{
   PtrElement          pChild;
   int                 len;
   boolean             ret;
   SRule              *rule;
   int                 RIdent;

   ret = False;
   if (pEl != NULL)
     {
	rule = &(pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1]);
	/* la regle de l'element n'est peut-etre pas une regle liste. */
	/* C'est peut-etre une identite. On cherche la regle liste */
	/* a laquelle renvoie la regle identite de l'element. */
	while (rule->SrConstruct == CsIdentity)
	  {
	     RIdent = rule->SrIdentRule;
	     rule = &(pEl->ElSructSchema->SsRule[RIdent - 1]);
	  }
	if (rule->SrConstruct == CsList)
	  {
	     /* count the number of children */
	     pChild = pEl->ElFirstChild;
	     /* ignore page breaks */
	     for (len = 0; pChild != NULL; pChild = pChild->ElNext)
	       {
		  if (!pChild->ElTerminal || pChild->ElLeafType != LtPageColBreak)
		     /* on ne compte que les elements du type (ou equivalent) */
		     /* prevu par la liste. (Inclusion possibles) */
		     if (EquivalentSRules (rule->SrListItem, pEl->ElSructSchema,
				     pChild->ElTypeNumber, pChild->ElSructSchema, pEl))
			len++;
	       }
	     ret = True;
	     if (delta >= 0)
		if (len + delta > rule->SrMaxItems)
		   ret = False;
	     if (delta <= 0)
		if (len + delta < rule->SrMinItems)
		   ret = False;
	  }
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |	SRuleForSibling cherche le type d'element qui peut etre cree comme	| */
/* |	voisin de l'element pointe par pEl. On ne tient pas compte des	| */
/* |	inclusions possibles.						| */
/* |	before indique s'il s'agit d'un voisin precedent (before=vrai)	| */
/* |	ou suivant (before=faux).					| */
/* |	distance est la distance (nombre d'elements possibles) entre	| */
/* |	l'element pEl et le type d'element a creer.			| */
/* |	Retourne dans typeNum le numero de la regle definissant le type	| */
/* |	du voisin possible, dans pSS un pointeur sur le schema de	| */
/* |	structure de ce type, et list vaut 'vrai' si le constructeur	| */
/* |	qui permet le voisinage est une liste, 'faux' si c'est un	| */
/* |	agregat. Au retour, optional vaut vrai si le voisin possible	| */
/* |	est un composant optionnel d'agregat ou un composant d'un	| */
/* |	agregat non ordonne'.						| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                SRuleForSibling (PtrElement pEl, boolean before, int distance, int * typeNum, PtrSSchema * pSS, boolean * list, boolean * optional)

#else  /* __STDC__ */
void                SRuleForSibling (pEl, before, distance, typeNum, pSS, list, optional)
PtrElement          pEl;
boolean             before;
int                 distance;
int        *typeNum;
PtrSSchema       *pSS;
boolean            *list;
boolean            *optional;

#endif /* __STDC__ */

{
   int                 i;
   PtrElement          pE;
   boolean             stop;
   PtrElement          pEquivEl;
   SRule              *pRule;
   boolean             reverse;

   *typeNum = 0;
   *optional = False;
   pEquivEl = pEl;
   i = 0;
   if (pEl != NULL)
      /* la racine d'un arbre ne peut pas avoir de voisin */
      if (pEl->ElParent != NULL)
	{
	   /* Si l'element pointe par pEl est une marque de page, il n'a pas */
	   /* de voisin possible */
	   if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
	      /* Teste si l'element pointe par pEl est un element de liste */
	      /* ou d'agregat */
	     {
		*typeNum = pEl->ElParent->ElTypeNumber;
		*pSS = pEl->ElParent->ElSructSchema;
		ListOrAggregateRule (pEl, typeNum, pSS);
		pEquivEl = pEl;
	     }
	   if (*typeNum > 0)
	      /* c'est un element de liste ou d'agregat */
	      if ((*pSS)->SsRule[*typeNum - 1].SrConstruct == CsList)
		 /* c'est un element de liste */
		{
		   *list = True;
		   if (distance > 1)
		      /* on ne peut creer dans une liste que des voisins immediats */
		     {
			*typeNum = 0;
			*pSS = NULL;
		     }
		   else
		      /* verifie si le nombre max. d'elements est atteint */
		     {
			if (!CanChangeNumberOfElem (pEquivEl->ElParent, 1))
			   *typeNum = 0;
			else
			   /* on va creer un sous-arbre du type des elements */
			   /* qui constituent la liste */
			   *typeNum = (*pSS)->SsRule[*typeNum - 1].SrListItem;
		     }
		}
	      else
		{
		   /* on est dans un agregat */
		   *list = False;
		   pRule = &(*pSS)->SsRule[*typeNum - 1];	/* regle de l'agregat */
		   if (pRule->SrConstruct == CsUnorderedAggregate)
		      /* agregat sans ordre */
		     {
			*optional = True;
			/* reponse negative a priori */
			*typeNum = 0;
			/* examine tous les composants declare's dans la */
			/* regle de l'agregat */
			i = 0;
			while (i < pRule->SrNComponents && *typeNum == 0)
			  {
			     /* cherche si ce composant a ete cree */
			     pE = pEquivEl->ElParent->ElFirstChild;
			     stop = False;
			     while (pE != NULL && !stop)
				if (EquivalentSRules (pRule->SrComponent[i], *pSS, pE->ElTypeNumber, pE->ElSructSchema, pEquivEl->ElParent))
				   /* c'est le composant cherche' */
				   stop = True;
				else
				   /* l'element n'a pas le type du composant cherche' */
				   pE = pE->ElNext;
			     if (pE != NULL)
				/* ce composant existe deja , essaie le suivant */
			       {
				  if (i < distance)
				     distance++;
				  i++;
			       }
			     else
				/* ce composant n'a pas encore ete cree */
			     if (i+1 >= distance)
				/* c'est le 1er composant qui convient */
				*typeNum = pRule->SrComponent[i];
			     else
				i++;
			  }
		     }
		   else
		      /* agregat ordonne' */
		     {
			/* cherche dans cet agregat le type de l'element */
			/* voisin de pEl */
			pEl = pEquivEl;
			/* cherche d'abord le rang de l'element pEl dans l'agregat */
			*typeNum = 0;
			reverse = False;
			while (*typeNum == 0 && pEl != NULL)
			  {
			     i = 0;
			     do
				  if (EquivalentSRules (pRule->SrComponent[i++], *pSS, pEl->ElTypeNumber,
					     pEl->ElSructSchema, pEl->ElParent))
				     /* trouve' */
				     *typeNum = i;

			     while (*typeNum == 0 && i < pRule->SrNComponents);

			     if (*typeNum == 0)
				/* l'element n'est pas un des composants de
				l'agregat, c'est sans doute une inclusion */
				if (AllowedIncludedElem (pEl->ElParent, pEl->ElTypeNumber, pEl->ElSructSchema))
				   /* c'est une inclusion, on essaie avec
				      l'element voisin */
				  {
				     if (before)
					/* cherche le voisin possible de
					l'element qui suit l'element inclusion */
					if (pEl->ElNext != NULL)
					   pEl = pEl->ElNext;
					else
					   /* pas de suivant, on change de sens, mais une */
					   /* seule fois */
					if (reverse)
					   pEl = NULL;	/* on a deja change' de sens */
					else
					  {
					     pEl = pEl->ElPrevious;
					     reverse = True;
					     before = !before;
					  }
				     else
					/* cherche le voisin possible de l'element */
					/* qui precede l'element inclusion */
				     if (pEl->ElPrevious != NULL)
					pEl = pEl->ElPrevious;
				     else
					/* pas de precedent, on change de sens,
					mais une seule fois */
				     if (reverse)
					/* on a deja change' de sens */
					pEl = NULL;
				     else
				       {
					  pEl = pEl->ElNext;
					  reverse = True;
					  before = !before;
				       }
				  }
				else
				   /* ce n'est pas une inclusion, on arrete */
				   pEl = NULL;
			  }

			if (*typeNum != 0)
			   /* trouve' */
			   if (before)
			      /* on cherche a creer avant */
			      if (i <= distance)
				 *typeNum = 0;
			      else
				 /* type de l'element possible */
				{
				   *optional = pRule->SrOptComponent[*typeNum - distance - 1];
				   *typeNum = pRule->SrComponent[*typeNum - distance - 1];
				   /* cherche si ce type d'element existe deja */
				   pE = pEl->ElPrevious;
				   while (pE != NULL && *typeNum != 0)
				     {
					if (EquivalentSRules (*typeNum, *pSS, pE->ElTypeNumber, pE->ElSructSchema, pEl->ElParent))
					  /* il existe deja */
					  {
					     *optional = False;
					     *typeNum = 0;
					  }
					pE = pE->ElPrevious;
				     }
				}
			   else
			      /* on cherche a creer apres */
			      if (*typeNum + distance > pRule->SrNComponents)
			         *typeNum = 0;
			   else
			      /* type de l'element possible */
			     {
				*optional = pRule->SrOptComponent[*typeNum + distance - 1];
				*typeNum = pRule->SrComponent[*typeNum + distance - 1];
				/* cherche si ce type d'element existe deja */
				pE = pEl->ElNext;
				while (pE != NULL && *typeNum != 0)
				  {
				     if (EquivalentSRules (*typeNum, *pSS, pE->ElTypeNumber, pE->ElSructSchema, pEl->ElParent))
				       {
					  *optional = False;
					  *typeNum = 0;	/* il existe deja */
				       }
				     pE = pE->ElNext;
				  }
			     }
		     }
		}
	}
   if (*typeNum == 0)
      *pSS = NULL;
}


/* ---------------------------------------------------------------------- */
/* |	ReferredType   pRefEl est un pointeur sur un element reference.	| */
/* |	pRefAttr est un pointeur sur un attribut reference. L'un des	| */
/* |	deux pointeurs est NULL. La procedure retourne dans typeNum le	| */
/* |	numero du type et dans pSS un pointeur sur le schema de		| */
/* |	structure qui definit le type des elements reference's par	| */
/* |	pRefEl ou pRefAttr.						| */
/* |	Retourne typeNum = 0 et pSS = NULL s'il s'agit d'une reference	| */
/* |	non typee : CsReference(Any)					| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                ReferredType (PtrElement pRefEl, PtrAttribute pRefAttr, PtrSSchema * pSS, int * typeNum)

#else  /* __STDC__ */
void                ReferredType (pRefEl, pRefAttr, pSS, typeNum)
PtrElement          pRefEl;
PtrAttribute         pRefAttr;
PtrSSchema       *pSS;
int        *typeNum;

#endif /* __STDC__ */

{
   int         referredNature;
   Name                 presName;
   SRule              *pRule;
   TtAttribute           *pAtt;

   if (pRefEl != NULL)
      /* c'est un element reference */
      if (pRefEl->ElSource != NULL)
	 /* c'est une reference inclusion */
	{
	   *pSS = pRefEl->ElSructSchema;
	   *typeNum = pRefEl->ElTypeNumber;
	}
      else
	{
	   /* la regle definissant la reference */
	   pRule = &pRefEl->ElSructSchema->SsRule[pRefEl->ElTypeNumber - 1];
	   /* cherche le type de l'element reference' prevu par le schema */
	   /* de structure */
	   *typeNum = pRule->SrReferredType;	
	   /* schema de structure ou est definie la reference */
	   if (*typeNum == 0)
	      *pSS = NULL;
	   else
	      *pSS = pRefEl->ElSructSchema;
	   if (pRule->SrRefTypeNat[0] != '\0')
	      /* le type reference' est defini dans un autre schema de */
	      /* structure */
	      /* cherche, ou charge si ce n'est pas deja fait, le schema de */
	      /* structure qui definit le type reference' */
	     {
		presName[0] = '\0';
		/* pas de schema de presentation prefere' */
		referredNature = CreeNature (pRule->SrRefTypeNat, presName, *pSS);
		if (referredNature == 0)
		   *pSS = NULL;
		else
		   /* pointeur sur le schema de structure qui definit le */
		   /* type d'element reference' */
		   *pSS = (*pSS)->SsRule[referredNature - 1].SrSSchemaNat;
	     }
	}
   if (pRefAttr != NULL)
      /* c'est un attribut reference */
     {
        /* regle de definition de l'attribut reference */
	pAtt = &pRefAttr->AeAttrSSchema->SsAttribute[pRefAttr->AeAttrNum - 1];
	/* cherche le type de l'element reference' prevu par le schema */
	/* de structure */
	*typeNum = pAtt->AttrTypeRef;
	if (*typeNum == 0)
	   *pSS = NULL;
	else
	   /* schema de structure qui definit l'attribut */
	   *pSS = pRefAttr->AeAttrSSchema;

	if (pAtt->AttrTypeRefNature[0] != '\0')
	   /* le type reference' est defini dans un autre schema de */
	   /* structure */
	   /* cherche, ou charge si ce n'est pas deja fait, le schema de */
	   /* structure qui definit le type reference' */
	  {
	     presName[0] = '\0';
	     /* pas de schema de presentation prefere' */
	     referredNature = CreeNature (pAtt->AttrTypeRefNature, presName, *pSS);
	     if (referredNature == 0)
		*pSS = NULL;
	     else
		/* pointeur sur le schema de structure qui definit le type */
		/* d'element reference' par l'attribut */
		*pSS = (*pSS)->SsRule[referredNature - 1].SrSSchemaNat;
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |	CanCutElement       indique si on peut couper l'element pEl.		| */
/* |	On considere l'exception NoCut et les composants obligatoires	| */
/* |	d'agregats et le nombre minimum d'elements des listes.		| */
/* |	pElCut sur le premier des elements deja coupes			| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             CanCutElement (PtrElement pEl, PtrDocument pDoc, PtrElement pElCut)

#else  /* __STDC__ */
boolean             CanCutElement (pEl, pDoc, pElCut)
PtrElement          pEl;
PtrDocument         pDoc;
PtrElement          pElCut;

#endif /* __STDC__ */

{
   int         typeNum;
   PtrSSchema        pSS;
   SRule              *pRule;
   int                 i;
   int                 view;
   PtrElement          viewRoot;
   boolean             found;
   boolean             ret, InCutBuffer;

   /* a priori, on peut couper l'element */
   ret = True;
   InCutBuffer = False;
   if (pEl != NULL)
      if (TypeHasException (ExcNoCut, pEl->ElTypeNumber, pEl->ElSructSchema))
	 /* l'exception NoCut est associee au type de l'element */
	{
	  if (ThotLocalActions[T_Tableau_OrphanCell]!=NULL)
	    {
	      (*ThotLocalActions[T_Tableau_OrphanCell])(pEl, pElCut, &InCutBuffer);
	      if (InCutBuffer)
		/* C'est une cellule de tableau orpheline, sa colonne de
		   reference a deja ete coupee */
		ret = True;
	      else
		ret = False;
	    }
	}
      else if (FullStructureChecking)
	 /* on est en mode de controle strict de la structure */
	 if (pEl->ElParent != NULL)
	   {
	      /* teste si l'element pointe par pEl est un element de liste */
	      /* ou d'agregat */
	      typeNum = pEl->ElParent->ElTypeNumber;
	      pSS = pEl->ElParent->ElSructSchema;
	      ListOrAggregateRule (pEl, &typeNum, &pSS);
	      if (typeNum > 0)
		 /* c'est un element de liste ou d'agregat */
		{
		   /* SRule qui definit la liste ou l'agregat */
		   pRule = &pSS->SsRule[typeNum - 1];
		   if (pRule->SrConstruct == CsList)
		      /* c'est un element de liste */
		     {
			if (pRule->SrMinItems > 0)
			   /* il y a un nombre minmum d'elements a respecter */
			   ret = CanChangeNumberOfElem (pEl->ElParent, -1);
		     }
		   else
		      /* c'est un element d'agregat */
		     {
			ret = False;
			/* cherche le rang de ce composant dans l'agregat */
			found = False;
			i = 0;
			while (!found && i < pRule->SrNComponents)
			   if (pRule->SrComponent[i] == pEl->ElTypeNumber)
			      found = True;
			   else
			      i++;
			/* on ne peut couper que les composants optionnels */
			if (found)
			   ret = pRule->SrOptComponent[i];
		     }
		}
	   }
   if (ret)
      /* est-ce la racine d'un sous-arbre d'affichage */
      if (pEl->ElAssocNum > 0)
	/* element associe */
	{
	   viewRoot = pDoc->DocAssocSubTree[pEl->ElAssocNum - 1];
	   ret = viewRoot == NULL || (pEl != viewRoot && !ElemIsAnAncestor (pEl, viewRoot));
	}
      else
	 /* element de l'arbre principal */
	{
	   /* parcourt toutes les vues ouvertes jusqu'a en trouver une */
	   /* dont l'element est la racine */
	   for (view = 0; view < MAX_VIEW_DOC && ret; view++)
	      if (pDoc->DocView[view].DvPSchemaView != 0)
		/* vue ouverte */
		{
		   viewRoot = pDoc->DocViewSubTree[view];
		   ret = (viewRoot == NULL ||
			  (pEl != viewRoot && !ElemIsAnAncestor (pEl, viewRoot)));
		}
	}
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    AllowedSibling  retourne vrai si on peut inserer un element de  | */
/* |    type typeNum (defini dans le schema de structure pSS) comme	| */
/* |    frere de l'element pEl qui appartient au document pDoc.		| */
/* |    before indique s'il s'agit d'un frere precedent ou suivant.	| */
/* |    user indique si l'insertion est demandee par l'utilisateur.	| */
/* |    inTree indique si l'element est deja en place dans l'arbre.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             AllowedSibling (PtrElement pEl, PtrDocument pDoc, int typeNum, PtrSSchema pSS, boolean before, boolean user, boolean inTree)

#else  /* __STDC__ */
boolean             AllowedSibling (pEl, pDoc, typeNum, pSS, before, user, inTree)
PtrElement          pEl;
PtrDocument         pDoc;
int                 typeNum;
PtrSSchema        pSS;
boolean             before;
boolean             user;
boolean             inTree;

#endif /* __STDC__ */

{
   int         ascTypeNum;
   PtrSSchema        pAscSS;
   SRule              *pRule;
   PtrElement          pEl1;
   boolean             ok;
   boolean             optional;
   boolean             stop;
   int         compNum;
   int                 i;

   ok = False;
   if (pEl != NULL)
      /* on ne peut rien inserer a cote' de la racine d'un arbre */
      if (pEl->ElParent != NULL)
	{
	   /* on peut inserer une marque de page n'importe ou` */
	   if (typeNum == ord (PageBreak) + 1)
	      ok = True;
	   /* OK si c'est une inclusion pour l'un des ascendants */
	   else if (AllowedIncludedElem (pEl->ElParent, typeNum, pSS))
	      ok = True;
	   if (ok)
	      /* dans le cas d'une inclusion ou d'une marque de page */
	      /* on ne peut quand meme pas inserer comme descendant direct */
	      /* d'un noeud CsChoice ou CsIdentity' */
	     {
		pRule = &pEl->ElParent->ElSructSchema->SsRule[pEl->ElParent->ElTypeNumber - 1];
		/* on ne fait pas cette verification si l'element y est deja */
		if (!inTree)
		   if (pRule->SrConstruct == CsChoice)
		      ok = False;
		   else if (pRule->SrConstruct == CsIdentity)
		      if (EquivalentSRules (pEl->ElTypeNumber, pEl->ElSructSchema,
			pEl->ElParent->ElTypeNumber, pEl->ElParent->ElSructSchema, pEl))
			 /* le fils et le pere sont de type equivalent, refus */
			 ok = False;
	     }
	   else
	     {
		if (pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak)
		   /* on veut inserer a cote' d'une marque de page. */
		  {
		     pEl1 = pEl;
		     stop = False;
		     /* si on insere avant, on cherche le premier suivant */
		     /* qui n'est pas une marque de page, sinon le precedent */
		     while (!stop)
			if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
			   /* ce n'est pas une page, on a trouve' */
			   stop = True;
			else
			   /* c'est une page, on continue */
			  {
			     if (before)
				pEl = pEl->ElNext;
			     else
				pEl = pEl->ElPrevious;
			     if (pEl == NULL)
				stop = True;
			  }
		     if (pEl == NULL && !inTree)
			/* pas encore trouve', on cherche dans l'autre sens */
			/* si l'element n'est pas deja a sa place */
		       {
			  pEl = pEl1;
			  stop = False;
			  while (!stop)
			     if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
				/* ce n'est pas une page, on a trouve' */
				stop = True;
			     else
				/* c'est une page, on continue */
			       {
				  if (before)
				     pEl = pEl->ElPrevious;
				  else
				     pEl = pEl->ElNext;
				  if (pEl == NULL)
				     stop = True;
			       }
			  before = !before;
		       }
		     if (pEl == NULL)
			/* il n'y a que des sauts de page, c'est comme si */
			/* l'element etait vide */
			ok = AllowedFirstChild (pEl1->ElParent, pDoc, typeNum, pSS,
						  user, inTree);
		  }
		if (pEl != NULL)
		  {
		     /* Teste si l'element pointe par pEl est un element de */
		     /* liste ou d'agregat */
		     ascTypeNum = pEl->ElParent->ElTypeNumber;
		     pAscSS = pEl->ElParent->ElSructSchema;
		     ListOrAggregateRule (pEl, &ascTypeNum, &pAscSS);
		     if (ascTypeNum > 0)
			/* c'est un element de liste ou d'agregat */
		       {
			  pRule = &pAscSS->SsRule[ascTypeNum - 1];
			  if (pRule->SrConstruct == CsList)
			     /* c'est un element de liste */
			    {
			       ok = True;
			       if (!inTree)
				  /*  verifie qu'on peut ajouter un element a la liste */
				  if (!CanChangeNumberOfElem (pEl->ElParent, 1))
				     /* liste pleine */
				     ok = False;
			       /* verifie le type des elements de liste */
			       if (ok)
				  ok = EquivalentSRules (pRule->SrListItem, pAscSS, typeNum,
						   pSS, pEl->ElParent);
			    }
			  else
			     /* c'est un element d'agregat */
			    {
			       if (pRule->SrConstruct == CsUnorderedAggregate)
				  /* examine tous les composants declare's dans la regle */
				  for (i = 0; i < pRule->SrNComponents && !ok; i++)
				     if (EquivalentSRules (pRule->SrComponent[i], pAscSS,
						   typeNum, pSS, pEl->ElParent))
				       {
					  ok = True;
					  compNum = i+1;
				       }
			       if (pRule->SrConstruct == CsAggregate)
				  /* agregat ordonne' */
				 {
				    /* cherche le rang compNum de l'element pEl dans l'agregat */
				    compNum = 0;
				    for (i = 0; i < pRule->SrNComponents && compNum == 0; i++)
				       if (EquivalentSRules (pRule->SrComponent[i], pAscSS,
						       pEl->ElTypeNumber, pEl->ElSructSchema, pEl->ElParent))
					  compNum = i+1;
				    if (compNum == 0)
				       /* l'element pEl n'est pas un composant prevu de */
				       /* l'agregat (c'est sans doute une inclusion) */
				      {
					 pEl1 = pEl;
					 stop = False;
					 /* on cherche le frere le plus proche qui ne soit */
					 /* ni une marque de page ni une inclusion */
					 while (!stop)
					    if (!(pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak) && !AllowedIncludedElem (pEl->ElParent, pEl->ElTypeNumber, pEl->ElSructSchema))
					       /* ce n'est ni une page ni une inclusion, on a trouve' */
					       stop = True;
					    else
					       /* on continue */
					      {
						 if (before)
						    pEl = pEl->ElNext;
						 else
						    pEl = pEl->ElPrevious;
						 if (pEl == NULL)
						    stop = True;
					      }
					 if (pEl == NULL && !inTree)
					    /* pas encore trouve', on cherche dans l'autre */
					    /* sens si l'element n'est pas deja a sa place */
					   {
					      pEl = pEl1;
					      stop = False;
					      while (!stop)
						 if (!(pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak) && !AllowedIncludedElem (pEl->ElParent, pEl->ElTypeNumber, pEl->ElSructSchema))
						    /* ce n'est ni une page ni une inclusion, on a trouve' */
						    stop = True;
						 else
						    /* on continue */
						   {
						      if (before)
							 pEl = pEl->ElPrevious;
						      else
							 pEl = pEl->ElNext;
						      if (pEl == NULL)
							 stop = True;
						   }
					      before = !before;
					   }
					 if (pEl == NULL)
					    /* il n'y a que des sauts de page et des */
					    /* inclusions, c'est comme si le pere etait vide */
					    ok = AllowedFirstChild (pEl1->ElParent, pDoc, typeNum,
							pSS, user, inTree);
					 else
					    /* on a trouve un frere qui n'est ni un saut de */
					    /* page ni une inclusion. On cherche son */
					    /* rang dans la regle qui definit l'agregat */
					    for (i = 0; i < pRule->SrNComponents && compNum == 0; i++)
					       if (EquivalentSRules (pRule->SrComponent[i], pAscSS,
							       pEl->ElTypeNumber, pEl->ElSructSchema, pEl->ElParent))
						  compNum = i+1;
				      }
				    if (pEl != NULL)
				      {
					 /* tous les composants sont optionnels si on n'est */
					 /* pas en mode de controle strict */
					 optional = True;
					 if (before)
					   {
					      if (compNum > 1)
						 /* pas de composant avant le premier */
						 for (i = compNum - 1; i >= 1 && optional && !ok; i--)
						    if (EquivalentSRules (pRule->SrComponent[i - 1], pAscSS, typeNum, pSS, pEl->ElParent))
						       ok = True;
						    else
						       optional = (pRule->SrOptComponent[i - 1] || !FullStructureChecking);
					   }
					 else
					    /* on veut inserer apres pEl */
					   {
					      if (compNum < pRule->SrNComponents)
						 /* pas de composant apres le dernier */
						 for (i = compNum + 1; i <= pRule->SrNComponents && optional
						      && !ok; i++)
						    if (EquivalentSRules (pRule->SrComponent[i - 1], pAscSS, typeNum, pSS, pEl->ElParent))
						       ok = True;
						    else
						       optional = (pRule->SrOptComponent[i - 1] || !FullStructureChecking);
					   }
				      }
				 }
			       /* verifie si ce composant existe deja */
			       if (ok && !inTree && pEl != NULL)
				 {
				    pEl1 = pEl->ElParent->ElFirstChild;
				    while (pEl1 != NULL && ok)
				       if (EquivalentSRules (typeNum, pSS,
						       pEl1->ElTypeNumber, pEl1->ElSructSchema, pEl->ElParent))
					  /* il existe deja */
					  ok = False;
				       else
					  pEl1 = pEl1->ElNext;
				 }
			    }
		       }
		     if (!ok && pEl != NULL)
			if (typeNum == ord (CharString) + 1)
			   if (pEl->ElTypeNumber == ord (CharString) + 1)
			      /* on veut inserer un element Texte a cote' d'un autre */
			      /* element texte, OK */
			      ok = True;
		  }
	     }
	   if (!ok)
	      /* si l'element a verifier porte une exception ExcPageBreakRepetition */
	      /* ExcPageBreakRepBefore, on l'autorise quand meme */
	      if (TypeHasException (ExcPageBreakRepBefore, typeNum, pSS))
		 ok = True;
	      else if (TypeHasException (ExcPageBreakRepetition, typeNum, pSS))
		 ok = True;
	   if (pEl != NULL)
	     {
		if (!ok)
		   /* si l'element voisin porte une exception ExcPageBreakRepetition */
		   /* ExcPageBreakRepBefore, on l'autorise quand meme */
		   if (TypeHasException (ExcPageBreakRepBefore, pEl->ElTypeNumber, pEl->ElSructSchema))
		      ok = True;
		   else if (TypeHasException (ExcPageBreakRepetition, pEl->ElTypeNumber, pEl->ElSructSchema))
		      ok = True;
		if (ok)
		   /* refus si c'est une exclusion pour l'un des ascendants */
		   if (ExcludedType (pEl->ElParent, typeNum, pSS))
		      ok = False;
		if (ok && user)
		   /* l'insertion est demande'e par l'utilisateur */
		   if (ElementIsReadOnly (pEl->ElParent))
		      ok = False;
		   else if (TypeHasException (ExcNoCreate, typeNum, pSS))
		      ok = False;
	     }
	}
   return ok;
}


/* ---------------------------------------------------------------------- */
/* |    AllowedFirstComponent      retourne vrai si on peut inserer un     | */
/* |    element de type compTypeNum (defini dans le schema de structure	| */
/* |    pCompSS) comme premier fils dans l'agregat defini par la regle	| */
/* |    de numero aggrTypeNum du schema de structure pCompSS.		| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             AllowedFirstComponent (int aggrTypeNum, PtrSSchema pAggrSS, int compTypeNum, PtrSSchema pCompSS)

#else  /* __STDC__ */
boolean             AllowedFirstComponent (aggrTypeNum, pAggrSS, compTypeNum, pCompSS)
int         aggrTypeNum;
PtrSSchema        pAggrSS;
int         compTypeNum;
PtrSSchema        pCompSS;

#endif /* __STDC__ */

{
   int                 i;
   SRule              *pRule;
   boolean             ok, optional;

   ok = False;
   optional = False;
   pRule = &pAggrSS->SsRule[aggrTypeNum - 1];
   switch (pRule->SrConstruct)
	 {
	    case CsAggregate:
	       /* on regarde si le type de l'element a inserer est celui */
	       /* des premiers composants optionnels de l'agregat jusqu'au */
	       /* premier composant obligatoire */
	       if (pRule->SrNComponents > 0)
		 {
		    i = 0;
		    do
		       if (pAggrSS->SsRule[pRule->SrComponent[i] - 1].SrConstruct == CsChoice && pAggrSS->SsRule[pRule->SrComponent[i] - 1].SrNChoices == -1)
			  /* le composant pre'vu est NATURE */
			 {
			    if (compTypeNum == pCompSS->SsRootElem)
			       /* l'element a inserer est la racine de son */
			       /* schema */
			       ok = True;
			 }
		       else if (pRule->SrComponent[i] == compTypeNum &&
				pAggrSS->SsCode == pCompSS->SsCode)
			  ok = True;
		       else
			 {
			    optional = pRule->SrOptComponent[i];
			    if (!optional)
			       if (!FullStructureChecking)
				  /* on n'est pas en mode de controle strict de
				  la structure: tous les voisins sont optionnels */
				  optional = True;
			    i++;
			 }
		    while (i < pRule->SrNComponents && !ok && optional);
		 }
	       break;
	    case CsUnorderedAggregate:
	       /* on regarde si le type de l'element a inserer est parmi */
	       /* les types des composants de l'agregat */
	       for (i = 0; i < pRule->SrNComponents && !ok; i++)
		  if (pAggrSS->SsRule[pRule->SrComponent[i] - 1].SrConstruct == CsChoice
		      && pAggrSS->SsRule[pRule->SrComponent[i] - 1].SrNChoices == -1)
		     /* le composant pre'vu est NATURE */
		    {
		       if (compTypeNum == pCompSS->SsRootElem)
			  /* l'element a inserer est la racine de son schema */
			  ok = True;
		    }
		  else if (pRule->SrComponent[i] == compTypeNum &&
			   pAggrSS->SsCode == pCompSS->SsCode)
		     ok = True;
	       break;
	    default:
	       break;
	 }
   return ok;
}


/* ---------------------------------------------------------------------- */
/* |    AllowedFirstChild     retourne vrai si on peut inserer un     | */
/* |    element de type typeNum (defini dans le schema de structure pSS)| */
/* |    comme premier fils de l'element pEl qui appartient au document  | */
/* |    pDoc.                                                           | */
/* |    user indique si l'insertion est demandee par l'utilisateur.     | */
/* |    inTree indique si l'element est deja en place dans l'arbre.	| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             AllowedFirstChild (PtrElement pEl, PtrDocument pDoc, int typeNum, PtrSSchema pSS, boolean user, boolean inTree)

#else  /* __STDC__ */
boolean             AllowedFirstChild (pEl, pDoc, typeNum, pSS, user, inTree)
PtrElement          pEl;
PtrDocument         pDoc;
int                 typeNum;
PtrSSchema        pSS;
boolean             user;
boolean             inTree;

#endif /* __STDC__ */

{
   SRule              *pRule1, *pRule2;
   PtrElement          pAsc, pChild;
   int         t;
   int                 i;
   boolean             ok, empty;
   boolean             stop;

   ok = False;
   if (pEl != NULL)
      /* on ne peut rien inserer dans un element terminal */
      if (!pEl->ElTerminal)
	{
	   if (inTree)
	      /* si le fils est deja dans l'arbre, on fait comme si son pere */
	      /* etait vide */
	      empty = True;
	   else if (pEl->ElFirstChild == NULL)
	      empty = True;
	   else
	      /* l'element a deja au moins un fils */
	     {
		pChild = pEl->ElFirstChild;
		/* on saute les marques de pages et les inclusions */
		stop = False;
		do
		  {
		     if (pChild == NULL)
			/* on a saute' tous les fils */
			stop = True;
		     else if (pChild->ElTerminal && pChild->ElLeafType == LtPageColBreak)
			/* ce fils est une marque de page, on le saute */
			pChild = pChild->ElNext;
		     else if (AllowedIncludedElem (pEl, pChild->ElTypeNumber, pChild->ElSructSchema))
			/* ce fils est une inclusion, on le saute */
			pChild = pChild->ElNext;
		     else
			/* ce fils n'est ni un saut de page ni une inclusion */
			/* on le garde */
			stop = True;
		  }
		while (!stop);
		if (pChild == NULL)
		   /* l'element ne contient que des marques de page, il est vide */
		   empty = True;
		else
		  {
		     empty = False;
		     ok = AllowedSibling (pChild, pDoc, typeNum, pSS, True, user, inTree);
		  }
	     }
	   if (empty)
	      if (pEl->ElTypeNumber > 0 && pEl->ElTypeNumber <= pEl->ElSructSchema->SsNRules)
		{
		   /* on peut inserer une marque de page n'importe ou` */
		   if (typeNum == ord (PageBreak) + 1)
		      ok = True;
		   /* OK si c'est une inclusion pour l'un des ascendants */
		   else if (AllowedIncludedElem (pEl, typeNum, pSS))
		      ok = True;
		   pRule1 = &pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1];
		   if (ok)
		      /* dans le cas d'une inclusion ou d'une marque de page */
		      /* on ne peut quand meme pas inserer comme descendant */
		      /* direct d'un noeud CsChoice */
		     {
			if (!inTree)
			   if (pRule1->SrConstruct == CsChoice)
			      ok = False;
		     }
		   else
		     {
			/* traitement selon le constructeur du type de l'element */
			switch (pRule1->SrConstruct)
			      {
				 case CsChoice:
				    if (pRule1->SrNChoices == -1)
				      /* c'est une regle NATURE */
				      {
					 if (typeNum == pSS->SsRootElem)
					    /* on veut inserer un element construit selon la */
					    /* regle racine de son schema de structure, OK */
					    ok = True;
				      }
				    else if (pRule1->SrNChoices == 0)
				       /* c'est une regle UNIT */
				       if (typeNum <= MAX_BASIC_TYPE)
					  /* on veut inserer un element de base, OK */
					  ok = True;
				       else
					 {
					    if (pSS->SsRule[typeNum - 1].SrUnitElem)
					       /* l'element a inserer est defini comme une unite' */
					       /* dans son schema. On cherche si ce schema est */
					       /* celui d'un des ascendants ou une extension */
					      {
						 pAsc = pEl;
						 while (pAsc != NULL && !ok)
						    if (pAsc->ElSructSchema->SsCode ==
							pSS->SsCode)
						       ok = True;
						    else if (ValidExtension (pAsc, &pSS))
						       ok = True;
						    else
						       pAsc = pAsc->ElParent;
					      }
					 }
				    else
				       /* c'est un choix avec indication des types possibles */
				       /* on regarde si le type a inserer est parmi les types */
				       /* possibles de ce choix */
				      {
					 i = 0;
					 while (!ok && i < pRule1->SrNChoices)
					   {
					      ok = EquivalentSRules (pRule1->SrChoice[i], pEl->ElSructSchema,
							   typeNum, pSS, pEl);
					      i++;
					   }
				      }
				    break;
				 case CsAggregate:
				 case CsUnorderedAggregate:
				    ok = AllowedFirstComponent (pEl->ElTypeNumber, pEl->ElSructSchema,
							     typeNum, pSS);
				    break;
				 case CsList:
				    /* on regarde si le type de l'element a inserer est */
				    /* equivalent au type des elements de la liste */
				    ok = EquivalentSRules (pRule1->SrListItem, pEl->ElSructSchema,
						   typeNum, pSS, pEl->ElParent);
				    break;
				 case CsNatureSchema:
				    /* le type a inserer doit etre la racine de la nature */
				    if (typeNum == pSS->SsRootElem)
				       if (pRule1->SrSSchemaNat != NULL)
					  if (pRule1->SrSSchemaNat->SsCode == pSS->SsCode)
					     ok = True;
				    break;
				 case CsIdentity:
				    /* on verifie d'abord si les types sont equivalents */
				    ok = EquivalentSRules (pRule1->SrIdentRule, pEl->ElSructSchema,
						     typeNum, pSS, pEl);
				    if (!ok)
				       /* les types ne sont pas directement equivalents */
				       /* peut-on passer par une regle CsList ou CsAggregate ? */
				      {
					 /* suit la chaine des identite's jusqu'au dernier */
					 /* type identique au type de pEl */
					 pRule2 = pRule1;
					 do
					   {
					      t = pRule2->SrIdentRule;
					      pRule2 = &pEl->ElSructSchema->SsRule[t - 1];
					   }
					 while (pRule2->SrConstruct == CsIdentity);
					 /* s'il s'agit d'une liste, le type a inserer doit */
					 /* etre equivalent aux elements de la liste */
					 if (pRule2->SrConstruct == CsList)
					    ok = EquivalentSRules (pRule2->SrListItem, pEl->ElSructSchema,
							   typeNum, pSS, pEl);
					 else if (pRule2->SrConstruct == CsAggregate ||
					     pRule2->SrConstruct == CsUnorderedAggregate)
					    /* c'est une regle CsAggregate */
					    ok = AllowedFirstComponent (t, pEl->ElSructSchema, typeNum,
								     pSS);
				      }
				    break;
				 case CsBasicElement:
				 case CsReference:
				 case CsConstant:
				 case CsPairedElement:
				    /* pas de descendance possible */
				    break;
				 default:
				    break;
			      }
		     }
		   if (ok)
		      if (ExcludedType (pEl, typeNum, pSS))
			 ok = False;
		   if (ok && user)
		      /* l'insertion est demande'e par l'utilisateur */
		      if (ElementIsReadOnly (pEl))
			 ok = False;
		      else if (TypeHasException (ExcNoCreate, typeNum, pSS))
			 ok = False;
		}
	}
   return ok;
}


/* ---------------------------------------------------------------------- */
/* |	InsertChildFirst   Insere dans l'arbre abstrait l'element pChild	| */
/* |	comme premier fils de l'element pEl.				| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                InsertChildFirst (PtrElement pEl, PtrElement pChild, PtrElement * pFeuille)

#else  /* __STDC__ */
void                InsertChildFirst (pEl, pChild, pFeuille)
PtrElement          pEl;
PtrElement          pChild;
PtrElement         *pFeuille;

#endif /* __STDC__ */

{
   boolean             same;

   if (pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsChoice)
     {
	/* verifie si pChild est la feuille qu'on doit retourner */
	same = pChild == *pFeuille;
	InsertElemInChoice (pEl, &pChild, False);
	if (same)
	   /* InsertElemInChoice peut avoir modifie' pChild. La feuille qu'on */
	   /* retournera doit rester l'element pChild */
	   *pFeuille = pChild;
     }
   else
      InsertFirstChild (pEl, pChild);
}


/* ---------------------------------------------------------------------- */
/* |	CreateDescendant  Cherche a creer, pour un element defini par la	| */
/* |	regle typeNum du schema de structure pSS, une descendance	| */
/* |	jusqu'a un element de type descTypeNum defini dans le schema de	| */
/* |	structure pDescSS.						| */
/* |	Retourne le pointeur le premier element de la descendance creee,| */
/* |	ou NULL s'il n'est pas possible de creer une telle descendance.	| */
/* |	pDoc: Pointeur sur le descripteur du document concerne.		| */
/* |	assocNum: Numero de liste d'elements associes.			| */
/* |	pLeaf: si succes, pointeur sur l'element de plus bas niveau	| */
/* |	cree: c'est un element de type descTypeNum.			| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrElement          CreateDescendant (int typeNum, PtrSSchema pSS, PtrDocument pDoc, PtrElement * pLeaf, int assocNum, int descTypeNum, PtrSSchema pDescSS)

#else  /* __STDC__ */
PtrElement          CreateDescendant (typeNum, pSS, pDoc, pLeaf, assocNum, descTypeNum, pDescSS)
int         typeNum;
PtrSSchema        pSS;
PtrDocument         pDoc;
PtrElement         *pLeaf;
int                 assocNum;
int         descTypeNum;
PtrSSchema        pDescSS;

#endif /* __STDC__ */

{
   PtrElement          pEl, pDesc, pEl1, pEl2;
   int                 i, j;
   Name                 N;
   boolean             stop;
   SRule              *pRule1;
   SRule              *pRule2;

   if (SameSRules (typeNum, pSS, descTypeNum, pDescSS))
      /* c'est un element du type voulu, on le cree */
     {
	pEl = NewSubtree (descTypeNum, pDescSS, pDoc, assocNum, False, True, True, True);
	*pLeaf = pEl;
     }
   else
     {
	pRule1 = &pSS->SsRule[typeNum - 1];
	pEl = NULL;
	stop = False;
	if (pRule1->SrRecursive)
	   /* SRule recursive */
	   if (pRule1->SrRecursDone)
	      /* on est deja passe' sur cette regle, on arrete */
	      stop = True;
	   else
	      /* on marque qu'on est passe' sur cette regle */
	      pRule1->SrRecursDone = True;
	if (!stop)
	   switch (pRule1->SrConstruct)
		 {
		    case CsBasicElement:
		    case CsReference:
		    case CsConstant:
		    case CsPairedElement:
		       /* regles terminales */
		       break;
		    case CsNatureSchema:
		       if (pRule1->SrSSchemaNat == NULL)
			  /* si le schema de nature n'est pas charge' on le charge */
			 {
			    N[0] = '\0';
			    /* pas de schema de presentation prefere' */
			    LoadNat (pSS, N, typeNum);
			 }
		       if (pRule1->SrSSchemaNat != NULL)
			 {
			    pEl = CreateDescendant (pRule1->SrSSchemaNat->SsRootElem,
				  pRule1->SrSSchemaNat, pDoc, pLeaf, assocNum,
					       descTypeNum, pDescSS);
			    if (pEl != NULL)
			       if (pEl->ElTypeNumber != pRule1->SrSSchemaNat->SsRootElem)
				  /* cree un element du type de la regle racine */
				 {
				    pEl1 = NewSubtree (pRule1->SrSSchemaNat->SsRootElem,
					    pRule1->SrSSchemaNat, pDoc, assocNum,
					     False, True, True, True);
				    InsertChildFirst (pEl1, pEl, pLeaf);
				    pEl = pEl1;
				 }
			 }
		       break;
		    case CsIdentity:
		       if (pRule1->SrIdentRule <= 0)
			  pEl = NULL;
		       else
			 {
			    pEl = CreateDescendant (pRule1->SrIdentRule, pSS, pDoc, pLeaf,
					       assocNum, descTypeNum, pDescSS);
			    if (pEl != NULL)
			       /* on a effectivement cree une descendance */
			       if (pEl->ElTypeNumber != pRule1->SrIdentRule)
				  /* le 1er element de la descendance n'est pas du type */
				  /* identique */
				 {
				    pRule2 = &pSS->SsRule[pRule1->SrIdentRule - 1];
				    if (pRule2->SrNInclusions > 0 || pRule2->SrNExclusions > 0)
				       /* le type identique porte des inclusions ou des */
				       /* exclusions, on le cree */
				      {
					 pEl1 = NewSubtree (pRule1->SrIdentRule, pSS, pDoc,
							    assocNum, False, True, True, True);
					 InsertChildFirst (pEl1, pEl, pLeaf);
					 pEl = pEl1;
				      }
				 }
			 }

		       break;
		    case CsList:
		       pEl = CreateDescendant (pRule1->SrListItem, pSS, pDoc, pLeaf, assocNum,
					  descTypeNum, pDescSS);
		       if (pEl != NULL)
			 {
			    pRule2 = &pSS->SsRule[pRule1->SrListItem - 1];
			    /* si les elements de la liste portent des inclusions */
			    /* ou des exclusions, on les cree */
			    if ((pRule2->SrConstruct == CsNatureSchema ||
				 EquivalentSRules (pEl->ElTypeNumber, pEl->ElSructSchema,
					  pRule1->SrListItem, pSS, pEl))
				&& pRule2->SrNInclusions == 0
				&& pRule2->SrNExclusions == 0)
			       pEl1 = pEl;
			    else
			      {
				 pEl1 = NewSubtree (pRule1->SrListItem, pSS, pDoc,
				     assocNum, False, True, True, True);
				 InsertChildFirst (pEl1, pEl, pLeaf);
				 pEl = pEl1;
			      }
			    for (i = 2; i <= pRule1->SrMinItems; i++)
			      {
				 pEl2 = NewSubtree (pRule1->SrListItem, pSS, pDoc, assocNum,
					      True, True, True, True);
				 if (pEl2 != NULL)
				   {
				      InsertElementAfter (pEl1, pEl2);
				      pEl1 = pEl2;
				   }
			      }
			 }
		       break;
		    case CsChoice:
		       if (pRule1->SrNChoices == 0)
			 {
			    /* c'est une regle UNIT */
			    if (descTypeNum <= MAX_BASIC_TYPE)
			       /* on cherche a creer un element d'un type de base */
			       /* Cree une feuille du type voulu */
			      {
				 pEl = NewSubtree (descTypeNum, pSS, pDoc, assocNum, True, True,
						   True, True);
				 *pLeaf = pEl;
			      }
			 }
		       else if (pRule1->SrNChoices == -1)
			  /* c'est une regle NATURE */
			  /* essaie toutes les regles de nature du schema */
			 {
			    i = MAX_BASIC_TYPE - 1;
			    do
			      {
				 if (pSS->SsRule[i++].SrConstruct == CsNatureSchema)
				    pEl = CreateDescendant (i, pSS, pDoc, pLeaf, assocNum,
						       descTypeNum, pDescSS);
			      }
			    while (pEl == NULL && i < pSS->SsNRules);
			 }
		       else
			  /* C'est un choix parmi des types en nombre limite */
			 {
			    i = 0;
			    pDesc = NULL;
			    do
				 pDesc = CreateDescendant (pRule1->SrChoice[i++], pSS, pDoc,
				      pLeaf, assocNum, descTypeNum, pDescSS);
			    while (pDesc == NULL && i < pRule1->SrNChoices);
			    if (pDesc != NULL)
			       /* une descendance possible du type voulu */
			       if ((pRule1->SrChoice[i - 1] <= MAX_BASIC_TYPE
				 || pDesc->ElTypeNumber == pRule1->SrChoice[i - 1])
				   && (pDesc->ElSructSchema->SsCode == pSS->SsCode
				       || pSS->SsRule[pRule1->SrChoice[i - 1] - 1].SrConstruct == CsNatureSchema))
				  pEl = pDesc;
			       else
				 {
				    pEl = NewSubtree (pRule1->SrChoice[i - 1], pSS, pDoc,
				     assocNum, False, True, True, True);
				    if (pEl != NULL)
				       InsertChildFirst (pEl, pDesc, pLeaf);
				 }
			 }
		       break;
		    case CsAggregate:
		    case CsUnorderedAggregate:
		       i = 0;
		       pDesc = NULL;
		       while (pDesc == NULL && i < pRule1->SrNComponents)
			 {
			    /* on ne tente de creer une descendance que pour les */
			    /* composants obligatoires de l'agregat */
			    if (!pRule1->SrOptComponent[i])
			       pDesc = CreateDescendant (pRule1->SrComponent[i], pSS, pDoc,
				      pLeaf, assocNum, descTypeNum, pDescSS);	
			    i++;
			 }
		       if (pDesc == NULL)
			  /* on n'a rien pu creer en ne prenant que les composants */
			  /* obligatoires, on essaie maintenant les composants optionnels */
			 {
			    i = 0;
			    while (pDesc == NULL && i < pRule1->SrNComponents)
			      {
				 if (pRule1->SrOptComponent[i])
				    pDesc = CreateDescendant (pRule1->SrComponent[i], pSS, pDoc,
				      pLeaf, assocNum, descTypeNum, pDescSS);
				 i++;
			      }
			 }
		       if (pDesc != NULL)
			  /* on a pu creer une descendance */
			 {
			    if ((pRule1->SrComponent[i - 1] <= MAX_BASIC_TYPE
				 || pDesc->ElTypeNumber == pRule1->SrComponent[i - 1])
				&& (pDesc->ElSructSchema->SsCode == pSS->SsCode
				    || pSS->SsRule[pRule1->SrComponent[i - 1] - 1].SrConstruct == CsNatureSchema))
			       pEl1 = pDesc;
			    else
			      {
				 pEl1 = NewSubtree (pRule1->SrComponent[i - 1], pSS, pDoc,
				     assocNum, False, True, True, True);
				 if (pEl1 != NULL)
				    InsertChildFirst (pEl1, pDesc, pLeaf);
			      }
			    if (i == 1)
			       pEl = pEl1;
			    /* cree les autres composants obligatoires de l'agregat */
			    for (j = 1; j <= pRule1->SrNComponents; j++)
			       if (j != i && !pRule1->SrOptComponent[j-1])
				 {
				    pEl2 = NewSubtree (pRule1->SrComponent[j-1], pSS, pDoc,
				      assocNum, True, True, True, True);
				    if (pEl2 != NULL)
				      {
					 if (pEl == NULL)
					    pEl = pEl2;
					 if (i > j)
					    InsertElementBefore (pEl1, pEl2);
					 else
					   {
					      InsertElementAfter (pEl1, pEl2);
					      pEl1 = pEl2;
					   }
				      }
				 }
			 }
		       break;
		    default:
		       break;
		 }
	if (pRule1->SrRecursive && pRule1->SrRecursDone && !stop)
	   pRule1->SrRecursDone = False;
     }
   return pEl;
}


/* ----------------------------------------------------------------------
   TtaGetSchemaExtension

   Returns the structure schema extension that is associated with a given
   document and that has a given name.

   Parameters:
   document:
   NomExtension:

   Return value:
   NULL if the extension is not associated with the document.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
SSchema             TtaGetSchemaExtension (Document document, char *NomExtension)
#else  /* __STDC__ */
SSchema             TtaGetSchemaExtension (document, NomExtension)
Document            document;
char               *NomExtension;

#endif /* __STDC__ */
{
   PtrSSchema        pSS;
   PtrSSchema        pExtSS;
   PtrDocument         pDoc;

   pDoc = TabDocuments[document - 1];
   pExtSS = NULL;
   if (pDoc != NULL && NomExtension != NULL)
     {
	pSS = pDoc->DocSSchema;
	while (pSS != NULL && pExtSS == NULL)
	   if (strcmp (pSS->SsName, NomExtension) == 0)
	      pExtSS = pSS;
	   else
	      pSS = pSS->SsNextExtens;
     }
   return (SSchema) pExtSS;
}


/* ---------------------------------------------------------------------- */
/* |    ExtensionRule retourne un pointeur sur la regle d'extension       | */
/* |    qui se trouve dans le schema d'extension pExtSS et qui          | */
/* |    s'applique aux elements de type typeNum definis dans le schema  | */
/* |    de structure pSS.                                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
SRule              *ExtensionRule (PtrSSchema pSS, int typeNum, PtrSSchema pExtSS)

#else  /* __STDC__ */
SRule              *ExtensionRule (pSS, typeNum, pExtSS)
PtrSSchema        pSS;
int         typeNum;
PtrSSchema        pExtSS;

#endif /* __STDC__ */

{
   SRule              *RegleExt;
   SRule              *Rule;
   Name                 typeName;
   int         r;

   RegleExt = NULL;
   if (pExtSS != NULL && pSS != NULL)
      if (pExtSS->SsExtension && !pSS->SsExtension)
	{
	   /*on cherche d'abord une regle d'extension ayant le nom du type */
	   strncpy (typeName, pSS->SsRule[typeNum - 1].SrName, MAX_NAME_LENGTH);
	   r = 0;
	   while (RegleExt == NULL && r < pExtSS->SsNExtensRules)
	      if (strcmp (typeName, pExtSS->SsExtensBlock->EbExtensRule[r].SrName) == 0)
		 RegleExt = &(pExtSS->SsExtensBlock->EbExtensRule[r]);
	      else
		 r++;
	   if (RegleExt == NULL)
	      /* on n'a pas trouve' de regle de meme nom */
	     {
		Rule = &pSS->SsRule[typeNum - 1];
		if (typeNum == pSS->SsRootElem ||
		    (Rule->SrConstruct == CsList &&
		     pSS->SsRule[Rule->SrListItem - 1].SrAssocElem))
		   /* il s'agit d'une racine de document ou d'une racine */
		   /* d'arbre associe. On cherche une regle d'extension qui */
		   /* s'applique aux racines (regle avec SrName vide). */
		  {
		     r = 0;
		     while (RegleExt == NULL && r < pExtSS->SsNExtensRules)
			if (pExtSS->SsExtensBlock->EbExtensRule[r].SrName[0] == '\0')
			   RegleExt = &(pExtSS->SsExtensBlock->EbExtensRule[r]);
			else
			   r++;
		  }
	     }
	}
   return RegleExt;
}

/* ---------------------------------------------------------------------- */
/* |    ValidExtension retourne vrai si l'extension de schema pExt     | */
/* |    peut etre appliquee au schema de l'element pEl et, dans ce cas, | */
/* |    retourne dans pExt le schema a appliquer.                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             ValidExtension (PtrElement pEl, PtrSSchema * pExt)

#else  /* __STDC__ */
boolean             ValidExtension (pEl, pExt)
PtrElement          pEl;
PtrSSchema       *pExt;

#endif /* __STDC__ */

{
   PtrSSchema        pExtSS, pSS;
   boolean             result;

   result = False;
   if (pEl == NULL)
      return False;
   pSS = pEl->ElSructSchema;
   while (pSS != NULL && *pExt != NULL && !result)
     {
	/* parcourt les extensions du schema pSS */
	pExtSS = pSS->SsNextExtens;
	while (pExtSS != NULL && !result)
	   if (pExtSS->SsCode == (*pExt)->SsCode)
	     {
		*pExt = pExtSS;
		result = True;
	     }
	   else
	      pExtSS = pExtSS->SsNextExtens;
	if (!result)
	   /* pas trouve' */
	   /* cherche un autre schema dans les elements ascendants */
	  {
	     pEl = pEl->ElParent;
	     while (pEl != NULL && pEl->ElSructSchema == pSS)
		pEl = pEl->ElParent;
	     if (pEl == NULL)
		pSS = NULL;
	     else
		pSS = pEl->ElSructSchema;
	  }
     }
   return result;
}


/* ---------------------------------------------------------------------- */
/* |    Cherche parmi les attributs attaches a l'element pointe' par    | */
/* |    pEl s'il en existe un du meme type (eventuellement avec une     | */
/* |    valeur differente) que celui pointe par pAttr.                  | */
/* |    Retourne un pointeur sur cet attribut ou NULL s'il n'existe pas | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrAttribute         GetAttributeOfElement (PtrElement pEl, PtrAttribute pAttr)

#else  /* __STDC__ */
PtrAttribute         GetAttributeOfElement (pEl, pAttr)
PtrElement          pEl;
PtrAttribute         pAttr;

#endif /* __STDC__ */

{
   boolean             found;
   PtrAttribute         pA;

   found = False;
   pA = pEl->ElFirstAttr;
   while (pA != NULL && !found)
     {
	/* attribute 1: Langue, quel que soit le schema */
	if (pA->AeAttrNum == pAttr->AeAttrNum &&
	    (pA->AeAttrNum == 1 ||
	     pA->AeAttrSSchema->SsCode == pAttr->AeAttrSSchema->SsCode))
	   /* c'est l'attribut cherche' */
	   found = True;
	else
	   /* passe a l'attribut suivant du meme element */
	   pA = pA->AeNext;
     }
   return pA;
}


/* ---------------------------------------------------------------------- */
/* |    Retourne un pointeur vers l'attribut qui contient la valeur	| */
/* |    pour l'element pEl de l'attribut pAttr				| */
/* |    Si cet attribut n'a pas de valeur pour l'element pEl, retourne	| */
/* |    NULL.								| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
PtrAttribute         AttributeValue (PtrElement pEl, PtrAttribute pAttr)

#else  /* __STDC__ */
PtrAttribute         AttributeValue (pEl, pAttr)
PtrElement          pEl;
PtrAttribute         pAttr;

#endif /* __STDC__ */

{
   if (pEl != NULL)
      return (GetAttributeOfElement (pEl, pAttr));
   else
      return (NULL);
}

/* ---------------------------------------------------------------------- */
/* |    CanAssociateAttr indique si on peut associer un attribut du	| */
/* |	type de pNewAttr a l'element pEl qui porte eventuellement un	| */
/* |	attribut pAttr de meme type que pNewAttr.			| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             CanAssociateAttr (PtrElement pEl, PtrAttribute pAttr, PtrAttribute pNewAttr, boolean * mandatory)

#else  /* __STDC__ */
boolean             CanAssociateAttr (pEl, pAttr, pNewAttr, mandatory)
PtrElement          pEl;
PtrAttribute         pAttr;
PtrAttribute         pNewAttr;
boolean            *mandatory;

#endif /* __STDC__ */

{
   boolean             requested, allowed;
   SRule              *pRe1;
   int                 locAtt;

   /* cherche si l'attribut a une valeur imposee pour cet element */
   requested = False;
   *mandatory = False;
   if (pAttr != NULL)
      /* l'attribut a une valeur */
      if (pAttr->AeDefAttr)
	 /* on peut modifier un attribut reference, meme impose' */
	 if (pAttr->AeAttrType != AtReferenceAttr)
	   {
	      requested = True;
	      TtaDisplaySimpleMessage (LIB, INFO, LIB_IMPOSED_ATTR);
	   }

   /* si c'est un attribut local, il ne s'applique que s'il correspond au */
   /* type de l'element */
   if (pNewAttr->AeAttrSSchema->SsAttribute[pNewAttr->AeAttrNum - 1].AttrGlobal)
      /*c'est un attribut global, il peut s'appliquer a tous les types d'elements */
      allowed = True;
   else
      /* c'est un attribut local */
     {
	/* a priori, on ne peut pas l'appliquer a ce type d'element */
	allowed = False;
	if (pEl->ElSructSchema->SsCode == pNewAttr->AeAttrSSchema->SsCode)
	  {
	     /* l'attribut est defini dans le meme schema que l'element */
	     pRe1 = &pEl->ElSructSchema->SsRule[pEl->ElTypeNumber - 1];
	     for (locAtt = 0; locAtt < pRe1->SrNLocalAttrs; locAtt++)
		if (pRe1->SrLocalAttr[locAtt] == pNewAttr->AeAttrNum)
		   /* l'attribut fait partie des attributs locaux de ce */
		   /* type d'element, on peut l'appliquer a l'element */
		  {
		     allowed = True;
		     *mandatory = pRe1->SrRequiredAttr[locAtt];
		  }
	  }
	else if (pNewAttr->AeAttrSSchema->SsExtension)
	   /* il s'agit d'un attribut defini dans une extension de schema */
	   if (ValidExtension (pEl, &pNewAttr->AeAttrSSchema))
	      /* cette extension s'applique au schema definissant l'element */
	     {
		pRe1 = ExtensionRule (pEl->ElSructSchema, pEl->ElTypeNumber,
				    pNewAttr->AeAttrSSchema);
		if (pRe1 != NULL)
		   /* ce type d'element a une regle d'extension: pRe1 */
		   /* on cherche si l'attribut fait partie des attributs locaux */
		   /* de cette regle d'extension */
		   for (locAtt = 0; locAtt < pRe1->SrNLocalAttrs; locAtt++)
		      if (pRe1->SrLocalAttr[locAtt] == pNewAttr->AeAttrNum)
			 /* l'attribut fait partie des attributs locaux */
			 /* on peut l'appliquer a l'element */
			{
			   allowed = True;
			   *mandatory = pRe1->SrRequiredAttr[locAtt];
			}
	     }
     }

   return (!requested && allowed);
}


/* ---------------------------------------------------------------------- */
/* |    CanSplitElement: Indique si on peut diviser un element en deux, | */
/* |            au caractere de rang firstChar dans l'element firstEl	| */
/* |            lineBlock indique si on veut diviser un line block ou	| */
/* |            un element non mis en ligne.                            | */
/* |    Retourne True si c'est possible et dans ce cas :                | */
/* |            pList : l'element CsList qui englobe l'element a diviser	| */
/* |            pEl : l'element devant lequel se fera la division.      | */
/* |            pSplitEl : l'element qui va etre divise'.		| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             CanSplitElement (PtrElement firstEl, int firstChar, boolean lineBlock,
	      PtrElement * pList, PtrElement * pEl, PtrElement * pSplitEl)
#else  /* __STDC__ */
boolean             CanSplitElement (firstEl, firstChar, lineBlock, pList, pEl, pSplitEl)
PtrElement          firstEl;
int                 firstChar;
boolean             lineBlock;
PtrElement         *pList;
PtrElement         *pEl;
PtrElement         *pSplitEl;

#endif /* __STDC__ */
{
   PtrElement          pE;
   boolean exctab;

   *pList = NULL;
   *pEl = NULL;
   *pSplitEl = NULL;
   if (firstEl != NULL)
     {
	*pEl = firstEl;
	if (lineBlock)
	   /* on veut couper un line block en deux */
	  {
	     /* Verification des conditions d'application: l'element courant
	        doit etre un atome contenu dans une liste qui elle-meme est une
	        descendante d'une autre liste (sans agregat entre les deux)
	        ou sinon un atome contenu dans une liste */
	     if (!ElementIsReadOnly (firstEl))
		if (firstEl->ElTerminal)
		   if (firstEl->ElLeafType != LtPageColBreak)
		      if (firstEl->ElParent != NULL)
			 if (firstEl->ElPrevious != NULL || firstChar > 1)
			   {
			      /* on cherche d'abord si un element ascendant
				 possede une exception ParagraphBreak */
			      pE = firstEl->ElParent;
			      while (pE != NULL && *pList == NULL)
				{
				   if (TypeHasException (ExcParagraphBreak, pE->ElTypeNumber,
						       pE->ElSructSchema))
				      *pList = AncestorList (pE->ElParent);
				   pE = pE->ElParent;
				}
			      if (*pList == NULL)
				 if (GetElementConstruct (firstEl->ElParent) == CsList)
				    *pList = AncestorList (firstEl->ElParent->ElParent);
				 else
				   {
				      pE = firstEl->ElParent;
				      if (GetElementConstruct (pE) == CsChoice)
					 if (pE->ElParent != NULL)
					    if (GetElementConstruct (pE->ElParent) == CsList)
					       pE = pE->ElParent->ElParent;
				      *pList = AncestorList (pE);
				   }
			   }
	     if (*pList != NULL)
		if (TypeHasException (ExcNoCreate, (*pList)->ElFirstChild->ElTypeNumber,
				    (*pList)->ElFirstChild->ElSructSchema))
		   *pList = NULL;
	     if (*pList != NULL)
		if (!CanChangeNumberOfElem (*pList, 1))
		   *pList = NULL;
	  }
	else
	   /* on veut diviser un "gros" element en deux */
	  {
	     pE = firstEl;
	     if (firstChar <= 1)
		/* la selection est en debut d'element */
	       {
		  /* tant qu'il n'y a pas de frere precedent, on remonte au pere */
		  while (pE->ElParent != NULL && pE->ElPrevious == NULL)
		     pE = pE->ElParent;
		  if (pE->ElPrevious == NULL)
		     /* il n'y a pas de frere precedent, on ne peut pas diviser */
		     pE = NULL;
	       }
	     if (pE != NULL)
	       {
		  /* l'element devant (ou sur) lequel on va couper */
		  *pEl = pE;
		  /* on cherche le premier element CsList ascendant */
		  pE = pE->ElParent;
		  while (pE != NULL && *pList == NULL)
		    {
		       pE = pE->ElParent;
		       if (pE != NULL)
			  if (GetElementConstruct (pE) == CsList)
			     *pList = pE;
		    }
	       }
	  }
     }
   if (*pList != NULL)
     {
	/* cherche l'element de plus haut niveau a dupliquer */
	pE = (*pEl)->ElParent;
	*pSplitEl = pE;
	while (pE != *pList)
	  {
	     *pSplitEl = pE;
	     pE = pE->ElParent;
	  }
	exctab = False;
	if (ThotLocalActions[T_Tableau_Except]!= NULL)
	  (*ThotLocalActions[T_Tableau_Except])(*pSplitEl, &exctab);
	if (exctab)
	   /* c'est la division d'un tableau, on refuse */
	  {
	     *pList = NULL;
	     *pSplitEl = NULL;
	     *pEl = NULL;
	  }
     }
   return (*pList != NULL);
}
