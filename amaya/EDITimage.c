/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2002
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya editing functions called form Thot and declared in HTML.A
 * These functions concern Image elements.
 *
 * Author: I. Vatton
 *         R. Guetari Unicode and Windows version.
 *
 */
 
/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "SVG.h"

static Document     BgDocument;
static int          RepeatValue;
static char       DirectoryImage[MAX_LENGTH];
static char       LastURLImage[MAX_LENGTH];
static char       ImageName[MAX_LENGTH];
static char       ImgAlt[MAX_LENGTH];

#include "AHTURLTools_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "EDITimage_f.h"
#include "HTMLimage_f.h"
#include "HTMLpresentation_f.h"
#include "init_f.h"
#include "html2thot_f.h"
#include "styleparser_f.h"

#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
   DeleteMap                                              
  ----------------------------------------------------------------------*/
ThotBool DeleteMap (NotifyElement * event)
{
   Element             image;
   ElementType	       elType;
   AttributeType       attrType;
   Attribute           attr;
   char               *url;
   int                 length;

   /* Search the refered image */
   elType = TtaGetElementType (event->element);
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = HTML_ATTR_Ref_IMG;
   attr = TtaGetAttribute (event->element, attrType);
   image = NULL;
   if (attr != NULL)
     {
       /* Search the IMAGE element associated with the MAP */
       length = MAX_LENGTH;
       url = TtaGetMemory (MAX_LENGTH);
       TtaGiveReferenceAttributeValue (attr, &image, url, &length);
       TtaFreeMemory (url);

       /* remove the attribute USEMAP of the image */
       attrType.AttrTypeNum = HTML_ATTR_USEMAP;
       attr = TtaGetAttribute (image, attrType);
       if (attr != NULL)
	 TtaRemoveAttribute (image, attr, event->document);
     }
  return FALSE;		/* let Thot perform normal operation */
}

/*----------------------------------------------------------------------
   CallbackImage
   Handle callbacks from the "Picture" and "Background image" forms.
  ----------------------------------------------------------------------*/
