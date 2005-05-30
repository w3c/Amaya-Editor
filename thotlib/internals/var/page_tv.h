/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
THOT_EXPORT int	        PageHeight;	/* page height (header + body) */
THOT_EXPORT int         RealPageHeight;	/* Page height for the formatter */
/* Note: The formatter does not break pages exactly at the position
   indicated, as this position may cross a line of text horizontally, for
   instance. RealPageHeight is the actual position of the page break, as
   computed by the formatter */

/* Page header boxes whose creation has been delayed */
THOT_EXPORT int		PageFooterHeight;
THOT_EXPORT int		NbBoxesPageHeaderToCreate;
THOT_EXPORT PtrElement 	WorkingPage;
THOT_EXPORT PtrPRule 	PageCreateRule;
THOT_EXPORT PtrPSchema 	PageSchPresRule;
THOT_EXPORT ThotBool	RunningPaginate;
