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
  RestCreateDescent
  cree une descendance d'elements correspondant a la branche de l'arbre
  de types destination definie par ancestTree...descendTree, ancestor
  est l'element existant auquel doit etre rattachee la branche creee
  newElem retourne l'element de plus bas niveau cree
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean RestCreateDescent (Element ancestor, Element *newElem, TypeTree ancestTree, TypeTree descendTree, Document doc)
#else  /* __STDC__ */
static boolean RestCreateDescent (ancestor, newElem, ancestTree, descendTree, doc)
Element ancestor;
Element *newElem;
TypeTree ancestTree;
TypeTree descendTree;
Document doc;
#endif  /* __STDC__ */
{
  ElementType elType;
  Element elLast, elNew;
  TypeTree firstChild, tChild;
  boolean found = FALSE;

  if (descendTree->TId != ancestTree->TId)
    {
      firstChild = NULL;
      if (ancestTree->TChild != NULL)
	firstChild = ancestTree->TChild;
      else if (ancestTree->TPrintSymb == '@')
	firstChild = ancestTree->TRecursive->TChild;
      tChild = firstChild;
      while (!found && tChild != NULL)
	{
	  found = (tChild->TId == descendTree->TId);
	  if (!found)
	    tChild = tChild->TNext;
	}
      if (!found)
	{
	  tChild = firstChild;
	  while (!found && tChild != NULL)
	    {
	      found = RestCreateDescent (ancestor, newElem, tChild, descendTree, doc);
	      if (!found)
		tChild = tChild->TNext;
	    }
	}
      if (found)
	{
	  elType.ElTypeNum = tChild->TypeNum;
	  elType.ElSSchema = (TtaGetElementType (ancestor)).ElSSchema;
	  elLast = TtaGetLastChild (ancestor);
	  if (elLast == NULL)
	    {
	      elNew = TtaNewElement (doc, elType);
	      TtaInsertFirstChild (&elNew, *newElem, doc);
	    }
	  else if (((TtaGetElementType (elLast)).ElTypeNum == elType.ElTypeNum)
               && (descendTree->TParent->TPrintSymb != '('))
	    {
	      elNew = elLast;
	    }
	   else 
	     {
	       elNew = TtaNewElement (doc, elType);
	       TtaInsertSibling (elNew, elLast, FALSE, doc);
	     }
	  *newElem = elNew;
	}
      
    }
  else
    *newElem = ancestor;
  return (!TtaGetErrorCode());
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
  Element elTarget, elTargetParent, prev, elParent, elChild;
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

  if (TtaIsLeaf (elType))
    {
      /* cas des feuilles : copie d'arbre */ 
      /* probleme avec TtaCreateDescentWithContent si elDestParent a deja */
      /* des fils, cree une branche AVANT !!! */
      elTarget = TtaCreateDescentWithContent (doc, elDestParent, elType);
      if (elTarget != NULL)
	result = RestTransferContent (elSource, elTarget, doc) &&
	  RestTransAttr (elSource, elTarget, doc);
    }
  else
    { 
      /*
      treeChild = RestNextEffNode (parentTree, parentTree);
      *//* recherche le type de elChild dans l'arbre de types source *//*
      while (treeChild != NULL && !IsNodeOfType(treeChild, elType))
	treeChild = RestNextEffNode (parentTree, treeChild);
	*/
      if (/*treeChild*/ sourceTree != NULL && sourceTree->TEffective)
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

	      targetType.ElSSchema = restr->RDestType.ElSSchema;
	      targetType.ElTypeNum = target->TypeNum;
#ifdef DEBUG
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
	  /* on recherche si un element du type fils est deja dans */
	  /* la descendence de elDestParent */
	  elTargetParent = RestSearchInTree (targetType, elDestParent);
	  if (elTargetParent == NULL)
	    /* on cree la descendence de elDestParent */
	    elTargetParent = TtaCreateDescent (doc, elDestParent, targetType);
	  if (elTargetParent != NULL)
	    if (target == NULL || target->TypeNum == sourceTree->TypeNum)
	      {
		/* les noeuds couples sont de meme type, */
		/* on copie la source */
		elParent = TtaGetParent (elTargetParent);
		elTarget = TtaCopyTree (elSource, doc, doc, elParent);
		prev = TtaGetLastChild (elParent);
		while (prev != NULL && TtaGetElementVolume (prev) == 0)
		  {
		    TtaDeleteTree (prev, doc);
		    prev = TtaGetLastChild (elParent);
		  }
		if (prev == NULL)
		  TtaInsertFirstChild (&elTarget, elParent, doc);
		else
		  TtaInsertSibling (elTarget, prev, FALSE, doc);
		if (TtaGetElementVolume (elTargetParent) == 0)
		  TtaDeleteTree (elTargetParent, doc);
		result = !TtaGetErrorCode();
	      }
	    else 
	      { 
		/* applique la transformation aux descendants */
		result = RestTransformChildren (restr, 
						elSource, 
						elTargetParent, 
						sourceTree, 
						doc);
		if (result) 
		  result = RestTransAttr (elSource, 
					  elTargetParent, 
					  doc);
		if (TtaGetElementVolume (elTargetParent) == 0)
		  TtaDeleteTree (elTargetParent, doc);
	      } 
	}
      else /* treeChild == NULL */
	/* l'element n'a pas de type marque comme effectif,
	   c'est donc un choix ou une liste uniquement instanciee
	   on transforme sa descendence en l'ignorant. */
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
  Element elChild;
  ElementType elType;
  boolean result = TRUE;
  TypeTree childTree;

  /* traite succesivement chacun des fils de oldElem */
  elChild = TtaGetFirstChild (srcElem);
  while (elChild != NULL && result)
    {
      childTree = typeTree->TChild;
      elType = TtaGetElementType (elChild);
      while (childTree != NULL && !IsNodeOfType (childTree, elType))
	/* childTree->TypeNum != elType.ElTypeNum */
	childTree = childTree->TNext;
      result = (result && 
		(childTree != NULL) && 
		RestTransformElement (restr, elChild, newElem, childTree, typeTree, doc));
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
	      elTargetParent = TtaCreateDescent (doc, newInstance, targetType);
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


