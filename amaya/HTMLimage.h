/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef HTML_IMAGE_F_
#define HTML_IMAGE_F_

#ifdef __STDC__
extern boolean      AddLoadedImage (char *name, char *pathname, Document doc, LoadedImageDesc ** desc);
extern void         DisplayImage (Document doc, Element el, char *imageName);
extern void         ImageLoaded (AHTReqContext * context, int status);
extern void         FetchImage (Document doc, Element el);
extern void         FetchAndDisplayImages (Document doc);

#else  /* __STDC__ */
extern boolean      AddLoadedImage ();
extern void         DisplayImage ();
extern void         ImageLoaded ();
extern void         FetchImage ();
extern void         FetchAndDisplayImages ();

#endif /* __STDC__ */

#endif /* HTML_IMAGE_F_ */