void CallbackImage (int ref, int typedata, char *data)
{
  Document           document;
  Element            el, elStyle, parent;
  Element            first, last;
  ElementType	     elType, parentType;
  LoadedImageDesc   *desc;
  char             tempfile[MAX_LENGTH];
  char             tempname[MAX_LENGTH];
  int                i, c1, cN;
  int                val;
  ThotBool           change;

  val = (int) data;
  switch (ref - BaseImage)
    {
    case FormAlt:
      break;
    case FormImage:
    case FormBackground:
      if (val == 2)
	{ 
	  /* Clear button */
	LastURLImage[0] = EOS;
#ifndef _WINDOWS
	TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
#endif /* !_WINDOWS */
	}
      else if (val == 3)
	{
	  /* Filter button */
	  /* reinitialize directories and document lists */
	  TtaListDirectory (DirectoryImage, ref,
			    TtaGetMessage (LIB, TMSG_DOC_DIR),
			    BaseImage + ImageDir,
			    ImgFilter,
			    TtaGetMessage (AMAYA, AM_FILES),
			    BaseImage + ImageSel);
	}
      else if (val == 0)
	{ /* Cancel button */ 
	  LastURLImage[0] = EOS;
	  TtaDestroyDialogue (ref);
	  BgDocument = 0;
	  /* Confirm button */
	}
      else if (ref - BaseImage == FormImage && ImgAlt[0] == EOS)
	{
	  /* IMG element without ALT attribute: error message */
#ifndef _WINDOWS
	  TtaNewLabel (BaseImage + ImageLabel4, BaseImage + FormImage,
		       TtaGetMessage (AMAYA, AM_ALT_MISSING));
#endif /* !_WINDOWS */
	}
      else if (ref == BaseImage + FormBackground && BgDocument != 0)
	{
	  TtaDestroyDialogue (ref);
	  /* save BgDocument because operation can be too long */
	  document = BgDocument;
	  /* get the first and last selected element */
	  TtaGiveFirstSelectedElement (document, &first, &c1, &i);
	  TtaGiveLastSelectedElement (document, &last, &i, &cN);
	  TtaOpenUndoSequence (document, first, last, c1, cN);
	
	  el = NULL;
	  if (first == NULL)
	    {
	      /* no current selection */
	      /* set the pRule on the root element */
	      el = TtaGetMainRoot (document);
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
	      if (elType.ElTypeNum == HTML_EL_HTML)
		{
		  elType.ElTypeNum = HTML_EL_BODY;
		  elStyle = TtaSearchTypedElement (elType, SearchInTree, el);
		  last = el;
		}
	      else if (elType.ElTypeNum == HTML_EL_BODY)
		{
		  /* move the pRule to the root element */
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
		    el = NULL;
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
			el = NULL;
		      else
			{
			  elType = TtaGetElementType (last);
			  if (elType.ElTypeNum == HTML_EL_MAP)
			    parent = el;
			  else
			    {
			      parentType.ElTypeNum = HTML_EL_MAP;
			      parent = TtaGetTypedAncestor (el, parentType);
			    }
			  if (parent != NULL)
			    el = NULL;
			}  
		    } 
		} 
	    } 
	  if (!el)
	    TtaSetStatus (document, 1,
			  TtaGetMessage(AMAYA, AM_BG_IMAGE_NOT_ALLOWED), NULL);
	  else
	    {
	      if (RepeatValue == 0)
		i = STYLE_REPEAT;
	      else if (RepeatValue == 1)
		i = STYLE_HREPEAT;
	      else if (RepeatValue == 2)
		i = STYLE_VREPEAT;
	      else
		i = STYLE_SCALE;
	      if (IsHTTPPath (DocumentURLs[document]) &&
		  !IsHTTPPath (LastURLImage))
		{
		/* load a local image into a remote document copy image file
		   into the temporary directory of the document */
		TtaExtractName (LastURLImage, tempfile, tempname);
		NormalizeURL (tempname, document, tempfile, tempname, NULL);
		AddLoadedImage (tempname, tempfile, document, &desc);
		if (desc)
		  {
		    desc->status = IMAGE_MODIFIED;
		    TtaFileCopy (LastURLImage, desc->localName);
		  }
		} 
	      do
		{
		  elType = TtaGetElementType (el);
		  /* if the PRule is on a text string or picture, move it to
		     the enclosing element */
		  if (elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
		      elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
		    {
		      el = TtaGetParent (el);
		      elStyle = el;
		      if (TtaIsAncestor (last, el))
			last = el;
		      elType = TtaGetElementType (el);
		    } 
		  /* if the PRule is on a Pseudo-Paragraph, move it to the
		     enclosing element */
		  if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph)
		    {
		      el = TtaGetParent (el);
		      elStyle = el;
		      if (TtaIsAncestor (last, el))
			last = el;
		    } 
		  if (LastURLImage[0] == EOS)
		    HTMLResetBackgroundImage (document, el);
		  else if (IsHTTPPath (DocumentURLs[document]) &&
			   !IsHTTPPath (LastURLImage))
		    HTMLSetBackgroundImage (document, el, i, tempname);
		  else
		    HTMLSetBackgroundImage (document, el, i, LastURLImage);
		  SetStyleAttribute (document, elStyle);
		  if (last == NULL || el == last)
		    el = NULL;
		  else
		    {
		      TtaGiveNextSelectedElement (document, &el, &c1,&cN);
		      elStyle = el;
		    }
		} while (el);
	    } 
	  TtaCloseUndoSequence (document);
	  TtaSetDocumentModified (document);
	}
      else
	TtaDestroyDialogue (ref);
      break;
    case RepeatImage:
      RepeatValue = val;
      break;
    case ImageFilter: /* Filter value */
      if (strlen(data) <= NAME_LENGTH)
	strcpy (ImgFilter, data);
#ifndef _WINDOWS
      else
	TtaSetTextForm (BaseImage + ImageFilter, ImgFilter);
#endif /* !_WINDOWS */
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
	  change = NormalizeFile (data, LastURLImage, AM_CONV_NONE);
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
    case ImageAlt:
      strncpy (ImgAlt, data, MAX_LENGTH - 1);
      ImgAlt[MAX_LENGTH - 1] = EOS;
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
#ifndef _WINDOWS
      TtaSetTextForm (BaseImage + ImageURL, DirectoryImage);
#endif /* !_WINDOWS */
      TtaListDirectory (DirectoryImage, BaseImage + FormImage,
			TtaGetMessage (LIB, TMSG_DOC_DIR), 
			BaseImage + ImageDir, ImgFilter,
			TtaGetMessage (AMAYA, AM_FILES), BaseImage + ImageSel);
      TtaListDirectory (DirectoryImage, BaseImage + FormBackground,
			TtaGetMessage (LIB, TMSG_DOC_DIR), 
			BaseImage + ImageDir, ImgFilter,
			TtaGetMessage (AMAYA, AM_FILES), BaseImage + ImageSel);
      ImageName[0] = EOS;
      break;
    case ImageSel:
      if (DirectoryImage[0] == EOS)
	/* set path on current directory */
	getcwd (DirectoryImage, MAX_LENGTH);
      /* construct the image full name */
      strcpy (LastURLImage, DirectoryImage);
      val = strlen (LastURLImage) - 1;
      if (LastURLImage[val] != DIR_SEP)
	strcat (LastURLImage, DIR_STR);
      strcat (LastURLImage, data);
#ifndef _WINDOWS
      TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
#endif /* !_WINDOWS */
      break;
    default:
      break;
    } 
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void InitImage (void)
{
   BaseImage = TtaSetCallback (CallbackImage, IMAGE_MAX_REF);
   RepeatValue = 0;
   LastURLImage[0] = EOS;
   strcpy (ImgFilter, "*.gif");
   /* set path on current directory */
   getcwd (DirectoryImage, MAX_LENGTH);
}


