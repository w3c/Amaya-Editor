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
 * TtaSkipBlanks skips all spaces, tabs, linefeeds and newlines at the
 * beginning of the string and returns the pointer to the new position. 
 */
extern char*      TtaSkipBlanks (char* ptr);

extern CHAR_T*    TtaSkipWCBlanks (CHAR_T* ptr);


/*
 * TtaIsBlank returns True if the first character is a space, a tab, a
 * linefeed or a newline.
 */
extern ThotBool    TtaIsBlank (char* ptr);
extern ThotBool    TtaIsWCBlank ( CHAR_T* ptr );

/*
 * TtaInitializeAppRegistry : initialize the Registry, the only argument
 *       given is a copy of the argv[0] received from the main().
 *       From this, we can deduce the installation directory of the programm,
 *       (using the PATH environment value if necessary) and the application
 *       name.
 *       We load the ressources file from the installation directory and
 *       the specific user values from the user HOME dir.
 */
extern void         TtaInitializeAppRegistry (CHAR_T*);

/*
 * TtaFreeAppRegistry : frees the memory associated with the
 *        registry
 */
extern void         TtaFreeAppRegistry (void);

/*
 * TtaGetEnvInt : read the integer value associated to an 
 * environment string.
 * Returns TRUE if the env variables exists or FALSE if it isn't the case.
 */
extern ThotBool     TtaGetEnvInt (char* name, int *value);

/*
 *  TtaGetEnvBoolean : read the ThotBool value associated to an 
 * environment string.
 * Returns TRUE if the env variables exists or FALSE if it isn't the case.
 */
extern ThotBool     TtaGetEnvBoolean (char* name, ThotBool *value);

/*
 * TtaGetEnvString : read the value associated to an environment string
 *                  if not present return NULL.
 */
extern CHAR_T*       TtaGetEnvString (char* name);

/*
 * TtaClearEnvString : clears the value associated with an environment
 *                     string, in the user registry.
 */
extern void         TtaClearEnvString (char* name);

/*
 * TtaSetEnvInt : set the value associated to an environment string,
 *                  for the current application.
 */
extern void         TtaSetEnvInt (char* name, int value, int overwrite);

/*
 * TtaSetEnvBoolean : set the value associated to an environment string,
 *                  for the current application.
 */
extern void         TtaSetEnvBoolean (char* name, ThotBool value, int overwrite);

/*
 * TtaSetEnvString : set the value associated to an environment string,
 *                  for the current application.
 */
extern void         TtaSetEnvString (char* name, CHAR_T* value, int overwrite);

/*
 * TtaSetDefEnvString : set the defaul value associated to an environment 
 *                      string, for the current application.
 */
extern void         TtaSetDefEnvString (char* name, CHAR_T* value, int overwrite);

/*
 * TtaGetDefEnvInt : read the default integer value associated to an 
 * environment string.
 * Returns TRUE if the env variables exists or FALSE if it isn't the case.
 */
extern ThotBool     TtaGetDefEnvInt (char* name, int *value);

/*
 *  TtaGetDefEnvBoolean : read the default ThotBool value associated to an 
 * environment string.
 * Returns TRUE if the env variables exists or FALSE if it isn't the case.
 */
extern ThotBool     TtaGetDefEnvBoolean (char* name, ThotBool *value);

/*
 * TtaGetDefEnvString : read the default value associated to an environment 
 *                   string. If not present, return NULL.
 */
extern CHAR_T*    TtaGetDefEnvString (char* name);

/*
 * TtaSaveAppRegistry : Save the Registry in the THOT_RC_FILENAME located
 *       in the user's directory.
 */
extern void         TtaSaveAppRegistry (void);

#else  /* __STDC__ */
extern char*        TtaSkipBlanks (/* char* ptr */);
extern CHAR_T*      TtaSkipWCBlanks (/* CHAR_T* ptr */);
extern ThotBool     TtaIsBlank (/* char* ptr */);
extern ThotBool     TtaIsWCBlank (/* CHAR_T* ptr */);
extern void         TtaInitializeAppRegistry ( /* CHAR_T* appArgv0 */ );
extern void         TtaFreeAppRegistry ( /* void */ );
extern ThotBool     TtaGetEnvBoolean ( /* STRING name, ThotBool *value */ );
extern ThotBool     TtaGetEnvInt ( /* char* name, int *value */ );
extern CHAR_T*      TtaGetEnvString ( /* STRING name */ );
extern void         TtaClearEnvString ( /* char* name */ );
extern void         TtaSetEnvInt ( /* char* name, int value, int overwrite */ );
extern void         TtaSetEnvBoolean ( /* char* name, ThotBool value, int overwrite */ );
extern void         TtaSetEnvString ( /* char* name, CHAR_T* value, int overwrite */ );
extern void         TtaSetDefEnvString ( /* char* name, CHAR_T* value, int overwrite */ );

extern ThotBool     TtaGetDefEnvBoolean ( /* char* name, ThotBool *value */ );
extern ThotBool     TtaGetDefEnvInt ( /* char* name, int *value */ );
extern CHAR_T*    TtaGetDefEnvString ( /* char* name */ );
extern void         TtaSaveAppRegistry ( /* void */ );

#endif /* __STDC__ */

#endif	/* THOT_REGISTRY_H */





