/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles attribute presentation
 *
 * Author: V. Quint (INRIA)
 *
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
   CreateInheritedAttrTable

   Allocate and initialize an InheritAttrTable for element pEl.
   This table indicates attributes that transmit presentation rules
   to element pEl.
   table[attr] = TRUE if the presentation schema contains in its
   ATTRIBUTES section a rule of the form

      AttrName(ElType): ...

   AttrName is the name of attribute attr (rank of attribute definition
   in structure schema) and ElType is the type of element pEl.
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
   InheritAttrTable   *table;
   PtrPSchema          pSchP;
   PtrHandlePSchema    pHd;

   pEl->ElTypeNumber = pEl->ElTypeNumber;
   if (pEl->ElStructSchema->SsPSchema != NULL)
     {
     /* table allocation and initialization */
     if ((table = (InheritAttrTable *) TtaGetMemory (sizeof (InheritAttrTable))) == NULL)
        /* memory exhausted */
        return;
     for (attr = 0; attr < MAX_ATTR_SSCHEMA; (*table)[attr++] = FALSE);

     pEl->ElStructSchema->SsPSchema->PsInheritedAttr[pEl->ElTypeNumber - 1] = table;
     /* for all attributes defined in the structure schema */
     for (attr = 0; attr < pEl->ElStructSchema->SsNAttributes; attr++)
       {
       /* check the main presentation schema and all its extensions */
       pSchP = pEl->ElStructSchema->SsPSchema;
       pHd = NULL;
       while (pSchP)
	 {
	   pAttrPR = pSchP->PsAttrPRule[attr];
	   if (pAttrPR != NULL)
	      /* check all presentation rules associated with that attribute */
	      for (rule = 0; rule < pSchP->PsNAttrPRule[attr]; rule++)
		{
		  if (pAttrPR->ApElemType == pEl->ElTypeNumber)
		     (*table)[attr] = TRUE;
		  pAttrPR = pAttrPR->ApNextAttrPres;
		}
	   /* next P schema */
	   if (pHd == NULL)
	      /* extension schemas have not been checked yet */
	      /* get the first extension schema */
	      pHd = pEl->ElStructSchema->SsFirstPSchemaExtens;
	   else
	      /* get the next extension schema */
	      pHd = pHd->HdNextPSchema;
	   if (pHd == NULL)
	     /* no more extension schemas. Stop */
	      pSchP = NULL;
	   else
	      pSchP = pHd->HdPSchema;
	 }
       }
     }
}


/*----------------------------------------------------------------------
  CreateComparAttrTable

  Allocate and initialize a ComparAttrTable for attribute pAttr.

  For each attribute, this table indicates the other attributes that compare
  to the attribute for applying some presentation rules.

  table[attr] = TRUE if the presentation schema contains in its
  ATTRIBUTES section a rule of the form

     AttrName > n : ...

  AttrName is the name of attribute attr (rank of attribute definition
  in structure schema).
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CreateComparAttrTable (PtrAttribute pAttr)

#else  /* __STDC__ */
void                CreateComparAttrTable (pAttr)
PtrAttribute        pAttr;

#endif /* __STDC__ */

