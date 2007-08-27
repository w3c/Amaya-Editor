/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999-2007
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

/* annotlib includes */
#include "annotlib.h"
#include "ANNOTlink_f.h"
#include "ANNOTschemas_f.h"
#include "ANNOTevent_f.h"
#include "ANNOTfiles_f.h"
#include "ANNOTtools_f.h"
#include "AHTrdf2annot_f.h"

/* Amaya includes */
#include "XPointer.h"
#include "XPointerparse_f.h"
#include "init_f.h"
#include "XLinkedit_f.h"
#include "fetchXMLname_f.h"
#include "AHTURLTools_f.h"
#include "HTMLactions_f.h"

/* schema includes */
#include "XLink.h"
#include "HTML.h"
#include "MathML.h"
#include "SVG.h"
#include "XML.h"

/* thotlib includes */
#include "content.h"

/*-----------------------------------------------------------------------
  LINK_CreateAName
  Creates an anchor name for the reverse link from the annotation to
  the document
  -----------------------------------------------------------------------*/
static void LINK_CreateAName (AnnotMeta *annot)
{
  char *ptr;
  char *author = annot->author ? annot->author : (char *)"";

  /* memorize the anchor of the reverse link target */
  annot->name = (char *)TtaGetMemory (strlen (ANNOT_ANAME) + strlen (author)
                                      + (annot->body_url 
                                         ? strlen (annot->body_url) : 0)
                                      + 20);
  sprintf (annot->name, "%s_%s_%s", ANNOT_ANAME, author,
           annot->body_url ? annot->body_url : "");
  /* and remove all the ? links so that it all becomes a link, and not a target */
  ptr = annot->name;
  while (*ptr)
    {
      if (*ptr == '?')
        *ptr = '_';
      ptr++;
    }
}

/*-----------------------------------------------------------------------
  LINK_GetAnnotationIndexFile 
  searches an entry in the main annot index file 
  The caller must free the returned string.
  -----------------------------------------------------------------------*/
char  *LINK_GetAnnotationIndexFile (char *source_url)
{
  int found;
  char *annot_dir;
  char *annot_main_index;
  char *annot_main_index_file;
  char *local_source_url;
  char url[MAX_LENGTH];
  char buffer[MAX_LENGTH];
  char *index_file;
  FILE *fp;

  if (!source_url)
    return NULL;

  annot_dir = GetAnnotDir ();
  annot_main_index = GetAnnotMainIndex ();
  annot_main_index_file = (char *)TtaGetMemory (strlen (annot_dir) 
                                                + strlen (annot_main_index) 
                                                + 10);
  sprintf (annot_main_index_file, "%s%c%s", 
           annot_dir, 
           DIR_SEP,  
           annot_main_index);
  local_source_url = TtaStrdup (source_url);
  WWWToLocal (local_source_url);

  if (TtaFileExist (annot_main_index_file))
    {
      index_file = (char *)TtaGetMemory (MAX_PATH);
      found = 0;
      if ((fp = TtaReadOpen (annot_main_index_file)))
        {
          while (fgets (buffer, MAX_LENGTH, fp))
            {
              sscanf (buffer, "%s %s\n", url, index_file);
              /* convert local URLs into local file system */
              WWWToLocal (url);
              WWWToLocal (index_file);
              if (!strcasecmp (local_source_url, url))
                {
                  found = 1;
                  break;
                }
            }
          TtaReadClose (fp);
        }
      if (!found)
        {
          TtaFreeMemory (index_file);
          index_file = NULL;
        }
    }
  else
    index_file = NULL;

  TtaFreeMemory (local_source_url);
  TtaFreeMemory (annot_main_index_file);

  return (index_file);
}

/*-----------------------------------------------------------------------
  LINK_UpdateAnnotationIndexFile
  old_source_url and new_source_url must be complete URLs.
  -----------------------------------------------------------------------*/
void LINK_UpdateAnnotationIndexFile (char *old_source_url, char *new_source_url)
{
  char *annot_dir;
  char *annot_main_index;
  char *annot_main_index_file;
  char *annot_new_main_index_file;
  char url[MAX_LENGTH];
  char index_file[MAX_LENGTH];
  char buffer[MAX_LENGTH];
  char *tmp_url;
  char *tmp_index_file;
  FILE *fp_old, *fp_new;
  ThotBool move;

  annot_dir = GetAnnotDir ();
  annot_main_index = GetAnnotMainIndex ();
  annot_main_index_file = (char *)TtaGetMemory (strlen (annot_dir) 
                                                + strlen (annot_main_index) 
                                                + 10);
  sprintf (annot_main_index_file, "%s%c%s", 
           annot_dir, 
           DIR_SEP,  
           annot_main_index);
  annot_new_main_index_file = (char *)TtaGetMemory (strlen (annot_dir) 
                                                    + strlen (annot_main_index) 
                                                    + 11);
  sprintf (annot_new_main_index_file, "%s%ct%s", 
           annot_dir, 
           DIR_SEP,  
           annot_main_index);

  move = FALSE;
  if (TtaFileExist (annot_main_index_file))
    {
      if ((fp_old = TtaReadOpen (annot_main_index_file)))
        {
          if ((fp_new = TtaWriteOpen (annot_new_main_index_file)))
            {
              while (fgets (buffer, MAX_LENGTH, fp_old))
                {
                  sscanf (buffer, "%s %s\n", url, index_file);
                  /* convert local URLs into file: ones */
                  if (!IsFilePath (url) && !IsW3Path (url))
                    tmp_url = LocalToWWW (url);
                  else
                    tmp_url = url;
                  if (!IsFilePath (index_file) && !IsW3Path (index_file))
                    tmp_index_file = LocalToWWW (index_file);
                  else
                    tmp_index_file = index_file;
                  if (!strcasecmp (old_source_url, tmp_url))
                    fprintf (fp_new, "%s %s\n", new_source_url, tmp_index_file);
                  else
                    fprintf (fp_new, "%s %s\n", tmp_url, tmp_index_file);
                  if (tmp_url != url)
                    TtaFreeMemory (tmp_url);
                  if (tmp_index_file != index_file)
                    TtaFreeMemory (tmp_index_file);
                }
              TtaWriteClose (fp_new);
              move = TRUE;
            }
          TtaReadClose (fp_old);
        }
    }
  
  if (move)
    {
      TtaFileCopy (annot_new_main_index_file, annot_main_index_file);
      TtaFileUnlink (annot_new_main_index_file);
    }

  TtaFreeMemory (annot_main_index_file);
  TtaFreeMemory (annot_new_main_index_file);
}

