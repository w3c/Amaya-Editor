/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya saving functions.
 *
 * Authors: D. Veillard, I. Vatton
 *
 */

/* DEBUG_AMAYA_SAVE Print out debug informations when saving */

#ifdef DEBUG_AMAYA_SAVE
#define DBG(a) a
#else
#define DBG(a)
#endif

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"

#include "HTMLbook_f.h"

static char         tempSavedObject[MAX_LENGTH];

/*
 * this structure describes the set of attributes and
 * the corresponding elements containing URLs.
 * This table is Up-to-date as of HTML 3.2
 * It is used when fetching pages from one server to
 * convert relative URLs to absolute ones.
 * Pairs using the same attribute should be groupped
 * to minimize scans along the document.
 */

typedef struct struct_URL_elem
  {
     int                 attr_type;
     int                 elem_type;
  }
URL_elem;

URL_elem            URL_elem_tab[] =
{
   {HTML_ATTR_HREF_, HTML_EL_Anchor},
   {HTML_ATTR_HREF_, HTML_EL_AREA},
   {HTML_ATTR_HREF_, HTML_EL_LINK},
   {HTML_ATTR_HREF_, HTML_EL_BASE},
/*** The conversion of SRC on images are handled separately
{  HTML_ATTR_SRC, HTML_EL_PICTURE_UNIT },
 ***/
   {HTML_ATTR_SRC, HTML_EL_Input},
   {HTML_ATTR_USEMAP, HTML_EL_PICTURE_UNIT},
   {HTML_ATTR_codebase, HTML_EL_Applet},
   {HTML_ATTR_Script_URL, HTML_EL_Form},
   {HTML_ATTR_background_, HTML_EL_BODY},
};

#define NB_URL_PAIR ((sizeof(URL_elem_tab)) / (sizeof(URL_elem)))

#include "init_f.h"
#ifndef AMAYA_JAVA
#include "query_f.h"
#endif
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"


/*----------------------------------------------------------------------
   SetAbsoluteURLs
   change relative URLs to absolute ones within an HTML document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetAbsoluteURLs (Document document, View view)
#else
void                SetAbsoluteURLs (document, view)
Document            document;
View                view;

#endif
{
   int                 index, i;	/* in URL_elem_tab */
   int                 attr;	/* value of attr_type */
   Element             el;
   ElementType         elType;
   Attribute           at;
   AttributeType       atType;
   char                old_url[MAX_LENGTH + 1];
   char                new_url[MAX_LENGTH + 50];
   char                name[MAX_LENGTH];
   int                 len;

DBG(fprintf(stderr, "SetAbsoluteURLs\n");)

   for (index = 0; index < NB_URL_PAIR;)
     {
	/* fetch a new attr */
	attr = URL_elem_tab[index].attr_type;

	/*
	 * search all elements having this attribute
	 */
	el = TtaGetMainRoot (SavingDocument);
	atType.AttrSSchema = TtaGetDocumentSSchema (SavingDocument);
	atType.AttrTypeNum = attr;
	TtaSearchAttribute (atType, SearchForward, el, &el, &at);
	while (el != NULL)
	  {

	     /*
	      * search for all consecutives pair of (attr,elem)
	      * if the current elem match.
	      */
	     elType = TtaGetElementType (el);
	     for (i = index;
		  (i < NB_URL_PAIR) && (URL_elem_tab[i].attr_type == attr);
		  i++)
	       {

		  if (elType.ElTypeNum == URL_elem_tab[i].elem_type)
		    {

		       /*
		        * get the URL contained in the attribute.
		        */
		       len = MAX_LENGTH;
		       TtaGiveTextAttributeValue (at, old_url, &len);
		       old_url[MAX_LENGTH] = EOS;

		       /*
		        * save the new SRC attr
		        */
		       NormalizeURL (old_url, document, new_url, name);
DBG(                   fprintf(stderr, "Changed SRC from %s to %s\n", old_url, new_url);)
		       TtaSetAttributeText (at, new_url, el, document);
		       break;
		    }
	       }
	     TtaSearchAttribute (atType, SearchForward, el, &el, &at);
	  }

	/*
	 * get next index in the table corresponding
	 * to a different attribute.
	 */
	for (i = index + 1;
	     (i < NB_URL_PAIR) && (URL_elem_tab[i].attr_type == attr);
	     i++) ;
	index = i;
     }
}

