/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Module traitant les attributs
 *
 * Author: V. Quint (INRIA)
 *
 */

#include "libmsg.h"
#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "language.h"
#include "appstruct.h"
#include "appdialogue.h"
#include "typecorr.h"
#include "application.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "page_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "applicationapi_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
#include "attrmenu_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "docs_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "draw_f.h"
#include "exceptions_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "presvariables_f.h"
#include "references_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "structschema_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "undo_f.h"

/*----------------------------------------------------------------------
   Retourne Vrai si les deux elements pointes par pEl1 et pEl2     
   possedent les memes attributs avec les memes valeurs            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            SameAttributes (PtrElement pEl1, PtrElement pEl2)
#else  /* __STDC__ */
ThotBool            SameAttributes (pEl1, pEl2)
PtrElement          pEl1;
PtrElement          pEl2;

#endif /* __STDC__ */
{
   PtrAttribute        pAttr1, pAttr2;
   int                 nAttr1, nAttr2;
   ThotBool            same = TRUE;

   /* nombre d'attributs du 1er element */
   pAttr1 = pEl1->ElFirstAttr;
   nAttr1 = 0;
   /* compte les attributs du 1er element */
   while (pAttr1 != NULL)
     {
	nAttr1++;
	pAttr1 = pAttr1->AeNext;
     }

   /* nombre d'attributs du 2eme element */
   pAttr2 = pEl2->ElFirstAttr;
   nAttr2 = 0;
   /* compte les attributs du 2eme element */
   while (pAttr2 != NULL)
     {
	nAttr2++;
	pAttr2 = pAttr2->AeNext;
     }

   /* compare le nombre d'attributs des deux elements */
   if (nAttr1 != nAttr2)
      same = FALSE;		/* nombres d'attributs differents, fin */
   else
      /* meme nombre d'attributs, compare les attributs et leurs valeurs */
     {
	pAttr1 = pEl1->ElFirstAttr;
	/* 1er attribut du 1er element */
	/* examine tous les attributs du 1er element */
	while (pAttr1 != NULL && same)
	   /* cherche si le 2eme element possede cet attribut du 1er elem */
	  {
	     pAttr2 = GetAttributeOfElement (pEl2, pAttr1);
	     if (pAttr2 == NULL)
		/* le 2eme element n'a pas cet attribut, fin */
		same = FALSE;
	     else
	       {
		  if (pAttr1->AeDefAttr != pAttr2->AeDefAttr)
		     /* valeurs differentes de cet attribut */
		     same = FALSE;
		  else
		     switch (pAttr1->AeAttrType)
			   {
			      case AtNumAttr:
			      case AtEnumAttr:
				 if (pAttr1->AeAttrValue != pAttr2->AeAttrValue)
				    same = FALSE;
				 break;
			      case AtReferenceAttr:
				 same = FALSE;
				 break;
			      case AtTextAttr:
				 same = TextsEqual (pAttr2->AeAttrText, pAttr1->AeAttrText);
				 break;
			      default:
				 break;
			   }
	       }
	     if (same)
		/* meme valeur,passe a l'attribut suivant du 1er element */
		pAttr1 = pAttr1->AeNext;
	  }
     }
   return same;
}

/*----------------------------------------------------------------------
   SetAttrReference fait pointer l'attribut reference pAttr sur    
   l'element pEl.							
   UNIQUEMENT pour une reference *INTERNE*                         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetAttrReference (PtrAttribute pAttr, PtrElement pEl)

#else  /* __STDC__ */
static void         SetAttrReference (pAttr, pEl)
PtrElement          pEl;
PtrAttribute        pAttr;

#endif /* __STDC__ */

{
   PtrReference        pPR1;

   if (pEl != NULL)
     {
	pPR1 = pAttr->AeAttrReference;
	if (pPR1 != NULL)
	  {
	     pPR1->RdNext = pEl->ElReferredDescr->ReFirstReference;
	     if (pPR1->RdNext != NULL)
		pPR1->RdNext->RdPrevious = pAttr->AeAttrReference;
	     pPR1->RdReferred = pEl->ElReferredDescr;
	     pPR1->RdReferred->ReFirstReference = pAttr->AeAttrReference;
	  }
     }
}