{
   AttributePres      *pAttrPR;
   int                 attNum;
   ComparAttrTable    *table;
   int                 attr;
   int                 rule;
   int                 item;
   PresentSchema      *pPS;
   PtrPSchema          pSchP;
   PtrHandlePSchema    pHd;   

   /* table allocation and initialization */
   pPS = pAttr->AeAttrSSchema->SsPSchema;
   if (!pPS)
      return;
   if ((table = (ComparAttrTable *) TtaGetMemory (sizeof (ComparAttrTable))) == NULL)
      /* memory exhausted */
      return;
   for (attr = 0; attr < MAX_ATTR_SSCHEMA; (*table)[attr++] = FALSE);
   attNum = pAttr->AeAttrNum;
   pPS->PsComparAttr[attNum - 1] = table;
   /* scan all attributes defined in the structure schema */
   for (attr = 0; attr < pAttr->AeAttrSSchema->SsNAttributes; attr++)
      /* check only integer attributes */
      if (pAttr->AeAttrSSchema->SsAttribute[attr].AttrType == AtNumAttr)
         {
         /* check the main presentation schema and all its extensions */
         pSchP = pPS;
         pHd = NULL;
         while (pSchP)
	    {
	    if (pSchP->PsNAttrPRule[attr] > 0)
	       {
	       pAttrPR = pSchP->PsAttrPRule[attr];
	       /* check presentation rules associated with attribute attr */
	       for (rule = 0; rule < pSchP->PsNAttrPRule[attr]; rule++)
		  {
		  if (pAttrPR->ApElemType == 0)
		     /* no inheritance */
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
	    /* next P schema */
	    if (pHd == NULL)
	       /* extension schemas have not been checked yet */
	       /* get the first extension schema */
	       pHd = pAttr->AeAttrSSchema->SsFirstPSchemaExtens;
	    else
	       /* get the next extension schema */
	       pHd = pHd->HdNextPSchema;
	    if (pHd == NULL)
	      /* no more extension schemas. Stop */
	       pSchP = NULL;
	    else
	       pSchP = pHd->HdPSchema;
	    
	    }
        }
}


/*----------------------------------------------------------------------
  TransmitElementContent

  Takes the contents of element pEl and make it the value of the
  attribute named attrName for all documents included in document
  pDoc by the rule inclRule from the pSS structure schema.
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                TransmitElementContent (PtrElement pEl, PtrDocument pDoc, CHAR_T* attrName, int inclRule, PtrSSchema pSS)

#else  /* __STDC__ */
void                TransmitElementContent (pEl, pDoc, attrName, inclRule, pSS)
PtrElement          pEl;
PtrDocument         pDoc;
CHAR_T*             attrName;
int                 inclRule;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   PtrReferredDescr    pReferredD;
   PtrReference        pRef;
   PtrElement          pIncludedEl;
   PtrDocument         pIncludedDoc;
   PtrAttribute        pAttr;
   DocumentIdentifier  IncludedDocIdent;
   int                 att;
   TtAttribute        *AttrDef;
   PtrElement          pChild;
   ThotBool            found;
   int                 len;

   /* Search all documents of the type in question included in document pDoc */
   pReferredD = pDoc->DocReferredEl;
   /* skip the first descriptor (it's a fake descriptor) */
   if (pReferredD != NULL)
      pReferredD = pReferredD->ReNext;
   /* scan all referred element descriptors */
   while (pReferredD != NULL)
     {
	if (pReferredD->ReExternalRef)
	   /* this referred element is in another document */
	  {
	     pRef = pReferredD->ReFirstReference;
	     /* scan all references to that external element */
	     while (pRef != NULL)
	       {
		  /* consider only inclusion references */
		  if (pRef->RdTypeRef == RefInclusion)
		    {
		       /* get the root of the included document */
		       pIncludedEl = ReferredElement (pRef, &IncludedDocIdent,
						      &pIncludedDoc);
		       if (pIncludedEl != NULL)
			 {
			    /* the included document is loaded. Search the */
			    /* attribute in its structure schema */
			    att = 0;
			    found = FALSE;
			    while (att < pIncludedEl->ElStructSchema->SsNAttributes &&
				   !found)
			      {
				 AttrDef = &(pIncludedEl->ElStructSchema->SsAttribute[att++]);
				 if (AttrDef->AttrType == AtTextAttr)
				    /* that's a text attribute */
				    if (ustrcmp (AttrDef->AttrOrigName, attrName) == 0)
				       /* that's the rigth attribute */
				       found = TRUE;
			      }
			    if (found)
			      {
				 GetAttribute (&pAttr);
				 pAttr->AeAttrSSchema = pIncludedEl->ElStructSchema;
				 pAttr->AeAttrNum = att;
				 pAttr->AeAttrType = AtTextAttr;
				 GetTextBuffer (&pAttr->AeAttrText);
				 /* copy the contents of element pEl into */
				 /* the attribute */
				 pChild = pEl;
				 /* first, get the first text leaf of pEl */
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
				      /* copy the contents of that leaf */
				      CopyTextToText ((PtrTextBuffer) pChild->ElText->BuContent,
						   pAttr->AeAttrText, &len);
				      /* associate the attribute with the */
				      /* of the included document */
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
  ApplyTransmitRules

  If element pEl (belonging to document pDoc) is a reference that
  includes an external document, apply all Transmit rules that
  assign a value to attributes of the included document.
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
   PtrSSchema          pSS;
   TransmitElem       *pTransR;
   int                 entry;
   int                 rule;
   int                 srcNumType;
   PtrElement          pSrcEl;
   int                 counter;
   Counter            *pCounter;

   if (pEl != NULL)
      if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrRefImportedDoc)
	{
	   /* it's an inclusion of an external document */
	   /* search the presentation schema that applies to the element */
	   SearchPresSchema (pEl, &pPSch, &entry, &pSS);
	   /* does this schema contain a Transmit rule that transmits the */
	   /* contents of some element to documents of the type of the */
	   /* included document ? */
	   /* scans all Transmit rule in the schema */
	   for (rule = 0; rule < pPSch->PsNTransmElems; rule++)
	     {
		/* pTransR: pointer to a Transmit rule */
		pTransR = &(pPSch->PsTransmElem[rule]);
		if (pTransR->TeTargetDoc == entry)
		   /* this Transmit rule acts on a document of the type of */
		   /* the included document */
		  {
		     /* what element type does transmit its contents? */
		     for (srcNumType = 0; srcNumType < pSS->SsNRules &&
			  pPSch->PsElemTransmit[srcNumType] != rule + 1; srcNumType++);
		     if (pPSch->PsElemTransmit[srcNumType] == rule + 1)
		       {
			  /* elements of type srcNumType+1 transmit their */
			  /* content to the documents of interest */
			  /* Search an element of that type in the document */
			  pSrcEl = FwdSearchTypedElem (pDoc->DocRootElement,
						       srcNumType + 1, pSS);
			  if (pSrcEl != NULL)
			     /* apply the Transmit rule to the element found */
			     TransmitElementContent (pSrcEl, pDoc, pTransR->TeTargetAttr, pTransR->TeTargetDoc, pSS);
		       }
		  }
	     }
	   /* does the presentation schema contain counters that transmit */
	   /* their value to some attribute in the included document? */
	   /* scan all counters defined in the presentation schema */
	   for (counter = 0; counter < pPSch->PsNCounters; counter++)
	     {
		pCounter = &pPSch->PsCounter[counter];
		/* scan all Transmit rules for that counter */
		for (rule = 0; rule < pCounter->CnNTransmAttrs; rule++)
		   if (pCounter->CnTransmSSchemaAttr[rule] == entry)
		     {
			/* this Transmit rule transmit the counter value to */
			/* a document of the type of the included document */
			/* apply the Transmit rule */
			TransmitCounterVal (pEl, pDoc, pCounter->CnTransmAttr[rule], counter + 1, pPSch, pSS);
		     }
	     }
	}
}

/*----------------------------------------------------------------------
  RepApplyTransmitRules

  Apply all Transmit rules to all elements of the same type as
  pTransmEl that follow element pEl in document pDoc.
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
	   pEl = FwdSearchTypedElem (pEl, pTransmEl->ElTypeNumber,
				     pTransmEl->ElStructSchema);
	   ApplyTransmitRules (pEl, pDoc);
	}
}
