
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

#ifdef WWW_XWINDOWS
extern void TtaInitDialogue ( char *server, char *txtOK, char *txtRAZ, char* txtDone, XtAppContext *app_context, Display **Dp );
extern void TtaInitDialogueTranslations ( XtTranslations translations );
#else /* WWW_XWINDOWS */
extern void TtaInitDialogue (char *server, char *txtOK, char *txtRAZ);
#endif /* !WWW_XWINDOWS */
extern char *TtaDialogueVersion ( void );
extern void TtaInitDialogueColors ( char *name );
extern void TtaChangeDialogueFonts ( char *menufont, char *formfont );
extern int TtaGetReferencesBase ( int number );
extern int TtaInitDialogueWindow ( char *name, char *geometry, Pixmap logo, Pixmap icon, int number, char *textmenu );
extern void TtaDefineDialogueCallback ( void (*procedure) () );

#else /* __STDC__ */

#ifdef WWW_XWINDOWS
extern void TtaInitDialogue (/* char *server, char *txtOK, char *txtRAZ, XtAppContext *app_context, Display **Dp */);
extern void TtaInitDialogueTranslations (/* XtTranslations translations */);
#else /* WWW_XWINDOWS */
extern void TtaInitDialogue (/* char *server, char *txtOK, char *txtRAZ */);
#endif /* !WWW_XWINDOWS */
extern char *TtaDialogueVersion (/* void */);
extern void TtaInitDialogueColors (/* char *name */);
extern void TtaChangeDialogueFonts (/* char *menufont, char *formfont */);
extern int TtaGetReferencesBase (/* int number */);
extern int TtaInitDialogueWindow (/* char *name, char *geometry, Pixmap logo, Pixmap icon, int number, char *textmenu */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
