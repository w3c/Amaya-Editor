/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Module de manipulations des images abstraites.
 *
 * Authors: V. Quint (INRIA)
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "picture.h"
#include "appdialogue.h"

#define THOT_EXPORT extern
#include "page_tv.h"
#include "appdialogue_tv.h"
#include "select_tv.h"
#include "boxes_tv.h"
#include "absboxes_f.h"
#include "abspictures_f.h"
#include "buildboxes_f.h"
#include "changeabsbox_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "presrules_f.h"
#include "schemas_f.h"
#include "scroll_f.h"
#include "structlist_f.h"
#include "structselect_f.h"
#include "tree_f.h"
#include "ustring_f.h"
#include "views_f.h"
#include "displaybox_f.h"

#ifdef _GL
#include "animbox_f.h"
#endif /* _GL */

#define MaxAsc 100
static char BoxTypeName[MAX_TXT_LEN];

/*----------------------------------------------------------------------
  AbsBoxType  rend un pointeur sur un buffer qui contient           
  le type de l'element de structure auquel correspond le  
  pave pointe' par pAb.                                  
  ----------------------------------------------------------------------*/
char *AbsBoxType (PtrAbstractBox pAb, ThotBool origName)
{
  PtrElement       pEl;

  if (pAb == NULL)
    strcpy (BoxTypeName, " ");
  else
    {
      pEl = pAb->AbElement;
      /* copie le nom du type d'element structure auquel appartient la boite */
      if (origName)
        strcpy (BoxTypeName,
                pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrOrigName);
      else
        strcpy (BoxTypeName,
                pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrName);
      if (pAb->AbPresentationBox)
        /* Ajoute le nom du type de boite de presentation */
        {
          strcat (BoxTypeName, ".");
          if (pAb->AbTypeNum == 0)
            strcat (BoxTypeName, "list-item-marker");
          else
            strcat (BoxTypeName,
                    pAb->AbPSchema->PsPresentBox->PresBox[pAb->AbTypeNum - 1]->PbName);
        }
    }
  return (BoxTypeName);
}



/*----------------------------------------------------------------------
  FreeAbView libere, pour une seule vue, tous les paves englobes par le
  pave pointe par pAb, lui-meme compris.
  ----------------------------------------------------------------------*/
void FreeAbView (PtrAbstractBox pAb, int frame)
{
  PtrAbstractBox      pAbb, pAbbNext;
  PtrTextBuffer       pBT, pNextBT;
  PtrPathSeg          pPa, pNextPa;
  PtrDelayedPRule     pDelPR, pNextDelPR;
  ThotPictInfo       *image;
  ThotBool            libAb, computeBBoxes = FALSE;

  if (pAb != NULL)
    {
      /* reformat the last edited paragraph if needed */
      if (pAb == LastInsertParagraph)
        LastInsertParagraph = NULL;
      pAbb = pAb->AbFirstEnclosed;
      /* libere tous les paves englobes */
      while (pAbb != NULL)
        {
          pAbbNext = pAbb->AbNext;
          FreeAbView (pAbb, frame);
          pAbb = pAbbNext;
        }

      if (pAb->AbBox && frame > 0)
        /* remove the box before freeing the abstract box */
        ComputeUpdates (pAb, frame, &computeBBoxes);

      /* dechaine pAb des autres paves */
      if (pAb->AbNext != NULL)
        pAb->AbNext->AbPrevious = pAb->AbPrevious;
      if (pAb->AbPrevious != NULL)
        pAb->AbPrevious->AbNext = pAb->AbNext;
      if (pAb->AbEnclosing != NULL && pAb->AbEnclosing->AbFirstEnclosed == pAb)
        pAb->AbEnclosing->AbFirstEnclosed = pAb->AbNext;
      /* Si c'est un pave obtenu par la regle de presentation Copy,
         libere le descripteur d'element copie' */
      if (pAb->AbCopyDescr != NULL)
        {
          if (pAb->AbCopyDescr->CdPrevious == NULL)
            pAb->AbCopyDescr->CdCopiedElem->ElCopyDescr = pAb->AbCopyDescr->CdNext;
          else
            pAb->AbCopyDescr->CdPrevious->CdNext = pAb->AbCopyDescr->CdNext;
          if (pAb->AbCopyDescr->CdNext != NULL)
            pAb->AbCopyDescr->CdNext->CdPrevious = pAb->AbCopyDescr->CdPrevious;
          FreeDescCopy (pAb->AbCopyDescr);
        }
      /* si c'est un pave de presentation ou le pave d'une reference ou */
      /* celui d'une marque de paire, on libere les buffers */
      libAb = FALSE;
      if (pAb->AbPresentationBox &&
          (pAb->AbLeafType == LtText || pAb->AbLeafType == LtPolyLine ||
           pAb->AbLeafType == LtPicture || pAb->AbLeafType == LtPath))
        libAb = TRUE;
      if (!libAb && !pAb->AbPresentationBox &&
          pAb->AbElement != NULL && pAb->AbElement->ElTerminal &&
          (pAb->AbElement->ElLeafType == LtReference ||
           pAb->AbElement->ElLeafType == LtPairedElem))
        libAb = TRUE;
      if (libAb)
        switch (pAb->AbLeafType)
          {
          case LtText:
          case LtPolyLine:
            if (pAb->AbLeafType == LtPolyLine)
              pBT = pAb->AbPolyLineBuffer;
            else
              pBT = pAb->AbText;
            while (pBT != NULL)
              {
                pNextBT = pBT->BuNext;
                DeleteTextBuffer (&pBT);
                pBT = pNextBT;
              }
            if (pAb->AbLeafType == LtPolyLine)
              pAb->AbPolyLineBuffer = NULL;
            else
              pAb->AbText = NULL;
            break;
          case LtPath:
            pPa = pAb->AbFirstPathSeg;
            while (pPa)
              {
                pNextPa = pPa->PaNext;
                FreePathSeg (pPa);
                pPa = pNextPa;
              }
            pAb->AbFirstPathSeg = NULL;
            break;
          case LtPicture:
            if (pAb->AbPresentationBox)
              {
                /* c'est une boite de presentation image */
                image = (ThotPictInfo *)pAb->AbPictInfo;
                TtaFreeMemory (image->PicFileName);
                CleanPictInfo (image);
                TtaFreeMemory (pAb->AbPictInfo);
                pAb->AbPictInfo = NULL;
              }
            break;
          case LtCompound:
            if (pAb->AbPictBackground != NULL)
              {
                image = (ThotPictInfo *)pAb->AbPictBackground;
                TtaFreeMemory (image->PicFileName);
                /* ce n'est pas un element image */
                CleanPictInfo (image);
                TtaFreeMemory (pAb->AbPictBackground);
                pAb->AbPictBackground = NULL;
              }
            if (pAb->AbPictListStyle != NULL)
              {
                image = (ThotPictInfo *)pAb->AbPictListStyle;
                TtaFreeMemory (image->PicFileName);
                /* ce n'est pas un element image */
                CleanPictInfo (image);
                TtaFreeMemory (pAb->AbPictListStyle);
                pAb->AbPictListStyle = NULL;
              }
            break;
          default:
            break;
          }

      /* dechaine pAb de son element */
      if (pAb->AbElement != NULL)
        {
#ifdef _GL
          AnimatedBoxDel (pAb->AbElement);	  
#endif/*  _GL */
          if (pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] == pAb)
            {
              if (pAb->AbNext != NULL)
                {
                  if (pAb->AbNext->AbElement == pAb->AbElement)
                    pAb->AbElement->ElAbstractBox[pAb->AbDocView- 1] = pAb->AbNext;
                  else
                    pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] = NULL;
                }
              else
                pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] = NULL;
            }
          else if (pAb->AbPresentationBox &&
                   /* Presentation abstract box generated by FnCreateEnclosing */
                   pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] != NULL &&
                   pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1]->AbEnclosing == pAb)
            pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1] = NULL;
        }
      /* libere les regles retardees qui n'ont pas ete appliquees */
      if (pAb->AbDelayedPRule != NULL)
        {
          pDelPR = pAb->AbDelayedPRule;
          while (pDelPR != NULL)
            {
              pNextDelPR = pDelPR->DpNext;
              FreeDifferedRule (pDelPR);
              pDelPR = pNextDelPR;
            }
        }
      FreeAbstractBox (pAb);
    }
}

