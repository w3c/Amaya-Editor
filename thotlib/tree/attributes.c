/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
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
#include "app.h"
#include "appdialogue.h"
#include "typecorr.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "page_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

#include "tree_f.h"
#include "attrpresent_f.h"
#include "attrmenu_f.h"
#include "structcreation_f.h"
#include "createabsbox_f.h"
#include "draw_f.h"
#include "callback_f.h"
#include "exceptions_f.h"
#include "buildboxes_f.h"
#include "memory_f.h"
#include "structmodif_f.h"
#include "changeabsbox_f.h"
#include "references_f.h"
#include "boxselection_f.h"
#include "structselect_f.h"
#include "structschema_f.h"
#include "content_f.h"
#include "presvariables_f.h"
#include "applicationapi_f.h"
#include "frame_f.h"

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
   boolean             found;

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

	      /* la variable de presentation a change' de valeur */
	     {
		pAbsBox->AbChange = TRUE;
		if (AssocView (pEl))
		   frame = (pDoc)->DocAssocFrame[(pEl)->ElAssocNum - 1];
		else
		   frame = (pDoc)->DocViewFrame[view - 1];
#ifdef __COLPAGE__
		height = BreakPageHeight;
#else  /* __COLPAGE__ */
		height = PageHeight;
#endif /* __COLPAGE__ */
		ChangeConcreteImage (frame, &height, pAbsBox);
		/* on ne reaffiche pas si on est en train de calculer les pages */
#ifdef __COLPAGE__
		if (BreakPageHeight == 0)
#else  /* __COLPAGE__ */
		if (PageHeight == 0)
#endif /* __COLPAGE__ */
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
   boolean             found;
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
   IsolateSelection
   Si la selection passee en parametre commence ou finit sur des   
   elements partiellement selectionnes, ces elements sont coupes   
   en deux et leurs paves egalement.                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                IsolateSelection (PtrDocument pDoc, PtrElement * pFirstSel, PtrElement * pLastSel, int *firstChar, int *lastChar, boolean createEmpty)

#else  /* __STDC__ */
void                IsolateSelection (pDoc, pFirstSel, pLastSel, firstChar, lastChar, createEmpty)
PtrDocument         pDoc;
PtrElement         *pFirstSel;
PtrElement         *pLastSel;
int                *firstChar;
int                *lastChar;
boolean		    createEmpty;

#endif /* __STDC__ */

{
   PtrElement	       pEl;
   int                 view;
   boolean	       done;

   if (*firstChar > 1)
      if ((*pFirstSel)->ElTerminal && (*pFirstSel)->ElLeafType == LtText)
	 /* la selection courante commence a l'interieur du premier element */
	 /* selectionne */
	 /* coupe le premier element selectionne' */
	{
	   SplitBeforeSelection (pFirstSel, firstChar, pLastSel, lastChar, pDoc);
	   /* prepare la creation des paves de la 2eme partie */
	   for (view = 0; view < MAX_VIEW_DOC; view++)
	      if (!AssocView (*pFirstSel))
		{
		   if (pDoc->DocView[view].DvPSchemaView > 0)
		      /* la vue est ouverte */
		      pDoc->DocViewFreeVolume[view] = THOT_MAXINT;
		}
	      else if (pDoc->DocAssocFrame[(*pFirstSel)->ElAssocNum - 1] != 0)
		 pDoc->DocAssocFreeVolume[(*pFirstSel)->ElAssocNum - 1] = THOT_MAXINT;
	   /* cree les paves de la deuxieme partie */
	   CreateNewAbsBoxes (*pFirstSel, pDoc, 0);
	   ApplDelayedRule (*pFirstSel, pDoc);
	}
   done = FALSE;
   if (createEmpty)
     if (*firstChar == 1 && *lastChar == 1 && *pFirstSel == *pLastSel)
       if ((*pLastSel)->ElTerminal && (*pLastSel)->ElLeafType == LtText)
	  {
	  pEl = NewSubtree ((*pFirstSel)->ElTypeNumber, (*pFirstSel)->ElStructSchema, pDoc, (*pFirstSel)->ElAssocNum, FALSE, TRUE, FALSE, TRUE);
	  InsertElementBefore (*pFirstSel, pEl);
	  for (view = 0; view < MAX_VIEW_DOC; view++)
	      if (!AssocView (*pFirstSel))
		{
		   if (pDoc->DocView[view].DvPSchemaView > 0)
		      /* la vue est ouverte */
		      pDoc->DocViewFreeVolume[view] = THOT_MAXINT;
		}
	      else if (pDoc->DocAssocFrame[(*pFirstSel)->ElAssocNum - 1] != 0)
		 pDoc->DocAssocFreeVolume[(*pFirstSel)->ElAssocNum - 1] = THOT_MAXINT;
	  CreateNewAbsBoxes (pEl, pDoc, 0);
	  ApplDelayedRule (pEl, pDoc);
	  *pFirstSel = pEl;
	  *pLastSel = pEl;
	  *firstChar = 0;
	  *lastChar = 0;
	  done = TRUE;
	  }
   if (!done)
     if (*lastChar > 0 && *pLastSel != NULL)
       if ((*pLastSel)->ElTerminal && (*pLastSel)->ElLeafType == LtText)
	 SplitAfterSelection (*pLastSel, *lastChar, pDoc);
}

/*----------------------------------------------------------------------
   applique a l'element pointe' par pEl du document pDoc les       
   regles de presentation correspondant a l'attribut decrit dans   
   le bloc pointe' par pAttr.                                      
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ApplyAttrPRulesToElem (PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr, boolean inherit)

#else  /* __STDC__ */
void                ApplyAttrPRulesToElem (pEl, pDoc, pAttr, inherit)
PtrElement          pEl;
PtrDocument         pDoc;
PtrAttribute        pAttr;
boolean             inherit;

#endif /* __STDC__ */

{
   PtrAttribute        pOldAttr;
   boolean             doIt;

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
	     UpdatePresAttr (pEl, pAttr, pDoc, FALSE, inherit, NULL);
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
void                ApplyAttrPRulesToSubtree (PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr)

#else  /* __STDC__ */
void                ApplyAttrPRulesToSubtree (pEl, pDoc, pAttr)
PtrElement          pEl;
PtrDocument         pDoc;
PtrAttribute        pAttr;

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
		 ApplyAttrPRulesToSubtree (pChild, pDoc, pAttr);

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
		     ApplyAttrPRulesToElem (pEl, pDoc, pAttr, TRUE);
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
		      ApplyAttrPRulesToElem (pEl, pDoc, pCompAttr, FALSE);
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
void                ChangeLanguage (PtrDocument pDoc, PtrElement pEl, Language lang, boolean force)

#else  /* __STDC__ */
void                ChangeLanguage (pDoc, pEl, lang, force)
PtrDocument         pDoc;
PtrElement          pEl;
Language            lang;
boolean             force;

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
   Retire la presentation associee a l'attribut pAttr dans les	
   images de l'element pEl.					
   Si pCompAttr != NULL, les regles de presentation dependant	
   de la comparaison doivent prendre pCompAttr comme attribut de   
   comparaison et ne pas en chercher d'autre dans les ascendants	
   de pEl.                                                         
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                RemoveAttrPresentation (PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr, boolean inherit, PtrAttribute pCompAttr)

#else  /* __STDC__ */
void                RemoveAttrPresentation (pEl, pDoc, pAttr, inherit, pCompAttr)
PtrElement          pEl;
PtrDocument         pDoc;
PtrAttribute        pAttr;
boolean             inherit;
PtrAttribute        pCompAttr;

#endif /* __STDC__ */

{
   /* on ne traite pas les marques page */
   if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
     {
	/* supprime la presentation attachee a la valeur de l'attribut, si */
	/* elle n'est pas nulle */
	UpdatePresAttr (pEl, pAttr, pDoc, TRUE, inherit, pCompAttr);

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
void                RemoveInheritedAttrPresent (PtrElement pEl, PtrDocument pDoc, PtrAttribute pAttr)

#else  /* __STDC__ */
void                RemoveInheritedAttrPresent (pEl, pDoc, pAttr)
PtrElement          pEl;
PtrDocument         pDoc;
PtrAttribute        pAttr;

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
		 RemoveInheritedAttrPresent (pChild, pDoc, pAttr);

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
		   RemoveAttrPresentation (pEl, pDoc, pAttr, TRUE, NULL);
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
		      RemoveAttrPresentation (pEl, pDoc, pCompAttr, FALSE, pAttr);
	     }
     }
}


/*----------------------------------------------------------------------
   Ajoute un attribut pNewAttr a l'element pEl                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
PtrAttribute        AddAttrToElem (PtrElement pEl, PtrAttribute pNewAttr)

#else  /* __STDC__ */
PtrAttribute        AddAttrToElem (pEl, pNewAttr)
PtrElement          pEl;
PtrAttribute        pNewAttr;

#endif /* __STDC__ */

{
   PtrAttribute        pAttr, pA;
   PtrReference        pRef;
   int                 len;

   GetAttribute (&pAttr);
   /* on chaine cet attribut apres le dernier attribut de  l'element */
   if (pEl->ElFirstAttr == NULL)
      /* c'est le 1er attribut de l'element */
      pEl->ElFirstAttr = pAttr;
   else
     {
	/* cherche le dernier attr */
	pA = pEl->ElFirstAttr;
	while (pA->AeNext != NULL)
	   pA = pA->AeNext;
	/* chaine le nouvel attribut */
	pA->AeNext = pAttr;
     }
   pAttr->AeNext = NULL;	/* c'est le dernier attribut */
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
	       pRef->RdNext = pNewAttr->AeAttrReference->RdNext;
	       if (pRef->RdNext != NULL)
		  pRef->RdNext->RdPrevious = pRef;
	       pRef->RdPrevious = pNewAttr->AeAttrReference;
	       if (pRef->RdPrevious != NULL)
		  pRef->RdPrevious->RdNext = pRef;
	       break;

	    default:
	       break;
	 }
   return (pAttr);
}