/*----------------------------------------------------------------------
   GetAlt gets the Alt value for an Area                            
  ----------------------------------------------------------------------*/
static void GetAlt (Document document, View view)
{
  ImgAlt[0] = EOS;
#ifndef _WINDOWS
  TtaNewForm (BaseImage + FormAlt, TtaGetViewFrame (document, view),
	       TtaGetMessage (AMAYA, AM_ALT),
	       TRUE, 1, 'L', D_DONE);
   TtaNewTextForm (BaseImage + ImageAlt, BaseImage + FormAlt,
		   TtaGetMessage (AMAYA, AM_ALT), 50, 1, TRUE);
   TtaNewLabel (BaseImage + ImageLabel4, BaseImage + FormAlt, " ");
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseImage + FormAlt, FALSE);
   TtaWaitShowDialogue ();
   while (ImgAlt[0] == EOS)
     {
       TtaNewLabel (BaseImage + ImageLabel4, BaseImage + FormAlt,
			   TtaGetMessage (AMAYA, AM_ALT_MISSING));
       TtaShowDialogue (BaseImage + FormAlt, FALSE);
       TtaWaitShowDialogue ();
     }
   TtaDestroyDialogue (BaseImage + FormAlt);   
#else  /* _WINDOWS */
   CreateAltDlgWindow ();
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  CreateAreaMap
  create an area in a map. shape indicates the shape of the area to be
  created:
  'R': rectangle
  'a': circle
  'p': polygon
  ----------------------------------------------------------------------*/