/*----------------------------------------------------------------------
  FreeAbEl libere, dans toutes les vues, tous les paves de      
  l'element pointe par pEl.                               
  ----------------------------------------------------------------------*/
void                FreeAbEl (PtrElement pEl, PtrDocument pDoc)
{
  PtrAbstractBox      pAbb, pAbbNext;
  int                 v;
  ThotBool            stop;

  if (pEl != NULL && pDoc != NULL)
    for (v = 0; v < MAX_VIEW_DOC; v++)
      {
        pAbb = pEl->ElAbstractBox[v];
        if (pAbb != NULL && pAbb->AbEnclosing != NULL &&
            pAbb->AbEnclosing->AbPresentationBox &&
            pAbb->AbEnclosing->AbElement == pEl)
          /* le pave englobant est un pave' de presentation cree' par */
          /* une regle FnCreateEnclosing */
          pAbb = pAbb->AbEnclosing;
        stop = FALSE;
        do
          {
            if (pAbb == NULL)
              stop = TRUE;
            else if (pAbb->AbElement != pEl)
              stop = TRUE;
            else
              {
                pAbbNext = pAbb->AbNext;
                FreeAbView (pAbb, pDoc->DocViewFrame[v]);
                if (pDoc->DocViewModifiedAb[v] == pAbb)
                  pDoc->DocViewModifiedAb[v] = NULL;
                pAbb = pAbbNext;
              }
          }
        while (!stop);
      }
}

/*----------------------------------------------------------------------
  FreeDeadAbstractBoxes libere tous les paves marques Mort dans le           
  sous-arbre de racine pAb.                              
  ----------------------------------------------------------------------*/
void FreeDeadAbstractBoxes (PtrAbstractBox pAb, int frame)
{
  PtrAbstractBox      pAbb, pAbbNext;

  if (pAb != NULL)
    {
      if (pAb->AbDead)
        FreeAbView (pAb, frame);
      else
        {
          pAbb = pAb->AbFirstEnclosed;
          /* teste tous les paves englobes */
          while (pAbb != NULL)
            {
              pAbbNext = pAbb->AbNext;
              FreeDeadAbstractBoxes (pAbb, frame);
              pAbb = pAbbNext;
            }
        }
    }
}


/*----------------------------------------------------------------------
  AddAbsBoxes complete la vue dont pAbbRoot est le pave racine   
  en ajoutant des paves, en tete si head est vrai,        
  en queue sinon.                                         
  pAbbRoot est une vraie racine de paves               
  ----------------------------------------------------------------------*/