/*----------------------------------------------------------------------
   Met a l'element pEl l'attribut auquel est associe'              
   l'exception de numero ExceptNum et retourne un pointeur sur cet 
   attribut. On ne met l'attribut que si l'element ne le possede   
   pas deja. Dans tous les cas, la fonction retourne un pointeur   
   sur l'attribut.                                                 
   S'il s'agit d'un attribut reference, pReferredEl designe	
   l'element sur lequel la reference doit pointer.			
   UNIQUEMENT pour une reference *INTERNE*                         
   Si pReferredEl est NULL, le lien de reference n'est pas etabli. 
   Ce lien peut ensuite etre etabli par la procedure SetReference, 
   en particulier pour un lien externe.                            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrAttribute        AttachAttrByExceptNum (int ExceptNum, PtrElement pEl, PtrElement pReferredEl, PtrDocument pDoc)

#else  /* __STDC__ */
PtrAttribute        AttachAttrByExceptNum (ExceptNum, pEl, pReferredEl, pDoc)
int                 ExceptNum;
PtrElement          pEl;
PtrElement          pReferredEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrAttribute        pAttr;
   PtrReference        pRef;
   PtrElement          pOldReferredEl;
   PtrSSchema          pSS;
   int                 attrNum;
   ThotBool            found;

   /* cherche d'abord le numero de l'attribut */
   pSS = pEl->ElStructSchema;
   attrNum = GetAttrWithException (ExceptNum, pSS);
   if (attrNum == 0)
      /* pas trouve' dans le schema de l'element, on cherche dans les */
      /* les extensions de schema du document. */
     {
	pSS = pDoc->DocSSchema->SsNextExtens;
	found = FALSE;
	/* parcourt tous les schemas d'extension du document */
	while (pSS != NULL && !found)
	  {
	     /* cherche le numero de l'attribut dans cette extension */
	     attrNum = GetAttrWithException (ExceptNum, pSS);
	     if (attrNum != 0)
		found = TRUE;
	     else
		/* passe au schema d'extension suivant */
		pSS = pSS->SsNextExtens;
	  }
     }

   /* cherche si l'attribut est deja present pour l'element */
   pAttr = pEl->ElFirstAttr;
   found = FALSE;
   while (!found && pAttr != NULL)
      if (pAttr->AeAttrNum == attrNum &&
      /* attrNum = 1: Langue, quel que soit le schema de structure */
	  (attrNum == 1 ||
	   pAttr->AeAttrSSchema->SsCode == pSS->SsCode))
	 found = TRUE;
      else
	 pAttr = pAttr->AeNext;
   if (found)
     {
	/* l'element porte deja cet attribut */
	if (pAttr->AeAttrType == AtReferenceAttr && pReferredEl != NULL)
	  {
	     /* c'est un attribut reference, on verifie qu'il pointe bien */
	     /* sur le bon element */
	     /* on cherche d'abord l'element reference' par l'attribut */
	     pOldReferredEl = NULL;
	     /* a priori, pas d'element reference' */
	     if (pAttr->AeAttrReference != NULL)
		if (pAttr->AeAttrReference->RdReferred != NULL)
		   if (!pAttr->AeAttrReference->RdReferred->ReExternalRef)
		      pOldReferredEl = pAttr->AeAttrReference->RdReferred->ReReferredElem;
	     if (pOldReferredEl != pReferredEl)
	       {
		  /* l'attribut ne designe pas le bon element */
		  /* coupe le lien avec l'ancien element reference' */
		  DeleteReference (pAttr->AeAttrReference);
		  if (pReferredEl->ElReferredDescr == NULL)
		     /* le nouvel element a referencer ne l'est pas encore */
		     /* attache a l'element un descripteur d'element reference' */
		    {
		       pReferredEl->ElReferredDescr = NewReferredElDescr (pDoc);
		       pReferredEl->ElReferredDescr->ReReferredElem = pReferredEl;
		    }
		  /* fait pointer la reference sur l'element */
		  SetAttrReference (pAttr, pReferredEl);
	       }
	  }
     }
   else
     {
	/* l'element ne porte pas cet attribut */
	/* cree et intialise l'attribut */
	GetAttribute (&pAttr);
	/* chaine le nouvel attribut en tete des attributs de l'element */
	pAttr->AeNext = pEl->ElFirstAttr;
	pEl->ElFirstAttr = pAttr;
	pAttr->AeAttrSSchema = pSS;
	pAttr->AeAttrNum = attrNum;
	pAttr->AeDefAttr = FALSE;
	pAttr->AeAttrType = pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType;
	if (pAttr->AeAttrType == AtReferenceAttr)
	  {
	     if (pReferredEl != NULL)
		/* l'element pointe' par pReferredEl va etre reference' */
		if (pReferredEl->ElReferredDescr == NULL)
		   /* cet element n'a pas encore de descripteur d'element */
		   /* reference', on lui en associe un */
		  {
		     pReferredEl->ElReferredDescr = NewReferredElDescr (pDoc);
		     pReferredEl->ElReferredDescr->ReReferredElem = pReferredEl;
		  }
	     GetReference (&pRef);
	     pAttr->AeAttrReference = pRef;
	     pRef->RdElement = pEl;
	     pRef->RdAttribute = pAttr;
	     pRef->RdTypeRef = RefFollow;
	     pRef->RdInternalRef = TRUE;
	     /* fait pointer la reference sur l'element */
	     SetAttrReference (pAttr, pReferredEl);
	  }
     }
   return pAttr;
}

/*----------------------------------------------------------------------
   On reaffiche un pave modifie                                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         RedisplayAbsBox (PtrAbstractBox pAbsBox, int boxNum, PtrPSchema pPSchema, int view, PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr)

#else  /* __STDC__ */
static void         RedisplayAbsBox (pAbsBox, boxNum, pPSchema, view, pEl, pDoc, pAttr)
PtrAbstractBox      pAbsBox;
int                 boxNum;
PtrPSchema          pPSchema;
int                 view;
PtrElement          pEl;
PtrDocument         pDoc;
PtrAttribute        pAttr;

#endif /* __STDC__ */

{
   int                 frame, height;
   PtrAbstractBox      pAbChild;

   if (pAbsBox->AbPresentationBox)
     {
	/* c'est un pave de presentation */
	if (pAbsBox->AbTypeNum == boxNum && pAbsBox->AbPSchema == pPSchema)
	   /* c'est bien un pave du type cherche' */
	   /* recalcule la valeur de la variable de presentation */
	    if (NewVariable (pPSchema->PsPresentBox[boxNum - 1].PbContVariable,
			     pAttr->AeAttrSSchema, pPSchema, pAbsBox, pDoc))
	      {

		/* la variable de presentation a change' de valeur */
		pAbsBox->AbChange = TRUE;
		if (AssocView (pEl))
		  frame = (pDoc)->DocAssocFrame[(pEl)->ElAssocNum - 1];
		else
		  frame = (pDoc)->DocViewFrame[view - 1];
		height = PageHeight;
		ChangeConcreteImage (frame, &height, pAbsBox);
		/* on ne reaffiche pas si on est en train de calculer les pages */
		if (PageHeight == 0)
		  DisplayFrame (frame);
	      }
     }
   else
      /* ce n'est pas un pave' de pre'sentation */
      /* cherche parmi les fils les paves de presentation */
      /* de l'element et de ses descendants */
     {
	pAbChild = pAbsBox->AbFirstEnclosed;
	while (pAbChild != NULL)
	  {
	     RedisplayAbsBox (pAbChild, boxNum, pPSchema, view, pEl, pDoc, pAttr);
	     /* next child abstract box */
	     pAbChild = pAbChild->AbNext;
	  }
     }
}

/*----------------------------------------------------------------------
   L'attribut pointe' par pAttr portant sur l'element              
   pointe' par pEl, dans le document pDoc, a change' de valeur.    
   Reafficher toutes les boites de presentation qui utilisent      
   la valeur de cet attribut.					
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RedisplayAttribute (PtrAttribute pAttr, PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                RedisplayAttribute (pAttr, pEl, pDoc)
PtrAttribute        pAttr;
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrPSchema          pPSchema;
   int                 varNum, item, presBox, view;
   ThotBool            found;
   PtrAbstractBox      pAbsBox;
   PresVariable       *pPresVar;

   /* l'attribut dont la valeur a ete modifie' apparait-il dans une */
   /* variable de presentation ? */
   /* schema de presentation de l'attribut */
   pPSchema = pAttr->AeAttrSSchema->SsPSchema;

   if (pPSchema != NULL)
      /* parcourt les variables de presentation du schema */
      for (varNum = 0; varNum < pPSchema->PsNVariables; varNum++)
	{
	   pPresVar = &pPSchema->PsVariable[varNum];
	   found = FALSE;
	   /* examine les items de la variable */
	   for (item = 0; item < pPresVar->PvNItems && !found; item++)
	      if (pPresVar->PvItem[item].ViType == VarAttrValue)
		 if (pPresVar->PvItem[item].ViAttr == pAttr->AeAttrNum)
		    found = TRUE;
	   if (found)
	      /* l'attribut est utilise' dans la variable */
	      /* cherche les boites du schema de presentation qui utilisent */
	      /* cette variable comme contenu */
	     {
		for (presBox = 0; presBox < pPSchema->PsNPresentBoxes; presBox++)
		   if (pPSchema->PsPresentBox[presBox].PbContent == ContVariable
		       && pPSchema->PsPresentBox[presBox].PbContVariable == varNum + 1)
		      /* cette boite a la variable comme contenu */
		      /* cherche dans toutes les vues du document les paves */
		      /* de l'element auquel correspond l'attribut qui sont */
		      /* des instannces de cette boite de presentation */
		      for (view = 0; view < MAX_VIEW_DOC; view++)
			{
			   pAbsBox = pEl->ElAbstractBox[view];
			   while (pAbsBox != NULL)	/* parcourt les paves de l'element */
			      if (pAbsBox->AbElement != pEl)
				 pAbsBox = NULL;	/* on a traite' tous les paves de l'element */
			      else
				 /* c'est un pave' de l'element */
				{
				   RedisplayAbsBox (pAbsBox, presBox + 1, pPSchema, view + 1, pEl, pDoc, pAttr);
				   pAbsBox = pAbsBox->AbNext;
				}
			}
	     }
	}
}

