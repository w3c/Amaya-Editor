
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "application.h"
#include "document.h"
#include "attribute.h"
#include "tree.h"
#include "typecorr.h"

#undef EXPORT
#define EXPORT
#include "edit.var"

#include "tree_f.h"
#include "attributes_f.h"
#include "attributeapi_f.h"
#include "draw_f.h"
#include "memory_f.h"
#include "references_f.h"
#include "structschema_f.h"
#include "content_f.h"
#include "thotmsg_f.h"

extern int          UserErrorCode;
extern int          AvecControleStruct;

static Name          nameBuffer;

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
   PtrAttribute         pAttr;

   UserErrorCode = 0;
   pAttr = NULL;
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
	GetAttr (&pAttr);
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
   PtrAttribute         pAttr;
   boolean             obligatoire;

   UserErrorCode = 0;
   if (element == NULL || attribute == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
      /* verifie le parametre document */
   if (document < 1 || document > MAX_DOCUMENTS)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else if (TabDocuments[document - 1] == NULL)
     {
	TtaError (ERR_invalid_document_parameter);
     }
   else
      /* parametre document correct */
      /* l'element porte-t-il deja un attribut du meme type ? */
   if (AttributeValue ((PtrElement) element, (PtrAttribute) attribute) != NULL)
      /* oui, erreur */
     {
	TtaError (ERR_duplicate_attribute);
     }
   else
      /* peut-on appliquer l'attribut a l'element ? */
   if (AvecControleStruct && !CanAssociateAttr ((PtrElement) element, NULL, (PtrAttribute) attribute, &obligatoire))
      /* non, erreur */
     {
	TtaError (ERR_attribute_element_mismatch);
     }
   else
     {
#ifndef NODISPLAY
	UndisplayHeritAttr ((PtrElement) element, (PtrAttribute) attribute, document, FALSE);
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
	pAttr = (PtrAttribute) attribute;
	pAttr->AeNext = NULL;
	pAttr->AeDefAttr = FALSE;
	if (pAttr->AeAttrType == AtReferenceAttr)
	   if (pAttr->AeAttrReference != NULL)
	      pAttr->AeAttrReference->RdElement = (PtrElement) element;
	/* traitement special a l'ajout d'un attribut a un */
	/* element d'un objet Draw */
	DrawAjAttr (&pAttr, (PtrElement) element);
#ifndef NODISPLAY
	DisplayAttribute ((PtrElement) element, pAttr, document);
#endif
     }
}

	/* AttrOfElement verifie que l'attribut appartient bien a l'element */
#ifdef __STDC__
static boolean      AttrOfElement (Attribute attribute, Element element)
#else  /* __STDC__ */
static boolean      AttrOfElement (attribute, element)
Attribute           attribute;
Element             element;

#endif /* __STDC__ */

