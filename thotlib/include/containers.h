#ifndef CONTAINERS_H_
#define CONTAINERS_H_

/**
 * Generic container
 * @{ */
/**
 * Container element.
 */
typedef void* ContainerElement;

/** 
 * Generic container node.
 */
typedef struct sContainerNode
{
    ContainerElement  elem;
}*ContainerNode;

/** Destruction function pointer */
typedef void (*Container_DestroyElementFunction)(ContainerElement);

/** Comparaison function.*/
typedef int (*Container_CompareFunction)(ContainerElement,ContainerElement);

/**
 * Generic container.
 */
typedef struct sContainer
{
  Container_DestroyElementFunction destroyElement;
}*Container;

#ifndef __CEXTRACT__
extern ContainerElement ContainerNode_GetElement(ContainerNode node);
extern void Container_DeleteElement(Container container, ContainerNode node);
extern void Container_DeleteNode(Container container, ContainerNode node);
#endif /* __CEXTRACT__ */
/** @} */






/**
 * Genric forward iterator.
 * @{ */
//struct sForwardIterator;
typedef struct _sForwardIterator *ForwardIterator;

/** Generic GetFirst function */
typedef ContainerNode (*ForwardIterator_GetFirstFunction)(ForwardIterator iter);
/** Generic GetNext function */
typedef ContainerNode (*ForwardIterator_GetNextFunction)(ForwardIterator iter);

/**
 * Generic forward iterator.
 */
typedef struct _sForwardIterator
{
  Container container;
  ContainerNode currentNode;
  ForwardIterator_GetFirstFunction getFirst;
  ForwardIterator_GetNextFunction getNext;

} sForwardIterator;

#ifndef __CEXTRACT__
extern ForwardIterator ForwardIterator_Create(Container container,
                     ForwardIterator_GetFirstFunction getFirst,
                     ForwardIterator_GetNextFunction  getNext);
extern ContainerNode ForwardIterator_GetFirst(ForwardIterator iter);
extern ContainerNode ForwardIterator_GetNext(ForwardIterator iter);

extern long ForwardIterator_GetCount(ForwardIterator iter);

#endif /* __CEXTRACT__ */

#define ITERATOR_FOREACH(iter, nodetype, node)\
  for (node = (nodetype)ForwardIterator_GetFirst(iter); node!=NULL; \
    node = (nodetype)ForwardIterator_GetNext(iter))

/** @} */




/**
 * Double linked list
 * Based on generic container.
 * Can be used by all generic container function (with casting).
 * @{  */
/** 
 * Double linked list node.
 * Derived from generic container node (same first members).
 */
typedef struct _sDLListNode *DLListNode;
typedef struct _sDLListNode
{
  ContainerElement  elem;
  DLListNode      next;
  DLListNode      prev;
} sDLListNode;

/**
 * Double linked list
 * Derived from generic container (same first members).
 */
typedef struct _sDLList *DLList;
typedef struct _sDLList
{
    Container_DestroyElementFunction destroyElement;
    DLListNode first;
    DLListNode last;
} sDLList;


#ifndef __CEXTRACT__
extern DLList           DLList_Create();
extern void             DLList_Destroy(DLList list);
extern void             DLList_Empty(DLList list);
extern ThotBool         DLList_IsEmpty(DLList list);
extern DLListNode       DLList_Append(DLList list, ContainerElement elem);
extern DLListNode       DLList_Prepend(DLList list, ContainerElement elem);
extern DLListNode       DLList_InsertAfter(DLList list, DLListNode before, ContainerElement elem);
extern DLListNode       DLList_InsertBefore(DLList list, DLListNode after, ContainerElement elem);
extern ContainerElement DLList_RemoveElement(DLList list, DLListNode node);
extern void             DLList_DestroyElement(DLList list, DLListNode node);
extern ForwardIterator  DLList_GetForwardIterator(DLList list);
extern void             DLList_SwapContent(DLList list, DLListNode node1, DLListNode node2);
extern void             DLList_Sort(DLList list, Container_CompareFunction compare);
extern int              DLList_GetSize(DLList list);
extern DLListNode       DLList_GetElement(DLList list, int index);
extern DLList           DLList_GetRefList(DLList srcList, Container_CompareFunction compare);
extern DLList           DLList_GetRefListFromIterator(ForwardIterator iter, Container_CompareFunction compare);
#endif /* __CEXTRACT__ */
/** @} */




