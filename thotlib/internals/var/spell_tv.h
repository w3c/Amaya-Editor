/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * variables for the spelling checker
 */

THOT_EXPORT int	       NC;	/* maximum number of corrections */
THOT_EXPORT char       ChkrCorrection[MAX_PROPOSAL_CHKR+1][MAX_WORD_LEN];  /* corrections */
THOT_EXPORT char       ChkrErrWord[MAX_WORD_LEN];               /* erroneous words */
THOT_EXPORT PtrElement ChkrElement;	     /* Element to be corrected */
THOT_EXPORT int	       ChkrIndChar;	/* index of erroneous word */
THOT_EXPORT Language   ChkrLanguage;     /* current language */
THOT_EXPORT PtrSearchContext ChkrRange;      /* range to be checked */
THOT_EXPORT PtrDict    ChkrFileDict;     /* dictionary for current document */
THOT_EXPORT char       RejectedChar[MAX_REJECTED_CHARS];
THOT_EXPORT ThotBool   IgnoreUppercase;  /* ignore words containing an uppercase char. */
THOT_EXPORT ThotBool   IgnoreArabic;  /* ignore words containing arabic digits */
THOT_EXPORT ThotBool   IgnoreRoman;  /* ignore words containing roman digits */
THOT_EXPORT ThotBool   IgnoreSpecial;  /* ignore words containing special characters */
