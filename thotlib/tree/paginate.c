/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2010
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */


/*
 * gestion de la pagination d'un arbre abstrait. Insere les diverses
 * marques de saut de page dans l'AA. Les effets de bord sont nombreux.
 * Ce module insere les marques de saut de page dans la
 * structure abstraite des documents.
 *
 * Authors: V. Quint (INRIA)
 *          I. Vatton (INRIA)
 *          C. Roisin (INRIA) - Pagination at printing time
 *
 */
#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "libmsg.h"
#include "message.h"
#include "appdialogue.h"
#include "fileaccess.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "appdialogue_tv.h"
#include "boxes_tv.h"
#include "units_tv.h"
#include "page_tv.h"

#include "absboxes_f.h"
#include "applicationapi_f.h"
#include "attributes_f.h"
#include "buildboxes_f.h"
#include "boxpositions_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "changepresent_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "documentapi_f.h"
#include "docs_f.h"
#include "exceptions_f.h"
#include "font_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "paginate_f.h"
#include "pagecommands_f.h"
#include "presrules_f.h"
#include "print_f.h"
#include "schemas_f.h"
#include "structcreation_f.h"
#include "structlist_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "presvariables_f.h"
#include "references_f.h"
#include "tableH_f.h"
#include "tree_f.h"
#include "units_f.h"
#include "writepivot_f.h"

#ifdef _GL
#include "glwindowdisplay.h"
#endif /*_GL*/

#define MaxPageLib 20
#define HMinPage 60		/* Hauteur minimum du corps de la page */
static int          pagesCounter;


#ifndef PAGINEETIMPRIME
/*----------------------------------------------------------------------
  AbortPageSelection
  Annule et deplace si besoin la selection courante du document.
  Retourne les valeurs de cette selection dans firstSelection, lastSelection,
  FirstSelectedChar et LastSelectedChar
  ----------------------------------------------------------------------*/
static ThotBool AbortPageSelection (PtrDocument pDoc, int schView,
                                    PtrElement *firstSelection,
                                    PtrElement *lastSelection,
                                    int *FirstSelectedChar, int *LastSelectedChar)
{
  PtrDocument         SelDoc;
  PtrElement          pEl1, pEl2, first, last;
  ThotBool            sel;

  /* demande quelle est la selection courante */
  sel = GetCurrentSelection (&SelDoc, &first, &last, FirstSelectedChar, LastSelectedChar);
  if (sel && SelDoc != pDoc)
    sel = FALSE;
  /* annule la selection si elle est dans le document a paginer */
  if (sel)
    {
      CancelSelection ();
      /* on verifie si la selection commence ou se termine sur une marque */
      /* de page qui va disparaitre et dans ce cas on change la selection */
      pEl1 = first;		/* debut de la selection */
      if (pEl1->ElTypeNumber == PageBreak + 1 &&
          pEl1->ElViewPSchema == schView &&
          pEl1->ElPageType == PgComputed)
        {
          /* c'est une marque de page qui va disparaitre */
          if (pEl1->ElNext != NULL)
            /* on selectionne l'element suivant la marque de page */
            {
              if (last == first)
                last = pEl1->ElNext;
              first = pEl1->ElNext;
            }
          else if (pEl1->ElPrevious != NULL)
            /* on selectionne l'element precedent la marque de page */
            {
              if (last == first)
                last = pEl1->ElPrevious;
              first = pEl1->ElPrevious;
            }
          else
            /* pas de suivant ni de precedent */
            /* on selectionne l'element englobant la marque de page */
            {
              first = pEl1->ElParent;
              last = pEl1->ElParent;
            }
        }
      pEl1 = last;
      /* dernier element de la selection */
      if (pEl1->ElTypeNumber == PageBreak + 1 &&
          pEl1->ElViewPSchema == schView &&
          pEl1->ElPageType == PgComputed)
        {
          /* le dernier element de la selection est une marque de */
          /* page qui va disparaitre */
          if (pEl1->ElPrevious != NULL)
            last = pEl1->ElPrevious;
          /* on selectionne le precedent */
          else if (pEl1->ElNext != NULL)
            last = pEl1->ElNext;
          /* on selectionne le suivant */
          else
            /* on selectionne l'englobant */
            {
              first = pEl1->ElParent;
              last = pEl1->ElParent;
            }
        }
      /* le debut de la selection est-il dans une feuille de texte qui
         n'est separee de la precedente que par une marque de page ? Dans
         ce cas il y aura fusion des deux feuilles et la deuxieme
         n'existera plus. */
      pEl1 = first;		/* debut de la selection */
      if (pEl1->ElTerminal && pEl1->ElLeafType == LtText &&
          /* la selection debute dans une feuille de texte */
          pEl1->ElPrevious && pEl1->ElPrevious->ElTypeNumber == PageBreak + 1 &&
          pEl1->ElPrevious->ElViewPSchema == schView &&
          pEl1->ElPrevious->ElPageType == PgComputed)
        /* la feuille de texte est precedee d'une marque de
           page qui va disparaitre, on examine l'element
           precedent la marque de page */
        {
          pEl2 = pEl1->ElPrevious->ElPrevious;
          if (pEl2 != NULL && pEl2->ElTerminal &&
              pEl2->ElLeafType == LtText &&
              /* c'est une feuille de texte */
              pEl2->ElLanguage == pEl1->ElLanguage &&
              /* meme langue */
              SameAttributes (first, pEl2) &&
              /* memes attributs */
              BothHaveNoSpecRules (first, pEl2))
            /* meme present. */
            /* les elements vont fusionner, on selectionne le 1er */
            {
              if (last == first)
                {
                  last = pEl2;
                  *LastSelectedChar = 0;
                }
              first = pEl2;
              *LastSelectedChar = 0;
            }
        }
      /* la fin de la selection est-il dans une feuille de texte qui
         n'est separee de la precedente que par une marque de page ? Dans
         ce cas il y aura fusion des deux feuilles et la deuxieme
         n'existera plus. */
      pEl1 = last;
      /* fin de la selection */
      if (pEl1->ElTerminal && pEl1->ElLeafType == LtText &&
          /* la selection se termine dans une feuille de texte */
          pEl1->ElPrevious &&
          pEl1->ElPrevious->ElTypeNumber == PageBreak + 1 &&
          pEl1->ElPrevious->ElViewPSchema == schView &&
          pEl1->ElPrevious->ElPageType == PgComputed)
        /* la feuille de texte est precedee d'une marque de
           page qui va disparaitre, on examine l'element
           precedent la marque de page */
        {
          pEl2 = pEl1->ElPrevious->ElPrevious;
          if (pEl2 && pEl2->ElTerminal &&
              pEl2->ElLeafType == LtText &&
              /* c'est une feuille de texte */
              pEl2->ElLanguage == pEl1->ElLanguage &&
              /* meme langue */
              SameAttributes (last, pEl2) &&
              /* memes attributs */
              BothHaveNoSpecRules (last, pEl2))
            /* meme present. */
            /* les elements vont fusionner, on selectionne le 1er */
            {
              last = pEl2;
              *LastSelectedChar = 0;
            }
        }
      *firstSelection = first;
      *lastSelection = last;
    }
  return sel;
}
#endif /* PAGINEETIMPRIME */


/*----------------------------------------------------------------------
  InsertPageInTable       Si pElPage est une marque de page dans une
  structure demandant une coupure spe'ciale, cree les elements a repeter
  devant et derriere le saut de page puis cree les paves de ces elements
  ainsi que ceux du saut de page
  et retourne Vrai. Si on n'est pas dans une structure a coupure speciale,
  ne fait rien et retourne Faux.
  ----------------------------------------------------------------------*/
void InsertPageInTable (PtrElement pElPage, PtrDocument pDoc, int viewNb,
                        ThotBool * cutDone)
{
  PtrElement          pEl, pElToCopy, pSpecial;
  ThotBool             finish;

  *cutDone = FALSE;
  /* cherche les ascendants qui demandent un traitement special des */
  /* coupures par saut de page */
  pSpecial = pElPage->ElParent;
  finish = FALSE;
  while (!finish)
    {
      if (pSpecial == NULL)
        finish = TRUE;
      else
        {
          if (TypeHasException (ExcPageBreak, pSpecial->ElTypeNumber,
                                pSpecial->ElStructSchema))
            /* cet element demande une coupure speciale */
            {
              *cutDone = TRUE;
              /*cherche l'element a repeter */
              pElToCopy = SearchTypeExcept (pSpecial, pElPage,
                                            ExcPageBreakRepBefore, FALSE);
              if (pElToCopy != NULL)
                /* il y a bien un element a repeter avant le saut de page */
                {
                  pEl = NewSubtree (pElToCopy->ElTypeNumber,
                                    pElToCopy->ElStructSchema, pDoc,
                                    FALSE, TRUE, TRUE, TRUE);
                  GetReference (&pEl->ElSource);
                  if (pEl->ElSource == NULL)
                    DeleteElement (&pEl, pDoc);
                  else
                    {
                      pEl->ElSource->RdElement = pEl;
                      pEl->ElSource->RdTypeRef = RefInclusion;
                      /* insere l'element cree' dans l'arbre abstrait */
                      InsertElementBefore (pElPage, pEl);
                      /* lie l'inclusion a l'element trouve' */
                      if (SetReference (pEl, NULL, pElToCopy, pDoc, pDoc,
                                        FALSE, FALSE))
                        /* l'element a inclure est connu, on le copie */
                        CopyIncludedElem (pEl, pDoc);
                      /* cree les paves du nouvel element */
                      CreateAllAbsBoxesOfEl (pEl, pDoc);
                    }
                }
              if (viewNb > 0)
                /* cree les paves du saut de page */
                CreateNewAbsBoxes (pElPage, pDoc, viewNb);

              /*cherche l'element a repeter */
              pElToCopy = SearchTypeExcept (pSpecial, pElPage,
                                            ExcPageBreakRepetition, FALSE);
              if (pElToCopy != NULL)
                /* il y a bien un element a repeter apres le saut de page */
                {
                  pEl = NewSubtree (pElToCopy->ElTypeNumber,
                                    pElToCopy->ElStructSchema, pDoc,
                                    FALSE, TRUE, TRUE, TRUE);
                  /* associe un bloc reference a l'element cree' */
                  GetReference (&pEl->ElSource);
                  if (pEl->ElSource == NULL)
                    DeleteElement (&pEl, pDoc);
                  else
                    {
                      pEl->ElSource->RdElement = pEl;
                      pEl->ElSource->RdTypeRef = RefInclusion;
                      /* insere l'element cree dans l'arbre abstrait */
                      InsertElementAfter (pElPage, pEl);
                      /* lie l'inclusion a l'element trouve' */
                      if (SetReference (pEl, NULL, pElToCopy, pDoc, pDoc, FALSE, FALSE))
                        /* l'element a inclure est connu, on le copie */
                        CopyIncludedElem (pEl, pDoc);
                      /* cree les paves du nouvel element */
                      CreateAllAbsBoxesOfEl (pEl, pDoc);
                    }
                }
            }
          pSpecial = pSpecial->ElParent;	/* passe a l'ascendant */
        }
    }
}


