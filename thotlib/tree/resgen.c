#include "thot_gui.h"
#include "thot_sys.h"
#include "tree.h"
#include "application.h"
#include "content.h"
#include "view.h"
#include "selection.h"
#include "conststr.h"
#include "typestr.h"
#define THOT_EXPORT extern
#include "constres.h"
#include "resmatch_f.h"

#ifdef __STDC__
static boolean RestTransformChildren (Restruct restr, Element oldElem, Element newElem, TypeTree typeTree, Document doc);
#else  /* __STDC__ */
static boolean RestTransformChildren (/* restr, oldElem, newElem, typeTree, doc */);
#endif  /* __STDC__ */


#ifdef __STDC__
static boolean IsNodeOfType (TypeTree node, ElementType elType)
#else  /* __STDC__ */
static boolean IsNodeOfType (node, elType)
TypeTree node;
ElementType elType;
#endif  /* __STDC__ */
{
  boolean result = FALSE;

  if (node->TypeNum == elType.ElTypeNum)
    result = TRUE;
  else if (node->TPrintSymb == 'U')
    if (TtaIsLeaf (elType))
      result = TRUE;
  
  return result;
}
/*----------------------------------------------------------------------
  RestNextEffNode retourne le noeud effectif suivant lastNode dans tree
  Profondeur d'abord
  ----------------------------------------------------------------------*/
#ifdef __STDC__
TypeTree RestNextEffNode (TypeTree tree, TypeTree lastNode) 
#else  /* __STDC__ */
TypeTree RestNextEffNode (tree, lastNode) 
TypeTree tree;
TypeTree lastNode;
#endif  /* __STDC__ */
{
  boolean found = FALSE;
  boolean wentUp = FALSE;
  TypeTree current = NULL;

  if (tree == NULL)
    return NULL;
  if (lastNode == NULL)
    /* recherche le premier */
    current = tree;
  else
    /* recherche le suivant */
    if (lastNode->TChild != NULL)
      current = lastNode->TChild;
    else if (lastNode != tree)
      if (lastNode->TNext != NULL)
	current = lastNode->TNext;
      else
	{
	  current = lastNode->TParent;
	  wentUp = TRUE;
	}

  while (!found && current != NULL && (!wentUp || current != tree))
    {
      if (wentUp)
	/* on remonte dans l'arbre, on ne veut pas voir les noeuds effectifs */
	if (current->TNext !=NULL)
	  {
	    current = current->TNext;
	    wentUp = FALSE;
	  }
	else
	  current = current->TParent;
      else
	{
	  /* examine le nouveau noeud */
	  found = (current->TEffective);
	  if (!found)
	    {
	      /* passe au noeud suivant */
	      if (current->TChild != NULL)
		current = current->TChild;
	      else if (current->TNext != NULL)
		current = current->TNext;
	      else
		{	
		  current = current->TParent;
		  wentUp = TRUE;
		}
	    }
	}
    }
  if (!found)
    return NULL;
  else
    return current;
}

