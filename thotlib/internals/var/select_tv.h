/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * variables used for selecting logical elements and contents
 */

THOT_EXPORT int          KeyboardMode;	/* current keyboard */

/* the document containing the current selection, */
/* NULL if there is no selection at all */
THOT_EXPORT PtrDocument  SelectedDocument;

/* number of the view from which the user has made the current selection */
THOT_EXPORT int          SelectedView;

/* first selected element */
THOT_EXPORT PtrElement   FirstSelectedElement;

/* last selected element */
THOT_EXPORT PtrElement   LastSelectedElement;

/* index of first selected character in the the first selected element */
/* 0 if the selection starts at the beginning of the first selected element */
THOT_EXPORT int          FirstSelectedChar;

/* index of last selected character in the last selected element */
/* Zero if the selection ends at the end of the element */
THOT_EXPORT int          LastSelectedChar;

/* the element where the user clicked first when setting the current selection */
THOT_EXPORT PtrElement   FixedElement;

/* rank of the character where the user clickeded first */
THOT_EXPORT int          FixedChar;

/* The current selection is simply a caret (i.e a position, not a range) */
THOT_EXPORT ThotBool     SelPosition;

/* If the current selection is a PolyLine, rank of the selected
   vertex, or 0 if the whole PolyLine is selected */
THOT_EXPORT int          SelectedPointInPolyline;

/* if the current selection is a picture, 1 means that the caret is
   on the right side, 0 means that it is on the left side or that the
   whole picture is selected */
THOT_EXPORT int          SelectedPictureEdge;

/* Views belonging to the same document are synchronized */
THOT_EXPORT ThotBool     SynchronizeViews;

/* Context sensitive menus must be computed when selection changes */
THOT_EXPORT ThotBool     SelectionUpdatesMenus;

/* selection mode */
/* If SelContinue is TRUE, the current selection is represented by variables
   FirstSelectedElement, LastSelectedElement, FirstSelectedChar, and
   LastSelectedChar */
/* If SelContinue is FALSE, the current selection is represented by table
   SelElement */
THOT_EXPORT ThotBool     SelContinue;
/* SelectedColumn is meaningful only when SelContinue is TRUE. In this case,
if SelectedColumn is not NULL, the current selection is all table cells
comprised between FirstSelectedElement and LastSelectedElement that belong
to the same table column whose head is SelectedColumn. FirstSelectedElement
and LastSelectedElement are (within) cells belonging to that column. */
THOT_EXPORT PtrElement   FirstSelectedColumn;
THOT_EXPORT PtrElement   LastSelectedColumn;
/* When FirstSelectedColumn is not NULL, WholeColumnSelected indicates whether
   the whole column is selected or only some (parts of) its cells */
THOT_EXPORT ThotBool     WholeColumnSelected;
/* Current selection mode */
THOT_EXPORT ThotBool 	   StructSelectionMode;

/* selection in the character string of an attribute value */
THOT_EXPORT PtrDocument  DocSelectedAttr;
THOT_EXPORT PtrAbstractBox AbsBoxSelectedAttr;
THOT_EXPORT int          FirstSelectedCharInAttr;
THOT_EXPORT int          LastSelectedCharInAttr;

/* elements corresponding to the items of Selection menu */
THOT_EXPORT PtrElement   SelMenuParentEl;
THOT_EXPORT PtrElement   SelMenuPreviousEl;
THOT_EXPORT PtrElement   SelMenuNextEl;
THOT_EXPORT PtrElement   SelMenuChildEl;

/*THOT_EXPORT PtrAbstractBox LastInsertParagraph;*/
