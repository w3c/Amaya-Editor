
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void TtcPageDown(Document document, View view);
extern void TtcPageEnd(Document document, View view);
extern void TtcPageTop(Document document, View view);
extern void TtcPageUp(Document document, View view);
extern void RetourKill(int *w, int fen, int *infos);
extern void XChangeTaille(int *w, int fen, int *infos);
extern void XChangeHScroll(int *w, int fen, int *infos);
extern void XChangeVScroll(int *w, int fen, int *infos);
extern void MsgSelect ( char *texte );
extern void VueDeFenetre(int fenetre, int *doc, int *vue);
extern void InitAutreContexts ( void );
#ifdef NEW_WILLOWS
extern LRESULT CALLBACK
ThotWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif /* NEW_WILLOWS */
#ifdef WWW_XWINDOWS
extern void RetourFntr(int fen, XEvent *ev);
#endif /* WWW_XWINDOWS */
extern void ThotGrab ( ThotWindow win, ThotCursor cursor, long events, int disp );
extern void ThotGrabRoot ( ThotCursor cursor, int disp );
extern void ThotUngrab ( void );
extern ThotWindow TtaGetThotWindow ( int fen );
extern void SetCursorWatch ( int thotThotWindowid );
extern void ResetCursorWatch ( int thotThotWindowid );
extern void DesignationPave ( int *fen, int *pave );
extern void ChangeTitre ( int fen, char *texte );
extern void ChangeSelFntr ( int fen );
extern int GetFenetre ( ThotWindow w );
#ifdef WWW_XWINDOWS
extern void TraiteExpose ( ThotWindow w, int fen, XExposeEvent *event );
#endif /* WWW_XWINDOWS */
extern void DimFenetre ( int fen, int *larg, int *haut );
extern void SetClip ( int fen, int orgx, int orgy, int *xd, int *yd, int *xf, int *yf, int raz );
extern void ResetClip ( int fen );
extern void MajScrolls ( int fen );
#else /* __STDC__ */

extern void TtcPageDown(/*Document document, View view*/);
extern void TtcPageEnd(/*Document document, View view*/);
extern void TtcPageTop(/*Document document, View view*/);
extern void TtcPageUp(/*Document document, View view*/);
extern void RetourKill(/*int *w, int fen, int *infos*/);
extern void XChangeTaille(/*int *w, int fen, int *infos*/);
extern void XChangeHScroll(/*int *w, int fen, int *infos*/);
extern void XChangeVScroll(/*int *w, int fen, int *infos*/);
extern void MsgSelect (/* char *texte */);
extern void VueDeFenetre(/*int fenetre, int *doc, int *vue*/);
extern void InitAutreContexts (/* void */);
#ifdef WWW_XWINDOWS
extern void RetourFntr(/*int fen, XEvent *ev*/);
#endif /* WWW_XWINDOWS */
extern void ThotGrab (/* ThotWindow win, ThotCursor cursor, long events, int disp */);
extern void ThotGrabRoot (/* ThotCursor cursor, int disp */);
extern void ThotUngrab (/* void */);
extern ThotWindow TtaGetThotWindow (/* int fen */);
extern void SetCursorWatch (/* int thotThotWindowid */);
extern void ResetCursorWatch (/* int thotThotWindowid */);
extern void DesignationPave (/* int *fen, int *pave */);
extern void ChangeTitre (/* int fen, char *texte */);
extern void ChangeSelFntr (/* int fen */);
extern int GetFenetre (/* ThotWindow w */);
#ifdef WWW_XWINDOWS
extern void TraiteExpose (/* ThotWindow w, int fen, XExposeEvent *event */);
#endif /* WWW_XWINDOWS */
extern void DimFenetre (/* int fen, int *larg, int *haut */);
extern void SetClip (/* int fen, int orgx, int orgy, int *xd, int *yd, int *xf, int *yf, int raz */);
extern void ResetClip (/* int fen */);
extern void MajScrolls (/* int fen */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
