/*
 * Copyright (c) 1996 INRIA, All rights reserved
 */


/*
  gestion de l'heritage des attributs.

 */


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#include "memory_f.h"
#include "references_f.h"
#include "tree_f.h"
#include "attributes_f.h"
#include "content_f.h"
#include "presrules_f.h"
#include "changeabsbox_f.h"

/*----------------------------------------------------------------------
   CreateInheritedAttrTable alloue et remplit une table de type          
   InheritAttrTable. Un element de cette table indique pour  
   un element les attributs dont il herite des regles de   
   presentation. table[att] = TRUE si le schema de		
   presentation contient dans la section ATTRIBUTES des	
   		regles du type ATTRIBUTES : "att(El) : ...".		
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CreateInheritedAttrTable (PtrElement pEl)

#else  /* __STDC__ */
void                CreateInheritedAttrTable (pEl)
PtrElement          pEl;

#endif /* __STDC__ */

{
   int                 attr;
   int                 rule;
   AttributePres      *pAttrPR;
   InheritAttrTable     *table;

   pEl->ElTypeNumber = pEl->ElTypeNumber;
   /* table allocation and initialization */
   if ((table = (InheritAttrTable *) TtaGetMemory (sizeof (InheritAttrTable))) == NULL)
	/* memory exhausted */
	return;
   for (attr = 0; attr < MAX_ATTR_SSCHEMA; (*table)[attr++] = FALSE);
   if (pEl->ElStructSchema->SsPSchema != NULL)
     {
	pEl->ElStructSchema->SsPSchema->PsInheritedAttr[pEl->ElTypeNumber - 1] = table;
	/* for all attributes defined in the structure schema */
	for (attr = 0; attr < pEl->ElStructSchema->SsNAttributes; attr++)
	   {
	   pAttrPR = pEl->ElStructSchema->SsPSchema->PsAttrPRule[attr];
	   if (pAttrPR != NULL)
	      /* check all presentation rules associated with that attribute */
	      for (rule = 0; rule < pEl->ElStructSchema->SsPSchema->PsNAttrPRule[attr]; rule++)
		{
		  if (pAttrPR->ApElemType == pEl->ElTypeNumber)
		    (*table)[attr] = TRUE;
		  pAttrPR = pAttrPR->ApNextAttrPres;
		}
	   }
     }
}


/*----------------------------------------------------------------------
   Alloue et remplit une table de type ComparAttrTable.		
   	Un element de cette table indique pour chaque attribut les	
   	autres attributs qui se comparent a cet attribut pour		
   	l'application des regles de presentation a` cet attribut.	
   	table[i] = TRUE si, dans la section ATTRIBUTES, le schema de	
   	presentation contient des regles du type "GetAttributeOfElement > n : ...".	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CreateComparAttrTable (PtrAttribute pAttr)

#else  /* __STDC__ */
void                CreateComparAttrTable (pAttr)
PtrAttribute         pAttr;

#endif /* __STDC__ */

{
   AttributePres      *pAttrPR;
   int      attNum;
   ComparAttrTable   *table;
   int                 attr;
   int                 rule;
   int                 item;
   PresentSchema         *pPS;

   /* table allocation and initialization */
   pPS = pAttr->AeAttrSSchema->SsPSchema;
   if ((table = (ComparAttrTable *) TtaGetMemory (sizeof (ComparAttrTable))) == NULL)
	/* memory exhausted */
	return;
   if (pPS != NULL)
     {
	for (attr = 0; attr < MAX_ATTR_SSCHEMA; (*table)[attr++] = FALSE) ;
	attNum = pAttr->AeAttrNum;
	pPS->PsComparAttr[attNum - 1] = table;
	/* parcours l'ensemble des attributs */
	for (attr = 0; attr < pAttr->AeAttrSSchema->SsNAttributes; attr++)
	     /* check only integer attributes */
	     if (pAttr->AeAttrSSchema->SsAttribute[attr].AttrType == AtNumAttr)
		  if (pPS->PsNAttrPRule[attr] != 0)
		    {
		       /* check presentation rules associated with attribute attr */
		       pAttrPR = pPS->PsAttrPRule[attr];
		       for (rule = 0; rule < pPS->PsNAttrPRule[attr]; rule++)
			  {
			  if (pAttrPR->ApElemType == 0)
			     /* not inheritance */
			    {
			       for (item = 0; item < pAttrPR->ApNCases; item++)
				 {
				    if (pAttrPR->ApCase[item].CaComparType == ComparAttr
					&& (pAttrPR->ApCase[item].CaLowerBound == attNum
					    || pAttrPR->ApCase[item].CaUpperBound == attNum))
				      {
					 (*table)[attr] = TRUE;
					 break;
				      }
				 }
			       break;
			    }
			  pAttrPR = pAttrPR->ApNextAttrPres;
			  }
		    }
     }
}


