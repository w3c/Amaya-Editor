/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*----------------------------------------------------------------------
   printstr.c : Impression en clair sur la sortie standard du contenu 
   d'un fichier .STR contenant un schema de structure.  

   V. Quint     Mars 1985
  ----------------------------------------------------------------------*/


#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "registry.h"
#include "libmsg.h"
#include "strmsg.h"
#include "message.h"

#define THOT_EXPORT
#include "platform_tv.h"

static PtrSSchema   pSchStr;
static PtrSSchema   pSchStrExt;
static int          a;
static int          i, min, nb;
static Name         fn;
static int          r;
static int          PremRegle;
static boolean      Prem;
static boolean      optionh = False;
static boolean      optionl = False;
static int          NbCommentaire;
static int          STR;	/* Identification des messages Str */
PtrSSchema          pSc1;
SRule              *pRe1;
TtAttribute        *pAt1;

#include "readstr_f.h"

#ifdef __STDC__
extern char        *TtaGetEnvString (char *);
extern void         TtaInitializeAppRegistry (char *);
extern void         TtaSaveAppRegistry (void);

#else
extern char        *TtaGetEnvString ();
extern void         TtaInitializeAppRegistry ();
extern void         TtaSaveAppRegistry ();

#endif /* __STDC__ */

/*----------------------------------------------------------------------
   wrcar ecrit un caractere.                                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrcar (unsigned char ch)

#else  /* __STDC__ */
static void         wrcar (ch)
unsigned char       ch;

#endif /* __STDC__ */

{
   int                 c;

   if (!optionh)
     {
	if (ch < ' ' || ch > '~')
	   /* caractere non imprimable, on ecrit son code octal apres \ */
	  {
	     c = (int) ch;
	     printf ("\\%o", c);
	  }
	else
	   putchar (ch);	/* caractere imprimable, on l'ecrit */
     }
   else
     {
	if (ch < ' ' || ch > '~')
	   /* caractere non imprimable, on le remplace par un caractere imprimable */
	  {
	     c = (int) ch;
	     switch (c)
		   {
		      case 0340:
		      case 0341:
		      case 0342:
		      case 0343:
		      case 0344:
		      case 0345:
			 printf ("a");
			 break;
		      case 0346:
			 printf ("ae");
			 break;
		      case 0347:
			 printf ("c");
			 break;
		      case 0350:
		      case 0351:
		      case 0352:
		      case 0353:
			 printf ("e");
			 break;
		      case 0354:
		      case 0355:
		      case 0356:
		      case 0357:
			 printf ("i");
			 break;
		      case 012:
		      case 021:
		      case 0240:
			 printf ("_");
			 break;
		      case 022:
			 printf ("n");
			 break;
		      case 0362:
		      case 0363:
		      case 0364:
		      case 0365:
		      case 0366:
			 printf ("o");
			 break;
		      case 0230:
			 printf ("oe");
			 break;
		      case 0371:
		      case 0372:
		      case 0373:
		      case 0374:
			 printf ("u");
			 break;
		      case 036:
			 printf ("y");
			 break;
		      default:
			 printf ("\\%o", c);
			 break;
		   }
	  }
	else
	  {
	     /* caractere imprimable, on l'ecrit */
	     putchar (ch);
	  }
     }
}


/*----------------------------------------------------------------------
   writenb ecrit un nombre.                                        
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         writenb (int n)

#else  /* __STDC__ */
static void         writenb (n)
int                 n;

#endif /* __STDC__ */

{
   if (n >= 100000)
      printf ("%6d", n);
   else if (n >= 10000)
      printf ("%5d", n);
   else if (n >= 1000)
      printf ("%4d", n);
   else if (n >= 100)
      printf ("%3d", n);
   else if (n >= 10)
      printf ("%2d", n);
   else
      printf ("%1d", n);
}


/*----------------------------------------------------------------------
   wrnom ecrit une chaine de caracteres.                           
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrnom (Name n)

#else  /* __STDC__ */
static void         wrnom (n)
Name                n;

#endif /* __STDC__ */

{
   int                 i;

   i = 0;
   while (n[i] != '\0')
     {
	wrcar (n[i]);
	i++;
     }
}


/*----------------------------------------------------------------------
   wrnomregle ecrit un nom de regle.                               
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrnomregle (int r)

#else  /* __STDC__ */
static void         wrnomregle (r)
int                 r;

#endif /* __STDC__ */

{
   if (pSchStr->SsRule[r - 1].SrName[0] == '\0')
      /* Name vide, on ecrit son numero */
     {
	printf ("Ident");
	writenb (r);
     }
   else
      wrnom (pSchStr->SsRule[r - 1].SrName);
}


