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

/* Thot printer variables */
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


/*----------------------------------------------------------------------
  SetInternalLinks
  Associate a InternalLink attribute with all anchor (A) elements of the
  document which designate another anchor in the same document.
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
   Element	        root, el;
   Element		link, target;
   ElementType		elType;
   Attribute		HrefAttr, IntLinkAttr;
   AttributeType	attrType;
   int			length;
   int                  status;
   char		       *text;

   /* Memorize the current status of the document */
   status = TtaIsDocumentModified (document);
   root = TtaGetMainRoot (document);
   elType = TtaGetElementType (root);
   elType.ElTypeNum = HTML_EL_BODY;
   el = TtaSearchTypedElement (elType, SearchForward, root);

   elType.ElTypeNum = HTML_EL_Anchor;
   attrType.AttrSSchema = elType.ElSSchema;
   /* looks for all anchors in the document */
   link = el;
   while (link != NULL)
     {
       link = TtaSearchTypedElement (elType, SearchForward, link);
       if (link != NULL)
	 /* an anchor has been found */
	 {
	 attrType.AttrTypeNum = HTML_ATTR_HREF_;
         HrefAttr = TtaGetAttribute (link, attrType);
         if (HrefAttr != NULL)
	   /* this anchor has an HREF attribute */
	   {
	   length = TtaGetTextAttributeLength (HrefAttr);
	   text = TtaGetMemory (length + 1);
	   TtaGiveTextAttributeValue (HrefAttr, text, &length);
	   if (text[0] == '#')
	      /* it'a an internal link. Attach an attribute InternalLink to */
	      /* the link, if this attribute does not exist yet */
	      {
	        attrType.AttrTypeNum = HTML_ATTR_InternalLink;
		IntLinkAttr = TtaGetAttribute (link, attrType);
		if (IntLinkAttr == NULL)
		   {
		     IntLinkAttr = TtaNewAttribute (attrType);
		     TtaAttachAttribute (link, IntLinkAttr, document);
		   }
		/* looks for the target element */
		target = SearchNAMEattribute (document, &text[1], NULL);
		if (target != NULL)
		   /* set the Thot link */
		   TtaSetAttributeReference (IntLinkAttr, link, document,
					     target, document);
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
       sprintf (&PSdir[lg], "/%s.ps", docName);
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
   PathBuffer          viewsToPrint;

   CheckPrintingDocument (document);
   strcpy (viewsToPrint, "Formatted_view ");
   if (withToC)
     strcat (viewsToPrint, "Table_of_contents ");
   if (numberLinks)
     {
       /* display numbered links */
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
	  if (!numberLinks)
	    SetInternalLinks (docPrint);
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
   char             bufMenu[MAX_LENGTH];
   int              i;

   /* Print form */
   CheckPrintingDocument (document);
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
static void         MoveDocumentBody (Element *el, Document destDoc,
				      Document sourceDoc, boolean deleteTree)
#else
static void         MoveDocumentBody (el, destDoc, sourceDoc, deleteTree)
Element            *el;
Document           destDoc;
Document           sourceDoc;
boolean            deleteTree;
#endif
{
   Element	   root, body, ancestor, elem, firstInserted,
		   lastInserted, srce, copy, old, parent, sibling;
   ElementType	   elType;
   NotifyElement   event;
   int		   checkingMode;

   firstInserted = NULL;
   /* get the BODY element of source document */
   root = TtaGetMainRoot (sourceDoc);
   elType = TtaGetElementType (root);
   elType.ElTypeNum = HTML_EL_BODY;
   body = TtaSearchTypedElement (elType, SearchForward, root);
   if (body != NULL)
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
	if (ancestor != NULL);
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
     
     /* do copy */
     lastInserted = NULL;
     srce = TtaGetFirstChild (body);
     while (srce != NULL)
	{
	copy = TtaCopyTree (srce, sourceDoc, destDoc, parent);
	if (copy != NULL)
	   {
	   if (lastInserted == NULL)
	      /* this is the first copied element. Insert it before elem */
	      {
	      TtaInsertSibling (copy, elem, TRUE, destDoc);
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

/*----------------------------------------------------------------------
  GetIncludedDocuments
  Look forward, starting from element el, for a link (A) with attribute
  REL="chapter" or REL="subdocument" and replace that link by the contents
  of the target document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      GetIncludedDocuments (Element el, Document document)
#else
static Element      GetIncludedDocuments (el, document)
Element		    el;
Document            document;
#endif
{
   Element		link, next;
   Attribute		RelAttr, HrefAttr;
   AttributeType	attrType;
   int			length;
   char			*text, *ptr;
   Document		includedDocument, newdoc;

   attrType.AttrSSchema = TtaGetDocumentSSchema (document);
   attrType.AttrTypeNum = HTML_ATTR_REL;
   link = el;
   RelAttr = NULL;
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

   if (RelAttr != NULL && link != NULL)
     /* a link with attribute REL="Chapter" has been found */
     {
       next = link;
       attrType.AttrTypeNum = HTML_ATTR_HREF_;
       HrefAttr = TtaGetAttribute (link, attrType);
       if (HrefAttr != NULL)
	 /* this link has an attribute HREF */
	 {
	   length = TtaGetTextAttributeLength (HrefAttr);
	   text = TtaGetMemory (length + 1);
	   TtaGiveTextAttributeValue (HrefAttr, text, &length);
	   /* ignore links to a particular position within a document */
	   ptr = strrchr (text, '#');
	   if (ptr == NULL)
	     /* this link designate the whole document */
	     {
	       /* create a new document and loads the target document */
	       includedDocument = TtaNewDocument ("HTML", "tmp");
	       TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_FETCHING), text);
	       newdoc = GetHTMLDocument (text, NULL, includedDocument,
					 document, DC_TRUE);
	       if (newdoc != 0 && newdoc != document)
		   /* it's not the document itself */
		   /* copy the target document at the position of the link */
		   MoveDocumentBody (&next, document, newdoc,
				     newdoc == includedDocument);
	       FreeDocumentResource (includedDocument);
	       TtaCloseDocument (includedDocument);
	     }
	   TtaFreeMemory (text);
	 }
       return (next);
     }
   return (NULL);
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

   root = TtaGetMainRoot (document);
   elType = TtaGetElementType (root);
   elType.ElTypeNum = HTML_EL_BODY;
   body = TtaSearchTypedElement (elType, SearchForward, root);
   TtaSetDocumentModified (document);
   el = body;
   while (el != NULL)
      el = GetIncludedDocuments (el, document);
   TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), "");
}


#ifdef R_HTML
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         LoadEntity (Document document, char *text)
#else
static void         LoadEntity (document, text)
Document            document;
char               *text;
#endif
{
  Document          includedDocument;
  Attribute	    attr;
  AttributeType	    attrType;
  Element           el, includedEl;
  ElementType	    elType;
  int               length;

  /* create the temporary document */
  includedDocument = TtaNewDocument ("HTML", "tmp");
  /* read the temporary document */
  includedDocument = GetHTMLDocument (text, NULL, includedDocument, document, DC_TRUE);
  
  if (includedDocument != 0)
    {
      /* To do: Seach entity in the table */
      /* locate the entity in the document */
      el = TtaGetMainRoot (document);
      elType = TtaGetElementType (el);
      elType.ElTypeNum = HTML_EL_Entity;
      /* TtaSearchElementByLabel (label, el); */
      el = TtaSearchTypedElement (elType, SearchForward, el);
      /* keep the entity name to know where to insert the sub-tree */
      attrType.AttrSSchema = TtaGetDocumentSSchema (document);
      attrType.AttrTypeNum = HTML_ATTR_entity_name;
      attr = TtaGetAttribute (el, attrType);
      if (attr != NULL)
	{
	  length = TtaGetTextAttributeLength (attr);
	  text = TtaGetMemory (length + 1);
	  TtaGiveTextAttributeValue (attr, text, &length);
	}
      /* To do: translate the entity name into element type
	 and search this first element type in included document */
      includedEl = TtaGetMainRoot (includedDocument);
      elType = TtaGetElementType (includedEl);
      elType.ElTypeNum = HTML_EL_TEXT_UNIT;
      includedEl = TtaSearchTypedElement (elType, SearchForward, includedEl);
      /* remove Entity */
      /* To do: insert sub-trees */
      FreeDocumentResource (includedDocument);
      TtaCloseDocument (includedDocument);
    }
}
#endif /* R_HTML */


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RealTimeHTML (Document document, View view)
#else
void                RealTimeHTML (document, view)
Document            document;
View                view;
#endif
{
#ifdef R_HTML
  Element	    root, el;
  ElementType	    elType;

  root = TtaGetMainRoot (document);
  elType = TtaGetElementType (root);
  elType.ElTypeNum = HTML_EL_Entity;
  el = TtaSearchTypedElement (elType, SearchForward, root);
  if (el != NULL)
    {
      /* document contains entities */
      /* To do -> build table of entities */

      /* simulate reception of different entities */
      LoadEntity (document, "0/0");
      LoadEntity (document, "0/1");
    }
#endif /* R_HTML */
}
