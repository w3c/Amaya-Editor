/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */

/* Included headerfiles */
#define EXPORT extern
#include "amaya.h"
#include "trans.h"
#include "html2thot_f.h"
#include "transparse_f.h"
 
/* variables du pattern matching */

/*pile */
static struct _stack
  {
     Tagnode            *Node;
     ListSymb           *Val;
  }
pile[MAXSTACK];
static int          maxMatchDepth;

static int          taillepile;

/* variables de generation du HTML */
/*pile */
typedef struct _NodeStack
  {
     char               *Tag;
     AttrDesc           *Attributes;
     int                 idf;
     int                 nbc;
  }
NodeStack;

static NodeStack   *pilenode[MAXSTACK];

static int          toppilenode;

static NodeDesc    *LastNode;
static char        *bufHTML;
static int          szHTML;
static int          LastRulePlace;
static int          cptidf;
static strmatch    *TabMenuTrans[20];


static boolean      ResultTrans;

/* variables memoire de la selection */
static int          ffc, flc, lfc, llc;
static int          maxSelDepth;
static Element      MyFirstSelect, OrigFirstSelect;
static Element      MyLastSelect, OrigLastSelect;
static Element      MySelect;
static boolean      isClosed;


/* fonctions du parser HTML && du parser trans utilisees ici */

#ifdef __STDC__
void                TransCallbackDialog (int ref, int typedata, char *data);

#else
void                TransCallbackDialog ( /* int ref, int typedata, char *data */ );

#endif
/*----------------------------------------------------------------------
   liberation d'un arbre de tags. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         freeTagTree (TagTree t)
#else
static void         freeTagTree (t)
TagTree             t;

#endif
{
   strmatch           *m, *m2;
   strmatchchildren   *mc, *mc2;
   Tagnode            *c, *n;

   if (t != NULL)
     {
	m = t->matches;
	while (m != NULL)
	  {
	     mc = m->childmatches;
	     while (mc != NULL)
	       {
		  mc2 = mc->next;
		  TtaFreeMemory ((char *) mc);
		  mc = mc2;
	       }
	     m2 = m->next;
	     TtaFreeMemory ((char *) m);
	     m = m2;
	  }

	c = t->child;
	while (c != NULL)
	  {
	     n = c->next;
	     freeTagTree (c);
	     c = n;
	  }
	TtaFreeMemory (t->tag);
	TtaFreeMemory ((char *) t);
     }
}


/*----------------------------------------------------------------------
   liberation de l'environnement de matching. 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         FreeMatchEnv ()
#else
static void         FreeMatchEnv ()
#endif
{
   ListElem           *l, *l1;

   /* liberation de la liste des sous-arbres transferes */
   l = match_env.listSubTrees;
   match_env.listSubTrees = NULL;
   while (l != NULL)
     {
	l1 = l;
	l = l->next;
	TtaFreeMemory ((char *) l1);
     }
   match_env.listSubTrees = NULL;

   /* liberation de l'arbre des tags */
   freeTagTree (match_env.subjecttree);
   match_env.subjecttree = NULL;
}

/*----------------------------------------------------------------------
   initialisation de l'application 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                InitTransform ()
#else
void                InitTransform ()
#endif
{
   TRANSDIAL = TtaGetMessageTable ("transdialogue", TRANS_MSG_MAX);
   TransBaseDialog = TtaSetCallback (TransCallbackDialog, MAX_TRANS_DLG);
   timeLastWrite = (time_t) 0;
}

/*----------------------------------------------------------------------
   construction et affichage des TagTrees     
  ----------------------------------------------------------------------*/
/* allocation d'un noeud */
#ifdef __STDC__
static TagTree      newNode (char *tag)
#else
static TagTree      newNode (tag)
char               *tag;

#endif
{
   TagTree             res;

   res = (TagTree) TtaGetMemory (sizeof (Tagnode));
   res->tag = (char *) TtaGetMemory (NAME_LENGTH);
   strcpy (res->tag, tag);
   res->matches = NULL;
   res->inter = NULL;
   res->element = NULL;
   res->transsymb = NULL;
   res->parent = NULL;
   res->child = NULL;
   res->next = NULL;
   res->prev = NULL;
   res->isTrans = FALSE;
   res->depth = 0;
   return res;
}


/*----------------------------------------------------------------------
   arbre thot -> arbre tag 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         BuildTagTree (Element elem, Document doc, TagTree father, int maxdepth, int depth)
#else
static void         BuildTagTree (elem, doc, father,, maxdepth, depth)
Element             elem;
Document            doc;
TagTree             father;
int                 maxdepth;
int                 depth;

#endif
{
   Element             elemcour;
   Attribute           attr;
   ElementType         eltype;
   char               *tag;
   TagTree             new, child;

   if (depth > maxdepth)
      return;
   tag = TtaGetMemory (NAME_LENGTH);
   eltype = TtaGetElementType (elem);
   strcpy (tag, GITagNameByType (eltype.ElTypeNum));
   attr = NULL;
   TtaNextAttribute (elem, &attr);
   if (strcmp (tag, "???") && (TtaGetFirstChild (elem) != NULL || attr != NULL || TtaIsLeaf (eltype)))
     {
	new = newNode (tag);
	new->element = elem;
	new->parent = father;
	new->depth = depth;
	if (father->child == NULL)
	  {
	     father->child = new;
	     new->prev = NULL;
	  }
	else
	  {
	     child = father->child;
	     while (child->next != NULL)
		child = child->next;
	     child->next = new;
	     new->prev = child;
	  }
	depth++;
     }
   else
      new = father;
   TtaFreeMemory (tag);
   if (eltype.ElTypeNum != HTML_EL_Comment_ && eltype.ElTypeNum != HTML_EL_Invalid_element)
     {
	elemcour = TtaGetFirstChild (elem);
	while (elemcour != NULL)
	  {
	     BuildTagTree (elemcour, doc, new, maxdepth, depth);
	     TtaNextSibling (&elemcour);
	  }
     }
}

/*----------------------------------------------------------------------
   fonctions de pattern matching 
  ----------------------------------------------------------------------*/


/* construit la liste res intersection dedes listes l1 et l2 : */
#ifdef __STDC__
static void         intersectmatch (ListSymb ** res, ListSymb * LS, strmatch * MS)
#else
static void         intersectmatch (res, LS, MS)
ListSymb          **res;
ListSymb           *LS;
strmatch           *MS;

#endif
{
   ListSymb           *pLS, *pres;
   strmatch           *pMS;
   boolean             found;

   if (*res != NULL)
     {
	freelist (*res);
	*res = NULL;
     }
   pres = NULL;
   pLS = LS;
   while (pLS != NULL)
     {
	pMS = MS;
	found = FALSE;
	while (!found && pMS != NULL)
	  {
	     found = (pMS->patsymb == pLS->symb);
	     pMS = pMS->next;
	  }
	if (found)
	  {
	     if (pres == NULL)
	       {
		  *res = (ListSymb *) TtaGetMemory (sizeof (ListSymb));
		  pres = *res;
	       }
	     else
	       {
		  pres->next = (ListSymb *) TtaGetMemory (sizeof (ListSymb));
		  pres = pres->next;
	       }
	     pres->next = NULL;
	     pres->symb = pLS->symb;
	  }
	pLS = pLS->next;
     }
}




