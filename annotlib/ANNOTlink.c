/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * ANNOTlink.c : module for handling the set of annotation indexes
 * and for merging them into documents.
 *
 * Author: J. Kahan (W3C/INRIA)
 *
 * Acknowledgments: inspired from code written by Christophe Marjoline 
 *                  for the byzance collaborative work application
 */

#include "annotlib.h"

/*-----------------------------------------------------------------------
   LINK_GetAnnotationIndexFile 
   searches an entry in the main annot index file 
   -----------------------------------------------------------------------*/
#ifdef __STDC__
char  *LINK_GetAnnotationIndexFile (char *source_url)
#else
char *LINK_GetAnnotationIndexFile (source_url)
char *source_url;
#endif /* __STDC__ */
{
  int found;
  char *annot_dir;
  char *annot_main_index;
  char *annot_main_index2;
  char url[MAX_LENGTH];
  char buffer[MAX_LENGTH];
  char *index_file;
  FILE *fp;
  
  annot_dir = GetAnnotDir ();
  annot_main_index = GetAnnotMainIndex ();
  annot_main_index2 = TtaGetMemory (strlen (annot_dir) + strlen (annot_main_index) + 10);
  sprintf (annot_main_index2, "%s%c%s", annot_dir, DIR_SEP,  annot_main_index);
  
  if (TtaFileExist (annot_main_index2))
    {
      index_file = TtaGetMemory (MAX_PATH);
      found = 0;
      if ((fp = fopen (annot_main_index2, "r")))
	{
	  while (fgets (buffer, MAX_LENGTH, fp))
	    {
	      sscanf (buffer, "%s %s\n", url, index_file);
	      if (!strcasecmp (source_url, url))
		{
		  found = 1;
		  break;
		}
	    }
	  fclose (fp);
	}
      if (!found)
	{
	  TtaFreeMemory (index_file);
	  index_file = NULL;
	}
    }
  else
    index_file = NULL;

  TtaFreeMemory (annot_main_index2);
  
  return (index_file);
}

/*-----------------------------------------------------------------------
  AddAnnotationIndexFile 
  adds a new entry to the main annot index file
  -----------------------------------------------------------------------*/
#ifdef __STDC__
static void AddAnnotationIndexFile (char *source_url, char *index_file)
#else
static void AddAnnotationIndexFile (source_url, index_file)
char *source_url;
char *index_file;
#endif /* __STDC__ */
{
  char *annot_dir;
  char *annot_main_index;
  char *annot_main_index2;
  FILE *fp;
  
  annot_dir = GetAnnotDir ();
  annot_main_index = GetAnnotMainIndex ();
  annot_main_index2 = TtaGetMemory (strlen (annot_dir) + strlen (annot_main_index) + 10);
  sprintf (annot_main_index2, "%s%c%s", annot_dir, DIR_SEP, annot_main_index);

  if ((fp = fopen (annot_main_index2, "a")))
    {
      fprintf (fp, "%s %s\n", source_url, index_file);
      fclose (fp);
    }
  TtaFreeMemory (annot_main_index2);
}

/*-----------------------------------------------------------------------
   Procedure LINK_New (doc, annotFile, labf, c1, labl, cN)
  -----------------------------------------------------------------------
  Adds a link from the source document pointing to the annotation document
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_New (Document source_doc, Document annot_doc, STRING labf, int c1, STRING labl, int cN)
#else /* __STDC__*/
void LINK_New (source_doc, annot_doc, labf, c1, labl, cN)
Document source_doc; 
Document annot_doc; 
STRING labf;
int c1;
STRING labl;
int cN;
#endif /* __STDC__*/
{
  View     view;
  STRING   annotName;
  Element  element;

  LINK_AddLink (source_doc, DocumentURLs[annot_doc], labf, c1, labl, cN);
  LINK_SaveLink (source_doc, annot_doc, labf, c1, labl, cN);

#if 0
  annotName = TtaGetMemory (20);
  strcpy (annotName, TtaGetDocumentName (docAnnot));
  document  = AnnotationTargetDocument (docAnnot);
  view      = TtaGetViewFromName (document, "Formatted_view");
  element   = TtaSearchElementByLabel(tabRefAnnot[docAnnot].labf, TtaGetMainRoot (source_doc));

  /* Si le lien d'annotation n'existe, on le rajoute dans le document et dans le fichier de liens */
  if (!SearchAnnotation (source_doc, annotName))
    {
      LINK_AddLink (source_doc, DocumentURLs[annot_doc], labf, c1, labl, cN);
      LINK_SaveLink (source_doc, annot_doc, labf, c1, labl, cN);
    }
#endif
}

