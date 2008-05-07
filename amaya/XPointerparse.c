/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 2000-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * XPointerparser.c : contains all the functions for parsing an XPointer
 * expression and returning the equivalent set of nodes in a Thot tree.
 *
 * Author: J. Kahan
 *
 * Status:
 *
 *   Experimental, only used with annotations for the moment.
 *   Not all of XPath expressions are supported yet.
 *
 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "XPointer.h"
#include "XPointer_f.h"
#undef THOT_EXPORT
#include "XPointerparse_f.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* To turn on debugging */
/* #define XPTR_ACTION_DEBUG */
/* #define XPTR_PARSE_DEBUG */

#define BSIZE 128       /* size of the lexical analyzer temporary buffer */

#define INC_CUR ctx->cur++
#define VAL_CUR *(ctx->cur)

/***************************************************
  Symbol table  (move this back to XPointerparse.c)
***************************************************/
/* values for pre-initializing the symbol tables */
static struct entry keywords[] = {
  {"id", FID},
  {"range-to", FRANGE_TO},
  {"string-range", FSTRING_RANGE},
  {"start-point", FSTARTP},
  {"end-point", FENDP},
  {"", 0}
};

static symTableCtx symtable;

/***************************************************
 local function prototypes 
***************************************************/
static void Expr (XPointerContextPtr ctx);
static void Term (XPointerContextPtr ctx);
static void Factor (XPointerContextPtr ctx);

/*----------------------------------------------------------------------
  CtxAddError
  puts an error code in the context
  ----------------------------------------------------------------------*/
static void CtxAddError (XPointerContextPtr ctx, const char *msg)
{
  if (!ctx->error)
    ctx->error = TtaStrdup (msg);
}

/****************
  Symbol table functions
 ***************/

/*----------------------------------------------------------------------
  LookupSymbol
  ----------------------------------------------------------------------*/
int LookupSymbol (XPointerContextPtr ctx, char *s)
{
  symTableCtxPtr me = ctx->symtable;

  int p;
  for (p = me->lastentry; p > 0; p = p - 1)
    if (strcmp (me->symtable[p].lexptr, s) == 0)
      return p;
  return 0;
}

/*----------------------------------------------------------------------
  InsertSymbol
  ----------------------------------------------------------------------*/
static int InsertSymbol (XPointerContextPtr ctx, const char *s, int tok)
{
  int len;
  symTableCtx *me = ctx->symtable;

  if (ctx->error)
    return DONE;

  len = strlen (s); /* strlen computes length of s */
  if (me->lastentry + 1 >= SYMMAX)
    {
      CtxAddError (ctx, "Parser: symbol table full");
      return DONE;
    }
  if (me->lastchar + len + 1 >= STRMAX)
    {
      CtxAddError (ctx, "Parser: lexemes table full");
      return DONE;
    }
  me->lastentry = me->lastentry + 1;
  me->symtable[me->lastentry].token = tok;
  me->symtable[me->lastentry].lexptr = &(me->lexemes[me->lastchar + 1]);
  me->lastchar = me->lastchar + len + 1;
  strcpy ((char*)me->symtable[me->lastentry].lexptr, s);

  return me->lastentry;
}

/************************************************** 
  Actions
 **************************************************/

static void AddChild (XPointerContextPtr ctx, const char *node)
{
  nodeInfo *curNode = ctx->curNode;

#ifdef XPTR_ACTION_DEBUG
  printf ("Adding child %s\n", node);
#endif

  if ((unsigned) curNode->nodeSize < strlen (node) + 1)
    {
      if (curNode->node == NULL)
	{
	  curNode->nodeSize = 30 + strlen (node) * 2 + 1;
	  curNode->node = (char *) TtaGetMemory (sizeof (char) * curNode->nodeSize);
	}
      else
	{
	  curNode->nodeSize = curNode->nodeSize + strlen (node) * 2 + 1;
	  /* we do a free, rather than a calloc because we're not interested
	     in keeping whatever was stored in this buffer */
	  TtaFreeMemory (curNode->node);
	  curNode->node =  (char *) TtaGetMemory (sizeof (char) * (curNode->nodeSize));
	}
    }
  strcpy (curNode->node, node);
}

