/*
 *
 *  (c) COPYRIGHT INRIA 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles the ChangeType, Surround, Copy, Cut, and Paste commands
 *
 * Authors: V. Quint (INRIA)
 *          S. Bonhomme (INRIA) - Separation between structured and
 *                                unstructured editing modes
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "constmenu.h"
#include "typemedia.h"
#include "modif.h"
#include "language.h"
#include "libmsg.h"
#include "message.h"
#include "fileaccess.h"
#include "appaction.h"
#include "appdialogue.h"
#include "dialog.h"
#include "tree.h"
#include "content.h"
#include "registry.h"
#include "application.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "constres.h"
#include "creation_tv.h"
#include "modif_tv.h"
#include "select_tv.h"
#include "edit_tv.h"
#include "appdialogue_tv.h"

/* isomorphism description */
typedef struct _IsomorphDesc *PtrIsomorphDesc;
typedef struct _IsomorphDesc
  {
     int                 IDtypeNum;
     PtrSSchema          IDStructSch;
     int                 IDtypeNumIso;
     PtrSSchema          IDStructSchIso;
     PtrIsomorphDesc     IDNext;
  }
IsomorphDesc;

typedef struct _ChoiceOptionDescr *PtrChoiceOptionDescr;
typedef struct _ChoiceOptionDescr
  {
     int                 COtypeNum;
     PtrSSchema          COStructSch;
     PtrChoiceOptionDescr CONext;
  }
ChoiceOptionDescr;

static PtrIsomorphDesc firstIsomorphDesc = NULL;

#define MAX_ITEMS_CHANGE_TYPE	25

/* types of elements proposed in the "Change Type" menu */
static int          NChangeTypeItems;
static int          ChangeTypeTypeNum[MAX_ITEMS_CHANGE_TYPE];
static PtrSSchema   ChangeTypeSSchema[MAX_ITEMS_CHANGE_TYPE];
static int          ChangeTypeMethod[MAX_ITEMS_CHANGE_TYPE];

/* transformation callback procedure */
static Func         TransformIntoFunction = NULL;

#define M_EQUIV 1
#define M_RESDYN 2

#include "absboxes_f.h"
#include "appdialogue_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attributeapi_f.h"
#include "attributes_f.h"
#include "attrpresent_f.h"
#include "boxselection_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "content_f.h"
#include "contentapi_f.h"
#include "createabsbox_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "docs_f.h"
#include "editcommands_f.h"
#include "exceptions_f.h"
#include "externalref_f.h"
#include "fileaccess_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "references_f.h"
#include "res_f.h"
#include "search_f.h"
#include "selectmenu_f.h"
#include "structcreation_f.h"
#include "structmodif_f.h"
#include "structschema_f.h"
#include "structselect_f.h"
#include "textcommands_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "views_f.h"

/* element types proposed in menu Surround */
static int          typeNumSurround[MAX_MENU];	/* type */
static PtrSSchema   pSSSurround[MAX_MENU];	/* schema */
static int          NElSurround;	/* number of entries in the table */



/*----------------------------------------------------------------------
   IsolateSelection
   Si la selection passee en parametre commence ou finit sur des   
   elements partiellement selectionnes, ces elements sont coupes   
   en deux et leurs paves egalement.                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                IsolateSelection (PtrDocument pDoc, PtrElement * pFirstSel, PtrElement * pLastSel, int *firstChar, int *lastChar, ThotBool createEmpty)
#else  /* __STDC__ */
void                IsolateSelection (pDoc, pFirstSel, pLastSel, firstChar, lastChar, createEmpty)
PtrDocument         pDoc;
PtrElement         *pFirstSel;
PtrElement         *pLastSel;
int                *firstChar;
int                *lastChar;
ThotBool		    createEmpty;
#endif /* __STDC__ */

{
   PtrElement	       pEl;
   int                 view;
   ThotBool	       done;

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
   IsolatedPairedElem   verifie si l'element pEl est un element de 
   paire dont l'homologue ne serait pas dans la partie             
   selectionnee determinee par firstSel et lastSel.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsolatedPairedElem (PtrElement pEl, PtrElement firstSel, PtrElement lastSel)
#else  /* __STDC__ */
static ThotBool     IsolatedPairedElem (pEl, firstSel, lastSel)
PtrElement          pEl;
PtrElement          firstSel;
PtrElement          lastSel;

#endif /* __STDC__ */
{
   PtrElement          pEl2, pAncest, pSel;
   ThotBool            alone;

   alone = FALSE;
   if (pEl->ElTerminal && pEl->ElLeafType == LtPairedElem)
      /* l'element est un element de paire */
     {
	/* le 2eme element de la paire */
	pEl2 = GetOtherPairedElement (pEl);
	/* Si le 2eme element de la paire n'existe pas, on accepte de */
	/* detruire cet element bien qu'il soit seul */
	if (pEl2 != NULL)
	   /* le 2eme element de la paire est-il dans la selection ? */
	  {
	     /* a priori, non */
	     alone = TRUE;
	     /* parcourt la suite des elements selectionne's */
	     pSel = firstSel;
	     while (pSel != NULL && alone)
	       {
		  /* verifie si cet element selectionne' est le 2eme element */
		  /* de la paire ou un de ses ascendants */
		  pAncest = pEl2;
		  /* remonte les ascendants du 2eme element de la paire */
		  while (pAncest != NULL && alone)
		     if (pAncest == pSel)
			alone = FALSE;	/* trouve. Il n'est donc pas seul */
		     else
			pAncest = pAncest->ElParent;
		  /* si on n'a pas encore trouve', on teste l'element */
		  /* selectionne' suivant */
		  if (alone)
		     pSel = NextInSelection (pSel, lastSel);
	       }
	  }
     }
   return alone;
}


/*----------------------------------------------------------------------
   IsomorphicTypes indique si les types (pSS1, typeNum1) et	
   	(pSS2, typeNum2) sont isomorphes on non.                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     IsomorphicTypes (PtrSSchema pSS1, int typeNum1,
				     PtrSSchema pSS2, int typeNum2)
#else
static ThotBool     IsomorphicTypes (pSS1, typeNum1, pSS2, typeNum2)
PtrSSchema          pSS1;
int                 typeNum1;
PtrSSchema          pSS2;
int                 typeNum2;

#endif /* __STDC__ */
{
   SRule              *pSRule1, *pSRule2;
   PtrIsomorphDesc     pIsoD;
   ThotBool            ret;

   ret = FALSE;
   if (pSS1->SsCode == pSS2->SsCode && typeNum1 == typeNum2)
      /* meme regle */
      ret = TRUE;
   else
     {
	pSRule1 = &pSS1->SsRule[typeNum1 - 1];
	pSRule2 = &pSS2->SsRule[typeNum2 - 1];
	if (pSRule1->SrConstruct == pSRule2->SrConstruct)
	   switch (pSRule1->SrConstruct)
		 {
		    case CsNatureSchema:
		       ret = (ustrcmp (pSRule1->SrName, pSRule2->SrName) == 0);
		       break;
		    case CsBasicElement:
		       ret = (pSRule1->SrBasicType == pSRule2->SrBasicType);
		       break;
		    case CsReference:
		       ret = (pSRule1->SrReferredType == pSRule2->SrReferredType &&
			      ustrcmp (pSRule1->SrRefTypeNat, pSRule2->SrRefTypeNat) == 0);
		       break;
		    case CsIdentity:
		       ret = (pSRule1->SrIdentRule == pSRule2->SrIdentRule);
		       break;
		    case CsList:
		       ret = IsomorphicTypes (pSS1, pSRule1->SrListItem, pSS2, pSRule2->SrListItem);
		       break;
		    case CsConstant:
		       ret = (pSRule1->SrIndexConst == pSRule2->SrIndexConst);
		       break;
		    default:	/* CsChoice, CsAggregate, CsUnorderedAggregate */
		       ret = FALSE;
		       break;
		 }
	/* on a trouve' un isomorphisme, on le note */
	if (ret)
	  {
	     pIsoD = (PtrIsomorphDesc) TtaGetMemory (sizeof (IsomorphDesc));
	     if (pIsoD != NULL)
	       {
		  pIsoD->IDtypeNum = typeNum1;
		  pIsoD->IDStructSch = pSS1;
		  pIsoD->IDtypeNumIso = typeNum2;
		  pIsoD->IDStructSchIso = pSS2;
		  pIsoD->IDNext = firstIsomorphDesc;
		  firstIsomorphDesc = pIsoD;
	       }
	  }
     }
   return ret;
}


/*----------------------------------------------------------------------
   RegisterIfIsomorphic    Si le type (pSS, typeNum) est isomorphe au 
   type de l'element pEl, mais different, on le met dans   
   la table qui va servir a construire le menu Change Type 
   Si ce type est un choix, on fait de meme avec toutes    
   ses options.                                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RegisterIfIsomorphic (PtrElement pEl, PtrSSchema pSS,
					  int typeNum)
#else
static void         RegisterIfIsomorphic (pEl, pSS, typeNum)
PtrElement          pEl;
PtrSSchema          pSS;
int                 typeNum;

#endif /* __STDC__ */
{
   SRule              *pSRule;
   int                 choice;
   ThotBool            found;
   int                 i;
   STRING	       strResDyn;

   /* on ne propose pas le type qu'a deja l'element */
   if (pEl->ElTypeNumber != typeNum ||
       pEl->ElStructSchema->SsCode != pSS->SsCode)
      /* on ne fait rien si la table est pleine */
      if (NChangeTypeItems < MAX_ITEMS_CHANGE_TYPE - 1)
	{
	   /* on cherche si ce type est deja dans la table */
	   found = FALSE;
	   for (i = 0; i < NChangeTypeItems && !found; i++)
	      if (typeNum == ChangeTypeTypeNum[i])
		 if (pSS->SsCode == ChangeTypeSSchema[i]->SsCode)
		    found = TRUE;
	   if (!found)
	     {
	       strResDyn = NULL;
	       /* ce type n'est pas deja dans la table */
	       if (IsomorphicTypes (pEl->ElStructSchema, pEl->ElTypeNumber,
				    pSS, typeNum))
		 /* ce type est isomorphe au type de l'element */
		 /* on le met dans la table */
		 {
		   ChangeTypeTypeNum[NChangeTypeItems] = typeNum;
		   ChangeTypeSSchema[NChangeTypeItems] = pSS;
		   ChangeTypeMethod[NChangeTypeItems] = M_EQUIV;
		   NChangeTypeItems++;
		 }
	       /* existe-t-il une relation facteur ou massif */
	       else if (strResDyn != NULL &&
			RestMatchElements ((Element)pEl, (Element)pEl, 
					   (SSchema)pSS, typeNum))
		 {
		   ChangeTypeTypeNum[NChangeTypeItems] = typeNum;
		   ChangeTypeSSchema[NChangeTypeItems] = pSS;
		   ChangeTypeMethod[NChangeTypeItems] = M_RESDYN;
		   NChangeTypeItems++;
		 }
	       else
		 /* ce type n'est pas isomorphe, mais c'est peut-etre un
		    Choix */
		 {
		   pSRule = &pSS->SsRule[typeNum - 1];
		   if (pSRule->SrConstruct == CsChoice)
		      /* c'est un CsChoix. On essaie de mettre chacune de */
		      /* ses options dans la table */
		      for (choice = 0; choice < pSRule->SrNChoices &&
		      NChangeTypeItems < MAX_ITEMS_CHANGE_TYPE - 1; choice++)
			 RegisterIfIsomorphic (pEl, pSS, pSRule->SrChoice[choice]);
		 }
	     }
	}
}


/*----------------------------------------------------------------------
   IsomorphicTransform transforme le sous-arbre dont la      	
   racine est pointee par pEl en un sous-arbre             
   isomorphe, dont la racine prend le type (pSS, typeNum). 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         IsomorphicTransform (PtrElement pEl, PtrSSchema pSS,
					 int typeNum, PtrDocument pDoc)
#else
static void         IsomorphicTransform (pEl, pSS, typeNum, pDoc)
PtrElement          pEl;
PtrSSchema          pSS;
int                 typeNum;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrElement          pChild;
   PtrSSchema          pNewSS;
   PtrAttribute        pAttr, pNextAttr, pAttrDouble, pNextDoubleAttr;
   PtrIsomorphDesc     pIsoD;
   SRule              *pSRule;
   int                 att, newType;
   ThotBool            found;

   if (pEl->ElStructSchema->SsCode == pSS->SsCode &&
       pEl->ElTypeNumber == typeNum)
      /* l'element a deja le type voulu, il n'y a rien a faire */
      return;
   pSRule = &pSS->SsRule[typeNum - 1];
   /* retire les attributs locaux propres a l'ancien type et qui */
   /* ne s'appliquent pas au nouveau type */
   pAttr = pEl->ElFirstAttr;
   while (pAttr != NULL)	/* examine tous les attributs de l'element */
     {
	pNextAttr = pAttr->AeNext;
	if (!pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrGlobal)
	   /* c'est un attribut local, il faut verifier */
	  {
	     found = FALSE;
	     for (att = 0; att < pSRule->SrNLocalAttrs && !found; att++)
		if (pSRule->SrLocalAttr[att] == pAttr->AeAttrNum)
		   /* c'est bien un attribut local du type de l'element */
		   found = TRUE;
	     if (!found)
		/* l'attribut ne figure pas parmi les attributs locaux du */
		/* type de l'element, on le supprime */
		{
		RemoveAttribute (pEl, pAttr);
		DeleteAttribute (pEl, pAttr);
		}
	  }
	/* passe a l'attribut suivant de l'element */
	pAttr = pNextAttr;
     }
   /* change le type de l'element */
   pEl->ElTypeNumber = typeNum;
   pEl->ElStructSchema = pSS;
   /* met les attributs imposes du nouveau type */
   AttachRequiredAttributes (pEl, pSRule, pSS, TRUE, pDoc);
   /* verifie qu'il n'y a pas d'attribut en double */
   pAttr = pEl->ElFirstAttr;
   while (pAttr != NULL)
     {
	pNextAttr = pAttr->AeNext;
	pAttrDouble = pNextAttr;
	while (pAttrDouble != NULL)
	  {
	     pNextDoubleAttr = pAttrDouble->AeNext;
	     if (pAttrDouble->AeAttrNum == pAttr->AeAttrNum &&
		 pAttrDouble->AeAttrSSchema->SsCode == pAttr->AeAttrSSchema->SsCode)
	       {
		  if (pNextAttr == pAttrDouble)
		     pNextAttr = pNextDoubleAttr;
		  RemoveAttribute (pEl, pAttrDouble);
		  DeleteAttribute (pEl, pAttrDouble);
	       }
	     pAttrDouble = pNextDoubleAttr;
	  }
	pAttr = pNextAttr;
     }
   if (!pEl->ElTerminal)
      /* on transforme les fils de l'element */
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL)
	  {
	     pNewSS = pChild->ElStructSchema;
	     newType = pChild->ElTypeNumber;
	     pIsoD = firstIsomorphDesc;
	     while (pIsoD != NULL)
	       {
		  if (pIsoD->IDtypeNum == pChild->ElTypeNumber &&
		      pIsoD->IDStructSch->SsCode == pChild->ElStructSchema->SsCode)
		    {
		       newType = pIsoD->IDtypeNumIso;
		       pNewSS = pIsoD->IDStructSchIso;
		       pIsoD = NULL;
		    }
		  else
		     pIsoD = pIsoD->IDNext;
	       }
	     IsomorphicTransform (pChild, pNewSS, newType, pDoc);
	     pChild = pChild->ElNext;
	  }
	/* on cree les elements obligatoires qui n'existent pas */
	CompleteElement (pEl, pDoc);
     }
}


/*----------------------------------------------------------------------
   SendEventSubTree      envoie l'evenement AppEvent.Pre pour      
   l'element pEl du document pDoc et, si le document le demande,   
   pour tous les descendants de pEl.                               
   Retourne TRUE si l'application n'est pas d'accord pour          
   appliquer la commande a` l'element pEl ou a` un de ses          
   descendants.                                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            SendEventSubTree (APPevent AppEvent, PtrDocument pDoc, PtrElement pEl, int end)
#else  /* __STDC__ */
ThotBool            SendEventSubTree (AppEvent, pDoc, pEl, end)
APPevent            AppEvent;
PtrDocument         pDoc;
PtrElement          pEl;
int                 end;

