/* definition des types utilises par la transformation de structure */
/* Stephane Bonhomme Apr 96 */
#define MaxSizePat  500
#define MaxWidthPat 50
#define MaxPatterns 50
#define MAXSTACK 500
#define LGBUFFER 5000

#ifndef PPSTANDALONE


/* definition de la structure de l'arbre sujet */
/* liste chainee des symboles correspondant a chaque fils */
typedef struct _strmatchchild
{
  struct _SymbDesc *patsymb; /* symbole de la pattern matche */
  struct _Tagnode *node;    /* noeud de l'arbre sujet */
  struct _strmatchchild *next;  
} strmatchchildren;
 
typedef struct _strmatch
{
  struct _SymbDesc *patsymb; /* symbole de la pattern matche */
  struct _Tagnode *node;    /* noeud de l'arbre sujet */
  strmatchchildren *childmatches;
  struct _strmatch *next;  
} strmatch;


/* definition d'un noeud de l'arbre sujet */
typedef struct _Tagnode
{
  char *tag;			
  Element element;
  struct _Tagnode *parent;
  struct _Tagnode *child;
  struct _Tagnode *next;
  struct _Tagnode *prev;
  boolean isTrans;
  struct _SymbDesc *transsymb;
  strmatch *matches;
  /* tableau des symboles candidats au resultat, associes aux symboles */
  /*correspondant a un ss ensemble des fils du noeud */
  struct _ListSymb *inter;
  int depth;
} Tagnode;

typedef Tagnode *TagTree;


#endif
/* environnement de transformation*/

typedef struct _AttrDesc
{
  char *NameAttr;
  int ThotAttr;
  boolean IsInt;
  boolean IsTransf;
  union
  {
    char *_TextVal;
    int _IntVal;
    struct _s0
    {
      char *_Tag;
      char *_Attr;
    }s0;
  }u;
  struct _AttrDesc *next;
}AttrDesc;

#define TextVal u._TextVal
#define IntVal u._IntVal
#define AttrTag u.s0._Tag
#define AttrAttr u.s0._Attr


/* definition des structures internes des regles de transformation */
typedef struct _NodeDesc
{
  char *Tag;
  AttrDesc *Attributes;
  struct _NodeDesc *next;
} NodeDesc; 

typedef struct _RuleDesc
{
  char *RName;
  NodeDesc *OptNodes;
  NodeDesc *NewNodes;
  struct _RuleDesc *next;
} RuleDesc;

#ifndef PPSTANDALONE
typedef struct _ListElem
{
  Element element;
  int id;
  int rank;
  struct _ListElem *next;
} ListElem;
#endif

typedef struct _ListSymb
{
  struct _SymbDesc *symb;
  struct _ListSymb *next;
} ListSymb;

typedef struct _SymbDesc
{
  char *SName;
  char *Tag;
  RuleDesc *Rule;
  boolean Optional;
  boolean ActiveSymb;
  boolean OptChild;
  int depth;
  AttrDesc *Attributes;
  ListSymb *Children;
  ListSymb *Nexts;
  struct _SymbDesc *next; 
} SymbDesc;

/* descripteurs de transformation */
typedef struct _TransDesc
{
  char *NameTrans;  
  int nbPatSymb;
  int nbRules;
  int patdepth;
  ListSymb *First;
  SymbDesc *rootdesc;
  SymbDesc *PatSymbs;
  RuleDesc *Rules;
  boolean ActiveTrans;
  char *TagDest;
  struct _TransDesc *next;  
} TransDesc;

struct _match_env
{
#ifndef PPSTANDALONE
  TagTree subjecttree;
  ListElem *listSubTrees;
#endif
  /* nombre de transformations*/
  int nbTrans;
  /* porfondeur max des patterns */
  int maxdepth;
  /* descripteurs de transformations */
  TransDesc *Transformations;
} match_env;



#ifndef PPSTANDALONE
/* document auquel est applique la transformation */
Document TransDoc;
/* elements de dialogue et messages */
int TransBaseDialog;
int TRANSDIAL;
#define TR_TRANSFORM 0
#define TRANS_MSG_MAX 1
#define TransMenu 1
#define MAX_TRANS_DLG 2

/*date de derniere modif du fichier de transformations */
time_t  timeLastWrite;
#endif