{
   PtrAttribute         pAttr;
   boolean             ok;

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
   PtrAttribute         pAttr;
   boolean             trouve;
   boolean             obligatoire;

   UserErrorCode = 0;
   if (element == NULL || attribute == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else
     {
	pAttr = ((PtrElement) element)->ElFirstAttr;
	trouve = FALSE;
	while (pAttr != NULL && !trouve)
	  {
	     if (pAttr->AeAttrSSchema->SsCode ==
		 ((PtrAttribute) attribute)->AeAttrSSchema->SsCode)
		if (pAttr->AeAttrNum == ((PtrAttribute) attribute)->AeAttrNum)
		   trouve = TRUE;
	     if (!trouve)
		pAttr = pAttr->AeNext;
	  }
	if (!trouve)
	  {
	     TtaError (ERR_attribute_element_mismatch);
	  }
	else
	  {
	     (void) CanAssociateAttr ((PtrElement) element, pAttr, pAttr,
					 &obligatoire);
	     if (!obligatoire)
		/* on interdit d'enlever l'attribut Langue d'un element */
		/* racine d'un arbre abstrait */
		if (((PtrElement) element)->ElParent == NULL)
		   if (pAttr->AeAttrNum == 1)
		      obligatoire = TRUE;
	     if (obligatoire)
		/* l'attribut est obligatoire pour ce type d'element */
	       {
		  TtaError (ERR_mandatory_attribute);
	       }
	     else
	       {
		  DeleteAttribute ((PtrElement) element, pAttr);
#ifndef NODISPLAY
		  UndisplayHeritAttr ((PtrElement) element, pAttr, document, TRUE);
#endif
		  /* traitement special a la suppression d'un */
		  /* attribut a un element d'un objet Draw */
		  DrawSupprAttr (pAttr, (PtrElement) element);
#ifndef NODISPLAY
		  UndisplayAttribute ((PtrElement) element, (PtrAttribute) attribute, document);
#endif
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
   PtrAttribute         pAttr;
   boolean             ok;

   UserErrorCode = 0;
   pAttr = (PtrAttribute) attribute;
   ok = FALSE;
   if (attribute == NULL)
     {
	TtaError (ERR_invalid_parameter);
     }
   else if (pAttr->AeAttrType != AtEnumAttr &&
	    pAttr->AeAttrType != AtNumAttr)
     {
	TtaError (ERR_invalid_attribute_value);
     }
   else if (AttrOfElement (attribute, element))
     {
	if (pAttr->AeAttrType == AtNumAttr)
	   if (abs (value) > 65535)
	      /* la forme pivot represente les entiers sur 2 octets */
	     {
		TtaError (ERR_invalid_attribute_value);
	     }
	   else
	      ok = TRUE;
	else if (pAttr->AeAttrSSchema == NULL)
	  {
	     TtaError (ERR_invalid_attribute_type);
	  }
	else if (value < 1)
	  {
	     TtaError (ERR_invalid_attribute_value);
	  }
	else if (value > pAttr->AeAttrSSchema->SsAttribute
		 [pAttr->AeAttrNum - 1].AttrNEnumValues)
	  {
	     TtaError (ERR_invalid_attribute_value);
	  }
	else
	   ok = TRUE;
	if (ok)
	  {
#ifndef NODISPLAY
	     if (element != NULL)
		UndisplayHeritAttr ((PtrElement) element, pAttr, document,
				    FALSE);
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
void                TtaSetAttributeText (Attribute attribute, char *buffer, Element element, Document document)

#else  /* __STDC__ */
void                TtaSetAttributeText (attribute, buffer, element, document)
Attribute           attribute;
char               *buffer;
Element             element;
Document            document;

#endif /* __STDC__ */

{
   int                 lg;
   PtrAttribute         pAttr;
   PtrAttribute         pAttrNouv;

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
	   UndisplayHeritAttr ((PtrElement) element, pAttr, document, FALSE);
#endif
	if (pAttr->AeAttrText == NULL)
	   GetBufTexte (&pAttr->AeAttrText);
	else
	   ClearText (pAttr->AeAttrText);
	/* met le nouveau contenu */
	CopyStringToText (buffer, pAttr->AeAttrText, &lg);
	if (pAttr->AeAttrNum == 1)
	  {
	     GetAttr (&pAttrNouv);
	     pAttrNouv->AeAttrSSchema = pAttr->AeAttrSSchema;
	     pAttrNouv->AeAttrNum = 1;
	     pAttrNouv->AeDefAttr = FALSE;
	     pAttrNouv->AeAttrType = pAttr->AeAttrType;
	     GetBufTexte (&pAttrNouv->AeAttrText);
	     CopyStringToText (buffer, pAttrNouv->AeAttrText, &lg);
	     AttachAttrToRange (pAttrNouv, 0, 0, (PtrElement) element, (PtrElement) element,
			    TabDocuments[document - 1]);
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
   PtrAttribute         nextAttribute;

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
   PtrAttribute         pAttr;
   PtrAttribute         attribute;
   boolean             trouve;
   boolean             erreur;

   UserErrorCode = 0;
   attribute = NULL;
   if (element == NULL || (attributeType.AttrSSchema == NULL && attributeType.AttrTypeNum != 1))
      /* attributeType.AttrTypeNum = 1 : attribut Langue dans tout schema */
      TtaError (ERR_invalid_parameter);
   else
     {
	erreur = FALSE;
	/* on ne fait pas d'autre verification si c'est l'attribut "Langue" */
	if (attributeType.AttrTypeNum != 1)
	   if (attributeType.AttrTypeNum < 1 ||
	       attributeType.AttrTypeNum > ((PtrSSchema) (attributeType.AttrSSchema))->SsNAttributes)
	      erreur = TRUE;
	if (erreur)
	   TtaError (ERR_invalid_attribute_type);
	else
	  {
	     attribute = NULL;
	     pAttr = ((PtrElement) element)->ElFirstAttr;
	     trouve = FALSE;
	     while (pAttr != NULL && !trouve)
	       {
		  if (pAttr->AeAttrNum == attributeType.AttrTypeNum)
		     /* meme numero d'attribut */
		     if (attributeType.AttrSSchema == NULL)
			/* on ne s'interesse pas au schems de structure */
			trouve = TRUE;
		     else if (pAttr->AeAttrSSchema->SsCode ==
			      ((PtrSSchema) (attributeType.AttrSSchema))->SsCode)
			/* meme schema de structure */
			trouve = TRUE;
		  if (trouve)
		     attribute = pAttr;
		  else
		     pAttr = pAttr->AeNext;
	       }
	  }
     }
   return ((Attribute) attribute);
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
void                TtaGiveAttributeTypeFromName (char *name, Element element, AttributeType * attributeType, int *attrKind)

#else  /* __STDC__ */
void                TtaGiveAttributeTypeFromName (name, element, attributeType, attrKind)
char               *name;
Element             element;
AttributeType      *attributeType;
int                *attrKind;

#endif /* __STDC__ */

{
   int                 i;
   PtrElement          pEl;
   boolean             trouve, nouveau;
   PtrSSchema        pSS;
   SRule              *pRe1;

#define MaxSch 20
   PtrSSchema        attrStruct[MaxSch];
   int                 att, nbsch;

   pSS = NULL;
   att = 0;
   UserErrorCode = 0;
   (*attributeType).AttrSSchema = NULL;
   (*attributeType).AttrTypeNum = 0;
   *attrKind = 0;
   if (name[0] == '\0' || strlen (name) >= MAX_NAME_LENGTH)
      TtaError (ERR_invalid_parameter);
   else
     {
	trouve = FALSE;
	nbsch = 0;
	pEl = (PtrElement) element;
	/* cherche tous les schemas de structure utilises par les */
	/* elements ascendants */
	while (pEl != NULL && !trouve)
	  {
	     /* schema de struct de l'element courant */
	     pSS = pEl->ElSructSchema;
	     /* on parcourt toutes les extensions de schema de ce schema */
	     do
	       {
		  /* on a deja traite' ce schema de structure ? */
		  nouveau = TRUE;
		  for (i = 1; i <= nbsch; i++)	/* parcourt la table */
		     if (pSS == attrStruct[i - 1])	/* deja dans la table */
			nouveau = FALSE;
		  if (nouveau)
		     /* l'element utilise un schema de structure pas encore */
		     /* rencontre' */
		    {
		       /* conserve le schema de structure dans la table */
		       if (nbsch < MaxSch)
			 {
			    nbsch++;
			    attrStruct[nbsch - 1] = pSS;
			 }
		       /* teste tous les attributs globaux de ce schema */
		       att = 0;
		       while (att < pSS->SsNAttributes && !trouve)
			 {
			    att++;
			    /* on saute les attributs locaux */
			    if (pSS->SsAttribute[att - 1].AttrGlobal)
			       if (strcmp (name, pSS->SsAttribute[att - 1].AttrName) == 0)
				  trouve = TRUE;
			 }
		    }
		  if (!trouve)
		     /* passe a l'extension de schema suivante */
		     pSS = pSS->SsNextExtens;
	       }
	     while (pSS != NULL && !trouve);
	     pEl = pEl->ElParent;	/* passe a l'element ascendant */
	  }
	if (!trouve)
	  {
	     /* cherche parmi les attributs locaux de l'element */
	     /* prend d'abord la regle qui definit cet element */
	     pSS = ((PtrElement) element)->ElSructSchema;
	     pRe1 = &pSS->SsRule[((PtrElement) element)->ElTypeNumber - 1];
	     do
	       {
		  if (pRe1 != NULL)
		     /* teste les attributs locaux definis dans cette regle */
		     for (i = 1; i <= pRe1->SrNLocalAttrs && !trouve; i++)
		       {
			  att = pRe1->SrLocalAttr[i - 1];
			  if (strcmp (name, pSS->SsAttribute[att - 1].AttrName) == 0)
			     trouve = TRUE;
		       }
		  if (!trouve)
		    {
		       /* passe a l'extension suivante du schema de structure */
		       pSS = pSS->SsNextExtens;
		       /* cherche dans cette extension de schema la regle */
		       /* d'extension pour l'element */
		       if (pSS != NULL)
			  pRe1 = ExtensionRule (((PtrElement) element)->ElSructSchema,
				       ((PtrElement) element)->ElTypeNumber, pSS);
		    }
	       }
	     while (pSS != NULL && !trouve);
	  }
	if (!trouve)
	  {
	     TtaError (ERR_invalid_parameter);
	  }
	else
	  {
	     (*attributeType).AttrSSchema = (SSchema) pSS;
	     (*attributeType).AttrTypeNum = att;
	     switch (pSS->SsAttribute[att - 1].AttrType)
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
char               *TtaGetAttributeName (AttributeType attributeType)

#else  /* __STDC__ */
char               *TtaGetAttributeName (attributeType)
AttributeType       attributeType;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
   nameBuffer[0] = '\0';
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
	strncpy (nameBuffer, ((PtrSSchema) (attributeType.AttrSSchema))->SsAttribute[attributeType.AttrTypeNum - 1].AttrName, MAX_NAME_LENGTH);
     }
   return nameBuffer;
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
   if (((PtrAttribute) attribute)->AeAttrType != AtEnumAttr &&
       ((PtrAttribute) attribute)->AeAttrType != AtNumAttr)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else
     {
	value = ((PtrAttribute) attribute)->AeAttrValue;
     }
   return value;
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
   PtrTextBuffer      pBT;

   UserErrorCode = 0;
   length = 0;
   if (((PtrAttribute) attribute)->AeAttrType != AtTextAttr)
     {
	TtaError (ERR_invalid_attribute_type);
     }
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
void                TtaGiveTextAttributeValue (Attribute attribute, char *buffer, int *length)

#else  /* __STDC__ */
void                TtaGiveTextAttributeValue (attribute, buffer, length)
Attribute           attribute;
char               *buffer;
int                *length;

#endif /* __STDC__ */

{

   UserErrorCode = 0;
/**** supprimer ca :	*length = 0;	****/
   *buffer = '\0';
   if (((PtrAttribute) attribute)->AeAttrType != AtTextAttr)
     {
	TtaError (ERR_invalid_attribute_type);
     }
   else
     {
	CopyTextToString (((PtrAttribute) attribute)->AeAttrText, buffer,
			      length);
     }
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
   PtrAttribute         pAttr;
   boolean             ok;

   UserErrorCode = 0;
   ok = TRUE;
   *elementFound = NULL;
   *attributeFound = NULL;
   if (element == NULL)
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
	   pEl = BackSearchAttribute ((PtrElement) element, searchedAttribute.AttrTypeNum, 0, "",
			    (PtrSSchema) (searchedAttribute.AttrSSchema));
	else
	   pEl = FwdSearchAttribute ((PtrElement) element, searchedAttribute.AttrTypeNum, 0, "",
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
		/* si on cherche un attribut quelconque, on retourne le 1er */
		/* attribut de l'element trouve', sinon, on parcourt les */
		/* attributs de l'element jusqu'a trouver le bon attribut */
		if (searchedAttribute.AttrSSchema != NULL)
		   do
		      if (pAttr->AeAttrSSchema->SsCode ==
			  ((PtrSSchema) (searchedAttribute.AttrSSchema))->SsCode
			  && pAttr->AeAttrNum == searchedAttribute.AttrTypeNum)
			 /* c'est l'attribut cherche' */
			 *attributeFound = (Attribute) pAttr;
		      else
			 pAttr = pAttr->AeNext;
		   while (pAttr != NULL && *attributeFound == NULL);
	  }
     }
}

/* fin du module */