/*----------------------------------------------------------------------
   pile de noeuds  pour l'algo de pattern matching (children_match) 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                init_pile (void)
#else
void                init_pile ()
#endif
{
   int                 i;

   for (i = 0; i < MAXSTACK; i++)
     {
	pile[i].Node = NULL;
	pile[i].Val = NULL;
     }
   taillepile = 0;
}

#ifdef __STDC__
void                empile (Tagnode * t, ListSymb * v)
#else
void                empile (t, v)
Tagnode            *t;
ListSymb           *v;

#endif
{
   if (taillepile < MAXSTACK)
     {
	pile[taillepile].Node = t;
	pile[taillepile++].Val = v;
     }
   else
     {
	printf ("Stack overflow \n");
     }
}



/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                depile (Tagnode ** t, ListSymb ** v)
#else
void                depile (t, v)
Tagnode           **t;
ListSymb          **v;

#endif
{
   if (taillepile > 0)
     {
	*t = pile[--taillepile].Node;
	*v = pile[taillepile].Val;
	pile[taillepile].Node = NULL;
	pile[taillepile].Val = NULL;
     }
   else
     {
	*t = NULL;
	*v = NULL;
     }
}

/*----------------------------------------------------------------------
   constlistmatch(node,symb) ajoute symb a la liste des matches du noeud  node    
   la liste des fils est mise a jour avec le contenu de la pile 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         constlistmatch (Tagnode * node, SymbDesc * symb)
#else
static void         constlistmatch (node, symb)
Tagnode            *node;
SymbDesc           *symb;

#endif
{
   strmatch           *sm;
   strmatchchildren   *smc;
   Tagnode            *n;
   ListSymb           *dl;

   sm = node->matches;
   if (sm == NULL)
     {
	node->matches = (strmatch *) TtaGetMemory (sizeof (strmatch));
	sm = node->matches;
     }
   else
     {
	while (sm->next != NULL && sm->patsymb != symb)
	   sm = sm->next;
	if (sm->patsymb != symb)
	  {
	     sm->next = (strmatch *) TtaGetMemory (sizeof (strmatch));
	     sm = sm->next;
	  }
	else
	   return;
     }
   sm->patsymb = symb;
   sm->node = node;
   sm->next = NULL;
   sm->childmatches = NULL;
   depile (&n, &dl);
   while (n != NULL)
     {
	smc = (strmatchchildren *) TtaGetMemory (sizeof (strmatchchildren));
	smc->next = sm->childmatches;
	sm->childmatches = smc;
	smc->patsymb = dl->symb;
	smc->node = n;
	depile (&n, &dl);
     }
}

/*----------------------------------------------------------------------
   children_match retourne TRUE si les fils du noeud n sont compatibles avec ceux de p 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      children_match (Tagnode * n, SymbDesc * p)
#else
static boolean      children_match (n, p)
Tagnode            *n;
SymbDesc           *p;

#endif
{
   Tagnode            *child;
   ListSymb           *candidat, *ms;
   boolean             matchfound, matchfailed;

   matchfound = FALSE;
   matchfailed = FALSE;

/*   candidat = match_env.TabChild[p][0]; */
   candidat = p->Children;
   if (candidat == NULL)
      /* s'il n'y a pas de fils possible pour n dans la pattern, on retourne vrai */
     {
	constlistmatch (n, p);
	return TRUE;
     }
   if (n->child == NULL)
      /* s'il y a un fils possible dans la pattern et n est une feuille */
     {
	if (p->OptChild)
	  {
	     constlistmatch (n, p);
	     return TRUE;
	  }
	else
	   return FALSE;
     }
   /* on cherche un match pour chacun des fils possibles de la pattern (candidat) */
   /* child est initialise avec le premier fils de n */
   child = n->child;
   intersectmatch (&(child->inter), p->Children, child->matches);
   candidat = child->inter;
   if (candidat == NULL)
     {
	return FALSE;
     }
   init_pile ();
   while (!matchfailed && !matchfound)
     {
	if (child == NULL && (candidat != NULL && candidat->symb == NULL))
	  {
	     /* tous les fils matchent, il faut depiler en gardant les relations */
	     /* on construit la liste des symboles matche's par les fils sur le noeud pere */
	     constlistmatch (n, p);
	     matchfound = TRUE;
	  }
	else if (child == NULL || candidat == NULL || candidat->symb == NULL)
	  {			/* && candidat !=0 */
	     /* on est sur une mauvaise branche */
	     /* on depile tant qu'on a pas une autre alternative */
	     depile (&child, &ms);
	     while (child != NULL &&
		    ms->next == NULL)
		depile (&child, &ms);
	     if (child == NULL)
	       {
		  matchfailed = TRUE;
		  /* aucun matching n'a ete trouve (fond de la pile) */
		  /*on sort de la boucle avec matchfailed */
	       }
	     else
	       {		/* un autre candidat n'a pas encore pu etre essaye */
		  candidat = ms->next;
	       }
	  }
	else
	  {			/* inv :  child!=NULL && candidat != -1 */
	     /* il faut comparer le noeud suivant avec les successeurs possibles */
	     /* du candidat  */

	     /* on empile le noeud, et le candidat */

	     empile (child, candidat);
	     /* s'il existe un fils suivant */
	     if (child->next != NULL)
	       {
		  /* on determine l'ensemble des candidats du suivant */
		  intersectmatch (&(child->next->inter),
				  candidat->symb->Nexts,
				  child->next->matches);
		  /* on commence par le premier */
		  candidat = child->next->inter;
	       }
	     else
	       {		/* inv : child->next==NULL */
		  /* candidat = (rankintabdesc(match_env.TabNext[candidat],0,MaxWidthPat)!=-1?0:-1); */
		  ms = candidat->symb->Nexts;
		  while (ms != NULL && ms->symb != NULL)
		     ms = ms->next;
		  if (ms != NULL)
		     candidat = ms;
	       }
	     child = child->next;
	  }
     }
   /* match_env.TabChild[p][0] est nul si les fils du symbole p sont optionnels */
   if (!matchfound && p->OptChild)
     {
	constlistmatch (n, p);
	matchfound = TRUE;
     }
   return matchfound;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      matchattr (SymbDesc * pSymb, Element elem)
#else
static boolean      matchattr (pSymb, elem)
SymbDesc           *pSymb;
Element             elem;

#endif
{
   boolean             result;
   AttrDesc           *pAttr;
   AttributeType       AttrTyp;
   Attribute           attr;
   char               *buf;
   int                 AttrKind, length;

   buf = TtaGetMemory (MAX_LENGTH);
   AttrTyp.AttrSSchema = TtaGetElementType (elem).ElSSchema;
   result = TRUE;
   pAttr = pSymb->Attributes;
   while (pAttr != NULL && result)
     {
	AttrTyp.AttrTypeNum = pAttr->ThotAttr;
	if (AttrTyp.AttrTypeNum != 0)
	  {
	     attr = TtaGetAttribute (elem, AttrTyp);
	     result = (attr != (Attribute) NULL);
	  }
	if (result)
	  {
	     TtaGiveAttributeType (attr, &AttrTyp, &AttrKind);
	     if (AttrKind == 2 && !pAttr->IsInt)
	       {
		  length = MAX_LENGTH;
		  TtaGiveTextAttributeValue (attr, buf, &length);
		  result = !strcmp (pAttr->TextVal, buf);
	       }
	     else if (AttrKind != 2 && pAttr->IsInt)
	       {
		  result = (TtaGetAttributeValue (attr) == pAttr->IntVal);
	       }
	     else
		result = FALSE;
	  }
	pAttr = pAttr->next;
     }
   TtaFreeMemory (buf);
   return result;
}


/*----------------------------------------------------------------------
   matchnode : construit la liste des pointeurs sur la pattern correspondant aux 
   matching possibles du noeud. Cette liste est calculee en fonction des                
   du noeud et du constructeur dans la pattern                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      matchnode (Tagnode * n)
#else
static boolean      matchnode (n)
Tagnode            *n;

#endif
{
   TransDesc          *td;
   SymbDesc           *sd;

   td = match_env.Transformations;
   while (td != NULL)
     {
	if (td->ActiveTrans)
	  {
	     sd = td->PatSymbs;
	     /* pour chaque occurence du tag dans la pattern, ou pour une etoile */
	     /* on regarde si les fils de n sont compatibles avec */
	     /* les tags des fils possibles de la pattern  */
	     while (sd != NULL)
	       {
		  if (sd->ActiveSymb)
		     if (n->depth - sd->depth <= maxSelDepth && n->depth - sd->depth >= 0)
			if (!strcmp (sd->Tag, "*") || !strcmp (sd->Tag, n->tag))
			   if (sd->Attributes == NULL || matchattr (sd, n->element))
			      children_match (n, sd);
		  sd = sd->next;
	       }
	     children_match (n, td->rootdesc);
	  }
	td = td->next;
     }
   return TRUE;
}

/*----------------------------------------------------------------------
   parcours postfixe de l'arbre, applicant la fonction booleenne f a tous les   
   noeuds de l'arbre t                                                  
   cond ition d'arret sur un noeud n : f(n) = FALSE                             
   retourne le noeud sur lequel le parcours s'est arrete, NULL si l'arbre entier a      
   ete parcouru                                                         
  ----------------------------------------------------------------------*/
static Tagnode     *ppostfix (TagTree t, boolean (*f) (Tagnode *));

#ifdef __STDC__
static Tagnode     *parcours_postfix (TagTree t, boolean (*f) (Tagnode *))
#else
static Tagnode     *parcours_postfix (t, f)
TagTree             t;

