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
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version.
 */


/* Included headerfiles */
#define THOT_EXPORT
#include "amaya.h"
#include "AHTURLTools_f.h"
#include "print.h"
#include "css.h"

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
     STRING           SDname;
  }SubDoc;

/* the structure used for the GetIncludedDocuments_callback function */
typedef struct _IncludeCtxt
{
  Element		div; /* enclosing element for the search */
  Element		link; /* current processed link */
  STRING		url; /* called url */
  STRING                name; /* the fragment name */
  struct _IncludeCtxt  *ctxt; /* the previous context */
} IncludeCtxt;

/* shared with windialogapi.c */
ThotBool         PrintURL;
ThotBool	 NumberLinks;
ThotBool	 WithToC;
ThotBool         IgnoreCSS;

static struct _SubDoc  *SubDocs;
static CHAR_T           PSdir[MAX_PATH];
static CHAR_T           PPrinter[MAX_PATH];
static Document		DocPrint;
static int              PaperPrint;
static int              ManualFeed;
static int              PageSize;
static int              BasePrint;

#include "init_f.h"
#include "HTMLactions_f.h"
#include "HTMLbook_f.h"
#include "HTMLedit_f.h"
#include "HTMLhistory_f.h"
#include "UIcss_f.h"

#ifdef _WINDOWS 
#include "wininclude.h"
#endif /* _WINDOWS */

static ThotBool GetIncludedDocuments ();

