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

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"

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
#include "query_f.h"
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"


/*----------------------------------------------------------------------
   SetAbsoluteURLs : change relative URLs to absolute ones.        
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
   SaveHTMLDocumentAs                                              
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

   if (SavingDocument != (Document) None)
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
   SaveinHTML save the local document in HTML format               
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SaveInHTML (char *directoryName, char *documentName)
#else
void                SaveInHTML (directoryName, documentName)
char               *directoryName;
char               *documentName;

#endif
{
   char                tempname[MAX_LENGTH];
   char                docname[100];

   strcpy (tempname, directoryName);
   strcat (tempname, DIR_STR);
   strcat (tempname, documentName);
   TtaExportDocument (SavingDocument, tempname, "HTMLT");
   TtaSetDocumentDirectory (SavingDocument, directoryName);
   strcpy (docname, documentName);
   ExtractSuffix (docname, tempname);
   /* Change the document name in all views */
   TtaSetDocumentName (SavingDocument, docname);
   TtaSetDocumentUnmodified (SavingDocument);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitSaveForm (Document document, View view, char *pathname)
#else
void                InitSaveForm (document, view, pathname)
Document            document;
View                view;
char               *pathname;

#endif
{
   char                buffer[3000];

   /* Dialogue form for saving in local */
   TtaNewForm (BaseDialog + SaveForm, TtaGetViewFrame (document, view), 
	       TtaGetMessage (AMAYA, AM_SAVE_AS), TRUE, 3, 'L', D_CANCEL);
   /* TtaGetMessage(LIB, DOCUMENT_NAME) */
   sprintf (buffer, "%s%c%s", TtaGetMessage (AMAYA, AM_BCOPY_IMAGES), EOS,
	    TtaGetMessage (AMAYA, AM_BTRANSFORM_URL));
   TtaNewToggleMenu (BaseDialog + ToggleSave, BaseDialog + SaveForm,
		     NULL, 2, buffer, NULL, TRUE);
   TtaSetToggleMenu (BaseDialog + ToggleSave, 0, CopyImages);
   TtaSetToggleMenu (BaseDialog + ToggleSave, 1, UpdateURLs);
   TtaListDirectory (DirectoryName, BaseDialog + SaveForm,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),		/* std thot msg */
		     BaseDialog + DirSave, "",
		     TtaGetMessage (AMAYA, AM_FILES), BaseDialog + DocSave);
   TtaNewTextForm (BaseDialog + NameSave, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_DOC_LOCATION), 50, 1, TRUE);
   TtaSetTextForm (BaseDialog + NameSave, pathname);
   TtaNewLabel (BaseDialog + Lbl1Save, BaseDialog + SaveForm, "");
   TtaNewLabel (BaseDialog + Lbl2Save, BaseDialog + SaveForm, "");
   TtaNewTextForm (BaseDialog + ImgDirSave, BaseDialog + SaveForm,
		   TtaGetMessage (AMAYA, AM_IMAGES_LOCATION), 50, 1, TRUE);
   TtaSetTextForm (BaseDialog + ImgDirSave, SaveImgsURL);
   TtaNewLabel (BaseDialog + Lbl3Save, BaseDialog + SaveForm, "");
   TtaNewLabel (BaseDialog + Lbl4Save, BaseDialog + SaveForm, "");
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + SaveForm, FALSE);
}