void  AddAbsBoxes (PtrAbstractBox pAbbRoot, PtrDocument pDoc, ThotBool head)
{
  PtrAbstractBox      pAb;
  PtrElement          pEl;
  ThotBool            complete;
  PtrAbstractBox      pAbbReDisp, pAbbR, pFirst, pLast, pAbb;
  ThotBool            stop;

  if ((head && pAbbRoot->AbTruncatedHead) ||
      (!head && pAbbRoot->AbTruncatedTail))
    {
      /* cree les paves de la partie coupee jusqu'a concurrence du
         volume libre */
      pEl = pAbbRoot->AbElement;
      pAb = AbsBoxesCreate (pEl, pDoc, pAbbRoot->AbDocView, (ThotBool)(!head),
                            TRUE, (ThotBool*)(&complete));
      /* recherche tous les paves crees, a partir du premier pave de plus */
      /* haut niveau cree', et aux niveaux inferieurs. */
      while (pAb != NULL)
        {
          /* cherche le premier et le dernier freres crees a ce niveau */
          pFirst = pAb;
          pLast = pAb;
          stop = FALSE;
          do
            if (pLast->AbNext == NULL)
              stop = TRUE;
            else if (!pLast->AbNext->AbNew)
              stop = TRUE;
            else
              pLast = pLast->AbNext;
          while (!stop);
          stop = FALSE;
          do
            if (pFirst->AbPrevious == NULL)
              stop = TRUE;
            else if (!pFirst->AbPrevious->AbNew)
              stop = TRUE;
            else
              pFirst = pFirst->AbPrevious;
          while (!stop);
          if (pFirst == pLast)
            /* un seul pave cree a ce niveau, c'est lui qu'il faudra */
            /* reafficher. */
            pAbbReDisp = pAb;
          else
            /* plusieurs paves crees, on reaffichera l'englobant */
            pAbbReDisp = pAb->AbEnclosing;
          /* modifie les paves environnant les paves crees */
          ApplyRefAbsBoxNew (pFirst, pLast, &pAbbR, pDoc);
          /* conserve le pointeur sur le pave a reafficher */
          pAbbReDisp = Enclosing (pAbbReDisp, pAbbR);
	   
          pEl = pAbbRoot->AbElement;
          pDoc->DocViewModifiedAb[pAbbRoot->AbDocView - 1] =
            Enclosing (pAbbReDisp,
                       pDoc->DocViewModifiedAb[pAbbRoot->AbDocView - 1]);
          /* passe au niveau inferieur */
          if (head)
            pAb = pLast->AbNext;
          else
            pAb = pFirst->AbPrevious;
          if (pAb != NULL)
            {
              /* saute les paves de presentation produits par CreateWith */
              if (head)
                while (pAb->AbPresentationBox && pAb->AbNext != NULL)
                  pAb = pAb->AbNext;
              else
                while (pAb->AbPresentationBox && pAb->AbPrevious != NULL)
                  pAb = pAb->AbPrevious;
              /* passe au premier pave fils */
              pAb = pAb->AbFirstEnclosed;
              if (pAb != NULL)
                {
                  if (head)
                    if (pAb->AbNew)
                      {
                        /* verifie s'il y a un pave ancien a ce niveau */
                        pAbb = pAb;
                        stop = FALSE;
                        do
                          if (pAbb == NULL)
                            {
                              /* pas d'ancien, on s'arrete si l'englobant est */
                              /* lui-meme un nouveau pave */
                              if (pAb->AbEnclosing->AbNew)
                                pAb = NULL;
                              stop = TRUE;
                            }
                          else if (!pAbb->AbNew)
                            stop = TRUE;
                          else
                            pAbb = pAbb->AbNext;
                        while (!stop);
                      }
                    else
                      {
                        /* cherche le premier niveau ou il y a un nouveau
                           en tete */
                        stop = FALSE;
                        do
                          if (pAb == NULL)
                            stop = TRUE;
                          else if (pAb->AbNew)
                            stop = TRUE;
                          else
                            {
                              /* saute les paves de presentation produits par
                                 CreateWith */
                              while (pAb->AbPresentationBox &&
                                     pAb->AbNext != NULL)
                                pAb = pAb->AbNext;
                              pAb = pAb->AbFirstEnclosed;
                            }
                        while (!stop);
                      }
                  else
                    {
                      /* on a ajoute' en queue */
                      /* on cherche le premier cree' a ce niveau */
                      if (pAb->AbNew)
                        {
                          /* il n'y a que des paves nouveaux, on arrete si */
                          /* l'englobant est lui-meme nouveau */
                          if (pAb->AbEnclosing->AbNew)
                            pAb = NULL;
                        }
                      else
                        {
                          stop = FALSE;
                          do
                            if (pAb == NULL)
                              stop = TRUE;
                            else if (pAb->AbNew)
                              stop = TRUE;
                            else if (pAb->AbNext == NULL)
                              {
                                /* saute les paves de presentation produits
                                   par CreateWith */
                                while (pAb->AbPresentationBox &&
                                       pAb->AbPrevious != NULL)
                                  pAb = pAb->AbPrevious;
                                pAb = pAb->AbFirstEnclosed;
                              }
                            else
                              pAb = pAb->AbNext;
                          while (!stop);
                        }
                      /* cherche le dernier  cree' */
                      if (pAb != NULL)
                        while (pAb->AbNext != NULL)
                          pAb = pAb->AbNext;
                    }
                }
            }
        }
    }
}


/*----------------------------------------------------------------------
  IsBreakable retourne vrai si le pave pointe par pAb est         
  englobe (a n'importe quel niveau) par un pave           
  non mis en ligne ou explicitement secable. Les marques  
  de page son traitees comme non-secables.                
  ----------------------------------------------------------------------*/
ThotBool IsBreakable (PtrAbstractBox pAb, PtrDocument pDoc)
{
  ThotBool            unbreakable;

  unbreakable = FALSE;
  /* boucle sur les paves englobants */
  while (pAb && !unbreakable)
    {
      if (pAb->AbElement && pAb->AbLeafType == LtCompound)
        /* pave' compose' */
        {
          if (pAb->AbElement->ElTypeNumber == PageBreak + 1)
            /* c'est une marque de saut de page, non-secable */
            unbreakable = TRUE;
          else
            /* un pave compose' est non-secable s'il est mis en lignes */
            unbreakable = pAb->AbInLine;
        }
      /* regarde dans le schema de presentation du pave s'il est secable */
      if (!unbreakable)
        unbreakable = pAb->AbBuildAll;
      /* passe a l'englobant */
      pAb = pAb->AbEnclosing;
    }
  return (!unbreakable);
}


/*----------------------------------------------------------------------
  KillPresSibling detruit les paves de presentation crees par les   
  regles CreateBefore et CreateAfter de pAb.                     
  ----------------------------------------------------------------------*/
static void KillPresSibling (PtrAbstractBox pAbbSibling, ThotBool ElemIsBefore,
                             PtrDocument pDoc, PtrAbstractBox * pAbbR,
                             PtrAbstractBox * pAbbReDisp, int *volsupp,
                             PtrAbstractBox pAb, ThotBool exceptCrWith)
{
  ThotBool            stop;

  stop = FALSE;
  do
    if (pAbbSibling == NULL)
      stop = TRUE;
    else if (pAbbSibling->AbElement != pAb->AbElement)
      stop = TRUE;
    else
      {
        if (pAbbSibling->AbPresentationBox)
          if (!exceptCrWith || TypeCreatedRule (pDoc, pAb, pAbbSibling) != FnCreateWith)
            {
              *volsupp += pAbbSibling->AbVolume;
              SetDeadAbsBox (pAbbSibling);
              *pAbbReDisp = Enclosing (*pAbbReDisp, pAbbSibling);
              /* traite les paves qui se referent au pave detruit */
              ApplyRefAbsBoxSupp (pAbbSibling, pAbbR, pDoc);
              *pAbbReDisp = Enclosing (*pAbbReDisp, *pAbbR);
            }
        if (ElemIsBefore)
          pAbbSibling = pAbbSibling->AbPrevious;
        else
          pAbbSibling = pAbbSibling->AbNext;
      }
  while (!stop);
}

/*----------------------------------------------------------------------
  KillPresEnclosing supprime tous les paves de presentation           
  crees par pAb et les paves de presentation crees par   
  les paves englobants a l'aide de regles Create et       
  CreateBefore (si head est vrai) ou CreateAfter et       
  CreateLast (si head est faux).                          
  Au retour volsupp indique le volume des paves de        
  presentation tues et pAbbReDisp le pave a reafficher.     
  ----------------------------------------------------------------------*/
