#ifndef UICSS_F__
#define UICSS_F__

#ifdef __STDC__
extern void         InitCSSDialog (Document doc, View view);
extern void         DeleteExternalCSS (Document doc, View view);
extern void         SelectExternalCSS (Document doc, View view);
extern void         CSSConfirm (Document document, View view, char *label);
extern void         RedisplayDocument (Document doc);
extern void         ApplyExtraPresentation (Document doc);
extern void         InitBrowse (Document doc, View view, char *url);
extern void         CSSCallbackDialogue (int ref, int typedata, char *data);

#else  /* __STDC__ */
extern void         InitCSSDialog ();
extern void         DeleteExternalCSS ();
extern void         SelectExternalCSS ();
extern void         CSSConfirm ();
extern void         RedisplayDocument ();
extern void         ApplyExtraPresentation ();
extern void         InitBrowse ();
extern void         CSSCallbackDialogue ();

#endif /* __STDC__ */

#endif /* UICSS_F__ */