/*----------------------------------------------------------------------
   TransmitElementContent transmet la valeur de l'element pEl a`		
   l'attribut de nom attrName des documents inclus dans le document
   pDoc par la regle inclRule du schema de structure pSchS	
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TransmitElementContent (PtrElement pEl, PtrDocument pDoc, Name attrName, int inclRule, PtrSSchema pSS)

#else  /* __STDC__ */
void                TransmitElementContent (pEl, pDoc, attrName, inclRule, pSS)
PtrElement          pEl;
PtrDocument         pDoc;
Name                 attrName;
int                 inclRule;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   PtrReferredDescr    pReferredD;
   PtrReference        pRef;
   PtrElement          pIncludedEl;
   PtrDocument         pIncludedDoc;
   PtrAttribute         pAttr;
   DocumentIdentifier     IncludedDocIdent;
   int                 att;
   TtAttribute           *AttrDef;
   PtrElement          pChild;
   boolean             found;
   int                 len;

   /* Cherche tous les documents du type voulu inclus dans pDoc */
   pReferredD = pDoc->DocReferredEl;
   /* saute le premier descripteur (bidon) */
   if (pReferredD != NULL)
      pReferredD = pReferredD->ReNext;
   /* parcourt la chaine des descripteurs d'elements references */
   while (pReferredD != NULL)
     {
	if (pReferredD->ReExternalRef)
	   /* c'est un descripteur d'element reference' externe */
	  {
	     pRef = pReferredD->ReFirstReference;
	     /* parcourt les references a cet element externe */
	     while (pRef != NULL)
	       {
		  /* on ne s'interesse qu'aux inclusions */
		  if (pRef->RdTypeRef == RefInclusion)
		       {
			  /* l'element qui reference est du type cherche' */
			  /* accede au document inclus (a sa racine) */
			  pIncludedEl = ReferredElement (pRef, &IncludedDocIdent, &pIncludedDoc);
			  if (pIncludedEl != NULL)
			    {
			       /* le document inclus est charge', cherche */
			       /* l'attribut dans son schema de structure */
			       att = 0;
			       found = FALSE;
			       while (att < pIncludedEl->ElStructSchema->SsNAttributes &&
				      !found)
				 {
				    AttrDef = &(pIncludedEl->ElStructSchema->SsAttribute[att++]);
				    if (AttrDef->AttrType == AtTextAttr)
				       /* c'est un attribut textuel */
				       if (strcmp (AttrDef->AttrOrigName, attrName) == 0)
					  /* il a le nom cherche' */
					  found = TRUE;
				 }
			       if (found)
				 {
				    /* l'attribut est bien defini dans le schema */
				    /* de structure du document inclus */
				    GetAttr (&pAttr);
				    pAttr->AeAttrSSchema = pIncludedEl->ElStructSchema;
				    pAttr->AeAttrNum = att;
				    pAttr->AeAttrType = AtTextAttr;
				    GetTextBuffer (&pAttr->AeAttrText);
				    /* copie le texte de l'element pEl dans l'attribut */
				    pChild = pEl;
				    /* cherche la premiere feuille de texte */
				    found = FALSE;
				    while (pChild != NULL && !found)
				       if (pChild->ElTerminal)
					 {
					    if (pChild->ElLeafType == LtText)
					       found = TRUE;
					 }
				       else
					  pChild = pChild->ElFirstChild;
				    if (found)
				      {
					 /* copie le contenu de la feuille */
					 CopyTextToText ((PtrTextBuffer) pChild->ElText->BuContent,
						  pAttr->AeAttrText, &len);
					 /* met l'attribut sur la racine du document */
					 AttachAttrWithValue (pIncludedEl, pIncludedDoc, pAttr);
				      }
				    DeleteAttribute (NULL, pAttr);
				 }
			    }
		       }
		  pRef = pRef->RdNext;
	       }
	  }
	pReferredD = pReferredD->ReNext;
     }
}