boolean (*f) (Tagnode *);
#endif
{
   Tagnode            *res;

   res = ppostfix (t, f);
   if (res == NULL)
     {
	if (f (t))
	   return NULL;
	else
	   return t;
     }
   else
      return res;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Tagnode     *ppostfix (TagTree t, boolean (*f) (Tagnode *))
#else
static Tagnode     *ppostfix (t, f)
TagTree             t;

boolean (*f) (Tagnode *);
#endif
{
   Tagnode            *n, *res;

   n = t->child;
   while (n != NULL)
     {
	res = ppostfix (n, f);
	if (res == NULL)
	  {
	     if (f (n))
		n = n->next;
	     else
		return n;
	  }
	else
	   return res;
     }
   return NULL;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         AddListSubTree (Element elem, int idf, int rank)
#else
static void         AddListSubTree (elem, idf, rank)
Element             elem;
int                 idf;
int                 rank;

#endif
{
   ListElem           *pcour, *pprec;

   pcour = match_env.listSubTrees;
   pprec = NULL;
   while (pcour != NULL && (pcour->id < idf || (pcour->id == idf && pcour->rank < rank)))
     {
	pprec = pcour;
	pcour = pcour->next;
     }
   if (pprec != NULL)
     {
	pprec->next = (ListElem *) TtaGetMemory (sizeof (ListElem));
	pprec = pprec->next;
     }
   else
     {
	match_env.listSubTrees = (ListElem *) TtaGetMemory (sizeof (ListElem));
	pprec = match_env.listSubTrees;
     }
   pprec->id = idf;
   pprec->rank = rank;
   pprec->element = elem;
   pprec->next = pcour;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element      FindListSTreeByLabel (char *label)
#else
static Element      FindListSTreeByLabel (label)
char               *label;

#endif
{
   ListElem           *pcour;
   boolean             found;

   found = FALSE;
   pcour = match_env.listSubTrees;
   while (!found && pcour != NULL)
     {
	found = !strcmp (TtaGetElementLabel (pcour->element), label);
	if (!found)
	   pcour = pcour->next;
     }
   if (!found)
      return NULL;
   else
      return pcour->element;
}

/*----------------------------------------------------------------------
   search for the next element with the identifier id in the listSubTrees, searches the first one if 
   *elem =NULL 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          FindListSubTree (int id, Element * elem)
#else
static int          FindListSubTree (id, elem)
int                 id;
Element            *elem;

#endif
{
   ListElem           *pcour;
   boolean             found;
   int                 result;

   found = FALSE;
   pcour = match_env.listSubTrees;
   while (!found && pcour != NULL)
     {
	found = (pcour->id == id && (*elem == NULL || *elem == pcour->element));
	if (!found)
	   pcour = pcour->next;
     }
   if (found && *elem == NULL)
     {
	*elem = pcour->element;
	result = pcour->rank;
     }
   else
     {
	*elem = NULL;
	result = 0;
	if (found)
	  {
	     pcour = pcour->next;
	     found = FALSE;
	     while (!found && pcour != NULL)
	       {
		  found = pcour->id == id;
		  if (!found)
		     pcour = pcour->next;
	       }
	     if (found)
	       {
		  *elem = pcour->element;
		  result = pcour->rank;
	       }
	  }
     }
   return result;
}

/*----------------------------------------------------------------------
   MyStartParser initializes  parsing environement in order to parse the HTML      
   fragment in buffer in the context of a last descendance of the element ElFather 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      MyStartParser (strmatchchildren * sMatch, Document doc)
#else
static boolean      MyStartParser (sMatch, doc)
strmatch           *sMatch;
Document            doc;

#endif
{
   strmatchchildren   *PrevMatch, *DMatch;
   Element             lastEl, courEl, invEl;
   ElementType         typeEl;
   boolean             res;


   PrevMatch = NULL;
   DMatch = sMatch;
   MyFirstSelect = DMatch->node->element;
   TtaPreviousSibling (&MyFirstSelect);
   isClosed = TRUE;
   res = TRUE;
   if (MyFirstSelect == NULL)
     {
	MyFirstSelect = DMatch->node->parent->element;
	isClosed = FALSE;
     }
   while (DMatch != NULL)
     {
	PrevMatch = DMatch;
	DMatch = DMatch->next;
	if (DMatch != NULL)
	   TtaRemoveTree (PrevMatch->node->element, doc);
     }
   if (PrevMatch != NULL)
     {
	MyLastSelect = PrevMatch->node->element;
	TtaNextSibling (&MyLastSelect);
	TtaRemoveTree (PrevMatch->node->element, doc);
     }
   if (strcmp (bufHTML, ""))
     {
	TtaSetStructureChecking (0, doc);
	InitializeParser (MyFirstSelect, isClosed, doc);
	HTMLparse (NULL, bufHTML);
	TtaSetStructureChecking (1, doc);
	typeEl = TtaGetElementType (MyFirstSelect);
	typeEl.ElTypeNum = HTML_EL_Invalid_element;
	invEl = NULL;
	if (isClosed)
	  {
	     courEl = MyFirstSelect;
	     TtaNextSibling (&courEl);
	     invEl = TtaSearchTypedElement (typeEl, SearchForward, courEl);
	     if (invEl != NULL)
	       {
		  if ((!MyLastSelect && TtaIsAncestor (invEl, TtaGetParent (MyFirstSelect)))
		   || ((MyLastSelect && TtaIsBefore (invEl, MyLastSelect))))
		    {		/* if the transformation failed */
		       res = FALSE;
		       while (courEl != NULL && courEl != MyLastSelect)
			 {	/* deleting the generated structure */
			    lastEl = courEl;
			    TtaNextSibling (&courEl);
			    TtaDeleteTree (lastEl, doc);
			 }
		       DMatch = sMatch;
		       lastEl = MyFirstSelect;
		       while (DMatch != NULL)
			 {	/* restoring the source elements */
			    TtaInsertSibling (DMatch->node->element, lastEl, FALSE, doc);
			    lastEl = DMatch->node->element;
			    DMatch = DMatch->next;
			 }
		    }
	       }
	  }
	else
	  {
	     invEl = TtaSearchTypedElement (typeEl, SearchInTree, MyFirstSelect);
	     if (invEl != NULL)
	       {
		  if (MyLastSelect == NULL || TtaIsBefore (invEl, MyLastSelect))
		    {		/* if the transformation failed */
		       res = FALSE;
		       /* deleting the generated structure */
		       courEl = TtaGetFirstChild (MyFirstSelect);
		       while (courEl != NULL && courEl != MyLastSelect)
			 {
			    TtaDeleteTree (courEl, doc);
			    courEl = TtaGetFirstChild (lastEl);
			 }
		       /* restoring the source elements */
		       DMatch = sMatch;
		       TtaInsertFirstChild (&(DMatch->node->element), MyFirstSelect, doc);
		       lastEl = DMatch->node->element;
		       DMatch = DMatch->next;
		       while (DMatch != NULL)
			 {
			    TtaInsertSibling (DMatch->node->element, lastEl, FALSE, doc);
			    lastEl = DMatch->node->element;
			    DMatch = DMatch->next;
			 }
		    }
	       }
	  }
     }
   return res;
}

