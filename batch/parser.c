
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
   analsynt.c : Ce module effectue l'analyse syntaxique d'un texte source dont
   il recoit les mots un par un.
   Il est parametre par une grammaire qui lui est fournie dans la table
   ruletable ou dans un fichier de type .GRM

   V. Quint     Juin 1984
   IV : Mai 92   adaptation Tool Kit
 */

#include "thot_sys.h"
#include "constgrm.h"
#include "typegrm.h"
#include "constmedia.h"
#include "typemedia.h"
#include "compilmsg.h"
#include "message.h"
#include "registry.h"
#include "storage.h"

typedef gname      *ptrgname;
typedef struct _stackelem
  {
     int                 strule;	/* numero de la regle */
     int                 struleptr;	/* pointeur dans la regle */
     boolean             option;	/* option en cours */
     boolean             alt;	/* on peut chercher une alternative */
     boolean             tested;	/* regle en cours de test avec le mot courant */
  }
stackelem;

typedef char        bufgname[200];
typedef bufgname   *ptrbufgname;

#define EXPORT extern
#include "compil.var"
#undef EXPORT
#define EXPORT
#include "analsynt.var"
#include "compilmsg.f"

extern int          linenb;	/* Numero de la ligne courante dans le fichier en cours

				   de compilation */

#define STACKSIZE 40		/* taille de la pile */

static boolean      comment;	/* on est dans un commentaire */
static int          sptr;	/* pointeur de pile */
static stackelem    stack[STACKSIZE];	/* pile d'analyse */

#include "storage.f"
#include "analsynt.f"

/* ---------------------------------------------------------------------- */
/* |    initsynt initialise les donnees de l'analyseur syntaxique.      | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                initsynt ()

#else  /* __STDC__ */
void                initsynt ()
#endif				/* __STDC__ */

{
   comment = False;		/* pas de commentaire en cours */
   sptr = 0;			/* initialise la pile */
   stack[0].strule = 0;
   stack[0].struleptr = 1;	/* au debut de la regle initiale */
}


/* ---------------------------------------------------------------------- */
/* |    testshortkw teste si le caractere qui est a` la position index  | */
/* |            dans la ligne courante est un mot-cle court. Rend dans  | */
/* |            ret le code de ce mot-cle ou 0 si ce n'est pas un       | */
/* |            mot-cle court.                                          | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         testshortkw (iline index, grmcode * ret)

#else  /* __STDC__ */
static void         testshortkw (index, ret)
iline               index;
grmcode            *ret;

#endif /* __STDC__ */

{
   int                 i;

   *ret = 0;
   i = 0;
   do
     {
	if (inputLine[index - 1] == kwtable[i].kwname[0])
	   *ret = kwtable[i].gcode;
	i++;
     }
   while (*ret == 0 && i < lastshortkw);
}


/* ---------------------------------------------------------------------- */
/* |    testkeyword teste si le mot de longueur lg qui commence a` la   | */
/* |            position index dans la ligne courante est un mot-cle'   | */
/* |            long. Rend dans ret le code de ce mot-cle' ou 0 si ce   | */
/* |            n'est pas un mot cle long.                              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         testkeyword (iline index, iline lg, grmcode * ret)

#else  /* __STDC__ */
static void         testkeyword (index, lg, ret)
iline               index;
iline               lg;
grmcode            *ret;

#endif /* __STDC__ */

{
   int                 i;

   *ret = 0;
   i = lastshortkw;
   do
     {
	if (kwtable[i].kwlg == lg)
	   if (strncasecmp (&inputLine[index - 1], kwtable[i].kwname, lg) == 0)
	      *ret = kwtable[i].gcode;
	i++;
     }
   while (*ret == 0 && i < lgkwtable);
}