static void CreateAreaMap (Document doc, View view, char *shape)
{
   Element             el, map, parent, image, child, newElem;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr, attrRef, attrShape, attrRefimg;
   char                *url;
#ifdef _I18N_
   char                *tmp;
#endif
   int                 length, w, h;
   int                 firstchar, lastchar;
   int                 docModified;
   DisplayMode         dispMode;
   ThotBool            newMap;

   /* get the first selected element */
   TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
   if (el == NULL)
     /* no selection. Nothing to do */
     return;

   elType = TtaGetElementType (el);
   if (strcmp(TtaGetSSchemaName (elType.ElSSchema), "HTML") != 0)
     /* not within an HTML element. Nothing to do */
     return;

   docModified = TtaIsDocumentModified (doc);
   /* ask Thot to stop displaying changes made in the document */
   dispMode = TtaGetDisplayMode (doc);
   /*if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, DeferredDisplay);*/

   TtaOpenUndoSequence (doc, el, el, 0, 0);
   newElem = NULL;
   attrRefimg = NULL;
   newMap = FALSE;
   if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
     /* an image is selected. Create an area for it */
     {
        url = TtaGetMemory (MAX_LENGTH);
	image = el;
	/* Search the USEMAP attribute */
	attrType.AttrSSchema = elType.ElSSchema;
	attrType.AttrTypeNum = HTML_ATTR_USEMAP;
	attr = TtaGetAttribute (image, attrType);
	map = NULL;
	if (attr != NULL)
	  {
	     /* Search the MAP element associated with IMG element */
	     length = TtaGetTextAttributeLength (attr) + 1;
	     TtaGiveTextAttributeValue (attr, url, &length);
	     if (url[0] == '#')
		map = SearchNAMEattribute (doc, &url[1], NULL);
	  }
	if (map == NULL)
	  {
	     /* create the MAP element */
	     elType.ElTypeNum = HTML_EL_MAP;
	     map = TtaNewElement (doc, elType);
	     newMap = TRUE;
	     newElem = map;
	     parent = image;
	     do
	       {
		 el = parent;
		 parent = TtaGetParent (el);
		 if (parent)
		   elType = TtaGetElementType (parent);
	       }
	     while (parent && elType.ElTypeNum != HTML_EL_BODY &&
		    elType.ElTypeNum != HTML_EL_Division );
	     TtaInsertSibling (map, el, FALSE, doc);
	     CreateTargetAnchor (doc, map, FALSE, FALSE);
	     attrType.AttrTypeNum = HTML_ATTR_NAME;
	     attr = TtaGetAttribute (map, attrType);

	     /* create the USEMAP attribute for the image */
	     length = TtaGetTextAttributeLength (attr) + 1;
	     url[0] = '#';
	     TtaGiveTextAttributeValue (attr, &url[1], &length);
	     attrType.AttrTypeNum = HTML_ATTR_USEMAP;
	     attr = TtaGetAttribute (image, attrType);
	     if (attr == NULL)
	       {
		 attr = TtaNewAttribute (attrType);
		 TtaAttachAttribute (image, attr, doc);
	         TtaSetAttributeText (attr, url, image, doc);
		 TtaRegisterAttributeCreate (attr, image, doc);
	       }
	     else
	       {
		 TtaRegisterAttributeReplace (attr, image, doc);
	         TtaSetAttributeText (attr, url, image, doc);
	       }
	     /* create the Ref_IMG attribute */
	     attrType.AttrTypeNum = HTML_ATTR_Ref_IMG;
	     attrRefimg = TtaNewAttribute (attrType);
	     TtaAttachAttribute (map, attrRefimg, doc);
	     TtaSetAttributeReference (attrRefimg, map, doc, image, doc);
	  }
	TtaFreeMemory (url);
     }
   else
     /* the selected element is not an image */
     {
	/* is the selection within a MAP element ? */
	if (elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT)
	  {
	     el = TtaGetParent (el);
	     map = TtaGetParent (el);
	  }
	else if (elType.ElTypeNum == HTML_EL_AREA)
	   map = TtaGetParent (el);
	else if (elType.ElTypeNum == HTML_EL_MAP)
	   map = el;
	else
	   /* cannot create the AREA */
	   map = NULL;

	if (map)
	  {
	    /* Search the Ref_IMG attribute */
	    attrType.AttrSSchema = elType.ElSSchema;
	    attrType.AttrTypeNum = HTML_ATTR_Ref_IMG;
	    attr = TtaGetAttribute (map, attrType);
	    image = NULL;
	    if (attr != NULL)
	      {
		/* Search the IMAGE element associated with the MAP */
		length = MAX_LENGTH;
		url = TtaGetMemory (MAX_LENGTH);
		TtaGiveReferenceAttributeValue (attr, &image, url, &length);
		TtaFreeMemory (url);
	      }
	  }
     }

   if (map == NULL || image == NULL)
     /* Nothing to do. Just reset display mode */
     TtaSetDisplayMode (doc, dispMode);
   else
     /* Create an AREA element */
     {
	elType.ElTypeNum = HTML_EL_AREA;
	/* Should we ask the user to give coordinates */
	if (shape[0] == 'R' || shape[0] == 'a')
	   TtaAskFirstCreation ();

	el = TtaNewTree (doc, elType, "");
	if (!newElem)
	   newElem = el;
	child = TtaGetLastChild (map);
	if (child == NULL)
	   TtaInsertFirstChild (&el, map, doc);
	else
	   TtaInsertSibling (el, child, FALSE, doc);
	child = TtaGetFirstChild (el);
	/* For polygons, sets the value after the Ref_IMG attribute is
	   created */
	if (shape[0] != 'p')
	   TtaSetGraphicsShape (child, shape[0], doc);

	/* create the shape attribute */
	attrType.AttrTypeNum = HTML_ATTR_shape;
	attrShape = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attrShape, doc);

	/* Create the coords attribute */
	attrType.AttrTypeNum = HTML_ATTR_coords;
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);

	if (shape[0] == 'R')
	   TtaSetAttributeValue (attrShape, HTML_ATTR_shape_VAL_rectangle,
				 el, doc);
	else if (shape[0] == 'a')
	   TtaSetAttributeValue (attrShape, HTML_ATTR_shape_VAL_circle,
				 el, doc);
	else if (shape[0] == 'p')
	  {
	     /* create the AreaRef_IMG attribute */
	     attrType.AttrTypeNum = HTML_ATTR_AreaRef_IMG;
	     attrRef = TtaNewAttribute (attrType);
	     TtaAttachAttribute (el, attrRef, doc);
	     TtaSetAttributeReference (attrRef, el, doc, image, doc);
	     TtaSetAttributeValue (attrShape, HTML_ATTR_shape_VAL_polygon,
				   el, doc);
	     TtaGiveBoxSize (image, doc, 1, UnPixel, &w, &h);
	     TtaChangeBoxSize (child, doc, 1, w, h, UnPixel);
	  }
	/* ask Thot to display changes made in the document */
	TtaSetDisplayMode (doc, dispMode);
	TtaSelectElement (doc, child);
	if (shape[0] == 'p')
	  {
	    TtcInsertGraph (doc, 1, 'p');
	    if (TtaGetElementVolume (child) < 3)
	      {
		/* the polyline doesn't have enough points */
		if (newMap)
		  TtaDeleteTree (map, doc);
		else
		  TtaDeleteTree (el, doc);
		TtaCancelLastRegisteredSequence (doc);
		if (!docModified)
		  TtaSetDocumentUnmodified (doc);
		TtaSelectElement (doc, image);
		return;
	      }
	  }
	/* Compute coords attribute */
	SetAreaCoords (doc, el, 0);

	/* create the attribute ALT */
	attrType.AttrTypeNum = HTML_ATTR_ALT;
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);
	GetAlt (doc, view);