/*----------------------------------------------------------------------
   	Si l'element pEl (appartenant au document pDoc) est une		
   	reference d'inclusion d'un document externe, applique toutes	
   	les regles de transmission qui donnent leurs valeurs aux	
   	attributs du document inclus.					
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ApplyTransmitRules (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                ApplyTransmitRules (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrPSchema          pPSch;
   PtrSSchema        pSS;
   TransmitElem          *pTransR;
   int                 entry;
   int                 rule;
   int                 srcNumType;
   PtrElement          pSrcEl;
   int                 counter;
   Counter           *pCounter;

   if (pEl != NULL)
      if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrRefImportedDoc)
	{
	   /* c'est un lien d'inclusion d'un document externe */
	   /* cherche le schema de presentation qui s'applique a` l'element */
	   SearchPresSchema (pEl, &pPSch, &entry, &pSS);
	   /* ce schema contient-il une regle Transmit qui transmet le */
	   /* contenu d'un type d'element a des documents de la classe du */
	   /* document inclus ? */
	   /* examine toutes les regles Transmit du schema */
	   for (rule = 0; rule < pPSch->PsNTransmElems; rule++)
	     {
		/* pTransR: pointeur sur une regle Transmit */
		pTransR = &(pPSch->PsTransmElem[rule]);
		/*if (pTransR->TeTargetDoc == pEl->ElTypeNumber) */
		if (pTransR->TeTargetDoc == entry)
		   /* cette regle Transmit transmet a un document de la */
		   /* classe du document inclus */
		  {
		     /* quel est le type d'element qui transmet sa valeur ? */
		     for (srcNumType = 0; srcNumType < pSS->SsNRules &&
			  pPSch->PsElemTransmit[srcNumType] != rule+1; srcNumType++) ;
		     if (pPSch->PsElemTransmit[srcNumType] == rule+1)
		       {
			  /* les elements de type srcNumType+1 transmettent leur valeur */
			  /* aux documents qui nous interessent. On cherche un */
			  /* element de ce type a partir de la racine du document */
			  pSrcEl = FwdSearchTypedElem (pDoc->DocRootElement, srcNumType+1, pSS);
			  if (pSrcEl != NULL)
			     /* applique la regle Transmit a l'element trouve' */
			     TransmitElementContent (pSrcEl, pDoc, pTransR->TeTargetAttr,
					      pTransR->TeTargetDoc, pSS);
		       }
		  }
	     }
	   /* le schema de presentation contient-il des compteurs qui */
	   /* transmettent leur valeur a des attributs du document inclus ? */
	   /* examine tous les compteurs definis dans le schema */
	   for (counter = 0; counter < pPSch->PsNCounters; counter++)
	     {
		pCounter = &pPSch->PsCounter[counter];
		/* examine toutes les regles Transmit du compteur courant */
		for (rule = 0; rule < pCounter->CnNTransmAttrs; rule++)
		   if (pCounter->CnTransmSSchemaAttr[rule] == entry)
		     {
			/* cette regle Transmit transmet le compteur a un document */
			/* de la classe du document inclus, on applique la regle */
			TransmetValCompt (pEl, pDoc, pCounter->CnTransmAttr[rule],
					  counter+1, pPSch, pSS);
		     }
	     }
	}
}

/*----------------------------------------------------------------------
   									
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RepApplyTransmitRules (PtrElement pTransmEl, PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
void                RepApplyTransmitRules (pTransmEl, pEl, pDoc)
PtrElement          pTransmEl;
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   if (pTransmEl != NULL)
      while (pEl != NULL)
	{
	   pEl = FwdSearchTypedElem (pEl, pTransmEl->ElTypeNumber, pTransmEl->ElStructSchema);
	   if (pEl != NULL)
	      if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrRefImportedDoc)
		ApplyTransmitRules (pEl, pDoc);
	}
}