static void AddIndex (XPointerContextPtr ctx, int index)
{
  nodeInfo *curNode = ctx->curNode;

#ifdef XPTR_ACTION_DEBUG
  printf ("Adding index %d\n", index);
#endif	  
  curNode->index = index;
}

static void GotoChild (XPointerContextPtr ctx)
{
  nodeInfo *curNode = ctx->curNode;

  if (curNode->processed)
    {
#ifdef XPTR_ACTION_DEBUG
      printf ("Child already processed\n");
#endif	        
      return;
    }

  if (curNode->node && curNode->node[0])
    {
#ifdef XPTR_ACTION_DEBUG
      printf ("Going to child: %s, index %d\n", curNode->node, curNode->index);
#endif /* XPTR_ACTION_DEBUG */
    }
  else
    {
#ifdef XPTR_ACTION_DEBUG
      printf ("First node, nothing in stack yet, ignoring it\n");
#endif /* XPTR_ACTION_DEBUG */
      return;
    }

  /* thot search for element with this nameid */
  if (!curNode->el)
    curNode->el = ctx->root;
  else
    curNode->el = TtaGetFirstChild (curNode->el);

    curNode->el = SearchSiblingIndex (curNode->el, curNode->node, 
				      &(curNode->index));
  if (!curNode->el)
    CtxAddError (ctx, "GotoChild: no such node");
}

static void GotoId (XPointerContextPtr ctx, const char *id)
{
  nodeInfo *curNode = ctx->curNode;
  
#ifdef XPTR_ACTION_DEBUG
  printf ("Going to id: %s\n", id);
#endif

  /* syntax analysis */
  if (curNode->el)
    {
      CtxAddError (ctx, "GotoId: id function is not at the beginning of the expression");
      return;
    }

  /* thot tree search for id */
  curNode->el = SearchAttrId (ctx->root, id);
  if (!curNode->el)
    CtxAddError (ctx, "GotoId: no such id\n");
}

static void RangeTo (XPointerContextPtr ctx)
{
  nodeInfo *curNode = ctx->curNode;
  
#ifdef XPTR_ACTION_DEBUG
  printf ("RangeTo: clearing curNode\n");
#endif

  if (!curNode || curNode->el == 0 || curNode == &(ctx->nodeEnd))
    /* @@ need to add other error codes */
    CtxAddError (ctx, "RangeTo: no existing content");
  else
    {
      ctx->type = RANGE_TO;
      /* point to the context of the second node */
      curNode->processed = 1;
      ctx->curNode = &(ctx->nodeEnd);
    }
}

static void StringRange (XPointerContextPtr ctx, int startC, int len)
{
  int pos = startC;

  nodeInfo *curNode = ctx->curNode;

  if (!curNode || curNode->el == 0)
    {
      CtxAddError (ctx, "StringRange: no location");
      return;
    }
  
  GotoChild (ctx);
  if (!ctx->error)
    {
      if (!SearchTextPosition (&(curNode->el), &pos))
	CtxAddError (ctx, "StringRange: couldn't find text position");
      else
	{
	  curNode->processed = 1;
	  curNode->startC = pos;
	  if (len > 1)
	    curNode->endC = pos + len;
	  else
	    curNode->endC = curNode->startC - 1;
	  curNode->type = (nodeType)((int)curNode->type | STRING_RANGE);
	}
    }
  
#ifdef XPTR_ACTION_DEBUG
  printf ("StringRange: adding info startc %d, endc %d\n", 
	  startC, startC + len);
#endif
}

static void StartPoint (XPointerContextPtr ctx)
{
  nodeInfo *curNode = ctx->curNode;

  curNode->type = (nodeType)((int)curNode->type | START_POINT);
  curNode->endC = curNode->startC - 1;
}

static void EndPoint (XPointerContextPtr ctx)
{
  nodeInfo *curNode = ctx->curNode;

  curNode->type = (nodeType)((int)curNode->type | END_POINT);
  curNode->startC++;
  curNode->endC = curNode->startC;
}