#ifndef _WIN_PRINT
/*----------------------------------------------------------------------
   Met dans l'element pEl la valeur de l'attribut pNewAttr		
   Les regles de presentation de cette nouvelle valeur sont	
   appliquees a l'element.						
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
   PtrAttribute        pAttr, pAttrAsc;
   PtrElement          pChild, pElAttr;
   boolean             suppress, compare, inherit, mandatory, create;
   NotifyAttribute     notifyAttr;

   mandatory = FALSE;

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
	inherit = (pNewAttr->AeAttrSSchema->SsPSchema->
		   PsNHeirElems[pNewAttr->AeAttrNum - 1] != 0);
	compare = (pNewAttr->AeAttrSSchema->SsPSchema->
		   PsNComparAttrs[pNewAttr->AeAttrNum - 1] != 0);
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
	     /* detache l'attribut de l'element s'il y a lieu */
	     RemoveAttribute (pEl, pAttr);
	     /* heritage et comparaison sont lies a un attribut de pEl */
	     /* On supprime d'abord les regles de presentation liees a
	        l'attribut sur l'element lui-meme */
	     RemoveAttrPresentation (pEl, pDoc, pAttr, FALSE, NULL);
	     /* supprime l'attribut */
	     DeleteAttribute (pEl, pAttr);
	     /* indique que le document a ete modifie' */
	     pDoc->DocModified = TRUE;
	     /* un changement d'attribut vaut dix caracteres saisis */
	     pDoc->DocNTypedChars += 10;
	     /* On supprime de pEl de son sous-arbre  la presentation venant
	        de l'heritage de cet attribut par le sous-arbre, s'il existe
	        des elements heritants de celui-ci */
	     if (inherit)
		RemoveInheritedAttrPresent (pEl, pDoc, pAttr);
	     /* On supprime des elements du sous arbre pEl la presentation
	        venant de la comparaison d'un attribut du sous-arbre avec ce
	        type d'attribut */
	     if (!pEl->ElTerminal && compare)
		for (pChild = pEl->ElFirstChild; pChild != NULL; pChild = pChild->ElNext)
		   RemoveComparAttrPresent (pChild, pDoc, pAttr);
	  }

	else if (pAttrAsc)
	  {
	     /* heritage et comparaison sont dus a un attribut d'un ascendant
	        de pEl */
	     /* on supprime du sous arbre pEl la presentation venant de
	        l'heritage de cet attribut par le sous-arbre, s'il existe des
	        elements heritants de celui-ci */
	     if (inherit)
		RemoveInheritedAttrPresent (pEl, pDoc, pAttrAsc);
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
	     pAttr = AddAttrToElem (pEl, pNewAttr);
	     /* indique que le document a ete modifie' */
	     pDoc->DocModified = TRUE;
	     /* un changement d'attribut vaut dix caracteres saisis */
	     pDoc->DocNTypedChars += 10;
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
	     ApplyAttrPRulesToElem (pEl, pDoc, pAttr, FALSE);
	     /* applique les regles de presentation venant de l'heritage de
	        cet attribut par le sous-arbre s'il existe des elements qui
	        heritent */
	     if (inherit)
		ApplyAttrPRulesToSubtree (pEl, pDoc, pAttr);
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
	     ApplyAttrPRulesToSubtree (pEl, pDoc, pAttrAsc);
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
#endif /* WIN_PRINT */

/*----------------------------------------------------------------------
   AttachAttrToRange applique l'attribut pAttr a une partie de document
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                AttachAttrToRange (PtrAttribute pAttr, int lastChar, int firstChar, PtrElement pLastSel, PtrElement pFirstSel, PtrDocument pDoc, boolean reDisplay)
#else  /* __STDC__ */
void                AttachAttrToRange (pAttr, lastChar, firstChar, pLastSel, pFirstSel, pDoc, reDisplay)
PtrAttribute        pAttr;
int                 lastChar;
int                 firstChar;
PtrElement          pLastSel;
PtrElement          pFirstSel;
PtrDocument         pDoc;
boolean		    reDisplay;
#endif /* __STDC__ */
{
   PtrElement          pEl;
   Language            lang;
   PtrAttribute        pAttrAsc;
   PtrElement          pElAttr;
   int                 i;

   /* eteint d'abord la selection */
   TtaClearViewSelections ();
   /* Coupe les elements du debut et de la fin de la selection s'ils */
   /* sont partiellement selectionnes */
   IsolateSelection (pDoc, &pFirstSel, &pLastSel, &firstChar, &lastChar, TRUE);
   /* parcourt les elements selectionnes */
   pEl = pFirstSel;
   while (pEl != NULL)
     {
	/* On ne traite pas les marques de page */
	if (!pEl->ElTerminal || pEl->ElLeafType != LtPageColBreak)
	  {
	     if (pAttr->AeAttrNum == 1)
		/* c'est l'attribut langue */
	       {
		  /* change la langue de toutes les feuilles de texte du sous-arbre */
		  /* de l'element */
		  if (pAttr->AeAttrText != NULL)
		     lang = TtaGetLanguageIdFromName (pAttr->AeAttrText->BuContent);
		  else
		     /* c'est une suppression de l'attribut Langue */
		    {
		       lang = TtaGetDefaultLanguage ();		/* langue par defaut */
		       /* on cherche si un ascendant porte l'attribut Langue */
		       if (pEl->ElParent != NULL)
			 pAttrAsc = GetTypedAttrAncestor (pEl->ElParent, 1, NULL, &pElAttr);
		       else
			 pAttrAsc = GetTypedAttrAncestor (pEl->ElParent, 1, NULL, &pElAttr);

		       if (pAttrAsc != NULL)
			  /* un ascendant definit la langue, on prend cette langue */
			  if (pAttrAsc->AeAttrText != NULL)
			     lang = TtaGetLanguageIdFromName (pAttrAsc->AeAttrText->BuContent);
		    }
		  ChangeLanguage (pDoc, pEl, lang, FALSE);
	       }

	     /* met la nouvelle valeur de l'attribut dans l'element et */
	     /* applique les regles de presentation de l'attribut a l'element */
	     AttachAttrWithValue (pEl, pDoc, pAttr);
	     if (ThotLocalActions[T_attrtable] != NULL)
		(*ThotLocalActions[T_attrtable])
		   (pEl, pAttr, pDoc);	/* cas particulier des tableaux */

	  }
	/* cherche l'element a traiter ensuite */
	pEl = NextInSelection (pEl, pLastSel);
     }
   /* si c'est un changement de langue qui s'applique a la racine de */
   /* l'arbre principal du document, on change aussi la langue de */
   /* tous les autres arbre de ce document */
   if (pAttr->AeAttrNum == 1)	/* attribut Langue = 1 */
      if (pFirstSel == pDoc->DocRootElement)
	{
	   for (i = 1; i <= MAX_PARAM_DOC; i++)
	      if (pDoc->DocParameters[i - 1] != NULL)
		 AttachAttrToRange (pAttr, 0, 0, pDoc->DocParameters[i - 1],
				    pDoc->DocParameters[i - 1], pDoc, FALSE);
	   for (i = 1; i <= MAX_ASSOC_DOC; i++)
	      if (pDoc->DocAssocRoot[i - 1] != NULL)
		 AttachAttrToRange (pAttr, 0, 0, pDoc->DocAssocRoot[i - 1],
				    pDoc->DocAssocRoot[i - 1], pDoc, FALSE);
	}
   /* parcourt a nouveau les elements selectionnes pour fusionner les */
   /* elements voisins de meme type ayant les memes attributs, reaffiche */
   /* toutes les vues et retablit la selection */
   if (reDisplay)
     MergeAndSelect (pDoc, pFirstSel, pLastSel, firstChar, lastChar);
}


/*----------------------------------------------------------------------
   AttachMandatoryAttrSRule verifie que l'element pEl possede les  
   attributs requis indique's dans la regle pSRule du schema de    
   structure pSS et, si certains attributs requis manquent, force  
   l'utilisateur a leur donner une valeur et met ces attributs sur 
   l'element pEl.                                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AttachMandatoryAttrSRule (PtrElement pEl, PtrDocument pDoc, SRule * pSRule, PtrSSchema pSS)
#else  /* __STDC__ */
static void         AttachMandatoryAttrSRule (pEl, pDoc, pSRule, pSS)
PtrElement          pEl;
PtrDocument         pDoc;
SRule              *pSRule;
PtrSSchema          pSS;

#endif /* __STDC__ */
{
   PtrElement          pRefEl;
   PtrAttribute        pAttr, pA;
   int                 i, att;
   PtrReference        pRef;
   boolean             found;
   boolean             MandatoryAttrOK;
   NotifyAttribute     notifyAttr;
   int                 len;

   /* parcourt tous les attributs locaux definis dans la regle */
   for (i = 0; i < pSRule->SrNLocalAttrs; i++)
      if (pSRule->SrRequiredAttr[i])
	 /* cet attribut local est obligatoire */
	 if (pDoc->DocSSchema != NULL)
	    /* le document n'a pas ete ferme' entre-temps */
	   {
	      att = pSRule->SrLocalAttr[i];
	      /* cherche si l'element possede cet attribut */
	      pAttr = pEl->ElFirstAttr;
	      found = FALSE;
	      while (pAttr != NULL && !found)
		 if (pAttr->AeAttrNum == att &&
		  (att == 1 || pAttr->AeAttrSSchema->SsCode == pSS->SsCode))
		    /* att = 1: Langue, quel que soit le schema de structure */
		    found = TRUE;
		 else
		    pAttr = pAttr->AeNext;
	      if (!found)
		 /* l'element ne possede pas cet attribut requis */
		{
		   /* envoie l'evenement AttrCreate.Pre */
		   notifyAttr.event = TteAttrCreate;
		   notifyAttr.document = (Document) IdentDocument (pDoc);
		   notifyAttr.element = (Element) pEl;
		   notifyAttr.attribute = NULL;
		   notifyAttr.attributeType.AttrSSchema = (SSchema) pSS;
		   notifyAttr.attributeType.AttrTypeNum = att;
		   CallEventAttribute (&notifyAttr, TRUE);
		   /* cree un nouvel attribut pour l'element */
		   GetAttribute (&pAttr);
		   pAttr->AeAttrSSchema = pSS;
		   pAttr->AeAttrNum = att;
		   pAttr->AeDefAttr = FALSE;
		   pAttr->AeAttrType = pSS->SsAttribute[att - 1].AttrType;
		   switch (pAttr->AeAttrType)
			 {
			    case AtNumAttr:
			    case AtEnumAttr:
			       pAttr->AeAttrValue = 0;
			       break;
			    case AtReferenceAttr:
			       /* attache un bloc reference a l'attribut */
			       GetReference (&pRef);
			       pAttr->AeAttrReference = pRef;
			       pRef->RdElement = pEl;
			       pRef->RdAttribute = pAttr;
			       break;
			    case AtTextAttr:
			       pAttr->AeAttrText = NULL;
			       break;
			    default:
			       break;
			 }
		   /* attache l'attribut a l'element */
		   if (pEl->ElFirstAttr == NULL)
		      /* c'est le 1er attribut de l'element */
		      pEl->ElFirstAttr = pAttr;
		   else
		     {
			pA = pEl->ElFirstAttr;	/* 1er attribut de l'element */
			while (pA->AeNext != NULL)
			   /* cherche le dernier attribut de l'element */
			   pA = pA->AeNext;
			/* chaine le nouvel attribut */
			pA->AeNext = pAttr;
		     }
		   /* c'est le dernier attribut de l'element */
		   pAttr->AeNext = NULL;
		   /* envoie l'evenement AttrModify.Pre */
		   notifyAttr.event = TteAttrModify;
		   notifyAttr.document = (Document) IdentDocument (pDoc);
		   notifyAttr.element = (Element) pEl;
		   notifyAttr.attribute = (Attribute) pAttr;
		   notifyAttr.attributeType.AttrSSchema = (SSchema) pSS;
		   notifyAttr.attributeType.AttrTypeNum = att;
		   if (!CallEventAttribute (&notifyAttr, TRUE))
		      /* l'application laisse l'editeur saisir la valeur de */
		      /* l'attribut requis */
		     {
			MandatoryAttrOK = FALSE;
			do
			  {
			     /* demande a l'utilisateur d'entrer une valeur */
			     /* pour l'attribut */
			     if (pAttr->AeAttrType == AtReferenceAttr)
				/* demande a l'utilisateur l'element reference' */
				MandatoryAttrOK = LinkReference (pEl, pAttr, pDoc, &pRefEl);
			     else
			       {
				  if (ThotLocalActions[T_attrreq] != NULL)
				     (*ThotLocalActions[T_attrreq]) (pAttr, pDoc);
				  else
				     switch (pAttr->AeAttrType)
					   {
					      case AtNumAttr:
						 /* attribut a valeur numerique */
						 pAttr->AeAttrValue = 0;
						 break;

					      case AtTextAttr:
						 /* attribut a valeur textuelle */
						 CopyStringToText (" ", pAttr->AeAttrText, &len);
						 break;

					      case AtEnumAttr:
						 /* attribut a valeurs enumerees */
						 pAttr->AeAttrValue = 1;
						 break;

					      default:
						 break;
					   }
				  MandatoryAttrOK = TRUE;
			       }
			  }
			while (!MandatoryAttrOK && pDoc->DocSSchema != NULL);

			if (MandatoryAttrOK && pDoc->DocSSchema != NULL)
			  {
			     /* envoie l'evenement AttrModify.Post */
			     notifyAttr.event = TteAttrModify;
			     notifyAttr.document = (Document) IdentDocument (pDoc);
			     notifyAttr.element = (Element) pEl;
			     notifyAttr.attribute = (Attribute) pAttr;
			     notifyAttr.attributeType.AttrSSchema = (SSchema) pSS;
			     notifyAttr.attributeType.AttrTypeNum = att;
			     CallEventAttribute (&notifyAttr, FALSE);
			  }
		     }
		   /* envoie l'evenement AttrCreate.Post */
		   notifyAttr.event = TteAttrCreate;
		   notifyAttr.document = (Document) IdentDocument (pDoc);
		   notifyAttr.element = (Element) pEl;
		   notifyAttr.attribute = (Attribute) pAttr;
		   notifyAttr.attributeType.AttrSSchema = (SSchema) pSS;
		   notifyAttr.attributeType.AttrTypeNum = att;
		   CallEventAttribute (&notifyAttr, FALSE);
		}
	   }
}


/*----------------------------------------------------------------------
   Verifie que tous les elements du sous-arbre de racine pEl       
   possedent les attributs requis et, si certains attributs requis 
   manquent, force l'utilisateur a leur donner une valeur et met   
   ces attributs sur les elements qui les requierent.              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                AttachMandatoryAttributes (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                AttachMandatoryAttributes (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pChild;
   SRule              *pSRule;
   PtrSSchema          pSS;

   if (pEl != NULL && pEl->ElStructSchema != NULL)
     {
	/* traite d'abord les attributs requis par la regle de structure qui */
	/* definit l'element */
	pSS = pEl->ElStructSchema;
	pSRule = &pSS->SsRule[pEl->ElTypeNumber - 1];
	AttachMandatoryAttrSRule (pEl, pDoc, pSRule, pSS);
	/* traite toutes les regles d'extension de ce type d'element */
	pSS = pDoc->DocSSchema;
	if (pSS != NULL)
	  {
	     pSS = pSS->SsNextExtens;
	     /* parcourt tous les schemas d'extension du document */
	     while (pSS != NULL)
	       {
		  /* cherche dans ce schema d'extension la regle qui concerne */
		  /* le type de l'element */
		  pSRule = ExtensionRule (pEl->ElStructSchema, pEl->ElTypeNumber, pSS);
		  if (pSRule != NULL)
		     /* il y a une regle d'extension, on la traite */
		     AttachMandatoryAttrSRule (pEl, pDoc, pSRule, pSS);
		  if (pDoc->DocSSchema == NULL)
		     /* le document a ete ferme' entre-temps */
		     pSS = NULL;
		  else
		     /* passe au schema d'extension suivant */
		     pSS = pSS->SsNextExtens;
	       }
	  }
	/* applique le meme traitement a tous les descendants de pEl */
	if (pDoc->DocSSchema != NULL)
	   /* le document n'a pas ete ferme' entre-temps */
	   if (!pEl->ElTerminal)
	     {
		pChild = pEl->ElFirstChild;
		while (pChild != NULL)
		  {
		     AttachMandatoryAttributes (pChild, pDoc);
		     if (pDoc->DocSSchema == NULL)
			/* le document n'existe plus */
			pChild = NULL;
		     else
			pChild = pChild->ElNext;
		  }
	     }
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
   boolean             found;
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


/*----------------------------------------------------------------------
   Modifie la valeur d'un attribut a valeurs enumerees porte par	
   l'element pEl dans le document pDoc. L'attribut porte		
   	l'exception de numero ExceptNum. La nouvelle valeur est attrVal	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                SetAttrValueByExceptNum (PtrElement pEl, PtrDocument pDoc, int attrVal, int ExceptNum)

#else  /* __STDC__ */
void                SetAttrValueByExceptNum (pEl, pDoc, attrVal, ExceptNum)
PtrElement          pEl;
PtrDocument         pDoc;
int                 attrVal;
int                 ExceptNum;

#endif /* __STDC__ */

{
   PtrAttribute        pAttr;

   GetAttribute (&pAttr);
   if (pAttr != NULL)
     {
	pAttr->AeAttrSSchema = pEl->ElStructSchema;
	pAttr->AeAttrNum = GetAttrWithException (ExceptNum, pEl->ElStructSchema);
	pAttr->AeDefAttr = FALSE;
	pAttr->AeAttrType = AtEnumAttr;
	pAttr->AeAttrValue = attrVal;
	AttachAttrWithValue (pEl, pDoc, pAttr);
	DeleteAttribute (NULL, pAttr);
     }
}