/* ---------------------------------------------------------------------- */
/* |    testident teste si le mot de longueur lg qui commence a` la     | */
/* |            position index dans la ligne courante est dans la table | */
/* |            des identificateurs. Rend dans ret le code du type      | */
/* |            grammatical de cet identificateur ou 0 s'il n'est       | */
/* |            pas dans la table. Rend dans nb le rang de              | */
/* |            l'identificateur dans la table identtable.              | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         testident (iline index, iline lg, grmcode * ret, int *nb)

#else  /* __STDC__ */
static void         testident (index, lg, ret, nb)
iline               index;
iline               lg;
grmcode            *ret;
int                *nb;

#endif /* __STDC__ */

{
   int                 i;

   *ret = 0;
   *nb = 0;
   i = 0;
   do
     {
	if (identtable[i].identlg == lg)
	   if (strncmp (&inputLine[index - 1], identtable[i].identname, lg) == 0)
	     {
		*nb = i + 1;
		*ret = identtable[i].identtype;
	     }
	i++;
     }
   while (*ret == 0 && i < lgidenttable);
}


/* ---------------------------------------------------------------------- */
/* |    addident ajoute a` la table des identificateurs le mot de       | */
/* |            longueur lg qui commence a` la position index dans la   | */
/* |            ligne courante et qui est de type grammatical code.     | */
/* |            Rend dans nb le rang de cet identificateur dans la      | */
/* |            table identtable.                                       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static void         addident (iline index, iline lg, grmcode code, int *nb)

#else  /* __STDC__ */
static void         addident (index, lg, code, nb)
iline               index;
iline               lg;
grmcode             code;
int                *nb;

#endif /* __STDC__ */

{
   iline               i;

   *nb = 0;
   if (lgidenttable >= nbident)
      CompilerError (index, COMPIL, FATAL, COMPIL_IDENTIFIER_TABLE_IS_FULL,
		     inputLine, linenb);
   else if (lg > identlen)
      CompilerError (index, COMPIL, FATAL, COMPIL_WORD_TOO_LONG,
		     inputLine, linenb);
   else
     {
	identtable[lgidenttable].identlg = lg;
	identtable[lgidenttable].identdef = 0;
	identtable[lgidenttable].identref = 0;
	identtable[lgidenttable].identtype = code;
	for (i = 0; i < lg; i++)
	   identtable[lgidenttable].identname[i] = inputLine[index + i - 1];
	lgidenttable++;
	*nb = lgidenttable;
     }
}


/* ---------------------------------------------------------------------- */
/* |    trnb traduit le nombre qui est sous sa forme ASCII a` la        | */
/* |            position index de la ligne courante et qui est de       | */
/* |            longueur lg.                                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 trnb (iline index, iline lg)

#else  /* __STDC__ */
int                 trnb (index, lg)
iline               index;
iline               lg;

#endif /* __STDC__ */

{
   int                 nb;

   sscanf (&inputLine[index - 1], "%d", &nb);
   if (nb > 65535)
     {
	CompilerError (index, COMPIL, FATAL, COMPIL_NUMBER_TOO_GREAT, inputLine, linenb);
	nb = 0;
     }
   return nb;
}


/* ---------------------------------------------------------------------- */
/* |    transchar remplace dans le buffer d'entree inputLine les        | */
/* |            sequences \nn par le caractere dont le code octal est   | */
/* |            nn. Remplace aussi \\ par \.                            | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                transchar ()

#else  /* __STDC__ */
void                transchar ()
#endif				/* __STDC__ */