#endif /* __STDC__ */
{
   NotifyElement       notifyEl;
   PtrElement          pChild;
   ThotBool            ret;
   ThotBool            cancel;

   /* envoie l'evenement appEvent.Pre a l'element pEl */
   notifyEl.event = AppEvent;
   notifyEl.document = (Document) IdentDocument (pDoc);
   notifyEl.element = (Element) pEl;
   notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
   notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
   if (AppEvent == TteElemDelete)
      notifyEl.position = end;
   else
      notifyEl.position = 0;
   notifyEl.info = 0;
   cancel = CallEventType ((NotifyEvent *) (&notifyEl), TRUE);
   if (pDoc->DocNotifyAll && !cancel)
      /* le document demande un evenement pour chaque element du sous-arbre */
      if (!pEl->ElTerminal)
	{
	   pChild = pEl->ElFirstChild;
	   /* envoie recursivement un evenement a chaque fils de pEl */
	   while (pChild != NULL)
	     {
		ret = SendEventSubTree (AppEvent, pDoc, pChild, 0);
		cancel = cancel | ret;
		pChild = pChild->ElNext;
	     }
	}
   return (cancel);
}


/*----------------------------------------------------------------------
   	DoChangeType							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     DoChangeType (PtrElement pEl, PtrDocument pDoc, int newTypeNum,
				  PtrSSchema newSSchema)
#else
static ThotBool     DoChangeType (pEl, pDoc, newTypeNum, newSSchema)
PtrElement          pEl;
PtrDocument         pDoc;
int                 newTypeNum;
PtrSSchema          newSSchema;

#endif /* __STDC__ */
{
   NotifyElement       notifyEl;
   PtrElement	       pDummyEl, nextEl;
   ThotBool            ret;

   ret = FALSE;
   /* envoie l'evenement TteElemChange.Pre a l'element pEl */
   notifyEl.event = TteElemChange;
   notifyEl.document = (Document) IdentDocument (pDoc);
   notifyEl.element = (Element) pEl;
   notifyEl.elementType.ElTypeNum = newTypeNum;
   notifyEl.elementType.ElSSchema = (SSchema) newSSchema;
   notifyEl.position = 0;
   if (!CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
     {
	/* garde l'ancien type de l'element */
	notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	/* annule d'abord la selection */
	TtaClearViewSelections ();
	/* detruit les paves de l'element qui va changer de type */
	DestroyAbsBoxes (pEl, pDoc, TRUE);
	AbstractImageUpdated (pDoc);

	/* create a dummy element for renumbering the following elements */
        GetElement (&pDummyEl);
	pDummyEl->ElTypeNumber = pEl->ElTypeNumber;
	pDummyEl->ElStructSchema = pEl->ElStructSchema;

	/* on transforme le type de l'element, en changeant les types */
	/* qui doivent l'etre dans ses descendants */
	IsomorphicTransform (pEl, newSSchema, newTypeNum, pDoc);

	/* The following elements must take into account the deletion of the
	   old type */
	nextEl = NextElement (pEl);
	UpdateNumbers (nextEl, pDummyEl, pDoc, TRUE);
	FreeElement (pDummyEl);

	/* transformation is done */
	ret = TRUE;
	RemoveExcludedElem (&pEl, pDoc);
	AttachMandatoryAttributes (pEl, pDoc);
	if (pDoc->DocSSchema != NULL)
	   /* le document n'a pas ete ferme' entre temps */
	  {
	     /* envoie a l'application l'evenement TteElemChange.Post */
	     notifyEl.event = TteElemChange;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) pEl;
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
	     /* cree les paves de l'element et reaffiche */
	     CreateAllAbsBoxesOfEl (pEl, pDoc);
	     AbstractImageUpdated (pDoc);
	     RedisplayDocViews (pDoc);
	     /* si on est dans un element copie' par inclusion, on met a jour
	        les copies de cet element. */
	     RedisplayCopies (pEl, pDoc, TRUE);
	     /* Renumbers the following element, taking into account the new
		type of the element */
	     UpdateNumbers (nextEl, pEl, pDoc, TRUE);
	     /* indique que le document est modifie' */
	     SetDocumentModified (pDoc, TRUE, 30);
	     SelectElementWithEvent (pDoc, pEl, TRUE, TRUE);
	  }
     }
   return ret;
}


/*----------------------------------------------------------------------
   ChangeReferences cherche dans le sous-arbre de racine pElem tous
   les elements reference's et transfert sur eux les references qui
   sont dans le tampon Copier-Couper-Coller                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ChangeReferences (PtrElement pElem, PtrDocument * docsel)
#else  /* __STDC__ */
static void         ChangeReferences (pElem, docsel)
PtrElement          pElem;
PtrDocument        *docsel;

#endif /* __STDC__ */

{
   PtrElement          pChild;
   PtrReferredDescr    pRefD;
   PtrReference        pRef1, pRef, pNextRef;

   if (pElem->ElReferredDescr != NULL)
      /* cet element est reference'. CopyTree n'a pas copie' son */
      /* descripteur d'element reference', qui est encore partage' avec */
      /* celui de l'element source */
      /* traite les references a l'element source qui sont dans le */
      /* buffer de sauvegarde */
     {
	pRefD = pElem->ElReferredDescr;
	/* descripteur d'element reference' */
	pElem->ElReferredDescr = NULL;
	/* reste attache' a l'element source */
	pRef = pRefD->ReFirstReference;
	/* 1ere reference a l'element source */
	while (pRef != NULL)
	   /* parcourt les references a l'element source */
	  {
	     pNextRef = pRef->RdNext;
	     /* prepare la reference suivante */
	     if (IsASavedElement (pRef->RdElement))
		/* la reference est dans le tampon, on la fait pointer vers */
		/* l'element traite' (pElem) */
	       {
		  if (pElem->ElReferredDescr == NULL)
		     /* l'element n'a pas de descripteur d'element */
		     /* reference',  on lui en affecte un */
		    {
		       pElem->ElReferredDescr = NewReferredElDescr (*docsel);
		       if (!pElem->ElReferredDescr->ReExternalRef)
			  pElem->ElReferredDescr->ReReferredElem = pElem;
		    }
		  /* lie le descripteur de reference et le descripteur */
		  /* d'element reference' de l'element traite' */
		  pRef1 = pRef;
		  /* dechaine le descripteur de la chaine des references a */
		  /* l'element source */
		  if (pRef1->RdNext != NULL)
		     pRef1->RdNext->RdPrevious = pRef1->RdPrevious;
		  if (pRef1->RdPrevious == NULL)
		     pRefD->ReFirstReference = pRef1->RdNext;
		  else
		     pRef1->RdPrevious->RdNext = pRef1->RdNext;
		  /* le chaine en tete de la liste des references a */
		  /* l'element traite' */
		  pRef1->RdReferred = pElem->ElReferredDescr;
		  pRef1->RdNext = pRef1->RdReferred->ReFirstReference;
		  if (pRef1->RdNext != NULL)
		     pRef1->RdNext->RdPrevious = pRef;
		  pRef1->RdReferred->ReFirstReference = pRef;
		  pRef1->RdPrevious = NULL;
	       }
	     pRef = pNextRef;
	     /* passe a la reference suivante */
	  }
     }
   if (!pElem->ElTerminal)
      /* element non terminal, on traite tous ses fils */
     {
	pChild = pElem->ElFirstChild;
	while (pChild != NULL)
	  {
	     ChangeReferences (pChild, docsel);
	     pChild = pChild->ElNext;
	  }
     }
}

/*----------------------------------------------------------------------
   FreeSavedElements libere la suite des elements sauve's par les	
   	commandes Copy et Cut.						
  ----------------------------------------------------------------------*/
void                FreeSavedElements ()
{
   PtrPasteElem        pPasteEl, pNextPasteEl;

   pPasteEl = FirstSavedElement;
   while (pPasteEl != NULL)
     {
	pNextPasteEl = pPasteEl->PeNext;
	if (pPasteEl->PeElement != NULL)
	   DeleteElement (&pPasteEl->PeElement, DocOfSavedElements);
	TtaFreeMemory ( pPasteEl);
	pPasteEl = pNextPasteEl;
     }
   FirstSavedElement = NULL;
   DocOfSavedElements = NULL;
   /* disable Paste command */
   if (ClipboardThot.BuLength != 0)
     /* switch the Paste entry in all documents */
     SwitchPaste (NULL, FALSE);
}

/*----------------------------------------------------------------------
   SaveElement     met l'element pointe' par pEl a la fin de la    
   liste des elements a copier.                    
   pParent est le pere de l'element original	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SaveElement (PtrElement pEl, PtrElement pParent)
#else
static void         SaveElement (pEl, pParent)
PtrElement          pEl;
PtrElement          pParent;

#endif /* __STDC__ */

{
   PtrPasteElem        pPasteEl, pNewPasteEl;
   PtrElement          pAncest;
   int                 level, i;

   pNewPasteEl = (PtrPasteElem) TtaGetMemory (sizeof (PasteElemDescr));
   if (pNewPasteEl != NULL)
     {
	if (FirstSavedElement == NULL)
	  {
	    /* enable the Paste command */
	    if (ClipboardThot.BuLength == 0)
	      /* switch the Paste entry in all documents */
	      SwitchPaste (NULL, TRUE);

	     FirstSavedElement = pNewPasteEl;
	     pPasteEl = NULL;
	     pNewPasteEl->PePrevious = NULL;
	     pEl->ElPrevious = NULL;
	  }
	else
	  {
	     pPasteEl = FirstSavedElement;
	     while (pPasteEl->PeNext != NULL)
		pPasteEl = pPasteEl->PeNext;
	     pNewPasteEl->PePrevious = pPasteEl;
	     pPasteEl->PeNext = pNewPasteEl;
	     pEl->ElPrevious = pPasteEl->PeElement;
	     pPasteEl->PeElement->ElNext = pEl;
	  }
	pNewPasteEl->PeNext = NULL;
	pEl->ElNext = NULL;
	pNewPasteEl->PeElement = pEl;
	pAncest = pParent;
	for (i = 0; i < MAX_PASTE_LEVEL; i++)
	  {
	     if (pAncest == NULL)
	       {
		  pNewPasteEl->PeAscendTypeNum[i] = 0;
		  pNewPasteEl->PeAscendSSchema[i] = NULL;
		  pNewPasteEl->PeAscend[i] = NULL;
	       }
	     else
	       {
		  pNewPasteEl->PeAscendTypeNum[i] = pAncest->ElTypeNumber;
		  pNewPasteEl->PeAscendSSchema[i] = pAncest->ElStructSchema;
		  pNewPasteEl->PeAscend[i] = pAncest;
		  pAncest = pAncest->ElParent;
	       }
	  }
	level = 0;
	pAncest = pParent;
	while (pAncest != NULL)
	  {
	     level++;
	     pAncest = pAncest->ElParent;
	  }
	pNewPasteEl->PeElemLevel = level;
     }
}


/*----------------------------------------------------------------------
   CopyCommand traite la commande COPY				
  ----------------------------------------------------------------------*/
void                CopyCommand ()
{
   PtrElement          firstSel, lastSel, pEl, pCopy, pE, pElAttr, pSecond;
   PtrPasteElem        pSave;
   PtrDocument         pSelDoc;
   PtrAttribute        pAttrLang, pAttrHerit;
   int                 firstChar, lastChar;
   ThotBool            cancopy;

   pCopy = NULL;
   /* y-a-t'il une selection courante ? */
   if (!GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar,
			     &lastChar))
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_SEL_EL);
   else
     {
	if (ThotLocalActions[T_cancopyorcut] != NULL)
	   ThotLocalActions[T_cancopyorcut] (&cancopy, pSelDoc, firstSel,
					     lastSel, firstChar, lastChar);
	else
	   cancopy = TRUE;
	if (cancopy)
	   /* on sauve les elements selectionnes */
	  {
	     if (firstSel->ElTerminal
		 && firstSel->ElLeafType == LtText
		 && firstSel->ElTextLength > 0
		 && firstSel->ElTextLength < firstChar)
		/* la selection commence apres l'element complet */
	       {
		  firstSel = NextElement (firstSel);
		  firstChar = 0;
	       }
	     if (firstSel != NULL)
		if (firstSel != lastSel || firstChar < lastChar - 1 ||
		    firstChar == 0)
		   /* il y a plus d'un caractere selectionne' */
		  {
		     /* libere la sauvegarde de l'Editeur */
		     /* sauvegarde la partie selectionnee */
		     FreeSavedElements ();
		     /* document d'ou vient la partie sauvee */
		     DocOfSavedElements = pSelDoc;
		     pEl = firstSel;
		     /* premier element selectionne */

		     while (pEl != NULL)
			/* pEl : pointeur sur l'element a sauver */
		       {
			  /* copie l'element courant avec ses attributs */
			  /* S'il est reference', perd le lien avec la
			     reference */
			  if (IsolatedPairedElem (pEl, firstSel, lastSel))
			     pCopy = NULL;
			  else
			     /* envoie l'evenement ElemCopy.Pre et demande a
			        l'application si elle est d'accord pour copier
			        l'element */
			  if (SendEventSubTree (TteElemCopy, pSelDoc, pEl, 0))
			     /* l'application ne veut pas que l'editeur copie
			        cet element */
			     pCopy = NULL;
			  else
			     /* pas d'objection de l'application */
			     /* cree une copie de l'element */
			     pCopy = CopyTree (pEl, pSelDoc, 0, pEl->ElStructSchema,
					       pSelDoc, NULL, FALSE, TRUE);
			  if (pCopy != NULL)
			     /* met la copie de l'element courant dans la 
			        chaine des elements sauvegardes */
			    {
			       if (pEl == firstSel && firstChar > 1)
				  /* coupe le premier element sauve' */
				 {
				    pE = pCopy;
				    SplitTextElement (pCopy, firstChar,
						      pSelDoc, FALSE,&pSecond);
				    pCopy = pSecond;
				    /* supprime la premiere partie */
				    DeleteElement (&pE, pSelDoc);
				    if (firstSel == lastSel)
				       /* la fin de la selection est dans le
				          nouvel element */
				       lastChar = lastChar - firstChar + 1;
				 }
			       if (pEl == lastSel)
				  if (lastChar > 0 &&
				      lastChar <= pCopy->ElTextLength)
				     /* coupe le dernier element sauve' */
				    {
				       SplitTextElement (pCopy, lastChar,
							 pSelDoc, FALSE, &pE);
				       /* supprime la deuxieme partie */
				       DeleteElement (&pE, pSelDoc);
				    }
			       SaveElement (pCopy, pEl->ElParent);
			       /* met l'attribut langue sur la copie s'il n'y
			          est pas deja */
			       if (GetTypedAttrForElem (pCopy, 1, NULL) == NULL)
				  /* la copie ne possede pas d'attribut Langue,
				     on le met */
				 {
				    /* cherche d'abord la valeur heritee par
				       l'original */
				    pAttrHerit = GetTypedAttrAncestor (pEl, 1,
							    NULL, &pElAttr);
				    if (pAttrHerit != NULL)
				      {
					 pAttrLang = AddAttrToElem (pCopy, pAttrHerit, NULL);
					 if (pAttrLang != NULL)
					    pAttrLang->AeAttrSSchema = pCopy->ElStructSchema;
				      }
				 }
			    }

			  /* cherche l'element a traiter apres l'element
			     courant */
			  pEl = NextInSelection (pEl, lastSel);
		       }
		  }
	     /* parmi les elements de la copie, cherche et traite tous ceux */
	     /* qui sont reference's */
	     pSave = FirstSavedElement;
	     while (pSave != NULL)
	       {
		  /* traite un sous-arbre */
		  ChangeReferences (pSave->PeElement, &pSelDoc);
		  /* passe au sous-arbre suivant */
		  pSave = pSave->PeNext;
	       }
	     /* il faudra changer les labels des elements insere's par la */
	     /* prochaine commande coller */
	     ChangeLabel = TRUE;

	     /* envoie l'evenement ElemCopy.Post pour tous les sous-arbres */
	     /* copie's */
	     pSave = FirstSavedElement;
	     while (pSave != NULL)
	       {
		  NotifySubTree (TteElemCopy, pSelDoc, pSave->PeElement, 0);
		  /* passe au sous-arbre suivant */
		  pSave = pSave->PeNext;
	       }
	  }
     }
}


