/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Initialization functions and button functions of Amaya application.
 *
 * Authors: V. Quint, I. Vatton
 *          R. Guetari (W3C/INRIA) - Windows routines.
 */


/* Included headerfiles */
#define THOT_EXPORT
#include "amaya.h"
#include "print.h"

#define NumFormPrint       1
#define NumMenuOptions     2
#define NumMenuPaperFormat 3
#define NumMenuSupport     4
#define NumZonePrinterName 5
#define PRINT_MAX_REF	   6

/* structure to register sub-documents in MakeBook function*/
typedef struct _SubDoc
  {
     struct _SubDoc  *SDnext;
     Element          SDel;
     char            *SDname;
  }SubDoc;

/* the structure used for the GetIncludedDocuments_callback function */
typedef struct _GetIncludedDocuments_context {
  Element		link, next;
  Attribute		RelAttr, HrefAttr;
  char			*url, *ptr, *text;
  Document		document;
} GetIncludedDocuments_context;

static struct _SubDoc  *SubDocs;
static int              PaperPrint;
static int              ManualFeed;
static int              PageSize;
static char             PSdir[MAX_PATH];
static char             pPrinter[MAX_PATH];
static Document		docPrint;
static boolean		numberLinks;
static boolean		withToC;
static int              basePrint;

#include "init_f.h"
#include "HTMLactions_f.h"
#include "HTMLbook_f.h"
#include "HTMLedit_f.h"
#include "HTMLhistory_f.h"

/*-----------------------------------------------------------------------
  Function prototypes
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         GetIncludedDocuments (Element el, Document document);
void                MakeBook_callback (Document document);
#else
static void         GetIncludedDocuments (/* el, document */);
void                MakeBook_callback (/* Document document */);
#endif

/*----------------------------------------------------------------------
  RegisterSubDoc adds a new entry in SubDoc table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RegisterSubDoc (Element el, char *url)
#else
static void         RegisterSubDoc (el, url)
Element             el;
char               *url;
#endif
{
  struct _SubDoc  *entry, *last;

  if (url == NULL || url[0] == EOS)
    return;

  entry = TtaGetMemory (sizeof (struct _SubDoc));
  entry->SDnext = NULL;
  entry->SDel = el;
  entry->SDname = TtaStrdup (url);

  if (SubDocs == NULL)
    SubDocs = entry;
  else
    {
      last = SubDocs;
      while (last->SDnext != NULL)
	last = last->SDnext;
      last->SDnext = entry;
    }
}


/*----------------------------------------------------------------------
  SearchSubDoc searches whether a document name is registered or not
  within the SubDoc table.
  Return the DIV element that correspond to the sub-document or NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      SearchSubDoc (char *url)
#else
static Element      SearchSubDoc (url)
char               *url;
#endif
{
  Element          el;
  struct _SubDoc  *entry;
  boolean          docFound;

  if (url == NULL || url[0] == EOS)
    return (NULL);

  entry = SubDocs;
  docFound = FALSE;
  el = NULL;
  while (!docFound && entry != NULL)
    {
      docFound = (strcmp (url, entry->SDname) == 0);
      if (!docFound)
	entry = entry->SDnext;
      else
	/* document found -> return the DIV element */
	el = entry->SDel;
    }
  return (el);
}

/*----------------------------------------------------------------------
  FreeSubDocTable frees all entries in SubDoc table.
  ----------------------------------------------------------------------*/
static void         FreeSubDocTable ()
{
  struct _SubDoc  *entry, *last;

  entry = SubDocs;
  while (entry != NULL)
    {
      last = entry;
      entry = entry->SDnext;
      TtaFreeMemory (last->SDname);
      TtaFreeMemory (last);
    }
  SubDocs = NULL;
}



