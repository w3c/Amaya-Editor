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
  CHAR_T *annot_dir;
  CHAR_T *annot_main_index;
  CHAR_T *annot_main_index_file;
  CHAR_T url[MAX_LENGTH];
  CHAR_T buffer[MAX_LENGTH];
  CHAR_T *index_file;
  FILE *fp;
  
  annot_dir = GetAnnotDir ();
  annot_main_index = GetAnnotMainIndex ();
  annot_main_index_file = TtaGetMemory (ustrlen (annot_dir) 
					+ ustrlen (annot_main_index) 
					+ 10);
  usprintf (annot_main_index_file, TEXT("%s%c%s"), 
	    annot_dir, 
	    DIR_SEP,  
	    annot_main_index);
  
  if (TtaFileExist (annot_main_index_file))
    {
      index_file = TtaGetMemory (MAX_PATH);
      found = 0;
      if ((fp = fopen (annot_main_index_file, "r")))
	{
	  while (ufgets (buffer, MAX_LENGTH, fp))
	    {
	      usscanf (buffer, TEXT("%s %s\n"), url, index_file);
	      if (!ustrcasecmp (source_url, url))
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

  TtaFreeMemory (annot_main_index_file);
  
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
  CHAR_T *annot_dir;
  CHAR_T *annot_main_index;
  CHAR_T *annot_main_index_file;
  FILE *fp;
  
  annot_dir = GetAnnotDir ();
  annot_main_index = GetAnnotMainIndex ();
  annot_main_index_file = TtaGetMemory (ustrlen (annot_dir) 
					+ ustrlen (annot_main_index)
					+ 10);
  usprintf (annot_main_index_file, 
	    TEXT("%s%c%s"), 
	    annot_dir, 
	    DIR_SEP, 
	    annot_main_index);

  if ((fp = fopen (annot_main_index_file, "a")))
    {
      ufprintf (fp, TEXT("%s %s\n"), source_url, index_file);
      fclose (fp);
    }
  TtaFreeMemory (annot_main_index_file);
}

/*-----------------------------------------------------------------------
  LINK_AddLinkToSource
   Ajoute un lien de type annotation dans un document donne vers le
   document de nom annotName. Le lien est cree juste avant l'element
   target
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_AddLinkToSource (Document source_doc, AnnotMeta *annot)
#else /* __STDC__*/
void LINK_AddLinkToSource (source_doc, annot)
Document source_doc; 
AnnotMeta *annot;
#endif /* __STDC__*/
{
  ElementType   elType;
  Element       el, first, anchor;
  AttributeType attrType;
  Attribute     attr;
  CHAR_T       *annotName;
  CHAR_T       *annot_user;

  annot_user = GetAnnotUser ();
  
  first   = TtaSearchElementByLabel(annot->labf, TtaGetMainRoot (source_doc));
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
  TtaSetAttributeText (attr, annot->body_url, anchor, source_doc);

  /* add a NAME attribute so that the annotation doc can point
     back to the source of the annotation link */
  attrType.AttrTypeNum = HTML_ATTR_NAME;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);
  annotName = TtaGetMemory (strlen (ANNOT_ANAME) + strlen (annot_user)
			    + strlen (annot->body_url) + 20);
  sprintf (annotName, "%s_%s_%s", ANNOT_ANAME, annot_user, annot->body_url);
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
void LINK_SaveLink (Document source_doc)
#else /* __STDC__*/
void LINK_SaveLink (source_doc)
     Document source_doc;
#endif /* __STDC__*/
{
  char   *annot_user;
  char   *indexName;
  AnnotMeta *annot;
  List   *annot_list;

  /* Open the annotation index */
  indexName = LINK_GetAnnotationIndexFile (DocumentURLs[source_doc]);
  if (!indexName)
    {
      indexName = GetTempName (GetAnnotDir (), "index");
      AddAnnotationIndexFile (DocumentURLs[source_doc], indexName);
    }

  annot_list = AnnotMetaDataList[source_doc];  
  /* write the update annotation list */
  AnnotList_writeIndex (indexName, annot_list);
  TtaFreeMemory (indexName);
  return;
}

/*-----------------------------------------------------------------------
  LINK_CreateMeta
  -----------------------------------------------------------------------*/

#ifdef __STDC__
AnnotMeta* LINK_CreateMeta (Document source_doc, Document annot_doc, CHAR_T *labf, int c1, CHAR_T *labl, int cl)
#else /* __STDC__*/
AnnotMeta* LINK_CreateMeta (source_doc, annot_doc, labf, c1, labl, cl)
     Document source_doc;
     Document annot_doc;
     CHAR_T *  labf;
     int      c1;
     CHAR_T *  labl;
     int      cl;
#endif /* __STDC__*/
{
  AnnotMeta   *annot;
  List        *annot_list;
  CHAR_T      *annot_user;

  /*
  **  Make a new annotation entry, add it to annotlist, and initialize it.
  */
  annot =  AnnotMeta_new ();
  annot_list = AnnotMetaDataList[source_doc];
  List_add (&annot_list, (void *) annot);
  if (! AnnotMetaDataList[source_doc])
    AnnotMetaDataList[source_doc] = annot_list;

  annot->source_url = TtaStrdup (DocumentURLs[source_doc]);

  ustrcpy (annot->labf, labf);
  annot->c1 = c1;
  ustrcpy (annot->labl, labl);
  annot->cl = cl;

  /* get the current date... cdate = mdate at this stage */
  annot->cdate = StrdupDate ();
  annot->mdate = TtaStrdup (annot->cdate);

  /* Annotation type */
  annot->type = TEXT("Proto-Annotation");

  annot_user = GetAnnotUser ();
  annot->author = TtaStrdup (annot_user);
  annot->content_type = TtaStrdup ("text/html");
  annot->body_url = TtaStrdup (DocumentURLs[annot_doc]); 
  return annot;
}

/*-----------------------------------------------------------------------
  Procedure LINK_DelMetaToMemory
  -----------------------------------------------------------------------
  Frees the memory used by the parsed metadata
  -----------------------------------------------------------------------*/
void LINK_DelMetaFromMemory (Document doc)
{
  if (!AnnotMetaDataList[doc])
    return;

  AnnotList_free (AnnotMetaDataList[doc]);
  AnnotMetaDataList[doc] = NULL;
}

/*-----------------------------------------------------------------------
   Procedure LINK_LoadAnnotationIndex (doc, char *annotIndex)
  -----------------------------------------------------------------------
   Searches for an annotation index related to the document. If it exists,
   it parses it and then displays all of its annotations
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_LoadAnnotationIndex (Document doc, CHAR_T *annotIndex)
#else /* __STDC__*/
void LINK_LoadAnnotationIndex (doc, annotIndex)
     Document doc;
     CHAR_T *annotIndex;
#endif /* __STDC__*/
{
  View    view;
  Element el, body;
  List *annot_list, *list_ptr;
  AnnotMeta *annot;

  if (!annotIndex || !(TtaFileExist (annotIndex)))
    /* there are no annotations */
    return;
  
  annot_list = RDF_parseFile (annotIndex, ANNOT_LIST);

  if (!annot_list)
    /* we didn't read any annotation */
    return;

  /* Insert the annotations in the body */
  /* @@@ possible memory bug */
  view = TtaGetViewFromName (doc, "Formatted_view");
  body = SearchElementInDoc (doc, HTML_EL_BODY);
  
  list_ptr = annot_list;

  while (list_ptr)
    {
      annot = (AnnotMeta *) list_ptr->object;
    if ((el = TtaSearchElementByLabel (annot->labf, body)) == NULL)
      /* @@ perhaps I should erase it from the list? */
      fprintf (stderr, "This annotation has lost its parent!\n");
    else 
      {
	LINK_AddLinkToSource (doc, annot);
	AnnotMetaDataList[doc] = annot_list;
      }
    /* @@@ */
    list_ptr = list_ptr->next;
  }
}

/***************************************************
 I've not yet used/cleaning the following legacy functions 
***************************************************/

/*-----------------------------------------------------------------------
   Procedure LINK_Remove (document, annotName)
  -----------------------------------------------------------------------
  -----------------------------------------------------------------------*/

#ifdef __STDC__
void LINK_Remove (Document document, CHAR_T *annotName)
#else /* __STDC__*/
void LINK_Remove (document, annotName)
     Document document;
     CHAR_T *  annotName;
#endif /* __STDC__*/
{
  ElementType   elType;
  Element       anchor, elCour, elSuiv;
  AttributeType attrTypeAnnot, attrTypeName;
  Attribute     attrAnnot, attrName;
  int           lg;
  ThotBool       removeAnnot;

  CHAR_T *textName =  TtaGetMemory (200);

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
void LINK_SaveLink (Document document, CHAR_T *annotName, CHAR_T *labf, int c1, CHAR_T *labl, int cN)
#else /* __STDC__*/
void LINK_SaveLink (document, annotName, labf, c1, labl, cN)
     Document document;
     CHAR_T *  annotName;
     CHAR_T *  labf;
     int      c1;
     CHAR_T *  labl;
     int      cN;
#endif /* __STDC__*/
{
  FILE   *newLinkFile, *oldLinkFile;
  char   buffer[MAX_LENGTH];
  CHAR_T *newFileName;
  CHAR_T *oldFileName;

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
void LINK_RemoveLink (Document document, CHAR_T *annotName)
#else /* __STDC__*/
void LINK_RemoveLink (document, annotName)
     Document document;
     CHAR_T *  annotName;
#endif /* __STDC__*/
{
  CHAR_T   *newFileName, *oldFileName, *docName, *userName, *labf, *labl;
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
  CHAR_T *       text;
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

  CHAR_T *anchorClass = TtaGetMemory (20);
  CHAR_T *anchorName  = TtaGetMemory (100);

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
  CHAR_T        *fileName, *docName, *prem, *der;
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
      LINK_AddLinkToSource (document, view, el, docName);
#endif
    }
  }
#if 0
  fileClose (linkFile, fileName);
#endif
}

#if 0
/*-----------------------------------------------------------------------
   Procedure LINK_AddMetaToMemory
  -----------------------------------------------------------------------
  Copies the parsed metadata to memory
  -----------------------------------------------------------------------*/
void LINK_AddMetaToMemory (Document doc, CHAR_T *annotUser, CHAR_T *annotDate, CHAR_T *annotType, CHAR_T *body_url)
{
  AnnotMeta *me;

  /* create a new element */
  me = TtaGetMemory (sizeof (AnnotMeta));
  me->author = TtaStrdup (annotUser);
  me->cdate = TtaStrdup (annotDate);
  me->type = TtaStrdup (annotType);
  me->body_url = TtaStrdup (body_url);

  /* add it to the list structure */
  if (!AnnotMetaDataList[doc]) 
    {
      /* the list was empty */
      me->next = NULL;
      AnnotMetaDataList[doc] = me;
    }
  else
    {
      /* adding new elements to the list */
      me->next = AnnotMetaDataList[doc];
      AnnotMetaDataList[doc] = me;
    }
}

#endif