/*----------------------------------------------------------------------  
  RestTransferContent
  transfere le contenu de l'element oldElem dans newElem, les deux elements
  appartiennent au meme type de  base
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean RestTransferContent (Element oldElem, Element newElem, Document doc)
#else  /* __STDC__ */
static boolean RestTransferContent (Element oldElem, Element newElem, Document doc)
#endif  /* __STDC__ */
{
  ElementType newType, oldType;
  Language lang;
  char *buffer;
  char shape;
  int len, rank, x, y;
  boolean result = FALSE;
#ifdef DEBUG
  char msgbuf[100];

  printf("\n TransferContent : ");
#endif
  oldType = TtaGetElementType (oldElem);
  newType = TtaGetElementType (newElem);
  if (!TtaSameTypes (oldType, newType))
    return FALSE;
  else
    switch (oldType.ElTypeNum)
      {
      case RestTypeTexte:
      case RestTypeImage:
        len = TtaGetTextLength (oldElem);
        buffer = TtaGetMemory (len++);
        TtaGiveTextContent (oldElem, buffer, &len, &lang); 
#ifdef DEBUG
	strncpy (msgbuf, buffer, 20);
	strcat (msgbuf,"...");
	printf("%s\n",msgbuf);
#endif
        TtaSetTextContent (newElem, buffer, lang, doc);
        result = !TtaGetErrorCode();
        break;
      case RestTypeGraphique:
      case RestTypeSymbol:
#ifdef DEBUG
	printf("graphique\n");
#endif
        shape = TtaGetGraphicsShape (oldElem);
        TtaSetGraphicsShape (newElem, shape, doc);
        switch (shape)
          {
          case 'p':
          case 'S':
          case 'N':
          case 'U':
          case 'M':
          case 's':
          case 'B':
          case 'F':
          case 'A':
          case 'D':
            len = TtaGetPolylineLength (oldElem);
            for (rank = 1; rank <= len; rank++)
              { 
                TtaGivePolylinePoint (oldElem, rank, UnPoint, &x, &y);
                TtaAddPointInPolyline (newElem, rank, UnPoint, x, y, doc);
              }
            break;
          default : break;
          }
        result = !TtaGetErrorCode();
        break;
      case RestTypePage:
        result = TRUE;
        break;
      default : 
        result = FALSE;
        break;
      }
  return result;
}
/*----------------------------------------------------------------------  
  IsPresentInAggregate
  est vrai si un element de type typeNum est present comme fils de 
  elParent (dont le constructeur est un aggregat), si oui elem est
  cet element. Dans le cas ou l'element n'est pas present, elem 
  contient l'element precedant dans l'aggregat (ou NULL s'il n'y en 
  a pas).
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean IsPresentInAggregate (Element elParent, int typeNum, Element *elem)
#else  /* __STDC__ */
static boolean IsPresentInAggregate (elParent, typeNum, elem)
Element elParent;
int typeNum;
Element *elem;
#endif  /* __STDC__ */
{
  int card, indice;
  ElementType *typesArray;
  ElementType typeParent, typeChild;
  Element elChild,elPrev;
  boolean result, found;
  
  typeParent = TtaGetElementType (elParent);
  card = TtaGetCardinalOfType (typeParent);
  typesArray = TtaGetMemory (card * sizeof (ElementType));
  TtaGiveConstructorsOfType (&typesArray, &card, typeParent);
  elChild = TtaGetFirstChild (elParent);
  if (elChild == NULL)
    {
      /* l'aggregat est vide */
      *elem = NULL;
      result = FALSE;
    }
  else
    {
      elPrev = NULL;
      typeChild = TtaGetElementType (elChild);
      found = FALSE;
      indice = 0;
       /* boucle sur les fils de l'aggregat */
      while (!found && 
	     elChild != NULL && 
	     indice < card && 
	     typeChild.ElTypeNum != typeNum)
	{
	  /* pour chaque fils, cherche son no de type dans le tableau */
	  /* des types, on arrete si on a trouve le no de type recheche */
	  while (!found && 
		 indice < card &&
		 typeChild.ElTypeNum != typesArray[indice].ElTypeNum)
	    {
	      found = (typesArray[indice].ElTypeNum == typeNum);
	      if (!found)
		indice ++;
	    }
	  if (!found && indice < card)
	    {
	      /* on a trouve le type du fils courant dans le tableau */
	      /* on passe au fils suivant */
	      elPrev = elChild;
	      TtaNextSibling (&elChild);
	      if (elChild != NULL)
		typeChild = TtaGetElementType (elChild);
	    }
	}
      if (found)
	{ /* on a trouve le no de type cherche dans le tableau */
	  /* mais l'element n'est pas present dans l'aggregat */
	  result = FALSE;
	  *elem = elPrev;
	}
      else
	if (typeChild.ElTypeNum == typeNum)
	  {
	    /* on a trouve l'element recherche */
	    result = TRUE;
	    *elem = elChild;
	  }
	else
	  {
	    /* autres cas (erreur ??)*/
	    result = FALSE;
	    *elem = elPrev;
	  }
    }
  return result;
}

 
/*----------------------------------------------------------------------  
  RestCreateDescent
  cree une descendance d'elements correspondant a la branche de l'arbre
  de types destination definie par ancestTree...descendTree, ancestor
  est l'element existant auquel doit etre rattachee la branche creee
  newElem retourne l'element de plus bas niveau cree
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element RestCreateDescent (Element ancestor, TypeTree ancestTree, TypeTree descendTree, Document doc)
#else  /* __STDC__ */
static boolean RestCreateDescent (ancestor, ancestTree, descendTree, doc)
Element ancestor;
Element *newElem;
TypeTree ancestTree;
TypeTree descendTree;
Document doc;
#endif  /* __STDC__ */
{
  ElementType elType;
  Element elLast, elNew, elParent;
  TypeTree nStack [10];
  int top, i;
  TypeTree tNode;
 
  elNew = NULL;
  top = 1;
  tNode = descendTree;
  while (ancestTree != NULL && tNode->TId != ancestTree->TId && top < 10)
    {
      /* recherche ancestTree dans les ancetres de descendTree */
      nStack [top++] = tNode;
      tNode = tNode->TParent;
    }
  elType = TtaGetElementType (ancestor);
  while (!IsNodeOfType (tNode, elType))
    { /* recherche le noeud de meme type que ancestor dans les ancetres */
      /* de ancestTree */
      nStack [top++] = tNode;
      tNode = tNode->TParent;
    }
  if (top < 10)
    {
      elParent = ancestor;
      for (i = top - 1; i > 0; i--)
	{
	  elType.ElTypeNum = nStack[i]->TypeNum;
	  elLast = TtaGetLastChild (elParent);
	  if (elLast == NULL)
	    {
	      elNew = TtaNewElement (doc, elType);
	      TtaInsertFirstChild (&elNew, elParent, doc);
	    }
	  else
	    if (nStack[i]->TParent->TPrintSymb == '[')
	      /* constructeur choix : echec si elLast n'est pas du type */
	      /* de nStack[i] */
	      if (IsNodeOfType (nStack[i], TtaGetElementType (elLast)))
		elNew = elLast;
	      else
		{
		  i = 0;
		  elNew = NULL;
		}
	    else if (nStack[i]->TParent->TPrintSymb == '{')
	      {
		/* constructeur aggregat : recherche si l'element est deja */
		/* present, le cree et l'insere sinon */
		if (IsPresentInAggregate (elParent, nStack[i]->TypeNum,&elLast))
		  elNew = elLast;
		else
		  {
		    elNew = TtaNewElement (doc, elType);
		    if (elLast != NULL)
		      TtaInsertSibling (elNew, elLast, FALSE, doc);
		    else
		      TtaInsertFirstChild (&elNew, elParent, doc);
		  }
	      }
	    else
	      if (nStack[i]->TPrintSymb != '{' &&
		  nStack[i]->TPrintSymb != '(' )
		{
		  /* constructeur liste (autre ??) on cree un nouvel elt */
		  elNew = TtaNewElement (doc, elType);
		  TtaInsertSibling (elNew, elLast, FALSE, doc);
		}
	      else
		elNew = elLast;
	  elParent = elNew;
	}
    }
  return elNew;
}

     