/*----------------------------------------------------------------------
  SetInternalLinks
  Associate an InternalLink attribute with all anchor (A) elements of the
  document which designate an element in the same document.
  InternalLink is a Thot reference attribute that links a source and a
  target anchor and that allows P schemas to display and print cross-references
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void             SetInternalLinks (Document document)
#else
void             SetInternalLinks (document)
Document                document;
#endif
{
  Element	        root, el, div;
  Element		link, target, sibling;
  ElementType		elType;
  Attribute		HrefAttr, IntLinkAttr;
  Attribute             attr, ExtLinkAttr;
  AttributeType	        attrType;
  char		       *text, *ptr, *url, number[10];
  char                  value[MAX_LENGTH];
  int			length, i, volume;
  int                   status, position;
  boolean               split;

  /* Remember the current status of the document */
  status = TtaIsDocumentModified (document);
  root = TtaGetMainRoot (document);
  volume = TtaGetElementVolume (root);
  elType = TtaGetElementType (root);
  elType.ElTypeNum = HTML_EL_BODY;
  el = TtaSearchTypedElement (elType, SearchForward, root);

  elType.ElTypeNum = HTML_EL_Anchor;
  attrType.AttrSSchema = elType.ElSSchema;
  /* looks for all anchors in the document */
  link = el;
  while (link != NULL)
    {
      /* display the progression of the work */
      el = link;
      position = 0;
      while (el != NULL)
	{
	  sibling = el;
	  do
	    {
	      /* add volume of each previous element */
	      TtaPreviousSibling (&sibling);
	      if (sibling != NULL)
		position += TtaGetElementVolume (sibling);
	    }
	  while (sibling != NULL);
	  el = TtaGetParent (el);
	}
      sprintf (number, "%d", position*100/volume);
      TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_UPDATED_LINK), number);
      TtaHandlePendingEvents ();
      link = TtaSearchTypedElement (elType, SearchForward, link);
      if (link != NULL)
	/* an anchor has been found */
	{
	  attrType.AttrTypeNum = HTML_ATTR_HREF_;
	  HrefAttr = TtaGetAttribute (link, attrType);
	  attrType.AttrTypeNum = HTML_ATTR_InternalLink;
	  IntLinkAttr = TtaGetAttribute (link, attrType);
	  attrType.AttrTypeNum = HTML_ATTR_ExternalLink;
	  ExtLinkAttr = TtaGetAttribute (link, attrType);
	  if (HrefAttr == NULL)
	    /* this anchor is not a link (no href attribute) */
	    /* remove attributes InternalLink and ExternalLink if they
	       are present */
	    {
	      if (IntLinkAttr != NULL)
		TtaRemoveAttribute (link, IntLinkAttr, document);
	      if (ExtLinkAttr != NULL)
		TtaRemoveAttribute (link, ExtLinkAttr, document);	   
	    }
	  else
	    /* this anchor has an HREF attribute */
	    {
	      length = TtaGetTextAttributeLength (HrefAttr);
	      text = TtaGetMemory (length + 1);
	      TtaGiveTextAttributeValue (HrefAttr, text, &length);

	      /* does an external link become an internal link ? */
	      if (document == DocBook && SubDocs != NULL)
		{
		  ptr = strrchr (text, '#');
		  url = text;
		  split = FALSE;
		  if (ptr == text)
		      /* a local link */
		      url = NULL;
		  else if (ptr != NULL)
		    {
		      /* split url and name part */
		      ptr[0] = EOS;
		      split = TRUE;
		    }

		  /* Is it a sub-document */
		  div = SearchSubDoc (url);
		  if (split)
		    /* retore the mark */
		    ptr[0] = '#';

		  if (div == NULL)
		    {
		      /* it's not a sub-document */
		      if (url == NULL)
			/* a local link */
			ptr = &text[1];
		      else
			/* still an externa; link */
			ptr = NULL;
		    }
		  else
		    {
		      /* this link becomes internal */
		      if (ptr != NULL)
			{
			  /* get the target name */
			  strcpy (value, ptr);
			  length = strlen (value);
			  /* check whether the name changed */
			  i = 0;
			  target = SearchNAMEattribute (document, &value[1], NULL);
			  while (target != NULL)
			    {
			      /* is it the right NAME */
			      if (TtaIsAncestor (target, div))
				target = NULL;
			      else
				{
				  /* continue the search */
				  i++;
				  sprintf (&value[length], "%d", i);
				  target = SearchNAMEattribute (document, &value[1], NULL);
				}
			    }
			}
		      else
			{
			  /* get the DIV name */
			  attrType.AttrTypeNum = HTML_ATTR_ID;
			  attr = TtaGetAttribute (div, attrType);
			  length = 200;
			  value[0] = '#';
			  TtaGiveTextAttributeValue (attr, &value[1], &length);
			}
		      ptr = &value[1];
		      TtaSetAttributeText (HrefAttr, value, link, document);
		    }
		}
	      else if (text[0] == '#')
		  ptr = &text[1];
	      else
		ptr = NULL;

	      if (ptr != NULL)
		/* it's an internal link. Attach an attribute InternalLink */
		/* to the link, if this attribute does not exist yet */
		{
		  if (IntLinkAttr == NULL)
		    {
		      attrType.AttrTypeNum = HTML_ATTR_InternalLink;
		      IntLinkAttr = TtaNewAttribute (attrType);
		      TtaAttachAttribute (link, IntLinkAttr, document);
		    }
		  /* looks for the target element */
		  target = SearchNAMEattribute (document, ptr, NULL);
		  if (target != NULL)
		    /* set the Thot link */
		    TtaSetAttributeReference (IntLinkAttr, link, document, target, document);
		}
	      else
		/* it's an external link */
		{
		  /* Remove the InternalLink attribute if it is present */
		  if (IntLinkAttr != NULL)
		    TtaRemoveAttribute (link, IntLinkAttr, document);
		  /* create an ExternalLink attribute if there is none */
		  if (ExtLinkAttr == NULL)
		    {
		      attrType.AttrTypeNum = HTML_ATTR_ExternalLink;
		      ExtLinkAttr = TtaNewAttribute (attrType);
		      TtaAttachAttribute (link, ExtLinkAttr, document);
		    }
		}
	      TtaFreeMemory (text);
	    }
	}
    }
  /* Reset document status */
  if (!status)
    TtaSetDocumentUnmodified (document);
}

