/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya browser functions called form Thot and declared in HTML.A
 * These functions concern Image elements.
 *
 * Author: I. Vatton
 *
 */

/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"


#include "init_f.h"
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
#include "query_f.h"
#endif
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "EDITORactions_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"

/* the structure used for storing the context of the 
   FetchAndDisplayImages_callback function */
typedef struct _FetchImage_context {
  STRING base_url;
  LoadedImageDesc    *desc;
} FetchImage_context;


/*----------------------------------------------------------------------
   AddLoadedImage adds a new image into image descriptor table.	
   Local images are not stored into this table and the function    
   returns no descriptor and the value FALSE.                      
   If the remote image is new, adds a descriptor for this image    
   notes its local name and returns TRUE.                          
   For already loaded remote images the function returns the      
   descriptor entry and the value FALSE.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             AddLoadedImage (STRING name, STRING pathname, Document doc, LoadedImageDesc ** desc)
#else  /* __STDC__ */
boolean             AddLoadedImage (name, pathname, doc, desc)
STRING              name;
STRING              pathname;
Document            doc;
LoadedImageDesc   **desc;

#endif /* __STDC__ */
{
   LoadedImageDesc    *pImage, *previous, *sameImage;
   STRING               localname;

   *desc = NULL;
   sameImage = NULL;
   if (pathname == NULL || name == NULL)
      return (FALSE);
   else if (IsHTTPPath (pathname))
     localname = GetLocalPath (doc, pathname);
   else
      /* it is a local image */
      return (FALSE);		/* nothing to do */

   pImage = ImageURLs;
   previous = NULL;
   while (pImage != NULL)
     {
	if (ustrcmp (pathname, pImage->originalName) == 0)
	  {
	     /* image already loaded */
	     sameImage = pImage;
	     if (pImage->document == doc)
	       {
		  *desc = pImage;
		  TtaFreeMemory (localname);
		  return (FALSE);
	       }
	     else
	       {
		  /* see the next descriptor */
		  previous = pImage;
		  pImage = pImage->nextImage;
	       }
	  }
	else
	  {
	     /* see the next descriptor */
	     previous = pImage;
	     pImage = pImage->nextImage;
	  }
     }

   /* It is a new loaded image */
   pImage = (LoadedImageDesc *) TtaGetMemory (sizeof (LoadedImageDesc));
   pImage->originalName = TtaGetMemory (ustrlen (pathname) + 1);
   ustrcpy (pImage->originalName, pathname);
   pImage->localName = TtaGetMemory (ustrlen (localname) + 1);
   ustrcpy (pImage->localName, localname);
   pImage->prevImage = previous;
   if (previous != NULL)
      previous->nextImage = pImage;
   else
      ImageURLs = pImage;
   pImage->nextImage = NULL;
   pImage->document = doc;
   pImage->elImage = NULL;
   pImage->imageType = unknown_type;
   *desc = pImage;
   TtaFreeMemory (localname);
   if (sameImage != NULL)
     {
	/* the image file exist for a different document */
	pImage->status = IMAGE_LOADED;
	TtaFileCopy (sameImage->localName, pImage->localName);
	return (FALSE);
     }
   else
     {
	pImage->status = IMAGE_NOT_LOADED;
	return (TRUE);
     }
}


/*----------------------------------------------------------------------
   SearchLoadedImage searches the image descriptor of a loaded image using
   its local name.
   The function returns the descriptor entry or NULL.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
LoadedImageDesc    *SearchLoadedImage (STRING localpath, Document doc)
#else  /* __STDC__ */
LoadedImageDesc    *SearchLoadedImage (localpath, doc)
STRING              localpath;
Document            doc;
#endif /* __STDC__ */
{
  LoadedImageDesc    *pImage;
  
  if (localpath == NULL)
    return (NULL);
  else
    {
      pImage = ImageURLs;
      while (pImage != NULL)
	{
	  if (ustrcmp (localpath, pImage->localName) == 0 && 
	      ((doc == 0) || (pImage->document == doc)))
	    /* image found */
	    return (pImage);
	  else
	    /* see the next descriptor */
	    pImage = pImage->nextImage;
	}
    }
  return (NULL);
}

