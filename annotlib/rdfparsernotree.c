
/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2000.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * rdfparse.c : parses an annotation RDF structure and intializes
                the corresponding memory elements 
 *
 * Author: J. Kahan (W3C/INRIA)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xmlparse.h"

int Eventcnt = 0;
char Buff[512];

char **cdata_buff;
int  cdata_buff_len;
int literal;

typedef struct _AnnotMeta {
  char *about;
  char *source_url;
  char *date;
  char *creator;
  char *content_type;
  char *content_length;
  char *body_url;
  char *body;
} AnnotMeta;

typedef struct _List {
  void *object;
  struct _List *next;
} List;

/* @@ all of these variables should be passed in the parser's user data
   context */
List *element_list;
List *annot_list;
AnnotMeta *annot; /* the current annotation */

void add_cdata_buff (const char *txt, int txtlen)
{
  char *ptr;
  
  ptr =  (char *) realloc (*cdata_buff, 
			   (cdata_buff_len + txtlen + 20) * sizeof (char));
  *cdata_buff = ptr;
  ptr = ptr + cdata_buff_len;
  memcpy (ptr, txt, txtlen);
  cdata_buff_len += txtlen;
}

void add_start_tag_attr_to_buff (const char *el, const char **attr)
{
 add_cdata_buff ("<", 1);
 add_cdata_buff (el, strlen (el));
 if (*attr)
   add_cdata_buff (" ", 1);
 while (*attr) 
   {
     /* add the attribute name */
     add_cdata_buff (*attr, strlen (*attr));
     attr++;
     /* add the attribute value */
     if (*attr)
       {
	 add_cdata_buff ("=\"", 2);
	 add_cdata_buff (*attr, strlen (*attr));
	 add_cdata_buff ("\"", 1);
       }
     attr++;
   }
 add_cdata_buff (">", 1);
}

void add_end_tag_to_buff (const char *el)
{
 add_cdata_buff ("</", 2);
 add_cdata_buff (el, strlen (el));
 add_cdata_buff (">", 1);
}

void add_element (List **me, char *object)
{
  List *new;

  new = (List *) malloc (sizeof (List));
  new->object = object;
  if (!*me)
      new->next = NULL;
  else
      new->next = *me;
  *me = new;
}

void del_element (List **me)
{
  List *ptr;

  if (*me)
    {
      ptr = (List *) (*me)->next;
      free (*me);
      *me = ptr;
    }
}

AnnotMeta *new_annotMeta (void)
{
  AnnotMeta *new;

  new = (AnnotMeta *) malloc (sizeof (AnnotMeta));
  if (new)
    memset (new, 0, sizeof (AnnotMeta));
  return new;
}

void start_hndl(void *data, const char *el, const char **attr)
{
  AnnotMeta *new;
  int set_literal = 0;
  char *char_p;

  if (!strcmp (el, "a:Annotation"))
    /* the start of a new annotation, we add it to the list */
    {
      annot =  new_annotMeta ();
      add_element (&annot_list, (void *) annot);
      if (attr[0] && !strcmp (attr[0], "about"))
	  annot->about = strdup ((char *) attr[1]);
    }
  else if (!strcmp (el, "xlink:href")) 
    {
      if (attr[0] && !strcmp (attr[0], "r:resource"))
	  annot->source_url = strdup ((char *) attr[1]);
    } 
  else if (!strcmp (el, "d:creator")) 
    cdata_buff = &(annot->creator);
  else if (!strcmp (el, "d:date"))
    cdata_buff = &(annot->date);
  else if (!strcmp (el, "http:ContentType"))
    cdata_buff = &(annot->content_type);
  else if (!strcmp (el, "http:ContentLength"))
    cdata_buff = &(annot->content_length);
  else if (!strcmp (el, "http:ContentLength"))
    cdata_buff = &(annot->content_length);
  else if (!strcmp (el, "a:body"))
    {
      if (attr[0] && !strcmp (attr[0], "r:resource"))
	annot->body_url = strdup ((char *) attr[1]);
    }
  else if (!strcmp (el, "http:Body"))
    {
      cdata_buff = &(annot->body);
      set_literal = 1;
    }
  else if (!literal && !element_list)
    cdata_buff = NULL;

  if (!literal && cdata_buff)
    {
      *cdata_buff = NULL;
      cdata_buff_len = 0;
      char_p = strdup (el);
      add_element (&element_list, (void *) char_p);
    }
  else if (literal)
      add_start_tag_attr_to_buff (el, attr);

  if (set_literal)
    literal = 1;
}  /* End of start_hndl */