/*----------------------------------------------------------------------
   fonctions de conversion automatique des sous-arbres pendants 
   ApplatitEtInsereElement : tente d'applatir le sous-arbre a transferer 
   el source : racine du sous arbre 
   elDest : voisin precendent ou pere dans la destination 
   voisin : si vrai : elDest est le prcedent sinon c;est le pere 
   Last created : retourne le dernier element cree 
   nbcreated : retourne le nombre d'element crees au plus haut niveau 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      ApplatitEtInsereElement (Element elSource, Element elDest, boolean voisin, Document doc, Element * LastCreated, int *nbcreated)
#else
static boolean      ApplatitEtInsereElement (elSource, elDest, voisin, doc, LastCreated, nbcreated)
Element             elSource;
Element             elDest;
boolean             voisin;
Document            doc;
Element            *LastCreated;
int                *nbcreated;

#endif
{
   Element             elCour, elPreced, elNext;
   ElementType         eltype;
   int                 nbc;
   boolean             res;

   *nbcreated = 0;
   if (TtaIsLeaf (TtaGetElementType (elSource)))
     {				/* if the source element is a leaf, it is impossible to flatten it more */
	LastCreated = NULL;

	return FALSE;
     }
   res = TRUE;
   elCour = TtaGetFirstChild (elSource);
   elPreced = NULL;
   if (voisin)
      elPreced = elDest;
   while (elCour != NULL && res)
     {				/* for each child of the source element */
	elNext = elCour;
	TtaNextSibling (&elNext);
	eltype = TtaGetElementType (elCour);
	TtaRemoveTree (elCour, doc);
	if (elPreced != NULL)
	   TtaInsertSibling (elCour, elPreced, FALSE, doc);
	else
	   TtaInsertFirstChild (&elCour, elDest, doc);
	res = !(boolean) TtaGetErrorCode ();
	if (res)
	  {
	     elPreced = elCour;
	     *LastCreated = elCour;
	     (*nbcreated)++;
	  }
	else
	  {
	     if (elPreced != NULL)
		res = ApplatitEtInsereElement (elCour, elPreced, TRUE, doc, LastCreated, &nbc);
	     else
		res = ApplatitEtInsereElement (elCour, elDest, FALSE, doc, LastCreated, &nbc);
	     elPreced = *LastCreated;
	     (*nbcreated) += nbc;
	  }
	elCour = elNext;
     }
   if (!res && TtaIsConstant (TtaGetElementType (elSource)))
      /* we don't mind about failure when inserting constants */
      return TRUE;
   else
      return res;
}
/*----------------------------------------------------------------------
   RankedInsertion


   Inserts an element as a given child or sibling of an element, creating a descendance 
   between the two elements if necesary. 

   Parameters:
   newEl: the element to be inserted
   parent: the element in which newEl will be inserted
   prevEl : the element after which newEl will be inserted
   rank:  rank of newEl 
   doc: document in which newwEl have to be inserted
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      RankedInsertion (Element newEl, Element parent, Element prevEl, int *rank, Document doc)
#else
static boolean      RankedInsertion (newEl, parent, prevEl, *rank, doc)
Element             newEl;
Element             parent;
Element             prevEl;
int                 rank;
Document            doc;

#endif
{
   ElementType         eltypenew;
   Element             elpremcree, eldercree, elprevcree, elcour, elem;
   int                 i;
   boolean             result;


   if (prevEl == NULL && parent == NULL)
      return FALSE;
   result = TRUE;
   TtaSetErrorMessages (0);

   elcour = NULL;
   if (*rank > 1 || prevEl != NULL)
     {
	/* if the element have not to be inserted as the first child of parent */
	if (prevEl == NULL)
	  {
	     elcour = TtaGetFirstChild (parent);
	     i = 2;
	  }
	else
	  {
	     elcour = prevEl;
	     i = 1;
	  }
	/* search for the previous sibling of the inserted element */
	for (; i < *rank && elcour != NULL; i++)
	   TtaNextSibling (&elcour);
	/* if there is not enough children of parent, */
	/*insert element as the next sibling of the last child of parent */
	if (elcour == NULL && parent != NULL)
	   elcour = TtaGetLastChild (parent);
	else if (elcour == NULL)
	   elcour = TtaGetLastChild (TtaGetParent (prevEl));
     }
   *rank = 1;
   if (elcour != NULL)
     {
	/* the newEl has to be inserted as the next sibling of elcour */
	TtaInsertSibling (newEl, elcour, FALSE, doc);
     }
   else
      /* the newEl has to be inserted as the first child of parent */
      TtaInsertFirstChild (&newEl, parent, doc);

   if (TtaGetErrorCode ())
     {
	/* the newEl has not been inserted */
	/* create a descendance between the parent and the newEl */
	eltypenew = TtaGetElementType (newEl);
	if (parent == NULL)
	   parent = TtaGetParent (elcour);
	eldercree = TtaCreateDescent (doc, parent, eltypenew);
	if (!TtaGetErrorCode ())
	  {
	     /* la descendance a ete cree */
	     elpremcree = TtaGetFirstChild (parent);
	     while (elpremcree != NULL && !TtaIsAncestor (newEl, elpremcree))
	       {
		  TtaNextSibling (&elpremcree);
	       }
	     /* elpremcree est le fils de parent ancetre de newEl */
	     if (elpremcree != NULL)
	       {
		  TtaRemoveTree (elpremcree, doc);
		  if (elcour != NULL)
		     TtaInsertSibling (elpremcree, elcour, FALSE, doc);
		  else
		     TtaInsertFirstChild (&elpremcree, parent, doc);
		  if (TtaGetErrorCode ())
		    {
		       TtaDeleteTree (elpremcree, doc);
		       result = FALSE;
		    }
		  else
		    {
		       elprevcree = TtaGetParent (eldercree);
		       TtaDeleteTree (eldercree, doc);
		       TtaInsertFirstChild (&newEl, elprevcree, doc);
		       if (TtaGetErrorCode ())
			 {
			    TtaDeleteTree (elpremcree, doc);
			    result = FALSE;
			 }
		    }
	       }
	  }
	else
	   result = FALSE;
     }
   if (result == FALSE)
      /* impossible to create a descendance to the element */
      /* trying to flatten the element to be transferred */
      /* this is the worst case */
      if (elcour == NULL)
	 result = ApplatitEtInsereElement (newEl, parent, FALSE, doc, &elem, rank);
      else
	 result = ApplatitEtInsereElement (newEl, elcour, TRUE, doc, &elem, rank);
   return result;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CopySTreeChildren (Element father, Document doc)
#else
static void         CopySTreeChildren (father, doc)
Element             father;
Document            doc;