/*-----------------------------------------------------------------------
  AddAnnotationIndexFile 
  adds a new entry to the main annot index file
  -----------------------------------------------------------------------*/
static void AddAnnotationIndexFile (char *source_url, char *index_file)
{
  char *annot_dir;
  char *annot_main_index;
  char *annot_main_index_file;
  char *www_source_url;
  char *www_index_file;

  FILE *fp;
  
  annot_dir = GetAnnotDir ();
  annot_main_index = GetAnnotMainIndex ();
  annot_main_index_file = (char *)TtaGetMemory (strlen (annot_dir) 
                                                + strlen (annot_main_index)
                                                + 10);
  www_source_url = LocalToWWW (source_url);
  www_index_file = LocalToWWW (index_file);
  sprintf (annot_main_index_file, 
           "%s%c%s", 
           annot_dir, 
           DIR_SEP, 
           annot_main_index);

  if ((fp = TtaAddOpen (annot_main_index_file)))
    {
      fprintf (fp, "%s %s\n", 
               (www_source_url) ? www_source_url : source_url,
               (www_index_file) ? www_index_file : index_file);
      TtaWriteClose (fp);
    }
  if (www_source_url)
    HT_FREE (www_source_url);
  if (www_index_file)
    TtaFreeMemory (www_index_file);
  TtaFreeMemory (annot_main_index_file);
}

/*-----------------------------------------------------------------------
  LINK_AddAnnotIcon
  If the annotation is not orphan, adds an annotation icon to a source 
  document, an annotation link pointing to the annotation.
  Returns TRUE if the annotation could be attached and FALSE if the
  annotation became orphan.
  -----------------------------------------------------------------------*/
void LINK_AddAnnotIcon (Document source_doc, Element anchor, AnnotMeta *annot)
{
  Element       el;
  char          iconName[MAX_LENGTH];
  char          previous[MAX_LENGTH];
  RDFStatementP iconS = (RDFStatementP) NULL;
  int           len;
  Language      lang;

  el = TtaGetFirstChild (anchor);
  
  if (!PROP_usesIcon)
    PROP_usesIcon = ANNOT_FindRDFResource (&annot_schema_list,
                                           USESICON_PROPNAME,
                                           TRUE);

  if (annot->type)
    iconS = ANNOT_FindRDFStatement (annot->type->statements, PROP_usesIcon);
  
  if (iconS)
    {
      char *ptr;
      char *path;
      char temp[MAX_LENGTH];

      /* @@@ JK? */
      strcpy (iconName, iconS->object->name);
      WWWToLocal (iconName);
      /* expand $THOTDIR and $APP_HOME */
      ptr = strstr (iconName, "$THOTDIR");
      if (ptr)
        {
          ptr += 8;
          path = TtaGetEnvString ("THOTDIR");
        }
      else
        {
          ptr = strstr (iconName, "$APP_HOME");
          if (ptr)
            {
              ptr += 9;
              path = TtaGetEnvString ("APP_HOME");
            }
        }
      if (ptr)
        {
          strcpy (temp, ptr);
          strcpy (iconName, path);
          strcat (iconName, temp);
        }
    }
  else
    sprintf (iconName, "%s%camaya%cannot.png",
             TtaGetEnvString ("THOTDIR"), DIR_SEP, DIR_SEP);

  /* only substitute the icon name if it has changed */
  len = TtaGetTextLength (el);
  if (len > 0 && len < (int) (sizeof (previous)))
    {
      TtaGiveTextContent (el, (unsigned char *)previous, &len, &lang);
      previous[len] = EOS;
    }
  else
    previous[0] = EOS;

  if (previous[0] == EOS || strcasecmp (iconName, previous))
    {
      ThotBool docModified;

      docModified = TtaIsDocumentModified (source_doc);
      TtaSetPictureContent (el, (unsigned char *)iconName, SPACE, source_doc, "image/png");
      if (!docModified)
        {
          TtaSetDocumentUnmodified (source_doc);
          DocStatusUpdate (source_doc, docModified);
        }
    }
}

/*-----------------------------------------------------------------------
  LINK_UpdateAnnotIcon
  When changing annotation types, updates the icon that corresponds to the
  type.
  -----------------------------------------------------------------------*/
void LINK_UpdateAnnotIcon (Document source_doc, AnnotMeta *annot)
{
  Element anchor;

  if (!annot || !annot->name || annot->is_orphan)
    return;

  anchor = SearchAnnotation (source_doc, annot->name);
  if (!anchor)
    return;

  LINK_AddAnnotIcon (source_doc, anchor, annot);
}

