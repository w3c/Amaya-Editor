/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Transformation language parser, see file HTML.trans for a 
 * description of the language				
 *
 * Authors: I. Vatton (W3C/INRIA), S. Bonhomme
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "fileaccess.h"
#include "fetchHTMLname.h"
#include "trans.h"

extern char*   SchemaPath;

/* pattern and rules internal representation */
typedef struct _parForest
{
     strListSymb           *first;
     strListSymb           *last;
     ThotBool               optional;
     struct _parForest     *next;
} parForest;

typedef struct _parChoice
{
     parForest          *forests;
     ThotBool            optional;
} parChoice;

static char            ParsedName[TRANSNAME];
static parChoice      *ParsedChoice;	/* current forest descriptor */
static parForest      *ParsedForest;	/* cuurent forest descriptor */
static parChoice      *ParsedLastChoice;
static strTransDesc   *ParsedTrans;	/* current transformation descriptor */
static strTransSet    *ParsedTransSet; /* current transformation set */
static strSymbDesc    *ParsedSymb;	/* current pattern symbol descriptor */
static strAttrDesc    *ParsedAttr;	/* attribute descriptor */
static strNodeDesc    *ParsedNode;	/* node descriptor */
static strRuleDesc    *ParsedRule;	/* rule descriptor */
static parChoice      *choiceStack[MAX_STACK];
static char            opStack[MAX_STACK];
static strSymbDesc    *symbolStack[MAX_STACK];
static int             sizeStack;
static int             patDepth;
static int             numberOfLinesRead;
static int             numberOfCharRead;
static ThotBool        ParsedError;
static ThotBool        ParsedIsNamed;
static ThotBool        ParsedOptional;
static ThotBool        ParsedIterTag;
static ThotBool	       SelRuleFlag;
static ThotBool        normalTransition;

#define MaxBufferLength   1000
#define AllmostFullBuffer  700
static unsigned char   inputBuffer[MaxBufferLength];
static int             ParsedLgBuffer = 0;	/* actual length of text in input buffer */
typedef int            State;	/* a state of the automaton */
static State           CurrentState;	/* current state of the automaton */
static State           ReturnState;	/* return state from subautomaton */

#include "fetchHTMLname_f.h"
#include "fetchXMLname_f.h"
#include "html2thot_f.h"
#include "transparse_f.h"
#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */

/*----------------------------------------------------------------------
  Init all transformation of the transformation set to valid,
   sets the value  transSet.MaxDepth 
  ----------------------------------------------------------------------*/
static void SetTransValid (strTransSet  *transSet)
{
   strTransDesc          *td;
   strSymbDesc           *symb;

   td = transSet->Transformations;
   while (td)
     {
	if (!(td->IsActiveTrans))
	  {
	     td->IsActiveTrans = TRUE;
	     if (td->PatDepth > transSet->MaxDepth)
		transSet->MaxDepth = td->PatDepth;
	     symb = td->Symbols;
	     while (symb)
	       {
		  symb->IsActiveSymb = TRUE;
		  symb = symb->Next;
	       }
	  }
	td = td->Next;
     }
}

/*----------------------------------------------------------------------
  Frees a transformtion descriptor
  ----------------------------------------------------------------------*/
static void FreeTrans (strTransDesc *td)
{
   strListSymb           *list, *list2;
   strSymbDesc           *symb, *symb2;
   strAttrDesc           *attr, *attr2;
   strRuleDesc           *rule, *rule2;
   strNodeDesc           *node, *node2;

   TtaFreeMemory (td->NameTrans);
   TtaFreeMemory (td->DestinationTag);
   list = td->First;
   while (list)
     {
	list2 = list->Next;
	TtaFreeMemory (list);
	list = list2;
     }
   if (td->RootDesc)
     {
	TtaFreeMemory (td->RootDesc->Tag);
	TtaFreeMemory (td->RootDesc);
     }
   symb = td->Symbols;
   while (symb)
     {
	TtaFreeMemory (symb->SymbolName);
	TtaFreeMemory (symb->Tag);
	list = symb->Children;
	while (list)
	  {
	     list2 = list->Next;
	     TtaFreeMemory (list);
	     list = list2;
	  }
	list = symb->Followings;
	while (list)
	  {
	     list2 = list->Next;
	     TtaFreeMemory (list);
	     list = list2;
	  }
	attr = symb->Attributes;
	while (attr)
	  {
	     TtaFreeMemory (attr->NameAttr);
	     if (attr->IsTransf)
	       {
		  TtaFreeMemory (attr->AttrTag);
		  TtaFreeMemory (attr->AttrAttr);
	       }
	     else if (!attr->IsInt)
		TtaFreeMemory (attr->TextVal);

	     attr2 = attr->Next;
	     TtaFreeMemory (attr);
	     attr = attr2;
	  }
	symb2 = symb->Next;
	TtaFreeMemory (symb);
	symb = symb2;
     }
   rule = td->Rules;
   while (rule)
     {
	node = rule->OptionNodes;
	while (node)
	  {
	     attr = node->Attributes;
	     while (attr)
	       {
		  TtaFreeMemory (attr->NameAttr);
		  if (attr->IsTransf)
		    {
		       TtaFreeMemory (attr->AttrTag);
		       TtaFreeMemory (attr->AttrAttr);
		    }
		  else if (!attr->IsInt)
		    {
		       TtaFreeMemory (attr->TextVal);
		    }
		  attr2 = attr->Next;
		  TtaFreeMemory (attr);
		  attr = attr2;
	       }
	     TtaFreeMemory (node->Tag);
	     node2 = node->Next;
	     TtaFreeMemory (node);
	     node = node2;
	  }
	node = rule->NewNodes;
	while (node)
	  {
	     attr = node->Attributes;
	     while (attr)
	       {
		  TtaFreeMemory (attr->NameAttr);
		  if (attr->IsTransf)
		    {
		       TtaFreeMemory (attr->AttrTag);
		       TtaFreeMemory (attr->AttrAttr);
		    }
		  else if (!attr->IsInt)
		    {
		       TtaFreeMemory (attr->TextVal);
		    }
		  attr2 = attr->Next;
		  TtaFreeMemory (attr);
		  attr = attr2;
	       }
	     TtaFreeMemory (node->Tag);
	     node2 = node->Next;
	     TtaFreeMemory (node);
	     node = node2;
	  }
	rule2 = rule->Next;
	TtaFreeMemory (rule->RuleName);
	TtaFreeMemory (rule);
	rule = rule2;
     }
   TtaFreeMemory (td);
}


/*----------------------------------------------------------------------
  Frees a list descriptor
  ----------------------------------------------------------------------*/
void FreeList (strListSymb *list)
{
   if (list)
     {
	FreeList (list->Next);
	TtaFreeMemory (list);
     }
}

/*----------------------------------------------------------------------
  Frees a forest descriptor
 ----------------------------------------------------------------------*/
static void FreeForest (parForest *pf)
{
   if (pf)
     {
	if (pf == ParsedForest)
	   ParsedForest = NULL;
	FreeList (pf->first);
	FreeList (pf->last);
	FreeForest (pf->next);
	TtaFreeMemory (pf);
     }
}

/*----------------------------------------------------------------------
  Frees a choice descriptor
  ----------------------------------------------------------------------*/
static void FreeChoice (parChoice *pc)
{
   if (pc)
     {
	FreeForest (pc->forests);
	TtaFreeMemory (pc);
     }
}

/*----------------------------------------------------------------------
   	ErrorMessage	print the error message msg on stderr.		
  ----------------------------------------------------------------------*/
static void ErrorMessage (unsigned char *msg)
{
   char                numline[5];

   sprintf (numline, "%d", numberOfLinesRead);
   TtaSetStatus (TransDoc, 1, TtaGetMessage (AMAYA, AM_TRANS_PARSE_ERROR), numline);
   normalTransition = FALSE;
}

/*----------------------------------------------------------------------
  adds the pattern symbol symb to the plist symbol list 
  ----------------------------------------------------------------------*/