/*----------------------------------------------------------------------
   DebutComment debute un commentaire.                             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         DebutComment ()

#else  /* __STDC__ */
static void         DebutComment ()
#endif				/* __STDC__ */

{
   if (NbCommentaire == 0)
      printf ("{ ");
   NbCommentaire++;
}


/*----------------------------------------------------------------------
   FinComment finit un commentaire.                                
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         FinComment ()

#else  /* __STDC__ */
static void         FinComment ()
#endif				/* __STDC__ */

{
   NbCommentaire--;
   if (NbCommentaire == 0)
      printf (" }");
}

/*----------------------------------------------------------------------
   wrNULL ecrit NULL.                                              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrNULL ()

#else  /* __STDC__ */
static void         wrNULL ()
#endif				/* __STDC__ */

{

   DebutComment ();
   printf ("NULL");
   FinComment ();
}


/*----------------------------------------------------------------------
   wrnomreglext ecrit le nom de la regle de numero r. Cette regle  
   fait partie du schema de structure traite' si N est nul.
   Sinon elle fait partie du schema de nom N.              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrnomreglext (int r, Name N)

#else  /* __STDC__ */
static void         wrnomreglext (r, N)
int                 r;
Name                N;

#endif /* __STDC__ */

{
   if (N[0] == '\0')
     {
	if (pSchStr->SsRule[r - 1].SrConstruct == CsPairedElement)
	   if (!pSchStr->SsRule[r - 1].SrFirstOfPair)
	      printf ("Second ");
	wrnomregle (r);
     }
   else
     {
	if (!ReadStructureSchema (N, pSchStrExt))
	   printf ("******");
	else
	  {
	     if (pSchStrExt->SsRule[r - 1].SrConstruct == CsPairedElement)
		if (!pSchStrExt->SsRule[r - 1].SrFirstOfPair)
		   printf ("Second ");
	     wrnom (pSchStrExt->SsRule[r - 1].SrName);
	  }
	printf ("(");
	wrnom (N);
	printf (")");
     }
}


/*----------------------------------------------------------------------
   wrNumExcept ecrit le numero d'exception Num.                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrNumExcept (int Num)

#else  /* __STDC__ */
static void         wrNumExcept (Num)
int                 Num;

#endif /* __STDC__ */

{
   switch (Num)
	 {
	    case ExcNoCut:
	       printf ("NoCut");
	       break;
	    case ExcNoCreate:
	       printf ("NoCreate");
	       break;
	    case ExcNoHMove:
	       printf ("NoHMove");
	       break;
	    case ExcNoVMove:
	       printf ("NoVMove");
	       break;
	    case ExcNoHResize:
	       printf ("NoHResize");
	       break;
	    case ExcNoVResize:
	       printf ("NoVResize");
	       break;
	    case ExcNewWidth:
	       printf ("NewWidth");
	       break;
	    case ExcNewHeight:
	       printf ("NewHeight");
	       break;
	    case ExcNewHPos:
	       printf ("NewHPos");
	       break;
	    case ExcNewVPos:
	       printf ("NewVPos");
	       break;
	    case ExcInvisible:
	       printf ("Invisible");
	       break;
	    case ExcNoMove:
	       printf ("NoMove");
	       break;
	    case ExcNoResize:
	       printf ("NoResize");
	       break;
	    case ExcNoSelect:
	       printf ("NoSelect");
	       break;
	    case ExcMoveResize:
	       printf ("MoveResize");
	       break;
	    case ExcNoSpellCheck:
	       printf ("NoSpellCheck");
	       break;
	    case ExcIsDraw:
	       printf ("NoIsDraw");
	       break;
	    case ExcGraphCreation:
	       printf ("GraphCreation");
	       break;
	    case ExcHidden:
	       printf ("Hidden");
	       break;
	    case ExcPageBreak:
	       printf ("PageBreak");
	       break;
	    case ExcPageBreakAllowed:
	       printf ("PageBreakAllowed");
	       break;
	    case ExcPageBreakPlace:
	       printf ("PageBreakPlace");
	       break;
	    case ExcPageBreakRepetition:
	       printf ("PageBreakRepetition");
	       break;
	    case ExcPageBreakRepBefore:
	       printf ("PageBreakRepBefore");
	       break;
	    case ExcActiveRef:
	       printf ("ActiveRef");
	       break;
	    default:
	       printf ("%2d", Num);
	       break;
	 }
}


/*----------------------------------------------------------------------
   Wdefine ecrit #define.                                          
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         Wdefine ()

#else  /* __STDC__ */
static void         Wdefine ()
#endif				/* __STDC__ */

