/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya editing functions called form Thot and declared in HTML.A
 * These functions concern Image elements.
 *
 * Author: I. Vatton
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "presentdriver.h"

#define ImageURL	1
#define ImageLabel	2
#define ImageDir	3
#define ImageSel	4
#define ImageFilter     5
#define FormImage	6
#define RepeatImage	7
#define FormBackground	8
#define IMAGE_MAX_REF	9

static Document     BgDocument;
static int          BaseImage;
static int          RepeatValue;
static char         DirectoryImage[MAX_LENGTH];
static char         LastURLImage[MAX_LENGTH];
static char         ImageName[MAX_LENGTH];
static char         ImgFilter[NAME_LENGTH];
#include "AHTURLTools_f.h"
#include "HTMLedit_f.h"
#include "EDITimage_f.h"
#include "HTMLimage_f.h"
#include "HTMLpresentation_f.h"
#include "HTMLstyle_f.h"
#include "init_f.h"
#include "html2thot_f.h"


/*----------------------------------------------------------------------
   CallbackImage manage returns of Picture form.                   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CallbackImage (int ref, int typedata, char *data)
#else  /* __STDC__ */
void                CallbackImage (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif /* __STDC__ */
{
  Document           document;
  ElementType	     elType;
  Element            el, elStyle;
  Element            first, last;
  char               tempfile[MAX_LENGTH];
  char               tempname[MAX_LENGTH];
  int                i, c1;
  int                val;
  boolean            change;

  val = (int) data;
  switch (ref - BaseImage)
    {
    case FormImage:
    case FormBackground:
      /* *********Load URL or local image********* */
      if (val == 2)
	{
	  /* Clear */
	  LastURLImage[0] = EOS;
	  TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
	}
      else if (val == 3)
	/* Parse */
	{
	  /* reinitialize directories and document lists */
	  TtaListDirectory (DirectoryImage, ref,
			    TtaGetMessage (LIB, TMSG_DOC_DIR), BaseImage + ImageDir,
			    ImgFilter, TtaGetMessage (AMAYA, AM_FILES), BaseImage + ImageSel);
	}
      else
	{
	  if (val == 0)
	    {
	      LastURLImage[0] = EOS;
	      TtaDestroyDialogue (ref);
	      BgDocument = 0;
	    }
	  else if (ref == BaseImage + FormBackground && BgDocument != 0)
	    {
	      /* save BgDocument because operation can be too long */
	      document = BgDocument;
	      /* get the first and last selected element */
	      TtaGiveFirstSelectedElement (document, &first, &c1, &i);
	      if (first == NULL)
		{
		  /* set the pRule on the root element */
		  el =  TtaGetMainRoot (document);
		  elType.ElSSchema = TtaGetDocumentSSchema (document);
		  elType.ElTypeNum = HTML_EL_BODY;
		  /* set the style on body element */
		  elStyle = TtaSearchTypedElement (elType, SearchInTree, el);
		  last = el;
		}
	      else
		{
		  elStyle = el = first;
		  elType = TtaGetElementType (el);
		  if (elType.ElTypeNum == HTML_EL_BODY)
		    {
		      /* move the pRule on the root element */
		      el =  TtaGetMainRoot (document);
		      last = el;
		    }
		  else if (elType.ElTypeNum == HTML_EL_HEAD)
		    {
		      /* set the style on body element */
		      elStyle = TtaSearchTypedElement (elType, SearchInTree, el);
		      last = el;
		    }
		  else
		    {
		      /* TODO:  TtaGiveLastSelectedElement (document, &last, &i, &cN); */
		      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
			  elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
			el = TtaGetParent (el);
		      /* if the PRule is on a Pseudo-Paragraph, move it to the enclosing
			 element */
		      elType = TtaGetElementType (el);
		      if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
			el = TtaGetParent (el);
		      elStyle = el;
		      last = el;
		    }
		}

	      if (LastURLImage[0] == EOS)
		HTMLResetBackgroundImage (document, el);
	      else
		{
		  if (RepeatValue == 0)
		    i = DRIVERP_REPEAT;
		  else if (RepeatValue == 1)
		    i = DRIVERP_HREPEAT;
		  else if (RepeatValue == 2)
		    i = DRIVERP_VREPEAT;
		  else
		    i = DRIVERP_SCALE;
		  HTMLSetBackgroundImage (document, el, i, LastURLImage);
		}
	      SetStyleAttribute (document, elStyle);
	      TtaSetDocumentModified (document);
	    }
	  else
	    TtaDestroyDialogue (ref);
	}
      break;
    case RepeatImage:
      RepeatValue = val;
      break;
    case ImageFilter:
      /* Filter value */
      if (strlen(data) <= NAME_LENGTH)
	strcpy (ImgFilter, data);
      else
	TtaSetTextForm (BaseImage + ImageFilter, ImgFilter);
      break;
    case ImageURL:
      if (data == NULL)
	break;
      if (IsW3Path (data))
	{
	  /* save the URL name */
	  strcpy (LastURLImage, data);
	  ImageName[0] = EOS;
	}
      else
	{
	  change = NormalizeFile (data, LastURLImage);
	  if (change)
	    TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
	  if (TtaCheckDirectory (LastURLImage))
	    {
	      strcpy (DirectoryImage, LastURLImage);
	      ImageName[0] = EOS;
	      LastURLImage[0] = EOS;
	    }
	  else
	    TtaExtractName (LastURLImage, DirectoryImage, ImageName);
	}
      break;
    case ImageDir:
      if (!strcmp (data, ".."))
	{
	  /* suppress last directory */
	  strcpy (tempname, DirectoryImage);
	  TtaExtractName (tempname, DirectoryImage, tempfile);
	}
      else
	{
	  strcat (DirectoryImage, DIR_STR);
	  strcat (DirectoryImage, data);
	}
      TtaSetTextForm (BaseImage + ImageURL, DirectoryImage);
      TtaListDirectory (DirectoryImage, BaseImage + FormImage,
			TtaGetMessage (LIB, TMSG_DOC_DIR), BaseImage + ImageDir,
			ImgFilter, TtaGetMessage (AMAYA, AM_FILES), BaseImage + ImageSel);
      TtaListDirectory (DirectoryImage, BaseImage + FormBackground,
			TtaGetMessage (LIB, TMSG_DOC_DIR), BaseImage + ImageDir,
			ImgFilter, TtaGetMessage (AMAYA, AM_FILES), BaseImage + ImageSel);
      ImageName[0] = EOS;
      break;
    case ImageSel:
      if (DirectoryImage[0] == EOS)
	{
	  /* set path on current directory */
	  getcwd (DirectoryImage, MAX_LENGTH);
	}
      /* construct the image full name */
      strcpy (LastURLImage, DirectoryImage);
      val = strlen (LastURLImage) - 1;
      if (LastURLImage[val] != DIR_SEP)
	strcat (LastURLImage, DIR_STR);
      strcat (LastURLImage, data);
      TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
      break;
    default:
      break;
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitImage (void)
#else  /* __STDC__ */
void                InitImage ()
#endif				/* __STDC__ */
{
   BaseImage = TtaSetCallback (CallbackImage, IMAGE_MAX_REF);
   RepeatValue = 0;
   LastURLImage[0] = EOS;
   strcpy(ImgFilter, ".png");
   /* set path on current directory */
   getcwd (DirectoryImage, MAX_LENGTH);

}


/*----------------------------------------------------------------------
   GetImageURL initializes the Picture form                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
char               *GetImageURL (Document document, View view)
#else  /* __STDC__ */
char               *GetImageURL (Document document, View view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   int                 i;
   char                s[MAX_LENGTH];

   /* Dialogue form for open URL or local */
   i = 0;
   strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));

   TtaNewSheet (BaseImage + FormImage, TtaGetViewFrame (document, view),  TtaGetMessage (AMAYA, AM_OPEN_URL),
		3, s, TRUE, 2, 'L', D_CANCEL);
   TtaNewTextForm (BaseImage + ImageURL, BaseImage + FormImage,
		   TtaGetMessage (AMAYA, AM_OPEN_URL), 50, 1, TRUE);
   TtaNewLabel (BaseImage + ImageLabel, BaseImage + FormImage, " ");
   TtaListDirectory (DirectoryImage, BaseImage + FormImage,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),
		     BaseImage + ImageDir, ImgFilter,
		     TtaGetMessage (AMAYA, AM_FILES), BaseImage + ImageSel);
   if (LastURLImage[0] != EOS)
      TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
   else
     {
	strcpy (s, DirectoryImage);
	strcat (s, DIR_STR);
	strcat (s, ImageName);
	TtaSetTextForm (BaseImage + ImageURL, s);
     }

   TtaNewTextForm (BaseImage + ImageFilter, BaseImage + FormImage,
		   TtaGetMessage (AMAYA, AM_PARSE), 10, 1, TRUE);
   TtaSetTextForm (BaseImage + ImageFilter, ImgFilter);
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseImage + FormImage, FALSE);
   TtaWaitShowDialogue ();
   return (LastURLImage);
}