#endif
{
   Element             courEl, eloriginal;
   AttributeType       AtType;
   Attribute           atfound;
   char                label[10];
   int                 l, rank, idf, delta;

   AtType.AttrSSchema = TtaGetDocumentSSchema (doc);
   AtType.AttrTypeNum = HTML_ATTR_Ghost_restruct;
   courEl = TtaGetFirstChild (father);
   while (courEl != NULL)
     {
	CopySTreeChildren (courEl, doc);
	atfound = TtaGetAttribute (courEl, AtType);
	if (atfound != NULL)
	  {
	     l = 10;
	     TtaGiveTextAttributeValue (atfound, label, &l);
	     TtaRemoveAttribute (courEl, atfound, doc);
	     idf = atoi (label);
	     eloriginal = NULL;
	     rank = FindListSubTree (idf, &eloriginal);
	     delta = 0;
	     while (eloriginal != NULL)
	       {
		  TtaRemoveTree (eloriginal, doc);
		  RankedInsertion (eloriginal, courEl, NULL, &rank, doc);
		  delta += rank - 1;
		  rank = delta + FindListSubTree (idf, &eloriginal);
	       }
	  }
	TtaNextSibling (&courEl);
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         CopieSousArbres (Tagnode * Root, Document doc)
#else
static void         CopieSousArbres (Root, doc)
Tagnode            *Root;
Document            doc;

#endif
{
   char                label[10];
   int                 l, idf, rank, delta;
   AttributeType       AtType;
   Attribute           atfound;
   Element             courEl, lastEl, eloriginal;

   courEl = MyFirstSelect;	/* premier element genere */
   lastEl = MyLastSelect;	/* dernier elt genere */
   if (isClosed)
      TtaNextSibling (&courEl);
   else
      courEl = TtaGetFirstChild (MyFirstSelect);
   AtType.AttrSSchema = TtaGetDocumentSSchema (doc);
   AtType.AttrTypeNum = HTML_ATTR_Ghost_restruct;
   while (courEl != NULL && courEl != lastEl)
     {				/* pour chaque elt genere */
	CopySTreeChildren (courEl, doc);	/* on copie les sous arbres dans ses fils */
	atfound = TtaGetAttribute (courEl, AtType);	/* cherche l'attribut ZZGHOST */
	if (atfound != NULL)
	  {
	     l = 10;
	     TtaGiveTextAttributeValue (atfound, label, &l);
	     TtaRemoveAttribute (courEl, atfound, doc);
	     idf = atoi (label);
	     eloriginal = NULL;
	     /* recherche et insere les fils originaux */
	     rank = FindListSubTree (idf, &eloriginal);
	     delta = 0;
	     while (eloriginal != NULL)
	       {
		  TtaRemoveTree (eloriginal, doc);
		  RankedInsertion (eloriginal, courEl, NULL, &rank, doc);
		  delta += rank - 1;
		  rank = delta + FindListSubTree (idf, &eloriginal);
	       }
	  }
	TtaNextSibling (&courEl);
     }
   /* insert the subtrees as children of the root */
   eloriginal = NULL;
   courEl = MyFirstSelect;
   rank = FindListSubTree (0, &eloriginal);
   delta = 0;
   while (eloriginal != NULL)
     {
	TtaRemoveTree (eloriginal, doc);
	if (isClosed)
	   RankedInsertion (eloriginal, NULL, courEl, &rank, doc);
	else
	   RankedInsertion (eloriginal, courEl, NULL, &rank, doc);
	delta += rank - 1;
	rank = delta + FindListSubTree (0, &eloriginal);
     }
}

/*----------------------------------------------------------------------
   PutBeginTag,PutEndTag,TransfertNode 
   fill the HTML buffer with the generated nodes 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      PutInHTMLBuf (char *s)
#else
static boolean      PutInHTMLBuf (s)
char               *s;

#endif
{
   if ((szHTML += strlen (s)) > LGBUFFER)
     {
	fprintf (stderr, "increase LGBUFFER");
	return FALSE;
     }
   else
     {
	strcat (bufHTML, s);
	return TRUE;
     }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         PutBeginTag (NodeDesc * ND, Tagnode * TN)
#else
static void         PutBeginTag (ND, TN)
NodeDesc           *ND;
Tagnode            *TN;

#endif
{
   AttrDesc           *AD;
   NodeStack          *NS;
   char               *chaine, *tag;
   Tagnode            *ancestor;
   boolean             found;
   AttributeType       attrType;
   Attribute           attr;
   int                 l, atkind;

   attrType.AttrSSchema = TtaGetDocumentSSchema (TransDoc);
   chaine = TtaGetMemory (NAME_LENGTH);
   tag = TtaGetMemory (NAME_LENGTH);
   pilenode[toppilenode]->nbc++;
   NS = (NodeStack *) TtaGetMemory (sizeof (NodeStack));
   NS->Tag = TtaGetMemory (NAME_LENGTH);
   strcpy (NS->Tag, ND->Tag);
   NS->idf = cptidf++;
   NS->nbc = 0;
   NS->Attributes = (AttrDesc *) TtaGetMemory (sizeof (AttrDesc));
   NS->Attributes->NameAttr = TtaGetMemory (NAME_LENGTH);
   strcpy (NS->Attributes->NameAttr, "ZZGHOST");
   NS->Attributes->IsInt = TRUE;
   NS->Attributes->IsTransf = FALSE;
   NS->Attributes->IntVal = NS->idf;
   NS->Attributes->next = ND->Attributes;
   pilenode[++toppilenode] = NS;

   /* writing the tag name */
   PutInHTMLBuf ("<");
   PutInHTMLBuf (NS->Tag);

   AD = NS->Attributes;
   /* wrting the attributes */
   while (AD != NULL)
     {
	if (AD->IsTransf)
	  {			/* transfer attribute */
	     ancestor = TN;
	     found = FALSE;
	     while (!found && ancestor != NULL)
	       {		/* searching for source element (in current element ancestors) */
		  found = (!strcmp (ancestor->transsymb->SName, AD->AttrTag)
			|| !strcmp (ancestor->transsymb->Tag, AD->AttrTag));
		  if (!found)
		     ancestor = ancestor->parent;
	       }
	     if (found)
	       {		/* searching for an ancestor of the source element which have the wanted attribute  */
		  if (ancestor != NULL)
		    {
		       strcpy (tag, GITagNameByType ((int) (TtaGetElementType (ancestor->element).ElTypeNum)));
		       attrType.AttrTypeNum = MapThotAttr (AD->AttrAttr, tag);
		    }
		  attr = NULL;
		  found = FALSE;
		  while (!found && ancestor != NULL)
		    {
		       if (attrType.AttrTypeNum != -1)
			  attr = TtaGetAttribute (ancestor->element, attrType);
		       found = (attr != NULL);
		       if (!found)
			 {
			    ancestor = ancestor->parent;
			    if (ancestor != NULL)
			      {
				 strcpy (tag, GITagNameByType ((int) (TtaGetElementType (ancestor->element).ElTypeNum)));
				 attrType.AttrTypeNum = MapThotAttr (AD->AttrAttr, tag);
			      }
			 }
		    }
		  if (found)
		    {		/* the attribute has been found, writing the attribute name */
		       PutInHTMLBuf (" ");
		       PutInHTMLBuf (AD->AttrAttr);
		       PutInHTMLBuf ("=");
		       /* writing the attribute value */
		       TtaGiveAttributeType (attr, &attrType, &atkind);
		       if (atkind == 2)
			 {	/* text attribute */
			    l = TtaGetTextAttributeLength (attr);
			    TtaGiveTextAttributeValue (attr, chaine, &l);
			    PutInHTMLBuf (chaine);
			 }
		       else
			 {	/* int attribute */
			    sprintf (chaine, "%d", TtaGetAttributeValue (attr));
			    PutInHTMLBuf (chaine);
			 }
		    }
	       }
	     if (!found)
	       {
		  fprintf (stderr, "can't transfer attribute %s\n", AD->AttrAttr);
	       }
	  }
	else
	  {			/* creation of an attribute */
	     PutInHTMLBuf (" ");
	     PutInHTMLBuf (AD->NameAttr);
	     PutInHTMLBuf ("=");
	     if (AD->IsInt)
	       {		/* int attribute */
		  sprintf (chaine, "%d", AD->IntVal);
		  PutInHTMLBuf (chaine);
	       }
	     else
	       {		/* text attribute */
		  l = strlen (bufHTML);
		  bufHTML[l] = '"';
		  bufHTML[l + 1] = EOS;
		  szHTML++;
		  PutInHTMLBuf (AD->TextVal);
		  l = strlen (bufHTML);
		  bufHTML[l] = '"';
		  bufHTML[l + 1] = EOS;
		  szHTML++;
	       }
	  }
	AD = AD->next;
     }
   /* closing the tag */
   PutInHTMLBuf (">");
   /*free the ZZGHOST attribute */
   TtaFreeMemory ((char *) NS->Attributes->NameAttr);
   TtaFreeMemory ((char *) NS->Attributes);
   NS->Attributes = NULL;
   TtaFreeMemory (chaine);
}

#ifdef __STDC__
static void         PutEndTag (NodeStack * ND)
#else
static void         PutEndTag (ND)
NodStack           *ND;

#endif
{
   if (strcmp (ND->Tag, "HR") && strcmp (ND->Tag, "BR"))
     {
	PutInHTMLBuf ("</");
	PutInHTMLBuf (ND->Tag);
	PutInHTMLBuf (">");
     }
}

/*----------------------------------------------------------------------
   TransfertChildren : copies the children of node into the result instance 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         TransfertChildren (Tagnode * node)
#else
static void         TransfertChildren (Tagnode * node)
Tagnode            *node;

#endif
{
   Tagnode            *child;

   child = node->child;
   while (child != NULL)
     {
	if (TtaGetElementVolume (child->element) != 0)
	  {			/* if the element is empty: no transfert */
	     pilenode[toppilenode]->nbc++;
	     AddListSubTree (child->element,
			     pilenode[toppilenode]->idf,
			     pilenode[toppilenode]->nbc);
	  }
	child = child->next;
     }
}

/*----------------------------------------------------------------------
   TransfertNode : copies a node and its content into the result instance 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         TransfertNode (Tagnode * node, boolean inplace)
#else
static void         TransfertNode (node, inplace)
Tagnode            *node;
boolean             inplace;

#endif
{
   if (TtaGetElementVolume (node->element) != 0)
     {				/* if the element is empty: no transfert */
	if (!inplace)
	   /* on ferme les elements generes par le regle precedente */
	   while (toppilenode >= LastRulePlace)
	     {
		PutEndTag (pilenode[toppilenode]);
		TtaFreeMemory (pilenode[toppilenode]->Tag);
		TtaFreeMemory ((char *) pilenode[toppilenode]);
		toppilenode--;
	     }
	pilenode[toppilenode]->nbc++;
	AddListSubTree (node->element,
			pilenode[toppilenode]->idf,
			pilenode[toppilenode]->nbc);
     }
}


/*----------------------------------------------------------------------
   Fonctions de transformation par regles            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         TransformNode (strmatchchildren * sm);

#else
static void         TransformNode (sm);

#endif


#ifdef __STDC__
static void         ApplyTransChild (strmatchchildren * smc)
#else
static void         ApplyTransChild (smc)
strmatchchildren   *smc;

#endif
{
   strmatchchildren   *smc2;
   strmatch           *sm;
   boolean             found;

   smc2 = smc;
   while (smc2 != NULL)
     {
	smc2->node->isTrans = TRUE;
	if (smc2->patsymb->Rule == NULL)
	  {
	     /* pas de regle de transformation pour ce noeud */
	     /* on cherche si l'un de ses descendants a ete matche */
	     sm = smc2->node->matches;
	     found = FALSE;
	     while (!found && sm != NULL)
	       {
		  found = (sm->patsymb == smc2->patsymb);
		  if (!found)
		     sm = sm->next;
	       }
	     if (found)
	       {
		  /* il existe un matching pour au moins un fils du noeud courant */
		  ApplyTransChild (sm->childmatches);
	       }
	     else
	       {		/* il n'y a pas de matching pour les fils de ce noeud */
		  TransfertNode (sm->node, FALSE);
	       }
	  }
	else
	  {			/* il existe une regle de transformation pour ce noeud */
	     /* on applique la regle de transformation */
	     TransformNode (smc2);
	  }
	smc2 = smc2->next;
     }
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         TransformNode (strmatchchildren * sm)
#else
static void         TransformNode (sm)
strmatchchildren   *sm;