#ifdef _I18N_
	tmp = TtaConvertByteToMbs (ImgAlt, TtaGetDefaultCharset ());
	TtaSetAttributeText (attr, tmp, el, doc);
	TtaFreeMemory (tmp);
#else /* _I18N_ */
	TtaSetAttributeText (attr, ImgAlt, el, doc);
#endif /* _I18N_ */
	ImgAlt[0] = EOS;
	/* The link element is a new created one */
	IsNewAnchor = TRUE;
	/* FrameUpdating creation of Area and selection of destination */
	SelectDestination (doc, el, FALSE);
     }
   if (newElem)
      TtaRegisterElementCreate (newElem, doc);
   /* if a map has been created, register its Ref_IMG attribute to
      avoid troubles when Undoing the command: function DeleteMap
      would delete the USEMAP attribute from the IMG otherwise.
      Undo already deletes this attribute! */
   if (attrRefimg)
      TtaRegisterAttributeCreate (attrRefimg, map, doc);
   TtaCloseUndoSequence (doc);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateAreaRect (Document doc, View view)
{
   CreateAreaMap (doc, view, "R");
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateAreaCircle (Document doc, View view)
{
   CreateAreaMap (doc, view, "a");
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CreateAreaPoly (Document doc, View view)
{
   CreateAreaMap (doc, view, "p");
}

/*----------------------------------------------------------------------
   GetImageURL initializes the Picture form                             
  ----------------------------------------------------------------------*/
char *GetImageURL (Document document, View view)
{
#ifndef _WINDOWS
   LoadedImageDesc   *desc;
   char               tempfile[MAX_LENGTH];
   char               s[MAX_LENGTH];
   int                 i;

   /* Dialogue form for open URL or local */
   i = 0;
   strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));

   TtaNewSheet (BaseImage + FormImage, TtaGetViewFrame (document, view),
		TtaGetMessage (AMAYA, AM_BUTTON_IMG),
		3, s, TRUE, 2, 'L', D_CANCEL);
   TtaNewTextForm (BaseImage + ImageURL, BaseImage + FormImage,
		   TtaGetMessage (AMAYA, AM_BUTTON_IMG), 50, 1, FALSE);
   TtaNewLabel (BaseImage + ImageLabel, BaseImage + FormImage, " ");
   TtaNewTextForm (BaseImage + ImageAlt, BaseImage + FormImage,
		   TtaGetMessage (AMAYA, AM_ALT), 50, 1, TRUE);
   TtaNewLabel (BaseImage + ImageLabel2, BaseImage + FormImage, " ");
   TtaListDirectory (DirectoryImage, BaseImage + FormImage,
		     TtaGetMessage (AMAYA, AM_IMAGES_LOCATION),
		     BaseImage + ImageDir, ImgFilter,
		     TtaGetMessage (AMAYA, AM_FILES), BaseImage + ImageSel);
   if (LastURLImage[0] != EOS)
      TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
   else
     {
	strcpy (LastURLImage, DirectoryImage);
	strcat (LastURLImage, DIR_STR);
	strcat (LastURLImage, ImageName);
	TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
     }
   ImgAlt[0] = EOS;
   TtaSetTextForm (BaseImage + ImageAlt, ImgAlt);
   TtaNewTextForm (BaseImage + ImageFilter, BaseImage + FormImage,
		   TtaGetMessage (AMAYA, AM_PARSE), 10, 1, TRUE);
   TtaSetTextForm (BaseImage + ImageFilter, ImgFilter);
   TtaNewLabel (BaseImage + ImageLabel3, BaseImage + FormImage, " ");
   TtaNewLabel (BaseImage + ImageLabel4, BaseImage + FormImage, " ");
   TtaSetDialoguePosition ();
   TtaShowDialogue (BaseImage + FormImage, FALSE);
   TtaWaitShowDialogue ();
   if (IsHTTPPath (DocumentURLs[document]) && !IsHTTPPath (LastURLImage))
     {
       /*
	 load a local image into a remote document 
	 copy image file into the temporary directory of the document
	 */
       TtaExtractName (LastURLImage, tempfile, s);
       if (s[0] == EOS)
	 return (LastURLImage);
       else
	 {
	   NormalizeURL (s, document, tempfile, ImageName, NULL);
	   AddLoadedImage (ImageName, tempfile, document, &desc);
	   desc->status = IMAGE_MODIFIED;
	   TtaFileCopy (LastURLImage, desc->localName);
	   return (ImageName);
	 }
     }
   else
     return (LastURLImage);
#else /* _WINDOWS */
   CreateOpenImgDlgWindow (TtaGetViewFrame (document, view), LastURLImage, -1,
			   -1, 1) ;
   return (LastURLImage);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  ChangeBackgroundImage
  display a form to set or change the background image
 -----------------------------------------------------------------------*/
void ChangeBackgroundImage (Document document, View view)
{
   char           *s = TtaGetMemory (MAX_LENGTH);
#ifndef _WINDOWS
   int             i;

   /* there is a selection */
   i = 0;
   strcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += strlen (&s[i]) + 1;
   strcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));

   TtaNewSheet (BaseImage + FormBackground, TtaGetViewFrame(document, view),
		TtaGetMessage (AMAYA, AM_BACKGROUND_IMAGE), 3, s, TRUE, 2,
		'L', D_CANCEL);
   TtaNewTextForm (BaseImage + ImageURL, BaseImage + FormBackground,
		   TtaGetMessage (AMAYA, AM_BACKGROUND_IMAGE), 50, 1, TRUE);
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
#else /* _WINDOWS */
   if (LastURLImage[0] != EOS)
      strcpy (s, LastURLImage);
   else {
      strcpy (s, DirectoryImage);
      strcat (s, DIR_STR);
      strcat (s, ImageName);
   }
   BgDocument = document;
   CreateBackgroundImageDlgWindow (TtaGetViewFrame (document, view), s);
#endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
   ComputeSRCattribute generates the value of the src attribute of element
   el if it's a HTML img, or the xlink:href attribute if it's a SVG image.
   doc the document to which el belongs.
   sourceDocument is the document where the image comes from.
   attr is the src or xlink:href attribute that has to be updated.
   text is the image name (relative or not).
  ----------------------------------------------------------------------*/
void ComputeSRCattribute (Element el, Document doc, Document sourceDocument,
			  Attribute attr, char *text)
{
  Element            pict;
  ElementType        elType;
  LoadedImageDesc   *desc;
  char              *value, *base;
  char               pathimage[MAX_LENGTH];
  char               localname[MAX_LENGTH];
  char               imagename[MAX_LENGTH];

  elType = TtaGetElementType (el);
  if (strcmp (TtaGetSSchemaName (elType.ElSSchema), "SVG"))
    /* it's not a SVG element, it's then a HTML img element, which is
       itself a Thot picture element */
    pict = el;
  else
    /* it's a SVG image. The Thot picture element is one of its children */
    {
      elType.ElTypeNum = SVG_EL_PICTURE_UNIT;
      pict = TtaSearchTypedElement (elType, SearchInTree, el);
      if (!pict)
	/* no Thot picture element. Create one */
	{
	  pict = TtaNewTree (doc, elType, "");
	  TtaInsertFirstChild (&el, pict, doc);
	}
    }

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
	  TtaSetTextContent (pict, desc->localName, SPACE, doc);
	  DisplayImage (doc, pict, desc->localName, NULL);
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
	  FetchImage (doc, pict, NULL, 0, NULL, NULL);
	  ResetStop (doc);
	}
    }
  else
    {
      /* local target document */
      NormalizeURL (text, sourceDocument, pathimage, imagename, NULL);
      if (!IsHTTPPath (pathimage))
	{
	  /* load a local image into a local document */
	  base = GetBaseURL (doc);
	  value = MakeRelativeURL (pathimage, base);
	  TtaSetAttributeText (attr, value, el, doc);
	  TtaFreeMemory (base);
	  TtaFreeMemory (value);
	  /* set the element content */
	  TtaSetTextContent (pict, pathimage, SPACE, doc);
	  DisplayImage (doc, pict, pathimage, NULL);
	}
      else
	{
	  /* load a remote image into a local document */
	  /* set stop button */
	  ActiveTransfer (doc);
	  TtaSetAttributeText (attr, pathimage, el, doc);
	  FetchImage (doc, pict, NULL, 0, NULL, NULL);
	  ResetStop (doc);
	}
    }
}