/*----------------------------------------------------------------------
   applique a l'element pointe' par pEl du document pDoc les       
   regles de presentation correspondant a l'attribut decrit dans   
   le bloc pointe' par pAttr.                                      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ApplyAttrPRulesToElem (PtrElement pEl, PtrDocument pDoc,
		      PtrAttribute pAttr, PtrElement pElAttr, ThotBool inherit)

#else  /* __STDC__ */
void                ApplyAttrPRulesToElem (pEl, pDoc, pAttr, pElAttr, inherit)
PtrElement          pEl;
PtrDocument         pDoc;
PtrAttribute        pAttr;
PtrElement          pElAttr;
ThotBool            inherit;

#endif /* __STDC__ */

{
   PtrAttribute        pOldAttr;
   ThotBool            doIt;

   /* on ne traite pas les marques de page */
   if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
     {
	doIt = FALSE;

	/* cherche la valeur de cet attribut pour l'element */
	pOldAttr = GetAttributeOfElement (pEl, pAttr);
	if (inherit)
	   /* traitement de la presentation par heritage */
	  {
	     if (pOldAttr == NULL || pOldAttr == pAttr)
		doIt = TRUE;
	  }
	else
	   /* presentation directe */
	  {
	     if (pOldAttr == NULL)
		doIt = TRUE;
	     /* si pour cet element, cet attribut a une valeur imposee par */
	     /* le schema de structure, on ne fait rien d'autre */
	     else if (!pOldAttr->AeDefAttr)
		doIt = TRUE;
	     /* s'il s'agit d'une suppression de l'attribut, on ne fait rien */
	     if (doIt)
		switch (pAttr->AeAttrType)
		      {
			 case AtEnumAttr:
			    doIt = pAttr->AeAttrValue > 0;
			    break;
			 case AtNumAttr:
			    doIt = pAttr->AeAttrValue >= -MAX_INT_ATTR_VAL
			       && pAttr->AeAttrValue <= MAX_INT_ATTR_VAL;
			    break;
			 case AtTextAttr:
			    if (pAttr->AeAttrText == NULL)
			       doIt = FALSE;
			    break;
			 case AtReferenceAttr:
			    doIt = TRUE;
			    break;
			 default:
			    break;
		      }
	  }

	if (doIt)
	  {
	     /* applique les regles de presentation de l'attribut */
	     UpdatePresAttr (pEl, pAttr, pElAttr, pDoc, FALSE, inherit, NULL);
	     /* reaffiche les variables de presentation qui utilisent */
	     /* l'attribut */
	     RedisplayAttribute (pAttr, pEl, pDoc);
	  }
     }
}


/*----------------------------------------------------------------------
   Applique au sous arbre nte' par pEl du document pDoc les     
   regles de presentation heritees de l'attribut pAttr		
   On arrete la recursion quand on rencontre un fils portant       
   lui-meme un attribut de meme type que pAttr			
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ApplyAttrPRulesToSubtree (PtrElement pEl, PtrDocument pDoc,
				       PtrAttribute pAttr, PtrElement pElAttr)

#else  /* __STDC__ */
void                ApplyAttrPRulesToSubtree (pEl, pDoc, pAttr, pElAttr)
PtrElement          pEl;
PtrDocument         pDoc;
PtrAttribute        pAttr;
PtrElement          pElAttr;

#endif /* __STDC__ */

{
   PtrElement          pChild;
   PtrAttribute        pOldAttr;
   InheritAttrTable   *inheritedAttr;

   pOldAttr = GetAttributeOfElement (pEl, pAttr);
   if (pOldAttr == NULL || pOldAttr == pAttr)
     {
	/* on traite d'abord tout le sous-arbre */
	if (!pEl->ElTerminal)
	   for (pChild = pEl->ElFirstChild; pChild != NULL; pChild = pChild->ElNext)
	      if (pChild->ElStructSchema == pEl->ElStructSchema)
		 /* same structure schema */
		 ApplyAttrPRulesToSubtree (pChild, pDoc, pAttr, pElAttr);

	/* on traite l'element lui-meme */
	/* on cherche d'abord si pEl herite de pAttr */
	if (pEl->ElStructSchema->SsPSchema != NULL)
	   if (pEl->ElStructSchema->SsPSchema->PsNInheritedAttrs[pEl->ElTypeNumber - 1])
	     {
		/* pEl peut heriter d'un attribut */
		if ((inheritedAttr = pEl->ElStructSchema->SsPSchema->
		     PsInheritedAttr[pEl->ElTypeNumber - 1]) == NULL)
		  {
		     /* la table d'heritage n'existe pas. On la cree */
		     CreateInheritedAttrTable (pEl);
		     inheritedAttr = pEl->ElStructSchema->SsPSchema->
			PsInheritedAttr[pEl->ElTypeNumber - 1];
		  }
		if ((*inheritedAttr)[pAttr->AeAttrNum - 1])
		  {
		     /* pEl herite de l'attribut pAttr */
		     /* on lui applique la presentation correspondante */
		     ApplyAttrPRulesToElem (pEl, pDoc, pAttr, pElAttr, TRUE);
		  }
	     }
     }
}

/*----------------------------------------------------------------------
   Applique au sous arbre pointe' par pEl du document pDoc les     
   regles de presentation des attributs dont les valeurs se	
   	comparent a pAttr.						
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ApplyAttrPRules (PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr)

#else  /* __STDC__ */
void                ApplyAttrPRules (pEl, pDoc, pAttr)
PtrElement          pEl;
PtrDocument         pDoc;
PtrAttribute        pAttr;

