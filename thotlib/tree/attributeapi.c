/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 *
 * Author: V. Quint (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "attribute.h"
#include "tree.h"
#include "typecorr.h"
#include "appdialogue.h"

#undef THOT_EXPORT
#define THOT_EXPORT
#include "edit_tv.h"
#include "select_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "applicationapi_f.h"
#include "attributeapi_f.h"
#include "attributes_f.h"
#include "callback_f.h"
#include "content_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "draw_f.h"
#include "memory_f.h"
#include "references_f.h"
#include "structcreation_f.h"
#include "structschema_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "viewapi_f.h"

static Name         bufferName;


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
#ifndef NODISPLAY
   PtrElement          pRefEl;
   int                 len;
#endif /* NODISPLAY */
   PtrAttribute        pAttr, pA;
   PtrReference        pRef;
   ThotBool            found;
   ThotBool            MandatoryAttrOK;
   NotifyAttribute     notifyAttr;
   int                 i, att;

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
#ifndef NODISPLAY
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
						 CopyStringToText (TEXT(" "), pAttr->AeAttrText, &len);
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
#endif /* NODISPLAY */
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

/* ----------------------------------------------------------------------
   TtaNewAttribute

   Creates an attribute that will be attached to an element.

   Parameter:
   attributeType: type of the attribute to be created.

   Return value:
   the attribute that has been created.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
Attribute           TtaNewAttribute (AttributeType attributeType)
#else  /* __STDC__ */
Attribute           TtaNewAttribute (attributeType)
AttributeType       attributeType;
#endif /* __STDC__ */

{
   PtrAttribute        pAttr;

   UserErrorCode = 0;
   pAttr = NULL;
   if (attributeType.AttrSSchema == NULL)
     TtaError (ERR_invalid_attribute_type);
   else if (attributeType.AttrTypeNum < 1 ||
	    attributeType.AttrTypeNum > ((PtrSSchema) (attributeType.AttrSSchema))->SsNAttributes)
     TtaError (ERR_invalid_attribute_type);
   else
     {
	GetAttribute (&pAttr);
	pAttr->AeNext = NULL;
	pAttr->AeAttrSSchema = (PtrSSchema) (attributeType.AttrSSchema);
	pAttr->AeAttrNum = attributeType.AttrTypeNum;
	pAttr->AeDefAttr = FALSE;
	pAttr->AeAttrType = pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrType;
	switch (pAttr->AeAttrType)
	      {
		 case AtEnumAttr:
		 case AtNumAttr:
		    pAttr->AeAttrValue = 0;
		    break;
		 case AtTextAttr:
		    pAttr->AeAttrText = NULL;
		    break;
		 case AtReferenceAttr:
		    pAttr->AeAttrReference = NULL;
		    break;
	      }
     }
   return ((Attribute) pAttr);
}

/* ----------------------------------------------------------------------
   TtaAttachAttribute

   Attaches an attribute to an element.

   Parameters:
   element: the element to which the attribute has to be attached.
   attribute: the attribute to be attached.
   document: the document to which the element belongs.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaAttachAttribute (Element element, Attribute attribute, Document document)
#else  /* __STDC__ */
void                TtaAttachAttribute (element, attribute, document)
Element             element;
Attribute           attribute;
Document            document;
#endif /* __STDC__ */
{
   PtrAttribute        pAttr;
   PtrDocument         pDoc;
   ThotBool            obligatory;

   UserErrorCode = 0;
   if (element == NULL || attribute == NULL || ((PtrElement) element)->ElStructSchema == NULL)
     TtaError (ERR_invalid_parameter);
   else if (document < 1 || document > MAX_DOCUMENTS)
     TtaError (ERR_invalid_document_parameter);
   else
     {
       pDoc = LoadedDocument[document - 1];
       if (pDoc == NULL)
	 TtaError (ERR_invalid_document_parameter);
       else if (AttributeValue ((PtrElement) element, (PtrAttribute) attribute) != NULL)
	 /* parameter document is correct */
	 /* has the element an attribute of the same type ? */
	 /* yes, error */ 
	 TtaError (ERR_duplicate_attribute);
       else if ((pDoc)->DocCheckingMode & STR_CHECK_MASK
		&& !CanAssociateAttr ((PtrElement) element, NULL, 
				      (PtrAttribute) attribute, &obligatory))
	 /* can wa apply the attribute to the element ? */
	 /* no, error */
	 TtaError (ERR_attribute_element_mismatch);
       else
	 {
#ifndef NODISPLAY
	   UndisplayInheritedAttributes ((PtrElement) element, (PtrAttribute) attribute, document, FALSE);
#endif
	   if (((PtrElement) element)->ElFirstAttr == NULL)
	     ((PtrElement) element)->ElFirstAttr = (PtrAttribute) attribute;
	   else
	     {
	       pAttr = ((PtrElement) element)->ElFirstAttr;
	       while (pAttr->AeNext != NULL)
		 pAttr = pAttr->AeNext;
	       pAttr->AeNext = (PtrAttribute) attribute;
	     }
	   /* update the menu attributes */
	   if (pDoc == SelectedDocument &&
	       (PtrElement) element == FirstSelectedElement)
	     if (ThotLocalActions[T_chattr] != NULL)
	       (*ThotLocalActions[T_chattr]) (pDoc);
	   
	   pAttr = (PtrAttribute) attribute;
	   pAttr->AeNext = NULL;
	   pAttr->AeDefAttr = FALSE;
	   if (pAttr->AeAttrType == AtReferenceAttr)
	     if (pAttr->AeAttrReference != NULL)
	       pAttr->AeAttrReference->RdElement = (PtrElement) element;
	   /* Special processing when adding an attribute to an element of a Draw object */
	   DrawAddAttr (&pAttr, (PtrElement) element);
#ifndef NODISPLAY
	   DisplayAttribute ((PtrElement) element, pAttr, document);
#endif
	 }
     }
}

/*----------------------------------------------------------------------
  AttrOfElement verifies that the attribute belongs to the element
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     AttrOfElement (Attribute attribute, Element element)
#else  /* __STDC__ */
static ThotBool     AttrOfElement (attribute, element)
Attribute           attribute;
Element             element;
#endif /* __STDC__ */
{
   PtrAttribute        pAttr;
   ThotBool            ok;

   if (element == NULL)
      ok = TRUE;
   else
     {
	pAttr = ((PtrElement) element)->ElFirstAttr;
	ok = FALSE;
	while (pAttr != NULL)
	   if (pAttr == (PtrAttribute) attribute)
	     {
		ok = TRUE;
		pAttr = NULL;
	     }
	   else
	      pAttr = pAttr->AeNext;
	if (!ok)
	   TtaError (ERR_attribute_element_mismatch);
     }
   return ok;
}

/* ----------------------------------------------------------------------
   TtaRemoveAttribute

   Removes an attribute from an element and release that attribute.

   Parameters:
   element: the element with which the attribute is associated.
   attribute: the attribute to be removed.
   document: the document to which the element belongs.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaRemoveAttribute (Element element, Attribute attribute, Document document)
#else  /* __STDC__ */
void                TtaRemoveAttribute (element, attribute, document)
Element             element;
Attribute           attribute;
Document            document;
#endif /* __STDC__ */
{
  PtrAttribute        pAttr;
  ThotBool            found;
  ThotBool            mandatory;

  UserErrorCode = 0;
  if (element == NULL || attribute == NULL)
    TtaError (ERR_invalid_parameter);
  else if (((PtrAttribute) attribute)->AeAttrSSchema == NULL ||
	   ((PtrElement) element)->ElStructSchema == NULL)
    TtaError (ERR_invalid_parameter);
  else
    {
      pAttr = ((PtrElement) element)->ElFirstAttr;
      found = FALSE;
      while (pAttr != NULL && !found)
	{
	  if (pAttr->AeAttrSSchema->SsCode == ((PtrAttribute) attribute)->AeAttrSSchema->SsCode)
	    if (pAttr->AeAttrNum == ((PtrAttribute) attribute)->AeAttrNum)
	      found = TRUE;
	  if (!found)
	    pAttr = pAttr->AeNext;
	}
      if (!found)
	TtaError (ERR_attribute_element_mismatch);
      else
	{
	  (void) CanAssociateAttr ((PtrElement) element, pAttr, pAttr, &mandatory);
	  if (!mandatory)
	    /* We prohibit to suppress the attbibute language of an element */
	    /* which is the root of an abstract tree */
	    if (((PtrElement) element)->ElParent == NULL)
	      if (pAttr->AeAttrNum == 1)
		mandatory = TRUE;
	  if (mandatory &&
	      (LoadedDocument[document - 1])->DocCheckingMode & STR_CHECK_MASK)
	    /* The attribute is required for this kind of element */
	    TtaError (ERR_mandatory_attribute);
	  else
	    {
	      RemoveAttribute ((PtrElement) element, pAttr);
#ifndef NODISPLAY
	      UndisplayInheritedAttributes ((PtrElement) element, pAttr, document, TRUE);
#endif
	      /* Special processig to suppress an attribute of an element of type object Draw */
	      DrawSupprAttr (pAttr, (PtrElement) element);
#ifndef NODISPLAY
	      UndisplayAttribute ((PtrElement) element, (PtrAttribute) attribute, document);
#endif
	      DeleteAttribute ((PtrElement) element, pAttr);
	    }
	}
    }
}

/* ----------------------------------------------------------------------
   TtaSetAttributeValue

   Changes the value of an attribute of type integer or enumerate.

   Parameters:
   attribute: the attribute to be modified.
   value: new value of the attribute.
   element: the element with which the attribute is associated,
   NULL if the attribute is not yet associated with an element.
   document: the document to which the element belongs.
   Must be 0 if element is NULL.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaSetAttributeValue (Attribute attribute, int value, Element element, Document document)
#else  /* __STDC__ */
void                TtaSetAttributeValue (attribute, value, element, document)
Attribute           attribute;
int                 value;
Element             element;
Document            document;
#endif /* __STDC__ */
{
  PtrAttribute        pAttr;
  ThotBool            ok;

  UserErrorCode = 0;
  pAttr = (PtrAttribute) attribute;
  ok = FALSE;
  if (attribute == NULL)
    TtaError (ERR_invalid_parameter);
  else if (pAttr->AeAttrType != AtEnumAttr && pAttr->AeAttrType != AtNumAttr)
    TtaError (ERR_invalid_attribute_value);
  else if (AttrOfElement (attribute, element))
    {
      if (pAttr->AeAttrType == AtNumAttr)
	if (abs (value) > 65535)
	  /* the pivot form represents integers coded on two bytes */
	  TtaError (ERR_invalid_attribute_value);
	else
	  ok = TRUE;
      else if (pAttr->AeAttrSSchema == NULL)
	TtaError (ERR_invalid_attribute_type);
      else if (value < 1)
	TtaError (ERR_invalid_attribute_value);
      else if (value > pAttr->AeAttrSSchema->SsAttribute[pAttr->AeAttrNum - 1].AttrNEnumValues)
	TtaError (ERR_invalid_attribute_value);
      else
	ok = TRUE;
      if (ok)
	{
#ifndef NODISPLAY
	  if (element != NULL)
	    UndisplayInheritedAttributes ((PtrElement) element, pAttr, document, FALSE);
#endif
	  pAttr->AeAttrValue = value;
#ifndef NODISPLAY
	  if (element != NULL)
	    DisplayAttribute ((PtrElement) element, pAttr, document);
#endif
	}
    }
}

/* ----------------------------------------------------------------------
   TtaSetAttributeText

   Changes the value of an attribute of type text.

   Parameters:
   attribute: the attribute to be modified.
   buffer: character string representing the new value of the attribute.
   element: the element with which the attribute is associated,
   NULL if the attribute is not yet associated with an element.
   document: the document to which the element belongs.
   Must be 0 if element is NULL.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaSetAttributeText (Attribute attribute, STRING buffer, Element element, Document document)
#else  /* __STDC__ */
void                TtaSetAttributeText (attribute, buffer, element, document)
Attribute           attribute;
STRING              buffer;
Element             element;
Document            document;
#endif /* __STDC__ */
{
   int                 lg;
   PtrAttribute        pAttr;
   Language	       lang;

   UserErrorCode = 0;
   pAttr = (PtrAttribute) attribute;
   if (pAttr == NULL)
      TtaError (ERR_invalid_parameter);
   else if (pAttr->AeAttrType != AtTextAttr)
      TtaError (ERR_invalid_attribute_type);
   else if (AttrOfElement (attribute, element))
     {
#ifndef NODISPLAY
	if (element != NULL)
	   UndisplayInheritedAttributes ((PtrElement) element, pAttr, document, FALSE);
#endif
	if (pAttr->AeAttrText == NULL)
	   GetTextBuffer (&pAttr->AeAttrText);
	else
	   ClearText (pAttr->AeAttrText);
	/* Sets the new value */
	CopyStringToText (buffer, pAttr->AeAttrText, &lg);
	if (pAttr->AeAttrNum == 1)
	  /* language attribute */
	  {
	     lang = TtaGetLanguageIdFromName (buffer);
#ifdef NODISPLAY
             ChangeLanguageLeaves((PtrElement) element, lang);
#else
	     ChangeLanguage (LoadedDocument[document - 1],
			     (PtrElement) element, lang, FALSE);
#endif
	  }
#ifndef NODISPLAY
	if (element != NULL)
	   DisplayAttribute ((PtrElement) element, pAttr, document);
#endif
     }
}

/* ----------------------------------------------------------------------
   TtaNextAttribute

   Returns the first attribute associated with a given element (if attribute
   is NULL) or the attribute that follows a given attribute of a given element.

   Parameters:
   element: the element of interest.
   attribute: an attribute of that element, or NULL if the
   first attribute is asked.

   Return parameter:
   attribute: the next attribute, or NULL if attribute
   is the last attribute of the element.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaNextAttribute (Element element, Attribute * attribute)
#else  /* __STDC__ */
void                TtaNextAttribute (element, attribute)
Element             element;
Attribute          *attribute;

#endif /* __STDC__ */
{
   PtrAttribute        nextAttribute;

   UserErrorCode = 0;
   nextAttribute = NULL;
   if (element == NULL)
      TtaError (ERR_invalid_parameter);
   else if (*attribute == NULL)
      nextAttribute = ((PtrElement) element)->ElFirstAttr;
   else
      nextAttribute = ((PtrAttribute) (*attribute))->AeNext;
   *attribute = (Attribute) nextAttribute;
}

/* ----------------------------------------------------------------------
   TtaGiveAttributeType

   Returns the type of a given attribute.

   Parameter:
   attribute: the attribute of interest.

   Return parameters:
   attributeType: type of the attribute.
   attrKind: kind of the attribute: 0 = Enumerate, 1 = Integer, 2 = Text,
   3 = CsReference

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaGiveAttributeType (Attribute attribute, AttributeType * attributeType, int *attrKind)
#else  /* __STDC__ */
void                TtaGiveAttributeType (attribute, attributeType, attrKind)
Attribute           attribute;
AttributeType      *attributeType;
int                *attrKind;

#endif /* __STDC__ */
{

   UserErrorCode = 0;
   if (attribute == NULL)

      TtaError (ERR_invalid_parameter);
   else
     {
	(*attributeType).AttrSSchema = (SSchema)
	   ((PtrAttribute) attribute)->AeAttrSSchema;
	(*attributeType).AttrTypeNum = ((PtrAttribute) attribute)->AeAttrNum;
	switch (((PtrAttribute) attribute)->AeAttrType)
	      {
		 case AtEnumAttr:
		    *attrKind = 0;
		    break;
		 case AtNumAttr:
		    *attrKind = 1;
		    break;
		 case AtTextAttr:
		    *attrKind = 2;
		    break;
		 case AtReferenceAttr:
		    *attrKind = 3;
		    break;
		 default:
		    TtaError (ERR_invalid_attribute_type);
		    break;
	      }
     }
}

/* ----------------------------------------------------------------------
   TtaGiveAttributeTypeFromName

   Retrieves the type of an attribute from its name.

   Parameter:
   name: name of the attribute.
   element: the element with which the attribute is associated.

   Return parameters:
   attributeType: type of the attribute.
   attrKind: kind of the attribute: 0 = Enumerate, 1 = Integer, 2 = Text,
   3 = CsReference
   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaGiveAttributeTypeFromName (STRING name, Element element, AttributeType * attributeType, int *attrKind)

#else  /* __STDC__ */
void                TtaGiveAttributeTypeFromName (name, element, attributeType, attrKind)
STRING              name;
Element             element;
AttributeType      *attributeType;
int                *attrKind;

#endif /* __STDC__ */

{
   PtrSSchema          pSS;

   UserErrorCode = 0;
   (*attributeType).AttrSSchema = NULL;
   (*attributeType).AttrTypeNum = 0;
   *attrKind = 0;
   if (name[0] == EOS || ustrlen (name) >= MAX_NAME_LENGTH || element == NULL)
      TtaError (ERR_invalid_parameter);
   else
     {
       GetAttrRuleFromName (&((*attributeType).AttrTypeNum), (PtrSSchema *)&((*attributeType).AttrSSchema), (PtrElement) element, name, USER_NAME);
       if ((*attributeType).AttrTypeNum == 0)
	 {
	   TtaError (ERR_invalid_parameter);
	 }
       else
	 {
	   pSS = (PtrSSchema)((*attributeType).AttrSSchema);
	   switch (pSS->SsAttribute[(*attributeType).AttrTypeNum - 1].AttrType)
	     {
	     case AtEnumAttr:
	       *attrKind = 0;
	       break;
	     case AtNumAttr:
	       *attrKind = 1;
	       break;
	     case AtTextAttr:
	       *attrKind = 2;
	       break;
	     case AtReferenceAttr:
	       *attrKind = 3;
	       break;
	     default:
	       TtaError (ERR_invalid_attribute_type);
	       break;
	     }
	 }
     }
}

/* ----------------------------------------------------------------------
   TtaGiveAttributeTypeFromOriginalName

   Retrieves the type of an attribute from its original name.

   Parameter:
   name: name of the attribute (in the language of the structure schema).
   element: the element with which the attribute is associated.

   Return parameters:
   attributeType: type of the attribute.
   attrKind: kind of the attribute: 0 = Enumerate, 1 = Integer, 2 = Text,
   3 = CsReference
   ---------------------------------------------------------------------- */

#ifdef __STDC__
void                TtaGiveAttributeTypeFromOriginalName (STRING name, Element element, AttributeType * attributeType, int *attrKind)

#else  /* __STDC__ */
void                TtaGiveAttributeTypeFromOriginalName (name, element, attributeType, attrKind)
STRING              name;
Element             element;
AttributeType      *attributeType;
int                *attrKind;

#endif /* __STDC__ */

{
   PtrSSchema          pSS;

   UserErrorCode = 0;
   (*attributeType).AttrSSchema = NULL;
   (*attributeType).AttrTypeNum = 0;
   *attrKind = 0;
   if (name[0] == EOS || ustrlen (name) >= MAX_NAME_LENGTH || element == NULL)
      TtaError (ERR_invalid_parameter);
   else
     {
       GetAttrRuleFromName (&((*attributeType).AttrTypeNum), (PtrSSchema *)&((*attributeType).AttrSSchema), (PtrElement) element, name, SCHEMA_NAME);
       if ((*attributeType).AttrTypeNum == 0)
	 {
	   TtaError (ERR_invalid_parameter);
	 }
       else
	 {
	   pSS = (PtrSSchema)((*attributeType).AttrSSchema);
	   switch (pSS->SsAttribute[(*attributeType).AttrTypeNum - 1].AttrType)
	     {
	     case AtEnumAttr:
	       *attrKind = 0;
	       break;
	     case AtNumAttr:
	       *attrKind = 1;
	       break;
	     case AtTextAttr:
	       *attrKind = 2;
	       break;
	     case AtReferenceAttr:
	       *attrKind = 3;
	       break;
	     default:
	       TtaError (ERR_invalid_attribute_type);
	       break;
	     }
	 }
     }
}

/* ----------------------------------------------------------------------
   TtaGetAttributeName

   Returns the name of an attribute type.

   Parameter:
   attributeType: type of the attribute.

   Return value:
   name of that type.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
STRING              TtaGetAttributeName (AttributeType attributeType)

#else  /* __STDC__ */
STRING              TtaGetAttributeName (attributeType)
AttributeType       attributeType;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
   bufferName[0] = EOS;
   if (attributeType.AttrSSchema == NULL)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else if (attributeType.AttrTypeNum < 1 ||
	    attributeType.AttrTypeNum > ((PtrSSchema) (attributeType.AttrSSchema))->SsNAttributes)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else
     {
	ustrncpy (bufferName, ((PtrSSchema) (attributeType.AttrSSchema))->SsAttribute[attributeType.AttrTypeNum - 1].AttrName, MAX_NAME_LENGTH);
     }
   return bufferName;
}

