#ifndef __INCLUDE_CSS_H__
#define __INCLUDE_CSS_H__
#include "pschema.h"
#include "cssInc.h"

#ifdef __STDC__
extern void         CSSClassChanged (NotifyAttribute * event);
extern void         ExplodeURL (char *url, char **proto, char **host, char **dir, char **file);
extern void         PrintCSS (CSSInfoPtr css, FILE * output);
extern int          DumpCSSToFile (Document doc, CSSInfoPtr css, char *filename);
extern void         PrintListCSS (FILE * output);
extern void         GotoPreviousHTML (Document doc, View view);
extern void         GotoNextHTML (Document doc, View view);
extern void         AddCSS (CSSInfoPtr css);
extern void         FreeCSS (CSSInfoPtr css);
extern void         InitDocumentCSS (Document doc);
extern void         CleanDocumentCSS (Document doc);
extern void         CleanListCSS (void);
extern CSSInfoPtr   NewCSS (void);
extern int          CmpCSS (CSSInfoPtr css, CSSInfoPtr cour);
extern CSSInfoPtr   SearchCSS (Document doc, CSSCategory category, char *url);
extern void         ClearCSS (CSSInfoPtr css);
extern void         RebuildCSS (CSSInfoPtr css);
extern CSSInfoPtr   GetDocumentStyle (Document doc);
extern PSchema      GetDocumentGenericPresentation (Document doc);
extern CSSInfoPtr   GetUserGenericPresentation (void);
extern void         RebuildHTMLStyleHeader (Document doc);
extern void         ParseHTMLStyleHeader (Element elem, char *attrstr, Document doc, Bool rebuild);
extern void         LoadHTMLStyleSheet (char *URL, Document doc);
extern void         LoadHTMLExternalStyleSheet (char *URL, Document doc, int merge);
extern void         LoadUserStyleSheet (Document doc);
extern void         CSSSetBackground (Document doc, PSchema gpres, int color);
extern void         CSSSetMagnification (Document doc, PSchema gpres, int zoom);
extern void         ApplyFinalStyle (Document doc);
extern void         MergeNewCSS (char *attrstr, Document doc, PSchema gPres);
extern void         RemoveCSS (char *name, Document doc);
extern PRuleInfoPtr SearchRPISel (char *selector, PRuleInfoPtr list);
extern void         SelectRPIEntry (char which, int index, char *value);
extern int          BuildCSSList (Document doc, char *buf, int size, char *first);
extern char        *GetlistEntry (char *list, int entry);
extern void         RedrawLCSS (char *name);
extern void         RedrawLRPI (char *name);
extern void         RedrawRCSS (char *name);
extern void         RedrawRRPI (char *name);
extern void         CSSHandleMerge (char which, Bool copy);
extern void         RebuildAllCSS (void);
extern int          SaveCSSThroughNet (Document doc, View view, CSSInfoPtr css);
extern void         InitCSS (void);
extern void         CloseCSS (void);

#else  /* __STDC__ */
extern void         CSSClassChanged ();
extern void         ExplodeURL ();
extern void         PrintCSS ();
extern int          DumpCSSToFile ();
extern void         PrintListCSS ();
extern void         GotoPreviousHTML ();
extern void         GotoNextHTML ();
extern void         AddCSS ();
extern void         FreeCSS ();
extern void         InitDocumentCSS ();
extern void         CleanDocumentCSS ();
extern void         CleanListCSS ();
extern CSSInfoPtr   NewCSS ();
extern int          CmpCSS ();
extern CSSInfoPtr   SearchCSS ();
extern void         ClearCSS ();
extern void         RebuildCSS ();
extern CSSInfoPtr   GetDocumentStyle ();
extern PSchema      GetDocumentGenericPresentation ();
extern CSSInfoPtr   GetUserGenericPresentation ();
extern void         RebuildHTMLStyleHeader ();
extern void         ParseHTMLStyleHeader ();
extern void         LoadHTMLStyleSheet ();
extern void         LoadHTMLExternalStyleSheet ();
extern void         LoadUserStyleSheet ();
extern void         CSSSetBackground ();
extern void         CSSSetMagnification ();
extern void         ApplyFinalStyle ();
extern void         MergeNewCSS ();
extern void         RemoveCSS ();
extern PRuleInfoPtr SearchRPISel ();
extern void         SelectRPIEntry ();
extern int          BuildCSSList ();
extern char        *GetlistEntry ();
extern void         RedrawLCSS ();
extern void         RedrawLRPI ();
extern void         RedrawRCSS ();
extern void         RedrawRRPI ();
extern void         CSSHandleMerge ();
extern void         RebuildAllCSS ();
extern int          SaveCSSThroughNet ();
extern void         InitCSS ();
extern void         CloseCSS ();

#endif /* __STDC__ */

#endif /* __INCLUDE_CSS_H__ */