/**
 * Hash map
 * Based on generic container.
 * Can be used by all generic container function (with casting).
 * @{  */

typedef void* HashMapKey;

/**
 * Hash map node.
 * Based on list node. 
 */
typedef struct _sHashMapNode *HashMapNode;
typedef struct _sHashMapNode
{
  ContainerElement  elem;
  HashMapNode     next;
  HashMapNode     prev;
  HashMapKey        key;
}sHashMapNode;


/**
 * Hash map key node.
 * Each keynode is a list.
 * Derived from list.
 */
typedef struct _sDLList sHashMapKeyNode, *HashMapKeyNode;

/**
 * Hash map hash key function.
 */
typedef int (*HashMap_HashFunction)(HashMapKey);

/**
 * Hash map destroy key function.
 */
typedef void (*HashMap_DestroyKeyFunction)(HashMapKey);

/**
 * Hash map compare key function.
 */
typedef int (*HashMap_CompareKeyFunction)(HashMapKey, HashMapKey);

/**
 * Hash map
 * Derived from generic container (same first members).
 */
typedef struct _sHashMap *HashMap;
typedef struct _sHashMap
{
  Container_DestroyElementFunction destroyElement;
  HashMap_DestroyKeyFunction       destroyKey;
  HashMap_HashFunction             hash;
  HashMap_CompareKeyFunction       compare;
  HashMapKeyNode* nodes;
  int nbNodes;
  
}sHashMap;

#ifndef __CEXTRACT__
extern HashMap HashMap_Create(Container_DestroyElementFunction destroy,
                       HashMap_HashFunction hash,
                       int nbNodes);
extern void             HashMap_Destroy(HashMap map);
extern void             HashMap_Empty(HashMap map);
extern ThotBool         HashMap_IsEmpty(HashMap map);
extern ContainerElement HashMap_Set(HashMap map, HashMapKey key, ContainerElement elem);
extern ContainerElement HashMap_Get(HashMap map, const HashMapKey key);
extern HashMapNode      HashMap_Find(HashMap map, const HashMapKey key);
extern ContainerElement HashMap_Remove(HashMap map, HashMapKey key);
extern void             HashMap_DestroyElement(HashMap map, HashMapKey key);
extern ForwardIterator  HashMap_GetForwardIterator(HashMap map);
extern void             HashMap_SwapContents(HashMap map1, HashMap map2);
extern void             HashMap_Dump(HashMap map, ThotBool isKeyString);
#endif /* __CEXTRACT__ */
/** @} */

/**
 * Pointer hash map.
 * Hash map with a pointer as key.
 * Use it for address-persistent keyd object.
 */
#ifndef __CEXTRACT__
extern HashMap PointerHashMap_Create(Container_DestroyElementFunction destroy
                                    , int nbNodes);
#endif /* __CEXTRACT__ */
  

/**
 * String hash map.
 * Hash map with a c-string key.
 * The Key is hashed with the whole string.
 */
#ifndef __CEXTRACT__
extern HashMap StringHashMap_Create(Container_DestroyElementFunction destroy,
                                      ThotBool keyIsStored, int nbNodes);
#endif /* __CEXTRACT__ */

/**
 * Keyword hash map.
 * Hash map with a c-string key.
 * The Key is hashed with the first letter.
 */
#ifndef __CEXTRACT__
extern HashMap KeywordHashMap_Create(Container_DestroyElementFunction destroy,
                                      ThotBool keyIsStored, int nbNodes);

/**
 * Create a Keyword hash map initialized with a keyword list and NULL elements.
 * Note that a such KeywordHashMap embed copies of keys. 
 * \param list space-separated list of element names.
 */
extern HashMap KeywordHashMap_CreateFromList(Container_DestroyElementFunction destroy,
                                      int nbNodes, const char *list);

#endif /* __CEXTRACT__ */




#endif /*CONTAINERS_H_*/
