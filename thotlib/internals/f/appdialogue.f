
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__
extern int CreeFenetre(char *schema, int view, char *name, int X, int Y, int large, int haut, int *volume, int doc);
extern void DetruitFenetre(int fen);
extern int FindMenu(int frame, int menuID, Menu_Ctl **ctxmenu);
extern void ThotCallback(int ref, int typedata, char *data);

#else /* __STDC__ */
extern int CreeFenetre(/*char *schema, int view, char *name, int X, int Y, int large, int haut, int *volume, int doc*/);
extern void DetruitFenetre(/*int fen*/);
extern int FindMenu(/*int frame, int menuID, Menu_Ctl **ctxmenu*/);
extern void ThotCallback(/*int ref, int typedata, char *data*/);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