{
   printf ("#define ");
}


/*----------------------------------------------------------------------
   wrattr ecrit un attribut.                                       
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrattr (int a)

#else  /* __STDC__ */
static void         wrattr (a)
int                 a;

#endif /* __STDC__ */

{
   int                 j;
   TtAttribute        *pAt1;

   pAt1 = &pSchStr->SsAttribute[a - 1];
   if (optionh)
     {
	if (pAt1->AttrGlobal)
	   return;		/* AttrGlobal signifie ``l'attribut a deja ete ecrit'' */
	Wdefine ();
	wrnom (pSc1->SsName);
	printf ("_ATTR_");
	wrnom (pAt1->AttrName);
	printf (" ");
	writenb (a);
	printf ("\n");
     }
   else
      wrnom (pAt1->AttrName);
   if (!pAt1->AttrGlobal)
      /* AttrGlobal signifie ``les valeurs de */
      /* l'attribut ont deja ete ecrites'' */
     {
	if (!optionh)
	   printf (" = ");
	switch (pAt1->AttrType)
	      {
		 case AtNumAttr:
		    if (!optionh)
		       printf ("NUMBER");
		    break;
		 case AtTextAttr:
		    if (!optionh)
		       printf ("TEXT");
		    break;
		 case AtReferenceAttr:
		    if (!optionh)
		      {
			 printf ("REFERENCE(");
			 if (pAt1->AttrTypeRef == 0)
			    printf ("ANY");
			 else
			    wrnomreglext (pAt1->AttrTypeRef, pAt1->AttrTypeRefNature);
			 printf (")");
		      }
		    break;
		 case AtEnumAttr:
		    for (j = 1; j <= pAt1->AttrNEnumValues; j++)
		       if (optionh)
			 {
			    Wdefine ();
			    wrnom (pSc1->SsName);
			    printf ("_ATTR_");
			    wrnom (pAt1->AttrName);
			    printf ("_VAL_");
			    wrnom (pAt1->AttrEnumValue[j - 1]);
			    printf (" ");
			    writenb (j);
			    printf ("\n");
			 }
		       else
			 {
			    wrnom (pAt1->AttrEnumValue[j - 1]);
			    if (j < pAt1->AttrNEnumValues)
			       printf (", ");
			 }
		    break;
	      }

     }
   pAt1->AttrGlobal = True;
}


/*----------------------------------------------------------------------
   wrext ecrit le mot extern si la regle de numero r est une      
   reference d'inclusion de document externe.              
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         wrext (int r)

#else  /* __STDC__ */
static void         wrext (r)
int                 r;

#endif /* __STDC__ */

{
   SRule              *pRe1;

   pRe1 = &pSchStr->SsRule[r - 1];
   if (pRe1->SrRefImportedDoc)
      if (pRe1->SrConstruct == CsReference)
	 printf (" extern");
}


/*----------------------------------------------------------------------
   welembase ecrit un element de base.                             
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         welembase ()

#else  /* __STDC__ */
static void         welembase ()
#endif				/* __STDC__ */

{
   PtrSSchema          pSc1;
   int                 r;

   pSc1 = pSchStr;

   Wdefine ();
   wrnom (pSc1->SsName);
   r = CharString + 1;
   printf ("_EL_");
   wrnomregle (r);
   printf (" ");
   writenb (r);
   printf ("\n");

   Wdefine ();
   wrnom (pSc1->SsName);
   r = GraphicElem + 1;
   printf ("_EL_");
   wrnomregle (r);
   printf (" ");
   writenb (r);
   printf ("\n");

   Wdefine ();
   wrnom (pSc1->SsName);
   r = Symbol + 1;
   printf ("_EL_");
   wrnomregle (r);
   printf (" ");
   writenb (r);
   printf ("\n");

   Wdefine ();
   wrnom (pSc1->SsName);
   r = Picture + 1;
   printf ("_EL_");
   wrnomregle (r);
   printf (" ");
   writenb (r);
   printf ("\n");

   Wdefine ();
   wrnom (pSc1->SsName);
   r = PageBreak + 1;
   printf ("_EL_");
   wrnomregle (r);
   printf (" ");
   writenb (r);
   printf ("\n");
}


/*----------------------------------------------------------------------
   wrrule ecrit une regle.                                         
   si pRegleExtens est non nul, il s'agit d'une regle d'extension  
  ----------------------------------------------------------------------*/


#ifdef __STDC__
static void         wrrule (int r, SRule * pRegleExtens)

