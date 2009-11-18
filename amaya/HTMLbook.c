/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Initialization functions and button functions of Amaya application.
 *
 * Authors: V. Quint, I. Vatton
 *          R. Guetari (W3C/INRIA) - Windows version.
 */

#ifdef _WX
#include "wx/wx.h"
#include "registry_wx.h"
#endif /* _WX */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "AHTURLTools_f.h"
#include "print.h"
#include "css.h"
#include "init_f.h"

#include "appdialogue_wx.h"
#include "message_wx.h"

/* structure to register sub-documents in MakeBook function*/
typedef struct _SubDoc
{
  struct _SubDoc  *SDnext;
  Element          SDel;
  char            *SDname;
}SubDoc;

/* the structure used for the GetIncludedDocuments_callback function */
typedef struct _IncludeCtxt
{
  Element		div; /* enclosing element for the search */
  Element		link; /* current processed link */
  char		       *utf8path; /* called url */
  char                 *name; /* the fragment name */
  struct _IncludeCtxt  *ctxt; /* the previous context */
} IncludeCtxt;

/* shared with windialogapi.c */
ThotBool         PrintURL;
ThotBool	 NumberLinks;
ThotBool	 WithToC;
ThotBool         IgnoreCSS;

static struct _SubDoc  *SubDocs = NULL;
static char             PSfile[MAX_PATH];
static char             PPrinter[MAX_PATH];
static char            *DocPrintURL;
static Document		DocPrint;
static int              PaperPrint;
static int              ManualFeed = PP_OFF;
static int              Orientation;
static int              PageSize;
static int              PagePerSheet;

#include "MENUconf.h"
#include "EDITORactions_f.h"
#include "init_f.h"
#include "HTMLactions_f.h"
#include "HTMLbook_f.h"
#include "HTMLedit_f.h"
#include "HTMLhistory_f.h"
#include "UIcss_f.h"

#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */

#ifdef _WX
#include "wxdialogapi_f.h"
#endif /* _WX */

static ThotBool GetIncludedDocuments (Element el, Element link,
                                      Document doc, IncludeCtxt *prev);
/*----------------------------------------------------------------------
  RedisplayDocument redisplays a view of a document
  ----------------------------------------------------------------------*/
void  RedisplayDocument(Document doc, View view)
{
  Element	       el;
  int		       position;
  int		       distance;

  /* get the current position in the document */
  position = RelativePosition (doc, &distance);
  TtaSetDisplayMode (doc, NoComputedDisplay);
  TtaSetDisplayMode (doc, DisplayImmediately);
  /* show the document at the same position */
  el = ElementAtPosition (doc, position);
  TtaShowElement (doc, view, el, distance);
}

/*----------------------------------------------------------------------
  RegisterSubDoc adds a new entry in SubDoc table.
  ----------------------------------------------------------------------*/
