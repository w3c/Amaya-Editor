/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2005
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*----------------------------------------------------------------------
   printstr.c : Impression en clair sur la sortie standard du contenu 
   d'un fichier .STR contenant un schema de structure.  

   V. Quint
  ----------------------------------------------------------------------*/

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "registry.h"
#include "libmsg.h"
#include "strmsg.h"
#include "message.h"

#define THOT_EXPORT extern
#include "platform_tv.h"

static PtrSSchema   pSchStr;
static PtrSSchema   pSchStrExt;
static int          a;
static int          i, min, nb;
static Name         fn;
static int          r;
static int          FirstRule;
static ThotBool     First;
static ThotBool     optionh = False;
static ThotBool     optionl = False;
static int          CommentNum;
static int          STR;	/* Identification des messages Str */
PtrSSchema          pSc1;
PtrTtAttribute      pAt1;

#include "readstr_f.h"

extern char*      TtaGetEnvString (char*);
extern void         TtaInitializeAppRegistry (char*);
extern void         TtaSaveAppRegistry (void);

/*----------------------------------------------------------------------
   wrcar ecrit un caractere.                                       
  ----------------------------------------------------------------------*/
static void         wrcar (unsigned char ch)
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
static void         writenb (int n)
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
static void         wrnom (Name n)
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
static void         wrnomregle (int r)
{
   if (pSchStr->SsRule->SrElem[r - 1]->SrName[0] == '\0')
      /* Name vide, on ecrit son numero */
     {
	printf ("Ident");
	writenb (r);
     }
   else
      wrnom (pSchStr->SsRule->SrElem[r - 1]->SrName);
}

/*----------------------------------------------------------------------
   OpenComment debute un commentaire.                             
  ----------------------------------------------------------------------*/
static void         OpenComment ()
{
   if (CommentNum == 0)
      printf ("{ ");
   CommentNum++;
}

/*----------------------------------------------------------------------
   CloseComment finit un commentaire.                                
  ----------------------------------------------------------------------*/
static void         CloseComment ()
{
   CommentNum--;
   if (CommentNum == 0)
      printf (" }");
}

/*----------------------------------------------------------------------
   wrNULL ecrit NULL.                                              
  ----------------------------------------------------------------------*/
static void         wrNULL ()
{

   OpenComment ();
   printf ("NULL");
   CloseComment ();
}

/*----------------------------------------------------------------------
   wrnomreglext ecrit le nom de la regle de numero r. Cette regle  
   fait partie du schema de structure traite' si N est nul.
   Sinon elle fait partie du schema de nom N.              
  ----------------------------------------------------------------------*/
static void         wrnomreglext (int r, Name N)
{
   if (N[0] == '\0')
     {
	if (pSchStr->SsRule->SrElem[r - 1]->SrConstruct == CsPairedElement)
	   if (!pSchStr->SsRule->SrElem[r - 1]->SrFirstOfPair)
	      printf ("Second ");
	wrnomregle (r);
     }
   else
     {
	if (!ReadStructureSchema (N, pSchStrExt))
	   printf ("******");
	else
	  {
	     if (pSchStrExt->SsRule->SrElem[r - 1]->SrConstruct == CsPairedElement)
		if (!pSchStrExt->SsRule->SrElem[r - 1]->SrFirstOfPair)
		   printf ("Second ");
	     wrnom (pSchStrExt->SsRule->SrElem[r - 1]->SrName);
	  }
	printf ("(");
	wrnom (N);
	printf (")");
     }
}

/*----------------------------------------------------------------------
   wrNumExcept ecrit le numero d'exception Num.                    
  ----------------------------------------------------------------------*/
