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
 *         R. Guetari (W3C/INRIA) Windows routines.
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
#include "HTMLactions_f.h"
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
  Element            el, elStyle, parent;
  Element            first, last;
  ElementType	     elType, parentType;
  LoadedImageDesc   *desc;
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
		  else
		    {
		      /* style is not allowed in Head section */
		      if (elType.ElTypeNum == HTML_EL_HEAD)
			parent = el;
		      else
			{
			  parentType.ElSSchema = elType.ElSSchema;
			  parentType.ElTypeNum = HTML_EL_HEAD;
			  parent = TtaGetTypedAncestor (el, parentType);
			}

		      if (parent != NULL)
			{
			  TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_INVALID_TARGET), NULL);
			  return;
			}
		      else
			{
			  /* style is not allowed in MAP */
			  if (elType.ElTypeNum == HTML_EL_MAP)
			    parent = el;
			  else
			    {
			      parentType.ElTypeNum = HTML_EL_MAP;
			      parent = TtaGetTypedAncestor (el, parentType);
			    }
			  if (parent != NULL)
			    {
			      TtaSetStatus (document, 1, TtaGetMessage (AMAYA, AM_INVALID_TARGET), NULL);
			      return;
			    }
			  else
			    last = el;
			}

		      /* TODO:  TtaGiveLastSelectedElement (document, &last, &i, &cN); */
		      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
			  elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
			{
			  el = TtaGetParent (el);
			  /* if the PRule is on a Pseudo-Paragraph,
			     move it to the enclosing element */
			  elType = TtaGetElementType (el);
			}
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
		  if (IsHTTPPath (DocumentURLs[document]) && !IsHTTPPath (LastURLImage))
		    {
		      /*
			load a local image into a remote document 
			copy image file into the temporary directory of the document
			*/
		      TtaExtractName (LastURLImage, tempfile, tempname);
		      NormalizeURL (tempname, document, tempfile, tempname, NULL);
		      AddLoadedImage (tempname, tempfile, document, &desc);
		      desc->status = IMAGE_MODIFIED;
		      TtaFileCopy (LastURLImage, desc->localName);
		      HTMLSetBackgroundImage (document, el, i, tempname);
		    }
		  else
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
   strcpy(ImgFilter, ".gif");
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
#  ifndef _WINDOWS
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
#  else /* _WINDOWS */
   CreateOpenDocDlgWindow (TtaGetViewFrame (document, view), LastURLImage, BaseImage, FormImage) ;

#  endif /* _WINDOWS */
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
   char*               s = (char*) TtaGetMemory (MAX_LENGTH * sizeof (char)); 
   int                 i;

   /* there is a selection */
   /* Dialogue form for open URL or local */
#  ifndef _WINDOWS
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
   TtaFreeMemory (s);
#  else /* _WINDOWS */
   if (LastURLImage[0] != EOS)
      strcpy (s, LastURLImage);
   else {
      strcpy (s, DirectoryImage);
      strcat (s, DIR_STR);
      strcat (s, ImageName);
   }
   BgDocument = document;
   CreateBackgroundImageDlgWindow (TtaGetViewFrame (document, view), BaseImage, FormBackground, ImageURL, ImageLabel, ImageDir, ImageSel, RepeatImage, s);
#  endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   ComputeSRCattribute computes the SRC attribute of the image.
   text is the image name (relative or not) and sourceDocument is the
   source document where the image comes from.
   el is the target picture element and doc the target document.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ComputeSRCattribute (Element el, Document doc, Document sourceDocument, Attribute attr, char *text)
#else  /* __STDC__ */
void                ComputeSRCattribute (el, doc, sourceDocument, attr, text)
Element             el;
Document            doc;
Document            sourceDocument;
Attribute           attr;
char               *text;