#endif
{
   int                 cournode;
   strmatch           *sm2;
   NodeDesc           *RNodeCour;
   boolean             stop, sonsmatch;

   sm2 = sm->node->matches;
   sonsmatch = FALSE;
   while (sm2 != NULL && !sonsmatch)
     {
	sonsmatch = (sm2->patsymb == sm->patsymb && sm2->childmatches != NULL);
	if (!sonsmatch)
	   sm2 = sm2->next;
     }
   /* sonsmatch est vrai s'il existe un matching pour au moins un fils du noeud Source */
   sm->node->transsymb = sm->patsymb;
   cournode = 1;
   RNodeCour = sm->patsymb->Rule->OptNodes;
   stop = (RNodeCour == NULL || cournode > toppilenode);
   while (!stop)
     {				/* pour chaque noeud optionnel */
	if (!strcmp (pilenode[cournode]->Tag, RNodeCour->Tag))
	  {			/* le tag est deja present : on ne genere rien */
	     RNodeCour = RNodeCour->next;
	     cournode++;
	     stop = (RNodeCour == NULL || cournode > toppilenode);
	  }
	else
	  {
	     /* il faut creer une nouvelle branche dans la destination */
	     stop = TRUE;
	  }
     }

   while (toppilenode >= cournode)
     {				/* on ferme les tags de la pile (crees par la regle precedente) */
	PutEndTag (pilenode[toppilenode]);
	TtaFreeMemory (pilenode[toppilenode]->Tag);
	TtaFreeMemory ((char *) pilenode[toppilenode]);
	toppilenode--;
     }

   while (RNodeCour != NULL)
     {				/* on genere des noeuds optionnels non deja presents */
	PutBeginTag (RNodeCour, sm->node);
/*       pilenode[++toppilenode]=RNodeCour; */
	cournode++;
	RNodeCour = RNodeCour->next;
     }

   LastRulePlace = cournode;
   RNodeCour = sm->patsymb->Rule->NewNodes;

   while (RNodeCour != NULL && strcmp (RNodeCour->Tag, "*"))
     {				/* on genere les nouveaux noeuds */
	PutBeginTag (RNodeCour, sm->node);
/*       pilenode[++toppilenode]=RNodeCour; */
	cournode++;
	RNodeCour = RNodeCour->next;
     }
   if (RNodeCour != NULL && !strcmp (RNodeCour->Tag, "*"))
     {
	TransfertNode (sm->node, TRUE);
     }
   /* on traite les fils du noeud */
   else if (sonsmatch)
     {				/* si les fils ont ete matches */
	ApplyTransChild (sm2->childmatches);
     }
   else
     {				/*Les fils ne matchent pas : on les transfere */
	TransfertChildren (sm->node);
     }
}

/*----------------------------------------------------------------------
   parcours de l'arbre source upside down en appliquant la transformation d'indice t 
   dans TabPatterns 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      ApplyTransformation (strmatch * sm, Document doc)
#else
static boolean      ApplyTransformation (sm, doc)
strmatch           *sm;
Document            doc;

#endif
{
   NodeStack          *ND;
   strmatchchildren   *DMatch;
   boolean             res;

   res = FALSE;
   cptidf = 0;
   ND = (NodeStack *) TtaGetMemory (sizeof (NodeStack));
   ND->Tag = TtaGetMemory (NAME_LENGTH);
   ND->Attributes = NULL;
   ND->idf = cptidf++;
   ND->nbc = 0;

   if (sm->childmatches != NULL)
     {
	/* initialize the transformation stack */
	DMatch = sm->childmatches;
	LastNode = NULL;
	toppilenode = 0;
	strcpy (ND->Tag, DMatch->node->parent->tag);
	pilenode[0] = ND;
	LastRulePlace = 1;
	szHTML = 0;
	bufHTML = TtaGetMemory (LGBUFFER);
	strcpy (bufHTML, "");
	/* applying the transformation */
	ApplyTransChild (sm->childmatches);
	while (toppilenode > 0)
	  {
	     PutEndTag (pilenode[toppilenode]);
	     TtaFreeMemory (pilenode[toppilenode]->Tag);
	     TtaFreeMemory ((char *) pilenode[toppilenode]);
	     toppilenode--;
	  }
	/* parsing the produced structure */
	res = MyStartParser (sm->childmatches, doc);

	TtaFreeMemory (bufHTML);
     }
   if (res)
     {				/* if the parsing was succesful */
	/* transfer the unchanged subtrees */
	CopieSousArbres (match_env.subjecttree, doc);
	/* deleting the source structure elements */
	DMatch = sm->childmatches;
	TtaSetErrorMessages (0);
	while (DMatch != NULL)
	  {
	     if (DMatch->node->element != NULL &&
		 FindListSTreeByLabel (TtaGetElementLabel (DMatch->node->element)) == NULL)
		TtaDeleteTree (DMatch->node->element, doc);
	     DMatch = DMatch->next;
	  }
	TtaSetErrorMessages (1);
     }
   return res;
}

/*----------------------------------------------------------------------
   CheckSelection : checks if all the selected element are at the same level. Extends the selction  
   to an element if all its children are selected                                       
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      CheckSelectionLevel (Document doc)
#else
static boolean      CheckSelectionLevel (doc)
Document            doc;

#endif
{
   Element             prevFirst, parentFirst, nextLast, parentLast;
   boolean             result;

   TtaGiveFirstSelectedElement (doc, &OrigFirstSelect, &ffc, &flc);
   TtaGiveLastSelectedElement (doc, &OrigLastSelect, &lfc, &llc);
   MyFirstSelect = OrigFirstSelect;
   MyLastSelect = OrigLastSelect;
   parentFirst = NULL;
   maxSelDepth = 0;
   if (MyFirstSelect != MyLastSelect)
     {
	if (MyFirstSelect != NULL && ffc <= 1)
	  {			/* searching for the first selected element */
	     prevFirst = MyFirstSelect;
	     TtaPreviousSibling (&prevFirst);
	     parentFirst = TtaGetParent (MyFirstSelect);
	     while (parentFirst != NULL && prevFirst == NULL && TtaIsBefore (parentFirst, MyLastSelect))
	       {
		  MyFirstSelect = parentFirst;
		  prevFirst = MyFirstSelect;
		  TtaPreviousSibling (&prevFirst);
		  parentFirst = TtaGetParent (MyFirstSelect);
	       }
	  }
	if (MyLastSelect != NULL && (llc == 0 || (llc > 0 && llc >= TtaGetTextLength (MyLastSelect))))
	  {			/* searching for the last selected element */
	     nextLast = MyLastSelect;
	     TtaNextSibling (&nextLast);
	     parentLast = TtaGetParent (MyLastSelect);
	     while (parentLast != NULL && nextLast == NULL && TtaIsBefore (MyFirstSelect, parentLast))
	       {
		  MyLastSelect = parentLast;
		  nextLast = MyLastSelect;
		  TtaNextSibling (&nextLast);
		  parentLast = TtaGetParent (MyLastSelect);
	       }
	  }
     }
   else
     {
	prevFirst = TtaGetFirstChild (MyFirstSelect);
	nextLast = TtaGetLastChild (MyFirstSelect);
	while (prevFirst != NULL && prevFirst == nextLast)
	  {
	     MyFirstSelect = prevFirst;
	     prevFirst = TtaGetFirstChild (MyFirstSelect);
	     nextLast = TtaGetLastChild (MyFirstSelect);
	  }
	if (prevFirst != NULL)
	  {
	     MyFirstSelect = prevFirst;
	     MyLastSelect = nextLast;
	  }
	else
	  {
	     MyLastSelect = MyFirstSelect;
	  }
	parentFirst = parentLast = TtaGetParent (MyFirstSelect);
     }
   MySelect = NULL;
   result = MyFirstSelect != NULL && (parentFirst == parentLast);
   if (result && parentFirst != NULL)
     {				/* if all selected elements are at the same level, checking if ancestors have any sibling */
	/* if it is not the case, they become the first selected element */

	nextLast = MyLastSelect;
	prevFirst = MyFirstSelect;
	do
	   TtaNextSibling (&nextLast);
	while (nextLast != NULL && GITagName (nextLast) == NULL);
	do
	   TtaPreviousSibling (&prevFirst);
	while (prevFirst != NULL && GITagName (prevFirst) == NULL);
	while (parentFirst != NULL &&
	       TtaGetElementType (parentFirst).ElTypeNum != HTML_EL_BODY &&
	       nextLast == NULL && prevFirst == NULL)
	  {
	     maxSelDepth++;
	     MySelect = parentFirst;
	     parentFirst = TtaGetParent (parentFirst);
	     if (parentFirst != NULL)
	       {
		  nextLast = MySelect;
		  prevFirst = MySelect;
		  do
		     TtaNextSibling (&nextLast);
		  while (nextLast != NULL && GITagName (nextLast) == NULL);
		  do
		     TtaPreviousSibling (&prevFirst);
		  while (prevFirst != NULL && GITagName (prevFirst) == NULL);
	       }
	  }
     }
   return result;
}