/*----------------------------------------------------------------------
  InitSaveForm
  Draw the Save As Dialog and prepare for input.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         InitSaveForm (Document document, View view, char *pathname)
#else
static void         InitSaveForm (document, view, pathname)
Document            document;
View                view;
char               *pathname;

#endif
{
   char             buffer[3000];
   char             s[MAX_LENGTH];
   int              i;

   /* Dialogue form for saving in local */
   i = 0;
   strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));
   TtaNewSheet (BaseDialog + SaveForm, TtaGetViewFrame (document, view), 
	       TtaGetMessage (AMAYA, AM_SAVE_AS), 3, s, TRUE, 3, 'L', D_CANCEL);
   sprintf (buffer, "%s%c%s%c%s%c%s%c%s", "BHTML", EOS, "BText", EOS, "S", EOS,
	    TtaGetMessage (AMAYA, AM_BCOPY_IMAGES), EOS,
	    TtaGetMessage (AMAYA, AM_BTRANSFORM_URL));
   TtaNewToggleMenu (BaseDialog + ToggleSave, BaseDialog + SaveForm,
		     "Output format", 5, buffer, NULL, TRUE);
   SaveAsHTML = TRUE;
   SaveAsText = FALSE;
   TtaSetToggleMenu (BaseDialog + ToggleSave, 0, SaveAsHTML);
   TtaSetToggleMenu (BaseDialog + ToggleSave, 1, SaveAsText);
   TtaSetToggleMenu (BaseDialog + ToggleSave, 3, CopyImages);
   TtaSetToggleMenu (BaseDialog + ToggleSave, 4, UpdateURLs);
   TtaListDirectory (DirectoryName, BaseDialog + SaveForm,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),
		     BaseDialog + DirSave, ScanFilter,
		     TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSave);
   TtaNewTextForm (BaseDialog + NameSave, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_DOC_LOCATION), 50, 1, TRUE);
   TtaSetTextForm (BaseDialog + NameSave, pathname);
   TtaNewLabel (BaseDialog + Label1, BaseDialog + SaveForm, "");
   TtaNewLabel (BaseDialog + Label2, BaseDialog + SaveForm, "");
   TtaNewTextForm (BaseDialog + ImgDirSave, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_IMAGES_LOCATION), 50, 1, TRUE);
   TtaSetTextForm (BaseDialog + ImgDirSave, SaveImgsURL);
   TtaNewLabel (BaseDialog + Label3, BaseDialog + SaveForm, "");
   TtaNewLabel (BaseDialog + Label4, BaseDialog + SaveForm, "");
   TtaNewTextForm (BaseDialog + FilterText, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_PARSE), 10, 1, TRUE);
   TtaSetTextForm (BaseDialog + FilterText, ScanFilter);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + SaveForm, FALSE);
}

/*----------------------------------------------------------------------
   SaveHTMLDocumentAs                                              
   Entry point called when the user selects the SaveAs function
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SaveDocumentAs (Document document, View view)
#else
void                SaveDocumentAs (document, view)
Document            document;
View                view;

#endif
{
   char                tempname[MAX_LENGTH];
   int                 i;

   if ((SavingDocument != (Document) None) && (SavingDocument != document))
      return;
   if (SavingObject != (Document) None)
      return;

   /* memorize the current document */
   SavingDocument = document;
   strcpy (tempname, DocumentURLs[document]);
   /* suppress compress suffixes from tempname */
   i = strlen (tempname) - 1;
   if (i > 2 && !strcmp (&tempname[i-2], ".gz"))
     {
       tempname[i-2] = EOS;
       TtaFreeMemory (DocumentURLs[SavingDocument]);
       DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempname);
     }
   else if (i > 1 && !strcmp (&tempname[i-1], ".Z"))
     {
       tempname[i-1] = EOS;
       TtaFreeMemory (DocumentURLs[SavingDocument]);
       DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempname);
     }

   /* if it is a W3 document use the current DirectoryName */
   if (IsW3Path (tempname))
     TtaExtractName (tempname, DirectoryName, DocumentName);
   else
     {
	TtaGetDocumentDirectory (SavingDocument, tempname, MAX_LENGTH);
	strcpy (DirectoryName, tempname);
	strcpy (DocumentName, TtaGetDocumentName (SavingDocument));
	strcat (tempname, DIR_STR);
	if (!IsHTMLName (DocumentName))
	   strcat (DocumentName, ".html");
	strcat (tempname, DocumentName);
     }

   /* display saving form */
   InitSaveForm (document, 1, tempname);
}


/*----------------------------------------------------------------------
   SaveDocumentLocally save the document in a local file.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SaveDocumentLocally (char *directoryName, char *documentName)
#else
static void         SaveDocumentLocally (directoryName, documentName)
char               *directoryName;
char               *documentName;

#endif
{
   char                tempname[MAX_LENGTH];
   char                docname[100];

DBG(fprintf(stderr, "SaveDocumentLocally :  %s / %s\n", directoryName, documentName);)

   strcpy (tempname, directoryName);
   strcat (tempname, DIR_STR);
   strcat (tempname, documentName);
   if (SaveAsText) 
     {
      SetInternalLinks (SavingDocument);
      TtaExportDocument (SavingDocument, tempname, "HTMLTT");
     }
   else
     {
      TtaExportDocument (SavingDocument, tempname, "HTMLT");
      TtaSetDocumentDirectory (SavingDocument, directoryName);
      strcpy (docname, documentName);
      ExtractSuffix (docname, tempname);
      /* Change the document name in all views */
      TtaSetDocumentName (SavingDocument, docname);
      TtaSetTextZone (SavingDocument, 1, 1, DocumentURLs[SavingDocument]);
      TtaSetDocumentUnmodified (SavingDocument);
     }
}

/*----------------------------------------------------------------------
  AddNoName
  This function is called whenether one tries to save a document
  without name (just the directory path e.g. http://www.w3.org/pub/WWW/ )
  It ask the user whether an extra name suffix should be added or
  abort.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      AddNoName (Document document, View view, char *url, boolean *ok)
#else
static boolean      AddNoName (document, view, url, ok)
Document            document;
View                view;
char               *url;
boolean            *ok;
#endif
{
   char                msg[MAX_LENGTH];
   char                documentname[MAX_LENGTH];

DBG(fprintf(stderr, "AddNoName :  %s \n", url);)

   TtaExtractName (url, msg, documentname);
   *ok = (documentname[0] != EOS);
   if (*ok)
     return (FALSE);
   else
     {
       /* the name is not correct for the put operation */
       strcpy (msg, TtaGetMessage(AMAYA, AM_NO_NAME));
       strcat (msg, url);
       strcat (msg, DIR_STR);
       strcat (msg, "noname.html");
       InitConfirm (document, view, msg);
       if (UserAnswer == 0)
	 return (FALSE);
       else
	 return (TRUE);
     }
}

