
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   label.c :  gestion des labels des elements des arbres abstraits.
   (Thotfon utilise une implementation differente de ce module)
 */

#include "constmedia.h"
#include "typemedia.h"

/* ---------------------------------------------------------------------- */
/* |    NewLabel retourne la valeur entiere du prochain label a         | */
/* |            affecter a un element du document pDoc.                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 NewLabel (PtrDocument pDoc)

#else  /* __STDC__ */
int                 NewLabel (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   pDoc->DocLabelExpMax++;
   return pDoc->DocLabelExpMax;
}

/* ---------------------------------------------------------------------- */
/* |    GetCurrentLabel retourne la valeur entiere du plus grand label  | */
/* |            affecte' a un element du document pDoc.                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 GetCurrentLabel (PtrDocument pDoc)

#else  /* __STDC__ */
int                 GetCurrentLabel (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   return pDoc->DocLabelExpMax;
}

/* ---------------------------------------------------------------------- */
/* |    SetCurrentLabel change la valeur entiere du plus grand label    | */
/* |            affecte' a un element du document pDoc.                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                SetCurrentLabel (PtrDocument pDoc, int label)

#else  /* __STDC__ */
void                SetCurrentLabel (pDoc, label)
PtrDocument         pDoc;
int                 label;

#endif /* __STDC__ */

{
   pDoc->DocLabelExpMax = label;
}
