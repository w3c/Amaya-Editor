#define MAXTYPE 500
#define MAX_DEST 20
#define MAXSCHEMA 10
#define MAXDEPTH 50
#define SIZEPRINT 1000

#define WITHOUTCHOICE TRUE
#define WITHOUTSINGLELIST TRUE

#define NUMTYPEROOT 8 
#define RestTypeTexte 1
#define RestTypeGraphique 2
#define RestTypeSymbol 3
#define RestTypeImage 4
#define RestTypeReference 5
#define RestTypePage 6

#define MAXPRINTMETHOD 10
#define PrintMethod unsigned char
#define PM_GENERIC 0
#define PM_EFFECTIF 1
#define PM_PUREEFF 2
#define PM_WTHTCHOICE 4
#define PM_WTHTLIST 8

#define MAXNODECOUPLE 8

/* domaines de valeurs */
typedef enum {NONE, 
	      IDENTITE , 
	      EQUIVALENCE, 
	      FACTOR , 
	      SUB_TYPE , 
	      CLUSTER, 
	      SUB_SCHEMA, 
	      PRE_COUPLED , 
	      REC_MASK
} TyRelation;

#ifndef STANDALONE

/* pre couplages de noeuds */
typedef struct _TypeAtom
{
  ElementType ElType;
  struct _TypeAtom *Parent;
} TypeAtom;

typedef struct _PreCouple PreCouple;
typedef struct _PreCouple
{
  TypeAtom *SourceContext;
  TypeAtom *DestContext;
} StrPreCouple;
#endif /* STANDALONE */

/* arbre de types */
typedef struct _StrTypeTree *TypeTree;
typedef struct _StrTypeTree
{
  int TypeNum;
  SSchema TSchema;
  int TId;
  TypeTree TParent;
  TypeTree TChild;
  TypeTree TNext;
  TypeTree TPrevious;
  TypeTree TRecursive;
#ifndef STANDALONE
  PreCouple TPreCouple[MAXNODECOUPLE];
#endif /* STANDALONE */
  ThotBool TEffective;
  ThotBool TDiscard;
  ThotBool TRecursApplied;
  char TPrintSymb;
  int TDepth;
} StrTypeTree;
  
/* structure des couplages */
typedef struct _RestCouple *RestCouple;
typedef struct _RestCouple
{
  TypeTree CDstNode;
  TypeTree CRecFrom; 
  TypeTree CRecTo;
} StrRestCouple;

/* contexte d'une restructuration */
typedef struct _StrSourcePrint *SourcePrint;
typedef struct _StrSourcePrint
{
  TypeTree STree;
  TypeTree SNodes[SIZEPRINT];
  char SPrint[SIZEPRINT];
  PrintMethod SPrintMethod;
  SourcePrint SNext;
}StrSourcePrint;

typedef struct _StrRestruct *Restruct;
typedef struct _StrRestruct
{
#ifndef STANDALONE
  ElementType RDestType;
#endif /* STANDALONE */
  TypeTree RDestTree;
  char RDestPrint[SIZEPRINT];
  TypeTree RDestNodes[SIZEPRINT];
  RestCouple RCoupledNodes[SIZEPRINT];
  SourcePrint RSrcPrint;
  TyRelation RRelation;
  Restruct RNext;
}StrRestruct;


typedef struct _StrRestContext *RestContext;
typedef struct _StrRestContext
{ 
#ifndef STANDALONE
  SSchema CSrcSchema;
#endif /* STANDALONE */
  Restruct CListRestruct;
  SourcePrint CListSourcePrints;
  int CNbPrintMethod;
  PrintMethod CPrintMethod[MAXPRINTMETHOD];
#ifndef STANDALONE
  Element COldElems[50];
#endif /* STANDALONE */
  int CNbOldElems;
  int CIdCount;
}StrRestContext;

typedef struct StrClusterStack
{
  char Emp;
  char Inverse;
  int Retour;
  int Compteur;
} TyClusterStack;

/* definition de descripteurs d'unites */
/* associe a un noeud unite un type de base */
typedef struct _StrUnitDesc
{
  TypeTree Tree;
  int TypeNumber;
  struct _StrUnitDesc *Next;
} StrUnitDesc;


THOT_EXPORT RestContext RContext;
