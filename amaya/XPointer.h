#ifndef AMAYA_XPOINTER_H
#define AMAYA_XPOINTER_H

/***************************************************
  General definitions
**************************************************/

/* the different types of nodes */
typedef enum _nodeType {
  SINGLE_NODE=0x1,     /* XPointer is a single node */
  RANGE_TO=0x2,        /* XPointer is a node set (two nodes */
  STRING_RANGE=0x4,    /* XPointer node points to a string */
  START_POINT=0x8,     /* XPointer node points to a string */
  END_POINT=0x10       /* XPointer node points to a string */

} nodeType;

/***************************************************
  Symbol table  (move this back to XPointerparse.c)
***************************************************/

#define STRMAX 999 /* size of lexemes array */
#define SYMMAX 100 /* size of symtable */

/* forms of symbol table entry */
struct entry { 
  const char *lexptr;
  int token;
};

/* tokens */
typedef enum {
  /* type identifiers */
  NUM=256,
  ID,
  /* the xpointer functions */
  FID,            
  FRANGE_TO,
  FSTRING_RANGE,
  FSTARTP,
  FENDP,
  /* non-operators */
  DONE,
  NONE
} tokEnum;

/* the symbol table */
typedef struct _symTableCtx {
  char lexemes[STRMAX];  /* lexemes table */
  int lastchar;          /* last used position in lexemes */
  struct entry symtable[SYMMAX]; 
  int lastentry;        /* last entry in the symtable */
} symTableCtx;

typedef symTableCtx *symTableCtxPtr;

/***************************************************
  The parser context
**************************************************/

/* the node info that the parsing will return */
typedef struct _nodeInfo {
  Element  el;
  char *node;      /* temporary buffer for storing a node's name */
  int  nodeSize;   /* size of the above temporary buffer */
  nodeType type;
  int  startC;
  int  endC;
  int  index;
  int  processed;
} nodeInfo;

typedef struct _XPointerContext {
  Document doc; /* document where we are resolving the XPointer */
  Element root; /* root element of the document */

  char *cur;    /* the current char being parsed */
  char *buffer; /* the input string */

  char *error;    /* error code */

  int   lookahead;  /* the lookahead token */
  tokEnum tok;      /* token */
  int tokval;       /* token value */

  symTableCtxPtr symtable; /* the symbol table */
  
  nodeType   type;        /* 0, single node, 1 range-to */
  nodeInfo  *curNode;
  nodeInfo   nodeStart;
  nodeInfo   nodeEnd;
} XPointerContext;

typedef XPointerContext *XPointerContextPtr;

#endif /* AMAYA_XPOINTER_H */