{
   int                 i, d, k, n;

   i = 0;
   while (inputLine[i] != '\0')
     {
	if (inputLine[i] == '\\')
	  {
	     d = 0;
	     if (inputLine[i + 1] == '\\')
		d = 1;
	     else if (inputLine[i + 1] >= '0' && inputLine[i + 1] <= '7')
	       {
		  /* \ suivi d'un chiffre octal */
		  k = i + 1;
		  n = 0;
		  while (inputLine[k] >= '0' && inputLine[k] <= '7' && k < i + 4)
		     n = n * 8 + ord (inputLine[k++]) - ord ('0');
		  if (n < 1 || n > 255)
		     CompilerError (i, COMPIL, FATAL, COMPIL_INVALID_CHARACTER,
				    inputLine, linenb);
		  else
		    {
		       inputLine[i] = (char) n;
		       d = k - i - 1;
		    }
	       }
	     if (d > 0)
		/* decale la fin de la ligne de d caracteres vers la gauche */
	       {
		  k = i + 1;
		  do
		    {
		       inputLine[k] = inputLine[k + d];
		       k++;
		    }
		  while (inputLine[k - 1] != '\0');
	       }
	  }
	else if (inputLine[i] < ' ')
	   inputLine[i] = ' ';
	i++;
     }
}


/* ---------------------------------------------------------------------- */
/* |    getword cherche le prochain mot a` partir de la position start  | */
/* |            dans la ligne courante. Au retour:                      | */
/* |            - wi: position dans la ligne du debut du mot trouve, ou | */
/* |            si pas trouve, 0.                                       | */
/* |            - wl: longueur du mot trouve, ou 0 si pas trouve. (lg+1 | */
/* |            si wn=strng).                                           | */
/* |            - wn: nature du mot trouve, ou err si pas trouve.       | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                getword (iline start, iline * wi, iline * wl, nature * wn)

#else  /* __STDC__ */
void                getword (start, wi, wl, wn)
iline               start;
iline              *wi;
iline              *wl;
nature             *wn;

#endif /* __STDC__ */

