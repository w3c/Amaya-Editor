/* Included headerfiles */
#include "amaya.h"
#include "content.h"
#include "view.h"
#include "interface.h"
#include "appaction.h"
#include "message.h"
#include "libmsg.h"
#include "dialog.h"
#include "browser.h"
#include "selection.h"
#include "presentation.h"
#include "HTMLactions.h"
#include "HTMLstyle.h"
#include "EDITOR.h"
#include "dialog.h"
#include "css.h"

#include "HTMLsave.h"
#include "init.h"
#include "HTMLimage.h"
#include "EDITimage.h"
#include "css.h"
#include "AHTURLTools.h"
#include "HTMLsave.h"

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

typedef struct struct_URL_elem {
   int                 attr_type;
   int                 elem_type;
} URL_elem;

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

/* ---------------------------------------------------------------------- */
/* |    SetAbsoluteURLs : change relative URLs to absolute ones.        | */
/* ---------------------------------------------------------------------- */
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

   for (index = 0; index < NB_URL_PAIR;) {
      /* fetch a new attr */
      attr = URL_elem_tab[index].attr_type;

      /*
       * search all elements having this attribute
       */
      el = TtaGetMainRoot (SavingDocument);
      atType.AttrSSchema = TtaGetDocumentSSchema (SavingDocument);
      atType.AttrTypeNum = attr;
      TtaSearchAttribute (atType, SearchForward, el, &el, &at);
      while (el != NULL) {

	 /*
	  * search for all consecutives pair of (attr,elem)
	  * if the current elem match.
	  */
	 elType = TtaGetElementType (el);
	 for (i = index;
	      (i < NB_URL_PAIR) && (URL_elem_tab[i].attr_type == attr);
	      i++) {

	    if (elType.ElTypeNum == URL_elem_tab[i].elem_type) {

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

/* ---------------------------------------------------------------------- */
/* |    SaveHTMLDocumentAs                                              | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
void                SaveDocumentAs (Document document, View view)
#else
void                SaveDocumentAs (document, view)
Document            document;
View                view;

#endif
{
   char                tempname[MAX_LENGTH];

   if (SavingDocument != (Document) None)
      return;
   if (SavingObject != (Document) None)
      return;

   /* memorize the current document */
   SavingDocument = document;
   /* if it is a W3 document use the current DirectoryName */
   if (IsW3Path (DocumentURLs[document])) {
      TtaExtractName (DocumentURLs[document], DirectoryName, DocumentName);
      strcpy (tempname, DocumentURLs[document]);
   } else {
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


/* ---------------------------------------------------------------------- */
/* |    SaveinHTML save the local document in HTML format               | */
/* ---------------------------------------------------------------------- */
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

/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
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
   TtaNewForm (BaseDialog + FormSauver, TtaGetViewFrame (document, view), 0, 0,
	       TtaGetMessage (AMAYA, AM_SAVE_LOCAL), TRUE, 3, 'L', D_DONE);
   /* TtaGetMessage(LIB, DOCUMENT_NAME) */
   sprintf (buffer, "%s%c%s", TtaGetMessage (AMAYA, AM_BCOPY_IMAGES), EOS,
	    TtaGetMessage (AMAYA, AM_BTRANSFORM_URL));
   TtaNewToggleMenu (BaseDialog + SauvToggle, BaseDialog + FormSauver,
		     NULL, 2, buffer, NULL, TRUE);
   TtaSetToggleMenu (BaseDialog + SauvToggle, 0, CopyImages);
   TtaSetToggleMenu (BaseDialog + SauvToggle, 1, UpdateURLs);
   TtaListDirectory (DirectoryName, BaseDialog + FormSauver,
		     TtaGetMessage (LIB, DOC_DIR),	/* std thot msg */
		     BaseDialog + SauvDir, "",
		     TtaGetMessage (AMAYA, AM_FILES), BaseDialog + SauvDoc);
   TtaNewTextForm (BaseDialog + SauvNom, BaseDialog + FormSauver,
		   TtaGetMessage (AMAYA, AM_DOC_LOCATION), 50, 1, TRUE);
   TtaSetTextForm (BaseDialog + SauvNom, pathname);
   TtaNewLabel (BaseDialog + SauvLbl1, BaseDialog + FormSauver, "");
   TtaNewLabel (BaseDialog + SauvLbl2, BaseDialog + FormSauver, "");
   TtaNewTextForm (BaseDialog + SauvImgsDir, BaseDialog + FormSauver,
		   TtaGetMessage (AMAYA, AM_IMAGES_LOCATION), 50, 1, TRUE);
   TtaSetTextForm (BaseDialog + SauvImgsDir, SaveImgsURL);
   TtaNewLabel (BaseDialog + SauvLbl3, BaseDialog + FormSauver, "");
   TtaNewLabel (BaseDialog + SauvLbl4, BaseDialog + FormSauver, "");
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + FormSauver, FALSE);
}

/* -------------------------------------------------------------- */
/* -------------------------------------------------------------- */
#ifdef __STDC__
int                 SaveDocumentThroughNet (Document document, View view, Boolean confirm)
#else
int                 SaveDocumentThroughNet (document, view, confirm)
Document            document;
View                view;
Boolean             confirm;

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
   TtaGetDocumentDirectory (document, url, MAX_LENGTH);
   TtaExtractName (DocumentURLs[document], url, documentname);
   if (documentname[0] == EOS) {
      strcpy (msg, "Document doesn't have a complete name. Saving to : \n");
      strcat (msg, url);
      strcat (msg, DIR_STR);
      strcpy (msg, "noname.html");
      InitConfirm (document, view, msg);
      if (UserAnswer == 0)
	 return (-1);
      strcpy (documentname, "noname.html");
   }
   sprintf (tempname, "%s%s%d%s", TempFileDirectory, DIR_STR, document, DIR_STR);
   strcat (tempname, documentname);
   TtaExportDocument (document, tempname, "HTMLT");

   if (confirm) {
      TtaNewForm (BaseDialog + SauvConfirm, TtaGetViewFrame (document, view),
		  0, 0, TtaGetMessage (LIB, LIB_CONFIRM), TRUE, 1, 'L', D_DONE);
      /*strcpy(msg,"Saving will overwrite the following URLs :"); */
      TtaNewLabel (BaseDialog + SauvConfirmLbl, BaseDialog + SauvConfirm, TtaGetMessage (AMAYA, AM_WARNING_SAVE_OVERWRITE));

      strcpy (&msg[index], DocumentURLs[document]);
      len = strlen (DocumentURLs[document]);
      len++;
      free -= len;
      index += len;
      nb++;

      pImage = ImageURLs;
      while (pImage != NULL) {
	 if (pImage->document == document) {
	    if (pImage->status == IMAGE_MODIFIED) {
	       if (nb > 30) {
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

      TtaNewSelector (BaseDialog + SauvConfirmList, BaseDialog + SauvConfirm,
		      NULL, nb, &msg[0], 6, NULL, FALSE, TRUE);

      TtaSetDialoguePosition ();
      TtaShowDialogue (BaseDialog + SauvConfirm, FALSE);
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
   while (pImage != NULL) {
      if (pImage->document == document) {
	 if (pImage->status == IMAGE_MODIFIED) {
	    res = PutObjectWWW (document, pImage->localName,
				pImage->originalName, AMAYA_SYNC,
				(TTcbf *) NULL, (void *) NULL);
	    if (res != HT_OK) {
	       FilesLoading[document] = 2;
	       ResetStop (document);
	       sprintf (msg, "%s %s\n%s", TtaGetMessage (AMAYA, AM_URL_SAVE_FAILED),
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

   if (res != HT_OK) {
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

/* -------------------------------------------------------------- */
/* -------------------------------------------------------------- */
#ifdef __STDC__
void                SaveDocument (Document document, View view)
#else
void                SaveDocument (document, view)
Document            document;
View                view;

#endif
{
   char                tempname[MAX_LENGTH];
   char                documentname[MAX_LENGTH];

   if (SavingDocument != (Document) None)
      return;
   SavingDocument = document;

   EndInsert ();
   TtaGetDocumentDirectory (document, tempname, MAX_LENGTH);

   /* attempt to save through network if possible */
   if ((IsW3Path (DocumentURLs[document])) &&
       (SaveDocumentThroughNet (document, view, FALSE) == 0)) {

      SavingDocument = (Document) None;
      return;
   }
   if (tempname[0] == EOS || IsW3Path (tempname)) {
      /* save into the temporary document file */
      TtaExtractName (DocumentURLs[document], tempname, documentname);
      sprintf (tempname, "%s%s%d%s", TempFileDirectory, DIR_STR, document, DIR_STR);
      strcat (tempname, documentname);
   } else
      strcpy (tempname, DocumentURLs[document]);

   TtaExportDocument (document, tempname, "HTMLT");
   TtaSetDocumentUnmodified (document);
   SavingDocument = (Document) None;
}
 
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean DocumentToSave(NotifyDialog *event)
#else /* __STDC__*/
boolean DocumentToSave(event)
     NotifyDialog *event;
#endif /* __STDC__*/
{
  SaveDocument(event->document, 1);
  /* This function has to be written */
  return True;
}

/* -------------------------------------------------------------- */
/* | CreateNewImagePaths : calculates the new URL and the new   | */
/* |     path to an Picture contained in a document.              | */
/* -------------------------------------------------------------- */
#ifdef __STDC__
void                CreateNewImagePaths (char *DocUrl, char *OldPath,
					 char *NewUrl, char *NewPath)
#else
void                CreateNewImagePaths (DocUrl, OldPath, NewUrl, NewPath)
char               *DocUrl;
char               *OldPath;
char               *NewUrl;
char               *NewPath;

#endif
{
}

/* -------------------------------------------------------------- */
/* -------------------------------------------------------------- */
#ifdef __STDC__
void                DoSaveAs (void)
#else
void                DoSaveAs ()
#endif
{
   char                tempfile[MAX_LENGTH];
   char                tempname[MAX_LENGTH];
   char                imgname[MAX_LENGTH];
   char                imgbase[MAX_LENGTH];
   char                url[MAX_LENGTH];
   char               *buf;
   int                 buflen;
   boolean             src_is_local;
   boolean             dst_is_local;
   LoadedImageDesc    *pImage;
   AttributeType       attrType;
   ElementType         elType;
   Attribute           attrSRC;
   Element             elSRC;
   int                 res;

   src_is_local = !IsW3Path (DocumentURLs[SavingDocument]);
   dst_is_local = !IsW3Path (DirectoryName);

   /*
    * create the base directory/url for the images output.
    */
   imgbase[0] = 0;
   if (SaveImgsURL[0] != EOS) {
      if (!dst_is_local) {
	 /* saving to a remote server */
	 if (IsW3Path (SaveImgsURL)) {
	    /* this indicate possibly a different server */
	    strcpy (imgbase, SaveImgsURL);
	 } else if (SaveImgsURL[0] == DIR_SEP) {
	    /* use a separate directory on the remote server */
	    char               *proto;
	    char               *host;
	    char               *dir;
	    char               *file;

	    strcpy (tempfile, DirectoryName);
	    strcat (tempfile, DIR_STR);
	    strcat (tempfile, "toto.html");
	    ExplodeURL (tempfile, &proto, &host, &dir, &file);
	    if ((proto == NULL) || (host == NULL)) {
	       strcpy (imgbase, DirectoryName);
	    } else
	       sprintf (imgbase, "%s://%s%s", proto, host, SaveImgsURL);
	 } else {
	    /* this is a subdir of the doc destination on the server */
	    strcpy (imgbase, DirectoryName);
	    strcat (imgbase, DIR_STR);
	    strcat (imgbase, SaveImgsURL);
	 }
      } else {
	 /* saving locally */
	 if (IsW3Path (SaveImgsURL)) {
	    /* not permitted, probably an error */
	    imgbase[0] = 0;
	 } else if (SaveImgsURL[0] == DIR_SEP) {
	    /* complete path to directory exists */
	    strcpy (imgbase, SaveImgsURL);
	 } else {
	    /* this is a subdir of the doc destination on the server */
	    strcpy (imgbase, DirectoryName);
	    strcat (imgbase, DIR_STR);
	    strcat (imgbase, SaveImgsURL);
	 }
      }
   }
   if (src_is_local && dst_is_local) {
    /********************************************
     *           Local to Local                 *
     *						*
     *   Just dump HTML to another directory    *
     ********************************************/

      /*
       * Moving a document locally : don't change anything on URL or images.
       */
      strcpy (tempfile, DirectoryName);
      strcat (tempfile, DIR_STR);
      strcat (tempfile, DocumentName);
      if (FileExist (tempfile)) {
	 /* ask confirmation */
	 sprintf (tempname, TtaGetMessage (LIB, FILE_EXIST), tempfile);
	 InitConfirm (SavingDocument, 1, tempname);
	 if (!UserAnswer) {
	    /* the user has to change the name of the saving file */
	    TtaSetDialoguePosition ();
	    TtaShowDialogue (BaseDialog + FormSauver, FALSE);
	    return;
	 }
      }
      /* save the local document */
      TtaDestroyDialogue (BaseDialog + FormSauver);
      SaveInHTML (DirectoryName, DocumentName);
      TtaFreeMemory (DocumentURLs[SavingDocument]);
      DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempfile);
      TtaSetTextZone (SavingDocument, 1, 1, DocumentURLs[SavingDocument]);

      SavingDocument = (Document) None;
   } else if ((!src_is_local) && dst_is_local) {
    /********************************************
     *           Remote to Local                *
     *						*
     *   + copy images				*
     *   + move URLs to absolutes ones.		*
     ********************************************/

      /*
       * verify that we don't overwite anything and ask for confirmation
       */
      strcpy (tempfile, DirectoryName);
      strcat (tempfile, DIR_STR);
      strcat (tempfile, DocumentName);
      if (FileExist (tempfile)) {
	 /* ask confirmation */
	 sprintf (tempname, TtaGetMessage (LIB, FILE_EXIST), tempfile);
	 InitConfirm (SavingDocument, 1, tempname);
	 if (!UserAnswer) {
	    /* the user has to change the name of the saving file */
	    TtaSetDialoguePosition ();
	    TtaShowDialogue (BaseDialog + FormSauver, FALSE);
	    return;
	 }
      }
      TtaDestroyDialogue (BaseDialog + FormSauver);

      /*
       * Transform all URLs to absolute ones.
       */
      if (UpdateURLs)
	 SetAbsoluteURLs (SavingDocument, 1);

      /*
       * change all remote Picture SRC to a local name.
       * and copy them to the local directory.
       */
      if (CopyImages) {
	 elSRC = TtaGetMainRoot (SavingDocument);
	 attrType.AttrSSchema = TtaGetDocumentSSchema (SavingDocument);
	 attrType.AttrTypeNum = HTML_ATTR_SRC;
	 TtaSearchAttribute (attrType, SearchForward, elSRC, &elSRC, &attrSRC);
	 while (elSRC != NULL) {
	    elType = TtaGetElementType (elSRC);
	    if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT) {
	       buflen = TtaGetTextAttributeLength (attrSRC);
	       buf = (char *) TtaGetMemory (buflen + 2);
	       if (buf == NULL)
		  break;
	       TtaGiveTextAttributeValue (attrSRC, buf, &buflen);

	       /* save the new SRC attr */
	       TtaExtractName (buf, url, imgname);
	       if (SaveImgsURL[0] != EOS) {
		  strcpy (url, SaveImgsURL);
		  strcat (url, DIR_STR);
		  strcat (url, imgname);
	       }
	       TtaSetAttributeText (attrSRC, url, elSRC, SavingDocument);

	       /* copy the file to the new location */
	       sprintf (tempname, "%s%s%d%s",
			TempFileDirectory, DIR_STR, SavingDocument, DIR_STR);
	       strcat (tempname, imgname);
	       if (imgbase[0] != EOS) {
		  strcpy (tempfile, imgbase);
		  strcat (tempfile, DIR_STR);
		  strcat (tempfile, imgname);
	       } else {
		  strcpy (tempfile, DirectoryName);
		  strcat (tempfile, DIR_STR);
		  strcat (tempfile, imgname);
	       }
	       ThotCopyFile (tempname, tempfile);

	       TtaFreeMemory (buf);
	    }
	    TtaSearchAttribute (attrType, SearchForward, elSRC, &elSRC, &attrSRC);
	 }
      }
      /*
       * now save the file as-if
       */
      SaveInHTML (DirectoryName, DocumentName);
      TtaFreeMemory (DocumentURLs[SavingDocument]);
      strcpy (tempfile, DirectoryName);
      strcat (tempfile, DIR_STR);
      strcat (tempfile, DocumentName);
      DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempfile);
      TtaSetTextZone (SavingDocument, 1, 1, DocumentURLs[SavingDocument]);

      SavingDocument = (Document) None;
   } else if (src_is_local && (!dst_is_local)) {
    /********************************************
     *           Local to Remote                *
     *						*
     *   + copy images				*
     ********************************************/

      /*
       * change all Picture SRC to the remote URL.
       * and add them to the list of remote images.
       */
      if (CopyImages) {
	 elSRC = TtaGetMainRoot (SavingDocument);
	 attrType.AttrSSchema = TtaGetDocumentSSchema (SavingDocument);
	 attrType.AttrTypeNum = HTML_ATTR_SRC;
	 TtaSearchAttribute (attrType, SearchForward, elSRC, &elSRC, &attrSRC);
	 while (elSRC != NULL) {
	    elType = TtaGetElementType (elSRC);
	    if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT) {
	       buflen = TtaGetTextAttributeLength (attrSRC);
	       buf = (char *) TtaGetMemory (buflen + 2);
	       if (buf == NULL)
		  break;
	       TtaGiveTextAttributeValue (attrSRC, buf, &buflen);

	       /* save the new SRC attr */
	       NormalizeURL (buf, SavingDocument, tempname, imgname);
	       if (SaveImgsURL[0] != EOS) {
		  strcpy (url, SaveImgsURL);
		  strcat (url, DIR_STR);
		  strcat (url, imgname);
	       } else {
		  strcpy (url, imgname);
	       }
	       TtaSetAttributeText (attrSRC, url, elSRC, SavingDocument);

	       /* create the new absolute url for the image */
	       if (imgbase[0] != EOS) {
		  strcpy (url, imgbase);
		  strcat (url, DIR_STR);
		  strcat (url, imgname);
	       } else {
		  strcpy (url, DirectoryName);
		  strcat (url, DIR_STR);
		  strcat (url, imgname);
	       }

	       /* add the localfile to the list */
	       NormalizeURL (buf, SavingDocument, tempname, imgname);
	       AddLocalImage (tempname, imgname, url, SavingDocument, &pImage);
	       TtaFreeMemory (buf);
	    }
	    TtaSearchAttribute (attrType, SearchForward, elSRC, &elSRC, &attrSRC);
	 }
      }
      /*
       * update informations on the document.
       */
      TtaFreeMemory (DocumentURLs[SavingDocument]);
      strcpy (tempfile, DirectoryName);
      strcat (tempfile, DIR_STR);
      strcat (tempfile, DocumentName);
      DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempfile);
      TtaSetTextZone (SavingDocument, 1, 1, DocumentURLs[SavingDocument]);

      /*
       * now save the file as through the normal process of saving
       * to a remote URL.
       */
      res = SaveDocumentThroughNet (SavingDocument, 1, TRUE);

      if (res) {
	 TtaSetDialoguePosition ();
	 TtaShowDialogue (BaseDialog + FormSauver, FALSE);
      } else {
	 TtaDestroyDialogue (BaseDialog + FormSauver);
	 SavingDocument = (Document) None;
      }
   } else {			/* ! src_is_local && ! dst_is_local */
    /********************************************
     *           Remote to Remote               *
     *						*
     *   + copy images				*
     *   + move URLs to absolutes ones.		*
     ********************************************/

      /*
       * Transform all URLs to absolute ones.
       */
      if (UpdateURLs)
	 SetAbsoluteURLs (SavingDocument, 1);

      /*
       * change all Picture SRC to the new remote URL.
       * and update modifications to the list of remote images.
       */
      if (CopyImages) {
	 elSRC = TtaGetMainRoot (SavingDocument);
	 attrType.AttrSSchema = TtaGetDocumentSSchema (SavingDocument);
	 attrType.AttrTypeNum = HTML_ATTR_SRC;
	 TtaSearchAttribute (attrType, SearchForward, elSRC, &elSRC, &attrSRC);
	 while (elSRC != NULL) {
	    elType = TtaGetElementType (elSRC);
	    if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT) {
	       buflen = TtaGetTextAttributeLength (attrSRC);
	       buf = (char *) TtaGetMemory (buflen + 2);
	       if (buf == NULL)
		  break;
	       TtaGiveTextAttributeValue (attrSRC, buf, &buflen);

	       /* save the new SRC attr */
	       NormalizeURL (buf, SavingDocument, tempname, imgname);
	       if (SaveImgsURL[0] != EOS) {
		  strcpy (url, SaveImgsURL);
		  strcat (url, DIR_STR);
		  strcat (url, imgname);
	       } else {
		  strcpy (url, imgname);
	       }
	       TtaSetAttributeText (attrSRC, url, elSRC, SavingDocument);

	       /* create the new absolute url for the image */
	       if (imgbase[0] != EOS) {
		  strcpy (url, imgbase);
		  strcat (url, DIR_STR);
		  strcat (url, imgname);
	       } else {
		  strcpy (url, DirectoryName);
		  strcat (url, DIR_STR);
		  strcat (url, imgname);
	       }

	       /* update the informations on the list */
	       if (!AddLoadedImage (imgname, tempname, SavingDocument,
				    &pImage)) {
		  /* image was loaded */
		  if (pImage->originalName != NULL)
		     TtaFreeMemory (pImage->originalName);
		  pImage->originalName = (char *) TtaStrdup (url);
		  pImage->status = IMAGE_MODIFIED;
	       } else {
		  /* well, this should not occurs */
		  pImage->status = IMAGE_NOT_LOADED;
	       }
	       TtaFreeMemory (buf);
	    }
	    TtaSearchAttribute (attrType, SearchForward, elSRC, &elSRC, &attrSRC);
	 }
      }
      /*
       * update informations on the document.
       */
      TtaFreeMemory (DocumentURLs[SavingDocument]);
      strcpy (tempfile, DirectoryName);
      strcat (tempfile, DIR_STR);
      strcat (tempfile, DocumentName);
      DocumentURLs[SavingDocument] = (char *) TtaStrdup (tempfile);
      TtaSetTextZone (SavingDocument, 1, 1, DocumentURLs[SavingDocument]);

      /*
       * now save the file as through the normal process of saving
       * to a remote URL.
       */
      res = SaveDocumentThroughNet (SavingDocument, 1, TRUE);

      if (res) {
	 TtaSetDialoguePosition ();
	 TtaShowDialogue (BaseDialog + FormSauver, FALSE);
      } else {
	 TtaDestroyDialogue (BaseDialog + FormSauver);
	 SavingDocument = (Document) None;
      }
   }
}



/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
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

   /* Dialogue form for saving in local */
   TtaNewForm (BaseDialog + FormSauver, TtaGetViewFrame (document, view), 0, 0,
	       TtaGetMessage (AMAYA, AM_SAVE_LOCAL), TRUE, 2, 'L', D_DONE);
   TtaListDirectory (DirectoryName, BaseDialog + FormSauver,
		     TtaGetMessage (LIB, DOC_DIR),	/* std thot msg */
		     BaseDialog + SauvDir, "",
		     TtaGetMessage (AMAYA, AM_FILES), BaseDialog + SauvDoc);
   /* TtaGetMessage(LIB, DOCUMENT_NAME) */
   TtaNewTextForm (BaseDialog + SauvNom, BaseDialog + FormSauver,
		   TtaGetMessage (AMAYA, AM_OBJECT_LOCATION), 50, 1, TRUE);
   TtaSetTextForm (BaseDialog + SauvNom, pathname);
   TtaExtractName (pathname, tempdir, ObjectName);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseDialog + FormSauver, FALSE);
}

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


   if (!dst_is_local) {
      res = PutObjectWWW (SavingObject, tempSavedObject, tempfile,
			  AMAYA_SYNC, (TTcbf *) NULL, (void *) NULL);

      if (res != HT_OK) {
	 TtaSetDialoguePosition ();
	 TtaShowDialogue (BaseDialog + FormSauver, FALSE);
	 return;
      }
      TtaDestroyDialogue (BaseDialog + FormSauver);
      SavingObject = (Document) None;
      SavingDocument = (Document) None;
      return;
   }
   if (FileExist (tempfile)) {
      /* ask confirmation */
      sprintf (msg, TtaGetMessage (LIB, FILE_EXIST), tempfile);
      InitConfirm (SavingObject, 1, msg);
      if (!UserAnswer) {
	 /* the user has to change the name of the saving file */
	 TtaSetDialoguePosition ();
	 TtaShowDialogue (BaseDialog + FormSauver, FALSE);
	 return;
      }
   }
   ThotCopyFile (tempSavedObject, tempfile);
   SavingObject = (Document) None;
   TtaDestroyDialogue (BaseDialog + FormSauver);
}