/*----------------------------------------------------------------------
   	NextElemToBeCut							
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrElement   NextElemToBeCut (PtrElement pEl, PtrElement lastSel,
				     PtrDocument pSelDoc, PtrElement pSave)
#else  /* __STDC__ */
static PtrElement   NextElemToBeCut (pEl, lastSel, pSelDoc, pSave)
PtrElement          pEl;
PtrElement          lastSel;
PtrDocument         pSelDoc;
PtrElement          pSave;

#endif /* __STDC__ */
{
   ThotBool            stop;
   PtrElement          pNext;

   stop = FALSE;
   pNext = pEl;
   do
      if (pNext == NULL)
	 stop = TRUE;
      else if (pNext->ElTerminal && pNext->ElLeafType == LtPageColBreak)
	 /* c'est une marque de page, */
	 /* on saute l'element */
	 pNext = NextInSelection (pNext, lastSel);
      else
	 /* ce n'est pas une marque de page */
      if (!CanCutElement (pNext, pSelDoc, pSave))
	 /* c'est un element indestructible, */
	 /* on le saute */
	{
	   /* message a l'utilisateur */
	   TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_LIB_MANDATORY_COMPONENT),
	     pNext->ElStructSchema->SsRule[pNext->ElTypeNumber - 1].SrName);
	   pNext = NextInSelection (pNext, lastSel);
	}
      else
	 /* on peut detruire cet element */
	 stop = TRUE;
   while (!stop);
   return pNext;
}


/*----------------------------------------------------------------------
   NextNotPage retourne l'element suivant pEl qui n'est pas un     
   saut de page.                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrElement          NextNotPage (PtrElement pEl)
#else  /* __STDC__ */
PtrElement          NextNotPage (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pNext;
   ThotBool            stop;

   pNext = NextElement (pEl);
   stop = FALSE;
   do
      if (pNext == NULL)
	 stop = TRUE;
      else if (pNext->ElTerminal && pNext->ElLeafType == LtPageColBreak)
	 pNext = NextElement (pNext);
      else
	 stop = TRUE;
   while (!stop);
   return pNext;
}

/*----------------------------------------------------------------------
   PreviousNotPage retourne l'element precedent pEl qui n'est pas  
   un saut de page.                                        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
PtrElement          PreviousNotPage (PtrElement pEl)
#else  /* __STDC__ */
PtrElement          PreviousNotPage (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   PtrElement          pPrev;
   ThotBool            stop;

   pPrev = pEl->ElPrevious;
   /* saute les marques de page */
   stop = FALSE;
   do
      if (pPrev == NULL)
	 stop = TRUE;
      else if (!pPrev->ElTerminal || pPrev->ElLeafType != LtPageColBreak)
	 stop = TRUE;
      else
	 pPrev = pPrev->ElPrevious;
   while (!stop);
   return pPrev;
}

/*----------------------------------------------------------------------
   ProcessFirstLast    Verifie si l'element pPrev devient le   	
   premier parmi ses freres et si pNext devient le dernier 
   Fait reevaluer les regles de presentation "IF First" et 
   "If Last" de ces elements si nececessaire.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ProcessFirstLast (PtrElement pPrev, PtrElement pNext, PtrDocument pDoc)
#else  /* __STDC__ */
void                ProcessFirstLast (pPrev, pNext, pDoc)
PtrElement          pPrev;
PtrElement          pNext;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pSibling;
   ThotBool            stop;

   if (pNext != NULL)
     {
	pSibling = pNext->ElPrevious;
	stop = FALSE;
	do
	   if (pSibling == NULL)
	      stop = TRUE;
	   else if (!pSibling->ElTerminal ||
		    pSibling->ElLeafType != LtPageColBreak)
	      stop = TRUE;
	   else
	      pSibling = pSibling->ElPrevious;
	while (!stop);
	if (pSibling == NULL)
	   /* l'element qui suit la partie detruite devient premier */
	   ChangeFirstLast (pNext, pDoc, TRUE, FALSE);
     }
   if (pPrev != NULL)
     {
	pSibling = pPrev->ElNext;
	stop = FALSE;
	do
	   if (pSibling == NULL)
	      stop = TRUE;
	   else if (!pSibling->ElTerminal ||
		    pSibling->ElLeafType != LtPageColBreak)
	      stop = TRUE;
	   else
	      pSibling = pSibling->ElNext;
	while (!stop);
	if (pSibling == NULL)
	   /* l'element qui precede la partie detruite devient dernier */
	   ChangeFirstLast (pPrev, pDoc, FALSE, FALSE);
     }
}

#define MAX_ANCESTOR 10

/*----------------------------------------------------------------------
   CutCommand  traite la commande CUT				
   Supprime du document la partie selectionnee.               
   Si save est vrai, on sauvegarde la partie selectionnee pour     
   pouvoir la coller ensuite (voir PasteCommand).                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CutCommand (ThotBool save)
#else  /* __STDC__ */
void                CutCommand (save)
ThotBool            save;