#endif /* __STDC__ */

{
   PtrElement          pChild;
   ComparAttrTable    *attrValComp;
   int                 i;
   PtrAttribute        pCompAttr;

   if (GetAttributeOfElement (pEl, pAttr) == NULL)
     {
	/* on traite d'abord les descendants */
	if (!pEl->ElTerminal)
	   for (pChild = pEl->ElFirstChild; pChild != NULL; pChild = pChild->ElNext)
	      if (pChild->ElStructSchema == pEl->ElStructSchema)
		 /* meme schema de structure */
		 ApplyAttrPRules (pChild, pDoc, pAttr);
     }

   /* on traite l'element lui-meme */
   /* cherche si pEl possede un attribut se comparant a pAttr */
   if (pEl->ElStructSchema->SsPSchema != NULL)
     {
	if ((attrValComp = pEl->ElStructSchema->SsPSchema->
	     PsComparAttr[pAttr->AeAttrNum - 1]) == NULL)
	  {
	     /* la table de comparaison n'existe pas. On la creee */
	     CreateComparAttrTable (pAttr);
	     attrValComp = pEl->ElStructSchema->SsPSchema->
		PsComparAttr[pAttr->AeAttrNum - 1];
	  }
	if (attrValComp != NULL)
	   for (i = pEl->ElStructSchema->SsNAttributes; i > 0; i--)
	     {
		if ((*attrValComp)[i - 1])
		   /* l'attribut de rang i se compare a pAttr */
		   if ((pCompAttr = GetTypedAttrForElem (pEl, i, pAttr->AeAttrSSchema)) != NULL)
		      /* pEl possede un attribut comme celui de rang i */
		      ApplyAttrPRulesToElem (pEl, pDoc, pCompAttr, pEl, FALSE);
	     }
     }
}


/*----------------------------------------------------------------------
   Chaine les elements liberes par les fusions de texte.           
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                KeepFreeElements (PtrElement pEl, PtrElement * pFirstFree)

#else  /* __STDC__ */
void                KeepFreeElements (pEl, pFirstFree)
PtrElement          pEl;
PtrElement         *pFirstFree;

#endif /* __STDC__ */

{
   pEl->ElNext = *pFirstFree;
   *pFirstFree = pEl;
}


/*----------------------------------------------------------------------
   Change la langue de toutes les feuilles de texte dans le        
   sous-arbre de l'element pointe' par pEl, appartenant au         
   document dont le contexte est pointe' par pDoc                  
   Le parametre force est vrai quand on veut forcer le changement  
   de langue.                                                      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ChangeLanguage (PtrDocument pDoc, PtrElement pEl, Language lang, ThotBool force)

#else  /* __STDC__ */
void                ChangeLanguage (pDoc, pEl, lang, force)
PtrDocument         pDoc;
PtrElement          pEl;
Language            lang;
ThotBool            force;

#endif /* __STDC__ */

{
   int                 view;
   PtrElement          pChild;
   PtrAbstractBox      pAbsBox;
   Language	       oldElLang;

   if (pEl == NULL)
      return;

   if (!pEl->ElTerminal)
      /* l'element n'est pas une feuille, on traite son sous-arbre */
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL)
	  {
	     /* si un descendant a un attribut Langue, on ne le change pas */
	     if (GetTypedAttrForElem (pChild, 1, NULL) == NULL)
		ChangeLanguage (pDoc, pChild, lang, force);
	     pChild = pChild->ElNext;
	  }
     }
   else
      /* l'element est une feuille */
   if (pEl->ElLeafType == LtText && pEl->ElLanguage != lang)
      /* c'est du texte dans une autre langue */
    	 /* change la langue de l'element */
	{
	  oldElLang = pEl->ElLanguage;
	   pEl->ElLanguage = lang;
	   /* parcourt toutes les vues du document pour changer les */
	   /* paves de l'element */
	   for (view = 0; view < MAX_VIEW_DOC; view++)
	      if (pEl->ElAbstractBox[view] != NULL)
		 /* l'element a au moins un pave dans la vue */
		{
		   pAbsBox = pEl->ElAbstractBox[view];
		   /* saute les paves de presentation de l'element */
		   while (pAbsBox->AbPresentationBox && pAbsBox->AbElement == pEl)
		      pAbsBox = pAbsBox->AbNext;
		   if (!pAbsBox->AbDead)
		      /* traite le pave' principal de l'element */
		      /* change la langue du pave */
		     {
			pAbsBox->AbLanguage = lang;
			if (force || TtaGetAlphabet (oldElLang) != TtaGetAlphabet (lang))
			  /* cette langue s'ecrit dans un alphabet different */
			  /* ou la langue est forcee */
			  {
			    pAbsBox->AbChange = TRUE;
			    /* conserve le pointeur sur le pave a reafficher */
			    if (AssocView (pEl))
			      pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1] =
				Enclosing (pAbsBox, pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1]);
			    else
			      pDoc->DocViewModifiedAb[view] =
				Enclosing (pAbsBox, pDoc->DocViewModifiedAb[view]);
			  }
		     }
		}
	}
}


/*----------------------------------------------------------------------
   Verifie si l'attribut pointe' par pAttr et appartenant a pEl	
   contient un attribut qui initialise un (des) compteur(s).	
   	Si oui, met a jour les valeurs de ce(s) compteur(s).		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                UpdateCountersByAttr (PtrElement pEl, PtrAttribute pAttr, PtrDocument pDoc)

#else  /* __STDC__ */
void                UpdateCountersByAttr (pEl, pAttr, pDoc)
PtrElement          pEl;
PtrAttribute        pAttr;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 counter;
   PtrPSchema          pPS;
   Counter            *pCnt;

   /* parcourt les compteurs definis dans le schema de presentation */
   /* correspondant a l'attribut pour trouver si l'attribut est la valeur */
   /* initiale d'un compteur */
   {
      /* schema de presentation qui s'applique a l'attribut */
      pPS = pAttr->AeAttrSSchema->SsPSchema;
      if (pPS != NULL)
	 for (counter = 0; counter < pPS->PsNCounters; counter++)
	   {
	      pCnt = &pPS->PsCounter[counter];
	      if (pCnt->CnItem[0].CiInitAttr == pAttr->AeAttrNum ||
		  pCnt->CnItem[0].CiReinitAttr == pAttr->AeAttrNum)
		 /* Si c'est un compteur de pages, on renumerote toutes les */
		 /* pages de l'arbre abstrait traite' */
		{
		   if (pCnt->CnItem[0].CiCntrOp == CntrRank && pCnt->CnItem[0].
		       CiElemType == PageBreak + 1)
		      /* c'est un compteur de la forme CntrRank of Page(view) */
		      ComputePageNum (pEl, pCnt->CnItem[0].CiViewNum);
		   else
		      /* c'est un compteur de la forme Set...Add n on Page(view) */
		      if (pCnt->CnItem[0].CiCntrOp == CntrSet
			  && pCnt->CnItem[1].CiElemType == PageBreak + 1)
		      ComputePageNum (pEl, pCnt->CnItem[1].CiViewNum);
		   /* fait reafficher toutes les boites de presentation dependant */
		   /* de la valeur de ce compteur */
		   UpdateBoxesCounter (pEl, pDoc, counter + 1, pPS, pEl->ElStructSchema);
		}
	   }
   }
}

