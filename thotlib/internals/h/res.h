/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* res.h */

#include "SelectRestruct.h"
#include "resdynmsg.h"


#define strEND  '\0'		/* fin de string */
#define NbMaxCanon 5000      /* nombre max de types cannoniques */
#define NbMaxUnit 100		/* nombre max de types cannoniques d'unites */
#define NbMaxTrace  300

/* type de relation existant entre les elements source et destination */
typedef enum {HOMONIMIE, EQUIVALENCE, FACTEUR, SOUS_TYPE, MASSIF, SOUS_SCHEMA, AUCUNE} TyRelation;

/* type d'operation a effectuer  */
typedef enum {COLLER_AVANT, COLLER_APRES, COLLER_DEDANS, CHANGE_FRERE, CHANGE_ANCETRE, CHANGE_DESCENDANT, CHANGE_AUTRE} TyOperation;

/* type d'attributs dans l'API */
typedef enum {APIattrKindEnum, APIattrKindInt, APIattrKindText, APIattrKindRef} APIattributes;

/* source ou destination*/
typedef enum {SOURCE, DEST} TySourceDest;

/* type d'empreinte effectif ou non */
typedef enum {EFFECTIF, EFFECTIF_PUR, NON_EFFECTIF} Tyff;

/*** arbre des types d'un schema ****/
typedef struct StrTreeSch *pTreeSch;

typedef struct StrTreeSch
    {
    int TypeNum;    /* numero de la regle */
    int TypeCanon;
    SSchema Schema;
    SRule *pRegle;           /* pointeur sur la regle */
    pTreeSch Pere;           /* pointeur sur le pere */
    pTreeSch Fils;           /* pointeur sur le premier fils */
    pTreeSch NextSibling;    /* pointeur sur le frere */
    ThotBool DebutRecursion; /* indique si le type est recursif */
    ThotBool Recursif;       /* indique si le type est recursif terminal*/
    } TreeSch;


/*** arbre des types ****/
typedef struct StrTreeTyp *pTreeTyp;
    
/**** arbre a plat *****/
typedef struct StrFlatTree *pFlatTree;

/**** arbre a plat *****/
    typedef struct StrFlatTree
    {
    char Emp;               /* empreinte */
    pTreeTyp TreeTypElem;   /* pointeur sur l'arbre des types */
    int TypeNum;   /* numero de regle */
    int TypeCanon;
    pFlatTree Couplage;     /* couplage */
    pFlatTree PreCouplage;
    ThotBool Utilise;
    ThotBool Recursif;
    pFlatTree NextSibling;
    pFlatTree RlPrevious;
    } FlatTree;


/*** arbre des types ****/
typedef struct StrTreeTyp
    {
    int TypeNum;    /* numero de la regle */
    int TypeCanon;
    SSchema Schema;         /* schema du type */
    SRule *pRegle;          /* pointeur sur la regle */
    ThotBool effective;     /* type utilise par l'instance */
    FlatTree *DebutFT;      /* debut dans l'arbre a plat */
    FlatTree *FinFT;        /* fin dans l'arbre a plat */
    pTreeTyp Couplage;      /* pointeur sur l'autre element constituant le couplage */
    pTreeTyp Pere;          /* pointeur sur le pere */
    pTreeTyp Fils;          /* pointeur sur le premier fils */
    pTreeTyp NextSibling;   /* pointeur sur le frere */
    ThotBool DebutRecursion;  /* indique si le type est recursif */
    ThotBool Recursif;       /* indique si le type est recursif */
    } TreeTyp;
    
    
/*** tableau indiquant si un type canonique est effectif ****/
    typedef struct
    {
    ThotBool ffectif;
    }Tableff;
    
/*** liste des structures Resdyn ****/
    typedef struct SchemaResdyn *PtrSchemaResdyn;

/* liste des types Resdyn des schemas de structure */
    typedef struct SchemaResdyn
    {
    char *NomSchema;         /* Name du schema de structure */
    SSchema SchDeStr;        /* pointeur sur le schema de structure */
    int DernierTypeCanon;
    Tableff *Tabff;        /* tableau indiquant si un type canonique est effectif */
    int *Units;      /* liste des units, le nombre de UNITS est en 0 */
    pTreeSch TreeSchRoot;    /* pointeur sur la racine de l'arbre du schema */
    PtrSchemaResdyn Next;    /* Pointeur sur l'empreinte du schema suivant*/
    } TySchemaResdyn;
    
