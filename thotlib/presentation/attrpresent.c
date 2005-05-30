/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module handles attribute presentation
 *
 * Author: V. Quint (INRIA)
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"

#include "attributes_f.h"
#include "changeabsbox_f.h"
#include "content_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "references_f.h"
#include "schemas_f.h"
#include "tree_f.h"

/*----------------------------------------------------------------------
   CreateInheritedAttrTable

   Allocate and initialize an InheritAttrTable for elements of type
   (typeNum, pSS) in presentation schema pSP.
   This table indicates attributes that transmit presentation rules
   to elements of that type.
   table[attr] != '\0' if the presentation schema contains in its
   ATTRIBUTES section a rule of the form

      AttrName(ElType): ...
  ----------------------------------------------------------------------*/
void  CreateInheritedAttrTable (int typeNum, PtrSSchema pSS, PtrPSchema pPS,
				PtrDocument pDoc)
{
  int                 attr;
  int                 rule;
  AttributePres      *pAttrPR;
  InheritAttrTable   *table;

  if (pSS != NULL && pPS != NULL &&
      pPS->PsInheritedAttr->ElInherit[typeNum - 1] == NULL)
    {
      /* table allocation and initialization */
      if ((table = (InheritAttrTable*) TtaGetMemory (pSS->SsNAttributes * sizeof (char))) == NULL)
        /* memory exhausted */
        return;
      pPS->PsInheritedAttr->ElInherit[typeNum - 1] = table;
      /* for all attributes defined in the structure schema */
      for (attr = 0; attr < pSS->SsNAttributes; attr++)
	{
	  (*table)[attr] = '\0';  /* no inheritance by default */
	  pAttrPR = pPS->PsAttrPRule->AttrPres[attr];
	  if (pAttrPR != NULL)
	    /* check all presentation rules associated with that attr */
	    for (rule = 0; rule < pPS->PsNAttrPRule->Num[attr]; rule++)
	      {
		if (pAttrPR->ApElemType == typeNum)
		  {
		    if (pAttrPR->ApElemInherits)
		      /* the element inherits some presentation properties
			 from its ancestors or from itself */
		      (*table)[attr] = 'S';
		    else
		      /* the element inherits some presentation properties
			 from its ancestors, but not itself */
		      (*table)[attr] = 'H';
		  }
		pAttrPR = pAttrPR->ApNextAttrPres;
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
void CreateComparAttrTable (PtrAttribute pAttr, PtrDocument pDoc,
			    PtrElement pEl)
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
  pPS = PresentationSchema (pAttr->AeAttrSSchema, pDoc);
  if (!pPS)
    return;
  if ((table = (ComparAttrTable *) TtaGetMemory (pAttr->AeAttrSSchema->SsNAttributes * sizeof (ThotBool))) == NULL)
    /* memory exhausted */
    return;
  attNum = pAttr->AeAttrNum;
  pPS->PsComparAttr->CATable[attNum - 1] = table;
  /* scan all attributes defined in the structure schema */
  for (attr = 0; attr < pAttr->AeAttrSSchema->SsNAttributes; attr++)
    {
    (*table)[attr] = FALSE;
    /* check only integer attributes */
    if (pAttr->AeAttrSSchema->SsAttribute->TtAttr[attr]->AttrType == AtNumAttr)
      {
	/* check the main presentation schema and all its extensions */
	pSchP = pPS;
	pHd = NULL;
	while (pSchP)
	  {
	    if (pSchP->PsNAttrPRule->Num[attr] > 0)
	      {
		pAttrPR = pSchP->PsAttrPRule->AttrPres[attr];
		/* check presentation rules associated with attribute attr */
		for (rule = 0; rule < pSchP->PsNAttrPRule->Num[attr]; rule++)
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
	      pHd = FirstPSchemaExtension (pAttr->AeAttrSSchema, pDoc, pEl);
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
  ApplyTransmitRules

  If element pEl (belonging to document pDoc) is a reference that
  includes an external document, apply all Transmit rules that
  assign a value to attributes of the included document.
  ----------------------------------------------------------------------*/
void ApplyTransmitRules (PtrElement pEl, PtrDocument pDoc)
{
  PtrPSchema          pPSch;
  PtrSSchema          pSS;
  int                 entry;
  int                 rule;
  int                 counter;
  Counter            *pCounter;

  if (pEl != NULL)
    if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrRefImportedDoc)
      {
	/* it's an inclusion of an external document */
	/* search the presentation schema that applies to the element */
	SearchPresSchema (pEl, &pPSch, &entry, &pSS, pDoc);
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
void  RepApplyTransmitRules (PtrElement pTransmEl, PtrElement pEl,
			     PtrDocument pDoc)
{
  if (pTransmEl != NULL)
    while (pEl != NULL)
      {
	pEl = FwdSearchTypedElem (pEl, pTransmEl->ElTypeNumber,
				  pTransmEl->ElStructSchema, NULL);
	ApplyTransmitRules (pEl, pDoc);
      }
}
