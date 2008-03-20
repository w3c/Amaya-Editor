/*
 *
 *  COPYRIGHT INRIA and W3C, 1996-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * Authors: Emilien Kia
 *
 */
#include "thot_sys.h"
#include "containers.h"
#include "application.h"

#define HASHMAP_DEFAULT_NODE_NUMBER 32

/*------------------------------------------------------------------------------
 * Generic container
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Return the element of a node.
 * @param node Node from which get the element.
 * @return Element of the node.
 ----------------------------------------------------------------------------*/
ContainerElement ContainerNode_GetElement(ContainerNode node)
{
  return node->elem;
}

/*------------------------------------------------------------------------------
 * Delete an element from a container node.
 * @param container Container of the element
 * @param node Node from which element must be delete.
 ----------------------------------------------------------------------------*/
void Container_DeleteElement(Container container, ContainerNode node)
{
  if (container && node && node->elem && container->destroyElement)
    container->destroyElement(node);
}

/*------------------------------------------------------------------------------
 * Delete a node from a container.
 * @param container Container of the node
 * @param node Node to delete.
 ----------------------------------------------------------------------------*/
void Container_DeleteNode(Container container, ContainerNode node)
{
  if (node)
  {
    if (container && node->elem && container->destroyElement)
      container->destroyElement(node);
    TtaFreeMemory(node);
  }
}

/*------------------------------------------------------------------------------
 * Generic forward iterator
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Create a forward iterator.
 * You can use TtaFreeMemory to delete it.
 * @param container Container on which iterate.
 * @param getFirst GetFirst function address.
 * @param getNext GetNext function address.
 ----------------------------------------------------------------------------*/
ForwardIterator ForwardIterator_Create (Container container,
                    ForwardIterator_GetFirstFunction getFirst,
                    ForwardIterator_GetNextFunction  getNext)
{
  ForwardIterator iter;

  iter = (ForwardIterator) TtaGetMemory (sizeof(sForwardIterator));
  memset (iter, 0, sizeof(sForwardIterator));
  iter->container = container;
  iter->getFirst = getFirst;
  iter->getNext = getNext;
  return iter;
}

/*------------------------------------------------------------------------------
 * Get the first element of an iterator.
 * @param iter Iterator
 * @return First node or NULL if empty.
 ----------------------------------------------------------------------------*/
ContainerNode ForwardIterator_GetFirst (ForwardIterator iter)
{
  if (iter)
    {
      iter->currentNode = iter->getFirst(iter);
      return iter->currentNode;
    }
  else
    return NULL;
}

/*------------------------------------------------------------------------------
 * Get the next item in an iterator.
 * @param iter Iterator.
 * @return next node or NULL if no more object.
 ----------------------------------------------------------------------------*/
ContainerNode ForwardIterator_GetNext (ForwardIterator iter)
{
  if (iter)
    {
      iter->currentNode = iter->getNext(iter);
      return iter->currentNode;
    }
  else
    return NULL;
}

/*------------------------------------------------------------------------------
 * Count the number of element in the iterator.
 ----------------------------------------------------------------------------*/
long ForwardIterator_GetCount (ForwardIterator iter)
{
  long l = 0;
  if (iter)
    {
      ContainerNode node = ForwardIterator_GetFirst(iter);
      for (node = ForwardIterator_GetFirst(iter); node;
            node = ForwardIterator_GetNext(iter))
          l++;
    }
  return l;
}

/*------------------------------------------------------------------------------
 * Double linked list
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Create a new list of elements.
 * @return Empty list of element. 
 ----------------------------------------------------------------------------*/
DLList DLList_Create()
{
  DLList list = (DLList)TtaGetMemory (sizeof(sDLList));
  memset (list, 0, sizeof(sDLList));
  return list;
}

/*------------------------------------------------------------------------------
 * Empty a list of element.
 * All elements are freed.
 * @param list List to empty.
 ----------------------------------------------------------------------------*/
