
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

#ifndef __CEXTRACT__
#ifdef __STDC__

extern void TraiteSauverDoc();
extern void docModify ( PtrDocument doc, PtrElement pEl );
extern int NbVueExiste(PtrDocument pDoc);
extern void DirDeDocu ( PtrDocument pdoc, PathBuffer Buf );
extern boolean VuePaginee(PtrDocument pDoc, int vue, boolean assoc);
extern void VueFen(int nfen, PtrDocument pD, int *nv, boolean *assoc);
extern void DocVueFen ( int nfen, PtrDocument *pD, int *nv, boolean *assoc );
extern int LesVuesDunDoc ( PtrDocument pDoc, AvailableView LesVues );
extern void PaginerDoc ( PtrDocument pDoc );
extern void LibDocument ( PtrDocument *pDoc );
extern void dest1vue ( PtrDocument pDoc, DocViewNumber v );
extern void detruit ( PtrDocument pDoc, int vue, boolean assoc, boolean AvecFermeDoc );
extern void MajAccessMode(PtrDocument pDoc, int accessMode);
extern void MajElInclus ( PtrElement pEl, PtrDocument pDoc );
extern void changenomdoc ( PtrDocument pDoc, char *Nm );
extern boolean SauverDoc ( PtrDocument pDoc, Name NomDuDocument, PathBuffer NomDirectory, boolean SauveDocAvecCopie, boolean SauveDocAvecMove );
extern boolean SauveDocument ( PtrDocument pDoc, int Mode );
extern void DestVue ( int nfen );
extern void OuvreVuesInit ( PtrDocument pDoc );
extern void LoadDocument ( PtrDocument *pDoc, char *nomfichier );
extern void NewDocument ( PtrDocument *pDoc, PtrBuffer nomschema, Name nomdoc, PathBuffer nomdir );
extern void NouveauDocument ( PtrBuffer nomsch );
extern void OuvreVueCreee ( PtrDocument pDoc, int vue, boolean Assoc, int X, int Y, int L, int H );
extern int CreVueNommee ( PtrDocument pDoc, Name nom, int X, int Y, int L, int H);
extern void TraiteRetMenuVues ( PtrDocument pDoc, int VdView, PtrElement SousArbreVueAOuvrir, DocViewNumber VueDeReference );
extern void MenuVuesAOuvrir ( PtrDocument pDoc, char *Buf, int *nbitem );
extern void OuvreVue ( boolean SousArbre );
extern void FermerVueDoc ( PtrDocument pDoc, int nv, boolean assoc );
extern void FermerVue ( void );
extern int CreeImageAbstraite(PtrDocument pDoc, int v, int r, PtrSSchema pSS, int vuedesignee, boolean debut, PtrElement RacineVue);

#else /* __STDC__ */

extern void TraiteSauverDoc();
extern int NbVueExiste(/*PtrDocument pDoc*/);
extern void docModify (/* PtrDocument doc, PtrElement pEl */);
extern void DirDeDocu (/* PtrDocument pdoc, PathBuffer Buf */);
extern boolean VuePaginee(/* PtrDocument pDoc, int vue, boolean assoc */);
extern void VueFen(/* int nfen, PtrDocument pD, int *nv, boolean *assoc */);
extern void DocVueFen (/* int nfen, PtrDocument *pD, int *nv, boolean *assoc */);
extern int LesVuesDunDoc (/* PtrDocument pDoc, AvailableView LesVues */);
extern void PaginerDoc (/* PtrDocument pDoc */);
extern void LibDocument (/* PtrDocument *pDoc */);
extern void dest1vue (/* PtrDocument pDoc, DocViewNumber v */);
extern void detruit (/* PtrDocument pDoc, int vue, boolean assoc, boolean AvecFermeDoc */);
extern void MajAccessMode(/* PtrDocument pDoc, int accessMode */);
extern void MajElInclus (/* PtrElement pEl, PtrDocument pDoc */);
extern void changenomdoc (/* PtrDocument pDoc, char *Nm */);
extern boolean SauverDoc (/* PtrDocument pDoc, Name NomDuDocument, PathBuffer NomDirectory, boolean SauveDocAvecCopie, boolean SauveDocAvecMove */);
extern boolean SauveDocument (/* PtrDocument pDoc, int Mode */);
extern void DestVue (/* int nfen */);
extern void OuvreVuesInit (/* PtrDocument pDoc */);
extern void LoadDocument (/* PtrDocument *pDoc, char *nomfichier */);
extern void NewDocument (/* PtrDocument *pDoc, PtrBuffer nomschema, Name nomdoc, PathBuffer nomdir */);
extern void NouveauDocument (/* PtrBuffer nomsch */);
extern void OuvreVueCreee (/* PtrDocument pDoc, int vue, boolean Assoc, int X, int Y, int L, int H */);
extern int CreVueNommee (/* PtrDocument pDoc, Name nom, int X, int Y, int L, int H */);
extern void TraiteRetMenuVues (/* PtrDocument pDoc, int VdView, PtrElement SousArbreVueAOuvrir, DocViewNumber VueDeReference */);
extern void MenuVuesAOuvrir (/* PtrDocument pDoc, char *Buf, int *nbitem */);
extern void OuvreVue (/* boolean SousArbre */);
extern void FermerVueDoc (/* PtrDocument pDoc, int nv, boolean assoc */);
extern void FermerVue (/* void */);
extern int CreeImageAbstraite(/*PtrDocument pDoc, int v, int r, PtrSSchema pSS, int vuedesignee, boolean debut, PtrElement RacineVue*/);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
