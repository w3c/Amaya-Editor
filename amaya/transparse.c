/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Transformation language parser, see file HTML.trans for a 
 * description of the language				
 *
 * Author: S. Bonhomme			
 *
 */

/* Compiling this module with -DPPSTANDALONE generates the main program of  */
/* a converter which reads a .trans file and creates the trans tables     */
/* Without this option, it creates a function ppStartParser that parses a   */
/* trans file and displays the internal representation of transformations.  */
/* !!!!!! WARNING : It does not works due to lack of time for maintening */
/* this code !!!!!!! */ 

#define THOT_EXPORT extern
#include "amaya.h"
#include "fileaccess.h"
#include "fetchHTMLname.h"
#ifdef  PPSTANDALONE
#define  NAME_LENGTH 20
#include "parser.h"
#else
#include "trans.h"
#endif


extern CHAR_T*   SchemaPath;


/* pattern and rules internal representation */
typedef struct _parForest
  {
     strListSymb           *first;
     strListSymb           *last;
     ThotBool            optional;
     struct _parForest  *next;
  }
parForest;

typedef struct _parChoice
  {
     parForest          *forests;
     ThotBool            optional;
  }
parChoice;

static ThotBool     ppError;
static ThotBool     ppIsNamed;
static ThotBool     ppOptional;
static ThotBool     ppIterTag;
static ThotBool	    selRuleFlag;
static CHAR_T         ppName[20];
static parChoice   *ppChoice;	/* current forest descriptor */
static parForest   *ppForest;	/* cuurent forest descriptor */
static parChoice   *ppLastChoice;
static strTransDesc   *ppTrans;	/* current transformation descriptor */
static strTransSet    *ppTransSet; /* current transformation set */
static strSymbDesc    *ppSymb;	/* current pattern symbol descriptor */
static strAttrDesc    *ppAttr;	/* attribute descriptor */
static strNodeDesc    *ppNode;	/* node descriptor */
static strRuleDesc    *ppRule;	/* rule descriptor */
static parChoice   *choiceStack[MAX_STACK];
static CHAR_T         opStack[MAX_STACK];
static strSymbDesc    *symbolStack[MAX_STACK];
static int          sizeStack;
static int          patDepth;
static int          numberOfLinesRead;
static int          numberOfCharRead;
static ThotBool     normalTransition;

#define MaxBufferLength   1000
#define AllmostFullBuffer  700
static UCHAR_T inputBuffer[MaxBufferLength];
static int          ppLgBuffer = 0;	/* actual length of text in input buffer */
typedef int         State;	/* a state of the automaton */
static State        currentState;	/* current state of the automaton */
static State        returnState;	/* return state from subautomaton */

#include "fetchHTMLname_f.h"
#include "html2thot_f.h"
#include "transparse_f.h"
#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

/*----------------------------------------------------------------------
  Init all transformation of the transformation set to valid,
   sets the value  transSet.MaxDepth 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SetTransValid (strTransSet  * transSet)
#else
static void         SetTransValid (transSet)
strTransSet          *transSet;

#endif
{
   strTransDesc          *td;
   strSymbDesc           *sd;

   td = transSet->Transformations;
   while (td)
     {
	if (!(td->IsActiveTrans))
	  {
	     td->IsActiveTrans = TRUE;
	     if (td->PatDepth > transSet->MaxDepth)
		transSet->MaxDepth = td->PatDepth;
	     sd = td->Symbols;
	     while (sd)
	       {
		  sd->IsActiveSymb = TRUE;
		  sd = sd->Next;
	       }
	  }
	td = td->Next;
     }
}

/*----------------------------------------------------------------------
  Frees a transformtion descriptor
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         FreeTrans (strTransDesc * td)
#else
static void         FreeTrans (td)
strTransDesc          *td;

#endif
{
   strListSymb           *ls, *ls2;
   strSymbDesc           *sd, *sd2;
   strAttrDesc           *ad, *ad2;
   strRuleDesc           *rd, *rd2;
   strNodeDesc           *n, *n2;

   TtaFreeMemory (td->NameTrans);
   TtaFreeMemory (td->DestinationTag);
   ls = td->First;
   while (ls)
     {
	ls2 = ls->Next;
	TtaFreeMemory ( ls);
	ls = ls2;
     }
   if (td->RootDesc)
     {
	TtaFreeMemory (td->RootDesc->Tag);
	TtaFreeMemory ( td->RootDesc);
     }
   sd = td->Symbols;
   while (sd)
     {
	TtaFreeMemory (sd->SymbolName);
	TtaFreeMemory (sd->Tag);
	ls = sd->Children;
	while (ls)
	  {
	     ls2 = ls->Next;
	     TtaFreeMemory ( ls);
	     ls = ls2;
	  }
	ls = sd->Followings;
	while (ls)
	  {
	     ls2 = ls->Next;
	     TtaFreeMemory ( ls);
	     ls = ls2;
	  }
	ad = sd->Attributes;
	while (ad)
	  {
	     TtaFreeMemory (ad->NameAttr);
	     if (ad->IsTransf)
	       {
		  TtaFreeMemory (ad->AttrTag);
		  TtaFreeMemory (ad->AttrAttr);
	       }
	     else if (!ad->IsInt)
		TtaFreeMemory (ad->TextVal);

	     ad2 = ad->Next;
	     TtaFreeMemory ( ad);
	     ad = ad2;
	  }
	sd2 = sd->Next;
	TtaFreeMemory ( sd);
	sd = sd2;
     }
   rd = td->Rules;
   while (rd)
     {
	n = rd->OptionNodes;
	while (n)
	  {
	     ad = n->Attributes;
	     while (ad)
	       {
		  TtaFreeMemory (ad->NameAttr);
		  if (ad->IsTransf)
		    {
		       TtaFreeMemory (ad->AttrTag);
		       TtaFreeMemory (ad->AttrAttr);
		    }
		  else if (!ad->IsInt)
		    {
		       TtaFreeMemory (ad->TextVal);
		    }
		  ad2 = ad->Next;
		  TtaFreeMemory ( ad);
		  ad = ad2;
	       }
	     TtaFreeMemory (n->Tag);
	     n2 = n->Next;
	     TtaFreeMemory ( n);
	     n = n2;
	  }
	n = rd->NewNodes;
	while (n)
	  {
	     ad = n->Attributes;
	     while (ad)
	       {
		  TtaFreeMemory (ad->NameAttr);
		  if (ad->IsTransf)
		    {
		       TtaFreeMemory (ad->AttrTag);
		       TtaFreeMemory (ad->AttrAttr);
		    }
		  else if (!ad->IsInt)
		    {
		       TtaFreeMemory (ad->TextVal);
		    }
		  ad2 = ad->Next;
		  TtaFreeMemory ( ad);
		  ad = ad2;
	       }
	     TtaFreeMemory (n->Tag);
	     n2 = n->Next;
	     TtaFreeMemory ( n);
	     n = n2;
	  }
	rd2 = rd->Next;
	TtaFreeMemory ( rd);
	rd = rd2;
     }
   TtaFreeMemory ( td);
}


/*----------------------------------------------------------------------
  Frees a list descriptor
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void            FreeList (strListSymb * pl)
#else
void            FreeList (pl)
strListSymb           *pl;

#endif
{
   if (pl)
     {
	FreeList (pl->Next);
	TtaFreeMemory ( pl);
     }
}

/*----------------------------------------------------------------------
  Frees a forest descriptor
 ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FreeForest (parForest * pf)
#else
static void         FreeForest (pf)
parForest          *pf;

#endif
{
   if (pf)
     {
	if (pf == ppForest)
	   ppForest = NULL;
	FreeList (pf->first);
	FreeList (pf->last);
	FreeForest (pf->next);
	TtaFreeMemory ( pf);
     }
}

/*----------------------------------------------------------------------
  Frees a choice descriptor
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FreeChoice (parChoice * pc)
#else
static void         FreeChoice (pc)
parChoice          *pc;

#endif
{
   if (pc)
     {
	FreeForest (pc->forests);
	TtaFreeMemory ( pc);
     }
}

/*----------------------------------------------------------------------
   	ErrorMessage	print the error message msg on stderr.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ErrorMessage (USTRING msg)
#else
static void         ErrorMessage (msg)
USTRING     msg;

#endif
{
#ifndef PPSTANDALONE
   CHAR_T                numline[5];

   usprintf (numline, TEXT("%d"), numberOfLinesRead);
   TtaSetStatus (TransDoc, 1, TtaGetMessage (AMAYA, AM_TRANS_PARSE_ERROR), numline);
#else
   fprintf (stderr, "line %d, char %d: %s\n", numberOfLinesRead, numberOfCharRead, msg);
#endif
   normalTransition = FALSE;
}

/*----------------------------------------------------------------------
  adds the pattern symbol symb to the plist symbol list 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddSymbToList (strListSymb ** pList, strSymbDesc * symb)
#else
static void         AddSymbToList (pList, symb)
strListSymb          **pList;
strSymbDesc           *symb;

#endif
{
   ThotBool            isjok, isnull, found;
   strListSymb           *pl, *plnext;

   if (*pList == NULL)
     {
	*pList = (strListSymb *) TtaGetMemory (sizeof (strListSymb));
	(*pList)->Next = NULL;
	(*pList)->Symbol = symb;
     }
   else
     {
	isjok = FALSE;
	isnull = (symb == NULL);
	if (!isnull)
	   isjok = (!ustrcmp (symb->Tag, TEXT("*")));
	pl = *pList;
	found = ((isnull && pl->Symbol == NULL) || (!isnull && pl->Symbol == symb));
	if (pl->Next == NULL && !isnull && !found)
	  {
	     if ((isjok && pl->Symbol == NULL) ||
	     (!isjok && (pl->Symbol == NULL || !ustrcmp (pl->Symbol->Tag, TEXT("*")))))
	       {
		  *pList = (strListSymb *) TtaGetMemory (sizeof (strListSymb));
		  (*pList)->Next = pl;
		  (*pList)->Symbol = symb;
	       }
	     else
	       {
		  pl->Next = (strListSymb *) TtaGetMemory (sizeof (strListSymb));
		  pl->Next->Next = NULL;
		  pl->Next->Symbol = symb;
	       }
	  }
	else
	  {
	     while (!found && pl->Next &&
		    ((isjok || isnull) || (pl->Next->Symbol && ustrcmp (pl->Next->Symbol->Tag, TEXT("*"))))
		    && (isnull || pl->Next->Symbol != NULL))
	       {
		  found = ((isnull && pl->Symbol == NULL) ||
			   (!isnull && pl->Symbol == symb));
		  pl = pl->Next;
	       }
	     if (!found)
	       {
		  plnext = pl->Next;
		  pl->Next = (strListSymb *) TtaGetMemory (sizeof (strListSymb));
		  pl->Next->Next = plnext;
		  pl->Next->Symbol = symb;
	       }
	  }
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddTerminal (parChoice * pc)
#else
static void         AddTerminal (pc)
parChoice          *pc;

#endif
{
   strListSymb           *pl, *pl2;
   parForest          *pf;
   strSymbDesc           *ps;

   pf = pc->forests;
   while (pf)
     {
	pl = pf->last;
	while (pl)
	  {
	     ps = pl->Symbol;
	     if (ps)
	       {
		  pl2 = ps->Followings;
		  while (pl2 && pl2->Next)
		     pl2 = pl2->Next;
		  if (!pl2)
		    {
		       ps->Followings = (strListSymb *) TtaGetMemory (sizeof (strListSymb));
		       pl2 = ps->Followings;
		    }
		  else
		    {
		       pl2->Next = (strListSymb *) TtaGetMemory (sizeof (strListSymb));
		       pl2 = pl2->Next;
		    }
		  pl2->Next = NULL;
		  pl2->Symbol = NULL;
	       }
	     pl = pl->Next;
	  }
	pf = pf->next;
     }
}

/*----------------------------------------------------------------------
  processes the previously read symbol
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ProcessSymbol (void)
#else
static void         ProcessSymbol ()
#endif
{
   strSymbDesc     *sd;
   CHAR_T             msgBuffer[MaxBufferLength];
   SSchema	    schema;

   if (ppLgBuffer != 0)
     {		
       /* cr4eates a new symbol in the pattern internal representation */
	ppTrans->NbPatSymb++;
	ppSymb = (strSymbDesc *) TtaGetMemory (sizeof (strSymbDesc));
	ppSymb->SymbolName = TtaAllocString (NAME_LENGTH);
	ppSymb->Tag = TtaAllocString (NAME_LENGTH);
	ppSymb->Rule = NULL;
	ppSymb->Children = NULL;
	ppSymb->Followings = NULL;
	ppSymb->IsOptional = ppOptional;
	ppSymb->IsActiveSymb = TRUE;
	ppSymb->Attributes = NULL;
	ppSymb->Next = NULL;
	ppSymb->Depth = patDepth;
	if (ppIsNamed)
	  {
	     ustrcpy (ppSymb->SymbolName, ppName);
	     ustrcpy (ppName, TEXT(""));
	  }
	else
	   ustrcpy (ppSymb->SymbolName, inputBuffer);
	ustrcpy (ppSymb->Tag, inputBuffer);

	ppIsNamed = FALSE;
	ppLgBuffer = 0;
	ppOptional = FALSE;
     }
   if (ppSymb)
     {
       /* adds the new symbol to those of the pattern */
	sd = ppTrans->Symbols;
	if (!sd)
	   ppTrans->Symbols = ppSymb;
	else
	  {
	     while (sd->Next)
		sd = sd->Next;
	     sd->Next = ppSymb;
	  }
	schema = ppTransSet->Schema;
	if (ustrcmp (ppSymb->Tag, TEXT("*")) && (MapGI (ppSymb->Tag, &schema, 0) == -1))
	  {
	     ppError = TRUE;
	     usprintf (msgBuffer, TEXT("unknown element %s"), ppSymb->Tag);
	     ErrorMessage (msgBuffer);
	  }
     }
}

