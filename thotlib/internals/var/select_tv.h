/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 

/*
 * variables used for selecting logical elements and contents
 */

THOT_EXPORT int KeyboardMode;	/* current keyboard */

/* the document containing the current selection, */
/* NULL if there is no selection at all */
THOT_EXPORT PtrDocument SelectedDocument;

/* number of the view from which the user has selected */
THOT_EXPORT int     SelectedView;

/* first selected element */
THOT_EXPORT PtrElement FirstSelectedElement;

/* last selected element */
THOT_EXPORT PtrElement LastSelectedElement;

/* index of first selected character in the the first selected element */
/* Zero if the selection starts at the beginning of the element */
THOT_EXPORT int FirstSelectedChar;

/* index of last selected character in the last selected element */
/* Zero if the selection ends at the end of the element */
THOT_EXPORT int LastSelectedChar;

/* The current selection is simply a caret */
THOT_EXPORT boolean SelPosition;

/* Views belonging to the same document are synchronized */
THOT_EXPORT boolean SynchronizeViews;

/* Context sensitive menus must be computed when selection changes */
THOT_EXPORT boolean SelectionUpdatesMenus;

/* selection mode */
/* If TRUE the selection is represented by variables FirstSelectedElement,
LastSelectedElement, FirstSelectedChar, LastSelectedChar */
/* If FALSE the selection is represented by table SelElement */
THOT_EXPORT boolean SelContinue;

/* elements corresponding to the items of Selection menu */
THOT_EXPORT PtrElement SelMenuParentEl;
THOT_EXPORT PtrElement SelMenuPreviousEl;
THOT_EXPORT PtrElement SelMenuNextEl;
THOT_EXPORT PtrElement SelMenuChildEl;

#ifdef __COLPAGE__
THOT_EXPORT PtrElement  SelMenuPageColParent;
THOT_EXPORT PtrElement  SelMenuPageColPrev;
THOT_EXPORT PtrElement  SelMenuPageColNext;
THOT_EXPORT PtrElement  SelMenuPageColChild;
#endif /* __COLPAGE__ */

