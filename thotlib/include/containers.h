#ifndef CONTAINERS_H_
#define CONTAINERS_H_

#include "thot_sys.h"

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
typedef int (*Container_CompareFunction)(const ContainerElement,const ContainerElement);

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
 * Simple list
 * Based on generic container.
 * Can be used by all generic container function (with casting).
 * @{  */
/** 
 * Simple list node.
 * Derived from generic container node (same first members).
 */
typedef struct _sSListNode *SListNode;
typedef struct _sSListNode
{
  ContainerElement elem;
  SListNode        next;
} sSListNode;

/**
 * Simple list
 * Derived from generic container (same first members).
 */
typedef struct _sSList *SList;
typedef struct _sSList
{
    Container_DestroyElementFunction destroyElement;
    SListNode first;
    SListNode last;
} sSList;

#ifndef __CEXTRACT__
extern SList            SList_Create();
extern void             SList_Destroy(SList list);
extern void             SList_Empty(SList list);
extern ThotBool         SList_IsEmpty(SList list);
extern SListNode        SList_Append(SList list, ContainerElement elem);
extern SListNode        SList_Prepend(SList list, ContainerElement elem);
#define                 SList_GetNext(list, node) (node->next)
extern SListNode        SList_GetPrev(SList list, SListNode node);
extern SListNode        SList_Find(SList list, const ContainerElement elem);
extern SListNode        SList_InsertAfter(SList list, SListNode before, ContainerElement elem);
extern SListNode        SList_InsertBefore(SList list, SListNode after, ContainerElement elem);
extern ContainerElement SList_RemoveElement(SList list, SListNode node);
extern void             SList_DestroyElement(SList list, SListNode node);
extern ForwardIterator  SList_GetForwardIterator(SList list);
extern int              SList_GetSize(SList list);
extern SListNode        SList_GetElement(SList list, int index);
#endif /* __CEXTRACT__ */
/** @} */






/**
 * Double linked list
 * Based on generic container.
 * Can be used by all generic container function (with casting).
 * @{  */
/** 
 * Double linked list node.
 * Derived from simple list node (same first members).
 */
typedef struct _sDLListNode *DLListNode;
typedef struct _sDLListNode
{
  ContainerElement  elem;
  DLListNode        next;
  DLListNode        prev;
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
inline void             DLList_Destroy(DLList list) {SList_Destroy((SList)list);} 
inline void             DLList_Empty(DLList list) {SList_Empty((SList)list);} 
inline ThotBool         DLList_IsEmpty(DLList list) {return SList_IsEmpty((SList)list);} 
extern DLListNode       DLList_Append(DLList list, ContainerElement elem);
extern DLListNode       DLList_Prepend(DLList list, ContainerElement elem);
#define                 DLList_GetNext(list, node) (node->next)
#define                 DLList_GetPrev(list, node) (node->prev)
inline DLListNode       DLList_Find(DLList list, const ContainerElement elem) {return (DLListNode) SList_Find((SList) list, elem);}
extern DLListNode       DLList_InsertAfter(DLList list, DLListNode before, ContainerElement elem);
extern DLListNode       DLList_InsertBefore(DLList list, DLListNode after, ContainerElement elem);
extern ContainerElement DLList_RemoveElement(DLList list, DLListNode node);
extern void             DLList_DestroyElement(DLList list, DLListNode node);
inline ForwardIterator  DLList_GetForwardIterator(DLList list) {return SList_GetForwardIterator((SList)list);} 
extern void             DLList_SwapContent(DLList list, DLListNode node1, DLListNode node2);
extern void             DLList_Sort(DLList list, Container_CompareFunction compare);
inline int              DLList_GetSize(DLList list) {return SList_GetSize((SList)list);} 
inline DLListNode       DLList_GetElement(DLList list, int index) {return (DLListNode)SList_GetElement((SList)list, index);}
extern DLList           DLList_GetRefList(DLList srcList, Container_CompareFunction compare);
extern DLList           DLList_GetRefListFromIterator(ForwardIterator iter, Container_CompareFunction compare);
#endif /* __CEXTRACT__ */
/** @} */




/**
 * Set
 * Based on generic container.
 * Can be used by all generic container function (with casting).
 * A set is a list where element are sorted an unique.
 * @{  */

typedef struct _sSListNode _sSSetNode, *SSetNode;

/**
 * Set
 * Derived from simple list (same first members).
 */
typedef struct _sSSet *SSet;
typedef struct _sSSet
{
    Container_DestroyElementFunction destroyElement;
    SSetNode first;
    SSetNode last;
    Container_CompareFunction compare;
} sSSet;


#ifndef __CEXTRACT__
extern SSet             SSet_Create(Container_CompareFunction compare);
inline void             SSet_Destroy(SSet set) { SList_Destroy((SList)set); }
inline void             SSet_Empty(SSet set)   { SList_Empty((SList)set); }
inline ThotBool         SSet_IsEmpty(SSet set) { return SList_IsEmpty((SList)set); }
#define                 SSet_GetNext(set, node) (SList_GetNext((SList)set, node))
inline SSetNode         SSet_GetPrev(SSet set, SSetNode node) { return SList_GetPrev((SList)set, node); }
extern SSetNode         SSet_Find(SSet set, const ContainerElement elem);
extern SSetNode         SSet_Insert(SSet set, ContainerElement elem);
inline ContainerElement SSet_RemoveElement(SSet set, SSetNode node) { return SList_RemoveElement((SList)set, node); }
inline void             SSet_DestroyElement(SSet set, SSetNode node) { SList_DestroyElement((SList)set, node); }
inline ForwardIterator  SSet_GetForwardIterator(SSet set) { return SList_GetForwardIterator((SList)set); }
inline int              SSet_GetSize(SSet set) { return SList_GetSize((SList)set); }
inline SSetNode         SSet_GetElement(SSet set, int index) { return SList_GetElement((SList)set, index); }
#endif /* __CEXTRACT__ */
/** @} */


/**
 * StringSet are set of strings.
 * String are embeded.
 * @{ 
 */
typedef SSet StringSet;
typedef SSetNode StringSetNode;

#ifndef __CEXTRACT__
inline StringSet        StringSet_Create();
extern StringSet        StringSet_CreateFromString(const char* str, const char* sep);
inline void             StringSet_Destroy(StringSet set) { SSet_Destroy(set); }
inline void             StringSet_Empty(StringSet set)   { SSet_Empty(set); }
inline ThotBool         StringSet_IsEmpty(StringSet set) { return SSet_IsEmpty(set);}
#define                 StringSet_GetNext(set, node) (SSet_GetNext((SSet)set, node))
inline StringSetNode    StringSet_GetPrev(StringSet set, StringSetNode node) { return SSet_GetPrev((SSet)set, node); }
inline StringSetNode    StringSet_Find(StringSet set, char* str) { return SSet_Find((SSet)set, str); }
extern StringSetNode    StringSet_Insert(StringSet set, const char* str);
extern void             StringSet_InsertMulti(StringSet set, const char* str, const char* sep);
inline char*            StringSet_RemoveElement(StringSet set, StringSetNode node) { return (char*)SSet_RemoveElement((SSet)set, node); }
inline void             StringSet_DestroyElement(StringSet set, StringSetNode node) { SSet_DestroyElement((SSet)set, node); }
inline ForwardIterator  StringSet_GetForwardIterator(StringSet set) { return SSet_GetForwardIterator((SSet)set); }
inline int              StringSet_GetSize(StringSet set) { return SSet_GetSize((SSet)set); }
inline StringSetNode    StringSet_GetElement(StringSet set, int index) { return SSet_GetElement((SSet)set, index); }
#endif /* __CEXTRACT__ */
/** @} */








/**
 * Searchable set.
 * Based on generic container.
 * Can be used by all generic container function (with casting).
 * A set is a list where element are sorted an unique.
 * @{  */

typedef _sSSetNode _sSearchSetNode, *SearchSetNode;

/**
 * Searchable Set
 * Derived from simple set (same first members).
 */
typedef struct _sSearchSet *SearchSet;
typedef struct _sSearchSet
{
    Container_DestroyElementFunction destroyElement;
    SSetNode first;
    SSetNode last;
    Container_CompareFunction compare;
    Container_CompareFunction search;
} sSearchSet;

#ifndef __CEXTRACT__
extern SearchSet        SearchSet_Create(Container_DestroyElementFunction destroy,
                          Container_CompareFunction compare,
                          Container_CompareFunction search);
inline void             SearchSet_Destroy(SearchSet set) { SSet_Destroy((SSet)set); }
inline void             SearchSet_Empty(SearchSet set)   { SSet_Empty((SSet)set); }
inline ThotBool         SearchSet_IsEmpty(SearchSet set) { return SSet_IsEmpty((SSet)set); }
#define                 SearchSet_GetNext(set, node) (SSet_GetNext((SSet)set, node))
inline SearchSetNode    SearchSet_GetPrev(SearchSet set, SearchSetNode node) { return SSet_GetPrev((SSet)set, node); }
inline SearchSetNode    SearchSet_Find(SearchSet set, const ContainerElement elem) { return SSet_Find((SSet)set, elem); }
inline SearchSetNode    SearchSet_Insert(SearchSet set, ContainerElement elem) { return SSet_Insert((SSet)set, elem); }
inline ContainerElement SearchSet_RemoveElement(SearchSet set, SearchSetNode node) { return SSet_RemoveElement((SSet)set, node); }
inline void             SearchSet_DestroyElement(SearchSet set, SearchSetNode node) { SSet_DestroyElement((SSet)set, node); }
inline ForwardIterator  SearchSet_GetForwardIterator(SearchSet set) { return SSet_GetForwardIterator((SSet)set); }
inline int              SearchSet_GetSize(SearchSet set) { return SSet_GetSize((SSet)set); }
inline SearchSetNode    SearchSet_GetElement(SearchSet set, int index) { return SSet_GetElement((SSet)set, index); }
extern SearchSetNode    SearchSet_Search(SearchSet set, void* searchKey, SearchSetNode node);
extern ContainerElement SearchSet_SearchElement(SearchSet set, void* searchKey, SearchSetNode node);
extern void             SearchSet_Swap(SearchSet set1, SearchSet set2);
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
  HashMapNode       next;
  HashMapNode       prev;
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