/*----------------------------------------------------------------------
  ExcCutPage est appele' par CutCommand qui effectue le traitement
  de la commande Couper.
  pElFirstSel et pElLastSel pointent le premier et le dernier element
  selectionne's, qui doivent etre coupe's.
  S'il s'agit d'un seul et meme element saut de page qui se trouve
  dans une structure demandant un traitement special des sauts de
  pages, on etend la selection a l'element portant l'exception
  PageBreakRepBefore qui precede ce saut de page et a l'element
  portant l'exception PageBreakRepetition qui
  suit, pour que CutCommand coupe les 3 elements a la fois.
  Dans ce cas, on met toBeSaved a Faux (on ne sauvera pas les elements
  coupe's dans le buffer Couper-Copier-Coller) et deletePage a
  Vrai (on detruira le saut de page bien qu'il ne soit plus le seul
  selectionne').
  ----------------------------------------------------------------------*/
void ExcCutPage (PtrElement *pElFirstSel, PtrElement *pElLastSel,
                 PtrDocument pDoc, ThotBool *toBeSaved, ThotBool *deletePage)
{
  PtrElement          pElPrec, pElNext;
  ThotBool             stop;

  if (*pElFirstSel == *pElLastSel &&
      /* one selected element */
      (*pElFirstSel)->ElTerminal &&
      (*pElFirstSel)->ElLeafType == LtPageColBreak)
    /* and a pagebreak */
    {
      /* les precedents peuvent etre des elements repete's */
      pElPrec = (*pElFirstSel)->ElPrevious;
      stop = FALSE;
      while (!stop)
        {
          if (pElPrec == NULL)
            stop = TRUE;	/* pas d'autre element precedent */
          else if (!TypeHasException (ExcPageBreakRepBefore, pElPrec->ElTypeNumber,
                                      pElPrec->ElStructSchema))
            /* l'element precedent n'est pas une repetition */
            stop = TRUE;
          else if (pElPrec->ElSource == NULL)
            /* l'element precedent n'est pas une inclusion */
            stop = TRUE;
          else
            /* il faut supprimer cet element precedent */
            {
              *pElFirstSel = pElPrec;
              *toBeSaved = FALSE;
              *deletePage = TRUE;
              /* passe au precedent */
              pElPrec = pElPrec->ElPrevious;
            }
        }
      /* les suivants peuvent etre des elements repetes */
      pElNext = (*pElLastSel)->ElNext;
      stop = FALSE;
      while (!stop)
        {
          if (pElNext == NULL)
            stop = TRUE;	/* pas d'autre element suivant */
          else if (!TypeHasException (ExcPageBreakRepetition, pElNext->ElTypeNumber,
                                      pElNext->ElStructSchema))
            /* l'element suivant n'est pas une repetition */
            stop = TRUE;
          else if (pElNext->ElSource == NULL)
            /* l'element suivant n'est pas une inclusion */
            stop = TRUE;
          else
            /* il faut supprimer cet element suivant */
            {
              *pElLastSel = pElNext;
              *toBeSaved = FALSE;
              *deletePage = TRUE;
              pElNext = pElNext->ElNext;
            }
        }
    }
}

/*----------------------------------------------------------------------
  DeletePageInTable      Si l'element saut de page pointe' par
  pElPage est dans une structure a coupure speciale, supprime les
  elements repetes qui precedent et qui suivent.
  ----------------------------------------------------------------------*/
static void DeletePageInTable (PtrElement pElPage, PtrDocument pDoc)
{
  PtrElement          pElPrevious, pElPrevious1, pElNext, pElNext1;
  ThotBool             stop;

  /* supprime les elements repetes precedents */
  pElPrevious = pElPage->ElPrevious;
  stop = FALSE;
  while (!stop)
    {
      if (pElPrevious == NULL)
        stop = TRUE;		/* pas d'autre element precedent */
      else if (!TypeHasException (ExcPageBreakRepBefore, pElPrevious->ElTypeNumber,
                                  pElPrevious->ElStructSchema))
        /* l'element precedent n'est pas une repetition */
        stop = TRUE;
      else if (pElPrevious->ElSource == NULL)
        /* l'element precedent n'est pas une inclusion */
        stop = TRUE;
      else
        /* il faut supprimer cet element precedent */
        {
          pElPrevious1 = pElPrevious->ElPrevious;
          DeleteElement (&pElPrevious, pDoc);
          pElPrevious = pElPrevious1;
        }
    }
  /* supprime les elements repetes suivants */
  pElNext = pElPage->ElNext;
  stop = FALSE;
  while (!stop)
    {
      if (pElNext == NULL)
        stop = TRUE;		/* pas d'autre element suivant */
      else if (!TypeHasException (ExcPageBreakRepetition, pElNext->ElTypeNumber,
                                  pElNext->ElStructSchema))
        /* l'element suivant n'est pas une repetition */
        stop = TRUE;
      else if (pElNext->ElSource == NULL)
        /* l'element suivant n'est pas une inclusion */
        stop = TRUE;
      else
        /* il faut supprimer cet element suivant */
        {
          pElNext1 = pElNext->ElNext;
          DeleteElement (&pElNext, pDoc);
          pElNext = pElNext1;
        }
    }
}


/*----------------------------------------------------------------------
  DeletePageAbsBoxes  Si l'element saut de page pointe'
  par pElPage est dans une structure demandant une coupure speciale,
  detruit les paves des elements repetes qui precedent et ceux des
  elements repetes qui suivent.
  ----------------------------------------------------------------------*/
static void DeletePageAbsBoxes (PtrElement pElPage, PtrDocument pDoc, int viewNb)
{
  PtrElement          pElPrevious, pElNext;
  ThotBool             stop;

  /* detruit les paves des elements repetes qui precedent */
  pElPrevious = pElPage->ElPrevious;
  stop = FALSE;
  while (!stop)
    {
      if (pElPrevious == NULL)
        stop = TRUE;		/* pas d'autre element precedent */
      else if (!TypeHasException (ExcPageBreakRepBefore, pElPrevious->ElTypeNumber,
                                  pElPrevious->ElStructSchema))
        /* l'element precedent n'est pas une repetition */
        stop = TRUE;
      else if (pElPrevious->ElSource == NULL)
        /* l'element precedent n'est pas une inclusion */
        stop = TRUE;
      else
        /* c'est bien un element repete', on detruit ses paves */
        {
          DestroyAbsBoxesView (pElPrevious, pDoc, FALSE, viewNb);
          pElPrevious = pElPrevious->ElPrevious;
        }
    }
  /* detruit les paves des elements repetes qui suivent */
  pElNext = pElPage->ElNext;
  stop = FALSE;
  while (!stop)
    {
      if (pElNext == NULL)
        stop = TRUE;		/* pas d'autre element suivant */
      else if (!TypeHasException (ExcPageBreakRepetition, pElNext->ElTypeNumber,
                                  pElNext->ElStructSchema))
        /* l'element suivant n'est pas une repetition */
        stop = TRUE;
      else if (pElNext->ElSource == NULL)
        /* l'element suivant n'est pas une inclusion */
        stop = TRUE;
      else
        /* c'est bien un element repete', on detruit ses paves */
        {
          DestroyAbsBoxesView (pElNext, pDoc, FALSE, viewNb);
          pElNext = pElNext->ElNext;
        }
    }
}

/*----------------------------------------------------------------------
  SuppressPageMark supprime la marque de page pointee par pPage et	
  essaie de fusionner l'element precedent avec l'element	
  suivant.						
  Retourne dans pLib un pointeur sur l'element a libere	
  resultant de la fusion, si elle a pu se faire.		
  ----------------------------------------------------------------------*/
static void SuppressPageMark (PtrElement pPage, PtrDocument pDoc, PtrElement * pLib)
{
  PtrElement          pPrevious;
  NotifyElement       notifyEl;
  int                 NSiblings;

  *pLib = NULL;
  /* envoie l'evenement ElemDelete.Pre */
  notifyEl.event = TteElemDelete;
  notifyEl.document = (Document) IdentDocument (pDoc);
  notifyEl.element = (Element) (pPage);
  notifyEl.elementType.ElTypeNum = pPage->ElTypeNumber;
  notifyEl.elementType.ElSSchema = (SSchema) (pPage->ElStructSchema);
  notifyEl.position = TTE_STANDARD_DELETE_LAST_ITEM;
  notifyEl.info = 0; /* not sent by undo */
  if (!CallEventType ((NotifyEvent *) & notifyEl, TRUE))
    {
      /* traitement de la suppression des pages dans les structures avec */
      /* coupures speciales */
      DeletePageInTable (pPage, pDoc);
      pPrevious = pPage->ElPrevious;
      /* prepare l'evenement ElemDelete.Post */
      notifyEl.event = TteElemDelete;
      notifyEl.document = (Document) IdentDocument (pDoc);
      notifyEl.element = (Element) (pPage->ElParent);
      notifyEl.info = 0; /* not sent by undo */
      notifyEl.elementType.ElTypeNum = pPage->ElTypeNumber;
      notifyEl.elementType.ElSSchema = (SSchema) (pPage->ElStructSchema);
      NSiblings = 0;
      DeleteElement (&pPage, pDoc);
      *pLib = NULL;
      if (pPrevious != NULL)
        {
          /* il y avait un element avant la marque de page, on essaie de le */
          /* fusionner avec l'element qui le suit maintenant. */
          if (!IsIdenticalTextType (pPrevious, pDoc, pLib))
            *pLib = NULL;
          while (pPrevious != NULL)
            {
              NSiblings++;
              pPrevious = pPrevious->ElPrevious;
            }
        }
      notifyEl.position = NSiblings;
      CallEventType ((NotifyEvent *) & notifyEl, FALSE);
    }
}