/*----------------------------------------------------------------------
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
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 SaveDocumentThroughNet (Document document, View view, boolean confirm)
#else
int                 SaveDocumentThroughNet (document, view, confirm)
Document            document;
View                view;
boolean             confirm;

#endif
{
   char                url[MAX_LENGTH];
   char                tempname[MAX_LENGTH];
   char                documentname[MAX_LENGTH];
   char                msg[10000];
   int                 free = 10000;
   int                 index = 0, len, nb = 0;
   LoadedImageDesc    *pImage;
   int                 res;

   if (!IsW3Path (DocumentURLs[document]))
      return (-1);

   /*
    * First step : build the output and ask for confirmation.
    */
   /* save into the temporary document file */
   TtaExtractName (DocumentURLs[document], url, documentname);
   sprintf (tempname, "%s%s%d%s", TempFileDirectory, DIR_STR, document, DIR_STR);
   strcat (tempname, documentname);
   TtaExportDocument (document, tempname, "HTMLT");

   if (confirm)
     {
	TtaNewForm (BaseDialog + ConfirmSave, TtaGetViewFrame (document, view), 
	            TtaGetMessage (LIB, TMSG_LIB_CONFIRM), TRUE, 1, 'L', D_CANCEL);
	/*strcpy(msg,"Saving will overwrite the following URLs :"); */
	TtaNewLabel (BaseDialog + ConfirmSaveLbl, BaseDialog + ConfirmSave, TtaGetMessage (AMAYA, AM_WARNING_SAVE_OVERWRITE));

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

   pImage = ImageURLs;
   while (pImage != NULL)
     {
	if (pImage->document == document)
	  {
	     if (pImage->status == IMAGE_MODIFIED)
	       {
		  res = PutObjectWWW (document, pImage->localName,
				      pImage->originalName, AMAYA_SYNC,
				      (TTcbf *) NULL, (void *) NULL);
		  if (res != HT_OK)
		    {
		       FilesLoading[document] = 2;
		       ResetStop (document);
		       sprintf (msg, "%s %s \n%s", TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
				pImage->originalName, TtaGetMessage (AMAYA, AM_SAVE_DISK));
		       InitConfirm (document, view, msg);
		       /* JK: to erase the last status message */
		       TtaSetStatus (document, view, "", NULL);

		       if (UserAnswer)
			  return (-1);
		       return (0);
		    }
		  pImage->status = IMAGE_LOADED;
	       }
	  }
	pImage = pImage->nextImage;
     }

   res = PutObjectWWW (document, tempname, DocumentURLs[document], AMAYA_SYNC,
		       (TTcbf *) NULL, (void *) NULL);

   if (res != HT_OK)
     {
	FilesLoading[document] = 2;
	ResetStop (document);
	sprintf (msg, "Failed to save to URL %s. Save to disk ?",
		 DocumentURLs[document]);
	InitConfirm (document, view, msg);
	/* JK: to erase the last status message */
	TtaSetStatus (document, view, "", NULL);
	if (UserAnswer)
	   return (-1);
	return (0);
     }
   ResetStop (document);
   TtaSetDocumentUnmodified (document);
   return (0);
}

/*----------------------------------------------------------------------
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
       if (ok && SaveDocumentThroughNet (document, view, FALSE) == 0)
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
  UpdateImages: changes image SRCs and saves image files if CopyImages is TRUE.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void            UpdateImages (char *imgbase, boolean dst_is_local, char *newURL)
#else
static void            UpdateImages (imgbase, dst_is_local, newURL)
char                  *imgbase;
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


   /* save the old document path to locate images */
   strcpy (tempfile, DocumentURLs[SavingDocument]);
   TtaExtractName (tempfile, oldpath, tempname);
   strcat (oldpath, DIR_STR);
   /* path to search image descriptors */
   sprintf (localpath, "%s%s%d%s", TempFileDirectory, DIR_STR, SavingDocument, DIR_STR);

   /* update the document url */
   TtaFreeMemory (DocumentURLs[SavingDocument]);
   DocumentURLs[SavingDocument] = (char *) TtaStrdup (newURL);
   if (CopyImages)
     {
       el = TtaGetMainRoot (SavingDocument);
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
	       if (imgbase[0] != EOS)
		 {
		   TtaSetAttributeText (attr, imgbase, el, SavingDocument);
		   imgbase[0] = EOS;
		 }
	       else
		 {
		   buf = HTParse (newURL, "", PARSE_ALL);
		   if (buf)
		     {
		       buf = HTSimplify (&buf);
		       TtaExtractName (buf, tempfile, tempname);
		       strcat (tempfile, DIR_STR);
		       TtaSetAttributeText (attr, tempfile, el, SavingDocument);
		       HT_FREE (buf);
		     }
		 }
	     }
	 }
       else
	 el = TtaGetMainRoot (SavingDocument);

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
		       strcpy (tempname, oldpath);
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
			   /* image was already loaded */
			   if (pImage->originalName != NULL)
			     TtaFreeMemory (pImage->originalName);
			   pImage->originalName = (char *) TtaStrdup (tempname);
			   pImage->status = IMAGE_MODIFIED;
			 }
		     }
		   else
		     {
		       /* reset the old name */
		       strcpy (tempfile, oldpath);
		       strcat (tempfile, imgname);
		       /* add the localfile to the images list */
		       AddLocalImage (tempfile, imgname, tempname, SavingDocument, &pImage);
		     }

		 }
	       TtaFreeMemory (buf);
	     }
	   TtaSearchAttribute (attrType, SearchForward, el, &el, &attr);
	 }
     }
}


