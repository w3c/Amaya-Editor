/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2004
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * profiles.h : define profiles data 
 */
#define MAX_PRO_LENGTH              200       /* Maximum length of a string */
#define DEF_FILE                   "amaya.profiles"

/* Masks used for the profiles */
#define L_Other             0x00 /* No one profile is defined */
#define L_Basic             0x01 /* accept only Basic values */
#define L_Strict            0x02 /* accept Strict and Basic values */
#define L_Xhtml11           0x04 /* accept Strict, Basic and Ruby values */
#define L_Transitional      0x08 /* accept all values except Ruby */
#define L_MathML            0x10 /* accept only MathML values */
#define L_SVG               0x20 /* accept only SVG values */

/* Values for the profiles */
#define L_BasicValue        0x0F /* allowed in all XHTML profiles */
#define L_StrictValue       0x0E /* allowed in all XHTML profiles except Basic */
#define L_RubyValue         0x04 /* allowed only in Xhtml11 profile */
#define L_TransitionalValue 0x08 /* allowed only in Transitional profile */
#define L_MathMLValue       0x1E /* allowed in MathML profile and  */
                                 /* XHTML profiles except Basic */
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
   TtaGetProfileFileName:  Get the text for the profile file name.
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