/*----------------------------------------------------------------------
  DestroyPageMarks	detruit toutes les marques de page de la
  vue schView, sauf les marques placees par l'utilisateur	
  et celles de debut des elements portant une regle Page.	
  ----------------------------------------------------------------------*/
static void DestroyPageMarks (PtrDocument pDoc, PtrElement pRootEl, int schView)
{
  PtrElement          pEl, pElPage, pElLib;

  pEl = pRootEl;
  pElPage = NULL;
  /* pas encore de marque de page a supprimer */
  while (pEl != NULL)
    /* cherche la prochaine marque de page */
    {
      pEl = FwdSearchTypedElem (pEl, PageBreak + 1, NULL, NULL);
      if (pEl != NULL && pEl->ElViewPSchema == schView)
        /* on a trouve' une marque de page concernant la vue */
        if (pEl->ElPageType == PgComputed)
          /* c'est une marque de page calculee */
          {
            if (pElPage != NULL)
              /* il y a deja une marque de page a supprimer, on la supprime */
              {
                SuppressPageMark (pElPage, pDoc, &pElLib);
                if (pElLib != NULL)
                  DeleteElement (&pElLib, pDoc);
              }
            /* on supprimera cette marque de page au tour suivant */
            pElPage = pEl;
          }
    }
  if (pElPage != NULL)
    /* il reste une marque de page a supprimer, on la supprime */
    {
      SuppressPageMark (pElPage, pDoc, &pElLib);
      if (pElLib != NULL)
        DeleteElement (&pElLib, pDoc);
    }
}
#ifndef PAGINEETIMPRIME

/*----------------------------------------------------------------------
  DisplaySelectPages
  Apres la pagination sous l'editeur, il faut recreer l'image et retablir
  la selection.	
  Procedure utilisee dans la pagination sous l'editeur (version vide pour
  l'appel depuis la commande d'impression)			
  ----------------------------------------------------------------------*/
static void DisplaySelectPages (PtrDocument pDoc, PtrElement firstPage,
                                int view, ThotBool sel,
                                PtrElement firstSelection,
                                PtrElement lastSelection,
                                int FirstSelectedChar,
                                int LastSelectedChar)
{
  PtrElement          pRootEl;
  PtrAbstractBox      rootAbsBox;
  int                 v, schView, frame, h;
  ThotBool            complete;

  /* reconstruit l'image de la vue et l'affiche */
  /* si on n'est pas en batch */
  PageHeight = 0;
  PageFooterHeight = 0;

  /* cree l'image abstraite des vues concernees */
  pRootEl = pDoc->DocDocElement;
  schView = AppliedView (pDoc->DocDocElement, NULL, pDoc, view);
  for (v = 1; v <= MAX_VIEW_DOC; v++)
    if (pDoc->DocView[v - 1].DvPSchemaView == schView)
      {
        pDoc->DocViewFreeVolume[v - 1] = pDoc->DocViewVolume[v - 1];
        rootAbsBox = pDoc->DocViewRootAb[v - 1];
        frame = pDoc->DocViewFrame[v - 1];
        AbsBoxesCreate (pRootEl, pDoc, v, TRUE, TRUE, &complete);
        h = 0;
        (void) ChangeConcreteImage (frame, &h, rootAbsBox);
        if (!sel)
          DisplayFrame (frame);
      }

  /* retablit la selection si elle ete supprimee avant le formatage */
  if (sel)
    SelectRange (pDoc, firstSelection, lastSelection, FirstSelectedChar,
                 LastSelectedChar);

  /* met a jour les numeros qui changent dans les autres vues a cause */
  /* de la creation des nouvelles marques de page */
  if (firstPage != NULL)
    UpdateNumbers (NextElement (firstPage), firstPage, pDoc, TRUE);
}
#endif /* PAGINEETIMPRIME */


/*----------------------------------------------------------------------
  Cut coupe l'element de texte pointe par pEl apres le		
  caractere de rang cutChar et met a jour les paves	
  correspondant.						
  ----------------------------------------------------------------------*/
static void Cut (PtrElement pEl, int cutChar, PtrDocument pDoc, int nbView)
{
  PtrElement	       pSecond;

  SplitTextElement (pEl, cutChar + 1, pDoc, TRUE, &pSecond, FALSE);
  /* reduit le volume du pave de l'element precedant le point de */
  /* coupure et de ses paves englobants, si ces paves existent dans la */
  /* vue traitee. */
  UpdateAbsBoxVolume (pEl, nbView - 1, pDoc);
  /* prepare la creation des paves de la 2eme partie */
  if (pDoc->DocView[nbView - 1].DvPSchemaView > 0)
    pDoc->DocViewFreeVolume[nbView - 1] = THOT_MAXINT;
  /* cree les paves de la deuxieme partie */
  CreateNewAbsBoxes (pSecond, pDoc, nbView);
  ApplDelayedRule (pSecond, pDoc);
}

/*----------------------------------------------------------------------
  AllowBreak returns TRUE when the abstract box can be broken.
  When TRUE, pR1 returns the break-top rule (NoBreak1) and pR2 returns
  the break-bottom rule (NoBreak2).
  If there is a rule NoBreak1 (pR1 != NULL) and it's given by an attribute
  pAt1 points to the attribute.
  If there is a rule NoBreak2 (pR2 != NULL) and it's given by an attribute
  pAt2 points to the attribute.
  ----------------------------------------------------------------------*/
static ThotBool AllowBreak (PtrAbstractBox pAb, PtrPRule *pR1,
                            PtrAttribute *pAt1, PtrPRule *pR2,
                            PtrAttribute *pAt2, int schView,
                            PtrDocument pDoc)
{
  PtrPSchema          pSchP;
  PtrSSchema          pSchS;
  int                 entry;
  ThotBool            ret;

  *pR1 = NULL;
  *pR2 = NULL;
  if (!pAb->AbAcceptPageBreak)
    /* don't allow a break */
    ret = FALSE;
  else  if (pAb->AbPresentationBox)
    /* a presentation box doesn't accept a break */
    ret = TRUE;
  else
    {
      /* look for the presentation schema */
      SearchPresSchema (pAb->AbElement, &pSchP, &entry, &pSchS, pDoc);
      ret = TRUE;
      /* look for the rule NoBreak1 */
      *pR1 = GlobalSearchRulepEl (pAb->AbElement, pDoc, &pSchP, &pSchS, FALSE,
                                  0, NULL, schView, PtBreak1, FnAny,
                                  FALSE, TRUE, pAt1);
      /* look for the rule NoBreak2 */
      *pR2 = GlobalSearchRulepEl (pAb->AbElement, pDoc, &pSchP, &pSchS, FALSE,
                                  0, NULL, schView, PtBreak2, FnAny,
                                  FALSE, TRUE, pAt2);
    }
  return ret;
}


/*----------------------------------------------------------------------
  PageBrk Page	indique si l'element pEl debute par un saut de
  page de la vue schView
  ----------------------------------------------------------------------*/
static PtrElement   PageBrk (PtrElement pEl, int schView)
{
  PtrElement          pE;
  ThotBool            found;

  if (pEl->ElTerminal)
    return NULL;
  else if (pEl->ElFirstChild == NULL)
    return NULL;
  else if (pEl->ElFirstChild->ElTypeNumber == PageBreak + 1)
    {
      /* on cherche dans les premiers fils une marque de la vue */
      found = FALSE;
      pE = pEl->ElFirstChild;
      while (!found && pE != NULL &&
             pE->ElTypeNumber == PageBreak + 1)
        if (pE->ElViewPSchema == schView)
          found = TRUE;
        else
          pE = pE->ElNext;
      if (found)
        return (pE);
      else
        return (NULL);
    }
  else
    return (PageBrk (pEl->ElFirstChild, schView));
}

/*----------------------------------------------------------------------
  FunctionRule retourne le pointeur sur la premiere regle        
  de fonction de presentation associee a l'element pEl dans le
  schema de presentation pSchP.                
  Retourne NULL s'il n'y a pas de regle de creation pour  
  cet element                                             
  ----------------------------------------------------------------------*/
static PtrPRule FunctionRule (PtrElement pEl, int index, PtrPSchema pSchP,
                              PtrDocument pDoc)
{
  PtrPRule            pRule;

  pRule = NULL;
  if (pSchP != NULL)
    {
      /* pRule : premiere regle de presentation specifique a ce type */
      /* d'element */
      pRule = (pSchP)->PsElemPRule->ElemPres[index - 1];
      if (pRule != NULL)
        {
          while (pRule->PrType < PtFunction && pRule->PrNextPRule != NULL)
            pRule = pRule->PrNextPRule;
          if (pRule->PrType != PtFunction)
            pRule = NULL;
        }
    }
  return pRule;
}

/*----------------------------------------------------------------------
  InsertMark insere une Marque de Page avant l'element auquel correspond
  le pave pointe' par pAb.
  Si la Marque doit etre placee dans un element mis en lignes, en premiere	
  position, elle est placee avant cet element. 		
  Retourne un pointeur sur l'element Marque de Page insere'.
  On detruit la partie de l'i.a. qui suit cette marque  et on reconstruit
  l'i.a. (donc le pave pAb change ! )
  position prend 3 valeurs :
  0 : insere une Marque de Page avant l'element.
  1 : insere apres l'element.
  2 : insere comme premier fils de l'element.
  ----------------------------------------------------------------------*/