/*----------------------------------------------------------------------
  RegisterSubDoc adds a new entry in SubDoc table.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RegisterSubDoc (Element el, STRING url)
#else
static void         RegisterSubDoc (el, url)
Element             el;
STRING              url;
#endif
{
  struct _SubDoc  *entry, *last;

  if (url == NULL || url[0] == EOS)
    return;

  entry = TtaGetMemory (sizeof (struct _SubDoc));
  entry->SDnext = NULL;
  entry->SDel = el;
  entry->SDname = TtaWCSdup (url);

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
static Element      SearchSubDoc (STRING url)
#else
static Element      SearchSubDoc (url)
STRING              url;
#endif
{
  Element          el;
  struct _SubDoc  *entry;
  ThotBool         docFound;

  if (url == NULL || url[0] == EOS)
    return (NULL);

  entry = SubDocs;
  docFound = FALSE;
  el = NULL;
  while (!docFound && entry != NULL)
    {
      docFound = (ustrcmp (url, entry->SDname) == 0);
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
  Element	        el, div, link, target, sibling;
  ElementType		elType, linkType;
  Attribute		HrefAttr, IntLinkAttr;
  Attribute             attr, ExtLinkAttr;
  AttributeType	        attrType;
  STRING		text, ptr, url; 
  CHAR_T                  number[10];
  CHAR_T                  value[MAX_LENGTH];
  int			length, i, volume;
  int                   status, position;
  ThotBool              split;

  /* Remember the current status of the document */
  status = TtaIsDocumentModified (document);
  el = TtaGetMainRoot (document);
  volume = TtaGetElementVolume (el);
  elType = TtaGetElementType (el);
  elType.ElTypeNum = HTML_EL_AnyLink;
  attrType.AttrSSchema = elType.ElSSchema;
  /* looks for all links in the document */
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
      usprintf (number, TEXT("%d"), position*100/volume);
      TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_UPDATED_LINK), number);
      TtaHandlePendingEvents ();
      link = TtaSearchTypedElement (elType, SearchForward, link);
      if (link != NULL)
	/* a link has been found */
	{
	  linkType = TtaGetElementType (link);
	  if (linkType.ElTypeNum == HTML_EL_Anchor)
	     attrType.AttrTypeNum = HTML_ATTR_HREF_;
	  else
	     attrType.AttrTypeNum = HTML_ATTR_cite;
	  HrefAttr = TtaGetAttribute (link, attrType);
	  attrType.AttrTypeNum = HTML_ATTR_InternalLink;
	  IntLinkAttr = TtaGetAttribute (link, attrType);
	  attrType.AttrTypeNum = HTML_ATTR_ExternalLink;
	  ExtLinkAttr = TtaGetAttribute (link, attrType);
	  if (HrefAttr == NULL)
	    /* this element is not a link (no href or cite attribute) */
	    /* remove attributes InternalLink and ExternalLink if they
	       are present */
	    {
	      if (IntLinkAttr != NULL)
		TtaRemoveAttribute (link, IntLinkAttr, document);
	      if (ExtLinkAttr != NULL)
		TtaRemoveAttribute (link, ExtLinkAttr, document);	   
	    }
	  else
	    /* this element has an HREF or cite attribute */
	    {
	      length = TtaGetTextAttributeLength (HrefAttr);
	      text = TtaAllocString (length + 1);
	      TtaGiveTextAttributeValue (HrefAttr, text, &length);

	      /* does an external link become an internal link ? */
	      if (document == DocBook && SubDocs != NULL)
		{
		  ptr = ustrrchr (text, '#');
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
			  ustrcpy (value, ptr);
			  length = ustrlen (value);
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
				  usprintf (&value[length], TEXT("%d"), i);
				  target = SearchNAMEattribute (document,
							&value[1], NULL);
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
		    TtaSetAttributeReference (IntLinkAttr, link, document,
					      target, document);
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
   CHAR_T         docName[MAX_LENGTH];
   CHAR_T*        ptr; 
   CHAR_T         suffix[MAX_LENGTH];
   int              lg;

   if (DocPrint != document)
     {
       /* initialize print parameters */
       DocPrint = document;

       /* define the new default PS file */
       ptr = TtaGetEnvString ("APP_TMPDIR");
       if (ptr != NULL && TtaCheckDirectory (ptr))
	     ustrcpy (PSdir, ptr);
       else
	     ustrcpy (PSdir, TtaGetDefEnvString ("APP_TMPDIR"));
	   lg = ustrlen (PSdir);
	   if (PSdir[lg - 1] == WC_DIR_SEP)
	     PSdir[--lg] = WC_EOS;

       ustrcpy (docName, TtaGetDocumentName (document));
       ExtractSuffix (docName, suffix);
       usprintf (&PSdir[lg], TEXT("%c%s.ps"), WC_DIR_SEP, docName);
       TtaSetPsFile (PSdir);
       /* define the new default PrintSchema */
       NumberLinks = FALSE;
       WithToC = FALSE;
       IgnoreCSS = FALSE;
       PrintURL = TRUE;
       TtaSetPrintSchema (TEXT(""));
       /* no manual feed */
       ManualFeed = PP_OFF;
       TtaSetPrintParameter (PP_ManualFeed, ManualFeed);
     }
}


/*----------------------------------------------------------------------
   PrintAs prints the document using predefined parameters.
   ----------------------------------------------------------------------*/  
#ifdef __STDC__
static void         PrintDocument (Document doc, View view)
#else  /* __STDC__ */
static void         PrintDocument (doc, view)
Document            doc;
#endif /* __STDC__ */
{
  AttributeType      attrType;
  Attribute          attr;
  Element            el;
  STRING             files, dir;
  CHAR_T             viewsToPrint[MAX_PATH];
  ThotBool           status, textFile;

  textFile = (DocumentTypes[doc] == docText ||
	      DocumentTypes[doc] == docTextRO ||
	      DocumentTypes[doc] == docCSS ||
	      DocumentTypes[doc] == docCSSRO);

   CheckPrintingDocument (doc);
   ustrcpy (viewsToPrint, TEXT("Formatted_view "));
   if (!textFile && WithToC)
     ustrcat (viewsToPrint, TEXT("Table_of_contents "));
   if (!textFile && NumberLinks)
     /* display numbered links */
     {
       /* associate an attribute InternalLink with all anchors refering
	  a target in the same document.  This allows P schemas to work
	  properly */
       SetInternalLinks (DocPrint);
       if (PageSize == PP_A4)
	 TtaSetPrintSchema (TEXT("HTMLPLP"));
       else
	 TtaSetPrintSchema (TEXT("HTMLPLPUS"));
       ustrcat (viewsToPrint, TEXT("Links_view "));
     }
   else
     {
       if (PageSize == PP_A4)
	 {
	   if (textFile)
	     TtaSetPrintSchema (TEXT("TextFilePP"));
	   else
	     TtaSetPrintSchema (TEXT("HTMLPP"));
	 }
       else
	 {
	   if (textFile)
	     TtaSetPrintSchema (TEXT("TextFilePPUS"));
	   else
	     TtaSetPrintSchema (TEXT("HTMLPPUS"));
	 }    
     }
   /* post or remove the PrintURL attribute */
   el =  TtaGetMainRoot (doc);
   status = TtaIsDocumentModified (doc);
   attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
   if (textFile)
     attrType.AttrTypeNum = TextFile_ATTR_PrintURL;
   else
     attrType.AttrTypeNum = HTML_ATTR_PrintURL;
   attr = TtaGetAttribute (el, attrType);
   if (attr == 0 && PrintURL)
     {
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);
     }

   if (attr != 0 && !PrintURL)
     TtaRemoveAttribute (el, attr, doc);
   /* get the path dir where css files have to be stored */
   if (textFile || IgnoreCSS)
     files = NULL;
   else
     {
       TtaGetPrintNames (&files, &dir);
       /* store css files and get the list of names */
       files = CssToPrint (doc, dir);
     }
   TtaPrint (DocPrint, viewsToPrint, files);
   TtaFreeMemory (files);
   if (!status)
     TtaSetDocumentUnmodified (doc);
}

/*----------------------------------------------------------------------
   PrintAs prints the document using predefined parameters.
   ----------------------------------------------------------------------*/  
#ifdef __STDC__
void                PrintAs (Document doc, View view)
#else  /* __STDC__ */
void                PrintAs (doc, view)
Document            doc;
#endif /* __STDC__ */
{
#ifdef _WINDOWS 
  SetupAndPrint (doc, view);
#else /* _WINDOWS */
  PrintDocument (doc, view);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   CallbackImage manage returns of Picture form.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackPrint (int ref, int typedata, STRING data)
#else  /* __STDC__ */
void                CallbackPrint (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;
#endif /* __STDC__ */
{
  int                 val;

  val = (int) data;
  switch (ref - BasePrint)
    {
    case NumFormPrint:
      TtaDestroyDialogue (BasePrint+NumFormPrint);
      switch (val)
	{
	case 1:
	  /* confirms the paper print option */
	  /* the other options are not taken into account without this
	     confirmation */
	  TtaSetPrintParameter (PP_Destination, PaperPrint);
	  TtaSetPrintParameter (PP_ManualFeed, ManualFeed);
	  TtaSetPrintParameter (PP_PaperSize, PageSize);
	  TtaSetPrintCommand (PPrinter);
	  TtaSetPsFile (PSdir);
	  /* update the environment variable */
	  TtaSetEnvString ("THOTPRINT", PPrinter, TRUE);
	  TtaSetEnvInt ("PAPERSIZE", PageSize, TRUE);
	  PrintDocument (DocPrint, 1);
	  break;
	case 0:
	  PaperPrint = (TtaGetPrintParameter (PP_Destination)) ? PP_PRINTER : PP_PS;
	  ManualFeed = TtaGetPrintParameter (PP_ManualFeed);
	  PageSize = TtaGetPrintParameter (PP_PaperSize);	  
	  TtaGetPrintCommand (PPrinter);
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
	  WithToC = !WithToC;
	  break;
	case 2:
	  /* NumberLinks option */
	  NumberLinks = !NumberLinks;
	case 3:
	  /* URL option */
	  PrintURL = !PrintURL;
	  break;
	case 4:
	  /* CSS option */
	  IgnoreCSS = !IgnoreCSS;
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
#         ifndef _WINDOWS
	      TtaSetTextForm (BasePrint+NumZonePrinterName, PPrinter);
#         endif /* !_WINDOWS */
	    }
	  break;
	case 1:
	  if (PaperPrint == PP_PRINTER)
	    {
	      PaperPrint = PP_PS;
#         ifndef _WINDOWS
	      TtaSetTextForm (BasePrint+NumZonePrinterName, PSdir);
#         endif /* !_WINDOWS */
	    }
	  break;
	}
      break;
    case NumZonePrinterName:
      if (data[0] != '\0')
	if (PaperPrint == PP_PRINTER)
	    /* text capture zone for the printer name */
	    ustrncpy (PPrinter, data, MAX_PATH);
	else
	  /* text capture zone for the name of the PostScript file */
	  ustrncpy (PSdir, data, MAX_PATH);
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
  CHAR_T* ptr;

   BasePrint = TtaSetCallback (CallbackPrint, PRINT_MAX_REF);
   DocPrint = 0;

   /* init printer variables */
   /* read default printer variable */
   ptr = TtaGetEnvString ("THOTPRINT");
   if (ptr == NULL)
     ustrcpy (PPrinter, TEXT(""));
   else
     ustrcpy (PPrinter, ptr);
   TtaGetEnvInt ("PAPERSIZE", &PageSize);
   PaperPrint = PP_PRINTER;
   PrintURL = TRUE;
   IgnoreCSS = FALSE;
   TtaSetPrintParameter (PP_Destination, PaperPrint);
   TtaSetPrintParameter (PP_PaperSize, PageSize);
   TtaSetPrintCommand (PPrinter);
}

/*----------------------------------------------------------------------
  SetupAndPrint sets printing parameters and starts the printing process
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetupAndPrint (Document doc, View view)
#else
void                SetupAndPrint (doc, view)
Document            doc;
View                view;
#endif
{
#  ifndef _WINDOWS
   CHAR_T             bufMenu[MAX_LENGTH];
   int              i;
#  endif /* !_WINDOWS */
   ThotBool           textFile;

   textFile = (DocumentTypes[doc] == docText ||
	       DocumentTypes[doc] == docTextRO ||
	       DocumentTypes[doc] == docCSS ||
	       DocumentTypes[doc] == docCSSRO);

   /* Print form */
   CheckPrintingDocument (doc);

   /* read the values that the user may have changed thru
      the configuration menu */
   TtaGetPrintCommand (PPrinter);
   PageSize = TtaGetPrintParameter (PP_PaperSize);	  

#  ifndef _WINDOWS
   TtaNewSheet (BasePrint+NumFormPrint, TtaGetViewFrame (doc, view), 
		TtaGetMessage (LIB, TMSG_LIB_PRINT),
	   1, TtaGetMessage (AMAYA, AM_BUTTON_PRINT), FALSE, 2, 'L', D_CANCEL);
   i = 0;
   sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (LIB, TMSG_MANUAL_FEED));
   i += ustrlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_PRINT_TOC));
   i += ustrlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_NUMBERED_LINKS));
   i += ustrlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_PRINT_URL));
   i += ustrlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_WITH_CSS));
   TtaNewToggleMenu (BasePrint+NumMenuOptions, BasePrint+NumFormPrint,
		TtaGetMessage (LIB, TMSG_OPTIONS), 5, bufMenu, NULL, FALSE);
   if (ManualFeed == PP_ON)
     TtaSetToggleMenu (BasePrint+NumMenuOptions, 0, TRUE);
   else
     TtaSetToggleMenu (BasePrint+NumMenuOptions, 0, FALSE);
   TtaSetToggleMenu (BasePrint+NumMenuOptions, 1, WithToC);
   TtaSetToggleMenu (BasePrint+NumMenuOptions, 2, NumberLinks);
   TtaSetToggleMenu (BasePrint+NumMenuOptions, 3, PrintURL);
   TtaSetToggleMenu (BasePrint+NumMenuOptions, 4, IgnoreCSS);

   /* Paper format submenu */
   i = 0;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_A4));
   i += ustrlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_US));
   TtaNewSubmenu (BasePrint+NumMenuPaperFormat, BasePrint+NumFormPrint, 0,
	     TtaGetMessage (LIB, TMSG_PAPER_SIZE), 2, bufMenu, NULL, FALSE);
   if (PageSize == PP_US)
      TtaSetMenuForm (BasePrint+NumMenuPaperFormat, 1);
   else
      TtaSetMenuForm (BasePrint+NumMenuPaperFormat, 0);

   /* Print to paper/ Print to file submenu */
   i = 0;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_PRINTER));
   i += ustrlen (&bufMenu[i]) + 1;
   sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_PS_FILE));
   TtaNewSubmenu (BasePrint+NumMenuSupport, BasePrint+NumFormPrint, 0,
                  TtaGetMessage (LIB, TMSG_OUTPUT), 2, bufMenu, NULL, TRUE);
   /* text capture zone for the printer name */
   TtaNewTextForm (BasePrint+NumZonePrinterName, BasePrint+NumFormPrint, NULL, 30, 1, FALSE);

   /* initialization of the PaperPrint selector */
   if (PaperPrint == PP_PRINTER)
     {
	TtaSetMenuForm (BasePrint+NumMenuSupport, 0);
	TtaSetTextForm (BasePrint+NumZonePrinterName, PPrinter);
     }
   else
     {
	TtaSetMenuForm (BasePrint+NumMenuSupport, 1);
	TtaSetTextForm (BasePrint+NumZonePrinterName, PSdir);
     }

   /* activates the Print form */
    TtaShowDialogue (BasePrint+NumFormPrint, FALSE);
    if (textFile) {
       /* invalid dialogue entries */
       TtaRedrawMenuEntry (BasePrint+NumMenuOptions, 1, NULL, -1, FALSE);
       TtaRedrawMenuEntry (BasePrint+NumMenuOptions, 2, NULL, -1, FALSE);	      
	}