static boolean RestTransAttr (Element oldElem, Element newElem, Document doc)
{
  return TRUE;
}

/*----------------------------------------------------------------------  
  RestTarget
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static TypeTree RestTarget (TypeTree source, Restruct restr)
#else  /* __STDC__ */
static TypeTree RestTarget (source, restr)
TypeTree source;
Restruct restr;
#endif  /* __STDC__ */
{
  int i = 0;
  TypeTree res = NULL;

  while (i<SIZEPRINT && res == NULL)
    {
      if (restr->RSrcPrint->SNodes[i] == source)
	res = restr->RCoupledNodes[i];
      if (res == NULL) i++;
    }
  return res;
}

/*----------------------------------------------------------------------  
  RestSearchInTree
  Recherche un element de type searchedType dans la descendance de 
  rootElem, retourne l'element trouve ou NULL si aucun element n'est 
  trouve
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static Element RestSearchInTree (ElementType searchedType, Element rootElem)
#else  /* __STDC__ */
static Element RestSearchInTree (searchedType, rootElem)
ElementType searchedType;
Element rootElem;
#endif  /* __STDC__ */
{
  Element result, precedent;
  
  precedent = NULL;
  result = TtaSearchTypedElement (searchedType, SearchInTree, rootElem);
  while (result != NULL)
    {
      precedent = result;
      result = TtaSearchTypedElement (searchedType,SearchForward, result);
      if (result != NULL && !TtaIsAncestor (result, rootElem))
	  result = NULL;
    }
  return precedent;
}

