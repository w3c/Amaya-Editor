#ifndef __HTML2THOT_F__
#define __HTML2THOT_F__

#ifdef __STDC__
extern void         ParseAreaCoords (Element element, Document document);
extern int          MapGI (char *gi);
extern void         GIType (char *gi, int *elem);
extern char        *GITagName (Element elem);
extern char        *GITagNameByType (int type);
extern int          MapThotAttr (char *Attr, char *tag);
extern int          MapAttrValue (int ThotAtt, char *AttrVal);
extern void         InitMapping (void);
extern void         ParseHTMLError (Document doc, unsigned char *msg);
extern boolean      IsCharacterLevelElement (Element el);
extern void         SetAttrIntItemStyle (Element el, Document doc);
extern void         CreateAttrIntSize (char *buffer, Element el, Document doc);
extern char         GetNextInputChar (void);
extern void         HTMLparse (FILE * infile, char *HTMLbuf);
extern void         CheckAbstractTree (char *pathURL);
extern void         InitializeParser (Element lastelem, boolean isclosed, Document doc);
extern void         StartHTMLParser (Document doc, char *htmlFileName, char *documentName, char *documentDirectory, char *pathURL);
extern void         CreateAttrWidthPercentPxl (char *buffer, Element el, Document doc);
extern void         OnlyOneOptionSelected (Element el, Document doc, boolean parsed);

#else  /* __STDC__ */
extern void         ParseAreaCoords ();
extern int          MapGI ();
extern void         GIType ();
extern char        *GITagName ();
extern char        *GITagNameByType ();
extern int          MapThotAttr ();
extern int          MapAttrValue ();
extern void         InitMapping ();
extern void         ParseHTMLError ();
extern boolean      IsCharacterLevelElement ();
extern void         SetAttrIntItemStyle ();
extern void         CreateAttrIntSize ();
extern char         GetNextInputChar ();
extern void         HTMLparse ();
extern void         CheckAbstractTree ();
extern void         InitializeParser ();
extern void         StartHTMLParser ();
extern void         CreateAttrWidthPercentPxl ();
extern void         OnlyOneOptionSelected ();

#endif /* __STDC__ */

#endif /* __HTML2THOT_F__ */