#endif /* __STDC__ */
{
  PtrElement          firstSel, lastSel, pEl, pE, pPrev, pNext, pParent,
                      pS, pSS, pParentEl, pFree, pF, pF1, pPrevPage, pSave,
                      pSel, pEl1, pA, firstSelInit, lastSelInit,
		      pAncestor[MAX_ANCESTOR],
		      pAncestorPrev[MAX_ANCESTOR],
		      pAncestorNext[MAX_ANCESTOR];
  DisplayMode         dispMode;
  Document            doc;
  PtrDocument         pSelDoc;
  NotifyElement       notifyEl;
  int                 firstChar, lastChar, nextChar, NSiblings, last, i,
                      firstCharInit, lastCharInit;
  ThotBool            oneAtLeast, cutPage, stop, pageSelected, cutAll;
  ThotBool            canCut, recorded, lock;

  pPrevPage = NULL;
  last = 0;
  lock = TRUE;
  /* y-a-t'il une selection courante ? */
  if (!GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar,
			    &lastChar))
    TtaDisplaySimpleMessage (INFO, LIB, TMSG_SEL_EL);
  else
    {
      canCut = TRUE;
      if (save && ThotLocalActions[T_cancopyorcut] != NULL)
	ThotLocalActions[T_cancopyorcut] (&canCut, pSelDoc, firstSel,
					  lastSel, firstChar, lastChar);
      /* on detruit les elements selectionnes, sauf si le document est en */
      /* lecture seule. */
      if (canCut)
	if (pSelDoc->DocReadOnly)
	  TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_DOC_FORBIDDEN);
	else
	  {
	    /* cherche si l'un des elements selectionne's est protege' */
	    pEl = firstSel;
	    stop = FALSE;
	    while (!stop && pEl != NULL)
	      if (ElementIsReadOnly (pEl))
		stop = TRUE;
	      else
		pEl = NextInSelection (pEl, lastSel);
	    if (stop)
	      TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_EL_FORBIDDEN);
	    else
	      /* pas d'element protege', on peut couper */
	      {
		/* conserve la selection initiale pour pouvoir la
		   retablir au cas ou la commande ne ferait rien */
		firstSelInit = firstSel;
		lastSelInit = lastSel;
		firstCharInit = firstChar;
		lastCharInit = lastChar;
		/* annule la selection */
		TtaClearViewSelections ();
		/* encore rien detruit */
		oneAtLeast = FALSE;
		cutPage = FALSE;
		/* traitement special pour les pages dans les structures
		   qui le demandent */
		if (ThotLocalActions[T_cutpage] != NULL)
		  (*ThotLocalActions[T_cutpage]) (&firstSel, &lastSel, pSelDoc,
						  &save, &cutPage);
		/* "remonte" la selection au niveau des freres si c'est
		   possible */
		if (ThotLocalActions[T_selectsiblings] != NULL)
		  (*ThotLocalActions[T_selectsiblings]) (&firstSel, &lastSel,
							&firstChar, &lastChar);
/******************/
		/* Si tout le contenu d'un element est selectionne', on
		   detruit l'element englobant la selection, sauf s'il
		   est indestructible. */
		if (firstChar <= 1 &&
		    (lastChar == 0 || lastChar > lastSel->ElTextLength))
		  /* le premier et le dernier element de la selection
		     sont selectionnes en entier */
		  {
		    cutAll = FALSE;
		    if (firstSel->ElPrevious == NULL
			&& lastSel->ElNext == NULL
			&& firstSel->ElParent == lastSel->ElParent
			&& firstSel->ElParent != NULL)
		      /* la selection commence par le premier fils et
			 se termine avec le dernier fils du meme pere.
			 On verifie tous les elements intermediaires */
		      {
			cutAll = TRUE;
			pEl = firstSel;
			do
			  if (!CanCutElement (pEl, pSelDoc, NULL))
			    /* l'element ne peut pas etre coupe', on
			       arrete */
			    cutAll = FALSE;
			  else
			    {
			      /* on passe a l'element selectionne'
				 suivant */
			      pEl1 = pEl->ElNext;
			      pEl = NextInSelection (pEl, lastSel);
			      if (pEl != NULL)
				/* il y a un element selectionne'
				   suivant */
				if (pEl != pEl1)
				  /* mais ce n'est pas le frere
				     suivant, on arrete */
				  cutAll = FALSE;
			    }
			while (cutAll && pEl != NULL);
		      }
		    if (cutAll && CanCutElement (firstSel->ElParent, pSelDoc, NULL))
		      {
			/* on fait comme si c'etait le pere qui etait
			   selectionne' */
			firstSel = firstSel->ElParent;
			/* tant que c'est un fils unique, on prend le
			   pere */
			while (firstSel->ElPrevious == NULL &&
			       firstSel->ElNext == NULL &&
			       firstSel->ElParent != NULL &&
			       CanCutElement (firstSel->ElParent, pSelDoc, NULL))
			firstSel = firstSel->ElParent;
			lastSel = firstSel;
			/* l'element est entierement selectionne' */
			firstChar = 0;
			lastChar = 0;
		      }
		  }
/**************/
		doc = IdentDocument (pSelDoc);
		/* lock tables formatting */
		if (ThotLocalActions[T_islock])
		  {
		    (*ThotLocalActions[T_islock]) (&lock);
		    if (!lock)
		      {
			dispMode = TtaGetDisplayMode (doc);
			if (dispMode == DisplayImmediately)
			  TtaSetDisplayMode (doc, DeferredDisplay);
			/* table formatting is not loked, lock it now */
			(*ThotLocalActions[T_lock]) ();
		      }
		  }
		/* on ne supprime pas l'element racine mais son contenu */
		stop = FALSE;
		do
		  if (firstSel == lastSel && firstSel->ElParent == NULL)
		    /* c'est la racine */
		    if (firstSel->ElTerminal ||
			firstSel->ElFirstChild == NULL)
		      {
			firstSel = NULL;
			/* pas de contenu, on ne supprime rien */
			stop = TRUE;
		      }
		    else
		      /* on va suprimer le contenu */
		      {
			firstSel = firstSel->ElFirstChild;
			lastSel = firstSel;
			while (lastSel->ElNext != NULL)
			  lastSel = lastSel->ElNext;
		      }
		  else
		    stop = TRUE;
		while (!stop);

		/* cherche l'element qui precede la partie selectionnee */
		pPrev = PreviousNotPage (firstSel);
		/* cherche le premier element apres la selection */
		pNext = NextNotPage (lastSel);
		nextChar = 0;
		pEl1 = firstSel;
		if (pEl1->ElTerminal
		    && pEl1->ElLeafType == LtText
		    && pEl1->ElTextLength > 0
		    && pEl1->ElTextLength < firstChar)
		  /* debut de la selection apres l'element complet */
		  {
		    firstSel = NextElement (firstSel);
		    firstChar = 0;
		    pPrev = firstSel;
		  }

		if (firstSel != NULL)
		  {
		    /* open the sequence of editing operations for the history */
		    OpenHistorySequence (pSelDoc, firstSelInit, lastSelInit,
				         firstCharInit, lastCharInit);
		    recorded = FALSE;
		    if (firstChar > 1)
		      /* la selection commence a l'interieur d'un element */
		      /* coupe le premier element selectionne */
		      {
		      AddEditOpInHistory (firstSel, pSelDoc, TRUE, TRUE);
		      recorded = TRUE;
		      pPrev = firstSel;
		      SplitBeforeSelection (&firstSel, &firstChar,
					    &lastSel, &lastChar, pSelDoc);
		      }

		    /* On a selectionne' seulement une marque de page ? */
		    pageSelected = cutPage;
		    if (firstSel == lastSel)
		      /* un seul element est selectionne' */
		      if (firstSel->ElTerminal)
			if (firstSel->ElLeafType == LtPageColBreak)
			  pageSelected = TRUE;
		    /* record the sibling of the ancestors */
		    if (firstSel == NULL)
		      pParent = NULL;
		    else
		      pParent = firstSel->ElParent;
		    pAncestorPrev[0] = pPrev;
		    pAncestorNext[0] = pNext;
		    pPrev = firstSel;
		    pNext = lastSel;
		    for (i = 0; i < MAX_ANCESTOR; i++)
		      {
			pAncestor[i] = pParent;
			if (pParent != NULL)
			  pParent = pParent->ElParent;
			if (i > 0)
			  {
			    if (pPrev == NULL)
			      pAncestorPrev[i] = NULL;
			    else
			      {
				pPrev = pPrev->ElParent;
				if (pPrev == NULL)
				  pAncestorPrev[i] = NULL;
				else
				  pAncestorPrev[i] = pPrev->ElPrevious;
			      }
			    if (pNext == NULL)
			      pAncestorNext[i] = NULL;
			    else
			      {
				pNext = pNext->ElParent;
				if (pNext == NULL)
				  pAncestorNext[i] = NULL;
				else
				  pAncestorNext[i] = pNext->ElNext;
			      }
			  }
		      }

		    /* traite tous les elements selectionnes */
		    pEl = firstSel;	/* premier element selectionne */
		    pS = NULL;
		    pSave = NULL;
		    pFree = NULL;
		    while (pEl != NULL)
		      {
			if (!pageSelected)
			  /* On ne detruit pas les marques de pages,
			     sauf si rien d'autre n'a ete selectionne'.
			     On ne detruit pas non plus les elements
			     indestructibles */
			  pEl = NextElemToBeCut (pEl, lastSel, pSelDoc, pSave);
			if (pEl != NULL)
			  {
			    /* pE : pointeur sur l'element a detruire */
			    pE = pEl;

			    if (pE == lastSel)
			      /* that's the last selected element */
		              if (lastSel->ElTerminal &&
				  lastSel->ElLeafType == LtText &&
				  lastChar > 0 &&
				  lastChar <= lastSel->ElTextLength)
				/* la selection se termine a l'interieur d'un
				   element, on le coupe en deux */
				{
				  AddEditOpInHistory (lastSel, pSelDoc, TRUE,
						      FALSE);
				  recorded = TRUE;
				  SplitAfterSelection (lastSel, lastChar,
						       pSelDoc);
				  pNext = lastSel->ElNext;
				  AddEditOpInHistory (pNext, pSelDoc, FALSE,
						      TRUE);
				  pAncestorNext[0] = pNext;
				}

			    /* cherche l'element a traiter apres
			       l'element courant */
			    /* on detruit une marque de page */
			    if (pageSelected && pE->ElTypeNumber == PageBreak + 1
				&& !cutPage)
			      {
				pPrevPage = pE->ElPrevious;
				/* element suivant de la selection */
				/* toujours nul */
				pEl = NULL;
			      }
			    else
			      /* cherche l'element suivant de la
				 selection */
			      pEl = NextInSelection (pEl, lastSel);
			    
			    /* verifie qu'il ne s'agit pas d'un element
			       de paire dont l'homologue ne serait pas
			       dans la selection */
			    if (!IsolatedPairedElem (pE, pE, lastSel))
			      /* envoie l'evenement ElemDelete.Pre et
				 demande a l'application si elle est
				 d'accord pour detruire l'element */
			      if (NextElemToBeCut (pEl, lastSel, pSelDoc, pSave) == NULL)
				last = TTE_STANDARD_DELETE_LAST_ITEM;
			      else
				last = TTE_STANDARD_DELETE_FIRST_ITEMS;
			    if (!SendEventSubTree (TteElemDelete, pSelDoc, pE, last))
			      {
				/* detruit les paves de l'element courant */
				DestroyAbsBoxes (pE, pSelDoc, TRUE);
				/* conserve un pointeur sur le pere */
				pParentEl = pE->ElParent;
				notifyEl.event = TteElemDelete;
				notifyEl.document = doc;
				notifyEl.element = (Element) pParentEl;
				notifyEl.elementType.ElTypeNum = pE->ElTypeNumber;
				notifyEl.elementType.ElSSchema = (SSchema) (pE->ElStructSchema);
				NSiblings = 0;
				pF = pE;
				while (pF->ElPrevious != NULL)
				  {
				    NSiblings++;
				    pF = pF->ElPrevious;
				  }
				notifyEl.position = NSiblings;
				notifyEl.info = 0;

				if (!recorded)
				  /* record that deletion in the history */
				  AddEditOpInHistory (pE, pSelDoc, TRUE, FALSE);
				recorded = FALSE;

				/* retire l'element courant de l'arbre */
				pA = GetOtherPairedElement (pE);
				RemoveElement (pE);
				/* Si c'est un membre d'une paire de
				   marques, indique a l'autre membre
				   qu'il doit etre detruit aussi */
				if (pA != NULL)
				  pA->ElOtherPairedEl = pA;
				SetDocumentModified (pSelDoc, TRUE, 30);
				CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
				oneAtLeast = TRUE;
				if (pageSelected &&
				    pE->ElTypeNumber == PageBreak + 1)
				  /* essaie de fusionner l'element
				     qui precede le saut de page
				     supprime' avec celui qui suit */
				  if (pPrevPage != NULL)
				    /* il y a un elem. precedent */
				    {
				      nextChar = pPrevPage->ElTextLength + 1;
				      if (!IsIdenticalTextType (pPrevPage, pSelDoc, &pF))
					/* il n'y a pas eu de fusion */
					nextChar = 0;
				      else
					/* il y a eu fusion */
					{
					  if (pPrevPage == pPrev)
					    pNext = pPrev;
					  if (pF != NULL)
					    /* chaine l'element libere' par la
					       fusion a la fin de la liste
					       des elements a liberer */
					    {
					      pF->ElNext = NULL;
					      /* il est le dernier de la liste */
					      if (pFree == NULL)
						/* la liste est vide */
						pFree = pF;
					      else
						/* cherche la fin de la liste */
						{
						  pF1 = pFree;
						  while (pF1->ElNext != NULL)
						    pF1 = pF1->ElNext;
						  /* chaine l'element en
						     fin de liste */
						  pF1->ElNext = pF;
						}
					    }
					}
				    }
				if (save)
				  {
				    if (pS == NULL)
				      {
					/* libere l'ancienne
					   sauvegarde */
					FreeSavedElements ();
					/* document d'ou vient la
					   partie sauvee */
					DocOfSavedElements = pSelDoc;
				      }
				    /* il ne faudra pas changer les
				       labels des elements exportables
				       inseres par la prochaine commande
				       Paster */
				    ChangeLabel = FALSE;
				    /* met l'element courant dans la
				       chaine des elements sauvegarde's */
				    SaveElement (pE, pParentEl);
				  }
				if (pS == NULL)
				  pSave = pE;
				else
				  {
				    pS->ElNext = pE;
				    pE->ElPrevious = pS;
				  }
				pS = pE;
				pE->ElParent = pParentEl;
				/* conserve le chainage avec le pere
				   pour SearchPresSchema appele' par
				   UpdateNumbers et pour RedisplayCopies */
			      }
			    if ((last == TTE_STANDARD_DELETE_LAST_ITEM)
				&& !cutPage)
			      pEl = NULL;
			  }
		      }
		    CloseHistorySequence (pSelDoc);

		    /* les elements a couper ont ete coupe's */
		    /* verifie que les elements suivant et precedent */
		    /* n'ont pas ete detruits par les actions */
		    /* declanchees par les evenements TteElemDelete */
		    /* cherche d'abord le premier ancetre qui soit */
		    /* encore dans le document */
		    pParent = NULL;
		    for (i = 0; i < MAX_ANCESTOR && pParent == NULL; i++)
		      {
			if (pAncestor[i] == NULL)
			  i = MAX_ANCESTOR;
			else
			  if (pAncestor[i]->ElStructSchema != NULL &&
			      DocumentOfElement (pAncestor[i]) ==pSelDoc)
			    pParent = pAncestor[i];
		      }
		    
		    pNext = NULL;
		    for (i = 0; i < MAX_ANCESTOR && pNext == NULL; i++)
		      {
			if (pAncestorNext[i] != NULL)
			  if (pAncestorNext[i]->ElStructSchema != NULL &&
			      DocumentOfElement (pAncestorNext[i]) == pSelDoc)
			    pNext = pAncestorNext[i];
			  else
			    if (pAncestorPrev[i] != NULL)
			      if (pAncestorPrev[i]->ElStructSchema != NULL &&
				  DocumentOfElement (pAncestorPrev[i]) == pSelDoc)
				if (pAncestorPrev[i]->ElNext != NULL)
				  pNext = pAncestorPrev[i]->ElNext;
		      }
		    
		    pPrev = NULL;
		    for (i = 0; i < MAX_ANCESTOR && pPrev == NULL; i++)
		      {
			if (pAncestorPrev[i] != NULL)
			  if (pAncestorPrev[i]->ElStructSchema != NULL &&
			      DocumentOfElement (pAncestorPrev[i]) == pSelDoc)
			    pPrev = pAncestorPrev[i];
			  else
			    if (pAncestorNext[i] != NULL)
			      if (pAncestorNext[i]->ElStructSchema != NULL &&
				  DocumentOfElement (pAncestorNext[i]) == pSelDoc)
				if (pAncestorNext[i]->ElPrevious != NULL)
				  pPrev = pAncestorNext[i]->ElPrevious;
		      }
		    
		    /* reaffiche les paves qui copient les elements detruits */
		    if (oneAtLeast)
		      {
			pS = pSave;
			while (pS != NULL)
			  /* parcourt la chaine des elements detruits */
			  {
			    RedisplayCopies (pS, pSelDoc, TRUE);
			    /* element detruit suivant */
			    pS = pS->ElNext;
			  }
		      }
		    /* renumerote la suite */
		    pE = pSave;
		    pS = pNext;
		    if (pS == NULL)
		      pS = NextElement (pPrev);
		    if (pS != NULL)
		      while (pE != NULL)
			{
			  UpdateNumbers (pS, pE, pSelDoc, TRUE);
			  pE = pE->ElNext;
			}
		    pNext = pS;
		    /* annule les pointeurs vers le pere */
		    pS = pSave;
		    while (pS != NULL)
		      /* parcourt la chaine des elements detruits */
		      {
			pS->ElParent = NULL;
			pS = pS->ElNext;
			/* element detruit suivant */
		      }

		    if (!oneAtLeast)
		      {
			/* nothing was destroyed: reset the selection */
			if (!lock)
			  {
			    /* unlock table formatting */
			    (*ThotLocalActions[T_unlock]) ();
			    if (dispMode == DisplayImmediately)
			      TtaSetDisplayMode (doc, DisplayImmediately);
			  }
			if (lastCharInit > 0)
			  lastCharInit--;
			if (firstCharInit > 1)
			  {
			    if (firstSelInit == lastSelInit)
			      i = lastCharInit;
			    else
			      i = 0;
			    SelectString (pSelDoc, firstSelInit, firstCharInit, i);
			  }
			else if ((lastCharInit == 0 && firstCharInit!=1) ||
				 lastSelInit != firstSelInit)
			  SelectElement (pSelDoc, firstSelInit, TRUE, TRUE);
			else
			  SelectString (pSelDoc, firstSelInit, 1, lastCharInit);
			if (lastSelInit != firstSelInit)
			  ExtendSelection (lastSelInit, lastCharInit, TRUE, FALSE, FALSE);
		      }
		    else
		      {
			/* some element were destroyed, finish the work:
			 * check if sibling elements become first or last
			 */
			ProcessFirstLast (pPrev, pNext, pSelDoc);
			if (pPrev != NULL)
			  /* traitement particulier aux tableaux */
			  if (ThotLocalActions[T_createhairline] != NULL)
			    (*ThotLocalActions[T_createhairline]) (pPrev, pSave, pSelDoc);
			/* reaffiche toutes les vues */
			AbstractImageUpdated (pSelDoc);
			RedisplayDocViews (pSelDoc);
			/* libere les elements qui ont ete fusionnes */
			if (pFree != NULL)
			  {
			    pF = pFree;
			    while (pF != NULL)
			      {
				pF1 = pF->ElNext;
				/* element suivant a liberer */
				DeleteElement (&pF, pSelDoc);
				/* libere l'element courant */
				pF = pF1;
				/* passe au suivant */
			      }
			  }

			/* la renumerotation est faite plus haut */
			/* reaffiche les references aux elements detruits */
			/* et enregistre les references sortantes coupees */
			/* ainsi que les elements coupe's qui sont reference's */
			/* par d'autres documents */
			pS = pSave;
			while (pS != NULL)
			  /* parcourt la chaine des elements detruits */
			  {
			    RedisplayEmptyReferences (pS, &pSelDoc, TRUE);
			    RegisterExternalRef (pS, pSelDoc, FALSE);
			    RegisterDeletedReferredElem (pS, pSelDoc);
			    pS = pS->ElNext;
			    /* element detruit suivant */
			  }
			/* Retransmet les valeurs des compteurs et attributs TRANSMIT */
			/* si il y a des elements apres */
			if (pPrev != NULL)
			  pSS = pPrev;
			else
			  pSS = pParent;
			pS = pSave;
			while (pS != NULL)
			  /* parcourt la chaine des elements detruits */
			  {
			    if ((pS->ElStructSchema->SsRule[pS->ElTypeNumber - 1].SrRefImportedDoc))
			      RepApplyTransmitRules (pS, pSS, pSelDoc);
			    pS = pS->ElNext;
			    /* element detruit suivant */
			  }

			if (!lock)
			  {
			    /* unlock table formatting */
			    (*ThotLocalActions[T_unlock]) ();
			    if (dispMode == DisplayImmediately)
			      TtaSetDisplayMode (doc, DisplayImmediately);
			  }

			/* did the selection change? */
			GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar, &lastChar);
			if (firstSel == pSave || ElemIsWithinSubtree(firstSel, pSave))
			  {
			    /* the selection points to deleted elements */
			    if (pNext != NULL)
			      if (nextChar == 0)
				{
				  /* select the following element */
				  pSel = FirstLeaf (pNext);
				  /* Selectionne le premier caractere, si de type texte, l'elt sinon */
				  if (pSel->ElTerminal && pSel->ElLeafType == LtText)
				    SelectPositionWithEvent (pSelDoc, pSel, 1);
				  else
				    SelectElementWithEvent (pSelDoc, pSel, TRUE, TRUE);
				}
			      else
				SelectPositionWithEvent (pSelDoc, pNext, nextChar);
			    else
			      /* s'il n'y a pas de suivant, selectionne le precedent */
			      if (pPrev != NULL)
				{
				  pSel = LastLeaf (pPrev);
				  if (pSel->ElTerminal && pSel->ElLeafType == LtText)
				    SelectPositionWithEvent (pSelDoc, pSel, pSel->ElTextLength + 1);
				  else
				    SelectElementWithEvent (pSelDoc, pSel, FALSE, TRUE);
				}
			      else
				/* s'il n'y a ni suivant ni precedent, selectionne l'englobant */
				SelectElementWithEvent (pSelDoc, pParent, TRUE, TRUE);
			  }

			if (!save)
			  {
			    /* libere les elements coupe's */
			    pE = pSave;
			    while (pE != NULL)
			      {
				pS = pE->ElNext;
				DeleteElement (&pE, pSelDoc);
				pE = pS;
			      }
			  }
		      }
		  }
	      }
	  }
    }
}

/*----------------------------------------------------------------------
   EmptyElement    retourne vrai si l'element pEl est vide.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            EmptyElement (PtrElement pEl)
#else  /* __STDC__ */
ThotBool            EmptyElement (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   ThotBool            empty;
   PtrElement          pChild;

   if (pEl->ElVolume == 0)
      empty = TRUE;
   else if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsConstant)
      empty = TRUE;
   else if (pEl->ElTerminal)
      empty = FALSE;
   else
     {
	empty = TRUE;
	pChild = pEl->ElFirstChild;
	while (pChild != NULL && empty)
	  {
	     empty = EmptyElement (pChild);
	     pChild = pChild->ElNext;
	  }
     }
   return empty;
}



/*----------------------------------------------------------------------
   CanSurround indique si un element de type (typeNum, pSS) peut etre 
   le pere des elements freres compris entre l'element     
   firstEl et l'elemnt lastEl, dans le document pDoc.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     CanSurround (int typeNum, PtrSSchema pSS, PtrElement firstEl,
				 PtrElement lastEl, PtrDocument pDoc)
#else
static ThotBool     CanSurround (typeNum, pSS, firstEl, lastEl, pDoc)
int                 typeNum;
PtrSSchema          pSS;
PtrElement          firstEl;
PtrElement          lastEl;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   PtrElement          pEl, pElSurround;
   ThotBool            ok;

   ok = TRUE;
   /* on teste d'abord le constructeur du candidat englobant */
   switch (pSS->SsRule[typeNum - 1].SrConstruct)
	 {
	    case CsConstant:
	    case CsReference:
	    case CsBasicElement:
	    case CsPairedElement:
	       /* types sans descendance possible, inutile d'aller plus loin */
	       ok = FALSE;
	       break;
	    case CsChoice:
	       if (pSS->SsRule[typeNum - 1].SrNChoices == -1)
		  /* NATURE, on n'a pas envie de changer de schema de struct. */
		  ok = FALSE;
	       break;
	    default:
	       /* on va regarder de plus pres... */
	       ok = TRUE;
	       break;
	 }
   if (ok && firstEl != NULL)
      /* le constructeur du candidat englobant permet une descendance */
      /* on verifie que les candidats englobe's peuvent etre ses */
      /* descendants */
     {
	/* on cree un element du type du candidat englobant */
	pElSurround = NewSubtree (typeNum, pSS, pDoc, firstEl->ElAssocNum,
				  FALSE, TRUE, FALSE, FALSE);
	/* on insere temporairement cet element dans l'arbre, comme frere */
	/* precedent du premier element a englober */
	InsertElementBefore (firstEl, pElSurround);
	pEl = firstEl;
	/* on parcourt tous les candidats englobe's */
	while (pEl != NULL && ok)
	  {
	     /* on verifie si un element du type de cet element a englober */
	     /* peut etre un fils du candidat englobant */
	     ok = AllowedFirstChild (pElSurround, pDoc, pEl->ElTypeNumber,
				     pEl->ElStructSchema, TRUE, FALSE);
	     if (ok)
		/* passe a l'element suivant */
		if (pEl == lastEl)
		   pEl = NULL;
		else
		   pEl = pEl->ElNext;
	  }
	/* retire et libere l'element cree' temporairement */
	DeleteElement (&pElSurround, pDoc);
     }
   return ok;
}