void DLList_Empty(DLList list)
{
  DLListNode node;

  if (list == NULL)
    return;

  node = list->first;
  while (node)
    {
      DLListNode next = (DLListNode) node->next;
      if (list->destroyElement)
        list->destroyElement (node->elem);
      node->elem = NULL;
      TtaFreeMemory (node);
      node = next;
    }
  list->first = list->last = NULL;  
}

/*------------------------------------------------------------------------------
 * Destroy a list of element
 * All element are freed.
 * @param list List to destroy.
 ----------------------------------------------------------------------------*/
void DLList_Destroy (DLList list)
{
  if (list)
    {
      DLList_Empty (list);
      TtaFreeMemory( list);
    }
}

/*------------------------------------------------------------------------------
 * Test if a list is empty.
 ----------------------------------------------------------------------------*/
ThotBool DLList_IsEmpty (DLList list)
{
  return list == NULL || list->first == NULL;
}

/*------------------------------------------------------------------------------
 * Append an element at the end of the list
 * @param list List at which the new element is append.
 * @param elem Element to append.
 ----------------------------------------------------------------------------*/
DLListNode DLList_Append (DLList list, ContainerElement elem)
{
  DLListNode node = NULL;
  if (list)
    {
      node = (DLListNode) TtaGetMemory (sizeof (sDLListNode));
      memset (node, 0, sizeof(sDLListNode));
      node->elem = elem;
      
      node->prev = list->last;
      if (!list->first)
        list->first = (DLListNode) node;
      if (list->last)
        list->last->next = node;
      list->last = (DLListNode) node;
    }
  return node;
}

/*------------------------------------------------------------------------------
 * Prepend an element at the begining of the list
 * @param list List at which the new element is append.
 * @param elem Element to append.
 ----------------------------------------------------------------------------*/
DLListNode DLList_Prepend (DLList list, ContainerElement elem)
{
  DLListNode node = (DLListNode) TtaGetMemory (sizeof(sDLListNode));

  memset (node, 0, sizeof(sDLListNode));
  node->elem = elem;
  if (list)
    {
      node->next = list->first;
      list->first = (DLListNode) node;
      if (list->last == NULL)
        list->last = (DLListNode) node;
      ((DLListNode)node->next)->prev = node;
    }
  return node;
}

/*------------------------------------------------------------------------------
 * Insert an element after an element in a list
 * @param list List at which the new element is append.
 * @param before Node after which the element is to be inserted.
 * @param elem Element to append.
 ----------------------------------------------------------------------------*/
DLListNode DLList_InsertAfter (DLList list, DLListNode before, ContainerElement elem)
{
  DLListNode node = (DLListNode)TtaGetMemory (sizeof(sDLListNode));

  memset (node, 0, sizeof(sDLListNode));
  node->elem = elem;
  if (before)
    {
      node->prev = before;
      node->next = before->next;
      before->next = node;
    }

  if (node->next)
    ((DLListNode)node->next)->prev = node;
  else
    list->last = node;
  
  return node;
}

/*------------------------------------------------------------------------------
 * Insert an element before an element in a list
 * @param list List at which the new element is append.
 * @param after Node before which the element is to be inserted.
 * @param elem Element to append.
 ----------------------------------------------------------------------------*/
DLListNode DLList_InsertBefore (DLList list, DLListNode after, ContainerElement elem)
{
  DLListNode node = (DLListNode)TtaGetMemory (sizeof(sDLListNode));

  memset (node, 0, sizeof(sDLListNode));
  node->elem = elem;
  if (after)
    {
      node->next = after;
      node->prev = after->prev;
      after->prev = node;
    }

  if (node->prev)
    ((DLListNode)node->prev)->next = node;
  else
    list->first = node;
  
  return node;
}

/*------------------------------------------------------------------------------
 * Remove an element from a list
 * @param list List from which remove the element.
 * @param node Node to remove.
 * @return Element
 ----------------------------------------------------------------------------*/
ContainerElement DLList_RemoveElement (DLList list, DLListNode node)
{
  ContainerElement elem;

  if (node == NULL)
    return NULL;

  elem = node->elem;
  if (node == list->first)
    list->first = node->next;
  else
    node->prev->next = node->next;

  if (node == list->last)
    list->last = node->prev;
  else
    node->next->prev = node->prev;

  TtaFreeMemory(node);
  return elem;
}