static PtrElement InsertMark (PtrAbstractBox pAb, int frame, int nbView,
                              PtrAbstractBox *origCutAbsBox, ThotBool *needBreak,
                              int schView, PtrDocument pDoc, PtrElement rootEl,
                              int position)
{
  PtrElement          pElPage, pEl;
  PtrPRule            pRule;
  PtrAbstractBox      modifAbsBox, topPageAbsBox, savePageAbsBox;
  PtrAbstractBox      pP1, pP;
  PtrPSchema          pSchP;
  PtrSSchema          pSchS;
  AbPosition         *pPos;
#ifndef PAGINEETIMPRIME
  PtrElement          pF;
  NotifyElement       notifyEl;
  int                 NSiblings;
#endif
  int                 cpt, h, val, index;
  ThotBool            stop, inTop;
  ThotBool            ElemIsChild, ElemIsBefore, cut;

  pElPage = NULL;
  pP = pAb;
  pP1 = pP->AbEnclosing;
  pEl = pP->AbElement;
  ElemIsChild = FALSE;
  if (position == 0)
    {
      /* teste si le pave est le premier d'un pave mis en lignes */
      if (pP1 != NULL && pP->AbPrevious == NULL)
        {
          if (pP1->AbInLine)
            /* on inserera la marque de page avant le pave englobant */
            pP = pP1;
        }

      /* teste si le pave est en haut de son englobant */
      do
        {
          stop = TRUE;
          inTop = FALSE;
          if (pP->AbEnclosing != NULL)
            {
              pPos = &pP->AbVertPos;
              if (pPos->PosAbRef == NULL)
                {
                  /* postion verticale par defaut = en haut de l'englobant */
                  inTop = TRUE;
                  /* si l'englobant est mis en ligne et que le pave n'est pas */
                  /* le premier des paves mis en ligne, il n'est pas en haut */
                  /* de son englobant */
                  if (pP->AbEnclosing->AbInLine && pP->AbPrevious != NULL)
                    inTop = FALSE;
                }
              else if (pPos->PosAbRef == pP->AbEnclosing &&
                       pPos->PosDistance == 0 && pPos->PosEdge == Top &&
                       pPos->PosRefEdge == Top)
                inTop = TRUE;
            }
          if (inTop)
            /* le pave est en haut de son englobant, on place la */
            /* marque de page avant l'englobant */
            {
              pP = pP->AbEnclosing;
              stop = FALSE;
            }
        }
      while (!stop);

      pEl = pP->AbElement;
      /* on inserera la nouvelle marque de page avant pEl */
      ElemIsBefore = TRUE;

      /*on regarde s'il n'y a pas deja une marque de page juste avant pEl */
      if (pEl->ElPrevious != NULL)
        {
          /* On ignore les elements repetes en haut de page */
          /* (tetieres de tableaux par exemple)             */
          stop = FALSE;
          do
            if (pEl->ElPrevious == NULL)
              stop = TRUE;
            else if (pEl->ElPrevious->ElIsCopy &&
                     TypeHasException (ExcPageBreakRepetition, pEl->ElPrevious->ElTypeNumber, pEl->ElPrevious->ElStructSchema))
              pEl = pEl->ElPrevious;
            else
              stop = TRUE;
          while (!stop);
        }

      pEl = PreviousLeaf (pEl);
      if (pEl != NULL && pEl->ElTerminal && pEl->ElLeafType == LtPageColBreak &&
          pEl->ElViewPSchema == schView)
        /* il y a deja devant l'element pEl une marque de page
           pour cette vue */
        /* on mettra la nouvelle marque apres l'element pEl */
        {
          ElemIsBefore = FALSE;
          /* il y a un pave plus haut que la page avant la */
          /* nouvelle marque de page que l'on va inserer */
          *needBreak = TRUE;
        }

      /* si le pave pP est un pave de presentation de l'element place' */
      /* apres ou comme dernier fils, on insere l'element marque de page */
      /* apres cet element. Rq: la page sera trop longue ! */
      if (pP->AbPresentationBox)
        {
          SearchPresSchema (pEl, &pSchP, &index, &pSchS, pDoc);
          pRule = FunctionRule (pEl, index, pSchP, pDoc);
          while (pRule != NULL && ElemIsBefore == TRUE)
            {
              if (pP->AbTypeNum == pRule->PrPresBox[0]
                  && pP->AbPSchema == pSchP)
                /* c'est la regle correspondant a ce pave */
                if (pRule->PrPresFunction == FnCreateAfter
                    || pRule->PrPresFunction == FnCreateLast)
                  /* on insere la marque apres l'element */
                  {
                    *origCutAbsBox = NULL;
                    ElemIsBefore = FALSE;
                  }
              pRule = pRule->PrNextPRule;
              if (pRule != NULL && pRule->PrType > PtFunction)
                pRule = NULL;
            }
        }
    }
  else if (position == 1 || pP->AbElement->ElTerminal)
    /* insert after */
    ElemIsBefore = FALSE;
  else
    {
      /* insert as first child */
      ElemIsBefore = FALSE;
      ElemIsChild = TRUE;
    }

  pEl = pP->AbElement;
  if (!ElemIsBefore && position == 0 && *origCutAbsBox != NULL)
    /* Il y a un pave insecable plus haut qu'une page, on inserera */
    /* la marque de page apres l'element de ce pave */
    pEl = (*origCutAbsBox)->AbElement;

  if (pEl->ElParent == NULL)
    {
      /* si pEl est la racine il faut descendre d'un niveau */
      if (ElemIsBefore)
        pEl = pEl->ElFirstChild;
      else if (!ElemIsChild)
        {
          pEl = pEl->ElFirstChild;
          while (pEl->ElNext != NULL)
            pEl = pEl->ElNext;
        }
    }

  if (ElemIsBefore)
    {
      if (pEl->ElTypeNumber == PageBreak + 1)
        return pEl;
      pElPage = PageBrk (pEl, schView);
      if (pElPage != NULL)
        /* on veut inserer un saut de page devant un element qui a la regle */
        /* Page. On n'insere pas de nouveau saut de page et on retourne le */
        /* saut de page de l'element suivant */
        return pElPage;
    }

#ifndef PAGINEETIMPRIME
  /* envoie l'evenement ElemNew.Pre */
  notifyEl.event = TteElemNew;
  notifyEl.document = (Document) IdentDocument (pDoc);
  notifyEl.info = 0; /* not sent by undo */
  NSiblings = 0;
  if (ElemIsChild)
    notifyEl.element = (Element) (pP->AbElement);
  else
    {
      notifyEl.element = (Element) (pP->AbElement->ElParent);
      pF = pEl;
      while (pF->ElPrevious != NULL)
        {
          NSiblings++;
          pF = pF->ElPrevious;
        }
      if (!ElemIsBefore)
        NSiblings++;
    }
  notifyEl.position = NSiblings;
  notifyEl.elementType.ElTypeNum = PageBreak + 1;
  notifyEl.elementType.ElSSchema = (SSchema) (rootEl->ElStructSchema);
  CallEventType ((NotifyEvent *) & notifyEl, TRUE);
#endif /* PAGINEETIMPRIME */
  /* cree l'element Marque de Page */
  pElPage = NewSubtree (PageBreak + 1, rootEl->ElStructSchema,
                        pDoc, TRUE, TRUE, TRUE, TRUE);
  /* insere l'element dans l'arbre abstrait */
  if (ElemIsChild)
    InsertFirstChild (pEl, pElPage);
  else if (pEl->ElParent != NULL)
    {
      if (ElemIsBefore)
        InsertElementBefore (pEl, pElPage);
      else
        InsertElementAfter (pEl, pElPage);
    }

  /* remplit l'element page cree' */
  pElPage->ElPageType = PgComputed;
  pElPage->ElViewPSchema = schView;
  /* cherche le compteur de page a appliquer */
  cpt = GetPageCounter (pElPage, pDoc, schView, &pSchP);
  if (cpt == 0)		/* page non numerotee */
    /* on entretient un compteur de pages pour pouvoir afficher un */
    /* message indiquant la progression du formatage */
    {
      pagesCounter++;
      pElPage->ElPageNumber = pagesCounter;
    }
  else				/* calcule le numero de page */
    pElPage->ElPageNumber = CounterVal (cpt, pElPage->ElStructSchema, pSchP,
                                        pElPage, schView, pDoc);
  /* envoie l'evenement ElemNew.Post */
#ifndef PAGINEETIMPRIME
  NotifySubTree (TteElemNew, pDoc, pElPage, 0, 0, FALSE, FALSE);
#endif /* PAGINEETIMPRIME */

  if (NbBoxesPageHeaderToCreate > 0)
    /* cherche d'abord la boite du filet de separation de pages: */
    /* c'est la premiere boite contenue dans la boite de saut de page */
    /* qui n'est pas une boite de presentation. */
    {
      if (WorkingPage->ElAbstractBox[nbView - 1] != NULL)
        {
          pP1 = WorkingPage->ElAbstractBox[nbView - 1]->AbFirstEnclosed;
          stop = FALSE;
          do
            if (pP1 == NULL)
              stop = TRUE;
            else if (!pP1->AbPresentationBox)
              stop = TRUE;
            else
              pP1 = pP1->AbNext;
          while (!(stop));
          savePageAbsBox = WorkingPage->ElAbstractBox[nbView - 1];
          WorkingPage->ElAbstractBox[nbView - 1] = pP1;
          topPageAbsBox = CrAbsBoxesPres (WorkingPage, pDoc, PageCreateRule,
                                          WorkingPage->ElStructSchema, NULL, nbView,
                                          PageSchPresRule, NULL, TRUE);
          WorkingPage->ElAbstractBox[nbView - 1] = savePageAbsBox;
          if (topPageAbsBox != NULL)
            /* signale ces paves au Mediateur, sans faire reevaluer
               la coupure de page. */
            {
              h = 0;
              (void) ChangeConcreteImage (frame, &h, topPageAbsBox);
            }
        }
      NbBoxesPageHeaderToCreate = 0;
    }
  /* cree les paves de l'element Marque de Page qu'on vient d'inserer */
  /* traitement de l'insertion des pages dans les structures avec coupures
     speciales */
  cut = FALSE;		/* a priori pas de coupure effectuee par l'exception */
  InsertPageInTable (pElPage, pDoc, nbView, &cut);
  if (!cut)
    CreateNewAbsBoxes (pElPage, pDoc, nbView);
  modifAbsBox = pDoc->DocViewModifiedAb[nbView - 1];
  /* signale ces paves au Mediateur, sans faire reevaluer la coupure de page. */
  if (modifAbsBox != NULL)
    {
      h = 0;
      pP1 = pElPage->ElAbstractBox[nbView - 1];
      if (position == 1)
        {
          pPos = &pP1->AbVertPos;
          pPos->PosAbRef = pAb;
          val = RealPageHeight - pAb->AbBox->BxYOrg - pAb->AbBox->BxHeight;
#ifdef _WIN_PRINT
          pPos->PosDistance = (val * ScreenDPI + PrinterDPI/ 2) / PrinterDPI;
#else /* _WIN_PRINT */
          pPos->PosDistance = val + (val * ViewFrameTable[frame - 1].FrMagnification / 10);
#endif /* _WIN_PRINT */
          pPos->PosEdge = Top;
          pPos->PosRefEdge = Bottom;
          pPos->PosUnit = UnPoint;
          pPos->PosUserSpecified = FALSE;
          pP1->AbVertPosChange = TRUE;
        }
      else if (position == 2)
        {
          pPos = &pP1->AbVertPos;
          pPos->PosAbRef = pAb;
          val = RealPageHeight - pAb->AbBox->BxYOrg;
#ifdef _WIN_PRINT
          pPos->PosDistance = (val * ScreenDPI + PrinterDPI/ 2) / PrinterDPI;
#else /* _WIN_PRINT */
          pPos->PosDistance = val + (val * ViewFrameTable[frame - 1].FrMagnification / 10);
#endif /* _WIN_PRINT */
          pPos->PosEdge = Top;
          pPos->PosRefEdge = Top;
          pPos->PosUnit = UnPoint;
          pPos->PosUserSpecified = FALSE;
          pP1->AbVertPosChange = TRUE;
        }
      (void) ChangeConcreteImage (frame, &h, modifAbsBox);
      pP1->AbOnPageBreak = FALSE;
      pP1->AbAfterPageBreak = FALSE;
      /* les paves devant lesquels on a mis la marque de page ne doivent */
      /* plus etre traites. On les marque hors page. */
      if (ElemIsBefore)
        {
          stop = FALSE;
          do
            {
              pAb->AbOnPageBreak = FALSE;
              pAb->AbAfterPageBreak = TRUE;
              if (pAb == pP)
                stop = TRUE;
              else
                pAb = pAb->AbEnclosing;
            }
          while (!stop);
        }
      else if (!ElemIsChild)
        {
          pAb->AbOnPageBreak = FALSE;
          pAb->AbAfterPageBreak = FALSE;
        }
    }
  return pElPage;
}