#else  /* __STDC__ */
static void         wrrule (r, pRegleExtens)
int                 r;
SRule              *pRegleExtens;

#endif /* __STDC__ */

{
   int                 i;
   PtrSSchema          pSc1;
   SRule              *pRe1;
   TtAttribute        *pAt1;

   pSc1 = pSchStr;
   if (pRegleExtens != NULL)
      pRe1 = pRegleExtens;
   else
      pRe1 = &pSc1->SsRule[r - 1];
   if (pRe1->SrConstruct != CsNatureSchema &&
       !(pRe1->SrConstruct == CsPairedElement && !pRe1->SrFirstOfPair))
     {
	if (optionh)
	  {
	     Wdefine ();
	     wrnom (pSc1->SsName);
	     if (pRegleExtens == NULL)
	       {
		  printf ("_EL_");
		  wrnomregle (r);
	       }
	     else
	       {
		  printf ("_EXT_");
		  if (pRe1->SrName[0] == '\0')
		     printf ("Root");
		  else
		     wrnom (pRe1->SrName);
	       }
	     printf (" ");
	     writenb (r);
	     printf ("\n");

	     if (pRe1->SrNLocalAttrs > 0)
	       {
		  for (i = 1; i <= pRe1->SrNLocalAttrs; i++)
		    {
		       /*          if (pRe1->SrRequiredAttr[i - 1])
		          printf("!"); */
		       wrattr (pRe1->SrLocalAttr[i - 1]);
		    }
	       }
	     return;
	  }
	printf (" ");
	DebutComment ();
	writenb (r);
	FinComment ();
	putchar ('\t');
	if (pRe1->SrRecursive)
	  {
	     DebutComment ();
	     printf ("Recursive");
	     FinComment ();
	  }
	if (pRegleExtens != NULL && pRe1->SrName[0] == '\0')
	   printf ("Root");
	else if (pRegleExtens != NULL)
	   wrnom (pRe1->SrName);
	else
	   wrnomregle (r);
	if (pRe1->SrNLocalAttrs > 0)
	  {
	     printf (" (ATTR ");
	     for (i = 1; i <= pRe1->SrNLocalAttrs; i++)
	       {
		  if (pRe1->SrRequiredAttr[i - 1])
		     printf ("!");
		  wrattr (pRe1->SrLocalAttr[i - 1]);
		  if (i == pRe1->SrNLocalAttrs)
		     printf (")");
		  else
		     printf (";\n%c%c", '\t', '\t');
	       }
	  }
	if (pRegleExtens == NULL)
	   printf (" = ");
	if (pRe1->SrNLocalAttrs > 0)
	   printf ("\n%c%c", '\t', '\t');
	switch (pRe1->SrConstruct)
	      {
		 case CsNatureSchema:
		    break;
		 case CsBasicElement:
		    printf ("? BASIC TYPE ?");
		    break;
		 case CsReference:
		    /* les references d'inclusion de documents externes sont */
		    /* traitees par la procedure wrext. */
		    if (!pRe1->SrRefImportedDoc)
		      {
			 printf ("REFERENCE(");
			 if (pRe1->SrReferredType == 0)
			    printf ("ANY");
			 else
			   {

			      wrnomreglext (pRe1->SrReferredType, pRe1->SrRefTypeNat);
			   }
			 printf (")");
		      }
		    break;
		 case CsIdentity:
		    if (pRe1->SrIdentRule == 0)
		       wrNULL ();
		    else
		      {
			 wrnomregle (pRe1->SrIdentRule);
			 wrext (pRe1->SrIdentRule);
		      }
		    break;
		 case CsList:
		    printf ("LIST");
		    if (pRe1->SrMinItems > 0 || pRe1->SrMaxItems < 32000)
		      {
			 printf ("[");
			 if (pRe1->SrMinItems <= 1)
			    printf ("*");
			 else
			    writenb (pRe1->SrMinItems);
			 printf ("..");
			 if (pRe1->SrMaxItems >= 32000)
			    printf ("*");
			 else
			    writenb (pRe1->SrMaxItems);
			 printf ("]");
		      }
		    printf (" OF (");
		    if (pRe1->SrListItem == 0)
		       wrNULL ();
		    else
		      {
			 wrnomregle (pRe1->SrListItem);
			 wrext (pRe1->SrListItem);
		      }
		    printf (")");
		    break;
		 case CsChoice:
		    if (pRe1->SrNChoices == -1)
		       printf ("NATURE");
		    else if (pRe1->SrNChoices == 0)
		       printf ("UNIT");
		    else
		      {
			 printf ("CASE OF ");
			 for (i = 1; i <= pRe1->SrNChoices; i++)
			    if (pRe1->SrChoice[i - 1] == 0)
			       wrNULL ();
			    else
			      {
				 wrnomregle (pRe1->SrChoice[i - 1]);
				 wrext (pRe1->SrChoice[i - 1]);
				 printf ("; ");
			      }
			 printf ("END");
		      }
		    break;
		 case CsAggregate:
		 case CsUnorderedAggregate:
		    if (pRe1->SrConstruct == CsAggregate)
		       printf ("BEGIN ");
		    else
		       printf ("AGGREGATE ");
		    for (i = 1; i <= pRe1->SrNComponents; i++)
		       if (pRe1->SrComponent[i - 1] == 0)
			  wrNULL ();
		       else
			 {
			    if (pRe1->SrOptComponent[i - 1])
			       printf ("? ");
			    wrnomregle (pRe1->SrComponent[i - 1]);
			    wrext (pRe1->SrComponent[i - 1]);
			    printf ("; ");
			 }
		    printf ("END");
		    break;
		 case CsConstant:
		    printf ("? CONSTANT ?");
		    break;
		 case CsPairedElement:
		    printf ("PAIR");
		    break;
		 case CsExtensionRule:

		    break;
	      }
	/* Fin du switch SrConstruct */
	/* ecrit les inclusions et les exclusions */
	if (pRe1->SrNInclusions > 0)
	  {
	     printf ("\n");
	     printf ("%c%c+ (", '\t', '\t');
	     for (i = 1; i <= pRe1->SrNInclusions; i++)
	       {
		  wrnomregle (pRe1->SrInclusion[i - 1]);
		  if (i < pRe1->SrNInclusions)
		     printf (", ");
		  else
		     printf (")");
	       }
	  }
	if (pRe1->SrNExclusions > 0)
	  {
	     printf ("\n");
	     printf ("%c%c- (", '\t', '\t');
	     for (i = 1; i <= pRe1->SrNExclusions; i++)
	       {
		  wrnomregle (pRe1->SrExclusion[i - 1]);
		  if (i < pRe1->SrNExclusions)
		     printf (", ");
		  else
		     printf (")");
	       }
	  }

	/* ecrit les attributs imposes */
	if (pRe1->SrNDefAttrs > 0)
	  {
	     printf ("\n");
	     printf ("%c%cWITH ", '\t', '\t');
	     for (i = 1; i <= pRe1->SrNDefAttrs; i++)
	       {
		  pAt1 = &pSc1->SsAttribute[pRe1->SrDefAttr[i - 1] - 1];
		  wrnom (pAt1->AttrName);
		  if (pAt1->AttrType != AtReferenceAttr)
		    {
		       if (pRe1->SrDefAttrModif[i - 1])
			  printf ("?");
		       printf ("=");
		    }
		  switch (pAt1->AttrType)
			{
			   case AtNumAttr:
			      writenb (pRe1->SrDefAttrValue[i - 1]);
			      break;
			   case AtTextAttr:
			      printf ("\'%s\'", pSc1->SsConstBuffer + pRe1->SrDefAttrValue[i - 1] - 1);
			      break;
			   case AtReferenceAttr:

			      break;
			   case AtEnumAttr:
			      wrnom (pAt1->AttrEnumValue[pRe1->SrDefAttrValue[i - 1] - 1]);
			      break;
			}

		  if (i < pRe1->SrNDefAttrs)
		     printf (", ");
	       }
	  }
	printf (";\n");
     }
}