/*----------------------------------------------------------------------
  ChangeBackgroundImage displays a form
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void ChangeBackgroundImage (Document document, View view)
#else /* __STDC__*/
void ChangeBackgroundImage (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
   char                s[MAX_LENGTH];
   int                 i;

   /* there is a selection */
   /* Dialogue form for open URL or local */
   i = 0;
   strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));

   TtaNewSheet (BaseImage + FormBackground, TtaGetViewFrame (document, view),  TtaGetMessage (AMAYA, AM_OPEN_URL),
		3, s, TRUE, 2, 'L', D_CANCEL);
   TtaNewTextForm (BaseImage + ImageURL, BaseImage + FormBackground,
		   TtaGetMessage (AMAYA, AM_OPEN_URL), 50, 1, TRUE);
   TtaNewLabel (BaseImage + ImageLabel, BaseImage + FormBackground, " ");
   TtaListDirectory (DirectoryImage, BaseImage + FormBackground,
		     TtaGetMessage (LIB, TMSG_DOC_DIR),
		     BaseImage + ImageDir, ImgFilter,
		     TtaGetMessage (AMAYA, AM_FILES), BaseImage + ImageSel);
   if (LastURLImage[0] != EOS)
      TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
   else
     {
	strcpy (s, DirectoryImage);
	strcat (s, DIR_STR);
	strcat (s, ImageName);
	TtaSetTextForm (BaseImage + ImageURL, s);
     }

   TtaNewTextForm (BaseImage + ImageFilter, BaseImage + FormBackground,
		   TtaGetMessage (AMAYA, AM_PARSE), 10, 1, TRUE);
   TtaSetTextForm (BaseImage + ImageFilter, ImgFilter);
   /* selector for repeat mode */
   i = 0;
   sprintf (&s[i], "%s%s", "B", TtaGetMessage (AMAYA, AM_REPEAT));
   i += strlen (&s[i]) + 1;
   sprintf (&s[i], "%s%s", "B", TtaGetMessage (AMAYA, AM_REPEAT_X));
   i += strlen (&s[i]) + 1;
   sprintf (&s[i], "%s%s", "B", TtaGetMessage (AMAYA, AM_REPEAT_Y));
   i += strlen (&s[i]) + 1;
   sprintf (&s[i], "%s%s", "B", TtaGetMessage (AMAYA, AM_NO_REPEAT));
   TtaNewSubmenu (BaseImage + RepeatImage, BaseImage + FormBackground, 0,
		  TtaGetMessage (AMAYA, AM_REPEAT_MODE), 4, s, NULL, FALSE);
   TtaSetMenuForm (BaseImage + RepeatImage, RepeatValue);
   /* save the document concerned */
   BgDocument = document;
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseImage + FormBackground, TRUE);
}