static void         wrNumExcept (int Num)
{
   switch (Num)
	 {
	    case ExcNoCut:
	       printf ("NoCut");
	       break;
	    case ExcCanCut:
	       printf ("CanCut");
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
	    case ExcEventAttr:
	       printf ("EventAttr");
	       break;
	    case ExcCssId:
	       printf ("CssId");
	       break;
	    case ExcCssClass:
	       printf ("CssClass");
	       break;
	    case ExcCssPseudoClass:
	       printf ("CssPseudoClass");
	       break;
	    case ExcCssBackground:
	       printf ("CssBackground");
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
	    case ExcSelectParent:
	       printf ("SelectParent");
	       break;
	    case ExcMoveResize:
	       printf ("MoveResize");
	       break;
	    case ExcNoSpellCheck:
	       printf ("NoSpellCheck");
	       break;
	    case ExcIsDraw:
	       printf ("IsDraw");
	       break;
	    case ExcIsMarker:
	       printf ("IsMarker");
	       break;
	    case ExcIsGhost:
	       printf ("IsGhost");
	       break;
	    case ExcIsImg:
	       printf ("IsImg");
	       break;
	    case ExcIsMap:
	       printf ("IsMap");
	       break;
	    case ExcNoShowBox:
	       printf ("NoShowBox");
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
	    case ExcNoPaginate:
	       printf ("NoPaginate");
	       break;
	    case ExcImportLine:
	       printf ("ImportLine");
	       break;
	    case ExcImportParagraph:
	       printf ("ImportParagraph");
	       break;
	    case ExcParagraphBreak:
	       printf ("ParagraphBreak");
	       break;
	    case ExcIsBreak:
	       printf ("IsBreak");
	       break;
      case ExcListItemBreak:
         printf ("ListItemBreak");
         break;
	    case ExcHighlightChildren:
	       printf ("HighlightChildren");
	       break;
	    case ExcExtendedSelection:
	       printf ("ExtendedSelection");
	       break;
	    case ExcClickableSurface:
	       printf ("ClickableSurface");
	       break;
	    case ExcReturnCreateNL:
	       printf ("ReturnCreateNL");
	       break;
	    case ExcReturnCreateWithin:
	       printf ("ReturnCreateWithin");
	       break;
	    case ExcIsTable:
	       printf ("IsTable");
	       break;
	    case ExcIsRow:
	       printf ("IsRow");
	       break;
	    case ExcIsColHead:
	       printf ("IsColHead");
	       break;
	    case ExcIsCell:
	       printf ("IsCell");
	       break;
	    case ExcIsCaption:
	       printf ("IsCaption");
	       break;
	    case ExcShadow:
	       printf ("Shadow");
	       break;
	    case ExcNewPercentWidth:
	       printf ("NewPercentWidth");
	       break;
	    case ExcColRef:
	       printf ("ColRef");
	       break;
	    case ExcColColRef:
	       printf ("ColColRef");
	       break;
	    case ExcColSpan:
	       printf ("ColSpan");
	       break;
	    case ExcRowSpan:
	       printf ("RowSpan");
	       break;

	    default:
	       printf ("%2d", Num);
	       break;
	 }
}

/*----------------------------------------------------------------------
   Wdefine ecrit #define.                                          
  ----------------------------------------------------------------------*/
static void         Wdefine ()
{
   printf ("#define ");
}

/*----------------------------------------------------------------------
   wrattr ecrit un attribut.                                       
  ----------------------------------------------------------------------*/
