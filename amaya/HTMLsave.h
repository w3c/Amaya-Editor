#ifndef __HTML_SAVE_F__
#define __HTML_SAVE_F__

#ifdef __STDC__
extern void         SetAbsoluteURLs (Document document, View view);
extern void         SaveDocumentAs (Document document, View view);
extern void         SaveInHTML (char *directoryName, char *documentName);
extern void         InitSaveForm (Document document, View view, char *pathname);
extern int          SaveDocumentThroughNet (Document document, View view, Boolean confirm);
extern void         SaveDocument (Document document, View view);
extern void         CreateNewImagePaths (char *DocUrl, char *OldPath, char *NewUrl, char *NewPath);
extern void         DoSaveAs (void);
extern void         InitSaveObjectForm (Document document, View view, char *object, char *pathname);
extern void         DoSaveObjectAs (void);

#else  /* __STDC__ */
extern void         SetAbsoluteURLs ();
extern void         SaveDocumentAs ();
extern void         SaveInHTML ();
extern void         InitSaveForm ();
extern int          SaveDocumentThroughNet ();
extern void         SaveDocument ();
extern void         CreateNewImagePaths ();
extern void         DoSaveAs ();
extern void         InitSaveObjectForm ();
extern void         DoSaveObjectAs ();

#endif /* __STDC__ */

#endif /* __HTML_SAVE_F__ */
