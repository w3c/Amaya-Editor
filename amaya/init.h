#ifndef INIT_F__
#define INIT_F__

#ifdef __STDC__
extern Document     IsDocumentLoaded (char *documentURL);
extern void         ExtractParameters (char *aName, char *parameters);
extern void         ExtractSuffix (char *aName, char *aSuffix);
extern void         ThotCopyFile (char *sourceFileName, char *targetFileName);
extern void         ResetStop (Document document);
extern void         ActiveTransfer (Document document);
extern void         StopTransfer (Document document, View view);
extern void         SetCharEmphasis (Document document, View view);
extern void         SetCharStrong (Document document, View view);
extern void         SetCharCode (Document document, View view);
extern void         InitFormAnswer (Document document, View view);
extern void         InitConfirm (Document document, View view, char *label);
extern void         OpenDocInNewWindow (Document document, View view);
extern void         OpenDoc (Document document, View view);
extern void         Reload (Document document, View view);
extern void         ShowStructure (Document document, View view);
extern void         ShowAlternate (Document document, View view);
extern boolean      ViewToOpen (NotifyDialog * event);
extern void         ShowMapAreas (Document document, View view);
extern Document     GetHTMLDocument (char *documentPath, char *form_data, Document doc, DoubleClickEvent DC_event);
extern boolean      NormalizeFile (char *src, char *target);
extern void         CallbackDialogue (int ref, int typedata, char *data);
extern void         InitAmaya (NotifyEvent * event);
extern void         HelpAmaya (Document document, View view);
extern void         AmayaClose (Document document, View view);

#else  /* __STDC__ */
extern Document     IsDocumentLoaded ();
extern void         ExtractParameters ();
extern void         ExtractSuffix ();
extern void         ThotCopyFile ();
extern void         ResetStop ();
extern void         ActiveTransfer ();
extern void         StopTransfer ();
extern void         SetCharEmphasis ();
extern void         SetCharStrong ();
extern void         SetCharCode ();
extern void         InitFormAnswer ();
extern void         InitConfirm ();
extern void         OpenDocInNewWindow ();
extern void         OpenDoc ();
extern void         Reload ();
extern void         ShowStructure ();
extern void         ShowAlternate ();
extern boolean      ViewToOpen ();
extern void         ShowMapAreas ();
extern Document     GetHTMLDocument ();
extern boolean      NormalizeFile ();
extern void         CallbackDialogue ();
extern void         InitAmaya ();
extern void         HelpAmaya ();
extern void         AmayaClose ();

#endif /* __STDC__ */

#endif /* INIT_F__ */