/*-----------------------------------------------------------------------
   Procedure LINK_AddLink (doc, annotFile, labf, c1, labl, cN)
  -----------------------------------------------------------------------
   Ajoute un lien de type annotation dans un document donne vers le
   document de nom annotName. Le lien est cree juste avant l'element
   target
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_AddLink (Document source_doc, char *annot_file, STRING labf, int c1, STRING labl, int cN)
#else /* __STDC__*/
void LINK_AddLink (source_doc, annot_file, labf, c1, labl, cN)
Document source_doc; 
STRING annot_file;
STRING labf;
int c1;
STRING labl;
int cN;
#endif /* __STDC__*/
{
  ElementType   elType;
  Element       el, first, anchor;
  AttributeType attrType;
  Attribute     attr;
  STRING        annotName;
  char         *annot_user;

  annot_user = GetAnnotUser ();
  first   = TtaSearchElementByLabel(labf, TtaGetMainRoot (source_doc));
  elType = TtaGetElementType (first);
  elType.ElTypeNum = HTML_EL_Anchor;
  anchor = TtaNewElement (source_doc, elType);
  /* is the user trying to annotate an anchor? */
  el = TtaGetTypedAncestor (first, elType);
  if (el)
    {
      /* yes, so we will add the anchor one element before
	in the struct tree */
      TtaInsertSibling (anchor, el, TRUE, source_doc);
    }
  else
    {
      /* no, add it to the beginning of the selection */
      TtaInsertSibling (anchor, first, TRUE, source_doc);
    }

  /* add the annotation attribute */
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_Annotation;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);  

  /* add an HREF attribute pointing to the annotation */
  attrType.AttrTypeNum = HTML_ATTR_HREF_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);
  TtaSetAttributeText (attr, annot_file, anchor, source_doc);

  /* add a NAME attribute so that the annotation doc can point
     back to the source of the annotation link */
  attrType.AttrTypeNum = HTML_ATTR_NAME;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);
  annotName = TtaGetMemory (strlen (ANNOT_ANAME) + strlen (annot_user)
			    + strlen (annot_file) + 20);
  sprintf (annotName, "%s_%s_%s", ANNOT_ANAME, annot_user, annot_file);
  TtaSetAttributeText (attr, annotName, anchor, source_doc);
  TtaFreeMemory (annotName);
  TtaUnselect (source_doc);
}

/*-----------------------------------------------------------------------
   Procedure LINK_SaveLink (source_doc, annot_doc, labf, c1, labl, cN)
  -----------------------------------------------------------------------
   Writes the metadata describing an annotation file and its source
   link to an index file. This metadata consists of the
   name of the annotation file, the selected elements on the document
   (using thot labels for the moment), and the first and last characters.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_SaveLink (Document source_doc, Document annot_doc, STRING labf, int c1, STRING labl, int cN)
#else /* __STDC__*/
void LINK_SaveLink (source_doc, annot_doc, annotName, labf, c1, labl, cN)
     Document source_doc;
     Document annot_doc;
     STRING   annotName;
     STRING   labf;
     int      c1;
     STRING   labl;
     int      cN;
#endif /* __STDC__*/
{
  FILE   *indexFile;
  char   *annot_user;
  char   *indexName;

  /* Open the annotation index */
  indexName = LINK_GetAnnotationIndexFile (DocumentURLs[source_doc]);
  if (!indexName)
    {
      indexName = GetTempName (GetAnnotDir (), "index");
      AddAnnotationIndexFile (DocumentURLs[source_doc], indexName);
    }
  indexFile = fopen (indexName, "a");
  TtaFreeMemory (indexName);

  /* Add the new link */
  annot_user = GetAnnotUser ();
  fprintf (indexFile, "%s|%s|%s|%d|%s|%d\n", annot_user, DocumentURLs[annot_doc], labf, c1, labl, cN);

  /* clean up and quit */
  fclose (indexFile);
}

/*-----------------------------------------------------------------------
   Procedure LINK_LoadAnnotations (doc, char *annotIndex)
  -----------------------------------------------------------------------
   Searches for an annotation index related to the document. If it exists,
   it parses it and then displays all of its annotations
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_LoadAnnotations (Document doc, char *annotIndex)
#else /* __STDC__*/
void LINK_LoadAnnotations (doc, annotIndex)
     Document doc;
     char *annotIndex;
