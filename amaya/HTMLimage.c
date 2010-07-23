/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2010
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
#include "SVG.h"
#ifdef _WX
#include "appdialogue_wx.h"
#endif /* _WX */

#include "init_f.h"
#include "query_f.h"
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "EDITORactions_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "html2thot_f.h"
#include "Xml2thot_f.h"


/*----------------------------------------------------------------------
  AddLoadedImage adds a new image into image descriptor table.	
  Local images are not stored into this table and the function    
  returns no descriptor and the value FALSE.                      
  If the remote image is new, adds a descriptor for this image    
  notes its local name and returns TRUE.                          
  For already loaded remote images the function returns the      
  descriptor entry and the value FALSE.                           
  ----------------------------------------------------------------------*/
ThotBool AddLoadedImage (char *name, char *pathname,
                         Document doc, LoadedImageDesc **desc)
{
  LoadedImageDesc    *pImage, *previous, *sameImage;
  char               *localname = NULL;

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
      if (strcmp (pathname, pImage->originalName) == 0)
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
  pImage->originalName = TtaStrdup (pathname);
  pImage->localName = TtaStrdup (localname);
  pImage->tempfile = NULL;
  pImage->prevImage = previous;
  if (previous)
    previous->nextImage = pImage;
  else
    ImageURLs = pImage;
  pImage->nextImage = NULL;
  pImage->document = doc;
  pImage->elImage = NULL;
  pImage->imageType = unknown_type;
  *desc = pImage;
  if (sameImage)
    {
      /* the image file exists for a different document */
      pImage->status = RESOURCE_LOADED;
      pImage->tempfile = localname;
      TtaFileCopy (sameImage->tempfile, pImage->tempfile);
      if (sameImage->content_type)
        pImage->content_type = TtaStrdup (sameImage->content_type);
      else
        pImage->content_type = NULL;
      return (FALSE);
    }
  else
    {
      pImage->status = RESOURCE_NOT_LOADED;
      pImage->content_type = NULL;
      TtaFreeMemory (localname);
      return (TRUE);
    }
}


/*----------------------------------------------------------------------
  RemoveLoadedResources removes loaded resources of the document in the
  current list.        
  ----------------------------------------------------------------------*/
void RemoveLoadedResources (Document doc, LoadedImageDesc **list)
{
  LoadedImageDesc    *pImage, *previous, *next;
  ElemImage          *ctxEl, *ctxPrev;
  char               *ptr;

  pImage = *list;
  previous = NULL;
  while (pImage)
    {
      next = pImage->nextImage;
      /* does the current image belong to the document ? */
      if (pImage->document == doc)
        {
          pImage->status = RESOURCE_NOT_LOADED;
          /* remove the image */
          TtaFileUnlink (pImage->tempfile);
          if (!strncmp (pImage->originalName, "internal:", sizeof ("internal:") - 1)
              && IsHTTPPath (pImage->originalName + sizeof ("internal:") - 1))
            {
              /* erase the local copy of the image */
              ptr = GetLocalPath (doc, pImage->originalName);
              TtaFileUnlink (ptr);
              TtaFreeMemory (ptr);
            }
          /* free the descriptor */
          if (pImage->originalName != NULL)
            TtaFreeMemory (pImage->originalName);
          if (pImage->localName != NULL)
            TtaFreeMemory (pImage->localName);
          if (pImage->tempfile != NULL)
            TtaFreeMemory (pImage->tempfile);
          if (pImage->content_type != NULL)
            TtaFreeMemory (pImage->content_type);
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
          if (previous)
            previous->nextImage = next;
          else
            *list = next;
          if (next)
            next->prevImage = previous;
          else
            
          TtaFreeMemory ((char *) pImage);
          pImage = previous;
        }
      /* next descriptor */
      previous = pImage;
      pImage = next;
    }
}

/*----------------------------------------------------------------------
  SearchLoadedImage searches the image descriptor of a loaded image using
  its local name.
  The function returns the descriptor entry or NULL.
  ----------------------------------------------------------------------*/