/*-----------------------------------------------------------------------
  LINK_AddLinkToSource
  Adds to a source document, an annotation link pointing to the annotation.
  Returns TRUE if the annotation could be attached and FALSE if the
  annotation became orphan.
  -----------------------------------------------------------------------*/
ThotBool LINK_AddLinkToSource (Document source_doc, AnnotMeta *annot)
{
  ElementType   elType, firstElType;
  Element       el, first, anchor, child_el;
  AttributeType attrType;
  Attribute     attr;
  SSchema       XLinkSchema;
  char         *docSchemaName, *CharNum;
  int           c1, cN, Char, line, length;
  int           length_el, line_el, count_child;
  ThotBool      check_mode;

  /*annot_user = GetAnnotUser ();*/
  if (DocumentTypes[source_doc] != docText)
    {
      if (annot->xptr)
        {
          XPointerContextPtr ctx;
          nodeInfo *node;
          ctx = XPointer_parse (source_doc, annot->xptr);
          node = XPointer_nodeStart (ctx);
          first = XPointer_el (node);
          c1 = XPointer_startC (node);
          cN = XPointer_endC (node);
          XPointer_free (ctx);
        }
      else
        {
          first  = TtaSearchElementByLabel(annot->labf, 
                                           TtaGetRootElement (source_doc));
          c1 = annot->c1;
          cN = annot->cl;
        }
    }
  else
    {
      first = SearchTextattribute (source_doc, annot->xptr);
      line = TtaGetElementLineNumber (first);
      CharNum = strstr (annot->xptr, "=");
      CharNum = &CharNum[1];
      Char = atoi (CharNum);
      c1 = CharNum_IN_Line (source_doc, Char);

      if(c1 == -1)
        c1 = 0;
      cN = c1 - 1;

      length = TtaGetElementVolume (first);
      el = TtaGetMainRoot (source_doc);
      elType = TtaGetElementType (el);
      elType.ElTypeNum = TextFile_EL_Line_;
      el = TtaSearchTypedElement (elType, SearchForward, el);
      for (line_el=1; line_el < line; line_el++)
        TtaNextSibling (&el);

      child_el = TtaGetFirstChild(el);
      length_el = TtaGetElementVolume(child_el);
      count_child = 1;
      while(child_el != first)
        {
          if(count_child % 3 != 2)
            {
              c1 -= length_el;
              cN = c1 - 1;
            }
          count_child++;
          TtaNextSibling(&child_el);
          length_el = TtaGetElementVolume(child_el);
        }
    }

  if (first)
    /* we found it */
    annot->is_orphan = FALSE;
  else
    {
      /* it's an orphan annotation */
      first = TtaGetRootElement (source_doc);
      annot->is_orphan = TRUE;
    }
  
#ifdef ANNOT_ON_ANNOT
  /* don't add the Xlink element for replies */
  if (annot->inReplyTo)
    return (!(annot->is_orphan));
#endif /* ANNOT_ON_ANNOT */

  /* Verify that the annotates property really does point to this document
     and discard if not (or if there is no annotates property). */
  if (!Annot_isSameURL (annot->source_url, DocumentURLs[source_doc]))
    return TRUE;		/* treat this as not orphanned */

  /* create the anotation element */
  XLinkSchema = GetXLinkSSchema (source_doc);
  elType.ElSSchema = XLinkSchema;
  elType.ElTypeNum = XLink_EL_XLink;
  anchor = TtaNewTree (source_doc, elType, "");
  
  /*
  ** insert the anchor in the document tree
  */

  firstElType = TtaGetElementType (first);
  docSchemaName = TtaGetSSchemaName (firstElType.ElSSchema);

  check_mode = TtaGetStructureChecking (source_doc);
  TtaSetStructureChecking (FALSE, source_doc);
  /* @@ JK: this doesn't seem useful anymore */
  /* is the user trying to annotate an annotation? */
  el = TtaGetTypedAncestor (first, elType);
  if (el)
    {
      /* yes, so we will add the anchor one element before
         in the struct tree */
      TtaInsertSibling (anchor, el, TRUE, source_doc);
    }
  else if (!strcmp (docSchemaName, "MathML"))
    {
      /* An annotation on a MathMl structure. We backtrace the tree
         until we find the Math root element and then add the annotation as 
         its first child. */
      el = first;
      while (el)
        {
          elType = TtaGetElementType (el);
          if (elType.ElTypeNum == MathML_EL_MathML)
            break;
          el = TtaGetParent (el);
        }
      TtaInsertFirstChild (&anchor, el, source_doc);
    }
  else if (!strcmp (docSchemaName, "SVG"))
    {
      /* An annotation on an SVG structure. We backtrace the tree
         until we find the SVG root element and then add the annotation as
         its first child. */
      el = first;
      while (el)
        {
          elType = TtaGetElementType (el);
          if (elType.ElTypeNum == SVG_EL_SVG)
            break;
          el = TtaGetParent (el);
        }
      TtaInsertFirstChild (&anchor, el, source_doc);
    }
  else if (!strcmp (docSchemaName, "HTML"))
    {
      /* it's an HTML document */
      el = first;
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == HTML_EL_HTML
          || elType.ElTypeNum == HTML_EL_HEAD
          || elType.ElTypeNum == HTML_EL_TITLE
          || elType.ElTypeNum == HTML_EL_BASE
          || elType.ElTypeNum == HTML_EL_META
          || elType.ElTypeNum == HTML_EL_SCRIPT_
          || elType.ElTypeNum == HTML_EL_STYLE_
          || elType.ElTypeNum == HTML_EL_BODY)
        {
          /* we can't put the annotation icon here, so let's find
             the first child of body and add it to it */
          el = TtaGetRootElement (source_doc);
          elType.ElTypeNum = HTML_EL_BODY;
          el = TtaSearchTypedElement (elType, SearchForward, el);
          if (el)
            /* add it to the beginning */
            TtaInsertFirstChild (&anchor, el, source_doc);
          else
            /* we add it where it was declared, although we may
               not see it at all */
            TtaInsertSibling (anchor, first, TRUE, source_doc);
        }
      else if (elType.ElTypeNum == HTML_EL_Unnumbered_List
               || elType.ElTypeNum == HTML_EL_Numbered_List
               || elType.ElTypeNum == HTML_EL_List_Item
               || elType.ElTypeNum == HTML_EL_Definition)
        {
          /* for lists, we attach the A-element to the first pseudo
             paragraph. The .S forbids doing so elsewhere */
          elType.ElTypeNum = HTML_EL_Pseudo_paragraph;
          el = TtaSearchTypedElement (elType, SearchForward, el);
          if (el)
            /* add it to the beginning */
            TtaInsertFirstChild (&anchor, el, source_doc);
          else
            /* we add it where it was declared, although we may
               not see it at all */
            TtaInsertSibling (anchor, first, TRUE, source_doc);
        }
      else if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
        {
          /* add it before the image */
          TtaInsertSibling (anchor, first, TRUE, source_doc);
        }
      else if (elType.ElTypeNum ==  HTML_EL_Table_
               || elType.ElTypeNum ==  HTML_EL_thead
               || elType.ElTypeNum ==  HTML_EL_tbody
               || elType.ElTypeNum ==  HTML_EL_tfoot
               )
        {
          /* add the A-element before the table */
          el = first;
          elType = TtaGetElementType (el);
          while (elType.ElTypeNum != HTML_EL_Table_)
            {
              el = TtaGetParent (el);
              elType = TtaGetElementType (el);
            }
          /* add it before the image */
          TtaInsertSibling (anchor, el, TRUE, source_doc);
        }
      else if (elType.ElTypeNum ==  HTML_EL_Table_row)
        {
          /* add the A-element to the first cell */
          el = first; 
          while ((el = TtaGetFirstChild (el)))
            {
              elType = TtaGetElementType (el);
              /* the elements where we can add the A-element */
              if (elType.ElTypeNum == HTML_EL_Table_cell
                  || elType.ElTypeNum == HTML_EL_Data_cell
                  || elType.ElTypeNum == HTML_EL_Heading_cell)
                break;
            }
          TtaInsertFirstChild (&anchor, el, source_doc);
        }
      else 
        {
          if (c1 == 0)
            {
              /* add it as the first child of the element */
              TtaInsertFirstChild (&anchor, el, source_doc);
            }
          else if (c1 == 1)
            {
              /* add it to the beginning of the element */
              TtaInsertSibling (anchor, first, TRUE, source_doc);
            }
          else if (c1 > 1)
            {
              /* split the text */
              int len;
	      
              len = TtaGetTextLength (first);

              if (cN > len && c1 == cN)
                /* add it to the end (a caret) */
                TtaInsertSibling (anchor, first, FALSE, source_doc);
              else
                {
                  /* add it in the middle */
                  TtaSplitText (first, c1, source_doc);
                  TtaNextSibling (&first);
                  TtaInsertSibling (anchor, first, TRUE, source_doc);
                }
            }
        }
    }
  else if (DocumentTypes[source_doc] == docXml)
    {
      /* An annotation on a generic XML structure. We don't do anything
         special. */      
      el = first;
      if (c1 == 0)
        {
          /* add it as the first child of the element */
          TtaInsertFirstChild (&anchor, el, source_doc);
        }
      else if (c1 == 1)
        {
          /* add it to the beginning of the element */
          TtaInsertSibling (anchor, first, TRUE, source_doc);
        }
      else if (c1 > 1)
        {
          /* split the text */
          int len;
	  
          len = TtaGetTextLength (first);	  
          if (cN > len && c1 == cN)
            /* add it to the end (a caret) */
            TtaInsertSibling (anchor, first, FALSE, source_doc);
          else
            {
              /* add it in the middle */
              TtaSplitText (first, c1, source_doc);
              TtaNextSibling (&first);
              TtaInsertSibling (anchor, first, TRUE, source_doc);
            }
        }
    }
  else if (!strcmp (docSchemaName, "TextFile"))
    {  
      el = first;
      elType = TtaGetElementType (el);
      elType.ElTypeNum = TextFile_EL_Line_;
      el = TtaSearchTypedElement (elType, SearchForward, el);
      if (c1 == 0)
        {
          /* add it as the first child of the element */
          TtaInsertFirstChild (&anchor, el, source_doc);
        }
      else if (c1 == 1)
        {
          /* add it to the beginning of the element */
          TtaInsertSibling (anchor, first, TRUE, source_doc);
        }
      else if (c1 > 1)
        {
          int len;

          len = TtaGetElementVolume(first);
          if (c1 > len)
            /* add it to the end (a caret)*/
            TtaInsertSibling (anchor, first, FALSE, source_doc);         
          else 
            {
              /* add it in the middle */
              TtaSplitText (first, c1, source_doc);
              TtaNextSibling (&first);
              TtaSplitText (first, 2,source_doc);	
              TtaInsertSibling (anchor, first, TRUE, source_doc);
              TtaSelectString(source_doc,first,1,0);
            }
        }	 
    }

  TtaSetStructureChecking (check_mode, source_doc);
  /* add the Xlink attribute */
  SetXLinkTypeSimple (anchor, source_doc, FALSE);

  /*
  ** add the other attributes
  */
  attrType.AttrSSchema = XLinkSchema;