#endif /* __STDC__*/
{
  View    view;
  Element el, body;
  STRING  annotFile, labf, labl, annotUser;
  FILE    *fp;
  int     c1, cN;
  char    buffer[MAX_LENGTH];

  if (!annotIndex)  /* there are no annotations */
    return;

  fp = fopen (annotIndex, "r");

  if (!fp)  /* annotation index file doesn't exist */
      return;

  /* Parcours et affichage de la liste de liens */
  /* @@@ possible memory bug */
  annotFile = TtaGetMemory (MAX_LENGTH);
  annotUser = TtaGetMemory (MAX_LENGTH);
  labf    = TtaGetMemory (10);
  labl    = TtaGetMemory (10);
  view = TtaGetViewFromName (doc, "Formatted_view");
  body = SearchElementInDoc (doc, HTML_EL_BODY);
  while (fgets (buffer, MAX_LENGTH, fp))
  {
    SubstituteCharInString (buffer, '|', ' ');
    sscanf (buffer, "%s %s %s %d %s %d\n", annotUser, annotFile, labf, &c1, labl, &cN);
    if ((el = TtaSearchElementByLabel (labf, body)) == NULL)
      fprintf (stderr, "This annotations has lost its parent!\n");
    else
      LINK_AddLink (doc, annotFile, labf, c1, labl, cN);
  }
  
  TtaFreeMemory (annotFile);
  TtaFreeMemory (annotUser);
  TtaFreeMemory (labf);
  TtaFreeMemory (labl);
  fclose (fp);
}

/***************************************************
 I've not yet used/cleaning the following legacy functions 
***************************************************/

/*-----------------------------------------------------------------------
   Procedure LINK_Remove (document, annotName)
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_Remove (Document document, STRING annotName)
#else /* __STDC__*/
void LINK_Remove (document, annotName)
     Document document;
     STRING   annotName;
#endif /* __STDC__*/
{
  ElementType   elType;
  Element       anchor, elCour, elSuiv;
  AttributeType attrTypeAnnot, attrTypeName;
  Attribute     attrAnnot, attrName;
  int           lg;
  ThotBool       removeAnnot;

  STRING textClass = TtaGetMemory (200);
  STRING textName =  TtaGetMemory (200);

  elCour = SearchElementInDoc (document, HTML_EL_BODY);
  elType.ElSSchema          = 
  attrTypeAnnot.AttrSSchema = 
  attrTypeName.AttrSSchema  = TtaGetSSchema ("HTML", document);
  elType.ElTypeNum = HTML_EL_Anchor;
  attrTypeAnnot.AttrTypeNum = HTML_ATTR_Annotation;
  attrTypeName.AttrTypeNum = HTML_ATTR_NAME;

  removeAnnot = FALSE;
  anchor = TtaSearchTypedElement (elType, SearchForward, elCour);
  while (elCour && anchor && !removeAnnot)
  {
    /* Searches for an element of type anchor */
    if (anchor)
    {
      attrAnnot = TtaGetAttribute (anchor, attrTypeAnnot);
      attrName = TtaGetAttribute (anchor, attrTypeName);
      if (attrAnnot)
      {
        TtaGiveTextAttributeValue (attrName, textName, &lg);
        removeAnnot = !strcmp (textName, annotName);
	/* Suppress the annotation */
	if (attrAnnot && removeAnnot)
	  {
	    TtaRemoveTree (anchor, document);
	    /* @and unlink the file, update the index ... */
	    break;
	  }
      }
      /* point to the next element */
      elCour = elSuiv = anchor;
      TtaNextSibling (&elSuiv);
      while ((elSuiv == NULL) && (elCour != NULL))
      {
        elCour = elSuiv = TtaGetParent (elCour);
        TtaNextSibling (&elSuiv);
      }
      elCour = elSuiv;
    }
    else
      elCour = anchor;
    anchor = TtaSearchTypedElement (elType, SearchForward, elCour);
  }
}