static void KillPresEnclosing (PtrAbstractBox pAb, ThotBool head,
                               PtrDocument pDoc, PtrAbstractBox * pAbbReDisp,
                               int *volsupp, ThotBool exceptCrWith)
{
  PtrAbstractBox      pAbbEnclosing, pAbb, pAbbR;
  ThotBool            stop;
  PtrElement          pEl1;

  *volsupp = 0;
  /* Detruit les paves de presentation crees par les regles */
  /* CreateBefore et CreateAfter de pAb. */
  KillPresSibling (pAb->AbPrevious, TRUE, pDoc, &pAbbR, pAbbReDisp, volsupp,
                   pAb, exceptCrWith);
  KillPresSibling (pAb->AbNext, FALSE, pDoc, &pAbbR, pAbbReDisp, volsupp,
                   pAb, exceptCrWith);
  /* traite les paves englobants */
  pAbbEnclosing = pAb->AbEnclosing;
  while (pAbbEnclosing != NULL)
    if ((head && pAbbEnclosing->AbTruncatedHead) ||
        (!head && pAbbEnclosing->AbTruncatedTail))
      /* pave deja traite', on s'arrete */
      pAbbEnclosing = NULL;
    else
      {
        if (IsBreakable (pAbbEnclosing, pDoc))
          {
            TruncateOrCompleteAbsBox (pAbbEnclosing, TRUE, head, pDoc);
            if (head)
              {
                pAbbEnclosing->AbTruncatedHead = TRUE;
                /* cherche et supprimes les paves crees par CreateFirst */
                pAbb = pAbbEnclosing->AbFirstEnclosed;
                stop = FALSE;
                do
                  if (pAbb == NULL)
                    stop = TRUE;
                  else if (!pAbb->AbPresentationBox || pAbb->AbDead ||
                           pAbb->AbElement != pAbbEnclosing->AbElement)
                    /* saute les marques de page de debut d'element */
                    {
                      pEl1 = pAbb->AbElement;
                      if (pEl1->ElTypeNumber == PageBreak + 1
                          && pEl1->ElPageType == PgBegin)
                        pAbb = pAbb->AbNext;
                      else
                        stop = TRUE;
                    }
                  else
                    {
                      *volsupp += pAbb->AbVolume;
                      SetDeadAbsBox (pAbb);
                      *pAbbReDisp = Enclosing (*pAbbReDisp, pAbb);
                      /* traite les paves qui se referent au pave detruit */
                      ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
                      *pAbbReDisp = Enclosing (*pAbbReDisp, pAbbR);
                      pAbb = pAbb->AbNext;
                    }
                while (!stop);
                /* cherche et supprime les paves crees par CreateBefore */
                /* mais ne supprime pas ceux crees par CreateWith */
                pAbb = pAbbEnclosing->AbPrevious;
                stop = FALSE;
                do
                  if (pAbb == NULL)
                    stop = TRUE;
                  else if (!pAbb->AbPresentationBox || pAbb->AbDead ||
                           pAbb->AbElement != pAbbEnclosing->AbElement)
                    stop = TRUE;
                  else
                    {
                      if (TypeCreatedRule (pDoc, pAbbEnclosing, pAbb) != FnCreateWith)
                        {
                          *volsupp += pAbb->AbVolume;
                          SetDeadAbsBox (pAbb);
                          *pAbbReDisp = Enclosing (*pAbbReDisp, pAbb);
                          /* traite les paves qui se referent au pave detruit */
                          ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
                          *pAbbReDisp = Enclosing (*pAbbReDisp, pAbbR);
                        }
                      pAbb = pAbb->AbPrevious;
                    }
                while (!stop);
              }
            else
              {
                /* head = FALSE */
                pAbbEnclosing->AbTruncatedTail = TRUE;
                /* cherche et supprime les paves crees par CreateLast */
                pAbb = pAbbEnclosing->AbFirstEnclosed;
                /* cherche d'abord le dernier pave englobe */
                stop = FALSE;
                if (pAbb != NULL)
                  do
                    if (pAbb->AbNext == NULL)
                      stop = TRUE;
                    else
                      pAbb = pAbb->AbNext;
                  while (!stop);
                stop = FALSE;
                do
                  if (pAbb == NULL)
                    stop = TRUE;
                  else if (!pAbb->AbPresentationBox || pAbb->AbDead ||
                           pAbb->AbElement != pAbbEnclosing->AbElement)
                    stop = TRUE;
                  else
                    {
                      *volsupp += pAbb->AbVolume;
                      SetDeadAbsBox (pAbb);
                      *pAbbReDisp = Enclosing (*pAbbReDisp, pAbb);
                      /* traite les paves qui se referent au pave detruit */
                      ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
                      *pAbbReDisp = Enclosing (*pAbbReDisp, pAbbR);
                      pAbb = pAbb->AbPrevious;
                    }
                while (!stop);
                /* cherche et supprime les paves crees par CreateAfter */
                /* mais ne supprime pas ceux crees par CreateWith */
                pAbb = pAbbEnclosing->AbNext;
                stop = FALSE;
                do
                  if (pAbb == NULL)
                    stop = TRUE;
                  else if (!pAbb->AbPresentationBox || pAbb->AbDead ||
                           pAbb->AbElement != pAbbEnclosing->AbElement)
                    stop = TRUE;
                  else
                    {
                      if (TypeCreatedRule (pDoc, pAbbEnclosing, pAbb) != FnCreateWith)
                        {
                          *volsupp += pAbb->AbVolume;
                          SetDeadAbsBox (pAbb);
                          *pAbbReDisp = Enclosing (*pAbbReDisp, pAbb);
                          /* traite les paves qui se referent au pave detruit */
                          ApplyRefAbsBoxSupp (pAbb, &pAbbR, pDoc);
                          *pAbbReDisp = Enclosing (*pAbbReDisp, pAbbR);
                        }
                      pAbb = pAbb->AbNext;
                    }
                while (!stop);
              }
          }
        /* passe au niveau superieur */
        pAbbEnclosing = pAbbEnclosing->AbEnclosing;
      }
}

/*----------------------------------------------------------------------
  SupprAbsBoxes reduit de dvol le volume du pave pAbbRoot en        
  supprimant des paves, en tete si head est vrai, en      
  queue sinon.                                            
  Au retour dvol contient le volume qui reste a supprimer.
  suppression page par page si vue avec pages             
  cette procedure est toujours appelee avec la racine de  
  la vue si le document est mis en pages.                 
  ----------------------------------------------------------------------*/