/*----------------------------------------------------------------------
   Give the next selected element, accordingly  to extension given by CheckSelectionLevel 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         MyNextSelectedElement (Document doc, Element * elselect)
#else
static void         MyNextSelectedElement (doc, elselect)
Document            doc;
Element            *elselect;

#endif
{
   Element             elfirst;
   int                 fc, lc;

   if (*elselect == NULL || *elselect == MySelect || *elselect == MyLastSelect)
     {				/* if the selection is an unique element, or elselect is the last */
	*elselect = NULL;
     }
   else
     {
	if (*elselect == MyFirstSelect)
	  {
	     TtaGiveFirstSelectedElement (doc, &elfirst, &fc, &lc);
	     if (elfirst == MyFirstSelect)
		TtaGiveNextSelectedElement (doc, &elfirst, &fc, &lc);
	     else
		while (elfirst != NULL && TtaIsAncestor (elfirst, MyFirstSelect))
		   TtaGiveNextSelectedElement (doc, &elfirst, &fc, &lc);
	  }
	else
	  {
	     elfirst = *elselect;
	     TtaGiveNextSelectedElement (doc, &elfirst, &fc, &lc);
	  }
	if (elfirst != NULL && TtaIsAncestor (elfirst, MyLastSelect))
	   *elselect = MyLastSelect;
	else
	   *elselect = elfirst;
     }
}
/*----------------------------------------------------------------------
   IsValidHtmlChild(element, tag)                                       
   returns TRUE if the tag is valid as a direct descendant of an element of type elType 
   
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      IsValidHtmlChild (ElementType elType, char *tag, char *prevtag)
#else
static boolean      IsValidHtmlChild (elType, tag, prevtag)
Element             element;
char               *tag;
char               *prevtag;

#endif
{

   ElementType         elTypeChild;
   int                 cardinal, prevtypenum, typenum, i;
   ElementType 	      *subTypes;
   boolean             result, found;
   Construct           constOfType;

   result = FALSE;
   elTypeChild.ElSSchema = elType.ElSSchema;
   cardinal = TtaGetCardinalOfType(elType);
   subTypes = (ElementType *)TtaGetMemory(cardinal * sizeof(ElementType));
   TtaGiveConstructorsOfType(&subTypes,&cardinal,elType);
   constOfType = TtaGetConstructOfType(elType);
   GIType (tag, &typenum);
   if (typenum == 0)
      return FALSE;
   switch (constOfType)
      {
            case ConstructIdentity:
               if (subTypes[0].ElTypeNum == typenum)
		  result = TRUE;
	       else if (!strcmp (GITagNameByType (subTypes[0].ElTypeNum), "???"))
		  /* search if tag can beinserted as a child of the identity */
		  result = IsValidHtmlChild (subTypes[0], tag, "");
	       break;

	    case ConstructList:
	       if (subTypes[0].ElTypeNum == typenum)
		  result = TRUE;
	       else if (!strcmp (GITagNameByType (subTypes[0].ElTypeNum), "???"))
                  result = IsValidHtmlChild (subTypes[0], tag, "");
	       break;

	    case ConstructChoice:
	       for (i = 0; !result && i < cardinal; i++)
		 {
		    if (subTypes[i].ElTypeNum == typenum)
		       result = TRUE;
		    else if (!strcmp (GITagNameByType (subTypes[i].ElTypeNum),"???"))
 		       result = IsValidHtmlChild (subTypes[i], tag, "");
		 }
	       break;

	    case ConstructOrderedAggregate:
	       found = (!strcmp (prevtag, ""));
	       GIType (prevtag, &prevtypenum);
	       found = (prevtypenum == 0);
	       /* on recherche la regle participant au tag precedent */
	       for (i = 0; !found && i < cardinal; i++)
		 {
		    if (prevtypenum == subTypes[i].ElTypeNum)
		       found = TRUE;
		    else if (strcmp (GITagNameByType (subTypes[i].ElTypeNum),"???"))
		       i = cardinal;
		 }
	       if (found)
		 {
		    while (!result && i < cardinal)
		      {
			 if (typenum == subTypes[i].ElTypeNum)
			    result = TRUE;
			 else if (!strcmp (GITagNameByType (subTypes[i].ElTypeNum), "???"))
			    result = IsValidHtmlChild (subTypes[i], tag, "");
			 if (!result)
			    if (TtaIsOptionalInAggregate(i,elType)) 
			       i++;
			    else
			       i = cardinal;
		      }
		 }
	       break;
	    case ConstructUnorderedAggregate:
	       while (!result && i < cardinal)
		 {
		    if (typenum == subTypes[i].ElTypeNum)
		       result = TRUE;
		    else if (!strcmp (GITagNameByType (subTypes[i].ElTypeNum), "???"))
		       result = IsValidHtmlChild (subTypes[i], tag, "");
		    if (!result)
	               if (TtaIsOptionalInAggregate(i,elType)) 
			  i++;
		       else
			  i = cardinal;
		 }
	    case ConstructConstant:
	    case ConstructReference:
	    case ConstructBasicType:
	    case ConstructNature:
	    case ConstructPair:
            case ConstructError:
	       break;
	 }
   TtaFreeMemory((char *)subTypes);
   return result;
}

/*----------------------------------------------------------------------
   CheckValidTransRoot                                                  
   checks if the higher-level generated elements are possible children of the   
   transformation root element                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      CheckValidTransRoot (strmatch * sm, ElementType elTypeRoot, char *prevTag)
#else
static boolean      CheckValidTransRoot (sm, elTypeRoot, prevTag)
strmatch           *sm;
ElementType         elTypeRoot;
char               *prevTag;

#endif
{
   strmatchchildren   *smc;
   strmatch           *sm2;
   NodeDesc           *node;
   boolean             result, sonsmatch;
   char               *curTag;

   curTag = TtaGetMemory (NAME_LENGTH);
   result = TRUE;
   smc = sm->childmatches;
   while (result && smc != NULL)
     {
	if (smc->patsymb->Rule == NULL)
	  {			/* there is no rule for the current node */
	     sm2 = smc->node->matches;
	     sonsmatch = FALSE;
	     while (sm2 != NULL && !sonsmatch)
	       {		/* checks if the children of the node have been matched */
		  sonsmatch = (sm2->patsymb == smc->patsymb && sm2->childmatches != NULL);
		  if (!sonsmatch)
		     sm2 = sm2->next;
	       }
	     if (!sonsmatch)
	       {		/* if the children of the node have not been matched */
		  /* checks if the node can be transferred in the destination */
		  if (TtaGetElementVolume (smc->node->element) != 0)
		    {		/* if the element is empty, it is ignored in transformation */
		       if (strcmp (prevTag, smc->node->tag))
			  result = IsValidHtmlChild (elTypeRoot,
						     smc->node->tag,
						     prevTag);
		       strcpy (prevTag, smc->node->tag);
		    }
	       }
	     else
	       {		/* if they have been, checks the elements generated by these children */
		  result = CheckValidTransRoot (sm2, elTypeRoot, prevTag);
	       }
	  }
	else
	  {			/* there is a rule for the current node */
	     node = smc->patsymb->Rule->OptNodes;
	     if (node != NULL)
	       {		/* if there is at least one place node */
		  if (strcmp (prevTag, node->Tag))
		    {
		       result = IsValidHtmlChild (elTypeRoot,
						  node->Tag,
						  prevTag);
		       strcpy (prevTag, smc->node->tag);
		    }
	       }
	     else
	       {
		  node = smc->patsymb->Rule->NewNodes;
		  if (node != NULL)
		    {
		       if (!strcmp (node->Tag, "*"))
			  strcpy (curTag, smc->node->tag);
		       else
			  strcpy (curTag, node->Tag);
		       result = IsValidHtmlChild (elTypeRoot,
						  curTag,
						  prevTag);
		       strcpy (prevTag, curTag);
		    }
		  else		/*error */
		     result = FALSE;
	       }
	  }
	if (result)
	   smc = smc->next;
     }
   TtaFreeMemory (curTag);
   return result;
}

/*----------------------------------------------------------------------
   callback of the transformation selection menu 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TransCallbackDialog (int ref, int typedata, char *data)
#else
void                TransCallbackDialog (ref, typedata, data)
int                 ref;
int                 typedata;
char               *data;

#endif
{
   int                 val;
   DisplayMode         olddispl;

   val = (int) data;
   switch (ref - TransBaseDialog)
	 {
	    case TransMenu:
	       olddispl = TtaGetDisplayMode (TransDoc);
	       TtaSetDisplayMode (TransDoc, NoComputedDisplay);
	       TtaSelectElement (TransDoc, NULL);
	       ResultTrans = ApplyTransformation (TabMenuTrans[val], TransDoc);
	       if (!ResultTrans)
		 {
		    /* transformation has failed, restoring the old selection */
		    if (ffc == 0 && flc == 0)
		       TtaSelectElement (TransDoc, OrigFirstSelect);
		    else
		       TtaSelectString (TransDoc, OrigFirstSelect, ffc, flc);
		    TtaExtendSelection (TransDoc, OrigLastSelect, llc);
		    /* displaying an error message */
		    TtaSetStatus (TransDoc, 1, TtaGetMessage (AMAYA, AM_TRANS_FAILED), NULL);
		 }
	       else
		 {
		    /* transformation was succesful, checking the thot tree */
		    InitializeParser (TtaGetMainRoot (TransDoc), TRUE, TransDoc);
		    TtaSetStructureChecking (0, TransDoc);
		    CheckAbstractTree (NULL);
		    TtaSetStructureChecking (1, TransDoc);
		    /* selecting the new elements */
		    if (MyLastSelect == NULL)
		       if (!isClosed)
			  MyLastSelect = TtaGetLastChild (MyFirstSelect);
		       else
			  MyLastSelect = TtaGetLastChild (TtaGetParent (MyFirstSelect));
		    else
		       TtaPreviousSibling (&MyLastSelect);
		    if (isClosed)
		       TtaNextSibling (&MyFirstSelect);
		    else
		       MyFirstSelect = TtaGetFirstChild (MyFirstSelect);
		    TtaSelectElement (TransDoc, MyFirstSelect);
		    if (MyLastSelect != NULL && TtaIsBefore (MyFirstSelect, MyLastSelect))
		       TtaExtendSelection (TransDoc, MyLastSelect, 0);
		 }
	       TtaSetDisplayMode (TransDoc, olddispl);
	       FreeMatchEnv ();
	       break;
	 }
}