#if 0
/*-----------------------------------------------------------------------
   Procedure LINK_SaveLink (doc, annotName, labf, c1, labl, cN)
  -----------------------------------------------------------------------
   Writes the metadata describing an annotation file and its source
   link to an index file. This metadata consists of the
   name of the annotation file, the selected elements on the document
   (using thot labels for the moment), and the first and last characters.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_SaveLink (Document document, STRING annotName, STRING labf, int c1, STRING labl, int cN)
#else /* __STDC__*/
void LINK_SaveLink (document, annotName, labf, c1, labl, cN)
     Document document;
     STRING   annotName;
     STRING   labf;
     int      c1;
     STRING   labl;
     int      cN;
#endif /* __STDC__*/
{
  FILE   *newLinkFile, *oldLinkFile;
  char   buffer[MAX_LENGTH];
  STRING newFileName;
  STRING oldFileName;

  /* Open the annotation index */
  oldFileName = LINK_GetAnnotationIndexFile (document);
  if (!oldFileName)
    {
      
    }

  newFileName = TtaGetMemory (strlen (oldFileName) + 5);
  sprintf (newFileName, "%s.new", oldFileName);
  oldLinkFile = fopen (oldFileName, "r");

  if (oldLinkFile)
    {
      /* Copy the existing links */
      while (fgets (buffer, MAX_LENGTH, oldLinkFile))
	fprintf (newLinkFile, buffer);
      fclose (oldLinkFile);
    }
  
  /* Add the new link */
  fprintf (newLinkFile, "%s|%s|%d|%s|%d\n", annotName, labf, c1, labl, cN);

  fclose (newLinkFile);
  TtaFileCopy (newFileName, oldFileName);
  TtaFileUnlink (newFileName);
  TtaFreeMemory (newFileName);
  TtaFreeMemory (oldFileName);
}
#endif

/*-----------------------------------------------------------------------
   Procedure LINK_RemoveLink (document, annotName)
  -----------------------------------------------------------------------
   Enleve le lien du document d'annotation dans le fichier de liens
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_RemoveLink (Document document, STRING annotName)
#else /* __STDC__*/
void LINK_RemoveLink (document, annotName)
     Document document;
     STRING   annotName;
#endif /* __STDC__*/
{
  STRING   newFileName, oldFileName, docName, userName, labf, labl;
  FILE     *newLinkFile, *oldLinkFile;
  int      nbAnnot, c1, cN, i;

  printf ("(LINK_RemoveLink) DEBUT\n");

  /* Ouverture fichier de liens */
  oldFileName = TtaGetMemory (100);
  sprintf (oldFileName, "%s/%s", GetAnnotDir (), ANNOT_INDEX_SUFFIX);
  newFileName = TtaGetMemory (100);
  sprintf (newFileName, "%s.new", oldFileName);

#if 0
  oldLinkFile = fileOpen (oldFileName);
  newLinkFile = fileCreateAndOpen (newFileName);
#endif
  
  /* Mise a jour du nombre d'annotations */
  fscanf (oldLinkFile, "%d\n", &nbAnnot);
  nbAnnot--;
  fprintf (newLinkFile, "%d\n", nbAnnot);

  /* On recopie les liens deja existants */
  annotName = TtaGetMemory (MAX_LENGTH);
  userName = TtaGetMemory (MAX_LENGTH);
  labf = TtaGetMemory (10);
  labl = TtaGetMemory (10);
  for (i=0 ; i<nbAnnot ; i++)
  {
    fscanf (oldLinkFile, "%s %s %s %d %s %d\n", userName, docName, labf, &c1, labl, &cN);
    if (strcmp (docName, annotName))
      fprintf (newLinkFile, "%s|%s|%d|%s|%d\n", docName, labf, c1, labl, cN);
  }

  /* Fermeture du fichier de liens */
#if 0
  fileClose (oldLinkFile, oldFileName);
  fileClose (newLinkFile, newFileName);
#endif
  TtaFileCopy (newFileName, oldFileName);
  TtaFileUnlink (newFileName);

  printf ("(LINK_RemoveLink) FIN\n");
}

/*-----------------------------------------------------------------------
   Procedure LINK_HideAnnotations (document)
  -----------------------------------------------------------------------
   Enleve tous les liens d'annotation dans un document donne
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_HideAnnotations (Document document)
#else /* __STDC__*/
void LINK_HideAnnotations (document)
     Document document;