static void         wrattr (int a)
{
   int                 j;
   PtrTtAttribute      pAt1;

   pAt1 = pSchStr->SsAttribute->TtAttr[a - 1];
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
static void         wrext (int r)
{
   PtrSRule         pRe1;

   pRe1 = pSchStr->SsRule->SrElem[r - 1];
   if (pRe1->SrRefImportedDoc)
      if (pRe1->SrConstruct == CsReference)
	 printf (" extern");
}

/*----------------------------------------------------------------------
   welembase ecrit un element de base.                             
  ----------------------------------------------------------------------*/
static void         welembase ()
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
   r = tt_Picture + 1;
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

   Wdefine ();
   wrnom (pSc1->SsName);
   r = AnyType + 1;
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
static void         wrrule (int r, SRule * pRegleExtens)
{
   int                 i;
   PtrSSchema          pSc1;
   PtrSRule            pRe1;
   PtrTtAttribute      pAt1;

   pSc1 = pSchStr;
   if (pRegleExtens != NULL)
      pRe1 = pRegleExtens;
   else
      pRe1 = pSc1->SsRule->SrElem[r - 1];
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

	     if (pRe1->SrNLocalAttrs > 0 && pRe1->SrLocalAttr)
	       {
		 for (i = 0; i < pRe1->SrNLocalAttrs; i++)
		   wrattr (pRe1->SrLocalAttr->Num[i]);
	       }
	     return;
	  }
	printf (" ");
	OpenComment ();
	writenb (r);
	CloseComment ();
	putchar ('\t');
	if (pRe1->SrRecursive)
	  {
	     OpenComment ();
	     printf ("Recursive");
	     CloseComment ();
	  }
	if (pRegleExtens != NULL && pRe1->SrName[0] == '\0')
	   printf ("Root");
	else if (pRegleExtens != NULL)
	   wrnom (pRe1->SrName);
	else
	  {
	    if (pRe1->SrConstruct == CsDocument)
	      OpenComment ();
	    wrnomregle (r);
	  }
	if (pRe1->SrNLocalAttrs > 0 &&
	    pRe1->SrRequiredAttr && pRe1->SrLocalAttr)
	  {
	     printf (" (ATTR ");
	     for (i = 0; i < pRe1->SrNLocalAttrs; i++)
	       {
		  if (pRe1->SrRequiredAttr->Bln[i])
		     printf ("!");
		  wrattr (pRe1->SrLocalAttr->Num[i]);
		  if (i == pRe1->SrNLocalAttrs)
		     printf (")");
		  else
		     printf (";\n%c%c", '\t', '\t');
	       }
	  }
	if (pRegleExtens == NULL && pRe1->SrConstruct != CsDocument)
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
	         case CsAny:
		    printf ("ANY");
		    break;
		 case CsExtensionRule:
	         case CsDocument:
	         case CsEmpty:
		    break;
	      }
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
		  pAt1 = pSc1->SsAttribute->TtAttr[pRe1->SrDefAttr[i - 1] - 1];
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
	if (pRe1->SrConstruct == CsDocument)
	  {
	    CloseComment ();
	    printf ("\n");
	  }
	else
	  printf (";\n");
     }
}

/*----------------------------------------------------------------------
   
  ----------------------------------------------------------------------*/
