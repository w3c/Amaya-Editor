/*
 *  FILE   : mydictionary.c
 *  AUTHOR : Francesc Campoy Flores
 */

#ifdef TEMPLATES

#define THOT_EXPORT extern
#include "amaya.h"
#include "mydictionary.h"


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
DicDictionary CreateDictionary()
{
	DicDictionary dic = (DicDictionary)malloc(sizeof(sDictionary));
	dic->first = NULL;
	dic->iter = NULL;
	return dic;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void CleanDictionary (DicDictionary dic)
{
	Record rec = dic->first;
	Record old;
	while (rec)
    {
      free(rec->key);
      old = rec;
      free(old);
      rec = rec->next;
    }
	dic->first = NULL;
	dic->iter = NULL;
}


/*----------------------------------------------------------------------
  Returns the Record with the passed key or the Record which would be
  before if there is no Record with that key.
  If the element should be the first it returns NULL
  ----------------------------------------------------------------------*/
Record Find (DicDictionary dic, char * key)
{
	Record rec = dic->first;
	Record precedent = NULL;
	int    cmp = -1;
	
	while (cmp < 0 && rec)
    {
      cmp = strcmp(rec->key,key);
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
Record FindPrevious (DicDictionary dic, char * key, ThotBool *isFirst)
{
	Record rec = dic->first;
	Record precedent = NULL;
	int cmp = -1;
	
	while(cmp<0 && rec)
    {
      cmp = strcmp(rec->key,key);
      if (cmp<0)
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
  ----------------------------------------------------------------------*/
DicElement Add (DicDictionary dic, char * key, DicElement el)
{	
	Record     rec = Find(dic, key);
	Record     newRec;
	DicElement result = NULL;
	
	if(!rec)
    {
      //The element should be the first
      newRec          = (Record) malloc(sizeof(sRecord));
      newRec->key     = (char *) strdup(key);
      newRec->element = el;
      newRec->next    = dic->first;
      dic->first = newRec;
    }

	else if (strcmp(rec->key, key)==0)
    {
      //The element has been found
      result          = rec->element; //We return the old element
      rec->element    = el; //And use the new one
    }

	else
    { //The element should be insered just after rec
      newRec          = (Record) malloc(sizeof(sRecord));
      newRec->key     = (char *) strdup(key);
      newRec->element = el;
      newRec->next    = rec->next;
      rec->next       = newRec;
    }

	return result;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
DicElement Remove (DicDictionary dic, char * key)
{
	ThotBool isFirst;
	Record aux = NULL;
	DicElement result = NULL;

	Record rec = FindPrevious(dic, key, &isFirst);
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

	free(aux->key);
	result = aux->element;
	free(aux);

	return result;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
DicElement Get (DicDictionary dic, char * key)
{
	Record rec = Find(dic,key);
	
	if (!rec)
    return NULL;

	if (strcmp(rec->key,key)==0)
		return rec->element;
	else
		return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void First (DicDictionary dic)
{
	dic->iter = dic->first;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void Next (DicDictionary dic)
{
	if(dic->iter)
		dic->iter = dic->iter->next;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool IsDone (DicDictionary dic)
{
	return dic->iter == NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
char* CurrentKey (DicDictionary dic)
{
	if(dic->iter)
		return dic->iter->key;
	else
		return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void* CurrentElement (DicDictionary dic)
{
	if(dic->iter)
		return dic->iter->element;
	else
		return NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool IsEmpty (DicDictionary dic)
{
	return !dic->first;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
ThotBool isEOSorWhiteSpace (char c)
{
	return c == SPACE || c == '\t' || c == '\n' || c == EOS;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
DicDictionary CreateDictionaryFromList (char *list)
{
	char temp[128];
	int labelSize;

	DicDictionary dic = CreateDictionary();

	for (unsigned int i=0; i<strlen(list); i++)
    {		
      labelSize = 0;
		
      while (isEOSorWhiteSpace(list[i]))
        ++i;

      while (!isEOSorWhiteSpace(list[i]))
        {
          temp[labelSize]=list[i];
          ++i;
          ++labelSize;
        }

      temp[labelSize] = EOS;
      Add(dic,temp, NULL);
    }

	return dic;
}
#endif /* TEMPLATES */