static void  SupprAbsBoxes (PtrAbstractBox pAbbRoot, PtrDocument pDoc,
                            ThotBool head, int *dvol)
{
  PtrAbstractBox      pAb, pAbbSibling, pAbbReDisp, pAbbR;
  int                 volsupp, volpres;
  ThotBool            stop, stop1;
  PtrElement          pEl1;

  /* cherche le premier pave englobe' de volume inferieur a dvol et qui */
  /* soit secable */
  pAb = pAbbRoot;
  volpres = 0;
  /* volume des paves de presentation des elem englobants */
  stop = FALSE;
  do
    if (pAb == NULL)
      stop = TRUE;
    else if (pAb->AbEnclosing != NULL &&
             (pAb->AbVolume + volpres <= *dvol || !IsBreakable (pAb, pDoc)))
      stop = TRUE;
    else
      {
        /* premier pave du niveau inferieur */
        pAb = pAb->AbFirstEnclosed;
        if (pAb != NULL)
          /* si on supprime en tete, cherche le premier pave qui ne soit */
          /* pas un pave de presentation */
          /* si on supprime en queue, cherche le dernier pave qui ne soit */
          /* pas un pave de presentation */
          {
            if (!head)
              while (pAb->AbNext != NULL)
                pAb = pAb->AbNext;
            stop1 = FALSE;
            do
              if (pAb == NULL)
                stop1 = TRUE;
              else if (!pAb->AbPresentationBox)
                stop1 = TRUE;
              else if (pAb->AbFirstEnclosed != NULL &&
                       pAb->AbFirstEnclosed->AbElement == pAb->AbElement)
                /* un pave de presentation cree' par la regle FnCreateEnclosing */
                /* on le considere comme un pave' d'element structure' */
                stop1 = TRUE;
              else
                {
                  volpres += pAb->AbVolume;
                  if (head)
                    pAb = pAb->AbNext;
                  else
                    pAb = pAb->AbPrevious;
                }
            while (!stop1);
          }
      }
  while (!stop);
  if (pAb == NULL)
    *dvol = 0;
  else if (pAb->AbEnclosing == NULL)
    /* don't destroy the Root box */
    *dvol = 0;
  else if (pAb->AbVolume + volpres > *dvol)
    /* on ne peut rien supprimer */
    *dvol = 0;
  if (*dvol > 0)
    {
      /* on peut supprimer le pave pAb */
      /* calcule le nouveau volume qui restera a supprimer apres la */
      /* suppression de pAb */
      *dvol -= pAb->AbVolume;
      pAbbReDisp = pAb;
      /* il faudra reafficher au moins pAb */
      /* tous les paves englobant pAb sont coupe's. On supprime leurs */
      /* paves de presentation. On supprime aussi ceux crees par le pave */
      /* tue'. */
      KillPresEnclosing (pAb, head, pDoc, &pAbbReDisp, &volsupp, FALSE);
      *dvol -= volsupp;
      /* detruit le pave trouve' et toute sa descendance */
      SetDeadAbsBox (pAb);
      /* detruit les paves freres tant qu'il reste du volume a supprimer */
      pAbbSibling = pAb;
      while (*dvol > 0 && pAbbSibling != NULL)
        {
          if (head)
            pAbbSibling = pAbbSibling->AbNext;
          else
            pAbbSibling = pAbbSibling->AbPrevious;
          if (pAbbSibling != NULL)
            /* on ne supprime pas les pave's de presentation, sauf ceux qui */
            /* ont ete cree's par une regle FnCreateEnclosing */
            if (!pAbbSibling->AbPresentationBox ||
                (pAbbSibling->AbPresentationBox &&
                 pAbbSibling->AbFirstEnclosed != NULL &&
                 pAbbSibling->AbFirstEnclosed->AbElement ==
                 pAbbSibling->AbElement))
              SupprAbsBoxes (pAbbSibling, pDoc, head, dvol);
          /* traite ensuite les paves qui se referent au pave detruit */
        }
      ApplyRefAbsBoxSupp (pAb, &pAbbR, pDoc);
      pAbbReDisp = Enclosing (pAbbReDisp, pAbbR);
      pEl1 = pAbbRoot->AbElement;
      pDoc->DocViewModifiedAb[pAbbRoot->AbDocView - 1] =
        Enclosing (pAbbReDisp, pDoc->DocViewModifiedAb[pAbbRoot->AbDocView - 1]);
    }			/* fin dvol > 0 */
}

/*----------------------------------------------------------------------
  IncreaseVolume Le Mediateur augmente de dVol le volume affichable  
  dans la fenetre ViewFrame.
  Met a jour la capacite de la vue affichee dans cette frame et cree de
  nouveaux paves en tete ou en queue, selon le booleen head, de l'image
  abstraite affichee dans ViewFrame.             
  On cree des paves, le Mediateur se charge du reaffichage
  ----------------------------------------------------------------------*/
void IncreaseVolume (ThotBool head, int dVol, int frame)
{
  PtrDocument         pDoc;
  PtrAbstractBox      pAb;
  int                 view, h;

  if (dVol <= 0)
    return;
  GetDocAndView (frame, &pDoc, &view);
  /* met a jour la nouvelle capacite de la vue, indique dans le contexte */
  /* du document le volume des paves a creer et cherche le pave racine de */
  /* la vue */
  if (pDoc != NULL && pDoc->DocViewRootAb[view - 1])
    {
      /* element de l'arbre principal */
      pAb = pDoc->DocViewRootAb[view - 1];
      pDoc->DocViewVolume[view - 1] = pAb->AbVolume + dVol;
      pDoc->DocViewFreeVolume[view - 1] = dVol;
      if (IsBreakable (pAb, pDoc))
        {
          /* cree les paves de la partie qui va apparaitre */
          AddAbsBoxes (pAb, pDoc, head);
          
          /* signale au Mediateur les paves crees et detruits */
          h = PageHeight;
          if (pDoc->DocViewModifiedAb[view - 1] != NULL)
            {
              pAb = pDoc->DocViewModifiedAb[view - 1];
              pDoc->DocViewModifiedAb[view - 1] = NULL;
              (void) ChangeConcreteImage (frame, &h, pAb);
              FreeDeadAbstractBoxes (pAb, pDoc->DocViewFrame[view - 1]);
            }
        }
    }
}


/*----------------------------------------------------------------------
  DecreaseVolume Le Mediateur reduit de dVol le volume affichable      
  dans la fenetre frame.
  Met a jour la capacite de la vue affichee dans cette frame et supprime
  des paves en tete ou en queue, selon le booleen head, de l'image abstraite
  affichee dans frame.
  On supprime des paves, le Mediateur se charge du reaffichage
  ----------------------------------------------------------------------*/