#ifdef ANNOT_ON_ANNOT
  /* @@ JK: Systematically hiding the thread annotations */
  if (annot->inReplyTo)
    {
      attrType.AttrTypeNum = XLink_ATTR_AnnotIsHidden;
      attr = TtaNewAttribute (attrType);
      TtaAttachAttribute (anchor, attr, source_doc);
    }
#endif /* ANNOT_ON_ANNOT */

  /* add the annotation icon */
  if (!annot->is_orphan)
    attrType.AttrTypeNum = XLink_ATTR_AnnotIcon1;
  else
    attrType.AttrTypeNum = XLink_ATTR_AnnotOrphIcon;
      
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);  

  /* add an HREF attribute pointing to the annotation */
  attrType.AttrTypeNum =  XLink_ATTR_href_;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);
  TtaSetAttributeText (attr, annot->body_url, anchor, source_doc);
  
  /* add a ID attribute so that the annotation doc can point
     back to the source of the annotation link */
  attrType.AttrTypeNum = XLink_ATTR_id;
  attr = TtaNewAttribute (attrType);
  TtaAttachAttribute (anchor, attr, source_doc);
  /* set the ID value (anchor endpoint) */
  TtaSetAttributeText (attr, annot->name, anchor, source_doc);
  /* add the annotation icon */
  LINK_AddAnnotIcon (source_doc, anchor, annot);
  return (!(annot->is_orphan));
}

