/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Header file for the acess to the Registry.
 */

#ifndef THOT_REGISTRY_H
#define THOT_REGISTRY_H

/*
 * TtaSkipBlanks skips all spaces, tabs, linefeeds and newlines at the
 * beginning of the string and returns the pointer to the new position. 
 */
extern const char *TtaSkipBlanks (const char *ptr);


/*
 * TtaIsBlank returns True if the first character is a space, a tab, a
 * linefeed or a newline.
 */
extern ThotBool TtaIsBlank (const char *ptr);

/*
 * TtaInitializeAppRegistry : initialize the Registry, the only argument
 *       given is a copy of the argv[0] received from the main().
 *       From this, we can deduce the installation directory of the programm,
 *       (using the PATH environment value if necessary) and the application
 *       name.
 *       We load the ressources file from the installation directory and
 *       the specific user values from the user HOME dir.
 */
extern void TtaInitializeAppRegistry (char *);

/*
 * TtaFreeAppRegistry : frees the memory associated with the
 *        registry
 */
extern void TtaFreeAppRegistry (void);

/*
 * TtaGetEnvInt : read the integer value associated to an 
 * environment string.
 * Returns TRUE if the env variables exists or FALSE if it isn't the case.
 */
extern ThotBool TtaGetEnvInt (const char *name, int *value);

/*
 *  TtaGetEnvBoolean : read the ThotBool value associated to an 
 * environment string.
 * Returns TRUE if the env variables exists or FALSE if it isn't the case.
 */
extern ThotBool TtaGetEnvBoolean (const char *name, ThotBool *value);

/*
 * TtaGetEnvString : read the value associated to an environment string
 *                  if not present return NULL.
 */
extern char *TtaGetEnvString (const char *name);

/*
 * TtaClearEnvString : clears the value associated with an environment
 *                     string, in the user registry.
 */
extern void TtaClearEnvString (const char *name);

/*
 * TtaSetEnvInt : set the value associated to an environment string,
 *                  for the current application.
 */
extern void TtaSetEnvInt (const char *name, int value, int overwrite);

/*
 * TtaSetEnvBoolean : set the value associated to an environment string,
 *                  for the current application.
 */
extern void TtaSetEnvBoolean (const char *name, ThotBool value, int overwrite);

/*
 * TtaSetEnvString : set the value associated to an environment string,
 *                  for the current application.
 */
extern void TtaSetEnvString (const char *name, const char *value, int overwrite);

/*
 * TtaSetDefEnvString : set the defaul value associated to an environment 
 *                      string, for the current application.
 */
extern void TtaSetDefEnvString (const char *name, const char *value, int overwrite);

/*
 * TtaGetDefEnvInt : read the default integer value associated to an 
 * environment string.
 * Returns TRUE if the env variables exists or FALSE if it isn't the case.
 */
extern ThotBool TtaGetDefEnvInt (const char *name, int *value);

/*
 *  TtaGetDefEnvBoolean : read the default ThotBool value associated to an 
 * environment string.
 * Returns TRUE if the env variables exists or FALSE if it isn't the case.
 */
extern ThotBool TtaGetDefEnvBoolean (const char *name, ThotBool *value);

/*
 * TtaGetDefEnvString : read the default value associated to an environment 
 *                   string. If not present, return NULL.
 */
extern char *TtaGetDefEnvString (const char *name);

/*
 * TtaSaveAppRegistry : Save the Registry in the THOT_RC_FILENAME located
 *       in the user's directory.
 */
extern void TtaSaveAppRegistry (void);

#endif	/* THOT_REGISTRY_H */





