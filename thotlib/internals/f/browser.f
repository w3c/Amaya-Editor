
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern boolean TtaIsSuffixFileIn ( char *aDirectory, char *suffix );
extern void TtaListDirectory ( char *aDirectory, int formRef, char *dirTitle, int dirRef, char *suffix, char *fileTitle, int fileRef );

#else /* __STDC__ */

extern boolean TtaIsSuffixFileIn (/* char *aDirectory, char *suffix */);
extern void TtaListDirectory (/* char *aDirectory, int formRef, char *dirTitle, int dirRef, char *suffix, char *fileTitle, int fileRef */);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
