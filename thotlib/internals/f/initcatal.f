
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void EntreeMenu ( int *Indx, char *Entree, char BufMenu[1024] );
extern void ButtonSheet(int *Indx, char *Entree, char BufMenu[MAX_TXT_LEN]);
extern void MenusInit ( void );

#else /* __STDC__ */

extern void EntreeMenu (/* int *Indx, char *Entree, char BufMenu[1024] */);
extern void ButtonSheet(/*int *Indx, char *Entree, char BufMenu[MAX_TXT_LEN]*/);
extern void MenusInit (/* void */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