/*----------------------------------------------------------------------
  SafeSaveFileThroughNet
  Send a file through the Network (using the PUT HTTP method) and double
  check for errors using a following GET.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          SafeSaveFileThroughNet (Document doc, char *localfile,
                          char *remotefile, PicType filetype)
#else
static int          SafeSaveFileThroughNet (doc, localfile, remotefile, filetype)
Document            doc;
char               *localfile;
char               *remotefile;
PicType             filetype;

#endif
{
    int res;
    char msg[10000];
    char tempfile[MAX_LENGTH]; /* Name of the file used to refetch */
    char tempURL[MAX_LENGTH];  /* May be redirected */
    char *no_reread_check;
    char *no_write_check;

    no_reread_check = TtaGetEnvString("NO_REREAD_CHECK");
    no_write_check = TtaGetEnvString("NO_WRITE_CHECK");

DBG(fprintf(stderr, "SafeSaveFileThroughNet :  %s to %s type %d\n", localfile, remotefile, filetype);)

    /*
     * Save.
     */
    res = PutObjectWWW(doc, localfile, remotefile, AMAYA_SYNC | AMAYA_NOCACHE,
                       filetype, NULL, NULL);
    if (res) {
        /*
	 * The HTTP PUT method failed !
	 */
        return(res);
    }

    if (no_reread_check != NULL) return(0);

    /*
     * Refetch
     */

DBG(fprintf(stderr, "SafeSaveFileThroughNet :  refetch %s \n", remotefile);)

    TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_VERIFYING), "");
    strcpy(tempURL, remotefile);
    res = GetObjectWWW(doc, tempURL, NULL, &tempfile[0],
                       AMAYA_SYNC | AMAYA_NOCACHE,
                       NULL, NULL, NULL, NULL, NO);
    if (res) {
        /*
	 * The HTTP GET method failed !
	 */
	sprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_RELOAD_FAILED),
		 remotefile);
	InitConfirm (doc, 1, msg);
	if (!UserAnswer) {
	   /* Trigger the error */
	   return (res);
	}
	/* Ignore the read failure */
	return(0);
    }

    /*
     * Compare URLs In case of redirection.
     */
    if (strcmp(remotefile, tempURL)) {
        /*
	 * Warning : redirect...
	 */
	sprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_REDIRECTED),
		 remotefile, tempURL);
	InitConfirm (doc, 1, msg);
	if (!UserAnswer) {
	   /* Trigger the error */
	   TtaFileUnlink(tempfile);
	   return(-1);
	}
    }

    if (no_write_check != NULL) return(0);

    /*
     * Compare content.
     */

DBG(fprintf(stderr, "SafeSaveFileThroughNet :  compare %s and %s \n", remotefile, localfile);)

    if (! TtaCompareFiles(tempfile, localfile)) {
	sprintf (msg, TtaGetMessage (AMAYA, AM_SAVE_COMPARE_FAILED),
		 remotefile);
	InitConfirm (doc, 1, msg);
	if (!UserAnswer) {
	   /* Trigger the error */
	   TtaFileUnlink(tempfile);
	   return(-1);
	}
    }

    TtaFileUnlink(tempfile);
    return(0);
}

/*----------------------------------------------------------------------
  SaveDocumentThroughNet
  Save a document and the included images to a remote network location.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          SaveDocumentThroughNet (Document document, View view,
					 boolean confirm, boolean with_images)
#else
static int          SaveDocumentThroughNet (document, view, confirm,
                                         with_images)
Document            document;
View                view;
boolean             confirm;
boolean             with_images;

#endif
{
   char             url[MAX_LENGTH];
   char             tempname[MAX_LENGTH];
   char             documentname[MAX_LENGTH];
   char             msg[5000];
   int              free = 10000;
   int              index = 0, len, nb = 0;
   LoadedImageDesc *pImage;
   int              res;
   int              imageType;

   if (!IsW3Path (DocumentURLs[document]))
      return (-1);

   /*
    * First step : build the output and ask for confirmation.
    */
   /* save into the temporary document file */
   TtaExtractName (DocumentURLs[document], url, documentname);
   sprintf (tempname, "%s%s%d%s", TempFileDirectory, DIR_STR, document, DIR_STR);
   strcat (tempname, documentname);

