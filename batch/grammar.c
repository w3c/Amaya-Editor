
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   grm : Le programme GRM cree un fichier contenant la representation
   codee d'une grammaire, a` partir d'un fichier contenant la   
   description de cette grammaire sous la forme BNF.
   La grammaire codee est destinee aux programmes STR, PRS, TRA ou APP
   qui compilent, selon cette grammaire, un schema de structure,
   de presentation, de traduction ou d'application.
 */

#include "thot_sys.h"
#include "grmmsg.h"
#include "compilmsg.h"
#include "constgrm.h"
#include "constint.h"
#include "conststr.h"
#include "constprs.h"
#include "message.h"
#include "storage.h"
#include "typegrm.h"
#include "typeint.h"

typedef char        fname[30];	/* nom de fichier */

#undef EXPORT
#define EXPORT
#include "compil.var"
#undef EXPORT
#define EXPORT extern
#include "analsynt.var"

#include "compilmsg.f"
#include "storage.f"

#define NBSTRING 200		/* nombre max de chaines dans une grammaire */
#define MAXNRULE 160		/* nombre max de regles de la grammaire */
#define MAXREF   50		/* nombre max de references a une regle */

typedef fname      *ptrfname;
typedef int         rulenb;	/* numero de la regle de grammaire en cours d'analyse */

typedef struct _listref
  {
     int                 listlen;	/* longueur de la liste */
     rulenb              listelem[MAXREF];	/* liste des references */
  }
listref;


int                 linenb;	/* compteur de lignes */
static listref      reftable[nbident];	/* table des references des identificateurs */
static listref      kreftable[NBSTRING];	/* table des references des mots-cles */
static listref      nameref;	/* liste des references a 'NOM' */
static listref      numberref;	/* liste des references a 'NOMBRE' */
static listref      stringref;	/* liste des references a 'STRING' */
static Name          pfilename;	/* nom du fichier a compiler */

static int          lgstrtable;	/* longueur effective de la table */
static identelem    strtable[NBSTRING];		/* table des chaines */
static int          shortkwcode;	/* code du dernier mot-cle court cree */
static int          keywordcode;	/* code du dernier mot-cle long cree */
static int          lgnruletb;	/* longueur effective de la table */
static rule         nruletb[MAXNRULE];	/* table des regles codees */
static int          currule;	/* regle en cours de generation */
static int          curptr;	/* pointeur courant dans la regle courante */
static FILE        *list;	/* fichier des listes */

#include "analsynt.f"
#include "cdialog.f"
#include "filesystem.f"
#include "message.f"

#ifdef __STDC__
extern void         TtaInitializeAppRegistry (char *);
extern void         TtaSaveAppRegistry (void);

#else
extern void         TtaInitializeAppRegistry ();
extern void         TtaSaveAppRegistry ();

#endif /* __STDC__ */

/* ---------------------------------------------------------------------- */
/* |    initreftb initialise les messages d'erreur et les tables des    | */
/* |            references.                                             | */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
static void         initreftb ()

#else  /* __STDC__ */
static void         initreftb ()
#endif				/* __STDC__ */

{
   int                 j;

   for (j = 0; j < nbident; j++)
      reftable[j].listlen = 0;
   for (j = 0; j < NBSTRING; j++)
      kreftable[j].listlen = 0;
   nameref.listlen = 0;
   numberref.listlen = 0;
   stringref.listlen = 0;
}

/* ---------------------------------------------------------------------- */
/* |    putr                                                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         putr (grmcode n, iline wi)

#else  /* __STDC__ */
static void         putr (n, wi)
grmcode             n;
iline               wi;

#endif /* __STDC__ */

{
   nruletb[currule - 1][curptr] = n;
   if (curptr >= maxlgrule)
      CompilerError (wi, GRM, FATAL, GRM_RULE_TOO_LONG, inputLine, linenb);
   else
      curptr++;
}


/* ---------------------------------------------------------------------- */
/* |    addref ajoute le numero de la regle courante dans la liste de   | */
/* |            references passee en parametre.                         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         addref (listref * ref, iline wi)

#else  /* __STDC__ */
static void         addref (ref, wi)
listref            *ref;
iline               wi;

#endif /* __STDC__ */