/* ----------------------------------------------------------------------
   TtaGetAttributeOriginalName

   Returns the name of an attribute type in the schema language.

   Parameter:
   attributeType: type of the attribute.

   Return value:
   name of that type.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
STRING              TtaGetAttributeOriginalName (AttributeType attributeType)

#else  /* __STDC__ */
STRING              TtaGetAttributeOriginalName (attributeType)
AttributeType       attributeType;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
   bufferName[0] = EOS;
   if (attributeType.AttrSSchema == NULL)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else if (attributeType.AttrTypeNum < 1 ||
	    attributeType.AttrTypeNum > ((PtrSSchema) (attributeType.AttrSSchema))->SsNAttributes)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else
     {
	ustrncpy (bufferName, ((PtrSSchema) (attributeType.AttrSSchema))->SsAttribute[attributeType.AttrTypeNum - 1].AttrOrigName, MAX_NAME_LENGTH);
     }
   return bufferName;
}

/* ----------------------------------------------------------------------
   TtaSameAttributeTypes

   Compares two attribute types.

   Parameter:
   type1: first attribute type.
   type2: second attribute type.

   Return value:
   0 if both types are different, 1 if they are identical.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 TtaSameAttributeTypes (AttributeType type1, AttributeType type2)
#else  /* __STDC__ */
int                 TtaSameAttributeTypes (type1, type2)
AttributeType       type1;
AttributeType       type2;
#endif /* __STDC__ */
{
   int                 result;

   UserErrorCode = 0;
   result = 0;
   if (type1.AttrSSchema == NULL || type2.AttrSSchema == NULL)
      if (type1.AttrTypeNum == 1 && type2.AttrTypeNum == 1)
	 result = 1;
      else
	 result = 0;
   else if (type1.AttrTypeNum < 1 ||
	    type2.AttrTypeNum < 1 ||
	    type1.AttrTypeNum > ((PtrSSchema) (type1.AttrSSchema))->SsNAttributes ||
      type2.AttrTypeNum > ((PtrSSchema) (type2.AttrSSchema))->SsNAttributes)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else
     {
	if (type1.AttrTypeNum == type2.AttrTypeNum)
	   if (((PtrSSchema) (type1.AttrSSchema))->SsCode ==
	       ((PtrSSchema) (type2.AttrSSchema))->SsCode)
	      result = 1;
     }
   return result;
}