#   else  /* _WINDOWS */
    CreatePrintDlgWindow (TtaGetViewFrame (doc, view), PSdir, BasePrint, NumMenuSupport, NumMenuOptions, NumMenuPaperFormat, NumZonePrinterName, NumFormPrint);
#   endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  SectionNumbering
  Execute the "Section Numbering" command
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SectionNumbering (Document doc, View view)
#else
void                SectionNumbering (doc, view)
Document            doc;
View                view;
#endif
{
   ChangeAttrOnRoot (doc, HTML_ATTR_SectionNumbering);
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

  nextEl = TtaGetFirstChild (el);
  while (nextEl != NULL)
    {
      event->element = nextEl;
      ElementPasted (event);
      TtaNextSibling (&nextEl);
    }
}


/*----------------------------------------------------------------------
  MoveDocumentBody
  Copy the elements contained in the BODY of the document sourceDoc at the
  position of the element el in the document destDoc.
  Delete the element containing el and all its empty ancestors.
  If deleteTree is TRUE, copied elements are deleted from the source
  document.
  Return the root element that delimits the new inserted part (a div).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element MoveDocumentBody (Element el, Document destDoc, Document sourceDoc, STRING target, STRING url, ThotBool deleteTree)
#else
static Element MoveDocumentBody (el, destDoc, sourceDoc, target, url, deleteTree)
Element        el;
Document       destDoc;
Document       sourceDoc;
STRING         target;
STRING         url;
ThotBool       deleteTree;
#endif
{
  Element	   root, ancestor, elem, firstInserted, div;
  Element          lastInserted, srce, copy, old, parent, sibling;
  ElementType	   elType;
  NotifyElement    event;
  int		   checkingMode;
  ThotBool         isID;

  if (target != NULL)
    {
      /* locate the target element within the source document */
      root = SearchNAMEattribute (sourceDoc, target, NULL);
      elType = TtaGetElementType (root);
      isID = (elType.ElTypeNum != HTML_EL_Anchor && elType.ElTypeNum != HTML_EL_MAP);
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
      /* get elem, the ancestor of el which is a child of a DIV or BODY
	 element in the destination document. The copied elements will be
	 inserted just before this element. */
      elem = el;
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
      /* this delimits the new inserted part of the document */
      RegisterSubDoc (lastInserted, url);
      CreateTargetAnchor (destDoc, lastInserted, FALSE);
      div = lastInserted;

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
      /* delete the parent element of el and all empty ancestors */
      elem = TtaGetParent (el);
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
      TtaSetStructureChecking ((ThotBool)checkingMode, destDoc);
    }
  /* return the address of the new division */
  return (div);
}


