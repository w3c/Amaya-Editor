/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
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
#define L_Annot             0x40 /* accept only Annot values */
#define L_Bookmarks         0x80 /* accept only Bookmarks values */
#define L_CSS               0x200 /* accept only CSS values */
#define L_TEXT              0x400 /* accept only TEXT values */

/* Values for the profiles */
#define L_BasicValue        0x0F /* allowed in all XHTML profiles */
#define L_StrictValue       0x0E /* allowed in all XHTML profiles except Basic */
#define L_RubyValue         0x04 /* allowed only in Xhtml11 profile */
#define L_TransitionalValue 0x08 /* allowed only in Transitional profile */
#define L_MathMLValue       0x3E /* allowed in MathML profile, SVG profile */
                                 /* and XHTML profiles except Basic */
#define L_RDFaValue         0x10 /* allowed only in XHTML+RDFa profile */
#define L_OtherValue        0xFF

/* 'Extra' profiles */
#define L_NoExtraProfile    0
#define L_RDFa              1

/*----------------------------------------------------------------------
  TtaCanEdit returns TRUE if there is almost one editing function active.
  ----------------------------------------------------------------------*/
extern ThotBool TtaCanEdit ();

/*----------------------------------------------------------------------
   TtaGetProfileFileName:  Get the text for the profile file name.
   name is a provided buffer of length characters to receive the name.
  ----------------------------------------------------------------------*/
extern void TtaGetProfileFileName (char *name, int length);

/*---------------------------------------------------------------
   TtaGetProfilesItems:  Get the text for the profile menu items.
   listEntries is a provided list of length pointers.
   Returns the number of items
------------------------------------------------------------------*/
extern int TtaGetProfilesItems (char **listEntries, int length);