/* ----------------------------------------------------------------------
   TtaGetAttributeValue

   Returns the value of a given attribute of type integer or enumerate.

   Parameter:
   attribute: the attribute of interest.

   Return value:
   Value of that attribute.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 TtaGetAttributeValue (Attribute attribute)
#else  /* __STDC__ */
int                 TtaGetAttributeValue (attribute)
Attribute           attribute;
#endif /* __STDC__ */
{
   int                 value;

   UserErrorCode = 0;
   value = 0;
   if (attribute == NULL)
     TtaError (ERR_invalid_attribute_type);
   else if (((PtrAttribute) attribute)->AeAttrType != AtEnumAttr &&
	    ((PtrAttribute) attribute)->AeAttrType != AtNumAttr)
     TtaError (ERR_invalid_attribute_type);
   else
     value = ((PtrAttribute) attribute)->AeAttrValue;
   return value;
}
/* ----------------------------------------------------------------------
   TtaGetAttributeValueOriginalName

   Returns the original name of a value of an attribute of type enumerate.
   (as it is defined in the S schema)
   Parameter:
   attributeType: type of the attribute.
   value: the value

   Return value:
   name of that value or empty string if error.

   ---------------------------------------------------------------------- */


#ifdef __STDC__
STRING              TtaGetAttributeValueOriginalName (AttributeType attributeType, int value)