/*----------------------------------------------------------------------  
  RestTransformElement
  transforme  elSource et sa descedance suivant les couplages des 
  descendants de parentTree et insere comme descendants de elDestParent,
  dans le document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean RestTransformElement (Restruct restr, Element elSource, Element elDestParent, TypeTree sourceTree, TypeTree parentTree, Document doc)
#else  /* __STDC__ */
static boolean RestTransformElement (restr, elSource, elDestParent, sourceTree, parentTree, doc)
Restruct restr;
Element elSource;
Element elDestParent;
TypeTree sourceTree;
TypeTree parentTree;
Document doc;
#endif  /* __STDC__ */
{
  ElementType elType, targetType;
  Element elTarget, prev, elParent, elChild;
  TypeTree target, treeChild, targetParent;
  boolean result = FALSE;
  
#ifdef DEBUG
  ElementType dbgType;
  char msgbuf[100];
  
  printf("\n TransformElement ");
#endif 
 
  if (TtaGetElementVolume (elSource) == 0)
    return TRUE;
  elType = TtaGetElementType (elSource);

#ifdef DEBUG
  strcpy (msgbuf, TtaGetElementTypeName (elType));
  printf (" source : %s \n",msgbuf);
  dbgType = TtaGetElementType (elDestParent);
  strcpy (msgbuf, TtaGetElementTypeName (dbgType));
  printf (" destParent : %s \n",msgbuf);
  
#endif 
  if ( sourceTree != NULL && sourceTree->TEffective)
    {
#ifdef DEBUG
      printf("trouve noeud effectif ");
#endif 
      /* on a trouve un noeud effectif correspondant au type du fils */
      target = RestTarget (sourceTree, restr);
      if (parentTree->TPrintSymb != '*')
	targetParent = RestTarget (parentTree, restr);
      else
	targetParent = restr->RDestTree;
      if (target != NULL)
	{
	  elTarget = RestCreateDescent (elDestParent, targetParent, target, doc);
#ifdef DEBUG
	  targetType.ElSSchema = restr->RDestType.ElSSchema;
	  targetType.ElTypeNum = target->TypeNum;

	  printf("couple avec ");
	  
	  strcpy (msgbuf, TtaGetElementTypeName (targetType));
	  printf ("%s \n",msgbuf);
#endif
	}
      else
	{
#ifdef DEBUG
	  printf("non couple\n ");
#endif 
	  targetType.ElSSchema = elType.ElSSchema;
	  targetType.ElTypeNum = elType.ElTypeNum;
	}
      if (target == NULL || IsNodeOfType (target, elType))
	  /* target->TypeNum == sourceTree->TypeNum)*/
	{
	  /* les noeuds couples sont de meme type, */
	  /* on copie la source */
	  if (elTarget != NULL)
	    {
	      prev = elTarget;
	      TtaPreviousSibling (&prev);
	      elParent = TtaGetParent (elTarget);
	      TtaDeleteTree (elTarget, doc);
	    }
	  else
	    {
	      elParent = elDestParent;
	      prev = TtaGetLastChild (elParent);
	    }
	  elTarget = TtaCopyTree (elSource, doc, doc, elParent);
	  /* on insere la copie */
	  if (prev == NULL)
	    TtaInsertFirstChild (&elTarget, elParent, doc);
	  else
	    TtaInsertSibling (elTarget, prev, FALSE, doc);
	  result = !TtaGetErrorCode();
	}
      else if (TtaIsLeaf(elType))
	{
	  /* RestTransferContent (elSource, elTarget, doc);*/
	  prev = TtaCopyTree (elSource, doc, doc, elTarget);
	  if (prev != NULL)
	    TtaInsertFirstChild (&prev, elTarget, doc);
	  result =!TtaGetErrorCode();
	}
      else
	{ /* les noeuds couple ne sont pas de meme type */
	  /* applique la transformation aux descendants */
	  if (elTarget != NULL)
	    elParent = elTarget;
	  else
	    elParent = elDestParent;
	  result = RestTransformChildren (restr, 
					  elSource, 
					  elParent, 
					  sourceTree, 
					  doc);
	  if (result) 
	    result = RestTransAttr (elSource, 
				    elParent, 
				    doc);
	  if (TtaGetElementVolume (elParent) == 0)
	    TtaDeleteTree (elParent, doc);
	} 
    }
  else /* sourceTree == NULL */
    /* l'element n'a pas de type marque comme effectif, */
    /* c'est donc un choix ou une liste uniquement instanciee */
    /* on transforme sa descendence en l'ignorant. */
    {
      /* recherche le fils de elSource et son arbre de type */
      elChild = TtaGetFirstChild (elSource);
      elType = TtaGetElementType (elChild);
      treeChild = sourceTree->TChild;
      while (treeChild != NULL && !IsNodeOfType (treeChild, elType))
	/*treeChild->TypeNum != elType.ElTypeNum*/
	treeChild = treeChild->TNext;
      
      result = (treeChild != NULL &&
		RestTransformElement (restr, 
				      elChild, 
				      elDestParent,
				      treeChild,
				      parentTree, 
				      doc));
    }
  return result;
}