void end_hndl(void *data, const char *el) 
{
  del_element (&element_list);

  if (! strcmp (el, "http:Body"))
    literal = 0;

  if (literal)
    {
      add_end_tag_to_buff (el);
    }
  else if (cdata_buff_len > 0)
    {
      (*cdata_buff)[cdata_buff_len] = '\0';
      cdata_buff = NULL;
      cdata_buff_len = 0;
    }

}  /* End of end_hndl */

void  char_hndl (void *data, const char *txt, int txtlen) 
{
   if (cdata_buff)
      add_cdata_buff (txt, txtlen);

}  /* End char_hndl */


void handler_init (XML_Parser p)
{
  XML_SetElementHandler(p, start_hndl, end_hndl);
  XML_SetCharacterDataHandler (p, char_hndl);
  literal = 0;
}


void print_annot_meta (List *annot_list)
{
  AnnotMeta *annot;
  List *annot_ptr;

  annot_ptr = annot_list;
  while (annot_ptr)
    {
      annot = (AnnotMeta *) annot_ptr->object;
      printf("\n=====annotation meta data =========\n");  
      if (annot->about)
	printf ("annot about URL = %s\n", annot->about);
      if (annot->source_url)
	printf ("annot source URL = %s\n", annot->source_url);
      if (annot->creator) 
	printf ("creator is = %s\n", annot->creator);
      if (annot->content_type)
	printf ("content_type is = %s\n", annot->content_type);
      if (annot->content_length) 
	printf ("content_length is = %s\n", annot->content_length);
      if (annot->body_url)
	printf ("body url is = %s\n", annot->body_url);
      if (annot->body)
	  printf ("======= body =============\n%s", annot->body);
      printf ("=========================\n");
      annot_ptr = annot_ptr->next;
    }
  printf ("\n");
}

void free_annot_meta (List *annot_list)
{
  AnnotMeta *annot;
  List *list_ptr, *next;

  list_ptr = annot_list;
  while (list_ptr)
    {
      annot = (AnnotMeta *) list_ptr->object;
      if (annot->about) 
	free (annot->about);
      if (annot->source_url) 
	free (annot->source_url);
      if (annot->creator) 
	free (annot->creator);
      if (annot->content_type) 
	free (annot->content_type);
      if (annot->content_length) 
	free (annot->content_length);
      if (annot->body_url) 
	free (annot->body_url);
      if (annot->body) 
	free (annot->body);
      next = list_ptr->next;
      free (list_ptr);
      list_ptr = next;
    }
}

int main (int argc, char *argv[])
{
  XML_Parser p;

  if (argc <2 || (strcmp (argv[1], "-single")
      && strcmp (argv[1], "-list")))
    {
      fprintf (stderr, "Usage: %s [-single || -list]\n", argv[0]);
      exit (-1);
    }
  else if (!strcmp (argv[1], "-single"))
    {
      /* we're parsing a single annotation. We create the element
	 where we'll store it (in a list on annotations, the elements
	 are created automatically) */
      annot =  new_annotMeta ();
      add_element (&annot_list, (void *) annot);
    } 

  /*
    p = XML_ParserCreateNS (NULL, ':');
  */

  p = XML_ParserCreate (NULL);
  if (!p)
    {
      fprintf(stderr, "Couldn't allocate memory for parser\n");
      exit(-1);
    }

  handler_init (p);

  /* start parsing */

 for (;;) {
    int done;
    int len;
    fgets(Buff, sizeof(Buff), stdin);
    len = strlen(Buff);
    if (ferror(stdin)) {
      fprintf(stderr, "Read error\n");
      exit(-1);
    }
    done = feof(stdin);
    if (done)
	len = 0;
    if (! XML_Parse(p, Buff, len, done)) {
      fprintf(stderr, "Parse error at line %d:\n%s\n",
              XML_GetCurrentLineNumber(p),
              XML_ErrorString(XML_GetErrorCode(p)));
      exit(-1);
    }

    if (done)
      break;
  }

 print_annot_meta (annot_list);
 free_annot_meta (annot_list);

 exit (0);
}