/*----------------------------------------------------------------------
  CheckPrintingDocument reinitialize printing parameters as soon as
  the printing document changes.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CheckPrintingDocument (Document document)
#else
static void         CheckPrintingDocument (document)
Document            document;
#endif
{
   char             docName[MAX_LENGTH];
   char            *ptr, suffix[MAX_LENGTH];
   int              lg;

   if (docPrint != document)
     {
       /* initialize print parameters */
       docPrint = document;

       /* define the new default PS file */
       ptr = TtaGetEnvString ("TMPDIR");
       if (ptr != NULL && TtaCheckDirectory (ptr))
	 {
	   strcpy(PSdir,ptr);
	   lg = strlen(PSdir);
	   if (PSdir[lg - 1] == DIR_SEP)
	     PSdir[--lg] = '\0';
	 }
       else
	 {
#          ifdef _WINDOWS
	   strcpy (PSdir,"C:\\TEMP");
#          else  /* !_WINDOWS */
	   strcpy (PSdir,"/tmp");
#          endif /* !_WINDOWS */
	   lg = strlen (PSdir);
	 }
       strcpy (docName, TtaGetDocumentName (document));
       ExtractSuffix (docName, suffix);
#      ifdef _WINDOWS
       sprintf (&PSdir[lg], "\\%s.ps", docName);
#      else /* !_WINDOWS */
       sprintf (&PSdir[lg], "/%s.ps", docName);
#      endif /* !_WINDOWS */
       TtaSetPsFile (PSdir);
       /* define the new default PrintSchema */
       numberLinks = FALSE;
       withToC = FALSE;
       TtaSetPrintSchema ("");
       /* no manual feed */
       ManualFeed = PP_OFF;
       TtaSetPrintParameter (PP_ManualFeed, ManualFeed);
     }
}


/*----------------------------------------------------------------------
   PrintAs prints the document using predefined parameters.
   ----------------------------------------------------------------------*/  
