/*
 *  FILE   : mydictionary.h
 *  AUTHOR : Francesc Campoy Flores
 *  NOTES  : Defines a dictionary of pointers (void*) using a char* as a key
 *           The dicionary doesn't take care of freeing pointed elements when needed.
 */

#ifndef _DICTIONARY_H_
#define _DICTIONARY_H_

typedef void  *DicElement;

//A record contains an element and its key.
struct sRecord;
typedef struct sRecord *Record;
struct sRecord
{
	char        *key;
  DicElement   element;
	Record       next;
};

//A dictionary contains a a sequence of Record
struct sDictionary;
typedef struct sDictionary* DicDictionary;
struct sDictionary
{
	Record first;
	Record iter;
};

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern DicDictionary CreateDictionary ( void );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern void CleanDictionary ( DicDictionary dic );

/*----------------------------------------------------------------------
  Returns the Record with the passed key or the Record which would be
  before if there is no Record with that key.
  If the element should be the first it returns NULL
  ----------------------------------------------------------------------*/
extern Record Find ( DicDictionary dic, const char * key );

/*----------------------------------------------------------------------
  Looks up dic for the element identified by key.
  Returns :
  null !isFirst  : if the element key has not been found.
	null isFirst   : if the element key has been found but it is the first 
  in the linked list.
  !null !isFirst : if the element key has been found returns the previous element.
  ----------------------------------------------------------------------*/
extern Record FindPrevious ( DicDictionary dic, const char * key, ThotBool *isFirst );

/*----------------------------------------------------------------------
  Looks up dic for the element identified by key.
  Returns :
  null !isFirst  : if the element key has not been found.
	null isFirst   : if the element key has been found but it is the first 
  in the linked list.
  !null !isFirst : if the element key has been found returns the previous element.
  ----------------------------------------------------------------------*/
extern Record FindPreviousElement ( DicDictionary dic, const DicElement el, ThotBool *isFirst );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern DicElement Add ( DicDictionary dic, const char * key, const DicElement el );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern DicElement Remove ( DicDictionary dic, const char * key);

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern DicElement RemoveElement ( DicDictionary dic, const DicElement el);


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern DicElement Get ( DicDictionary dic, const char * key);

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern void First ( DicDictionary dic );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern void Next ( DicDictionary dic );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern ThotBool IsDone ( const DicDictionary dic );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern char* CurrentKey ( const DicDictionary dic );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern void* CurrentElement ( const DicDictionary dic );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern ThotBool IsEmpty ( const DicDictionary dic );
 
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern ThotBool isEOSorWhiteSpace ( const char c );

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
extern DicDictionary CreateDictionaryFromList ( const char *list );

#endif  /* _DICTIONARY_H_ */