/*----------------------------------------------------------------------
   callback of  the tranform entry of type menu 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TransformType (Document doc, View view)
#else
void                TransformType (doc, view)
Document            doc;
View                view;

#endif
{
   Element             elemselect;
   int                 i, j, k;
   char               *menuBuf, *tag;
   strmatch           *sm;
   TagTree             node;

   match_env.subjecttree = NULL;
   match_env.listSubTrees = NULL;
   ResultTrans = FALSE;
   TransDoc = doc;
   /* initialisation du contexte de matching (lecture & parsing des patterns) */
   if (CheckSelectionLevel (TransDoc) && ppStartParser ("HTML"))
     {
	/* allocate temporary text buffer */
	menuBuf = TtaGetMemory (MAX_LENGTH);
	/* contruction de l'arbre de tags */
	maxMatchDepth = match_env.maxdepth + maxSelDepth;
	match_env.subjecttree = newNode ("Root");
	if (MySelect != NULL)
	  {
	     (match_env.subjecttree)->element = TtaGetParent (MySelect);
	     BuildTagTree (MySelect, TransDoc, match_env.subjecttree, maxMatchDepth, 0);
	  }
	else
	  {
	     (match_env.subjecttree)->element = TtaGetParent (MyFirstSelect);
	     elemselect = MyFirstSelect;
	     while (elemselect != NULL)
	       {
		  BuildTagTree (elemselect, TransDoc, match_env.subjecttree, maxMatchDepth, 0);
		  MyNextSelectedElement (TransDoc, &elemselect);
	       }
	  }
	/* pattern matching */
	parcours_postfix (match_env.subjecttree, matchnode);

	/* construct the popup menu */
	node = match_env.subjecttree;
	i = 0;
	j = 0;
	tag = TtaGetMemory (NAME_LENGTH);
	do
	  {			/* for each node above the first selected */
	     sm = node->matches;
	     while (sm != NULL)
	       {		/* for each matching of the node */
		  if (!strcmp (sm->patsymb->Tag, "pattern_root"))
		    {		/* if it is matching a pattern root : insert the transformation name */
		       /* in the menu buffer */
		       strcpy (tag, "");
		       if (CheckValidTransRoot (sm,
				      TtaGetElementType (sm->node->element),
						tag))
			 {
			    for (k = 0; k < i && strcmp (TabMenuTrans[k]->patsymb->SName, sm->patsymb->SName); k++) ;
			    if (k == i)
			      {
				 sprintf (&menuBuf[j], "%s%s", "B", sm->patsymb->SName);
				 j += strlen (&menuBuf[j]) + 1;
				 TabMenuTrans[i++] = (strmatch *) sm;
			      }
			    /*  else */
			    /*     TabMenuTrans[k]=(strmatch *)sm; */
			 }
		    }
		  sm = sm->next;
	       }
	     node = node->child;
	  }
	while (node != NULL &&
	       (!TtaIsAncestor (node->element, MyFirstSelect)));
	TtaFreeMemory (tag);
	if (i > 0)
	  {			/* if some transformations have been matched, shows the menu */
	     TtaNewPopup (TransBaseDialog + TransMenu, 0, TtaGetMessage (TRANSDIAL, TR_TRANSFORM), i, menuBuf, NULL, 'L');
	     TtaSetDialoguePosition ();
	     TtaShowDialogue (TransBaseDialog + TransMenu, TRUE);
	  }
	else
	   /* display an status message */
	   TtaSetStatus (TransDoc, 1, TtaGetMessage (AMAYA, AM_NO_TRANS), NULL);
	TtaFreeMemory (menuBuf);
     }
   else
      /* display an status message */
      TtaSetStatus (TransDoc, 1, TtaGetMessage (AMAYA, AM_NO_TRANS), NULL);
}



/*----------------------------------------------------------------------
   TransformIntoType:                                                           
   fonction de changement de type sur place                                             
   tente de changer la selection courante dans le document doc dans le type DestType    
   methode : on selectionne les transformation qui produisent le type demande           
   effectue un pattern matching de ces transformations sur la selection,                        
   applique la premiere transformation trouvee                                  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean             TransformIntoType (ElementType resultType, Document doc)
#else
boolean             TransformIntoType (resultType, doc)
ElementType         resultType;
Document            doc;

#endif
{
   char                DestTag[20];
   boolean             ok;
   int                 i;
   Element             elemselect;
   TransDesc          *td;
   SymbDesc           *sd;
   strmatch           *sm;
   TagTree             node;
   char               *tag;

   match_env.subjecttree = NULL;
   match_env.listSubTrees = NULL;
   ResultTrans = FALSE;
   TransDoc = doc;

   /* initialisation du contexte de matching (lecture & parsing des patterns) */
   if (CheckSelectionLevel (TransDoc) && ppStartParser ("HTML"))
     {
	ok = FALSE;
	strcpy (DestTag, GITagNameByType (resultType.ElTypeNum));
	/* selection des transformation en fonction du type demande */
	td = match_env.Transformations;
	match_env.maxdepth = 0;
	while (td != NULL)
	  {
	     if (td->TagDest == NULL || strcmp (td->TagDest, DestTag))
	       {		/* the transformation does not produce the asked tag, it is desactived */
		  td->ActiveTrans = FALSE;
		  sd = td->PatSymbs;
		  while (sd != NULL)
		    {
		       sd->ActiveSymb = FALSE;
		       sd = sd->next;
		    }
	       }
	     else
	       {
		  ok = TRUE;	/* at least 1 transformation produces the tag */
		  if (td->patdepth > match_env.maxdepth)
		     match_env.maxdepth = td->patdepth;
	       }
	     td = td->next;
	  }

	if (ok)
	  {
	     maxMatchDepth = match_env.maxdepth + maxSelDepth;
	     /* construction de l'arbre des types */

	     match_env.subjecttree = newNode ("Root");
	     if (MySelect != NULL)
	       {
		  (match_env.subjecttree)->element = TtaGetParent (MySelect);
		  BuildTagTree (MySelect, TransDoc, match_env.subjecttree, maxMatchDepth, 0);
	       }
	     else
	       {
		  (match_env.subjecttree)->element = TtaGetParent (MyFirstSelect);
		  elemselect = MyFirstSelect;
		  while (elemselect != NULL)
		    {
		       BuildTagTree (elemselect, TransDoc, match_env.subjecttree, maxMatchDepth, 0);
		       MyNextSelectedElement (TransDoc, &elemselect);
		    }
	       }
	     /* pattern matching */

	     parcours_postfix (match_env.subjecttree, matchnode);
	     /* construct the result list of matching */
	     node = match_env.subjecttree;
	     /* sets node to the selected node */
	     while (node->child != NULL && TtaIsAncestor (MyFirstSelect, node->element))
		node = node->child;
	     tag = TtaGetMemory (NAME_LENGTH);
	     i = 0;
	     do
	       {		/* for each node above the first selected */
		  node = node->parent;
		  sm = node->matches;
		  while (sm != NULL)
		    {		/* for each matching of the node */
		       if (!strcmp (sm->patsymb->Tag, "pattern_root"))
			 {	/* if it is matching a pattern root : insert the transformation */
			    /* in the matched transformations list */

			    strcpy (tag, "");
			    if (CheckValidTransRoot (sm,
				      TtaGetElementType (sm->node->element),
						     tag))
			       TabMenuTrans[i++] = (strmatch *) sm;
			 }
		       sm = sm->next;
		    }
	       }
	     while (node != match_env.subjecttree);
	     TtaFreeMemory (tag);
	     if (i > 0)
		/* if at least one transformation have been matched, apply the first one */
		TransCallbackDialog (TransBaseDialog + TransMenu, 1 + i, (char *) 0);

	  }
     }
   return ResultTrans;
}