#else  /* __STDC__ */
STRING              TtaGetAttributeValueOriginalName (attributeType, value)
AttributeType       attributeType;
int		    value;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
   bufferName[0] = EOS;
   if (attributeType.AttrSSchema == NULL)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else if (attributeType.AttrTypeNum < 1 ||
	    attributeType.AttrTypeNum > ((PtrSSchema) (attributeType.AttrSSchema))->SsNAttributes)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else if (((PtrSSchema) (attributeType.AttrSSchema))->SsAttribute[attributeType.AttrTypeNum - 1].AttrType != AtEnumAttr )
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else if (value < 1 || value > ((PtrSSchema) (attributeType.AttrSSchema))->SsAttribute[attributeType.AttrTypeNum - 1].AttrNEnumValues)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	ustrncpy (bufferName, ((PtrSSchema) (attributeType.AttrSSchema))->SsAttribute[attributeType.AttrTypeNum - 1].AttrEnumOrigValue[value - 1], MAX_NAME_LENGTH);
     }
   return bufferName;
}


/* ----------------------------------------------------------------------
   TtaGetAttributeValueName

   Returns the name of a value of an attribute of type enumerate.

   Parameter:
   attributeType: type of the attribute.
   value: the value

   Return value:
   name of that value or empty string if error.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
STRING              TtaGetAttributeValueName (AttributeType attributeType, int value)

#else  /* __STDC__ */
STRING              TtaGetAttributeValueName (attributeType, value)
AttributeType       attributeType;
int		    value;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
   bufferName[0] = EOS;
   if (attributeType.AttrSSchema == NULL)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else if (attributeType.AttrTypeNum < 1 ||
	    attributeType.AttrTypeNum > ((PtrSSchema) (attributeType.AttrSSchema))->SsNAttributes)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else if (((PtrSSchema) (attributeType.AttrSSchema))->SsAttribute[attributeType.AttrTypeNum - 1].AttrType != AtEnumAttr )
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else if (value < 1 || value > ((PtrSSchema) (attributeType.AttrSSchema))->SsAttribute[attributeType.AttrTypeNum - 1].AttrNEnumValues)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	ustrncpy (bufferName, ((PtrSSchema) (attributeType.AttrSSchema))->SsAttribute[attributeType.AttrTypeNum - 1].AttrEnumValue[value - 1], MAX_NAME_LENGTH);
     }
   return bufferName;
}


