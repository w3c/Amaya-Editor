/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Variables used by compilers grm, str, prs, and app
 */

/* message origin */
EXPORT int		COMPIL;
EXPORT int        	STR;
EXPORT int        	PRS;
EXPORT int        	TRA;
EXPORT int        	GRM;
EXPORT int		APP;

/* a line from the source file being compiled */
EXPORT lineBuffer 	inputLine;

/* an error has been detected */
EXPORT boolean 	error;