#ifdef __STDC__
void                PrintAs (Document document, View view)
#else  /* __STDC__ */
void                PrintAs (document, view)
Document            document;
#endif /* __STDC__ */
{
   char             viewsToPrint[MAX_PATH];

   CheckPrintingDocument (document);
   strcpy (viewsToPrint, "Formatted_view ");
   if (withToC)
     strcat (viewsToPrint, "Table_of_contents ");
   if (numberLinks)
     /* display numbered links */
     {
       /* associate an attribute InternalLink with all anchors refering
	  a target in the same document.  This allows P schemas to work
	  properly */
       SetInternalLinks (docPrint);
       if (PageSize == PP_A4)
	 TtaSetPrintSchema ("HTMLPLP");
       else
	 TtaSetPrintSchema ("HTMLPLPUS");
       strcat (viewsToPrint, "Links_view ");
     }
   TtaPrint (docPrint, viewsToPrint);
}


/*----------------------------------------------------------------------
   CallbackImage manage returns of Picture form.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackPrint (int ref, int typedata, char *data)
#else  /* __STDC__ */
void                CallbackPrint (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;
#endif /* __STDC__ */
{
  int                 val;

  val = (int) data;
  switch (ref - basePrint)
    {
    case NumFormPrint:
      TtaDestroyDialogue (basePrint+NumFormPrint);
      switch (val)
	{
	case 1:
	  /* confirms the paper print option */
	  /* the other options are not taken into account without this
	     confirmation */
	  TtaSetPrintParameter (PP_Destination, PaperPrint);
	  TtaSetPrintParameter (PP_ManualFeed, ManualFeed);
	  TtaSetPrintParameter (PP_PaperSize, PageSize);
	  TtaSetPrintCommand (pPrinter);
	  TtaSetPsFile (PSdir);
	  PrintAs (docPrint, 1);
	  break;
	case 0:
	  PaperPrint = TtaGetPrintParameter (PP_Destination);
	  ManualFeed = TtaGetPrintParameter (PP_ManualFeed);
	  PageSize = TtaGetPrintParameter (PP_PaperSize);	  
	  TtaGetPrintCommand (pPrinter);
	  TtaGetPsFile (PSdir);
	  break;
	default:
	  break;
	}
      break;
    case NumMenuOptions:
      switch (val)
	{
	case 0:
	  /* Manual feed option */
	  if (ManualFeed == PP_ON)
	    ManualFeed = PP_OFF;
	  else
	    ManualFeed = PP_ON;
	  break;
	case 1:
	  /* Toc option */
	  withToC = !withToC;
	  break;
	case 2:
	  /* numberLinks option */
	  numberLinks = !numberLinks;
	  break;
	}
      break;
    case NumMenuPaperFormat:
      /* page size submenu */
      switch (val)
	{
	case 0:
	  PageSize = PP_A4;
	  break;
	case 1:
	  PageSize = PP_US;
	  break;
	}
      break;
    case NumMenuSupport:
      /* paper print/save PostScript submenu */
      switch (val)
	{
	case 0:
	  if (PaperPrint == PP_PS)
	    {
	      PaperPrint = PP_PRINTER;
	      TtaSetTextForm (basePrint+NumZonePrinterName, pPrinter);
	    }
	  break;
	case 1:
	  if (PaperPrint == PP_PRINTER)
	    {
	      PaperPrint = PP_PS;
	      TtaSetTextForm (basePrint+NumZonePrinterName, PSdir);
	    }
	  break;
	}
      break;
    case NumZonePrinterName:
      if (data[0] != '\0')
	if (PaperPrint == PP_PRINTER)
	  /* text capture zone for the printer name */
	  strncpy (pPrinter, data, MAX_PATH);
	else
	  /* text capture zone for the name of the PostScript file */
	  strncpy (PSdir, data, MAX_PATH);
      break;
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitPrint (void)
#else  /* __STDC__ */
void                InitPrint ()
#endif /* __STDC__ */
{
  char *ptr;

   basePrint = TtaSetCallback (CallbackPrint, PRINT_MAX_REF);
   docPrint = 0;

   /* init printer variables */
   /* read default printer variable */
   ptr = TtaGetEnvString ("THOTPRINT");
   if (ptr == NULL)
     strcpy (pPrinter, "");
   else
     strcpy (pPrinter, ptr);

   PageSize = PP_A4;
   PaperPrint = PP_PRINTER;
   TtaSetPrintParameter (PP_Destination, PaperPrint);
   TtaSetPrintParameter (PP_PaperSize, PageSize);
   TtaSetPrintCommand (pPrinter);
}

/*----------------------------------------------------------------------
  SetupAndPrint sets printing parameters and starts the printing process
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetupAndPrint (Document document, View view)
#else
void                SetupAndPrint (document, view)
Document            document;
View                view;
#endif
{
#  ifndef _WINDOWS
   char             bufMenu[MAX_LENGTH];
   int              i;
#  endif /* !_WINDOWS */

   /* Print form */
   CheckPrintingDocument (document);

#  ifndef _WINDOWS
   TtaNewSheet (basePrint+NumFormPrint, TtaGetViewFrame (document, view), 
		TtaGetMessage (LIB, TMSG_LIB_PRINT),
	   1, TtaGetMessage (AMAYA, AM_BUTTON_PRINT), FALSE, 2, 'L', D_CANCEL);
   i = 0;
   sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (LIB, TMSG_MANUAL_FEED));
   i += strlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_PRINT_TOC));
   i += strlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_NUMBERED_LINKS));
   TtaNewToggleMenu (basePrint+NumMenuOptions, basePrint+NumFormPrint,
		TtaGetMessage (LIB, TMSG_OPTIONS), 3, bufMenu, NULL, FALSE);
   if (ManualFeed == PP_ON)
      TtaSetToggleMenu (basePrint+NumMenuOptions, 0, TRUE);
   if (withToC)
      TtaSetToggleMenu (basePrint+NumMenuOptions, 1, TRUE);
   if (numberLinks)
      TtaSetToggleMenu (basePrint+NumMenuOptions, 2, TRUE);

   /* Paper format submenu */
   i = 0;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_A4));
   i += strlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_US));
   TtaNewSubmenu (basePrint+NumMenuPaperFormat, basePrint+NumFormPrint, 0,
	     TtaGetMessage (LIB, TMSG_PAPER_SIZE), 2, bufMenu, NULL, FALSE);
   if (PageSize == PP_US)
      TtaSetMenuForm (basePrint+NumMenuPaperFormat, 1);
   else
      TtaSetMenuForm (basePrint+NumMenuPaperFormat, 0);

   /* Print to paper/ Print to file submenu */
   i = 0;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_PRINTER));
   i += strlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_PS_FILE));
   TtaNewSubmenu (basePrint+NumMenuSupport, basePrint+NumFormPrint, 0,
                  TtaGetMessage (LIB, TMSG_OUTPUT), 2, bufMenu, NULL, TRUE);
   /* text capture zone for the printer name */
   TtaNewTextForm (basePrint+NumZonePrinterName, basePrint+NumFormPrint, NULL, 30, 1, FALSE);

   /* initialization of the PaperPrint selector */
   if (PaperPrint == PP_PRINTER)
     {
	TtaSetMenuForm (basePrint+NumMenuSupport, 0);
	TtaSetTextForm (basePrint+NumZonePrinterName, pPrinter);
     }
   else
     {
	TtaSetMenuForm (basePrint+NumMenuSupport, 1);
	TtaSetTextForm (basePrint+NumZonePrinterName, PSdir);
     }

   /* activates the Print form */
    TtaShowDialogue (basePrint+NumFormPrint, FALSE);
