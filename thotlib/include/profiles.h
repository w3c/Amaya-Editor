/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * profiles.h : define profiles data 
 */
#define MAX_PRO_LENGTH              200       /* Maximum length of a string */
#define DEF_FILE                   "amaya.profiles"

/* Masks for the profiles */
#define L_Other             0x00 /* No one profile is defined */
#define L_Basic             0x02 /* accept only Basic values */
#define L_Strict            0x06 /* accept Strict and Basic values */
#define L_Xhtml11           0x1E /* accept Strict, Basic and Ruby values */
#define L_Transitional      0x2E /* accept all values except Ruby */
#define L_MathML            0x40 /* accept only MathML values */

/* Values for the profiles */
#define L_BasicValue        0x02 /* allowed for all profiles */
#define L_StrictValue       0x0C /* allowed for Basic and Strict profiles */
#define L_Xhmli11Value      0x0C /* allowed for Basic, Strict and Xhmli11 profiles */
#define L_RubyValue         0x10 /* allowed only for Xhmli11 profile */
#define L_TransitionalValue 0x20 /* allowed only for Transitional profile */
#define L_MathMLValue       0x7C /* allowed for all profiles except Basic */
#define L_OtherValue        0xFF

/*----------------------------------------------------------------------
  TtaRebuildProTable: Rebuild the Profiles Table
  ----------------------------------------------------------------------*/
extern void TtaRebuildProTable (char *prof_file);

/*----------------------------------------------------------------------
  TtaCanEdit returns TRUE if there is almost one editing function active.
  ----------------------------------------------------------------------*/
extern ThotBool TtaCanEdit ();

/*----------------------------------------------------------------------
   TtaGetProfileFineName:  Get the text for the profile file name.
   name is a provided buffer of length characters to receive the name.
  ----------------------------------------------------------------------*/
extern void TtaGetProfileFileName (char *name, int length);

/*----------------------------------------------------------------------
   TtaGetDefProfileFileName:  Get the text for the default profile file name.
   name is a provided buffer of length characters to receive the name.
  ----------------------------------------------------------------------*/
extern void TtaGetDefProfileFileName (char *name, int length);

/*---------------------------------------------------------------
   TtaGetProfilesItems:  Get the text for the profile menu items.
   listEntries is a provided list of length pointers.
   Returns the number of items
------------------------------------------------------------------*/
extern int TtaGetProfilesItems (char **listEntries, int length);