static void RegisterSubDoc (Element el, char *url)
{
  struct _SubDoc  *entry, *last;

  if (url == NULL || url[0] == EOS)
    return;

  entry = (struct _SubDoc *)TtaGetMemory (sizeof (struct _SubDoc));
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
static Element SearchSubDoc (char *url)
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
static void FreeSubDocTable ()
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
void SetInternalLinks (Document document)
{
  Element	        el, div, link, target, sibling;
  ElementType		elType, linkType;
  Attribute		HrefAttr, IntLinkAttr;
  Attribute             attr, ExtLinkAttr;
  AttributeType	        attrType;
  char		       *text, *ptr, *url; 
  char                  number[10];
  char                  value[MAX_LENGTH];
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
      sprintf (number, "%d", position*100/volume);
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
              text = (char *)TtaGetMemory (length + 1);
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
                          target = SearchNAMEattribute (document, &value[1], NULL, NULL);
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
                                  target = SearchNAMEattribute (document,
                                                                &value[1], NULL, NULL);
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
                  target = SearchNAMEattribute (document, ptr, NULL, NULL);
                  if (target != NULL)
                    /* set the Thot link */
                    TtaSetAttributeReference (IntLinkAttr, link, document,
                                              target);
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
static void CheckPrintingDocument (Document document)
{
  char         docName[MAX_LENGTH];
  char        *ptr; 
  char         suffix[MAX_LENGTH];
  int            lg;

  if (DocPrint != document || DocPrintURL == NULL ||
      strcmp(DocPrintURL, DocumentURLs[document]))
    {
      /* initialize print parameters */
      TtaFreeMemory (DocPrintURL);
      DocPrint = document;
      DocPrintURL = TtaStrdup (DocumentURLs[document]);
      
      /* define the new default PS file */
      ptr = TtaGetDocumentsDir ();
      TtaCheckMakeDirectory (ptr, TRUE);
      strcpy (PSfile, ptr);
      lg = strlen (PSfile);
      if (PSfile[lg - 1] == DIR_SEP)
        PSfile[--lg] = EOS;
      strcpy (docName, TtaGetDocumentName (document));
      TtaExtractSuffix (docName, suffix);
      sprintf (&PSfile[lg], "%c%s.ps", DIR_SEP, docName);
      TtaSetPsFile (PSfile);
    }
}

/*----------------------------------------------------------------------
  PrintDocument prints the document using predefined parameters.
  ----------------------------------------------------------------------*/  
static void PrintDocument (Document doc, View view)
{
  AttributeType      attrType;
  ElementType        elType;
  Attribute          attr;
  Element            el, docEl;
  char              *files, *dir;
  char               viewsToPrint[MAX_PATH];
  ThotBool           status, textFile;

  textFile = (DocumentTypes[doc] == docText ||
              DocumentTypes[doc] == docSource ||
              DocumentTypes[doc] == docCSS);

  /* initialize printing information */
  CheckPrintingDocument (doc);
  strcpy (viewsToPrint, "Formatted_view ");
  if (DocumentTypes[doc] == docHTML && WithToC)
    strcat (viewsToPrint, "Table_of_contents ");
  
  if (textFile)
    {
      if (PageSize == PP_A4)
        {
          if (Orientation == PP_Landscape)
            TtaSetPrintSchema ("TextFilePL");
          else
            TtaSetPrintSchema ("TextFilePP");
        }
      else
        {
          if (Orientation == PP_Landscape)
            TtaSetPrintSchema ("TextFileUSL");
          else
            TtaSetPrintSchema ("TextFilePPUS");
        }
    }
  else if (DocumentTypes[doc] == docSVG)
    TtaSetPrintSchema ("SVGP");
  else if (DocumentTypes[doc] == docMath)
    TtaSetPrintSchema ("MathMLP");
  else if (DocumentTypes[doc] == docAnnot)
    TtaSetPrintSchema ("AnnotP");
  else if (DocumentTypes[doc] == docHTML)
    {
      if (NumberLinks)
        /* display numbered links */
        {
          /* associate an attribute InternalLink with all anchors refering
             a target in the same document.  This allows P schemas to work
             properly */
          SetInternalLinks (DocPrint);
          if (PageSize == PP_A4)
            {
              if (Orientation == PP_Landscape)
                TtaSetPrintSchema ("HTMLPLL");
              else
                TtaSetPrintSchema ("HTMLPLP");
            }
          else
            {
              if (Orientation == PP_Landscape)
                TtaSetPrintSchema ("HTMLUSLL");
              else
                TtaSetPrintSchema ("HTMLPLPUS");
            }
          strcat (viewsToPrint, "Links_view ");
        }
      else if (PageSize == PP_A4)
        {
          if (Orientation == PP_Landscape)
            TtaSetPrintSchema ("HTMLPL");
          else
            TtaSetPrintSchema ("HTMLPP");
        }
      else
        {
          if (Orientation == PP_Landscape)
            TtaSetPrintSchema ("HTMLUSL");
          else
            TtaSetPrintSchema ("HTMLPPUS");
        }    
    }
  status = TtaIsDocumentModified (doc);

  if (textFile || DocumentTypes[doc] == docImage ||
      DocumentTypes[doc] == docHTML)
    {
      /* post or remove the PrintURL attribute */
      attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
      elType.ElSSchema = attrType.AttrSSchema;
      if (textFile)
        {
          elType. ElTypeNum = TextFile_EL_TextFile;
          attrType.AttrTypeNum = TextFile_ATTR_PrintURL;
        }
      else
        {
          elType. ElTypeNum = HTML_EL_HTML;
          attrType.AttrTypeNum = HTML_ATTR_PrintURL;
        }
      docEl = TtaGetMainRoot (doc);
      el = TtaSearchTypedElement (elType, SearchForward, docEl);
      attr = TtaGetAttribute (el, attrType);
      if (!attr && PrintURL)
        {
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
        }
      if (attr && !PrintURL)
        TtaRemoveAttribute (el, attr, doc);
    }
  
  /* get the path dir where css files have to be stored */
  if ((DocumentTypes[doc] == docHTML || DocumentTypes[doc] == docSVG ||
       DocumentTypes[doc] == docXml) &&
      !IgnoreCSS)
    {
      TtaGetPrintNames (&files, &dir);
      /* store css files and get the list of names */
      files = CssToPrint (doc, dir);
    }
  else
    files = NULL;
  TtaPrint (DocPrint, viewsToPrint, files);
  if (files)
    TtaFreeMemory (files);
  if (!status)
    TtaSetDocumentUnmodified (doc);
}

/*----------------------------------------------------------------------
  PrintAs prints the document using predefined parameters.
  ----------------------------------------------------------------------*/  
void PrintAs (Document doc, View view)
{
#ifdef _WINGUI
  DocPrint = doc;
  ReusePrinterDC ();
#else /* _WINGUI */
  PrintDocument (doc, view);
#endif /* _WINGUI */
}

/*----------------------------------------------------------------------
  CallbackImage handle return of Print form.                   
  ----------------------------------------------------------------------*/
void CallbackPrint (int ref, int typedata, char *data)
{
  long int            val;

  val = (long int) data;
  switch (ref - BasePrint)
    {
    case FormPrint:
      TtaDestroyDialogue (BasePrint + FormPrint);
      switch (val)
        {
        case 1:
          TtaSetPrintCommand (PPrinter);
          TtaSetPsFile (PSfile);
          /* update the environment variable */
          TtaSetEnvString ("THOTPRINT", PPrinter, TRUE);
          TtaSaveAppRegistry ();
          PrintDocument (DocPrint, 1);
          break;
        default:
          break;
        }
      break;
    case PrintOptions:
      switch (val)
        {
        case 0:
          /* Manual feed option */
          if (ManualFeed == PP_ON)
            ManualFeed = PP_OFF;
          else
            ManualFeed = PP_ON;
          TtaSetPrintParameter (PP_ManualFeed, ManualFeed);
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
    case PaperFormat:
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
      TtaSetPrintParameter (PP_PaperSize, PageSize);
      break;
    case PaperOrientation:
      /* orientation submenu */
      Orientation = val;
      TtaSetPrintParameter (PP_Orientation, Orientation);
      break;
    case PPagesPerSheet:
      /* pages per sheet submenu */
      switch (val)
        {
        case 0:
          PagePerSheet = 1;
          break;
        case 1:
          PagePerSheet = 2;
          break;
        case 2:
          PagePerSheet = 4;
          break;
        }
      TtaSetPrintParameter (PP_PagesPerSheet, PagePerSheet);
      break;
    case PrintSupport:
      /* paper print/save PostScript submenu */
      switch (val)
        {
        case 0:
          if (PaperPrint == PP_PS)
            {
              PaperPrint = PP_PRINTER;
              TtaSetPrintParameter (PP_Destination, PaperPrint);
            }
          break;
        case 1:
          if (PaperPrint == PP_PRINTER)
            {
              PaperPrint = PP_PS;
              TtaSetPrintParameter (PP_Destination, PaperPrint);
            }
          break;
        }
      break;
    case PPrinterName:
      if (data[0] != EOS)
        {
          if (PaperPrint == PP_PRINTER)
            /* text capture zone for the printer name */
            strncpy (PPrinter, data, MAX_PATH);
          else
            /* text capture zone for the name of the PostScript file */
            strncpy (PSfile, data, MAX_PATH);
        }
      break;
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InitPrint (void)
{
  char* ptr;

  BasePrint = TtaSetCallback ((Proc)CallbackPrint, PRINT_MAX_REF);
  DocPrint = 0;
  DocPrintURL = NULL;

  /* read default printer variable */
  ptr = TtaGetEnvString ("THOTPRINT");
  if (ptr == NULL)
    strcpy (PPrinter, "");
  else
    strcpy (PPrinter, ptr);
  TtaSetPrintCommand (PPrinter);
#ifdef _MACOS
  PaperPrint = PP_PS;
#else /* _MACOS */
  PaperPrint = PP_PRINTER;
#endif /* _MACOS */
  TtaSetPrintParameter (PP_Destination, PaperPrint);

  /* define the new default PrintSchema */
  NumberLinks = FALSE;
  WithToC = FALSE;
  IgnoreCSS = FALSE;
  PrintURL = TRUE;
  PageSize = TtaGetPrintParameter (PP_PaperSize);	  
  TtaSetPrintSchema ("");
  /* no manual feed */
  ManualFeed = PP_OFF;
  TtaSetPrintParameter (PP_ManualFeed, ManualFeed);
  PagePerSheet = 1;
  TtaSetPrintParameter (PP_PagesPerSheet, PagePerSheet);
}

/*----------------------------------------------------------------------
  SetupAndPrint sets printing parameters and starts the printing process
  ----------------------------------------------------------------------*/
void SetupAndPrint (Document doc, View view)
{
#ifdef _GTK
  char           bufMenu[MAX_LENGTH];
  int            i;
#endif /* _GTK */
  ThotBool       textFile;

  textFile = (DocumentTypes[doc] == docText || DocumentTypes[doc] == docCSS);
  /* Print form */
  CheckPrintingDocument (doc);

#ifdef _GTK
  TtaNewSheet (BasePrint + FormPrint, TtaGetViewFrame (doc, view), 
               TtaGetMessage (LIB, TMSG_LIB_PRINT), 1,
               TtaGetMessage (LIB, TMSG_BUTTON_PRINT), FALSE, 3, 'L', D_CANCEL);

  /* Paper format submenu */
  i = 0;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_A4));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_US));
  TtaNewSubmenu (BasePrint + PaperFormat, BasePrint + FormPrint, 0,
                 TtaGetMessage (LIB, TMSG_PAPER_SIZE), 2, bufMenu, NULL, 0, TRUE);
  if (PageSize == PP_US)
    TtaSetMenuForm (BasePrint + PaperFormat, 1);
  else
    TtaSetMenuForm (BasePrint + PaperFormat, 0);
  
  /* Orientation submenu */
  i = 0;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (AMAYA, AM_PORTRAIT));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (AMAYA, AM_LANDSCAPE));
  TtaNewSubmenu (BasePrint + PaperOrientation, BasePrint + FormPrint, 0,
                 TtaGetMessage (AMAYA, AM_ORIENTATION), 2, bufMenu, NULL, 0, TRUE);
  if (Orientation == PP_Landscape)
    TtaSetMenuForm (BasePrint + PaperOrientation, 1);
  else
    TtaSetMenuForm (BasePrint + PaperOrientation, 0);
  /* Pages per sheet submenu */
  i = 0;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_1_PAGE_SHEET));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_2_PAGE_SHEET));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_4_PAGE_SHEET));
  TtaNewSubmenu (BasePrint + PPagesPerSheet, BasePrint + FormPrint, 0,
                 TtaGetMessage (LIB, TMSG_REDUCTION), 3, bufMenu, NULL, 0, TRUE);
  if (PagePerSheet == 1)
    TtaSetMenuForm (BasePrint + PPagesPerSheet, 0);
  else if (PagePerSheet == 2)
    TtaSetMenuForm (BasePrint + PPagesPerSheet, 1);
  else
    TtaSetMenuForm (BasePrint + PPagesPerSheet, 2);
    
  /* Print to paper/ Print to file submenu */
  i = 0;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_PRINTER));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "B", TtaGetMessage (LIB, TMSG_PS_FILE));
  TtaNewSubmenu (BasePrint + PrintSupport, BasePrint + FormPrint, 0,
                 TtaGetMessage (LIB, TMSG_OUTPUT), 2, bufMenu, NULL, 0, TRUE);

  /* PaperPrint selector */
  TtaNewTextForm (BasePrint + PPrinterName, BasePrint + FormPrint, NULL, 30, 1, TRUE);
  if (PaperPrint == PP_PRINTER)
    {
      TtaSetMenuForm (BasePrint + PrintSupport, 0);
      TtaSetTextForm (BasePrint + PPrinterName, PPrinter);
    }
  else
    {
      TtaSetMenuForm (BasePrint + PrintSupport, 1);
      TtaSetTextForm (BasePrint + PPrinterName, PSfile);
    }

  /* The toggle */
  i = 0;
  sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (LIB, TMSG_MANUAL_FEED));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_PRINT_TOC));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_NUMBERED_LINKS));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_PRINT_URL));
  i += strlen (&bufMenu[i]) + 1;
  sprintf (&bufMenu[i], "%s%s", "T", TtaGetMessage (AMAYA, AM_WITH_CSS));
  TtaNewToggleMenu (BasePrint + PrintOptions, BasePrint + FormPrint,
                    TtaGetMessage (LIB, TMSG_OPTIONS), 5, bufMenu, NULL, FALSE);
  if (ManualFeed == PP_ON)
    TtaSetToggleMenu (BasePrint + PrintOptions, 0, TRUE);
  else
    TtaSetToggleMenu (BasePrint + PrintOptions, 0, FALSE);
  TtaSetToggleMenu (BasePrint + PrintOptions, 1, WithToC);
  TtaSetToggleMenu (BasePrint + PrintOptions, 2, NumberLinks);
  TtaSetToggleMenu (BasePrint + PrintOptions, 3, PrintURL);
  TtaSetToggleMenu (BasePrint + PrintOptions, 4, IgnoreCSS);
  
  /* activates the Print form */
  TtaShowDialogue (BasePrint+FormPrint, FALSE, TRUE);
  if (textFile)
    {
      /* invalid dialogue entries */
      TtaRedrawMenuEntry (BasePrint + PrintOptions, 1, NULL, (ThotColor)-1, FALSE);
      TtaRedrawMenuEntry (BasePrint + PrintOptions, 2, NULL, (ThotColor)-1, FALSE);
    }