#endif /* __STDC__*/
{
  ElementType   elType;
  Element       anchor, elCour, elSuiv;
  AttributeType attrType;
  Attribute     attr;
  STRING        text;
  int           *text_lg;
  ThotBool       removeAnnot;

  elCour = SearchElementInDoc (document, HTML_EL_BODY);
  elType.ElSSchema = TtaGetSSchema ("HTML", document);
  elType.ElTypeNum = HTML_EL_Anchor;
  attrType.AttrSSchema = TtaGetSSchema ("HTML", document);
  attrType.AttrTypeNum = HTML_ATTR_Class;
  text = TtaGetMemory (200);
  text_lg = TtaGetMemory (4);

  while (elCour && anchor)
  {
    /* Recherche d'un element de type anchor */
    anchor = TtaSearchTypedElement (elType, SearchForward, elCour);
    if (anchor)
    {
      attr = TtaGetAttribute (anchor, attrType);
      if (attr)
      {
        TtaGiveTextAttributeValue (attr, text, text_lg);
        removeAnnot = !strcmp (text, "Annotation");
      }
      /* Passage a l'element suivant */
      elCour = elSuiv = anchor;
      TtaNextSibling (&elSuiv);
      while ((elSuiv == NULL) && (elCour != NULL))
      {
        elCour = elSuiv = TtaGetParent (elCour);
        TtaNextSibling (&elSuiv);
      }
      elCour = elSuiv;
      /* Suppression de l'annotation */
      if (attr && removeAnnot)
      {
        TtaRemoveTree (anchor, document);
      }
    }
  }
}

/*-----------------------------------------------------------------------
   Procedure LINK_ParcoursAnchor (document)
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_ParcoursAnchor (Document document)
#else /* __STDC__*/
void LINK_ParcoursAnchor (document)
     Document document;
#endif /* __STDC__*/
{
  ElementType elType;
  Element     body, elCour;

  STRING anchorClass = TtaGetMemory (20);
  STRING anchorName  = TtaGetMemory (100);

  body = SearchElementInDoc (document, HTML_EL_BODY);
  elType = TtaGetElementType (body);
  elType.ElTypeNum = HTML_EL_Anchor;
  elCour = TtaSearchTypedElement (elType, SearchForward, body);
  while (elCour != NULL)
  {
    anchorClass = SearchAttributeInElt (document, elCour, HTML_ATTR_Class);
    if (anchorClass == NULL)
      printf ("Pas une annotation\n");
    else
    {
      anchorName = SearchAttributeInElt (document, elCour, HTML_ATTR_NAME);
      printf ("ANNOTATION : %s\n", anchorName);
    }
    elCour = TtaGetSuccessor (elCour);
    elCour = TtaSearchTypedElement (elType, SearchForward, elCour);
  }
}

/*-----------------------------------------------------------------------
   Procedure LINK_UpdateAnnotations (document)
  -----------------------------------------------------------------------
   Rafraichit l'affichage des liens d'un document : les liens existants
   sont laisses, les liens manquants sont rajoutes.
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_UpdateAnnotations (Document document)
#else /* __STDC__*/
void LINK_UpdateAnnotations (document)
     Document document;
#endif /* __STDC__*/
{
  Element       body, el;
  View          view;
  STRING        fileName, docName, prem, der;
  FILE          *linkFile;
  int            nbAnnot, c_prem, c_der, i;

  /* Ouverture fichier de liens */
  fileName = TtaGetMemory (100);
  sprintf (fileName, "%s/%s", GetAnnotDir (), ANNOT_INDEX_SUFFIX);
#if 0
  if ((linkFile = fileOpen (fileName)) == NULL)
    return;
#endif
  /* Lecture du nombre d'annotations */
  fscanf (linkFile, "%d\n", &nbAnnot);

  /* Parcours et affichage de la liste de liens */
  docName = TtaGetMemory (50);
  prem    = TtaGetMemory (10);
  der     = TtaGetMemory (10);
  view = TtaGetViewFromName (document, "Formatted_view");
  body = SearchElementInDoc (document, HTML_EL_BODY);
  for (i=0 ; i<nbAnnot ; i++)
  {
    fscanf (linkFile, "%s %s %d %s %d\n", docName, prem, &c_prem, der, &c_der);
    if ((el = TtaSearchElementByLabel (prem, body)) == NULL)
    {
      printf ("Cette pauvre petite annotation est orpheline....\n");
    }
    else if (!SearchAnnotation (document, docName))
    {
#if 0
      LINK_AddLink (document, view, el, docName);
#endif
    }
  }
#if 0
  fileClose (linkFile, fileName);
#endif
}