void DecreaseVolume (ThotBool head, int dVol, int frame)
{
  PtrDocument         pDoc;
  PtrAbstractBox      pAb;
  int                 view, h;

  if (dVol <= 0)
    return;
  GetDocAndView (frame, &pDoc, &view);
  /* met a jour la nouvelle capacite de la vue et cherche le pave racine */
  /* de la vue */
  if (pDoc != NULL)
    {
      pAb = pDoc->DocViewRootAb[view - 1];
      pDoc->DocViewVolume[view - 1] = pAb->AbVolume - dVol;
      /* supprime les paves */
      SupprAbsBoxes (pAb, pDoc, head, &dVol);
      /* signale au Mediateur les paves modifies */
      h = PageHeight;
      if (pDoc->DocViewModifiedAb[view - 1] != NULL)
        {
          pAb = pDoc->DocViewModifiedAb[view - 1];
          pDoc->DocViewModifiedAb[view - 1] = NULL;
          (void) ChangeConcreteImage (frame, &h, pAb);
          FreeDeadAbstractBoxes (pAb, pDoc->DocViewFrame[view - 1]);
        }
    }
}


/*----------------------------------------------------------------------
  CheckAbsBox verifie que l'element pointe' par pEl a au moins      
  un pave dans la vue view. S'il n'en a pas, essaie d'en   
  creer un en modifiant l'image abstraite de cette vue.   
  Si debut est vrai, on cree l'image de la vue en         
  commencant par l'element pointe' par pEl, sinon, on     
  place cet element au milieu de l'image creee.           
  Si display est Vrai, l'image est reaffichee.            
  ----------------------------------------------------------------------*/
