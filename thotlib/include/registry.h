/*
 * Header file for the acess to the Registry.
 */

#ifndef THOT_REGISTRY_H
#define THOT_REGISTRY_H

#ifdef __STDC__
extern void         TtaInitializeAppRegistry (char *appArgv0);
extern char        *TtaGetEnvString (char *name);
extern void         TtaSetEnvString (char *name, char *value, int overwrite);
extern void         TtaSaveAppRegistry (void);

#else  /* __STDC__ */
extern void         TtaInitializeAppRegistry ( /* char *appArgv0 */ );
extern char        *TtaGetEnvString ( /* char *name */ );
extern void         TtaSetEnvString ( /* char *name, char *value, int overwrite */ );
extern void         TtaSaveAppRegistry ( /* void */ );

#endif /* __STDC__ */

#endif	/* THOT_REGISTRY_H */