/*----------------------------------------------------------------------
   ComputeSRCattribute  computes the SRC attribute value.		
   		Get text as the proposed value for SRCattribute.	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ComputeSRCattribute (Element el, Document doc, Document originDocument, Attribute attr, char *text)
#else  /* __STDC__ */
void                ComputeSRCattribute (el, doc, originDocument, attr, text)
Element             el;
Document            doc;
Document            originDocument;
Attribute           attr;
char               *text;

#endif /* __STDC__ */
{
   char                name[MAX_LENGTH];
   char                pathdoc[MAX_LENGTH];
   char                pathimage[MAX_LENGTH];
   char                localname[MAX_LENGTH];
   char                imagename[MAX_LENGTH];
   LoadedImageDesc    *desc;

   NormalizeURL (text, originDocument, pathimage, name);
   /* copy the text into the SRC attribute */
   if (IsHTTPPath (DocumentURLs[doc]))
     {
	if (!IsHTTPPath (pathimage))
	  {
	     /* try to load a local image within a remote document */
	     /* copy image file into the temporary directory of the document */
	     TtaExtractName (pathimage, localname, imagename);
	     NormalizeURL (imagename, doc, localname, imagename);
	     AddLoadedImage (imagename, localname, doc, &desc);
	     desc->status = IMAGE_MODIFIED;
	     /* JK: was name, seems it should be pathimage */
	     TtaFileCopy (pathimage, desc->localName);

	     TtaExtractName (DocumentURLs[doc], pathdoc, name);
	     TtaExtractName (imagename, pathimage, name);
	     if (!strcmp (pathimage, pathdoc))
		/* convert absolute SRC into local */
		TtaSetAttributeText (attr, name, el, doc);
	     else
		TtaSetAttributeText (attr, imagename, el, doc);

	     /* set contents of the picture element */
	     TtaSetTextContent (el, desc->localName, SPACE, doc);
	     DisplayImage (doc, el, desc->localName);
	  }
	else
	  {
	     /* load from the Web */
	     /* set stop button */
	     ActiveTransfer (doc);
	     TtaSetAttributeText (attr, pathimage, el, doc);
	     FetchImage (doc, el, NULL, 0, NULL, NULL);
	     ResetStop (doc);
	  }
     }
   else
     {
	TtaSetAttributeText (attr, pathimage, el, doc);
	if (!IsHTTPPath (pathimage))
	  {
	     /* set the element content */
	     TtaSetTextContent (el, pathimage, SPACE, doc);
	     DisplayImage (doc, el, pathimage);
	  }
	else
	  {
	     /* set stop button */
	     ActiveTransfer (doc);
	     TtaSetAttributeText (attr, pathimage, el, doc);
	     FetchImage (doc, el, NULL, 0, NULL, NULL);
	     ResetStop (doc);
	  }
     }

}