#endif /* _GTK */
#ifdef _WINGUI
  CreatePrintDlgWindow (TtaGetViewFrame (doc, view), PSfile);
#endif /* _WINGUI */
#ifdef _WX
  {
    int paper_format;
    int orientation;
    int disposition;
    int paper_print;
    ThotBool manual_feed;
    ThotBool with_toc;
    ThotBool with_links;
    ThotBool with_url;
    ThotBool ignore_css;

    /* Paper format submenu */
    if (PageSize == PP_US)
      paper_format = 1;
    else
      paper_format = 0;

    /* Orientation submenu */
    if (Orientation == PP_Landscape)
      orientation = 1;
    else
      orientation = 0;

    /* Pages per sheet submenu */
    if (PagePerSheet == 1)
      disposition = 0;
    else if (PagePerSheet == 2)
      disposition = 1;
    else
      disposition = 2;

    /* PaperPrint selector */
    if (PaperPrint == PP_PRINTER)
      paper_print = 0;
    else
      paper_print = 1;
    
    /* The toggle */
    manual_feed = (ManualFeed == PP_ON);
    with_toc = WithToC;
    with_links = NumberLinks;
    with_url = PrintURL;
    ignore_css = IgnoreCSS;

    /* The dialog creation */
    ThotBool created;
    created = CreatePrintDlgWX (BasePrint + FormPrint, TtaGetViewFrame (doc, view),
                                PPrinter,
                                PSfile, 
                                paper_format,
                                orientation,
                                disposition,
                                paper_print,
                                manual_feed,
                                with_toc,
                                with_links,
                                with_url,
                                ignore_css);
    if (created)
      {
        TtaShowDialogue (BasePrint+FormPrint, FALSE, TRUE);
      }
  }
  
