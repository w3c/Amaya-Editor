/* index.var */
/* modif Janvier 1996 H. Richy */

EXPORT   int		INDEX;	    /* Indentification des messages index */
EXPORT   PtrDocument   pDocu;      /* document courant (index et options) */
EXPORT   PtrDocument   pDocCour;   /* document courant (marques et desc) */
EXPORT   char          Cle[Lg_Cle];
EXPORT   PtrElement    pFirstTable;/* 1e table d'index */
EXPORT   int           indexisok;  /* vaut -1 en cas d'erreur sur une table */
EXPORT   PtrElement    pDefTable;  /* Def_table_index de la table d'index pTable */

EXPORT   char          TitreMenu[80];
EXPORT   PtrTabTri    *pTri;           /* pointeurs vers les Tab_tri */
EXPORT   PtrTabInd     tab_ind;        /* indices de tri */
EXPORT   int           itabcle;        /* nb entrees ds tab_tri, tab_ind */  

EXPORT PathBuffer Irepertoire;       /* variable d'environnement INDPAR */
EXPORT int        TriCode1[256];     /* code des caracteres pour le tri */
EXPORT int        TriCode2[256];     /* code des caracteres pour le tri */
EXPORT char       TriPrem[256];      /* initiale pour le tri */
EXPORT char       TriCap[256];       /* initiale pour la presentation */
EXPORT TAlgotri   Ialgo;             /* algorithme de tri */
EXPORT TPresTerme Ipres;             /* presentation des termes ds index */
EXPORT boolean    Iparpage;          /* presentation des references */
EXPORT boolean    Igroupe;           /* titre de groupe visible ou non */
