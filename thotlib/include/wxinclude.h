#ifdef _WX

/* in this file we can find dialogue creation functions exported from 'amaya' */
extern ThotBool CreateSearchDlgWX (int ref, ThotWindow parent, char *caption,
				   char* searched,  char* replace,
				   ThotBool withReplace, ThotBool searchAfter);
extern ThotBool CreateSpellCheckDlgWX (int ref, ThotWindow parent);
#endif /* _WX */