/* ----------------------------------------------------------------------
   TtaGetAttributeValueFromOriginalName

   Retrieves the int value of an attribute of type enumerate from its 
   original name (as it is defined in the S schema).

   Parameter:
   name: original name of the value.
   attrType: type of the attribute

   Return value:
   the corresponding int value, or 0 if error.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                TtaGetAttributeValueFromOriginalName (STRING name, AttributeType attributeType)

#else  /* __STDC__ */
int                TtaGetAttributeValueFromOriginalName (name, attributeType)
STRING              name;
AttributeType      attributeType;

#endif /* __STDC__ */

{
   TtAttribute	      *attr;
   int		       value, i;

   UserErrorCode = 0;
   value = 0;
   if (name[0] == EOS || ustrlen (name) >= MAX_NAME_LENGTH)
     {
        TtaError (ERR_invalid_parameter);
     }
   if (attributeType.AttrSSchema == NULL)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else if (attributeType.AttrTypeNum < 1 ||
	    attributeType.AttrTypeNum > ((PtrSSchema) (attributeType.AttrSSchema))->SsNAttributes)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else if (((PtrSSchema) (attributeType.AttrSSchema))->SsAttribute[attributeType.AttrTypeNum - 1].AttrType != AtEnumAttr )
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else
     {
        attr = & ((PtrSSchema) (attributeType.AttrSSchema))->SsAttribute[attributeType.AttrTypeNum - 1];
        for (i = 0; value == 0 && i < attr->AttrNEnumValues; i++)
	    if (ustrncmp(attr->AttrEnumOrigValue[i], name, MAX_NAME_LENGTH) == 0)
		value = i+1;
     }
   return value;
}