#   else  /* _WINDOWS */
    CreatePrintDlgWindow (TtaGetViewFrame (document, view), PSdir, basePrint, NumMenuSupport, NumMenuOptions, NumMenuPaperFormat, NumZonePrinterName, NumFormPrint); 
#   endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  SectionNumbering
  Execute the "Section Numbering" command
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SectionNumbering (Document document, View view)
#else
void                SectionNumbering (document, view)
Document            document;
View                view;
#endif
{
   ChangeAttrOnRoot (document, HTML_ATTR_SectionNumbering);
}

/*----------------------------------------------------------------------
  UpdateURLsInSubtree
  Update NAMEs and URLs in subtree of el element, to take into account
  the move from one document to another.
  If a NAME attribute already exists in the new document, it is changed
  to avoid duplicate names.
  Transform the HREF and SRC attribute to make them independent from their
  former base.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         UpdateURLsInSubtree (NotifyElement *event, Element el)
#else
static void         UpdateURLsInSubtree (event, el)
NotifyElement      *event;
Element             el;
#endif
{
Element             nextEl;

  event->element = el;
  ElementPasted (event);
  nextEl = TtaGetFirstChild (el);
  while (nextEl != NULL)
    {
      UpdateURLsInSubtree (event, nextEl);
      TtaNextSibling (&nextEl);
    }
}


/*----------------------------------------------------------------------
  MoveDocumentBody
  Copy the elements contained in the BODY of document sourceDoc to the
  position of element *el in document destDoc.
  Delete the element containing *el and all its empty ancestors.
  If deleteTree is TRUE, copied elements are deleted from the source
  document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void    MoveDocumentBody (Element *el, Document destDoc, Document sourceDoc, char *target, char *url, boolean deleteTree)
#else
static void    MoveDocumentBody (el, destDoc, sourceDoc, target, url, deleteTree)
Element       *el;
Document       destDoc;
Document       sourceDoc;
char          *target;
char          *url;
boolean        deleteTree;
#endif
{
  Element	   root, ancestor, elem, firstInserted;
  Element          lastInserted, srce, copy, old, parent, sibling;
  ElementType	   elType;
  NotifyElement    event;
  int		   checkingMode;
  boolean          isID;

  if (target != NULL)
    {
      /* locate the target element within the source document */
      root = SearchNAMEattribute (sourceDoc, target, NULL);
      elType = TtaGetElementType (root);
      isID = (elType.ElTypeNum != HTML_EL_Anchor 
	      && elType.ElTypeNum != HTML_EL_MAP);
    }
  else
    {
      isID = FALSE;
      /* get the BODY element of source document */
      root = TtaGetMainRoot (sourceDoc);
      elType = TtaGetElementType (root);
      elType.ElTypeNum = HTML_EL_BODY;
      root = TtaSearchTypedElement (elType, SearchForward, root);
    }

  if (root != NULL)
    {
      /* don't check the abstract tree against the structure schema */
      checkingMode = TtaGetStructureChecking (destDoc);
      TtaSetStructureChecking (0, destDoc);
      /* get elem, the ancestor of *el which is a child of a DIV or BODY
	 element in the destination document. The copied elements will be
	 inserted just before this element. */
      elem = *el;
      do
	{
	  ancestor = TtaGetParent (elem);
	  if (ancestor != NULL)
	    {
	      elType = TtaGetElementType (ancestor);
	      if (elType.ElTypeNum == HTML_EL_BODY ||
		  elType.ElTypeNum == HTML_EL_Division)
		ancestor = NULL;
	      else
		elem = ancestor;
	    }
	}
      while (ancestor != NULL);
      parent = TtaGetParent (elem);

      /* insert a DIV element */
      elType.ElTypeNum = HTML_EL_Division;
      lastInserted = TtaNewElement (destDoc, elType);
      TtaInsertSibling (lastInserted, elem, TRUE, destDoc);
      RegisterSubDoc (lastInserted, url);
      CreateTargetAnchor (destDoc, lastInserted);

      /* do copy */
      firstInserted = NULL;
      if (isID)
	srce = root;
      else
	srce = TtaGetFirstChild (root);
      while (srce != NULL)
	{
	  copy = TtaCopyTree (srce, sourceDoc, destDoc, parent);
	  if (copy != NULL)
	    {
	      if (firstInserted == NULL)
		/* this is the first copied element. Insert it before elem */
		{
		  TtaInsertFirstChild (&copy, lastInserted, destDoc);
		  firstInserted = copy;
		}
	      else
		/* insert the new copied element after the element previously
		   copied */
		TtaInsertSibling (copy, lastInserted, FALSE, destDoc);
	      lastInserted = copy;
	      /* update the NAMEs and URLs in the copied element */
	      event.document = destDoc;
	      event.position = sourceDoc;
	      UpdateURLsInSubtree(&event, copy);
	    }
	  /* get the next element in the source document */
	  old = srce;
	  TtaNextSibling (&srce);
	  if (deleteTree)
	    TtaDeleteTree (old, sourceDoc);
	  /* Stop here if the target points to a specific element with an ID */
	  if (isID)
	    srce = NULL;
	}
      
      /* delete the element(s) containing the link to the copied document */
      /* delete the parent element of *el and all empty ancestors */
      elem = TtaGetParent (*el);
      do
	{
	  sibling = elem;
	  TtaNextSibling (&sibling);
	  if (sibling == NULL)
	    {
	      sibling = elem;
	      TtaPreviousSibling (&sibling);
	      if (sibling == NULL)
		elem = TtaGetParent (elem);
	    }
	}
      while (sibling == NULL);
      TtaDeleteTree (elem, destDoc);
      /* restore previous chacking mode */
      TtaSetStructureChecking (checkingMode, destDoc);
      /* return the address of the first copied element */
      *el = firstInserted;
    }
}


