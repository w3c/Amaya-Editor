/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _HTML_EDIT_F__
#define _HTML_EDIT_F__

#ifdef __STDC__
extern void         SetTargetContent (Document doc, Attribute attrNAME);
extern void         SetHREFattribute (Element element, Document document, Document targetDoc);
extern Attribute    GetNameAttr (Document doc, Element selectedElement);
extern void         CreateTargetAnchor (Document doc, Element el);
extern void         ElementPasted (NotifyElement * event);
extern void         CreateLink (Document doc, View view);
extern void         CreateTarget (Document doc, View view);
extern void         coordsModified (NotifyAttribute * event);
extern void         graphicsModified (NotifyAttribute * event);
extern void         AttrWidthCreated (NotifyAttribute * event);
extern boolean      AttrWidthDelete (NotifyAttribute * event);
extern void         AttrWidthModified (NotifyAttribute * event);
extern void         AttrFontSizeCreated (NotifyAttribute * event);
extern boolean      AttrFontSizeDelete (NotifyAttribute * event);
extern void         AttrColorCreated (NotifyAttribute * event);
extern boolean      AttrColorDelete (NotifyAttribute * event);
extern void         ListItemCreated (NotifyElement * event);
extern void         ListChangedType (NotifyElement * event);
extern void         UpdateAttrIntItemStyle (NotifyAttribute * event);
extern void         AttrItemStyle (NotifyAttribute * event);
extern boolean      AttrNAMEinMenu (NotifyAttribute * event);
extern void         ResetFontOrPhraseOnText (Document document, Element elem, int notType);
extern void         SetFontOrPhraseOnText (Document document, Element * elem, int newtype);
extern void         CreateElemEmphasis (Document document, View view);
extern void         CreateElemStrong (Document document, View view);
extern void         CreateElemCite (Document document, View view);
extern void         CreateElemDefinition (Document document, View view);
extern void         CreateElemCode (Document document, View view);
extern void         CreateElemVariable (Document document, View view);
extern void         CreateElemSample (Document document, View view);
extern void         CreateElemKeyboard (Document document, View view);
extern void         CreateElemItalic (Document document, View view);
extern void         CreateElemBold (Document document, View view);
extern void         CreateElemTeletype (Document document, View view);
extern void         CreateElemStrikeOut (Document document, View view);
extern void         CreateElemBig (Document document, View view);
extern void         CreateElemSmall (Document document, View view);
extern void         CreateSub (Document document, View view);
extern void         CreateSup (Document document, View view);
extern void         CreateElemFont (Document document, View view);
extern void         DocumentClosed (NotifyDialog * event);
extern Element      SearchNAMEattribute (Document doc, char *nameVal, Attribute ignore);
extern Element      SearchAnchor (Document doc, Element element, boolean link);
extern char        *GetActiveImageInfo (Document document, Element element);
extern void         UpdateAtom (Document doc, char *url, char *title);
extern void         UpdateTitle (Element el, Document doc);
extern void         TitleModified (NotifyOnTarget * event);

#else  /* __STDC__ */
extern void         SetTargetContent ();
extern void         SetHREFattribute ();
extern Attribute    GetNameAttr ();
extern void         CreateTargetAnchor ();
extern void         ElementPasted ();
extern void         CreateLink ();
extern void         CreateTarget ();
extern void         coordsModified ();
extern void         graphicsModified ();
extern void         AttrWidthCreated ();
extern boolean      AttrWidthDelete ();
extern void         AttrWidthModified ();
extern void         AttrFontSizeCreated ();
extern boolean      AttrFontSizeDelete ();
extern void         AttrColorCreated ();
extern boolean      AttrColorDelete ();
extern void         ListItemCreated ();
extern void         ListChangedType ();
extern void         UpdateAttrIntItemStyle ();
extern void         AttrItemStyle ();
extern boolean      AttrNAMEinMenu ();
extern void         ResetFontOrPhraseOnText ();
extern void         SetFontOrPhraseOnText ();
extern void         CreateElemEmphasis ();
extern void         CreateElemStrong ();
extern void         CreateElemCite ();
extern void         CreateElemDefinition ();
extern void         CreateElemCode ();
extern void         CreateElemVariable ();
extern void         CreateElemSample ();
extern void         CreateElemKeyboard ();
extern void         CreateElemItalic ();
extern void         CreateElemBold ();
extern void         CreateElemTeletype ();
extern void         CreateElemStrikeOut ();
extern void         CreateElemBig ();
extern void         CreateElemSmall ();
extern void         CreateSub ();
extern void         CreateSup ();
extern void         CreateElemFont ();
extern void         DocumentClosed ();
extern Element      SearchNAMEattribute ();
extern Element      SearchAnchor ();
extern char        *GetActiveImageInfo ();
extern void         UpdateAtom ();
extern void         UpdateTitle ();
extern void         TitleModified ();

#endif /* __STDC__ */

#endif /* _HTML_EDIT_F__ */