/*----------------------------------------------------------------------
   RemoveAttrPresentation
   Retire la presentation associee a l'attribut pAttr dans les	
   images de l'element pEl.					
   Si pCompAttr != NULL, les regles de presentation dependant	
   de la comparaison doivent prendre pCompAttr comme attribut de   
   comparaison et ne pas en chercher d'autre dans les ascendants	
   de pEl.                                                         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RemoveAttrPresentation (PtrElement pEl, PtrDocument pDoc,
		                     PtrAttribute pAttr, PtrElement pElAttr,
				     ThotBool inherit, PtrAttribute pCompAttr)

#else  /* __STDC__ */
void                RemoveAttrPresentation (pEl, pDoc, pAttr, pElAttr, inherit,
					    pCompAttr)
PtrElement          pEl;
PtrDocument         pDoc;
PtrAttribute        pAttr;
PtrElement          pElAttr;
ThotBool            inherit;
PtrAttribute        pCompAttr;

#endif /* __STDC__ */

{
   /* on ne traite pas les marques page */
   if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
     {
	/* supprime la presentation attachee a la valeur de l'attribut, si */
	/* elle n'est pas nulle */
	UpdatePresAttr (pEl, pAttr, pElAttr, pDoc, TRUE, inherit, pCompAttr);

	/* reaffiche les variables de presentation qui utilisent */
	/* l'attribut */
	RedisplayAttribute (pAttr, pEl, pDoc);
	UpdateCountersByAttr (pEl, pAttr, pDoc);
     }
}

/*----------------------------------------------------------------------
   Retire du sous arbre pEl du document pDoc les  regles de	
   	presentation heritees de l'attribut pAttr.			
   Si pEl porte lui-meme un attribut de type pAttr, on arrete.	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RemoveInheritedAttrPresent (PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr, PtrElement pElAttr)

#else  /* __STDC__ */
void                RemoveInheritedAttrPresent (pEl, pDoc, pAttr, pElAttr)
PtrElement          pEl;
PtrDocument         pDoc;
PtrAttribute        pAttr;
PtrElement          pElAttr;

#endif /* __STDC__ */

{
   PtrElement          pChild;
   InheritAttrTable   *inheritedAttr;

   if (GetAttributeOfElement (pEl, pAttr) == NULL)
     {
	/* pEl does not have an attribute of that type */
	/* process the subtree */
	if (!pEl->ElTerminal)
	   for (pChild = pEl->ElFirstChild; pChild != NULL; pChild = pChild->ElNext)
	      if (pChild->ElStructSchema == pEl->ElStructSchema)
		 /* same structure schema */
		 RemoveInheritedAttrPresent (pChild, pDoc, pAttr, pElAttr);

	/* process element pEl itself */
	if (pEl->ElStructSchema->SsPSchema != NULL)
	   if (pEl->ElStructSchema->SsPSchema->PsNInheritedAttrs[pEl->ElTypeNumber - 1])
	     {
		/* pEl can inherit some presentation rules from attributes */
		if ((inheritedAttr = pEl->ElStructSchema->SsPSchema->
		     PsInheritedAttr[pEl->ElTypeNumber - 1]) == NULL)
		  {
		     /* la table d'heritage n'existe pas. On la cree */
		     CreateInheritedAttrTable (pEl);
		     inheritedAttr = pEl->ElStructSchema->SsPSchema->
			PsInheritedAttr[pEl->ElTypeNumber - 1];
		  }
		if ((*inheritedAttr)[pAttr->AeAttrNum - 1])
		   RemoveAttrPresentation (pEl, pDoc, pAttr, pElAttr,
					   TRUE, NULL);
	     }
     }
}


/*----------------------------------------------------------------------
   Retire du sous arbre pointe' par pEl du document pDoc les     
   regles de presentation provenant de la comparaison avec		
   l'attribut pAttr.						
   On arrete la recursion quand on rencontre un fils portant       
   lui-meme un attribut de meme type que pAttr			
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RemoveComparAttrPresent (PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr)

#else  /* __STDC__ */
void                RemoveComparAttrPresent (pEl, pDoc, pAttr)
PtrElement          pEl;
PtrDocument         pDoc;
PtrAttribute        pAttr;

#endif /* __STDC__ */

{
   PtrElement          pChild;
   ComparAttrTable    *attrValComp;
   int                 i;
   PtrAttribute        pCompAttr;

   if (GetAttributeOfElement (pEl, pAttr) == NULL)
     {
	/* on traite d'abord les descendants */
	if (!pEl->ElTerminal)
	   for (pChild = pEl->ElFirstChild; pChild != NULL; pChild = pChild->ElNext)
	      if (pChild->ElStructSchema == pEl->ElStructSchema)
		 /* meme schema de structure */
		 RemoveComparAttrPresent (pChild, pDoc, pAttr);
     }

   /* on traite l'element lui-meme */
   /* cherche si pEl possede un attribut se comparant a pAttr */
   if (pEl->ElStructSchema->SsPSchema != NULL)
     {
	if ((attrValComp = pEl->ElStructSchema->SsPSchema->
	     PsComparAttr[pAttr->AeAttrNum - 1]) == NULL)
	  {
	     /* la table de comparaison n'existe pas. On la creee */
	     CreateComparAttrTable (pAttr);
	     attrValComp = pEl->ElStructSchema->SsPSchema->
		PsComparAttr[pAttr->AeAttrNum - 1];
	  }
	if (attrValComp != NULL)
	   for (i = pEl->ElStructSchema->SsNAttributes; i > 0; i--)
	     {
		if ((*attrValComp)[i - 1])
		   /* l'attribut de rang i se compare a pAttr */
		   if ((pCompAttr = GetTypedAttrForElem (pEl, i, pAttr->AeAttrSSchema)) != NULL)
		      /* pEl possede un attribut comme celui de rang i */
		      RemoveAttrPresentation (pEl, pDoc, pCompAttr, pEl,
					      FALSE, pAttr);
	     }
     }
}