/*----------------------------------------------------------------------  
  RestTransformChildren
  transforme la descendance de srcElem suivant les couplages de typeTree
  et les insere comme descendants de newElem
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean RestTransformChildren (Restruct restr, Element srcElem, Element newElem, TypeTree typeTree, Document doc)
#else  /* __STDC__ */
static boolean RestTransformChildren (restr, srcElem, newElem, typeTree, doc)
Restruct restr;
Element srcElem;
Element newElem;
TypeTree typeTree;
Document doc;
#endif  /* __STDC__ */
{
  Element elChild, elCopy, prev;
  ElementType elType;
  boolean result = TRUE;
  TypeTree childTree;

  /* traite succesivement chacun des fils de oldElem */
  elChild = TtaGetFirstChild (srcElem);
  while (elChild != NULL && result)
    {
      childTree = typeTree->TChild;
      elType = TtaGetElementType (elChild);
      /* recherche dans les fils de typeTree le noeud de type elType */
      /* recherche dans les petit-fils si le fils est un choix */
      while (childTree != NULL && 
	     (!IsNodeOfType (childTree, elType) &&
	      !(childTree->TPrintSymb == '[' &&
		childTree->TChild != NULL &&
		IsNodeOfType (childTree->TChild, elType))))
	childTree = childTree->TNext;

      if (childTree != NULL && !IsNodeOfType (childTree, elType))
	childTree = childTree->TChild;
      if (childTree != NULL)
	result = (result && 
		  RestTransformElement (restr, elChild, newElem, childTree, typeTree, doc));
      else if (TtaIsLeaf (elType))
	{
	  elCopy = TtaCopyTree (elChild, doc, doc, newElem);
	  prev = TtaGetLastChild (newElem);
	  if (elCopy != NULL)
	    if (prev == NULL)
	      TtaInsertFirstChild (&elCopy, newElem, doc);
	    else
	      TtaInsertSibling (elCopy, prev, FALSE, doc);
      	  result = !TtaGetErrorCode();
	}
      else
	result = FALSE; 
	
      /* traite le fils suivant */
      TtaNextSibling (&elChild);
    }
  return result;
} 