DBG(fprintf(stderr, "SaveFileThroughNet :  export to %s \n", tempname);)

   TtaExportDocument (document, tempname, "HTMLT");

   if (confirm && with_images)
     {
       TtaNewForm (BaseDialog + ConfirmSave, TtaGetViewFrame (document, view), 
		   TtaGetMessage (LIB, TMSG_LIB_CONFIRM), TRUE, 1, 'L', D_CANCEL);
       TtaNewLabel (BaseDialog + Label1, BaseDialog + ConfirmSave, TtaGetMessage (AMAYA, AM_WARNING_SAVE_OVERWRITE));
       
       strcpy (&msg[index], DocumentURLs[document]);
       len = strlen (DocumentURLs[document]);
       len++;
       free -= len;
       index += len;
       nb++;

       pImage = ImageURLs;
       while (pImage != NULL)
	 {
	   if (pImage->document == document)
	     {
	       if (pImage->status == IMAGE_MODIFIED)
		 {
		   if (nb > 30)
		     {
		       strcpy (&msg[index], "...");
		       len = strlen ("...");
		       len++;
		       free -= len;
		       index += len;
		       nb++;
		       break;
		     }
		   strcpy (&msg[index], pImage->originalName);
		   len = strlen (pImage->originalName);
		   len++;
		   free -= len;
		   index += len;
		   nb++;
		 }
	     }
	   pImage = pImage->nextImage;
	 }

       TtaNewSelector (BaseDialog + ConfirmSaveList, BaseDialog + ConfirmSave,
		       NULL, nb, &msg[0], 6, NULL, FALSE, TRUE);
       
       TtaSetDialoguePosition ();
       TtaShowDialogue (BaseDialog + ConfirmSave, FALSE);
       /* wait for an answer */
       TtaWaitShowDialogue ();
       if (!UserAnswer)
	 return (-1);
     }
   /*
    * Second step : saving the HTML content and the images modified locally.
    *               if saving failed, suggest to save to disk.
    */

   ActiveTransfer (document);
   TtaHandlePendingEvents ();

   if (with_images)
      pImage = ImageURLs;
   else
      pImage = NULL;
   while (pImage != NULL)
     {
	if (pImage->document == document)
	  {
	     if (pImage->status == IMAGE_MODIFIED)
	       {

DBG(fprintf(stderr, "Image %s modified\n", pImage->localName);)

		  imageType = (int) TtaGetPictureType ((Element) pImage->elImage);
		  res = SafeSaveFileThroughNet(document, pImage->localName,
					   pImage->originalName, imageType);
		  if (res)
		    {
#ifndef AMAYA_JAVA
		      DocNetworkStatus[document] |= AMAYA_NET_ERROR;
#endif /* AMAYA_JAVA */
		       ResetStop (document);
		       sprintf (msg, "%s %s \n%s\n%s",
		                TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
				pImage->originalName, 
				AmayaLastHTTPErrorMsg,
				TtaGetMessage (AMAYA, AM_SAVE_DISK));
		       InitConfirm (document, view, msg);
		       /* JK: to erase the last status message */
		       TtaSetStatus (document, view, "", NULL);

		       if (UserAnswer)
			  return (-1);
		       return (0);
		    }
		  pImage->status = IMAGE_LOADED;
	       }

DBG(  else fprintf(stderr, "Image %s not modified\n", pImage->localName);)

	  }
	pImage = pImage->nextImage;
     }

DBG(fprintf(stderr, "Saving HTML document %s\n", tempname);)

   res = SafeSaveFileThroughNet (document, tempname, DocumentURLs[document],
                             unknown_type);

   if (res)
     {
#ifndef AMAYA_JAVA
       DocNetworkStatus[document] |= AMAYA_NET_ERROR;
#endif /* AMAYA_JAVA */
        ResetStop (document);
	sprintf (msg, "%s %s \n%s\n%s",
		 TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
		 DocumentURLs[document],
		 AmayaLastHTTPErrorMsg,
		 TtaGetMessage (AMAYA, AM_SAVE_DISK));
	InitConfirm (document, view, msg);
	/* JK: to erase the last status message */
	TtaSetStatus (document, view, "", NULL);
	if (UserAnswer)
	   return (-1);
	return (0);
     }
   ResetStop (document);
   TtaSetDocumentUnmodified (document);

DBG(fprintf(stderr, "Saving completed\n");)

   return (0);
}

/*----------------------------------------------------------------------
  SaveDocument
  Entry point called whenether the user select the Save menu entry or
  press the Save button.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SaveDocument (Document document, View view)
#else
void                SaveDocument (document, view)
Document            document;
View                view;

#endif
{
   char                tempname[MAX_LENGTH];
   int                 i;
   boolean             ok;

   if (SavingDocument != (Document) None)
      return;
   SavingDocument = document;
   ok = FALSE;

   /* attempt to save through network if possible */
   strcpy (tempname, DocumentURLs[document]);
   /* suppress compress suffixes from tempname */
   i = strlen (tempname) - 1;
   if (i > 2 && !strcmp (&tempname[i-2], ".gz"))
     {
       tempname[i-2] = EOS;
       TtaFreeMemory (DocumentURLs[SavingDocument]);
       DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempname);
     }
   else if (i > 1 && !strcmp (&tempname[i-1], ".Z"))
     {
       tempname[i-1] = EOS;
       TtaFreeMemory (DocumentURLs[SavingDocument]);
       DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempname);
     }

DBG(fprintf(stderr, "SaveDocument : %d to %s\n", document, tempname);)

   if (IsW3Path (tempname))
     {
       if (AddNoName (document, view, tempname, &ok))
	 {
	   ok = TRUE;
	   /* need to update the document url */
	   strcat (tempname, DIR_STR);
	   strcat (tempname, "noname.html");
	   TtaFreeMemory (DocumentURLs[SavingDocument]);
	   DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempname);
	 }

