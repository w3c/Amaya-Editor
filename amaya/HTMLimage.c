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
#include "query_f.h"
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "EDITORactions_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"


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
ThotBool            AddLoadedImage (STRING name, STRING pathname, Document doc, LoadedImageDesc ** desc)
#else  /* __STDC__ */
ThotBool            AddLoadedImage (name, pathname, doc, desc)
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
   pImage->originalName = TtaAllocString (ustrlen (pathname) + 1);
   ustrcpy (pImage->originalName, pathname);
   pImage->localName = TtaAllocString (ustrlen (localname) + 1);
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
   SetAreaCoords computes the coords attribute value from x, y,       
   width and height of the box.                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                SetAreaCoords (Document document, Element element, int attrNum)
#else  /* __STDC__ */
void                SetAreaCoords (document, element, attrNum)
Document            document;
Element             element;
int                 attrNum;
#endif /* __STDC__ */
{
   ElementType         elType;
   Element             child, map;
   AttributeType       attrType;
   Attribute           attrCoords, attrX, attrY;
   Attribute           attrW, attrH, attrShape;
   STRING              text, buffer;
   int                 x1, y1, x2, y2;
   int                 w, h;
   int                 length, shape, i;

   /* Is it an AREA element */
   elType = TtaGetElementType (element);
   if (elType.ElTypeNum != HTML_EL_AREA)
      return;
   /* get size of the map */
   map = TtaGetParent (element);
   TtaGiveBoxSize (map, document, 1, UnPixel, &w, &h);
   /* Search the coords attribute */
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_coords;
   attrCoords = TtaGetAttribute (element, attrType);
   if (attrCoords == NULL)
      return;

   /* Search the shape attribute */
   attrType.AttrTypeNum = HTML_ATTR_shape;
   attrShape = TtaGetAttribute (element, attrType);
   if (attrShape == NULL)
      return;
   shape = TtaGetAttributeValue (attrShape);
   /* prepare the coords string */
   length = 2000;
   text = TtaAllocString (length);
   if (shape == HTML_ATTR_shape_VAL_rectangle || shape == HTML_ATTR_shape_VAL_circle)
     {
	/* Search the x_coord attribute */
	attrType.AttrTypeNum = HTML_ATTR_x_coord;
	attrX = TtaGetAttribute (element, attrType);
	if (attrX == NULL)
	   return;
	/* Search the y_coord attribute */
	attrType.AttrTypeNum = HTML_ATTR_y_coord;
	attrY = TtaGetAttribute (element, attrType);
	if (attrY == NULL)
	   return;
	/* Search the width attribute */
	attrType.AttrTypeNum = HTML_ATTR_IntWidthPxl;
	attrW = TtaGetAttribute (element, attrType);
	if (attrW == NULL)
	   return;
	/* Search the height attribute */
	attrType.AttrTypeNum = HTML_ATTR_height_;
	attrH = TtaGetAttribute (element, attrType);
	if (attrH == NULL)
	   return;

	x1 = TtaGetAttributeValue (attrX);
	if (x1 < 0)
	  {
	    /* out of left side */
	    x1 = 0;
	    TtaSetAttributeValue (attrX, x1, element, document);	    
	  }
	y1 = TtaGetAttributeValue (attrY);
	if (y1 < 0)
	  {
	    /* out of top side */
	    y1 = 0;
	    TtaSetAttributeValue (attrY, y1, element, document);	    
	  }
	x2 = TtaGetAttributeValue (attrW);
	if (x1 + x2 > w)
	  {
	    /* out of right side */
	    if (x1 > w - 4)
	      {
		if (x2 < w)
		  x1 = w - x2;
		else
		  {
		    x1 = 0;
		    x2 = w;
		    TtaSetAttributeValue (attrW, x2, element, document);
		  }
		TtaSetAttributeValue (attrX, x1, element, document);	    
	      }
	    else
	      {
		x2 = w - x1;
		TtaSetAttributeValue (attrW, x2, element, document);
	      }	    
	  }
	y2 = TtaGetAttributeValue (attrH);
	if (y1 + y2 > h)
	  {
	    /* out of bottom side */
	    if (y1 > h - 4)
	      {
		if (y2 < h)
		  y1 = h - y2;
		else
		  {
		    y1 = 0;
		    y2 = h;
		    TtaSetAttributeValue (attrH, y2, element, document);
		  }
		TtaSetAttributeValue (attrY, y1, element, document);	    
	      }
	    else
	      {
		y2 = h - y1;
		TtaSetAttributeValue (attrH, y2, element, document);
	      }    
	  }
	if (shape == HTML_ATTR_shape_VAL_rectangle)
	   usprintf (text, TEXT("%d,%d,%d,%d"), x1, y1, x1 + x2, y1 + y2);
	else
	  {
	     /* to make a circle, height and width have to be equal */
	     if ((attrNum == 0 && x2 > y2) ||
		 attrNum == HTML_ATTR_height_)
	       {
		 /* we need to update the width */
		 w = y2;
		 h = w / 2;
		 TtaSetAttributeValue (attrW, w, element, document);
	       }
	     else if ((attrNum == 0 && x2 < y2) ||
		      attrNum == HTML_ATTR_IntWidthPxl)
	       {
		 /* we need to update the height */
		 w = x2;
		 h = w / 2;
		 TtaSetAttributeValue (attrH, w, element, document);
	       }
	     else
	       if (x2 > y2)
		 h = y2 / 2;
	       else
		 h = x2 / 2;
	     usprintf (text, TEXT("%d,%d,%d"), x1 + h, y1 + h, h);
	  }
     }
   else if (shape == HTML_ATTR_shape_VAL_polygon)
     {
	child = TtaGetFirstChild (element);
	length = TtaGetPolylineLength (child);
	/* keep points */
	i = 1;
	buffer = TtaAllocString (100);
	text[0] = EOS;
	while (i <= length)
	  {
	     TtaGivePolylinePoint (child, i, UnPixel, &x1, &y1);
	     usprintf (buffer, TEXT("%d,%d"), x1, y1);
	     ustrcat (text, buffer);
	     if (i < length)
	       ustrcat (text, TEXT(","));
	     i++;
	  }
	TtaFreeMemory (buffer);
     }
   TtaSetAttributeText (attrCoords, text, element, document);
   TtaFreeMemory (text);
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
	text = TtaAllocString (length);
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
    {
      TtaSetDocumentUnmodified (doc);
      /* switch Amaya buttons and menus */
      DocStatusUpdate (doc, modified);
    }
  /* the image is loaded */
  TtaSetStatus (doc, 1, TEXT(" "), NULL);
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
/***
   STRING              pathname;	***/
   STRING              tempfile;
   FetchImage_context *FetchImage_ctx;
   LoadedImageDesc    *desc;
   STRING              base_url;
   STRING              ptr;
   ElemImage          *ctxEl, *ctxPrev;
   ElementType         elType;

   /* restore the context */
   desc = NULL;
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

   if (DocumentURLs[doc] != NULL)
     {
	/* the image could not be loaded */
	if ((status != 200) && (status != 0))
	   return;
	tempfile = TtaAllocString (MAX_LENGTH);
	/* rename the local file of the image */
	ustrcpy (tempfile, desc->localName);
	
	/* If this is an image document, point to the correct files */
	if (DocumentTypes[doc] == docImage)
	  {
	    ptr = ustrrchr (tempfile, '.');
	    if (ptr) 
	      {
		ptr++;
		ustrcpy (ptr, TEXT("html"));
	      }
	    else
	      ustrcat (tempfile, TEXT(".html"));
	    TtaFreeMemory (desc->localName);
	    desc->localName = TtaWCSdup (tempfile);

	  }
	else
	  {
	    TtaFileUnlink (tempfile);	
#   ifndef _WINDOWS
	    rename (outputfile, tempfile);
#   else /* _WINDOWS */
	    if (urename (outputfile, tempfile) != 0)
	      usprintf (tempfile, TEXT("%s"), outputfile); 
#   endif /* _WINDOWS */
	  }

	/* save pathname */
	/* That could make confusion if the image is redirected:
	   the registered name is not the original name
	   TtaFreeMemory (desc->originalName);
	   pathname = urlName;
	   desc->originalName = TtaAllocString (ustrlen (pathname) + 1);
	   ustrcpy (desc->originalName, pathname);
	*/
	desc->status = IMAGE_LOADED;
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
                                     STRING outputfile, AHTHeaders *http_headers,
				       void * context)
#else  /* __STDC__ */
void                libWWWImageLoaded (doc, status, urlName, outputfile, 
				       http_headers, context)
int doc;
int status;
STRING urlName;
STRING outputfile;
AHTHeaders *http_headers;
void *context;

#endif /* __STDC__ */
{
  FetchImage_context *FetchImage_ctx;

  if (DocumentURLs[doc] != NULL)
    {
      /* an image of the document is now loaded */
      /* update the stop button status */
      ResetStop (doc);

      /* the image could not be loaded */
      if (status != 0)
	{
	  /* erase the context */
	  FetchImage_ctx = (FetchImage_context *) context;
	  if (FetchImage_ctx) 
	    {
	      if (FetchImage_ctx->base_url)
		TtaFreeMemory (FetchImage_ctx->base_url);
	      /* should we also erase ->desc or update it somehow? */
	      TtaFreeMemory (FetchImage_ctx);
	    }
	  return;
	}

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
       ptr = TtaAllocString (27);
       usprintf (ptr, TEXT("?%d,%d"), X, Y);
     }
   return ptr;
}

/*----------------------------------------------------------------------
   FetchImage loads an IMG from local file or from the web. The flags
   may indicate extra transfer parameters, for example bypassing the cache.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                FetchImage (Document doc, Element el, STRING URL, int flags, LoadedImageCallback callback, void *extra)
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
  ThotBool            update;
  ThotBool            newImage;
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
		  imageName = TtaAllocString (length + 7);
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
	      FetchImage_ctx->base_url =  TtaWCSdup (DocumentURLs[doc]);

	      UpdateTransfer(doc);
	      i = GetObjectWWW (doc, pathname, NULL, tempfile,
	                        AMAYA_ASYNC | flags, NULL, NULL,
				(void *) libWWWImageLoaded,
				(void *) FetchImage_ctx, NO, NULL);
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
		    if (!ustrncmp(pathname, TEXT("file:/"), 6))
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
		      callback (doc, el, desc->localName, extra);
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
ThotBool            FetchAndDisplayImages (Document doc, int flags)
#else  /* __STDC__ */
ThotBool            FetchAndDisplayImages (doc, flags)
Document            doc;
int                 flags;

#endif /* __STDC__ */
{
   AttributeType       attrType;
   Attribute           attr;
   Element             el, elFound;
   ElementType         elType;
   STRING              currentURL;
   ThotBool            stopped_flag;

   /* JK: verify if StopTransfer was previously called */
   if (W3Loading == doc || DocNetworkStatus[doc] & AMAYA_NET_INACTIVE)
     {
       /* transfer interrupted */
       TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_LOAD_ABORT), NULL);
       DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
       return FALSE;
     }
   else if (DocumentTypes[doc] == docText ||
	    DocumentTypes[doc] == docTextRO ||
	    DocumentTypes[doc] == docCSS ||
	    DocumentTypes[doc] == docCSSRO)
     return FALSE;

   /* register the current URL */
   currentURL = TtaWCSdup (DocumentURLs[doc]);
   /* get the root element */
   el = TtaGetMainRoot (doc);
   /* prepare the type of element to be searched */
   elType = TtaGetElementType (el);
   /* prepare the attribute to be searched */
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_SRC;
   /* We are currently fetching images for this document */
   /* during this time LoadImage has not to stop transfer */
   /* search all elements having an attribute SRC */
   do
     {
	TtaHandlePendingEvents ();
	/* verify if StopTransfer was called */
	if (DocumentURLs[doc] == NULL || ustrcmp (currentURL, DocumentURLs[doc]))
	    /* the document has been removed */
	    break;

	if (W3Loading == doc || DocNetworkStatus[doc] & AMAYA_NET_INACTIVE)
	    break;
	/* search the next element having an attribute SRC */
	TtaSearchAttribute (attrType, SearchForward, el, &elFound, &attr);
	el = elFound;
	/* FetchImage increments FilesLoading[doc] for each new get request */
	if (el != NULL)
	    FetchImage (doc, el, NULL, flags, NULL, NULL);
     }
   while (el != NULL);

   if (W3Loading != doc)
       stopped_flag = FALSE;
   else
     stopped_flag = TRUE;

   /* Images fetching is now finished */
   TtaFreeMemory (currentURL);

   return (stopped_flag);
}