#ifdef __STDC__
void               GetIncludedDocuments_callback (int newdoc, int status, 
					     char *urlName,
					     char *outputfile, 
					     char *content_type,
					     void * context)
#else  /* __STDC__ */
void               GetIncludedDocuments_callback (newdoc, status, urlName,
                                             outputfile, content_type, 
                                             context)
int newdoc;
int status;
char *urlName;
char *outputfile;
char *content_type;
void *context;
#endif /* __STDC__ */
{
  Element		link, next;
  Attribute		RelAttr, HrefAttr;
  char			*url, *ptr, *text;
  Document		document;
  GetIncludedDocuments_context  *ctx;

  /* restore GetIncludedDocuments's context */
  ctx = (GetIncludedDocuments_context *) context;
  
  if (!ctx)
    return;

  RelAttr = ctx->RelAttr;
  link = ctx->link;
  HrefAttr = ctx->HrefAttr;
  url = ctx->url;
  ptr = ctx->ptr;
  document = ctx->document;
  next = ctx->next;
  text = ctx->text;

  TtaFreeMemory (ctx);

  if (RelAttr != NULL && link != NULL)
    {
      if (HrefAttr != NULL)
	{
	  if (url != NULL)
	    {
	      if (newdoc != 0 && newdoc != document)
		{
		  /* it's not the document itself */
		  /* copy the target document at the position of the link */
		  MoveDocumentBody (&next, document, newdoc, ptr, url,
				    newdoc == IncludedDocument);
		}
	      /* global variables */
	      FreeDocumentResource (IncludedDocument);
	      TtaCloseDocument (IncludedDocument);
	      IncludedDocument = 0;
	    }
	  TtaFreeMemory (text);
	}
    }
  
  if (next != NULL && DocBook == document)
    {
      SetStopButton (document);
      GetIncludedDocuments (next, document);
    }
  else
    MakeBook_callback (document);
}