#endif /* _WX */
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
void UpdateURLsInSubtree (NotifyElement *event, Element el)
{
  Element             nextEl, child;
  ElementType         elType;
  SSchema             HTMLschema;

  nextEl = TtaGetFirstChild (el);
  HTMLschema = TtaGetSSchema ("HTML", event->document);
  if (HTMLschema)
    {
      // first check the id of the enclosing division
      MakeUniqueName (el, event->document, TRUE, TRUE);
      elType.ElSSchema = HTMLschema;
      while (nextEl)
        {
          CheckPastedElement (nextEl, event->document, 0, event->position, TRUE);
          /* manage included links and anchors */
          elType.ElTypeNum = HTML_EL_Anchor;
          child = TtaSearchTypedElement (elType, SearchInTree, nextEl);
          while (child)
            {
              CheckPastedElement (child, event->document, 0, event->position, TRUE);
              child = TtaSearchTypedElementInTree (elType, SearchForward, nextEl, child);
            }
	  
          /* manage included links and anchors */
          elType.ElTypeNum = HTML_EL_PICTURE_UNIT;
          child = TtaSearchTypedElement (elType, SearchInTree, nextEl);
          while (child)
            {
              CheckPastedElement (child, event->document, 0, event->position, TRUE);
              child = TtaSearchTypedElementInTree (elType, SearchForward, nextEl, child);
            }
          TtaNextSibling (&nextEl);
        }
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
static Element MoveDocumentBody (Element el, Document destDoc,
                                 Document sourceDoc, char *target,
                                 char *url, ThotBool deleteTree)
{
  Element	         root, ancestor, elem, firstInserted, div;
  Element          lastInserted, srce, copy, old, parent, sibling;
  ElementType	     elType;
  SSchema          nature;
  NotifyElement    event;
  int              i, j;
  ThotBool         checkingMode, isID;

  div = NULL;
  if (target)
    {
      /* locate the target element within the source document */
      root = SearchNAMEattribute (sourceDoc, target, NULL, NULL);
      elType = TtaGetElementType (root);
      isID = (elType.ElTypeNum != HTML_EL_Anchor &&
              elType.ElTypeNum != HTML_EL_MAP);
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

  if (root)
    {
      /* don't check the abstract tree against the structure schema */
      checkingMode = TtaGetStructureChecking (destDoc);
      TtaSetStructureChecking (FALSE, destDoc);
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
      while (ancestor);
      parent = TtaGetParent (elem);

      /* insert a DIV element */
      elType.ElTypeNum = HTML_EL_Division;
      lastInserted = TtaNewElement (destDoc, elType);
      TtaInsertSibling (lastInserted, elem, TRUE, destDoc);
      /* this delimits the new inserted part of the document */
      RegisterSubDoc (lastInserted, url);
      CreateTargetAnchor (destDoc, lastInserted, FALSE,TRUE,  FALSE);
      div = lastInserted;

      // check if new natures are added
      if (DocumentMeta[destDoc] && !DocumentMeta[destDoc]->compound)
        {
          nature = NULL;
          TtaNextNature (sourceDoc, &nature);
          if (nature)
            DocumentMeta[destDoc]->compound = TRUE;
        }

      /* do copy */
      firstInserted = NULL;
      if (isID)
        srce = root;
      else
        srce = TtaGetFirstChild (root);
      while (srce)
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
              event.event = TteElemPaste;
              event.document = destDoc;
              event.element = copy;
              event.elementType = TtaGetElementType (copy);
              event.position = sourceDoc;
              event.info = 0;
              UpdateURLsInSubtree(&event, copy);
            }
          /* get the next element in the source document */
          old = srce;
          TtaNextSibling (&srce);
          if (deleteTree)
            {
              // remove the current selection if it concerns the removed subtree
              TtaGiveFirstSelectedElement (sourceDoc, &elem, &i, &j);
              if (elem)
                {
                  if (TtaIsAncestor (elem, old))
                    TtaCancelSelection (sourceDoc);
                  else
                    {
                      TtaGiveLastSelectedElement (sourceDoc, &elem, &i, &j);
                      if (elem &&TtaIsAncestor (elem, old))
                        TtaCancelSelection (sourceDoc);
                    }
                }
              TtaDeleteTree (old, sourceDoc);
            }
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
      TtaSetStructureChecking (checkingMode, destDoc);
    }
  /* return the address of the new division */
  return (div);
}

/*----------------------------------------------------------------------
  CloseMakeBook
  ----------------------------------------------------------------------*/
static void CloseMakeBook (Document document)
{
  ResetStop (document);
  /* update internal links */
  SetInternalLinks (document);
  /* if the document changed force the browser mode */
  if (SubDocs)
    /* send a warning to the user to avoid to save this document */
    /* remove registered  sub-documents */
    FreeSubDocTable ();
  DocBook = 0;
  TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_DOCUMENT_LOADED), NULL);
}

/*----------------------------------------------------------------------
  GetIncludedDocuments_callback finishes the GetIncludedDocuments procedure
  ----------------------------------------------------------------------*/
void   GetIncludedDocuments_callback (int newdoc, int status, 
                                      char *urlName,
                                      char *outputfile,
				      char *proxyName, 
                                      AHTHeaders *http_headers,
                                      void * context)
{
  Element		link, div;
  IncludeCtxt          *ctx, *prev;
  char		       *utf8path, *ptr;
  ThotBool              found = FALSE;

  /* restore GetIncludedDocuments's context */
  ctx = (IncludeCtxt *) context;  
  if (!ctx)
    return;

  div = NULL;
  link = ctx->link;
  ptr = ctx->name;
  utf8path = ctx->utf8path;
  if (utf8path)
    {
      if (newdoc && newdoc != DocBook)
        {
          /* it's not the DocBook itself */
          /* copy the target document at the position of the link */
          TtaSetDocumentModified (DocBook);
          div = MoveDocumentBody (link, DocBook, newdoc, ptr, utf8path,
                                  (ThotBool)(newdoc == IncludedDocument));
        }
      /* global variables */
      FreeDocumentResource (IncludedDocument);
      TtaCloseDocument (IncludedDocument);
      IncludedDocument = 0;
    }

  if (div)
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
      TtaFreeMemory (utf8path);
      utf8path = NULL;
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
static ThotBool GetIncludedDocuments (Element el, Element link,
                                      Document doc, IncludeCtxt *prev)
{
  ElementType           elType;
  Attribute		attr;
  AttributeType	        attrType;
  Document		newdoc;
  IncludeCtxt          *ctx = NULL;
  char		       *utf8path, *ptr;
  int			length;
  ThotBool              found = FALSE;

  /* look for anchors with the attribute rel within the element  el */
  attr = NULL;
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
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
          utf8path = (char *)TtaGetMemory (length + 1);
          TtaGiveTextAttributeValue (attr, utf8path, &length);
          /* Valid rel values are rel="chapter" or rel="subdocument" */
          if (strcasecmp (utf8path, "chapter") &&
              strcasecmp (utf8path, "subdocument"))
            attr = NULL;
          TtaFreeMemory (utf8path);
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
          utf8path = (char *)TtaGetMemory (length + 1);
          TtaGiveTextAttributeValue (attr, utf8path, &length);
          /* remove trailing spaces */
	  while (length > 0 &&
		 (utf8path[length - 1] == SPACE || utf8path[length - 1] == TAB))
	    {
	      utf8path[length-1] = EOS;
	      length--;
	    }
          ptr = strrchr (utf8path, '#');
          if (ptr)
            {
              /* link to a particular position within a document */
              if (ptr == utf8path)
                {
                  /* local link */
                  TtaFreeMemory (utf8path);
                  utf8path = NULL;
                }
              else
                {
                  ptr[0] = EOS;
                  ptr = &ptr[1];
                }
            }
		  
          if (utf8path)
            /* this link designates an external document */
            {
              /* create a new document and loads the target document */
              IncludedDocument = TtaNewDocument ("HTML", "tmp");
              if (IncludedDocument != 0)
                {
                  TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_FETCHING), utf8path);
                  ctx = (IncludeCtxt *)TtaGetMemory (sizeof (IncludeCtxt));
                  ctx->div =  el;
                  ctx->link = link;
                  ctx->utf8path = utf8path; /* the URL of the document */
                  ctx->name = ptr;
                  ctx->ctxt = prev; /* previous context */
                  /* Get the reference of the calling document */
                  SetStopButton (doc);
                  newdoc = GetAmayaDoc (utf8path, NULL, IncludedDocument,
                                        doc, CE_MAKEBOOK, FALSE, 
                                        (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*)) GetIncludedDocuments_callback,
                                        (void *) ctx);
                  found = TRUE;
                }
            }
          else
            TtaFreeMemory (utf8path);
        }
    }
  return (found);
}