/************************************************** 
  Lexical analyzer functions
 **************************************************/

/*----------------------------------------------------------------------
  IsValidName
  returns true if an identifier name can start with this character.
  ----------------------------------------------------------------------*/
static int IsValidName (char c)
{
  if (isalpha (c)
      || c == '_'
      || c == ':')
    return 1;
  else
    return 0;
}


/*----------------------------------------------------------------------
  IsValidChar
  returns true if the character is a valid identifier one.
  ----------------------------------------------------------------------*/
static int IsValidChar (char c)
{
  if (c == '/'
      || c == '['
      || c == ']'
      || c == '('
      || c == ')'
      || c == ','
      || c == '"')
    return 0;
  else
    return 1;
}

/*----------------------------------------------------------------------
  LexAn
  The lexical analyzer
  ----------------------------------------------------------------------*/
static int LexAn (XPointerContextPtr ctx) 
{
  char lexbuf[BSIZE];

  /* convert symbol to token */
  while (1)
    {
      if (VAL_CUR == EOS || ctx->error)
	return DONE;
      else if (VAL_CUR == ' ' || VAL_CUR == '\t')
	{ 
	  /* strip out white space */
	  INC_CUR;
	  continue;
	}

      else if (isdigit (VAL_CUR))  
	{ 
	  /* *s is a digit */
	  ctx->tokval = 0;
	  while (VAL_CUR && isdigit (VAL_CUR))
	    {
	      ctx->tokval *= 10;
	      ctx->tokval += VAL_CUR - '0';
	      INC_CUR;
	    }
	  return NUM;
	}
	  
      else if (IsValidName (VAL_CUR))
	{
	  /* *s is a letter */
	  int p, b = 0;
	  while (VAL_CUR)
	    {
	      /* () must be escaped with a ^, ^^ escapes ^ */
	      if (VAL_CUR == '^')
		{
		  if (*(ctx->cur+1) == '('
		      || *(ctx->cur+1) == ')'
		      || *(ctx->cur+1) == '^')
		    INC_CUR;
		  else
		    {
		      CtxAddError (ctx, "LexAn: syntax error in expression");
		      return DONE;
		    }
		}
	      else if (!IsValidChar (VAL_CUR))
		break;

	      /* *s is alphanumeric */
	      /* @@ the test should rather be all valid xml chars, and
		 also escape characters */
	      lexbuf[b] = VAL_CUR;
	      INC_CUR;
	      b++;
	      if (b >= BSIZE)
		{
		CtxAddError (ctx, "LexAn: temp buffer out of space");
		return NONE;
		}
	    }
	  lexbuf[b] = EOS;
	  p = LookupSymbol (ctx, lexbuf);
	  if (p == 0)
	    p = InsertSymbol (ctx, lexbuf, ID);
	  ctx->tokval = p;
	  return ctx->symtable->symtable[p].token;
	}
      else 
	{
	  /* s is either a valid separator or an invalid character */
	  /* @@ this case should just return the char */
	  int i;
	  switch (VAL_CUR) 
	    {
	    case '/':
	    case '[':
	    case ']':
	    case '(':
	    case ')':
	    case ',':
	    case '"':
	      i = VAL_CUR;
	      INC_CUR;
	      ctx->tokval = NONE;
	      return (i);
	      break;
	    default:
	      CtxAddError (ctx, "LexAn : syntax error");
              return NONE;			 
	    }
	}
    }
}

/*----------------------------------------------------------------------
  Match
  ----------------------------------------------------------------------*/
static void Match (XPointerContextPtr ctx, int t)
{
  if (ctx->error)
    return;

  if (ctx->lookahead == t)
    ctx->lookahead = LexAn (ctx);
  else
    CtxAddError(ctx, "match : syntax error");
}

/*----------------------------------------------------------------------
  Term
  ----------------------------------------------------------------------*/
