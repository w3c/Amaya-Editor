/* ---------------------------------------------------------------------- */
/* |	Parser du langage de transformations				| */
/* |									| */
/* |	Author:	Stephane Bonhomme					| */
/* |	VarDate:	April 1996						| */
/* |									| */
/* |									| */
/* ---------------------------------------------------------------------- */

/* Compiling this module with -DPPSTANDALONE generates the main program of  */
/* a converter which reads a .trans file and creates the trans tables	  */
/* Without this option, it creates a function ppStartParser that parses a   */
/* trans file and displays the internal representation of transformations.  */

#include "amaya.h"
#ifndef  PPSTANDALONE
#include "application.h"
#include "document.h"
#include "content.h"
#include "tree.h"
#include "browser.h"
#include "interface.h"
#include "selection.h"
#include "dialog.h"
#include "app.h"
#include "message.h"
#include "HTMLstyle.h"
#include "trans.h"
#endif
#ifdef   PPSTANDALONE
#define  NAME_LENGTH 20
#include "parser.h"
#endif

#include "transparse.h"

extern  PathBuffer  SchemaPath;


/* structures de donees du parser */
typedef struct _parForest {
        ListSymb*          first ;
        ListSymb*          last ;
        boolean            optional ;
        struct _parForest* next;
} parForest;

typedef struct _parChoice {
        parForest*  forests;
        boolean     optional;
} parChoice;

static boolean    ppError;
static boolean    ppisnamed;
static boolean    ppOptional;
static boolean    ppIterTag;
static char       ppname[20];
static parChoice* ppChoice;	/* current forest descriptor */
static parForest* ppForest;	/* cuurent forest descriptor */
static parChoice* ppLastChoice;
static TransDesc* ppTrans;	/* current transformation descriptor */
static SymbDesc*  ppSymb;	/* current pattern symbol descriptor */
static AttrDesc*  ppAttr;	/* attribute descriptor */
static NodeDesc*  ppNode;	/* node descriptor */
static RuleDesc*  ppRule;	/* rule descriptor */
static parChoice* ChStack[MAXSTACK];
static char       OpStack[MAXSTACK];
static SymbDesc*  SymbStack[MAXSTACK];
static int        SzStack;
static int        patDepth;
static int        numberOfLinesRead;
static int        numberOfCharRead;
static boolean	  NormalTransition;

#define MaxBufferLength   1000
#define AllmostFullBuffer  700
static unsigned char	inputBuffer[MaxBufferLength];
static int	        ppLgBuffer = 0;	/* actual length of text in input buffer */
typedef int             State;		/* a state of the automaton */
static State            currentState;      /* current state of the automaton */
static State	        returnState;	/* return state from subautomaton */

#include "html2thot.h"

/* ---------------------------------------------------------------------- */
/* initialise toutes les transformations a valide     */
/* mise a jour de match_env.maxdepth */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void SetTransValid (TransDesc* trans)
#else 
static void SetTransValid (trans)
TransDesc* trans;
#endif 
{
   TransDesc* td;
   SymbDesc* sd;

   td = trans ;
   while (td) {
         if (!(td->ActiveTrans)) {
	    td->ActiveTrans = TRUE;
	    if (td->patdepth > match_env.maxdepth)
	       match_env.maxdepth=td->patdepth;
	    sd=td->PatSymbs;
	    while (sd) {
	         sd->ActiveSymb = TRUE ;
	         sd = sd->next ;
	    }
	 }
         td=td->next;
   }
}

/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void freetrans (TransDesc* td)
#else
static void freetrans (td)
TransDesc* td;
#endif
{
   ListSymb *ls,*ls2;
   SymbDesc *sd,*sd2;
   AttrDesc *ad,*ad2;
   RuleDesc *rd,*rd2;
   NodeDesc *n, *n2 ;

   TtaFreeMemory (td->NameTrans);
   TtaFreeMemory (td->TagDest);
   ls = td->First;
   while(ls) {
      ls2 = ls->next;
      TtaFreeMemory ((char*) ls);
      ls = ls2;
   }
   if (td->rootdesc) {
      TtaFreeMemory (td->rootdesc->Tag);
      TtaFreeMemory ((char*) td->rootdesc);
   }
   sd = td->PatSymbs;
   while(sd) {
      TtaFreeMemory (sd->SName);
      TtaFreeMemory (sd->Tag);
      ls = sd->Children;
      while(ls) {
	 ls2 = ls->next;
	 TtaFreeMemory ((char*) ls);
	 ls = ls2;
      }
      ls = sd->Nexts;
      while (ls) {
	 ls2 = ls->next;
	 TtaFreeMemory ((char*) ls);
	 ls = ls2;
      }
      ad = sd->Attributes;
      while (ad) {
	 TtaFreeMemory (ad->NameAttr);
	 if (ad->IsTransf) {
	    TtaFreeMemory (ad->AttrTag);
	    TtaFreeMemory (ad->AttrAttr);
	 } else if (!ad->IsInt)
	    TtaFreeMemory (ad->TextVal);

	  ad2 = ad->next;
	  TtaFreeMemory ((char*) ad);
	  ad=ad2;
      }
      sd2 = sd->next;
      TtaFreeMemory ((char*) sd);
      sd = sd2;
   }
   rd=td->Rules;
   while(rd) {
      n = rd->OptNodes;
      while (n) {
	 ad = n->Attributes;
	 while (ad) {
	    TtaFreeMemory (ad->NameAttr);
	    if (ad->IsTransf) {
	       TtaFreeMemory (ad->AttrTag);
	       TtaFreeMemory (ad->AttrAttr);
	    } else if (!ad->IsInt) {
	       TtaFreeMemory(ad->TextVal);
	    }
	    ad2 = ad->next;
	    TtaFreeMemory ((char*) ad);
	    ad=ad2;
	 }
	 TtaFreeMemory (n->Tag);
	 n2 = n->next;
	 TtaFreeMemory ((char*) n);
	 n = n2;
      }
      n = rd->NewNodes;
      while (n) {
	 ad = n->Attributes;
	 while (ad) {
	    TtaFreeMemory (ad->NameAttr);
	    if (ad->IsTransf) {
	       TtaFreeMemory (ad->AttrTag);
	       TtaFreeMemory (ad->AttrAttr);
	    } else if(!ad->IsInt) {
	       TtaFreeMemory (ad->TextVal);
	    }
	    ad2 = ad->next;
	    TtaFreeMemory ((char*) ad);
	    ad = ad2;
	 }
	 TtaFreeMemory (n->Tag);
	 n2 = n->next;
	 TtaFreeMemory ((char*) n);
	 n = n2;
      }
      rd2 = rd->next;
      TtaFreeMemory ((char*) rd);
      rd = rd2;
   }
   TtaFreeMemory ((char*) td);
}


#ifdef __STDC__
void freelist (ListSymb* pl)
#else
void freelist (pl)
ListSymb* pl;
#endif 
{
   if (pl) {
      freelist (pl->next);
      TtaFreeMemory ((char*) pl);
   }
}

#ifdef __STDC__
static void freeforest (parForest* pf)
#else
static void freeforest (pf)
parForest* pf;
#endif
{
   if (pf) {
      if (pf == ppForest)
	 ppForest = NULL;
      freelist (pf->first);
      freelist (pf->last);
      freeforest (pf->next);
      TtaFreeMemory ((char*) pf);
   }
}

#ifdef __STDC__
static void freechoice (parChoice* pc)
#else
static void freechoice (pc)
parChoice* pc;
#endif 
{
   if (pc) {
      freeforest (pc->forests);
      TtaFreeMemory ((char*) pc);
   }
}

