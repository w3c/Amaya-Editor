#ifndef THOT_REGISTRY_F
#define THOT_REGISTRY_F

#ifdef __STDC__
extern int          SearchFile (char *fileName, int dir, char *fullName);
 
#else  /* __STDC__ */
extern int          SearchFile ( /* fileName, dir, fullName */ );
 
#endif /* __STDC__ */
 
#endif  /* THOT_REGISTRY_F */