/*----------------------------------------------------------------------
  CloseMakeBook
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void CloseMakeBook (Document document)
#else
static void CloseMakeBook (document)
Document    document;
#endif /* __STDC__ */

{
  ResetStop (document);
  /* update internal links */
  SetInternalLinks (document);
  /* if the document changed force the browser mode */
  if (SubDocs)
    SetBrowserEditor (document);
  /* remove registered  sub-documents */
  FreeSubDocTable ();
  DocBook = 0;
  TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
}


/*----------------------------------------------------------------------
  GetIncludedDocuments_callback finishes the GetIncludedDocuments procedure
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void   GetIncludedDocuments_callback (int newdoc, int status, 
				      STRING urlName,
				      STRING outputfile, 
				      AHTHeaders *http_headers,
				      void * context)
#else  /* __STDC__ */
void   GetIncludedDocuments_callback (newdoc, status, urlName,
				      outputfile, http_headers,
				      context)
int    newdoc;
int    status;
STRING urlName;
STRING outputfile;
AHTHeaders *http_headers;
void  *context;
#endif /* __STDC__ */
{
  Element		link, div;
  IncludeCtxt          *ctx, *prev;
  STRING		url, ptr;
  ThotBool              found = FALSE;

  /* restore GetIncludedDocuments's context */
  ctx = (IncludeCtxt *) context;  
  if (!ctx)
    return;

  div = NULL;
  link = ctx->link;
  ptr = ctx->name;
  url = ctx->url;
  if (url)
    {
      if (newdoc && newdoc != DocBook)
	{
	  /* it's not the DocBook itself */
	  /* copy the target document at the position of the link */
	  TtaSetDocumentModified (DocBook);
	  div = MoveDocumentBody (link, DocBook, newdoc, ptr, url,
				  (ThotBool)(newdoc == IncludedDocument));
	}
      /* global variables */
      FreeDocumentResource (IncludedDocument);
      TtaCloseDocument (IncludedDocument);
      IncludedDocument = 0;
    }

  if (div != NULL)
    {
      /* new starting point for the search */
      ctx->link = div;
      found = GetIncludedDocuments (div, div, DocBook, ctx);
    }
  while (!found && ctx)
    {
      /* this sub-document has no more inclusion, examine the caller */
      div = ctx->div;
      link = ctx->link;
      prev = ctx->ctxt;
      TtaFreeMemory (url);
	  url = NULL;
      TtaFreeMemory (ctx);
      ctx = prev;
      found = GetIncludedDocuments (div, link, DocBook, ctx);
    }
  if (!found)
    /* all links are now managed */
    CloseMakeBook (DocBook);
}