/*----------------------------------------------------------------------
   Ajoute un attribut pNewAttr a l'element pEl avant l'attribut pAttrNext
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrAttribute        AddAttrToElem (PtrElement pEl, PtrAttribute pNewAttr, PtrAttribute pAttrNext)
#else  /* __STDC__ */
PtrAttribute        AddAttrToElem (pEl, pNewAttr, pAttrNext)
PtrElement          pEl;
PtrAttribute        pNewAttr;
PtrAttribute        pAttrNext;
#endif /* __STDC__ */
{
  PtrAttribute        pAttr, pA;
  PtrReference        pRef;
  int                 len;

  GetAttribute (&pAttr);
  /* on chaine cet attribut apres le dernier attribut de  l'element */
  if (pEl)
    if (pEl->ElFirstAttr == NULL || pEl->ElFirstAttr == pAttrNext)
      {
      /* c'est le 1er attribut de l'element */
      pAttrNext = pEl->ElFirstAttr;
      pEl->ElFirstAttr = pAttr;
      }
    else
      {
      /* cherche l'attribut qui doit etre le suivant */
      pA = pEl->ElFirstAttr;
      while (pA->AeNext != pAttrNext)
	pA = pA->AeNext;
      /* chaine le nouvel attribut */
      pA->AeNext = pAttr;
      }
  pAttr->AeNext = pAttrNext;
  pAttr->AeAttrSSchema = pNewAttr->AeAttrSSchema;
  pAttr->AeAttrNum = pNewAttr->AeAttrNum;
  pAttr->AeDefAttr = FALSE;
  pAttr->AeAttrType = pNewAttr->AeAttrType;
  switch (pAttr->AeAttrType)
    {
    case AtEnumAttr:
    case AtNumAttr:
      pAttr->AeAttrValue = pNewAttr->AeAttrValue;
      break;
    case AtTextAttr:
      if (pNewAttr->AeAttrText != NULL)
	{
	  GetTextBuffer (&pAttr->AeAttrText);
	  CopyTextToText (pNewAttr->AeAttrText, pAttr->AeAttrText, &len);
	}
      break;
    case AtReferenceAttr:
      GetReference (&pRef);
      pAttr->AeAttrReference = pRef;
      pRef->RdElement = pEl;
      pRef->RdAttribute = pAttr;
      pRef->RdReferred = pNewAttr->AeAttrReference->RdReferred;
      pRef->RdTypeRef = pNewAttr->AeAttrReference->RdTypeRef;
      pRef->RdInternalRef = pNewAttr->AeAttrReference->RdInternalRef;
      /* chaine la reference du nouvel attribut apres celle de */
      /* pNewAttr. */
      if (pEl)
	 {
         pRef->RdNext = pNewAttr->AeAttrReference->RdNext;
         if (pRef->RdNext != NULL)
	   pRef->RdNext->RdPrevious = pRef;
         pRef->RdPrevious = pNewAttr->AeAttrReference;
         if (pRef->RdPrevious != NULL)
	   pRef->RdPrevious->RdNext = pRef;
	 }
      break;
    default:
      break;
    }
  return (pAttr);
}

/*----------------------------------------------------------------------
   AttachAttrWithValue
   Met dans l'element pEl la valeur de l'attribut pNewAttr		
   Les regles de presentation de cette nouvelle valeur sont	
   appliquees a l'element.
   Egalement utilise' pour supprimer un attribut et desappliquer les
   regles de presentation correspondantes.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AttachAttrWithValue (PtrElement pEl, PtrDocument pDoc, PtrAttribute pNewAttr)
#else  /* __STDC__ */
void                AttachAttrWithValue (pEl, pDoc, pNewAttr)
PtrElement          pEl;
PtrdocumentA        pDoc;
PtrAttribute        pNewAttr;