/*----------------------------------------------------------------------
  DoSurround
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     DoSurround (PtrElement firstEl, PtrElement lastEl, int firstChar,
		int lastChar, PtrDocument pDoc, int typeNum, PtrSSchema pSS)
#else
static ThotBool     DoSurround (firstEl, lastEl, firstChar, lastChar, pDoc, typeNum, pSS)
PtrElement          firstEl;
PtrElement          lastEl;
int                 firstChar;
int                 lastChar;
PtrDocument         pDoc;
int                 typeNum;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   PtrElement          pSibling, pEl, pRoot, pElSurround, pNext, pPrev,
                       pEl1;
   NotifyElement       notifyEl;
   int                 NSiblings;
   ThotBool            unit, splitElem, ok;

   /* l'element devant lequel on va creer le nouvel element englobant */
   pEl1 = firstEl;
   /* on verifie qu'on produit bien une structure correcte */
   splitElem = FALSE;
   ok = FALSE;
   do
     {
	ok = AllowedSibling (pEl1, pDoc, typeNum, pSS, TRUE, FALSE, FALSE);
	if (!ok)
	  {
	     if ((firstEl->ElTerminal && firstEl->ElLeafType == LtText &&
		  firstChar > 1) ||
		 (pEl1->ElPrevious != NULL))
		splitElem = TRUE;
	     pEl1 = pEl1->ElParent;
	  }
     }
   while (pEl1 != NULL && !ok);
   if (pEl1 == NULL || !ok)
      return FALSE;

   /* demande a l'application si on peut creer ce type d'element */
   notifyEl.event = TteElemNew;
   notifyEl.document = (Document) IdentDocument (pDoc);
   notifyEl.element = (Element) (pEl1->ElParent);
   notifyEl.elementType.ElTypeNum = typeNum;
   notifyEl.elementType.ElSSchema = (SSchema) pSS;
   pSibling = pEl1;
   NSiblings = 1;
   while (pSibling->ElPrevious != NULL)
     {
	NSiblings++;
	pSibling = pSibling->ElPrevious;
     }
   notifyEl.position = NSiblings;
   if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
      /* l'application refuse la creation de ce type d'element */
      pEl1 = NULL;
   else
      /* l'application accepte la creation de ce type d'element */
     {
	/* demande a l'application si elle accepte de deplacer les elements
	   qui doivent l'etre */
	pEl = firstEl;
	while (pEl != NULL)
	  {
	     notifyEl.event = TteElemMove;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) pEl;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	     notifyEl.position = 0;
	     if (CallEventType ((NotifyEvent *) & notifyEl, TRUE))
		/* l'application refuse */
	       {
		  pEl = NULL;
		  pEl1 = NULL;
	       }
	     else
		/* l'application accepte. Passe a l'element suivant */
	     if (pEl == lastEl)
		pEl = NULL;
	     else
		pEl = pEl->ElNext;
	  }
     }
   if (pEl1 == NULL)
      return FALSE;
   else
     {
	/* on cree un element du type choisi par l'utilisateur */
	pElSurround = NewSubtree (typeNum, pSS, pDoc, firstEl->ElAssocNum,
				  FALSE, TRUE, TRUE, TRUE);
	pRoot = pElSurround;
	unit = FALSE;
	if (pElSurround->ElStructSchema->SsCode !=
	    pEl1->ElParent->ElStructSchema->SsCode)
	   /* cet element appartient a un schema de structure different de */
	   /* celui de l'element qui devrait devenir son pere */
	   if (pElSurround->ElStructSchema->SsRule[pElSurround->ElTypeNumber - 1].SrUnitElem)
	      /* c'est une unite', on le note */
	      unit = TRUE;
	   else if (pElSurround->ElTypeNumber != pElSurround->ElStructSchema->SsRootElem)
	      /* ce n'est pas la racine de son schema */
	     {
		/* on cree un element racine du schema */
		pRoot = NewSubtree (pElSurround->ElStructSchema->SsRootElem,
				    pElSurround->ElStructSchema, pDoc,
				    firstEl->ElAssocNum, FALSE, TRUE, TRUE,
				    TRUE);
		/* l'element qui va englober les elements selectionne's */
		/* devient le fils de ce nouvel element */
		InsertFirstChild (pRoot, pElSurround);
	     }

	if (pRoot == NULL || pElSurround == NULL)
	   /* impossible de creer l'element voulu, abandon */
	   return FALSE;
	/* on a cree' l'element voulu */
	TtaClearViewSelections ();	/* on annule d'abord la selection */
	/* on coupe eventuellement les atomes de texte */
	SplitBeforeSelection (&firstEl, &firstChar, &lastEl, &lastChar, pDoc);
	SplitAfterSelection (lastEl, lastChar, pDoc);

	if (splitElem)
	   /* coupe l'element en deux */
	  {
	     AddEditOpInHistory (pEl1, pDoc, TRUE, TRUE);
	     if (BreakElement (pEl1, firstEl, firstChar, FALSE, FALSE))
		{
		pEl1 = pEl1->ElNext;
		AddEditOpInHistory (pEl1, pDoc, FALSE, TRUE);
		}
	     else
		CancelLastEditFromHistory (pDoc);
	  }

	/* on detruit les paves des elements qui vont etre deplaces */
	pEl = firstEl;
	while (pEl != NULL)
	  {
	     DestroyAbsBoxes (pEl, pDoc, TRUE);
	     if (pEl == lastEl)
		pEl = NULL;
	     else
		pEl = pEl->ElNext;
	  }
	AbstractImageUpdated (pDoc);

	/* on insere l'element cree' comme frere precedent du premier */
	/* element a englober */
	InsertElementBefore (pEl1, pRoot);
	AddEditOpInHistory (pRoot, pDoc, FALSE, TRUE);
	pEl = firstEl;
	pPrev = NULL;
	/* on parcourt tous les elements a englober et on les deplace */
	/* pour les inserer comme fils de l'element englobant */
	while (pEl != NULL)
	  {
	     pNext = pEl->ElNext;
	     AddEditOpInHistory (pEl, pDoc, TRUE, FALSE);
	     /* retire un element de l'arbre abstrait */
	     RemoveElement (pEl);
	     /* si c'est une feuille unite', elle prend le schema de structure
	        de son futur pere. */
	     if (unit)
		if (pEl->ElTerminal)
		   if (pEl->ElLeafType == LtText ||
		       pEl->ElLeafType == LtPicture ||
		       pEl->ElLeafType == LtPolyLine ||
		       pEl->ElLeafType == LtSymbol ||
		       pEl->ElLeafType == LtGraphics)
		      pEl->ElStructSchema = pElSurround->ElStructSchema;
	     /* place l'element a sa nouvelle position */
	     if (pPrev == NULL)
		InsertFirstChild (pElSurround, pEl);
	     else
		InsertElementAfter (pPrev, pEl);
	     pPrev = pEl;
	     /* on envoie un evenement ElemMove.Post a l'application */
	     notifyEl.event = TteElemMove;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) pEl;
	     notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pEl->ElStructSchema);
	     notifyEl.position = 0;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	     /* passe a l'element suivant */
	     if (pEl == lastEl)
		pEl = NULL;
	     else
		pEl = pNext;
	  }
	/* on cree les elements manquants dans l'element qui vient */
	/* d'etre cree', et on lui met les attributs obligatoires */
	CompleteElement (pElSurround, pDoc);
	RemoveExcludedElem (&pRoot, pDoc);
	AttachMandatoryAttributes (pElSurround, pDoc);
	if (pDoc->DocSSchema != NULL)
	   /* le document n'a pas ete ferme' pendant que l'utilisateur avait */
	   /* le controle pour donner les valeurs des attributs obligatoires */
	  {
	     /* on envoie un evenement ElemNew.Post a l'application */
	     notifyEl.event = TteElemNew;
	     notifyEl.document = (Document) IdentDocument (pDoc);
	     notifyEl.element = (Element) pElSurround;
	     notifyEl.elementType.ElTypeNum = pElSurround->ElTypeNumber;
	     notifyEl.elementType.ElSSchema = (SSchema) (pElSurround->ElStructSchema);
	     notifyEl.position = 0;
	     if (pElSurround->ElTypeNumber == pElSurround->ElStructSchema->SsRootElem)
		/* root element in a different structure schema */
		/* Put number of elements in the "position" field */
		notifyEl.position = pElSurround->ElStructSchema->SsNObjects;
	     CallEventType ((NotifyEvent *) & notifyEl, FALSE);
	     /* cree les paves du nouvel element et reaffiche */
	     CreateAllAbsBoxesOfEl (pRoot, pDoc);
	     AbstractImageUpdated (pDoc);
	     RedisplayDocViews (pDoc);
	     /* si on est dans un element copie' par inclusion, on met a jour
	        les copies de cet element. */
	     RedisplayCopies (pRoot, pDoc, TRUE);
	     /* on met a jour les numeros affectes par la creation des */
	     /* nouveaux elements */
	     UpdateNumbers (NextElement (pRoot), pRoot, pDoc, TRUE);
	     /* on indique que le document est modifie' */
	     SetDocumentModified (pDoc, TRUE, 30);
	     /* on selectionne l'element englobant */
	     SelectElementWithEvent (pDoc, pElSurround, TRUE, TRUE);
	  }
     }
   return TRUE;
}

/*----------------------------------------------------------------------
   SearchChoiceRules cherche les regles CsChoice qui derivent de la   
   regle (pSS, typeNum) et qui menent au type de l'element pEl.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     SearchChoiceRules (PtrSSchema pSS, int typeNum,
				       PtrElement pEl, int *param)
#else
static ThotBool     SearchChoiceRules (pSS, typeNum, pEl, param)
PtrSSchema          pSS;
int                 typeNum;
PtrElement          pEl;
int                *param;

#endif /* __STDC__ */
{
   SRule              *pSRule;
   PtrChoiceOptionDescr pChoiceD, *Anchor;
   int                 choice;
   ThotBool            found, doit;

   found = FALSE;
   Anchor = (PtrChoiceOptionDescr *) param;
   if (pSS->SsCode != pEl->ElStructSchema->SsCode ||
       typeNum != pEl->ElTypeNumber)
      /* on n'est pas arrive' encore au type de l'element pEl */
     {
	pSRule = &(pSS->SsRule[typeNum - 1]);
	doit = TRUE;
	if (pSRule->SrRecursive)
	   /* regle recursive */
	   if (pSRule->SrRecursDone)
	      /* elle a deja ete rencontree, on ne fait rien */
	      doit = FALSE;
	   else
	      /* elle n'a pas encore ete rencontree, on la traite */
	      pSRule->SrRecursDone = TRUE;
	if (doit)
	   /* traitement selon le constructeur du type */
	   switch (pSRule->SrConstruct)
		 {
		    case CsIdentity:
		       /* CsIdentity', on continue */
		       found = SearchChoiceRules (pSS, pSRule->SrIdentRule,
						  pEl, (int *) Anchor);
		       break;
		    case CsList:
		       /* CsList, on continue */
		       found = SearchChoiceRules (pSS, pSRule->SrListItem, pEl,
						  (int *) Anchor);
		       break;
		    case CsChoice:
		       if (pSRule->SrNChoices == 0)
			  /* SRule UNIT */
			 {
			    if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrUnitElem)
			       /* l'element est une unite', on a trouve' la regle */
			       found = TRUE;
			 }
		       else if (pSRule->SrNChoices > 0)
			  /* SRule CsChoice avec options explicites */
			 {
			    /* on verifie d'abord si le type de l'element est une */
			    /* des options de ce choix */
			    if (pSS->SsCode == pEl->ElStructSchema->SsCode)
			       for (choice = 0; choice < pSRule->SrNChoices &&
				    !found; choice++)
				  if (pSRule->SrChoice[choice] == pEl->ElTypeNumber)
				     /* c'est effectivement une des options */
				     found = TRUE;
			    if (!found)
			       /* on n'a pas trouve'. On cherche a partir des
			          regles qui definissent les options du choix */
			       for (choice = 0; choice < pSRule->SrNChoices &&
				    !found; choice++)
				  found = SearchChoiceRules (pSS,
					      pSRule->SrChoice[choice], pEl,
							     (int *) Anchor);
			 }
		       if (found)
			  /* cette regle CsChoice mene au type de pEl */
			  /* on l'enregistre dans la liste des regles ChoiX */
			  /* traversees */
			 {
			    pChoiceD = (PtrChoiceOptionDescr) TtaGetMemory (sizeof (ChoiceOptionDescr));
			    if (pChoiceD != NULL)
			      {
				 pChoiceD->COtypeNum = typeNum;
				 pChoiceD->COStructSch = pSS;
				 pChoiceD->CONext = *Anchor;
				 *Anchor = pChoiceD;
			      }
			 }
		       break;
		    default:
		       break;
		 }
	if (pSRule->SrRecursive && pSRule->SrRecursDone && doit)
	   pSRule->SrRecursDone = FALSE;
     }
   param = (int *) Anchor;
   return found;
}


/*----------------------------------------------------------------------
   	BuildChangeTypeTable						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BuildChangeTypeTable (PtrElement pEl)
#else
static void         BuildChangeTypeTable (pEl)
PtrElement          pEl;

#endif /* __STDC__ */
{
   PtrIsomorphDesc     pIsoD, pNextIsoD;
   PtrChoiceOptionDescr pChoicesFound, pChoiceD, pOldChoiceD;
   SRule              *pSRule;
   PtrSSchema          pSSasc;
   PtrElement          pAncest, pPrev;
   int                 choice, typeNum;
   STRING	       strResDyn;

   NChangeTypeItems = 0;	/* la table est vide pour l'instant */
   if (pEl != NULL)
      if (pEl->ElParent != NULL)
	 /* on cherche les regles CsChoice qui permettent de passer du pere */
	 /* de l'element pEl a l'element pEl lui-meme */
	{
	   pChoicesFound = NULL;
	   if (SearchChoiceRules (pEl->ElParent->ElStructSchema,
				  pEl->ElParent->ElTypeNumber,
				  pEl, (int *) &pChoicesFound))
	      /* on a trouve' au moins une regle CsChoice dont pEl est issu */
	     {
		/* on vide la liste des descripteurs de types isomorphes */
		pIsoD = firstIsomorphDesc;
		while (pIsoD != NULL)
		  {
		     pNextIsoD = pIsoD->IDNext;
		     TtaFreeMemory ( pIsoD);
		     pIsoD = pNextIsoD;
		  }
		firstIsomorphDesc = NULL;
		strResDyn = NULL;
		if (strResDyn!= NULL)
		  /* on initialise la transformation automatique */
		  RestInitMatch ((Element)pEl, (Element)pEl);

 		/* on commence par remplir la table des types a proposer dans
		   le menu de changement de types */
		/* on parcourt la liste des regles choix trouvees */
		pChoiceD = pChoicesFound;
		while (pChoiceD != NULL)
		  {
		     /* la regle CsChoice courante */
		     pSRule = &(pChoiceD->COStructSch->SsRule[pChoiceD->COtypeNum - 1]);
		     if (pSRule->SrNChoices == 0)
			/* c'est une regle UNIT */
		       {
			  /* cherche toutes les unites definies dans les schemas de */
			  /* structure des elements ascendants */
			  pAncest = pEl;
			  pPrev = NULL;
			  /* examine tous les elements ascendants tant que la table */
			  /* n'est pas pleine */
			  while (pAncest != NULL &&
			       NChangeTypeItems < MAX_ITEMS_CHANGE_TYPE - 1)
			    {
			       if (pPrev == NULL ||
				   pAncest->ElStructSchema != pPrev->ElStructSchema)
				  /* cet element englobant appartient a un
				     schema different de l'element traite'
				     precedemment. On examine les unites
				     definies dans ce schema */
				 {
				    pSSasc = pAncest->ElStructSchema;
				    /* si c'est une extension de schema, on
				       revient au schema de base pour examiner
				       toutes les extensions */
				    while (pSSasc->SsPrevExtens != NULL)
				       pSSasc = pSSasc->SsPrevExtens;
				    /* cherche les unites definies dans ce
				       schema et dans ses extensions */
				    do
				      {
					 /* boucle sur toutes les regles du schema */
					 for (typeNum = 1; typeNum <= pSSasc->SsNRules &&
					      NChangeTypeItems < MAX_ITEMS_CHANGE_TYPE - 1; typeNum++)
					    if (pSSasc->SsRule[typeNum - 1].SrUnitElem)
					       /* cette regle definit une unite'. On essaie de la */
					       /* mettre dans la table associee au menu */
					       RegisterIfIsomorphic (pEl, pSSasc, typeNum);
					 /* passe a l'extension de schema suivante */
					 pSSasc = pSSasc->SsNextExtens;
				      }
				    while (pSSasc != NULL);
				 }
			       /* passe a l'element ascendant du dessus */
			       pPrev = pAncest;
			       pAncest = pAncest->ElParent;
			    }
		       }
		     else
			/* c'est un choix a options explicites */
			/* on essaie de mettre dans la table associee au menu
			   toutes les options du choix */
			for (choice = 0; choice < pSRule->SrNChoices &&
			     NChangeTypeItems < MAX_ITEMS_CHANGE_TYPE - 1;
			     choice++)
			   RegisterIfIsomorphic (pEl, pChoiceD->COStructSch,
						 pSRule->SrChoice[choice]);
		     /* passe a la regle CsChoice suivante */
		     pOldChoiceD = pChoiceD;
		     pChoiceD = pChoiceD->CONext;
		     TtaFreeMemory ( pOldChoiceD);
		  }
	     }
	}
}