/*----------------------------------------------------------------------
  creates a new symbol descriptor and links it with the pattern representation
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         NewSymbol (void)
#else
static void         NewSymbol ()
#endif
{
   strSymbDesc           *ps;
   strListSymb           *pList;

   if (ppSymb)
     {
	if (ppForest->first == NULL)
	  {
	    /* the new symbol is the first and last of current forest */
	     ppForest->first = (strListSymb *) TtaGetMemory (sizeof (strListSymb));
	     ppForest->first->Next = NULL;
	     ppForest->first->Symbol = ppSymb;
	     ppForest->last = (strListSymb *) TtaGetMemory (sizeof (strListSymb));
	     ppForest->last->Next = NULL;
	     ppForest->last->Symbol = ppSymb;
	  }
	else
	  {
	    /* the new symbol is not the first of current forest */
	    /* it is added as a next possible symbol of each candidate to the last symbol of */
	       /* current forest */
	     pList = ppForest->last;
	     while (pList)
	       {
		  ps = pList->Symbol;
		  if (ps)
		    {
		       AddSymbToList (&ps->Followings, ppSymb);
		    }
		  pList = pList->Next;
	       }
	     if (ppForest->optional)
	       {
		 /* if all previous symbol of the forest are optional, adds the new symbol */
		 /*as a candidate for the first symbol of the current forest */
		 AddSymbToList (&ppForest->first, ppSymb);
		 ppForest->optional = ppSymb->IsOptional;
	       }
	     if (ppSymb->IsOptional)
	       {
		 /* if the symbol is optional, it is added to the list of last possible of the */
		 /* current forest */
		  AddSymbToList (&ppForest->last, ppSymb);
	       }
	     else
	       {
		 /* if the symbol is mandatory, it becomes the last possible of current forest */
		  FreeList (ppForest->last);
		  ppForest->last = (strListSymb *) TtaGetMemory (sizeof (strListSymb));
		  ppForest->last->Next = NULL;
		  ppForest->last->Symbol = ppSymb;
	       }
	  }
	if (ppIterTag)
	   AddSymbToList (&ppSymb->Followings, ppSymb);
	ppIterTag = FALSE;
     }
   ppSymb = NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SymbolName (UCHAR_T c)
#else
static void         SymbolName (c)
UCHAR_T       c;

#endif
{
   if (ppLgBuffer == 0)
     {
	ppError = TRUE;
	ErrorMessage (TEXT("missing tag name"));
     }
   ppIsNamed = TRUE;
   ustrcpy (ppName, inputBuffer);
   ppLgBuffer = 0;
}