/* ---------------------------------------------------------------------- */
/* |	errorMessage	print the error message msg on stderr.		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void errorMessage (unsigned char* msg)
#else
static void errorMessage (msg)
unsigned char* msg;
#endif 
{
#ifndef PPSTANDALONE
   char numline[5];

   sprintf(numline,"%d", numberOfLinesRead);
   TtaSetStatus (TransDoc, 1, TtaGetMessage (AMAYA, AM_TRANS_PARSE_ERROR), numline) ;
#else
   fprintf (stderr, "line %d, char %d: %s\n", numberOfLinesRead, numberOfCharRead, msg);
#endif
   NormalTransition = FALSE;
}

#ifdef __STDC__
static void addSymb2List (ListSymb** pList, SymbDesc* symb)
#else
static void addSymb2List(pList, symb)
ListSymb** pList;
SymbDesc*  symb;
#endif 
{
   boolean isjok, isnull, found;
   ListSymb *pl, *plnext;

   if (*pList == NULL) {
      *pList = (ListSymb*) TtaGetMemory (sizeof (ListSymb));
      (*pList)->next = NULL;
      (*pList)->symb = symb;
   } else {
      isjok = FALSE;
      isnull = (symb == NULL);
      if (!isnull)
	 isjok = (!strcmp (symb->Tag, "*"));
      pl = *pList;
      found = ((isnull && pl->symb == NULL)|| (!isnull && pl->symb == symb));
      if (pl->next == NULL && !isnull && !found) {
	 if ((isjok && pl->symb == NULL)  ||
	     (!isjok && (pl->symb == NULL || !strcmp (pl->symb->Tag, "*")))) {
	    *pList = (ListSymb*) TtaGetMemory (sizeof (ListSymb));
	    (*pList)->next = pl;
	    (*pList)->symb = symb;
	 } else {
	    pl->next = (ListSymb*) TtaGetMemory (sizeof (ListSymb));
	    pl->next->next = NULL;
	    pl->next->symb = symb;
	 }
      } else {
	 while (!found  && pl->next  && 
                ((isjok || isnull) || (pl->next->symb && strcmp (pl->next->symb->Tag, "*")))
		&& (isnull || pl->next->symb!=NULL)) {
	    found = ((isnull && pl->symb == NULL)||
		     (!isnull && pl->symb == symb));
	    pl = pl->next;
	 }
	 if (!found) {
	    plnext = pl->next;
	    pl->next = (ListSymb*) TtaGetMemory (sizeof (ListSymb));
	    pl->next->next = plnext;
	    pl->next->symb = symb;
	 }
      }
   }
}


#ifdef __STDC__
static void addterminal (parChoice* pc)
#else
static void addterminal (pc)
parChoice* pc;
#endif 
{
   ListSymb  *pl,*pl2;
   parForest *pf;
   SymbDesc  *ps;

   pf = pc->forests;
   while (pf) {
      pl = pf->last;
      while (pl) {
	 ps = pl->symb;
	 if (ps) {
	    pl2 = ps->Nexts;
	    while (pl2 && pl2->next)
	       pl2 = pl2->next;
	    if (!pl2) {
	       ps->Nexts = (ListSymb*) TtaGetMemory (sizeof (ListSymb));
	       pl2 = ps->Nexts;
	    } else {
	       pl2->next = (ListSymb*) TtaGetMemory (sizeof (ListSymb));
	       pl2 = pl2->next;
	    }
	    pl2->next = NULL;
	    pl2->symb = NULL;
	 }
	 pl = pl->next;
      }
      pf = pf->next;
   }
}

/* ---------------------------------------------------------------------- */
/* |   traite le symbole de pattern qui vient d'etre lu	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void TraiteFinSymb (void)
#else
static void TraiteFinSymb ()
#endif 
{ 
   SymbDesc* sd;  
   char	     msgBuffer[MaxBufferLength];

   if (ppLgBuffer != 0) { /* cree un nouveau symbole dans le pattern */
      ppTrans->nbPatSymb++;
      ppSymb = (SymbDesc*) TtaGetMemory (sizeof (SymbDesc));
      ppSymb->SName = (char*) TtaGetMemory (NAME_LENGTH);
      ppSymb->Tag = (char*) TtaGetMemory (NAME_LENGTH);
      ppSymb->Rule = NULL;
      ppSymb->Children = NULL;
      ppSymb->Nexts = NULL;
      ppSymb->Optional = ppOptional;
      ppSymb->ActiveSymb = TRUE;
      ppSymb->Attributes = NULL;
      ppSymb->next = NULL;
      ppSymb->depth = patDepth;
      if (ppisnamed) {
	 strcpy (ppSymb->SName, ppname);
	 strcpy (ppname,"");
      } else
	 strcpy (ppSymb->SName, inputBuffer);
      strcpy (ppSymb->Tag, inputBuffer);
      
      ppisnamed = FALSE;
      ppLgBuffer = 0;
      ppOptional = FALSE;
   }
   if (ppSymb) {
      /* on ajoute le nouveau symbole a ceux du pattern */
      sd = ppTrans->PatSymbs;
      if (!sd)
	 ppTrans->PatSymbs=ppSymb;
      else {
	 while (sd->next)
	    sd = sd->next;
	 sd->next = ppSymb;
      }
      if (strcmp (ppSymb->Tag, "*") && (MapGI(ppSymb->Tag) == -1)) {
	  ppError = TRUE;
	  sprintf (msgBuffer, "unknown tag </%s>", ppSymb->Tag);
	  errorMessage (msgBuffer);
      }
   } 
}

