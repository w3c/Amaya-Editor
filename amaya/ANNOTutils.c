/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * ANNOTutil.c : different miscellaneous functions used by the other
 *               annotation modules
 *
 * Author: J. Kahan (W3C/INRIA)
 *
 * Acknowledgments: inspired from code written by Christophe Marjoline 
 *                  for the byzance collaborative work application
 */

#include "annot.h"

/*-----------------------------------------------------------------------
   Procedure SearchAnnotation (doc, annotDoc)
  -----------------------------------------------------------------------
   Searches doc and returns the link element that points to annotDoc, or
   NULL if it doesn't exist.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
Element SearchAnnotation (Document doc, STRING annotDoc)
#else /* __STDC__*/
Element SearchAnnotation (doc, annotDoc)
     Document doc;
     STRING   annotDoc;
#endif /* __STDC__*/
{
  ElementType elType;
  Element     elCour;
  STRING ancName = NULL;

  elCour = SearchElementInDoc (doc, HTML_EL_BODY);
  elType = TtaGetElementType (elCour);
  elType.ElTypeNum = HTML_EL_Anchor;

  /* @@@ need to add a filter to use HTML_ATTR_Annotation */
  /* Searches the first anchor */
  elCour = TtaSearchTypedElement (elType, SearchForward, elCour);

  /* Searchs the anchor that points to the annotDoc */
  while (elCour != NULL) 
  {
    ancName = SearchAttributeInElt (doc, elCour, HTML_ATTR_NAME);
    if (ancName) 
      {
	if (!strcmp (ancName, annotDoc))
	  break;
	TtaFreeMemory (ancName);
      }
    elCour = TtaGetSuccessor (elCour);
    elCour = TtaSearchTypedElement (elType, SearchForward, elCour);
    ancName = SearchAttributeInElt (doc, elCour, HTML_ATTR_NAME);
  }

  if (ancName)
    TtaFreeMemory (ancName);

  return elCour;
}

/*-----------------------------------------------------------------------
   Procedure SearchElementInDoc (doc, elTypeNum)
  -----------------------------------------------------------------------
   Returns the first element of type elTypeNum found in the document
   or NULL if it doesn't exist.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
Element SearchElementInDoc (Document doc, int elTypeNum)
#else /* __STDC__*/
Element SearchElementInDoc (doc, elTypeNum)
     Document doc;
     int      elTypeNum;
#endif /* __STDC__*/
{
  Element     root;
  ElementType elType;

  if ((root = TtaGetMainRoot (doc)) == NULL)
    return NULL;
  elType = TtaGetElementType (root);
  elType.ElTypeNum = elTypeNum;
  return TtaSearchTypedElement (elType, SearchInTree, root);
}

/*-----------------------------------------------------------------------
   Procedure SearchAttributeInElt (doc, el, attrTypeNum)
  -----------------------------------------------------------------------
   Returns the value of attribute type attrTypeNum if it exists in the
   document element or NULL otherwise.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
STRING SearchAttributeInElt (Document doc, Element el, int attrTypeNum)
#else /* __STDC__*/
STRING SearchAttributeInElt (doc, el, attrTypeNum)
     Document doc;
     Element  el;
     int      attrTypeNum;
#endif /* __STDC__*/
{
  AttributeType attrType;
  Attribute     attr;
  STRING        text = TtaGetMemory (50);
  int           text_lg;

  if (!el) 
    return NULL;

  text[0] = '\0';
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = attrTypeNum;
  attr = TtaGetAttribute (el, attrType);
  if (!attr)
    return NULL;
  else
  {
    /* @@@ bug! */
    text = TtaGetMemory (50);
    text_lg = 50;
    TtaGiveTextAttributeValue (attr, text, &text_lg);
    return text;
  }
}

/*-----------------------------------------------------------------------
   Procedure SubstituteCharInString (buffer, old, new)
  -----------------------------------------------------------------------
   Substitutes all occurences of old char with new char in string buffer
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void SubstituteCharInString (char *buffer, char old, char new)
#else
void SubstituteCharInString (buffer, old, new)
char *buffer;
char old;
char new;
#endif /* __STDC__ */
{
  char *ptr;

  ptr = buffer;
  while (*ptr)
    {
      if (*ptr == old)
	*ptr = new;
      ptr++;
    }
}