/*----------------------------------------------------------------------
   	ChangeTypeOfElements						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     ChangeTypeOfElements (PtrElement firstEl, PtrElement lastEl,
		    PtrDocument pDoc, int newTypeNum, PtrSSchema newSSchema)
#else  /* __STDC__ */
static ThotBool     ChangeTypeOfElements (firstEl, lastEl, pDoc, newTypeNum, newSSchema)
PtrElement          firstEl;
PtrElement          lastEl;
PtrDocument         pDoc;
int                 newTypeNum;
PtrSSchema          newSSchema;

#endif /* __STDC__ */
{
   PtrElement          pEl;
   Element             El;
   ElementType         elType;
   int                 ent, method = 0;
   ThotBool            ok;
   ThotBool            done = FALSE;

   /* Don't do anything if it's not a sequence of sibling elements */
   if (firstEl->ElParent == lastEl->ElParent)
      /* on ne peut rien faire si le pere est protege' */
      if (!ElementIsReadOnly (firstEl->ElParent))
	{
	   /* on essaie de les englober dans un element du type demande' */
	   /* verifie si ce type d'elements accepte les elements voulus */
	   /* comme fils */
	   if (CanSurround (newTypeNum, newSSchema, firstEl, lastEl, pDoc))
	      /* verifie si ce type peut etre un fils du pere des elements a */
	      /* englober */
	      if (AllowedFirstChild (firstEl->ElParent, pDoc, newTypeNum,
				     newSSchema, FALSE, FALSE))
		 done = DoSurround (firstEl, lastEl, 0, 0, pDoc, newTypeNum,
				    newSSchema);
	   if (!done)
	      /* essaie de changer le type des elements selectionne's */
	     {
	     if (lastEl == firstEl)
		/* un seul element selectionne', on essaie de changer son type */
		pEl = firstEl;
	     else
		/* plusieurs freres selectionn'es */
		if (firstEl->ElPrevious == NULL && lastEl->ElNext == NULL)
		   /* tous les freres sont selectionne's. On essaie de changer
		      le type de leur pere */
		   pEl = firstEl->ElParent;
		else
		   /* tous les freres ne sont pas selectionne's: on ne fait rien */
		   pEl = NULL;
		while (pEl != NULL && !done)
		  {
		     if (ElementIsReadOnly (pEl->ElParent))
			pEl = NULL;
		     else
		       {
			  /* on cree la table des types equivalents a celui de pEl */
			  BuildChangeTypeTable (pEl);
			  /* le type cible est-il dans cette table ? */
			  ok = FALSE;
			  for (ent = 0; !ok && ent < NChangeTypeItems; ent++)
			    {
			       if (newTypeNum == ChangeTypeTypeNum[ent])
				  if (newSSchema->SsCode ==
				      ChangeTypeSSchema[ent]->SsCode)
				    {
                                      method = ChangeTypeMethod[ent];
				      ok = TRUE;
				    }
			    }
			  if (ok)
			     /* le type est dans la table, on effectue le
			        changement */
			    {
			    /* store the command in the history */
			    AddEditOpInHistory (pEl, pDoc, TRUE, TRUE);
			    done = FALSE;
			    switch (method)
			      {
			      case M_EQUIV :
				done = DoChangeType (pEl, pDoc, newTypeNum,
						     newSSchema);
				break;
			      case M_RESDYN :
				done = RestChangeType((Element)pEl, IdentDocument (pDoc),
						      newTypeNum, (SSchema)newSSchema);
				break;
			      }
			    if (!done)
			       CancelLastEditFromHistory (pDoc);
			    }
		       }
		     if (!done)
			/* on essaie de changer le type du pere si on est sur
			   un fils unique */
			if (pEl->ElPrevious == NULL && pEl->ElNext == NULL)
			   pEl = pEl->ElParent;
			else
			   pEl = NULL;
		  }
	     }
	   if (!done)
	      /* si l'element est vide, essaie de creer un element du tye */
	      /* voulu a l'interieur de l'element vide */
	      if (lastEl == firstEl && !firstEl->ElTerminal &&
		  firstEl->ElFirstChild == NULL)
		{
		   AddEditOpInHistory (firstEl, pDoc, TRUE, TRUE);
		   elType.ElSSchema = (SSchema) newSSchema;
		   elType.ElTypeNum = newTypeNum;
		   El = TtaCreateDescentWithContent (IdentDocument (pDoc),
						 (Element) firstEl, elType);
		   if (El != NULL)
		      /* on a pu creer l'element du type voulu */
		      done = TRUE;
		   else
		      CancelLastEditFromHistory (pDoc);
		}
	   if (!done)
	      /* on essaie d'englober les elements selectionne's dans un element */
	      /* du type demande', eventuellement en eclatant les englobants */
	      /* verifie si ce type d'elements accepte les elements voulus */
	      /* comme fils */
	      if (CanSurround (newTypeNum, newSSchema, firstEl, lastEl, pDoc))
		 done = DoSurround (firstEl, lastEl, 0, 0, pDoc, newTypeNum,
				    newSSchema);
     }
   return done;
}

/*----------------------------------------------------------------------
  CanInsertBySplitting
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool	    CanInsertBySplitting (PtrElement *pEl, int charSplit,
			ThotBool *splitElem, PtrElement *pSplitEl,
			PtrElement *pElSplit, ThotBool createAfter,
			int typeNum, PtrSSchema pSS, PtrDocument pDoc)
#else  /* __STDC__ */
static ThotBool	    CanInsertBySplitting (pEl, charSplit, splitElem, pSplitEl,
			pElSplit, createAfter, typeNum, pSS, pDoc)

PtrElement	 *pEl;
int		  charSplit;
ThotBool		 *splitElem;
PtrElement	 *pSplitEl;
PtrElement	 *pElSplit;
ThotBool		 createAfter;
int		 typeNum;
PtrSSchema	 pSS;
PtrDocument	 pDoc;

#endif /* __STDC__ */

{
   PtrElement	pElem, Sibling, pList, pF, pSplit;
   ThotBool	ok;

   Sibling = NULL;
   *splitElem = FALSE;
   *pSplitEl = NULL;
   *pElSplit = NULL;
   ok = FALSE;
   pElem = *pEl;
   do
     {
     /* isolate pElem from its sibling. If pElem is a component */
     /* of an aggregate, AllowedSibling would always say no otherwise */
     if (*splitElem)
       if (createAfter)
         {
         Sibling = pElem->ElNext;
         pElem->ElNext = NULL;
         }
       else
         {
         Sibling = pElem->ElPrevious;
         pElem->ElPrevious = NULL;
         }
     ok = AllowedSibling (pElem, pDoc, typeNum, pSS, (ThotBool)(!createAfter), TRUE,FALSE);
     /* restore link with sibling */
     if (*splitElem)
       if (createAfter)
         pElem->ElNext = Sibling;
       else
         pElem->ElPrevious = Sibling;

     if (ok)
       *pEl = pElem;
     else
       {
       if ((pElem->ElTerminal && pElem->ElLeafType == LtText && charSplit > 0)
	   || *splitElem ||
   	   (createAfter && pElem->ElNext != NULL) ||
   	   (!createAfter && pElem->ElPrevious != NULL))
   	  {
   	  if (*splitElem && pElem != *pSplitEl)
   	     {
   	     if (pElem == *pSplitEl)
   		pElem = NULL;
   	     }
   	  else
   	     {
   	     if (createAfter && charSplit == 0 && !*splitElem)
   		pSplit = pElem->ElNext;
   	     else
   		pSplit = pElem;
   	     if (CanSplitElement (pSplit, charSplit, FALSE, &pList, &pF,
				  pSplitEl))
   		{
   	        *splitElem = TRUE;
   		if (*pElSplit == NULL)
   		   *pElSplit = pSplit;
   		}
   	     else
   		pElem = NULL;
   	     }
   	  }
       if (pElem != NULL)
          pElem = pElem->ElParent;
       }
     }
   while (pElem != NULL && !ok);
   return ok;
}

/*----------------------------------------------------------------------
   CreateNewElement						
   L'utilisateur veut creer, pour le document pDoc, au voisinage   
   de la selection courante, un element du type typeNum defini     
   dans le schema de structure pSS.               			
   Before indique si on veut inserer le nouvel element avant la    
   selection courante (TRUE) ou a la place de la selection         
   courante (FALSE).                                               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateNewElement (int typeNum, PtrSSchema pSS, PtrDocument pDoc, ThotBool Before)
#else  /* __STDC__ */
void                CreateNewElement (typeNum, pSS, pDoc, Before)
int                 typeNum;
PtrSSchema          pSS;
PtrDocument         pDoc;
ThotBool            Before;
#endif /* __STDC__ */

