/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifdef __COLPAGE__
/* page height without the minimum footer */
THOT_EXPORT int	BreakPageHeight;	/* height of page body + header */
THOT_EXPORT int	PageHeaderHeight;	/* height of page header */
THOT_EXPORT int	WholePageHeight;	/* total height (header+body+footer)
					   This variable is recomputed whenever
					   a page of type PgBegin is created */
THOT_EXPORT boolean	StopBeforeCreation;
THOT_EXPORT boolean	FoundPageHF;	/* to know the reason for stopping abstract
				   boxes creation: volume or associated element
				   to be displayed in a page header or footer*/
THOT_EXPORT boolean StopGroupCol;	/* a grouped column has been detected */
THOT_EXPORT boolean ToBalance; 	/* ask for detection of grouped column, in
				   order to balance columns in the same group*/
THOT_EXPORT PtrElement 	HFPageRefAssoc;/* the reference element that designates
			the associated element whose image appears in a page
			header or footer and has caused a page overflow */

THOT_EXPORT PtrAbstractBox	AbsBoxAssocToDestroy;	/* the abstract box to be
			deleted in case of page overflow. It's either the
			abst. box of the associated element or its parent, if
			it is the only in the current page. It's always the
			main abstract box (not a presentation box). */
#else /* __COLPAGE__ */
THOT_EXPORT int		PageHeight;	/* page height (header + body) */
#endif /* __COLPAGE__ */

THOT_EXPORT int      RealPageHeight;		/* Page height for the formatter */
/* Note: The formatter does not break pages exactly at the position
   indicated, as this position may cross a line of text horizontally, for
   instance. RealPageHeight is the actual position of the page break, as
   computed by the formatter */

/* Page header boxes whose creation has been delayed */
THOT_EXPORT int		PageFooterHeight;
THOT_EXPORT int		NbBoxesPageHeaderToCreate;
THOT_EXPORT PtrElement	PageHeaderRefAssoc;
THOT_EXPORT PtrElement 	WorkingPage;
THOT_EXPORT PtrPRule 	PageCreateRule;
THOT_EXPORT PtrPSchema 	PageSchPresRule;
THOT_EXPORT boolean		RunningPaginate;