/*-----------------------------------------------------------------------
  GetTempName
  Front end to the Unix tempnam function, which is independent of the
  value of the TMPDIR env value 
  Returns a dynamically allocated string with a tempname. The user
  must free this memory.
  -----------------------------------------------------------------------*/
#ifdef __STDC__
char *GetTempName (const char *dir, const char *prefix)
#else
char *GetTempName (dir, prefix)
const char *dir;
const char *prefix;
#endif /* __STDC__ */
{
  char *tmpdir;
  char *tmp;
  char *name = NULL;

  /* save the value of TMPDIR */
  tmp = getenv ("TMPDIR");
  if (tmp)
     tmpdir = strdup (tmp);
  else
     tmpdir = NULL;

  /* rempve TMPDIR from the environment */
  if (tmpdir)
     unsetenv ("TMPDIR");

  name = tempnam (dir, prefix);
  /* restore the value of TMPDIR */
  if (tmpdir)
    setenv ("TMPDIR", tmpdir, 0);

  return (name);
}

/***************************************************
 I've not yet used/cleaning the following legacy functions 
***************************************************/

/*-----------------------------------------------------------------------
   Procedure IsAnnotationDocument (document)
  -----------------------------------------------------------------------
   Retourne vrai si le document est un document d'annotations
  -----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool IsAnnotationDocument (Document document)
#else /* __STDC__*/
ThotBool IsAnnotationDocument (document)
     Document document;
#endif /* __STDC__*/
{
  ElementType elType;
  Element     root, dateAnnot;

  root = TtaGetMainRoot (document);
  elType = TtaGetElementType (root);
#if 0
  /* I could search for metadata here */
  elType.ElTypeNum = HTML_EL_AuteurAnnot;
#endif
  dateAnnot = TtaSearchTypedElement (elType, SearchInTree, root);

  return (dateAnnot != NULL);
}

/*-----------------------------------------------------------------------
   Procedure IsAnnotationLink (document, element)
  -----------------------------------------------------------------------
   Retourne vrai si l'element du document est un lien d'annotation
  -----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool IsAnnotationLink (Document document, Element element)
#else /* __STDC__*/
ThotBool IsAnnotationLink (document, element)
     Document document;
     Element  element;
#endif /* __STDC__*/
{
  /* @@ is this ok? */
  STRING text = SearchAttributeInElt (document, element, HTML_ATTR_Annotation);
  return !strcmp (text, "Annotation");
}

/*-----------------------------------------------------------------------
   Procedure IsInTable (docName)
  -----------------------------------------------------------------------
   Retourne vrai si le document d'annotation docName est dana la table
   de references des annotations, ce qui veut dire qu'il est en cours
   d'edition
  -----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool IsInTable (STRING docName)
#else /* __STDC__*/
ThotBool IsInTable (docName)
     STRING docName;
#endif /* __STDC__*/
{
  return 0;
#if 0
  int i = 0;

  while ((i < 10) && (strcmp (docName, tabRefAnnot[i].docName)))
    i++;
  return (i != 10);
#endif
}

/*-----------------------------------------------------------------------
   Procedure AnnotationTargetDocument (annotDoc)
  -----------------------------------------------------------------------
   Retourne le document annote par annotDoc ou NULL si celui-ci n'est
   pas ouvert ou n'existe pas
  -----------------------------------------------------------------------*/

#ifdef __STDC__
Document AnnotationTargetDocument (Document annotDoc)
#else /* __STDC__*/
Document AnnotationTargetDocument (annotDoc)
     Document annotDoc;
#endif /* __STDC__*/
{
  Element  ptr_annotDoc, text;
  Language lang;
  int      lg = 200;
  STRING   docName = TtaGetMemory (200);

#if 0
  ptr_annotDoc = SearchElementInDoc (annotDoc, HTML_EL_PtrDocAnnot);
#endif
  text = TtaGetFirstChild (ptr_annotDoc);
  TtaGiveTextContent (text, docName, &lg, &lang);

  return TtaGetDocumentFromName (docName);
}