static void Term (XPointerContextPtr ctx)
{
  int i;

  while (1)
    {
      if (ctx->error)
	return;

      switch (ctx->lookahead) 
	{
	case '/': 		  /* child sequence */
	  Match (ctx, '/'); 
#ifdef XPTR_PARSE_DEBUG
	  printf ("\nchild\n");
#endif	  
	  GotoChild (ctx);
	  break;

	case (ID):
	  i = ctx->tokval;
#ifdef XPTR_PARSE_DEBUG
	  printf ("element %s found\n", 
		  ctx->symtable->symtable[i].lexptr);
#endif
	  Match (ctx, ID);
	  AddChild (ctx, ctx->symtable->symtable[i].lexptr);
	  break;

	case (NUM):
#ifdef XPTR_PARSE_DEBUG
	  printf ("child number %d found\n", ctx->tokval);	      
#endif
	  Match (ctx, NUM);
	  break;

	case '[':        /* [index counter] */
	  Match (ctx, '['); 
	  i = ctx->tokval;
	  Match (ctx, NUM);
	  Match (ctx, ']');
#ifdef XPTR_PARSE_DEBUG
	  printf ("index %d\n", i);
#endif
	  AddIndex (ctx, i);
	  break;
	  
	case FID:       /* id("name") */
	  Match (ctx, FID);
	  Match (ctx, '(');
	  Match (ctx, '\"'); 
	  i = ctx->tokval;
	  Match (ctx, ID); 
	  Match (ctx, '\"');
	  Match (ctx, ')');
#ifdef XPTR_PARSE_DEBUG
	  printf ("id %s\n", ctx->symtable->symtable[i].lexptr);
#endif
	  GotoId (ctx, ctx->symtable->symtable[i].lexptr);
	  break;

	case FSTARTP:       /* start-point(point) */
#ifdef XPTR_PARSE_DEBUG
	  printf ("beginning of start-point function\n");
#endif
	  Match (ctx, FSTARTP);
	  Match (ctx, '(');
	  Factor (ctx);
	  Match (ctx, ')');
	  StartPoint (ctx);
#ifdef XPTR_PARSE_DEBUG
	  printf ("end of start-point function\n");
#endif
	  break;

	case FENDP:       /* end-point(point) */
#ifdef XPTR_PARSE_DEBUG
	  printf ("begining of end-point function\n");
#endif
	  Match (ctx, FENDP);
	  Match (ctx, '(');
	  Factor (ctx);
	  Match (ctx, ')');
	  EndPoint (ctx);
#ifdef XPTR_PARSE_DEBUG
	  printf ("end of end-point function\n");
#endif
	  break;

	default:
	  return;
	}
    }
}

/*----------------------------------------------------------------------
  Factor
  ----------------------------------------------------------------------*/
static void Factor (XPointerContextPtr ctx)
{
  int startC;
  int len;

  Term (ctx);
  while (1)
    {
      if (ctx->error)
	return;

      switch (ctx->lookahead)
	{
	case FSTRING_RANGE: /* string-range (expr, "", [NUM, [NUM]]) */
#ifdef XPTR_PARSE_DEBUG
	  printf ("start of string-range\n");
#endif
	  startC = 0;
	  len = 0;

	  Match (ctx, FSTRING_RANGE);
	  Match (ctx, '('); 
	  /* expr (arg1) */
	  if (ctx->lookahead == FSTRING_RANGE)
	    Factor (ctx);
	  else
	    Term (ctx);
	  /* string (arg2) */
	  if (ctx->lookahead == ',')
	    {
	      Match (ctx, ',');
	      Match (ctx, '"');
	      if (ctx->lookahead == ID)
		{
#ifdef XPTR_PARSE_DEBUG
		  printf ("string is %s\n", 
			  ctx->symtable->symtable[ctx->tokval].lexptr);
#endif
		  Match (ctx, ID);
		}
	      else
		{
#ifdef XPTR_PARSE_DEBUG
		  printf ("string is empty\n");
#endif
		}
	      Match (ctx, '"');
	    }
	  /* starting char (arg3) */
	  if (ctx->lookahead == ',')
	    {
	      Match (ctx, ',');
	      startC= ctx->tokval;
	      Match (ctx, NUM);
#ifdef XPTR_PARSE_DEBUG
	      printf ("string starts at char: %d\n", startC);
#endif
	    }
	  /* range length (arg4) */
	  if (ctx->lookahead == ',')
	    {
	      Match (ctx, ',');
	      len = ctx->tokval;
	      Match (ctx, NUM);
#ifdef XPTR_PARSE_DEBUG
	      printf ("string len is: %d\n", len);
#endif
	    }
	  Match (ctx, ')');
#ifdef XPTR_PARSE_DEBUG
	  printf ("end of string-range\n");
#endif
	  StringRange (ctx, startC, len);
	  Term (ctx);
	  break;
	  
	default:
	  return;
	}
    }
}

