/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifdef __COLPAGE__
/* page height without the minimum footer */
EXPORT int	BreakPageHeight;	/* height of page body + header */
EXPORT int	PageHeaderHeight;	/* height of page header */
EXPORT int	WholePageHeight;	/* total height (header+body+footer)
					   This variable is recomputed whenever
					   a page of type PgBegin is created */
EXPORT boolean	StopBeforeCreation;
EXPORT boolean	FoundPageHF;	/* to know the reason for stopping abstract
				   boxes creation: volume or associated element
				   to be displayed in a page header or footer*/
EXPORT boolean StopGroupCol;	/* a grouped column has been detected */
EXPORT boolean ToBalance; 	/* ask for detection of grouped column, in
				   order to balance columns in the same group*/
EXPORT PtrElement 	HFPageRefAssoc;/* the reference element that designates
			the associated element whose image appears in a page
			header or footer and has caused a page overflow */

EXPORT PtrAbstractBox	AbsBoxAssocToDestroy;	/* the abstract box to be
			deleted in case of page overflow. It's either the
			abst. box of the associated element or its parent, if
			it is the only in the current page. It's always the
			main abstract box (not a presentation box). */
#else /* __COLPAGE__ */
EXPORT int		PageHeight;	/* page height (header + body) */
#endif /* __COLPAGE__ */

EXPORT int      RealPageHeight;		/* Page height for the formatter */
/* Note: The formatter does not break pages exactly at the position
   indicated, as this position may cross a line of text horizontally, for
   instance. RealPageHeight is the actual position of the page break, as
   computed by the formatter */

/* Page header boxes whose creation has been delayed */
EXPORT int		PageFooterHeight;
EXPORT int		NbBoxesPageHeaderToCreate;
EXPORT PtrElement	PageHeaderRefAssoc;
EXPORT PtrElement 	WorkingPage;
EXPORT PtrPRule 	PageCreateRule;
EXPORT PtrPSchema 	PageSchPresRule;
EXPORT boolean		RunningPaginate;