/*----------------------------------------------------------------------
  UpdateImageMap sets or updates Ref_IMG MAP attributes for the current
  image.
  If there is a map, updates all mapareas.
  oldWidth is -1 or the old image width.
  oldHeight is -1 or the old image height.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                UpdateImageMap (Element image, Document document, int oldWidth, int oldHeight)
#else  /* __STDC__ */
void                UpdateImageMap (image, document, oldWidth, oldHeight)
Element             image;
Document            document;
int                 oldWidth;
int                 oldHeight;
#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   Element             el, child;
   STRING              text;
   int                 shape, w, h, length;
   int                 deltax, deltay, val;
   DisplayMode         dispMode;

   dispMode = TtaGetDisplayMode (document);
   /* Search the USEMAP attribute */
   attrType.AttrSSchema = TtaGetDocumentSSchema (document);
   attrType.AttrTypeNum = HTML_ATTR_USEMAP;
   attr = TtaGetAttribute (image, attrType);
   if (attr != NULL)
     {
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

	/* ask Thot to stop displaying changes made in the document */
        if (dispMode == DisplayImmediately)
	  TtaSetDisplayMode (document, DeferredDisplay);

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
		       /* do we need to initialize the polyline limits */
		       if (oldWidth == -1 && oldHeight == -1)
			 {
                            child = TtaGetFirstChild (el);
                            TtaChangeLimitOfPolyline (child, UnPixel, w, h, document);
			 }
		    }
		  else if (oldWidth != -1 || oldHeight != -1)
		    {
		      deltax = deltay = 0;
		      /* move and resize the current map area */
		      if (oldWidth != -1 && w != 0)
			{
			  deltax = (w - oldWidth) * 100 / oldWidth;
			  if (deltax != 0)
			    {
			      /* Search the x_coord attribute */
			      attrType.AttrTypeNum = HTML_ATTR_x_coord;
			      attr = TtaGetAttribute (el, attrType);
			      val = TtaGetAttributeValue (attr);
			      val = val + (val * deltax / 100);
			      TtaSetAttributeValue (attr, val, el, document);    
			      /* Search the width attribute */
			      attrType.AttrTypeNum = HTML_ATTR_IntWidthPxl;
			      attr = TtaGetAttribute (el, attrType);
			      val = TtaGetAttributeValue (attr);
			      val = val + (val * deltax / 100);
			      TtaSetAttributeValue (attr, val, el, document);
			    }
			}
		      if (oldHeight != -1 && h != 0)
			{
			  deltay = (h - oldHeight) * 100 / oldHeight;
			  if (deltay != 0)
			    {
			      /* Search the y_coord attribute */
			      attrType.AttrTypeNum = HTML_ATTR_y_coord;
			      attr = TtaGetAttribute (el, attrType);
			      val = TtaGetAttributeValue (attr);
			      val = val + (val * deltay / 100);
			      TtaSetAttributeValue (attr, val, el, document);
			      /* Search the height attribute */
			      attrType.AttrTypeNum = HTML_ATTR_height_;
			      attr = TtaGetAttribute (el, attrType);
			      val = TtaGetAttributeValue (attr);
			      val = val + (val * deltay / 100);
			      TtaSetAttributeValue (attr, val, el, document);
			    }
			}

		      /* update area coords */
		      if (deltax && deltay)
			/* both width and height */
			SetAreaCoords (document, el, 0);
		      else if (deltax)
			/* only width */
			SetAreaCoords (document, el, HTML_ATTR_IntWidthPxl);
		      else
			/* only height */
			SetAreaCoords (document, el, HTML_ATTR_height_);
		    }
	       }
	     TtaNextSibling (&el);
	  }
     }

   /* ask Thot to display changes made in the document */
   TtaSetDisplayMode (document, dispMode);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                DisplayImage (Document doc, Element el, STRING imageName)
#else  /* __STDC__ */
void                DisplayImage (doc, el, imageName)
Document            doc;
Element             el;
STRING              imageName;
#endif /* __STDC__ */
{
  ElementType         elType;
  int                 modified;

  modified = TtaIsDocumentModified (doc);
  elType = TtaGetElementType (el);
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
    {
      /* display the content of a picture element */
      TtaSetTextContent (el, imageName, SPACE, doc);
      UpdateImageMap (el, doc, -1, -1);
    }
  else
    {
#      ifndef _WINDOWS
       fprintf(stderr,"Background image !\n");
#      endif /* _WINDOWS */
      /* create a background image for the element */
      /* set the value */
    }
  
  /* if the document was not modified before this update reset it unmodified */
  if (!modified)
    TtaSetDocumentUnmodified (doc);
  /* the image is loaded */
  TtaSetStatus (doc, 1, " ", NULL);
}