static void  AddSymbToList (strListSymb **pList, strSymbDesc *symb)
{
   ThotBool            isjok, isnull, found;
   strListSymb        *pl, *plnext;

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
	   isjok = (!strcmp ((char *)symb->Tag, "*"));
	pl = *pList;
	found = ((isnull && pl->Symbol == NULL) || (!isnull && pl->Symbol == symb));
	if (pl->Next == NULL && !isnull && !found)
	  {
	     if ((isjok && pl->Symbol == NULL) ||
	     (!isjok && (pl->Symbol == NULL || !strcmp ((char *)pl->Symbol->Tag, "*"))))
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
		    ((isjok || isnull) || (pl->Next->Symbol && strcmp ((char *)pl->Next->Symbol->Tag, "*")))
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
static void AddTerminal (parChoice * pc)
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
static void ProcessSymbol (void)
{
   strSymbDesc     *symb;
   char             msgBuffer[MaxBufferLength];
   SSchema	    schema;

   if (ParsedLgBuffer != 0)
     {		
       /* cr4eates a new symbol in the pattern internal representation */
	ParsedTrans->NbPatSymb++;
	ParsedSymb = (strSymbDesc *) TtaGetMemory (sizeof (strSymbDesc));
	ParsedSymb->Rule = NULL;
	ParsedSymb->Children = NULL;
	ParsedSymb->Followings = NULL;
	ParsedSymb->IsOptional = ParsedOptional;
	ParsedSymb->IsActiveSymb = TRUE;
	ParsedSymb->Attributes = NULL;
	ParsedSymb->Next = NULL;
	ParsedSymb->Depth = patDepth;
	if (ParsedIsNamed)
	  {
	     ParsedSymb->SymbolName = (unsigned char *)TtaStrdup ((char *)ParsedName);
	     ParsedName[0] = EOS;
	  }
	else
	   ParsedSymb->SymbolName = (unsigned char *)TtaStrdup ((char *)inputBuffer);
	ParsedSymb->Tag = (unsigned char *)TtaStrdup ((char *)inputBuffer);
	ParsedIsNamed = FALSE;
	ParsedLgBuffer = 0;
	ParsedOptional = FALSE;
     }
   if (ParsedSymb)
     {
       /* adds the new symbol to those of the pattern */
	symb = ParsedTrans->Symbols;
	if (!symb)
	   ParsedTrans->Symbols = ParsedSymb;
	else
	  {
	     while (symb->Next)
		symb = symb->Next;
	     symb->Next = ParsedSymb;
	  }
	schema = ParsedTransSet->Schema;
	if (strcmp ((char *)ParsedSymb->Tag, "*") && (MapGI ((char *)ParsedSymb->Tag, &schema, 0) == -1))
	  {
	     ParsedError = TRUE;
	     sprintf (msgBuffer, "unknown element %s", ParsedSymb->Tag);
	     ErrorMessage ((unsigned char *)msgBuffer);
	  }
     }
}

/*----------------------------------------------------------------------
  creates a new symbol descriptor and links it with the pattern representation
  ----------------------------------------------------------------------*/
static void NewSymbol (void)
{
   strSymbDesc           *symb;
   strListSymb           *pList;

   if (ParsedSymb)
     {
	if (ParsedForest->first == NULL)
	  {
	    /* the new symbol is the first and last of current forest */
	     ParsedForest->first = (strListSymb *) TtaGetMemory (sizeof (strListSymb));
	     ParsedForest->first->Next = NULL;
	     ParsedForest->first->Symbol = ParsedSymb;
	     ParsedForest->last = (strListSymb *) TtaGetMemory (sizeof (strListSymb));
	     ParsedForest->last->Next = NULL;
	     ParsedForest->last->Symbol = ParsedSymb;
	  }
	else
	  {
	    /* the new symbol is not the first of current forest */
	    /* it is added as a next possible symbol of each candidate to the last symbol of */
	       /* current forest */
	     pList = ParsedForest->last;
	     while (pList)
	       {
		  symb = pList->Symbol;
		  if (symb)
		    {
		       AddSymbToList (&symb->Followings, ParsedSymb);
		    }
		  pList = pList->Next;
	       }
	     if (ParsedForest->optional)
	       {
		 /* if all previous symbol of the forest are optional, adds the new symbol */
		 /*as a candidate for the first symbol of the current forest */
		 AddSymbToList (&ParsedForest->first, ParsedSymb);
		 ParsedForest->optional = ParsedSymb->IsOptional;
	       }
	     if (ParsedSymb->IsOptional)
	       {
		 /* if the symbol is optional, it is added to the list of last possible of the */
		 /* current forest */
		  AddSymbToList (&ParsedForest->last, ParsedSymb);
	       }
	     else
	       {
		 /* if the symbol is mandatory, it becomes the last possible of current forest */
		  FreeList (ParsedForest->last);
		  ParsedForest->last = (strListSymb *) TtaGetMemory (sizeof (strListSymb));
		  ParsedForest->last->Next = NULL;
		  ParsedForest->last->Symbol = ParsedSymb;
	       }
	  }
	if (ParsedIterTag)
	   AddSymbToList (&ParsedSymb->Followings, ParsedSymb);
	ParsedIterTag = FALSE;
     }
   ParsedSymb = NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void SymbolName (unsigned char c)
{
   if (ParsedLgBuffer == 0)
     {
	ParsedError = TRUE;
	ErrorMessage ((unsigned char *)"missing tag name");
     }
   ParsedIsNamed = TRUE;
   strcpy ((char *)ParsedName, (char *)inputBuffer);
   ParsedLgBuffer = 0;
}

/*----------------------------------------------------------------------
  A transformation name has been read, allocates a new trasformation
  descriptor
  ----------------------------------------------------------------------*/
static void EndNameTrans (unsigned char c)
{

   if (ParsedLgBuffer != 0)
     {	/* allocates the descriptor    */
	patDepth = 0;
	ParsedTrans = (strTransDesc *) TtaGetMemory (sizeof (strTransDesc));
#ifdef _WX
	/* store UTF-8 names */
	ParsedTrans->NameTrans = TtaConvertByteToMbs (inputBuffer, ISO_8859_1);
#else /* _WX */
	ParsedTrans->NameTrans = (unsigned char *)TtaStrdup ((char *)inputBuffer);
#endif /* _WX */
	ParsedTrans->NbPatSymb = 0;
	ParsedTrans->NbRules = 0;
	ParsedTrans->PatDepth = 0;
	ParsedTrans->First = NULL;
	ParsedTrans->RootDesc = NULL;
	ParsedTrans->Symbols = NULL;
	ParsedTrans->Rules = NULL;
	ParsedTrans->IsActiveTrans = TRUE;
	ParsedTrans->IsAction = FALSE; /* not an action by default */
	ParsedTrans->DestinationTag = NULL;
	ParsedTrans->Next = NULL;
	ParsedLgBuffer = 0;

	/* allocates new choice and forest descriptors */
	ParsedChoice = (parChoice *) TtaGetMemory (sizeof (parChoice));
	ParsedForest = (parForest *) TtaGetMemory (sizeof (parForest));
	ParsedChoice->forests = ParsedForest;
	ParsedChoice->optional = FALSE;
	ParsedForest->first = NULL;
	ParsedForest->last = NULL;
	ParsedForest->optional = FALSE;
	ParsedForest->next = NULL;
     }
   else
     {
	ParsedError = TRUE;
	ErrorMessage ((unsigned char *)"Missing Transformation Name");
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void Option (unsigned char c)
{
   ParsedOptional = TRUE;
   if (ParsedForest->first == NULL)
      ParsedForest->optional = TRUE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void BeginExp (unsigned char c)
{
   /*   ProcessSymbol(); */
   choiceStack[sizeStack] = ParsedChoice;
   opStack[sizeStack] = c;
   symbolStack[sizeStack++] = ParsedSymb;
   ParsedChoice = (parChoice *) TtaGetMemory (sizeof (parChoice));
   ParsedForest = (parForest *) TtaGetMemory (sizeof (parForest));
   ParsedChoice->forests = ParsedForest;
   ParsedChoice->optional = ParsedOptional;
   ParsedForest->first = NULL;
   ParsedForest->last = NULL;
   ParsedForest->optional = FALSE;
   ParsedForest->next = NULL;
   ParsedOptional = FALSE;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void EndExp (unsigned char c)
{
  parForest             *pf;
  strSymbDesc           *symb;
  strListSymb           *pList, *pL3;
  
  ProcessSymbol ();
  NewSymbol ();
  if (sizeStack < 1 || opStack[sizeStack - 1] != '(')
    {
      ParsedError = TRUE;
      ErrorMessage ((unsigned char *)"mismatched parentheses");
    }
  else
    {
      if (ParsedLastChoice)
	FreeChoice (ParsedLastChoice);
      ParsedLastChoice = ParsedChoice;
      pf = ParsedChoice->forests;
      while (pf && !ParsedChoice->optional)
	{
	  ParsedChoice->optional = ParsedChoice->optional || pf->optional;
	  pf = pf->next;
	}
      /* pops the choice descriptor */
      ParsedChoice = choiceStack[sizeStack - 1];
      sizeStack--;
      ParsedForest = ParsedChoice->forests;
      /* for each alternative of the choice, adds the symbols of parsed expression to the descriptor */
      while (ParsedForest->next)
	ParsedForest = ParsedForest->next;

      if (ParsedForest->first == NULL)
	{
	  /* if the forest descriptor is empty, replaces nodes lists by those of parsed expression */
	  pf = ParsedLastChoice->forests;
	  while (pf)
	    {
	      pList = pf->first;
	      while (pList)
		{
		  AddSymbToList (&ParsedForest->first, pList->Symbol);
		  pList = pList->Next;
		}
	      pList = pf->last;
	      while (pList)
		{
		  AddSymbToList (&ParsedForest->last, pList->Symbol);
		  pList = pList->Next;
		}
	      pf = pf->next;
	    }
	  ParsedForest->optional = ParsedLastChoice->optional;
	}
      else
	{
	  /* the first symbol of parsed expressions are added to the list of first possible */
	  /* symbols of the current forest */
	  pList = ParsedForest->last;
	  while (pList)
	    {
	      symb = pList->Symbol;
	      if (symb)
		{	
		  pf = ParsedLastChoice->forests;
		  while (pf)
		    {
		      pL3 = pf->first;
		      while (pL3)
			{
			  AddSymbToList (&symb->Followings, pL3->Symbol);
			  pL3 = pL3->Next;
			}
		      pf = pf->next;
		    }
		}
	      pList = pList->Next;
	    }

	  if (ParsedForest->optional)
	    {
	      /* if all symbols are otional in current forest, the first possible symbol of parsed */
	      /* expression are added to the first possible of current forest descriptor */
	      pf = ParsedLastChoice->forests;
	      while (pf)
		{
		  pList = pf->first;
		  while (pList)
		    {
		      AddSymbToList (&ParsedForest->first, pList->Symbol);
		      pList = pList->Next;
		    }
		  pf = pf->next;
		}
	    }

	  if (!ParsedLastChoice->optional)
	    {
	      FreeList (ParsedForest->last);
	      ParsedForest->last = NULL;
	    }
	  /* adds the last possible of last parsed choice to the last pssoble of current forest descriptor */
	  pf = ParsedLastChoice->forests;
	  while (pf)
	    {
	      pList = pf->last;
	      while (pList)
		{
		  AddSymbToList (&ParsedForest->last, pList->Symbol);
		  pList = pList->Next;
		}
	      pf = pf->next;
	    }
	}
      ParsedForest->optional = ParsedForest->optional && ParsedLastChoice->optional;
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void IterationTag (unsigned char c)
{
   ParsedIterTag = TRUE;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void Iteration (unsigned char c)
{
  parForest          *pf1, *pf2;
  strListSymb           *plfirst, *pllast;
  strSymbDesc           *symb;

  if (ParsedLastChoice)
    {
      /* adds the first symbols of last choice descriptor as next
	 possible of its last symbols */
      pf1 = ParsedLastChoice->forests;
      while (pf1)
	{
	  plfirst = pf1->first;
	  while (plfirst)
	    {
	      pf2 = ParsedLastChoice->forests;
	      while (pf2)
		{
		  pllast = pf2->last;
		  while (pllast)
		    {
		      symb = pllast->Symbol;
		      if (symb)
			AddSymbToList (&symb->Followings, plfirst->Symbol);
		      pllast = pllast->Next;
		    }
		  pf2 = pf2->next;
		}
	      plfirst = plfirst->Next;
	    }
	  pf1 = pf1->next;
	}
      FreeChoice (ParsedLastChoice);
      ParsedLastChoice = NULL;
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void BeginChild (unsigned char c)
{
   ProcessSymbol ();
   symbolStack[sizeStack] = ParsedSymb;
   NewSymbol ();
   opStack[sizeStack] = c;
   choiceStack[sizeStack++] = ParsedChoice;
   ParsedSymb = NULL;
   ParsedChoice = (parChoice *) TtaGetMemory (sizeof (parChoice));
   ParsedForest = (parForest *) TtaGetMemory (sizeof (parForest));
   ParsedChoice->forests = ParsedForest;
   ParsedChoice->optional = FALSE;
   ParsedForest->first = NULL;
   ParsedForest->last = NULL;
   ParsedForest->optional = FALSE;
   ParsedForest->next = NULL;
   ParsedOptional = FALSE;
   patDepth++;
}
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void EndChild (unsigned char c)
{
   parChoice          *pc;
   parForest          *pf;
   strListSymb           *pl;

   if (sizeStack < 1 || opStack[sizeStack - 1] != '{')
     {
	ParsedError = TRUE;
	ErrorMessage ((unsigned char *)"mismatched parentheses");
     }
   else
     {
	/* process the last read symbol */
	ProcessSymbol ();
	NewSymbol ();
	AddTerminal (ParsedChoice);
	/* check if the current depth is the maximal depth */
	if (patDepth > ParsedTransSet->MaxDepth)
	   ParsedTransSet->MaxDepth = patDepth;
	if (patDepth > ParsedTrans->PatDepth)
	   ParsedTrans->PatDepth = patDepth;
	patDepth--;

	pc = ParsedChoice;
	/* pops last context */
	ParsedChoice = choiceStack[sizeStack - 1];
	ParsedSymb = symbolStack[sizeStack - 1];
	sizeStack--;
	pf = pc->forests;
	while (pf && !pc->optional)
	  {
	     pc->optional = pc->optional || pf->optional;
	     pf = pf->next;
	  }
	ParsedSymb->IsOptChild = pc->optional;

	/* adds first symbols of child context as first children of parent context */
	pf = pc->forests;
	while (pf)
	  {
	     pl = pf->first;
	     while (pl)
	       {
		  AddSymbToList (&ParsedSymb->Children, pl->Symbol);
		  pl = pl->Next;
	       }
	     pf = pf->next;
	  }
	FreeChoice (pc);
	pf = ParsedChoice->forests;
	while (pf->next)
	   pf = pf->next;
	ParsedForest = pf;
	ParsedSymb = NULL;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void EndChoice (unsigned char c)
{
   ProcessSymbol ();
   NewSymbol ();
   ParsedForest->next = (parForest *) TtaGetMemory (sizeof (parForest));
   ParsedForest = ParsedForest->next;
   ParsedForest->first = NULL;
   ParsedForest->last = NULL;
   ParsedForest->optional = FALSE;
   ParsedForest->next = NULL;
   ParsedOptional = FALSE;
   if (ParsedLastChoice)
     {
	FreeChoice (ParsedLastChoice);
	ParsedLastChoice = NULL;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void EndPatNode (unsigned char c)
{
   ProcessSymbol ();
   NewSymbol ();
   ParsedOptional = FALSE;
   if (ParsedLastChoice)
     {
	FreeChoice (ParsedLastChoice);
	ParsedLastChoice = NULL;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void EndPattern (unsigned char c)
{
   parForest          *pf;
   strListSymb           *pl, *pl2;

   ProcessSymbol ();
   NewSymbol ();
   if (sizeStack != 1)
     {
	ParsedError = TRUE;
	ErrorMessage ((unsigned char *)"Syntax error");
     }
   else
     {
	AddTerminal (ParsedChoice);
	pl2 = ParsedTrans->First;
	pf = ParsedChoice->forests;
	while (pf)
	  {
	     pl = pf->first;
	     pf->first = NULL;
	     if (pl)
	       {
		  if (ParsedTrans->First == NULL)
		    {
		       ParsedTrans->First = pl;
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
   FreeChoice (ParsedChoice);
   ParsedChoice = NULL;
   ParsedForest = NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void BeginOfTag (unsigned char c)
{
   if (ParsedLgBuffer != 0)
     {
	ParsedError = TRUE;
	ErrorMessage ((unsigned char *)"Syntax Error");
     }
   else
     {
	ParsedSymb = (strSymbDesc *) TtaGetMemory (sizeof (strSymbDesc));
	ParsedSymb->SymbolName = (unsigned char *)TtaGetMemory (NAME_LENGTH);
	ParsedSymb->Tag = (unsigned char *)TtaGetMemory (NAME_LENGTH);
	strcpy ((char *)ParsedSymb->SymbolName, "");
	strcpy ((char *)ParsedSymb->Tag, "");
	ParsedSymb->Rule = NULL;
	ParsedSymb->Children = NULL;
	ParsedSymb->Followings = NULL;
	ParsedSymb->IsOptional = ParsedOptional;
	ParsedSymb->IsActiveSymb = TRUE;
	ParsedSymb->Attributes = NULL;
	ParsedSymb->Next = NULL;
	if (ParsedIsNamed)
	  {
	     strcpy ((char *)ParsedSymb->SymbolName, (char *)ParsedName);
	     strcpy ((char *)ParsedName, "");
	  }
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void BeginRuleTag (unsigned char c)
{
   if (ParsedLgBuffer != 0)
     {
	ParsedError = TRUE;
	ErrorMessage ((unsigned char *)"Syntax Error");
     }
   else
     {
	strcpy ((char *)ParsedNode->Tag, "");
	ParsedNode->Attributes = NULL;
	ParsedNode->Next = NULL;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void EndOfTagName (unsigned char c)
{
   if (ParsedLgBuffer != 0)
     {
	strcpy ((char *)ParsedSymb->Tag, (char *)inputBuffer);
	if (!ParsedIsNamed)
	   strcpy ((char *)ParsedSymb->SymbolName, (char *)inputBuffer);
	ParsedLgBuffer = 0;
	ParsedIsNamed = FALSE;
     }
   else if (!strcmp ((char *)ParsedSymb->Tag, ""))
     {
	ParsedError = TRUE;
	ErrorMessage ((unsigned char *)"Missing Tag Name");
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void EndRuleTagName (unsigned char c)
{
   if (ParsedLgBuffer != 0)
     {
	strcpy ((char *)ParsedNode->Tag, (char *)inputBuffer);
	ParsedLgBuffer = 0;
     }
   else if (!strcmp ((char *)ParsedNode->Tag, ""))
     {
	ParsedError = TRUE;
	ErrorMessage ((unsigned char *)"Missing Tag Name");
     }
}


/*----------------------------------------------------------------------
   MapThotAttr
   search in AttributeMaParsedingTable the entry for the attribute of name Attr
   and returns the Thot Attribute corresponding to the rank of that entry.
  ----------------------------------------------------------------------*/
int MapThotAttr (const char* attrName, const char *elementName)
{
  int        i, thotAttr;
  ThotBool   level;

  i = MapXMLAttribute (XHTML_TYPE, attrName, elementName, &level, TransDoc, &thotAttr);
  if (i < 0)
    i = MapXMLAttribute (MATH_TYPE, attrName, elementName, &level, TransDoc, &thotAttr);
  if (i < 0)
    i = MapXMLAttribute (SVG_TYPE, attrName, elementName, &level, TransDoc, &thotAttr);
  return thotAttr;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void DoEndOfAttrName (unsigned char c)
{
  int               thotAttr;
  char              msgBuffer[MaxBufferLength];

  if (ParsedLgBuffer != 0)
    {
      thotAttr = MapThotAttr ((char *)inputBuffer, (char *)ParsedSymb->Tag);
      if (thotAttr != -1)
	{
	  ParsedAttr = ParsedSymb->Attributes;
	  if (!ParsedAttr)
	    {
	      ParsedSymb->Attributes = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
	      ParsedAttr = ParsedSymb->Attributes;
	      ParsedAttr->NameAttr = (unsigned char *)TtaGetMemory (NAME_LENGTH);
	      ParsedAttr->Next = NULL;
	    }
	  else
	    {
	      while (ParsedAttr->Next && strcmp ((char *)ParsedAttr->NameAttr, (char *)inputBuffer))
		ParsedAttr = ParsedAttr->Next;
	      if (!strcmp ((char *)ParsedAttr->NameAttr, (char *)inputBuffer))
		{
		  ParsedError = TRUE;
		  ErrorMessage ((unsigned char *)"Multi valued attribute");
		}
	      else
		{
		  ParsedAttr->Next = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
		  ParsedAttr = ParsedAttr->Next;
		  ParsedAttr->NameAttr = (unsigned char *)TtaGetMemory (NAME_LENGTH);
		  ParsedAttr->Next = NULL;
		}
	    }
	  strcpy ((char *)ParsedAttr->NameAttr, (char *)inputBuffer);
	  ParsedAttr->ThotAttr = thotAttr;
	  ParsedAttr->IsInt = TRUE;
	  ParsedAttr->IsTransf = FALSE;
	  ParsedAttr->IntVal = 0;
	}
      else
	{
	  ParsedError = TRUE;
	  sprintf (msgBuffer, "unknown attribute %s", inputBuffer);
	  ErrorMessage ((unsigned char *)msgBuffer);
	}
      ParsedLgBuffer = 0;
    }
  else
    {
      ParsedError = TRUE;
      ErrorMessage ((unsigned char *)"Missing Attribute Name");
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void DoEndRuleAttrName (unsigned char c)
{
  int                 thotAttr;
  char              msgBuffer[MaxBufferLength];

  if (ParsedLgBuffer != 0)
    {
      if (strcmp ((char *)ParsedNode->Tag, "*") == 0)
	thotAttr = MapThotAttr ((char *)inputBuffer, NULL);
      else
	thotAttr = MapThotAttr ((char *)inputBuffer, (char *)ParsedNode->Tag);
      if (thotAttr != -1)
	{
	  ParsedAttr = ParsedNode->Attributes;
	  if (!ParsedAttr)
	    {
	      ParsedNode->Attributes = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
	      ParsedAttr = ParsedNode->Attributes;
	      ParsedAttr->NameAttr = (unsigned char *)TtaStrdup ((char *)inputBuffer);
	      ParsedAttr->Next = NULL;
	    }
	  else
	    {
	      while (ParsedAttr->Next && strcmp ((char *)ParsedAttr->NameAttr, (char *)inputBuffer))
		ParsedAttr = ParsedAttr->Next;
	      if (!strcmp ((char *)ParsedAttr->NameAttr, (char *)inputBuffer))
		{
		  ParsedError = TRUE;
		  ErrorMessage ((unsigned char *)"Multi valued attribute");
		}
	      else
		{
		  ParsedAttr->Next = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
		  ParsedAttr = ParsedAttr->Next;
		  ParsedAttr->NameAttr = (unsigned char *)TtaStrdup ((char *)inputBuffer);
		  ParsedAttr->Next = NULL;
		}
	    }
	  ParsedAttr->ThotAttr = thotAttr;
	  ParsedAttr->IsInt = TRUE;
	  ParsedAttr->IsTransf = FALSE;
	  ParsedAttr->IntVal = 0;
	}
      else
	{
	  ParsedError = TRUE;
	  sprintf (msgBuffer, "unknown attribute %s", inputBuffer);
	  ErrorMessage ((unsigned char *)msgBuffer);
	}
      ParsedLgBuffer = 0;
    }
  else
    {
      ParsedError = TRUE;
      ErrorMessage ((unsigned char *)"Missing Attribute Name");
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void DoTransAttr (unsigned char c)
{
  int                 thotAttr;

  if (ParsedLgBuffer != 0)
    {
      thotAttr = MapThotAttr ((char *)inputBuffer, (char *)ParsedNode->Tag);
      if (thotAttr != -1)
	{
	  ParsedAttr = ParsedNode->Attributes;
	  if (!ParsedAttr)
	    {
	      ParsedNode->Attributes = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
	      ParsedAttr = ParsedNode->Attributes;
	      ParsedAttr->NameAttr = (unsigned char *)TtaGetMemory (NAME_LENGTH);
	      ParsedAttr->Next = NULL;
	    }
	  else
	    {
	      while (ParsedAttr->Next && strcmp ((char *)ParsedAttr->NameAttr, (char *)inputBuffer))
		ParsedAttr = ParsedAttr->Next;
	      if (!strcmp ((char *)ParsedAttr->NameAttr, (char *)inputBuffer))
		{
		  ParsedError = TRUE;
		  ErrorMessage ((unsigned char *)"Multi valued attribute");
		}
	      else
		{
		  ParsedAttr->Next = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
		  ParsedAttr = ParsedAttr->Next;
		  ParsedAttr->NameAttr = (unsigned char *)TtaGetMemory (NAME_LENGTH);
		  ParsedAttr->Next = NULL;
		}
	    }
	  ParsedAttr->AttrTag = (unsigned char *)TtaGetMemory (NAME_LENGTH);
	  strcpy ((char *)ParsedAttr->AttrTag, (char *)inputBuffer);
	  strcpy ((char *)ParsedAttr->NameAttr, "");
	  ParsedAttr->ThotAttr = thotAttr;
	  ParsedAttr->IsInt = FALSE;
	  ParsedAttr->IsTransf = TRUE;
	}
      else
	{
	  ParsedError = TRUE;
	  ErrorMessage ((unsigned char *)"Unknown attribute");
	}
      ParsedLgBuffer = 0;
    }
  else
    {
      ParsedError = TRUE;
      ErrorMessage ((unsigned char *)"Missing Attribute Name");
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void DoTransAttrValue (unsigned char c)
{
  if (ParsedLgBuffer != 0)
    {
      ParsedAttr->IsTransf = TRUE;
      ParsedAttr->AttrTag = (unsigned char *)TtaGetMemory (NAME_LENGTH);
      strcpy ((char *)ParsedAttr->AttrTag, (char *)inputBuffer);
      ParsedLgBuffer = 0;
    }
  else
    {
      ParsedError = TRUE;
      ErrorMessage ((unsigned char *)"Missing Tag Name");
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void DoEndTransAttr (unsigned char c)
{
  if (ParsedLgBuffer != 0)
    {
      ParsedAttr->AttrAttr = (unsigned char *)TtaGetMemory (NAME_LENGTH);
      strcpy ((char *)ParsedAttr->AttrAttr, (char *)inputBuffer);
      if (!strcmp ((char *)ParsedAttr->NameAttr, ""))
	{
	  strcpy ((char *)ParsedAttr->NameAttr, (char *)inputBuffer);
	}
      ParsedLgBuffer = 0;
    }
  else
    {
      ParsedError = TRUE;
      ErrorMessage ((unsigned char *)"Missing Attribute Name");
    }
}

/*----------------------------------------------------------------------
  ParsedPutInBuffer	put character c in the input buffer.		
  ----------------------------------------------------------------------*/
static void DoPutInBuffer (unsigned char c)
{
  int                 len;
  
  if ((ParsedLgBuffer == 0 || inputBuffer[0] != '\"') &&
      (c == ':' || c == ';' || c == '(' || c == ')' || c == '{' || 
       c == '}' || c == '+' || c == ',' || c == '|' || c == '>' || 
       c == '<' || c == '.' || c == '!' || c == '?'))
    {
      ParsedError = TRUE;
      ErrorMessage ((unsigned char *)"Invalid char");
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
	  if (ParsedLgBuffer + len >= MaxBufferLength)
	    {
	      ParsedError = TRUE;
	      ErrorMessage ((unsigned char *)"Buffer overflow - Transformation process");
	      ParsedLgBuffer = 0;
	    }
	  if (len == 1)
	    inputBuffer[ParsedLgBuffer++] = c;
	  else		/* HT */
	    do
	      {
		inputBuffer[ParsedLgBuffer++] = SPACE;
		len--;
	      }
	    while (len > 0);
	}
      inputBuffer[ParsedLgBuffer] = EOS;
    }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void DoStartOfAttrValue (unsigned char c)
{
  if (c == '\"' || c == '\'')
    DoPutInBuffer ('\"');
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void DoEndOfAttrValue (unsigned char c)
{
   ThotBool            isText;
   int                 i, attrVal;

   if (ParsedLgBuffer == 0)
     {
	ParsedAttr->IsInt = FALSE;
	ParsedAttr->TextVal = (unsigned char *)TtaGetMemory (ParsedLgBuffer + 2);

	strcpy ((char *)ParsedAttr->TextVal, "");
     }
   else
     {
	isText = FALSE;
	for (i = 0; !isText && i < ParsedLgBuffer; i++)
	   isText = (inputBuffer[i] < '1' || inputBuffer[i] > '9');
	if (!isText)
	  {
	     ParsedAttr->IsInt = TRUE;
	     ParsedAttr->IntVal = atoi ((char *)inputBuffer);
	  }
	else
	  {
	     attrVal = MapAttrValue (ParsedAttr->ThotAttr, (char *)inputBuffer);
	     if (attrVal != -1)
	       {
		  ParsedAttr->IsInt = TRUE;
		  ParsedAttr->IntVal = attrVal;
	       }
	     else
	       {
		  ParsedAttr->TextVal = (unsigned char *)TtaGetMemory (ParsedLgBuffer + 2);
		  if (inputBuffer [0] == '\"')
		    strcpy ((char *)ParsedAttr->TextVal, (char *)&(inputBuffer[1]));
		  else
		    strcpy ((char *)ParsedAttr->TextVal, (char *)inputBuffer);
		  ParsedAttr->IsInt = FALSE;
	       }
	  }
	ParsedLgBuffer = 0;
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void BeginRules (unsigned char c)
{
  SelRuleFlag = FALSE;
}


/*----------------------------------------------------------------------
  Action name complete
  ----------------------------------------------------------------------*/
static void EndAction (unsigned char c)
{
  ParsedTrans->IsAction = TRUE;
  ParsedTrans->DestinationTag = (unsigned char *)TtaStrdup ((char *)inputBuffer);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void EndLeftPartRule (unsigned char c)
{
   if (ParsedLgBuffer != 0)
     {				/* allocates a new rule descriptor */
	ParsedRule = (strRuleDesc *) TtaGetMemory (sizeof (strRuleDesc));
	ParsedRule->RuleName = (unsigned char *)TtaStrdup ((char *)inputBuffer);
	ParsedRule->NextRule = NULL;
	ParsedRule->Next = NULL;
	ParsedRule->OptionNodes = (strNodeDesc *) TtaGetMemory (sizeof (strNodeDesc));
	ParsedRule->DeleteRule = FALSE;
	ParsedRule->NewNodes = NULL;
	ParsedNode = ParsedRule->OptionNodes;
	ParsedNode->Tag = (unsigned char *)TtaGetMemory (NAME_LENGTH);
	strcpy ((char *)ParsedNode->Tag, "");
	ParsedNode->Attributes = NULL;
	ParsedNode->Next = NULL;
	ParsedLgBuffer = 0;
     }
   else
     {
	ParsedError = TRUE;
	ErrorMessage ((unsigned char *)"Missing left part of rule");
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void DeleteElementRule (unsigned char c)
{
  if (ParsedLgBuffer != 0)
    {
      /* allocates a new rule descriptor */
      ParsedRule = (strRuleDesc *) TtaGetMemory (sizeof (strRuleDesc));
      ParsedRule->RuleName = (unsigned char *)TtaStrdup ((char *)inputBuffer);
      ParsedRule->NextRule = NULL;
      ParsedRule->Next = NULL;
      ParsedRule->NewNodes = NULL;
      ParsedRule->OptionNodes = NULL;
      ParsedLgBuffer = 0;
      ParsedRule->DeleteRule = TRUE;
      ParsedLgBuffer = 0;
    }
  else
    {
      ParsedError = TRUE;
      ErrorMessage ((unsigned char *)"Missing rule name");
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void EndNode (unsigned char c)
{
   char              msgBuffer[MaxBufferLength];
   SSchema	     schema;

   if (ParsedLgBuffer != 0)
     {
	strcpy ((char *)ParsedNode->Tag, (char *)inputBuffer);
	ParsedLgBuffer = 0;
	schema = ParsedTransSet->Schema;
	if (MapGI ((char *)ParsedNode->Tag, &schema, 0) == -1)
	  {
	     ParsedError = TRUE;
	     sprintf (msgBuffer, "unknown tag </%s>", ParsedNode->Tag);
	     ErrorMessage ((unsigned char *)msgBuffer);
	  }
     }
   if (ParsedNode && c == '.')
     {				/* allocate the next node descriptor */
	ParsedNode->Next = (strNodeDesc *) TtaGetMemory (sizeof (strNodeDesc));
	ParsedNode = ParsedNode->Next;
	ParsedNode->Tag = (unsigned char *)TtaGetMemory (NAME_LENGTH);
	strcpy ((char *)ParsedNode->Tag, "");
	ParsedNode->Attributes = NULL;
	ParsedNode->Next = NULL;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void EndOptNodes (unsigned char c)
{
   strAttrDesc		*attr, *attr2;
   char			msgBuffer[MaxBufferLength];
   SSchema		schema;

   if (ParsedLgBuffer != 0)
     {
	strcpy ((char *)ParsedNode->Tag, (char *)inputBuffer);
	ParsedLgBuffer = 0;
	schema = ParsedTransSet->Schema;
	if (MapGI ((char *)ParsedNode->Tag, &schema, 0) == -1)
	  {
	     ParsedError = TRUE;
	     sprintf (msgBuffer, "unknown tag </%s>", ParsedNode->Tag);
	     ErrorMessage ((unsigned char *)msgBuffer);
	  }
     }
   if (!strcmp ((char *)ParsedRule->OptionNodes->Tag, ""))
     {
	/* frees the current node descriptor if it is empty (the rule has no opt. node */
	attr = ParsedRule->OptionNodes->Attributes;
	while (attr)
	  {
	     TtaFreeMemory (attr->NameAttr);
	     if (attr->IsTransf)
	       {
		  TtaFreeMemory (attr->AttrTag);
		  TtaFreeMemory (attr->AttrAttr);
	       }
	     else if (!attr->IsInt)
		TtaFreeMemory (attr->TextVal);
	     attr2 = attr->Next;
	     TtaFreeMemory (attr);
	     attr = attr2;
	  }
	TtaFreeMemory (ParsedRule->OptionNodes->Tag);
	TtaFreeMemory (ParsedRule->OptionNodes);
	ParsedRule->OptionNodes = NULL;
     }
   /* allocate a New node descriptor */
   ParsedRule->NewNodes = (strNodeDesc *) TtaGetMemory (sizeof (strNodeDesc));
   ParsedNode = ParsedRule->NewNodes;
   ParsedNode->Tag = (unsigned char *)TtaGetMemory (NAME_LENGTH);
   strcpy ((char *)ParsedNode->Tag, "");
   ParsedNode->Attributes = NULL;
   ParsedNode->Next = NULL;
}
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void SelectionRule (unsigned char c)
{
  char                   msgBuffer[MaxBufferLength];
  strAttrDesc		*ParsedAttr;

  if (SelRuleFlag)
    {
      sprintf (msgBuffer, "Too much selection rules");
      ErrorMessage ((unsigned char *)msgBuffer);
    }
  else
    {
      SelRuleFlag = TRUE;
      if (ParsedNode != NULL)
	{
	  ParsedAttr = ParsedNode->Attributes;
	  if (ParsedAttr == NULL)
	    {
	      ParsedNode->Attributes = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
	      ParsedAttr = ParsedNode->Attributes;
	      ParsedAttr->NameAttr = (unsigned char *)TtaGetMemory (NAME_LENGTH);
	      ParsedAttr->Next = NULL;
	    }
	  else
	    {
	      while (ParsedAttr->Next && strcmp ((char *)ParsedAttr->NameAttr, (char *)inputBuffer))
		ParsedAttr = ParsedAttr->Next;
	      if (!strcmp ((char *)ParsedAttr->NameAttr, (char *)inputBuffer))
		{
		  ParsedError = TRUE;
		  ErrorMessage ((unsigned char *)"Multi valued attribute");
		}
	      else
		{
		  ParsedAttr->Next = (strAttrDesc *) TtaGetMemory (sizeof (strAttrDesc));
		  ParsedAttr = ParsedAttr->Next;
		  ParsedAttr->NameAttr = (unsigned char *)TtaGetMemory (NAME_LENGTH);
		  ParsedAttr->Next = NULL;
		}
	    }
	  strcpy ((char *)ParsedAttr->NameAttr, "zzghost");
	  ParsedAttr->ThotAttr = HTML_ATTR_Ghost_restruct;
	  ParsedAttr->IsInt = FALSE;
	  ParsedAttr->IsTransf = FALSE;
	  ParsedAttr->TextVal = (unsigned char *)TtaGetMemory (NAME_LENGTH);
    	  strcpy ((char *)ParsedAttr->TextVal, "Select");
	}
    }
}
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void EndRule (unsigned char c)
{
  ThotBool               ok;
  strRuleDesc           *rule;
  strSymbDesc           *symb;
  strNodeDesc           *pnode;
  strAttrDesc           *attr, *attr2;
  char                   msgBuffer[MaxBufferLength];
  SSchema		 schema;

  if (ParsedLgBuffer != 0)
    {
      strcpy ((char *)ParsedNode->Tag, (char *)inputBuffer);
      ParsedLgBuffer = 0;
      schema = ParsedTransSet->Schema;
      if (strcmp ((char *)ParsedNode->Tag, "*") && 
	  strcmp ((char *)ParsedNode->Tag, "#") &&
	  ParsedNode->Tag[0] != '\"' &&
	  (MapGI ((char *)ParsedNode->Tag, &schema, 0) == -1))
	{
	  ParsedError = TRUE;
	  sprintf (msgBuffer, "unknown tag </%s>", ParsedNode->Tag);
	  ErrorMessage ((unsigned char *)msgBuffer);
	}
    }
  if (ParsedRule->OptionNodes && !strcmp ((char *)ParsedRule->OptionNodes->Tag, ""))
    {				/* free the last Option node if it is empty */
      attr = ParsedRule->OptionNodes->Attributes;
      while (attr)
	{
	  TtaFreeMemory (attr->NameAttr);
	  if (attr->IsTransf)
	    {
	      TtaFreeMemory (attr->AttrTag);
	      TtaFreeMemory (attr->AttrAttr);
	    }
	  else if (!attr->IsInt)
	    TtaFreeMemory (attr->TextVal);
	  attr2 = attr->Next;
	  TtaFreeMemory (attr);
	  attr = attr2;
	}
      TtaFreeMemory (ParsedRule->OptionNodes->Tag);
      TtaFreeMemory (ParsedRule->OptionNodes);
      ParsedRule->NewNodes = NULL;
    }

  if (ParsedRule->NewNodes && !strcmp ((char *)ParsedRule->NewNodes->Tag, ""))
    {				/* free the last New node if it is empty */
      attr = ParsedRule->NewNodes->Attributes;
      while (attr)
	{
	  TtaFreeMemory (attr->NameAttr);
	  if (attr->IsTransf)
	    {
	      TtaFreeMemory (attr->AttrTag);
	      TtaFreeMemory (attr->AttrAttr);
	    }
	  else if (!attr->IsInt)
	    TtaFreeMemory (attr->TextVal);
	  attr2 = attr->Next;
	  TtaFreeMemory (attr);
	  attr = attr2;
	}
      TtaFreeMemory (ParsedRule->NewNodes->Tag);
      TtaFreeMemory (ParsedRule->NewNodes);
      ParsedRule->NewNodes = NULL;
    }
  ParsedNode = NULL;

  /* insert the rule descriptor in the transformation descriptor */
  rule = ParsedTrans->Rules;
  if (rule == NULL)
    ParsedTrans->Rules = ParsedRule;
  else
    {
      while (rule->Next)
	rule = rule->Next;
      rule->Next = ParsedRule;
    }
  ParsedTrans->NbRules++;

  /* link the symbols of the pattern to the current rule */
  symb = ParsedTrans->Symbols;
  ok = FALSE;
  while (symb)
    {
      if (!strcmp ((char *)ParsedRule->RuleName, (char *)symb->SymbolName))
	{
	  rule = symb->Rule;
	  if (rule == NULL)
	    symb->Rule = ParsedRule;
	  else
	    {
	      while (rule->NextRule)
		rule = rule->NextRule;
	      rule->NextRule = ParsedRule;
	    }
	  ok = TRUE;
	}
      symb = symb->Next;
    }

  if (ok)
    {
      /* the rule is linked to 1 symbol at least */
      /* check its consistence with the destination type of the current transformation */
      if (ParsedRule->OptionNodes != NULL)
	pnode = ParsedRule->OptionNodes;
      else
	pnode = ParsedRule->NewNodes;
      if (pnode && strcmp ((char *)pnode->Tag, "*") != 0)
	{
	if (ParsedTrans->DestinationTag == NULL)
	  {
	    /* the dest type is undefined => the first tag of the rule defines */
	    /* the destination type of the transformation */
	    ParsedTrans->DestinationTag = (unsigned char *)TtaGetMemory (NAME_LENGTH);
	    strcpy ((char *)ParsedTrans->DestinationTag, (char *)pnode->Tag);
	  }
	else if (strcmp ((char *)ParsedTrans->DestinationTag, (char *)pnode->Tag))
	  /* the first tag of the rule is different from the destination type */
	  /* the rule has no destination type */
	  strcpy ((char *)ParsedTrans->DestinationTag, "");
	}
    }
  else
    {
      ParsedError = TRUE;
      ErrorMessage ((unsigned char *)"undefined pattern symbol");
    }
  ParsedRule = NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void EndTransformation (unsigned char c)
{
   strTransDesc          *trans;

   /* create the pattern virtual root node */
   ParsedTrans->RootDesc = (strSymbDesc *) TtaGetMemory (sizeof (strSymbDesc));
   ParsedTrans->RootDesc->SymbolName = ParsedTrans->NameTrans;
   ParsedTrans->RootDesc->Tag = (unsigned char *)TtaStrdup ((char *)"pattern_root");
   /* warning : the Rule points the transformation record (no rule for the root node) */
   ParsedTrans->RootDesc->Rule = (strRuleDesc *) ParsedTrans;
   ParsedTrans->RootDesc->IsOptional = FALSE;
   ParsedTrans->RootDesc->IsActiveSymb = TRUE;
   ParsedTrans->RootDesc->IsOptChild = FALSE;
   ParsedTrans->RootDesc->Children = ParsedTrans->First;
   ParsedTrans->RootDesc->Followings = NULL;
   ParsedTrans->RootDesc->Next = NULL;

   /* inserts the new transformation in the list of transformations */
   trans = ParsedTransSet->Transformations;
   ParsedTransSet->NbTrans++;
   if (trans == NULL)
      ParsedTransSet->Transformations = ParsedTrans;
   else
     {
	while (trans->Next != NULL)
	   trans = trans->Next;
	trans->Next = ParsedTrans;
     }
   ParsedTrans = NULL;
}

/*----------------------------------------------------------------------
  Do_nothing	Do nothing.				       	
  ----------------------------------------------------------------------*/
static void Do_nothing (char c)
{
}


/* some type definitions for the automaton */
typedef struct _Transition *PtrTransition;
typedef struct _Transition
{
  /* a transition of the automaton in   "executable" form */
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
{
  /* a state of the automaton */
  State               automatonState;	/* the state */
  PtrTransition       firstTransition;	/* first transition from that state */
}
StateDescr;

/* the automaton that drives the HTML parser */
#define MaxState 50
static StateDescr   automaton[MaxState];

typedef struct _sourceTransition
{
  /* a transition of the automaton in "source" form */
  State               initState;	/* initial state of transition */
  char                trigger;	/* the imput character that triggers
				   the transition */
  Proc                transitionAction;	/* the procedure to be called when
					   the transition occurs */
  State               newState;	/* final state of the transition */
}
sourceTransition;

/* the automaton in "source" form */
static sourceTransition ParsedsourceAutomaton[] =
{
  /*
   state   trigger  action  new state
 */
/* state 0: */
   {0, ':', (Proc) EndNameTrans, 1},
   {0, '*', (Proc) DoPutInBuffer, 0},
/*state 1 : */
   {1, 'S', (Proc) Do_nothing, 1},
   {1, '?', (Proc) Option, 1},
   {1, '(', (Proc) BeginExp, 1},
   {1, '<', (Proc) BeginOfTag, 4},
   {1, '*', (Proc) DoPutInBuffer, 2},

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
   {2, '*', (Proc) DoPutInBuffer, 2},
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
   {4, '*', (Proc) DoPutInBuffer, 4},
/* state 5: expexting a space or an end tag */
   {5, 'S', (Proc) Do_nothing, 6},
   {5, '>', (Proc) Do_nothing, 2},
/* state 6: reading an attribute name */
   {6, 'S', (Proc) DoEndOfAttrName, 6},
   {6, '=', (Proc) DoEndOfAttrName, 7},
   {6, '>', (Proc) DoEndOfAttrName, 2},
   {6, '*', (Proc) DoPutInBuffer, 6},
/* state 7: reading an attribute value */
   {7, '\"', (Proc) DoStartOfAttrValue, 8},
   {7, '\'', (Proc) DoStartOfAttrValue, 9},
   {7, 'S', (Proc) DoEndOfAttrValue, 6},
   {7, '>', (Proc) DoEndOfAttrValue, 2},
   {7, '*', (Proc) DoPutInBuffer, 7},
/* state 8: reading an attribute value between double quotes */
   {8, '\"', (Proc) DoEndOfAttrValue, 5},
   {8, '*', (Proc) DoPutInBuffer, 8},
/* state 9: reading an attribute value  between quotes */
   {9, '\'', (Proc) DoEndOfAttrValue, 5},
   {9, '*', (Proc) DoPutInBuffer, 9},
  /* transformation rules */
/* state 10: */
   {10, 'S', (Proc) Do_nothing, 10},
   {10, '{', (Proc) BeginRules, 11},
/* state 11: reading the LeftPartRule */
   {11, '$', (Proc) Do_nothing, 17},
   {11, 'S', (Proc) Do_nothing, 11},
   {11, '>', (Proc) EndLeftPartRule, 12},
   {11, '/', (Proc) DeleteElementRule, 16},
   {11, '*', (Proc) DoPutInBuffer, 11},
/* state 12: reading the position path */
   {12, 'S', (Proc) Do_nothing, 12},
   {12, '<', (Proc) BeginRuleTag, -20},
   {12, ':', (Proc) EndOptNodes, 13},
   {12, '%', (Proc) SelectionRule, 12},
   {12, ';', (Proc) EndRule, 14},
   {12, '.', (Proc) EndNode, 12},
   {12, '*', (Proc) DoPutInBuffer, 12},
/* state 13: reading the generated nodes */
   {13, 'S', (Proc) Do_nothing, 13},
   {13, '<', (Proc) BeginRuleTag, -20},
   {13, '%', (Proc) SelectionRule, 13},
   {13, ';', (Proc) EndRule, 14},
   {13, '\"', (Proc) DoPutInBuffer, 15},
   {13, '.', (Proc) EndNode, 13},
   {13, '*', (Proc) DoPutInBuffer, 13},
/* state 14: waiting for the next rule or the end of rules */
   {14, 'S', (Proc) Do_nothing, 14},
   {14, '}', (Proc) EndTransformation, 0},
   {14, '*', (Proc) DoPutInBuffer, 11},
/* state 15: reading a content string */
   {15, '\"', (Proc) DoPutInBuffer, 13},
   {15, '*', (Proc) DoPutInBuffer, 15},
/* state 16: waiting a ; ending rule */
   {16, ';', (Proc) EndRule, 14},
   {16, 'S', (Proc) Do_nothing, 16},
/* state 17: waiting a ; ending declaration of action */
   {17, ';', (Proc) EndAction, 14},
   {17, 'S', (Proc) Do_nothing, 17},
   {17, '*', (Proc) DoPutInBuffer, 17},

  /*sub automaton for tags  in transformation rules */
/* state 20: a '<' has been read : reading tag name */
   {20, 'S', (Proc) EndRuleTagName, 22},
   {20, '>', (Proc) EndRuleTagName, -1},
   {20, '*', (Proc) DoPutInBuffer, 20},
/* state 21: expexting a space or an end tag */
   {21, 'S', (Proc) Do_nothing, 22},
   {21, '>', (Proc) Do_nothing, -1},
/* state 22: reading an attribute name */
   {22, 'S', (Proc) DoEndRuleAttrName, 22},
   {22, '=', (Proc) DoEndRuleAttrName, 23},
   {22, '.', (Proc) DoTransAttr, 26},
   {22, '>', (Proc) DoEndRuleAttrName, -1},
   {22, '*', (Proc) DoPutInBuffer, 22},
/* state 23: reading an attribute value */
   {23, '\"', (Proc) DoStartOfAttrValue, 24},
   {23, '\'', (Proc) DoStartOfAttrValue, 25},
   {23, 'S', (Proc) DoEndOfAttrValue, 22},
   {23, '>', (Proc) DoEndOfAttrValue, -1},
   {23, '.', (Proc) DoTransAttrValue, 26},
   {23, '*', (Proc) DoPutInBuffer, 23},
/* state 24: reading an attribute value between double quotes */
   {24, '\"', (Proc) DoEndOfAttrValue, 21},
   {24, '*', (Proc) DoPutInBuffer, 24},
/* state 25: reading an attribute value  between quotes */
   {25, '\'', (Proc) DoEndOfAttrValue, 21},
   {25, '*', (Proc) DoPutInBuffer, 25},
/* state 26: reading a transferred attribute name */
   {26, 'S', (Proc) DoEndTransAttr, 22},
   {26, '>', (Proc) DoEndTransAttr, -1},
   {26, '*', (Proc) DoPutInBuffer, 26},


/* st1ate 1000: fictious state. End of automaton table */
/* the next line must be the last one in the automaton declaration */
   {1000, '*', (Proc) Do_nothing, 1000}
};

/*----------------------------------------------------------------------
  DoInitAutomaton	read the "source" form of the automaton and	
  build the "executable" form.			
  ----------------------------------------------------------------------*/
void DoInitAutomaton (void)
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
	theState = ParsedsourceAutomaton[entry].initState;
	if (theState < 1000)
	  {
	     trans = (PtrTransition) TtaGetMemory (sizeof (Transition));
	     trans->nextTransition = NULL;
	     trans->trigger = ParsedsourceAutomaton[entry].trigger;
	     trans->action = ParsedsourceAutomaton[entry].transitionAction;
	     trans->newState = ParsedsourceAutomaton[entry].newState;
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
void FreeTransform (void)
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
static void TRANSparse (BinFile infile)
{
  unsigned char       charRead, oldcharRead;
  ThotBool            match, readOk;
  PtrTransition       trans;
  
  /* initialize automaton */
  /* parse the input file    */
  readOk = FALSE;
  numberOfCharRead = 0;
  numberOfLinesRead = 1;
  CurrentState = 0;
  charRead = EOS;
  readOk = FALSE;
  /* read the file sequentially */
  do
    {
      /* read one character from the input file, if the character read */
      /* previously has already been processed */
      if (charRead == EOS)
        readOk = TtaReadByte(infile,&charRead);
      if (readOk)
        {
          if (charRead == '!' && numberOfCharRead == 0)
            {
              /* reading a comment */
              do
                readOk = TtaReadByte(infile, &charRead);
              while (charRead != EOL && readOk);
            }
        }
      
      if (readOk)
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
          if ((int) charRead < 32 ||
              ((int) charRead >= 127 && (int) charRead <= 143))
            charRead = EOS;
          if (charRead != EOS)
            {
              /* a valid character has been read */
              /* first transition of the automaton for the current state */
              trans = automaton[CurrentState].firstTransition;
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
                        (*(Proc1)(trans->action)) ((void *)(int)charRead);
                      if (normalTransition)
                        {
                          /* the input character has been processed */
                          charRead = EOS;
                          /* the procedure associated with the transition has not */
                          /* changed state explicitely */
                          /* change current automaton state */
                          if (trans->newState >= 0)
                            CurrentState = trans->newState;
                          else if (trans->newState == -1)
                            /* return form subautomaton */
                            CurrentState = ReturnState;
                          else
                            {
                              /* calling a subautomaton */
                              ReturnState = CurrentState;
                              CurrentState = -trans->newState;
                            }
                        }
                      else
                        {
                          /* an error has been found skiParseding the transformation */
                          readOk = TtaReadByte (infile, &oldcharRead);
                          if (readOk)
                            readOk = TtaReadByte (infile, &charRead);
                          while (readOk && (charRead != '}' || oldcharRead != ';'))
                            {
                              oldcharRead = charRead;
                              readOk = TtaReadByte (infile, &charRead);
                              while (readOk  && ((int) charRead == 9 ||
                                                 (int) charRead == 10 || 
                                                 (int) charRead == 12 || 
                                                 (int) charRead == 13 ||
                                                 (int) charRead == 32 ))
                                readOk = TtaReadByte (infile, &charRead);
                            }
                          if (readOk)
                            {
                              if (ParsedTrans)
                                {
#ifdef AMAYA_DEBUG
                                  fprintf (stderr, "skip transformation %s\n", ParsedTrans->NameTrans);
#endif
                                  FreeTrans (ParsedTrans);
                                  ParsedTrans = NULL;
                                }
                              else
#ifdef AMAYA_DEBUG
                                fprintf (stderr, "skip transformation\n");
#endif
                              if (ParsedChoice != NULL)
                                FreeChoice (ParsedChoice);
                              ParsedChoice = NULL;
                              ParsedError = FALSE;
                              CurrentState = 0;
                              ParsedLgBuffer = 0;
                              charRead = EOS;
                              ParsedOptional = FALSE;
                              ParsedIterTag = FALSE;
                              ParsedAttr = NULL;
                              ParsedNode = NULL;
                              ParsedRule = NULL;
                              ParsedIsNamed = FALSE;
                              strcpy ((char *)ParsedName, "");
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
  while (readOk && !ParsedError);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void InitDoParse (void)
{
  strTransDesc          *trans, *trans2;

  /* frees old transformation descriptors */
  trans = ParsedTransSet->Transformations;
  while (trans)
    {
      trans2 = trans->Next;
      FreeTrans (trans);
      trans = trans2;
    }
  ParsedTransSet->NbTrans = 0;
  ParsedTransSet->MaxDepth = 0;
  ParsedTransSet->Transformations = NULL;
}


/*----------------------------------------------------------------------
  DoStartParser loads the file Directory/FileName for parsing	
  ----------------------------------------------------------------------*/
ThotBool DoStartParser (char *name, SSchema tStrSchema, strTransSet **resTrSet)
{
   BinFile             infile = (BinFile)0;
   char                msg[200];
   char	               fileName[MAX_LENGTH];
   char                paths[MAX_LENGTH];
   char               *next, *cour, *lan;
   ThotBool            found = FALSE;
   struct stat        *StatBuffer;
   int                 len, status;

   /* searches if a transformation set is already allocated */
   /* for the file to be parsed */
   ParsedTransSet = strMatchEnv.TransSets;
   while (ParsedTransSet != NULL &&
	  (strcmp ((char *)ParsedTransSet->TransFileName, (char *)name) != 0))
     ParsedTransSet = ParsedTransSet->Next;
   if (ParsedTransSet == NULL)
     {
       ParsedTransSet = (strTransSet*)TtaGetMemory (sizeof (strTransSet));
       strcpy ((char *)ParsedTransSet->TransFileName, (char *)name);
       ParsedTransSet->Schema = tStrSchema;
       ParsedTransSet->timeLastWrite = (time_t) 0;
       ParsedTransSet->NbTrans = 0;
       ParsedTransSet->MaxDepth = 0;
       ParsedTransSet->Transformations = NULL;
       ParsedTransSet->Next = strMatchEnv.TransSets;
       strMatchEnv.TransSets = ParsedTransSet;
     }

   /* build the transformation file name from schema directory and schema name */
   TtaGetSchemaPath (paths, MAX_LENGTH);
   cour = paths;
   while (!found && cour != NULL)
     {
   	next = strchr (cour, PATH_SEP);
	if (next == NULL)
       	  strcpy ((char *)fileName, (char *)cour);
        else
	  {
            strncpy (fileName, cour, (size_t)(next - cour)); 
	    fileName[(next - cour)] = EOS;
          }
        len = strlen (fileName);
        if (fileName[len] != DIR_SEP)
	  /* add a file separator */
          strcat (fileName, DIR_STR);
        strcat (fileName,name);
        strcat (fileName, ".trans");
        len = strlen (fileName);
	strcat (fileName, ".");
	lan = TtaGetVarLANG ();
        strcat (fileName, lan);
	if (!TtaFileExist(fileName))
	  /* remove the language specificity */
	  fileName[len] = EOS;
	found = TtaFileExist(fileName);
	if (!found)
	  {
	   if (next == NULL)
	      cour = NULL;
	   else
	      cour = next + 1;
	  }
     }
   
   /* check if the file is newer than last read */
   StatBuffer = (struct stat *) TtaGetMemory (sizeof (struct stat));
   
   status = stat (fileName, StatBuffer);
   if (status != -1)
     {
	if (StatBuffer->st_mtime == ParsedTransSet->timeLastWrite)
	/* the file is unchanged, activing all the transformations */
	   SetTransValid (ParsedTransSet);
	else
	  {
	    /* the file xxx.trans has been touched, parsing it */
	     ParsedTransSet->timeLastWrite = StatBuffer->st_mtime;
	     infile = TtaReadOpen(fileName);
	   if (infile == 0)
	     {
		sprintf (msg, "Can't open file %s.trans", name);
		ErrorMessage ((unsigned char *)msg);
		ParsedError = TRUE;
	     }
	   else
	     {
		InitDoParse ();
		ParsedError = FALSE;
		ParsedOptional = FALSE;
		ParsedIterTag = FALSE;
		ParsedAttr = NULL;
		ParsedNode = NULL;
		ParsedRule = NULL;
		ParsedIsNamed = FALSE;
		strcpy ((char *)ParsedName, "");
		opStack[0] = EOS;
		symbolStack[0] = NULL;
		choiceStack[0] = NULL;
		sizeStack = 1;
		inputBuffer[0] = EOS;	/* initialize input buffer */
		ParsedLgBuffer = 0;
		TRANSparse (infile);
	        TtaReadClose (infile);
	      }
          }
     }
   TtaFreeMemory (StatBuffer);
   if (!ParsedError)
     *resTrSet = ParsedTransSet;
   else
     *resTrSet = NULL;
   return !ParsedError;
}