/*----------------------------------------------------------------------  
  RestChangeOnPlace
  applique la transformation restruct pour changement type sur place
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean RestChangeOnPlace (Document doc, Restruct restruct)
#else  /* __STDC__ */
boolean RestChangeOnPlace (doc, restruct)
Document doc;
Restruct restruct;
#endif  /* __STDC__ */
{
  TypeTree ptrTree, target;
  Element newInstance, elTargetParent, father, prev, tfather, tprev;
  int i;
  ElementType destType, targetType;
  boolean result = TRUE;

  destType = restruct->RDestType;
  ptrTree =  restruct->RSrcPrint->STree;
  
#ifdef DEBUG
  printf ("Application de la transformation :\n");
  printf (" source : %s\n", restruct->RSrcPrint->SPrint);
  printf (" cible  : %s\n\n", restruct->RDestPrint);
#endif

  /* memorise l'element precedant l'element transforme */
  father = NULL;
  prev = RContext->COldElems[0];
  TtaPreviousSibling (&prev);
  if (prev == NULL)
    /* l'element transformer est un premier fils, on memorise son pere */
    father = TtaGetParent (RContext->COldElems[0]);
 
  /* detache les elements a transformer */
  for (i = 0; i < RContext->CNbOldElems; i++)
    TtaRemoveTree (RContext->COldElems[i], doc);
  /* genere le nouvel element racine*/
  newInstance = TtaNewElement (doc, destType);
  if (prev == NULL)
    TtaInsertFirstChild (&newInstance, father, doc);
  else
    TtaInsertSibling (newInstance, prev, FALSE, doc);

  /* transforme la descendance */
  for (i = 0; i < RContext->CNbOldElems; i++)
    if (TtaIsLeaf (TtaGetElementType(RContext->COldElems[i])))
      /* cas des feuilles : diretement transferees */
      result = result && RestTransferContent (RContext->COldElems[i], newInstance, doc);
    else
      /* source multiples : transforme chacun des elements */
      if (ptrTree->TPrintSymb == '*')
	result = result && RestTransformElement (restruct, RContext->COldElems[i], newInstance, ptrTree->TChild, ptrTree, doc);
      else
	{
	  /* element source unique */
	  target = RestTarget (ptrTree, restruct);
	  if (target == NULL || target->TypeNum != ptrTree->TypeNum)
	    /* le type de la racine source n'est pas couple avec un noeud */
	    /* de meme type : engage la transformation */
	    result = result && RestTransformChildren (restruct, RContext->COldElems[i], newInstance, ptrTree, doc);
	  else
	    {
	      /* le type de la racine source est couple avec un noeud */
	      /* de meme type : cree une descendance et copie l'element */
	      targetType.ElSSchema = destType.ElSSchema;
	      targetType.ElTypeNum = target->TypeNum;
	      elTargetParent = RestCreateDescent (newInstance, NULL, target, doc);
	      tprev = elTargetParent;
	      TtaPreviousSibling (&tprev);
	      if (tprev == NULL)
		tfather = TtaGetParent (elTargetParent);
	      TtaDeleteTree (elTargetParent, doc);
	      
	      newInstance = TtaCopyTree (RContext->COldElems[i], doc, doc, tfather); 
	      if (tprev == NULL)
		TtaInsertFirstChild (&newInstance, tfather, doc);
	      else
		TtaInsertSibling (newInstance, tprev, FALSE, doc);
	    }
	}
	      
  if (!result)
    {
      /* la transformation a echoue : on restaure l'instance source */
      TtaDeleteTree (newInstance, doc);
      if (prev == NULL)
        TtaInsertFirstChild (&(RContext->COldElems[0]), father, doc);
      else
        TtaInsertSibling (RContext->COldElems[0], prev, FALSE, doc);
      TtaSelectElement (doc, RContext->COldElems[0]); 
      for (i = 1; i < RContext->CNbOldElems; i++)
	TtaInsertSibling (RContext->COldElems[i], RContext->COldElems[i - 1], FALSE, doc);
      if (RContext->CNbOldElems > 1)
	TtaExtendSelection (doc, RContext->COldElems[RContext->CNbOldElems - 1],0);
    }
  else
    /* transformation reussie : on selectionne la nouvelle instance */
    /* et on efface l'ancienne */
    { 
      TtaSetDocumentModified (doc);
      TtaSelectElement (doc, newInstance);
      for (i = 0; i < RContext->CNbOldElems; i++)
	TtaDeleteTree(RContext->COldElems[i], doc);
    }
  return result;
}


