#ifdef _WX

/* in this file we can find dialogue creation functions exported from 'amaya' */
extern ThotBool CreateSearchDlgWX (int ref, ThotWindow parent, char *caption,
				   char* searched,  char* replace,
				   ThotBool withReplace, ThotBool anycase, ThotBool searchAfter);
extern ThotBool CreateSpellCheckDlgWX (int ref, int base, ThotWindow parent,
				       int checkingArea);
extern ThotBool CreateStyleDlgWX ( int ref, ThotWindow parent);
extern ThotBool CreateTextDlgWX ( int ref, int subref, ThotWindow parent,
				  const char *title, const char *label,
				  const char *value );
extern ThotBool CreateListDlgWX( int ref, int subref, ThotWindow parent, char *title,
				 int nb_item, char *items);
extern ThotBool CreateListEditDlgWX( int ref, ThotWindow parent,
				     const char *title, const char * list_title,
				     int nb_item, const char *items, const char * selected_item );
extern ThotBool CreateNumDlgWX( int ref, int subref, ThotWindow parent,
				const char *title,
				const char *label,
				int value );
extern ThotBool CreateEnumListDlgWX( int ref, int subref, ThotWindow parent,
				     const char *title,
				     const char *label,
				     int nb_item,
				     const char *items,
				     int selection );
#endif /* _WX */