void                printName (char *buffer)
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
int                 main (int argc, char **argv)
{
   PtrSRule            pRe1;
   char               *exec, c;
   int                 inputfile = False;
   int                 k, l;
   ThotBool            premattrlocal;

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
	CommentNum = 0;
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
	if (pSc1->SsNAttributes > 0 &&
	    pSc1->SsAttribute->TtAttr[0]->AttrGlobal)
	  {
	     if (!optionh)
		printf ("ATTR\n");
	     else
		printf ("\n/* Global attributes */\n");
	  }

	for (i = 1; i <= pSc1->SsNAttributes; i++)
	   if (pSc1->SsAttribute->TtAttr[i - 1]->AttrGlobal)
	     {
		if (!optionh)
		   printf ("   ");
		pSc1->SsAttribute->TtAttr[i - 1]->AttrGlobal = False;
		/* indique a wrattr qu'il faut */
		/* ecrire les valeurs de l'attribut */
		wrattr (i);
		pSc1->SsAttribute->TtAttr[i - 1]->AttrGlobal = True;
		if (!optionh)
		   printf (";\n");
	     }
	if (optionh)
	   /* ecrit les attributs locaux */
	  {
	     premattrlocal = True;
	     for (i = 1; i <= pSc1->SsNAttributes; i++)
		if (!pSc1->SsAttribute->TtAttr[i - 1]->AttrGlobal)
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
	if (pSc1->SsRule->SrElem[r - 1]->SrConstruct == CsConstant)
	  {
	   if (!optionh)
	     {
		printf ("CONST\n");
		while (pSc1->SsRule->SrElem[r - 1]->SrConstruct == CsConstant)
		  {
		     pRe1 = pSc1->SsRule->SrElem[r - 1];
		     printf (" ");
		     OpenComment ();
		     writenb (r);
		     CloseComment ();
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
		while (pSc1->SsRule->SrElem[r - 1]->SrConstruct == CsConstant)
		  {
		     wrrule (r, NULL);
		     r++;
		  }
	     }
	  }
	FirstRule = r;
	/* ecrit les elements structures */
	if (pSc1->SsNRules >= FirstRule)
	  {
	   if (optionh)
	      printf ("\n/* Elements */\n");
	   else
	      printf ("STRUCT\n");
	  }
	for (r = FirstRule; r <= pSc1->SsNRules; r++)
	  {
	     pRe1 = pSc1->SsRule->SrElem[r - 1];
	     /* saute les elements Extern et Included et les unites */
	     if (!pRe1->SrRefImportedDoc && !pRe1->SrUnitElem)
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
	/* ecrit les unites exportees */
	First = True;
	for (r = FirstRule; r <= pSc1->SsNRules; r++)
	   if (pSc1->SsRule->SrElem[r - 1]->SrUnitElem)
	     {
		if (First)
		  {
		     if (!optionh)
			printf ("UNITS\n");
		     else
			printf ("\n/* Units */\n");
		     First = False;
		  }
		wrrule (r, NULL);
	     }
	/* ecrit les types exportes */
	if (!optionh)
	   if (pSc1->SsExport)
	     {
		First = True;
		printf ("THOT_EXPORT\n");
		for (r = FirstRule; r <= pSc1->SsNRules; r++)
		  {
		     pRe1 = pSc1->SsRule->SrElem[r - 1];
		     if (pRe1->SrExportedElem)
		       {
			  if (!First)
			     printf (",\n");
			  First = False;
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
		     pAt1 = pSc1->SsAttribute->TtAttr[a - 1];
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
		     pRe1 = pSc1->SsRule->SrElem[r - 1];
		     if (pRe1->SrFirstExcept > 0)
			/* ce type a des exceptions */
		       {
			  printf ("   ");
			  if (pRe1->SrConstruct == CsPairedElement)
			    {
			     if (pRe1->SrFirstOfPair)
				printf ("First ");
			     else
				printf ("Second ");
			    }
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
	     First = True;
	     for (r = FirstRule; r <= pSc1->SsNRules; r++)
		if (pSc1->SsRule->SrElem[r - 1]->SrConstruct == CsNatureSchema)
		  {
		     if (First)
		       {
			  printf ("{ External structures used: ");
			  First = False;
		       }
		     printf ("\n");
		     printf ("   ");
		     writenb (r);
		     putchar ('\t');
		     wrnomregle (r);
		  }
	     if (!First)
		printf (" }\n");
	  }
	else
	  {
	     First = True;
	     for (r = FirstRule; r <= pSc1->SsNRules; r++)
		if (pSc1->SsRule->SrElem[r - 1]->SrConstruct == CsNatureSchema)
		  {
		     if (First)
		       {
			  printf ("\n/* Imported natures */\n");
			  First = False;
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
	   printName (&(pSchStr->SsRule->SrElem[k]->SrName[0]));

	for (k = 0; k < pSchStr->SsNExtensRules; k++)
	   if (pSchStr->SsExtensBlock->EbExtensRule[k].SrName[0] != '\0')
	      printName (&(pSchStr->SsExtensBlock->EbExtensRule[k].SrName[0]));
	for (k = 0; k < pSchStr->SsNAttributes; k++)
	   printName (&(pSchStr->SsAttribute->TtAttr[k]->AttrName[0]));

	for (k = 0; k < pSchStr->SsNAttributes; k++)
	  {
	     if (pSchStr->SsAttribute->TtAttr[k]->AttrType == AtEnumAttr)
		for (l = 0; l < pSchStr->SsAttribute->TtAttr[k]->AttrNEnumValues; l++)
		   printName (&(pSchStr->SsAttribute->TtAttr[k]->AttrEnumValue[l][0]));
	  }
     }
   TtaSaveAppRegistry ();
   exit (0);

 Usage:
   fprintf (stderr, "usage : %s [-h] [-l] file> \n", exec);
   exit (1);
}
/* End Of Module Printstr */