/*-----------------------------------------------------------------------
  LINK_RemoveLinkFromSource
  -----------------------------------------------------------------------*/
void LINK_RemoveLinkFromSource (Document source_doc, Element el)
{
  TtaRemoveTree (el, source_doc);
}


/*-----------------------------------------------------------------------
  LINK_SaveLink
  Writes the metadata describing an annotation file and its source
  link to an index file. This metadata consists of the
  name of the annotation file, and the Xpointer that specifies the 
  selected  elements on the document
  -----------------------------------------------------------------------*/
void LINK_SaveLink (Document source_doc, ThotBool isReplyTo)
{
  char   *indexName, *doc_url;
  List   *annot_list = NULL;
#ifdef ANNOT_ON_ANNOT
  Document rootDoc;
  AnnotThreadList *thread = NULL;
#endif /* ANNOT_ON_ANNOT */

#ifdef ANNOT_ON_ANNOT
  if (isReplyTo)
    {
      thread = AnnotMetaData[source_doc].thread;
      if (thread)
        {
          rootDoc = AnnotThread_searchRoot (thread->rootOfThread);
          annot_list = AnnotMetaData[rootDoc].annotations;  
          doc_url = AnnotMetaData[rootDoc].annot_url;
          /* doc_url = DocumentURLs[rootDoc]; */
        }
      /* JK: what shall we do if there is no thread, return. signal an error? */
      else
        return;
    }
  else
#endif /* ANNOT_ON ANNOT */
    {
#ifdef ANNOT_ON_ANNOT
      /* the threads that have a root of thread on source_doc */
      thread = &AnnotThread[source_doc];
#endif /* ANNOT_ON ANNOT */
      /* the annotations on this URL */
      annot_list = AnnotMetaData[source_doc].annotations;
      if (DocumentTypes[source_doc] == docAnnot &&  AnnotMetaData[source_doc].annot_url)
        doc_url = AnnotMetaData[source_doc].annot_url;
      else
        doc_url = DocumentURLs[source_doc];
    }

  indexName = LINK_GetAnnotationIndexFile (doc_url);
  if (!indexName)
    {
      indexName = GetTempName (GetAnnotDir (), "index");
      AddAnnotationIndexFile (doc_url, indexName);
    }

  /* write the updated annotation list */
#ifdef ANNOT_ON_ANNOT
  AnnotList_writeIndex (indexName, annot_list, (thread) ? thread->annotations : NULL);
#else /* ANNOT_ON_ANNOT */
  AnnotList_writeIndex (indexName, annot_list, NULL);
#endif /* ANNOT_ON_ANNOT */

  TtaFreeMemory (indexName);
}

/*-----------------------------------------------------------------------
  LINK_UpdateLink
  Updates the local index reference of an annotation that was posted
  but that still has local annotations attached to it.
  -----------------------------------------------------------------------*/
void LINK_UpdateLink (Document source_doc, ThotBool isReplyTo)
{
  char   *indexName, *doc_url;
  List   *annot_list = NULL;
  
  annot_list = AnnotMetaData[source_doc].annotations;  
  doc_url = DocumentURLs[source_doc];
  indexName = LINK_GetAnnotationIndexFile (doc_url);
}

/*-----------------------------------------------------------------------
  LINK_DeleteLink
  For a given source doc, deletes the index entry from the main index and
  removes the documents index file.
  -----------------------------------------------------------------------*/
