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
static boolean RestTransformChildren (Restruct restr, Element oldElem, Element newElem, TypeTree typeTree, TypeTree ancestTree, Document srcDoc, Document dstDoc);
#else  /* __STDC__ */
static boolean RestTransformChildren (/* restr, oldElem, newElem, typeTree, ancestTree, doc */);
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
 Element RestCreateDescent (Element ancestor, TypeTree ancestTree, TypeTree descendTree, TypeTree recOrigTree, TypeTree recDestTree, Document doc)
#else  /* __STDC__ */
 Element RestCreateDescent (ancestor, ancestTree, descendTree, recOrigTree, recDestTree, doc)
Element ancestor;
Element *newElem;
TypeTree ancestTree;
TypeTree descendTree;
TypeTree recOrigTree;
TypeTree recDestTree;
Document doc;
#endif  /* __STDC__ */
{

  ElementType elType;
  Element elLast, elNew, elParent;
  TypeTree nStack [10];
  int top, i;
  TypeTree tNode;
 

 
  elNew = NULL;
  top = 0;
  
  tNode = descendTree;
  if (recDestTree != NULL)
    {
      while (tNode != NULL && tNode->TId != recDestTree->TId && top < 10)
	{
	  nStack [top++] = tNode;
	  tNode = tNode->TParent;
	}
      if (tNode != NULL)
	tNode = recOrigTree;
    }
  while (ancestTree != NULL && tNode != NULL && tNode->TId != ancestTree->TId && top < 10)
    {
      /* recherche ancestTree dans les ancetres de descendTree */
      nStack [top++] = tNode;
      tNode = tNode->TParent;
    }
   if (tNode != NULL)
    {
      elType = TtaGetElementType (ancestor);
      while (top < 10 && !IsNodeOfType (tNode, elType))
	{ /* recherche le noeud de meme type que ancestor dans les ancetres */
	  /* de ancestTree */
	  nStack [top++] = tNode;
	  tNode = tNode->TParent;
	}
    }
  if (top < 10)
    {
      elParent = ancestor;
      for (i = top - 1; i >= 0; i--)
	{
	  elType.ElTypeNum = nStack[i]->TypeNum;
	  elType.ElSSchema = nStack[i]->TSchema;
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
 TypeTree RestTarget (TypeTree source, Restruct restr)
#else  /* __STDC__ */
 TypeTree RestTarget (source, restr)
TypeTree source;
Restruct restr;
#endif  /* __STDC__ */
{
  int i = 0;
  TypeTree res = NULL;
  boolean found = FALSE;

  while (i<SIZEPRINT && !found)
    {
      if (restr->RSrcPrint->SNodes[i] == source)
	{
	  res = restr->RCoupledNodes[i]->CDstNode;
	  found = TRUE;
	}
      else
	i++;
    }
  return res;
}

/*----------------------------------------------------------------------  
  RestSourceRec
  ----------------------------------------------------------------------*/
#ifdef __STDC__
 TypeTree RestSourceRec (TypeTree source, Restruct restr)
#else  /* __STDC__ */
 TypeTree RestSourceRec (source, restr)
TypeTree source;
Restruct restr;
#endif  /* __STDC__ */
{
  int i = 0;
  TypeTree res = NULL;
  boolean found = FALSE;

  while (i<SIZEPRINT && !found)
    {
      if (restr->RSrcPrint->SNodes[i] == source)
	{
	  res = restr->RCoupledNodes[i]->CRecFrom;
	  found = TRUE;
	}
      else
	i++;
    }
  return res;
}

/*----------------------------------------------------------------------  
  RestDestRec
  ----------------------------------------------------------------------*/
#ifdef __STDC__
 TypeTree RestDestRec (TypeTree source, Restruct restr)
#else  /* __STDC__ */
 TypeTree RestDestRec (source, restr)
TypeTree source;
Restruct restr;
#endif  /* __STDC__ */
{
  int i = 0;
  TypeTree res = NULL;
  boolean found = FALSE;

  while (i<SIZEPRINT && !found)
    {
      if (restr->RSrcPrint->SNodes[i] == source)
	{
	  res = restr->RCoupledNodes[i]->CRecTo;
	  found = TRUE;
	}
      else
	i++;
    }
  return res;
}

/*----------------------------------------------------------------------  
  RestSearchInTree
  
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static TypeTree RestSearchInTree (ElementType searchedType, TypeTree root)
#else  /* __STDC__ */
static TypeTree RestSearchInTree (searchedType, root)
ElementType searchedType;
TypeTree root;
#endif  /* __STDC__ */
{
  TypeTree currentNode, result = NULL;

  currentNode = root->TChild;
  while (result == NULL && currentNode !=NULL)
    {
      if (IsNodeOfType (currentNode, searchedType))
	result = currentNode;
      if (result == NULL && currentNode->TDiscard == TRUE)
	{
	  result = RestSearchInTree (searchedType, currentNode);
	}
      currentNode = currentNode->TNext;
    }
  return result;
}

/*----------------------------------------------------------------------  
  RestTransformElement
  transforme  elSource et sa descedance suivant les couplages des 
  descendants de parentTree et insere comme descendants de elDestParent,
  dans le document doc.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean RestTransformElement (Restruct restr, Element elSource, Element elDestParent, TypeTree sourceTree, TypeTree parentTree, Document srcDoc, Document dstDoc)
#else  /* __STDC__ */
static boolean RestTransformElement (restr, elSource, elDestParent, sourceTree, parentTree, srcDoc, dstDoc)
Restruct restr;
Element elSource;
Element elDestParent;
TypeTree sourceTree;
TypeTree parentTree;
Document srcDoc;
Document dstDoc;
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
  if (sourceTree != NULL && sourceTree->TEffective)
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
	  /* retrouver si recusif, les orig et des de la recursion */
	  
	  elTarget = RestCreateDescent (elDestParent, 
					targetParent, 
					target,
					RestSourceRec (sourceTree, restr),
					RestDestRec (sourceTree, restr),
					dstDoc);
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
	  printf("Noeud effectif non couple\n ");
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
	      TtaDeleteTree (elTarget, dstDoc);
	    }
	  else
	    {
	      elParent = elDestParent;
	      prev = TtaGetLastChild (elParent);
	    }
	  elTarget = TtaCopyTree (elSource, srcDoc, dstDoc, elParent);
	  /* on insere la copie */
	  if (prev == NULL)
	    TtaInsertFirstChild (&elTarget, elParent, dstDoc);
	  else
	    TtaInsertSibling (elTarget, prev, FALSE, dstDoc);
	  result = !TtaGetErrorCode();
	}
      else if (TtaIsLeaf(elType))
	{
	  /* RestTransferContent (elSource, elTarget, doc);*/
	  prev = TtaCopyTree (elSource, srcDoc, dstDoc, elTarget);
	  if (prev != NULL)
	    TtaInsertFirstChild (&prev, elTarget, dstDoc);
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
					  sourceTree,
					  srcDoc,
					  dstDoc);
	  if (result) 
	    result = RestTransAttr (elSource, 
				    elParent, 
				    dstDoc);
	  if (TtaGetElementVolume (elParent) == 0)
	    TtaDeleteTree (elParent, dstDoc);
	} 
    }
  else 
    result = FALSE;
  return result;
}

