/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
/*
 * Header file for the acess to the Registry.
 */

#ifndef THOT_REGISTRY_H
#define THOT_REGISTRY_H

#ifdef __STDC__

/*
 * TtaInitializeAppRegistry : initialize the Registry, the only argument
 *       given is a copy of the argv[0] received from the main().
 *       From this, we can deduce the installation directory of the programm,
 *       (using the PATH environment value if necessary) and the application
 *       name.
 *       We load the ressources file from the installation directory and
 *       the specific user values from the user HOME dir.
 */
extern void         TtaInitializeAppRegistry (char *appArgv0);

/*
 * TtaGetEnvString : read the value associated to an environment string
 *                  if not present return NULL.
 */
extern char        *TtaGetEnvString (char *name);

/*
 * TtaSetEnvString : set the value associated to an environment string,
 *                  for the current application.
 */
extern void         TtaSetEnvString (char *name, char *value, int overwrite);

/*
 * TtaSaveAppRegistry : Save the Registry in the THOT_RC_FILENAME located
 *       in the user's directory.
 */
extern void         TtaSaveAppRegistry (void);

#else  /* __STDC__ */
extern void         TtaInitializeAppRegistry ( /* char *appArgv0 */ );
extern char        *TtaGetEnvString ( /* char *name */ );
extern void         TtaSetEnvString ( /* char *name, char *value, int overwrite */ );
extern void         TtaSaveAppRegistry ( /* void */ );

#endif /* __STDC__ */

#endif	/* THOT_REGISTRY_H */