/*----------------------------------------------------------------------
  MoveCut cherche dans les paves du sous-arbre de racine pAb, en
  appliquant les regles de condition de coupure NoBreak1 (ou NoBreak2,
  selon NoBr1), le premier pave qui necessite un deplacement (vers le haut)
  de la frontiere de page. Retourne la nouvelle frontiere de page, en
  points typographiques, ou 0 si la coupure de page convient.
  ----------------------------------------------------------------------*/
static int MoveCut (PtrDocument pDoc, PtrAbstractBox pAb, ThotBool NoBr1,
                    int schView)
{
  int                 ret, h, org, cutChar, min, i;
  PtrPRule            pRNoBr1, pRNoBr2;
  PtrAttribute        pA1, pA2;
  ThotBool            cutAbsBox;
  PtrPRule            pRe1;

  ret = 0;
  /* cherche si la coupure de page convient au pave */
  if (pAb->AbOnPageBreak)
    {
      if (!AllowBreak (pAb, &pRNoBr1, &pA1, &pRNoBr2, &pA2, schView, pDoc))
        /* no break accepted within the element, break before */
        {
          SetPageHeight (pAb, &h, &org, &cutChar);
          ret = org;
        }
      else if (NoBr1)
        /* break allowed check the rule NoBreak1 */
        {
          if (pRNoBr1)
            {
              if (pAb->AbLeafType != LtCompound)
                cutAbsBox = FALSE;
              else if (pAb->AbBox &&
                       (pAb->AbBox->BxType == BoBlock ||
                        pAb->AbBox->BxType == BoFloatBlock))
                cutAbsBox = FALSE;
              else
                /* don't check if the element is truncated */
                cutAbsBox = pAb->AbTruncatedHead;
              if (pAb->AbBox &&
                  (pAb->AbBox->BxType == BoGhost ||
                   pAb->AbBox->BxType == BoStructGhost ||
                   pAb->AbBox->BxType == BoFloatGhost))
                /* ignore the rule NoBreak1 */
                cutAbsBox = TRUE;
              if (!cutAbsBox)
                {
                  pRe1 = pRNoBr1;
                  /* get the position of the top of the page */
                  SetPageHeight (pAb, &h, &org, &cutChar);
                  /* get the page height (in pt) */
                  if (pRe1->PrMinAttr)
                    i = AttrValue (pA1);
                  else
                    i = pRe1->PrMinValue;
                  min = PixelValue (i, pRe1->PrMinUnit, pAb, 0);
                  if (min < h && RealPageHeight - org < min)
                    /* the rule NoBreak1 is not respected, break before */
                    ret = org;
                }
            }
        }
      else if (pRNoBr2)
        /* break allowed check the rule NoBreak2 */
        {
          if (pAb->AbLeafType != LtCompound)
            cutAbsBox = FALSE;
          else if (pAb->AbBox &&
                   (pAb->AbBox->BxType == BoBlock ||
                    pAb->AbBox->BxType == BoFloatBlock))
            cutAbsBox = FALSE;
          else
            /* don't check if the element is truncated */
            cutAbsBox = pAb->AbTruncatedHead;
          if (pAb->AbBox &&
              (pAb->AbBox->BxType == BoGhost ||
               pAb->AbBox->BxType == BoStructGhost ||
               pAb->AbBox->BxType == BoFloatGhost))
            /* ignore the rule NoBreak1 */
            cutAbsBox = TRUE;
          if (!cutAbsBox)
            {
              pRe1 = pRNoBr2;
              /* get the position of the top of the page */
              SetPageHeight (pAb, &h, &org, &cutChar);
              /* get the page height (in pt) */
              if (pRe1->PrMinAttr)
                i = AttrValue (pA2);
              else
                i = pRe1->PrMinValue;
              min = PixelValue (i, pRe1->PrMinUnit, pAb, 0);
              if (min < h && org + h - RealPageHeight < min)
                /* the rule NoBreak1 is not respected, reduce the page height */
                ret = org + h - min;
            }
        }

      if (ret == 0)
        /* check included abstract boxes */
        {
          pAb = pAb->AbFirstEnclosed;
          while (ret == 0 && pAb != NULL)
            {
              ret = MoveCut (pDoc, pAb, NoBr1, schView);
              pAb = pAb->AbNext;
            }
        }
    }
  return ret;
}

/*----------------------------------------------------------------------
  SetMark place dans l'arbre de racine pAb la marque de page en fonction
  de la position des paves relativement a la limite de page.
  The parameter notfound is TRUE when the specific cut postion is not
  already found.
  ----------------------------------------------------------------------*/
static void SetMark (PtrAbstractBox pAb, PtrElement rootEl, PtrDocument pDoc,
                     int schView, ThotBool *needBreak,
                     PtrAbstractBox *origCutAbsBox, int nbView, int frame,
                     PtrElement *pPage, ThotBool notfound)
{
  PtrElement          pLast;
  PtrAbstractBox      pChild;
  ThotBool            toCut;
  PtrAbstractBox      pCreator;
  int                 h, org, cutChar;
  ThotBool            done;

  if (!pAb)
    return;
  if (pAb->AbOnPageBreak)
    {
      /* le pave' est traverse' par la limite de page */
      if (pAb->AbFirstEnclosed == NULL)
        /* c'est un pave' feuille */
        {
          /* a priori on va le couper en deux */
          toCut = TRUE;
          if (pAb->AbPresentationBox)
            /* c'est un pave de presentation */
            {
              /* on cherche le pave qui l'a cree' pour connaitre sa regle */
              /* de creation */
              pCreator = pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1];
              while (pCreator->AbPresentationBox)
                pCreator = pCreator->AbNext;
              /* on cherche la regle de creation */
              if (notfound)
                {
                  if (TypeCreatedRule (pDoc, pCreator, pAb) == FnCreateWith)
                    /* c'est une regle CreateWith, on ne fait rien */
                    toCut = FALSE;
                  else if (pAb->AbLeafType == LtGraphics &&
                           pCreator->AbFirstEnclosed != NULL)
                    /* c'est un graphique de presentation d'un pave compose */
                    toCut = FALSE;
                }
              else if (!pCreator->AbOnPageBreak && pCreator->AbBox &&
                       pCreator->AbBox->BxType == BoCell)
                {
                  /* generate a break at the end of that cell */
                  pLast = pCreator->AbElement;
                  while (pLast && pLast->ElNext)
                    pLast = pLast->ElNext;
                  if (pLast)
                    {
                      toCut = FALSE;
                      /* insert a new page break after that last element */
                      pAb = pLast->ElAbstractBox[pAb->AbDocView - 1];
                      *pPage = InsertMark (pAb, frame, nbView, origCutAbsBox,
                                           needBreak, schView, pDoc, rootEl, 1);
                    }
		    
                }
            }
          if (toCut)
            {
              /* demande au mediateur sur quel caractere a lieu la coupure */
              /* (si ce n'est pas une feuille de texte, on placera la marque */
              /* de page avant le pave) */
              SetPageHeight (pAb, &h, &org, &cutChar);
              if (cutChar <= 0)
                /* place la marque de page avant le pave */
                *pPage = InsertMark (pAb, frame, nbView, origCutAbsBox,
                                     needBreak, schView, pDoc, rootEl, 0);
              else if (cutChar >= pAb->AbElement->ElTextLength)
                {
                  /* la coupure tombe a la fin du pave */
                  pAb->AbOnPageBreak = FALSE;
                  pAb = pAb->AbNext;
                  if (pAb)
                    {
                      pAb->AbAfterPageBreak = TRUE;
                      *pPage = InsertMark (pAb, frame, nbView,
                                           origCutAbsBox, needBreak,
                                           schView, pDoc, rootEl, 0);
                    }
                }
              else
                /* coupe l'element de texte */
                {
                  Cut (pAb->AbElement, cutChar, pDoc, nbView);
                  pAb->AbOnPageBreak = FALSE;
                  pAb = pAb->AbNext;
                  pAb->AbAfterPageBreak = TRUE;
                  *pPage = InsertMark (pAb, frame, nbView,
                                       origCutAbsBox, needBreak,
                                       schView, pDoc, rootEl, 0);
                }
            }
        }
      else
        /* ce n'est pas un pave feuille, on examine tous les paves */
        /* englobes par ce pave' */
        {
          pAb = pAb->AbFirstEnclosed;
          done = FALSE;
          while (pAb && !done)
            {
              if (pAb->AbOnPageBreak)
                /* la frontiere de page traverse ce pave, on place une */
                /* marque de page a l'interieur */
                {
                  SetMark (pAb, rootEl, pDoc, schView, needBreak,
                           origCutAbsBox, nbView, frame, pPage, notfound);
                  done = (*pPage != NULL);
                  if (TypeHasException (ExcIsCell, pAb->AbElement->ElTypeNumber,
                                        pAb->AbElement->ElStructSchema) &&
                      !done)
                    {
                      /* the cell has to be split but the page break was not given */
                      /* look for the abstract box of the cell and skip
                         presentation boxes */
                      pCreator = pAb->AbElement->ElAbstractBox[pAb->AbDocView - 1];
                      /*WinErrorBox (NULL, "Printing");*/
                      while (pCreator->AbPresentationBox)
                        pCreator = pCreator->AbNext;
                      /* it's the cell abstract box */
                      pAb = pCreator;
                      /* insert the page break within the cell */
                      pChild = pCreator->AbFirstEnclosed;
                      /* locate the last child */
                      while (pChild)
                        {
                          while (pChild->AbNext && !pChild->AbNext->AbAfterPageBreak)
                            pChild = pChild->AbNext;
                          /* skip bacwards presentation boxes of the cell */
                          while (pChild && pChild->AbPresentationBox)
                            pChild = pChild->AbPrevious;
                          if (pChild)
                            {
                              pCreator = pChild;
                              if (pChild->AbLeafType == LtCompound)
                                pChild = pCreator->AbFirstEnclosed;
                              else
                                pChild = NULL;
                            }
                        }
                      /* see if the the break can be inserted after the parent */
                      /*while (!pCreator->AbNext && pCreator->AbEnclosing &&
                        pCreator->AbEnclosing != pAb)
                        pCreator = pCreator->AbEnclosing;*/
                      *pPage = InsertMark (pCreator, frame, nbView, origCutAbsBox,
                                           needBreak, schView, pDoc, rootEl, 1);
                      pCreator->AbOnPageBreak = TRUE;
                      done = (*pPage != NULL);
                    }
                }
              else if (pAb->AbAfterPageBreak && !done)
                /* c'est le premier pave englobe' au-dela de la */
                /* frontiere, on pose une marque de page devant lui */
                {
                  *pPage = InsertMark (pAb, frame, nbView, origCutAbsBox,
                                       needBreak, schView, pDoc, rootEl, 0);
                  done = TRUE;
                  pAb = NULL;
                }
              if (pAb)
                pAb = pAb->AbNext;
            }
        }
    }
  else
    /* insert the page break here */
    *pPage = InsertMark (pAb, frame, nbView, origCutAbsBox,
                         needBreak, schView, pDoc, rootEl, 0);
}