#ifdef THIS_IS_GARBISH

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
				      srcDoc,
				      dstDoc));
     }
return result;

#endif

/*----------------------------------------------------------------------  
  RestTransformChildren
  transforme la descendance de srcElem suivant les couplages de typeTree
  et les insere comme descendants de newElem
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean RestTransformChildren (Restruct restr, Element srcElem, Element newElem, TypeTree typeTree, TypeTree ancestTree, Document srcDoc, Document dstDoc)
#else  /* __STDC__ */
static boolean RestTransformChildren (restr, srcElem, newElem, typeTree, ancestTree, srcDoc, dstDoc)
Restruct restr;
Element srcElem;
Element newElem;
TypeTree typeTree;
TypeTree ancestTree;
Document srcDoc;
Document dstDoc;
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
   
      elType = TtaGetElementType (elChild);
      /* recherche dans les descendants de typeTree le noeud de type elType */
      /* recherche dans les petit-fils si le fils est un choix */
      childTree = RestSearchInTree (elType, typeTree);
      if (childTree != NULL && childTree->TEffective)
	result = (result && RestTransformElement (restr, 
						  elChild, 
						  newElem, 
						  childTree, 
						  ancestTree, 
						  srcDoc, 
						  dstDoc));
      else if (childTree != NULL)
	result = (result && RestTransformChildren (restr,
						   elChild,
						   newElem,
						   childTree,
						   ancestTree,
						   srcDoc, 
						   dstDoc));
      else
	result = FALSE;
      TtaNextSibling (&elChild);
    }
  return result;
} 
  
#ifdef THIS_IS_GARBISH
{    while (childTree != NULL && 
	     (!IsNodeOfType (childTree, elType) &&
	      !(childTree->TPrintSymb == '[' &&
		childTree->TChild != NULL &&
		IsNodeOfType (childTree->TChild, elType))))
	childTree = childTree->TNext;

      if (childTree != NULL && !IsNodeOfType (childTree, elType))
	childTree = childTree->TChild;
      if (childTree != NULL)
	result = (result && 
		  RestTransformElement (restr, elChild, newElem, childTree, typeTree, srcDoc, dstDoc));
      else if (TtaIsLeaf (elType))
	{
	  elCopy = TtaCopyTree (elChild, srcDoc, dstDoc, newElem);
	  prev = TtaGetLastChild (newElem);
	  if (elCopy != NULL)
	    if (prev == NULL)
	      TtaInsertFirstChild (&elCopy, newElem, dstDoc);
	    else
	      TtaInsertSibling (elCopy, prev, FALSE, dstDoc);
      	  result = !TtaGetErrorCode();
	}
      else
	result = FALSE; 
	
      /* traite le fils suivant */
      TtaNextSibling (&elChild);
    }
  return result;