/*----------------------------------------------------------------------
  Expr
  ----------------------------------------------------------------------*/
static void Expr (XPointerContextPtr ctx)
{
  Factor (ctx);

  while (1)
    {
      if (ctx->error)
	return;

      switch (ctx->lookahead)
	{
	case FRANGE_TO:    /* range-to (expr) */
#ifdef XPTR_PARSE_DEBUG
	  printf ("start of range-to\n");
#endif
	  RangeTo (ctx);
	  Match (ctx, FRANGE_TO);
	  Match (ctx, '('); Factor (ctx); Match (ctx, ')');
#ifdef XPTR_PARSE_DEBUG
	  printf ("end of range-to\n");
#endif
	  GotoChild (ctx);
	  ctx->curNode->processed = 1;
	  break;

	case DONE:
	  GotoChild (ctx);
	  return;
	  break;

	default:
	  CtxAddError (ctx, "Expr: unknown expression");
	  break;
	}
    }
}

/*----------------------------------------------------------------------
  InitSymTable
  ----------------------------------------------------------------------*/
static void InitSymtable (XPointerContextPtr ctx)
{
  struct entry *p;

  /* init the symbol table */
  for (p = (struct entry *) keywords; p->token; p++)
    InsertSymbol (ctx, p->lexptr, p->token);
}

/**************************************************
  PUBLIC API 
**************************************************/

/*----------------------------------------------------------------------
  SelectNode
  Initialize a selection
  ----------------------------------------------------------------------*/
static void SelectNode (Document doc, nodeInfo *node)
{
  if (node->type & STRING_RANGE)
    TtaSelectString (doc, node->el, node->startC, node->endC);
  else
    TtaSelectElement (doc, node->el);
}

/*----------------------------------------------------------------------
  SelectToNode
  Extend a selection
  ----------------------------------------------------------------------*/
static void SelectToNode (Document doc, nodeInfo *node)
{
  TtaExtendSelection (doc, node->el, node->endC);
}

/*----------------------------------------------------------------------
  XPointer_select
  ----------------------------------------------------------------------*/
void XPointer_select (XPointerContextPtr ctx)
{
  Element root;

  if (!ctx || ctx->error)
    return;

  if (TtaGetSelectedDocument () == ctx->doc)
    TtaUnselect (ctx->doc);

  root = TtaGetRootElement (ctx->doc);
  /* only do the selection if it's not the root */
  if (ctx->nodeStart.el != root)
    {
      SelectNode (ctx->doc, &(ctx->nodeStart));
      if (ctx->type == RANGE_TO)
	SelectToNode (ctx->doc, &(ctx->nodeEnd));
    }
}

/*----------------------------------------------------------------------
  XPointer_free
  ----------------------------------------------------------------------*/
void XPointer_free (XPointerContextPtr ctx)
{
  if (!ctx)
    return;
  
  if (ctx->error)
    TtaFreeMemory (ctx->error);
  if (ctx->nodeStart.node)
    TtaFreeMemory (ctx->nodeStart.node);
  if (ctx->nodeEnd.node)
    TtaFreeMemory (ctx->nodeEnd.node);

  TtaFreeMemory (ctx);
}


/*----------------------------------------------------------------------
  XPointer_isRangeTo
  ----------------------------------------------------------------------*/