/*----------------------------------------------------------------------
  a transformation name has been read, allocates a new trasformation descriptor
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndNameTrans (UCHAR_T c)
#else
static void         EndNameTrans (c)
UCHAR_T       c;

#endif
{

   if (ppLgBuffer != 0)
     {	/* allocates the descriptor    */
	patDepth = 0;
	ppTrans = (strTransDesc *) TtaGetMemory (sizeof (strTransDesc));
	ppTrans->NameTrans = TtaAllocString (NAME_LENGTH);
	ustrcpy (ppTrans->NameTrans, inputBuffer);
	ppTrans->NbPatSymb = 0;
	ppTrans->NbRules = 0;
	ppTrans->PatDepth = 0;
	ppTrans->First = NULL;
	ppTrans->RootDesc = NULL;
	ppTrans->Symbols = NULL;
	ppTrans->Rules = NULL;
	ppTrans->IsActiveTrans = TRUE;
	ppTrans->DestinationTag = NULL;
	ppTrans->Next = NULL;
	ppLgBuffer = 0;

	/* allocates new choice and forest descriptors */
	ppChoice = (parChoice *) TtaGetMemory (sizeof (parChoice));
	ppForest = (parForest *) TtaGetMemory (sizeof (parForest));
	ppChoice->forests = ppForest;
	ppChoice->optional = FALSE;
	ppForest->first = NULL;
	ppForest->last = NULL;
	ppForest->optional = FALSE;
	ppForest->next = NULL;
     }
   else
     {
	ppError = TRUE;
	ErrorMessage (TEXT("Missing Transformation Name"));
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Option (UCHAR_T c)
#else
static void         Option (c)
UCHAR_T       c;

#endif
{
   ppOptional = TRUE;
   if (ppForest->first == NULL)
      ppForest->optional = TRUE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BeginExp (UCHAR_T c)
#else
static void         BeginExp (c)
UCHAR_T       c;

#endif
{
   /*   ProcessSymbol(); */
   choiceStack[sizeStack] = ppChoice;
   opStack[sizeStack] = c;
   symbolStack[sizeStack++] = ppSymb;
   ppChoice = (parChoice *) TtaGetMemory (sizeof (parChoice));
   ppForest = (parForest *) TtaGetMemory (sizeof (parForest));
   ppChoice->forests = ppForest;
   ppChoice->optional = ppOptional;
   ppForest->first = NULL;
   ppForest->last = NULL;
   ppForest->optional = FALSE;
   ppForest->next = NULL;
   ppOptional = FALSE;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndExp (UCHAR_T c)
#else
static void         EndExp (c)
UCHAR_T       c;

#endif
{

   parForest          *pf;
   strSymbDesc           *ps;
   strListSymb           *pList, *pL3;

   ProcessSymbol ();
   NewSymbol ();
   if (sizeStack < 1 || opStack[sizeStack - 1] != '(')
     {
	ppError = TRUE;
	ErrorMessage (TEXT("mismatched parentheses"));
     }
   else
     {
	if (ppLastChoice)
	   FreeChoice (ppLastChoice);
	ppLastChoice = ppChoice;
	pf = ppChoice->forests;
	while (pf && !ppChoice->optional)
	  {
	     ppChoice->optional = ppChoice->optional || pf->optional;
	     pf = pf->next;
	  }
	/* pops the choice descriptor */
	ppChoice = choiceStack[sizeStack - 1];
	sizeStack--;
	ppForest = ppChoice->forests;
	/* for each alternative of the choice, adds the symbols of parsed expression to the descriptor */
	while (ppForest->next)
	   ppForest = ppForest->next;

	if (ppForest->first == NULL)
	  {
	     /* if the forest descriptor is empty, replaces nodes lists by those of parsed expression */
	     pf = ppLastChoice->forests;
	     while (pf)
	       {
		  pList = pf->first;
		  while (pList)
		    {
		       AddSymbToList (&ppForest->first, pList->Symbol);
		       pList = pList->Next;
		    }
		  pList = pf->last;
		  while (pList)
		    {
		       AddSymbToList (&ppForest->last, pList->Symbol);
		       pList = pList->Next;
		    }
		  pf = pf->next;
	       }
	     ppForest->optional = ppLastChoice->optional;
	  }
	else
	  {
	    /* the first symbol of parsed expressions are added to the list of first possible */
	    /* symbols of the current forest */
	     pList = ppForest->last;
	     while (pList)
	       {
		  ps = pList->Symbol;
		  if (ps)
		    {	
		       pf = ppLastChoice->forests;
		       while (pf)
			 {
			    pL3 = pf->first;
			    while (pL3)
			      {
				 AddSymbToList (&ps->Followings, pL3->Symbol);
				 pL3 = pL3->Next;
			      }
			    pf = pf->next;
			 }
		    }
		  pList = pList->Next;
	       }

	     if (ppForest->optional)
	       {
		 /* if all symbols are otional in current forest, the first possible symbol of parsed */
		 /* expression are added to the first possible of current forest descriptor */
		  pf = ppLastChoice->forests;
		  while (pf)
		    {
		       pList = pf->first;
		       while (pList)
			 {
			    AddSymbToList (&ppForest->first, pList->Symbol);
			    pList = pList->Next;
			 }
		       pf = pf->next;
		    }
	       }

	     if (!ppLastChoice->optional)
	       {
		  FreeList (ppForest->last);
		  ppForest->last = NULL;
	       }
	     /* adds the last possible of last parsed choice to the last pssoble of current forest descriptor */
	     pf = ppLastChoice->forests;
	     while (pf)
	       {
		  pList = pf->last;
		  while (pList)
		    {
		       AddSymbToList (&ppForest->last, pList->Symbol);
		       pList = pList->Next;
		    }
		  pf = pf->next;
	       }
	  }
	ppForest->optional = ppForest->optional && ppLastChoice->optional;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         IterationTag (UCHAR_T c)
#else
static void         IterationTag (c)
UCHAR_T       c;

#endif
{
   ppIterTag = TRUE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Iteration (UCHAR_T c)
#else
static void         Iteration (c)
UCHAR_T       c;

#endif
{
   parForest          *pf1, *pf2;
   strListSymb           *plfirst, *pllast;
   strSymbDesc           *ps;

   if (ppLastChoice)
     { /* adds the first symbols of last choice descriptor as next possible of its last symbols */
	pf1 = ppLastChoice->forests;
	while (pf1)
	  {
	     plfirst = pf1->first;
	     while (plfirst)
	       {
		  pf2 = ppLastChoice->forests;
		  while (pf2)
		    {
		       pllast = pf2->last;
		       while (pllast)
			 {
			    ps = pllast->Symbol;
			    if (ps)
			       AddSymbToList (&ps->Followings, plfirst->Symbol);
			    pllast = pllast->Next;
			 }
		       pf2 = pf2->next;
		    }
		  plfirst = plfirst->Next;
	       }
	     pf1 = pf1->next;
	  }
	FreeChoice (ppLastChoice);
	ppLastChoice = NULL;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BeginChild (UCHAR_T c)
#else
static void         BeginChild (c)
UCHAR_T       c;

#endif
{
   ProcessSymbol ();
   symbolStack[sizeStack] = ppSymb;
   NewSymbol ();
   opStack[sizeStack] = c;
   choiceStack[sizeStack++] = ppChoice;
   ppSymb = NULL;
   ppChoice = (parChoice *) TtaGetMemory (sizeof (parChoice));
   ppForest = (parForest *) TtaGetMemory (sizeof (parForest));
   ppChoice->forests = ppForest;
   ppChoice->optional = FALSE;
   ppForest->first = NULL;
   ppForest->last = NULL;
   ppForest->optional = FALSE;
   ppForest->next = NULL;
   ppOptional = FALSE;
   patDepth++;
}
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndChild (UCHAR_T c)
#else
static void         EndChild (c)
UCHAR_T       c;

#endif
{
   parChoice          *pc;
   parForest          *pf;
   strListSymb           *pl;

   if (sizeStack < 1 || opStack[sizeStack - 1] != '{')
     {
	ppError = TRUE;
	ErrorMessage (TEXT("mismatched parentheses"));
     }
   else
     {
	/* process the last read symbol */
	ProcessSymbol ();
	NewSymbol ();
	AddTerminal (ppChoice);
	/* check if the current depth is the maximal depth */
	if (patDepth > ppTransSet->MaxDepth)
	   ppTransSet->MaxDepth = patDepth;
	if (patDepth > ppTrans->PatDepth)
	   ppTrans->PatDepth = patDepth;
	patDepth--;

	pc = ppChoice;
	/* pops last context */
	ppChoice = choiceStack[sizeStack - 1];
	ppSymb = symbolStack[sizeStack - 1];
	sizeStack--;
	pf = pc->forests;
	while (pf && !pc->optional)
	  {
	     pc->optional = pc->optional || pf->optional;
	     pf = pf->next;
	  }
	ppSymb->IsOptChild = pc->optional;

	/* adds first symbols of child context as first children of parent context */
	pf = pc->forests;
	while (pf)
	  {
	     pl = pf->first;
	     while (pl)
	       {
		  AddSymbToList (&ppSymb->Children, pl->Symbol);
		  pl = pl->Next;
	       }
	     pf = pf->next;
	  }
	FreeChoice (pc);
	pf = ppChoice->forests;
	while (pf->next)
	   pf = pf->next;
	ppForest = pf;
	ppSymb = NULL;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndChoice (UCHAR_T c)
#else
static void         EndChoice (c)
UCHAR_T       c;

#endif
{
   ProcessSymbol ();
   NewSymbol ();
   ppForest->next = (parForest *) TtaGetMemory (sizeof (parForest));
   ppForest = ppForest->next;
   ppForest->first = NULL;
   ppForest->last = NULL;
   ppForest->optional = FALSE;
   ppForest->next = NULL;
   ppOptional = FALSE;
   if (ppLastChoice)
     {
	FreeChoice (ppLastChoice);
	ppLastChoice = NULL;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         EndPatNode (UCHAR_T c)
#else
static void         EndPatNode (c)
UCHAR_T       c;

#endif
{

   ProcessSymbol ();
   NewSymbol ();
   ppOptional = FALSE;
   if (ppLastChoice)
     {
	FreeChoice (ppLastChoice);
	ppLastChoice = NULL;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndPattern (UCHAR_T c)
#else
static void         EndPattern (c)
UCHAR_T       c;

#endif
{
   parForest          *pf;
   strListSymb           *pl, *pl2;

   ProcessSymbol ();
   NewSymbol ();
   if (sizeStack != 1)
     {
	ppError = TRUE;
	ErrorMessage (TEXT("Syntax error"));
     }
   else
     {
	AddTerminal (ppChoice);
	pl2 = ppTrans->First;
	pf = ppChoice->forests;
	while (pf)
	  {
	     pl = pf->first;
	     pf->first = NULL;
	     if (pl)
	       {
		  if (ppTrans->First == NULL)
		    {
		       ppTrans->First = pl;
		       pl2 = pl;
		    }
		  else
		    {
		       while (pl2->Next)
			  pl2 = pl2->Next;
		       pl2->Next = pl;
		    }
	       }
	     pf = pf->next;
	  }
     }
   FreeChoice (ppChoice);
   ppChoice = NULL;
   ppForest = NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BeginOfTag (UCHAR_T c)
#else
static void         BeginOfTag (c)
UCHAR_T       c;

#endif
{
   if (ppLgBuffer != 0)
     {
	ppError = TRUE;
	ErrorMessage (TEXT("Syntax Error"));
     }
   else
     {
	ppSymb = (strSymbDesc *) TtaGetMemory (sizeof (strSymbDesc));
	ppSymb->SymbolName = TtaAllocString (NAME_LENGTH);
	ppSymb->Tag = TtaAllocString (NAME_LENGTH);
	ustrcpy (ppSymb->SymbolName, TEXT(""));
	ustrcpy (ppSymb->Tag, TEXT(""));
	ppSymb->Rule = NULL;
	ppSymb->Children = NULL;
	ppSymb->Followings = NULL;
	ppSymb->IsOptional = ppOptional;
	ppSymb->IsActiveSymb = TRUE;
	ppSymb->Attributes = NULL;
	ppSymb->Next = NULL;
	if (ppIsNamed)
	  {
	     ustrcpy (ppSymb->SymbolName, ppName);
	     ustrcpy (ppName, TEXT(""));
	  }
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BeginRuleTag (UCHAR_T c)
#else
static void         BeginRuleTag (c)
UCHAR_T       c;

#endif
{
   if (ppLgBuffer != 0)
     {
	ppError = TRUE;
	ErrorMessage (TEXT("Syntax Error"));
     }
   else
     {
	ustrcpy (ppNode->Tag, TEXT(""));
	ppNode->Attributes = NULL;
	ppNode->Next = NULL;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOfTagName (UCHAR_T c)
#else
static void         EndOfTagName (c)
UCHAR_T       c;

#endif
{
   if (ppLgBuffer != 0)
     {
	ustrcpy (ppSymb->Tag, inputBuffer);
	if (!ppIsNamed)
	   ustrcpy (ppSymb->SymbolName, inputBuffer);
	ppLgBuffer = 0;
	ppIsNamed = FALSE;
     }
   else if (!ustrcmp (ppSymb->Tag, TEXT("")))
     {
	ppError = TRUE;
	ErrorMessage (TEXT("Missing Tag Name"));
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndRuleTagName (UCHAR_T c)
#else
static void         EndRuleTagName (c)
UCHAR_T       c;

#endif
{
   if (ppLgBuffer != 0)
     {
	ustrcpy (ppNode->Tag, inputBuffer);
	ppLgBuffer = 0;
     }
   else if (!ustrcmp (ppNode->Tag, TEXT("")))
     {
	ppError = TRUE;
	ErrorMessage (TEXT("Missing Tag Name"));
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ppEndOfAttrName (UCHAR_T c)
#else
static void         ppEndOfAttrName (c)
UCHAR_T       c;

#endif
{
   int                 ThotAttrNum;
   CHAR_T                msgBuffer[MaxBufferLength];

   if (ppLgBuffer != 0)
     {
#ifndef PPSTANDALONE
	ThotAttrNum = MapThotAttr (inputBuffer, ppSymb->Tag);
	if (ThotAttrNum != -1)
	  {
#endif
	     ppAttr = ppSymb->Attributes;
	     if (!ppAttr)
	       {
		  ppSymb->Attributes = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
		  ppAttr = ppSymb->Attributes;
		  ppAttr->NameAttr = TtaAllocString (NAME_LENGTH);
		  ppAttr->Next = NULL;
	       }
	     else
	       {
		  while (ppAttr->Next && ustrcmp (ppAttr->NameAttr, inputBuffer))
		     ppAttr = ppAttr->Next;
		  if (!ustrcmp (ppAttr->NameAttr, inputBuffer))
		    {
		       ppError = TRUE;
		       ErrorMessage (TEXT("Multi valued attribute"));
		    }
		  else
		    {
		       ppAttr->Next = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
		       ppAttr = ppAttr->Next;
		       ppAttr->NameAttr = TtaAllocString (NAME_LENGTH);
		       ppAttr->Next = NULL;
		    }
	       }
	     ustrcpy (ppAttr->NameAttr, inputBuffer);
	     ppAttr->ThotAttr = ThotAttrNum;
	     ppAttr->IsInt = TRUE;
	     ppAttr->IsTransf = FALSE;
	     ppAttr->IntVal = 0;
#ifndef PPSTANDALONE
	  }
	else
	  {
	     ppError = TRUE;
	     usprintf (msgBuffer, TEXT("unknown attribute %s"), inputBuffer);
	     ErrorMessage (msgBuffer);
	  }
#endif
	ppLgBuffer = 0;
     }
   else
     {
	ppError = TRUE;
	ErrorMessage (TEXT("Missing Attribute Name"));
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ppEndRuleAttrName (UCHAR_T c)
#else
static void         ppEndRuleAttrName (c)
UCHAR_T       c;

#endif
{
   int                 ThotAttrNum;
   CHAR_T                msgBuffer[MaxBufferLength];

   if (ppLgBuffer != 0)
     {
#ifndef PPSTANDALONE
       if (ustrcmp (ppNode->Tag, TEXT("*")) == 0)
	 ThotAttrNum = MapThotAttr (inputBuffer, TEXT(""));
       else
	 ThotAttrNum = MapThotAttr (inputBuffer, ppNode->Tag);
	if (ThotAttrNum != -1)
	  {
#endif
	     ppAttr = ppNode->Attributes;
	     if (!ppAttr)
	       {
		  ppNode->Attributes = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
		  ppAttr = ppNode->Attributes;
		  ppAttr->NameAttr = TtaAllocString (NAME_LENGTH);
		  ppAttr->Next = NULL;
	       }
	     else
	       {
		  while (ppAttr->Next && ustrcmp (ppAttr->NameAttr, inputBuffer))
		     ppAttr = ppAttr->Next;
		  if (!ustrcmp (ppAttr->NameAttr, inputBuffer))
		    {
		       ppError = TRUE;
		       ErrorMessage (TEXT("Multi valued attribute"));
		    }
		  else
		    {
		       ppAttr->Next = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
		       ppAttr = ppAttr->Next;
		       ppAttr->NameAttr = TtaAllocString (NAME_LENGTH);
		       ppAttr->Next = NULL;
		    }
	       }
	     ustrcpy (ppAttr->NameAttr, inputBuffer);
	     ppAttr->ThotAttr = ThotAttrNum;
	     ppAttr->IsInt = TRUE;
	     ppAttr->IsTransf = FALSE;
	     ppAttr->IntVal = 0;
#ifndef PPSTANDALONE
	  }
	else
	  {
	     ppError = TRUE;
	     usprintf (msgBuffer, TEXT("unknown attribute %s"), inputBuffer);
	     ErrorMessage (msgBuffer);
	  }
#endif
	ppLgBuffer = 0;
     }
   else
     {
	ppError = TRUE;
	ErrorMessage (TEXT("Missing Attribute Name"));
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ppTransAttr (UCHAR_T c)
#else
static void         ppTransAttr (c)
UCHAR_T       c;

#endif
{
   int                 ThotAttrNum;

   if (ppLgBuffer != 0)
     {
#ifndef PPSTANDALONE
	ThotAttrNum = MapThotAttr (inputBuffer, ppNode->Tag);
	if (ThotAttrNum != -1)
	  {
#endif
	     ppAttr = ppNode->Attributes;
	     if (!ppAttr)
	       {
		  ppNode->Attributes = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
		  ppAttr = ppNode->Attributes;
		  ppAttr->NameAttr = TtaAllocString (NAME_LENGTH);
		  ppAttr->Next = NULL;
	       }
	     else
	       {
		  while (ppAttr->Next && ustrcmp (ppAttr->NameAttr, inputBuffer))
		     ppAttr = ppAttr->Next;
		  if (!ustrcmp (ppAttr->NameAttr, inputBuffer))
		    {
		       ppError = TRUE;
		       ErrorMessage (TEXT("Multi valued attribute"));
		    }
		  else
		    {
		       ppAttr->Next = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
		       ppAttr = ppAttr->Next;
		       ppAttr->NameAttr = TtaAllocString (NAME_LENGTH);
		       ppAttr->Next = NULL;
		    }
	       }
	     ppAttr->AttrTag = TtaAllocString (NAME_LENGTH);
	     ustrcpy (ppAttr->AttrTag, inputBuffer);
	     ustrcpy (ppAttr->NameAttr, TEXT(""));
	     ppAttr->ThotAttr = ThotAttrNum;
	     ppAttr->IsInt = FALSE;
	     ppAttr->IsTransf = TRUE;
#ifndef PPSTANDALONE
	  }
	else
	  {
	     ppError = TRUE;
	     ErrorMessage (TEXT("Unknown attribute"));
	  }
#endif
	ppLgBuffer = 0;
     }
   else
     {
	ppError = TRUE;
	ErrorMessage (TEXT("Missing Attribute Name"));
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ppTransAttrValue (UCHAR_T c)
#else
static void         ppTransAttrValue (c)
UCHAR_T       c;

#endif
{
   if (ppLgBuffer != 0)
     {
	ppAttr->IsTransf = TRUE;
	ppAttr->AttrTag = TtaAllocString (NAME_LENGTH);
	ustrcpy (ppAttr->AttrTag, inputBuffer);
	ppLgBuffer = 0;
     }
   else
     {
	ppError = TRUE;
	ErrorMessage (TEXT("Missing Tag Name"));
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ppEndTransAttr (UCHAR_T c)
#else
static void         ppEndTransAttr (c)
UCHAR_T       c;

#endif
{
   if (ppLgBuffer != 0)
     {
	ppAttr->AttrAttr = TtaAllocString (NAME_LENGTH);
	ustrcpy (ppAttr->AttrAttr, inputBuffer);
	if (!ustrcmp (ppAttr->NameAttr, TEXT("")))
	  {
	     ustrcpy (ppAttr->NameAttr, inputBuffer);
	  }
	ppLgBuffer = 0;
     }
   else
     {
	ppError = TRUE;
	ErrorMessage (TEXT("Missing Attribute Name"));
     }
}

/*----------------------------------------------------------------------
   	ppPutInBuffer	put character c in the input buffer.		
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ppPutInBuffer (UCHAR_T c)
#else
static void         ppPutInBuffer (c)
UCHAR_T       c;

#endif
{
   int                 len;

   if ((ppLgBuffer == 0 || inputBuffer[0] != '\"') &&
       (c == ':' || c == ';' || c == '(' || c == ')' || c == '{' || 
	c == '}' || c == '+' || c == ',' || c == '|' || c == '>' || 
	c == '<' || c == '.' || c == '!' || c == '?'))
     {
       ppError = TRUE;
       ErrorMessage (TEXT("Invalid char"));
     }
   else
     {
	/* put the character into the buffer if it is not an ignored char. */
	if ((int) c == 9)	/* HT */
	   len = 8;		/* HT = 8 spaces */
	else
	   len = 1;
	if (c != EOS)
	  {
	     if (ppLgBuffer + len >= MaxBufferLength)
	       {
		  ppError = TRUE;
		  ErrorMessage (TEXT("Panic: buffer overflow"));
		  ppLgBuffer = 0;
	       }
	     if (len == 1)
		inputBuffer[ppLgBuffer++] = c;
	     else		/* HT */
		do
		  {
		     inputBuffer[ppLgBuffer++] = SPACE;
		     len--;
		  }
		while (len > 0);
	  }
	inputBuffer[ppLgBuffer] = EOS;
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ppStartOfAttrValue (UCHAR_T c)
#else
static void         ppStartOfAttrValue (c)
UCHAR_T       c;

#endif
{
  if (c == '\"' || c == '\'')
    ppPutInBuffer ('\"');
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ppEndOfAttrValue (UCHAR_T c)
#else
static void         ppEndOfAttrValue (c)
UCHAR_T       c;

#endif
{
   ThotBool            isText;
   int                 i, AttrVal;

   if (ppLgBuffer == 0)
     {
	ppAttr->IsInt = FALSE;
	ppAttr->TextVal = TtaAllocString (ppLgBuffer + 2);

	ustrcpy (ppAttr->TextVal, TEXT(""));
     }
   else
     {
	isText = FALSE;
	for (i = 0; !isText && i < ppLgBuffer; i++)
	   isText = (inputBuffer[i] < '1' || inputBuffer[i] > '9');
	if (!isText)
	  {
	     ppAttr->IsInt = TRUE;
	     ppAttr->IntVal = uctoi (inputBuffer);
	  }
	else
	  {
#ifdef PPSTANDALONE
	     AttrVal = -1;
#else
	     AttrVal = MapAttrValue (ppAttr->ThotAttr, inputBuffer);
#endif
	     if (AttrVal != -1)
	       {
		  ppAttr->IsInt = TRUE;
		  ppAttr->IntVal = AttrVal;
	       }
	     else
	       {
		  ppAttr->TextVal = TtaAllocString (ppLgBuffer + 2);
		  if (inputBuffer [0] == '\"')
		    ustrcpy (ppAttr->TextVal, &(inputBuffer[1]));
		  else
		    ustrcpy (ppAttr->TextVal, inputBuffer);
		  ppAttr->IsInt = FALSE;
	       }
	  }
	ppLgBuffer = 0;
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BeginRules (UCHAR_T c)
#else
static void         BeginRules (c)
UCHAR_T       c;

#endif
{
  selRuleFlag = FALSE;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndLeftPartRule (UCHAR_T c)
#else
static void         EndLeftPartRule (c)
UCHAR_T       c;

#endif
{
   if (ppLgBuffer != 0)
     {				/* allocates a new rule descriptor */
	ppRule = (strRuleDesc *) TtaGetMemory (sizeof (strRuleDesc));
	ppRule->RuleName = TtaAllocString (20);
	ustrcpy (ppRule->RuleName, inputBuffer);
	ppRule->NextRule = NULL;
	ppRule->Next = NULL;
	ppRule->OptionNodes = (strNodeDesc *) TtaGetMemory (sizeof (strNodeDesc));
	ppRule->DeleteRule = FALSE;
	ppRule->NewNodes = NULL;
	ppNode = ppRule->OptionNodes;
	ppNode->Tag = TtaAllocString (NAME_LENGTH);
	ustrcpy (ppNode->Tag, TEXT(""));
	ppNode->Attributes = NULL;
	ppNode->Next = NULL;
	ppLgBuffer = 0;
     }
   else
     {
	ppError = TRUE;
	ErrorMessage (TEXT("Missing left part of rule"));
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DeleteElementRule(UCHAR_T c)
#else
static void          DeleteElementRule (c)
UCHAR_T       c;

#endif
{
  if (ppLgBuffer != 0)
    {				/* allocates a new rule descriptor */
      ppRule = (strRuleDesc *) TtaGetMemory (sizeof (strRuleDesc));
      ppRule->RuleName = TtaAllocString (20);
      ustrcpy (ppRule->RuleName, inputBuffer);
      ppRule->NextRule = NULL;
      ppRule->Next = NULL;
      ppRule->NewNodes = NULL;
      ppRule->OptionNodes = NULL;
      ppLgBuffer = 0;
      ppRule->DeleteRule = TRUE;
      ppLgBuffer = 0;
    }
  else
    {
      ppError = TRUE;
      ErrorMessage (TEXT("Missing rule name"));
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndNode (UCHAR_T c)
#else
static void         EndNode (c)
UCHAR_T       c;

#endif
{
   CHAR_T                msgBuffer[MaxBufferLength];
   SSchema	       schema;

   if (ppLgBuffer != 0)
     {
	ustrcpy (ppNode->Tag, inputBuffer);
	ppLgBuffer = 0;
	schema = ppTransSet->Schema;
	if (MapGI (ppNode->Tag, &schema, 0) == -1)
	  {
	     ppError = TRUE;
	     usprintf (msgBuffer, TEXT("unknown tag </%s>"), ppNode->Tag);
	     ErrorMessage (msgBuffer);
	  }
     }
   if (ppNode && c == '.')
     {				/* allocate the next node descriptor */
	ppNode->Next = (strNodeDesc *) TtaGetMemory (sizeof (strNodeDesc));
	ppNode = ppNode->Next;
	ppNode->Tag = TtaAllocString (NAME_LENGTH);
	ustrcpy (ppNode->Tag, TEXT(""));
	ppNode->Attributes = NULL;
	ppNode->Next = NULL;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndOptNodes (UCHAR_T c)
#else
static void         EndOptNodes (c)
UCHAR_T       c;
#endif
{
   strAttrDesc		*ad, *ad2;
   CHAR_T			msgBuffer[MaxBufferLength];
   SSchema		schema;

   if (ppLgBuffer != 0)
     {
	ustrcpy (ppNode->Tag, inputBuffer);
	ppLgBuffer = 0;
	schema = ppTransSet->Schema;
	if (MapGI (ppNode->Tag, &schema, 0) == -1)
	  {
	     ppError = TRUE;
	     usprintf (msgBuffer, TEXT("unknown tag </%s>"), ppNode->Tag);
	     ErrorMessage (msgBuffer);
	  }
     }
   if (!ustrcmp (ppRule->OptionNodes->Tag, TEXT("")))
     {
	/* frees the current node descriptor if it is empty (the rule has no opt. node */
	ad = ppRule->OptionNodes->Attributes;
	while (ad)
	  {
	     TtaFreeMemory (ad->NameAttr);
	     if (ad->IsTransf)
	       {
		  TtaFreeMemory (ad->AttrTag);
		  TtaFreeMemory (ad->AttrAttr);
	       }
	     else if (!ad->IsInt)
		TtaFreeMemory (ad->TextVal);
	     ad2 = ad->Next;
	     TtaFreeMemory ( ad);
	     ad = ad2;
	  }
	TtaFreeMemory (ppRule->OptionNodes->Tag);
	TtaFreeMemory ( ppRule->OptionNodes);
	ppRule->OptionNodes = NULL;
     }
   /* allocate a New node descriptor */
   ppRule->NewNodes = (strNodeDesc *) TtaGetMemory (sizeof (strNodeDesc));
   ppNode = ppRule->NewNodes;
   ppNode->Tag = TtaAllocString (NAME_LENGTH);
   ustrcpy (ppNode->Tag, TEXT(""));
   ppNode->Attributes = NULL;
   ppNode->Next = NULL;
}
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         SelectionRule (UCHAR_T c)
#else
static void         SelectionRule (c)
UCHAR_T       c;
#endif
{
  CHAR_T                   msgBuffer[MaxBufferLength];
  strAttrDesc		*ppAttr;

  if (selRuleFlag)
    {
      usprintf (msgBuffer, TEXT("Too much selection rules"));
      ErrorMessage (msgBuffer);
    }
  else
    {
      selRuleFlag = TRUE;
      if (ppNode != NULL)
	{
	  ppAttr = ppNode->Attributes;
	  if (ppAttr == NULL)
	    {
	      ppNode->Attributes = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
	      ppAttr = ppNode->Attributes;
	      ppAttr->NameAttr = TtaAllocString (NAME_LENGTH);
	      ppAttr->Next = NULL;
	    }
	  else
	    {
	      while (ppAttr->Next && ustrcmp (ppAttr->NameAttr, inputBuffer))
		ppAttr = ppAttr->Next;
	      if (!ustrcmp (ppAttr->NameAttr, inputBuffer))
		{
		  ppError = TRUE;
		  ErrorMessage (TEXT("Multi valued attribute"));
		}
	      else
		{
		  ppAttr->Next = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
		  ppAttr = ppAttr->Next;
		  ppAttr->NameAttr = TtaAllocString (NAME_LENGTH);
		  ppAttr->Next = NULL;
		}
	    }
	  ustrcpy (ppAttr->NameAttr, TEXT("zzghost"));
	  ppAttr->ThotAttr = HTML_ATTR_Ghost_restruct;
	  ppAttr->IsInt = FALSE;
	  ppAttr->IsTransf = FALSE;
	  ppAttr->TextVal = TtaAllocString (NAME_LENGTH);
    	  ustrcpy (ppAttr->TextVal, TEXT("Select"));
	}
    }
}
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndRule (UCHAR_T c)
#else
static void         EndRule (c)
UCHAR_T       c;

#endif
{
  ThotBool               ok;
  strRuleDesc           *prule;
  strSymbDesc           *psymb;
  strNodeDesc           *pnode;
  strAttrDesc           *ad, *ad2;
  CHAR_T                   msgBuffer[MaxBufferLength];
  SSchema		  schema;

  if (ppLgBuffer != 0)
    {
      ustrcpy (ppNode->Tag, inputBuffer);
      ppLgBuffer = 0;
      schema = ppTransSet->Schema;
      if (ustrcmp (ppNode->Tag, TEXT("*")) && 
	  ustrcmp (ppNode->Tag, TEXT("#")) &&
	  ppNode->Tag[0] != '\"' &&
	  (MapGI (ppNode->Tag, &schema, 0) == -1))
	{
	  ppError = TRUE;
	  usprintf (msgBuffer, TEXT("unknown tag </%s>"), ppNode->Tag);
	  ErrorMessage (msgBuffer);
	}
    }
  if (ppRule->OptionNodes && !ustrcmp (ppRule->OptionNodes->Tag, TEXT("")))
    {				/* free the last Option node if it is empty */
      ad = ppRule->OptionNodes->Attributes;
      while (ad)
	{
	  TtaFreeMemory (ad->NameAttr);
	  if (ad->IsTransf)
	    {
	      TtaFreeMemory (ad->AttrTag);
	      TtaFreeMemory (ad->AttrAttr);
	    }
	  else if (!ad->IsInt)
	    TtaFreeMemory (ad->TextVal);
	  ad2 = ad->Next;
	  TtaFreeMemory ( ad);
	  ad = ad2;
	}
      TtaFreeMemory (ppRule->OptionNodes->Tag);
      TtaFreeMemory ( ppRule->OptionNodes);
      ppRule->NewNodes = NULL;
    }

  if (ppRule->NewNodes && !ustrcmp (ppRule->NewNodes->Tag, TEXT("")))
    {				/* free the last New node if it is empty */
      ad = ppRule->NewNodes->Attributes;
      while (ad)
	{
	  TtaFreeMemory (ad->NameAttr);
	  if (ad->IsTransf)
	    {
	      TtaFreeMemory (ad->AttrTag);
	      TtaFreeMemory (ad->AttrAttr);
	    }
	  else if (!ad->IsInt)
	    TtaFreeMemory (ad->TextVal);
	  ad2 = ad->Next;
	  TtaFreeMemory ( ad);
	  ad = ad2;
	}
      TtaFreeMemory (ppRule->NewNodes->Tag);
      TtaFreeMemory ( ppRule->NewNodes);
      ppRule->NewNodes = NULL;
    }
  ppNode = NULL;

  /* insert the rule descriptor in the transformation descriptor */
  prule = ppTrans->Rules;
  if (prule == NULL)
    ppTrans->Rules = ppRule;
  else
    {
      while (prule->Next)
	prule = prule->Next;
      prule->Next = ppRule;
    }
  ppTrans->NbRules++;

  /* link the symbols of the pattern to the current rule */
  psymb = ppTrans->Symbols;
  ok = FALSE;
  while (psymb)
    {
      if (!ustrcmp (ppRule->RuleName, psymb->SymbolName))
	{
	  prule = psymb->Rule;
	  if (prule == NULL)
	    psymb->Rule = ppRule;
	  else
	    {
	      while (prule->NextRule)
		prule = prule->NextRule;
	      prule->NextRule = ppRule;
	    }
	  ok = TRUE;
	}
      psymb = psymb->Next;
    }

  if (ok)
    {				/* the rule is linked to 1 symbol at least */
      /* check its consistence with the destination type of the current transformation */
      if (ppRule->OptionNodes != NULL)
	pnode = ppRule->OptionNodes;
      else
	pnode = ppRule->NewNodes;
      if (pnode && ustrcmp (pnode->Tag, TEXT("*")) != 0)
	if (ppTrans->DestinationTag == NULL)
	  {
	    /* the dest type is undefined => the first tag of the rule defines */
	    /* the destination type of the transformation */
	    ppTrans->DestinationTag = TtaAllocString (NAME_LENGTH);
	    ustrcpy (ppTrans->DestinationTag, pnode->Tag);
	  }
	else if (ustrcmp (ppTrans->DestinationTag, pnode->Tag))
	  /* the first tag of the rule is different from the destination type */
	  /* the rule has no destination type */
	  ustrcpy (ppTrans->DestinationTag, TEXT(""));
    }
  else
    {
      ppError = TRUE;
      ErrorMessage (TEXT("undefined pattern symbol"));
    }
  ppRule = NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         EndTransformation (UCHAR_T c)
#else
static void         EndTransformation (c)
UCHAR_T       c;

#endif
{
   strTransDesc          *td;

   /* create the pattern virtual root node */
   ppTrans->RootDesc = (strSymbDesc *) TtaGetMemory (sizeof (strSymbDesc));
   ppTrans->RootDesc->SymbolName = ppTrans->NameTrans;
   ppTrans->RootDesc->Tag = TtaAllocString (NAME_LENGTH);
   ustrcpy (ppTrans->RootDesc->Tag, TEXT("pattern_root"));
   /* warning : the Rule points the transformation record (no rule for the root node) */
   ppTrans->RootDesc->Rule = (strRuleDesc *) ppTrans;
   ppTrans->RootDesc->IsOptional = FALSE;
   ppTrans->RootDesc->IsActiveSymb = TRUE;
   ppTrans->RootDesc->IsOptChild = FALSE;
   ppTrans->RootDesc->Children = ppTrans->First;
   ppTrans->RootDesc->Followings = NULL;
   ppTrans->RootDesc->Next = NULL;

   /* inserts the new transformation in the list of transformations */
   td = ppTransSet->Transformations;
   ppTransSet->NbTrans++;
   if (td == NULL)
      ppTransSet->Transformations = ppTrans;
   else
     {
	while (td->Next != NULL)
	   td = td->Next;
	td->Next = ppTrans;
     }
   ppTrans = NULL;
}

/*----------------------------------------------------------------------
   	Do_nothing	Do nothing.				       	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         Do_nothing (CHAR_T c)
#else
static void         Do_nothing (c)
CHAR_T                c;

#endif
{
}

#ifdef PPSTANDALONE
typedef void        (*Proc) ();

#endif

/* some type definitions for the automaton */

typedef struct _Transition *PtrTransition;

typedef struct _Transition
  {				/* a transition of the automaton in   "executable" form */
     char       trigger;	/* the imput character that triggers
					   the transition */
     Proc                action;	/* the procedure to be called when
					   the transition occurs */
     State               newState;	/* the new state of the automaton
					   after the transition */
     PtrTransition       nextTransition;	/* next transition from the same
						   state */
  }
Transition;

typedef struct _StateDescr
  {				/* a state of the automaton */
     State               automatonState;	/* the state */
     PtrTransition       firstTransition;	/* first transition from that state */
  }
StateDescr;

/* the automaton that drives the HTML parser */
#define MaxState 50
static StateDescr   automaton[MaxState];

typedef struct _sourceTransition
  {				/* a transition of the automaton in
				   "source" form */
     State               initState;	/* initial state of transition */
     char                trigger;	/* the imput character that triggers
					   the transition */
     Proc                transitionAction;	/* the procedure to be called when
						   the transition occurs */
     State               newState;	/* final state of the transition */
  }
sourceTransition;

/* the automaton in "source" form */
static sourceTransition ppsourceAutomaton[] =
{
/*
   state        trigger     action              new state
 */
/* state 0: */
   {0, ':', (Proc) EndNameTrans, 1},
   {0, '*', (Proc) ppPutInBuffer, 0},
/*state 1 : */
   {1, 'S', (Proc) Do_nothing, 1},
   {1, '?', (Proc) Option, 1},
   {1, '(', (Proc) BeginExp, 1},
   {1, '<', (Proc) BeginOfTag, 4},
   {1, '*', (Proc) ppPutInBuffer, 2},

/* state 2:  */
   {2, 'S', (Proc) Do_nothing, 2},
   {2, '|', (Proc) EndChoice, 1},
   {2, ',', (Proc) EndPatNode, 1},
   {2, ':', (Proc) SymbolName, 1},
   {2, '{', (Proc) BeginChild, 1},
   {2, '+', (Proc) IterationTag, 3},
   {2, ')', (Proc) EndExp, 3},
   {2, '}', (Proc) EndChild, 3},
   {2, ';', (Proc) EndPattern, 10},
   {2, '*', (Proc) ppPutInBuffer, 2},
/* state 3: */
   {3, '|', (Proc) EndChoice, 1},
   {3, ',', (Proc) EndPatNode, 1},
   {3, ')', (Proc) EndExp, 3},
   {3, '}', (Proc) EndChild, 3},
   {3, '+', (Proc) Iteration, 3},
   {3, ';', (Proc) EndPattern, 10},
  /* attributes in patterns */
/* state 4: a '<' has been read : reading tag name */
   {4, 'S', (Proc) EndOfTagName, 6},
   {4, '>', (Proc) EndOfTagName, 2},
   {4, '*', (Proc) ppPutInBuffer, 4},
/* state 5: expexting a space or an end tag */
   {5, 'S', (Proc) Do_nothing, 6},
   {5, '>', (Proc) Do_nothing, 2},
/* state 6: reading an attribute name */
   {6, 'S', (Proc) ppEndOfAttrName, 6},
   {6, '=', (Proc) ppEndOfAttrName, 7},
   {6, '>', (Proc) ppEndOfAttrName, 2},
   {6, '*', (Proc) ppPutInBuffer, 6},
/* state 7: reading an attribute value */
   {7, '\"', (Proc) ppStartOfAttrValue, 8},
   {7, '\'', (Proc) ppStartOfAttrValue, 9},
   {7, 'S', (Proc) ppEndOfAttrValue, 6},
   {7, '>', (Proc) ppEndOfAttrValue, 2},
   {7, '*', (Proc) ppPutInBuffer, 7},
/* state 8: reading an attribute value between double quotes */
   {8, '\"', (Proc) ppEndOfAttrValue, 5},
   {8, '*', (Proc) ppPutInBuffer, 8},
/* state 9: reading an attribute value  between quotes */
   {9, '\'', (Proc) ppEndOfAttrValue, 5},
   {9, '*', (Proc) ppPutInBuffer, 9},
  /* transformation rules */
/* state 10: */
   {10, 'S', (Proc) Do_nothing, 10},
   {10, '{', (Proc) BeginRules, 11},
/* state 11: reading the LeftPartRule */
   {11, 'S', (Proc) Do_nothing, 11},
   {11, '>', (Proc) EndLeftPartRule, 12},
   {11, '/', (Proc) DeleteElementRule, 16},
   {11, '*', (Proc) ppPutInBuffer, 11},
/* state 12: reading the position path */
   {12, 'S', (Proc) Do_nothing, 12},
   {12, '<', (Proc) BeginRuleTag, -20},
   {12, ':', (Proc) EndOptNodes, 13},
   {12, '%', (Proc) SelectionRule, 12},
   {12, ';', (Proc) EndRule, 14},
   {12, '.', (Proc) EndNode, 12},
   {12, '*', (Proc) ppPutInBuffer, 12},
/* state 13: reading the generated nodes */
   {13, 'S', (Proc) Do_nothing, 13},
   {13, '<', (Proc) BeginRuleTag, -20},
   {13, '%', (Proc) SelectionRule, 13},
   {13, ';', (Proc) EndRule, 14},
   {13, '\"', (Proc) ppPutInBuffer, 15},
   {13, '.', (Proc) EndNode, 13},
   {13, '*', (Proc) ppPutInBuffer, 13},
/* state 14: waiting for the next rule or the end of rules */
   {14, 'S', (Proc) Do_nothing, 14},
   {14, '}', (Proc) EndTransformation, 0},
   {14, '*', (Proc) ppPutInBuffer, 11},
/* state 15: reading a content string */
   {15, '\"', (Proc) ppPutInBuffer, 13},
   {15, '*', (Proc) ppPutInBuffer, 15},
/* state 16: waiting a ; ending rule */
   {16, ';', (Proc) EndRule, 14},
   {16, 'S', (Proc) Do_nothing, 16},

  /*sub automaton for tags  in transformation rules */
/* state 20: a '<' has been read : reading tag name */
   {20, 'S', (Proc) EndRuleTagName, 22},
   {20, '>', (Proc) EndRuleTagName, -1},
   {20, '*', (Proc) ppPutInBuffer, 20},
/* state 21: expexting a space or an end tag */
   {21, 'S', (Proc) Do_nothing, 22},
   {21, '>', (Proc) Do_nothing, -1},
/* state 22: reading an attribute name */
   {22, 'S', (Proc) ppEndRuleAttrName, 22},
   {22, '=', (Proc) ppEndRuleAttrName, 23},
   {22, '.', (Proc) ppTransAttr, 26},
   {22, '>', (Proc) ppEndRuleAttrName, -1},
   {22, '*', (Proc) ppPutInBuffer, 22},
/* state 23: reading an attribute value */
   {23, '\"', (Proc) ppStartOfAttrValue, 24},
   {23, '\'', (Proc) ppStartOfAttrValue, 25},
   {23, 'S', (Proc) ppEndOfAttrValue, 22},
   {23, '>', (Proc) ppEndOfAttrValue, -1},
   {23, '.', (Proc) ppTransAttrValue, 26},
   {23, '*', (Proc) ppPutInBuffer, 23},
/* state 24: reading an attribute value between double quotes */
   {24, '\"', (Proc) ppEndOfAttrValue, 21},
   {24, '*', (Proc) ppPutInBuffer, 24},
/* state 25: reading an attribute value  between quotes */
   {25, '\'', (Proc) ppEndOfAttrValue, 21},
   {25, '*', (Proc) ppPutInBuffer, 25},
/* state 26: reading a transferred attribute name */
   {26, 'S', (Proc) ppEndTransAttr, 22},
   {26, '>', (Proc) ppEndTransAttr, -1},
   {26, '*', (Proc) ppPutInBuffer, 26},


/* st1ate 1000: fictious state. End of automaton table */
/* the next line must be the last one in the automaton declaration */
   {1000, '*', (Proc) Do_nothing, 1000}
};

/*----------------------------------------------------------------------
   	ppInitAutomaton	read the "source" form of the automaton and	
   			build the "executable" form.			
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         ppInitAutomaton (void)
#else
void         ppInitAutomaton ()
#endif
{
   int                 entry;
   State               theState;
   State               curState;
   PtrTransition       trans;
   PtrTransition       prevTrans;

   for (entry = 0; entry < MaxState; entry++)
       automaton[entry].firstTransition = NULL;
   entry = 0;
   curState = 1000;
   prevTrans = NULL;
   do
     {
	theState = ppsourceAutomaton[entry].initState;
	if (theState < 1000)
	  {
	     trans = (PtrTransition) TtaGetMemory (sizeof (Transition));
	     trans->nextTransition = NULL;
	     trans->trigger = ppsourceAutomaton[entry].trigger;
	     trans->action = ppsourceAutomaton[entry].transitionAction;
	     trans->newState = ppsourceAutomaton[entry].newState;
	     if (trans->trigger == 'S')		/* any spacing character */
		trans->trigger = SPACE;
	     if (trans->trigger == '*')		/* any character */
		trans->trigger = EOS;
	     if (theState != curState)
	       {
		  automaton[theState].automatonState = theState;
		  automaton[theState].firstTransition = trans;
		  curState = theState;
	       }
	     else
		prevTrans->nextTransition = trans;
	     prevTrans = trans;
	     entry++;
	  }
     }
   while (theState < 1000);
}

/*----------------------------------------------------------------------
   	FreeTransform	free the automaton.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void         FreeTransform (void)
#else
void         FreeTransform ()
#endif
{
   PtrTransition       trans, nextTrans;
   int		       entry;

   /* free the internal representation of the automaton */
   for (entry = 0; entry < MaxState; entry++)
      {
      trans = automaton[entry].firstTransition;
      while (trans != NULL)
	 {
	 nextTrans = trans->nextTransition;
	 TtaFreeMemory (trans);
	 trans = nextTrans;
	 }
      }  
}

/*----------------------------------------------------------------------
   	TRANSparse	parses the transformation file infile and builds the	
   			equivalent matching environment.			
  ----------------------------------------------------------------------*/
#ifdef PPSTANDALONE
#ifdef __STDC__
static void         TRANSparse (FILE * infile)
#else
static void         TRANSparse (infile)
FILE               *infile;
#endif
#else
#ifdef __STDC__
static void         TRANSparse (BinFile infile)
#else
static void         TRANSparse (infile)
BinFile               infile;
#endif
#endif
{
   unsigned char       charRead, oldcharRead;
   ThotBool            match, readOk;
   PtrTransition       trans;

   /* initialize automaton */
   /* parse the input file    */
   readOk = FALSE;
   numberOfCharRead = 0;
   numberOfLinesRead = 1;
   currentState = 0;
   charRead = EOS;
   readOk = FALSE;
   /* read the file sequentially */
   do
     {
	/* read one character from the input file, if the character read */
	/* previously has already been processed */
	if (charRead == EOS)
#ifdef PPSTANDALONE
	   charRead = getc (infile);
	if (!feof (infile) && !ferror (infile))
#else
	  readOk = TtaReadByte(infile,&charRead);
	if(readOk)
#endif
	  {
	     if (charRead == '!' && numberOfCharRead == 0)
	       {		/*comment */
		  do
#ifdef PPSTANDALONE
		    charRead = getc (infile);
		  while (charRead != EOL && !feof (infile) && !ferror (infile));
#else
		    readOk = TtaReadByte(infile,&charRead);
		  while (charRead != EOL && readOk);
#endif
	       }
	  }
#ifdef PPSTANDALONE
	if (!feof (infile) && !ferror (infile))
#else
	  if(readOk)
#endif
	  {
	     if (charRead == EOL)
	       {
		  /* new line in file */
		  numberOfLinesRead++;
		  numberOfCharRead = 0;
	       }
	     else
		numberOfCharRead++;

	     /* Check the character read */
	     /* Ignore end of line , non printable */
	     if (charRead < SPACE || (int) charRead >= 254 || ((int) charRead >= 127 && (int) charRead <= 159))
		charRead = EOS;
	     if (charRead != EOS)
	       {
		  /* a valid character has been read */
		  /* first transition of the automaton for the current state */
		  trans = automaton[currentState].firstTransition;
		  /* search a transition triggered by the character read */
		  while (trans)
		    {
		       match = FALSE;
		       if (charRead == trans->trigger)	/* the char is the trigger */
			  match = TRUE;
		       else if (trans->trigger == EOS)	/* any char is a trigger */
			  match = TRUE;
		       else if (trans->trigger == SPACE)	/* any space is a trigger */
			  if ((int) charRead == 9 || (int) charRead == 10 ||
			      (int) charRead == 12 || (int) charRead == 13)	/* a delimiter has been read */
			     match = TRUE;
		       if (match)
			 {
			    /* transition found. Activate the transition */
			    /* call the procedure associated with the transition */
			    normalTransition = TRUE;
			    if (trans->action)
			       (*(trans->action)) (charRead);
			    if (normalTransition)
			      {
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
				 else
				   {
				      /* calling a subautomaton */
				      returnState = currentState;
				      currentState = -trans->newState;
				   }
			      }
			    else
			      {	/* an error has been found skipping the transformation */
#ifdef PPSTANDALONE
				 oldcharRead = getc (infile);
				 if (!feof (infile) && !ferror (infile))
				    charRead = getc (infile);
				 while ((!feof (infile) && !ferror (infile)) && (charRead != '}' || oldcharRead != ';'))
				   {
				      oldcharRead = charRead;
				      charRead = getc (infile);
				      while (!feof (infile) && !ferror (infile) && ((int) charRead == 9 ||
										    (int) charRead == 10 || (int) charRead == 12 || (int) charRead == 13))
					 charRead = getc (infile);
				   }
				 if (!feof (infile) && !ferror (infile))
#else
				 readOk=TtaReadByte(infile,&oldcharRead);
				 if (readOk)
				    readOk=TtaReadByte(infile,&charRead);
				 while (readOk && (charRead != '}' || oldcharRead != ';'))
				   {
				      oldcharRead = charRead;
				      readOk=TtaReadByte(infile,&charRead);
				      while (readOk  && ((int) charRead == 9 ||
							 (int) charRead == 10 || 
							 (int) charRead == 12 || 
							 (int) charRead == 13 ||
							 (int) charRead == 32 ))
					 readOk=TtaReadByte(infile,&charRead);
				   }
				 if (readOk)
#endif
				   {
				      if (ppTrans)
					{
#ifdef AMAYA_DEBUG
					   fprintf (stderr, "skipping transformation %s\n", ppTrans->NameTrans);
#endif
					   FreeTrans (ppTrans);
					   ppTrans = NULL;
					}
				      else
#ifdef AMAYA_DEBUG
					 fprintf (stderr, "skipping transformation\n");
#endif
				      if (ppChoice != NULL)
					 FreeChoice (ppChoice);
				      ppChoice = NULL;
				      ppError = FALSE;
				      currentState = 0;
				      ppLgBuffer = 0;
				      charRead = EOS;
				      ppOptional = FALSE;
				      ppIterTag = FALSE;
				      ppAttr = NULL;
				      ppNode = NULL;
				      ppRule = NULL;
				      ppIsNamed = FALSE;
				      ustrcpy (ppName, TEXT(""));
				      opStack[0] = EOS;
				      symbolStack[0] = NULL;
				      choiceStack[0] = NULL;
				      sizeStack = 1;
				   }
			      }	/* done */
			    trans = NULL;
			 }
		       else
			 {
			    /* access next transition from the same state */
			    trans = trans->nextTransition;
			    if (trans == NULL)
			       charRead = EOS;
			 }
		    }
	       }
	  }
     }
#ifdef PPSTANDALONE
   while (!feof (infile) && !ferror (infile) && !ppError);
#else
   while (readOk && !ppError);
#endif
   /* end of HTML file */
}



/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         initpparse (void)
#else
static void         initpparse ()
#endif
{
   strTransDesc          *td, *td2;

   /* frees old transformation descriptors */
   td = ppTransSet->Transformations;
   while (td)
     {
	td2 = td->Next;
	FreeTrans (td);
	td = td2;
     }
   ppTransSet->NbTrans = 0;
   ppTransSet->MaxDepth = 0;
   ppTransSet->Transformations = NULL;
}


/*----------------------------------------------------------------------
   	ppStartParser loads the file Directory/FileName for parsing	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 ppStartParser (STRING name,SSchema tStrSchema, strTransSet **resTrSet)
#else
int                 ppStartParser (name, tStrSchema, resTrSet)
STRING              name;
SSchema            tStrSchema;
strTransSet        **resTrSet;
#endif
{
   CHAR_T                msg[200];

#ifdef PPSTANDALONE
   FILE               *infile = (FILE *)0;
#else
   BinFile             infile = (BinFile)0;
   CHAR_T		       fileName[MAX_LENGTH];
   CHAR_T                pathes[MAX_LENGTH];
   STRING              next, cour;
   ThotBool            found = FALSE;
   struct stat        *StatBuffer;
   int                 len, status;
#endif

#ifndef PPSTANDALONE
   /* searches if a transformation set is already allocated */
   /* for the file to be parsed */
   ppTransSet = strMatchEnv.TransSets;
   while (ppTransSet != NULL && (ustrcmp (ppTransSet->TransFileName, name) != 0))
     ppTransSet = ppTransSet->Next;
   if (ppTransSet == NULL)
     {
       ppTransSet = TtaGetMemory (sizeof (strTransSet));
       ustrcpy (ppTransSet->TransFileName, name);
       ppTransSet->Schema = tStrSchema;
       ppTransSet->timeLastWrite = (time_t) 0;
       ppTransSet->NbTrans = 0;
       ppTransSet->MaxDepth = 0;
       ppTransSet->Transformations = NULL;
       ppTransSet->Next = strMatchEnv.TransSets;
       strMatchEnv.TransSets = ppTransSet;
     }

   /* build the transformation file name from schema directory and schema name */
   
   TtaGetSchemaPath (pathes, MAX_LENGTH);
   cour = pathes;
   while (!found && cour != NULL)
     {
   	next = ustrchr (cour, PATH_SEP);
	if (next == NULL)
       	  ustrcpy (fileName, cour);
        else
	  {
            ustrncpy (fileName, cour, (size_t)(next - cour)); 
	    fileName[(next - cour)] = '\0';
          }
        len = ustrlen(fileName);
        if (fileName[len]!= WC_DIR_SEP)
          ustrcat (fileName, WC_DIR_STR);
        ustrcat (fileName,name);
        ustrcat (fileName, TEXT(".trans"));
	found = (TtaFileExist(fileName) == 1);
	if (!found)
	   if (next == NULL)
	      cour = NULL;
	   else
	      cour = next+1;
     }
   
   /* check if the file is newer than last read */
   StatBuffer = (struct stat *) TtaGetMemory (sizeof (struct stat));
   
   status = ustat (fileName, StatBuffer);
   if (status != -1)
     {
	if (StatBuffer->st_mtime == ppTransSet->timeLastWrite)
	/* the file is unchanged, activing all the transformations */
	   SetTransValid (ppTransSet);
	else
	  {
       	/* the file xxx.trans has been touched, parsing it */
	     ppTransSet->timeLastWrite = StatBuffer->st_mtime;
	     infile = TtaReadOpen(fileName);
#else
   infile = fopen (name, "r");
#endif
	   if (infile == 0)
	     {
		usprintf (msg, TEXT("Can't open file %s.trans"), name);
		ErrorMessage (msg);
		ppError = TRUE;
	     }
	   else
	     {
		initpparse ();
		ppError = FALSE;
		ppOptional = FALSE;
		ppIterTag = FALSE;
		ppAttr = NULL;
		ppNode = NULL;
		ppRule = NULL;
		ppIsNamed = FALSE;
		ustrcpy (ppName, TEXT(""));
		opStack[0] = EOS;
		symbolStack[0] = NULL;
		choiceStack[0] = NULL;
		sizeStack = 1;
		inputBuffer[0] = EOS;	/* initialize input buffer */
		ppLgBuffer = 0;
		TRANSparse (infile);
#ifndef PPSTANDALONE
	        TtaReadClose (infile);
	      }
          }
     }
   TtaFreeMemory (StatBuffer);
#else
	   	fclose (infile);
  	       }
#endif
   if (!ppError)
     *resTrSet = ppTransSet;
   else
     *resTrSet = NULL;
   return !ppError;
}


#ifdef PPSTANDALONE
void                main ()
{
   strTransDesc          *td;
   strSymbDesc           *sd;
   strListSymb           *ls;
   strAttrDesc           *pa;
   strNodeDesc           *nd;
   strRuleDesc           *pr;

   ppStartParser ("test.trans",NULL,NULL);
   printf ("\n");
   td = strMatchEnv.Transformations;
   while (td)
     {
	printf ("transformation : %s\n", td->NameTrans);
	printf ("entrees : ");
	ls = td->First;
	while (ls)
	  {
	     printf ("%s, ", ls->Symbol->SymbolName);
	     ls = ls->Next;
	  }
	printf ("\n");
	sd = td->Symbols;
	while (sd)
	  {
	     printf ("%s  ", s d->SymbolName);
	     pa = sd->Attributes;
	     while (pa)
	       {
		  printf ("%s=", pa->NameAttr);
		  if (pa->IsInt)
		     printf ("%d ,", pa->IntVal);
		  else
		     printf ("%s ,", pa->TextVal);
		  pa = pa->Next;
	       }
	     printf ("-- ");
	     ls = sd->Followings;
	     while (ls)
	       {
		  if (ls->Symbol == NULL)
		     printf ("end");
		  else
		     printf ("%s, ", ls->Symbol->SymbolName);
		  ls = ls->Next;
	       }
	     printf ("%s ", sd->IsOptChild ? "-?" : "-");
	     ls = sd->Children;
	     while (ls)
	       {
		  if (ls->Symbol == NULL)
		     printf ("end");
		  else
		     printf ("%s, ", ls->Symbol->SymbolName);
		  ls = ls->Next;
	       }
	     pr = sd->Rule;
	     if (pr)
	       {
		  printf ("\n %s > ", pr->RuleName);
		  nd = pr->OptionNodes;
		  while (nd)
		    {
		       if (nd->Attributes)
			  printf ("<");
		       printf ("%s", nd->Tag);
		       pa = nd->Attributes;
		       while (pa)
			 {
			    printf (" %s=", pa->NameAttr);
			    if (pa->IsTransf)
			       printf ("%s.%s", pa->AttrTag, pa->AttrAttr);
			    else if (pa->IsInt)
			       printf ("%d", pa->IntVal);
			    else
			       printf ("%s", pa->TextVal);
			    pa = pa->Next;
			    if (pa)
			       printf (" ");
			 }
		       if (nd->Attributes)
			  printf (">");
		       nd = nd->Next;
		       if (nd)
			  printf (".");
		       else
			  printf (":");
		    }
		  nd = pr->NewNodes;
		  while (nd)
		    {
		       if (nd->Attributes)
			  printf ("<");
		       printf ("%s", nd->Tag);
		       pa = nd->Attributes;
		       while (pa)
			 {
			    printf (" %s=", pa->NameAttr);
			    if (pa->IsTransf)
			       printf ("%s.%s", pa->AttrTag, pa->AttrAttr);
			    else if (pa->IsInt)
			       printf ("%d", pa->IntVal);
			    else
			       printf ("%s", pa->TextVal);
			    pa = pa->Next;
			    if (pa)
			       printf (" ");
			 }
		       if (nd->Attributes)
			  printf (">");
		       nd = nd->Next;
		       if (nd)
			  printf (".");
		       else
			  printf (";");
		    }
	       }
	     printf ("\n");
	     sd = sd->Next;
	  }
	printf ("\n");
	td = td->Next;
     }
   printf ("\n");
}
#endif
