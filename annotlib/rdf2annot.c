
/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 2000.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * rdfparse.c : parses an annotation RDF schema and intializes
                the corresponding memory elements 
 *
 * Author: J. Kahan (W3C/INRIA)
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xmlparse.h"
#include "annotlib.h"

/********************** static variables ***********************/

/* temporary storage buffer */
static char **cdata_buff;
static int  cdata_buff_len;

/* set to true whenever we find an element that has a parseType attribute
set to literal. Everything that's parsed from that moment will be stored as
is in the buffer, until the end tag of the element */
static int literal;

AnnotMeta *annot; /* the current annotation */

/* a list of elements */
List *element_list;

/* a list of annotations */
List *annot_list;

/********************** common functions ***********************/

/* ------------------------------------------------------------
   RDFStrAllocCat
   dynamically concatenate a string into the buffer to which
   cdata_buff is pointing to.
   ------------------------------------------------------------*/
static void RDF_StrAllocCat (const char *txt, int txtlen)
{
  char *ptr;
  
  ptr =  (char *) realloc (*cdata_buff, 
			   (cdata_buff_len + txtlen + 20) * sizeof (char));
  *cdata_buff = ptr;
  ptr = ptr + cdata_buff_len;
  memcpy (ptr, txt, txtlen);
  cdata_buff_len += txtlen;
}

/* ------------------------------------------------------------
   ParseIdFragment
   Extracts the id (or in this case the thotlib labels)
   from a URL and reoves this info from the URL.
   ------------------------------------------------------------*/
static void ParseIdFragment (AnnotMeta *annot)
{
  CHAR_T *c, *d;
  
  c = ustrrchr (annot->source_url, TEXT('#'));
  if (c)
    {
      *c = WC_EOS;
      c += 4;
      d = c;
      while (*d)
	{
	  switch (*d) 
	    {
	    case TEXT('|'):
	      *d = ' ';
	      break;
	    case TEXT(')'):
	      *d = WC_EOS;
	      break;
	    }
	  d++;
	}
      usscanf (c, TEXT("%s %d %s %d"), annot->labf, (&annot->c1),
	      annot->labl, &(annot->cl));
    }
}

/********************** parser handlers ***********************/

/* ------------------------------------------------------------
   default_hndl
   Used for copying the unparsed string into the cdata_buffer.
   ------------------------------------------------------------*/
static void default_hndl(void *data, const char *s, int len)
{
  RDF_StrAllocCat (s, len);
}  /* End default_hndl */

/* ------------------------------------------------------------
   add_unparsed
   Used for copying the unparsed string into the cdata_buffer.
   ------------------------------------------------------------*/
static void add_unparsed (XML_Parser p)
{
  XML_SetDefaultHandler (p, default_hndl);
  XML_DefaultCurrent (p);
  XML_SetDefaultHandler (p, (XML_DefaultHandler) 0);
}

/* ------------------------------------------------------------
   start_hndl
   Called for each start tag. See the expat doc.
   ------------------------------------------------------------*/
static void start_hndl(void *data, const char *el, const char **attr)
{
  int set_literal = 0;
  char *char_p;

  if (!strcmp (el, "a:Annotation"))
    /* the start of a new annotation, we add it to the list */
    {
      annot =  AnnotMeta_new ();
      List_add (&annot_list, (void *) annot);
      if (attr[0] && !strcmp (attr[0], "about"))
	  annot->about = strdup ((char *) attr[1]);
    }
  else if (!strcmp (el, "xlink:href")) 
    {
      if (attr[0] && !strcmp (attr[0], "r:resource"))
	{
	  annot->source_url = strdup ((char *) attr[1]);
	  /* extract the "id" fragment from the URL */
	  ParseIdFragment (annot);
	} 
    }
  else if (!strcmp (el, "d:creator")) 
    cdata_buff = &(annot->author);
  else if (!strcmp (el, "d:date"))
    cdata_buff = &(annot->date);
  else if (!strcmp (el, "http:ContentType"))
    cdata_buff = &(annot->content_type);
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
      List_add (&element_list, (void *) char_p);
    }
  else if (literal)
    {
      /* copy the current input buffer to the cdata_buff structure */
      add_unparsed ((XML_Parser) data);
    }

  if (set_literal)
    literal = 1;

}  /* End of start_hndl */