/*----------------------------------------------------------------------
   HandleImageLoaded is the callback procedure when the image is loaded	
   		from the web.						
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                HandleImageLoaded (int doc, int status, STRING urlName,
                                     STRING outputfile, void * context)
#else  /* __STDC__ */
void                HandleImageLoaded (doc, status, urlName, outputfile, context)
int doc;
int status;
STRING urlName;
STRING outputfile;
void *context;

#endif /* __STDC__ */
{
   STRING              pathname;
   STRING               tempfile;
   FetchImage_context  *FetchImage_ctx;
   LoadedImageDesc    *desc;
   STRING              base_url;
   STRING              ptr;
   ElemImage          *ctxEl, *ctxPrev;
   ElementType         elType;

   /* restore the context */
   FetchImage_ctx = (FetchImage_context *) context;
   if (FetchImage_ctx) 
     {
       desc = FetchImage_ctx->desc;   
       base_url = FetchImage_ctx->base_url;
       TtaFreeMemory (FetchImage_ctx);
       
       /* check if this request wasn't aborted */
       if (ustrcmp (base_url, DocumentURLs[doc]))
	 {
	   /* it's not the same url, so let's just return */
	   TtaFreeMemory (base_url);
	   /* @@ remove desc, close file? */
	   return;
	 }
       else
	 TtaFreeMemory (base_url);
     }

#if defined(AMAYA_JAVA) || defined(AMAYA_ILU)
   FilesLoading[doc]--;
#endif
   if (DocumentURLs[doc] != NULL)
     {
	/* the image could not be loaded */
	if ((status != 200) && (status != 0))
	   return;
	tempfile = (STRING) TtaGetMemory (sizeof (CHAR_T) * MAX_LENGTH);
	/* rename the local file of the image */
	ustrcpy (tempfile, desc->localName);
	
	/* If this is an image document, point to the correct files */
	if (DocumentTypes[doc] == docImage)
	  {
	    ptr = ustrrchr (tempfile, '.');
	    if (ptr) 
	      {
		ptr++;
		ustrcpy (ptr, "html");
	      }
	    else
	      ustrcat (tempfile, ".html");
	    TtaFreeMemory (desc->localName);
	    desc->localName = TtaStrdup (tempfile);
	  }
	else
	  {
	    TtaFileUnlink (tempfile);	
#   ifndef _WINDOWS
	    rename (outputfile, tempfile);
#   else /* _WINDOWS */
	    if (rename (outputfile, tempfile) != 0)
	      sprintf (tempfile, "%s", outputfile); 
#   endif /* _WINDOWS */
	  }

	/* save pathname */
	TtaFreeMemory (desc->originalName);
	pathname = urlName;
	desc->originalName = TtaGetMemory (ustrlen (pathname) + 1);
	desc->status = IMAGE_LOADED;
	ustrcpy (desc->originalName, pathname);
	/* display for each elements in the list */
	ctxEl = desc->elImage;
	desc->elImage = NULL;
	while (ctxEl != NULL)
	  {
	    elType = TtaGetElementType (ctxEl->currentElement);
	    if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
	      DisplayImage (doc, ctxEl->currentElement, tempfile);
	    else if (ctxEl->callback != NULL)
	      ctxEl->callback(doc, ctxEl->currentElement, tempfile, ctxEl->extra);
	    /* get image type */
	    if (desc->imageType == unknown_type)
	      desc->imageType = TtaGetPictureType (ctxEl->currentElement);
	    ctxPrev = ctxEl;
	    ctxEl = ctxEl->nextElement;
	    TtaFreeMemory ( ctxPrev);
	  }
	TtaFreeMemory (tempfile);
     }
}

/*----------------------------------------------------------------------
   libWWWImageLoaded is the libWWW callback procedure when the image
                is loaded from the web.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                libWWWImageLoaded (int doc, int status, STRING urlName,
                                     STRING outputfile, STRING content_type,
				       void * context)
#else  /* __STDC__ */
void                libWWWImageLoaded (doc, status, urlName, outputfile, 
				       content_type, context)
int doc;
int status;
STRING urlName;
STRING outputfile;
STRING content_type;
void *context;

#endif /* __STDC__ */
{
  if (DocumentURLs[doc] != NULL)
    {
      /* an image of the document is now loaded */

#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
      /* update the stop button status */
      ResetStop (doc);
#endif /* AMAYA_JAVA */

      /* the image could not be loaded */
      if (status != 0)
	return;

      /* rename the local file of the image */
      HandleImageLoaded (doc, status, urlName, outputfile, context);
     }
}