/*------------------------------------------------------------------------------
 * Destroy an element.
 * Remove an element and destroy it.
 * @param list List from which remove the element.
 * @param node Node to remove.
 ----------------------------------------------------------------------------*/
void  DLList_DestroyElement (DLList list, DLListNode node)
{
  ContainerElement elem = DLList_RemoveElement(list, node);
  if (elem && list->destroyElement)
    list->destroyElement(elem);
}


/*------------------------------------------------------------------------------
 ----------------------------------------------------------------------------*/
static DLListNode DLListIterator_GetFirst (ForwardIterator iter)
{
  DLList list = (DLList)iter->container;
  if (list)
    return list->first;
  else
    return NULL;
}

/*------------------------------------------------------------------------------
 ----------------------------------------------------------------------------*/
static DLListNode DLListIterator_GetNext(ForwardIterator iter)
{
  DLListNode node = (DLListNode) iter->currentNode;
  if (node)
    return node->next;
  else
    return NULL;
}

/*------------------------------------------------------------------------------
 * Create a DLList forward iterator.
 * @param list List on which iterate.
 * @return the iterator.
 ----------------------------------------------------------------------------*/
ForwardIterator DLList_GetForwardIterator(DLList list)
{
  return ForwardIterator_Create((Container)list,
          (ForwardIterator_GetFirstFunction)DLListIterator_GetFirst,
          (ForwardIterator_GetNextFunction)DLListIterator_GetNext);
}


/*------------------------------------------------------------------------------
 * Swap two nodes.
 ----------------------------------------------------------------------------*/
void DLList_SwapContent(DLList list, DLListNode node1, DLListNode node2)
{
  ContainerElement elem;
  elem = node1->elem;
  node1->elem = node2->elem;
  node2->elem = elem;
}

/*------------------------------------------------------------------------------
 * Retrieve the size of the list.
 * \note : o(n) function.
 ----------------------------------------------------------------------------*/
int DLList_GetSize(DLList list)
{
  int        i = 0;
  DLListNode node;
  if(list && list->first!=0)
    {
      node = list->first;
      while(node)
        {
          i++;
          node = node->next;
        }
    }
  return i;
}

/*------------------------------------------------------------------------------
 * Retrieve an element from its index.
 * \note : o(n) function.
 ----------------------------------------------------------------------------*/
DLListNode DLList_GetElement(DLList list, int index)
{
  int        i = 0;
  DLListNode node = NULL;
  if(list && list->first!=0)
    {
      node = list->first;
      while(node)
        {
          if(i==index)
            break;
          i++;
          node = node->next;
        }
    }
  return node;  
}

/*------------------------------------------------------------------------------
 * Sort a list content.
 * @note use a simple bubble sort algorithm (o(n^2)).
 * @todo Implement another more efficient alorithm (in o(nlogn)) 
 * @param compare, comparaison function.
 ----------------------------------------------------------------------------*/
void DLList_Sort(DLList list, Container_CompareFunction compare)
{
  DLListNode node1, node2;
  node1 = list->first;
  while (node1!=NULL)
  {
    node2 = node1->next;
    while (node2!=NULL)
    {
      if (compare(node1->elem, node2->elem)>0)
      {
        DLList_SwapContent(list, node1, node2);
      }
      node2 = node2->next;
    }
    node1 = node1->next;    
  }
}

/*------------------------------------------------------------------------------
 * Return a new list with references to the param list elements.
 * Reference elements are not destroy if the list is destroyed.
 * @param srcList Source list from which reference elements.
 * @param compare, If not NULL, use it to sort references.
 * @note Use an insert sort algorithm.
 ----------------------------------------------------------------------------*/