LoadedImageDesc *SearchLoadedImage (char *localpath, Document doc)
{
  LoadedImageDesc    *pImage;
  
  if (localpath == NULL)
    return (NULL);
  else
    {
      pImage = ImageURLs;
      while (pImage != NULL)
        {
          if (strcmp (localpath, pImage->localName) == 0 && 
              (doc == 0 || pImage->document == doc))
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
  SearchLoadedImageByURL searches the image descriptor of a loaded image 
  using the docImage document id and the URL (there's only one such image
  per document).
  The function returns the descriptor entry or NULL.
  ----------------------------------------------------------------------*/
LoadedImageDesc *SearchLoadedImageByURL (Document doc, char *url)
{
  LoadedImageDesc    *pImage;
  char               *ptr;

  pImage = ImageURLs;
  while (pImage != NULL)
    {
      if (pImage->document == doc)
        {
          if (url && !strcmp (pImage->originalName, url))
            break;
            
          ptr = pImage->originalName + sizeof ("internal:") - 1;
          if (!strcmp (ptr, url))
            break;
        }
      pImage = pImage->nextImage;
    }
  return (pImage);
}


/*----------------------------------------------------------------------
  SetAreaCoords computes the coords attribute value from x, y,       
  width and height of the box.                           
  ----------------------------------------------------------------------*/
void SetAreaCoords (Document document, Element element, int attrNum, Element image)
{
  ElementType         elType;
  Element             child, map;
  AttributeType       attrType;
  Attribute           attrCoords, attrX, attrY;
  Attribute           attrW, attrH, attrShape;
  char               *text, *buffer;
  int                 x1, y1, x2, y2;
  int                 w, h;
  int                 length, shape, i;

  /* Is it an AREA element */
  elType = TtaGetElementType (element);
  if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") &&
      elType.ElTypeNum != HTML_EL_AREA)
    return;
  /* get size of the map */
  map = TtaGetParent (element);
  if (image)
    TtaGiveBoxSize (image, document, 1, UnPixel, &w, &h);
  else
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
  text = (char *)TtaGetMemory (length);
  if (shape == HTML_ATTR_shape_VAL_rectangle ||
      shape == HTML_ATTR_shape_VAL_circle)
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
      attrType.AttrTypeNum = HTML_ATTR_IntHeightPxl;
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
        sprintf (text, "%d,%d,%d,%d", x1, y1, x1 + x2, y1 + y2);
      else
        {
          /* to make a circle, height and width have to be equal */
          if ((attrNum == 0 && x2 > y2) ||
              attrNum == HTML_ATTR_IntHeightPxl)
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
          sprintf (text, "%d,%d,%d", x1 + h, y1 + h, h);
        }
    }
  else if (shape == HTML_ATTR_shape_VAL_polygon)
    {
      child = TtaGetFirstChild (element);
      length = TtaGetPolylineLength (child);
      /* get points */
      i = 1;
      buffer = (char *)TtaGetMemory (100);
      text[0] = EOS;
      while (i <= length)
        {
          TtaGivePolylinePoint (child, i, UnPixel, &x1, &y1);
          sprintf (buffer, "%d,%d", x1, y1);
          strcat (text, buffer);
          if (i < length)
            strcat (text, ",");
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
void UpdateImageMap (Element image, Document doc, int oldWidth, int oldHeight)
{
  AttributeType       attrType;
  Attribute           attr;
  Element             el, child;
  Element             prev, next, parent;
  ElementType         elType;
  char               *text;
  int                 shape, w, h, length;
  int                 deltax, deltay, val;
  DisplayMode         dispMode;
  ThotBool            newMap;
  ThotBool            oldStructureChecking;

  if (image == NULL)
    return;
  dispMode = TtaGetDisplayMode (doc);
  /* Search the USEMAP attribute */
  elType = TtaGetElementType (image);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_USEMAP;
  parent = TtaGetParent (image);
  if (parent)
    {
      elType = TtaGetElementType (parent);
      if (elType.ElTypeNum == HTML_EL_IMG &&
          !strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        image = parent;
    }
  
  attr = TtaGetAttribute (image, attrType);
  if (attr)
    {
      /* Disable structure checking */
      oldStructureChecking = TtaGetStructureChecking (doc);
      TtaSetStructureChecking (FALSE, doc);
      /* Search the MAP element associated with IMAGE element */
      length = TtaGetTextAttributeLength (attr);
      length++;
      text = (char *)TtaGetMemory (length);
      TtaGiveTextAttributeValue (attr, text, &length);
      if (text[0] == '#')
        el = SearchNAMEattribute (doc, &text[1], NULL, NULL);
      else
        el = SearchNAMEattribute (doc, &text[0], NULL, NULL);
      TtaFreeMemory (text);
      if (el == NULL)
        return;

      /* ask Thot to stop displaying changes made in the document */
      if (dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);

      /* Update MAP attribute */
      attrType.AttrTypeNum = HTML_ATTR_Ref_IMG;
      attr = TtaGetAttribute (el, attrType);
      parent = NULL;
      prev = el;
      newMap = (attr == NULL);
      if (newMap)
        {
          /* change the type of the map */
          TtaPreviousSibling (&prev);
          if (prev == NULL)
            {
              next = el;
              TtaNextSibling (&next);
              if (next == NULL)
                parent = TtaGetParent (el);
            }
	    
          TtaRemoveTree (el, doc);
          TtaChangeElementType (el, HTML_EL_MAP);
          /* create the attribute */
          attr = TtaNewAttribute (attrType);
          TtaAttachAttribute (el, attr, doc);
          /* now reinsert the element new map */
          if (prev != NULL)
            TtaInsertSibling (el, prev, FALSE, doc);
          else if (next != NULL)
            TtaInsertSibling (el, next, TRUE, doc);
          else
            TtaInsertFirstChild (&el, parent, doc);
        }
      TtaSetAttributeReference (attr, el, doc, image);

      /* Update AREAs attribute */
      el = TtaGetFirstChild (el);
      TtaGiveBoxSize (image, doc, 1, UnPixel, &w, &h);
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
                      TtaAttachAttribute (el, attr, doc);
                    }
                  TtaSetAttributeReference (attr, el, doc, image);
                  /* do we need to initialize the polyline limits */
                  if (oldWidth == -1 && oldHeight == -1)
                    {
                      child = TtaGetFirstChild (el);
                      TtaChangeLimitOfPolyline (child, UnPixel, w, h, doc);
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
                          TtaSetAttributeValue (attr, val, el, doc);    
                          /* Search the width attribute */
                          attrType.AttrTypeNum = HTML_ATTR_IntWidthPxl;
                          attr = TtaGetAttribute (el, attrType);
                          val = TtaGetAttributeValue (attr);
                          val = val + (val * deltax / 100);
                          TtaSetAttributeValue (attr, val, el, doc);
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
                          TtaSetAttributeValue (attr, val, el, doc);
                          /* Search the height attribute */
                          attrType.AttrTypeNum = HTML_ATTR_IntHeightPxl;
                          attr = TtaGetAttribute (el, attrType);
                          val = TtaGetAttributeValue (attr);
                          val = val + (val * deltay / 100);
                          TtaSetAttributeValue (attr, val, el, doc);
                        }
                    }

                  /* update area coords */
                  if (deltax && deltay)
                    /* both width and height */
                    SetAreaCoords (doc, el, 0, image);
                  else if (deltax)
                    /* only width */
                    SetAreaCoords (doc, el, HTML_ATTR_IntWidthPxl, image);
                  else
                    /* only height */
                    SetAreaCoords (doc, el, HTML_ATTR_IntHeightPxl, image);
                }
            }
          TtaNextSibling (&el);
        }
      /* Restore the structure checking */
      TtaSetStructureChecking (oldStructureChecking, doc);
    }

  /* ask Thot to display changes made in the document */
  TtaSetDisplayMode (doc, dispMode);
}