/*----------------------------------------------------------------------
  MakeBook
  Replace all links in a document which have an attribute REL="chapter"
  or REL="subdocument" by the corresponding target document.
  ----------------------------------------------------------------------*/
void MakeBook (Document doc, View view)
{
  Element	    root, body;
  ElementType	    elType;

  if (SubDocs)
    // another make_book is wi=orking
    return;
  /* stops all current transfers on this document */
  StopTransfer (doc, 1);
  /* simulate a transfert in the main document */
  DocBook = doc;
  IncludedDocument = 0;
  root = TtaGetMainRoot (doc);
  elType = TtaGetElementType (root);
  elType.ElTypeNum = HTML_EL_BODY;
  body = TtaSearchTypedElement (elType, SearchForward, root);
  if (body)
    GetIncludedDocuments (body, body, doc, NULL);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsUTF_8 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, UTF_8);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsISO_8859_1 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, ISO_8859_1);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsISO_8859_2 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, ISO_8859_2);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsISO_8859_3 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, ISO_8859_3);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsISO_8859_4 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, ISO_8859_4);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsISO_8859_5 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, ISO_8859_5);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsISO_8859_6 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, ISO_8859_6);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsISO_8859_7 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, ISO_8859_7);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsISO_8859_8 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, ISO_8859_8);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsISO_8859_9 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, ISO_8859_9);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsISO_8859_15 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, ISO_8859_15);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsKOI8_R (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, KOI8_R);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsWINDOWS_1250 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, WINDOWS_1250);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsWINDOWS_1251 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, WINDOWS_1251);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsWINDOWS_1252 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, WINDOWS_1252);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsWINDOWS_1253 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, WINDOWS_1253);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsWINDOWS_1254 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, WINDOWS_1254);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsWINDOWS_1255 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, WINDOWS_1255);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsWINDOWS_1256 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, WINDOWS_1256);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsWINDOWS_1257 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, WINDOWS_1257);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsGB_2312 (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, GB_2312);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsISO_2022_JP (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, ISO_2022_JP);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsEUC_JP (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, EUC_JP);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void ReadAsSHIFT_JIS (Document doc, View view)
{
  ReparseAs (doc, view, FALSE, SHIFT_JIS);
}

/*----------------------------------------------------------------------
  SectionNumbering generates numbers for all HTML H* elements after
  the current position.
  ----------------------------------------------------------------------*/