/* ----------------------------------------------------------------------
   TtaGetAttributeValueFromName

   Retrieves the int value of an attribute of type enumerate from its name.

   Parameter:
   name: name of the value.
   attrType: type of the attribute

   Return value:
   the corresponding int value, or 0 if error.

   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                TtaGetAttributeValueFromName (STRING name, AttributeType attributeType)

#else  /* __STDC__ */
int                TtaGetAttributeValueFromName (name, attributeType)
STRING              name;
AttributeType      attributeType;

#endif /* __STDC__ */

{
   TtAttribute	      *attr;
   int		       value, i;

   UserErrorCode = 0;
   value = 0;
   if (name[0] == EOS || ustrlen (name) >= MAX_NAME_LENGTH)
     {
        TtaError (ERR_invalid_parameter);
     }
   if (attributeType.AttrSSchema == NULL)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else if (attributeType.AttrTypeNum < 1 ||
	    attributeType.AttrTypeNum > ((PtrSSchema) (attributeType.AttrSSchema))->SsNAttributes)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else if (((PtrSSchema) (attributeType.AttrSSchema))->SsAttribute[attributeType.AttrTypeNum - 1].AttrType != AtEnumAttr )
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else
     {
        attr = & ((PtrSSchema) (attributeType.AttrSSchema))->SsAttribute[attributeType.AttrTypeNum - 1];
        for (i = 0; value == 0 && i < attr->AttrNEnumValues; i++)
	    if (ustrncmp(attr->AttrEnumValue[i], name, MAX_NAME_LENGTH) == 0)
		value = i+1;
     }
   return value;
}