/*----------------------------------------------------------------------
   UpdateSRCattribute  creates or updates the SRC attribute value	
   		when the contents of element IMG is set.		
  ----------------------------------------------------------------------*/
void UpdateSRCattribute (NotifyElement *event)
{
  AttributeType      attrType;
  Attribute          attr;
  Element            elSRC, el;
  Document           doc;
  char*            text;
  char*            pathimage;
  char*            tmp;

   el = event->element;
   doc = event->document;

   /* if it's not an HTML picture (it could be an SVG image for instance),
      ignore */
   if (strcmp(TtaGetSSchemaName (event->elementType.ElSSchema), "HTML"))
     return;

   /* Select an image name */
   text = GetImageURL (doc, 1);
   if (text == NULL || text[0] == EOS)
     {
	/* delete the empty PICTURE element */
	TtaDeleteTree (el, doc);
	return;
     }
   elSRC = TtaGetParent (el);
   if (elSRC != NULL)
      elSRC = el;
   /* add the ALT attribute */
   attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
   attrType.AttrTypeNum = HTML_ATTR_ALT;
   attr = TtaGetAttribute (elSRC, attrType);
   if (attr == 0)
     {
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (elSRC, attr, doc);
     }
   /* copy image name in ALT attribute */
   if (ImgAlt[0] == EOS)
     {
       tmp = TtaGetMemory (MAX_LENGTH);
       pathimage = TtaGetMemory (MAX_LENGTH);
       strcpy (tmp, " ");
       TtaExtractName (text, pathimage, &tmp[1]);
       strcat (tmp, " ");
       TtaSetAttributeText (attr, tmp, elSRC, doc);
       TtaFreeMemory (pathimage);
       TtaFreeMemory (tmp);
     }
   else
     {
#ifdef _I18N_
       tmp = TtaConvertByteToMbs (ImgAlt, TtaGetDefaultCharset ());
       TtaSetAttributeText (attr, tmp, elSRC, doc);
       TtaFreeMemory (tmp);
#else /* _I18N_ */
       TtaSetAttributeText (attr, ImgAlt, elSRC, doc);
#endif /* _I18N_ */
       ImgAlt[0] = EOS;
     }
   /* search the SRC attribute */
   attrType.AttrTypeNum = HTML_ATTR_SRC;
   attr = TtaGetAttribute (elSRC, attrType);
   if (attr == 0)
     {
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (elSRC, attr, doc);
     }
   ComputeSRCattribute (elSRC, doc, 0, attr, text);
}