void LINK_DeleteLink (Document source_doc, ThotBool isReplyTo)
{
  char   *doc_index;
  char buffer[255];
  char *annot_dir;
  char *main_index;
  char *main_index_file_old;
  char *main_index_file_new;
  char *source_doc_url;
  int len;
  int error;
  FILE *fp_old = NULL;
  FILE *fp_new;
  

  /* get the annotation index */
  doc_index = LINK_GetAnnotationIndexFile (DocumentURLs[source_doc]);
  if (!doc_index)
    doc_index = LINK_GetAnnotationIndexFile (AnnotMetaData[source_doc].annot_url);
  if (!doc_index)
    return;

  error = 0;
  annot_dir = GetAnnotDir ();
  main_index = GetAnnotMainIndex ();
  main_index_file_old = (char *)TtaGetMemory (strlen (annot_dir) 
                                              + strlen (main_index)
                                              + 10);

  main_index_file_new = (char *)TtaGetMemory (strlen (annot_dir) 
                                              + strlen (main_index)
                                              + 14);
  sprintf (main_index_file_old, 
           "%s%c%s", 
           annot_dir, 
           DIR_SEP, 
           main_index);

  sprintf (main_index_file_new, 
           "%s%c%s.tmp", 
           annot_dir, 
           DIR_SEP, 
           main_index);

  if (!(fp_new = TtaWriteOpen (main_index_file_new)))
    error++;

  if (!error)
    {
      if (!(fp_old = TtaReadOpen (main_index_file_old)))
        {
          TtaReadClose (fp_new);
          error++;
        }
    }

  if (!error)
    {
      if (IsW3Path (DocumentURLs[source_doc])
          || IsFilePath (DocumentURLs[source_doc]))
        source_doc_url = DocumentURLs[source_doc];
      else
        source_doc_url = ANNOT_MakeFileURL (DocumentURLs[source_doc]);
		    
		    
      /* search and remove the index entry */
      len = strlen (DocumentURLs[source_doc]);
      while (fgets (buffer, sizeof (buffer), fp_old))
        {
          if (strncmp (source_doc_url, buffer, len))
            fputs (buffer, fp_new);
        }
      TtaWriteClose (fp_new);
      TtaReadClose (fp_old);
      
      if (source_doc_url != DocumentURLs[source_doc])
        TtaFreeMemory (source_doc_url);

      /* rename the main index file */
      TtaFileUnlink (main_index_file_old);
      TtaFileRename (main_index_file_new, main_index_file_old);
      /* remove the index file */
      TtaFileUnlink (doc_index);
    }

  TtaFreeMemory (main_index_file_old);
  TtaFreeMemory (main_index_file_new);
  TtaFreeMemory (doc_index);
}

/*-----------------------------------------------------------------------
  LINK_CreateMeta
  -----------------------------------------------------------------------*/
AnnotMeta *LINK_CreateMeta (Document source_doc, Document annot_doc, AnnotMode mode)

{
  AnnotMeta   *annot;
  char      *annot_user;
  char      *source_doc_url; /* the url we will use to refer to the source doc */

  /*
  **  Make a new annotation entry, add it to annotlist, and initialize it.
  */

#ifdef ANNOT_ON_ANNOT
  if (DocumentTypes[source_doc] == docAnnot && AnnotMetaData[source_doc].annot_url)
    {
      /* we reply to an annotation (using it's metadata). However, we annotate the
         body of an annotation, not its metadata */
      if (mode & ANNOT_isReplyTo)
        source_doc_url = AnnotMetaData[source_doc].annot_url;
      else
        source_doc_url = DocumentURLs[source_doc];
    }
  else
#endif /* ANNOT_ON_ANNOT */
    source_doc_url = DocumentURLs[source_doc];


  /* download the local annotations, if they do exist, but mark them
     invisible */
  if (!IsW3Path (DocumentURLs[source_doc]) && 
      (!AnnotMetaData[source_doc].annotations 
       && !AnnotMetaData[source_doc].local_annot_loaded))
    {
      char *annotIndex = NULL;

      annotIndex = LINK_GetAnnotationIndexFile (source_doc_url);
      LINK_LoadAnnotationIndex (source_doc, annotIndex, FALSE);
      AnnotMetaData[source_doc].local_annot_loaded = TRUE;
    }

  annot =  AnnotMeta_new ();
  if (source_doc_url &&
      (IsW3Path (source_doc_url) || IsFilePath(source_doc_url)))
    annot->source_url = TtaStrdup (source_doc_url);
  else
    annot->source_url = ANNOT_MakeFileURL (source_doc_url);
  
  /* get the current date... cdate = mdate at this stage */
  annot->cdate = StrdupDate ();
  annot->mdate = TtaStrdup (annot->cdate);

  /* Annotation type -- will be reassigned below for Reply */
  annot->type = DEFAULT_ANNOTATION_TYPE;

  /* Annotation XPointer */
  /* annot->xptr = XPointer_build (source_doc, 1, useDocRoot);*/

  annot_user = GetAnnotUser ();
  annot->author = TtaStrdup (annot_user);
  annot->content_type = TtaStrdup (AM_XHTML_MIME_TYPE);
  if (!IsW3Path (DocumentURLs[annot_doc]) 
      && !IsFilePath (DocumentURLs[annot_doc]))
    {
      annot->body_url = ANNOT_MakeFileURL (DocumentURLs[annot_doc]);
    }
  else
    annot->body_url = TtaStrdup (DocumentURLs[annot_doc]);

  /* the annotation is visible to the user */
  annot->is_visible = TRUE;
  /* display the annotation */
  annot->show = TRUE;
  /* it's not an orphan */
  annot->is_orphan = FALSE;
  
  /* create the reverse link name */
  LINK_CreateAName (annot);

#ifdef ANNOT_ON_ANNOT
  /* initialize the annot_url we will use to reference this annotation*/
  AnnotMetaData[annot_doc].annot_url = TtaStrdup (annot->body_url);

  if (mode & ANNOT_isReplyTo)
    {
      char *source_annot_url;

      annot->type = DEFAULT_REPLY_TYPE;

      source_annot_url = AnnotMetaData[source_doc].annot_url;
      /* initialize the thread info */
      if (!AnnotMetaData[source_doc].thread)
        {
          AnnotMetaData[source_doc].thread = &AnnotThread[source_doc];
          AnnotThread[source_doc].rootOfThread = TtaStrdup (source_annot_url);
        }
      AnnotMetaData[annot_doc].thread = AnnotMetaData[source_doc].thread;
      annot->thread = AnnotMetaData[source_doc].thread;
      AnnotMetaData[source_doc].thread->references++;
      annot->inReplyTo = TtaStrdup (source_annot_url);
      /* maybe useless */
      annot->rootOfThread = TtaStrdup (AnnotMetaData[source_doc].thread->rootOfThread);
      /* and add the annotation to the list of threads */
      List_add (&(AnnotMetaData[source_doc].thread->annotations), (void *) annot);
    }
  else
#endif /* ANNOT_ON_ANNOT */
    {
      /* add the annotation to the list of annotations */
      List_add (&(AnnotMetaData[source_doc].annotations), (void *) annot);
    }

  return annot;
}