/* ----------------------------------------------------------------------
   TtaSearchAttribute

   Searches the next element that has a given attribute.
   Searching can be done in a subtree or starting from a given element towards
   the beginning or the end of the abstract tree.

   Parameters:
   searchedAttribute: attribute to be searched.
   If searchedAttribute.AttrSSchema is NULL, the next element
   that has an attribute is searched, whatever the attribute.
   scope: SearchForward, SearchBackward or SearchInTree.
   element: the element that is the root of the tree
   (if scope = SearchInTree) or the starting element
   (if scope = SearchForward or SearchBackward).

   Return parameters:
   elementFound: the element found, or NULL if not found.
   attributeFound: the searched attribute, or NULL if not found.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaSearchAttribute (AttributeType searchedAttribute, SearchDomain scope, Element element, Element * elementFound, Attribute * attributeFound)

#else  /* __STDC__ */
void                TtaSearchAttribute (searchedAttribute, scope, element, elementFound, attributeFound)
AttributeType       searchedAttribute;
SearchDomain        scope;
Element             element;
Element            *elementFound;
Attribute          *attributeFound;
#endif /* __STDC__ */
{
   PtrElement          pEl;
   PtrAttribute        pAttr;
   ThotBool            ok;

   UserErrorCode = 0;
   ok = TRUE;
   *elementFound = NULL;
   *attributeFound = NULL;
   if (element == NULL)
     {
	TtaError (ERR_invalid_parameter);
	ok = FALSE;
     }
   else if (((PtrElement) element)->ElStructSchema == NULL)
     {
	TtaError (ERR_invalid_parameter);
	ok = FALSE;
     }
   else if (searchedAttribute.AttrSSchema == NULL)
      searchedAttribute.AttrTypeNum = 0;
   else if (searchedAttribute.AttrTypeNum < 1 ||
	    searchedAttribute.AttrTypeNum >
	    ((PtrSSchema) (searchedAttribute.AttrSSchema))->SsNAttributes)
     {
	TtaError (ERR_invalid_attribute_type);
	ok = FALSE;
     }
   if (ok)
     {
	if (scope == SearchBackward)
	   pEl = BackSearchAttribute ((PtrElement) element, searchedAttribute.AttrTypeNum, 0, _EMPTYSTR_,
			      (PtrSSchema) (searchedAttribute.AttrSSchema));
	else
	   pEl = FwdSearchAttribute ((PtrElement) element, searchedAttribute.AttrTypeNum, 0, _EMPTYSTR_,
			      (PtrSSchema) (searchedAttribute.AttrSSchema));
	if (pEl != NULL)
	   if (scope == SearchInTree)
	     {
		if (!ElemIsWithinSubtree (pEl, (PtrElement) element))
		   pEl = NULL;
	     }
	if (pEl != NULL)
	  {
	     *elementFound = (Element) pEl;
	     pAttr = pEl->ElFirstAttr;
	     if (pAttr != NULL)
		/* if we look at any attribute, we find the first attribut of the given element, */
		/* else we go over the attributs of the element until we find the right one */
		if (searchedAttribute.AttrSSchema != NULL)
		   do
		      if (pAttr->AeAttrSSchema->SsCode ==
		      ((PtrSSchema) (searchedAttribute.AttrSSchema))->SsCode
		       && pAttr->AeAttrNum == searchedAttribute.AttrTypeNum)
			 /* the expected attribute */
			 *attributeFound = (Attribute) pAttr;
		      else
			 pAttr = pAttr->AeNext;
		   while (pAttr != NULL && *attributeFound == NULL);
	  }
     }
}

/* fin du module */
