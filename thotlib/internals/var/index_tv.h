/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/* index.var */
/* modif Janvier 1996 H. Richy */

THOT_EXPORT   int		INDEX;	    /* Indentification des messages index */
THOT_EXPORT   PtrDocument   pDocu;      /* document courant (index et options) */
THOT_EXPORT   PtrDocument   pDocCour;   /* document courant (marques et desc) */
THOT_EXPORT   char          Cle[Lg_Cle];
THOT_EXPORT   PtrElement    pFirstTable;/* 1e table d'index */
THOT_EXPORT   int           indexisok;  /* vaut -1 en cas d'erreur sur une table */
THOT_EXPORT   PtrElement    pDefTable;  /* Def_table_index de la table d'index pTable */

THOT_EXPORT   char          TitreMenu[80];
THOT_EXPORT   PtrTabTri    *pTri;           /* pointeurs vers les Tab_tri */
THOT_EXPORT   PtrTabInd     tab_ind;        /* indices de tri */
THOT_EXPORT   int           itabcle;        /* nb entrees ds tab_tri, tab_ind */  

THOT_EXPORT PathBuffer Irepertoire;       /* variable d'environnement INDPAR */
THOT_EXPORT int        TriCode1[256];     /* code des caracteres pour le tri */
THOT_EXPORT int        TriCode2[256];     /* code des caracteres pour le tri */
THOT_EXPORT char       TriPrem[256];      /* initiale pour le tri */
THOT_EXPORT char       TriCap[256];       /* initiale pour la presentation */
THOT_EXPORT TAlgotri   Ialgo;             /* algorithme de tri */
THOT_EXPORT TPresTerme Ipres;             /* presentation des termes ds index */
THOT_EXPORT boolean    Iparpage;          /* presentation des references */
THOT_EXPORT boolean    Igroupe;           /* titre de groupe visible ou non */
