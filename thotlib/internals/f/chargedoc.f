
#ifdef __STDC__

extern void LoadDocumentPiv(PtrDocument *pDoc, char *nomfichier);
extern void  LoadReferedDocuments(PtrDocument pDoc);

#else /* __STDC__ */

extern void LoadDocumentPiv(/* PtrDocument *pDoc, char *nomfichier */);
extern void  LoadReferedDocuments(/* PtrDocument pDoc */);

#endif /* __STDC__ */