DBG(fprintf(stderr, "SaveDocument : remote saving\n");)

       if (ok && SaveDocumentThroughNet (document, view, FALSE, TRUE) == 0)
	 {
	   TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_SAVED), DocumentURLs[document]);
	   SavingDocument = (Document) None;
	   ok = TRUE;
	 }
       else
	 ok = FALSE;

       if (!ok)
	 {
	   /* cannot save */
	   TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), DocumentURLs[document]);
	   SavingDocument = (Document) None;
	   SaveDocumentAs (document, 1);
	 }
       }
   else
     {

DBG(fprintf(stderr, "SaveDocument : local saving\n");)

       TtaExportDocument (document, tempname, "HTMLT");
       TtaSetDocumentUnmodified (document);
       TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_SAVED), DocumentURLs[document]);
       SavingDocument = (Document) None;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             DocumentToSave (NotifyDialog * event)
#else  /* __STDC__ */
boolean             DocumentToSave (event)
NotifyDialog       *event;

#endif /* __STDC__ */
{
   SaveDocument (event->document, 1);
   /* This function has to be written */
   return True;
}


/*----------------------------------------------------------------------
  UpdateImages
  changes image SRCs and saves image files if CopyImages is TRUE.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void            UpdateImages (char *imgbase, boolean src_is_local, boolean dst_is_local, char *newURL)
#else
static void            UpdateImages (imgbase, src_is_local, dst_is_local, newURL)
char                  *imgbase;
boolean                src_is_local;
boolean                dst_is_local;
char                  *newURL;
#endif
{
   char                tempfile[MAX_LENGTH];
   char                localpath[MAX_LENGTH];
   char                oldpath[MAX_LENGTH];
   char                tempname[MAX_LENGTH];
   char                imgname[MAX_LENGTH];
   char                url[MAX_LENGTH];
   char               *buf;
   int                 buflen;
   AttributeType       attrType;
   ElementType         elType;
   Attribute           attr;
   Element             el;
   LoadedImageDesc    *pImage;

DBG(fprintf(stderr, "UpdateImages : base %s, src_is_local %d, dst_local %d, URL %s\n", imgbase, (int) src_is_local, (int) dst_is_local, newURL);)

   /* save the old document path to locate images */
   strcpy (tempfile, DocumentURLs[SavingDocument]);
   TtaExtractName (tempfile, oldpath, tempname);
   strcat (oldpath, DIR_STR);
   /* path to search image descriptors */
   sprintf (localpath, "%s%s%d%s", TempFileDirectory, DIR_STR, SavingDocument, DIR_STR);

   /* update the document url */
   TtaFreeMemory (DocumentURLs[SavingDocument]);
   DocumentURLs[SavingDocument] = (char *) TtaStrdup (newURL);
   el = TtaGetMainRoot (SavingDocument);
   if (CopyImages)
     {
       /* search the BASE element */
       elType.ElSSchema = TtaGetDocumentSSchema (SavingDocument);
       elType.ElTypeNum = HTML_EL_BASE;
       el = TtaSearchTypedElement (elType, SearchInTree, el);
       if (el)
	 {
	   /* 
	   ** The document has a BASE element 
	   ** Get the HREF attribute of the BASE Element 
	   */
	   attrType.AttrSSchema = elType.ElSSchema;
	   attrType.AttrTypeNum = HTML_ATTR_HREF_;
	   attr = TtaGetAttribute (el, attrType);
	   if (attr)
	     {
	       /* change the base */
	       buflen = MAX_LENGTH;
	       TtaGiveTextAttributeValue (attr, oldpath, &buflen);

DBG(fprintf(stderr, "     old BASE %s\n", oldpath);)

	       if (imgbase[0] != EOS)
		 {

DBG(fprintf(stderr, "     changing to new BASE %s\n", imgbase);)

		   TtaSetAttributeText (attr, imgbase, el, SavingDocument);
		   imgbase[0] = EOS;
		 }
	       else
		 {
		   buf = AmayaParseUrl (newURL, "", AMAYA_PARSE_ALL);
		   if (buf)
		     {
		       buf = AmayaSimplifyUrl (&buf);
		       TtaExtractName (buf, tempfile, tempname);
		       strcat (tempfile, DIR_STR);

DBG(fprintf(stderr, "     changing to new BASE %s\n", tempfile);)

		       TtaSetAttributeText (attr, tempfile, el, SavingDocument);
		       TtaFreeMemory (buf);
		     }
		 }
	     }
	 }
       else
         {

DBG(fprintf(stderr, "     document has no BASE\n");)

	 el = TtaGetMainRoot (SavingDocument);
	 }

       /* Change all Picture SRC and prepare the saving process */
       /* 
	*                       \   newpath=local |  newpath=remote
	* oldpath                \                |
	* ------------------------|---------------|------------------
	*        | old img=remote | .amaya->file  | update descriptor
	*  local |----------------|---------------|------------------
	*        | old img=local  | file->file    | add descriptor
	* ------------------------|---------------|------------------
	*        | old img=remote | .amaya->file  | update descriptor
	* remote |----------------|---------------|------------------
	*        | old img=local  |   xxxxxxxxxxxxxxxxxxxxxxxxxxxx
	* ------------------------|---------------|------------------
	*/
       attrType.AttrSSchema = TtaGetDocumentSSchema (SavingDocument);
       attrType.AttrTypeNum = HTML_ATTR_SRC;
       TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
       while (el != NULL)
	 {
	   elType = TtaGetElementType (el);
	   if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
	     {
#ifdef COUGAR
	       elType = TtaGetElementType (TtaGetParent(el));
	       if (elType.ElTypeNum != HTML_EL_Object)
		 {
#endif
	       buflen = TtaGetTextAttributeLength (attr);
	       buf = (char *) TtaGetMemory (buflen + 2);
	       if (buf == NULL)
		 break;
	       TtaGiveTextAttributeValue (attr, buf, &buflen);
	       
	       /* extract the image name */
	       NormalizeURL (buf, SavingDocument, tempname, imgname);
	       /* save the new SRC attr */
	       if (imgbase[0] != EOS)
		 {
		   /* absolute name */
		   strcpy (url, imgbase);
		   strcat (url, DIR_STR);
		   strcat (url, imgname);
		 }
	       else
		 /* relative name */
		 strcpy (url, imgname);

DBG(fprintf(stderr, "     SRC from %s to %s\n", buf, url);)

               if ((src_is_local) && (!dst_is_local)) {

DBG(fprintf(stderr, "     AddLocalImage %s\n", buf);)

		       /* add the localfile to the images list */
		       AddLocalImage (buf, imgname, tempname, SavingDocument, &pImage);
	       }

	       TtaSetAttributeText (attr, url, el, SavingDocument);

	       /* mark the image descriptor or copy the file */
	       if (dst_is_local)
		 {
		   /* copy the file to the new location */
		   if (IsW3Path (buf) || IsHTTPPath (oldpath))
		     {
		       /* it was a remote image */
		       /* change tempname to the local temporary name */
		       strcpy (tempname, localpath);
		       strcat (tempname, imgname);
		     }
		   else
		     {
		       /* rebuild the old image path */
		       if (buf[0] != DIR_SEP)
			   strcpy (tempname, oldpath);
		       else
		           tempname[0] = '\0';
		       strcat (tempname, buf);
		     }
		   if (imgbase[0] != EOS)
		     {
		       strcpy (tempfile, imgbase);
		       strcat (tempfile, DIR_STR);
		       strcat (tempfile, imgname);
		     }
		   else
		     {
		       strcpy (tempfile, DirectoryName);
		       strcat (tempfile, DIR_STR);
		       strcat (tempfile, imgname);
		     }

DBG(fprintf(stderr, "     Copying image locally from %s to %s\n", tempname, tempfile);)

		   TtaFileCopy (tempname, tempfile);
		 }
	       else
		 {
		   /* compute the real new path of the image */
		   NormalizeURL (url, SavingDocument, tempname, imgname);
		   /* save on a remote server */
		   if (IsW3Path (tempname) || IsHTTPPath (oldpath))
		     {
		       /* it was already a remote image */
		       /* change tempname to the local temporary name */
		       strcpy (tempfile, localpath);
		       strcat (tempfile, imgname);
		       pImage = SearchLoadedImage (tempfile, SavingDocument);
		       /* update the informations on the images list */
		       if (pImage)
			 {

DBG(fprintf(stderr, "     Image was loaded from net from %s\n", pImage->originalName);)

			   /* image was already loaded */
			   if (pImage->originalName != NULL)
			     TtaFreeMemory (pImage->originalName);

DBG(fprintf(stderr, "     Mark it MODIFIED(%s)\n", tempname);)

			   pImage->originalName = (char *) TtaStrdup (tempname);
			   pImage->status = IMAGE_MODIFIED;
			   pImage->elImage = (struct _ElemImage *) el;
			 }
		     }
		   else
		     {
		       /* reset the old name */
		       strcpy (tempfile, oldpath);
		       strcat (tempfile, imgname);

DBG(fprintf(stderr, "     Image was loaded from disk %s\n", tempfile);)
DBG(fprintf(stderr, "     AddLocalImage %s\n", tempname);)

		       /* add the localfile to the images list */
		       AddLocalImage (tempfile, imgname, tempname, SavingDocument, &pImage);
		     }

		 }
	       TtaFreeMemory (buf);
#ifdef COUGAR
		 }
#endif
	     }
	   TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
	 }

       /*****************************************************************
	*								*
	*   Mark modified all images loaded for this document		*
	*								*
	*****************************************************************/
       pImage = ImageURLs;
       while (pImage != NULL) {
          if ((pImage->status == IMAGE_LOADED) &&
	      (pImage->document == SavingDocument))
	      pImage->status = IMAGE_MODIFIED;
	  pImage = pImage->nextImage;
       }

     }
   else
     {

DBG(fprintf(stderr, "     Images are not saved\n");)

       /* do not publish images */
     }
}