/* ------------------------------------------------------------
   end_hndl
   See the expat doc.
   ------------------------------------------------------------*/
static void end_hndl(void *data, const char *el) 
{
  List_del (&element_list);

  if (! strcmp (el, "http:Body"))
    /* turn off the literal mode */
    literal = 0;

  if (literal)
    add_unparsed ((XML_Parser) data);
  else if (cdata_buff_len > 0)
    {
      /* add the final EOS char to the buffer and clean up the
	 cdata variable so that it's ready for the next element */
      (*cdata_buff)[cdata_buff_len] = '\0';
      cdata_buff = NULL;
      cdata_buff_len = 0;
    }

}  /* End of end_hndl */

/* ------------------------------------------------------------
   char_hndl
   See the expat doc.
   ------------------------------------------------------------*/
static void  char_hndl (void *data, const char *txt, int txtlen) 
{
   if (cdata_buff)
      RDF_StrAllocCat (txt, txtlen);

}  /* End char_hndl */


/********************** other functions ********************/

/* ------------------------------------------------------------
   handler_init
   Initializes the expat handlers.
   ------------------------------------------------------------*/
static void handler_init (XML_Parser p)
{
  XML_SetElementHandler(p, start_hndl, end_hndl);
  XML_SetCharacterDataHandler (p, char_hndl);
  XML_UseParserAsHandlerArg (p);
  literal = 0;
}

/********************** API entry point ********************/

/* ------------------------------------------------------------
   ParseRDFFile
   Initializes the expat handlers.
   ------------------------------------------------------------*/
List *RDF_parseFile (char *file_name, AnnotFileType type)
{
  /* the file input buffer */
  char buff[512];
  FILE *fp;
  XML_Parser p;
  ThotBool error;

  annot_list = NULL;
  error = FALSE;

  fp = fopen (file_name, "r");
  if (!fp)  /* annotation index file doesn't exist */
      return NULL;

  if (type == ANNOT_SINGLE)
    {
      /* we're parsing a single annotation. We create the element
	 where we'll store it (in a list on annotations, the elements
	 are created automatically) */
      annot =  AnnotMeta_new ();
      List_add (&annot_list, (void *) annot);
    }
  else if (type != ANNOT_LIST)
    {
      fclose (fp);
      return NULL;
    }

#ifdef USE_NS
  p = XML_ParserCreateNS (NULL, ':');
#else
  p = XML_ParserCreate (NULL);
#endif /* USE_NS */

  if (!p)
    {
      fprintf(stderr, "Couldn't allocate memory for parser\n");
      fclose (fp);
      AnnotList_free (annot_list);
      return NULL;
    }

  handler_init (p);

  /* start parsing */
 for (;;) {
    int done;
    int len;
    fgets(buff, sizeof(buff), fp);
    if (ferror(fp)) {
      fprintf (stderr, "Read error\n");
      error = TRUE;
      break;
    }
    done = feof(fp);
    if (done)
	len = 0;
    else
      len = strlen (buff);
    if (! XML_Parse(p, buff, len, done)) 
      {
 	fprintf (stderr, "Parse error at line %d:\n%s\n",
		XML_GetCurrentLineNumber(p),
		XML_ErrorString(XML_GetErrorCode(p)));
	error = TRUE;
	break;
      }
    if (done)
      break;
 }

 fclose (fp);

 if (error)
   {
     /* clear data structure */
     AnnotList_free (annot_list);
     annot_list = NULL;
   }
 
 /* output whatever we parsed */
 AnnotList_print (annot_list);

 return (annot_list);
}