/*----------------------------------------------------------------------
   UpdateSRCattribute  creates or updates the SRC attribute value	
   		when the contents of element IMG is set.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateSRCattribute (NotifyElement * event)
#else  /* __STDC__ */
void                UpdateSRCattribute (event)
NotifyElement      *event;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attrSRC;
   Element             elSRC, el;
   char               *text;
   Document            doc;

   /* Select an image name */
   el = event->element;
   doc = event->document;
   text = GetImageURL (doc, 1);
   if (text == NULL || text[0] == EOS)
     {
	/* JK: remove the empty SRC element */
	TtaRemoveTree (el, doc);
	return;
     }
   /* search the SRC attribute */
   attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
   attrType.AttrTypeNum = HTML_ATTR_SRC;
   elSRC = TtaGetParent (el);
   if (elSRC != NULL)
      elSRC = el;
   attrSRC = TtaGetAttribute (elSRC, attrType);
   if (attrSRC == 0)
     {
	attrSRC = TtaNewAttribute (attrType);
	TtaAttachAttribute (elSRC, attrSRC, doc);
     }
   ComputeSRCattribute (elSRC, doc, 0, attrSRC, text);
}


/*----------------------------------------------------------------------
   SRCattrModified updates the contents of element IMG according   
   to the new value of attribute SRC.                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SRCattrModified (NotifyAttribute * event)
#else  /* __STDC__ */
void                SRCattrModified (event)
NotifyAttribute    *event;

