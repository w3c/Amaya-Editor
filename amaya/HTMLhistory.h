#ifndef _HTML_HISTORY_F_
#define _HTML_HISTORY_F_

#ifdef __STDC__
extern void         AddCSSHistory (CSSInfoPtr css);
extern void         AddHTMLHistory (char *url);
extern int          BuildCSSHistoryList (Document doc, char *buf, int size, char *first);

#else  /* __STDC__ */
extern void         AddCSSHistory ();
extern void         AddHTMLHistory ();
extern int          BuildCSSHistoryList ();

#endif /* __STDC__ */

#endif /* _HTML_HISTORY_F_ */