DLList DLList_GetRefList(DLList srcList, Container_CompareFunction compare)
{
  DLList list = DLList_Create();
  DLListNode node = srcList->first;
  if (compare == NULL)
  {
    while (node)
    {
      DLList_Append(list, node->elem);
      node = node->next;
    }
  }
  else
  {
    if (node)
    {
      DLList_Append(list, node->elem);
      node = node->next;
    }
    while (node)
    {
      DLListNode test = list->first;
      while (test && compare(test->elem, node->elem))
        test = test->next;
      if (test)
        DLList_InsertBefore(list, test, node->elem);
      else
        DLList_Append(list, node->elem);
      node = node->next;
    }
  }  
  return list;
}

/*------------------------------------------------------------------------------
 * Return a new list with references to the param iterator elements.
 * Reference elements are not destroy if the list is destroyed.
 * @param iter Iterator from which reference elements.
 * @param compare, If not NULL, use it to sort references.
 * @note Use an insert sort algorithm.
 ----------------------------------------------------------------------------*/
DLList DLList_GetRefListFromIterator(ForwardIterator iter, Container_CompareFunction compare)
{
  DLList list = DLList_Create();
  ContainerNode node = ForwardIterator_GetFirst(iter);
  if (compare==NULL)
  {
    while (node)
    {
      DLList_Append(list, node->elem);
      node = ForwardIterator_GetNext(iter);
    }
  }
  else
  {
    if (node)
    {
      DLList_Append(list, node->elem);
      node = ForwardIterator_GetNext(iter);
    }
    while (node)
    {
      DLListNode test = list->first;
      while (test && compare(test->elem, node->elem))
        test = test->next;
      if (test)
        DLList_InsertBefore(list, test, node->elem);
      else
        DLList_Append(list, node->elem);
      node = ForwardIterator_GetNext(iter);
    }
  }  
  return list; 
}


/*----------------------------------------------------------------------
  Simple Hash map "destroy key" function.
  -----------------------------------------------------------------------*/
static void HashMap_SimpleDestroyKey(HashMapKey key)
{
  /* Do nothing.*/
}

/*----------------------------------------------------------------------
  Simple Hash map "compare key" function.
  -----------------------------------------------------------------------*/
static int HashMap_SimpleCompareKey(HashMapKey key1, HashMapKey key2)
{
  return (int)((intptr_t)key1-(intptr_t)key2);
}
  
/*----------------------------------------------------------------------
 * Allocate a hash map key node (list of node).
  -----------------------------------------------------------------------*/
static HashMapKeyNode HashMap_CreateHashMapKeyNode(HashMap map)
{
  HashMapKeyNode node = (HashMapKeyNode)DLList_Create();
  node->destroyElement = map->destroyElement;
  return node;
}
 
/*----------------------------------------------------------------------
  Create a new hash map.
  Nodes Number of hashing slot, 0 or negative to default node number.
  return Empty hash map. 
  -----------------------------------------------------------------------*/
HashMap HashMap_Create(Container_DestroyElementFunction destroy,
                       HashMap_HashFunction hash,
                       int nbNodes)
{
  HashMap map = (HashMap)TtaGetMemory (sizeof(sHashMap));
  
  memset (map, 0, sizeof(sHashMap));
  if (nbNodes < 1)   /* 0 or negative : Default node number.*/
    nbNodes = 32; /* Default node number.*/
  
  map->destroyElement = destroy;
  map->destroyKey     = HashMap_SimpleDestroyKey;
  map->compare        = HashMap_SimpleCompareKey;
  map->hash           = hash;
  map->nbNodes        = nbNodes;
  map->nodes = (HashMapKeyNode*) TtaGetMemory(sizeof(HashMapKeyNode)*map->nbNodes);
  memset(map->nodes, 0, sizeof(HashMapKeyNode)*map->nbNodes);
  return map;
}

/*----------------------------------------------------------------------
 * Empty an hash map.
 * All element are freed.
 * @param map Map to empty.
  -----------------------------------------------------------------------*/
void HashMap_Empty(HashMap map)
{
  int i;
  for (i=0; i<map->nbNodes; i++)
  {
    if (map->nodes[i] !=NULL)
    {
      DLList_Destroy ((DLList)map->nodes[i]);
      map->nodes[i] = NULL;
    }
  }  
}