/*----------------------------------------------------------------------
  DoSaveAs
  This function is called when the user press the OK button on the
  Save As dialog. This is tricky, one must take care of a lot of
  parameters, whether initial and final location are local or remote
  and recomputes URLs accordingly.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DoSaveAs (void)
#else
void                DoSaveAs ()
#endif
{
   Document            doc;
   char                tempfile[MAX_LENGTH];
   char                tempname[MAX_LENGTH];
   char                imgbase[MAX_LENGTH];
   char               *proto;
   char               *host;
   char               *dir;
   char               *file;
   boolean             src_is_local;
   boolean             dst_is_local, ok;
   boolean             with_images;
   int                 res;

   src_is_local = !IsW3Path (DocumentURLs[SavingDocument]);
   dst_is_local = !IsW3Path (DirectoryName);

   if (CopyImages) with_images = TRUE;
   else with_images = FALSE;


DBG(fprintf(stderr, "DoSaveAs : from %s to %s/%s , with images %d\n", DocumentURLs[SavingDocument], DirectoryName, DocumentName, (int) with_images);)

   if (DocumentName[0] == '\0')
     {
       strcpy (tempfile, DirectoryName);
       if (AddNoName (SavingDocument, 1, tempfile, &ok))
	 {
	   res = strlen(DirectoryName) - 1;
	   if (DirectoryName[res] == DIR_SEP)
	     DirectoryName[res] = '\0';
	   /* need to update the document url */
	   strcpy (DocumentName, "noname.html");