/*----------------------------------------------------------------------
   SvgImageCreated
   The user is creating an SVG image. Ask for the mandatory attributes
   and associate them with the new image.
  ----------------------------------------------------------------------*/
void SvgImageCreated (NotifyElement *event)
{
  AttributeType      attrType;
  Attribute          attr;
  ElementType        elType;
  Element            el, desc, leaf;
  Document           doc;
  char              *text;
  char              *pathimage;
  char              *imagename;

   el = event->element;
   doc = event->document;
   /* display the Image form and get the user feedback */
   text = GetImageURL (doc, 1);
   if (text == NULL || text[0] == EOS)
     {
	/* delete the empty image element */
	TtaDeleteTree (el, doc);
	return;
     }
   elType = TtaGetElementType (el);
   /* set the desc child */
   elType.ElTypeNum = SVG_EL_desc;
   desc = TtaSearchTypedElement (elType, SearchInTree, el);
   if (!desc)
     {
	desc = TtaNewTree (doc, elType, "");
	TtaInsertFirstChild (&el, desc, doc);
     }
   leaf = TtaGetFirstChild (desc);
   if (ImgAlt[0] == EOS)
     /* The user has not provided any alternate name. Copy the image name in
	the desc element */
     {
       imagename = TtaGetMemory (MAX_LENGTH);
       pathimage = TtaGetMemory (MAX_LENGTH);
       strcpy (imagename, " ");
       TtaExtractName (text, pathimage, &imagename[1]);
       strcat (imagename, " ");
       /* set the element content */
       TtaSetTextContent (leaf, imagename, SPACE, doc);
       TtaFreeMemory (pathimage);
       TtaFreeMemory (imagename);
     }
   else
     {
       TtaSetTextContent (leaf, ImgAlt, SPACE, doc);
       ImgAlt[0] = EOS;
     }
   /* search the xlink:href attribute */
   attrType.AttrSSchema = elType.ElSSchema;
   attrType.AttrTypeNum = SVG_ATTR_xlink_href;
   attr = TtaGetAttribute (el, attrType);
   if (attr == 0)
     {
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);
     }
   ComputeSRCattribute (el, doc, 0, attr, text);
}


/*----------------------------------------------------------------------
   SRCattrModified updates the contents of element IMG according   
   to the new value of attribute SRC.                      
  ----------------------------------------------------------------------*/
void                SRCattrModified (NotifyAttribute *event)
{
   Element             el;
   Attribute           attr;
   Document            doc;
   int                 length;
   char               *buf1, *buf2;
   char               *localname, *imageName;
   LoadedImageDesc    *desc;

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
  CreateImage
  ----------------------------------------------------------------------*/
void CreateImage (Document doc, View view)
{
  Element            sibling;
  ElementType        elType;
  char              *name;
  int                c1, i;

  TtaGiveFirstSelectedElement (doc, &sibling, &c1, &i); 
  if (sibling)
    {
      /* Get the type of the first selected element */
      elType = TtaGetElementType (sibling);
      name = TtaGetSSchemaName (elType.ElSSchema);
      if (!strcmp (name, "SVG"))
	elType.ElTypeNum = SVG_EL_image;
      else
	{
	  elType.ElSSchema = TtaGetSSchema ("HTML", doc);
	  elType.ElTypeNum = HTML_EL_PICTURE_UNIT;
	}
      TtaCreateElement (elType, doc);
    }
}

/*----------------------------------------------------------------------
   AddLocalImage adds a new local image into image descriptor table   
   with the purpose of having it saved through the Net later.    
   This function copy the image in the TempFileDirectory.        
   fullname is the complete path to the local file.              
   name is the name of the local file.                           
   url is the complete URL of the distant location.              
  ----------------------------------------------------------------------*/
ThotBool AddLocalImage (char *fullname, char *name, char *url, Document doc,
			LoadedImageDesc **desc)
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
	  /* clear the structure */
	  memset ((void *) pImage, 0, sizeof (LoadedImageDesc));
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
	  pImage->imageType = unknown_type;
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
void                RemoveDocumentImages (Document doc)
{
   LoadedImageDesc    *pImage, *previous, *next;
   ElemImage          *ctxEl, *ctxPrev;
   char               *ptr;

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