/*----------------------------------------------------------------------
  SetPage place la marque de page en respectant la hauteur de page
  demandee et les conditions de coupure des paves de la page.
  ----------------------------------------------------------------------*/
static void SetPage (PtrElement *pPage, int frame, PtrAbstractBox *origCutAbsBox,
                     ThotBool *needBreak, PtrDocument pDoc, int schView,
                     int nbView, PtrElement rootEl)
{
  int                 turn, newheight, oldheight;
  ThotBool            noBr1, moved;
#ifdef PRINT_DEBUG1
  FILE               *list;
  char                localname[50];
  static int          n = 1;

  sprintf (localname, "/tmp/printpage%d.debug", n);
  n++;
  list = TtaWriteOpen (localname);
  TtaListBoxes (1, 1, list);
  TtaWriteClose (list);
#endif
  /* explore deux fois l'arbre des paves a la recherche des paves */
  /* traverses par la frontiere de page et decale la frontiere de page */
  /* vers le haut pour respecter les regles NoBreak1 et NoBreak2 des */
  /* paves traverses par la frontiere de page. Le premier tour traite */
  /* les regles NoBreak2 et le 2eme tour les regles NoBreak1. */
  moved = FALSE;
  for (turn = 1; turn <= 2; turn++)
    {
      noBr1 = turn == 2;
      /* on traite les regles NoBreak1 au 2eme tour */
      do
        /* on commence par la racine de la vue */
        {
          newheight = MoveCut (pDoc, rootEl->ElAbstractBox[nbView - 1], noBr1,
                               schView);
          if (newheight)
            {
              /* a new position of the page break is requested */
              moved = TRUE;
              oldheight = RealPageHeight;
              RealPageHeight = newheight;
              SetPageBreakPosition (rootEl->ElAbstractBox[nbView - 1], &RealPageHeight);
              /* check if the new position is accepted */
              if (RealPageHeight == oldheight || RealPageHeight == newheight)
                /* the returned value is a previous one, stop the loop */
                newheight = 0;
            }
        }
      while (newheight);
    }
  /* place la marque de page dans l'arbre abstrait */
  if (moved)
    /* look for the right new position */
    SetMark (rootEl->ElAbstractBox[nbView - 1], rootEl, pDoc, schView,
             needBreak, origCutAbsBox, nbView, frame, pPage, moved);
  else
    SetMark (*origCutAbsBox, rootEl, pDoc, schView,
             needBreak, origCutAbsBox, nbView, frame, pPage, moved);
}


/*----------------------------------------------------------------------
  PutMark pour la vue de numero nbView, dans le document pDoc,	
  insere dans l'arbre abstrait de racine rootEl un	
  element Marque de Page a la frontiere de page et detruit
  tous les paves qui precedent cet element.		
  Retourne 'vrai' si l'image restante est plus petite qu'une page.
  Met a jour les pointeurs pT:pPageTraitee et pAT:pPageATraiter qui sont
  deux parametres en plus
  Retourne l'element marque page creee.
  ----------------------------------------------------------------------*/
static PtrElement  PutMark (PtrElement rootEl, int nbView, PtrDocument pDoc,
                            int frame, int schView)
{
  PtrAbstractBox      pAb;
  PtrAbstractBox      origCutAbsBox, possibleCut;
  PtrElement          pPage;
  PtrElement          pElLib;
  PtrAbstractBox      previousAbsBox, redispAb;
  int                 putVThread, cutChar, h, dh, normalPageHeight;
  ThotBool            stop;
  ThotBool            needBreak;
#ifdef PRINT_DEBUG1
  FILE               *list;
  char                localname[50];
  static int          n = 1;
   
  sprintf (localname, "/tmp/print%d.debug", n);
  n++;
  list = TtaWriteOpen (localname);
  TtaListBoxes (1, 1, list);
  TtaWriteClose (list);
#endif
  pPage = NULL;
  needBreak = FALSE;
  /* on recherche le pave de plus haut niveau qui soit insecable et */
  /* traverse' par la frontiere normale de page. On aura besoin de */
  /* connaitre ce pave' s'il est plus haut qu'une page et qu'on doit */
  /* inserer une marque de page apres lui. */
  origCutAbsBox = NULL;	/* on n'a pas encore trouve' ce pave' */
  possibleCut = NULL;
  pAb = rootEl->ElAbstractBox[nbView - 1];	/* on commence par le pave racine */
  do
    if (pAb->AbAfterPageBreak)
      /* the page break should appear before that abstract box */
      origCutAbsBox = pAb;
    else if (pAb->AbOnPageBreak)
      {
        if (!pAb->AbAcceptPageBreak)
          /* the abstract box cannot be cut */
          origCutAbsBox = pAb;
        else if (pAb->AbFirstEnclosed == NULL)
          {
            /* leaf box */
            if (TypeHasException (ExcIsCell,
                                  pAb->AbElement->ElTypeNumber,
                                  pAb->AbElement->ElStructSchema) &&
                pAb->AbNext)
              {
                /* we should cut that cell */
                if (!possibleCut)
                  /* if any other cut box found, we'll use that */
                  possibleCut = pAb;
                pAb = pAb->AbNext;
              }
            else
              /* insert the page-break before that abstract box */
              origCutAbsBox = pAb;
          }
        else
          pAb = pAb->AbFirstEnclosed;
      }
    else if (pAb->AbNext)
      /* see the next abstract box */
      pAb = pAb->AbNext;
    else if (pAb->AbEnclosing)
      /* see the next abstract box */
      pAb = pAb->AbEnclosing->AbNext;
  while (origCutAbsBox == NULL && pAb);

  if (origCutAbsBox == NULL && possibleCut)
    origCutAbsBox = possibleCut;
  /* place les marques de page sans tenir compte des boites de haut et de */
  /* bas de page de hauteur variable (notes de bas de page par exemple) */
  SetPage (&pPage, frame, &origCutAbsBox, &needBreak, pDoc, schView, nbView,
           rootEl);
  if (pPage)
    {
      /* on a insere' au moins une marque de page dans l'arbre abstrait */
      /* On verifie que la page n'est pas trop haute (il peut y avoir des */
      /* boites de haut ou de bas de page qui augmentent la hauteur de page).*/
      /* Inutile d'essayer de reduire la hauteur de la page s'il y a un */
      /* pave insecable plus haut que la page */
      if (!needBreak)
        /* cherche d'abord la boite du filet de separation de pages: c'est */
        /* la premiere boite contenue dans la boite de page qui n'est */
        /* pas une boite de presentation. */
        {
          if (pPage->ElAbstractBox[nbView - 1] != NULL)
            pAb = pPage->ElAbstractBox[nbView - 1]->AbFirstEnclosed;
          else
            pAb = NULL;
          stop = FALSE;
          do
            if (pAb == NULL)
              stop = TRUE;
            else if (!pAb->AbPresentationBox)
              stop = TRUE;
            else
              pAb = pAb->AbNext;
          while (!stop);
	  
          if (pAb == NULL)
            putVThread = 0;
          else
            /* get the position of the page break */
            SetPageHeight (pAb, &h, &putVThread, &cutChar);
          /* verifie la hauteur de la page */
          if (putVThread > RealPageHeight + PageFooterHeight)
            /* la page est trop haute */
            /* dh: hauteur qui depasse de la page standard */
            {
              dh = putVThread - PageHeight - PageFooterHeight;
              /* cherche le pave qui precede la marque de page */
              previousAbsBox = pPage->ElAbstractBox[nbView - 1];
              stop = FALSE;
              do
                if (previousAbsBox == NULL)
                  stop = TRUE;
                else if (previousAbsBox->AbPrevious != NULL)
                  stop = TRUE;
                else
                  previousAbsBox = previousAbsBox->AbEnclosing;
              while (!stop);

              if (previousAbsBox != NULL)
                /* la page n'est pas vide */
                /* sauve la hauteur de page normale */
                {
                  normalPageHeight = PageHeight;
                  /* reduit la hauteur de page */
                  PageHeight = RealPageHeight - dh;
                  if (PageHeight < HMinPage)
                    PageHeight = HMinPage;
                  /* detruit le saut de page et ses paves */
                  DestroyAbsBoxesView (pPage, pDoc, FALSE, nbView);
                  /* traitement des elements demandant des coupures speciales */
                  DeletePageAbsBoxes (pPage, pDoc, nbView);
                  if (WorkingPage == pPage)
                    NbBoxesPageHeaderToCreate = 0;
                  /* signale les paves morts au Mediateur */
                  redispAb = pDoc->DocViewModifiedAb[nbView - 1];
                  /*RealPageHeight = PageHeight;*/
                  (void) ChangeConcreteImage (frame, &RealPageHeight, redispAb);
                  /* libere tous les paves morts de la vue */
                  FreeDeadAbstractBoxes (pAb, frame);
                  /* detruit la marque de page a liberer dans l'arbre abstrait */
                  SuppressPageMark (pPage, pDoc, &pElLib);
                  /* signale au Mediateur les paves morts par suite de */
                  /* fusion des elements precedent et suivant les marques */
                  /* supprimees. */
                  redispAb = pDoc->DocViewModifiedAb[nbView - 1];
                  if (redispAb != NULL)
                    {
                      h = RealPageHeight;
                      (void) ChangeConcreteImage (frame, &h, redispAb);
                    }
                  /* libere les elements rendus inutiles par les fusions */
                  DeleteElement (&pElLib, pDoc);
                  pPage = NULL;
                  pagesCounter--;
                  /* on place les marques de page plus haut */
                  SetPage (&pPage, frame, &origCutAbsBox, &needBreak, pDoc, schView, nbView, rootEl);
                  /* retablit la hauteur de page */
                  PageHeight = normalPageHeight;
                }
            }
        }
    }
  return pPage;
}