/*----------------------------------------------------------------------
   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                printName (char *buffer)

#else  /* __STDC__ */
void                printName (buffer)
char               *buffer;

#endif /* __STDC__ */

{
   int                 i = 0;

   while (buffer[i] != '\0')
     {
	if (buffer[i] < ' ' || (unsigned char) buffer[i] > 127)
	   printf ("\\%o", (unsigned char) buffer[i]);
	else
	   printf ("%c", buffer[i]);
	i++;
     }
   printf (": \n");
}


/*----------------------------------------------------------------------
   main.                                                           
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 main (int argc, char **argv)

#else  /* __STDC__ */
int                 main (argc, argv)
int                 argc;
char              **argv;

#endif /* __STDC__ */

{
   char               *exec, c;
   int                 inputfile = False;
   int                 k, l;
   boolean             premattrlocal;

   TtaInitializeAppRegistry (argv[0]);
   STR = TtaGetMessageTable ("strdialogue", STR_MSG_MAX);
   exec = argv[0];

   for (argc--, argv++; argc > 0; argc--, argv++)
     {
	c = *argv[0];
	if (c != '-')
	  {
	     if (!inputfile)
	       {
		  strcpy (fn, *argv);
		  inputfile = True;
	       }
	     else
		goto Usage;
	  }
	else if ((*argv)[1] == 'h')
	   optionh = True;
	else if ((*argv)[1] == 'l')
	   optionl = True;
	else
	   goto Usage;
     }
   if (!inputfile)
      goto Usage;

   pSchStr = (PtrSSchema) malloc (sizeof (StructSchema));
   pSchStrExt = (PtrSSchema) malloc (sizeof (StructSchema));

   /* on utilise le directory courant */
   if (!ReadStructureSchema (fn, pSchStr))
      TtaDisplaySimpleMessage (FATAL, STR, STR_CANNOT_READ_STRUCT_SCHEM);
   else if (!optionl)
     {
	pSc1 = pSchStr;
	if (pSc1->SsExport)
	   if (!optionh)
	      printf ("{ This generic structure exports elements }\n");
	if (optionh)
	   printf ("/* Types and attributes for the document type %s */\n", pSc1->SsName);
	NbCommentaire = 0;
	if (!optionh)
	  {
	     printf ("STRUCTURE ");
	     if (pSc1->SsExtension)
		printf ("EXTENSION ");

	     wrnom (pSc1->SsName);
	     printf (";\n");
	     printf ("DEFPRES ");
	     wrnom (pSc1->SsDefaultPSchema);
	     printf (";\n");
	  }
	/* ecrit les attributs globaux */
	if (pSc1->SsNAttributes > 0 && pSc1->SsAttribute[0].AttrGlobal)
	  {
	     if (!optionh)
		printf ("ATTR\n");
	     else
		printf ("\n/* Global attributes */\n");
	  }

	for (i = 1; i <= pSc1->SsNAttributes; i++)
	   if (pSc1->SsAttribute[i - 1].AttrGlobal)
	     {
		if (!optionh)
		   printf ("   ");
		pSc1->SsAttribute[i - 1].AttrGlobal = False;
		/* indique a wrattr qu'il faut */
		/* ecrire les valeurs de l'attribut */
		wrattr (i);
		pSc1->SsAttribute[i - 1].AttrGlobal = True;
		if (!optionh)
		   printf (";\n");
	     }
	if (optionh)
	   /* ecrit les attributs locaux */
	  {
	     premattrlocal = True;
	     for (i = 1; i <= pSc1->SsNAttributes; i++)
		if (!pSc1->SsAttribute[i - 1].AttrGlobal)
		  {
		     if (premattrlocal)
		       {
			  printf ("\n/* Local attributes */\n");
			  premattrlocal = False;
		       }
		     wrattr (i);
		  }
	  }

	if (optionh)
	   /* ecrit les types de base */
	  {
	     printf ("\n/* Basic elements */\n");
	     welembase ();
	  }

	/* ecrit les constantes */
	r = MAX_BASIC_TYPE + 1;
	if (pSc1->SsRule[r - 1].SrConstruct == CsConstant)
	   if (!optionh)
	     {
		printf ("CONST\n");
		while (pSc1->SsRule[r - 1].SrConstruct == CsConstant)
		  {
		     pRe1 = &pSc1->SsRule[r - 1];
		     printf (" ");
		     DebutComment ();
		     writenb (r);
		     FinComment ();
		     putchar ('\t');
		     wrnomregle (r);
		     printf (" = \'");
		     i = pRe1->SrIndexConst;
		     while (pSc1->SsConstBuffer[i - 1] > '\0')
		       {
			  wrcar (pSc1->SsConstBuffer[i - 1]);
			  i++;
		       }
		     printf ("\';\n");
		     r++;
		  }
	     }
	   else
	     {
		printf ("\n/* Constants */\n");
		while (pSc1->SsRule[r - 1].SrConstruct == CsConstant)
		  {
		     wrrule (r, NULL);
		     r++;
		  }
	     }
	PremRegle = r;
	/* ecrit les parametres */
	Prem = True;
	for (r = PremRegle; r <= pSc1->SsNRules; r++)
	   if (pSc1->SsRule[r - 1].SrParamElem)
	     {
		if (Prem)
		  {
		     if (!optionh)
			printf ("PARAM\n");
		     else
			printf ("\n/* Parameters */\n");
		     Prem = False;
		  }
		wrrule (r, NULL);
	     }
	/* ecrit les elements structures */
	if (pSc1->SsNRules >= PremRegle)
	   if (optionh)
	      printf ("\n/* Elements */\n");
	   else
	      printf ("STRUCT\n");
	for (r = PremRegle; r <= pSc1->SsNRules; r++)
	  {
	     pRe1 = &pSc1->SsRule[r - 1];
	     /* saute les parametres, les elements associes, les */
	     /* elements Extern et Included et les unites */
	     if (!pRe1->SrParamElem &&
		 !pRe1->SrAssocElem &&
		 !pRe1->SrRefImportedDoc &&
		 !pRe1->SrUnitElem)
		/* ignore les regles listes ajoutees pour les elements associes */
		if (pRe1->SrConstruct != CsList)
		   wrrule (r, NULL);
		else if (!pSc1->SsRule[pRe1->SrListItem - 1].SrAssocElem)
		   wrrule (r, NULL);
	  }
	/* ecrit les regles d'extension */
	if (pSc1->SsExtension && pSc1->SsNExtensRules > 0)
	  {
	     if (optionh)
		printf ("\n/* Extension rules */\n");
	     else
		printf ("EXTENS\n");
	     for (r = 1; r <= pSc1->SsNExtensRules; r++)
	       {
		  pRe1 = &pSc1->SsExtensBlock->EbExtensRule[r - 1];
		  wrrule (r, pRe1);
	       }
	  }
	/* ecrit les elements associes */
	Prem = True;
	for (r = PremRegle; r <= pSc1->SsNRules; r++)
	   if (pSc1->SsRule[r - 1].SrAssocElem)
	     {
		if (Prem)
		  {
		     if (!optionh)
			printf ("ASSOC\n");
		     else
			printf ("\n/* Associated elements */\n");
		     Prem = False;
		  }
		wrrule (r, NULL);
	     }
	if (!Prem)
	   /* il y a au moins un element associe', on ecrit les regles listes */
	   /* ajoutees pour les elements associes */
	  {
	     if (!optionh)
	       {
		  DebutComment ();
		  printf ("lists of associated elements:\n");
	       }
	     for (r = PremRegle; r <= pSc1->SsNRules; r++)
	       {
		  pRe1 = &pSc1->SsRule[r - 1];
		  if (pRe1->SrConstruct == CsList)
		     if (pSc1->SsRule[pRe1->SrListItem - 1].SrAssocElem)
			wrrule (r, NULL);
	       }
	     if (!optionh)
	       {
		  FinComment ();
		  printf ("\n");
	       }
	  }
	/* ecrit les unites exportees */
	Prem = True;
	for (r = PremRegle; r <= pSc1->SsNRules; r++)
	   if (pSc1->SsRule[r - 1].SrUnitElem)
	     {
		if (Prem)
		  {
		     if (!optionh)
			printf ("UNITS\n");
		     else
			printf ("\n/* Units */\n");
		     Prem = False;
		  }
		wrrule (r, NULL);
	     }
	/* ecrit les types exportes */
	if (!optionh)
	   if (pSc1->SsExport)
	     {
		Prem = True;
		printf ("THOT_EXPORT\n");
		for (r = PremRegle; r <= pSc1->SsNRules; r++)
		  {
		     pRe1 = &pSc1->SsRule[r - 1];
		     if (pRe1->SrExportedElem)
		       {
			  if (!Prem)
			     printf (",\n");
			  Prem = False;
			  printf ("   ");
			  wrnomregle (r);
			  if (pRe1->SrExportContent == 0)
			     printf (" with nothing");
			  else
			    {
			       printf (" with ");
			       wrnomreglext (pRe1->SrExportContent, pRe1->SrNatExpContent);
			    }
		       }
		  }
		printf (";\n");
	     }
	/* ecrit les exceptions */
	if (!optionh)
	   if (pSc1->SsNExceptions > 0)
	     {
		printf ("EXCEPT\n");
		/* ecrit d'abord les exceptions de tous les attributs */
		for (a = 1; a <= pSc1->SsNAttributes; a++)
		  {
		     pAt1 = &pSc1->SsAttribute[a - 1];
		     if (pAt1->AttrFirstExcept > 0)
			/* cet attribut a des exceptions */
		       {
			  printf ("   ");
			  wrnom (pAt1->AttrName);
			  printf (": ");
			  for (i = pAt1->AttrFirstExcept; i <= pAt1->AttrLastExcept; i
			       ++)
			    {
			       wrNumExcept (pSc1->SsException[i - 1]);
			       if (i < pAt1->AttrLastExcept)
				  printf (", ");
			    }
			  printf (";\n");
		       }
		  }
		/* ecrit les exceptions des types d'elements */
		for (r = 1; r <= pSc1->SsNRules; r++)
		  {
		     pRe1 = &pSc1->SsRule[r - 1];
		     if (pRe1->SrFirstExcept > 0)
			/* ce type a des exceptions */
		       {
			  printf ("   ");
			  if (pRe1->SrConstruct == CsPairedElement)
			     if (pRe1->SrFirstOfPair)
				printf ("First ");
			     else
				printf ("Second ");
			  wrnom (pRe1->SrName);
			  printf (": ");
			  for (i = pRe1->SrFirstExcept; i <= pRe1->SrLastExcept; i++)
			    {
			       wrNumExcept (pSc1->SsException[i - 1]);
			       if (i < pRe1->SrLastExcept)
				  printf (", ");
			    }
			  printf (";\n");
		       }
		  }
		/* ecrit les exceptions des regles d'extension */
		for (r = 1; r <= pSc1->SsNExtensRules; r++)
		  {
		     pRe1 = &pSc1->SsExtensBlock->EbExtensRule[r - 1];
		     if (pRe1->SrFirstExcept > 0)
		       {
			  printf ("   EXTERN ");
			  wrnom (pRe1->SrName);
			  printf (": ");
			  for (i = pRe1->SrFirstExcept; i <= pRe1->SrLastExcept; i++)
			    {
			       wrNumExcept (pSc1->SsException[i - 1]);
			       if (i < pRe1->SrLastExcept)
				  printf (", ");
			    }
			  printf (";\n");
		       }
		  }
	     }
	/* ecrit en commentaire les natures importees */
	if (!optionh)
	  {
	     Prem = True;
	     for (r = PremRegle; r <= pSc1->SsNRules; r++)
		if (pSc1->SsRule[r - 1].SrConstruct == CsNatureSchema)
		  {
		     if (Prem)
		       {
			  printf ("{ External structures used: ");
			  Prem = False;
		       }
		     printf ("\n");
		     printf ("   ");
		     writenb (r);
		     putchar ('\t');
		     wrnomregle (r);
		  }
	     if (!Prem)
		printf (" }\n");
	  }
	else
	  {
	     Prem = True;
	     for (r = PremRegle; r <= pSc1->SsNRules; r++)
		if (pSc1->SsRule[r - 1].SrConstruct == CsNatureSchema)
		  {
		     if (Prem)
		       {
			  printf ("\n/* Imported natures */\n");
			  Prem = False;
		       }
		     Wdefine ();
		     wrnom (pSc1->SsName);
		     printf ("_EL_");
		     wrnomregle (r);
		     printf (" ");
		     writenb (r);
		     printf ("\n");
		  }
	  }
	/* ecrit en commentaire les numeros d'exception utilises, dans l'ordre */
	/* croissant */
	if (!optionh)
	   if (pSc1->SsNExceptions > 0)
	     {
		printf ("{ Exception numbers:\n");
		printf ("  ");
		min = 100;
		do
		  {
		     nb = THOT_MAXINT;
		     for (i = 1; i <= pSc1->SsNExceptions; i++)
			if (pSc1->SsException[i - 1] > min)
			   if (pSc1->SsException[i - 1] < nb)
			      nb = pSc1->SsException[i - 1];
		     if (nb < THOT_MAXINT)
			printf ("%3d ", nb);
		     min = nb;
		  }
		while (nb != THOT_MAXINT);
		printf ("}\n");
	     }
	if (!optionh)
	   printf ("END\n");
     }
   else
      /* optionl */
     {
	printf ("document\n\ntranslation\n\n");
	for (k = 0; k < pSchStr->SsNRules; k++)
	   printName (&(pSchStr->SsRule[k].SrName[0]));

	for (k = 0; k < pSchStr->SsNExtensRules; k++)
	   if (pSchStr->SsExtensBlock->EbExtensRule[k].SrName[0] != '\0')
	      printName (&(pSchStr->SsExtensBlock->EbExtensRule[k].SrName[0]));
	for (k = 0; k < pSchStr->SsNAttributes; k++)
	   printName (&(pSchStr->SsAttribute[k].AttrName[0]));

	for (k = 0; k < pSchStr->SsNAttributes; k++)
	  {
	     if (pSchStr->SsAttribute[k].AttrType == AtEnumAttr)
		for (l = 0; l < pSchStr->SsAttribute[k].AttrNEnumValues; l++)
		   printName (&(pSchStr->SsAttribute[k].AttrEnumValue[l][0]));
	  }
     }
   TtaSaveAppRegistry ();
   exit (0);

 Usage:
   fprintf (stderr, "usage : %s [-h] [-l] file> \n", exec);
   exit (1);
}
/* End Of Module Printstr */