/*----------------------------------------------------------------------
   GetActiveImageInfo returns the URL information if the current      
   element is an image map and NULL if it is not.          
   The non-null returned string has the form "?X,Y"        
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING              GetActiveImageInfo (Document document, Element element)
#else  /* __STDC__ */
STRING              GetActiveImageInfo (document, element)
Document            document;
Element             element;

#endif /* __STDC__ */
{
   STRING              ptr;
   int                 X, Y;

   ptr = NULL;
   if (element != NULL)
     {
       /* initialize X and Y. The user may click in any view. If it's not */
       /* the formatted view (view 1), TtaGiveSelectPosition does not */
       /* change variables X and Y. */
       X = Y = 0;
       /* Get the coordinates of the mouse within the image */
       TtaGiveSelectPosition (document, element, 1, &X, &Y);
       if (X < 0)
	 X = 0;
       if (Y < 0)
	 Y = 0;
       /* create the search string to be appended to the URL */
       ptr = TtaGetMemory (27);
       sprintf (ptr, "?%d,%d", X, Y);
     }
   return ptr;
}

/*----------------------------------------------------------------------
   FetchImage loads an IMG from local file or from the web. The flags
   may indicate extra transfer parameters, for example bypassing the cache.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FetchImage (Document doc, Element el, STRING URL, int flags,
                                LoadedImageCallback callback, void *extra)
#else  /* __STDC__ */
void                FetchImage (doc, el, URL, flags, callback, extra)
Document            doc;
Element             el;
STRING              URL;
int                 flags;
LoadedImageCallback callback;
void               *extra;

#endif /* __STDC__ */
{
  ElemImage          *ctxEl;
  AttributeType       attrType;
  Attribute           attr;
  LoadedImageDesc    *desc;
  STRING              imageName;
  CHAR_T                pathname[MAX_LENGTH];
  CHAR_T                tempfile[MAX_LENGTH];
  int                 length, i;
  boolean             update;
  boolean             newImage;
  FetchImage_context  *FetchImage_ctx;

  pathname[0] = EOS;
  tempfile[0] = EOS;
  imageName = NULL;
  attr = NULL;
  FetchImage_ctx = NULL;

  if (el != NULL && DocumentURLs[doc] != NULL)
    {
      if (URL == NULL)
	{
	  /* prepare the attribute to be searched */
	  attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
	  attrType.AttrTypeNum = HTML_ATTR_SRC;
	  attr = TtaGetAttribute (el, attrType);
	  if (attr != NULL)
	    /* an element with an attribute SRC has been found */
	    {
	      /* get the attribute value */
	      length = TtaGetTextAttributeLength (attr);
	      if (length > 0)
		{
		  /* allocate some memory: length of name + 6 cars for noname */
		  imageName = TtaGetMemory (length + 7);
		  TtaGiveTextAttributeValue (attr, imageName, &length);
		}
	    }
	}
      else
	imageName = URL;

      if (imageName)
	{
	  update = TRUE;
	  /* add BASE to image name, if necessary */
	  NormalizeURL (imageName, doc, pathname, imageName, NULL);
	  /* is the image already loaded ? */
	  newImage = AddLoadedImage (imageName, pathname, doc, &desc);
	  if (newImage)
	    {
	      /* the current element has to be updated when the image 
		 will be loaded */
	      ctxEl = (ElemImage *) TtaGetMemory (sizeof (ElemImage));
	      desc->elImage = ctxEl;
	      ctxEl->currentElement = el;
	      ctxEl->nextElement = NULL;
	      ctxEl->callback = callback;
	      ctxEl->extra = extra;
	      update = FALSE;	/* the image is not loaded yet */
	      /* store the context before downloading the images */
	      FetchImage_ctx = TtaGetMemory (sizeof (FetchImage_context));
	      FetchImage_ctx->desc = desc;
	      FetchImage_ctx->base_url =  TtaStrdup (DocumentURLs[doc]);

#if defined(AMAYA_JAVA) || defined(AMAYA_ILU)
	      FilesLoading[doc]++;
	      i = GetObjectWWW (doc, pathname, NULL, tempfile,
		                AMAYA_ASYNC | flags, NULL, NULL,
				(void *) libWWWImageLoaded,
				(void *) FetchImage_ctx, NO, NULL);
#else /* !AMAYA_JAVA && !AMAYA_ILU */
	      UpdateTransfer(doc);
	      i = GetObjectWWW (doc, pathname, NULL, tempfile,
	                        AMAYA_ASYNC | flags, NULL, NULL,
				(void *) libWWWImageLoaded,
				(void *) FetchImage_ctx, NO, NULL);
#endif /* !AMAYA_JAVA && !AMAYA_ILU */
	      if (i != -1) 
		desc->status = IMAGE_LOADED;
	      else
		{
		  update = TRUE;
		  desc->status = IMAGE_NOT_LOADED;
		}
	    }

	  /* display the image within the document */
	  if (update)
	    {
	      if (desc == NULL)
		{
		  /* it is a local image */
		if (callback)
		  {
		    if (!ustrncmp(pathname, "file:/", 6))
		      callback(doc, el, &pathname[6], extra);
		    else
		      callback(doc, el, &pathname[0], extra);
		  }
		else
		  DisplayImage (doc, el, pathname);
	      }
	      else
		if (TtaFileExist (desc->localName))
		  {
		    /* remote image, but already here */
		    if (callback)
		      callback(doc, el, desc->localName, extra);
		    else
		      DisplayImage (doc, el, desc->localName);
		    /* get image type */
		    desc->imageType = TtaGetPictureType (el);
		  }
		else
		  {
		    /* chain this new element as waiting for this image */
		    ctxEl = desc->elImage;
		    if (ctxEl != NULL && ctxEl->currentElement != el)
		      {
			/* concerned elements are different */
			while (ctxEl->nextElement != NULL)
			  ctxEl = ctxEl->nextElement;
			ctxEl->nextElement = (ElemImage *) TtaGetMemory (sizeof (ElemImage));
			ctxEl = ctxEl->nextElement;
			ctxEl->callback = callback;
			ctxEl->extra = extra;
			ctxEl->currentElement = el;
			ctxEl->nextElement = NULL;
		      }
		  }
	    }
	}
      
      if (attr != NULL && imageName)
	TtaFreeMemory (imageName);
    }
  TtaHandlePendingEvents ();
}