DBG(fprintf(stderr, " set DocumentName to noname.html\n");)

	 }
       else if (!ok)
	 {
	   /* save into the temporary document file */
	   doc = SavingDocument;
	   TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_CANNOT_SAVE), DocumentURLs[doc]);
	   SavingDocument = (Document) None;
	   SaveDocumentAs (doc, 1);
	   return;
	 }
     }

   /*
    * create the base directory/url for the images output.
    */
   imgbase[0] = 0;
   if (SaveImgsURL[0] != EOS)
     {
	if (!dst_is_local)
	  {
	     /* saving to a remote server */
	     if (IsW3Path (SaveImgsURL))
	       {
		  /* this indicate possibly a different server */
		  strcpy (imgbase, SaveImgsURL);
	       }
	     else if (SaveImgsURL[0] == DIR_SEP)
	       {
		  /* use a separate directory on the remote server */
		  strcpy (tempfile, DirectoryName);
		  strcat (tempfile, DIR_STR);
		  strcat (tempfile, "toto.html");
		  ExplodeURL (tempfile, &proto, &host, &dir, &file);
		  if ((proto == NULL) || (host == NULL))
		    strcpy (imgbase, DirectoryName);
		  else
		    sprintf (imgbase, "%s://%s%s", proto, host, SaveImgsURL);
	       }
	     else
	       {
		  /* this is a subdir of the doc destination on the server */
		  strcpy (imgbase, DirectoryName);
		  strcat (imgbase, DIR_STR);
		  strcat (imgbase, SaveImgsURL);
	       }

DBG(fprintf(stderr, "   Remote image location : %s\n", imgbase);)

	  }
	else
	  {
	     /* saving locally */
	     if (IsW3Path (SaveImgsURL))
	       {
		  /* not permitted, probably an error */
		  imgbase[0] = 0;
	       }
	     else if (SaveImgsURL[0] == DIR_SEP)
	       {
		  /* complete path to directory exists */
		  strcpy (imgbase, SaveImgsURL);
	       }
	     else
	       {
		  /* this is a subdir of the doc destination on the server */
		  strcpy (imgbase, DirectoryName);
		  strcat (imgbase, DIR_STR);
		  strcat (imgbase, SaveImgsURL);
	       }

DBG(fprintf(stderr, "   Local image location : %s\n", imgbase);)

	  }
     }

   if (src_is_local && dst_is_local)
     {
       /*
	*           Local to Local                 
	*						
	*   Just dump HTML to another directory    
	*/

	/*
	 * Moving a document locally : don't change anything on URL or images.
	 */
	strcpy (tempfile, DirectoryName);
	strcat (tempfile, DIR_STR);
	strcat (tempfile, DocumentName);

DBG(fprintf(stderr, "   Moving document locally : to %s\n", tempfile);)

	if (TtaFileExist (tempfile))
	  {
	     /* ask confirmation */
	     sprintf (tempname, TtaGetMessage (LIB, TMSG_FILE_EXIST), tempfile);
	     InitConfirm (SavingDocument, 1, tempname);
	     if (!UserAnswer)
	       {
		  /* the user has to change the name of the saving file */
		  SaveDocumentAs(SavingDocument, 1);
		  return;
	       }
	  }

	/*
	 * change all Picture SRC to the remote URL.
	 * and add them to the list of remote images.
	 */
	UpdateImages (imgbase, src_is_local, dst_is_local, tempfile);

	/* save the local document */
	SaveDocumentLocally (DirectoryName, DocumentName);
        TtaSetStatus (SavingDocument, 1, TtaGetMessage (AMAYA, AM_SAVED), tempfile);
        SavingDocument = (Document) None;
     }
   else if ((!src_is_local) && dst_is_local)
     {
       /*
	*           Remote to Local                
	*						
	*   + copy images				
	*   + move URLs to absolutes ones.		
	*/

	/*
	 * verify that we don't overwite anything and ask for confirmation
	 */
	strcpy (tempfile, DirectoryName);
	strcat (tempfile, DIR_STR);
	strcat (tempfile, DocumentName);

DBG(fprintf(stderr, "   Saving document locally from net to %s\n", tempfile);)

	if (TtaFileExist (tempfile))
	  {
	     /* ask confirmation */
	     sprintf (tempname, TtaGetMessage (LIB, TMSG_FILE_EXIST), tempfile);
	     InitConfirm (SavingDocument, 1, tempname);
	     if (!UserAnswer)
	       {
		  /* the user has to change the name of the saving file */
		  SaveDocumentAs(SavingDocument, 1);
		  return;
	       }
	  }

	/*
	 * Transform all URLs to absolute ones.
	 */
	if (UpdateURLs)
	   SetAbsoluteURLs (SavingDocument, 1);

	/*
	 * change all Picture SRC to the remote URL.
	 * and add them to the list of remote images.
	 */
	strcpy (tempfile, DirectoryName);
	strcat (tempfile, DIR_STR);
	strcat (tempfile, DocumentName);
	UpdateImages (imgbase, src_is_local, dst_is_local, tempfile);

	/*
	 * now save the file as-if
	 */
	SaveDocumentLocally (DirectoryName, DocumentName);
        TtaSetStatus (SavingDocument, 1, TtaGetMessage (AMAYA, AM_SAVED), tempfile);
        SavingDocument = (Document) None;
     }
   else if (src_is_local && (!dst_is_local))
     {
       /*
	*           Local to Remote                
	*						
	*   + copy images				
	*/

	/* change all Picture SRC to the remote URL and add them to the */
        /* list of remote images */
	strcpy (tempfile, DirectoryName);
	strcat (tempfile, DIR_STR);
	strcat (tempfile, DocumentName);

DBG(fprintf(stderr, "   Uploading document to net %s\n", tempfile);)

	UpdateImages (imgbase, src_is_local, dst_is_local, tempfile);

	/* update informations on the document. */
	TtaSetTextZone (SavingDocument, 1, 1, DocumentURLs[SavingDocument]);

	/* now save the file as through the normal process of saving */
	/* to a remote URL. */
	res = SaveDocumentThroughNet (SavingDocument, 1, TRUE, with_images);

	if (res)
	  /* restore all urls */
	  SaveDocumentAs(SavingDocument, 1);
	else
	  {
	     TtaSetStatus (SavingDocument, 1, TtaGetMessage (AMAYA, AM_SAVED), tempfile);
	     SavingDocument = (Document) None;
	  }
     }
   else	/* ! src_is_local && ! dst_is_local */
     {
       /*
	*           Remote to Remote               
	*						
	*   + copy images				
	*   + move URLs to absolutes ones.		
	*/

	/*
	 * Transform all URLs to absolute ones.
	 */
	if (UpdateURLs)
	   SetAbsoluteURLs (SavingDocument, 1);

	/* change all Picture SRC to the new remote URL. */
	/* and update modifications to the list of remote images. */
	strcpy (tempfile, DirectoryName);
	strcat (tempfile, DIR_STR);
	strcat (tempfile, DocumentName);

DBG(fprintf(stderr, "   Copying remote document to remote URL %s\n", tempfile);)

	UpdateImages (imgbase, src_is_local, dst_is_local, tempfile);

	/* update informations on the document. */
	TtaSetTextZone (SavingDocument, 1, 1, DocumentURLs[SavingDocument]);
	/* now save the file as through the normal process of saving
	 * to a remote URL. */
	res = SaveDocumentThroughNet (SavingDocument, 1, TRUE, with_images);

	if (res)
	  SaveDocumentAs(SavingDocument, 1);
	else
	  {
	     TtaSetStatus (SavingDocument, 1, TtaGetMessage (AMAYA, AM_SAVED), tempfile);
	     SavingDocument = (Document) None;
	  }
     }
}