/*----------------------------------------------------------------------
  GetIncludedDocuments
  Look forward, starting from element el, for a link (A) with attribute
  REL="chapter" or REL="subdocument" and replace that link by the contents
  of the target document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         GetIncludedDocuments (Element el, Document document)
#else
static void         GetIncludedDocuments (el, document)
Element		    el;
Document            document;
#endif
{
  Element		link, next;
  Attribute		RelAttr, HrefAttr = NULL;
  AttributeType	attrType;
  int			length;
  char			*text, *ptr, *url;
  Document		newdoc;
  boolean              call_callback = FALSE;
  GetIncludedDocuments_context  *ctx = NULL;

  link = el;
  RelAttr = NULL;
  next = NULL;

  ctx = TtaGetMemory (sizeof (GetIncludedDocuments_context));
  memset (ctx, 0, sizeof (GetIncludedDocuments_context));

  attrType.AttrSSchema = TtaGetDocumentSSchema (document);
  attrType.AttrTypeNum = HTML_ATTR_REL;


  /* looks for an anchor having an attribute REL="chapter" or
     REL="subdocument" */
  while (link != NULL && RelAttr == NULL)
    {
      TtaSearchAttribute (attrType, SearchForward, link, &link, &RelAttr);
      if (link != NULL && RelAttr != NULL)
	{
	  length = TtaGetTextAttributeLength (RelAttr);
	  text = TtaGetMemory (length + 1);
	  TtaGiveTextAttributeValue (RelAttr, text, &length);
	  if (strcasecmp (text, "chapter") && strcasecmp (text, "subdocument"))
	    RelAttr = NULL;
	  TtaFreeMemory (text);
	}
    }
  
  ctx->RelAttr = RelAttr;
  ctx->link =  link;
  ctx->document = document;
  ctx->HrefAttr = HrefAttr;

  if (RelAttr != NULL && link != NULL)
    /* a link with attribute REL="Chapter" has been found */
    {
      next = link;
      ctx->next =next;
      attrType.AttrTypeNum = HTML_ATTR_HREF_;
      HrefAttr = TtaGetAttribute (link, attrType);
      if (HrefAttr != NULL)
	 /* this link has an attribute HREF */
	 {
	   length = TtaGetTextAttributeLength (HrefAttr);
	   text = TtaGetMemory (length + 1);
	   TtaGiveTextAttributeValue (HrefAttr, text, &length);
	   ptr = strrchr (text, '#');
	   url = text;
	   if (ptr != NULL)
	     {
	       if (ptr == text)
		 url = NULL;
	       /* link to a particular position within a remote document */
	       ptr[0] = EOS;
	       ptr = &ptr[1];
	     }
	   
	   ctx->url = url;
	   ctx->ptr = ptr;
	   ctx->text = text;
	   ctx->HrefAttr = HrefAttr;

	   if (url != NULL)
	     /* this link designates an external document */
	     {
	       /* create a new document and loads the target document */
	       IncludedDocument = TtaNewDocument ("HTML", "tmp");
	       TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_FETCHING), url);
	       newdoc = GetHTMLDocument (url, NULL, IncludedDocument,
					 document, CE_MAKEBOOK, FALSE, 
					 (void *) GetIncludedDocuments_callback,
					 (void *) ctx);
	     }
	   else 
	     call_callback = TRUE;
         }
       else
	 call_callback = TRUE;
     } 
   else
     call_callback = TRUE;
    
    if (call_callback)
      GetIncludedDocuments_callback (document, -1, url, NULL, NULL, 
				     (void *) ctx);
}


/*----------------------------------------------------------------------
  MakeBook_callback
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void MakeBook_callback (Document document)
#else
void MakeBook_callback (Document document)
#endif /* __STDC__ */

{
  ResetStop (document);
  /* update internal links */
  SetInternalLinks (document);
  /* remove registered  sub-documents */
  FreeSubDocTable ();
  DocBook = 0;
  TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
}

/*----------------------------------------------------------------------
  MakeBook
  Replace all links in a document which have an attribute REL="chapter"
  or REL="subdocument" by the corresponding target document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                MakeBook (Document document, View view)
#else
void                MakeBook (document, view)
Document            document;
View                view;
#endif
{
   Element	    root, body, el;
   ElementType	    elType;

   /* stops all current transfers on this document */
   StopTransfer (document, 1);
   /* simulate a transfert in the main document */
   ActiveMakeBook (document);
   root = TtaGetMainRoot (document);
   elType = TtaGetElementType (root);
   elType.ElTypeNum = HTML_EL_BODY;
   body = TtaSearchTypedElement (elType, SearchForward, root);
   TtaSetDocumentModified (document);
   el = body;

   if (el != NULL && DocBook == document)
     GetIncludedDocuments (el, document);
   else
     MakeBook_callback (document);
}