/*----------------------------------------------------------------------
  GetIncludedDocuments
  Look forward within the element el, starting from element link, for a 
  link (A) with attribute rel="chapter" or rel="subdocument" and replace
  that link by the contents of the target document.
  Return TRUE if one inclusion is launched.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool  GetIncludedDocuments (Element el, Element link, Document document, IncludeCtxt *prev)
#else
static ThotBool  GetIncludedDocuments (el, link, document, prev)
Element	     el;
Element	     link;
Document     document;
IncludeCtxt *prev;
#endif
{
  ElementType           elType;
  Attribute		attr;
  AttributeType	        attrType;
  Document		newdoc;
  IncludeCtxt          *ctx = NULL;
  STRING		text, ptr, url = NULL;
  int			length;
  ThotBool              found = FALSE;

  /* look for anchors with the attribute rel within the element  el */
  attr = NULL;
  attrType.AttrSSchema = TtaGetSSchema (TEXT("HTML"), document);
  elType.ElSSchema = attrType.AttrSSchema;
  elType.ElTypeNum = HTML_EL_Anchor;

  /* Get only one included file each time */
  while (link && attr == NULL)
    {
      link = TtaSearchTypedElementInTree (elType, SearchForward, el, link);
      if (link)
	{
	  attrType.AttrTypeNum = HTML_ATTR_REL;
	  attr = TtaGetAttribute (link, attrType);
	}
      if (attr)
	{
	  length = TtaGetTextAttributeLength (attr);
	  text = TtaAllocString (length + 1);
	  TtaGiveTextAttributeValue (attr, text, &length);
	  /* Valid rel values are rel="chapter" or rel="subdocument" */
	  if (ustrcasecmp (text, TEXT("chapter")) &&
	      ustrcasecmp (text, TEXT("subdocument")))
	    attr = NULL;
	  TtaFreeMemory (text);
	}
  
      if (attr)
	{
	  /* a link with attribute rel="Chapter" has been found */
	  attrType.AttrTypeNum = HTML_ATTR_HREF_;
	  attr = TtaGetAttribute (link, attrType);
	}
      if (attr)
	/* this link has an attribute HREF */
	{
	  length = TtaGetTextAttributeLength (attr);
	  text = TtaAllocString (length + 1);
	  TtaGiveTextAttributeValue (attr, text, &length);
	  ptr = ustrrchr (text, '#');
	  url = text;
	  if (ptr != NULL)
	    {
	      if (ptr == text)
		url = NULL;
	      /* link to a particular position within a remote document */
	      ptr[0] = EOS;
	      ptr = &ptr[1];
	    }
		  
	  if (url != NULL)
	    /* this link designates an external document */
	    {
	      /* create a new document and loads the target document */
	      IncludedDocument = TtaNewDocument (TEXT("HTML"), TEXT("tmp"));
	      if (IncludedDocument != 0)
		{
		  TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_FETCHING), url);
		  ctx = TtaGetMemory (sizeof (IncludeCtxt));
		  ctx->div =  el;
		  ctx->link = link;
		  ctx->url = url; /* the URL of the document */
		  ctx->name = ptr;
		  ctx->ctxt = prev; /* previous context */
		  /* Get the reference of the calling document */
		  SetStopButton (document);
		  newdoc = GetHTMLDocument (url, NULL, IncludedDocument,
					    document, CE_MAKEBOOK, FALSE, 
					    (void *) GetIncludedDocuments_callback,
					    (void *) ctx);
		  found = TRUE;
		}
	    }
	  else
	    TtaFreeMemory (text);
	}
    }
  return (found);
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
  Element	    root, body;
  ElementType	    elType;

  /* stops all current transfers on this document */
  StopTransfer (document, 1);
  /* simulate a transfert in the main document */
  DocBook = document;
  IncludedDocument = 0;
  root = TtaGetMainRoot (document);
  elType = TtaGetElementType (root);
  elType.ElTypeNum = HTML_EL_BODY;
  body = TtaSearchTypedElement (elType, SearchForward, root);

  if (body)
    GetIncludedDocuments (body, body, document, NULL);
}