/*----------------------------------------------------------------------
  DetrImAbs
  detruit l'image abstraite de la vue concernee et	
  efface sa frame si la vue est une vue pour schView
  ----------------------------------------------------------------------*/
static void DestroyImAbsPages (int view, PtrDocument pDoc, int schView)
{
  PtrAbstractBox      pAb;
  int                 h;

  /* ThotBool       tropcourt; */
  int                 frame;
  PtrAbstractBox      rootAbsBox;
  ThotBool            toDestroy;

  frame = 1;			/* initialisation (pour le compilateur !) */
  rootAbsBox = NULL;		/* initialisation (pour le compilateur !) */
  /* on verifie si c'est bien une vue correspondant a la vue du schema */
  if (pDoc->DocView[view - 1].DvPSchemaView == schView)
    {
      rootAbsBox = pDoc->DocViewRootAb[view - 1];
      frame = pDoc->DocViewFrame[view - 1];
      toDestroy = (rootAbsBox != NULL);
    }
  else
    toDestroy = FALSE;	/* rien a faire */
  if (toDestroy)
    {
      /* tous les paves englobes par le pave racine de la vue sont marques */
      /* morts */
      pAb = rootAbsBox->AbFirstEnclosed;
      while (pAb != NULL)
        {
          SetDeadAbsBox (pAb);
          pAb = pAb->AbNext;
        }
      /* ceci est signale au Mediateur */
      h = 0;
      (void) ChangeConcreteImage (frame, &h, rootAbsBox);
      /* libere tous les paves morts de la vue */
      FreeDeadAbstractBoxes (rootAbsBox, frame);
      /* indique qu'il faudra reappliquer les regles de presentation du */
      /* pave racine, par exemple pour recreer les boites de presentation */
      /* creees par lui et qui viennent d'etre detruites. */
      rootAbsBox->AbSize = -1;
      /* on marque le pave racine complet en tete pour que AbsBoxesCreate */
      /* engendre effectivement les paves de presentation cree's en tete */
      /* par l'element racine (regles CreateFirst). */
      if (rootAbsBox->AbLeafType == LtCompound)
        rootAbsBox->AbTruncatedHead = FALSE;
    }
}

/*----------------------------------------------------------------------
  AddLastPageBreak	ajoute une marque de page a la fin de la vue	
  schView de l'arbre de racine pRootEl s'il n'y en a pas deja une
  ----------------------------------------------------------------------*/
PtrElement AddLastPageBreak (PtrElement pRootEl, int schView, PtrDocument pDoc,
                             ThotBool withAPP)
{
  PtrElement          pEl;
  PtrElement          pElPage;
  PtrPSchema          pSchP;
  int                 cpt;
  ThotBool            pageAtEnd;
  ThotBool            stop, stop1, ok;
  NotifyElement       notifyEl;
  int                 NSiblings;

  pElPage = NULL;
  /* cherche d'abord s'il n'y en pas deja une */
  pageAtEnd = FALSE;		/* on n'en pas encore vu */
  if (pRootEl == NULL)
    pEl = NULL;
  else
    pEl = pRootEl->ElFirstChild;
  stop = FALSE;		/* descend l'arbre */
  if (pEl != NULL)
    do
      if (pEl->ElTypeNumber == PageBreak + 1 && pEl->ElViewPSchema == schView)
        {
          /* c'est une marque de page pour cette vue */
          /* saute les marques de page suivantes, qui concernent d'autres vues */
          stop1 = FALSE;
          do
            if (pEl->ElNext == NULL)
              /* dernier element de ce niveau */
              pageAtEnd = TRUE;
          /* la marque trouvee est bien en fin de vue */
            else
              {		/* examine l'element suivant */
                pEl = pEl->ElNext;
                if (pEl->ElTypeNumber != PageBreak + 1)
                  stop1 = TRUE;
                /* ce n'est pas une marque de page, la */
                /* marque trouvee n'est donc pas en fin */
              }
          while (!(stop1 || pageAtEnd));
        }
      else
        /* ce n'est pas une marque de page pour la vue */
        if (pEl->ElNext != NULL)
          pEl = pEl->ElNext;	/* passe au suivant */
        else
          /* il n'y a pas d'element suivant */
          if (pEl->ElTerminal)
            stop = TRUE;	/* c'est une feuille, on s'arrete */
          else
            pEl = pEl->ElFirstChild;	/* descend d'un niveau */
    while (!(stop || pEl == NULL || pageAtEnd));

  pSchP = PresentationSchema (pDoc->DocSSchema, pDoc);
  if (GetPageRule (pRootEl, pDoc, schView, &pSchP))
    /* the document element has a PAGE rule. Add the last PAGE element
       as its last child */
    pEl = pRootEl->ElFirstChild;
  else
    /* the PAGE rule is then associated with the root element */
    {
      pEl = FwdSearchTypedElem (pRootEl, pRootEl->ElStructSchema->SsRootElem,
                                pRootEl->ElStructSchema, NULL);
      if (pEl)
        pEl = pEl->ElFirstChild;
    }

  if (pEl != NULL && !pageAtEnd)
    /* il n'y a pas de marque de page a la fin de la vue */
    /* cree une marque de page */
    {
      /* cherche le dernier fils de la racine */
      NSiblings = 1;
      while (pEl->ElNext != NULL)
        {
          NSiblings++;
          pEl = pEl->ElNext;
        }
      if (withAPP)
        {
          /* envoie l'evenement ElemNew.Pre */
          notifyEl.event = TteElemNew;
          notifyEl.document = (Document) IdentDocument (pDoc);
          notifyEl.element = (Element) (pEl->ElParent);
          notifyEl.info = 0; /* not sent by undo */
          notifyEl.elementType.ElTypeNum = PageBreak + 1;
          notifyEl.elementType.ElSSchema = (SSchema) (pRootEl->ElStructSchema);
          notifyEl.position = NSiblings;
          ok = !CallEventType ((NotifyEvent *) & notifyEl, TRUE);
        }
      else
        ok = TRUE;
      if (ok)
        {
          /* cree l'element marque de page */
          pElPage = NewSubtree (PageBreak + 1, pRootEl->ElStructSchema,
                                pDoc, TRUE, TRUE, TRUE, TRUE);
          /* insere la nouvelle marque de page apres le dernier fils */
          InsertElementAfter (pEl, pElPage);	/* remplit cette marque de page */
          pElPage->ElPageType = PgComputed;
          pElPage->ElViewPSchema = schView;
          /* cherche le compteur de page a appliquer */
          cpt = GetPageCounter (pElPage, pDoc, schView, &pSchP);
          if (cpt == 0)
            /* page non numerotee */
            pElPage->ElPageNumber = 1;
          else
            /* calcule le numero de page */
            pElPage->ElPageNumber = CounterVal (cpt, pElPage->ElStructSchema,
                                                pSchP, pElPage, schView, pDoc);
#ifndef PAGINEETIMPRIME
          /* envoie l'evenement ElemNew.Post */
          if (withAPP)
            NotifySubTree (TteElemNew, pDoc, pElPage, 0, 0, FALSE, FALSE);
#endif /* PAGINEETIMPRIME */
        }
    }
  return pElPage;
}

/*----------------------------------------------------------------------
  PaginateView l'utilisateur demande le (re)decoupage en pages de la	
  vue de numero Vue pour le document pointe' par pDoc.	
  ----------------------------------------------------------------------*/