{
  PtrElement          firstSel, lastSel, pNew, pF, pSibling, pEl, pSecond;
  PtrElement          pElem, pElSplit, pSplitEl, pNextEl;
  ElementType	      elType;
  PtrDocument         pSelDoc;
  NotifyElement       notifyEl;
  int                 firstChar, lastChar, origFirstChar, origLastChar,
		      NSiblings, ancestorRule,
		      rule, prevrule, prevprevrule, nComp;
  ThotBool            InsertionPoint, ok, createAfter, splitElem, elConst;
  ThotBool            empty, selHead, selTail, done, deleted;

  NSiblings = 0;
  if (!GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar, &lastChar))
    TtaDisplaySimpleMessage (INFO, LIB, TMSG_SEL_EL);
  else if (pSelDoc != pDoc)
    /* the document asking for the creation of a new element is NOT the */
    /* document containing the current selection */
    return;
  else if (pSelDoc->DocReadOnly)
    TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_DOC_FORBIDDEN);
  else
    /* there is a selection and the document can be modified */
    {
      elConst = FALSE;
      empty = FALSE;
      InsertionPoint = (firstSel == lastSel  &&
			firstSel->ElTerminal &&
			((firstSel->ElLeafType == LtText && firstChar > 0 &&
			  firstChar == lastChar)            ||
			 firstSel->ElLeafType == LtPicture  ||
			 firstSel->ElLeafType == LtGraphics ||
			 firstSel->ElLeafType == LtPolyLine ||
			 firstSel->ElLeafType == LtSymbol     ));
      /* Peut-on considerer la selection courante comme un simple point */
      /* d'insertion ? */
      if (!InsertionPoint)
	if (firstSel == lastSel)
	  /* un seul element selectionne' */
	  if (GetElementConstruct (firstSel, &nComp) == CsConstant)
	    /* c'est une constante, on va creer le nouvel element devant */
	    {
	      InsertionPoint = TRUE;
	      elConst = TRUE;
	    }
      if (!InsertionPoint)
	if (firstSel == lastSel)
	  if (EmptyElement (firstSel))
	    /* c'est un element vide unique */
	    {
	      InsertionPoint = TRUE;
	      empty = TRUE;
	    }

      if (!InsertionPoint)
	/* il n'y a pas un simple point d'insertion, mais du texte et/ou */
	/* un ou des elements selectionne's */
	{
	  /* Coupe les elements du debut et de la fin de la selection s'ils */
	  /* sont partiellement selectionnes */
	  TtaClearViewSelections ();
	  IsolateSelection (pSelDoc, &firstSel, &lastSel, &firstChar,
			    &lastChar, FALSE);
	  AbstractImageUpdated (pSelDoc);
	  RedisplayDocViews (pSelDoc);
	  if (lastSel->ElTerminal && lastSel->ElLeafType == LtText &&
	      lastChar >= lastSel->ElTextLength)
	    {
	    lastChar = 0;
	    if (firstSel == lastSel && firstChar == 1)
	       firstChar = 0; 
	    }
	  if (Before)
	    InsertionPoint = TRUE;
	  else if (firstChar <= 1 && lastChar == 0)
	    /* le premier et le dernier element selectionnes sont
	       entierement selectionne's */
	    /* on essaie de changer le type des elements selectionne's */
	    {
	      /* on essaie d'abord de transformer l'ensemble des elements
		 selectionne's en un element du type demande' */
	      /* remonte la selection courante sur une suite de freres si
		 c'est equivalent */
	      if (firstSel->ElParent != lastSel->ElParent)
	        {
		/* essaie de ramener la selection a une suite de freres */
		firstChar = 0;
		lastChar = 0;
		if (ThotLocalActions[T_selectsiblings] != NULL)
		   (*ThotLocalActions[T_selectsiblings]) (&firstSel, &lastSel,
						       &firstChar, &lastChar);
	        }
	      if (firstSel->ElParent != lastSel->ElParent)
		ok = FALSE;
	      else
		{
		OpenHistorySequence (pSelDoc, firstSel, lastSel, firstChar,
				     lastChar);
	        ok = ChangeTypeOfElements (firstSel, lastSel, pSelDoc, typeNum, pSS);
		CloseHistorySequence (pSelDoc);
		}
	      if (!ok)
		/* ca n'a pas marche'. essaie les transformations de */
		/* type par patterns */
		{
		  elType.ElTypeNum = typeNum;
                  elType.ElSSchema = (SSchema) pSS;

                  if (TransformIntoFunction != NULL)
                     ok = TransformIntoFunction (elType,  (Document) IdentDocument (pSelDoc));
		}
	      /* si ca n'a pas marche' et si plusieurs elements sont
		 selectionne's, on essaie de transformer chaque element
		 selectionne' en un element du type demande' */
	      if (!ok)
		if (firstSel != lastSel)
		  {
		    /* store the command in the history */
		    OpenHistorySequence (pSelDoc, firstSel, lastSel, firstChar,
				         lastChar);
		    pEl = firstSel;
		    do
		      {
			/* essaie de transformer un element */
			ok = ChangeTypeOfElements (pEl, pEl, pSelDoc, typeNum,
			     pSS);
			if (!ok)
			   CancelLastEditFromHistory (pSelDoc);
			/* cherche l'element suivant de la selection */
			pEl = NextInSelection (pEl, lastSel);
		      }
		    while (pEl != NULL);
		    CloseHistorySequence (pSelDoc);
		  }
	    }
	  else
	    /* on ne fait rien */
	    return;
	}

      if (InsertionPoint)
	/* il y a un simple point d'insertion */
	{
	  origFirstChar = firstChar;
	  origLastChar = lastChar;
	  if (origFirstChar > 0 && origFirstChar == origLastChar)
	      origLastChar--;
	  /* verifie si l'element a creer porte l'exception NoCreate */
	  if (TypeHasException (ExcNoCreate, typeNum, pSS))
	    /* abandon */
	    return;
	  
	  if (elConst || empty)
	    {
	      selHead = TRUE;
	      selTail = FALSE;
	    }
	  else
	    {
	      /* la selection commence-t-elle en tete d'un element ? */
	      selHead = (firstSel == lastSel &&
			 firstSel->ElPrevious == NULL &&
			 lastSel->ElTerminal &&
			 ((lastSel->ElLeafType == LtText && firstChar <= 1) ||
			  (lastSel->ElLeafType == LtPicture && firstChar == 0) ||
			  lastSel->ElLeafType == LtGraphics ||
			  lastSel->ElLeafType == LtPolyLine ||
			  lastSel->ElLeafType == LtSymbol ));
	      /* la selection est-t-elle a la fin de la derniere feuille
		 de texte d'un element */
	      selTail = (firstSel == lastSel &&
			 lastSel->ElNext == NULL &&
			 lastSel->ElTerminal &&
			 ((lastSel->ElLeafType == LtText &&
			   firstChar > lastSel->ElTextLength) ||
			  (lastSel->ElLeafType == LtPicture &&
			   firstChar > 0) ||
			  lastSel->ElLeafType == LtGraphics ||
			  lastSel->ElLeafType == LtPolyLine ||
			  lastSel->ElLeafType == LtSymbol ));
	    }
	  
	  /* verifie si la selection est en fin ou debut de paragraphe */
	  if (selHead)
	    {
	      pEl = firstSel;
	      firstChar = 0;
	      createAfter = FALSE;
	    }
	  else if (selTail)
	    {
	      pEl = lastSel;
	      firstChar = 0;
	      createAfter = TRUE;
	    }
	  else
	    {
	      pEl = firstSel;
	      createAfter = FALSE;
	      if (lastSel->ElTerminal &&
		  (lastSel->ElLeafType == LtText || lastSel->ElLeafType == LtPicture) &&
		  firstChar > 1)
		createAfter = TRUE;
	    }
	  /* on verifie si on peut couper un element ascendant en deux et
	     creer le nouvel element entre les deux parties obtenues */
	  ok = CanInsertBySplitting (&pEl, firstChar, &splitElem,
				     &pSplitEl, &pElSplit, createAfter,
				     typeNum, pSS, pSelDoc);
	  if (ok)
	    if (empty)
	      if (!EmptyElement (pEl))
		empty = FALSE;

	  ancestorRule = 0;
	  if (!ok)
	    /* si l'element a creer apparait dans le schema de structure
	       comme un element de liste ou d'agregat, on essaie de creer
	       une telle liste ou un tel agregat */
	    {
	      rule = typeNum;
	      prevrule = 0;  prevprevrule = 0;
	      while (rule > 0 && !ok)
		{
		  /* on cherche d'abord une regle CsList */
		  ancestorRule = ListRuleOfElem (rule, pSS);
		  if (ancestorRule == 0)
		    /* Pas trouve' de regle Liste On cherche une regle 
		       Aggregate */
		    ancestorRule = AggregateRuleOfElem (rule, pSS);
		  if (ancestorRule == 0)
		    rule = 0;
		  else
		    {
		      ok = CanInsertBySplitting (&pEl, firstChar, &splitElem,
				    &pSplitEl, &pElSplit, createAfter,
				    ancestorRule, pSS, pSelDoc);
		      if (!ok && ancestorRule > 0)
			if (ancestorRule == prevrule ||
			    ancestorRule == prevprevrule)
			   rule = 0;
			else
			   {
			   prevprevrule = prevrule;
			   prevrule = rule;
			   rule = ancestorRule;
			   }
		    }
		}
	    }
	  
	  if (ok && pEl != NULL)
	    {
	      /* demande a l'application si on peut creer ce type d'element */
	      notifyEl.event = TteElemNew;
	      notifyEl.document = (Document) IdentDocument (pSelDoc);
	      notifyEl.element = (Element) (pEl->ElParent);
	      notifyEl.elementType.ElTypeNum = typeNum;
	      notifyEl.elementType.ElSSchema = (SSchema) pSS;
	      pF = pEl;
	      NSiblings = 0;
	      while (pF->ElPrevious != NULL)
		{
		  NSiblings++;
		  pF = pF->ElPrevious;
		}
	      if (createAfter)
		NSiblings++;
	      notifyEl.position = NSiblings;
	      if (CallEventType ((NotifyEvent *) (&notifyEl), TRUE))
		/* l'application refuse */
		pEl = NULL;
	    }

	  pNew = NULL;
	  if (ok && pEl != NULL)
	    {
	      /* After element */
	      OpenHistorySequence (pSelDoc, firstSel, lastSel,
				   origFirstChar, origLastChar);
	      ok = !CannotInsertNearElement (pEl, FALSE);
	      if (ok)
		{
		  done = FALSE;
		  if (splitElem)
		    /* coupe l'element en deux */
		    {
		      /* store the editing operation in the history */
		      AddEditOpInHistory (pSplitEl, pSelDoc, TRUE, TRUE);
		      ok = BreakElement (pSplitEl, pElSplit, firstChar, FALSE, FALSE);
		      if (!ok)
			  CancelLastEditFromHistory (pSelDoc);
		      else
			{
			  AddEditOpInHistory (pSplitEl->ElNext, pSelDoc, FALSE, TRUE);
			  createAfter = TRUE;
			  if (ancestorRule > 0)
			    {
			      pElem = pSplitEl;
			      do
				{
				  ok = AllowedSibling (pElem, pSelDoc, typeNum,
					       pSS, (ThotBool)(!createAfter), TRUE, FALSE);
				  if (ok)
				    {
				      pEl = pElem;
				      done = TRUE;
				    }
				  else if (pElem->ElTerminal)
				    pElem = NULL;
				  else
				    {
				      pElem = pElem->ElFirstChild;
				      while (pElem != NULL && pElem->ElNext != NULL)
					pElem = pElem->ElNext;
				    }
				}
			      while (!ok && pElem != NULL);
			    }
			}
		    }
		  if (!done)
		    if (ancestorRule > 0)
		      {
			pNew = NewSubtree (ancestorRule, pSS, pSelDoc,
					   pEl->ElAssocNum, FALSE, TRUE, TRUE, TRUE);
			elType.ElTypeNum = typeNum;
			elType.ElSSchema = (SSchema) pSS;
			TtaCreateDescent (IdentDocument (pSelDoc),
					  (Element) pNew, elType);
			ok = True;
		      }
		}
	      if (ok)
		{
		  /* annule la selection */
		  TtaClearViewSelections ();
		  if (!splitElem)
		    if (firstSel->ElTerminal &&
			firstSel->ElLeafType == LtText &&
			firstChar > 1 &&
			firstChar <= firstSel->ElTextLength && !splitElem)
		      {
		        /* store the editing operation in the history */
		        AddEditOpInHistory (firstSel, pSelDoc, TRUE, FALSE);
			pNextEl = firstSel->ElNext;
			SplitTextElement (firstSel, firstChar, pSelDoc, TRUE,
					  &pSecond);
			BuildAbsBoxSpliText (firstSel, pSecond, pNextEl, pSelDoc);
		      }
		  if (pNew == NULL)
		    pNew = NewSubtree (typeNum, pSS, pSelDoc, pEl->ElAssocNum,
				       TRUE, TRUE, TRUE, TRUE);
		  
		  /* Insertion du nouvel element */
		  if (createAfter)
		    {
		      pSibling = pEl->ElNext;
		      FwdSkipPageBreak (&pSibling);
		      InsertElementAfter (pEl, pNew);
		      AddEditOpInHistory (pNew, pSelDoc, FALSE, TRUE);
		      if (pSibling == NULL)
			/* l'element pEl n'est plus le dernier fils de
			   son pere */
			ChangeFirstLast (pEl, pSelDoc, FALSE, TRUE);
		    }
		  else
		    {
		      pSibling = pEl->ElPrevious;
		      InsertElementBefore (pEl, pNew);
		      if (empty)
			/* on doit detruire l'element vide devant lequel on
			   vient de creer un nouvel element */
			/* verifie si l'element a detruire porte l'exception
			   NoCut */
			if (TypeHasException (ExcNoCut, pEl->ElTypeNumber,
					      pEl->ElStructSchema))
			   empty = FALSE;
		        else
			  /* envoie l'evenement ElemDelete.Pre a
			     l'application */
			  if (SendEventSubTree (TteElemDelete, pSelDoc, pEl,
						TTE_STANDARD_DELETE_LAST_ITEM))
			    /* l'application refuse de detruire cet element */
			    empty = FALSE;
		      /* register the new element in the editing history */
		      AddEditOpInHistory (pNew, pSelDoc, FALSE, TRUE);       
		      deleted = FALSE;
		      if (empty)
			/* on detruit l'element vide devant lequel on
			   vient de creer un nouvel element */
			{
			/* register the empty element that will be deleted*/
			AddEditOpInHistory (pEl, pSelDoc, TRUE, FALSE);
			/* detruit les paves de l'element vide a detruire */
			DestroyAbsBoxes (pEl, pSelDoc, TRUE);
			AbstractImageUpdated (pSelDoc);
			/* prepare l'evenement ElemDelete.Post */
			notifyEl.event = TteElemDelete;
			notifyEl.document = (Document) IdentDocument (pSelDoc);
			notifyEl.element = (Element) (pEl->ElParent);
			notifyEl.elementType.ElTypeNum = pEl->ElTypeNumber;
			notifyEl.elementType.ElSSchema =
			                       (SSchema) (pEl->ElStructSchema);
			notifyEl.position = NSiblings + 1;
			notifyEl.info = 0;
			pSibling = NextElement (pEl);
			/* retire l'element de l'arbre abstrait */
			RemoveElement (pEl);
			UpdateNumbers (pSibling, pEl, pSelDoc, TRUE);
			RedisplayCopies (pEl, pSelDoc, TRUE);
			DeleteElement (&pEl, pSelDoc);
			/* envoie l'evenement ElemDelete.Post a l'application*/
			CallEventType ((NotifyEvent *) (&notifyEl), FALSE);
			deleted = TRUE;
			}
		      if (!deleted && pSibling == NULL)
			/* l'element pEl n'est plus le premier fils de
			   son pere */
			ChangeFirstLast (pEl, pSelDoc, TRUE, TRUE);
		    }
		  if (pNew != NULL)
		    {
		      /* traite les exclusions des elements crees */
		      RemoveExcludedElem (&pNew, pSelDoc);
		      /* traite les attributs requis des elements crees */
		      AttachMandatoryAttributes (pNew, pSelDoc);
		      if (pSelDoc->DocSSchema != NULL)
			/* le document n'a pas ete ferme' entre temps */
			{
			  /* traitement des exceptions */
			  CreationExceptions (pNew, pSelDoc);
			  /* cree les paves du nouvel element et */
			  /* met a jour ses voisins */
			  CreateAllAbsBoxesOfEl (pNew, pSelDoc);
			  /* Mise a jour des images abstraites */
			  AbstractImageUpdated (pSelDoc);
			  /* indique au Mediateur les modifications */
			  RedisplayDocViews (pSelDoc);
			  /* si on est dans un element copie' par inclusion,
			     on met a jour les copies de cet element. */
			  RedisplayCopies (pNew, pSelDoc, TRUE);
			  UpdateNumbers (NextElement (pNew), pNew, pSelDoc, TRUE);
			  /* Indiquer que le document est modifie' */
			  SetDocumentModified (pSelDoc, TRUE, 30);
			  /* envoie un evenement ElemNew.Post a l'application */
			  NotifySubTree (TteElemNew, pSelDoc, pNew, 0);
			  /* Replace la selection */
			  SelectElementWithEvent (pSelDoc, FirstLeaf (pNew), TRUE, TRUE);
			}
		    }
		}
	      CloseHistorySequence (pSelDoc);
	    }
	}
    }
}

/* ----------------------------------------------------------------------
   TtaCreateElement

   Create an element of a given type and insert it at the current position within
   a given document. The current position is defined by the current selection.
   If the current position is a single position (insertion point) the new element
   is simply inserted at that position. If one or several characters and/or
   elements are selected, the new element is created at that position and the
   selected characters/elements become the content of the new element, provided
   the  structure schema allows it.

   Parameters:
   elementType: type of the element to be created.
   document: the document for which the element is created.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaCreateElement (ElementType elementType, Document document)
#else  /* __STDC__ */
void                TtaCreateElement (elementType, document)
ElementType         elementType;
Document            document;

#endif /* __STDC__ */
{
   UserErrorCode = 0;
   if (elementType.ElSSchema == NULL)
      TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
      /* Checks the parameter document */
      TtaError (ERR_invalid_document_parameter);
   else if (LoadedDocument[document - 1] == NULL)
      TtaError (ERR_invalid_document_parameter);
   else if (elementType.ElTypeNum < 1 ||
   elementType.ElTypeNum > ((PtrSSchema) (elementType.ElSSchema))->SsNRules)
      /* Parameter document is ok */
      TtaError (ERR_invalid_element_type);
   else
      CreateNewElement (elementType.ElTypeNum,
			(PtrSSchema) (elementType.ElSSchema),
			LoadedDocument[document - 1], FALSE);
}


/*----------------------------------------------------------------------
   TtaSetTransformCallback permet de connecter une fonction de l'application
   au changement de type d'element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void  TtaSetTransformCallback (Func callbackFunc)
#else
void  TtaSetTransformCallback ()
Func callbackFunc;
#endif /* __STDC__ */
{
   TransformIntoFunction = callbackFunc;
}
 
/*----------------------------------------------------------------------
   AddEntrySurround ajoute, dans la table des elements a mettre    
   dans le menu Surround, le type d'element (typeNum, pSS),  
   s'il n'y est pas deja.                                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddEntrySurround (PtrSSchema pSS, int typeNum)
#else
static void         AddEntrySurround (pSS, typeNum)
PtrSSchema          pSS;
int                 typeNum;

#endif /* __STDC__ */
{
   int                 i;
   ThotBool            found;

   /* Ce type est-il deja dans la table ? */
   found = FALSE;
   i = 0;
   while (!found && i < NElSurround)
      if (typeNumSurround[i] == typeNum &&
	  pSSSurround[i]->SsCode == pSS->SsCode)
	 found = TRUE;
      else
	 i++;
   if (!found)
      /* ce type n'est pas deja dans la table */
      if (NElSurround < MAX_MENU - 1)
	 /* la table n'est pas pleine */
	{
	   /* on met le type dans la table */
	   typeNumSurround[NElSurround] = typeNum;
	   pSSSurround[NElSurround] = pSS;
	   NElSurround++;
	}
}


/*----------------------------------------------------------------------
   SurroundChoice Si un element du type (typeNum, pSS) peut englober 
   la suite des elements compris entre firstEl et lastEl   
   dans le document pDoc, on met ce type dans la table     
   qui servira a construire le menu Surround.              
   Si le type (typeNum, pSS) est un choix, ce n'est pas ce 
   type qu'on met dans la table, mais on essaie de mettre  
   toutes ses options.                                     
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SurroundChoice (int typeNum, PtrSSchema pSS, PtrElement firstEl,
				    PtrElement lastEl, PtrDocument pDoc)
#else
static void         SurroundChoice (typeNum, pSS, firstEl, lastEl, pDoc)
int                 typeNum;
PtrSSchema          pSS;
PtrElement          firstEl;
PtrElement          lastEl;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   Name                menuTitle;
   CHAR_T                choiceMenuBuf[MAX_TXT_LEN];
   int                 choiceTypeNum[MAX_MENU];
   PtrSSchema          choicePSSchema[MAX_MENU];
   int                 i, NChoiceItems;
   SRule              *pSRule;

   /* prend la regle de structure qui definit le type a traiter */
   pSRule = &(pSS->SsRule[typeNum - 1]);
   /* si c'est une regle de changement de schema, on traite le type */
   /* racine du schema en question */
   if (pSRule->SrConstruct == CsNatureSchema && pSRule->SrSSchemaNat != NULL)
     {
	pSS = pSRule->SrSSchemaNat;
	typeNum = pSS->SsRootElem;
	pSRule = &(pSS->SsRule[typeNum - 1]);
     }
   if (pSRule != NULL && pSRule->SrConstruct == CsChoice &&
       pSRule->SrNChoices != -1)
      /* il s'agit bien d'un choix et pas d'une regle NATURE */
     {
	/* compose la table des options de ce choix */
	NChoiceItems = MenuChoixElem (pSS, typeNum, firstEl, choiceMenuBuf,
				      menuTitle, pDoc);
	/* attention : la procedure est recursive ; il faut copier */
	/* cette table statique dans des variables dynamiques */
	for (i = 0; i < NChoiceItems; i++)
	  {
	     choiceTypeNum[i] = ChoiceMenuTypeNum[i];
	     choicePSSchema[i] = ChoiceMenuSSchema[i];
	  }
	/* essaie de mettre toutes les options du choix dans la liste */
	/* des types candidats englobants, en developpant les options */
	/* qui sont a leur tour des choix */
	for (i = 0; i < NChoiceItems; i++)
	   SurroundChoice (choiceTypeNum[i], choicePSSchema[i],
			   firstEl, lastEl, pDoc);
     }
   else
      /* verifie si ce type d'elements accepte les elements voulus */
      /* comme fils */
   if (CanSurround (typeNum, pSS, firstEl, lastEl, pDoc))
      /* verifie si ce type peut etre un fils du pere des */
      /* elements a englober */
      if (AllowedFirstChild (firstEl->ElParent, pDoc, typeNum, pSS, FALSE, FALSE))
	 /* on met ce type dans la table qui servira a produire le */
	 /* menu Surround */
	 AddEntrySurround (pSS, typeNum);
}