#endif /* __STDC__ */
{
   PtrAttribute        pAttr, pAttrAsc, pAttrNext;
   PtrElement          pChild, pElAttr;
   ThotBool            suppress, compare, inherit, mandatory, create;
   NotifyAttribute     notifyAttr;

   if (pNewAttr == NULL || pNewAttr->AeAttrNum == 0)
     /* it's not a valid attribute */
     return;
   mandatory = FALSE;
   pAttrNext = NULL;
   /* l'element porte-t-il deja un attribut du meme type ? */
   pAttr = AttributeValue (pEl, pNewAttr);
   create = (pAttr == NULL);

   /* peut-on appliquer l'attribut a l'element ? */
   if (create && !CanAssociateAttr (pEl, pAttr, pNewAttr, &mandatory))
      return;

   /* est-ce une suppression d'attribut */
   suppress = TRUE;
   switch (pNewAttr->AeAttrType)
	 {
	    case AtEnumAttr:
	       if (pNewAttr->AeAttrValue != 0)
		  suppress = FALSE;
	       break;

	    case AtNumAttr:
	       if (pNewAttr->AeAttrValue >= -MAX_INT_ATTR_VAL
		   && pNewAttr->AeAttrValue <= MAX_INT_ATTR_VAL)
		  suppress = FALSE;
	       break;

	    case AtTextAttr:
	       if (pNewAttr->AeAttrText != NULL)
		  suppress = FALSE;
	       break;

	    case AtReferenceAttr:
	       if (pNewAttr->AeAttrReference != NULL)
		  if (pNewAttr->AeAttrReference->RdReferred != NULL)
		     suppress = FALSE;
	       break;

	    default:
	       break;
	 }
   if (suppress)
     {
	if (pAttr == NULL)
	   /* suppression d'un attribut inexistant */
	   return;
	if (!mandatory)
	   /* si c'est un attribut Langue sur un element racine, il est obligatoire */
	   if (pEl->ElParent == NULL)
	      if (pNewAttr->AeAttrNum == 1)
		 mandatory = TRUE;
	if (mandatory)
	   /* suppression d'un attribut obligatoire. Interdit */
	  {
	     TtaDisplaySimpleMessage (INFO, LIB, TMSG_LIB_ATTR_REQUIRED_NOT_DELETED);
	     return;
	  }
     }

   /* faut-il traiter des heritages et comparaisons d'attributs */
   inherit = FALSE;
   compare = FALSE;
   if (pNewAttr->AeAttrSSchema->SsPSchema != NULL)
     {
       inherit = (pNewAttr->AeAttrSSchema->SsPSchema->PsNHeirElems[pNewAttr->AeAttrNum - 1] != 0);
       compare = (pNewAttr->AeAttrSSchema->SsPSchema->PsNComparAttrs[pNewAttr->AeAttrNum - 1] != 0);
     }
   if (inherit || compare)
      /* cherche le premier attribut de meme type sur un ascendant de pEl */
      pAttrAsc = GetTypedAttrAncestor (pEl, pNewAttr->AeAttrNum,
				       pNewAttr->AeAttrSSchema, &pElAttr);
   else
      pAttrAsc = NULL;

   /* prepare et envoie l'evenement pour l'application */
   if (create)
      notifyAttr.event = TteAttrCreate;
   else if (suppress)
      notifyAttr.event = TteAttrDelete;
   else
      notifyAttr.event = TteAttrModify;
   notifyAttr.document = (Document) IdentDocument (pDoc);
   notifyAttr.element = (Element) pEl;
   notifyAttr.attribute = (Attribute) pAttr;
   notifyAttr.attributeType.AttrSSchema = (SSchema) (pNewAttr->AeAttrSSchema);
   notifyAttr.attributeType.AttrTypeNum = pNewAttr->AeAttrNum;
   if (!CallEventAttribute (&notifyAttr, TRUE))
     {
	/* l'application accepte l'operation */
	/* on supprime sur pEl et son sous arbre les regles de presentation
	   anciennes liees a l'heritage d'un attribut du type pAttr porte'
	   soit par pEl soit par un ascendant */
	if (pAttr)
	  {
	    /* register the attribute in history */
	    if (ThotLocalActions[T_attraddhistory] != NULL)
	      (*ThotLocalActions[T_attraddhistory]) (pAttr, pEl, pDoc, TRUE, FALSE);
	    /* detache l'attribut de l'element s'il y a lieu */
	    pAttrNext = pAttr->AeNext;
	    /* supprime l'attribut */
	    RemoveAttribute (pEl, pAttr);
	    /* heritage et comparaison sont lies a un attribut de pEl */
	    /* On supprime d'abord les regles de presentation liees a
	       l'attribut sur l'element lui-meme */
	    RemoveAttrPresentation (pEl, pDoc, pAttr, pEl, FALSE, NULL);
	    /* indique que le document a ete modifie' */
	    /* un changement d'attribut vaut dix caracteres saisis */
	    SetDocumentModified (pDoc, TRUE, 10);
	    /* On supprime de pEl de son sous-arbre  la presentation venant
	       de l'heritage de cet attribut par le sous-arbre, s'il existe
	       des elements heritants de celui-ci */
	    if (inherit)
	       RemoveInheritedAttrPresent (pEl, pDoc, pAttr, pEl);
	    /* On supprime des elements du sous arbre pEl la presentation
	       venant de la comparaison d'un attribut du sous-arbre avec ce
	       type d'attribut */
	    if (!pEl->ElTerminal && compare)
	      for (pChild = pEl->ElFirstChild; pChild != NULL; pChild = pChild->ElNext)
		RemoveComparAttrPresent (pChild, pDoc, pAttr);
	    DeleteAttribute (pEl, pAttr);
	  }

	else if (pAttrAsc)
	  {
	     /* heritage et comparaison sont dus a un attribut d'un ascendant
	        de pEl */
	     /* on supprime du sous arbre pEl la presentation venant de
	        l'heritage de cet attribut par le sous-arbre, s'il existe des
	        elements heritants de celui-ci */
	     if (inherit)
		RemoveInheritedAttrPresent (pEl, pDoc, pAttrAsc, pElAttr);
	     /* on supprime du sous-arbre pEl la presentation venant de la
	        comparaison d'un attribut du sous-arbre avec ce type d'attribut */
	     if (compare)
		RemoveComparAttrPresent (pEl, pDoc, pAttrAsc);
	  }

	if (inherit || compare)
	   /* cherche le premier attribut de meme type sur un ascendant de pEl */
	   pAttrAsc = GetTypedAttrAncestor (pEl, pNewAttr->AeAttrNum,
					 pNewAttr->AeAttrSSchema, &pElAttr);
	else
	   pAttrAsc = NULL;

	/* on met l'attribut */
	if (!suppress)
	  {
	    /* add a copy of the new attribute before pAttrNext */
	    pAttr = AddAttrToElem (pEl, pNewAttr, pAttrNext);
	    /* register the attribute in history */
	    if (ThotLocalActions[T_attraddhistory] != NULL)
	      (*ThotLocalActions[T_attraddhistory]) (pAttr, pEl, pDoc, FALSE, TRUE);
	    /* indique que le document a ete modifie' */
	    /* un changement d'attribut vaut dix caracteres saisis */
	    SetDocumentModified (pDoc, TRUE, 10);
	    /* traitement special a l'ajout d'un attribut a un element d'un
	       objet Draw */
	    DrawAddAttr (&pAttr, pEl);
	  }

	else
	   /* suppression */
	  {
	     /* traitement special a la suppression d'un attribut a un element
	        d'un objet Draw */
	     DrawSupprAttr (pNewAttr, pEl);
	     /* valide les modifications */
	     AbstractImageUpdated (pDoc);
	     /* la suppression est maintenant prise en compte dans les
	        copies-inclusions de l'element */
	     RedisplayCopies (pEl, pDoc, TRUE);
	  }

	/* on applique les regles de presentation */
	if (pAttr != NULL && !suppress)
	  {
	     /* applique les regles du nouvel attribut */
	     /* applique d'abord les regles de presentation associees a
	        l'attribut sur l'element lui-meme */
	     ApplyAttrPRulesToElem (pEl, pDoc, pAttr, pEl, FALSE);
	     /* applique les regles de presentation venant de l'heritage de
	        cet attribut par le sous-arbre s'il existe des elements qui
	        heritent */
	     if (inherit)
		ApplyAttrPRulesToSubtree (pEl, pDoc, pAttr, pEl);
	     /* applique sur les elements du sous-arbre les regles de
	        presentation venant de la comparaison d'un attribut du
	        sous-arbre avec ce type d'attribut */
	     if (!pEl->ElTerminal && compare)
		for (pChild = pEl->ElFirstChild; pChild != NULL; pChild = pChild->ElNext)
		   ApplyAttrPRules (pChild, pDoc, pAttr);
	     if (pAttr->AeAttrType == AtNumAttr)
		/* s'il s'agit d'un attribut initialisant un compteur, il */
		/* faut mettre a jour les boites utilisant ce compteur */
		UpdateCountersByAttr (pEl, pNewAttr, pDoc);

	     /* le nouvel attribut est pris en compte dans les 
	        les copies-inclusions de l'element */
	     RedisplayCopies (pEl, pDoc, TRUE);
	  }
	else if (pAttrAsc != NULL && suppress)
	  {
	     /* applique les regles venant de l'heritage ou de la comparaison
	        avec un ascendant */
	     /* applique sur les elements du sous-arbre les regles de
	        presentation venant de l'heritage de cet attribut par le
	        sous-arbre, s'il existe des elements qui heritent */
	     ApplyAttrPRulesToSubtree (pEl, pDoc, pAttrAsc, pElAttr);
	     /* applique sur les elements du sous-arbre les regles de
	        presentation venant de la comparaison d'un attribut de
	        sous-arbre avec ce type d'attribut */
	     ApplyAttrPRules (pEl, pDoc, pAttrAsc);
	  }

	/* prepare et envoie a l'application l'evenement TteAttrCreate.Post */
	if (notifyAttr.event == TteAttrCreate)
	  notifyAttr.attribute = (Attribute) pAttr;
	else if (notifyAttr.event == TteAttrDelete)
	  notifyAttr.attribute = NULL;
	CallEventAttribute (&notifyAttr, FALSE);
     }
}

