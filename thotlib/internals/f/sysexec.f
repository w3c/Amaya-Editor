
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void Imprimer ( char *nom, char *dir, char *thotsch, char *thotdoc, char *thotpres, char *realname, char *realdir, char *imprimante, int pagedeb, int pagefin, int nbex, int decalage_h, int decalage_v, int user_orientation, int reduction, int nb_ppf, int suptrame, int alimmanuelle, int noiretblanc, int paginer, char *vuesaimprimer );
extern void SauverPS ( char *nom, char *dir, char *thotsch, char *thotdoc, char *thotpres, char *realname, char *realdir, char *nomps, int pagedeb, int pagefin, int nbex, int decalage_h, int decalage_v, int user_orientation, int reduction, int nb_ppf, int suptrame, int alimmanuelle, int noiretblanc, int paginer, char *vuesaimprimer );
extern void Lire ( char *nomtemp, char *dirtemp, char *filename, char *structure, char *thotsch, char *docformat );
extern void GetTempNames ( char *dir, char *file );
extern void InitShellMsg ( void );
extern void RmShellMsg ( void );
extern void GetShellMsg ( void );

#else /* __STDC__ */

extern void Imprimer (/* char *nom, char *dir, char *thotsch, char *thotdoc, char *thotpres, char *realname, char *realdir, char *imprimante, int pagedeb, int pagefin, int nbex, int decalage_h, int decalage_v, int user_orientation, int reduction, int nb_ppf, int suptrame, int alimmanuelle, int noiretblanc, int paginer, char *vuesaimprimer */);
extern void SauverPS (/* char *nom, char *dir, char *thotsch, char *thotdoc, char *thotpres, char *realname, char *realdir, char *nomps, int pagedeb, int pagefin, int nbex, int decalage_h, int decalage_v, int user_orientation, int reduction, int nb_ppf, int suptrame, int alimmanuelle, int noiretblanc, int paginer, char *vuesaimprimer */);
extern void Lire (/* char *nomtemp, char *dirtemp, char *filename, char *structure, char *thotsch, char *docformat */);
extern void GetTempNames (/* char *dir, char *file */);
extern void InitShellMsg (/* void */);
extern void RmShellMsg (/* void */);
extern void GetShellMsg (/* void */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
