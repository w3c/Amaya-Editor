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
 *         R. Guetari Unicode and Windows version.
 *
 */
 
/* Included headerfiles */
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"

#define ImageURL	1
#define ImageLabel	2
#define ImageLabel2	3
#define ImageLabel3	4
#define ImageLabel4	5
#define ImageDir	6
#define ImageSel	7
#define ImageFilter     8
#define FormImage	9
#define RepeatImage    10
#define FormBackground 11
#define ImageAlt       12
#define FormAlt        13
#define IMAGE_MAX_REF  14

static Document     BgDocument;
static int          BaseImage;
static int          RepeatValue;
static CHAR_T       DirectoryImage[MAX_LENGTH];
static CHAR_T       LastURLImage[MAX_LENGTH];
static CHAR_T       ImageName[MAX_LENGTH];
static CHAR_T       ImgFilter[NAME_LENGTH];
static CHAR_T       ImgAlt[NAME_LENGTH];

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
#ifdef __STDC__
ThotBool            DeleteMap (NotifyElement * event)
#else
ThotBool            DeleteMap (event)
NotifyElement      *event;

#endif
{
   Element             image;
   ElementType	       elType;
   AttributeType       attrType;
   Attribute           attr;
   STRING              url;
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
       url = TtaAllocString (MAX_LENGTH);
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
#ifdef __STDC__
void                CallbackImage (int ref, int typedata, STRING data)
#else  /* __STDC__ */
void                CallbackImage (ref, typedata, data)
int                 ref;
int                 typedata;
STRING              data;

#endif /* __STDC__ */
{
  Document           document;
  Element            el, elStyle, parent;
  Element            first, last;
  ElementType	     elType, parentType;
  LoadedImageDesc   *desc;
  CHAR_T             tempfile[MAX_LENGTH];
  CHAR_T             tempname[MAX_LENGTH];
  int                i, c1, cN;
  int                val;
  ThotBool           change;

  val = (int) data;
  switch (ref - BaseImage) {
         case FormAlt:
              break;
         case FormImage:
         case FormBackground:
	   if (val == 2) { /* Clear button */
	     LastURLImage[0] = WC_EOS;
#ifndef _WINDOWS
	     TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
#endif /* !_WINDOWS */
	   }
	   else if (val == 3) { /* Filter button */
	     /* reinitialize directories and document lists */
	     TtaListDirectory (DirectoryImage, ref, TtaGetMessage (LIB, TMSG_DOC_DIR), BaseImage + ImageDir,
			       ImgFilter, TtaGetMessage (AMAYA, AM_FILES), BaseImage + ImageSel);
	   }
	   else if (val == 0) { /* Cancel button */ 
	     LastURLImage[0] = WC_EOS;
	     TtaDestroyDialogue (ref);
	     BgDocument = 0;
	     /* Confirm button */
	   }
	   else if (ref - BaseImage == FormImage && ImgAlt[0] == WC_EOS) { /* IMG element without ALT attribute: error message */
#                    ifndef _WINDOWS
	     TtaNewLabel (BaseImage + ImageLabel4, BaseImage + FormImage, TtaGetMessage (AMAYA, AM_ALT_MISSING))
#                    endif /* !_WINDOWS */
	       ; 
	   }
	   else if (ref == BaseImage + FormBackground && BgDocument != 0) { /* save BgDocument because operation can be too long */
	     document = BgDocument;
	     /* get the first and last selected element */
	     TtaGiveFirstSelectedElement (document, &first, &c1, &i);
	     TtaGiveLastSelectedElement (document, &last, &i, &cN);
	     TtaOpenUndoSequence (document, first, last, c1, cN);
	     
	     el = NULL;
	     if (first == NULL) { /* no current selection */
	       /* set the pRule on the root element */
	       el = TtaGetMainRoot (document);
	       elType.ElSSchema = TtaGetDocumentSSchema (document);
	       elType.ElTypeNum = HTML_EL_BODY;
	       /* set the style on body element */
	       elStyle = TtaSearchTypedElement (elType, SearchInTree, el);
	       last = el;
	     }
	     else {
	       elStyle = el = first;
	       elType = TtaGetElementType (el);
	       if (elType.ElTypeNum == HTML_EL_HTML) {
		 elType.ElTypeNum = HTML_EL_BODY;
		 elStyle = TtaSearchTypedElement (elType, SearchInTree, el);
		 last = el;
	       } else if (elType.ElTypeNum == HTML_EL_BODY) {
		 /* move the pRule on the root element */
		 el =  TtaGetMainRoot (document);
		 last = el;
	       } else {
		 /* style is not allowed in Head section */
		 if (elType.ElTypeNum == HTML_EL_HEAD)
		   parent = el;
		 else {
		   parentType.ElSSchema = elType.ElSSchema;
		   parentType.ElTypeNum = HTML_EL_HEAD;
		   parent = TtaGetTypedAncestor (el, parentType);
		 } 
		 if (parent != NULL)
		   el = NULL;
		 else { 
		   /* style is not allowed in MAP */
		   if (elType.ElTypeNum == HTML_EL_MAP)
		     parent = el;
		   else {
		     parentType.ElTypeNum = HTML_EL_MAP;
		     parent = TtaGetTypedAncestor (el, parentType);
		   } 
		   if (parent != NULL)
		     el = NULL;
		   else {
		     elType = TtaGetElementType (last);
		     if (elType.ElTypeNum == HTML_EL_MAP)
		       parent = el;
		     else {
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
	       TtaSetStatus (document, 1, TtaGetMessage(AMAYA, AM_BG_IMAGE_NOT_ALLOWED), NULL);
                     else {
		       if (RepeatValue == 0)
			 i = STYLE_REPEAT;
		       else if (RepeatValue == 1)
			 i = STYLE_HREPEAT;
		       else if (RepeatValue == 2)
			 i = STYLE_VREPEAT;
		       else
			 i = STYLE_SCALE;
		       if (IsHTTPPath (DocumentURLs[document]) && !IsHTTPPath (LastURLImage)) {
			 /* load a local image into a remote document copy image file into the temporary directory of the document */
			 TtaExtractName (LastURLImage, tempfile, tempname);
			 NormalizeURL (tempname, document, tempfile, tempname, NULL);
			 AddLoadedImage (tempname, tempfile, document, &desc);
			 if (desc) {
			   desc->status = IMAGE_MODIFIED;
			   TtaFileCopy (LastURLImage, desc->localName);
			 }
		       } 
		       do {
			 elType = TtaGetElementType (el);
			 /* if the PRule is on a text string or picture, move it to the enclosing element */
			 if (elType.ElTypeNum == HTML_EL_TEXT_UNIT || elType.ElTypeNum == HTML_EL_PICTURE_UNIT) {
			   el = TtaGetParent (el);
			   elStyle = el;
			   if (TtaIsAncestor (last, el))
			     last = el;
			   elType = TtaGetElementType (el);
			 } 
			 /* if the PRule is on a Pseudo-Paragraph, move it to the enclosing element */
			 if (elType.ElTypeNum == HTML_EL_Pseudo_paragraph) {
			   el = TtaGetParent (el);
			   elStyle = el;
			   if (TtaIsAncestor (last, el))
			     last = el;
			 } 
			 if (LastURLImage[0] == WC_EOS)
			   HTMLResetBackgroundImage (document, el);
			 else if (IsHTTPPath (DocumentURLs[document]) && !IsHTTPPath (LastURLImage))
			   HTMLSetBackgroundImage (document, el, i, tempname);
			 else
			   HTMLSetBackgroundImage (document, el, i, LastURLImage);
			 SetStyleAttribute (document, elStyle);
			 if (last == NULL || el == last)
			   el = NULL;
			 else {
			   TtaGiveNextSelectedElement (document, &el, &c1,&cN);
			   elStyle = el;
			 }
		       } while (el);
		     } 
	     TtaCloseUndoSequence (document);
	     TtaSetDocumentModified (document);
	   } else
	     TtaDestroyDialogue (ref);
	   break;
         case RepeatImage:
              RepeatValue = val;
              break;
         case ImageFilter: /* Filter value */
              if (ustrlen(data) <= NAME_LENGTH)
                 ustrcpy (ImgFilter, data);
#             ifndef _WINDOWS
              else
                  TtaSetTextForm (BaseImage + ImageFilter, ImgFilter);
#             endif /* !_WINDOWS */
              break;
         case ImageURL:
              if (data == NULL)
                 break;
              if (IsW3Path (data)) 
		{
		  /* save the URL name */
		  ustrcpy (LastURLImage, data);
		  ImageName[0] = WC_EOS;
		} 
	      else 
		{
		  change = NormalizeFile (data, LastURLImage, AM_CONV_NONE);
		  if (TtaCheckDirectory (LastURLImage)) 
		    {
		      ustrcpy (DirectoryImage, LastURLImage);
		      ImageName[0] = WC_EOS;
		      LastURLImage[0] = WC_EOS;
		    } 
		  else
		    TtaExtractName (LastURLImage, DirectoryImage, ImageName);
		}
              break;
         case ImageAlt:
              ustrncpy (ImgAlt, data, NAME_LENGTH-1);
              ImgAlt[NAME_LENGTH-1] = WC_EOS;
              break;
         case ImageDir:
              if (!ustrcmp (data, TEXT(".."))) {
                 /* suppress last directory */
                 ustrcpy (tempname, DirectoryImage);
                 TtaExtractName (tempname, DirectoryImage, tempfile);
			  } else {
                     ustrcat (DirectoryImage, WC_DIR_STR);
                     ustrcat (DirectoryImage, data);
			  }
#             ifndef _WINDOWS
              TtaSetTextForm (BaseImage + ImageURL, DirectoryImage);
#             endif /* !_WINDOWS */
              TtaListDirectory (DirectoryImage, BaseImage + FormImage, TtaGetMessage (LIB, TMSG_DOC_DIR), 
                                BaseImage + ImageDir, ImgFilter, TtaGetMessage (AMAYA, AM_FILES), BaseImage + ImageSel);
              TtaListDirectory (DirectoryImage, BaseImage + FormBackground, TtaGetMessage (LIB, TMSG_DOC_DIR), 
                                BaseImage + ImageDir, ImgFilter, TtaGetMessage (AMAYA, AM_FILES), BaseImage + ImageSel);
              ImageName[0] = WC_EOS;
              break;
         case ImageSel:
              if (DirectoryImage[0] == WC_EOS) {
                 /* set path on current directory */
                 ugetcwd (DirectoryImage, MAX_LENGTH);
			  } 
              /* construct the image full name */
              ustrcpy (LastURLImage, DirectoryImage);
              val = ustrlen (LastURLImage) - 1;
              if (LastURLImage[val] != WC_DIR_SEP)
                 ustrcat (LastURLImage, WC_DIR_STR);
              ustrcat (LastURLImage, data);
#             ifndef _WINDOWS
              TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
#             endif /* !_WINDOWS */
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
   LastURLImage[0] = WC_EOS;
   ustrcpy (ImgFilter, TEXT(".gif"));
   /* set path on current directory */
   ugetcwd (DirectoryImage, MAX_LENGTH);
}


/*----------------------------------------------------------------------
   GetAlt gets the Alt value for an Area                            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         GetAlt (Document document, View view)
#else  /* __STDC__ */
static void         GetAlt (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
  ImgAlt[0] = WC_EOS;
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
   while (ImgAlt[0] == WC_EOS)
     {
       TtaNewLabel (BaseImage + ImageLabel4, BaseImage + FormAlt,
			   TtaGetMessage (AMAYA, AM_ALT_MISSING));
       TtaShowDialogue (BaseImage + FormAlt, FALSE);
       TtaWaitShowDialogue ();
     }
   TtaDestroyDialogue (BaseImage + FormAlt);   
#  else  /* _WINDOWS */
   CreateAltDlgWindow (BaseImage, FormAlt, ImageAlt, ImageLabel4, TtaGetMessage (AMAYA, AM_ALT), TtaGetMessage (AMAYA, AM_ALT_MISSING));
#  endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
  CreateAreaMap
  create an area in a map. shape indicates the shape of the area to be
  created:
  'R': rectangle
  'a': circle
  'p': polygon
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CreateAreaMap (Document doc, View view, char* shape)
#else  /* __STDC__ */
static void         CreateAreaMap (document, view, shape)
Document            document;
View                view;
char*               shape;

#endif /* __STDC__ */
{
   Element             el, map, parent, image, child, newElem;
   ElementType         elType;
   AttributeType       attrType;
   Attribute           attr, attrRef, attrShape, attrRefimg;
   STRING              url;
   int                 length, w, h;
   int                 firstchar, lastchar;
   DisplayMode         dispMode;

   /* get the first selected element */
   TtaGiveFirstSelectedElement (doc, &el, &firstchar, &lastchar);
   if (el == NULL)
     /* no selection. Nothing to do */
     return;

   elType = TtaGetElementType (el);
   if (ustrcmp(TtaGetSSchemaName (elType.ElSSchema), TEXT("HTML")) != 0)
     /* not within an HTML element. Nothing to do */
     return;

   /* ask Thot to stop displaying changes made in the document */
   dispMode = TtaGetDisplayMode (doc);
   if (dispMode == DisplayImmediately)
     TtaSetDisplayMode (doc, DeferredDisplay);

   TtaOpenUndoSequence (doc, el, el, 0, 0);
   newElem = NULL;
   attrRefimg = NULL;

   if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
     /* an image is selected. Create an area for it */
     {
        url = TtaAllocString (MAX_LENGTH);
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
	     newElem = map;
	     parent = image;
	     do
	       {
		  el = parent;
		  parent = TtaGetParent (el);
		  elType = TtaGetElementType (parent);
	       }
	     while (elType.ElTypeNum != HTML_EL_BODY);
	     TtaInsertSibling (map, el, FALSE, doc);
	     CreateTargetAnchor (doc, map, FALSE);
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
		url = TtaAllocString (MAX_LENGTH);
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
	/* For polygons, sets the value after the Ref_IMG attribute is created */
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
	     /*TtaChangeLimitOfPolyline (child, UnPixel, w, h, doc);*/
	  }
	/* ask Thot to display changes made in the document */
	TtaSetDisplayMode (doc, dispMode);
	TtaSelectElement (doc, child);
	if (shape[0] == 'p')
	   TtcInsertGraph (doc, 1, 'p');
	/* Compute coords attribute */
	SetAreaCoords (doc, el, 0);

	/* create the attribute ALT */
	attrType.AttrTypeNum = HTML_ATTR_ALT;
	attr = TtaNewAttribute (attrType);
	TtaAttachAttribute (el, attr, doc);
	GetAlt (doc, view);
	TtaSetAttributeText (attr, ImgAlt, el, doc);
	ImgAlt[0] = WC_EOS;
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
#ifdef __STDC__
void                CreateAreaRect (Document doc, View view)
#else  /* __STDC__ */
void                CreateAreaRect (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateAreaMap (doc, view, "R");
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateAreaCircle (Document doc, View view)
#else  /* __STDC__ */
void                CreateAreaCircle (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateAreaMap (doc, view, "a");
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CreateAreaPoly (Document doc, View view)
#else  /* __STDC__ */
void                CreateAreaPoly (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   CreateAreaMap (doc, view, "p");
}

/*----------------------------------------------------------------------
   GetImageURL initializes the Picture form                             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
STRING              GetImageURL (Document document, View view)
#else  /* __STDC__ */
STRING              GetImageURL (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
#  ifndef _WINDOWS
   LoadedImageDesc   *desc;
   char               tempfile[MAX_LENGTH];
   char               s[MAX_LENGTH];
   int                 i;

   /* Dialogue form for open URL or local */
   i = 0;
   ustrcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += ustrlen (&s[i]) + 1;
   ustrcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += ustrlen (&s[i]) + 1;
   ustrcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));

   TtaNewSheet (BaseImage + FormImage, TtaGetViewFrame (document, view), TtaGetMessage (AMAYA, AM_BUTTON_IMG),
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
   if (LastURLImage[0] != WC_EOS)
      TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
   else
     {
	ustrcpy (LastURLImage, DirectoryImage);
	ustrcat (LastURLImage, WC_DIR_STR);
	ustrcat (LastURLImage, ImageName);
	TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
     }
   ImgAlt[0] = WC_EOS;
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
#  else /* _WINDOWS */
   CreateOpenImgDlgWindow (TtaGetViewFrame (document, view), LastURLImage, BaseImage, FormImage, ImageAlt, -1, -1, 1) ;
   return (LastURLImage);
#  endif /* _WINDOWS */
}


/*----------------------------------------------------------------------
  ChangeBackgroundImage
  display a form to set or change the background image
 -----------------------------------------------------------------------*/
#ifdef __STDC__
void ChangeBackgroundImage (Document document, View view)
#else /* __STDC__*/
void ChangeBackgroundImage (document, view)
     Document document;
     View view;
#endif /* __STDC__*/
{
   STRING           s = TtaAllocString (MAX_LENGTH); 
#  ifndef _WINDOWS
   int                 i;

   /* there is a selection */
   i = 0;
   ustrcpy (&s[i], TtaGetMessage (LIB, TMSG_LIB_CONFIRM));
   i += ustrlen (&s[i]) + 1;
   ustrcpy (&s[i], TtaGetMessage (AMAYA, AM_CLEAR));
   i += ustrlen (&s[i]) + 1;
   ustrcpy (&s[i], TtaGetMessage (AMAYA, AM_PARSE));

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
   if (LastURLImage[0] != WC_EOS)
      TtaSetTextForm (BaseImage + ImageURL, LastURLImage);
   else
     {
	ustrcpy (s, DirectoryImage);
	ustrcat (s, DIR_STR);
	ustrcat (s, ImageName);
	TtaSetTextForm (BaseImage + ImageURL, s);
     }

   TtaNewTextForm (BaseImage + ImageFilter, BaseImage + FormBackground,
		   TtaGetMessage (AMAYA, AM_PARSE), 10, 1, TRUE);
   TtaSetTextForm (BaseImage + ImageFilter, ImgFilter);
   /* selector for repeat mode */
   i = 0;
   sprintf (&s[i], "%s%s", "B", TtaGetMessage (AMAYA, AM_REPEAT));
   i += ustrlen (&s[i]) + 1;
   sprintf (&s[i], "%s%s", "B", TtaGetMessage (AMAYA, AM_REPEAT_X));
   i += ustrlen (&s[i]) + 1;
   sprintf (&s[i], "%s%s", "B", TtaGetMessage (AMAYA, AM_REPEAT_Y));
   i += ustrlen (&s[i]) + 1;
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
   if (LastURLImage[0] != WC_EOS)
      ustrcpy (s, LastURLImage);
   else {
      ustrcpy (s, DirectoryImage);
      ustrcat (s, WC_DIR_STR);
      ustrcat (s, ImageName);
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
void                ComputeSRCattribute (Element el, Document doc, Document sourceDocument, Attribute attr, STRING text)
#else  /* __STDC__ */
void                ComputeSRCattribute (el, doc, sourceDocument, attr, text)
Element             el;
Document            doc;
Document            sourceDocument;
Attribute           attr;
STRING              text;

#endif /* __STDC__ */
{
  STRING             value, base;
  CHAR_T             pathimage[MAX_LENGTH];
  CHAR_T             localname[MAX_LENGTH];
  CHAR_T             imagename[MAX_LENGTH];
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
  CHAR_T*            text;
  CHAR_T*            pathimage;
  CHAR_T*            imagename;

   /* Select an image name */
   el = event->element;
   doc = event->document;
   text = GetImageURL (doc, 1);
   if (text == NULL || text[0] == WC_EOS)
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
   if (ImgAlt[0] == WC_EOS)
     {
       imagename = TtaAllocString (MAX_LENGTH);
       pathimage = TtaAllocString (MAX_LENGTH);
       ustrcpy (imagename, TEXT(" "));
       TtaExtractName (text, pathimage, &imagename[1]);
       ustrcat (imagename, TEXT(" "));
       TtaSetAttributeText (attr, imagename, elSRC, doc);
       TtaFreeMemory (pathimage);
       TtaFreeMemory (imagename);
     }
   else
     {
       TtaSetAttributeText (attr, ImgAlt, elSRC, doc);
       ImgAlt[0] = WC_EOS;
     }
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
   STRING              buf1, buf2;
   STRING              localname, imageName;
   LoadedImageDesc   *desc;

   doc = event->document;
   el = event->element;
   attr = event->attribute;
   /* get a buffer for the attribute value */
   length = MAX_LENGTH;
   buf1 = TtaAllocString (length);
   buf2 = TtaAllocString (length);
   imageName = TtaAllocString (length);
   /* copy the SRC attribute into the buffer */
   TtaGiveTextAttributeValue (attr, buf1, &length);
   NormalizeURL (buf1, doc, buf2, imageName, NULL);
   /* extract image name from full name */
   TtaExtractName (buf2, buf1, imageName);
   if (ustrlen (imageName) != 0)
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
#ifdef __STDC__
void                CreateImage (Document document, View view)
#else  /* __STDC__ */
void                CreateImage (document, view)
Document            document;
View                view;

#endif /* __STDC__ */
{
   ElementType         elType;

   elType.ElSSchema = TtaGetSSchema (TEXT("HTML"), document);
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
ThotBool            AddLocalImage (CHAR_T* fullname, CHAR_T* name, CHAR_T* url, Document doc, LoadedImageDesc ** desc)
#else  /* __STDC__ */
ThotBool            AddLocalImage (fullname, name, url, doc, desc)
CHAR_T*             fullname;
CHAR_T*             name;
CHAR_T*             url;
Document            doc;
LoadedImageDesc   **desc;

#endif /* __STDC__ */
{
  LoadedImageDesc    *pImage, *previous;
  CHAR_T*             localname;

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
	      (ustrcmp (url, pImage->originalName) == 0))
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
	  pImage->originalName = TtaAllocString (ustrlen (url) + 1);
	  ustrcpy (pImage->originalName, url);
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
	     TtaFreeMemory ((STRING) pImage);
	     pImage = previous;
	  }
	/* next descriptor */
	previous = pImage;
	pImage = next;
     }
}