void PaginateView (PtrDocument pDoc, int view)
{
  PtrElement          pRootEl, firstPage, lastPage, pPage;
  PtrAbstractBox      rootAbsBox, pP;
  PtrAbstractBox      previousPageAbBox;
#ifdef PAGINEETIMPRIME
  PtrAbstractBox      pAb;
  PtrPSchema          pSchP;
  PtrElement          pEl;
  AbPosition         *pPos;
  int                 val;
  int                 h, cpt;
#else /*  PAGINEETIMPRIME */
  PtrElement          firstSelection = NULL, lastSelection = NULL;
  int                 FirstSelectedChar = 0, LastSelectedChar = 0;
  ThotBool            sel;
#endif /* PAGINEETIMPRIME */
  PtrPSchema          pSchPage;
  int                 b, saveHeight;
  int                 schView;
  int                 v, clipOrg;
  int                 frame, volume, volprec, iview;
  ThotBool            shorter;
  ThotBool            complete;
  ThotBool            isFirstPage;
  ThotBool            somthingAdded;

  RunningPaginate = TRUE;
  clipOrg = 0;
  isFirstPage = TRUE;
  pagesCounter = 1;
  firstPage = NULL;
  previousPageAbBox = NULL;
  NbBoxesPageHeaderToCreate = 0;
  /* numero dans le document de la vue a paginer */
  iview = view - 1;
  /* numero dans le schema de la vue a paginer */
  schView = AppliedView (pDoc->DocDocElement, NULL, pDoc, view);
  pRootEl = pDoc->DocDocElement;
  frame = pDoc->DocViewFrame[iview];
  pPage = NULL;
#ifndef PAGINEETIMPRIME
  sel = AbortPageSelection (pDoc, schView, &firstSelection, &lastSelection, &FirstSelectedChar, &LastSelectedChar);
#endif /* PAGINEETIMPRIME */
  /* detruit l'image abstraite de la vue concernee, en conservant la racine */
  for (v = 1; v <= MAX_VIEW_DOC; v++)
    DestroyImAbsPages (v, pDoc, schView);
  /* destruction des marques de page */
  DestroyPageMarks (pDoc, pRootEl, schView);
  pDoc->DocViewFreeVolume[iview] = pDoc->DocViewVolume[iview];
  
  PageHeight = 0;
  PageFooterHeight = 0;	/* cree les paves du debut de la vue */
  pP = AbsBoxesCreate (pRootEl, pDoc, view, TRUE, TRUE, &complete);
  volume = 0;
  /* mise a jour de rootAbsBox apres la creation des paves */
  /* dans le cas de l'appel depuis print, il n'y avait aucun pave cree */
  rootAbsBox = pRootEl->ElAbstractBox[iview];
  if (pDoc->DocViewRootAb[iview] == NULL)
    pDoc->DocViewRootAb[iview] = rootAbsBox;
  pP = rootAbsBox;
  /* cherche la 1ere marque de page dans le debut de l'image du document, */
  /* pour connaitre la hauteur des pages */
  /* cherche d'abord le 1er pave feuille ou la premiere marque de page */
  while (pP->AbFirstEnclosed != NULL &&
         pP->AbElement->ElTypeNumber != PageBreak + 1)
    pP = pP->AbFirstEnclosed;
  if (pP->AbElement->ElTypeNumber != PageBreak + 1)
    /* le document ne commence pas par une marque de page pour cette */
    /* vue ; on cherche la premiere marque de page qui suit */
    pP = AbsBoxFromElOrPres (pP, FALSE, FALSE, PageBreak + 1, NULL, NULL);
  
  previousPageAbBox = pP;
  if (pP != NULL && pP->AbElement->ElTypeNumber == PageBreak + 1)
    /* on a trouve une marque de page, on determine */
    /* la hauteur de ce type de page */
    PageHeaderFooter (pP->AbElement, pDoc, schView, &b, &pSchPage);
  
  /* fait calculer l'image par le Mediateur */
  RealPageHeight = PageHeight;
  shorter = ChangeConcreteImage (frame, &RealPageHeight, rootAbsBox);
  do
    /* traite une page apres l'autre */
    {
      if (previousPageAbBox)
        pP = previousPageAbBox;
      else
        pP = AbsBoxFromElOrPres (rootAbsBox, FALSE, FALSE, PageBreak + 1, NULL, NULL);

      if (pP)
        {
          if (pP->AbElement->ElTypeNumber == PageBreak + 1)
            {
              /* memorize the previous page break */
              previousPageAbBox = pP;
	      
              /* get the height pf the page element */
              PageHeaderFooter (pP->AbElement, pDoc, schView, &b, &pSchPage);
              if (firstPage == NULL)
                firstPage = pP->AbElement;
              /* go to the end of the page element */
              while (pP->AbFirstEnclosed != NULL)
                {
                  pP = pP->AbFirstEnclosed;
                  while (pP->AbNext != NULL)
                    pP = pP->AbNext;
                }
            }
          /* look for a next page break generated by an element */
          pP = AbsBoxFromElOrPres (pP, FALSE, FALSE, PageBreak + 1, NULL, NULL);
          if (!pP)
            pPage = NULL;
          else if (!shorter && pP->AbAfterPageBreak)
            /* a page break found but it's too far away */
            pPage = NULL;
          else
            pPage = pP->AbElement;

          volprec = 0;
          if (pPage == NULL && !shorter)
            {
              volprec = rootAbsBox->AbVolume;
              /* generate a new page break */
              pPage = PutMark (pRootEl, view, pDoc, frame, schView);
            }

          if (pPage && pPage->ElAbstractBox[iview])
            {
              /* previousPageAbBox contient le pave de la page precedente */
#ifdef PAGINEETIMPRIME
              /* generate the page number */
              if (pPage->ElPageType != PgComputed)
                {
                  /* get the current page counter */
                  cpt = GetPageCounter (pPage, pDoc, schView, &pSchP);
                  if (cpt == 0)
                    {
                      pagesCounter++;
                      pPage->ElPageNumber = pagesCounter;
                    }
                  else
                    {
                      pPage->ElPageNumber = CounterVal (cpt, 
                                                        pPage->ElStructSchema, 
                                                        pSchP, pPage, 
                                                        schView, pDoc);
                      /* update the presentation box for next pages */
                      /* this update is done at the end when the pagination */
                      /* is not associated with print */		      
                      UpdateNumbers (pPage, pPage, pDoc, TRUE);
                    }
                  PageHeaderFooter (pPage, pDoc, schView, &b, &pSchPage);
                }
              /* print the new page */
              if (!PrintOnePage (pDoc, previousPageAbBox, pPage->ElAbstractBox[iview],
                                 rootAbsBox, clipOrg, FALSE))
                return;
#endif /* PAGINEETIMPRIME */
              /* kill previous abstract boxes before the page break */
              shorter = KillAbsBoxBeforePage (pPage->ElAbstractBox[iview],
                                              frame, pDoc, view, &clipOrg);
              /* previousPageAbBox points to the current page */
              previousPageAbBox = pPage->ElAbstractBox[iview];
              /* get the removed volume for the new generation */
              if (rootAbsBox->AbVolume < 0)
                rootAbsBox->AbVolume = 0;
              volume = volume + volprec - rootAbsBox->AbVolume;
            }
          else
            /* no page found */
            shorter = TRUE;
        }
      /* complete the image till a new page could be generated */
      /* or the end of the document is reached */
      somthingAdded = FALSE;
      while (shorter && rootAbsBox->AbTruncatedTail)
        {
          if (volume < 100)
            /* minimum added */
            volume = 100;
          do
            {
              pDoc->DocViewFreeVolume[iview] = volume;
              /* volume before the generation*/
              volprec = rootAbsBox->AbVolume;
              /* generation */
              AddAbsBoxes (rootAbsBox, pDoc, FALSE);
              if (rootAbsBox->AbVolume <= volprec)
                /* nothing added */
                volume = 2 * volume;
              else
                /* new abstract boxes generated */
                somthingAdded = TRUE;
            }
          while (rootAbsBox->AbVolume < volprec && rootAbsBox->AbTruncatedTail);
	  
          /* nothing to add */
          volume = 0;
          /* call ChangeConcreteImage to know if we have enough volume */
          saveHeight = RealPageHeight;
          if (pDoc->DocViewModifiedAb[iview] != NULL)
            {
              shorter = ChangeConcreteImage (frame, &RealPageHeight,
                                             pDoc->DocViewModifiedAb[iview]);
              if (!shorter)
                RealPageHeight = saveHeight;
              pDoc->DocViewModifiedAb[iview] = NULL;
            }
        }
    }
  while (pPage || somthingAdded || rootAbsBox->AbTruncatedTail);

  /* Ajoute le saut de page qui manque eventuellement a la fin */
  lastPage = AddLastPageBreak (pRootEl, schView, pDoc, TRUE);

#ifdef PAGINEETIMPRIME
  /* il faut imprimer la derniere page */
  /* on cree d'abord son pave */
  pDoc->DocViewFreeVolume[iview] = 10000;
  rootAbsBox->AbTruncatedTail = TRUE;
  if (lastPage)
    {
      pEl = lastPage->ElParent;
      while (pEl)
        {
          if (pEl->ElAbstractBox[iview])
            pEl->ElAbstractBox[iview]->AbTruncatedTail = TRUE;
          pEl = pEl->ElParent;
        } 
    }
  /* il reste des paves a creer : ce sont ceux de la nouvelle marque de page*/
  AddAbsBoxes (rootAbsBox, pDoc, FALSE);
   
  /* cherche la marque de page qui vient d'etre inseree */
  /* cherche la derniere feuille dans la marque de page precedente */
  pP = previousPageAbBox;
  if (pP != NULL)
    {
      while (pP->AbFirstEnclosed != NULL)
        {
          pP = pP->AbFirstEnclosed;
          while (pP->AbNext != NULL)
            pP = pP->AbNext;
        }
      pP = AbsBoxFromElOrPres (pP, FALSE, FALSE, PageBreak + 1, NULL, NULL);
    }
  if (pP != NULL)
    /* on fait calculer l'image par le mediateur avant d'appeler */
    /* l'impression */
    {
      h = 0;		/* on ne fait pas evaluer la hauteur de coupure */
      if (pDoc->DocViewModifiedAb[iview] != NULL)
        {
          shorter = ChangeConcreteImage (frame, &h, pDoc->DocViewModifiedAb[iview]);
          pDoc->DocViewModifiedAb[iview] = NULL;
        }
      pPos = &pP->AbVertPos;
      pAb = pPos->PosAbRef;
      if (pAb)
        {
          val = RealPageHeight - pAb->AbBox->BxYOrg - pAb->AbBox->BxHeight;
#ifdef _WIN_PRINT
          pPos->PosDistance = (val * ScreenDPI + PrinterDPI/ 2) / PrinterDPI;
#else /* _WIN_PRINT */
          pPos->PosDistance = val + (val * ViewFrameTable[frame - 1].FrMagnification / 10);
#endif /* _WIN_PRINT */
          pP->AbVertPosChange = TRUE;
          ChangeConcreteImage (frame, &h, pP);
        }
      // print the last page
      PrintOnePage (pDoc, previousPageAbBox, pP, rootAbsBox, clipOrg, TRUE);
    }
#endif /* PAGINEETIMPRIME */
   
  RunningPaginate = FALSE;
  /* detruit l'image abstraite de la fin du document */
  DestroyImAbsPages (view, pDoc, schView);
  /* reconstruit l'image de la vue et l'affiche */
#ifndef PAGINEETIMPRIME
  DisplaySelectPages (pDoc, firstPage, view, sel, firstSelection,
                      lastSelection, FirstSelectedChar, LastSelectedChar);
#endif /* PAGINEETIMPRIME */
  /* paginer un document le modifie ... */
  SetDocumentModified (pDoc, TRUE, 0);
}
/* End Of Module page */