/*** liste de types****/

    typedef struct StrListeType *pStrListeType;
    
    typedef struct StrListeType
    {
    pTreeTyp Typ;
    pStrListeType Next;
    } StrListeTyp;
    
    typedef struct StrTypeCouplage *PtrStrTypeCouplage;
    
    typedef struct StrTypeCouplage
    {
    int LgEmp;
    FlatTree *ArbrePlat;                /* debut de l'arbre a plat */
    pTreeTyp TreeTypRoot;
    SSchema Schema;
    int NumRegle;
    PtrStrTypeCouplage CoupleAvec;
    TyRelation TypeRelation;
    TySourceDest SourceDest;
    StrListeTyp *ListePereUNITS;
    PtrStrTypeCouplage Pere;
    PtrStrTypeCouplage Next;
    } TypeCouplage;
    

/*** reconnaissance d'un massif ****/
    typedef struct StrPileMassif
    {
    char Emp;
    char Inverse;
    int Retour;
    int Counter;
    } TyPileMassif;


/*** liste de la memoire utilisee ?****/

    typedef struct StrListeMemoire *pStrListeMemoire;
    
    typedef struct StrListeMemoire
    { int Taille;
      int Indice;
      char * Module;
      char * Memoire;
      pStrListeMemoire Next;
      } StrListeMem;

      
/*** liste d'elements****/

    typedef struct StrListeElem *pStrListeElem;
    
    typedef struct StrListeElem
    {
    Element OldElem; /* ancien element source dans le document source */
    Element PereOldElem; /* pere de OldElem */
    Element FrereOldElem; /* frere gauche de OldElem */
    Element PcWithin; /* ancetre eventuellement cree en destination */
    Element NewElem;    /* racine de l'element cree en destination*/
    pStrListeElem Next;
    } StrListeEl;

/*** liste des references a ajouter ****/
/* OldRef est une reference dans le document DocOldRef, elle referencait l'element OldTarget */
    typedef struct StrListeRef *pStrListeRef;
    
    typedef struct StrListeRef
      {
      Element RefOrigin;	/* element ref origine */
      Attribute AttrOrigin;	/* attribut ref origine */
      Document DocOrigin;	/* document de la ref origin */
      Element RefTrans;		/* element ref transformee */
      Attribute AttrTrans;	/* attribut ref transformee */
      Document DocTrans;	/* document de la ref transformee */	  
      Element CibleTrans;	/* element cible transformee */
      Document DocCibleTrans;	/* document de la cible transformee */
      pStrListeRef Next;
    } StrListeR;

/*** liste de tous les elements recopies de la source vers la destination
     avec insertion en tete ****/

    typedef struct StrListeRecopie *pStrListeRecopie;
    
    typedef struct StrListeRecopie
    {
    Element OldElem; /* l'ancien element */
    Element NewElem; /* le nouvel element */
    pStrListeRecopie Next;
    } StrListeRecop;
   
/*** descripteur d'unites effective ****/

typedef struct _StrListeUnit *pStrListeUnit;
typedef struct _StrListeUnit
  {
  int TypeCanon;	/* type canonique de l'unite */
  int TypeUnit;		/* numero du type des instances 
			   (0 si les instances ont un type different) */
  pTreeTyp pType;	/* noeud dans l'arbre des types */
  pStrListeUnit Next;   /* descr suivant */
  }StrListeUnit;
    
/*** contexte de conversion ****/

      typedef struct
    {
    PtrSchemaResdyn  AllResdyn;
    TypeCouplage *AllCouple; /* debut de liste couplage source et couplage destination */
    Document DocSour, DocDest; /* numero du document contenant la source, la destination */
    Document DocType;
    TypeCouplage *SourCouple, *DestCouple; /* pointeur sur le couplage de la source et de la dest */
    Element ElSour; /* element source, appartient au buffer de copie */
    Element ElDest; /* element destination, appartient au document destination */
    TyOperation TypeOperation;
    StrListeEl *ListeEl;
    StrListeR *ListeReference;
    StrListeRecop *ListeRecopie;
    StrListeUnit *ListeUnites; /* liste chainee des unites effectives */
    } TyResdynCt;

      
/*** contexte de comparaison ****/

      typedef struct
    {
    char *SourEmp;
    FlatTree **FTSour;
    char *DestEmp;
    FlatTree **FTDest;
    Tyff Typeff;
    } TyComp;