#endif

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
	result = result && RestTransformElement (restruct, RContext->COldElems[i], newInstance, ptrTree->TChild, ptrTree, doc, doc);
      else
	{
	  /* element source unique */
	  target = RestTarget (ptrTree, restruct);
	  if (target == NULL || target->TypeNum != ptrTree->TypeNum)
	    /* le type de la racine source n'est pas couple avec un noeud */
	    /* de meme type : engage la transformation */
	    result = result && RestTransformChildren (restruct, RContext->COldElems[i], newInstance, ptrTree, ptrTree, doc, doc);
	  else
	    {
	      /* le type de la racine source est couple avec un noeud */
	      /* de meme type : cree une descendance et copie l'element */
	      targetType.ElSSchema = destType.ElSSchema;
	      targetType.ElTypeNum = target->TypeNum;
	      elTargetParent = RestCreateDescent (newInstance, 
						  NULL, 
						  target,
						  RestSourceRec (ptrTree, restruct),
						  RestDestRec (ptrTree, restruct),
						  doc);
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


/*----------------------------------------------------------------------  
  RestChangeToType
  applique la transformation restruct pour changement type 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
boolean RestChangeToType (Document sourceDoc, Document destDoc, Element destParent, Element destSibling, Restruct restruct)
#else  /* __STDC__ */
boolean RestChangeToType (sourceDoc, destDoc, destParent, destSibling, restruct)
Document sourceDoc;
Document destDoc;
Element destParent;
Element destSibling;
Restruct restruct;
#endif  /* __STDC__ */
{
  Element newInstance, elChild, elTargetParent, tprev, tfather;
  ElementType destType, targetType;
  Attribute attrDst, attrMandatory = NULL;
  TypeTree ptrTree, target;
  int i;
  boolean result = TRUE;
  
  destType = restruct->RDestType;
  ptrTree =  restruct->RSrcPrint->STree;
  
#ifdef DEBUG
  printf ("Application de la transformation :\n");
  printf (" source : %s\n", restruct->RSrcPrint->SPrint);
  printf (" cible  : %s\n\n", restruct->RDestPrint);
#endif
 if (destParent == NULL)
   {
     newInstance = TtaGetMainRoot (destDoc);
     elChild = TtaGetFirstChild (newInstance);
     while (elChild != NULL)
       {
	 TtaDeleteTree (elChild, destDoc);
	 elChild = TtaGetFirstChild (newInstance);
       }
     attrDst = NULL;
     TtaNextAttribute (newInstance, &attrDst);
     while (attrDst!=NULL)
       {
	 TtaRemoveAttribute (newInstance, attrDst, destDoc);
	 if (TtaGetErrorCode != 0)
	   attrMandatory = attrDst;
	 attrDst = attrMandatory;
	 TtaNextAttribute (newInstance, &attrDst);
       }
   }
 else
   {
     newInstance = TtaNewElement (destDoc, destType);
     if (destSibling == NULL)
       TtaInsertFirstChild (&newInstance, destParent, destDoc);
     else
       TtaInsertSibling (newInstance, destSibling, FALSE, destDoc);
   }

   /* transforme la descendance */
 for (i = 0; i < RContext->CNbOldElems; i++)
   if (TtaIsLeaf (TtaGetElementType(RContext->COldElems[i])))
     /* cas des feuilles : diretement transferees */
     result = result && RestTransferContent (RContext->COldElems[i], newInstance, destDoc);
   else
     /* source multiples : transforme chacun des elements */
     if (ptrTree->TPrintSymb == '*')
       result = result && RestTransformElement (restruct, RContext->COldElems[i], newInstance, ptrTree->TChild, ptrTree, sourceDoc, destDoc);
     else
       {
	 /* element source unique */
	 target = RestTarget (ptrTree, restruct);
	 if (target == NULL || target->TypeNum != ptrTree->TypeNum)
	   /* le type de la racine source n'est pas couple avec un noeud */
	   /* de meme type : engage la transformation */
	   result = result && RestTransformChildren (restruct, RContext->COldElems[i], newInstance, ptrTree, ptrTree, sourceDoc, destDoc);
	 else
	   {
	     /* le type de la racine source est couple avec un noeud */
	     /* de meme type : cree une descendance et copie l'element */
	     targetType.ElSSchema = destType.ElSSchema;
	     targetType.ElTypeNum = target->TypeNum;
	     elTargetParent = RestCreateDescent (newInstance, 
						 NULL, 
						 target,
						 RestDestRec (ptrTree, restruct),
						 RestSourceRec (ptrTree, restruct),
						 destDoc);
	     tprev = elTargetParent;
	     TtaPreviousSibling (&tprev);
	     if (tprev == NULL)
	       tfather = TtaGetParent (elTargetParent);
	     TtaDeleteTree (elTargetParent, destDoc);
	     
	     newInstance = TtaCopyTree (RContext->COldElems[i], sourceDoc, destDoc, tfather); 
	     if (tprev == NULL)
	       TtaInsertFirstChild (&newInstance, tfather, destDoc);
	     else
	       TtaInsertSibling (newInstance, tprev, FALSE, destDoc);
	   }
       }
 return result;
}
