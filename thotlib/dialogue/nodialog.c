/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   nodialog : fonctions vides pour l'edition de lien
   Major changes:
   I. Vatton - Mai 1994
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "frame.h"

int                 FindePage = 0;

#include "memory.f"

/*******************************************************************/
/* Les fonctions vides pour l'edition de lien */

#ifdef __STDC__
void                Ind_Creation (PtrElement pEl, PtrDocument pDoc)
#else  /* __STDC__ */
void                Ind_Creation (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                Ind_VerifMarque (PtrElement * pEl, PtrDocument pDoc)
#else  /* __STDC__ */
void                Ind_VerifMarque (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
}


#ifdef __STDC__
Pixmap              TtaCreatePixmapLogo (char **data)
#else  /* __STDC__ */
Pixmap              TtaCreatePixmapLogo (data)
char              **data;

#endif /* __STDC__ */
{
   return None;
}


#ifdef __STDC__
void                ConfigTranslateSSchema (PtrSSchema pSS)
#else  /* __STDC__ */
void                ConfigTranslateSSchema (pSS)
PtrSSchema        pSS;

#endif /* __STDC__ */
{
}

void                ReaffPaveCopie ()
{
}

void                DrawAjAttr ()
{
}
#ifdef __STDC__
void                AfCar (char car, int frame, int x, int y, ptrfont font, int RO, int active, int fg)

#else  /* __STDC__ */
void                AfCar (car, frame, x, y, font, RO, active, fg)
char                car;
int                 frame;
int                 x;
int                 y;
ptrfont             font;
int                 RO;
int                 active;
int                 fg;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                DrawSupprAttr (PtrAttribute pAttr, PtrElement pEl)
#else  /* __STDC__ */
void                DrawSupprAttr (pAttr, pEl)
PtrAttribute         pAttr;
PtrElement          pEl;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                AttachAttrWithValue (PtrElement pEl, PtrDocument pDoc, PtrAttribute pNewAttr)
#else  /* __STDC__ */
void                AttachAttrWithValue (pEl, pDoc, pNewAttr)
PtrElement          pEl;
PtrDocument         pDoc;
PtrAttribute         pNewAttr;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
boolean             DansTampon (PtrElement pEl)
#else  /* __STDC__ */
boolean             DansTampon (pEl)
PtrElement          pEl;

#endif /* __STDC__ */
{
   return FALSE;
}

#ifdef __STDC__
void                SaisitNomSchPres (PtrSSchema pSchStr, Name name)
#else  /* __STDC__ */
void                SaisitNomSchPres (pSchStr, name)
PtrSSchema        pSchStr;
Name                 name;

#endif /* __STDC__ */
{
}

boolean             RemplRefer ()
{
   return FALSE;
}

#ifdef __STDC__
void                NouvPosition (PtrAbstractBox pAb, int DeltaX, int DeltaY, int frame, boolean Disp)
#else  /* __STDC__ */
void                NouvPosition (pAb, DeltaX, DeltaY, frame, Disp)
PtrAbstractBox             pAb;
int                 DeltaX;
int                 DeltaY;
int                 frame;
boolean             Disp;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                NouvDimension (PtrAbstractBox pAb, int DeltaX, int DeltaY, int frame, boolean Disp)
#else  /* __STDC__ */
void                NouvDimension (pAb, DeltaX, DeltaY, frame, Disp)
PtrAbstractBox             pAb;
int                 DeltaX;
int                 DeltaY;
int                 frame;
boolean             Disp;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
boolean             PavResizable (PtrAbstractBox pAb, boolean Horiz)
#else  /* __STDC__ */
boolean             PavResizable (pAb, Horiz)
PtrAbstractBox             pAb;
boolean             Horiz;

#endif /* __STDC__ */
{
   return TRUE;
}

#ifdef __STDC__
boolean             PavMovable (PtrAbstractBox pAb, boolean Horiz)
#else  /* __STDC__ */
boolean             PavMovable (pAb, Horiz)
PtrAbstractBox             pAb;
boolean             Horiz;

#endif /* __STDC__ */
{
   return TRUE;
}

#ifdef __STDC__
boolean             MemesRegleSpecif (PtrElement pEl1, PtrElement pEl2)
#else  /* __STDC__ */
boolean             MemesRegleSpecif (pEl1, pEl2)
PtrElement          pEl1;
PtrElement          pEl2;

#endif /* __STDC__ */
{
   return TRUE;
}

#ifdef __STDC__
void                ModPresent (boolean OnlyChars)
#else  /* __STDC__ */
void                ModPresent (OnlyChars)
boolean             OnlyChars;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                MontrerBoite (int frame, PtrBox boite, int position, int pourcent)
#else  /* __STDC__ */
void                MontrerBoite (frame, boite, position, pourcent)
int                 frame;
PtrBox            boite;
int                 position;
int                 pourcent;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
int                 IdentDocument (PtrDocument pDoc)
#else  /* __STDC__ */
int                 IdentDocument (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   return 1;
}

#ifdef __STDC__
boolean             ThotSendMessage (int *notifyEvent, boolean pre)
#else  /* __STDC__ */
boolean             ThotSendMessage (notifyEvent, pre)
int                *notifyEvent;
boolean             pre;

#endif /* __STDC__ */
{
   return FALSE;
}

#ifdef __STDC__
boolean             SendAttributeMessage (NotifyAttribute * notifyAttr, boolean pre)
#else  /* __STDC__ */
boolean             SendAttributeMessage (notifyAttr, pre)
NotifyAttribute    *notifyAttr;
boolean             pre;

#endif /* __STDC__ */
{
   return FALSE;
}

void                EndInsert ()
{
}

#ifdef __STDC__
void                FinInsertParagraph (PtrAbstractBox pAb, int frame)
#else  /* __STDC__ */
void                FinInsertParagraph (pAb, frame)
PtrAbstractBox             pAb;
int                 frame;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                InsertChar (unsigned char car, int clavier)
#else  /* __STDC__ */
void                InsertChar (car, clavier)
unsigned char       car;
int                 clavier;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                EmacsCommand (int code)
#else  /* __STDC__ */
void                EmacsCommand (code)
int                 code;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                EditionEtSpecials (int numero, boolean IsCar)
#else  /* __STDC__ */
void                EditionEtSpecials (numero, IsCar)
int                 numero;
boolean             IsCar;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                CollerXBuffer (unsigned char *Xbuffer, int nbytes)
#else  /* __STDC__ */
void                CollerXBuffer (Xbuffer, nbytes)
unsigned char      *Xbuffer;
int                 nbytes;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
int                 CopierXBuffer (unsigned char **buffer)
#else  /* __STDC__ */
int                 CopierXBuffer (buffer)
unsigned char     **buffer;

#endif /* __STDC__ */
{
   return 0;
}

#ifdef __STDC__
void                MajScrolls (int frame)
#else  /* __STDC__ */
void                MajScrolls (frame)
int                 frame;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                ChangeTitre (int frame, char *texte)
#else  /* __STDC__ */
void                ChangeTitre (frame, texte)
int                 frame;
char               *texte;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                SetClip (int frame, int orgx, int orgy, int *xd, int *yd, int *xf, int *yf)
#else  /* __STDC__ */
void                SetClip (frame, orgx, orgy, xd, yd, xf, yf)
int                 frame;
int                 orgx;
int                 orgy;
int                *xd;
int                *yd;
int                *xf;
int                *yf;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                ResetClip (int frame)
#else  /* __STDC__ */
void                ResetClip (frame)
int                 frame;

#endif /* __STDC__ */
{
   FindePage = 1;
}

#ifdef __STDC__
void                SetCursorWatch (int thotWindowid)
#else  /* __STDC__ */
void                SetCursorWatch (thotWindowid)
int                 thotWindowid;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                ResetCursorWatch (int thotWindowid)
#else  /* __STDC__ */
void                ResetCursorWatch (thotWindowid)
int                 thotWindowid;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                ChaineChoix (PtrElement pEl, PtrElement * p, PtrDocument pDoc)

#else  /* __STDC__ */
void                ChaineChoix (pEl, p, pDoc)
PtrElement          pEl;
PtrElement         *p;
PtrDocument         pDoc;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                VisuSelect (PtrAbstractBox PavRac, boolean Visible)
#else  /* __STDC__ */
void                VisuSelect (PavRac, Visible)
PtrAbstractBox             PavRac;
boolean             Visible;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                SetSelect (int frame, boolean Allume)
#else  /* __STDC__ */
void                SetSelect (frame, Allume)
int                 frame;
boolean             Allume;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
PtrElement          SelSuivant (PtrElement pEl, PtrElement PcLast)

#else  /* __STDC__ */
PtrElement          SelSuivant (pEl, PcLast)
PtrElement          pEl;
PtrElement          PcLast;

#endif /* __STDC__ */
{
   return NULL;
}

#ifdef __STDC__
void                SelAjoute (PtrElement pEl, boolean dernier)

#else  /* __STDC__ */
void                SelAjoute (pEl, dernier)
PtrElement          pEl;
boolean             dernier;

#endif /* __STDC__ */

{
}

#ifdef __STDC__
void                ResetSelect (int frame)
#else  /* __STDC__ */
void                ResetSelect (frame)
int                 frame;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                SelectEl (PtrDocument pDoc, PtrElement pEl, boolean Debut, boolean Controle)

#else  /* __STDC__ */
void                SelectEl (pDoc, pEl, Debut, Controle)
PtrDocument         pDoc;
PtrElement          pEl;
boolean             Debut;
boolean             Controle;

#endif /* __STDC__ */

{
}

#ifdef __STDC__
void                ReevalMrq (ViewSelection * marque)
#else  /* __STDC__ */
void                ReevalMrq (marque)
ViewSelection            *marque;

#endif /* __STDC__ */
{
}


#ifdef __STDC__
void                AllumeSelection (boolean DebVisible, boolean drag)
#else  /* __STDC__ */
void                AllumeSelection (DebVisible, drag)
boolean             DebVisible;
boolean             drag;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                AnnuleSelect ()

#else  /* __STDC__ */
void                AnnuleSelect ()
#endif				/* __STDC__ */
{
}


#ifdef __STDC__
void                FusEtSel (PtrDocument SelDoc, PtrElement PremSel, PtrElement DerSel, int premcar, int dercar)

#else  /* __STDC__ */
void                FusEtSel (SelDoc, PremSel, DerSel, premcar, dercar)
PtrDocument         SelDoc;
PtrElement          PremSel;
PtrElement          DerSel;
int                 premcar;
int                 dercar;

#endif /* __STDC__ */
{
}


#ifdef __STDC__
void                MajPavSelect (int frame, PtrAbstractBox pAb, boolean Etat)
#else  /* __STDC__ */
void                MajPavSelect (frame, pAb, Etat)
int                 frame;
PtrAbstractBox             pAb;
boolean             Etat;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                ModeCreation (PtrBox pBox, int frame)
#else  /* __STDC__ */
void                ModeCreation (pBox, frame)
PtrBox            pBox;
int                 frame;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
boolean             PaveAffiche (PtrAbstractBox pav, int frame)
#else  /* __STDC__ */
boolean             PaveAffiche (pav, frame)
PtrAbstractBox             pav;
int                 frame;

#endif /* __STDC__ */
{
   return FALSE;
}

/* Les fonctions vides pour l'edition de lien   */
#ifdef __STDC__
void                Clear (int frame, int larg, int haut, int x, int y)
#else  /* __STDC__ */
void                Clear (frame, larg, haut, x, y)
int                 frame;
int                 larg;
int                 haut;
int                 x;
int                 y;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
boolean             ConfigGetPSchemaNature (PtrSSchema pSS, char *nomNature, char *presNature)
#else  /* __STDC__ */
boolean             ConfigGetPSchemaNature (pSS, nomNature, presNature)
PtrSSchema        pSS;
char               *nomNature;
char               *presNature;

#endif /* __STDC__ */
{
   return FALSE;
}


#ifdef __STDC__
boolean             ConfigDefaultPSchema (char *schema, char *schpres)

#else  /* __STDC__ */
boolean             ConfigDefaultPSchema (schema, schpres)
char               *schema;
char               *schpres;

#endif /* __STDC__ */
{
   return FALSE;
}

/* Les procedures de geom.c */
#ifdef __STDC__
void                ChPosition (int frame, int *x, int *y, int large, int haut, int xr, int yr, int xmin, int xmax, int ymin, int ymax, int xm, int ym)
#else  /* __STDC__ */
void                ChPosition (frame, x, y, large, haut, xr, yr, xmin, xmax, ymin, ymax, xm, ym)
int                 frame;
int                *x;
int                *y;
int                 large;
int                 haut;
int                 xr;
int                 yr;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
int                 xm;
int                 ym;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                ChDimension (int frame, int x, int y, int *large, int *haut, int xr, int yr, int xmin, int xmax, int ymin, int ymax, int xm, int ym)
#else  /* __STDC__ */
void                ChDimension (frame, x, y, large, haut, xr, yr, xmin, xmax, ymin, ymax, xm, ym)
int                 frame;
int                 x;
int                 y;
int                *large;
int                *haut;
int                 xr;
int                 yr;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
int                 xm;
int                 ym;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                GeomCreation (int frame, int *x, int *y, int xr, int yr, int *large, int *haut, int xmin, int xmax, int ymin, int ymax, int PosX, int PosY, int DimX, int DimY)
#else  /* __STDC__ */
void                GeomCreation (frame, x, y, xr, yr, large, haut, xmin, xmax, ymin, ymax, PosX, PosY, DimX, DimY)
int                 frame;
int                *x;
int                *y;
int                 xr;
int                 yr;
int                *large;
int                *haut;
int                 xmin;
int                 xmax;
int                 ymin;
int                 ymax;
int                 PosX;
int                 PosY;
int                 DimX;
int                 DimY;

#endif /* __STDC__ */
{
}

#ifdef __STDC__
void                PolyLineModification (int frame, int x, int y, PtrTextBuffer Pbuffer, PtrTextBuffer Bbuffer, int nbpoints, int point, boolean close)
#else  /* __STDC__ */
void                PolyLineModification (frame, x, y, Pbuffer, Bbuffer, nbpoints, point, close)
int                 frame;
int                 x;
int                 y;
PtrTextBuffer      Pbuffer;
PtrTextBuffer      Bbuffer;
int                 nbpoints;
int                 point;
boolean             close;

#endif /* __STDC__ */
{
}
/*******************************************************************/

/*******************************************************************/

/* procedures vides de page.c (cas de la pagination depuis print)  */
/*******************************************************************/