/*-----------------------------------------------------------------------
  Procedure LINK_DelMetaFromMemory
  -----------------------------------------------------------------------
  Frees the memory used by the parsed metadata
  -----------------------------------------------------------------------*/
void LINK_DelMetaFromMemory (Document doc)
{
  /* @@ JK remove this?
     if (!AnnotMetaData[doc].annotations)
     return;
  */

  AnnotList_free (AnnotMetaData[doc].annotations);
  AnnotMetaData[doc].annotations = NULL;
  AnnotMetaData[doc].local_annot_loaded = FALSE;
  /* delete any previous filters */
  AnnotFilter_deleteAll (doc);
  /* we no longer need this part of the RDF model; it holds only
     for the annotations of this document */
  SCHEMA_FreeRDFModel (&AnnotMetaData[doc].rdf_model);

  if (AnnotMetaData[doc].annot_url)
    {
      TtaFreeMemory (AnnotMetaData[doc].annot_url);
      AnnotMetaData[doc].annot_url = NULL;
    }

#ifdef ANNOT_ON_ANNOT 
  /* @@ JK: maybe remove the references too */
  AnnotMetaData[doc].thread = NULL;
  
  /* free the data associated with the thread */
  if (AnnotThread[doc].annotations)
    {
      /* @@ JK: we need a function to erase all the annotations in the list */
      /*
        AnnotList_delAnnot (&(AnnotMetaData[source_doc].annotations),
        annot->body_url, FALSE);
      */
      AnnotList_free (AnnotThread[doc].annotations);
      AnnotThread[doc].annotations = NULL;
    }
  if (AnnotThread[doc].rootOfThread)
    {
      TtaFreeMemory (AnnotThread[doc].rootOfThread);
      AnnotThread[doc].rootOfThread = NULL;
    }
#endif /* ANNOT_ON_ANNOT */
}

/*-----------------------------------------------------------------------
  LINK_ReloadAnnotationIndex
  -----------------------------------------------------------------------
  Redisplays an already loaded annotation index.
  -----------------------------------------------------------------------*/
void LINK_ReloadAnnotationIndex (Document doc, View view)
{
  Element body;
  List *annot_list, *list_ptr;
  AnnotMeta *annot;
  DisplayMode dispMode;
  /* counts the number of orphan annotations in the document */
  int orphan_count = 0;

  annot_list = AnnotMetaData[doc].annotations;

  if (!annot_list)
    /* there are no annotations */
    return;

  /* avoid refreshing the document while adding the annotation links */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  /* Insert the annotations in the body */
  view = TtaGetViewFromName (doc, "Formatted_view");
  body = SearchElementInDoc (doc, HTML_EL_BODY);
  
  list_ptr = annot_list;
  
  while (list_ptr)
    {
      annot = (AnnotMeta *) list_ptr->object;
      
      if (annot->is_visible)
        {
          if (! LINK_AddLinkToSource (doc, annot))
            annot->show = TRUE;
        }
      list_ptr = list_ptr->next;
    }

  /* show the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);

  if (orphan_count)
    {
      /* warn the user there were some orphan annotations */
      InitInfo ("Annotation load", 
                "There were some orphan annotations. You may See them with the Links view.");
    }
}

/*-----------------------------------------------------------------------
  LINK_LoadAnnotationIndex
  -----------------------------------------------------------------------
  Searches for an annotation index related to the document. If it exists,
  it parses it and then, if the variable mark_visible is set true, adds
  links to them from the source document.
  -----------------------------------------------------------------------*/