void CheckAbsBox (PtrElement pEl, int view, PtrDocument pDoc, ThotBool begin,
                  ThotBool display)
{
  ThotBool            openedView, creation, stop;
  PtrElement          pElAscent, pEl1;
  PtrElement          pAsc[MaxAsc];
  int                 NumAsc, i, volsupp, frame, h;
  PtrSSchema          pSchS;
  PtrPSchema          pSchP;
  PtrPRule            pPRule;
  PtrAttribute        pAttr;
  PtrAbstractBox      pAbbDestroyed, pAbbRemain, pAbbLastEmptyCr,
    pAbbFirstEmptyCr, pAbbReDisp, pAbbRoot, pPrevious;
  ThotBool            complete, appl;

  pAbbLastEmptyCr = NULL;
  pAbbRoot = NULL;
  for (i = 0; i < MaxAsc; i++)
    pAsc[i] = NULL;
  if (pEl != NULL && pEl->ElAbstractBox[view - 1] == NULL)
    {
      /* verifie si la vue a ete creee */
      openedView = pDoc->DocView[view - 1].DvPSchemaView > 0;

      /* si la vue n'est pas creee, il n'y a rien a faire */
      if (openedView)
        {
          /* cherche les elements ascendants qui n'ont pas de pave dans */
          /* cette vue */
          NumAsc = 0;
          /* niveau dans la pile des elements dont il faut */
          /* creer un pave */
          pElAscent = pEl;
          stop = FALSE;
          do
            if (pElAscent == NULL)
              stop = TRUE;
            else if (pElAscent->ElAbstractBox[view - 1] != NULL)
              stop = TRUE;
            else
              /* met un element dans la pile */
              {
                if (NumAsc < MaxAsc)
                  {
                    pAsc[NumAsc] = pElAscent;
                    NumAsc++;
                  }
                /* passe a l'ascendant */
                pElAscent = pElAscent->ElParent;
              }
          while (!stop);

          if (pAsc[NumAsc - 1]->ElParent == NULL)
            /* la racine de l'arbre n'a pas de pave dans cette vue */
            /* c'est une creation de vue */
            creation = TRUE;
          else
            creation = FALSE;
          /* essaie de creer les paves de ces elements, en commencant par */
          /* celui qui contient tous les autres. Il s'agit seulement de */
          /* trouver s'il y a un pave ascendant non encore cree et visible */
          pAbbFirstEmptyCr = NULL;
          if (openedView && NumAsc > 0)
            {
              i = NumAsc;
              do
                {
                  i--;
                  pEl1 = pAsc[i];
                  /* cree juste le pave, sans sa descendance et sans */
                  /* lui appliquer les regles de presentation. */
                  pPrevious = AbsBoxesCreate (pAsc[i], pDoc, view, TRUE, FALSE,
                                              &complete);
                  if (pEl1->ElAbstractBox[view - 1] != NULL)
                    pPrevious = pEl1->ElAbstractBox[view - 1];
                  if (pPrevious != NULL)
                    /* marque sur le pave cree qu'il faudra lui appliquer ses */
                    /* regles de presentation (ce sera fait par AbsBoxesCreate) */
                    pPrevious->AbSize = -1;
		  
                  if (pPrevious != NULL)
                    /* on a cree un pave */
                    {
                      pAbbLastEmptyCr = pPrevious;
                      /* dernier pave cree' */
                      if (pAbbFirstEmptyCr == NULL)
                        pAbbFirstEmptyCr = pPrevious; /* 1er pave cree' */
                    }
                }
              while (i > 0);
            }

          /* si aucun pave n'a ete cree', il n'y a rien d'autre a faire */
          if (pAbbFirstEmptyCr != NULL)
            {
              if (creation)
                /* initialise le pointeur sur la racine de la vue si */
                /* c'est une creation de vue */
                {
                  pDoc->DocViewRootAb[view - 1] = pAbbFirstEmptyCr;
                  pAbbRoot = pDoc->DocViewRootAb[view - 1];
                  frame = pDoc->DocViewFrame[view - 1];
                }
              else
                /* ce n'est pas une creation de vue */
                /* detruit l'ancien contenu de la vue */
                {
                  pAbbRemain = pAbbFirstEmptyCr;
                  /* tue les paves de presentation des elements englobants */
                  pAbbReDisp = NULL;
                  KillPresEnclosing (pAbbRemain, TRUE, pDoc, &pAbbReDisp, &volsupp, TRUE);
                  pAbbReDisp = NULL;
                  KillPresEnclosing (pAbbRemain, FALSE, pDoc, &pAbbReDisp, &volsupp, TRUE);
                  /* detruit les paves qui precedent et qui suivent le pave */
                  /* cree de plus haut niveau et ses paves englobants */
                  while (pAbbRemain != NULL)
                    {
                      pAbbDestroyed = pAbbRemain->AbPrevious;
                      while (pAbbDestroyed != NULL)
                        {
                          if (!pAbbDestroyed->AbDead)
                            /* on ne detruit pas les paves des elements restant */
                            if (pAbbDestroyed->AbElement != pAbbRemain->AbElement)
                              {
                                SetDeadAbsBox (pAbbDestroyed);
                                pAbbReDisp = NULL;
                                ApplyRefAbsBoxSupp (pAbbDestroyed, &pAbbReDisp, pDoc);
                              }
                          pAbbDestroyed = pAbbDestroyed->AbPrevious;
                        }
                      pAbbDestroyed = pAbbRemain->AbNext;
                      while (pAbbDestroyed != NULL)
                        {
                          if (!pAbbDestroyed->AbDead)
                            /* on ne detruit pas les paves des elements restant */
                            if (pAbbDestroyed->AbElement != pAbbRemain->AbElement)
                              {
                                SetDeadAbsBox (pAbbDestroyed);
                                pAbbReDisp = NULL;
                                ApplyRefAbsBoxSupp (pAbbDestroyed, &pAbbReDisp, pDoc);
                              }
                          pAbbDestroyed = pAbbDestroyed->AbNext;
                        }
                      pAbbRemain = pAbbRemain->AbEnclosing;
                      /* passe a l'englobant */
                    }
                  /* fait effacer tout le contenu de la vue par le Mediateur */
                  h = 0;    /* on ne s'occupe pas de la hauteur de page */
                  pAbbRoot = pDoc->DocViewRootAb[view - 1];
                  frame = pDoc->DocViewFrame[view - 1];
                  if (frame != 0)
                    {
                      pDoc->DocViewModifiedAb[view - 1] = NULL;
                      pAbbRoot->AbDead = TRUE;
                      ChangeConcreteImage (frame, &h, pAbbRoot);
                      pAbbRoot->AbDead = FALSE;
                      FreeDeadAbstractBoxes (pAbbRoot, frame);
                      /* the root box should be regenerated */
                      pAbbRoot->AbNew = TRUE;
                    }
                }
              if (begin)
                pDoc->DocViewFreeVolume[view - 1] = pDoc->DocViewVolume[view - 1];
              else
                pDoc->DocViewFreeVolume[view - 1] = pDoc->DocViewVolume[view - 1] / 2;
              /* marque comme anciens tous les paves conserves (pour */
              /* que AddAbsBoxes travaille correctement) */
              pAbbRemain = pAbbLastEmptyCr;
              while (pAbbRemain != NULL)
                {
                  pAbbRemain->AbNew = FALSE;
                  if (creation)
                    /* a priori les paves ne sont pas complets */
                    if (pAbbRemain->AbLeafType == LtCompound
                        && !pAbbRemain->AbInLine)
                      {
                        pAbbRemain->AbTruncatedHead = TRUE;
                        pAbbRemain->AbTruncatedTail = TRUE;
                      }
                  pAbbRemain = pAbbRemain->AbEnclosing;
                  /* passe a l'englobant */
                }
              /* applique les regles des paves nouvellement crees et cree */
              /* d'autres paves derriere */
              AddAbsBoxes (pAbbRoot, pDoc, FALSE);
              /* complete les boites qui doivent etre completes */
              i = NumAsc;
              do
                {
                  pPRule = GlobalSearchRulepEl (pAsc[i - 1], pDoc, &pSchP,
                                                &pSchS, FALSE, 0, NULL, pDoc->DocView[view - 1].DvPSchemaView,
                                                PtGather, FnAny, FALSE, TRUE, &pAttr);
                  if (pPRule)
                    if (BoolRule (pPRule, pAsc[i - 1], view, &appl))
                      /* cet element a la regle Gather: Yes */
                      /* cree le pave avec toute sa descendance, si */
                      /* ce n'est pas encore fait */
                      {
                        pPrevious = AbsBoxesCreate (pAsc[i - 1], pDoc, view,
                                                    TRUE, TRUE, &complete);
                        i = 1;
                      }
                  i--;
                }
              while (i != 0);
              if (!begin)
		  
                /* cree d'autres paves devant, jusqu'a remplir le volume de */
                /* la fenetre */
                {
                  pDoc->DocViewFreeVolume[view - 1] = pDoc->DocViewVolume[view - 1] / 2;
                  /* marque comme anciens tous les paves de presentation qui */
                  /* viennent d'etre crees par AddAbsBoxes devant les paves */
                  /* conserves. Ces paves de presentation seront ainsi traites */
                  /* correctement lors du prochain appel de AddAbsBoxes. */
                  pAbbRemain = pAbbLastEmptyCr;
                  while (pAbbRemain != NULL)
                    {
                      pPrevious = pAbbRemain->AbPrevious;
                      stop = FALSE;
                      do
                        if (pPrevious == NULL)
                          stop = TRUE;
                        else if (pPrevious->AbElement != pAbbRemain->AbElement)
                          stop = TRUE;
                        else
                          {
                            pPrevious->AbNew = FALSE;
                            pPrevious = pPrevious->AbPrevious;
                          }
                      while (!stop);
                      pAbbRemain = pAbbRemain->AbEnclosing;
                      /* passe a l'englobant */
                    }
                  /* cree de nouveaux paves */
                  AddAbsBoxes (pAbbRoot, pDoc, TRUE);
                  /* marque comme nouveaux tous les paves de presentation qui */
                  /* viennent d'etre marques anciens. Ces paves de presentation */
                  /* seront ainsi traites correctement par le Mediateur. */
                  pAbbRemain = pAbbLastEmptyCr;
                  while (pAbbRemain != NULL)
                    {
                      pPrevious = pAbbRemain->AbPrevious;
                      stop = FALSE;
                      do
                        if (pPrevious == NULL)
                          stop = TRUE;
                        else if (pPrevious->AbElement != pAbbRemain->AbElement)
                          stop = TRUE;
                        else
                          {
                            pPrevious->AbNew = TRUE;
                            pPrevious = pPrevious->AbPrevious;
                          }
                      while (!stop);
                      pAbbRemain = pAbbRemain->AbEnclosing;
                      /* passe a l'englobant */
                    }
                }
              /* marque comme nouveaux tous les paves conserves (pour qu'ils */
              /* soient traites correctement par le Mediateur) */
              pAbbRemain = pAbbLastEmptyCr;
              while (pAbbRemain != NULL)
                {
                  pAbbRemain->AbNew = TRUE;
                  pAbbRemain = pAbbRemain->AbEnclosing;
                  /* passe a l'englobant */
                }
              /* indique les nouvelles modifications au Mediateur et */
              /*  lui fait reafficher toute la vue */
              if (frame != 0)
                {
                  h = 0;
                  /* il n'y a plus rien a reafficher dans cette vue */
                  pDoc->DocViewModifiedAb[view - 1] = NULL;
                  ChangeConcreteImage (frame, &h, pAbbRoot);
                  if (display)
                    DisplayFrame (frame);
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  VolumeTree    retourne                                        
  *volBefore: le volume total des elements de l'arbre abstrait   
  qui precedent l'element auquel appartient le pave  
  pAbbFirst.                                           
  *volAfter: le volume total des elements de l'arbre abstrait   
  qui se trouvent apres l'element auquel appartient  
  le pave pAbbLast.                                    
  volTree: le volume total de l'arbre abstrait.               
  ----------------------------------------------------------------------*/
void  VolumeTree (PtrAbstractBox pAbbRoot, PtrAbstractBox pAbbFirst,
                  PtrAbstractBox pAbbLast, int *volBefore, int *volAfter,
                  int *volTree)
{
  PtrElement          pEl;
  
  if (pAbbFirst == NULL || pAbbLast == NULL)
    /* les parametres d'appels sont errone's */
    {
      *volBefore = 0;
      *volAfter = 0;
      *volTree = 0;
    }
  else
    {
      /* calcule d'abord le volume total de l'arbre abstrait */
      *volTree = pAbbRoot->AbElement->ElVolume;
      
      /* on calcule maintenant le volume de ce qui precede */
      *volBefore = 0;
      if (pAbbRoot->AbTruncatedHead)
        {
          pEl = pAbbFirst->AbElement;
          /* pour l'element et tous ses ascendants, on accumule le */
          /* volume de tous leurs freres precedents */
          do
            {
              /* traite tous les freres precedents */
              while (pEl->ElPrevious != NULL)
                {
                  pEl = pEl->ElPrevious;
                  *volBefore += pEl->ElVolume;
                }
              /* passe a l'ascendant */
              pEl = pEl->ElParent;
            }
          while (pEl != NULL);
        }
      
      /* on calcule le volume de ce qui suit */
      *volAfter = 0;
      if (pAbbRoot->AbTruncatedTail)
        {
          pEl = pAbbLast->AbElement;
          /* pour l'element et tous ses ascendants, on accumule le */
          /* volume de tous leurs freres suivants */
          do
            {
              /* traite tous les freres suivants */
              while (pEl->ElNext != NULL)
                {
                  pEl = pEl->ElNext;
                  *volAfter += pEl->ElVolume;
                }
              /* passe a l'ascendant */
              pEl = pEl->ElParent;
            }
          while (pEl != NULL);
        }
    }
}


/*----------------------------------------------------------------------
  JumpIntoView fait afficher dans la fenetre la partie de        
  document qui se trouve a la distance indiquee du debut  
  de l'arbre abstrait.                                    
  distance est un pourcentage : 0 <= distance <= 100      
  Si distance = 0, on fait afficher le debut de l'arbre.  
  Si distance = 100, on fait afficher la fin de l'arbre.  
  ----------------------------------------------------------------------*/
void JumpIntoView (int frame, int distance)
{
  PtrDocument         pDoc;
  int                 view;
  PtrElement          pEl;
  int                 volBefore, volPresent;
  ThotBool            after;
  int                 position;
  PtrAbstractBox      pAbbRoot, pAb;
  PtrElement          pElSuiv;
  
  position = 0;
  if (distance >= 0 && distance <= 100)
    /* la distance demandee est valide */
    {
      /* cherche le document et la vue correspondant a la fenetre */
      GetDocAndView (frame, &pDoc, &view);
      if (pDoc != NULL)
        {
          /* prend la racine de l'arbre affiche' dans cette frame */
          pEl = pDoc->DocDocElement;
          if (pEl != NULL)
            {
              pAbbRoot = pEl->ElAbstractBox[view - 1];
              /* volume avant l'element a rendre visible */
              volBefore = (pEl->ElVolume * distance) / 100;
              /* cherche l'element a rendre visible */
              volPresent = 0;
              while (!pEl->ElTerminal && pEl->ElFirstChild != NULL)
                {
                  pEl = pEl->ElFirstChild;
                  while (volPresent + pEl->ElVolume <= volBefore
                         && pEl->ElNext != NULL)
                    {
                      volPresent += pEl->ElVolume;
                      pEl = pEl->ElNext;
                    }
                }
              if (pEl == NULL)
                pAb = NULL;
              else
                {
                  pAb = pEl->ElAbstractBox[view - 1];
                  if (pAb == NULL)
                    {
                      CheckAbsBox (pEl, view, pDoc, FALSE, FALSE);
                      pAb = pEl->ElAbstractBox[view - 1];
                    }
                }
              after = TRUE;
              while (pEl != NULL && pAb == NULL)
                /* pEl n'a pas de pave dans le vue traitee. */
                /* On cherche la feuille la plus proche de pEl qui */
                /* soit visible dans la vue */
                {
                  if (after)
                    pElSuiv = NextElement (pEl);
                  else
                    pElSuiv = NULL;
                  if (pElSuiv != NULL)
                    /* on prend la feuille pElSuivante */
                    if (!pElSuiv->ElTerminal)
                      pEl = FirstLeaf (pElSuiv);
                    else
                      pEl = pElSuiv;
                  else
                    /* on prend la feuille precedente */
                    {
                      after = FALSE;
                      while (pEl->ElPrevious == NULL && pEl->ElParent != NULL)
                        pEl = pEl->ElParent;
                      pEl = pEl->ElPrevious;
                      if (pEl != NULL)
                        while (!pEl->ElTerminal && pEl->ElFirstChild != NULL)
                          {
                            pEl = pEl->ElFirstChild;
                            while (pEl->ElNext != NULL)
                              pEl = pEl->ElNext;
                          }
                    }
		  
                  /* rend l'element visible dans sa frame, sans l'afficher */
                  if (pEl != NULL)
                    {
                      CheckAbsBox (pEl, view, pDoc, FALSE, FALSE);
                      pAb = pEl->ElAbstractBox[view - 1];
                    }
                }
	      
              pAb = NULL;
              /* positionne l'image dans sa frame, en l'affichant */
              if (distance == 0)
                /* boite racine de la vue en haut de la fenetre */
                {
                  position = 0;
                  pAb = pAbbRoot;
                }
              else if (distance == 100)
                /* boite racine de la vue en bas de la fenetre */
                {
                  position = 2;
                  pAb = pAbbRoot;
                }
              else if (pEl)
                /* boite de l'element au milieu de la fenetre */
                {
                  if (pEl->ElAbstractBox[view - 1] == NULL)
                    /* cherche le premier element precedent qui a */
                    /* un pave' dans la vue */
                    pEl = BackSearchVisibleElem (pAbbRoot->AbElement, pEl, view);
                  if (pEl != NULL)
                    pAb = pEl->ElAbstractBox[view - 1];
                  position = 1;
                }
              if (pAb)
                {
                  ShowBox (frame, pAb->AbBox, position, 0, TRUE);
                  /* update the selection */
                  if (pDoc == SelectedDocument)
                    ShowSelection (SelectedDocument->DocViewRootAb[view], FALSE);
                }
            }
        }
    }
}