#endif /* __STDC__ */
{
   Element             el;
   Attribute           attr;
   Document            doc;
   int                 length;
   char               *buf1, *buf2, *imageName;

   doc = event->document;
   el = event->element;
   attr = event->attribute;
   /* get a buffer for the attribute value */
   length = TtaGetTextAttributeLength (attr);
   buf1 = TtaGetMemory (length + 1);
   buf2 = TtaGetMemory (length + 1);
   imageName = TtaGetMemory (length + 1);
   /* copy the SRC attribute into the buffer */
   TtaGiveTextAttributeValue (attr, buf1, &length);
   /* extract image name from full name */
   TtaExtractName (buf1, buf2, imageName);
   if (strlen (imageName) == 0)
      /* full names ends with ''/ */
      TtaExtractName (buf2, buf1, imageName);
   if (strlen (imageName) != 0)
      TtaSetTextContent (el, imageName, TtaGetDefaultLanguage (), doc);
   TtaFreeMemory (buf1);
   TtaFreeMemory (buf2);
   TtaFreeMemory (imageName);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateImage (Document document, View view)
#else  /* __STDC__ */
void                CreateImage (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetDocumentSSchema (document);
   elType.ElTypeNum = HTML_EL_PICTURE_UNIT;
   TtaCreateElement (elType, document);
}

/*----------------------------------------------------------------------
   AddLocalImage adds a new local image into image descriptor table   
   with the purpose of having it saved through the Net later.    
   This function copy the image in the TempFileDirectory.        
   fullname is the complete path to the local file.              
   name is the name of the local file.                           
   url is the complete URL of the distant location.              
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             AddLocalImage (char *fullname, char *name, char *url, Document doc, LoadedImageDesc ** desc)
#else  /* __STDC__ */
boolean             AddLocalImage (fullname, name, url, doc, desc)
char               *fullname;
char               *name;
char               *url;
Document            doc;
LoadedImageDesc   **desc;

#endif /* __STDC__ */
{
   LoadedImageDesc    *pImage, *previous;
   char                localname[MAX_LENGTH];

   if (!TtaFileExist (fullname))
      return (FALSE);

   *desc = NULL;
   if (url == NULL || name == NULL)
      return (FALSE);
   else if (IsHTTPPath (url))
     {
	/* It is an image loaded from the Web */
	sprintf (localname, "%s%s%d%s", TempFileDirectory, DIR_STR, doc, DIR_STR);
	strcat (localname, name);
     }
   else
      /* it is a local image */
      return (FALSE);		/* nothing to do */

   pImage = ImageURLs;
   previous = NULL;
   while (pImage != NULL)
     {
	if ((pImage->document == doc) &&
	    (strcmp (url, pImage->originalName) == 0))
	  {
	     /* image already loaded */
	     *desc = pImage;
	     break;
	  }
	else
	  {
	     /* see the next descriptor */
	     previous = pImage;
	     pImage = pImage->nextImage;
	  }
     }

   /*
    * copy the image in place.
    */
   TtaFileCopy (fullname, localname);

   /*
    * add a new identifier to the list if necessary.
    */
   if (pImage == NULL)
     {
	/* It is a new loaded image */
	pImage = (LoadedImageDesc *) TtaGetMemory (sizeof (LoadedImageDesc));
	pImage->originalName = TtaGetMemory (strlen (url) + 1);
	strcpy (pImage->originalName, url);
	pImage->localName = TtaGetMemory (strlen (localname) + 1);
	strcpy (pImage->localName, localname);
	pImage->prevImage = previous;
	if (previous != NULL)
	   previous->nextImage = pImage;
	else
	   ImageURLs = pImage;
	pImage->nextImage = NULL;
	pImage->document = doc;
	pImage->elImage = NULL;
     }
   pImage->status = IMAGE_MODIFIED;
   *desc = pImage;
   return (TRUE);
}

/*----------------------------------------------------------------------
   RemoveDocumentImages removes loaded images of the document.        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                RemoveDocumentImages (Document doc)
#else  /* __STDC__ */
void                RemoveDocumentImages (doc)
Document            doc;

#endif /* __STDC__ */
{
   LoadedImageDesc    *pImage, *previous, *next;

   pImage = ImageURLs;
   previous = NULL;
   if (doc == (Document) None)
      return;			/* nothing to do */

   while (pImage != NULL)
     {
	next = pImage->nextImage;
	/* does the current image belong to the document ? */
	if (pImage->document == doc)
	  {
	     pImage->status = IMAGE_NOT_LOADED;
	     /* remove the image */
	     TtaFileUnlink (pImage->localName);
	     /* free the descriptor */
	     if (pImage->originalName != NULL)
		TtaFreeMemory (pImage->originalName);
	     if (pImage->localName != NULL)
		TtaFreeMemory (pImage->localName);

	     /* set up the image descriptors link */
	     if (previous != NULL)
		previous->nextImage = next;
	     else
		ImageURLs = next;
	     if (next != NULL)
		next->prevImage = previous;
	     TtaFreeMemory ((char *) pImage);
	     pImage = previous;
	  }
	/* next descriptor */
	previous = pImage;
	pImage = next;
     }
}

/*----------------------------------------------------------------------
   	UpdateImageMap sets or updates Ref_IMG MAP attributes for the	
   		    current image.					
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateImageMap (Element image, Document document)
#else  /* __STDC__ */
void                UpdateImageMap (image, document)
Element             image;
Document            document;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   Element             el, child;
   char               *text;
   int                 shape, w, h, length;
   DisplayMode         dispMode;

   dispMode = TtaGetDisplayMode (document);
   /* Search the USEMAP attribute */
   attrType.AttrSSchema = TtaGetDocumentSSchema (document);
   attrType.AttrTypeNum = HTML_ATTR_USEMAP;
   attr = TtaGetAttribute (image, attrType);
   if (attr != NULL)
     {
	/* ask Thot to stop displaying changes made in the document */
       if (dispMode == DisplayImmediately)
	 TtaSetDisplayMode (document, DeferredDisplay);

	/* Search the MAP element associated with IMAGE element */
	length = TtaGetTextAttributeLength (attr);
	length++;
	text = TtaGetMemory (length);
	TtaGiveTextAttributeValue (attr, text, &length);
	if (text[0] == '#')
	   el = SearchNAMEattribute (document, &text[1], NULL);
	else
	   el = NULL;
	TtaFreeMemory (text);
	if (el == NULL)
	   return;

	/* Update MAP attribute */
	attrType.AttrTypeNum = HTML_ATTR_Ref_IMG;
	attr = TtaGetAttribute (el, attrType);
	if (attr == NULL)
	  {
	     /* create it */
	     attr = TtaNewAttribute (attrType);
	     TtaAttachAttribute (el, attr, document);
	  }
	TtaSetAttributeReference (attr, el, document, image, document);

	/* Update AREAs attribute */
	el = TtaGetFirstChild (el);
	TtaGiveBoxSize (image, document, 1, UnPixel, &w, &h);
	while (el != NULL)
	  {
	     /* Search the shape attribute */
	     attrType.AttrTypeNum = HTML_ATTR_shape;
	     attr = TtaGetAttribute (el, attrType);
	     if (attr != NULL)
	       {
		  shape = TtaGetAttributeValue (attr);
		  if (shape == HTML_ATTR_shape_VAL_polygon)
		    {
		       attrType.AttrTypeNum = HTML_ATTR_AreaRef_IMG;
		       attr = TtaGetAttribute (el, attrType);
		       if (attr == NULL)
			 {
			    /* create it */
			    attr = TtaNewAttribute (attrType);
			    TtaAttachAttribute (el, attr, document);
			 }
		       TtaSetAttributeReference (attr, el, document, image, document);

		       if (shape == HTML_ATTR_shape_VAL_polygon)
			 {
			    child = TtaGetFirstChild (el);
			    TtaChangeLimitOfPolyline (child, UnPixel, w, h, document);
			 }
		    }
	       }
	     TtaNextSibling (&el);
	  }
     }

   /* ask Thot to display changes made in the document */
   TtaSetDisplayMode (document, dispMode);
}