#ifdef __STDC__
static void NouveauSymbole (void)
#else
static void NouveauSymbole ()
#endif 
{ 
   SymbDesc* ps;
   ListSymb* pList;
  
   if (ppSymb) {
      if (ppForest->first == NULL) {/* le nouveau symbole est le premier et le dernier de la foret courante */
	 ppForest->first = (ListSymb*) TtaGetMemory (sizeof (ListSymb));
	 ppForest->first->next = NULL;
	 ppForest->first->symb = ppSymb;
	 ppForest->last = (ListSymb*) TtaGetMemory (sizeof (ListSymb));
	 ppForest->last->next = NULL;
	 ppForest->last->symb = ppSymb;
      }
      else {/* le symbole n'est pas le premier de la foret courante */
	    /* le nouveau symbole est ajoute a la liste des suivants */
	    /*de chaque dernier symbole de la foret */
	 pList = ppForest->last;
	 while (pList) {
	    ps = pList->symb;
	    if (ps) {
	       addSymb2List (&ps->Nexts, ppSymb);
	    }
	    pList=pList->next;
	 }
	 if (ppForest->optional) {
	    /* si les symboles precedents sont tous optionnels, */
	    /*on ajoute le symbole courant a la liste des premiers de la foret */
	    addSymb2List (&ppForest->first, ppSymb);
	    ppForest->optional = ppSymb->Optional;
	 }
	 if (ppSymb->Optional) {
	    /* si le symbole est optionnel, */
	    /* on l'ajoute a la liste des derniers de la foret courante */
	    addSymb2List (&ppForest->last, ppSymb);    
	 } else {
	    /* si le symbole est obligatoire, */
	    /* il devient le dernier possible de la foret courante */
	    freelist (ppForest->last);
	    ppForest->last = (ListSymb*) TtaGetMemory (sizeof(ListSymb));
	    ppForest->last->next = NULL;
	    ppForest->last->symb = ppSymb;
	 } 
      }
      if (ppIterTag)
	 addSymb2List (&ppSymb->Nexts, ppSymb);
      ppIterTag=FALSE;
   }
   ppSymb = NULL;
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void SymbName (unsigned char c)
#else
static void SymbName (c)
unsigned char	c;
#endif 
{
   if (ppLgBuffer == 0) { 
      ppError = TRUE;
      errorMessage ("missing tag name");
   }
   ppisnamed = TRUE;
   strcpy (ppname, inputBuffer);
   ppLgBuffer = 0;
}

/* -------------------------------------------------------------------- */
/* |	un nom de transformation a ete lu, on alloue un descripteur de transformation	| */
/* |   modif : 20/05/96								| */
/* -------------------------------------------------------------------- */
#ifdef __STDC__
static void EndNameTrans (unsigned char c)
#else
static void EndNameTrans (c)
unsigned char	c;
#endif 
{

   if (ppLgBuffer !=0) { /* on alloue un nouveau descripteur de transformation */
      patDepth = 0;
      ppTrans = (TransDesc*) TtaGetMemory (sizeof (TransDesc));
      ppTrans->NameTrans = TtaGetMemory (NAME_LENGTH);
      strcpy (ppTrans->NameTrans, inputBuffer);
      ppTrans->nbPatSymb = 0;
      ppTrans->nbRules = 0;
      ppTrans->patdepth = 0;
      ppTrans->First = NULL;
      ppTrans->rootdesc = NULL;
      ppTrans->PatSymbs = NULL;
      ppTrans->Rules = NULL;
      ppTrans->ActiveTrans = TRUE;
      ppTrans->TagDest = NULL;
      ppTrans->next = NULL;
      ppLgBuffer = 0;
    
      /* on alloue de nouvelles structures choix et foret */ 
      ppChoice = (parChoice*) TtaGetMemory (sizeof (parChoice));
      ppForest = (parForest*) TtaGetMemory (sizeof (parForest));
      ppChoice->forests = ppForest;
      ppChoice->optional = FALSE;
      ppForest->first = NULL;
      ppForest->last = NULL;
      ppForest->optional = FALSE;
      ppForest->next = NULL;
   } else {
      ppError = TRUE;
      errorMessage ("Missing Transformation Name");
   }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void Option (unsigned char c)
#else
static void Option (c)
unsigned char c;
#endif 
{
   ppOptional = TRUE;
   if (ppForest->first == NULL)
      ppForest->optional = TRUE;
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void BeginExp (unsigned char c)
#else
static void BeginExp (c)
unsigned char	c;
#endif 
{
   /*   TraiteFinSymb(); */
   ChStack[SzStack] = ppChoice;
   OpStack[SzStack] = c;
   SymbStack[SzStack++] = ppSymb; /* on empile le symbole */
   ppChoice = (parChoice*) TtaGetMemory (sizeof (parChoice));
   ppForest = (parForest*) TtaGetMemory (sizeof (parForest));
   ppChoice->forests = ppForest;
   ppChoice->optional = ppOptional;
   ppForest->first = NULL;
   ppForest->last = NULL;
   ppForest->optional = FALSE;
   ppForest->next = NULL;
   ppOptional = FALSE;
}


/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void EndExp (unsigned char c)
#else
static void EndExp (c)
unsigned char	c;
#endif 
{

   parForest* pf;
   SymbDesc*  ps;
   ListSymb*  pList, *pL3;
  
   TraiteFinSymb ();
   NouveauSymbole ();
   if (SzStack < 1 || OpStack[SzStack-1] != '(') { 
      ppError = TRUE;
      errorMessage ("mismatched parenthesises");
   } else { 
      if (ppLastChoice)
	 freechoice (ppLastChoice);
      ppLastChoice = ppChoice;
      pf = ppChoice->forests;
      while (pf && !ppChoice->optional) {
	 ppChoice->optional = ppChoice->optional || pf->optional;
	 pf = pf->next;
      }
      /* depile le descripteur de choix*/
      ppChoice = ChStack [SzStack-1];
      SzStack--;
      ppForest = ppChoice->forests;
      /* parcours les descripteurs fils du choix (le courant est le dernier) */
      while (ppForest->next)
	 ppForest = ppForest->next;

      if (ppForest->first == NULL) { 
         /* si le descripteur de foret est vide */
	 /* on remplace les listes de noeuds par celles de l'expression parenthesee */ 
	 pf = ppLastChoice->forests;
	 while (pf) {
	    pList = pf->first;
	    while (pList) {
	       addSymb2List (&ppForest->first, pList->symb);
	       pList = pList->next;
	    }
	    pList = pf->last;
	    while (pList) {
	       addSymb2List (&ppForest->last, pList->symb);
	       pList = pList->next;
	    }
	    pf = pf->next;
	 }
	 ppForest->optional=ppLastChoice->optional;
      } else {
	  /* les premiers de l'expression parenthesee sont ajoutes aux suivants */
	  /*possibles des derniers symboles de l'expression courante */
	  pList = ppForest->last;
	  while (pList) {
	     ps = pList->symb;
	     if (ps) { /* ps: un symbole dernier */
		pf = ppLastChoice->forests;
		while (pf) {
		   pL3 = pf->first;
		   while (pL3) {
		      addSymb2List (&ps->Nexts, pL3->symb);
		      pL3 = pL3->next;
		   }
		   pf = pf->next;
		}
	     }		      
	     pList = pList->next;
	  }

	  if (ppForest->optional) { 
	     /* si les noeuds precedents sont tous optionnels, les premiers de l'expression */
	     /* parenthesee sont ajoutes a ceux de l'expression courante */
	     pf = ppLastChoice->forests;
	     while (pf) {
		pList=pf->first;
		while (pList) {
		   addSymb2List (&ppForest->first, pList->symb);
		   pList = pList->next;
		}
		pf=pf->next;
	     }
	  }

	  if (!ppLastChoice->optional) { /* on remplace ppF->last par les derniers de ppLastChoice */
	     freelist (ppForest->last);
	     ppForest->last = NULL;
	  }
	  /*on ajoute les derniers de ppLastChoice a ppF->last */ 
	  pf = ppLastChoice->forests;
	  while (pf) {
	     pList = pf->last;
	     while (pList) {
		addSymb2List (&ppForest->last, pList->symb);
		pList = pList->next;
	     }
	     pf = pf->next;
	  }
      }
       ppForest->optional = ppForest->optional && ppLastChoice->optional;
   }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void IterationTag (unsigned char c)
#else
static void IterationTag (c)
unsigned char	c;
#endif 
{
   ppIterTag = TRUE;
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void Iteration (unsigned char c)
#else
static void Iteration (c)
unsigned char	c;
#endif 
{
   parForest *pf1, *pf2;
   ListSymb  *plfirst, *pllast;
   SymbDesc  *ps;

   if (ppLastChoice) {
      /* on ajoute les premiers symboles de ppLastChoice aux suivants possibles des derniers */
      pf1 = ppLastChoice->forests;
      while (pf1) {
	 plfirst = pf1->first;
	 while (plfirst) {
	    pf2 = ppLastChoice->forests;
	    while (pf2) {
	       pllast = pf2->last;
	       while (pllast) {
		  ps = pllast->symb;
		  if (ps)
		     addSymb2List (&ps->Nexts, plfirst->symb);
		  pllast = pllast->next;
	       }
	       pf2 = pf2->next;
	    }
	    plfirst = plfirst->next;
	 }
	 pf1 = pf1->next;
      }
      freechoice (ppLastChoice);
      ppLastChoice = NULL;
   }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void BeginChild (unsigned char c)
#else
static void BeginChild (c)
unsigned char	c;
#endif 
{
   TraiteFinSymb ();
   SymbStack[SzStack] = ppSymb;
   NouveauSymbole ();
   OpStack[SzStack] = c;
   ChStack[SzStack++] = ppChoice;  
   ppSymb = NULL;
   ppChoice = (parChoice*) TtaGetMemory (sizeof (parChoice));
   ppForest = (parForest*) TtaGetMemory (sizeof (parForest));
   ppChoice->forests = ppForest;
   ppChoice->optional = FALSE;
   ppForest->first = NULL;
   ppForest->last = NULL;
   ppForest->optional = FALSE;
   ppForest->next = NULL;
   ppOptional = FALSE;
   patDepth++;
}
/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void EndChild (unsigned char c)
#else
static void EndChild (c)
unsigned char	c;
#endif 
{
   parChoice* pc;
   parForest* pf;
   ListSymb*  pl;
 
   if (SzStack<1 || OpStack[SzStack-1]!='{') {
      ppError = TRUE;
      errorMessage ("mismatched parenthesizes");
   } else { 
      /* process the last read symbol*/
      TraiteFinSymb ();
      NouveauSymbole ();
      addterminal (ppChoice);
      /* check if the current depth is the maximal depth */
      if (patDepth > match_env.maxdepth) 
	 match_env.maxdepth=patDepth;
      if (patDepth > ppTrans->patdepth) 
	 ppTrans->patdepth=patDepth;
      patDepth--;

      pc = ppChoice;
      /*on depile le contexte precedent */
      ppChoice = ChStack[SzStack-1];
      ppSymb = SymbStack[SzStack-1];
      SzStack--;
      pf = pc->forests;
      while (pf && !pc->optional) {
	 pc->optional = pc->optional || pf->optional;
	 pf = pf->next;
      }
      ppSymb->OptChild = pc->optional;

      /* on insere les premiers du contexte fils comme enfants du contexte pere */
      /*ppSymb->Children;*/
      pf = pc->forests;
      while (pf) {
	 pl = pf->first;
	 while (pl) {
	    addSymb2List (&ppSymb->Children, pl->symb);
	    pl = pl->next;
	 }
	 pf = pf->next;
      }
      freechoice (pc); 
      pf = ppChoice->forests;
      while (pf->next)
	 pf = pf->next;
      ppForest = pf;
      ppSymb = NULL;
   }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void EndChoice (unsigned char c)
#else
static void EndChoice (c)
unsigned char	c;
#endif 
{
   TraiteFinSymb ();
   NouveauSymbole ();
   ppForest->next = (parForest*) TtaGetMemory (sizeof (parForest));
   ppForest = ppForest->next;
   ppForest->first = NULL;
   ppForest->last = NULL;
   ppForest->optional = FALSE;
   ppForest->next = NULL;
   ppOptional = FALSE;
   if (ppLastChoice) {
      freechoice (ppLastChoice);
      ppLastChoice = NULL;
   }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void EndPatNode (unsigned char c)
#else
static void EndPatNode (c)
unsigned char	c;
#endif 
{  
  
   TraiteFinSymb ();
   NouveauSymbole ();
   ppOptional = FALSE;
   if (ppLastChoice) {
      freechoice (ppLastChoice);
      ppLastChoice = NULL;
   }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void EndPattern (unsigned char c)
#else
static void EndPattern (c)
unsigned char	c;
#endif 
{
   parForest *pf;
   ListSymb  *pl,*pl2;

   TraiteFinSymb ();
   NouveauSymbole ();
   if (SzStack != 1) {
      ppError = TRUE;
      errorMessage ("Syntax error");
   } else {
      addterminal (ppChoice);
      pl2 = ppTrans->First;
      pf = ppChoice->forests;
      while (pf) {
	 pl = pf->first;
	 pf->first = NULL;
	 if (pl) {
	    if (ppTrans->First==NULL) {
	       ppTrans->First = pl;
	       pl2 = pl;
	    } else {
	       while (pl2->next)
		  pl2 = pl2->next;
	       pl2->next = pl;
	    }
	 }
	 pf = pf->next;
      }
   }
   freechoice (ppChoice);
   ppChoice = NULL;
   ppForest = NULL;
}

/* actions semantiques des tags */
/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void BeginOfTag (unsigned char c)
#else
static void BeginOfTag (c)
unsigned char	c;
#endif 
{  
   if (ppLgBuffer!=0) { 
      ppError = TRUE;
      errorMessage ("Syntax Error");
   } else { 
       ppSymb = (SymbDesc *)TtaGetMemory(sizeof(SymbDesc));
       ppSymb->SName= (char *)TtaGetMemory(NAME_LENGTH);
       ppSymb->Tag = (char *)TtaGetMemory(NAME_LENGTH);
       strcpy(ppSymb->SName,"");
       strcpy(ppSymb->Tag,"");
       ppSymb->Rule = NULL;
       ppSymb->Children=NULL;
       ppSymb->Nexts=NULL;
       ppSymb->Optional=ppOptional;
       ppSymb->ActiveSymb=TRUE;
       ppSymb->Attributes=NULL;
       ppSymb->next=NULL;
       if (ppisnamed) {
	  strcpy (ppSymb->SName, ppname);
	  strcpy (ppname,"");
       }
   }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void BeginRuleTag (unsigned char c)
#else
static void BeginRuleTag (c)
unsigned char	c;
#endif 
{
   if (ppLgBuffer!=0) { 
      ppError = TRUE;
      errorMessage ("Syntax Error");
   } else { 
      strcpy (ppNode->Tag, "");
      ppNode->Attributes = NULL;
      ppNode->next = NULL;
   }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void EndOfTagName (unsigned char c)
#else
static void EndOfTagName (c)
unsigned char	c;
#endif 
{
   if (ppLgBuffer!=0) {
      strcpy (ppSymb->Tag, inputBuffer);
      if (!ppisnamed)
	 strcpy(ppSymb->SName, inputBuffer);
      ppLgBuffer = 0;
      ppisnamed = FALSE;
   } else if (!strcmp (ppSymb->Tag, "")) {
      ppError = TRUE;
      errorMessage ("Missing Tag Name");
   }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void EndRuleTagName (unsigned char c)
#else
static void EndRuleTagName (c)
unsigned char	c;
#endif 
{
   if (ppLgBuffer!=0) {
      strcpy (ppNode->Tag, inputBuffer);
      ppLgBuffer = 0;
   } else if (!strcmp(ppNode->Tag, "")) {
      ppError = TRUE;
      errorMessage ("Missing Tag Name");
   }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void ppEndOfAttrName (unsigned char c)
#else
static void ppEndOfAttrName (c)
unsigned char	c;
#endif 
{
   int ThotAttrNum;
   char	msgBuffer[MaxBufferLength];

   if (ppLgBuffer != 0) {
#ifndef PPSTANDALONE
      ThotAttrNum = MapThotAttr (inputBuffer, ppSymb->Tag);
      if (ThotAttrNum != -1) {
#endif
	 ppAttr = ppSymb->Attributes;
	 if (!ppAttr) {
	    ppSymb->Attributes = (AttrDesc*) TtaGetMemory (sizeof (AttrDesc));
	    ppAttr = ppSymb->Attributes;
	    ppAttr->NameAttr = (char*) TtaGetMemory (NAME_LENGTH);
	    ppAttr->next = NULL;
	 } else {
	    while (ppAttr->next && strcmp (ppAttr->NameAttr, inputBuffer))
	       ppAttr = ppAttr->next;
	    if (!strcmp (ppAttr->NameAttr, inputBuffer)) {
	       ppError = TRUE;
	       errorMessage ("Multi valued attribute");
	    } else {
	       ppAttr->next = (AttrDesc*) TtaGetMemory (sizeof (AttrDesc));
	       ppAttr = ppAttr->next;
	       ppAttr->NameAttr = TtaGetMemory (NAME_LENGTH);
	       ppAttr->next = NULL;
	    }
	 }
	 strcpy (ppAttr->NameAttr, inputBuffer);
	 ppAttr->ThotAttr = ThotAttrNum;
	 ppAttr->IsInt = TRUE;
	 ppAttr->IsTransf = FALSE;
	 ppAttr->IntVal = 0;
#ifndef PPSTANDALONE
      } else {
	 ppError = TRUE;
	 sprintf (msgBuffer, "unknown attribute %s", inputBuffer);
	 errorMessage (msgBuffer);
      }
#endif
      ppLgBuffer = 0;
   } else {
      ppError = TRUE;
      errorMessage ("Missing Attribute Name");
   }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void ppEndRuleAttrName (unsigned char c)
#else
static void ppEndRuleAttrName (c)
unsigned char	c;
#endif 
{
   int ThotAttrNum;
   char	msgBuffer[MaxBufferLength];

   if (ppLgBuffer!=0) {
#ifndef PPSTANDALONE
      ThotAttrNum = MapThotAttr (inputBuffer, ppNode->Tag);
      if (ThotAttrNum != -1) {
#endif
	 ppAttr = ppNode->Attributes;
	 if (!ppAttr) {
	    ppNode->Attributes = (AttrDesc*) TtaGetMemory (sizeof (AttrDesc));
	    ppAttr = ppNode->Attributes;
	    ppAttr->NameAttr = (char*) TtaGetMemory (NAME_LENGTH);
	    ppAttr->next = NULL;
	 } else {
	     while (ppAttr->next&& strcmp (ppAttr->NameAttr, inputBuffer))
	        ppAttr = ppAttr->next;
	     if (!strcmp (ppAttr->NameAttr, inputBuffer)) {
		ppError = TRUE;
		errorMessage ("Multi valued attribute");
	     } else {
		ppAttr->next = (AttrDesc*) TtaGetMemory (sizeof (AttrDesc));
		ppAttr = ppAttr->next;
		ppAttr->NameAttr = TtaGetMemory (NAME_LENGTH);
		ppAttr->next = NULL;
	     }
	  }
	  strcpy (ppAttr->NameAttr, inputBuffer);
	  ppAttr->ThotAttr = ThotAttrNum;
	  ppAttr->IsInt = TRUE;
	  ppAttr->IsTransf = FALSE;
	  ppAttr->IntVal = 0;
#ifndef PPSTANDALONE
       } else {
	  ppError = TRUE;
	  sprintf (msgBuffer, "unknown attribute %s", inputBuffer);
	  errorMessage (msgBuffer);
       }
#endif
       ppLgBuffer = 0;
    } else {
       ppError = TRUE;
       errorMessage ("Missing Attribute Name");
    }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void ppTransAttr (unsigned char c)
#else
static void ppTransAttr (c)
unsigned char	c;
#endif 
{
   int ThotAttrNum;

   if (ppLgBuffer != 0) {
#ifndef PPSTANDALONE
      ThotAttrNum = MapThotAttr (inputBuffer, ppNode->Tag);
      if (ThotAttrNum != -1) {
#endif
	 ppAttr = ppNode->Attributes;
	 if (!ppAttr) {
	    ppNode->Attributes = (AttrDesc*) TtaGetMemory (sizeof (AttrDesc));
	    ppAttr = ppNode->Attributes;
	    ppAttr->NameAttr = (char*) TtaGetMemory (NAME_LENGTH);
	    ppAttr->next = NULL;
	 } else {
	    while (ppAttr->next && strcmp (ppAttr->NameAttr, inputBuffer))
	       ppAttr = ppAttr->next;
	    if (!strcmp (ppAttr->NameAttr, inputBuffer)) {
	       ppError=TRUE;
	       errorMessage ("Multi valued attribute");
	    } else {
		ppAttr->next = (AttrDesc*) TtaGetMemory (sizeof (AttrDesc));
		ppAttr = ppAttr->next;
		ppAttr->NameAttr = TtaGetMemory (NAME_LENGTH);
		ppAttr->next = NULL;
	     }
	  }
	  ppAttr->AttrTag = TtaGetMemory (NAME_LENGTH);
	  strcpy (ppAttr->AttrTag, inputBuffer);
	  strcpy (ppAttr->NameAttr, "");
	  ppAttr->ThotAttr = ThotAttrNum;
	  ppAttr->IsInt = FALSE;
	  ppAttr->IsTransf = TRUE;
#ifndef PPSTANDALONE
       } else {
	  ppError = TRUE;
	  errorMessage ("Unknown attribute");
       }
#endif
       ppLgBuffer=0;
    } else {
       ppError = TRUE;
       errorMessage ("Missing Attribute Name");
    }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void ppTransAttrValue (unsigned char c)
#else
static void ppTransAttrValue (c)
unsigned char	c;
#endif 
{
   if (ppLgBuffer != 0) {
      ppAttr->IsTransf = TRUE;
      ppAttr->AttrTag = TtaGetMemory (NAME_LENGTH);
      strcpy (ppAttr->AttrTag, inputBuffer);
      ppLgBuffer = 0;
   } else {
      ppError = TRUE;
      errorMessage ("Missing Tag Name");
   }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void ppEndTransAttr (unsigned char c)
#else
static void ppEndTransAttr (c)
unsigned char	c;
#endif 
{ 
   if (ppLgBuffer != 0) {
      ppAttr->AttrAttr = TtaGetMemory (NAME_LENGTH);
      strcpy (ppAttr->AttrAttr, inputBuffer);
      if (!strcmp (ppAttr->NameAttr, "")) {
	 strcpy (ppAttr->NameAttr, inputBuffer);
      }
      ppLgBuffer = 0;
   } else {
      ppError = TRUE;
      errorMessage ("Missing Attribute Name");
   }
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void ppStartOfAttrValue (unsigned char c)
#else
static void ppStartOfAttrValue (c)
unsigned char	c;
#endif 
{
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void ppEndOfAttrValue (unsigned char c)
#else
static void ppEndOfAttrValue (c)
unsigned char	c;
#endif 
{
   boolean isText;
   int i, AttrVal;

   if (ppLgBuffer == 0) {
      ppAttr->IsInt = FALSE;
      ppAttr->TextVal = TtaGetMemory (sizeof (char) * (ppLgBuffer+2));
      strcpy (ppAttr->TextVal, "");
   } else {
      isText = FALSE;
      for (i = 0; !isText && i < ppLgBuffer; i++)
	  isText = (inputBuffer[i ]< '1' || inputBuffer[i ]> '9');
      if (!isText) {
	 ppAttr->IsInt = TRUE;
	 ppAttr->IntVal = atoi (inputBuffer);
      } else {
#ifdef PPSTANDALONE
	 AttrVal = -1;
#else
	 AttrVal = MapAttrValue (ppAttr->ThotAttr, inputBuffer);
#endif
	 if (AttrVal != -1) {
	    ppAttr->IsInt = TRUE;
	    ppAttr->IntVal = AttrVal;
	 } else {
	    ppAttr->TextVal = (char*) TtaGetMemory (sizeof (char) * (ppLgBuffer+2));
	    strcpy (ppAttr->TextVal, inputBuffer);
	    ppAttr->IsInt = FALSE;
	 }
      }
      ppLgBuffer = 0;
   }
}


/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void BeginRules (unsigned char c)
#else
static void BeginRules (c)
unsigned char	c;
#endif 
{
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void EndPremisse (unsigned char c)
#else
static void EndPremisse (c)
unsigned char	c;
#endif 
{
   if (ppLgBuffer != 0) {/* allocates a new rule descriptor */
      ppRule = (RuleDesc*) TtaGetMemory (sizeof (RuleDesc));
      ppRule->RName = TtaGetMemory (20);
      strcpy (ppRule->RName, inputBuffer);
      ppRule->next = NULL;
      ppRule->OptNodes = (NodeDesc*) TtaGetMemory (sizeof (NodeDesc));
      ppRule->NewNodes = NULL;
      ppNode = ppRule->OptNodes;
      ppNode->Tag = TtaGetMemory (NAME_LENGTH);
      strcpy (ppNode->Tag, "");
      ppNode->Attributes = NULL;
      ppNode->next = NULL;
      ppLgBuffer = 0;
   } else {
      ppError = TRUE;
      errorMessage ("Missing left part of rule");
   }
}


/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void EndNode (unsigned char c)
#else
static void EndNode (c)
unsigned char	c;
#endif 
{ 
   char	msgBuffer[MaxBufferLength];

   if (ppLgBuffer != 0)  {
      strcpy (ppNode->Tag, inputBuffer);
      ppLgBuffer = 0;
      if (strcmp (ppNode->Tag, "*") && (MapGI(ppNode->Tag) == -1)) {
	 ppError = TRUE;
	 sprintf (msgBuffer, "unknown tag </%s>", ppNode->Tag);
	 errorMessage (msgBuffer);
      }
   }
   if (ppNode && c == '.') {/* allocate the next node descriptor*/
      ppNode->next = (NodeDesc*) TtaGetMemory (sizeof (NodeDesc));
      ppNode = ppNode->next;
      ppNode->Tag = TtaGetMemory (NAME_LENGTH);
      strcpy (ppNode->Tag, "");
      ppNode->Attributes = NULL;
      ppNode->next = NULL;
   }
}
/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void EndOptNodes (unsigned char c)
#else
static void EndOptNodes (c)
unsigned char	c;
#endif 
{
   AttrDesc *ad, *ad2;
   char	msgBuffer[MaxBufferLength];

   if (ppLgBuffer != 0)  {
      strcpy (ppNode->Tag,inputBuffer);
      ppLgBuffer = 0;
      if (strcmp (ppNode->Tag, "*") && (MapGI (ppNode->Tag) == -1)) {
	 ppError = TRUE;
	 sprintf (msgBuffer, "unknown tag </%s>", ppNode->Tag);
	 errorMessage (msgBuffer);
      }      
   }
   if (!strcmp (ppRule->OptNodes->Tag, "")) {
      /* frees the current node descriptor if it is empty (the rule has no opt. node */
      ad = ppRule->OptNodes->Attributes;
      while (ad) {
	 TtaFreeMemory (ad->NameAttr);
	 if (ad->IsTransf) {
	    TtaFreeMemory (ad->AttrTag);
	    TtaFreeMemory (ad->AttrAttr);
	 } else if(!ad->IsInt) 
	    TtaFreeMemory (ad->TextVal);
	 ad2 = ad->next;
	 TtaFreeMemory ((char*) ad);
	 ad = ad2;
      }
      TtaFreeMemory (ppRule->OptNodes->Tag);
      TtaFreeMemory ((char*) ppRule->OptNodes); 
      ppRule->OptNodes = NULL;
   }
   /* allocate a New node descriptor*/
   ppRule->NewNodes = (NodeDesc*) TtaGetMemory (sizeof (NodeDesc));
   ppNode = ppRule->NewNodes;
   ppNode->Tag = TtaGetMemory (NAME_LENGTH);
   strcpy (ppNode->Tag, "");
   ppNode->Attributes = NULL;
   ppNode->next = NULL;
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void EndRule (unsigned char c)
#else
static void EndRule (c)
unsigned char	c;
#endif 
{
   boolean ok;
   RuleDesc *prule;
   SymbDesc *psymb;
   NodeDesc *pnode;
   AttrDesc *ad, *ad2;
   char	msgBuffer[MaxBufferLength];

   if (ppLgBuffer !=0 )  {
      strcpy (ppNode->Tag, inputBuffer);
      ppLgBuffer = 0;
      if (strcmp (ppNode->Tag,"*") && (MapGI (ppNode->Tag) == -1)) {
	 ppError = TRUE;
	 sprintf (msgBuffer, "unknown tag </%s>", ppNode->Tag);
	 errorMessage (msgBuffer);
      }    
   }
   if (ppRule->OptNodes && !strcmp (ppRule->OptNodes->Tag, "")) {/* free the last node if it is empty */
      ad = ppRule->OptNodes->Attributes;
      while (ad) {
	 TtaFreeMemory (ad->NameAttr);
	 if (ad->IsTransf) {
	    TtaFreeMemory (ad->AttrTag);
	    TtaFreeMemory (ad->AttrAttr);
	 } else if (!ad->IsInt) 
	    TtaFreeMemory (ad->TextVal);
	 ad2 = ad->next;
	 TtaFreeMemory ((char*) ad);
	 ad = ad2;
      }
      TtaFreeMemory (ppRule->OptNodes->Tag);
      TtaFreeMemory ((char*) ppRule->OptNodes); 
      ppRule->NewNodes = NULL;
   }  
  
   if (ppRule->NewNodes && !strcmp (ppRule->NewNodes->Tag, "")) {/* free the last node if it is empty */
      ad = ppRule->NewNodes->Attributes;
      while (ad) {
	 TtaFreeMemory (ad->NameAttr);
	 if (ad->IsTransf) {
	    TtaFreeMemory (ad->AttrTag);
	    TtaFreeMemory (ad->AttrAttr);
	 } else if (!ad->IsInt) 
	    TtaFreeMemory (ad->TextVal);
	 ad2 = ad->next;
	 TtaFreeMemory ((char*) ad);
	 ad = ad2;
      }
      TtaFreeMemory (ppRule->NewNodes->Tag);
      TtaFreeMemory ((char*) ppRule->NewNodes); 
      ppRule->NewNodes = NULL;
   }
   ppNode = NULL;

   /* insert the rule descriptor in the transformation descriptor */ 
   prule = ppTrans->Rules;
   if (prule == NULL)
      ppTrans->Rules=ppRule;
   else {    
    while (prule->next)
       prule = prule->next;
       prule->next = ppRule;
    }
    ppTrans->nbRules++;

   /* link the symbols of the pattern to the current rule */
   psymb = ppTrans->PatSymbs;
   ok = FALSE;
   while (psymb) {
      if (!strcmp (ppRule->RName, psymb->SName)) {
	 psymb->Rule = ppRule;
	 ok = TRUE;
      }
      psymb = psymb->next;
   }

   if (ok) {/* the rule is linked to 1 symbol at least */
            /* check its consistence with the destination type of the current transformation */
      if (ppRule->OptNodes)
	 pnode = ppRule->OptNodes;
      else
	 pnode = ppRule->NewNodes;
         if (pnode && ppTrans->TagDest == NULL) {
            /* the destination type is undefined => the first tag of the rule defines */
	    /* the destination type of the transformation */
	    ppTrans->TagDest = TtaGetMemory (NAME_LENGTH);
	    strcpy (ppTrans->TagDest, pnode->Tag);
	 } else if (pnode && strcmp (ppTrans->TagDest, pnode->Tag))
            /* the first tag of the rule is different from the destination type : the */
	    /* rule has no destination type */
	    strcpy (ppTrans->TagDest, "");
      } else {
         ppError = TRUE;
      errorMessage ("undefined pattern symbol");
   }
   ppRule = NULL;
}

/* ---------------------------------------------------------------------- */
/* |			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void EndTransformation (unsigned char c)
#else
static void EndTransformation (c)
unsigned char	c;
#endif 
{  
   TransDesc *td;

   /* create the pattern virtual root node */
   ppTrans->rootdesc = (SymbDesc*) TtaGetMemory (sizeof (SymbDesc));
   ppTrans->rootdesc->SName = ppTrans->NameTrans;
   ppTrans->rootdesc->Tag = TtaGetMemory (NAME_LENGTH);
   strcpy (ppTrans->rootdesc->Tag, "pattern_root");
   /* warning : the Rule points the transformation record (no rule for the root node)*/
   ppTrans->rootdesc->Rule = (RuleDesc*) ppTrans;
   ppTrans->rootdesc->Optional = FALSE;
   ppTrans->rootdesc->ActiveSymb = TRUE;
   ppTrans->rootdesc->OptChild = FALSE;
   ppTrans->rootdesc->Children = ppTrans->First;
   ppTrans->rootdesc->Nexts = NULL;
   ppTrans->rootdesc->next = NULL;

   /* inserts the new transformation in the list of transformations */
   td=match_env.Transformations;
   match_env.nbTrans++;
   if (td == NULL)
      match_env.Transformations=ppTrans;
   else {
       while (td->next != NULL)
 	  td = td->next;
       td->next=ppTrans;
   }
   ppTrans = NULL;  
}

/* ---------------------------------------------------------------------- */
/* |	ppPutInBuffer	put character c in the input buffer.		| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void ppPutInBuffer (unsigned char c)
#else
static void ppPutInBuffer (c)
unsigned char	c;
#endif 
{
   int  len;

   if (c==':'||c==';'||c=='('||c==')'||c=='{'||c=='}'||c=='+'||
       c==','|| c=='|'||c=='>'||c=='<'||c=='.'||c=='!'||c=='?') {
      ppError = TRUE;
      errorMessage ("Invalid char");
   } else {
      /* put the character into the buffer if it is not an ignored char. */
      if ((int)c == 9)	/* HT */
         len = 8;		/* HT = 8 spaces */
      else
         len = 1;
      if (c != EOS) {
         if (ppLgBuffer+len >= MaxBufferLength) {
	    ppError = TRUE;
	    errorMessage ("Panic: buffer overflow");
	    ppLgBuffer = 0;
	 }
	 if (len == 1)
	    inputBuffer[ppLgBuffer++] = c;
	 else /* HT */
            do {
               inputBuffer[ppLgBuffer++] = SPACE;
               len --;
            } while (len > 0 );
      }	
      inputBuffer[ppLgBuffer] = EOS;
   }
}

/* ---------------------------------------------------------------------- */
/* |	Do_nothing	Do nothing.				       	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void Do_nothing (char c)
#else
static void Do_nothing (c)
char	c;
#endif 
{
}

#ifdef PPSTANDALONE
typedef void (*Proc) ();
#endif

/* some type definitions for the automaton */

typedef struct _Transition *PtrTransition;

typedef struct _Transition {		/* a transition of the automaton in   "executable" form */
	unsigned char	trigger;	/* the imput character that triggers
					   the transition */
	Proc		action;		/* the procedure to be called when
					   the transition occurs */
	State		newState;	/* the new state of the automaton
					   after the transition */
	PtrTransition	nextTransition;	/* next transition from the same
					   state */
} Transition;

typedef struct _StateDescr {		/* a state of the automaton */
	State		automatonState;	/* the state */
	PtrTransition	firstTransition;/* first transition from that state */
} StateDescr;

/* the automaton that drives the HTML parser */
#define MaxState 50
static StateDescr	automaton[MaxState];

typedef struct _sourceTransition {	/* a transition of the automaton in
					   "source" form */
	State		initState;	/* initial state of transition */
	char		trigger;	/* the imput character that triggers
					   the transition */
	Proc		transitionAction; /* the procedure to be called when
					     the transition occurs */
	State		newState;	/* final state of the transition */
} sourceTransition;

/* the automaton in "source" form */
static sourceTransition ppsourceAutomaton[] = {
/*
state	trigger	    action		new state
*/
/* state 0: */
{0,	':',	(Proc) EndNameTrans,	1},
{0,	'*',	(Proc) ppPutInBuffer,	0}, 
/*state 1 :*/
{1,	'S',	(Proc) Do_nothing,	1},
{1,	'?',	(Proc) Option,		1},
{1,	'(',	(Proc) BeginExp,	1},
{1,	'<',	(Proc) BeginOfTag,	4},
{1,	'*',	(Proc) ppPutInBuffer,	2},
	
/* state 2:  */
{2,	'|',	(Proc) EndChoice,	1},
{2,	',',	(Proc) EndPatNode,	1},
{2,	':',	(Proc) SymbName,	1},
{2,	'{',	(Proc) BeginChild,	1},
{2,	'+',	(Proc) IterationTag,	3},
{2,	')',	(Proc) EndExp,		3},
{2,	'}',	(Proc) EndChild,	3},
{2,	';',	(Proc) EndPattern,	10},
{2,	'*',	(Proc) ppPutInBuffer,	2},
/* state 3: */
{3,	'|',	(Proc) EndChoice,	1},
{3,	',',	(Proc) EndPatNode,	1},	
{3,	')',	(Proc) EndExp,		3},
{3,	'}',	(Proc) EndChild,	3},
{3,	'+',	(Proc) Iteration,	3},
{3,	';',	(Proc) EndPattern,	10},
  /* attributes in patterns */
/* state 4: a '<' has been read : reading tag name*/
{4,	'S',	(Proc) EndOfTagName,	6},	
{4,	'>',	(Proc) EndOfTagName,	2},
{4,	'*',	(Proc) ppPutInBuffer,	4},
/* state 5: expexting a space or an end tag */
{5,	'S',	(Proc) Do_nothing,	6},
{5,	'>',	(Proc) Do_nothing,	2},
/* state 6: reading an attribute name*/
{6,	'S',	(Proc) ppEndOfAttrName,	6},
{6,     '=',    (Proc) ppEndOfAttrName, 7},
{6,	'>',	(Proc) ppEndOfAttrName,	2},
{6,	'*',	(Proc) ppPutInBuffer,	6},
/* state 7: reading an attribute value */
{7,	'\"',	(Proc) ppStartOfAttrValue,8},
{7,	'\'',	(Proc) ppStartOfAttrValue,9},
{7,     'S',    (Proc) ppEndOfAttrValue,6},
{7,	'>',	(Proc) ppEndOfAttrValue,2},
{7,	'*',	(Proc) ppPutInBuffer,	7},
/* state 8: reading an attribute value between double quotes */
{8,     '\"',   (Proc) ppEndOfAttrValue,	5},
{8,     '*',    (Proc) ppPutInBuffer,	8},
/* state 9: reading an attribute value  between quotes */
{9,     '\'',    (Proc) ppEndOfAttrValue,5},
{9,     '*',    (Proc) ppPutInBuffer,	9},
  /* transformation rules */
/* state 10: */
{10,	'S',	(Proc) Do_nothing,	10},	
{10,	'{',	(Proc) BeginRules,	11},
/* state 11: reading the premisse*/
{11,	'S',	(Proc) Do_nothing,	11},	
{11,	'>',	(Proc) EndPremisse,	12},
{11,	'*',	(Proc) ppPutInBuffer,	11},
/* state 12: reading the position path*/
{12,	'S',	(Proc) Do_nothing,	12},
{12,	'<',	(Proc) BeginRuleTag,	-15},
{12,	':',	(Proc) EndOptNodes,	13},
{12,	';',	(Proc) EndRule,		14},
{12,	'.',	(Proc) EndNode,		12},
{12,	'*',	(Proc) ppPutInBuffer,	12},
/* state 13: reading the generated nodes*/
{13,	'S',	(Proc) Do_nothing,	13},
{13,	'<',	(Proc) BeginRuleTag,	-15},
{13,	';',	(Proc) EndRule,		14},
{13,	'.',	(Proc) EndNode,		12},
{13,	'*',	(Proc) ppPutInBuffer,	12},
/* state 14: waiting for the next rule or the end of rules*/
{14,	'S',	(Proc) Do_nothing,	14},
{14,	'}',	(Proc) EndTransformation,0},
{14,	'*',	(Proc) ppPutInBuffer,	11},

  /*sub automaton for tags  in transformation rules*/
/* state 15: a '<' has been read : reading tag name*/
{15,	'S',	(Proc) EndRuleTagName,	17},	
{15,	'>',	(Proc) EndRuleTagName,	-1},
{15,	'*',	(Proc) ppPutInBuffer,	15},
/* state 16: expexting a space or an end tag */
{16,	'S',	(Proc) Do_nothing,	17},
{16,	'>',	(Proc) Do_nothing,	-1},
/* state 17: reading an attribute name*/
{17,	'S',	(Proc) ppEndRuleAttrName,17},
{17,    '=',    (Proc) ppEndRuleAttrName,18},
{17,	'.',	(Proc) ppTransAttr,	21},
{17,	'>',	(Proc) ppEndRuleAttrName,-1},
{17,	'*',	(Proc) ppPutInBuffer,	17},
/* state 18: reading an attribute value */
{18,	'\"',	(Proc) ppStartOfAttrValue,19},
{18,	'\'',	(Proc) ppStartOfAttrValue,20},
{18,    'S',    (Proc) ppEndOfAttrValue,17},
{18,	'>',	(Proc) ppEndOfAttrValue,-1},
{18,	'.',	(Proc) ppTransAttrValue,21},
{18,	'*',	(Proc) ppPutInBuffer,	18},
/* state 19: reading an attribute value between double quotes */
{19,    '\"',   (Proc) ppEndOfAttrValue,	16},
{19,    '*',    (Proc) ppPutInBuffer,	19},
/* state 20: reading an attribute value  between quotes */
{20,    '\'',    (Proc) ppEndOfAttrValue,16},
{20,    '*',    (Proc) ppPutInBuffer,	20},
/* state 21: reading a transferred attribute name */
{21,	'S',	(Proc) ppEndTransAttr,	17},
{21,	'>',	(Proc) ppEndTransAttr,	-1},	
{21,    '*',    (Proc) ppPutInBuffer,	21},


/* st1ate 1000: fictious state. End of automaton table */
/* the next line must be the last one in the automaton declaration */
{1000,	'*',	(Proc) Do_nothing,	1000}
};

/* ---------------------------------------------------------------------- */
/* |	ppInitAutomaton	read the "source" form of the automaton and	| */
/* |			build the "executable" form.			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void ppInitAutomaton (void)
#else
static void ppInitAutomaton ()
#endif
{
   int            entry;
   State          theState;
   State          curState;
   PtrTransition  trans;
   PtrTransition  prevTrans;

   entry = 0;
   curState = 1000;
   prevTrans = NULL;
   do {
      trans = (PtrTransition) TtaGetMemory (sizeof (Transition));
      trans->nextTransition = NULL;
      theState = ppsourceAutomaton[entry].initState;
      if (theState < 1000) {
         trans->trigger = ppsourceAutomaton[entry].trigger;
	 trans->action = ppsourceAutomaton[entry].transitionAction;
	 trans->newState = ppsourceAutomaton[entry].newState;
	 if (trans->trigger == 'S')	/* any spacing character */
	    trans->trigger = SPACE;
	 if (trans->trigger == '*')	/* any character */
	    trans->trigger = EOS;
	 if (theState != curState) {
	    automaton[theState].automatonState = theState;
	    automaton[theState].firstTransition = trans;
	    curState = theState;
	 } else
	    prevTrans->nextTransition = trans;
	 prevTrans = trans;
	 entry++;
      }
   }
   while (theState < 1000);
}

/* ---------------------------------------------------------------------- */
/* |	TRANSparse	parses the transformation file infile and builds the	| */
/* |			equivalent matching environment.			| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void TRANSparse (FILE* infile)
#else
static void TRANSparse (infile)
FILE* infile;
#endif 
{
   unsigned char   charRead,oldcharRead;
   boolean         match;
   PtrTransition   trans;

   /* initialize automaton */ 	      /* parse the input file    */
   numberOfCharRead = 0;
   numberOfLinesRead = 1;
   currentState = 0;
   ppInitAutomaton ();
   charRead = EOS;

   /* read the file sequentially */
   do {
      /* read one character from the input file, if the character read */
      /* previously has already been processed */
      if (charRead == EOS)
	 charRead = getc (infile);
	 if (!feof (infile) && !ferror (infile)) {
	    if (charRead == '!' && numberOfCharRead == 0) {/*comment*/
	       do
	          charRead = getc (infile);
		  while (charRead != EOL&& !feof (infile) && !ferror (infile));
	       }
	    }
	    if (!feof (infile) && !ferror (infile)) {
		if (charRead == EOL) {
		   /* new line in file */
		   numberOfLinesRead++;
		   numberOfCharRead = 0;
		} else
		   numberOfCharRead ++;

	        /* Check the character read */
	        /* Ignore end of line , non printable*/
	       if (charRead < SPACE || (int) charRead >=254 || ((int) charRead >= 127 && (int) charRead <=159))
	          charRead = EOS;
	       if (charRead != EOS) {
	          /* a valid character has been read */
	  	  /* first transition of the automaton for the current state */
		  trans = automaton[currentState].firstTransition;
		  /* search a transition triggered by the character read */
		  while (trans) {
		     match = FALSE;
		     if (charRead == trans->trigger) /* the char is the trigger */
		        match = TRUE;
		     else if (trans->trigger == EOS) /* any char is a trigger */
		        match = TRUE;
		     else if (trans->trigger == SPACE) /* any space is a trigger */
		        if ((int) charRead == 9  || (int) charRead == 10 ||
			    (int) charRead == 12 || (int) charRead == 13) /* a delimiter has been read */
			   match = TRUE;
		           if (match) {
		              /* transition found. Activate the transition */
		   	      /* call the procedure associated with the transition */
			      NormalTransition = TRUE;
			      if (trans->action)
			         (*(trans->action)) (charRead);
			         if (NormalTransition) {
			            /* the input character has been processed */
			            charRead = EOS;
			            /* the procedure associated with the transition has not */
			            /* changed state explicitely */
			            /* change current automaton state */
			            if (trans->newState >= 0)
			               currentState = trans->newState;
			            else if (trans->newState == -1)
				       /* return form subautomaton */
				       currentState = returnState;
			            else {
				       /* calling a subautomaton */
				       returnState = currentState;
				       currentState = - trans->newState;
				    }
				 } else {/* an error has been found skipping the transformation */
			            oldcharRead = getc(infile);
			            if (!feof (infile) && !ferror (infile))
			               charRead = getc(infile);
			            while ((!feof(infile) && !ferror(infile))&&(charRead !='}'||oldcharRead!=';')) {
				       oldcharRead=charRead;
				       charRead = getc (infile);
				       while (!feof (infile) && !ferror (infile) && ((int) charRead == 9  || 
                                             (int) charRead == 10 ||(int) charRead == 12 || (int) charRead == 13))
				         charRead=getc(infile);
				    }
			            if (!feof(infile) && !ferror(infile)) {
				       if (ppTrans) {
				          fprintf (stderr, "skipping transformation %s\n", ppTrans->NameTrans);
				          freetrans (ppTrans); 
				          ppTrans = NULL;
				       } else
				          fprintf (stderr, "skipping transformation\n");
				       if(ppChoice!=NULL)
				       freechoice(ppChoice);
				       ppChoice=NULL;
				       ppError=FALSE;
				       currentState=0;
				       ppLgBuffer=0;
				       charRead=EOS; 
				       ppOptional=FALSE;
				       ppIterTag=FALSE;
				       ppAttr=NULL;
				       ppNode=NULL;
				       ppRule=NULL;
				       ppisnamed=FALSE;
				       strcpy(ppname,"");
				       OpStack[0] = EOS;
				       SymbStack[0] = NULL;
				       ChStack[0] = NULL;
				       SzStack = 1;
				    }
				 } /* done */
			         trans = NULL;
			   } else {
		              /* access next transition from the same state */
			      trans = trans->nextTransition;
			      if (trans == NULL)
			         charRead = EOS;
			   }
		  }
	       }
	    }
   }
   while (!feof(infile) && !ferror (infile) && !ppError);
   /* end of HTML file */
}



/* --------------------------------------------------------------------- */
#ifdef __STDC__
static void initpparse (void)
#else 
static void initpparse ()
#endif 
{
   TransDesc *td, *td2;

   /* liberation des anciens decripteurs de transformation */
   td = match_env.Transformations;
   while (td) {
      td2 = td->next;
      freetrans (td);
      td = td2;
   }
   match_env.nbTrans=0;
   match_env.maxdepth=0;
   match_env.Transformations=NULL;  
}


/* ---------------------------------------------------------------------- */
/* |	ppStartParser loads the file Directory/FileName for parsing	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
int ppStartParser (char* name)
#else 
int ppStartParser (name)
char* name;
#endif 
{
   FILE*infile = (FILE*) 0;
   char	  msg[200];
#ifndef PPSTANDALONE
   PathBuffer   DirBuffer, filename;
   struct stat* StatBuffer;
   int i, status;
   /* compose le nom du fichier a ouvrir avec le nom du directory */
   /* des schemas et le suffixe */
   strncpy (DirBuffer, SchemaPath, MAX_PATH);
   BuildFileName (name,"trans", DirBuffer, filename, &i);
 
   /* verifie la date du fichier */
   StatBuffer = (struct stat*) malloc (sizeof (struct stat));
   status = stat (filename, StatBuffer);
   if (status != -1) {
      if (StatBuffer->st_mtime == timeLastWrite) /* the file is unchanged, activing all the transformations */
	 SetTransValid(match_env.Transformations);
      else {/* the file HTML.trans has been touched, parsing it */
	 timeLastWrite = StatBuffer->st_mtime;
	 /* ouvre le fichier */
	 infile = fopen(filename, "r");
#else
         infile = fopen(name, "r");
#endif
         if (infile == 0) {
	    sprintf (msg, "Can't open file %s.trans", name);
	    errorMessage(msg);
	    ppError = TRUE;
	 } else {
	     initpparse();
	     ppError=FALSE;
	     ppOptional=FALSE;
	     ppIterTag=FALSE;
	     ppAttr=NULL;
	     ppNode=NULL;
	     ppRule=NULL;
	     ppisnamed=FALSE;
	     strcpy(ppname,"");
	     OpStack[0] = EOS;
	     SymbStack[0] = NULL;
	     ChStack[0] = NULL;
	     SzStack = 1;
	     inputBuffer [0] = EOS ; 	      /* initialize input buffer */
	     ppLgBuffer = 0 ;
	     TRANSparse(infile);
	     fclose(infile);
	 }
#ifndef PPSTANDALONE
      }
   }
   free (StatBuffer);
#endif   
  
  return !ppError;
}

    
#ifdef PPSTANDALONE
void main()
{
   TransDesc *td;
   SymbDesc *sd;
   ListSymb *ls;
   AttrDesc *pa;
   NodeDesc *nd;
   RuleDesc *pr;

   ppStartParser("test.trans");
   printf ("\n");
   td=match_env.Transformations;
   while (td) {
      printf ("transformation : %s\n", td->NameTrans);
      printf ("entrees : ");
      ls = td->First;
      while (ls) {
	 printf("%s, ", ls->symb->SName); 
	 ls = ls->next;
      }
      printf ("\n");
      sd = td->PatSymbs;
      while (sd) {
	 printf ("%s  ",s d->SName);
	 pa=sd->Attributes;
	 while (pa) {
	    printf ("%s=",pa->NameAttr);
	    if (pa->IsInt)
	       printf ("%d ,", pa->IntVal);
	    else
		printf ("%s ,",pa->TextVal);
	    pa=pa->next;
	 }
	 printf("-- ");
	 ls = sd->Nexts;
	 while (ls) {
	    if (ls->symb == NULL)
	       printf("end");
	    else
	       printf("%s, ",ls->symb->SName);
	    ls=ls->next;
	 }
	 printf ("%s ", sd->OptChild?"-?":"-");
	 ls = sd->Children;
	 while (ls) {
	    if (ls->symb == NULL)
	       printf("end");
	    else
	       printf("%s, ",ls->symb->SName);
	    ls = ls->next;
	 }
	 pr = sd->Rule;
	 if (pr) {
	    printf ("\n %s > ", pr->RName);
	    nd = pr->OptNodes;
	    while (nd) {
	       if (nd->Attributes)
		  printf ("<");
	       printf ("%s",nd->Tag);
	       pa = nd->Attributes;
	       while (pa) {
		  printf (" %s=", pa->NameAttr);
		  if (pa->IsTransf)
		     printf ("%s.%s", pa->AttrTag, pa->AttrAttr);
		     else if (pa->IsInt)
		        printf ("%d", pa->IntVal);
		     else
			printf("%s",pa->TextVal);
		     pa=pa->next;
		     if (pa) printf (" ");
	       }
	       if (nd->Attributes)
		  printf (">");
	       nd = nd->next;
	       if (nd) printf (".");
	       else printf (":");
	    }
	    nd = pr->NewNodes;
	    while (nd) {
	       if (nd->Attributes)
		  printf ("<");
	       printf ("%s",nd->Tag);
	       pa = nd->Attributes;
	       while (pa) {
		  printf (" %s=", pa->NameAttr);
	          if (pa->IsTransf)
		     printf ("%s.%s", pa->AttrTag, pa->AttrAttr);
	          else if (pa->IsInt)
		     printf ("%d", pa->IntVal) ;
	          else
		     printf ("%s", pa->TextVal);
	          pa=pa->next;
	          if (pa) printf (" ");
	       }
	       if (nd->Attributes)
		  printf (">");
	       nd = nd->next;
	       if (nd) printf (".");
	       else printf (";");
	    }
	 }
	 printf ("\n");
	 sd = sd->next;
      }
      printf ("\n");
      td = td->next;
   }
   printf("\n");
}
#endif