#endif /* __STDC__ */
{
  char              *value, *base;
  char               pathimage[MAX_LENGTH];
  char               localname[MAX_LENGTH];
  char               imagename[MAX_LENGTH];
  LoadedImageDesc   *desc;

  /* get the absolute URL of the image */
  NormalizeURL (text, doc, pathimage, imagename, NULL);
  if (IsHTTPPath (DocumentURLs[doc]))
    {
      /* remote target document */
      if (!IsHTTPPath (pathimage))
	{
	  /* load a local image into a remote document */
	  /* copy image file into the temporary directory of the document */
	  TtaExtractName (pathimage, localname, imagename);
	  NormalizeURL (imagename, doc, localname, imagename, NULL);
	  AddLoadedImage (imagename, localname, doc, &desc);
	  desc->status = IMAGE_MODIFIED;
	  TtaFileCopy (pathimage, desc->localName);
	  
	  /* suppose that the image will be stored in the same directory */
	  TtaSetAttributeText (attr, imagename, el, doc);

	  /* set contents of the picture element */
	  TtaSetTextContent (el, desc->localName, SPACE, doc);
	  DisplayImage (doc, el, desc->localName);
	}
      else
	{
	  /* load a remote image into a remote document */
	  base = GetBaseURL (doc);
	  value = MakeRelativeURL (pathimage, base);
	  TtaSetAttributeText (attr, value, el, doc);
	  TtaFreeMemory (base);
	  TtaFreeMemory (value);
	  /* set stop button */
	  ActiveTransfer (doc);
	  FetchImage (doc, el, NULL, 0, NULL, NULL);
	  ResetStop (doc);
	}
    }
  else
    {
      /* local target document */
      if (!IsHTTPPath (pathimage))
	{
	  /* load a local image into a local document */
	  base = GetBaseURL (doc);
	  value = MakeRelativeURL (pathimage, base);
	  TtaSetAttributeText (attr, value, el, doc);
	  TtaFreeMemory (base);
	  TtaFreeMemory (value);
	  /* set the element content */
	  TtaSetTextContent (el, pathimage, SPACE, doc);
	}
      else
	{
	  /* load a remote image into a local document */
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
  AttributeType      attrType;
  Attribute          attr;
  Element            elSRC, el;
  Document           doc;
  char              *text;
  char              *pathimage;
  char              *imagename;

   /* Select an image name */
   el = event->element;
   doc = event->document;
   text = GetImageURL (doc, 1);
   if (text == NULL || text[0] == EOS)
     {
	/* delete the empty SRC element */
	TtaDeleteTree (el, doc);
	return;
     }
   /* search the SRC attribute */
   attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
   attrType.AttrTypeNum = HTML_ATTR_SRC;
   elSRC = TtaGetParent (el);
   if (elSRC != NULL)
      elSRC = el;
   attr = TtaGetAttribute (elSRC, attrType);
   if (attr == 0)
     {
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (elSRC, attr, doc);
     }
   ComputeSRCattribute (elSRC, doc, 0, attr, text);
   /* add the ALT attribute */
   attrType.AttrTypeNum = HTML_ATTR_ALT;
   attr = TtaGetAttribute (elSRC, attrType);
   if (attr == 0)
     {
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (elSRC, attr, doc);
     }
   /* copy image name in ALT attribute */
   imagename = (char*) TtaGetMemory (MAX_LENGTH);
   pathimage = (char*) TtaGetMemory (MAX_LENGTH);
   strcpy (imagename, " ");
   TtaExtractName (text, pathimage, &imagename[7]);
   strcat (imagename, " ");
   TtaSetAttributeText (attr, imagename, elSRC, doc);
   TtaFreeMemory (pathimage);
   TtaFreeMemory (imagename);
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
   char               *buf1, *buf2;
   char               *localname, *imageName;
   LoadedImageDesc   *desc;

   doc = event->document;
   el = event->element;
   attr = event->attribute;
   /* get a buffer for the attribute value */
   length = MAX_LENGTH;
   buf1 = TtaGetMemory (length);
   buf2 = TtaGetMemory (length);
   imageName = TtaGetMemory (length);
   /* copy the SRC attribute into the buffer */
   TtaGiveTextAttributeValue (attr, buf1, &length);
   NormalizeURL (buf1, doc, buf2, imageName, NULL);
   /* extract image name from full name */
   TtaExtractName (buf2, buf1, imageName);
   if (strlen (imageName) != 0)
     {
       if (IsHTTPPath(buf2))
	 {
	   /* remote image */
	   localname = GetLocalPath (doc, buf2);
	   /* load a remote image into a remote document */
	   TtaSetTextContent (el, localname, SPACE, doc);
	   TtaFreeMemory (localname);
	   ActiveTransfer (doc);
	   FetchImage (doc, el, NULL, 0, NULL, NULL);
	   ResetStop (doc);
	 }
       else
	 {
	   /* local image */
	   if (IsHTTPPath (DocumentURLs[doc]))
	     {
	       NormalizeURL (imageName, doc, buf1, imageName, NULL);
	       /* load a local image into a remote document */
	       AddLoadedImage (imageName, buf1, doc, &desc);
	       desc->status = IMAGE_MODIFIED;
	       TtaFileCopy (buf2, desc->localName);
	       TtaSetTextContent (el, desc->localName, SPACE, doc);
	     }
	   else
	     {
	       /* load a local image into a local document */
	       TtaSetTextContent (el, buf2, SPACE, doc);
	     }
	 }
     }
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
  char               *localname;

  *desc = NULL;
  if (!TtaFileExist (fullname))
    return (FALSE);
  else if (url == NULL || name == NULL)
    return (FALSE);
  else if (!IsHTTPPath (url))
    /* it is a local image - nothing to do */
    return (FALSE);
  else
    {
      /* It is an image loaded from the Web */
      localname = GetLocalPath (doc, url);

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

      /* copy the image in place */
      TtaFileCopy (fullname, localname);
      /* add a new identifier to the list if necessary */
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
      TtaFreeMemory (localname);
      return (TRUE);
    }
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
   ElemImage          *ctxEl, *ctxPrev;

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
	     if (pImage->elImage)
	       {
		 ctxEl = pImage->elImage;
		 pImage->elImage = NULL;
		 while (ctxEl != NULL)
		   {
		     ctxPrev = ctxEl;
		     ctxEl = ctxEl->nextElement;
		     TtaFreeMemory ( ctxPrev);
		   }
	       }
	     /* set up the image descriptors link */
	     if (previous != NULL)
		previous->nextImage = next;
	     else
		ImageURLs = next;
	     if (next != NULL)
		next->prevImage = previous;
	     TtaFreeMemory ((char*) pImage);
	     pImage = previous;
	  }
	/* next descriptor */
	previous = pImage;
	pImage = next;
     }
}
