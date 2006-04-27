/*
 *  FILE   : mydictionary.h
 *  AUTHOR : Francesc Campoy Flores
 *  NOTES  : Defines a dictionary of pointers (void*) using a char* as a key
 *           The dicionary doesn't take care of freeing pointed elements when needed.
 */

#ifndef _DICTIONARY_H_

  typedef char* DicKey;
  typedef void* DicElement;

  struct sDictionary;
  typedef struct sDictionary* DicDictionary;


/*----------------------------------------------------------------------
  Creates an empty dictionary and returns it.
  ----------------------------------------------------------------------*/
  DicDictionary		CreateDictionary();

/*----------------------------------------------------------------------
  Removes all the elements in the dictionary.
  Warning : No element wil be freed.
  ----------------------------------------------------------------------*/
  void				CleanDictionary(DicDictionary dic);
  
/*----------------------------------------------------------------------
  Adds an element el with a key key to the dictionary dic. If an element
  existed already for the given key the old element is returned in order
  to be freed if necessary.  
  ----------------------------------------------------------------------*/
  DicElement		Add(DicDictionary dic, DicKey key, DicElement el);

/*----------------------------------------------------------------------
  Adds an element el with a key key to the dictionary dic. If an element
  existed already for the given key the old element is returned in order
  to be freed if necessary.  
  ----------------------------------------------------------------------*/
  DicElement		Remove(DicDictionary dic, DicKey key);

/*----------------------------------------------------------------------
  Returns an element by its key or NULL if the element doesn't exist.
  ----------------------------------------------------------------------*/
  DicElement		Get(DicDictionary dic, DicKey key);

/*----------------------------------------------------------------------
  Returns true if there is no element in the dictionary.
  ----------------------------------------------------------------------*/
  bool				isEmpty(DicDictionary dic);

/*----------------------------------------------------------------------
  Dictionary Iterator
  ----------------------------------------------------------------------*/
  void              First			(DicDictionary dic);
  void				Next			(DicDictionary dic);
  bool				IsDone			(DicDictionary dic);
  char*				CurrentKey		(DicDictionary dic);
  void*				CurrentElement	(DicDictionary dic);

  /*----------------------------------------------------------------------
  Returns true if the character c is a white space or '\0'.
  ----------------------------------------------------------------------*/
  inline bool isEOSorWhiteSpace(char c);

/*----------------------------------------------------------------------
  Returns a dictionary where all the keys are the tokens from a string
  where each item is separated by white spaces.
  All the elements are null.
  ----------------------------------------------------------------------*/
  DicDictionary CreateDictionaryFromList(char *list);


#endif  /* _DICTIONARY_H_ */