{
   iline               j, k;
   boolean             stop;

   *wi = 0;
   *wl = 0;
   *wn = err;
   stop = False;
   j = start - 1;
   do
     {
	/* saute les caracteres vides et commentaires */
	/* les commentaires sont delimites par des accolades exclusivement */
	if (comment)
	   /* cherche la fin du commentaire ou de la ligne */
	  {
	     while (inputLine[j] != '}' && inputLine[j] != '\0')
		j++;
	     if (inputLine[j] == '}')
	       {
		  comment = False;
		  j++;
	       }
	  }
	if (inputLine[j] != ' ')
	   if (inputLine[j] == '\0')
	      stop = True;
	   else if (inputLine[j] == '{')
	      /* debut de commentaire */
	     {
		comment = True;
		while (inputLine[j] != '}' && inputLine[j] != '\0')
		   j++;
		if (inputLine[j] == '\0')
		   stop = True;
		else
		   comment = False;
	     }
	   else
	     {
		stop = True;
		*wi = j + 1;
	     }
	j++;
     }
   while (!(stop));

   /* analyse les caracteres significatifs */
   if (*wi > 0 && !error)
     {
	stop = False;
	j = *wi - 1;
	*wl = 1;
	/* nature du mot trouve, d'apres son premier caractere */
	if (inputLine[j] >= '0' && inputLine[j] <= '9')
	   *wn = number;
	else if ((inputLine[j] >= 'A' && inputLine[j] <= 'Z')
		 || inputLine[j] == '\240'	/*nobreakspace */
		 || (inputLine[j] >= 'a' && inputLine[j] <= 'z')

		 || (ord (inputLine[j]) >= 192 && ord (inputLine[j]) <= 255))
	   *wn = name;
	else if (inputLine[j] == '\'')
	   *wn = strng;
	else
	   *wn = shortkw;
	j++;
	if (*wn == number || *wn == name)
	   /* verifie que le mot est bien forme et cherche la fin */
	   do
	     {
		if (inputLine[j] == '\0'
		    || inputLine[j] == ' '
		    || (inputLine[j] >= '!' && inputLine[j] <= '/')
		    || (inputLine[j] >= ':' && inputLine[j] <= '@')
		    || (inputLine[j] >= '[' && inputLine[j] <= '^')
		    || (inputLine[j] >= '{' && inputLine[j] <= '~')
		    || inputLine[j] == (char) (127)
		    || inputLine[j] == '`')
		   /* on a trouve un separateur */
		   stop = True;	/* verifie l'homogeneite */
		else
		  {
		     switch (*wn)
			   {
			      case number:
				 if (!(inputLine[j] >= '0' && inputLine[j] <= '9'))
				   {
				      CompilerError (j + 1, COMPIL, FATAL, COMPIL_INCOR_NUMBER,
						     inputLine, linenb);
				      *wn = err;
				      stop = True;
				   }
				 break;
			      case name:
				 if (!((inputLine[j] >= 'A' && inputLine[j] <= 'Z')
				       || (inputLine[j] == '\240' /*nobreakspace */ )

				       || (inputLine[j] >= 'a' && inputLine[j] <= 'z')
				       || (inputLine[j] >= '0' && inputLine[j] <= '9')
				       || (ord (inputLine[j]) >= 192 && ord (inputLine[j]) <= 255)
				 /* lettre accentuee */
				       || inputLine[j] == '_'))
				   {
				      CompilerError (j + 1, COMPIL, FATAL, COMPIL_NCORRECT_WORD,
						     inputLine, linenb);
				      *wn = err;
				      stop = True;
				   }
				 break;
			      default:
				 break;
			   }
		     (*wl)++;
		  }
		j++;
	     }
	   while (!(stop));
	else if (*wn == strng)
	   /* chaine de caracteres */
	  {
	     /* saute le quote initial */
	     (*wi)++;
	     *wl = 0;
	     do
		if (inputLine[j] == '\0')
		  {
		     CompilerError (*wi,
		       COMPIL, FATAL, COMPIL_STRINGS_CANNOT_EXCEED_ONE_LINE,
				    inputLine, linenb);
		     *wn = err;
		     stop = True;
		  }
		else
		  {
		     if (inputLine[j] == '\'')
			if (inputLine[j + 1] == '\'')
			   /* represente un seul quote */
			  {
			     k = j;
			     do
				/* ecrase le quote double */
			       {
				  k++;
				  inputLine[k] = inputLine[k + 1];
			       }
			     while (!(inputLine[k] == '\0'));
			     j++;
			  }
			else
			   stop = True;		/* quote final */
		     else
			j++;
		     (*wl)++;
		  }
	     while (!(stop));
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* |    wordmatch retourne vrai si dans la ligne courante le mot        | */
/* |            commencant a` l'index wi de longueur wl et de nature wn | */
/* |            correspond a` l'element de code c qui apparait dans la  | */
/* |            regle r de la grammaire. Rend dans nb le rang du mot    | */
/* |            dans la table identtable si c'est un identificateur.    | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
static boolean      wordmatch (iline wi, iline wl, nature wn, grmcode c, grmcode r, int *nb)

#else  /* __STDC__ */
static boolean      wordmatch (wi, wl, wn, c, r, nb)
iline               wi;
iline               wl;
nature              wn;
grmcode             c;
grmcode             r;
int                *nb;

#endif /* __STDC__ */

{
   grmcode             code;
   boolean             match;

   match = False;		/* on verra bien... */
   if (c < 1000)
      /* identificateur attendu */
      match = True;		/* a priori ca marche */
   else if (c >= 1000 && c < 1100)
      /* mot-cle court attendu */
     {
	if (wl == 1 && wn == shortkw)
	   /* c'est un mot-cle court */
	  {
	     testshortkw (wi, &code);	/* est-il valide ? */
	     if (code > 0)
		/* mot-cle court valide */
	       {
		  if (code == c)
		     /* c'est le mot-cle court attendu */
		     match = True;
	       }
	     else
		/* mot-cle court invalide */
		CompilerError (wi, COMPIL, FATAL, COMPIL_INVALID_SYMBOL, inputLine, linenb);
	  }
     }
   else if (c >= 1100 && c < 2000)
      /* mot-cle long attendu */
     {
	testkeyword (wi, wl, &code);
	/* est-ce un mot-cle long valide ? */
	if (code == c)
	   match = True;
	/* c'est le mot-cle long attendu */
     }
   else if (c > 3000)
      /* type de base attendu */
     {
	if (c == 3001 && wn == name)
	   /* on attend un nom et c'est un nom */
	  {
	     testident (wi, wl, &code, nb);
	     /* est-il dans la table ? */
	     if (code > 0)
		/* il y est */
	       {
		  if (code == r)
		     match = True;
		  /* oui, et avec le type attendu */
	       }
	     else
		/* non, il n'est pas encore connu */
	       {
		  testkeyword (wi, wl, &code);
		  /* est-ce un mot-cle long ? */
		  if (code == 0)
		     /* ce n'est pas un mot-cle long, c'est */
		     /* donc un identificateur */
		    {
		       addident (wi, wl, r, nb);
		       /* on l'ajoute dans la table */
		       match = True;	/* et c'est bon... */
		    }
	       }
	  }
	if (c == 3002 && wn == number)
	   match = True;
	if (c == 3003 && wn == strng)
	   match = True;
     }
   return match;
}


/* ---------------------------------------------------------------------- */
/* |    analword procede a` l'analyse du mot commencant a` la postion wi| */
/* |            de la ligne courante (inputLine), de longueur wl et de  | */
/* |            nature wn. Rend dans c le code grammatical du mot, dans | */
/* |            r le numero de la derniere regle ou` il a ete trouve et | */
/* |            dans nb son rang dans identtable, si c'est un           | */
/* |            identificateur. Dans pr se trouve le numero de l'avant  | */
/* |            derniere regle appliquee.                               | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                analword (iline wi, iline wl, nature wn, grmcode * c, rnb * r, int *nb, rnb * pr)

#else  /* __STDC__ */
void                analword (wi, wl, wn, c, r, nb, pr)
iline               wi;
iline               wl;
nature              wn;
grmcode            *c;
rnb                *r;
int                *nb;
rnb                *pr;

#endif /* __STDC__ */

{
   boolean             stop;
   boolean             st1;
   boolean             ok;
   boolean             meta;
   int                 s;

   if (sptr < 0)
      CompilerError (wi, COMPIL, FATAL, COMPIL_END_HAS_ALREADY_BEEN_REACHED,
		     inputLine, linenb);
   else
     {
	ok = False;
	stop = False;
	for (s = 0; s < sptr; s++)
	   stack[s].tested = False;
	/* aucune regle de la pile n'a ete testee avec ce mot, sauf celle
	   du haut, en cours */
	do
	   /* cherche une regle a laquelle le mot correspond */
	  {
	     meta = True;
	     /* traite les meta symboles de la regle courante */
	     do
	       {
		  if (ruletable[stack[sptr].strule][stack[sptr].struleptr] >= 2000
		      && ruletable[stack[sptr].strule][stack[sptr].struleptr] <= 2005)
		     switch (ruletable[stack[sptr].strule][stack[sptr].struleptr])
			   {
			      case 2001:
				 /* debut d'option */
				 stack[sptr].option = True;
				 stack[sptr].struleptr++;
				 break;
			      case 2002:
				 /* fin d'option */
				 stack[sptr].option = False;
				 stack[sptr].struleptr++;
				 break;
			      case 2000:
			      case 2003:
				 /* fin de regle ou alternative */
				 sptr--;	/* depile une regle */
				 if (sptr < 0)
				    stop = True;
				 /* on est a la fin de la regle initiale */
				 else
				   {
				      if (stack[sptr].alt)
					 /* essaie une alternative de la nvelle regle */
					{
					   do
					      stack[sptr].struleptr++;
					   while (!(ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2003
						    || ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2000));
					   if (ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2003)
					      stack[sptr].struleptr++;
					}
				      else
					 stack[sptr].struleptr++;
				   }
				 break;
			      case 2004:
				 /* debut de repetition */
				 stack[sptr].struleptr++;
				 stack[sptr].option = True;
				 break;
			      case 2005:
				 /* fin de repetition */
				 if (stack[sptr].tested)
				    /* on a deja essaye ce mot, on saute */
				   {
				      stack[sptr].struleptr++;
				      stack[sptr].option = False;
				   }
				 else
				    /* ce mot n'a pas ete essaye, on y va */
				   {
				      do
					 stack[sptr].struleptr--;
				      while (!(ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2004));
				      stack[sptr].struleptr++;
				      stack[sptr].option = True;
				      stack[sptr].tested = True;
				   }
				 break;
			   }

		  else
		     meta = False;
	       }
	     while (meta && sptr >= 0);
	     if (sptr >= 0)
	       {
		  *c = ruletable[stack[sptr].strule][stack[sptr].struleptr];
		  if (wordmatch (wi, wl, wn, *c, stack[sptr].strule + 1, nb))
		     /* ca correspond */
		     if (*c < 1000)
			/* symbole non terminal */
			if (sptr >= STACKSIZE)
			   CompilerError (wi, COMPIL, FATAL, COMPIL_STACK_IS_FULL,
					  inputLine, linenb);
			else
			   /* empile la regle definissant ce symbole */
			  {
			     sptr++;
			     stack[sptr].strule = *c - 1;
			     stack[sptr].struleptr = 1;
			     stack[sptr].option = False;
			     stack[sptr].alt = True;
			     stack[sptr].tested = True;
			  }
		     else
		       {
			  ok = True;
			  *r = stack[sptr].strule + 1;
			  if (sptr > 0)
			     *pr = stack[sptr - 1].strule + 1;
			  for (s = 0; s < sptr; s++)
			    {
			       /* sur toute la pile */
			       stack[s].alt = False;
			       /* s'il y a une alternative, on ne peut */
			       /* plus en changer */
			       stack[s].option = False;
			    }
			  do
			     /* si on etait dans une partie */
			     /* optionnelle, on ne peut plus la sauter */
			     /* traite les meta symboles suivant le mot ok */
			    {
			       stack[sptr].struleptr++;
			       if (ruletable[stack[sptr].strule][stack[sptr].struleptr] >= 2000
				   && ruletable[stack[sptr].strule][stack[sptr].struleptr] <= 2005)
				  switch (ruletable[stack[sptr].strule][stack[sptr].struleptr])
					{
					   case 2001:
					      stop = True;	/* debut d'option */
					      break;
					   case 2002:
					      stack[sptr].option = False;	/* fin d'option */
					      break;
					   case 2000:
					   case 2003:
					      /* fin de regle ou alternative */
					      sptr--;	/* regle epuisee */
					      if (sptr < 0)
						 stop = True;
					      /* on est a la fin de la regle initiale */
					      break;
					   case 2004:
					      stop = True;	/* debut de repetition */
					      break;
					   case 2005:
					      /* fin de repetition */
					      do
						 stack[sptr].struleptr--;
					      while (!(ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2004));
					      stack[sptr].struleptr++;
					      stack[sptr].option = True;
					      stop = True;
					      break;
					}

			       else
				  stop = True;
			    }
			  while (!stop && sptr >= 0);
		       }
		  else
		    {
		       /* le mot ne correspond pas */
		       if (stack[sptr].option)	/* saute la partie optionnelle */
			  /* cherche la fin de la partie a option */
			 {
			    do
			       stack[sptr].struleptr++;
			    while (!(ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2002
				     || ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2005));
			    stack[sptr].struleptr++;
			    stack[sptr].option = False;
			    if (ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2000
				|| ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2003)
			       /* fin regle */
			      {
				 sptr--;	/* depile une regle */
				 if (sptr < 0)
				    stop = True;	/* pile vide */
				 else
				    stack[sptr].struleptr++;
			      }
			 }
		       else
			  /* ce n'est pas un element optionnel */
			 {
			    st1 = False;
			    do
			       /* cherche une alternative dans la regle */
			      {
				 do
				    stack[sptr].struleptr++;
				 while (!(ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2003
					  || ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2000));
				 if (ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2003)
				    /* une alternative dans la regle */
				   {
				      if (stack[sptr].alt)
					 /* alternative autorisee */
					{
					   stack[sptr].struleptr++;	/* essaie l'alternative */
					   st1 = True;
					}
				   }
				 else
				    /* pas d'alternative dans la regle */
				    /* depile les regles jusqu'a en trouver une ou on est */
				    /* dans une partie optionnelle ou un choix */
				   {
				      while (sptr >= 0 && !st1)
					{
					   sptr--;
					   if (sptr >= 0)
					     {
						if (stack[sptr].option)
						   /* saute la partie optionnelle */
						   /* cherche la fin de la partie a option */
						  {
						     do
							stack[sptr].struleptr++;
						     while (!(ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2002
							      || ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2005));
						     stack[sptr].struleptr++;
						     stack[sptr].option = False;
						     st1 = True;
						  }
						else if (stack[sptr].alt)
						   /* cherche une alternative */
						  {
						     do
							stack[sptr].struleptr++;
						     while (!(ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2003
							      || ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2000));
						     if (ruletable[stack[sptr].strule][stack[sptr].struleptr] == 2003)
						       {
							  stack[sptr].struleptr++;
							  st1 = True;
						       }
						  }
					     }
					}
				      st1 = True;
				   }
			      }
			    while (!(st1));
			 }
		    }
	       }
	  }
	while (!stop && sptr >= 0);	/* mot ok ou fin de regle */
	if (!ok)
	   CompilerError (wi, COMPIL, FATAL, COMPIL_SYNTAX_ERR, inputLine, linenb);
     }
}


/* ---------------------------------------------------------------------- */
/* |    termsynt verifie, en fin de fichier source, que tout est correct| */
/* |            du point de vue syntaxique.                             | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                termsynt ()

#else  /* __STDC__ */
void                termsynt ()
#endif				/* __STDC__ */

{
   if (sptr >= 0)
      /* la pile n'est pas vide */
      CompilerError (1, COMPIL, FATAL, COMPIL_ABNORMAL_END, inputLine, linenb);
   else
     {
	/* la pile est vide */
	if (ruletable[stack[0].strule][stack[0].struleptr] != 2000)
	   /* la regle initiale n'est pas terminee */
	   CompilerError (1, COMPIL, FATAL, COMPIL_ABNORMAL_END, inputLine, linenb);
     }
}


/* ---------------------------------------------------------------------- */
/* |    initgrm initialise la table des mots-cles et la table des regles| */
/* |            a` partir d'un fichier grammaire de type GRM. fn est le | */
/* |            nom du fichier grammaire, avec le suffixe .GRM.         | */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
void                initgrm (gname fn)

#else  /* __STDC__ */
void                initgrm (fn)
gname               fn;

#endif /* __STDC__ */

{
   iline               j, wind, wlen;
   nature              wnat;
   int                 l;
   boolean             defkwd;
   int                 ruleptr;
   int                 currule;
   bufgname            pgrname;
   bufgname            pnomcourt;
   kwelem             *pkw1;
   BinFile             grmfile;
   boolean             fileOK;

   strcpy (pnomcourt, fn);
   /* cherche dans le directory compil si le fichier grammaire existe */
   if (SearchFile (pnomcourt, 3, pgrname) == 0)
      CompilerError (0, COMPIL, FATAL, COMPIL_GRAMMAR_FILE_GRM_NOT_FOUND,
		     inputLine, linenb);
   else
     {
	/* ouvre le fichier grammaire */
	grmfile = BIOreadOpen (pgrname);
	/* le fichier des mots-cles est ouvert */
	lgkwtable = 0;
	lastshortkw = 0;
	defkwd = True;		/* on commence par les mots-cles */
	lgruletable = 0;
	ruleptr = 0;
	fileOK = True;
	while (fileOK)
	   /* lit une ligne */
	  {
	     j = 0;
	     do
	       {
		  fileOK = BIOreadByte (grmfile, &inputLine[j]);
		  j++;
	       }
	     while (j < linelen && inputLine[j - 1] != '\n' && fileOK);
	     /* marque la fin reelle de la ligne */
	     inputLine[j - 1] = '\0';
	     /* traite la ligne */
	     j = 1;
	     do
	       {
		  getword (j, &wind, &wlen, &wnat);
		  /* mot suivant de la ligne */
		  if (defkwd)
		     /* definition des mots-cles */
		    {
		       if (wnat == name && lastshortkw == 0)
			  lastshortkw = lgkwtable;
		       /* on passe des courts aux longs */
		       if (wnat == name || wnat == shortkw)
			 {
			    if (lgkwtable >= nbkw)
			       /* table saturee */
			       CompilerError (wind, COMPIL, FATAL, COMPIL_KEYWORD_TABLE_FULL,
					      inputLine, linenb);
			    else
			       lgkwtable++;
			    /* entree suivante de la table */
			    pkw1 = &kwtable[lgkwtable - 1];
			    /* remplit cette nouvelle entree */
			    if (wlen > kwlen)
			      {
				 wlen = kwlen;
				 CompilerError (wind, COMPIL, FATAL, COMPIL_KEYWORD_TOO_LONG,
						inputLine, linenb);
			      }
			    pkw1->kwlg = wlen;
			    for (l = 1; l <= wlen; l++)
			      {
				 pkw1->kwname[l - 1] = inputLine[wind + l - 2];
				 if ((char) (ord (pkw1->kwname[l - 1]) - 32) >= 'A'
				     && (char) (ord (pkw1->kwname[l - 1]) - 32) <= 'Z')
				    pkw1->kwname[l - 1] = (char) (ord (pkw1->kwname[l - 1]) - 32);
			      }
			    /* traduit */
			    /* le mot-cle en majuscules */
			    j = wind + wlen;
			    getword (j, &wind, &wlen, &wnat);
			    /* lit le code */
			    /* grammatical du mot-cle */
			    if (wnat == number)
			       pkw1->gcode = trnb (wind, wlen);
			    else
			       /* fichier incorrect */
			       CompilerError (wind, COMPIL, FATAL, COMPIL_INCOR_GRAMMAR_FILE_GRM, inputLine, linenb);
			 }
		       else if (wnat == number)
			  /* fin de la section mots-cles */
			  defkwd = False;
		    }
		  if (!defkwd)
		     /* definition de la table des regles */
		     if (wnat == number)
			if (ruleptr == 0)
			   /* nouvelle regle */
			  {
			     currule = trnb (wind, wlen);	/* numero de regle */
			     if (currule > maxrule)
			       {
				  /* table des regles saturee */
				  CompilerError (wind, COMPIL, FATAL, COMPIL_GRAMMAR_TABLE_FULL, inputLine, linenb);
				  currule = maxrule;
			       }
			     ruletable[currule - 1][0] = 0;
			     ruleptr = 1;
			     if (currule > lgruletable)
				lgruletable = currule;
			  }
			else
			  {
			     /* on est dans une regle */
			     ruletable[currule - 1][ruleptr] = trnb (wind, wlen);
			     if (ruletable[currule - 1][ruleptr] == 2000)
				ruleptr = 0;	/* fin regle */
			     else if (ruleptr >= maxlgrule)
				/* regle trop longue */
				CompilerError (wind, COMPIL, FATAL, COMPIL_GRAMMAR_RULE_TOO_LONG, inputLine, linenb);
			     else
				ruleptr++;
			  }
		     else
			/* ce n'est pas un nombre */
		     if (wind > 0)
			/* fichier incorrect */
			CompilerError (wind, COMPIL, FATAL, COMPIL_INCOR_GRAMMAR_FILE_GRM, inputLine, linenb);
		  j = wind + wlen;	/* fin du mot */
	       }
	     while (!(wind == 0));	/* plus de mot dans la ligne */
	  }
	BIOreadClose (grmfile);
     }
}