void LINK_LoadAnnotationIndex (Document doc, char *annotIndex, ThotBool mark_visible)
{
  View    view;
  Element body;
  List *annot_list, *list_ptr;
  AnnotMeta *annot;
  AnnotMeta *old_annot;
  DisplayMode dispMode;
  /* counts the number of orphan annotations in the document */
  int orphan_count = 0;
#ifdef ANNOT_ON_ANNOT
  Document doc_thread;
  AnnotThreadList *thread;
#endif /* ANNOT_ON_ANNOT */

  if (!annotIndex || !(TtaFileExist (annotIndex)))
    /* there are no annotations */
    return;
  
  annot_list = RDF_parseFile (annotIndex, &AnnotMetaData[doc].rdf_model);
  TtaFreeMemory (annotIndex);
  annotIndex = NULL;
  if (!annot_list)
    /* we didn't read any annotation */
    return;

  /* avoid refreshing the document while adding the annotation links */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);

  /* Insert the annotations in the body */
  view = TtaGetViewFromName (doc, "Formatted_view");
  body = SearchElementInDoc (doc, HTML_EL_BODY);
  
  list_ptr = annot_list;

  while (list_ptr)
    {
      char *url;
      AnnotMetaDataSearch searchType;

      annot = (AnnotMeta *) list_ptr->object;
      
      /* don't add an annotation if it's already on the list */
      /* @@ JK: later, Ralph will add code to delete the old one.
         We should remove the old annotations and preserve the newer
         ones. Take into account that an anotation window may be open
         and that we'll have to close it without saving... or just update
         the metadata... */
      
      if (IsW3Path (annot->annot_url))
        {
          url = annot->annot_url;
          searchType = AM_ANNOT_URL;
        }
      else
        {
          url = annot->body_url;
          searchType = AM_BODY_URL;
        }
      old_annot = AnnotList_searchAnnot (AnnotMetaData[doc].annotations,
                                         url, searchType);
#ifdef ANNOT_ON_ANNOT
      /* do the same thing to avoid duplicating the thread items */
      if (!old_annot && AnnotThread[doc].annotations)
        old_annot = AnnotList_searchAnnot (AnnotThread[doc].annotations,
                                           url, searchType);
#endif /* ANNOT_ON_ANNOT */

      if (!old_annot)
        {
          if (mark_visible)
            {
              /* create the reverse link name */
              LINK_CreateAName (annot);
              if (! LINK_AddLinkToSource (doc, annot))
                orphan_count++;
              annot->is_visible = TRUE;
              annot->show = TRUE;
            }
          else
            annot->is_visible = FALSE;
#ifdef ANNOT_ON_ANNOT
          if (annot->inReplyTo)
            {
              doc_thread = AnnotThread_searchRoot (annot->rootOfThread);
              /* if there's no other thread, then use the source doc as the
                 start of the thread */
              if (doc_thread ==  0)
                {
                  thread = NULL;
                  doc_thread = doc;
                }
              else
                thread = &AnnotThread[doc_thread];

              /* there was no thread. Create a new one if it's the same rootOfThread document */
              if (!thread && Annot_isSameURL (AnnotMetaData[doc].annot_url, annot->rootOfThread))
                {
                  /* add the root of thread (used by load index later on) */
                  AnnotThread[doc].rootOfThread = 
                    TtaStrdup (annot->rootOfThread);
                  AnnotThread[doc].references = 1;
                  thread = &AnnotThread[doc_thread];
                }

              /* add and show the thread item */
              if (thread)
                {
                  List_add (&(thread->annotations), (void *) annot);
                  if (!AnnotMetaData[doc].thread)
                    AnnotMetaData[doc].thread = thread;
                  annot->thread = thread;
                  /* ANNOT_AddThreadItem (doc_thread, annot); */
                }
              else
                Annot_free (annot);
            }
          else /* not a reply */
#endif /* ANNOT_ON_ANNOT */
            /* Verify that the annotates property really does point
               to this document and discard if not or if there is
               no annotates property. */
            if (Annot_isSameURL (annot->source_url, DocumentURLs[doc]))
              List_add (&AnnotMetaData[doc].annotations, (void *) annot);
            else
              Annot_free (annot);
        }
      else
        Annot_free (annot);
      List_delFirst (&list_ptr);
    }

#ifdef ANNOT_ON_ANNOT
  /* erase and redisplay the thread items */
  ANNOT_DeleteThread (doc);
  if (AnnotThread[doc].annotations)
    ANNOT_BuildThread (doc);
#endif /* ANNOT_ON_ANNOT */

  /* show the document */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);

  if (orphan_count)
    {
      /* warn the user there were some orphan annotations */
      InitInfo ("Annotation load", 
                "There were some orphan annotations. You may See them with the Links view.");
    }
}

/*-----------------------------------------------------------------------
  LINK_SelectSourceDoc
  Selects the text that was annotated in a document.
  Returns the element corresponding to the annotation anchor in the
  source document if the return_el flag is set to TRUE.
  The parameter annot_url is a utf-8 string.
  -----------------------------------------------------------------------*/
Element LINK_SelectSourceDoc (Document doc, CONST char *annot_url, 
                              ThotBool return_el)
{
  XPointerContextPtr xptr_ctx;
  AnnotMeta *annot;
  char *url;
  Element el = NULL;
  ThotBool selected = FALSE;

  if (IsW3Path (annot_url) || IsFilePath (annot_url))
    url = (char *) annot_url;
  else
    url = ANNOT_MakeFileURL (annot_url);

  annot = AnnotList_searchAnnot (AnnotMetaData[doc].annotations,
                                 url, AM_BODY_URL);
  if (url != annot_url)
    TtaFreeMemory (url);

  if (annot)
    {
      if (!annot->is_orphan)
        {
          xptr_ctx = XPointer_parse (doc, annot->xptr);
          if (!xptr_ctx->error)
            {
              XPointer_select (xptr_ctx);
              selected = TRUE;
            }
#ifdef JK_DEBUG
          else
            fprintf (stderr, 
                     "LINK_SelectSource: impossible to set XPointer\n");
#endif
          XPointer_free (xptr_ctx);
        }
    }
#ifdef JK_DEBUG
  else
    fprintf (stderr, "LINK_SelectSourceDoc: couldn't find annotation metadata\n");
#endif 

  /* search the element corresponding to the anchor in the source
     document */
  if (selected && return_el)
    el = SearchAnnotation (doc, annot->name);

  return el;
}