void SectionNumbering (Document doc, View view)
{
  Element             root, el, new_, child;
  DisplayMode         dispMode;
  SSchema             HTMLschema;
  ElementType         elType, childType, searchedType1, searchedType2;
  ElementType         searchedType3, searchedType4, searchedType5;
  Language            lang;
  char                s[MAX_LENGTH], n[20], *text;
  int                 nH1, nH2, nH3, nH4, nH5, nH6, length, i;
  ThotBool            closeUndo, manyH1, change = FALSE;

  /* check if there is any HTML element within the document */
  HTMLschema = TtaGetSSchema ("HTML", doc);
  if (HTMLschema == NULL)
    /* no HTML element */
    return;

  dispMode = TtaGetDisplayMode (doc);
  if (TtaHasUndoSequence (doc))
    closeUndo = FALSE;
  else
    {
      closeUndo = TRUE;
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
    }

  /* check if there are more than one HTML H1 element */
  manyH1 = FALSE;
  root = TtaGetMainRoot (doc);
  elType.ElSSchema = HTMLschema;
  elType.ElTypeNum = HTML_EL_H1;
  el = TtaSearchTypedElement (elType, SearchForward, root);
  if (el)
    {
      if (TtaSearchTypedElement (elType, SearchForward, el))
	/* there are at least 2 H1 elements in the document */
	manyH1 = TRUE;
      else
	/* there is 1 and only 1 H1 in the document */
	{
	  /* remove its section number if it has one */
          /* look for the first leaf child */
          child = el;
	  do
	    {
	      child = TtaGetFirstChild (child);
	      if (child)
		childType = TtaGetElementType (child);
	    }
	  while (child && !TtaIsLeaf (childType) &&
		 childType.ElSSchema == HTMLschema);
          if (child && childType.ElSSchema == HTMLschema &&
              childType.ElTypeNum == HTML_EL_TEXT_UNIT)
	    /* the first leaf is a text unit */
            {
              /* check the text contents */
              length = TtaGetTextLength (child) + 1;
              text = (char *)TtaGetMemory (length);
              TtaGiveTextContent (child, (unsigned char *)text, &length, &lang);
              /* remove the old number */
              i = 0;
              while (isdigit (text[i]) || text[i] == '.')
                i++;
              // remove extra spaces
              while (text[i] == SPACE)
                i++;
              TtaRegisterElementReplace (child, doc);
              TtaSetTextContent (child, (unsigned char *)&text[i],
				 Latin_Script, doc);
              TtaFreeMemory (text);
              change = TRUE;
            }
	}
    }

  searchedType1.ElSSchema = HTMLschema;
  searchedType2.ElSSchema = HTMLschema;
  searchedType3.ElSSchema = HTMLschema;
  searchedType4.ElSSchema = HTMLschema;
  searchedType5.ElSSchema = HTMLschema;
  if (manyH1)
    {
      searchedType1.ElTypeNum = HTML_EL_H1;
      searchedType2.ElTypeNum = HTML_EL_H2;
      searchedType3.ElTypeNum = HTML_EL_H3;
      searchedType4.ElTypeNum = HTML_EL_H4;
      searchedType5.ElTypeNum = HTML_EL_H5;
    }
  else
    {
      searchedType1.ElTypeNum = HTML_EL_H2;
      searchedType2.ElTypeNum = HTML_EL_H3;
      searchedType3.ElTypeNum = HTML_EL_H4;
      searchedType4.ElTypeNum = HTML_EL_H5;
      searchedType5.ElTypeNum = HTML_EL_H6;
    }
  nH1 = nH2 = nH3 = nH4 = nH5 = nH6 = 0;
  el = root;
  while (el)
    {
      el = TtaSearchElementAmong5Types (searchedType1, searchedType2,
                                        searchedType3, searchedType4,
                                        searchedType5, SearchForward, el);
      if (el)
        {
          /* generate the text number */
          elType = TtaGetElementType (el);
          s[0] = EOS;
          switch (elType.ElTypeNum)
            {
            case HTML_EL_H1:
	      nH1++;
	      nH2 = nH3 = nH4 = nH5 = nH6 = 0;
              sprintf (s, "%d.", nH1);
	      break;
            case HTML_EL_H2:
              nH2++;
              nH3 = nH4 = nH5 = nH6 = 0;
	      if (manyH1)
		{
		  if (nH1)
		    sprintf (s, "%d.", nH1);
		  sprintf (n, "%d.", nH2);
		  strcat (s, n);
		}
	      else
		sprintf (s, "%d.", nH2);
              break;
            case HTML_EL_H3:
              nH3++;
              nH4 = nH5 = nH6 = 0;
	      if (manyH1)
		{
		  if (nH1)
		    sprintf (s, "%d.", nH1);
		  if (nH2)
		    {
		      sprintf (n, "%d.", nH2);
		      strcat (s, n);
		    }
		}
	      else
		{
		  if (nH2)
		    sprintf (s, "%d.", nH2);
		}
	      sprintf (n, "%d.", nH3);
	      strcat (s, n);
              break;
            case HTML_EL_H4:
              nH4++;
              nH5 = nH6 = 0;
	      if (manyH1)
		{
		  if (nH1)
		    sprintf (s, "%d.", nH1);
		  if (nH2)
		    {
		      sprintf (n, "%d.", nH2);
		      strcat (s, n);
		    }
		}
	      else
		{
		  if (nH2)
		    sprintf (s, "%d.", nH2);
		}
	      if (nH3)
		{
		  sprintf (n, "%d.", nH3);
		  strcat (s, n);
		}
	      sprintf (n, "%d.", nH4);
	      strcat (s, n);
              break;
            case HTML_EL_H5:
              nH5++;
              nH6 = 0;
	      if (manyH1)
		{
		  if (nH1)
		    sprintf (s, "%d.", nH1);
		  if (nH2)
		    {
		      sprintf (n, "%d.", nH2);
		      strcat (s, n);
		    }
                }
	      else
		{
		  if (nH2)
		    sprintf (s, "%d.", nH2);
		}
	      if (nH3)
		{
		  sprintf (n, "%d.", nH3);
		  strcat (s, n);
		}
	      if (nH4)
		{
		  sprintf (n, "%d.", nH4);
		  strcat (s, n);
		}
	      sprintf (n, "%d.", nH5);
	      strcat (s, n);
              break;
            case HTML_EL_H6:
              nH6++;
              if (nH2)
                sprintf (s, "%d.", nH2);
              if (nH3)
                {
                  sprintf (n, "%d.", nH3);
                  strcat (s, n);
                }
              if (nH4)
                {
                  sprintf (n, "%d.", nH4);
                  strcat (s, n);
                }
              if (nH5)
                {
                  sprintf (n, "%d.", nH5);
                  strcat (s, n);
                }
              sprintf (n, "%d.", nH6);
              strcat (s, n);
              break;
            default: break;
            }
          strcat (s, " ");

          /* look for the first leaf child */
          child = el;
          if (child)
            {
              do
                {
                  child = TtaGetFirstChild (child);
                  childType = TtaGetElementType (child);
                }
              while (!TtaIsLeaf (childType) &&
		     childType.ElSSchema == HTMLschema);
            }
          if (child && childType.ElSSchema == HTMLschema &&
              childType.ElTypeNum == HTML_EL_TEXT_UNIT)
            {
              /* the first leaf child is a text unit */
              new_ = NULL;
              /* check the text contents */
              length = TtaGetTextLength (child) + 1;
              text = (char *)TtaGetMemory (length);
              TtaGiveTextContent (child, (unsigned char *)text, &length, &lang);
              /* remove the old number */
              i = 0;
              while (isdigit (text[i]) || text[i] == '.')
                i++;
              // remove extra spaces
              while (text[i] == SPACE)
                i++;
              TtaRegisterElementReplace (child, doc);
              TtaSetTextContent (child, (unsigned char *)s, Latin_Script, doc);
              TtaAppendTextContent (child, (unsigned char *)&text[i], doc);
              TtaFreeMemory (text);
              change = TRUE;
            }
          else
            {
              /* add a new text element */
              elType.ElTypeNum = HTML_EL_TEXT_UNIT;
              new_ = TtaNewElement (doc, elType);
              if (child)
                /* insert before */
                TtaInsertSibling (new_, child, TRUE, doc);
              else
                TtaInsertFirstChild (&new_, el, doc);
              TtaSetTextContent (new_, (unsigned char *)s, Latin_Script, doc);
              TtaRegisterElementCreate (new_, doc);
              change = TRUE;
            }
        }
    }

  if (closeUndo)
    TtaCloseUndoSequence (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
  if (change)
    TtaSetDocumentModified (doc);
}

/*----------------------------------------------------------------------
  MakeToC generates a Table of Contents at the current position.
  Looks for all HTML H* elements after the current position.
  ----------------------------------------------------------------------*/
void MakeToc (Document doc, View view)
{
  Element             el, new_, *list, parent, copy, srce, child, prev, ancest;
  Element             root, toc, lH1, lH2, lH3, lH4, lH5, lH6, item;
  ElementType         elType, searchedType1, searchedType2;
  ElementType         searchedType3, searchedType4, searchedType5;
  ElementType         ulType, copyType;
  AttributeType       attrType;
  Attribute           attr;
  DisplayMode         dispMode;
  char               *s, *id, *value;
  int                 firstChar, i;
  ThotBool            closeUndo, found, manyH1, extendUndo;

  dispMode = TtaGetDisplayMode (doc);

  /* get the insert point */
  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &i);
  if (el == NULL || TtaIsReadOnly (el))
    {
      /* no selection */
      TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_EL_RO);
      return;
    }

  elType = TtaGetElementType (el);
  s = TtaGetSSchemaName (elType.ElSSchema);
  if (strcmp (s, "HTML"))
    /* not within HTML element */
    {
      /* skip ancestors that are Template elements */
      ancest = el;
      while (ancest && !strcmp (s, "Template"))
        {
          ancest = TtaGetParent (ancest);
          if (ancest)
            {
              elType = TtaGetElementType (ancest);
              s = TtaGetSSchemaName (elType.ElSSchema);
            }
        }
      if (strcmp (s, "HTML"))
        return;
    }
  if (!TtaIsSelectionEmpty())
    {
      TtaInsertAnyElement (doc, TRUE);
      extendUndo = TRUE;
    }
  else
     extendUndo = FALSE;
  if (!HTMLelementAllowed (doc))
    /* the creation of an HTML element is not allowed here */
    return;

  root = TtaGetMainRoot (doc);
  elType.ElTypeNum = HTML_EL_H1;
  manyH1 = FALSE;
  lH1 = TtaSearchTypedElement (elType, SearchForward, root);
  if (lH1)
    {
      if (TtaSearchTypedElement (elType, SearchForward, lH1))
        /* there are at least 2 H1 elements in the document */
        manyH1 = TRUE;
    }

  attrType.AttrSSchema = elType.ElSSchema;
  ulType.ElSSchema = elType.ElSSchema;
  ulType.ElTypeNum = HTML_EL_Unnumbered_List;
  toc = lH1 = lH2 = lH3 = lH4 = lH5 = lH6 = prev = NULL;
  list = NULL;
  /* check if the insert point is already within a table of contents */
  ancest = el;
  found = FALSE;
  while (ancest &&
         (elType.ElSSchema != ulType.ElSSchema ||
          elType.ElTypeNum != HTML_EL_Division))
    {
      if (!found && !strcmp (s, "Template"))
        // cross an enclosing XTiger element
        found = TRUE;
      ancest = TtaGetParent (ancest);
      if (ancest)
        {
          elType = TtaGetElementType (ancest);
          s = TtaGetSSchemaName (elType.ElSSchema);
        }
    }
  if (ancest)
    {
      // check if that division has the toc class
      attrType.AttrTypeNum = HTML_ATTR_Class;
      attr = TtaGetAttribute (ancest, attrType);
      if (attr)
        {
          i = TtaGetTextAttributeLength (attr);
          if (i == 3)
            {
              value = (char *)TtaGetMemory (i + 1);
              TtaGiveTextAttributeValue(attr, value, &i);
              if (strcmp (value, "toc"))
                attr = NULL;
              TtaFreeMemory (value);
              value = NULL;
            }
          else
            attr = NULL;
        }
      if (attr)
        {
          if (found)
            {
              TtaDisplaySimpleMessage (CONFIRM, LIB, TMSG_EL_RO);
              return;
            }
          else
            toc = ancest;
        }
    }

  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, SuspendDisplay);

  if (TtaHasUndoSequence (doc))
    closeUndo = FALSE;
  else
    {
      closeUndo = TRUE;
      if (extendUndo)
        TtaExtendUndoSequence (doc);
      else
        TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
    }

  if (toc)
    {
      // replace the old toc by the new one
      child = TtaGetFirstChild (toc);
      TtaRegisterElementDelete (child, doc);
      TtaRemoveTree (child, doc);
      TtaSetDocumentModified (doc);
    }

  searchedType1.ElSSchema = elType.ElSSchema;
  searchedType2.ElSSchema = elType.ElSSchema;
  searchedType3.ElSSchema = elType.ElSSchema;
  searchedType4.ElSSchema = elType.ElSSchema;
  searchedType5.ElSSchema = elType.ElSSchema;
  if (manyH1)
    {
      searchedType1.ElTypeNum = HTML_EL_H1;
      searchedType2.ElTypeNum = HTML_EL_H2;
      searchedType3.ElTypeNum = HTML_EL_H3;
      searchedType4.ElTypeNum = HTML_EL_H4;
      searchedType5.ElTypeNum = HTML_EL_H5;
    }
  else
    {
      searchedType1.ElTypeNum = HTML_EL_H2;
      searchedType2.ElTypeNum = HTML_EL_H3;
      searchedType3.ElTypeNum = HTML_EL_H4;
      searchedType4.ElTypeNum = HTML_EL_H5;
      searchedType5.ElTypeNum = HTML_EL_H6;
    }

  // keep in memory the current selected element
  ancest = el;
  el = root;
  while (el)
    {
      el = TtaSearchElementAmong5Types (searchedType1, searchedType2,
                                        searchedType3, searchedType4,
                                        searchedType5, SearchForward, el);
      if (el)
        {
          if (toc == NULL)
            {
              /* generate the enclosing division */
              elType.ElTypeNum = HTML_EL_Division;
              if (TtaInsertElement (elType, doc))
                {
                  TtaGiveFirstSelectedElement (doc, &child, &firstChar, &i);
                  /* the div and its initial content is now created */
                  toc = TtaGetTypedAncestor (child, elType);
                  TtaRegisterElementDelete (child, doc);
                  TtaRemoveTree (child, doc);
                }
              else
                {
                  // the division is not created
                  if (closeUndo)
                    TtaCloseUndoSequence (doc);
                  if (dispMode == DisplayImmediately)
                    TtaSetDisplayMode (doc, dispMode);
                  TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NOT_ALLOWED);
                  return;
                }

              if (toc)
                {
                  /* it's the last created element */
                  attrType.AttrTypeNum = HTML_ATTR_Class;
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (toc, attr, doc);
                  TtaSetAttributeText (attr, "toc", toc, doc);
                  TtaRegisterAttributeCreate (attr, toc, doc);
                }
            }

          if (toc == NULL)
            el = NULL;
          else
            {
              /* does the element have an ID attribute already? */
              attrType.AttrTypeNum = HTML_ATTR_ID;
              attr = TtaGetAttribute (el, attrType);
              if (!attr)
                {
                  /* generate the ID if it does't exist */
                  CreateTargetAnchor (doc, el, TRUE, TRUE, TRUE);
                  attr = TtaGetAttribute (el, attrType);
                }
              i = TtaGetTextAttributeLength (attr) + 1;
              id = (char *)TtaGetMemory (i + 1);
              id[0] = '#';
              TtaGiveTextAttributeValue (attr, &id[1], &i);
	      
              /* locate or generate the list */
              elType = TtaGetElementType (el);

              if (elType.ElTypeNum == HTML_EL_H1)
                {
                  parent = toc;
                  lH2 = lH3 = lH4 = lH5 = NULL;
                  list = &lH1;
                }
	      else if (elType.ElTypeNum == HTML_EL_H2)
                {
		  if (manyH1)
		    {
		      if (lH1)
			parent = TtaGetLastChild (lH1);
		      else
			parent = toc;
		      lH3 = lH4 = lH5 = NULL;
		      list = &lH2;
		    }
		  else
		    {
		      parent = toc;
		      lH3 = lH4 = lH5 = lH6 = NULL;
		      list = &lH2;
		    }
                }
              else if (elType.ElTypeNum == HTML_EL_H3)
                {
		  if (manyH1)
		    {
		      if (lH2)
			parent =  TtaGetLastChild (lH2);
		      else if (lH3)
			parent =  TtaGetLastChild (lH3);
		      else
			parent = toc;
		      lH4 = lH5 = NULL;
		      list = &lH3;
		    }
		  else
		    {
		      if (lH2)
			parent = TtaGetLastChild (lH2);
		      else
			parent = toc;
		      lH4 = lH5 = lH6 = NULL;
		      list = &lH3;
		    }
                }
              else if (elType.ElTypeNum == HTML_EL_H4)
                {
		  if (manyH1)
		    {
		      if (lH3)
			parent =  TtaGetLastChild (lH3);
		      else if (lH2)
			parent =  TtaGetLastChild (lH2);
		      else if (lH1)
			parent =  TtaGetLastChild (lH1);
		      else
			parent = toc;
		      lH5 = NULL;
		      list = &lH4;
		    }
		  else
		    {
		      if (lH3)
			parent =  TtaGetLastChild (lH3);
		      else if (lH2)
			parent =  TtaGetLastChild (lH2);
		      else
			parent = toc;
		      lH5 = lH6 = NULL;
		      list = &lH4;
		    }
                }
              else if (elType.ElTypeNum == HTML_EL_H5)
                {
		  if (manyH1)
		    {
		      if (lH4)
			parent =  TtaGetLastChild (lH4);
		      else if (lH3)
			parent =  TtaGetLastChild (lH3);
		      else if (lH2)
			parent =  TtaGetLastChild (lH2);
		      else if (lH1)
			parent =  TtaGetLastChild (lH1);
		      else
			parent = toc;
		      list = &lH5;
		    }
		  else
		    {
		      if (lH4)
			parent =  TtaGetLastChild (lH4);
		      else if (lH3)
			parent =  TtaGetLastChild (lH3);
		      else if (lH2)
			parent =  TtaGetLastChild (lH2);
		      else
			parent = toc;
		      lH6 = NULL;
		      list = &lH5;
		    }
                }
              else if (elType.ElTypeNum == HTML_EL_H6)
                {
		  if (lH5)
		    parent =  TtaGetLastChild (lH5);
		  else if (lH4)
		    parent =  TtaGetLastChild (lH4);
		  else if (lH3)
		    parent =  TtaGetLastChild (lH3);
		  else if (lH2)
		    parent =  TtaGetLastChild (lH2);
		  else
		    parent = toc;
		  list = &lH6;
                }

              if (*list == NULL)
                {
                  /* generate the list */
                  *list = TtaNewElement (doc, ulType);
                  child = TtaGetLastChild (parent);
                  if (child)
                    TtaInsertSibling (*list, child, FALSE, doc);
                  else
                    TtaInsertFirstChild (list, parent, doc);
                  TtaRegisterElementCreate (*list, doc);
                }
              /* generate the list item */
              elType.ElTypeNum = HTML_EL_List_Item;
              item = TtaNewElement (doc, elType);
              /* generate the HTML_EL_Pseudo_paragraph */
              elType.ElTypeNum =  HTML_EL_Pseudo_paragraph;
              parent = TtaNewElement (doc, elType);
              TtaInsertFirstChild (&parent, item, doc);
              /* generate the link anchor */
              elType.ElTypeNum = HTML_EL_Anchor;
              new_ = TtaNewElement (doc, elType);
              TtaInsertFirstChild (&new_, parent, doc);
              attrType.AttrTypeNum = HTML_ATTR_HREF_;
              attr = TtaNewAttribute (attrType);
              TtaAttachAttribute (new_, attr, doc);
              TtaSetAttributeText (attr, id, new_, doc);
              TtaFreeMemory (id);
              id = NULL;
              /* get a copy of the Hi contents */
              srce = TtaGetFirstChild (el);
              prev = NULL;
              parent = NULL;
              while (srce)
                {
                  copyType = TtaGetElementType (srce);
                  s = TtaGetSSchemaName (copyType.ElSSchema);
                  while (srce && !TtaIsLeaf (copyType) &&
                         !strcmp (s, "Template"))
                    {
                      /* copy the anchor contents instead of the anchor */
                      if (parent == NULL)
                        parent = srce;
                      srce = TtaGetFirstChild (srce);
                      if (srce)
                        {
                          copyType = TtaGetElementType (srce);
                          s = TtaGetSSchemaName (copyType.ElSSchema);
                        }
                    }
                  if (srce &&
                      copyType.ElTypeNum == HTML_EL_Anchor &&
                      copyType.ElSSchema == elType.ElSSchema)
                    {
                      /* copy the anchor contents instead of the anchor */
                      if (parent == NULL)
                        parent = srce;
                      srce = TtaGetFirstChild (srce);
                    }
                  if (srce)
                    {
                      /* copy children of the next source */
                      copy = TtaCopyTree (srce, doc, doc, new_);
                      if (copy)
                        {
                          if (prev == NULL)
                            /* this is the first copied element. Insert it before elem */
                            TtaInsertFirstChild (&copy, new_, doc);
                          else
                            /* insert the new copied element after the element previously
                               copied */
                            TtaInsertSibling (copy, prev, FALSE, doc);
                          prev = copy;
                        }
                      TtaNextSibling (&srce);
                    }
                  if (srce == NULL && parent)
                    {
                      /* copy children of an anchor */
                      srce = parent;
                      parent = NULL;
                      if (srce)
                        TtaNextSibling (&srce);
                    }
                }
              child = TtaGetLastChild (*list);
              if (child)
                TtaInsertSibling (item, child, FALSE, doc);
              else
                TtaInsertFirstChild (&item, *list, doc);
              TtaRegisterElementCreate (item, doc);
            }
        }
    }

  if (closeUndo)
    TtaCloseUndoSequence (doc);

  if (toc == NULL)
    {
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, dispMode);
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_HEADING_FOUND);
    }
  else
    {
      /* force a complete redisplay to apply CSS */
      TtaSetDisplayMode (doc, NoComputedDisplay);
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, dispMode);
      /* select the end of the toc */
      if (prev)
        {
          child = prev;
          while (child)
            {
              child = TtaGetLastChild (prev);
              if (child)
                prev = child;
            }
          elType = TtaGetElementType (prev);
          if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
            {
              i = TtaGetElementVolume (prev);
              TtaSelectString (doc, prev, i+1, i);
            }
          else
            TtaSelectElement (doc, prev);
        }
    }
}