/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitSaveObjectForm (Document document, View view, char *object,
					char *pathname)
#else
void                InitSaveObjectForm (document, view, object, pathname)
Document            document;
View                view;
char               *object;
char               *pathname;

#endif
{
   char                tempdir[MAX_LENGTH];

   if (SavingDocument != (Document) None)
      return;
   if (SavingObject != (Document) None)
      return;
   SavingObject = document;
   strncpy (tempSavedObject, object, sizeof (tempSavedObject));

   /* Dialogue form for saving as */
   TtaNewForm (BaseDialog + SaveForm, TtaGetViewFrame (document, view), 
	       TtaGetMessage (AMAYA, AM_SAVE_AS), TRUE, 2, 'L', D_CANCEL);
   TtaListDirectory (DirectoryName, BaseDialog + SaveForm,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),
		     BaseDialog + DirSave, "",
		     TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSave);
   TtaNewTextForm (BaseDialog + NameSave, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_OBJECT_LOCATION), 50, 1, TRUE);
   TtaSetTextForm (BaseDialog + NameSave, pathname);
   TtaExtractName (pathname, tempdir, ObjectName);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + SaveForm, FALSE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DoSaveObjectAs (void)
#else
void                DoSaveObjectAs ()
#endif
{
   char                tempfile[MAX_LENGTH];
   char                msg[MAX_LENGTH];
   boolean             dst_is_local;
   int                 res;

   dst_is_local = !IsW3Path (DirectoryName);

   strcpy (tempfile, DirectoryName);
   strcat (tempfile, DIR_STR);
   strcat (tempfile, ObjectName);


   if (!dst_is_local)
     {
	res = PutObjectWWW (SavingObject, tempSavedObject, tempfile,
	                    unknown_type, AMAYA_SYNC, NULL, NULL);

	if (res)
	  {
	     TtaSetDialoguePosition ();
	     TtaShowDialogue (BaseDialog + SaveForm, FALSE);
	     return;
	  }
	SavingObject = 0;
	SavingDocument = 0;
	return;
     }
   if (TtaFileExist (tempfile))
     {
	/* ask confirmation */
	sprintf (msg, TtaGetMessage (LIB, TMSG_FILE_EXIST), tempfile);
	InitConfirm (SavingObject, 1, msg);
	if (!UserAnswer)
	  {
	     /* the user has to change the name of the saving file */
	     TtaSetDialoguePosition ();
	     TtaShowDialogue (BaseDialog + SaveForm, FALSE);
	     return;
	  }
     }
   TtaFileCopy (tempSavedObject, tempfile);
   SavingObject = 0;
   SavingDocument = 0;
}