/*----------------------------------------------------------------------
   FetchAndDisplayImages   fetch and display all images referred   
   by document doc. The flags may indicate extra transfer parameters,
   for example bypassing the cache.
   Returns TRUE if the the transfer succeeds without being stopped;
   Otherwise, returns FALSE.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             FetchAndDisplayImages (Document doc, int flags)
#else  /* __STDC__ */
boolean             FetchAndDisplayImages (doc, flags)
Document            doc;
int                 flags;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   Element             el, elFound;
   ElementType         elType;
   STRING              currentURL;
   boolean             stopped_flag;

#if defined(AMAYA_JAVA) || defined(AMAYA_ILU)
   if (FilesLoading[doc] == 0)
     {
#else 
   /* JK: verify if StopTransfer was previously called */
   if (W3Loading == doc || DocNetworkStatus[doc] & AMAYA_NET_INACTIVE)
     {
       /* transfer interrupted */
       TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_LOAD_ABORT), NULL);
       DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
#endif  /* AMAYA_JAVA || AMAYA_ILU */
       return FALSE;
     }
   else if (DocumentTypes[doc] == docText ||
	    DocumentTypes[doc] == docTextRO ||
	    DocumentTypes[doc] == docCSS ||
	    DocumentTypes[doc] == docCSSRO)
     return FALSE;

   /* register the current URL */
   currentURL = TtaStrdup (DocumentURLs[doc]);
   /* get the root element */
   el = TtaGetMainRoot (doc);
   /* prepare the type of element to be searched */
   elType = TtaGetElementType (el);
   /* prepare the attribute to be searched */
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_SRC;
#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
   /* We are currently fetching images for this document */
   /* during this time LoadImage has not to stop transfer */
#endif
   /* search all elements having an attribute SRC */
   do
     {
	TtaHandlePendingEvents ();
	/* verify if StopTransfer was called */
	if (DocumentURLs[doc] == NULL || ustrcmp (currentURL, DocumentURLs[doc]))
	    /* the document has been removed */
	    break;

#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
	if (W3Loading == doc || DocNetworkStatus[doc] & AMAYA_NET_INACTIVE)
	    break;
#endif
	/* search the next element having an attribute SRC */
	TtaSearchAttribute (attrType, SearchForward, el, &elFound, &attr);
	el = elFound;
	/* FetchImage increments FilesLoading[doc] for each new get request */
	if (el != NULL)
	    FetchImage (doc, el, NULL, flags, NULL, NULL);
     }
   while (el != NULL);

#if !defined(AMAYA_JAVA) && !defined(AMAYA_ILU)
   if (W3Loading != doc)
       stopped_flag = FALSE;
   else
     stopped_flag = TRUE;
#else
   stopped_flag = FALSE;
#endif

   /* Images fetching is now finished */
   TtaFreeMemory (currentURL);

   return (stopped_flag);
}