/*----------------------------------------------------------------------
   SurroundMenuInput : traite le retour du menu Surround.          
   entree est le numero de l'entree que l'utilisateur a    
   choisie dans ce menu.                                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SurroundMenuInput (int item)
#else
void                SurroundMenuInput (item)
int                 item;

#endif /* __STDC__ */
{
   PtrElement          firstEl, lastEl;
   PtrDocument         pDoc;
   int                 firstChar, lastChar;
   ThotBool	       done;

   /* recupere la selection courante */
   GetCurrentSelection (&pDoc, &firstEl, &lastEl, &firstChar, &lastChar);
   if (!firstEl)
      /* pas de selection, on ne fait rien */
      return;
   OpenHistorySequence (pDoc, firstEl, lastEl, firstChar, lastChar);
   done = DoSurround (firstEl, lastEl, firstChar, lastChar, pDoc,
		      typeNumSurround[item], pSSSurround[item]);
   CloseHistorySequence (pDoc);
}

/*----------------------------------------------------------------------
   BuildSurroundMenu cree dans le buffer menuBuffer le menu Surround  
   qui correspond a la selection courante. Retourne dans   
   NItems le nombre d'entrees de ce menu (0 si la		
   commande Surround est inapplicable).                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BuildSurroundMenu (STRING menuBuffer, int *NItems)
#else
static void         BuildSurroundMenu (menuBuffer, NItems)
STRING              menuBuffer;
int                *NItems;

#endif /* __STDC__ */
{
   PtrElement          firstSel, lastSel, pEl, pList, pAncest;
   PtrDocument         pDoc;
   PtrSSchema          pSS, pExtSS;
   SRule              *pSRule;
   PtrSSchema          choicePSSchema[MAX_MENU];
   int                 choiceTypeNum[MAX_MENU];
   int                 typeNum, i, NChoiceItems, firstChar, lastChar, menuInd,
                       len;
   Name                menuTitle, typeName;
   CHAR_T              choiceMenuBuf[MAX_TXT_LEN];

   if (ThotLocalActions[T_rsurround] == NULL)
     TteConnectAction (T_rsurround, SurroundMenuInput);
   /* prend la selection courante */
   GetCurrentSelection (&pDoc, &firstSel, &lastSel, &firstChar, &lastChar);
   if (pDoc != NULL && firstSel != NULL)
      /* il y a bien une selection */
     {
	pEl = firstSel;		/* pEl : le premier element selectionne' */
	/* on cherche le premier element CsList ascendant, mais sans */
	/* traverser d'agregat */
	pList = AncestorList (pEl);
	if (pList != NULL)
	   /* il y a bien un ascendant CsList */
	  {
	     /* on prend le type des elements de cette liste */
	     pSS = pList->ElStructSchema;
	     typeNum = pSS->SsRule[pList->ElTypeNumber - 1].SrListItem;
	     /* regle de structure definissant les elements de la liste */
	     pSRule = &(pSS->SsRule[typeNum - 1]);
	     /* on verifie que ces elements sont des choix, en sautant les */
	     /* identites et les natures */
	     while (pSRule != NULL && pSRule->SrConstruct != CsChoice)
	       {
		  if (pSRule->SrConstruct == CsIdentity)
		    {
		       typeNum = pSRule->SrIdentRule;
		       pSRule = &(pSS->SsRule[typeNum - 1]);
		    }
		  else if (pSRule->SrConstruct == CsNatureSchema)
		     if (pSRule->SrSSchemaNat != NULL)
		       {
			  pSS = pSRule->SrSSchemaNat;
			  typeNum = pSS->SsRootElem;
			  pSRule = &(pSS->SsRule[typeNum - 1]);
		       }
		     else
			pSRule = NULL;
		  else
		     pSRule = NULL;
	       }
	     if (pSRule != NULL && pSRule->SrConstruct == CsChoice)
		/* on ignore les regles NATURE */
		if (pSRule->SrNChoices != -1)
		   /* les composants de la liste sont bien des choix */
		  {
		     /* compose la liste des options du choix */
		     NChoiceItems = MenuChoixElem (pSS, typeNum, pEl,
						   choiceMenuBuf,
						   menuTitle, pDoc);
		     /* Attention a la recursivite': il faut recopier cette */
		     /* liste statique dans des variables dynamiques */
		     for (i = 0; i < NChoiceItems; i++)
		       {
			  choiceTypeNum[i] = ChoiceMenuTypeNum[i];
			  choicePSSchema[i] = ChoiceMenuSSchema[i];
		       }
		     /* essaie de mettre toutes les options du choix dans la */
		     /* liste des types candidats englobants, en developpant */
		     /* les options qui sont a leur tour des choix */
		     for (i = 0; i < NChoiceItems; i++)
			SurroundChoice (choiceTypeNum[i], choicePSSchema[i],
					firstSel, lastSel, pDoc);
		  }
	  }
	/* on cherche maintenant les inclusions definies par tous */
	/* les ascendants des elements selectionne's */
	pAncest = pEl->ElParent;
	while (pAncest != NULL)
	  {
	     /* regle de structuration de l'ascendant courant */
	     pSS = pAncest->ElStructSchema;
	     pSRule = &pSS->SsRule[pAncest->ElTypeNumber - 1];
	     /* on n'a pas encore traite' les extensions de schema */
	     pExtSS = NULL;
	     do
	       {
		  if (pSRule != NULL)
		     /* parcourt la liste des inclusions definies dans la regle */
		     for (i = 0; i < pSRule->SrNInclusions; i++)
		       {
			  if (!ExcludedType (pEl->ElParent,
					     pSRule->SrInclusion[i], pSS))
			     /* cette inclusion n'est pas une exclusion */
			     if (!TypeHasException (ExcNoCreate,
						    pSRule->SrInclusion[i],
						    pSS))
				/* essaie de mettre cette inclusion dans la
				   liste des types candidats englobants, en
				   developpant si c'est un choix */
				SurroundChoice (pSRule->SrInclusion[i], pSS,
						firstSel, lastSel, pDoc);
		       }
		  /* passe au schema d'extension suivant */
		  if (pExtSS == NULL)
		     pExtSS = (pDoc)->DocSSchema->SsNextExtens;
		  else
		     pExtSS = pExtSS->SsNextExtens;
		  if (pExtSS != NULL)
		    {
		       pSS = pExtSS;
		       pSRule = ExtensionRule (pAncest->ElStructSchema,
					     pAncest->ElTypeNumber, pExtSS);
		    }
	       }
	     while (pExtSS != NULL);
	     /* passe a l'ascendant */
	     pAncest = pAncest->ElParent;
	  }
	/* la table est maintenant complete */
	/* construit le menu a partir de la table */
	*NItems = 0;
	menuBuffer[0] = EOS;
	menuInd = 0;
	for (i = 0; i < NElSurround; i++)
	  {
	     GetExternalTypeName (pSSSurround[i], typeNumSurround[i], typeName);
	     len = ustrlen (typeName) + 2;
	     if (len + menuInd + 1 < MAX_TXT_LEN)
	       {
		  /* indique une nouvelle entree */
		  menuBuffer[menuInd] = TEXT('B');
		  ustrcpy (&(menuBuffer[menuInd + 1]), typeName);
		  menuInd += len;
		  (*NItems)++;
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   TtcSurround : traite la commande Surround de l'editeur.         
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcSurround (Document document, View view)
#else  /* __STDC__ */
void                TtcSurround (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrElement          firstSel, lastSel;
   PtrDocument         pSelDoc;
   int                 firstChar, lastChar, NItems;
   Name                title;
   CHAR_T              menuBuffer[MAX_TXT_LEN];
   ThotBool            protected;

   /* terminer une insertion eventuelle */
   CloseInsertion ();
   /* y-a-t'il une selection ? */
   if (!GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar,
			     &lastChar))
      /* non, message 'Selectionnez' */
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_SEL_EL);
   else if (pSelDoc->DocReadOnly)
      /* document en lecture seule */
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_DOC_FORBIDDEN);
   else
     {
	protected = FALSE;
	if (firstSel->ElTypeNumber == (CharString + 1) && firstChar > 1)
	   /* la selection commence a l'interieur d'une chaine de */
	   /* caracteres, on ne peut rien faire si la chaine est protegee */
	   protected = ElementIsReadOnly (firstSel);
	if (!protected)
	   if (lastSel->ElTypeNumber == (CharString + 1) &&
	       lastChar <= lastSel->ElTextLength)
	      /* la selection finit a l'interieur d'une chaine de caracteres,
	         on ne peut rien faire si la chaine est protegee */
	      protected = ElementIsReadOnly (lastSel);
	if (!protected)
	   /* on ne peut rien faire si le pere est protege' */
	   protected = ElementIsReadOnly (firstSel->ElParent);
	if (!protected)
	  {
	     /* construit le menu pour le choix de l'elment englobant a creer */
	     BuildSurroundMenu (menuBuffer, &NItems);
	     if (NItems > 0)
		/* le menu n'est pas vide */
	       {
		  ustrncpy (title, TtaGetMessage (LIB, TMSG_SURROUND), MAX_NAME_LENGTH);
		  /* cree le pop-up menu Surround */
		  TtaNewPopup (NumMenuSurround, 0, title, NItems, menuBuffer, NULL, 'L'); 
		  /* affiche le pop-up menu */
#         ifndef _WINDOWS
		  TtaShowDialogue (NumMenuSurround, FALSE);
		  /* attend la reponse de l'utilisateur */
		  TtaWaitShowDialogue ();
#         endif /* !_WINDOWS */
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   ChangeTypeMenuInput : traite le retour du menu "Changer en".      
   entree est le numero de l'entree qui a ete choisie par  
   l'utilisateur dans ce menu.                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ChangeTypeMenuInput (int entree)
#else
void                ChangeTypeMenuInput (entree)
int                 entree;

#endif /* __STDC__ */
{
   PtrElement          pEl, lastEl;
   PtrDocument         pDoc;
   int                 firstChar, lastChar;
   ThotBool            done;

   GetCurrentSelection (&pDoc, &pEl, &lastEl, &firstChar, &lastChar);
   if (pEl == NULL)
     return;
   /* store the command in the history */
   OpenHistorySequence (pDoc, pEl, lastEl, firstChar, lastChar);
   AddEditOpInHistory (pEl, pDoc, TRUE, TRUE);
   done = FALSE;
   switch (ChangeTypeMethod[entree])
     {
     case M_EQUIV :
       done = DoChangeType (pEl, pDoc, ChangeTypeTypeNum[entree],
			    ChangeTypeSSchema[entree]);
       break;
     case M_RESDYN :
       done = RestChangeType((Element)pEl,IdentDocument (pDoc),
			     ChangeTypeTypeNum[entree],
			     (SSchema)ChangeTypeSSchema[entree]);
       break;
     }
   if (!done)
      CancelLastEditFromHistory (pDoc);
   CloseHistorySequence (pDoc);
}

/*----------------------------------------------------------------------
   BuildChangeTypeMenu   cree dans le buffer menuBuffer le menu des 
   types d'elements a proposer a l'utilisateur lorsqu'il   
   veut changer le type de l'element pEl.                  
   Au retour, NItems contient le nombre d'entrees		
   qui constituent ce menu (eventuellemenmt 0, si l'element
   ne peut pas changer de type).                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void  BuildChangeTypeMenu (STRING menuBuffer, int *NItems, PtrElement pEl)
#else
static void  BuildChangeTypeMenu (menuBuffer, NItems, pEl)
STRING       menuBuffer;
int         *NItems;
PtrElement   pEl;

#endif /* __STDC__ */
{
   int                 i, menuInd, len;
   Name                typeName;

   /* on cree d'abord la table des types equivalents a celui de pEl */
   BuildChangeTypeTable (pEl);
   /* on construit le menu a partir de la table */
   *NItems = 0;
   menuBuffer[0] = EOS;
   menuInd = 0;
   for (i = 0; i < NChangeTypeItems; i++)
     {
       GetExternalTypeName (ChangeTypeSSchema[i], ChangeTypeTypeNum[i], typeName);
       len = ustrlen (typeName) + 2;
       if (len + menuInd + 1 < MAX_TXT_LEN)
	 {
	   menuBuffer[menuInd] = TEXT('B');
	   ustrcpy (&(menuBuffer[menuInd + 1]), typeName);
	   menuInd += len;
	   (*NItems)++;
	 }
     }
}

/*----------------------------------------------------------------------
   TtcChangeType : traite la commande "Changer en".                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcChangeType (Document document, View view)
#else  /* __STDC__ */
void                TtcChangeType (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   PtrElement          firstSel, lastSel;
   PtrDocument         pSelDoc;
   int                 firstChar, lastChar, NItems;
   CHAR_T              menuBuffer[MAX_TXT_LEN];
   Name                title;

   if (ThotLocalActions[T_rchangetype] == NULL)
     TteConnectAction (T_rchangetype, (Proc) ChangeTypeMenuInput);
   /* terminer une insertion eventuelle */
   CloseInsertion ();
   /* y-a-t'il une selection ? */
   if (!GetCurrentSelection (&pSelDoc, &firstSel, &lastSel, &firstChar,
			     &lastChar))
      /* non, message 'Selectionnez' */
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_SEL_EL);
   else if (pSelDoc->DocReadOnly)
      /* document en lecture seule */
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_RO_DOC_FORBIDDEN);
   else if (!ElementIsReadOnly (firstSel->ElParent))
      /* on ne peut rien faire si le pere est protege' */
      if (firstSel == lastSel)
	 /* on ne change qu'un element a la fois */
	 if (firstSel->ElStructSchema->SsRule[firstSel->ElTypeNumber - 1].SrConstruct != CsBasicElement)
	    /* on ne change pas les types de base */
	   {
	      /* construit le menu pour le choix du nouveau type de l'element */
	      BuildChangeTypeMenu (menuBuffer, &NItems, firstSel);
	      if (NItems > 0)
		 /* le menu n'est pas vide */
		{
		   ustrncpy (title, TtaGetMessage (LIB, TMSG_CHANGE_TYPE), MAX_NAME_LENGTH);
		   /* cree le pop-up menu Change Type */
		   TtaNewPopup (NumMenuChangeType, 0, title, NItems, menuBuffer, NULL, 'L');
		   /* affiche le pop-up menu */
#          ifndef _WINDOWS
		   TtaShowDialogue (NumMenuChangeType, FALSE);
		   /* attend la reponse de l'utilisateur */
		   TtaWaitShowDialogue ();
#          endif /* !_WINDOWS */
		}
	   }
}