/*----------------------------------------------------------------------
  SetAttrOnElement 
  attach a new (or existing) attribut to an element and set his value
  int attrNum	: the attribut identifier
  int value	: the attribut value
  ----------------------------------------------------------------------*/
void SetAttrOnElement ( Document doc, Element el, int attrNum, int value )
{
  AttributeType attrType;
  Attribute	attr;
  ThotBool	docModified;

  docModified = TtaIsDocumentModified (doc);
  attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
  attrType.AttrTypeNum = attrNum;
  attr = TtaGetAttribute (el, attrType);
  if (attr == NULL)
    {
      /* the element does not have that attribute. Create it */
      attr = TtaNewAttribute (attrType);
      TtaSetAttributeValue (attr, value, NULL, doc);
      TtaAttachAttribute (el, attr, doc);
    }
  else
    /* change the attribute value */
    TtaSetAttributeValue (attr, value, el, doc);

  if (!docModified)
    {
      TtaSetDocumentUnmodified (doc);
      /* switch Amaya buttons and menus */
      DocStatusUpdate (doc, docModified);
    }
}

/*----------------------------------------------------------------------
  DisplayImage
  Return TRUE if an external document is loaded (the picture element
  can be lost)
  ----------------------------------------------------------------------*/
void DisplayImage (Document doc, Element el, LoadedImageDesc *desc,
                       char *localfile, char *mime_type)
{
  ElementType         elType, parentType;
  Element             parent;
  AttributeType       attrType;
  Attribute           attr;
  int                 i;
  DocumentType        thotType;
  PicType             picType;
  int                 parsingLevel, extraProfile;
  CHARSET             charset;
  char                charsetname[MAX_LENGTH];
  char               *imageName;
  char               *tempfile;
  char               *originalName;
  ThotBool            modified;
  ThotBool            is_svg, is_mml, is_html, htmlok;
  ThotBool            xmlDec, withDoctype, isXML, useMath, isKnown;

  picType = TtaGetPictureType (el);
  if (desc)
    {
      imageName = desc->localName;
      tempfile = desc->tempfile;
      originalName = desc->originalName;
      /* get image type */
      desc->imageType = picType;
    }
  else
    {
      imageName = localfile;
      tempfile = localfile;
      originalName = localfile;
    }

  modified = TtaIsDocumentModified (doc);
  elType = TtaGetElementType (el);
  is_svg = FALSE;
  is_mml = FALSE;
  is_html = FALSE;
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
      ((elType.ElTypeNum == HTML_EL_Object ||
        elType.ElTypeNum == HTML_EL_IFRAME ||
        elType.ElTypeNum == HTML_EL_Embed_) &&
       (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") == 0)))
    {
      /* Determine the type of the external element */
      htmlok = TRUE;
      if (picType == unknown_type)
        {
          if (!strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") &&
              (elType.ElTypeNum == HTML_EL_PICTURE_UNIT))
            {
              parent = TtaGetParent (el);
              parentType = TtaGetElementType (parent);
              if (parentType.ElTypeNum != HTML_EL_Object &&
                  parentType.ElTypeNum != HTML_EL_Embed_)
                htmlok = FALSE;
            }
          if (mime_type)
            {
              if (!strncmp (mime_type, "image/svg", 9) ||
                  !strcmp (mime_type, AM_SVG_MIME_TYPE))
                is_svg = TRUE;
              else if (!strncmp (mime_type, "application/mathml+xml", 22) ||
                       !strcmp (mime_type, AM_MATHML_MIME_TYPE))
                is_mml = TRUE;
              else if (!strncmp (mime_type, "text/mathml", 11) ||
                       !strcmp (mime_type, AM_MATHML_MIME_TYPE))
                is_mml = TRUE;
              else if (!strncmp (mime_type, "text/htm", 8) ||
                       !strcmp (mime_type, AM_XHTML_MIME_TYPE))
                is_html = TRUE;
              else if (!IsImageType (mime_type))
                /* unknown mime_type, check with another method */
                mime_type = NULL;
            }
          if (!mime_type)
            {
              /* try the file's extension */
              for (i = strlen (imageName); i > 0 && imageName[i] != '.'; i--);
              if (imageName[i] == '.' && !strcmp (&imageName[i+1], "svg"))
                is_svg = TRUE;
              else if (imageName[i] == '.' && !strcmp (&imageName[i+1], "mml"))
                is_mml = TRUE;
              else if (imageName[i] == '.' && !strncmp (&imageName[i+1], "htm", 3))
                is_html = TRUE;
              else /* try sniffing */
                {
                  CheckDocHeader (tempfile, &xmlDec, &withDoctype, &isXML, &useMath, &isKnown, 
				  &parsingLevel, &charset, charsetname, &thotType, &extraProfile);
                  if (isXML && thotType == docSVG)
                    is_svg = TRUE;
                  if (isXML && thotType == docMath)
                    is_mml = TRUE;
                  if (isXML && thotType == docHTML)
                    is_html = TRUE;
                }
            }
        }
 
      /* If image load failed show the alternate text or content */
      parent = TtaGetParent (el);
      parentType = TtaGetElementType (parent);
      if ((desc && desc->status == RESOURCE_NOT_LOADED) ||
          (desc == NULL && !TtaFileExist (originalName)))
        {
          if (parentType.ElTypeNum == HTML_EL_Object ||
              parentType.ElTypeNum == HTML_EL_Embed_)
            /* it's an image into an object -> display object content */
            SetAttrOnElement (doc, parent, HTML_ATTR_NoObjects, 1);
          else if (elType.ElTypeNum == HTML_EL_IFRAME)
            SetAttrOnElement (doc, el, HTML_ATTR_NoObjects, 1);
          else
            /* it's an image -> display image alt text */
            SetAttrOnElement (doc, parent, HTML_ATTR_NoImages, 1);
        }
      else
        /* image successfuly loaded */
        if (parentType.ElTypeNum == HTML_EL_Object)
          /* remove attribute NoObjects from the Object element */
          {
            attrType.AttrSSchema = parentType.ElSSchema;
            attrType.AttrTypeNum = HTML_ATTR_NoObjects;
            attr = TtaGetAttribute (parent, attrType);
            if (attr)
              TtaRemoveAttribute (parent, attr, doc);
          }

      if (is_svg)
        {
#ifdef _SVG
          TtaSetPictureType (el, AM_SVG_MIME_TYPE);
          if (desc)
            desc->imageType = svg_type;
          /* parse the SVG file and include the parsed tree at the
             position of the image element */
          ParseExternalDocument (tempfile, originalName, el, FALSE, doc, 
                                 TtaGetDefaultLanguage(), "SVG");
#endif /* _SVG */
        }
      else if (is_mml)
        {
          TtaSetPictureType (el, AM_MATHML_MIME_TYPE);
          if (desc)
            desc->imageType = mathml_type;
          /* parse the MathML file and include the parsed tree at the
             position of the image element */
          ParseExternalDocument (tempfile, originalName, el, FALSE, doc, 
                                 TtaGetDefaultLanguage(), "MathML");
        }
      else if (is_html && htmlok)
        {
          TtaSetPictureType (el, AM_XHTML_MIME_TYPE);
          if (desc)
            desc->imageType = html_type;
      	  
          /* parse the HTML file and include the parsed tree at the
             position of the image element */
          ParseExternalDocument (tempfile, originalName, el, FALSE, doc, 
                                 TtaGetDefaultLanguage(), "HTML");
        }
      else
        {
          /* display the content of a picture element */
          TtaSetPictureContent (el, (unsigned char *)tempfile, SPACE, doc, mime_type);
          UpdateImageMap (el, doc, -1, -1);
        }
    }
  else if ((elType.ElTypeNum == SVG_EL_use_ ||
            elType.ElTypeNum == SVG_EL_tref) &&
           (strcmp(TtaGetSSchemaName (elType.ElSSchema), "SVG")  == 0))
    {
      is_svg = TRUE;
      /* parse the SVG file and include the parsed sub-tree at the
         position of the use element */
      ParseExternalDocument (tempfile, originalName, el, FALSE, doc, 
                             TtaGetDefaultLanguage(), "SVG");
    }
  
  /* if the document was not modified before this update reset it unmodified */
  if (!modified)
    {
      TtaSetDocumentUnmodified (doc);
      /* switch Amaya buttons and menus */
      DocStatusUpdate (doc, modified);
    }
  /* the image is loaded */
  TtaSetStatus (doc, 1, " ", NULL);
}

/*----------------------------------------------------------------------
  SetContainerImageName changes the image name when a container is generated.
  ----------------------------------------------------------------------*/
void SetContainerImageName (char *imagefile)
{
  char *ptr, *nextdot;

  /* Rename the current downloaded file (an image) so that we can
     find it easily next time around. 
     The convention is to change the image's extension to 'html',
     and give the HTML's container the image's extension */
  ptr = strrchr (imagefile, DIR_SEP);
  ptr++;
  /* look for the last dot */
  nextdot = strchr (ptr, '.');
  while (nextdot)
    {
      ptr = nextdot + 1;
      nextdot = strchr (ptr, '.');
    }
  if (ptr)
    strcpy (ptr, "html");
  else
    strcat (imagefile, ".html");
}

/*----------------------------------------------------------------------
  HandleImageLoaded is the callback procedure when the image is loaded	
  from the web.						
  ----------------------------------------------------------------------*/
static void HandleImageLoaded (int doc, int status, char *urlName, char *outputfile,
                               char *proxyname, AHTHeaders *http_headers, void * context)
{
  FetchImage_context *FetchImage_ctx;
  LoadedImageDesc    *desc;
  ElemImage          *ctxEl, *ctxPrev;
  ElementType         elType;
  DisplayMode         dispMode;
  char               *tempfile;
  char               *base_url;
  char               *ptr;
  char               *dir, *name;
  char               *prefix;
  int                 i, j;
  ThotBool            oldStructureChecking;

  /* restore the context */
  desc = NULL;
  FetchImage_ctx = (FetchImage_context *) context;
  if (FetchImage_ctx) 
    {
      desc = FetchImage_ctx->desc;   
      base_url = FetchImage_ctx->base_url;
      TtaFreeMemory (FetchImage_ctx);
       
      /* check if this request wasn't aborted */
      if (base_url && DocumentURLs[doc] &&
          strcmp (base_url, DocumentURLs[doc]))
        {
          /* it's not the same url, so let's just return */
          TtaFreeMemory (base_url);
          /* @@ remove desc, close file? */
          return;
        }
      else
        TtaFreeMemory (base_url);
    }

  if (doc == 0 || DocumentURLs[doc])
    {
      /* compute the tempfile name */
      if (desc->tempfile)
        TtaFreeMemory (desc->tempfile);
      tempfile = (char *)TtaGetMemory (MAX_LENGTH);
      strncpy (tempfile, desc->localName, MAX_LENGTH-1);
      tempfile[MAX_LENGTH-1] = EOS;
      /* If this is an image document, point to the correct files */
      if (DocumentTypes[doc] == docImage)
        {
          SetContainerImageName (tempfile);
          desc->tempfile = tempfile;
        }
      else if (status < 0)
        {
          // load error
          TtaFileUnlink (outputfile);
          TtaFreeMemory (tempfile);
          desc->tempfile = NULL;
        }
      else
        {
          /* make a unique name */
          ptr = strrchr (tempfile, DIR_SEP);
          if (ptr)
            {
              *ptr = EOS;
              ptr++;
            }
          dir = tempfile;
          if (ptr)
            {
              prefix = ptr;
              ptr = strchr (prefix, '.');
              if (ptr)
                {
                  // add the suffix (x.png differs to x.gif)
                  i = strlen (ptr);
                  if (i > 4)
                    i = 4;
                  for (j = 0; j < i; j++)
                    {
                      ptr[j] = ptr[j+1];
                      ptr += strlen (ptr);
                    }
                  ptr[j] = EOS;
                }
            }
          else
            prefix = NULL;
          desc->tempfile = GetTempName (dir, prefix);
          TtaFreeMemory (tempfile);
          TtaFileUnlink (desc->tempfile);
          if (TtaFileCopyUncompress (outputfile, desc->tempfile))
            /* copy done */
            TtaFileUnlink (outputfile);
          else
            /* change the tempfile name */
            sprintf (desc->tempfile, "%s", outputfile); 
        }

      /* update desc->status in order to alert DisplayImage if the
         image was not found */	
      if (status != 200 && status != 0)
        desc->status = RESOURCE_NOT_LOADED;
      else
        desc->status = RESOURCE_LOADED;
	
      /* display for each elements in the list */
      /* get the mime type if the image was downloaded from the net */
      if (DocumentTypes[doc] == docImage && DocumentMeta[doc])
        ptr = DocumentMeta[doc]->content_type;
      else
        ptr = HTTP_headers (http_headers, AM_HTTP_CONTENT_TYPE); 
      /* memorize the mime type (in case we want to save the file later on) */
      if (ptr)
        desc->content_type = TtaStrdup (ptr);

      ctxEl = desc->elImage;
      desc->elImage = NULL;
      /* Avoid too many redisplay */
      dispMode = TtaGetDisplayMode (doc);
      if (doc != 0 && dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, DeferredDisplay);
      while (ctxEl)
        {
          /* the image may be included using either an SRC, an EMBED,
             an OBJECT, a use or a tref element */
          if (ctxEl->callback)
            {
              if (desc->status == RESOURCE_LOADED)
                ctxEl->callback (doc, ctxEl->currentElement, desc->tempfile,
                                 ctxEl->extra, TRUE);
            }
          else if (DocumentMeta[doc] && ctxEl->currentElement == DocumentMeta[doc]->link_icon)
            {
#ifdef _WX
              TtaSetPageIcon (doc, 1, desc->tempfile);
              DocumentMeta[doc]->link_icon = NULL;
#endif /* _WX */
            }
          else
            {
              elType = TtaGetElementType (ctxEl->currentElement);
              name = TtaGetSSchemaName (elType.ElSSchema);
              if ((strcmp (name, "HTML") == 0 &&
                   desc->status == RESOURCE_LOADED &&
                   (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
                    elType.ElTypeNum == HTML_EL_Embed_ ||
                    elType.ElTypeNum == HTML_EL_Object ||
                    elType.ElTypeNum == HTML_EL_IFRAME)) ||
                  (strcmp (name, "SVG") == 0 &&
                   (elType.ElTypeNum == SVG_EL_PICTURE_UNIT ||
                    elType.ElTypeNum == SVG_EL_use_ ||
                    elType.ElTypeNum == SVG_EL_tref)))
                {
                  /* do not check mandatory attributes */
                  oldStructureChecking = TtaGetStructureChecking (doc);
                  TtaSetStructureChecking (FALSE, doc);
                  DisplayImage (doc, ctxEl->currentElement, desc, NULL, ptr);
                  TtaSetStructureChecking (oldStructureChecking, doc);
                }
            }
          ctxPrev = ctxEl;
          ctxEl = ctxEl->nextElement;
          TtaFreeMemory (ctxPrev);
        }
      /* Restore the display mode */
      if (doc != 0 && dispMode == DisplayImmediately)
        TtaSetDisplayMode (doc, dispMode);
    }
}

/*----------------------------------------------------------------------
  libWWWImageLoaded is the libWWW callback procedure when the image
  is loaded from the web.
  ----------------------------------------------------------------------*/
static void libWWWImageLoaded (int doc, int status, char *urlName, char *outputfile,
			       char *proxyname, AHTHeaders *http_headers, void * context)
{
  if (doc == 0 || DocumentURLs[doc])
    {
      /* an image of the document is now loaded */
      /* update the stop button status */
      ResetStop (doc);
#ifdef _GL
      if (FilesLoading[doc] == 0 && TtaGetViewFrame (doc, 1) != 0)
        /* all files included in this document have been loaded and the
           document is displayed. Animations can be played now */
        TtaPlay (doc, 1);
#endif /* _GL */
      
      /* rename the local file of the image */
      HandleImageLoaded (doc, status, urlName, outputfile, NULL, http_headers, context);
    }
}

/*----------------------------------------------------------------------
  GetActiveImageInfo returns the URL information if the current      
  element is an image map and NULL if it is not.          
  The non-null returned string has the form "?X,Y"        
  ----------------------------------------------------------------------*/
char *GetActiveImageInfo (Document document, Element element)
{
  char               *ptr;
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
      ptr = (char *)TtaGetMemory (27);
      sprintf (ptr, "?%d,%d", X, Y);
    }
  return ptr;
}

/*----------------------------------------------------------------------
  FetchImage loads an image from local file or from the web.
  The flags may indicate extra transfer parameters, for example bypassing
  the cache.
  The imageURI is encoded with the default charset.
  Return TRUE if an action is done.
  ----------------------------------------------------------------------*/
ThotBool FetchImage (Document doc, Element el, char *imageURI, int flags,
                     LoadedImageCallback callback, void *extra)
{
  ElemImage          *ctxEl;
  ElementType         elType;
  Element             elAttr;
  AttributeType       attrType, attrType2;
  Attribute           attr;
  LoadedImageDesc    *desc;
  char               *imageName, *utf8value, *utf8pathname;
  char                pathname[MAX_LENGTH];
  char                tempfile[MAX_LENGTH];
  int                 length, i, newflags;
  ThotBool            update, ret;
  ThotBool            newImage, clean;
  FetchImage_context *FetchImage_ctx;

  pathname[0] = EOS;
  tempfile[0] = EOS;
  utf8pathname = NULL;
  imageName = NULL;
  attr = NULL;
  FetchImage_ctx = NULL;
  update = FALSE;
  attrType2.AttrSSchema = NULL;
  attrType2.AttrTypeNum = 0;
  ret = TRUE;
  if (el || extra)
    {
      if (imageURI == NULL)
        {
          /* prepare the attribute to be searched */
          elType = TtaGetElementType (el);
          attrType.AttrSSchema = elType.ElSSchema;
          if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
            /* it's not a SVG element, it's then a HTML img element */
            {
              attrType.AttrTypeNum = HTML_ATTR_SRC;
              attrType2.AttrSSchema = attrType.AttrSSchema;
              attrType2.AttrTypeNum = HTML_ATTR_FrameSrc;
              elAttr = el;
            }
#ifdef _SVG
          else
            {
              attrType.AttrTypeNum = SVG_ATTR_xlink_href;
              elAttr = TtaGetParent (el);
            }
#endif /* _SVG */
          attr = TtaGetAttribute (elAttr, attrType);
          if (attr == NULL)
            attr = TtaGetAttribute (elAttr, attrType2);
          if (attr)
            /* an element with an attribute SRC or xlink:href has been found */
            {
              /* get the attribute value */
              length = TtaGetTextAttributeLength (attr);
              if (length > 0)
                {
                  /* allocate some memory: length of name + 6 cars for noname */
                  utf8value = (char *)TtaGetMemory (length + 7);
                  TtaGiveTextAttributeValue (attr, utf8value, &length);
                  // remove extra spaces
                  clean = FALSE;
                  while (length > 0 && utf8value[length-1] == SPACE)
                    {
                      utf8value[length-1] = EOS;
                      length--;
                      clean = TRUE;
                    }
                  if (clean)
                    TtaSetAttributeText (attr, utf8value, elAttr, doc);
                  imageName = (char *)TtaConvertMbsToByte ((unsigned char *)utf8value,
                                                           TtaGetDefaultCharset ());
                  TtaFreeMemory (utf8value);
                }
            }
        }
      else
        imageName = imageURI;
      
      /* LC 3/7/01 */
      /* Don't treat the  xlink:href attributes 
         that begins with the string  'data:' */
#ifdef _BASE64     
      if (imageName)
        {
          if (strncasecmp (imageName, "data:", 5) == 0)
            {
              if (MakeImageFromBase64 (imageName))	
                update = TRUE;
            }
          else
            update = TRUE;
        }

      if (update)
        {
#else /*_BASE64*/
      if (imageName && strncasecmp (imageName, "data:", 5) != 0)
        {
          update = TRUE;    
#endif /*_BASE64*/
          /* add BASE to image name, if necessary */
          NormalizeURL (imageName, doc, pathname, imageName, NULL);
          /* if it's not a remote URL, make any necessary file: conversions */
          if (!IsW3Path (pathname))
            {
              NormalizeFile (pathname, tempfile, AM_CONV_ALL);
              strcpy (pathname, tempfile);
              tempfile[0] = EOS;
            }
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
              FetchImage_ctx = (FetchImage_context*)TtaGetMemory (sizeof (FetchImage_context));
              FetchImage_ctx->desc = desc;
              FetchImage_ctx->base_url =  TtaStrdup (DocumentURLs[doc]);
              
              UpdateTransfer(doc);
              if (flags & AMAYA_MBOOK_IMAGE)
                newflags = flags | AMAYA_SYNC;
              else
                newflags = flags | AMAYA_ASYNC;
	      
              /* Convert uri into utf8 */
              utf8pathname = (char *)TtaConvertByteToMbs ((unsigned char *)pathname,
                                                          TtaGetDefaultCharset ());
              i = GetObjectWWW (doc, doc, utf8pathname, NULL, tempfile,
                                newflags, NULL, NULL,
                                (void (*)(int, int, char*, char*, char*, const AHTHeaders*, void*)) libWWWImageLoaded,
                                (void *) FetchImage_ctx, NO, NULL);
              if (i != -1) 
                desc->status = RESOURCE_LOADED;
              else
                {
                  update = TRUE;
                  desc->status = RESOURCE_NOT_LOADED;
                }
              TtaFreeMemory (utf8pathname);
            }
          
          /* display the image within the document */
          if (update)
            {
              if (desc == NULL)
                {
                  /* it is a local image */
                  if (callback)
                    {
                      if (!strncmp (pathname, "file:/", 6))
                        callback (doc, el, &pathname[6], extra, TRUE);
                      else
                        callback (doc, el, pathname, extra, TRUE);
                    }
                  else if (DocumentMeta[doc] &&
                           el == DocumentMeta[doc]->link_icon)
                    {
#ifdef _WX
                      if (!strncmp (pathname, "file:/", 6))
                        TtaSetPageIcon (doc, 1, &pathname[6]);
                      else
                        TtaSetPageIcon (doc, 1, pathname);
                      DocumentMeta[doc]->link_icon = NULL;
#endif /* _WX */
                    }
                  else
                    DisplayImage (doc, el, NULL, pathname, NULL);
                }
              else if (desc->tempfile && TtaFileExist (desc->tempfile))
                {
                  /* remote image, but already here */
                  if (callback)
                    {
                      callback (doc, el, desc->tempfile, extra, FALSE);
                    }
                  else if (DocumentMeta[doc] &&
                           el == DocumentMeta[doc]->link_icon)
                    {
#ifdef _WX
                      TtaSetPageIcon (doc, 1, desc->tempfile);
                      DocumentMeta[doc]->link_icon = NULL;
#endif /* _WX */
                    }
                  else
                    DisplayImage (doc, el, desc, NULL, desc->content_type);
                }
              else
                {
                  /* chain this new element as waiting for this image */
                  ctxEl = desc->elImage;
                  if (ctxEl && (ctxEl->currentElement != el || callback))
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
                  else if (callback)
                    {
                      /* an element is already connected */
                      TtaFreeMemory (extra);
                      ret = FALSE;
                    }
                }
            }
        }
      else if (callback)
        {
          /* no callback connected */
          TtaFreeMemory (extra);
          ret = FALSE;
        }

          if (attr && imageName)
        TtaFreeMemory (imageName);
    }
  TtaHandlePendingEvents ();
  return ret;
}

/*----------------------------------------------------------------------
  FetchIcon
  ----------------------------------------------------------------------*/
static void FetchIcon (Document doc, int flags, Element el,
                         char *currentURL, AttributeType attrType)
{
  Attribute           attr;
  char               *imageURI, *utf8value;
  int                 length = 0;

  attr = TtaGetAttribute (el, attrType);
  if (attr)
    length = TtaGetTextAttributeLength (attr);
  if (length > 0)
    {
      /* allocate some memory */
      utf8value = (char *)TtaGetMemory (length + 7);
      TtaGiveTextAttributeValue (attr, utf8value, &length);
      imageURI = (char *)TtaConvertMbsToByte ((unsigned char *)utf8value,
                                              TtaGetDefaultCharset ());
      TtaFreeMemory (utf8value);
      FetchImage (doc, el, imageURI, flags, NULL, NULL);
      TtaFreeMemory (imageURI);
    }
 }


/*----------------------------------------------------------------------
  FetchImages fetches images linked by attrType1 or attrType2 attributes.  
  The flags may indicate extra transfer parameters, for example bypassing
  the cache.
  elSubTree indicates we are parsing an external image or object.
  Returns TRUE if the the transfer succeeds without being stopped;
  Otherwise, returns FALSE.
  ----------------------------------------------------------------------*/
static void FetchImages (Document doc, int flags, Element elSubTree,
                         char *currentURL,
                         AttributeType attrType1, AttributeType attrType2,
                         ThotBool loadImages, ThotBool loadObjects)
{
  Attribute           attr, attrFound;
  Element             el, elFound, pic, elNext;
  Element             parent;
  ElementType         elType, parentType;
  char               *name;
  char               *imageURI, *utf8value;
  int                 length;

  /* Start from the root element */
  if (elSubTree == NULL)
    {
      el = TtaGetMainRoot (doc);
      TtaSearchAttributes (attrType1,attrType2, SearchForward,
                           el, &elFound, &attrFound);
    }
  else
    TtaSearchAttributes (attrType1,attrType2, SearchInTree,
                         elSubTree, &elFound, &attrFound);
  el = elFound;
  attr = attrFound;
  do
    {
      TtaHandlePendingEvents ();
      /* verify if StopTransfer was called */
      if (DocumentURLs[doc] == NULL ||
          strcmp (currentURL, DocumentURLs[doc]))
        /* the document has been removed */
        break;
          
      if (W3Loading == doc || DocNetworkStatus[doc] & AMAYA_NET_INACTIVE)
        break;
          
      /* FetchImage increments FilesLoading[doc] for
         each new get request */
      if (el)
        {
          /* search the next element having an attribute SRC */
          elNext = el;
          TtaSearchAttributes (attrType1, attrType2,
                               SearchForward, elNext, &elFound, &attrFound);
          if (elSubTree && elFound && !TtaIsAncestor (elFound, elSubTree))
            elFound = NULL;
              
          /* Load only wanted elements (images, objects) :
           * this could be changed into preferences menu (browsing) */
          elType = TtaGetElementType (el);
          name = TtaGetSSchemaName (elType.ElSSchema);
          parent = TtaGetParent (el);
          parentType = TtaGetElementType (parent);
         if (!strcmp(name, "SVG"))
           {
#ifdef _SVG
              if (elType.ElTypeNum == SVG_EL_a)
                pic = NULL;
              else if (elType.ElTypeNum == SVG_EL_use_ ||
                       elType.ElTypeNum == SVG_EL_tref)
                pic = el;
              else
                {
                  elType.ElTypeNum = SVG_EL_PICTURE_UNIT;
                  pic = TtaSearchTypedElement (elType, SearchInTree, el);
                }
             if (pic)
                {
                  /* get the attribute value */
                  length = TtaGetTextAttributeLength (attr);
                  if (length > 0)
                    {
                      /* allocate some memory */
                      utf8value = (char *)TtaGetMemory (length + 7);
                      TtaGiveTextAttributeValue (attr, utf8value, &length);
                      imageURI = (char *)TtaConvertMbsToByte ((unsigned char *)utf8value,
                                                              TtaGetDefaultCharset ());
                      TtaFreeMemory (utf8value);
                      if (!( (imageURI[0] == '#')))
                        /* don't handle internal links for a use element */
                        FetchImage (doc, pic, imageURI, flags, NULL, NULL);
                      TtaFreeMemory (imageURI);
                    }
                }
#endif /* _SVG */
           }
         else
           {
             if (parentType.ElTypeNum == HTML_EL_Object ||
                 parentType.ElTypeNum == HTML_EL_Embed_)
               {
                 /* this element is an OBJECT or an EMBED */
                 if (loadObjects)
                   {
                     FetchImage (doc, el, NULL, flags, NULL, NULL);
                     // Check included objects or images ???
                   }
               }
             else if (elType.ElTypeNum == HTML_EL_IFRAME)
               {
                 if (loadObjects)
                   FetchImage (doc, el, NULL, flags, NULL, NULL);		  
               }
             else if (elType.ElTypeNum == HTML_EL_IMG ||
                      elType.ElTypeNum == HTML_EL_Image_Input)
               {
                 /* this element is an IMG or INPUT */
                 if (loadImages)
                   {
                     // manage the included PICTURE element
                     elType.ElTypeNum = HTML_EL_PICTURE_UNIT;
                     pic = NULL;
                     pic = TtaSearchTypedElement (elType, SearchInTree, el);
                     if (pic)
                       {
                         if (pic == elFound)
                           // look for th next image
                           TtaSearchAttributes (attrType1, attrType2, SearchForward,
                                                pic, &elFound, &attrFound);
                         FetchImage (doc, pic, NULL, flags, NULL, NULL);
                       }
                   }
               }
             else
               {
                 /* this element is an PICTURE UNIT */
                 if (loadImages)
                   FetchImage (doc, el, NULL, flags, NULL, NULL);
               }
           }
              
          el = elFound;
          attr = attrFound;
        }
    }
  while (el);
}

/*----------------------------------------------------------------------
  FetchAndDisplayImages   fetch and display all images referred   
  by document doc. The flags may indicate extra transfer parameters,
  for example bypassing the cache.
  elSubTree indicates we are parsing an external image or object
  Returns TRUE if the the transfer succeeds without being stopped;
  Otherwise, returns FALSE.
  ----------------------------------------------------------------------*/
ThotBool FetchAndDisplayImages (Document doc, int flags, Element elSubTree)
{
  AttributeType       attrType, attrType1, attrType2;
  DisplayMode         dispMode;
  char               *currentURL;
  ThotBool            stopped_flag, loadImages, loadObjects;

  TtaGetEnvBoolean ("LOAD_IMAGES", &loadImages);
  TtaGetEnvBoolean ("LOAD_OBJECTS", &loadObjects);

  if (!loadObjects)
    ChangeAttrOnRoot (doc, HTML_ATTR_NoObjects);

  if (!loadImages)
    ChangeAttrOnRoot (doc, HTML_ATTR_NoImages);
    
  /* JK: verify if StopTransfer was previously called */
  if (W3Loading == doc || DocNetworkStatus[doc] & AMAYA_NET_INACTIVE)
    {
      /* transfer interrupted */
      TtaSetStatus (doc, 1, TtaGetMessage (AMAYA, AM_LOAD_ABORT), NULL);
      DocNetworkStatus[doc] |= AMAYA_NET_ERROR;
      return FALSE;
    }
  else if (DocumentTypes[doc] == docText || DocumentTypes[doc] == docCSS)
    return FALSE;

  /* register the current URL */
  currentURL = TtaStrdup (DocumentURLs[doc]);

  /* Avoid too many redisplay */
  dispMode = TtaGetDisplayMode (doc);
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, DeferredDisplay);
  /* We are currently fetching images for this document */
  /* during this time LoadImage has not to stop transfer */
  /* prepare the attribute to be searched */
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  if (attrType.AttrSSchema)
    /* there are some HTML elements in this documents. 
       Get all 'img' or 'object' or 'embed' elements */
    {
      if (DocumentMeta[doc] && DocumentMeta[doc]->link_icon &&
          loadImages && elSubTree == NULL)
        {
          attrType.AttrTypeNum = HTML_ATTR_HREF_;
          FetchIcon (doc, flags, DocumentMeta[doc]->link_icon,
                       currentURL, attrType);
        }
      /* search all elements having an attribute SRC */
      attrType1.AttrSSchema = attrType.AttrSSchema;
      attrType2.AttrSSchema = attrType.AttrSSchema;
      attrType1.AttrTypeNum = HTML_ATTR_SRC;
      attrType2.AttrTypeNum = HTML_ATTR_FrameSrc;
      FetchImages (doc, flags, elSubTree, currentURL,
                   attrType1, attrType2, loadImages, loadObjects);
    }

#ifdef _SVG
  /* Now, load all SVG images */
  /* prepare the attribute to be searched */
  attrType.AttrSSchema = TtaGetSSchema ("SVG", doc);
  if (attrType.AttrSSchema)
    {
      attrType.AttrTypeNum = SVG_ATTR_xlink_href;
      /* Search the next element having an attribute xlink_href */
      /* Start from the root element */
      FetchImages (doc, flags, elSubTree, currentURL,
                   attrType, attrType, loadImages, loadObjects);
    }
#endif /* _SVG */

  if (W3Loading != doc)
    stopped_flag = FALSE;
  else
    stopped_flag = TRUE;

  /* Restore the display mode */
  if (dispMode == DisplayImmediately)
    TtaSetDisplayMode (doc, dispMode);
  
  /* Images fetching is now finished */
  TtaFreeMemory (currentURL);
  
  return (stopped_flag);
}

 /*----------------------------------------------------------------------
   SelectPicture
   The user has clicked a PICTURE element. If it's the content of an
   object element, select the object
   -----------------------------------------------------------------------*/
 ThotBool SelectPicture (NotifyElement *event)
 {
   Element      parent;
   ElementType  elType;
   
   parent = TtaGetParent (event->element);
   if (parent)
     {
       elType = TtaGetElementType (parent);
       if (elType.ElTypeNum == HTML_EL_Object &&
           !strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
         {
           TtaSelectElement (event->document, parent);
           return TRUE;  /* don't do anything else */
         }
     }
   return FALSE; /* let Thot perform normal operation */
 }


 /*----------------------------------------------------------------------
   DeletePicture
   The user wants to delete a PICTURE element. If it's the content of an
   object element, cancel the operation
   -----------------------------------------------------------------------*/
 ThotBool DeletePicture (NotifyElement *event)
 {
   Element      parent, el;
   ElementType  elType;
   int          firstchar, lastchar;
   
   parent = TtaGetParent (event->element);
   if (parent)
     {
       elType = TtaGetElementType (parent);
       if (elType.ElTypeNum == HTML_EL_Object &&
           !strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
         {
           TtaGiveFirstSelectedElement (event->document, &el, &firstchar, &lastchar);
           /* accept to delete the picture when the object itself is deleted */
           if (el == event->element)
             return TRUE;   /* don't delete this picture */
         }
     }
   return FALSE; /* let Thot perform normal operation */
 }
