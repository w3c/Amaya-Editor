
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean AskForNew_RemplRefer ( boolean *Creer, Name NomType );
extern void RetMenuAskForNew ( int Val );
extern void CreeEtActiveMenuChoix ( char *BufMenu, Name TitreMenu, int nbentree, boolean ChoixElemNature );
extern void SeparMenuCreeColle ( int *prevmenuind, int *nbentree, int *menuind, char *BufMenu );
extern void CreeEtActiveMenuCreeColle ( int RefMenu, char *BufMenu, Name titre, int nbentree, char bouton );

#else /* __STDC__ */

extern boolean AskForNew_RemplRefer (/* boolean *Creer, Name NomType */);
extern void RetMenuAskForNew (/* int Val */);
extern void CreeEtActiveMenuChoix (/* char *BufMenu, Name TitreMenu, int nbentree, boolean ChoixElemNature */);
extern void SeparMenuCreeColle (/* int *prevmenuind, int *nbentree, int *menuind, char *BufMenu */);
extern void CreeEtActiveMenuCreeColle (/* int RefMenu, char *BufMenu, Name titre, int nbentree, char bouton */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
