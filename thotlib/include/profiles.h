/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * profiles.h : define profiles data 
 */

#define MAX_PRO_LENGTH              200       /* Maximum length of a string */
#define DEF_FILE                   "amaya.profiles"
#define PROFILE_START              '<'
#define MODULE_START               '['
#define PROFILE_END                '>'
#define MODULE_END                 ']'
#define MODULE_REF                 '+'
#define EDITING_REF                '&'

#ifdef __STDC__
/*----------------------------------------------------------------------
  TtaRebuildProTable: Rebuild the Profiles Table
  ----------------------------------------------------------------------*/
extern void     TtaRebuildProTable (CHAR_T* prof_file);

/*----------------------------------------------------------------------
  TtaCanEdit returns TRUE if there is almost one editing function active.
  ----------------------------------------------------------------------*/
extern ThotBool    TtaCanEdit ();

/*----------------------------------------------------------------------
   TtaGetProfileFineName:  Get the text for the profile file name.
   name is a provided buffer of length characters to receive the name.
  ----------------------------------------------------------------------*/
extern void     TtaGetProfileFileName (STRING name, int length);

/*----------------------------------------------------------------------
   TtaGetDefProfileFileName:  Get the text for the default profile file name.
   name is a provided buffer of length characters to receive the name.
  ----------------------------------------------------------------------*/
extern void     TtaGetDefProfileFileName (STRING name, int length);

/*---------------------------------------------------------------
   TtaGetProfilesItems:  Get the text for the profile menu items.
   listEntries is a provided list of length pointers.
   Returns the number of items
------------------------------------------------------------------*/
extern int      TtaGetProfilesItems (char** listEntries, int length);

#else  /* !__STDC__ */
extern void     TtaRebuildProTable (/*CHAR_T* prof_file*/);
extern ThotBool TtaCanEdit ();
extern void     TtaGetProfileFileName (/*STRING name, int length*/);
extern void     TtaGetDefProfileFileName (/*STRING name, int length*/);
extern int      TtaGetProfilesItems (/*char** listEntries, int length*/);

#endif /* !__STDC__ */