/*----------------------------------------------------------------------
 * Destroy an hash map.
 * All element are freed.
 * @param map Map to destroy.
  -----------------------------------------------------------------------*/
void HashMap_Destroy(HashMap map)
{
  if (map)
  {
    HashMap_Empty (map);
    TtaFreeMemory (map->nodes);
    TtaFreeMemory (map);
  }
}

/*----------------------------------------------------------------------
 * Test if the map is empty.
  -----------------------------------------------------------------------*/
ThotBool HashMap_IsEmpty(HashMap map)
{
  int i;
  for (i = 0; i<map->nbNodes; i++)
  {
    if (map->nodes[i] != NULL)
    {
      return FALSE;
    }
  }
  return TRUE;
}

/*----------------------------------------------------------------------
 * Get the internal list for a specified key.
 * Allocate it if not already done.
  -----------------------------------------------------------------------*/
static HashMapKeyNode HashMap_GetHashMapKeyNode(HashMap map, HashMapKey key, ThotBool create)
{
  int keyval;
  if(map==NULL || key==NULL)
    return NULL;
  keyval = (map->hash(key))%map->nbNodes;
  if (map->nodes[keyval]==NULL){
    if (create)
      map->nodes[keyval] = HashMap_CreateHashMapKeyNode(map);
  }
  return map->nodes[keyval];
}

/*----------------------------------------------------------------------
 * Set an element in the hash map, the old element is returned.
 * The element and the key are stored in the map.
 * @param key Key of the element.
 * @param elem Element.
 * @return The old element, NULL if none.
  -----------------------------------------------------------------------*/
ContainerElement HashMap_Set(HashMap map, HashMapKey key, ContainerElement elem)
{
  HashMapKeyNode keynode = HashMap_GetHashMapKeyNode(map, key, TRUE);
  HashMapNode node = (HashMapNode)keynode->first;
  ContainerElement old = NULL;
  while (node && map->compare(key, node->key) != 0)
    node = (HashMapNode)node->next;

  if (node)
  {
    old = node->elem;
    node->elem = elem;
    // free the key duplication.
    TtaFreeMemory (key);
  }
  else
  {
    node = (HashMapNode) TtaGetMemory (sizeof(sHashMapNode));
    memset (node, 0, sizeof(sHashMapNode));
    node->elem = elem;
    node->key  = key;
    node->prev = NULL;
    if (keynode->first == NULL)
    {
      keynode->first = keynode->last = (DLListNode)node;
      node->next = NULL;
    }
    else
    {
      keynode->first->prev = (DLListNode)node;
      node->next = (HashMapNode)keynode->first;
      keynode->first = (DLListNode)node; 
    }
  }
  return (ContainerElement)old;
}

/*----------------------------------------------------------------------
 * Find an element of the map.
 * Return the element node or NULL if not found.
  -----------------------------------------------------------------------*/
HashMapNode HashMap_Find(HashMap map, const HashMapKey key)
{
  HashMapKeyNode keynode = HashMap_GetHashMapKeyNode(map, key, FALSE);
  if (keynode!=NULL)
  {
    HashMapNode node = (HashMapNode)keynode->first;
    while (node!=NULL)
    {
      if (map->compare(key, node->key)==0)
        return node;
      node = node->next;
    }
  }
  return NULL;
}

/*----------------------------------------------------------------------
 * Get an element from the hash map.
 * Only the new element is stored in the map.
 * @param key Key of the element
 * @return The searched element or NULL if not found.
  -----------------------------------------------------------------------*/
ContainerElement HashMap_Get(HashMap map, const HashMapKey key)
{
  HashMapNode node = HashMap_Find(map, key);
  if (node!=NULL)
    return node->elem;
  return NULL;
}

/*----------------------------------------------------------------------
 * Remove an element from the hash map and return it.
 * The stored key is destroyed from the map.
  -----------------------------------------------------------------------*/
