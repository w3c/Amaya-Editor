/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _TRANS_H__
#define _TRANS_H__
/*----------------------------------------------------------------------
     Definitions of types and variables used in type transformation (trans.c and transparse.c)
     Stephane Bonhomme Apr 96   
  ----------------------------------------------------------------------*/
#define MAX_STACK 100       /* size of html generation and pattern matching stacks */
#define BUFFER_LEN 10000     /* size of html buffer */

#ifndef PPSTANDALONE

/*----------------------------------------------------------------------
    structure definitions
  ----------------------------------------------------------------------*/
/*relation between pattern nodes (also called Symbols) and source structure tree nodes*/
typedef struct _MatchChildren
  {
     struct _SymbDesc   *MatchSymb;	/* pattern symbol */
     struct _Node       *MatchNode;	/* source tree node */
     struct _MatchChildren *Next;
  }
strMatchChildren;

typedef struct _Match
  {
     struct _SymbDesc   *MatchSymb;	/* pattern symbol */
     struct _Node       *MatchNode;	/* source tree node */
     strMatchChildren   *MatchChildren;	/* relation between chidren in both pattern and source tree */
     struct _Match   *Next;
  }
strMatch;


/* Source structure tree nodes definition */
typedef struct _Node
  {
     char               *Tag;		/* HTML tag */
     Element             Elem;		/* element instance */
     struct _Node       *Parent;	
     struct _Node       *Child;
     struct _Node       *Next;
     struct _Node       *Previous;
     int                 NodeDepth;
     boolean             IsTrans;
     struct _SymbDesc   *MatchSymb;	/* symbol matched (transformation phase) */
     strMatch           *Matches;	/* Symbols Matched (pattern matching) */
     struct _ListSymb   *Candidates;	/* list of symbols potientally matched */
    }
strNode;

typedef strNode   *StructureTree;

#endif

/* internal reprensentation of transformation rules */

/* Attribute descriptor */
typedef struct _AttrDesc
  {
     char               *NameAttr;
     int                 ThotAttr;
     boolean             IsInt;
     boolean             IsTransf;
     union
       {
	  char               *_TextVal;
	  int                 _IntVal;
	  struct _s0
	    {
	       char               *_Tag;
	       char               *_Attr;
	    }
	  s0;
       }
     u;
     struct _AttrDesc   *Next;
  }
strAttrDesc;

#define TextVal u._TextVal
#define IntVal u._IntVal
#define AttrTag u.s0._Tag
#define AttrAttr u.s0._Attr

/* node generated */
typedef struct _NodeDesc
  {
     char               *Tag;
     strAttrDesc        *Attributes;
     struct _NodeDesc   *Next;
  }
strNodeDesc;

typedef struct _RuleDesc
  {
     char               *RuleName;
     strNodeDesc        *OptionNodes;
     strNodeDesc        *NewNodes;
     boolean		DeleteRule;
     struct _RuleDesc   *NextRule;
     struct _RuleDesc   *Next;
  }
strRuleDesc;

#ifndef PPSTANDALONE
typedef struct _ListElem
  {
     Element             Elem;
     int                 Id;
     int                 Rank;
     struct _ListElem   *Next;
  }
strListElem;

#endif

typedef struct _ListSymb
  {
     struct _SymbDesc   *Symbol;
     struct _ListSymb   *Next;
  }
strListSymb;

/* pattern node (symbol) */
typedef struct _SymbDesc
  {
     char               *SymbolName;
     char               *Tag;
     strRuleDesc        *Rule;
     boolean             IsOptional;
     boolean             IsActiveSymb;
     boolean             IsOptChild;
     int                 Depth;
     strAttrDesc        *Attributes;
     strListSymb        *Children;
     strListSymb        *Followings;
     struct _SymbDesc   *Next;
  }
strSymbDesc;

/* transformation descriptor */
typedef struct _TransDesc
  {
     char               *NameTrans;
     int                 NbPatSymb;
     int                 NbRules;
     int                 PatDepth;
     strListSymb        *First;
     strSymbDesc        *RootDesc;
     strSymbDesc        *Symbols;
     strRuleDesc        *Rules;
     boolean             IsActiveTrans;
     char               *DestinationTag;
     struct _TransDesc  *Next;
  }
strTransDesc;

typedef struct _TransSet
{
  /* name of the transformation set (eg, name of file without suffix .trans) */
  char		      TransFileName [20];
#ifndef PPSTANDALONE
  SSchema	      Schema;
#endif
  time_t	      timeLastWrite;
  /* number of transformations */
  int                 NbTrans;
  /* patterns max depth */
  int                 MaxDepth;
  /* transformation descriptor list */
  strTransDesc       *Transformations;
  /* next set */
  struct _TransSet   *Next;
}
strTransSet;

/* transformation environement */
struct _strMatchEnv
{
#ifndef PPSTANDALONE
  StructureTree       SourceTree;
  strListElem        *ListSubTrees;
#endif
  strTransSet	     *TransSets; 
}
strMatchEnv;




#ifndef PPSTANDALONE
/* document to wich a transformation is to be applied */
Document            TransDoc;

/* dialog and messages */
int                 TransBaseDialog;
int                 TRANSDIAL;

#define TR_TRANSFORM 0
#define TRANS_MSG_MAX 1
#define TransMenu 1
#define MAX_TRANS_DLG 2

#endif
#endif /* _TRANS_H__ */