{
   if (ref->listlen >= MAXREF)
      CompilerError (wi, GRM, FATAL, GRM_REF_TABLE_FULL, inputLine, linenb);
   else
     {
	ref->listlen++;
	ref->listelem[ref->listlen - 1] = currule;
     }
}


/* ---------------------------------------------------------------------- */
/* |    ProcessToken traite le mot commencant a` la position wi dans la | */
/* |            ligne courante, de longueur wl et code grammatical c,   | */
/* |            apparaissant dans la regl r. Si c'est un identif, nb    | */
/* |            contient son rang dans la table des identificateurs.    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         ProcessToken (iline wi, iline wl, grmcode c, rulenb r, int nb)

#else  /* __STDC__ */
static void         ProcessToken (wi, wl, c, r, nb)
iline               wi;
iline               wl;
grmcode             c;
rulenb              r;
int                 nb;

#endif /* __STDC__ */

{
   int                 i;
   int                 j;
   boolean             known;

   if (c < 1099 && c > 1000)
      /* un caractere separateur */
      switch (c - 1000)
	    {
	       case 1:
		  curptr = 1;	/* =  */
		  break;
	       case 2:
		  putr (2000, wi);	/* .  */
		  curptr = 0;
		  break;
	       case 3:
		  putr (2003, wi);	/* /  */
		  break;
	       case 4:
		  putr (2001, wi);	/* [  */
		  break;
	       case 5:
		  putr (2002, wi);	/* ]  */
		  break;
	       case 6:
		  putr (2004, wi);	/* <  */
		  break;
	       case 7:
		  putr (2005, wi);	/* >  */
		  break;
	    }
   else if (c >= 1102 && c <= 1104)
      /* un type de base */
      switch (c)
	    {
	       case 1102:
		  /* NOM */
		  putr (3001, wi);
		  addref (&nameref, wi);
		  break;
	       case 1103:
		  /* STRING */
		  putr (3003, wi);
		  addref (&stringref, wi);
		  break;
	       case 1104:
		  /* NOMBRE */
		  putr (3002, wi);
		  addref (&numberref, wi);
		  break;
	    }
   else if (c == 3001)
      /* un symbole de la grammaire */
      if (curptr == 0)
	 /* nouvelle regle */
	{
	   currule = nb;
	   fprintf (list, "%5d", nb);
	   if (currule > lgnruletb)
	      if (currule <= MAXNRULE)
		 lgnruletb = currule;
	      else
		 CompilerError (wi, GRM, FATAL, GRM_RULES_TABLE_FULL, inputLine, linenb);
	   identtable[nb - 1].identdef = nb;
	}
      else
	{
	   /* la regle est en cours */
	   if (nb == currule)
	      CompilerError (wi, GRM, FATAL, GRM_RULES_TABLE_FULL, inputLine, linenb);
	   else
	      putr (nb, wi);
	   identtable[nb - 1].identref = nb;
	   addref (&reftable[nb - 1], wi);
	}
   else if (c == 3003)
      /* une chaine entre quotes */
     {
	i = 0;
	known = False;
	if (wl - 1 > identlen)
	   CompilerError (wi, GRM, FATAL, GRM_STRING_TOO_LONG, inputLine, linenb);
	/* est-elle deja dans la table ? */
	else
	  {
	     do
	       {
		  if (strtable[i].identlg == wl - 1)
		    {
		       j = 0;
		       do
			  j++;
		       while (!(inputLine[wi + j - 2] != strtable[i].identname[j - 1] || j
				== wl - 1));
		       if (j == wl - 1)
			  if (inputLine[wi + j - 2] == strtable[i].identname[j - 1])
			     known = True;
		    }
		  i++;
	       }
	     while (!(known || i >= lgstrtable));
	     if (known)
		/* deja dans la table */
		addref (&kreftable[i - 1], wi);
	     /* elle n'est pas dans la table, on la met */
	     else if (lgstrtable >= NBSTRING)
		CompilerError (wi, GRM, FATAL, GRM_STRING_TABLE_FULL, inputLine, linenb);
	     else
	       {
		  strtable[lgstrtable].identlg = wl - 1;
		  for (j = 0; j < wl - 1; j++)
		     strtable[lgstrtable].identname[j] = inputLine[wi + j - 1];
		  if (strtable[lgstrtable].identlg == 1)
		     /* mot-cle court */
		    {
		       shortkwcode++;
		       strtable[lgstrtable].identtype = shortkwcode;
		    }
		  else
		     /* mot-cle long */
		    {
		       keywordcode++;
		       strtable[lgstrtable].identtype = keywordcode;
		    }
		  addref (&kreftable[lgstrtable], wi);
		  lgstrtable++;
		  i = lgstrtable;
	       }
	     if (!error)
		putr (strtable[i - 1].identtype, wi);
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    initgrammar initialise la table des mots-cles et la table des   | */
/* |            regles.                                                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         initgrammar ()

#else  /* __STDC__ */
static void         initgrammar ()
#endif				/* __STDC__ */

{
   kwtable[0].kwlg = 1;
   kwtable[0].kwname[0] = '=';
   kwtable[0].gcode = 1001;
   kwtable[1].kwlg = 1;
   kwtable[1].kwname[0] = '.';
   kwtable[1].gcode = 1002;
   kwtable[2].kwlg = 1;
   kwtable[2].kwname[0] = '/';
   kwtable[2].gcode = 1003;
   kwtable[3].kwlg = 1;
   kwtable[3].kwname[0] = '[';
   kwtable[3].gcode = 1004;
   kwtable[4].kwlg = 1;
   kwtable[4].kwname[0] = ']';
   kwtable[4].gcode = 1005;
   kwtable[5].kwlg = 1;
   kwtable[5].kwname[0] = '<';
   kwtable[5].gcode = 1006;
   kwtable[6].kwlg = 1;
   kwtable[6].kwname[0] = '>';
   kwtable[6].gcode = 1007;
   lastshortkw = 7;
   kwtable[7].kwlg = 3;
   strncpy (kwtable[7].kwname, "END             ", kwlen);
   kwtable[7].gcode = 1101;
   kwtable[8].kwlg = 4;
   strncpy (kwtable[8].kwname, "NAME            ", kwlen);
   kwtable[8].gcode = 1102;
   kwtable[9].kwlg = 6;
   strncpy (kwtable[9].kwname, "STRING          ", kwlen);
   kwtable[9].gcode = 1103;
   kwtable[10].kwlg = 6;
   strncpy (kwtable[10].kwname, "NUMBER          ", kwlen);
   kwtable[10].gcode = 1104;
   lgkwtable = 11;
   ruletable[0][1] = 2;
   ruletable[0][2] = 2004;
   ruletable[0][3] = 2;
   ruletable[0][4] = 2005;
   ruletable[0][5] = 1101;
   ruletable[0][6] = 2000;
   ruletable[1][1] = 3;
   ruletable[1][2] = 1001;
   ruletable[1][3] = 4;
   ruletable[1][4] = 1002;
   ruletable[1][5] = 2000;
   ruletable[2][1] = 3001;
   ruletable[2][2] = 2000;
   ruletable[3][1] = 5;
   ruletable[3][2] = 2003;
   ruletable[3][3] = 6;
   ruletable[3][4] = 2000;
   ruletable[4][1] = 1102;
   ruletable[4][2] = 2003;
   ruletable[4][3] = 1103;
   ruletable[4][4] = 2003;
   ruletable[4][5] = 1104;
   ruletable[4][6] = 2000;
   ruletable[5][1] = 7;
   ruletable[5][2] = 2004;
   ruletable[5][3] = 1003;
   ruletable[5][4] = 7;
   ruletable[5][5] = 2005;
   ruletable[5][6] = 2000;
   ruletable[6][1] = 8;
   ruletable[6][2] = 2004;
   ruletable[6][3] = 8;
   ruletable[6][4] = 2005;
   ruletable[6][5] = 2000;
   ruletable[7][1] = 9;
   ruletable[7][2] = 2003;
   ruletable[7][3] = 1004;
   ruletable[7][4] = 9;
   ruletable[7][5] = 2004;
   ruletable[7][6] = 9;
   ruletable[7][7] = 2005;
   ruletable[7][8] = 1005;
   ruletable[7][9] = 2003;
   ruletable[7][10] = 1006;
   ruletable[7][11] = 9;
   ruletable[7][12] = 2004;
   ruletable[7][13] = 9;
   ruletable[7][14] = 2005;
   ruletable[7][15] = 1007;
   ruletable[7][16] = 2000;
   ruletable[8][1] = 3;
   ruletable[8][2] = 2003;
   ruletable[8][3] = 10;
   ruletable[8][4] = 2000;
   ruletable[9][1] = 3003;
   ruletable[9][2] = 2000;
   lgruletable = 10;
}


/* ---------------------------------------------------------------------- */
/* |    writefiles ecrit les mots-cles et regles dans le fichier de     | */
/* |            sortie de type .GRM, produit le fichier .LST et le      | */
/* |            fichier .h                                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         writefiles ()

#else  /* __STDC__ */
static void         writefiles ()
#endif				/* __STDC__ */

{
   int                 mc;
   int                 l;
   int                 r;
   int                 i;
   int                 ic;
   int                 lgln;
   int                 identmax;
   int                 j;
   FILE               *outfile;
   FILE               *includefile;

   /* met le suffixe GRM a la fin du nom de fichier */
   lgln = 0;
   while (pfilename[lgln] != '.')
      lgln++;
   strcpy (&pfilename[lgln + 1], "GRM");
   /* cree le fichier .GRM */
   outfile = fopen (pfilename, "w");

   /* cree le fichier .h */
   strcpy (&pfilename[lgln + 1], "h");
   includefile = fopen (pfilename, "w");
   if (includefile == NULL)
     {
	TtaDisplayMessage (FATAL, TtaGetMessage(GRM, GRM_CAN_T_CREATE_HEADER_FILE), pfilename);
	exit (1);
     }
   else
     {
	pfilename[lgln] = '\0';
	putc ((char) (FF), list);
	fprintf (list, "\n\n*** SHORT KEY-WORDS ***\n\n");
	fprintf (list, "code  character  numbers of rules using the key-word\n\n");
	fprintf (includefile, "/* Definitions for compiler of language %s */\n\n", pfilename);
	fprintf (includefile, "/*  SHORT KEY-WORDS  */\n\n");

	for (mc = 0; mc < lgstrtable; mc++)
	  {
	     if (strtable[mc].identlg == 1)
	       {
		  fprintf (includefile, "#define CHR_%d \t%4d\n",
		   (int) strtable[mc].identname[0], strtable[mc].identtype);
		  fprintf (outfile, "%c %4d\n", strtable[mc].identname[0], strtable[mc].identtype);
		  fprintf (list, "%4d      ", strtable[mc].identtype);
		  if (strtable[mc].identname[0] < ' ')
		     fprintf (list, "\\%3d ", strtable[mc].identname[0]);
		  else
		     fprintf (list, "%c    ", strtable[mc].identname[0]);
		  lgln = 15;
		  for (j = 0; j < kreftable[mc].listlen; j++)
		    {
		       if (lgln > 76)
			 {
			    fprintf (list, "\n");
			    fprintf (list, "               ");
			    lgln = 15;
			 }
		       fprintf (list, " %3d", kreftable[mc].listelem[j]);
		       lgln += 4;
		    }
		  fprintf (list, "\n");
	       }
	  }
	fprintf (outfile, "\n");
	fprintf (list, "\n\n\n*** LONG KEY-WORDS ***\n\n");
	fprintf (list, "code  key-word      numbers of rules using the key-word\n\n");
	fprintf (includefile, "\n/*  LONG KEY-WORDS  */\n\n");

	/* cherche la longueur du plus long des mots-cles */
	identmax = 0;
	for (mc = 0; mc < lgstrtable; mc++)
	   if (strtable[mc].identlg > identmax)
	      identmax = strtable[mc].identlg;
	/* ecrit les mots-cles et les regles qui les utilisent */
	for (mc = 0; mc < lgstrtable; mc++)
	  {
	     if (strtable[mc].identlg > 1)
	       {
		  fprintf (includefile, "#define KWD_%s \t%4d\n",
			   strtable[mc].identname, strtable[mc].identtype);
		  fprintf (list, "%4d  ", strtable[mc].identtype);
		  for (l = 0; l < strtable[mc].identlg; l++)
		    {
		       putc (strtable[mc].identname[l], outfile);
		       if (strtable[mc].identname[l] < ' ')
			  fprintf (list, "*");
		       else
			  putc (strtable[mc].identname[l], list);
		    }
		  for (j = strtable[mc].identlg + 1; j <= identmax; j++)
		     fprintf (list, " ");
		  lgln = identmax + 6;
		  fprintf (outfile, " %4d\n", strtable[mc].identtype);
		  for (j = 0; j < kreftable[mc].listlen; j++)
		    {
		       if (lgln > 76)
			 {
			    fprintf (list, "\n");
			    lgln = identmax + 8;
			    for (l = 1; l <= lgln; l++)
			       fprintf (list, " ");
			 }
		       fprintf (list, " %3d", kreftable[mc].listelem[j]);
		       lgln += 4;
		    }
		  fprintf (list, "\n");
	       }
	  }
	fprintf (list, "\n\n\n*** TABLE OF RULES AND REFERENCES ***\n\n");

	fprintf (list, "The code is the code of the symbol and the code of the rule\n");
	fprintf (list, "where that symbol appears in the left part. For each symbole,\n");
	fprintf (list, "the rule numbers indicate the rules that use that symbol in\n");
	fprintf (list, "right part.\n\n");
	fprintf (list, " code  symbol        rule numbers\n\n");
	fprintf (includefile, "\n/*  RULES  */\n\n");

	/* cherche la longueur du plus long des symboles */
	identmax = 0;
	for (ic = 0; ic < lgidenttable; ic++)
	   if (identtable[ic].identlg > identmax)
	      identmax = identtable[ic].identlg;
	/* ecrit les symboles et leurs references */
	for (ic = 0; ic < lgidenttable; ic++)
	  {
	     fprintf (includefile, "#define RULE_%s \t%4d\n", identtable[ic].identname, ic + 1);
	     fprintf (list, " %4d  ", ic + 1);
	     for (l = 0; l < identtable[ic].identlg; l++)
		if (identtable[ic].identname[l] < ' ')
		   fprintf (list, "*");
		else
		   putc (identtable[ic].identname[l], list);
	     for (j = identtable[ic].identlg; j < identmax; j++)
		fprintf (list, " ");
	     lgln = identmax + 7;
	     for (j = 0; j < reftable[ic].listlen; j++)
	       {
		  if (lgln > 76)
		    {
		       fprintf (list, "\n");
		       lgln = identmax + 9;
		       for (l = 1; l <= lgln; l++)
			  fprintf (list, " ");
		    }
		  fprintf (list, " %3d", reftable[ic].listelem[j]);
		  lgln += 4;
	       }
	     fprintf (list, "\n");
	  }
	fflush (includefile);
	fclose (includefile);

	fprintf (list, "\n\n\n*** REFERENCES TO BASIC TYPES ***\n\n");
	fprintf (list, "Numbers of the rules using NAME\n");

	lgln = 0;
	for (j = 0; j < nameref.listlen; j++)
	  {
	     if (lgln > 76)
	       {
		  fprintf (list, "\n");
		  lgln = 0;
	       }
	     fprintf (list, " %3d", nameref.listelem[j]);
	     lgln += 4;
	  }
	fprintf (list, "\n\n");

	fprintf (list, "Numbers of the rules using NUMBER\n");

	lgln = 0;
	for (j = 0; j < numberref.listlen; j++)
	  {
	     if (lgln > 76)
	       {
		  fprintf (list, "\n");
		  lgln = 0;
	       }
	     fprintf (list, " %3d", numberref.listelem[j]);
	     lgln += 4;
	  }
	fprintf (list, "\n\n");

	fprintf (list, "Numbers of the rules using STRING\n");

	lgln = 0;
	for (j = 0; j < stringref.listlen; j++)
	  {
	     if (lgln > 76)
	       {
		  fprintf (list, "\n");
		  lgln = 0;
	       }
	     fprintf (list, " %3d", stringref.listelem[j]);
	     lgln += 4;
	  }
	fprintf (list, "\n");
	fprintf (outfile, "\n");
	lgln = 0;		/* ligne courante vide */
	for (r = 0; r < lgnruletb; r++)
	   /* ecrit la table des regles */
	  {
	     fprintf (outfile, "%4d ", r + 1);
	     /* numero du symbole defini par la regle */
	     lgln += 5;
	     /* ecrit le contenu de la regle */
	     i = 0;
	     do
	       {
		  i++;
		  fprintf (outfile, "%4d ", nruletb[r][i]);
		  lgln += 5;
		  if (lgln > 79)
		    {
		       fprintf (outfile, "\n");
		       lgln = 0;
		    }
	       }
	     while (!(nruletb[r][i] == 2000));
	     /* 2000 = Fin de la regle */
	     if (lgln > 0)
	       {
		  fprintf (outfile, "\n");
		  lgln = 0;
	       }
	  }
     }
   fclose (outfile);
}

/* ---------------------------------------------------------------------- */
/* |    checkword teste si la chaine de longueur wl commencant a` la    | */
/* |            position wi de la ligne courante est un mot-cle bien    | */
/* |            forme'.                                                 | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         checkword (iline wi, iline wl)

#else  /* __STDC__ */
static void         checkword (wi, wl)
iline               wi;
iline               wl;

#endif /* __STDC__ */

{
   iline               j;

   if (wl == 2)
     {
	/* mot-cle court */
	if ((inputLine[wi - 1] >= '!'
	     && inputLine[wi - 1] <= '/') || (inputLine[wi - 1] >= ':'
		   && inputLine[wi - 1] <= '@') || (inputLine[wi - 1] >= '['
						    && inputLine[wi - 1] <= '`') || inputLine[wi - 1] == '|' || inputLine[wi - 1] == '~')
	  {
	     if (inputLine[wi - 1] == '\'')
		CompilerError (wi, GRM, FATAL, GRM_INCOR_KEY_WORD, inputLine, linenb);
	  }
	else
	   CompilerError (wi, GRM, FATAL, GRM_INCOR_KEY_WORD, inputLine, linenb);
     }
   else
     {
	/* mot-cle long */
	if ((inputLine[wi - 1] >= 'A'
	     && inputLine[wi - 1] <= 'Z') || (inputLine[wi - 1] >= 'a'
	       && inputLine[wi - 1] <= 'z') || (inputLine[wi - 1] >= 1
					  && inputLine[wi - 1] <= 31))
	   /* le premier caractere est une lettre */
	  {
	     j = wi;
	     do
	       {
		  if (!((inputLine[j] >= 'A'
			 && inputLine[j] <= 'Z') || (inputLine[j] >= 'a'
			     && inputLine[j] <= 'z') || (inputLine[j] >= '0'
		    && inputLine[j] <= '9') || (inputLine[wi - 1] >= 1
						&& inputLine[wi - 1] < ' ')))
		     CompilerError (wi, GRM, FATAL, GRM_INCOR_KEY_WORD, inputLine, linenb);
		  j++;
	       }
	     while (!error && j < wi + wl - 2);
	  }
	else
	   CompilerError (wi, GRM, FATAL, GRM_INCOR_KEY_WORD, inputLine, linenb);
     }
}


/* ---------------------------------------------------------------------- */
/* |    defrefok verifie que tous les symboles intermediaires sont      | */
/* |            bien definis et reference's.                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      defrefok ()

#else  /* __STDC__ */
static boolean      defrefok ()
#endif				/* __STDC__ */

{
   int                 ic;
   boolean             ok;

   ok = True;
   for (ic = 0; ic < lgidenttable; ic++)
     {
	if (identtable[ic].identdef == 0)
	  {
	     TtaDisplayMessage (FATAL, TtaGetMessage(GRM, GRM_UNDEFINED_SYMBOL), identtable[ic].identname);
	     ok = False;
	  }
	if (identtable[ic].identref == 0)
	   if (ic > 0)
	     {
		TtaDisplayMessage (FATAL, TtaGetMessage(GRM, GRM_UNREFERENCED_SYMBOL), identtable[ic].identname);
		ok = False;
	     }
     }
   return ok;
}


/* ---------------------------------------------------------------------- */
/* |    main program                                                    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 main (int argc, char **argv)

#else  /* __STDC__ */
int                 main (argc, argv)
int                 argc;
char              **argv;

#endif /* __STDC__ */

{
   FILE               *infile;
   boolean             fileOK;
   lineBuffer          srcline;	/* ligne source non traduite */
   iline               i;	/* position courante dans la ligne en cours */
   iline               wi;	/* position du debut du mot courant dans la ligne */
   iline               wl;	/* longueur du mot courant */
   nature              wn;	/* nature du mot courant */
   rnb                 r;	/* numero de regle */
   rnb                 pr;	/* numero de la regle precedente */
   grmcode             c;	/* code grammatical du mot trouve */
   int                 nb;	/* indice dans identtable du mot trouve, si

				   identificateur */

   TtaInitializeAppRegistry (argv[0]);
   GRM = TtaGetMessageTable ("grmdialogue", GRM_MSG_MAX);
   COMPIL = TtaGetMessageTable ("compildialogue", COMPIL_MSG_MAX);

   if (argc != 2)
      TtaDisplaySimpleMessage (FATAL, GRM, GRM_NO_SUCH_FILE);
   else
     {
	strncpy (pfilename, argv[1], MAX_NAME_LENGTH - 1);
	i = strlen (pfilename);
	/* ajoute le suffixe .LAN */
	strcat (pfilename, ".LAN");

	if (FileExist (pfilename) == 0)
	   TtaDisplaySimpleMessage (FATAL, GRM, GRM_NO_SUCH_FILE);
	else
	   /* le fichier d'entree existe, on l'ouvre */
	  {
	     infile = fopen (pfilename, "r");
	     error = False;
	     initgrammar ();
	     lgidenttable = 0;	/* table des identificateurs vide */
	     lgstrtable = 0;	/* table des chaines vide */
	     lgnruletb = 0;	/* table des  regles vide */
	     curptr = 0;
	     linenb = 0;
	     shortkwcode = 1000;
	     keywordcode = 1100;
	     initreftb ();	/* initialise la table des references */
	     InitParser ();	/* initialise l'analyseur syntaxique */
	     /* met le suffixe LST a la fin du nom de fichier */
	     strcpy (&pfilename[i + 1], "LST");
	     /* cree le fichier .LST */
	     list = fopen (pfilename, "w");
	     fprintf (list, "GRAMMAR OF FILE ");
	     i = 0;
	     while (pfilename[i] != '.')
		putc (pfilename[i++], list);
	     fprintf (list, ".LAN\n\n\n");
	     fprintf (list, " rule\n");
	     fprintf (list, "number\n");

	     fprintf (list, "\n");
	     /* lit tout le fichier et fait l'analyse */
	     fileOK = True;
	     while (fileOK && !error)
		/* lit une ligne */
	       {
		  i = 0;
		  do
		     fileOK = BIOreadByte (infile, &inputLine[i++]);
		  while (i < linelen && inputLine[i - 1] != '\n' && fileOK);
		  /* marque la fin reelle de la ligne */
		  inputLine[i - 1] = '\0';
		  /* garde une copie de la ligne avant traduction */
		  strncpy (srcline, inputLine, linelen);
		  linenb++;
		  /* traduit les caracteres de la ligne */
		  OctalToChar ();
		  /* analyse la ligne */
		  wi = 1;
		  wl = 0;
		  /* analyse tous les mots de la ligne courante */
		  do
		    {
		       i = wi + wl;
		       GetNextToken (i, &wi, &wl, &wn);	/* mot suivant */
		       if (wi > 0)
			 {
			    /* on a trouve un mot */
			    if (wn == strng)
			       checkword (wi, wl);	/* mot-cle valide ? */
			    if (!error)
			       AnalyzeToken (wi, wl, wn, &c, &r, &nb, &pr);		/* on analyse le mot */
			    if (!error)
			       ProcessToken (wi, wl, c, r, nb);	/* on le traite */
			 }
		    }
		  while (wi != 0 && !error);
		  /* il n'y a plus de mots dans la ligne */
		  /* ecrit la ligne source dans le fichier .LST */
		  putc ((char) (HT), list);
		  wi = 0;
		  while (srcline[wi] != '\0')
		     putc (srcline[wi++], list);
		  fprintf (list, "\n");
	       }
	     if (!error)
		ParserEnd ();	/* fin d'analyse */
	     if (!error)
		if (defrefok ())
		  {
		     TtaDisplaySimpleMessage (INFO, GRM, GRM_CREATING_GRAMMAR_FILE);
		     writefiles ();	/* ecrit les tables dans le fichier */
		  }
	     fclose (infile);
	     fclose (list);
	     TtaSaveAppRegistry ();
	  }
     }
   exit (0);
}