ThotBool XPointer_isRangeTo (XPointerContextPtr ctx)
{
  if (!ctx)
    return FALSE;

  return (ctx->type == RANGE_TO);
}

/*----------------------------------------------------------------------
  XPointer_isStringRange
  ----------------------------------------------------------------------*/
ThotBool XPointer_isStringRange (nodeInfo *node)
{
  if (!node)
    return FALSE;

  return (node->type & STRING_RANGE) != 0;
}

/*----------------------------------------------------------------------
  XPointer_nodeStart
  ----------------------------------------------------------------------*/
nodeInfo *XPointer_nodeStart (XPointerContextPtr ctx)
{
  if (!ctx)
    return NULL;

  return (&(ctx->nodeStart));
}

/*----------------------------------------------------------------------
  XPointer_nodeEnd
  ----------------------------------------------------------------------*/
nodeInfo *XPointer_nodeEnd (XPointerContextPtr ctx)
{
  if (!ctx)
    return NULL;

  return (&(ctx->nodeEnd));
}

/*----------------------------------------------------------------------
  XPointer_el
  ----------------------------------------------------------------------*/
Element XPointer_el (nodeInfo *node)
{
  if (!node)
    return NULL;

  return (node->el);
}

/*----------------------------------------------------------------------
  XPointer_startC
  ----------------------------------------------------------------------*/
int XPointer_startC (nodeInfo *node)
{
  if (!node)
    return 0;

  return (node->startC);
}

/*----------------------------------------------------------------------
  XPointer_endC
  ----------------------------------------------------------------------*/
int XPointer_endC (nodeInfo *node)
{
  if (!node)
    return 0;

  return (node->endC);
}

/*----------------------------------------------------------------------
  XPointer_parse
  ----------------------------------------------------------------------*/
XPointerContextPtr XPointer_parse (Document doc, char *buffer) 
{
  XPointerContextPtr context;
  
  /* init the context */
  context = (XPointerContextPtr) TtaGetMemory (sizeof (XPointerContext));
  memset (context, 0, sizeof (XPointerContext));

  /* verify the schema */
  if (!buffer || buffer[0] == EOS)
    {
      CtxAddError (context, "Empty buffer");
      return (context);
    }

  /* verify the schema */
  if (strncmp (buffer, "xpointer(", 9))
    {
      CtxAddError (context, "Unknown XPointer schema");
      return (context);
    }

  /* point to the buffer (while removing the schema) */
  context->buffer = buffer+9;
  buffer[strlen (buffer) - 1] = EOS;
  context->cur = context->buffer;

  /* symtable (in case one day we want to use dynamically 
     allocated symtables) */
  symtable.lastchar = -1;
  symtable.lastentry = 0;
  context->symtable = &symtable;
  InitSymtable (context);

  /* point to the first node */
  context->curNode = &(context->nodeStart);
  /* and initialize the document root */
  context->doc = doc;
  context->root = AGetRootElement (doc);

  /* start parsing */
  context->lookahead = LexAn (context);
  while (context->lookahead != DONE && !context->error)
    Expr (context);

  /* restore the buffer */
  buffer[strlen (buffer)] = ')';
  /* print parse results */

#ifdef XPTR_ACTION_DEBUG
  if (context->error)
      printf ("Error: %s\n", context->error);
  else
    {
      if (context->nodeStart.el)
	printf ("el 1 is %p\n", context->nodeStart.el);
      if (context->nodeEnd.el)
	printf ("el 2 is %p\n", context->nodeEnd.el);
    }
  XPointer_select (context);
#endif /*  XPTR_ACTION_DEBUG */

  return (context);
}


#ifdef XPTR_PARSE_DEBUG
int main(void) 
{
  char str[500];

  while (1)
    {
      printf("\nenter expression (q to quit):\n");
      /* scan for next symbol */
      gets(str);
      if (!str || *str == 'q')
	exit(0);
      XPointer_Parse (str);
    }
  return 0;
}
#endif /* XPTR_PARSE_DEBUG */