/*----------------------------------------------------------------------
   Retourne un pointeur sur l'attribut associe' a l'exception      
   ExceptNum et porte' par l'element pEl.				
   	On retourne NULL si l'element n'a pas cet attribut.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrAttribute        GetAttrByExceptNum (PtrElement pEl, int ExceptNum)
#else  /* __STDC__ */
PtrAttribute        GetAttrByExceptNum (pEl, ExceptNum)
PtrElement          pEl;
int                 ExceptNum;
#endif /* __STDC__ */
{
   PtrSSchema          pSS;
   PtrAttribute        pAttr;
   ThotBool            found;
   int                 attrNum;

   pSS = pEl->ElStructSchema;
   /* on recupere le numero d'attribut associe a l'exception */
   attrNum = GetAttrWithException (ExceptNum, pEl->ElStructSchema);
   if (attrNum != 0)
     {
	/* on cherche un attribut sur l'element */
	pAttr = pEl->ElFirstAttr;
	found = FALSE;
	while (pAttr != NULL && !found)
	   if (pAttr->AeAttrSSchema->SsCode == pSS->SsCode && pAttr->AeAttrNum == attrNum)
	      found = TRUE;
	   else
	      pAttr = pAttr->AeNext;
	return pAttr;
     }
   else
      return NULL;
}

/* ----------------------------------------------------------------------
   TtaGetAttribute

   Returns an attribute of a given type associated with a given element.

   Parameters:
   element: the element of interest.
   attributeType: type of the desired attribute. If the attribute "Language"
   is searched, attributeType.AttrTypeNum must be 1. If the attribute
   "Language" is searched whatever its structure schema,
   attributeType.AttrSSchema must be NULL. A NULL
   attributeType.AttrSSchema is accepted only when an attribute
   "Language" is searched.

   Return value:
   the attribute found, or NULL if the element  does not have this
   type of attribute.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
Attribute           TtaGetAttribute (Element element, AttributeType attributeType)
#else  /* __STDC__ */
Attribute           TtaGetAttribute (element, attributeType)
Element             element;
AttributeType       attributeType;

#endif /* __STDC__ */
{
   PtrAttribute        pAttr;
   PtrAttribute        attribute;
   ThotBool            found;
   ThotBool            error;

   UserErrorCode = 0;
   attribute = NULL;
   if (element == NULL || (attributeType.AttrSSchema == NULL && attributeType.AttrTypeNum != 1))
      /* attributeType.AttrTypeNum = 1 : attribute Language in the whole schema */
      TtaError (ERR_invalid_parameter);
   else
     {
	error = FALSE;
	/* No other verification if the attibute is "language" */
	if (attributeType.AttrTypeNum != 1)
	   if (attributeType.AttrTypeNum < 1 ||
	       attributeType.AttrTypeNum > ((PtrSSchema) (attributeType.AttrSSchema))->SsNAttributes)
	      error = TRUE;
	if (error)
	   TtaError (ERR_invalid_attribute_type);
	else
	  {
	     attribute = NULL;
	     pAttr = ((PtrElement) element)->ElFirstAttr;
	     found = FALSE;
	     while (pAttr != NULL && !found)
	       {
		  if (pAttr->AeAttrNum == attributeType.AttrTypeNum)
		     /* Same attribute number */
		     if (attributeType.AttrSSchema == NULL)
			/* The structure schema does not interest us */
			found = TRUE;
		     else if (pAttr->AeAttrSSchema->SsCode ==
			 ((PtrSSchema) (attributeType.AttrSSchema))->SsCode)
			/* Same schema of structure */
			found = TRUE;
		  if (found)
		     attribute = pAttr;
		  else
		     pAttr = pAttr->AeNext;
	       }
	  }
     }
   return ((Attribute) attribute);
}

 /* ----------------------------------------------------------------------
   TtaGetTextAttributeLength

   Returns the length of a given attribute of type text.

   Parameter:
   attribute: the attribute of interest.

   Return values:
   length of the character string contained in the attribute.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 TtaGetTextAttributeLength (Attribute attribute)
#else  /* __STDC__ */
int                 TtaGetTextAttributeLength (attribute)
Attribute           attribute;
#endif /* __STDC__ */
{
   int                 length;
   PtrTextBuffer       pBT;

   UserErrorCode = 0;
   length = 0;
   if (attribute == NULL)
     TtaError (ERR_invalid_attribute_type);
   else if (((PtrAttribute) attribute)->AeAttrType != AtTextAttr)
     TtaError (ERR_invalid_attribute_type);
   else
     {
       pBT = ((PtrAttribute) attribute)->AeAttrText;
       while (pBT != NULL)
	 {
	   length += pBT->BuLength;
	   pBT = pBT->BuNext;
	 }
     }
   return length;
}

/* ----------------------------------------------------------------------
   TtaGiveTextAttributeValue

   Returns the value of a given attribute of type text.

   Parameters:
   attribute: the attribute of interest.
   buffer: address of the buffer that will contain the value of the attribute.
   length: size of the buffer (in bytes).

   Return values:
   buffer: character string representing the value of the attribute.
   length: actual length of the character string.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaGiveTextAttributeValue (Attribute attribute, STRING buffer, int *length)
#else  /* __STDC__ */
void                TtaGiveTextAttributeValue (attribute, buffer, length)
Attribute           attribute;
STRING              buffer;
int                *length;
#endif /* __STDC__ */

{
  UserErrorCode = 0;
  *buffer = EOS;
  if (attribute == NULL)
    TtaError (ERR_invalid_attribute_type);
   else if (((PtrAttribute) attribute)->AeAttrType != AtTextAttr)
    TtaError (ERR_invalid_attribute_type);
  else
    CopyTextToString (((PtrAttribute) attribute)->AeAttrText, buffer, length);
}
