
/*
 * variables used for selecting logical elements and contents
 */

EXPORT int KeyboardMode;	/* current keyboard */

/* the document containing the current selection, */
/* NULL if there is no selection at all */
EXPORT PtrDocument SelectedDocument;

/* number of the view from which the user has selected */
EXPORT int     SelectedView;

/* first selected element */
EXPORT PtrElement FirstSelectedElement;

/* last selected element */
EXPORT PtrElement LastSelectedElement;

/* index of first selected character in the the first selected element */
/* Zero if the selection starts at the beginning of the element */
EXPORT int FirstSelectedChar;

/* index of last selected character in the last selected element */
/* Zero if the selection ends at the end of the element */
EXPORT int LastSelectedChar;

/* The current selection is simply a caret */
EXPORT boolean SelPosition;

/* Views belonging to the same document are synchronized */
EXPORT boolean SynchronizeViews;

/* Context sensitive menus must be computed when selection changes */
EXPORT boolean SelectionUpdatesMenus;

/* selection mode */
/* If TRUE the selection is represented by variables FirstSelectedElement,
LastSelectedElement, FirstSelectedChar, LastSelectedChar */
/* If FALSE the selection is represented by table SelElement */
EXPORT boolean SelContinue;

/* elements corresponding to the items of Selection menu */
EXPORT PtrElement SelMenuParentEl;
EXPORT PtrElement SelMenuPreviousEl;
EXPORT PtrElement SelMenuNextEl;
EXPORT PtrElement SelMenuChildEl;

#ifdef __COLPAGE__
EXPORT PtrElement  SelMenuPageColParent;
EXPORT PtrElement  SelMenuPageColPrev;
EXPORT PtrElement  SelMenuPageColNext;
EXPORT PtrElement  SelMenuPageColChild;
#endif /* __COLPAGE__ */