/*----------------------------------------------------------------------
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
   int                 res;

   src_is_local = !IsW3Path (DocumentURLs[SavingDocument]);
   dst_is_local = !IsW3Path (DirectoryName);
   if (!dst_is_local)
     {
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
	if (TtaFileExist (tempfile))
	  {
	     /* ask confirmation */
	     sprintf (tempname, TtaGetMessage (LIB, TMSG_FILE_EXIST), tempfile);
	     InitConfirm (SavingDocument, 1, tempname);
	     if (!UserAnswer)
	       {
		  /* the user has to change the name of the saving file */
		  TtaSetDialoguePosition ();
		  TtaShowDialogue (BaseDialog + SaveForm, FALSE);
		  return;
	       }
	  }
	TtaDestroyDialogue (BaseDialog + SaveForm);

	/*
	 * change all Picture SRC to the remote URL.
	 * and add them to the list of remote images.
	 */
	UpdateImages (imgbase, dst_is_local, tempfile);
	/* save the local document */
	SaveInHTML (DirectoryName, DocumentName);
	TtaSetTextZone (SavingDocument, 1, 1, DocumentURLs[SavingDocument]);
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
	if (TtaFileExist (tempfile))
	  {
	     /* ask confirmation */
	     sprintf (tempname, TtaGetMessage (LIB, TMSG_FILE_EXIST), tempfile);
	     InitConfirm (SavingDocument, 1, tempname);
	     if (!UserAnswer)
	       {
		  /* the user has to change the name of the saving file */
		  TtaSetDialoguePosition ();
		  TtaShowDialogue (BaseDialog + SaveForm, FALSE);
		  return;
	       }
	  }
	TtaDestroyDialogue (BaseDialog + SaveForm);

	/*
	 * change all Picture SRC to the remote URL.
	 * and add them to the list of remote images.
	 */
	strcpy (tempfile, DirectoryName);
	strcat (tempfile, DIR_STR);
	strcat (tempfile, DocumentName);
	UpdateImages (imgbase, dst_is_local, tempfile);
	/*
	 * Transform all URLs to absolute ones.
	 */
	if (UpdateURLs)
	   SetAbsoluteURLs (SavingDocument, 1);

	/*
	 * now save the file as-if
	 */
	SaveInHTML (DirectoryName, DocumentName);
	TtaSetTextZone (SavingDocument, 1, 1, DocumentURLs[SavingDocument]);
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
	UpdateImages (imgbase, dst_is_local, tempfile);
	/* update informations on the document. */
	TtaSetTextZone (SavingDocument, 1, 1, DocumentURLs[SavingDocument]);
	/* now save the file as through the normal process of saving */
	/* to a remote URL. */
	res = SaveDocumentThroughNet (SavingDocument, 1, TRUE);

	if (res)
	  {
	    /* restore all urls */
	    TtaSetDialoguePosition ();
	    TtaShowDialogue (BaseDialog + SaveForm, FALSE);
	  }
	else
	  {
	     TtaDestroyDialogue (BaseDialog + SaveForm);
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
	if (UpdateImages)
	   SetAbsoluteURLs (SavingDocument, 1);

	/* change all Picture SRC to the new remote URL. */
	/* and update modifications to the list of remote images. */
	strcpy (tempfile, DirectoryName);
	strcat (tempfile, DIR_STR);
	strcat (tempfile, DocumentName);
	UpdateImages (imgbase, dst_is_local, tempfile);
	/* update informations on the document. */
	TtaSetTextZone (SavingDocument, 1, 1, DocumentURLs[SavingDocument]);
	/* now save the file as through the normal process of saving
	 * to a remote URL. */
	res = SaveDocumentThroughNet (SavingDocument, 1, TRUE);

	if (res)
	  {
	     TtaSetDialoguePosition ();
	     TtaShowDialogue (BaseDialog + SaveForm, FALSE);
	  }
	else
	  {
	     TtaDestroyDialogue (BaseDialog + SaveForm);
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
		     TtaGetMessage (LIB, TMSG_DOC_DIR),		/* std thot msg */
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
			    AMAYA_SYNC, (TTcbf *) NULL, (void *) NULL);

	if (res != HT_OK)
	  {
	     TtaSetDialoguePosition ();
	     TtaShowDialogue (BaseDialog + SaveForm, FALSE);
	     return;
	  }
	TtaDestroyDialogue (BaseDialog + SaveForm);
	SavingObject = (Document) None;
	SavingDocument = (Document) None;
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
   SavingObject = (Document) None;
   TtaDestroyDialogue (BaseDialog + SaveForm);
}