ContainerElement HashMap_Remove(HashMap map, HashMapKey key)
{
  HashMapKeyNode keynode = HashMap_GetHashMapKeyNode(map, key, FALSE);  
  if (keynode != NULL)
  {
    HashMapNode node = (HashMapNode)keynode->first;
    while (node != NULL)
    {
      if (map->compare(key, node->key)==0)
      {
        if (node->key)
          map->destroyKey(node->key);
        return DLList_RemoveElement((DLList)keynode, (DLListNode) node); 
      }
      node = node->next;
    }
  }
  return NULL;  
}

/*----------------------------------------------------------------------
 * Destroy an element from the hash map.
 * Both stored element and key are destroyed.
  -----------------------------------------------------------------------*/
void HashMap_DestroyElement(HashMap map, HashMapKey key)
{
  HashMapKeyNode keynode = HashMap_GetHashMapKeyNode(map, key, FALSE);
  if (keynode!=NULL)
  {
    HashMapNode node = (HashMapNode)keynode->first;
    while (node != NULL)
    {
      if (map->compare(key, node->key)==0)
      {
        if (node->key)
          map->destroyKey(node->key);
        DLList_DestroyElement((DLList)keynode, (DLListNode) node);
        return;
      }
      node = node->next;
    }
  }
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
static HashMapNode HashMapIterator_GetFirst(ForwardIterator iter)
{
  HashMap map = (HashMap) iter->container;
  int i;
  for (i=0; i< map->nbNodes; i++)
  {
    if (map->nodes[i]!=NULL){
      if (map->nodes[i]->first!=NULL)
        return (HashMapNode)map->nodes[i]->first;
    }
  }
  return NULL;
}

/*----------------------------------------------------------------------
  -----------------------------------------------------------------------*/
static HashMapNode HashMapIterator_GetNext(ForwardIterator iter)
{
  HashMap map = (HashMap) iter->container;
  HashMapNode node = (HashMapNode) iter->currentNode;
  if (node->next!=NULL)
  {
    return node->next;
  }
  else
  {
    int i = (map->hash(node->key)%map->nbNodes)+1;
    for (; i< map->nbNodes; i++)
    {
      if (map->nodes[i] != NULL)
        {
          if (map->nodes[i]->first != NULL)
            return (HashMapNode)map->nodes[i]->first;
        }
    }
  }
  return NULL;
}

/*----------------------------------------------------------------------
 * Get a simple forward iterator on the content of the map.
 * This iterator simply list all the content, the iterator order is not sorted.
  -----------------------------------------------------------------------*/
ForwardIterator  HashMap_GetForwardIterator(HashMap map)
{
  if (map)
    return ForwardIterator_Create((Container)map,
            (ForwardIterator_GetFirstFunction)HashMapIterator_GetFirst,
            (ForwardIterator_GetNextFunction)HashMapIterator_GetNext);
  else
    return NULL;
}


/*----------------------------------------------------------------------
 * Swap the content of two HashMap.
 * Usefull to move the content of a map (funtion param) to an empty map
 * and to clean the first one.
  -----------------------------------------------------------------------*/
void HashMap_SwapContents(HashMap map1, HashMap map2)
{
  _sHashMap temp;
  memcpy(&temp, map2, sizeof(_sHashMap));
  memcpy(map2, map1, sizeof(_sHashMap));
  memcpy(map1, &temp, sizeof(_sHashMap));
}

/*----------------------------------------------------------------------
 * Dump the content of a hashmap.
  -----------------------------------------------------------------------*/
void HashMap_Dump (HashMap map, ThotBool isKeyString)
{
  int            i;
  HashMapKeyNode keynode;
  HashMapNode    node;
  
  printf ("Dump of hashmap %p\n", map);
  if (map)
    {
      printf("  destroyElement : %p\n", map->destroyElement);
      printf("  destroyKey     : %p\n", map->destroyKey);
      printf("  hash           : %p\n", map->hash);
      printf("  compare        : %p\n", map->compare);
      printf("  nodes          : %p\n", map->nodes);
      printf("  nbNodes        : %d\n", map->nbNodes);
      for (i = 0; i<map->nbNodes; i++)
        {
          keynode = map->nodes[i];
          if (keynode)
            {
              printf("  [%02d] %p (%p -> %p)\n", i,  keynode, keynode->first, keynode->last);
              node = (HashMapNode) keynode->first;
              while (node!=NULL)
                {
                  if(isKeyString)
                    printf("      (%p>>) %p (>>%p) : %p %s  (%d) - %p\n", node->prev, 
                                        node, node->next,
                                        node->key, (char*)node->key, 
                                        map->hash(node->key), node->elem);
                  else
                    printf("      (%p>>) %p (>>%p) : %p (%d) - %p\n", node->prev, 
                                        node, node->next,
                                        node->key, map->hash(node->key), node->elem);
                  node = node->next;
                }
            }
        }
    }
}

/*----------------------------------------------------------------------
  Pointer hash map
  -----------------------------------------------------------------------*/
static intptr_t PointerHashMap_HashFunction (void* key)
{
  return (intptr_t) key; 
}

/*------------------------------------------------------------------------------
 ----------------------------------------------------------------------------*/
HashMap PointerHashMap_Create(Container_DestroyElementFunction destroy, int nbNodes)
{
  return HashMap_Create(destroy, (HashMap_HashFunction)PointerHashMap_HashFunction, nbNodes);
}

/*------------------------------------------------------------------------------
  String hash map
 ----------------------------------------------------------------------------*/
static int StringHashMap_HashFunction(char* key)
{
  int res = 0;
  if (key!=NULL)
    {
      while (*key!=0)
        {
          res += *key;
          key++;
        }
      res %= 256;
    }
  return res;
}

/*------------------------------------------------------------------------------
 ----------------------------------------------------------------------------*/
static int StringHashMap_CompareKey(HashMapKey key1, HashMapKey key2)
{
  return strcmp((const char*)key1, (const char*)key2);
}
  
/*------------------------------------------------------------------------------
  Create a string hash map
 ----------------------------------------------------------------------------*/
HashMap StringHashMap_Create (Container_DestroyElementFunction destroy,
                              ThotBool keyIsStored, int nbNodes)
{
  HashMap map = HashMap_Create (destroy,
                              (HashMap_HashFunction)StringHashMap_HashFunction, nbNodes);
  map->compare = StringHashMap_CompareKey;
  return map;
}

/*------------------------------------------------------------------------------
  Keyword hash map
 ----------------------------------------------------------------------------*/
static int KeywordHashMap_HashFunction(char* key)
{
  if (key)
   return (int)key[0];
  else
    return 0;
}
 
/*------------------------------------------------------------------------------
 ----------------------------------------------------------------------------*/
HashMap KeywordHashMap_Create(Container_DestroyElementFunction destroy,
                              ThotBool keyIsStored, int nbNodes)
{
  HashMap map = HashMap_Create(destroy,
                              (HashMap_HashFunction)KeywordHashMap_HashFunction, nbNodes);
  map->compare = StringHashMap_CompareKey;
  return map;
}


/*------------------------------------------------------------------------------
 ----------------------------------------------------------------------------*/
static ThotBool isEOSorWhiteSpace (const char c)
{
  return c == SPACE || c == '\t' || c == '\n' || c == EOS;
}

/*------------------------------------------------------------------------------
 ----------------------------------------------------------------------------*/
HashMap KeywordHashMap_CreateFromList(Container_DestroyElementFunction destroy,
                                      int nbNodes, const char *list)
{
  char temp[128];
  int labelSize;

  HashMap map = KeywordHashMap_Create(destroy, TRUE, nbNodes);
  if (list!=NULL && *list!=0)
    {
      for (unsigned int i=0; i<strlen (list); i++)
        {   
          labelSize = 0;
        
          while (isEOSorWhiteSpace (list[i]))
            ++i;
    
          while (!isEOSorWhiteSpace (list[i]))
            {
              temp[labelSize]=list[i];
              ++i;
              ++labelSize;
            }
    
          temp[labelSize] = EOS;
          HashMap_Set (map, TtaStrdup(temp), NULL);
        }
    }
  
  return map;
}
