/*
 *  FILE   : mydictionary.c
 *  AUTHOR : Francesc Campoy Flores

 *  NOTES  : Defines a dictionary of pointers (void*) using a char* as a key
 *           The dicionary doesn't take care of freeing pointed elements when needed.

 */

#define THOT_EXPORT extern
#include "amaya.h"
#include "templates.h"
#include "mydictionary_f.h"

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
DicDictionary Dictionary_Create ()
{
	DicDictionary dic = (DicDictionary)TtaGetMemory (sizeof (sDictionary));
  memset (dic, 0, sizeof (sDictionary));
	dic->first = NULL;
	dic->iter = NULL;
	return dic;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void Dictionary_Clean (DicDictionary dic)
{
	Record rec = dic->first;
	Record old;
	while (rec)
    {
      free (rec->key);
      old = rec;
	  rec = rec->next;
      free (old);
    }
	dic->first = NULL;
	dic->iter = NULL;
  TtaFreeMemory (dic);
}

/*----------------------------------------------------------------------
  Returns the Record with the passed key or the Record which would be
  before if there is no Record with that key.
  If the element should be the first it returns NULL
  ----------------------------------------------------------------------*/
Record Dictionary_Find (DicDictionary dic, const char * key)
{
	Record rec = dic->first;
	Record precedent = NULL;
	int    cmp = -1;

  if (!key)
    return NULL;
	while (cmp < 0 && rec)
    {
      cmp = strcmp (rec->key,key);
      if (cmp < 0)
        {
          precedent = rec;
          rec = rec->next;
        }
    }

	if (cmp == 0)
		//Element found
		return rec;
	else
		//Element not found, it should be after the precedent.
		return precedent;
}

/*----------------------------------------------------------------------
  Looks up dic for the element identified by key.
  Returns :
  null !isFirst  : if the element key has not been found.
	null isFirst   : if the element key has been found but it is the first 
  in the linked list.
  !null !isFirst : if the element key has been found returns the previous element.
  ----------------------------------------------------------------------*/
Record Dictionary_FindPrevious (DicDictionary dic, const char * key, ThotBool *isFirst)
{
	Record rec = dic->first;
	Record precedent = NULL;
	int cmp = -1;
	
	while (cmp<0 && rec)
    {
      cmp = strcmp (rec->key,key);
      if (cmp < 0)
        {
          precedent = rec;
          rec = rec->next;
        }
    }

	if (cmp==0)
    {
      *isFirst = precedent == NULL;
      return precedent;
    }
	else
    { //Element not found
      *isFirst = FALSE;
      return NULL;
    }
}

/*----------------------------------------------------------------------
  Looks up dic for the element
  Returns :
  null !isFirst  : if the element key has not been found.
	null isFirst   : if the element key has been found but it is the first 
  in the linked list.
  !null !isFirst : if the element key has been found returns the previous element.
  ----------------------------------------------------------------------*/
Record Dictionary_FindPreviousElement (DicDictionary dic, const DicElement el, ThotBool *isFirst)
{
	Record rec        = dic->first;
	Record precedent  = NULL;
  ThotBool found    = FALSE;

  while (rec && !found)
    {
      found = rec->element == el;
      if (!found)
        {
          precedent = rec;
          rec = rec->next;
        }
    }
	
	if (found)
    {
      *isFirst = precedent == NULL;
      return precedent;
    }
	else
    { //Element not found
      *isFirst = FALSE;
      return NULL;
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
DicElement Dictionary_Add (DicDictionary dic, const char * key, const DicElement el)
{	
	Record     rec = Dictionary_Find (dic, key);
	Record     newRec;
	DicElement result = NULL;
	
	if (!rec)
    {
      //The element should be the first
      newRec = (Record) TtaGetMemory (sizeof (sRecord));
      memset (newRec, 0, sizeof (sRecord));
      newRec->key = (char *) TtaStrdup (key);
      newRec->element = el;
      newRec->next = dic->first;
      dic->first = newRec;
    }

	else if (strcmp (rec->key, key)==0)
    {
      //The element has been found
      result          = rec->element; //We return the old element
      rec->element    = el; //And use the new one
    }

	else
    { //The element should be insered just after rec
      newRec = (Record) TtaGetMemory (sizeof (sRecord));
      memset (newRec, 0, sizeof (sRecord));
      newRec->key = (char *) TtaStrdup (key);
      newRec->element = el;
      newRec->next = rec->next;
      rec->next = newRec;
    }

	return result;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
DicElement Dictionary_Remove (DicDictionary dic, const char * key)
{
	ThotBool isFirst;
	Record aux = NULL;
	DicElement result = NULL;

	Record rec = Dictionary_FindPrevious (dic, key, &isFirst);
	if (isFirst)
    {
      aux = dic->first;
      dic->first = aux->next;
    }
	else if (!rec)
    //The element is not in the dictionary
		return NULL; 
	else
    {
      aux = rec->next;
      rec->next = aux->next;
    }

	TtaFreeMemory (aux->key);
	result = aux->element;
	TtaFreeMemory (aux);

	return result;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
DicElement Dictionary_RemoveElement (DicDictionary dic, const DicElement el)
{
	ThotBool isFirst;
	Record aux = NULL;
	DicElement result = NULL;

	Record rec = Dictionary_FindPreviousElement (dic, el, &isFirst);
	if (isFirst)
    {
      aux = dic->first;
      dic->first = aux->next;
    }
	else if (!rec)
    //The element is not in the dictionary
		return NULL; 
	else
    {
      aux = rec->next;
      rec->next = aux->next;
    }

	TtaFreeMemory (aux->key);
	result = aux->element;
	TtaFreeMemory (aux);

	return result;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
DicElement Dictionary_Get (DicDictionary dic, const char * key)
{
  if (!key)
    return NULL;

	Record rec = Dictionary_Find (dic,key);
	
	if (!rec)
    return NULL;

	if (strcmp (rec->key,key) == 0)
		return rec->element;
	else
		return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void Dictionary_First (DicDictionary dic)
{
	dic->iter = dic->first;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void Dictionary_Next (DicDictionary dic)
{
	if (dic->iter)
		dic->iter = dic->iter->next;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool Dictionary_IsDone (const DicDictionary dic)
{
	return dic->iter == NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
char* Dictionary_CurrentKey (const DicDictionary dic)
{
	if (dic->iter)
		return dic->iter->key;
	else
		return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void* Dictionary_CurrentElement (const DicDictionary dic)
{
	if (dic->iter)
		return dic->iter->element;
	else
		return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool Dictionary_IsEmpty (const DicDictionary dic)
{
	return !dic->first;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool isEOSorWhiteSpace (const char c)
{
	return c == SPACE || c == '\t' || c == '\n' || c == EOS;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
DicDictionary Dictionary_CreateFromList (const char *list)
{
	char temp[128];
	int labelSize;

	DicDictionary dic = Dictionary_Create ();

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
      Dictionary_Add (dic,temp, NULL);
    }
	return dic;
}
