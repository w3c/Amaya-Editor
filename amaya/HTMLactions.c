/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Amaya browser functions called form Thot and declared in HTML.A.
 * These functions concern links and other HTML general features.
 *
 * Authors: V. Quint, I. Vatton
 *
 */

/* Included headerfiles */
#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "amaya.h"
#include "css.h"
#include "trans.h"
#include "XLink.h"
#include "MathML.h"
#ifdef _SVG
#include "SVG.h"
#endif /* _SVG */


#ifdef TEMPLATES
#include "templates.h"
#include "containers.h"
#include "Elemlist.h"
#include "insertelem_f.h"
#include "Template.h"
#include "templates_f.h"
#include "templateDeclarations_f.h"
#endif /* TEMPLATES */

#ifdef DAV
#define WEBDAV_EXPORT extern
#include "davlib.h"
#include "davlib_f.h"
#include "davlibRequests_f.h"
#include "davlibUI_f.h"
#endif /* DAV */

#include "XML.h"
#include "MENUconf.h"
#include "anim_f.h"
#include "css_f.h"
#include "EDITORactions_f.h"
#include "init_f.h"
#include "AHTURLTools_f.h"
#include "EDITimage_f.h"
#include "EDITstyle_f.h"
#include "fetchXMLname_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLform_f.h"
#include "HTMLhistory_f.h"
#include "HTMLimage_f.h"
#include "HTMLsave_f.h"
#include "html2thot_f.h"
#include "Mathedit_f.h"
#include "selection.h"
#include "styleparser_f.h"
#include "trans_f.h"
#include "transparse_f.h"
#include "UIcss_f.h"
#include "Xml2thot_f.h"
#include "wxdialogapi_f.h"

#ifdef ANNOTATIONS
#include "annotlib.h"
#include "ANNOTevent_f.h"
#endif /* ANNOTATIONS */
#ifdef BOOKMARKS
#include "bookmarks.h"
#include "BMevent_f.h"
#endif /* BOOKMARKS */
#ifdef DAV
#include "davlib_f.h"
#endif /* DAV */

#ifdef _WX
#include "appdialogue_wx.h"
#include "paneltypes_wx.h"
#endif /* _WX */


/* Some prototypes */
static ThotBool     Do_follow_link (Element anchor, Element elSource,
                                   Attribute HrefAttr, Document doc);

/* the structure used for the Forward and Backward buttons history callbacks */
typedef struct _Do_follow_link_context {
  Document             doc;
  Element              anchor;
  Element              elSource;
  char                *sourceDocUrl;
  char                *utf8path;
} Do_follow_link_context;


/* info about the last element highlighted when synchronizing with the
   source view */
static ThotBool     Follow_exclusive = FALSE;
static ThotBool     Refresh_exclusive = FALSE;
static ThotBool     SelectionChanging = FALSE;
static ThotBool     GoToSection = FALSE;
/* last closed tab */
static char        *LastClosedTab = NULL;

/*----------------------------------------------------------------------
  CharNum_IN_Line
  calculate the firstchar of the line based on Char Scheme Value
  ----------------------------------------------------------------------*/
int CharNum_IN_Line (Document doc, int CharNum)
{
  Element             el;
  Element             child, prev;
  ElementType         elType;
  int                 i = 0, len, time = 0 ;
  int                 Char;
  int                 *len_of_child, *memory_of_child;
  int                 len_of_line, count_child,length_child;
  int                 k;

  el = TtaGetMainRoot (doc);
  elType = TtaGetElementType(el);
  elType.ElTypeNum = TextFile_EL_Line_;
  el = TtaSearchTypedElement (elType, SearchForward, el);
  Char = CharNum;
  child = TtaGetFirstChild(el);

  if (el)
    {
      i = Char;
      len = TtaGetElementVolume(el);
      len_of_line = TtaGetElementVolume(child);
      count_child = 1;

      /* Consider "Annotation Icon" if Annotation Icon is on the line */
      if (len != len_of_line)
        {
          //analysis the structure of target line including "Annotation Icon"
          while (len != len_of_line)
            {
              TtaNextSibling(&child);
              length_child = TtaGetElementVolume(child);
              len_of_line += length_child;
              count_child++; // count the number of child element in the line
            }

          len_of_child = (int *)TtaGetMemory( (sizeof(int)) * (count_child + 1) );
          memory_of_child = len_of_child;

          child = TtaGetFirstChild(el);

          // input the length of child one by one
          for (k=0; k < count_child; k++)
            {
              length_child = TtaGetElementVolume(child);
              len_of_child[k] = length_child;
              TtaNextSibling(&child);
            }

          // adjust the length of line (subtract Annotation Icon's length)
          for (k=1; k < count_child; k+=3)
            len -= len_of_child[k];

          child = TtaGetFirstChild(el);
          if ( (len + time) < i )
						TtaFreeMemory(memory_of_child);
        }
      // interpret Char Scheme value and calculate the firstchar of the line
      while (child && ((len < i && time ==0 ) || ((len+time) < i && time >=1 )))
        {
          i -= len;
          prev = child;
          TtaNextSibling(&el);
          child = TtaGetFirstChild(el);

          if (child == NULL)
            {
              len = i;
              child = prev;
            }
          else if (child !=NULL)
            {
              len = TtaGetElementVolume(el);
              len_of_line = TtaGetElementVolume(child);
              time++;
              count_child = 1;

              /* Consider "Annotation Icon" if Annotation Icon is on the line */
              if (len != len_of_line)
                {
                  //analysis the structure of target line including "Annotation Icon"
                  while (len != len_of_line)
                    {
                      TtaNextSibling(&child);
                      length_child = TtaGetElementVolume(child);
                      len_of_line += length_child;
                      count_child++; // count the number of child element in the line
                    }

                  len_of_child = (int *)TtaGetMemory( (sizeof(int)) * (count_child + 1) );
                  memory_of_child = len_of_child;
                  child = TtaGetFirstChild(el);

                  // input the length of child one by one
                  for (k=0; k < count_child; k++)
                    {
                      length_child = TtaGetElementVolume(child);
                      len_of_child[k] = length_child;
                      TtaNextSibling(&child);
                    }

                  // adjust the length of line (subtract Annotation Icon's length)
                  for (k=1; k < count_child; k+=3)
                    len -= len_of_child[k];

                  child = TtaGetFirstChild(el);
                  if ((len + time) < i)
                    TtaFreeMemory(memory_of_child);
                }
            }
        }
    }
	return (i+1-time);
}

/*----------------------------------------------------------------------
  GetElemWithChar
  access the position identified between characters by Char Scheme
  ----------------------------------------------------------------------*/
Element GetElemWithChar ( Document doc, char *nameVal)
{
  Element             el;
  char               *CharNum;
  Element             child, prev;
  ElementType         elType;
  int                 i,line, len,len1,len_of_line,count_child=1;
  int                 Char,time = 0;
  int                 *len_of_child = NULL,*memory_of_child = NULL;
  int                 sum_length_child;
  ThotBool            selpos=FALSE;
  int                 length_child,j,k;

  if (!nameVal)
    return NULL;

  /* Extract The Char scheme value */
  CharNum = strstr(nameVal,"=");
  CharNum = &CharNum[1];
  Char = atoi(CharNum);
  el = TtaGetMainRoot (doc);
  elType = TtaGetElementType(el);
  elType.ElTypeNum = TextFile_EL_Line_;
  el = TtaSearchTypedElement (elType, SearchForward, el);
  child = TtaGetFirstChild(el);
  if (el)
    {
      i = Char;
      len = TtaGetElementVolume(el);
      len1 = TtaGetElementVolume(child);
      len_of_line = len1;
      count_child = 1;

      /* Consider "Annotation Icon" if Annotation Icon is on the line */
      if (len != len1)
        {
          //analysis the structure of target line including "Annotation Icon"
          while (len != len_of_line)
            {
              TtaNextSibling(&child);
              length_child = TtaGetElementVolume(child);
              len_of_line += length_child;
              count_child++; // count the number of child element in the line
            }

          len_of_child = (int *)TtaGetMemory( (sizeof(int)) * (count_child + 1) );
          memory_of_child = len_of_child;
          child = TtaGetFirstChild(el);

          // input the length of child one by one
          for (k = 0; k < count_child; k++)
            {
              length_child = TtaGetElementVolume(child);
              len_of_child[k] = length_child;
              TtaNextSibling(&child);
            }

          // adjust the length of line (subtract Annotation Icon's length)
          for (k = 1; k < count_child; k += 3)
            len -= len_of_child[k];

          child = TtaGetFirstChild(el);
          if ((len + time) < i)
            TtaFreeMemory(memory_of_child);
        }

      line=0;
      // interpret Char Scheme value and calculate the element including the identified position
      while (child && ((len < i && time == 0) || ((len+time) < i && time >= 1)))
        {
          i -= len;
          prev = child;
          TtaNextSibling(&el);
          child = TtaGetFirstChild(el);
          if (child == NULL)
            {
              len = i;
              child = prev;
            }
          else if (child != NULL)
            {
              len = TtaGetElementVolume(el);
              len1 = TtaGetElementVolume(child);
              time++;
              line++;
              len_of_line = len1;
              count_child = 1;

              /* Consider "Annotation Icon" if Annotation Icon is on the line */
              if (len != len1)
                {
                  //analysis the structure of target line including "Annotation Icon"
                  while (len != len_of_line)
                    {
                      TtaNextSibling(&child);
                      length_child = TtaGetElementVolume(child);
                      len_of_line += length_child;
                      count_child++; // count the number of child element in the line
                    }

                  len_of_child = (int *)TtaGetMemory ((sizeof(int)) * (count_child + 1));
                  memory_of_child = len_of_child;
                  child = TtaGetFirstChild(el);

                  // input the length of child one by one
                  for (k=0; k < count_child; k++)
                    {
                      length_child = TtaGetElementVolume(child);
                      len_of_child[k] = length_child;
                      TtaNextSibling(&child);
                    }

                  // adjust the length of line (subtract Annotation Icon's length)
                  for (k=1; k < count_child; k+=3)
                    len -= len_of_child[k];
                  child = TtaGetFirstChild(el);
                  if (len + time < i)
                    TtaFreeMemory(memory_of_child);
                }
            }
          if (len == 0 && time == i)
            break;
        }

      child = TtaGetFirstChild (el);
      if (selpos)
        TtaSelectString(doc,child,i+1,i);

      else if (!selpos)
        if (time==0) //if the identified position is on first line
          TtaSelectString(doc,child,i+1,i);

			//if the position is on other line
			if (time >=1)
        {
          //if position is in the the part before Annotation Icon
          if ( (i-time) <= len1 )
            TtaSelectString(doc,child,(i-time)+1,(i-time));

          //else if behind Icon

          else if ((i-time) > len_of_child[0])
            {
              sum_length_child = len_of_child[0];
              for (k=2; k < count_child; k += 3)
                {
                  if (i-time <= sum_length_child + len_of_child[k])
                    {
                      for (j=1; j <=k; j++)
                        TtaNextSibling(&child);

                      TtaSelectString (doc, child,(i-time+1) - sum_length_child,(i-time) - sum_length_child);
                      TtaFreeMemory(memory_of_child);
                      break;
                    }
                  else if ((i-time > sum_length_child + len_of_child[k] ) &&
                           (i-time <= sum_length_child + len_of_child[k] + len_of_child[k+1]))
                    {
                      for (j=1; j<=k+1; j++)
                        TtaNextSibling(&child);
                      TtaSelectString(doc,child,(i-time+1) - sum_length_child - len_of_child[k],(i-time) - sum_length_child - len_of_child[k]);
                      TtaFreeMemory(memory_of_child);
                      break;
                    }
                  sum_length_child += len_of_child[k] + len_of_child[k+1];
                }
            }
        }
    }
  return child;
}

/*----------------------------------------------------------------------
  GetElemWithLine
  access the position identified between lines by Line Scheme
  ----------------------------------------------------------------------*/
Element GetElemWithLine ( Document doc, char *nameVal)
{
  Element             el;
  char               *LineNum;
  Element             child, prev;
  ElementType         elType;
  int                 i, len ;
  int                 line,index = 0;
  ThotBool            selpos = FALSE;

  if (!nameVal)
    return NULL;

  /* Extract The Line Number */
  LineNum = strstr (nameVal, "=");
  LineNum++;
  line = atoi(LineNum);
  if (line >= 0)
    {
      el = TtaGetMainRoot (doc);
      elType = TtaGetElementType (el);
      elType.ElTypeNum = TextFile_EL_Line_;
      el = TtaSearchTypedElement (elType, SearchForward, el);

      for (i = 1; i <= line; i++)
        TtaNextSibling (&el);

      if (el)
        {
          child = TtaGetFirstChild (el);

          if (child)
            {
              if (index > 0)
                {
                  i = index;
                  len = TtaGetElementVolume (child);
                  while (child && len < i)
                    {
                      /* skip previous elements in the same line */
                      i -= len;
                      prev = child;
                      TtaNextSibling (&child);
                      if (child == NULL)
                        {
                          len = i;
                          child = prev;
                        }
                      else
                        len = TtaGetElementVolume (child);
                    }
                  if (selpos)
                    TtaSelectString (doc, child, i, i-1);
                  else
                    TtaSelectString (doc, child, i, i);
                }
              else
                //display the head point on indicated line
                TtaSelectString (doc, child, 1, 0);
            }
        }
      else
        TtaSetStatus (doc, 1, "   ", NULL);
    }
  return el;
}

/*----------------------------------------------------------------------
  GetElemWithLineRange
  access the range identified between lines by Line Scheme
  ----------------------------------------------------------------------*/
Element GetElemWithLineRange ( Document doc, char *nameVal)
{
  Element             el, el2;
  Element             child, child2;
  ElementType         elType, elType2;
  char               *name, *Line_first, *memory_Line;
  int                 i, len_value;
  int                 line1, line2;

  if (!nameVal)
    return NULL;

  i = 0;
  // Extract The Line Number
  name = strstr(nameVal,"=");
  name++;
  len_value = strlen(name);
  Line_first = (char *)TtaGetMemory (len_value + 1);
  memory_Line = Line_first;

  while (*(name + i) != ',')
	  i++;

  strncpy (Line_first, name, i);
  Line_first[i] = EOS;
  line1 = atoi (Line_first);
  name = strstr (name, ",");
  name++;
  line2 = atoi (name);
  TtaFreeMemory (memory_Line);

  //define the first Element
  el = TtaGetMainRoot (doc);
  elType = TtaGetElementType (el);
  elType.ElTypeNum = TextFile_EL_Line_;
  el = TtaSearchTypedElement (elType, SearchForward, el);

  for (i = 1; i <= line1; i++)
    TtaNextSibling (&el);

  child = TtaGetFirstChild (el);
  //define the second Element
  el2 = TtaGetMainRoot (doc);
  elType2 = TtaGetElementType (el2);
  elType2.ElTypeNum = TextFile_EL_Line_;
  el2 = TtaSearchTypedElement (elType2, SearchForward, el2);

  for (i = 1; i < line2; i++)
    TtaNextSibling (&el2);

  child2 = TtaGetFirstChild(el2);
  if (line1 != line2)
    {
      if (el != NULL && el2 != NULL)
        {
          //display the first Elememt
          TtaSelectElement(doc,el);
          //Extend the range to the second Element
          TtaExtendSelection(doc,el2,0);
        }
	  }
  else if (line1 == line2)
	  TtaSelectString(doc,child,1,0);

  return child;
}

/*----------------------------------------------------------------------
  GetElemWithCharRange
  access the range identified between characters by Char Scheme
  ----------------------------------------------------------------------*/
Element GetElemWithCharRange ( Document doc, char *nameVal)
{
  Element             el, el2;
  char                *name, *val_string;
  Element             child, prev;
  ElementType         elType;
  int                 len_value;
  int                 i, len,len_of_text = 0;
  int                 char1, char2;

  if (!nameVal && DocumentTypes[doc] != docText)
    return NULL;
  //Extract the firstchar and lastchar numbers
  name = strstr (nameVal, "=");
  if (name == NULL)
    return NULL;
  name++;
  len_value = strlen (name);
  if (len_value == 0)
    return NULL;

  // define the first and second Element
  el = TtaGetMainRoot (doc);
  len = TtaGetElementVolume(el);
  val_string = (char *)TtaGetMemory(len_value + 1);
  i = 0;
  while (name[i] != ',' && name[i] != EOS)
    {
      val_string[i] = name[i];
      i++;
    }
  val_string[i] = EOS;
  char1 = atoi (val_string);
  name = strstr (name, ",");
  if (name == NULL)
    char2 = len;
  else
    {
      name++;
      char2 = atoi (name);
    }
  TtaFreeMemory (val_string);
  if (char1 > len)
     char1 = len;
  if (char2 > len)
     char2 = len;
  elType = TtaGetElementType (el);
  elType.ElTypeNum = TextFile_EL_Line_;
  el = TtaSearchTypedElement (elType, SearchForward, el);
  child = TtaGetFirstChild (el);
  el2 = el;
  if (el == NULL)
    return NULL;

#ifndef IV
  /* interpret char1 and calculate the position */
  i = char1;
  do
    {
      len = TtaGetElementVolume (el);
      child = TtaGetFirstChild (el);
      len_of_text = TtaGetElementVolume (child);
      // get the next line
      TtaNextSibling (&el);
      if (len != len_of_text)
        {
          /* skip annotations in the line */
          do
            {
              elType = TtaGetElementType (child);
              prev = child;
              // get the next child
              TtaNextSibling (&child);
              if (elType.ElTypeNum == TextFile_EL_TEXT_UNIT)
                // a text
                len_of_text = TtaGetElementVolume (prev);
              else
                len_of_text = 0;
              if (child == NULL)
                len_of_text++;
              if (i < len_of_text)
                {
                  // select within this line
                  TtaSelectString (doc, prev, i+1, i);
                  child = NULL;
                  el = NULL;
                }
              else
                i -= len_of_text;
            }
          while (child);
        }
      else if ( i < len_of_text)
        {
          // select within this line
          TtaSelectString (doc, child, i+1, i);
          el = NULL;
        }
      else
        i = i - len_of_text - 1;
    }
  while (el);

  i = char2;
  el = el2;
  do
    {
      len = TtaGetElementVolume (el);
      child = TtaGetFirstChild (el);
      len_of_text = TtaGetElementVolume (child);
      // get the next line
      TtaNextSibling (&el);
      if (len != len_of_text)
        {
          /* skip annotations in the line */
          do
            {
              elType = TtaGetElementType (child);
              prev = child;
              // get the next chil
              TtaNextSibling (&child);
               if (elType.ElTypeNum == TextFile_EL_TEXT_UNIT)
                // a text
                len_of_text = TtaGetElementVolume (prev);
              else
                len_of_text = 0;
              if (child == NULL)
                len_of_text++;
              if (i < len_of_text)
                {
                  // select within this line
                  i++;
                  TtaExtendSelection (doc, prev, i);
                  child = NULL;
                  el = NULL;
                }
              else
                i -= len_of_text;
            }
          while (child && i > 0);
        }
      else if ( i <= len_of_text + 1)
        {
          // select within this line
          i++;
          TtaExtendSelection (doc, child, i);
          el = NULL;
        }
      else
        i = i - len_of_text - 1;
    }
  while (el);
  return child;
#else /* IV */
  Element             child2;
  int                 count_child = 0, len_of_line;
  int                 *child_string = NULL;
  int                 *child_string2 = NULL;
  int                 sum_length_child;
  int                 length_child;
  int                 j, k, time = 0;

  child2 = child;
  /* interpret char1 and calculate the position */
  if (el)
    {
      i = char1;
      len = TtaGetElementVolume (el);
      len_of_text = TtaGetElementVolume (child);
      len_of_line = len_of_text;
      count_child = 1;
      /* Consider "Annotation Icon" if Annotation Icon is on the line */
      if (len != len_of_text)
        {
          //analysis the structure of the target line including "Annotation Icon"
          while (len != len_of_line)
            {
              TtaNextSibling (&child);
              length_child = TtaGetElementVolume (child);
              len_of_line += length_child;
              count_child++; // count the number of child in the line
            }

          child_string = (int *)TtaGetMemory( (sizeof(int)) * (count_child + 1) );
          child = TtaGetFirstChild (el);

          // input the length of child one by one
          for (k = 0; k < count_child; k++)
            {
              length_child = TtaGetElementVolume(child);
              child_string[k] = length_child;
              TtaNextSibling(&child);
            }

          //adjust the length of line (subtract that of Annotation Icon)
          for (k = 1; k < count_child; k+=3)
            len -= child_string[k];

          child = TtaGetFirstChild (el);
          if ( len < i )
            {
              TtaFreeMemory(child_string);
              child_string = NULL;
            }
        }

      // interpret Char Scheme value and calculate the element including the starting position
      while (child && ((len < i && time == 0) || (len+time < i && time >= 1)))
        {
          i -= len;
          prev = child;
          TtaNextSibling (&el);
          child = TtaGetFirstChild (el);

          if (child == NULL)
            {
              len = i;
              child = prev;
            }
          else
            {
              len = TtaGetElementVolume (el);
              len_of_text = TtaGetElementVolume (child);
              time++;
              len_of_line = len_of_text;
              count_child = 1;

              /* Consider "Annotation Icon" if Annotation Icon is on the line */
              if (len != len_of_text)
                {
                  // analysis the structure of the line including "Annotation Icon"
                  while (len != len_of_line)
                    {
                      TtaNextSibling (&child);
                      length_child = TtaGetElementVolume(child);
                      len_of_line += length_child;
                      count_child++; // count the number of child in the line
                    }

                  child_string = (int *)TtaGetMemory( (sizeof(int)) * (count_child + 1) );
                  child = TtaGetFirstChild (el);

                  // input the length of child one by one
                  for (k = 0; k < count_child; k++)
                    {
                      length_child = TtaGetElementVolume(child);
                      child_string[k] = length_child;
                      TtaNextSibling (&child);
                    }

                  // adjust the length of line (subtract that of Annotation Icon)
                  for (k = 1; k < count_child; k += 3)
                    len -= child_string[k];

                  child = TtaGetFirstChild(el);
                  if (len + time < i)
                    {
                      TtaFreeMemory(child_string);
                      child_string = NULL;
                    }
                }
            }
          if (len == 0 && (time) == i)
            break;
        }
    }

  //if position is in the part before Annotation Icon
  if ( i-time <= len_of_text)
    TtaSelectString (doc, child, (i-time)+1, (i-time));
  //else if behind Icon
  else if (child_string && i-time > child_string[0])
    {
      sum_length_child = child_string[0];
      for (k=2; k < count_child; k += 3)
        {
          if ((i-time) <= sum_length_child + child_string[k])
            {
							for (j=1; j <=k; j++)
                TtaNextSibling(&child);

							TtaSelectString (doc, child,
                               (i-time+1) - sum_length_child,(i-time) - sum_length_child);
              TtaFreeMemory(child_string);
							break;
						}
          else if ((i-time > sum_length_child + child_string[k]) &&
                   (i-time <= sum_length_child + child_string[k] + child_string[k+1]))
            {
							for (j=1; j<=k+1; j++)
                TtaNextSibling (&child);

							TtaSelectString(doc,child,(i-time+1) - sum_length_child - child_string[k],(i-time) - sum_length_child - child_string[k]);

							TtaFreeMemory(child_string);
              child_string = NULL;
              break;
						}
          sum_length_child += child_string[k] + child_string[k+1];
        }
    }

	/* interpret char2 and calculate the position */
  time = 0;
  if (el)
    {
      i = char2;
      len = TtaGetElementVolume (el2);
      len_of_text = TtaGetElementVolume (child2);
      len_of_line = len_of_text;
      count_child = 1;

      /* Consider "Annotation Icon" if Annotation Icon is on the line */
      if (len != len_of_text)
        {
          // analysis the structure of the line including "Annotation Icon"
          while (len != len_of_line)
            {
              TtaNextSibling(&child2);
              length_child = TtaGetElementVolume(child2);
              len_of_line += length_child;
              count_child++; // count the number of child in the line
            }

          child_string2 = (int *)TtaGetMemory( (sizeof(int)) * (count_child + 1) );
          child2 = TtaGetFirstChild (el2);

          // input the length of child one by one
          for (k = 0; k < count_child; k++)
            {
              length_child = TtaGetElementVolume (child2);
              child_string2[k] = length_child;
              TtaNextSibling (&child2);
            }

          // adjust the length of line (subtract that of Annotation Icon)
          for (k = 1; k < count_child; k+=3)
            len -= child_string2[k];
          child2 = TtaGetFirstChild (el2);
          if ((len + time) < i)
            {
            TtaFreeMemory (child_string2);
            child_string2 = NULL;
            }
        }

      // interpret Char Scheme value and calculate the element including the ending position
      while (child2 && ((len < i && time == 0) || ((len+time) < i && time >=1 )))
        {
          i -= len;
          prev = child2;
          TtaNextSibling (&el2);
          child2 = TtaGetFirstChild (el2);
          if (child2 == NULL)
            {
              len = i;
              child2 = prev;
            }
          else
            {
              len = TtaGetElementVolume (el2);
              len_of_text = TtaGetElementVolume(child2);
              time++;
              len_of_line = len_of_text;
              count_child = 1;

              /* Consider "Annotation Icon" if Annotation Icon is on the line */
              if (len != len_of_text)
                {
                  // analysis the structure of the line including "Annotation Icon"
                  while (len != len_of_line)
                    {
                      TtaNextSibling(&child2);
                      length_child = TtaGetElementVolume (child2);
                      len_of_line += length_child;
                      count_child++;
                    }

                  child_string2 = (int *)TtaGetMemory ((sizeof(int)) * (count_child + 1));
                  child2 = TtaGetFirstChild(el2);

                  // input the length of child one by one
                  for (k=0; k < count_child; k++)
                    {
                      length_child = TtaGetElementVolume(child2);
                      child_string2[k] = length_child;
                      TtaNextSibling(&child2);
                    }

                  // adjust the length of line (subtract that of Annotation Icon)
                  for (k=1; k < count_child; k+=3)
                    len -= child_string2[k];
                  child2 = TtaGetFirstChild(el2);
                  if ( (len + time) < i )
                    {
                      TtaFreeMemory(child_string2);
                      child_string2 = NULL;
                    }
                }
            }
          if (len == 0 && time == i)
            break;
        }
    }

  //extend the range from first position to second
  if (i-time <= len_of_text)
    TtaExtendSelection(doc,child2,(i-time)+1);
  else if (child_string2 && i-time > child_string2[0])
    {
      sum_length_child = child_string2[0];

      for (k=2; k < count_child; k += 3)
        {
          if (i-time <= sum_length_child + child_string2[k])
            {
							for (j=1; j <=k; j++)
                TtaNextSibling(&child2);

              TtaExtendSelection(doc,child2,(i-time+1) - sum_length_child);
							TtaFreeMemory(child_string2);
              child_string2 = NULL;
							break;
						}
          else if ((i-time > sum_length_child + child_string2[k] ) &&
                   (i-time <= sum_length_child + child_string2[k] + child_string2[k+1] ))
            {
							for (j=1; j<=k+1; j++)
                TtaNextSibling(&child2);
							TtaExtendSelection (doc, child2,
                                  (i-time+1) - sum_length_child - child_string2[k]);
							TtaFreeMemory(child_string2);
              child_string2 = NULL;
							break;
						}

					sum_length_child += child_string2[k] + child_string2[k+1];
        }
      TtaFreeMemory(child_string2);
    }
  return child;
#endif
}

/*----------------------------------------------------------------------
  GetElemWithMatch
  access the part that the target text appeared  by Match Scheme
  ----------------------------------------------------------------------*/
Element GetElemWithMatch ( Document  doc, char *nameVal)
{
  Element             el, elFound,temp;
  ElementType         elType;
  CHARSET             charset;
  int                 length;
  char               *CharNum, *Numbersub, *memory_match;
  int                 error_flag;
  int                 *firstCh, *lastCh;
  int                 Char;

  length = strlen(nameVal);
  if (!nameVal)
    return NULL;

  charset = TtaGetDocumentCharset (doc);
  /* Extract The Match Scheme's value */
  CharNum = strstr(nameVal, "=");
  CharNum = &CharNum[1];
  Char = strlen (CharNum);
  Numbersub = (char *)TtaGetMemory (Char + 1);
  memory_match = Numbersub;
  firstCh = (int *)TtaGetMemory (sizeof(int));
  lastCh = (int *)TtaGetMemory (sizeof(int));
  elFound = NULL;
  el = TtaGetMainRoot (doc);
  elType = TtaGetElementType (el);
  elType.ElTypeNum = TextFile_EL_Line_;
  el = TtaSearchTypedElement (elType, SearchForward, el);
  strncpy (Numbersub, CharNum, Char);
  Numbersub[Char] = EOS;
  error_flag = FALSE;

  while (elFound == NULL)
    {
      // search the target text from head on Plain Text
      elFound = TtaSearchText (doc, el, FALSE, Numbersub, firstCh, lastCh, charset);
      temp = el;
      if (elFound == NULL)
        {
          TtaNextSibling (&el);
          if (temp == el)
            {
              error_flag = TRUE;
              break;
            }
        }
    }

  if (error_flag == FALSE)
    {
      TtaSelectString (doc, elFound, *firstCh, *lastCh);
      TtaFreeMemory (memory_match);
      TtaFreeMemory (firstCh);
      TtaFreeMemory (lastCh);
    }
  return elFound;
}

/*----------------------------------------------------------------------
  SearchTextattribute
  Depending on the value, inplement which of functions to identifier
  the position or range
  ----------------------------------------------------------------------*/
Element SearchTextattribute (Document doc, char *nameVal)
{
  Element             elFound;

  if (strncmp (nameVal,"line",4) == 0 && strstr(nameVal, ",") == NULL)
    elFound = GetElemWithLine(doc,nameVal);
  else if (strncmp(nameVal,"char",4) == 0 && strstr(nameVal,",") == NULL)
    elFound = GetElemWithChar(doc,nameVal);
  else if (strncmp(nameVal,"line",4) == 0 &&strstr(nameVal, ",") != NULL)
    elFound = GetElemWithLineRange(doc,nameVal);
  else if (strncmp(nameVal,"char",4) == 0 &&strstr(nameVal, ",") != NULL)
    elFound = GetElemWithCharRange(doc,nameVal);
  else if (strncmp(nameVal,"match",5) == 0 &&strstr(nameVal, ",") == NULL)
    elFound = GetElemWithMatch(doc,nameVal);
  else
    return NULL;
  return (elFound);
}

/*----------------------------------------------------------------------
  GetElemWithAttr
  Search in document doc an element having an attribute of type attrType
  whose value is nameVal.
  Return that element or NULL if not found.
  If ignoreAtt is not NULL, it is an attribute that should be ignored when
  comparing attributes.
  If ignoreEl is not NULL, it is an element that should be ignored when
  comparing attributes.
  ----------------------------------------------------------------------*/
Element GetElemWithAttr (Document doc, AttributeType attrType, char *nameVal,
                         Attribute ignoreAtt, Element ignoreEl)
{
  Element             el, elFound;
  Attribute           nameAttr;
  char               *name;
  int                 length;
  ThotBool            found;

  if (!nameVal)
    return NULL;
  elFound = NULL;
  el = TtaGetMainRoot (doc);
  found = FALSE;
  do
    {
      TtaSearchAttribute (attrType, SearchForward, el, &elFound, &nameAttr);
      if (nameAttr && elFound)
        {
          if (nameAttr != ignoreAtt &&
              /* skip the element which receive the new attribute and
                 element copies */
              elFound != ignoreEl && TtaIsCopy (elFound) == 0)
            {
              length = TtaGetTextAttributeLength (nameAttr);
              length++;
              name = (char *)TtaGetMemory (length);
              if (name != NULL)
                {
                  TtaGiveTextAttributeValue (nameAttr, name, &length);
                  /* compare the NAME attribute */
                  found = (strcmp (name, nameVal) == 0);
                  TtaFreeMemory (name);
                }
            }
        }
      if (!found)
        el = elFound;
    }
  while (!found && elFound);
  if (!found)
    elFound = NULL;
  return elFound;
}

/*----------------------------------------------------------------------
  SearchNAMEattribute
  search in document doc an element having an attribut NAME or ID (defined
  in DTD HTML, MathML, SVG, Template or generic XML) whose value is nameVal.
  Return that element or NULL if not found.
  If ignoreAtt is not NULL, it is an attribute that should be ignored when
  comparing NAME attributes.
  If ignoreEl is not NULL, it is an element that should be ignored when
  comparing NAME attributes.
  ----------------------------------------------------------------------*/
Element SearchNAMEattribute (Document doc, char *nameVal, Attribute ignoreAtt,
                             Element ignoreEl)
{
  Element             elFound;
  AttributeType       attrType;
  char               *name;

  /* search all elements having an attribute NAME */
  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_NAME;
  elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);

  if (!elFound)
    {
      /* search all elements having an attribute ID */
      attrType.AttrTypeNum = HTML_ATTR_ID;
      elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
      if (!elFound)
        {
          /* search all elements having an attribute ID */
          attrType.AttrTypeNum = HTML_ATTR_xmlid;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
        }
    }
  if (!elFound)
    {
      /* search all elements having an attribute ID (defined in the
         MathML DTD) */
      attrType.AttrSSchema = TtaGetSSchema ("MathML", doc);
      if (attrType.AttrSSchema)
        /* this document uses the MathML DTD */
        {
          attrType.AttrTypeNum = MathML_ATTR_id;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
          if (!elFound)
            {
              /* search all elements having an attribute ID */
              attrType.AttrTypeNum = MathML_ATTR_xmlid;
              elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
            }
        }
    }
#ifdef _SVG
  if (!elFound)
    {
      /* search all elements having an attribute ID (defined in the
         SVG DTD) */
      attrType.AttrSSchema = TtaGetSSchema ("SVG", doc);
      if (attrType.AttrSSchema)
        /* this document uses the SVG DTD */
        {
          attrType.AttrTypeNum = SVG_ATTR_id;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
          if (!elFound)
            {
              /* search all elements having an attribute ID */
              attrType.AttrTypeNum = SVG_ATTR_xmlid;
              elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
            }
        }
    }
#endif /* _SVG */
#ifdef ANNOTATIONS
  if (!elFound)
    {
      /* search all elements having an attribute ID (defined in the
         XLink S schema) */
      attrType.AttrSSchema = TtaGetSSchema ("XLink", doc);
      if (attrType.AttrSSchema)
        /* this document uses the XLink DTD */
        {
          attrType.AttrTypeNum = XLink_ATTR_id;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
        }
    }
#endif /* ANNOTATIONS */
#ifdef TEMPLATES
  if (!elFound)
    {
      attrType.AttrSSchema = TtaGetSSchema ("Template", doc);
      if (attrType.AttrSSchema)
        {
          /* search all elements having an attribute ID (defined in the XTiger schema) */
          attrType.AttrTypeNum = Template_ATTR_name;
          elFound = GetElemWithAttr (doc, attrType, nameVal, ignoreAtt, ignoreEl);
        }

    }
#endif /* TEMPLATES */
#ifdef XML_GENERIC
  if (!elFound)
    {
      /* search all elements having an attribute ID (defined in the
         XML DTD) */
      attrType.AttrSSchema = TtaGetDocumentSSchema (doc);
      if (attrType.AttrSSchema)
        {
          name = TtaGetSSchemaName (attrType.AttrSSchema);
          if (strcmp(name, "HTML") &&
              strcmp(name, "MathML") &&
              strcmp(name, "SVG"))
            {
              attrType.AttrTypeNum = XML_ATTR_xmlid;
              elFound = GetElemWithAttr (doc, attrType, nameVal,
                                         ignoreAtt, ignoreEl);
            }
        }
    }
#endif /* XML_GENERIC */

  return (elFound);
}


/*----------------------------------------------------------------------
  CheckUniqueName
  If attribute value is duplicated, generates a parsing error message.
  If it's not a valid ID value, generates a parsing error message.
  ----------------------------------------------------------------------*/
void CheckUniqueName (Element el, Document doc, Attribute attr,
                      AttributeType attrType)
{
#define MaxMsgLength 200
  ElementType    elType;
  int            lineNum, length;
  char          *name;
  char           msgBuffer[MaxMsgLength];

  elType = TtaGetElementType (el);
  if (attr)
    {
      name = (char*)GetXMLAttributeName (attrType, elType, doc);
      if (!strcmp (name, "id"))
        {
          if (!TtaIsValidID (attr, FALSE))
            {
              length = MAX_LENGTH - 1;
              name = (char *)TtaGetMemory (MAX_LENGTH);
              TtaGiveTextAttributeValue (attr, name, &length);
              name[MAX_LENGTH-1] = EOS;
              sprintf (msgBuffer, "Invalid ID value \"%s\"", name);
              lineNum = TtaGetElementLineNumber(el);
              if (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat)
                XmlParseError (errorParsing, (unsigned char *)msgBuffer, lineNum);
              else
                HTMLParseError (doc, msgBuffer, lineNum);
              TtaFreeMemory (name);
            }
          else
            {
             // this function is optional because it increases the loading time
              if (Check_read_ids && MakeUniqueName (el, doc, FALSE, FALSE))
                {
                  length = MAX_LENGTH - 1;
                  name = (char *)TtaGetMemory (MAX_LENGTH);
                  TtaGiveTextAttributeValue (attr, name, &length);
                  name[MAX_LENGTH-1] = EOS;
                  sprintf (msgBuffer, "Duplicate ID value \"%s\"", name);
                  lineNum = TtaGetElementLineNumber(el);
                  if (DocumentMeta[doc] && DocumentMeta[doc]->xmlformat)
                    XmlParseError (errorParsing, (unsigned char *)msgBuffer, lineNum);
                  else
                    HTMLParseError (doc, msgBuffer, lineNum);
                }
            }
        }
    }
}


/*----------------------------------------------------------------------
  Do_follow_link_callback
  This function is called when the document is loaded
  ----------------------------------------------------------------------*/
void Do_follow_link_callback (int targetDocument, int status, char *urlName,
                             char *outputfile, char *proxyname,
                             AHTHeaders *http_headers, void *context)
{
  Element             elFound=NULL;
  ElementType         elType;
  Element             elSource;
  Document            doc;
  Element             anchor;
  AttributeType       attrType;
  Attribute           PseudoAttr, HrefAttr;
  SSchema             docSchema;
  View                view;
  Do_follow_link_context  *ctx = (Do_follow_link_context *) context;
  char               *sourceDocUrl, *utf8path;
  char                newurl[MAX_LENGTH * 2], newname[MAX_LENGTH];

  /* retrieve the context */
  if (ctx == NULL)
    return;

  doc = ctx->doc;
  sourceDocUrl = ctx->sourceDocUrl;
  anchor = ctx->anchor;
  utf8path = ctx->utf8path;
  elSource = ctx->elSource;
  if (utf8path[0] == '#' && targetDocument != 0)
    /* attribute HREF contains the NAME of a target anchor */
    elFound = SearchNAMEattribute (targetDocument, &utf8path[1], NULL, NULL);
  if (DocumentURLs[doc] && !strcmp (DocumentURLs[doc], sourceDocUrl))
    {
      elType = TtaGetElementType (anchor);
      if (elType.ElTypeNum == HTML_EL_Anchor &&
          !strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        {
          /* it's an HTML A element. Change it's color */
          docSchema =   TtaGetSSchema ("HTML", doc);
          if (docSchema && (doc != targetDocument || utf8path[0] == '#') && anchor)
            {
              /* search PseudoAttr attribute */
              attrType.AttrSSchema = docSchema;
              attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
              PseudoAttr = TtaGetAttribute (anchor, attrType);
              /* if the target document has replaced the clicked
                 document, pseudo attribute "visited" should not be set */
              if (targetDocument == doc)
                /* the target document is in the same window as the
                   source document */
                if (strcmp (sourceDocUrl, DocumentURLs[targetDocument]))
                  /* both document have different URLs */
                  PseudoAttr = NULL;
              /* only turn off the link if it points that exists or that we can
                 follow */
              if (PseudoAttr && status != -1)
                {
                  if (utf8path[0] == '#')
                    {
                      if (targetDocument != 0 && elFound)
                        TtaSetAttributeText (PseudoAttr, "visited", anchor, doc);
                    }
                  else
                    TtaSetAttributeText (PseudoAttr, "visited", anchor, doc);
                }
            }
        }
    }

  NormalizeURL (utf8path, doc, newurl, newname, NULL);
  if ((utf8path[0] == '#' || (DocumentURLs[doc] && !strcmp(newurl, DocumentURLs[doc])))
          && targetDocument != 0)
    {
      if (!elFound)
        elFound = TtaGetMainRoot(doc);

      elType = TtaGetElementType (elFound);
      if (elType.ElTypeNum == HTML_EL_LINK &&
          !strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
        {
          /* the target is a HTML link element, follow this link */
          attrType.AttrSSchema = elType.ElSSchema;
          attrType.AttrTypeNum = HTML_ATTR_HREF_;
          HrefAttr = TtaGetAttribute (elFound, attrType);
          if (HrefAttr)
            Do_follow_link (elFound, elSource, HrefAttr, doc);
          return;
        }
      else
        {
          if (targetDocument == doc && utf8path[0] == '#')
            {
              /* jump in the same document */
              /* record current position in the history */
              AddDocHistory (doc, DocumentURLs[doc],
                             DocumentMeta[doc]->initial_url,
                             DocumentMeta[doc]->form_data,
                             DocumentMeta[doc]->method);
            }
          /* show the target element in all views */
          for (view = 1; view < 6; view++)
            if (TtaIsViewOpen (targetDocument, view))
              TtaShowElement (targetDocument, view, elFound, 0);
        }
    }
  TtaFreeMemory (utf8path);
  TtaFreeMemory (sourceDocUrl);
  TtaFreeMemory (ctx);
}

/*----------------------------------------------------------------------
  IsCSSLink returns TRUE is the element links a CSS stylesheet.
  ----------------------------------------------------------------------*/
ThotBool IsCSSLink (Element el, Document doc)
{
  AttributeType       attrType;
  Attribute           attr;
  char                buffer[MAX_LENGTH];
  int                 length;

  attrType.AttrSSchema = TtaGetSSchema ("HTML", doc);
  attrType.AttrTypeNum = HTML_ATTR_REL;
  attr = TtaGetAttribute (el, attrType);
  if (attr)
    {
      /* get a buffer for the attribute value */
      length = MAX_LENGTH / 4;
      TtaGiveTextAttributeValue (attr, buffer, &length);
      buffer[MAX_LENGTH / 4] = EOS;
      /* remove trailing spaces */
      while (buffer[length-1] == SPACE || buffer[length-1] == TAB)
        {
          buffer[length-1] = EOS;
          length--;
        }
      /* Check alternate style sheets (see section 14.3.2 of the HTML 4.01
         spec.) but note that Firefox accepts "stylesheet alternative" and
         possibly other non-standard values. */
      if (strncasecmp (buffer, "alternate", 9) == 0)
        /* it's probably an alternate style sheet (check that the next word
           is "stylesheet") */
        {
          /* @@@@@ just ignore it. But we should accept it and handle it as an
             alternate style sheet */
          return FALSE;
        }
      else if (strcasecmp (buffer, "stylesheet") == 0 ||
               strcasecmp (buffer, "style") == 0)
        {
          /* now check the type of the stylesheet */
          attrType.AttrTypeNum = HTML_ATTR_Link_type;
          attr = TtaGetAttribute (el, attrType);
          if (attr == NULL)
            /* by default it's a CSS stylesheet */
            return TRUE;
          else
            {
              /* get a buffer for the attribute value */
              length = MAX_LENGTH / 4;
              TtaGiveTextAttributeValue (attr, buffer, &length);
	      buffer[MAX_LENGTH / 4] = EOS;
              if (!strcasecmp (buffer, "text/css"))
                return TRUE;
            }
        }
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  Do_follow_link follows the link starting from the anchor element for a
  double click on the elSource element.
  The parameter doc is the document that contains the origin element.
  ----------------------------------------------------------------------*/
static ThotBool Do_follow_link (Element anchor, Element elSource,
                               Attribute HrefAttr, Document doc)
{
  AttributeType          attrType;
  Attribute              PseudoAttr, attr;
  Element                root;
  ElementType            elType;
  Document               targetDocument, reldoc;
  SSchema                HTMLSSchema;
  char                  *pathname, *utf8value;
  char                   documentname[MAX_LENGTH];
  char                  *utf8path, *info, *s;
  char                   newurl[MAX_LENGTH * 2], newname[MAX_LENGTH];
  int                    length, method;
  Do_follow_link_context *ctx;
  ThotBool		 isHTML, history, readonly = FALSE;

  if (Follow_exclusive || Synchronizing)
    // there is already a current follow the link or a synchronizing
    return FALSE;
  else
    Follow_exclusive = TRUE;
  if (anchor == NULL || HrefAttr == NULL)
    return FALSE;

  info = pathname = NULL;
  elType = TtaGetElementType (anchor);
  attrType.AttrTypeNum = 0;
  HTMLSSchema = TtaGetSSchema ("HTML", doc);
  isHTML = TtaSameSSchemas (elType.ElSSchema, HTMLSSchema);
  targetDocument = 0;
  PseudoAttr = NULL;
  /* get a buffer for the target URL */
  length = TtaGetTextAttributeLength (HrefAttr) + 1;
  if (length > MAX_LENGTH / 4)
    length = MAX_LENGTH / 4;
  utf8path = (char *)TtaGetMemory (MAX_LENGTH);
  if (utf8path)
    {
      elType = TtaGetElementType (anchor);
      if (isHTML && elType.ElTypeNum == HTML_EL_Anchor)
        {
          /* it's an HTML anchor */
          /* attach an attribute PseudoClass = active */
          attrType.AttrSSchema = HTMLSSchema;
          attrType.AttrTypeNum = HTML_ATTR_PseudoClass;
          PseudoAttr = TtaGetAttribute (anchor, attrType);
          if (PseudoAttr == NULL)
            {
              PseudoAttr = TtaNewAttribute (attrType);
              TtaAttachAttribute (anchor, PseudoAttr, doc);
            }
          TtaSetAttributeText (PseudoAttr, "active", anchor, doc);
        }
      /* get the URL itself (limited to MAX_LENGTH) */
      TtaGiveTextAttributeValue (HrefAttr, utf8path, &length);
      utf8path[length] = EOS;
      /* suppress white spaces at the end */
      length--;
      while (utf8path[length] == ' ')
        utf8path[length--] = EOS;

      /* save the context */
      ctx = (Do_follow_link_context*)TtaGetMemory (sizeof (Do_follow_link_context));
      ctx->anchor = anchor;
      ctx->doc = doc;
      ctx->utf8path = utf8path;
      ctx->elSource = elSource;
      /* save the complete URL of the source document */
      ctx->sourceDocUrl = TtaStrdup (DocumentURLs[doc]);
      TtaSetSelectionMode (TRUE);

      NormalizeURL (utf8path, doc, newurl, newname, NULL);
      if (utf8path[0] == '#' || !strcmp (newurl, DocumentURLs[doc]))
        {
          /* the target element is part of the same document */
          targetDocument = doc;
          /* manually invoke the callback */
          Do_follow_link_callback (targetDocument, 0, NULL, NULL, NULL, NULL,
                                  (void *) ctx);
        }
      else
        /* the target element seems to be in another document */
        {
          /* is the source element an image map? */
          if (HTMLSSchema)
            {
              attrType.AttrSSchema = HTMLSSchema;
              attrType.AttrTypeNum = HTML_ATTR_ISAMAP;
              attr = TtaGetAttribute (elSource, attrType);
              if (attr)
                {
                  /* it's an image map */
                  utf8value = GetActiveImageInfo (doc, elSource);
                  info = (char *)TtaConvertMbsToByte ((unsigned char *)utf8value,
                                                      TtaGetDefaultCharset ());
                  TtaFreeMemory (utf8value);
                }
            }

          s = (char *)TtaConvertMbsToByte ((unsigned char *)utf8path,
                                           TtaGetDefaultCharset ());
          length = MAX_LENGTH;
          if (info)
            length += strlen (info);
          pathname = (char *)TtaGetMemory (length + 1);
          strncpy (pathname, s, MAX_LENGTH / 4);
          pathname[MAX_LENGTH / 4] = EOS;
          TtaFreeMemory (s);
          if (info)
            {
              /* @@ what do we do with the precedent parameters?*/
              strcat (pathname, info);
              TtaFreeMemory (info);
            }
          /* interrupt current transfer */
          StopTransfer (doc, 1);
          /* get the referred document */
          if (!strncmp (pathname, "mailto:", 7))
            {
              TtaSetStatus (doc, 1,
                            TtaGetMessage (AMAYA, AM_CANNOT_LOAD),
                            pathname);
              TtaFreeMemory (pathname);
              TtaFreeMemory (utf8path);
              TtaFreeMemory (ctx);
              Follow_exclusive = FALSE;
              return (FALSE);
            }
#ifdef ANNOTATIONS
          /* is it an annotation link? */
          else if (elType.ElSSchema == TtaGetSSchema ("XLink", doc) &&
                   elType.ElTypeNum == XLink_EL_XLink)
            {
              /* loading an annotation */
              reldoc = 0;
              method = CE_ANNOT;
              history = FALSE;
            }
#endif /* ANNOTATIONS */
          else
            {
              reldoc = doc;
              method = CE_RELATIVE;
              history = TRUE;
              if (isHTML && elType.ElTypeNum == HTML_EL_LINK &&
                  IsCSSLink (anchor, doc))
                {
                  /* opening a CSS */
                  reldoc = 0;
                  method = CE_CSS;
                  history = FALSE;
                  /* normalize the URL */
                  s = (char *)TtaGetMemory (length + 1);
                  strcpy (s, pathname);
                  NormalizeURL (s, doc, pathname, documentname, NULL);
                  TtaFreeMemory (s);
                }
            }

          if (method != CE_RELATIVE || DontReplaceOldDoc ||
              CanReplaceCurrentDocument (doc, 1))
            {
              if (IsUndisplayedName (pathname))
                /* it's not necessary to open a new window */
                DontReplaceOldDoc = FALSE;

              /* Set the Help document in ReadOnly mode */
              root = TtaGetMainRoot (doc);
              readonly = (TtaGetAccessRight (root) == ReadOnly);
              /* Load the new document */
              targetDocument = GetAmayaDoc (pathname, NULL, reldoc, doc,
                                            method, history,
                                            (void (*)(int, int, char*, char*, char*,
                                                      const AHTHeaders*, void*)) Do_follow_link_callback,
                                            (void *) ctx);
              if (readonly)
                {
                  /* transmit the ReadOnly mode */
                  root = TtaGetMainRoot (targetDocument);
                  TtaSetAccessRight (root, ReadOnly, targetDocument);
                }
            }
          else
            {
              TtaFreeMemory (utf8path);
              TtaFreeMemory (ctx);
            }
          TtaFreeMemory (pathname);
        }
      Follow_exclusive = FALSE;
      return (TRUE);
    }
  Follow_exclusive = FALSE;
  return (FALSE);
}

/*----------------------------------------------------------------------
  CheckRefresh checks if a refresh is requested.
  ----------------------------------------------------------------------*/
void CheckRefresh (Document doc)
{
  Element	 elhead, el;
  ElementType	 elType;
  Attribute      attr;
  AttributeType  attrType, attrType1;
  char           value[MAX_LENGTH], *ptr;
  char          pathname[MAX_LENGTH], documentname[MAX_LENGTH];
  int            length;

  if (Refresh_exclusive)
    return;
  else
    Refresh_exclusive = TRUE;

  if (DocumentTypes[doc] == docHTML)
    /* it's an HTML document */
    {
      el = TtaGetRootElement (doc);
      elType = TtaGetElementType (el);
      elType.ElTypeNum = HTML_EL_HEAD;
      elhead = TtaSearchTypedElement (elType, SearchInTree, el);
      el = elhead;
      elType.ElTypeNum = HTML_EL_META;
      attrType.AttrSSchema = elType.ElSSchema;
      attrType.AttrTypeNum = HTML_ATTR_http_equiv;
      attrType1.AttrSSchema = elType.ElSSchema;
      attrType1.AttrTypeNum = HTML_ATTR_meta_content;
      while (el)
        {
          /* look for meta element within the head element */
          el = TtaSearchTypedElementInTree (elType, SearchForward, elhead, el);
          if (el)
            {
              attr = TtaGetAttribute (el, attrType);
              if (attr)
                {
                  value[0] = EOS;
                  length = MAX_LENGTH / 4;
                  TtaGiveTextAttributeValue (attr, value, &length);
		  value[MAX_LENGTH / 4] = EOS;
                  if (!strcasecmp (value, "refresh"))
                    {
                      attr = TtaGetAttribute (el, attrType1);
                      if (attr)
                        {
                          value[0] = EOS;
                          length = MAX_LENGTH / 4;
                          TtaGiveTextAttributeValue (attr, value, &length);
			  value[MAX_LENGTH / 4] = EOS;
                          ptr = strstr (value, "URL=");
                          if (ptr == NULL)
                            ptr = strstr (value, "url=");
                          if (ptr)
                            {
                              NormalizeURL (&ptr[4], doc, pathname, documentname, NULL);
                              if (IsUndisplayedName (pathname))
                                /* it's not necessary to open a new window */
                                DontReplaceOldDoc = FALSE;

                              /* Load the new document */
                              doc = GetAmayaDoc (pathname, NULL, doc, doc,
                                                 CE_RELATIVE, FALSE,
                                                 NULL, NULL);
                              el = NULL;
                            }
                        }
                    }
                }
            }
        }
    }
  Refresh_exclusive = FALSE;
}

/*----------------------------------------------------------------------
  DblClickOnButton     The user has double-clicked a BUTTON element.
  ----------------------------------------------------------------------*/
static void DblClickOnButton (Element element, Document document)
{
  AttributeType       attrType;
  Attribute           attr;
  ElementType         elType;
  int		       type;

  elType = TtaGetElementType (element);
  attrType.AttrSSchema = elType.ElSSchema;
  attrType.AttrTypeNum = HTML_ATTR_Button_type;
  attr = TtaGetAttribute (element, attrType);
  if (!attr)
    /* default value of attribute type is submit */
    type = HTML_ATTR_Button_type_VAL_submit;
  else
    type = TtaGetAttributeValue (attr);
  if (type == HTML_ATTR_Button_type_VAL_button)
    {
      /**** Activate the corresponding event ****/;
    }
  else
    {
      /* interrupt current transfer */
      StopTransfer (document, 1);
      SubmitForm (document, element);
    }
}

/*----------------------------------------------------------------------
  WithinLinkElement returns TRUE if the selection is within a link
  ----------------------------------------------------------------------*/
ThotBool WithinLinkElement (Element element, Document document)
{
  Element   ancestor;
  Attribute attr;

  ancestor = SearchAnchor (document, element, &attr, FALSE);
  return (ancestor != NULL);
}

/*----------------------------------------------------------------------
  ActivateElement   The user has activated an element.
  ----------------------------------------------------------------------*/
static ThotBool ActivateElement (Element element, Document doc)
{
  Attribute           HrefAttr;
  Element             anchor, elFound;
  ElementType         elType, elType1;
  char               *name;
  ThotBool	          ok, isHTML, isXLink, isSVG;

  elType = TtaGetElementType (element);
  name = TtaGetSSchemaName(elType.ElSSchema);
  anchor = NULL;
  isSVG = FALSE;
  isXLink = FALSE;
  isHTML = FALSE;
  if (!strcmp (name, "HTML"))
    isHTML = TRUE;
  else if (!strcmp (name, "XLink"))
    isXLink = TRUE;
  else if (!strcmp (name, "SVG"))
    isSVG = TRUE;

  /* Check if the current element is interested in double clicks */
  ok = FALSE;
  if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
      elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
      elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT ||
      elType.ElTypeNum == HTML_EL_SYMBOL_UNIT)
    /* it's a basic element. It is interested whatever its namespace */
    ok = TRUE;
  else if (isHTML)
    ok = (elType.ElTypeNum == HTML_EL_LINK ||
          elType.ElTypeNum == HTML_EL_Anchor ||
          elType.ElTypeNum == HTML_EL_AREA ||
          elType.ElTypeNum == HTML_EL_FRAME ||
          elType.ElTypeNum == HTML_EL_Block_Quote ||
          elType.ElTypeNum == HTML_EL_Quotation ||
          elType.ElTypeNum == HTML_EL_ins ||
          elType.ElTypeNum == HTML_EL_del ||
          elType.ElTypeNum == HTML_EL_C_Empty ||
          elType.ElTypeNum == HTML_EL_Radio_Input ||
          elType.ElTypeNum == HTML_EL_Checkbox_Input ||
          elType.ElTypeNum == HTML_EL_Option_Menu ||
          elType.ElTypeNum == HTML_EL_Submit_Input ||
          elType.ElTypeNum == HTML_EL_Reset_Input ||
          elType.ElTypeNum == HTML_EL_BUTTON_ ||
          elType.ElTypeNum == HTML_EL_File_Input);
  else if (isXLink)
    ok = TRUE;
  else if (isSVG)
    ok = TRUE;
  else
    ok = WithinLinkElement (element, doc);

  if (!ok)
    /* DoubleClick is disabled for this element type */
    return (FALSE);

  if (isHTML && (elType.ElTypeNum == HTML_EL_Submit_Input ||
                 elType.ElTypeNum == HTML_EL_Reset_Input))
    /* Form button */
    {
      if (elType.ElTypeNum == HTML_EL_Submit_Input ||
          elType.ElTypeNum == HTML_EL_Reset_Input)
        /* it 's a double click on a submit or reset button */
        {
          /* interrupt current transfer and submit the corresponding form */
          StopTransfer (doc, 1);
          SubmitForm (doc, element);
        }
      else if (elType.ElTypeNum == HTML_EL_BUTTON_)
        DblClickOnButton (element, doc);
      return (TRUE);
    }
  else if (isHTML && (elType.ElTypeNum == HTML_EL_PICTURE_UNIT ||
                      elType.ElTypeNum == HTML_EL_TEXT_UNIT ||
                      elType.ElTypeNum == HTML_EL_GRAPHICS_UNIT ||
                      elType.ElTypeNum == HTML_EL_SYMBOL_UNIT))
    {
      /* is it a double click in a BUTTON element? */
      elType1.ElSSchema = elType.ElSSchema;
      elType1.ElTypeNum = HTML_EL_BUTTON_;
      elFound = TtaGetTypedAncestor (element, elType1);
      if (elFound)
        {
          DblClickOnButton (elFound, doc);
          return (TRUE);
        }
      else if (elType.ElTypeNum == HTML_EL_PICTURE_UNIT)
        {
          /* check if it's a input image */
          elFound = TtaGetParent (element);
          elType1 = TtaGetElementType (elFound);
          if (elType1.ElTypeNum == HTML_EL_Image_Input)
            {
              /* interrupt current transfer */
              StopTransfer (doc, 1);
              SubmitForm (doc, element);
              return (TRUE);
            }
        }
    }

  if (isHTML && elType.ElTypeNum == HTML_EL_TEXT_UNIT)
    {
      /* is it an option menu ? */
      elFound = TtaGetParent (element);
      elType1 = TtaGetElementType (elFound);
      if (elType1.ElTypeNum == HTML_EL_Option)
        {
          SelectOneOption (doc, elFound);
          return (TRUE);
        }
    }
  else if (isHTML && elType.ElTypeNum == HTML_EL_Option_Menu)
    {
      /* it is an option menu */
      elType1.ElSSchema = elType.ElSSchema;
      elType1.ElTypeNum = HTML_EL_Option;
      elFound = TtaSearchTypedElement (elType1, SearchInTree, element);
      if (elFound)
        {
          SelectOneOption (doc, elFound);
          return (TRUE);
        }
    }
  else if (isHTML && elType.ElTypeNum == HTML_EL_Checkbox_Input)
    {
      SelectCheckbox (doc, element);
      return (TRUE);
    }
  else if (isHTML && elType.ElTypeNum == HTML_EL_Radio_Input)
    {
      SelectOneRadio (doc, element);
      return (TRUE);
    }
  else if (isHTML && elType.ElTypeNum == HTML_EL_File_Input)
    {
      ActivateFileInput (doc, element);
      return (TRUE);
    }

  /* Search the anchor or LINK element */
  anchor = SearchAnchor (doc, element, &HrefAttr, FALSE);

  if (anchor && HrefAttr)
    {
      if ((DocumentMeta[doc] &&
	   DocumentMeta[doc]->method == CE_HELP) ||
          !TtaIsActionAvailable ("GotoPreviousHTML"))
        {
	  if (!TtaIsSelectionEmpty ())
	    return FALSE;
          if (DocumentURLs[doc] &&
              strstr (DocumentURLs[doc], "Manual.html"))
              // open in new tab
              DontReplaceOldDoc = TRUE;
          else
            // replace current help page
            DontReplaceOldDoc = FALSE;
          InNewWindow = FALSE;
        }
      return (Do_follow_link (anchor, element, HrefAttr, doc));
    }
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  CanFollowTheLink returns TRUE if the currently right-clic selected element
  is a link element.
  ----------------------------------------------------------------------*/
ThotBool CanFollowTheLink (Document doc)
{
  if (Right_ClickedElement && TtaGetDocument(Right_ClickedElement) == doc)
    {
      if (WithinLinkElement (Right_ClickedElement, doc))
        return TRUE;
    }
  return FALSE;
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void FollowTheLink (Document doc, View view)
{
  DontReplaceOldDoc = FALSE;
  if (Right_ClickedElement)
    ActivateElement (Right_ClickedElement, doc);
  Right_ClickedElement = NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void FollowTheLinkNewWin (Document doc, View view)
{
  DontReplaceOldDoc = TRUE;
  InNewWindow       = TRUE;
  if (Right_ClickedElement)
    ActivateElement (Right_ClickedElement, doc);
  Right_ClickedElement = NULL;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void FollowTheLinkNewTab (Document doc, View view)
{
  DontReplaceOldDoc = TRUE;
  InNewWindow       = FALSE;
  if (Right_ClickedElement)
    ActivateElement (Right_ClickedElement, doc);
  Right_ClickedElement = NULL;
}

/*----------------------------------------------------------------------
  DisplayUrlAnchor displays the url when an anchor is selected
  Return TRUE if an anchor is selected
  ----------------------------------------------------------------------*/
ThotBool DisplayUrlAnchor (Element element, Document doc)
{
  Attribute           HrefAttr, titleAttr;
  Element             anchor, ancestor;
  ElementType         elType;
  AttributeType       attrType;
  char                *url, *pathname, *documentname, *utf8value;
  int                 length;

  /* Search an ancestor that acts as a link anchor */
  HrefAttr = NULL;
  anchor = SearchAnchor (doc, element, &HrefAttr, FALSE);

  if (anchor && HrefAttr)
    {
      /* Get a buffer for the target URL */
      length = MAX_LENGTH / 4;
      utf8value = (char *)TtaGetMemory (MAX_LENGTH);
      if (utf8value != NULL)
        {
          /* Get the URL */
          TtaGiveTextAttributeValue (HrefAttr, utf8value, &length);
          utf8value[MAX_LENGTH / 4] = EOS;
          url = (char *)TtaConvertMbsToByte ((unsigned char *)utf8value,
                                             TtaGetDefaultCharset ());
          TtaFreeMemory (utf8value);
          pathname = (char *)TtaGetMemory (MAX_LENGTH + strlen (url));
          documentname = (char *)TtaGetMemory (MAX_LENGTH);
          if (url[0] == '#')
            {
              strcpy (pathname, DocumentURLs[doc]);
              strcat (pathname, url);
            }
          else
            /* Normalize the URL */
            NormalizeURL (url, doc, pathname, documentname, NULL);

          /* Display the URL in the status line */
          /* look for a Title attribute */
          titleAttr = NULL;
          ancestor = element;
          do
            {
              elType = TtaGetElementType (ancestor);
              if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
                {
                  if (elType.ElTypeNum == HTML_EL_LINK ||
                      elType.ElTypeNum == HTML_EL_Anchor)
                    {
                      attrType.AttrSSchema = elType.ElSSchema;
                      attrType.AttrTypeNum = HTML_ATTR_Title;
                      titleAttr = TtaGetAttribute (ancestor, attrType);
                    }
                }
              ancestor = TtaGetParent (ancestor);
            }
          while (titleAttr == NULL && ancestor != NULL);
          if (titleAttr)
            {
              TtaFreeMemory (url);
              length = TtaGetTextAttributeLength (titleAttr);
              length ++;
              utf8value = (char *)TtaGetMemory (length);
              if (utf8value)
                {
                  TtaGiveTextAttributeValue (titleAttr, utf8value, &length);
                  url = (char *)TtaConvertMbsToByte ((unsigned char *)utf8value,
                                                     TtaGetDefaultCharset ());
                  TtaFreeMemory (utf8value);
                  strcat (pathname, " (");
                  strcat (pathname, url);
                  strcat (pathname, ")");
                }
            }

          TtaSetStatus (doc, 1, pathname, NULL);
          TtaFreeMemory (pathname);
          TtaFreeMemory (documentname);
          TtaFreeMemory (url);
          return TRUE;
        }
    }
  return FALSE;
}

/*----------------------------------------------------------------------
  DoAction activates the current element from the keyboard
  ----------------------------------------------------------------------*/
void DoAction (Document doc, View view)
{
  Element             firstSel;
  int                 firstChar, lastChar;

  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
  if (firstSel)
    {
      if (!ActivateElement (firstSel, doc))
        TtaSelectWord (firstSel, firstChar, doc, view);
    }
}

/*----------------------------------------------------------------------
  AcceptTab inserts a TAB.
  -----------------------------------------------------------------------*/
ThotBool AcceptTab (NotifyOnTarget *event)
{
  TtcInsertChar (event->document, 1, TAB);
  /* don't let Thot perform it's normal operation */
  return TRUE;
}

/*----------------------------------------------------------------------
  NextLinkOrFormElement selects the next link or form element.
  -----------------------------------------------------------------------*/
void NextLinkOrFormElement (Document doc, View view)
{
#ifdef TEMPLATES
  ElementType         elType;
  Element             root, child, next, startEl, el;
  Attribute           attr;
  AttributeType       attrType1, attrType2;
  SSchema             schema;
  ThotBool            found, cycle;
  int                 i;
  int                 firstChar, lastChar;

  schema = TtaGetSSchema ("HTML", doc);
  attrType1.AttrTypeNum = HTML_ATTR_NAME;
  attrType1.AttrSSchema = schema;
  attrType2.AttrTypeNum = HTML_ATTR_HREF_;
  attrType2.AttrSSchema = schema;
  root = TtaGetRootElement (doc);
  TtaGiveFirstSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    {
      /* start from the root element */
      el = root;
      /* we don't accept to restart from the beginning */
      cycle = TRUE;
    }
  else
    cycle = FALSE;

  /* don't manage this element */
  startEl = el;
  /* we're looking for a next element */
  TtaSearchAttributes (attrType1, attrType2, SearchForward, el, &el, &attr);
  found = FALSE;
  while (!found)
    {
      if (el == NULL)
        {
          /* end of the document */
          el = NULL;
          attr = NULL;
          if (!cycle)
            {
              /* restart from the beginning of the document */
              cycle = TRUE;
              el = root;
            }
          else
            /* stop the search */
            found = TRUE;
        }
      else if (el == startEl)
        {
          /* we made a complete cycle and no other element was found */
          el = NULL;
          attr = NULL;
          found = TRUE;
        }
      else if (attr)
        {
          elType = TtaGetElementType (el);
          switch (elType.ElTypeNum)
            {
            case HTML_EL_Option_Menu:
            case HTML_EL_Checkbox_Input:
            case HTML_EL_Radio_Input:
            case HTML_EL_Submit_Input:
            case HTML_EL_Reset_Input:
            case HTML_EL_Button_Input:
            case HTML_EL_BUTTON_:
            case HTML_EL_Anchor:
		    /* no included text: select the element itself */
              TtaSelectElement (doc, el);
              found =TRUE;
              break;

            case HTML_EL_Text_Area:
            case HTML_EL_Text_Input:
            case HTML_EL_File_Input:
            case HTML_EL_Password_Input:
              /* look for the last included text */
              elType.ElTypeNum = HTML_EL_TEXT_UNIT;
              child = TtaSearchTypedElement (elType, SearchForward, el);
              if (child)
                {
                  next = child;
                  do
                    {
                      child = next;
                      next = TtaSearchTypedElementInTree (elType,
                                                          SearchForward,
                                                          el, child);
                    }
                  while (next);
                  i = TtaGetTextLength (child);
                  TtaSelectString (doc, child, i+1, i);
                }
              found = TRUE;
              break;

            default:
              attr = NULL;
              break;
            }
        }
      if (!found)
        TtaSearchAttributes (attrType1, attrType2, SearchForward, el, &el, &attr);
    }
#endif // TEMPLATES
}


/*----------------------------------------------------------------------
  PreviousLinkOrFormElement selects the previous link or form element.
  -----------------------------------------------------------------------*/
void PreviousLinkOrFormElement (Document doc, View view)
{
  ElementType         elType;
  Element             root, child, next, startEl, el;
  Attribute           attr;
  AttributeType       attrType1, attrType2;
  SSchema             schema;
  ThotBool            found, cycle;
  int                 i;
  int                 firstChar, lastChar;

  if (DocumentTypes[doc] == docText ||
      DocumentTypes[doc] == docCSS ||
      DocumentTypes[doc] == docSource)
    // do nothing
    return;
  schema = TtaGetSSchema ("HTML", doc);
  attrType1.AttrTypeNum = HTML_ATTR_NAME;
  attrType1.AttrSSchema = schema;
  attrType2.AttrTypeNum = HTML_ATTR_HREF_;
  attrType2.AttrSSchema = schema;

  /* keep in mind the last element of the document */
  root = TtaGetRootElement (doc);
  el = TtaGetLastChild (root);
  attr = NULL;

  while (el)
    {
      root = el;
      /* check if this element matches */
      attr = TtaGetAttribute (el, attrType1);
      if (attr == NULL)
        attr = TtaGetAttribute (el, attrType2);
      if (attr == NULL)
        el = TtaGetLastChild (root);
      else
        /* a right element is found */
        el = NULL;
    }
  TtaGiveLastSelectedElement (doc, &el, &firstChar, &lastChar);
  if (el == NULL)
    {
      /* start from the end of the document */
      el = root;
      /* we don't accept to restart from the beginning */
      cycle = TRUE;
      /* attr != 0 if this element matches */
      startEl = NULL;
    }
  else
    {
      cycle = FALSE;
      attr = NULL;
      /* don't manage this element */
      startEl = el;
    }

  if (attr == NULL)
    /* we're looking for a next element */
    TtaSearchAttributes (attrType1, attrType2, SearchBackward, el, &el, &attr);
  found = FALSE;
  while (!found)
    {
      if (el == NULL)
        {
          /* begginning of the document */
          el = NULL;
          attr = NULL;
          if (!cycle)
            {
              /* restart from the end of the document */
              cycle = TRUE;
              el = root;
              /* check if this element matches */
              attr = TtaGetAttribute (el, attrType1);
              if (attr == NULL)
                attr = TtaGetAttribute (el, attrType2);
            }
          else
            /* stop the search */
            found = TRUE;
        }
      else if (el == startEl)
        {
          /* we made a complete cycle and no other element was found */
          el = NULL;
          attr = NULL;
          found = TRUE;
        }
      else if (attr)
        {
          elType = TtaGetElementType (el);
          switch (elType.ElTypeNum)
            {
            case HTML_EL_Option_Menu:
            case HTML_EL_Checkbox_Input:
            case HTML_EL_Radio_Input:
            case HTML_EL_Submit_Input:
            case HTML_EL_Reset_Input:
            case HTML_EL_Button_Input:
            case HTML_EL_BUTTON_:
            case HTML_EL_Anchor:
              /* no included text: select the element itself */
              TtaSelectElement (doc, el);
              found =TRUE;
              break;

            case HTML_EL_Text_Area:
            case HTML_EL_Text_Input:
            case HTML_EL_File_Input:
            case HTML_EL_Password_Input:
              /* look for the last included text */
              elType.ElTypeNum = HTML_EL_TEXT_UNIT;
              child = TtaSearchTypedElement (elType, SearchForward, el);
              if (child)
                {
                  next = child;
                  do
                    {
                      child = next;
                      next = TtaSearchTypedElementInTree (elType,
                                                          SearchForward,
                                                          el, child);
                    }
                  while (next);
                  i = TtaGetTextLength (child);
                  TtaSelectString (doc, child, i+1, i);
                }
              found =TRUE;
              break;

            default:
              attr = NULL;
              break;
            }
        }
      if (!found && !attr)
        TtaSearchAttributes (attrType1, attrType2, SearchBackward, el, &el, &attr);
    }
}

/*----------------------------------------------------------------------
  AccessKeyHandler handles links or select elements
  ----------------------------------------------------------------------*/
void AccessKeyHandler (Document doc, void *param)
{
  Element             el, child, next;
  ElementType         elType;
  SSchema             HTMLschema;
  int                 i;

  el = (Element)param;
  if (el)
    {
      elType = TtaGetElementType (el);
      HTMLschema = TtaGetSSchema ("HTML", doc);
      if (TtaSameSSchemas (elType.ElSSchema, HTMLschema) &&
          (elType.ElTypeNum == HTML_EL_LEGEND ||
           elType.ElTypeNum == HTML_EL_LABEL))
        TtaNextSibling (&el);
      /* activate or select the element */
      if (!ActivateElement (el, doc))
        {
          /* look for the last included text */
          elType.ElTypeNum = HTML_EL_TEXT_UNIT;
          child = TtaSearchTypedElement (elType, SearchForward, el);
          if (child)
            {
              next = child;
              do
                {
                  child = next;
                  next = TtaSearchTypedElementInTree (elType, SearchForward, el, child);
                }
              while (next);
              i = TtaGetTextLength (child);
              TtaSelectString (doc, child, i+1, i);
            }
          else
            /* no included text: select the element itself */
            TtaSelectElement (doc, el);
        }
    }
}


/*----------------------------------------------------------------------
  IgnoreEvent       An empty function to be able to ignore events.
  ----------------------------------------------------------------------*/
ThotBool IgnoreEvent (NotifyElement *event)
{
  /* don't let Thot perform it's normal operation */
  return TRUE;
}

/*----------------------------------------------------------------------
  DoubleClick     The user has double-clicked an element.
  ----------------------------------------------------------------------*/
ThotBool DoubleClick (NotifyElement *event)
{
  ThotBool usedouble;


  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);
  if (usedouble &&
      (DocumentMeta[event->document] == NULL ||
       DocumentMeta[event->document]->method != CE_HELP))
    {
      /* don't let Thot perform normal operation */
      return (ActivateElement (event->element, event->document));
    }
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  SimpleClick     The user has clicked an element.
  ----------------------------------------------------------------------*/
ThotBool SimpleClick (NotifyElement *event)
{
  ThotBool  usedouble;

#ifdef RDFa
#define MAX_NS 20
  char     *declarations[MAX_NS];
  char     *prefixes[MAX_NS];
  int       i;

  for (i = 0; i < MAX_NS; i++)
    {
      declarations[i] = NULL;
      prefixes[i] = NULL;
    }

  TtaGiveElemNamespaceDeclarations (event->document, event->element,
				    &declarations[0], &prefixes[0]);

  for (i=0; i<MAX_NS; i++)
    {
      if (declarations[i] != NULL)
	{
	  printf ("\nxmlns= ");
	  if (prefixes[i] != NULL)
	    printf ("%s: ", prefixes[i]);
	  printf ("%s ", declarations[i]);
	}
      if (prefixes[i] == NULL && declarations[i] == NULL)
	i = MAX_NS;
    }
  printf ("\n");
#endif /* RDFa */

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);
  if (usedouble &&
      (DocumentMeta[event->document] == NULL ||
       DocumentMeta[event->document]->method != CE_HELP))
    {
      DisplayUrlAnchor (event->element, event->document);
      return TRUE;
    }
  else
    {
      /* don't let Thot perform normal operation if there is an activation */
      return (ActivateElement (event->element, event->document));
    }
}

/*----------------------------------------------------------------------
  SimpleLClick     The user has clicked with the Left button.
  ----------------------------------------------------------------------*/
ThotBool SimpleLClick (NotifyElement *event)
{
  /* let Thot perform normal operation */
  Right_ClickedElement = NULL;
  return FALSE;
}

/*----------------------------------------------------------------------
  SimpleRClick     The user has clicked an element.
  ----------------------------------------------------------------------*/
ThotBool SimpleRClick (NotifyElement *event)
{
#ifdef _WX
  ElementType         elType;
  Element             el;

  Right_ClickedElement = event->element;
#ifdef TEMPLATES
  elType = TtaGetElementType (Right_ClickedElement);
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "Template"))
    {
      if (elType.ElTypeNum == Template_EL_repeat ||
          elType.ElTypeNum == Template_EL_bag)
        {
          el = TtaGetFirstChild (Right_ClickedElement);
          if (el)
            Right_ClickedElement = el;
        }
      else
        {
          /* select the following use element in the repeat */
           el = TtaGetParent (Right_ClickedElement);
           if (el)
             {
               elType = TtaGetElementType (el);
               if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "Template") &&
                   elType.ElTypeNum == Template_EL_repeat)
                 {
                   el = Right_ClickedElement;
                   TtaNextSibling (&el);
                   if (el)
                     Right_ClickedElement = el;
                 }
             }
        }
      TtaSelectElement (event->document, Right_ClickedElement);
    }
#endif /* _TEMPLATES */
  /* let Thot perform normal operation */
  return FALSE;
#endif /* _WX*/
}

/*----------------------------------------------------------------------
  AnnotSimpleClick     The user has clicked on an annotation icon
  ----------------------------------------------------------------------*/
ThotBool AnnotSimpleClick (NotifyElement *event)
{
#ifdef ANNOTATIONS
  /* if it's an annotation link, highlight the annotated text  */
  ANNOT_SelectSourceDoc (event->document, event->element);
#endif /* ANNOTATIONS */
  return SimpleClick (event);
}


/*----------------------------------------------------------------------
  UpdateXmlElementListTool update the content of the XML element list tool.
  doc : Current document.
  el : Selected element.
  ----------------------------------------------------------------------*/
void UpdateXmlElementListTool (Element el, Document doc)
{
#ifdef TEMPLATES
  if (!IsTemplateInstanceDocument(doc))
#endif /* TEMPLATE */
    TtaRefreshElementMenu(doc, 1);
}


/*----------------------------------------------------------------------
  UpdateTitle update the content of the Title field on top of the
  main window, according to the contents of element el.
  ----------------------------------------------------------------------*/
void UpdateTitle (Element el, Document doc)
{
  Element             textElem, next, sibling;
  ElementType         elType, siblingType;
  Language            lang;
  char               *text;
#ifndef _WX
  char               *src;
#endif /* _WX */
  int                 length, i, l;

  if (TtaGetViewFrame (doc, 1) == 0)
    /* this document is not displayed */
    return;

  elType = TtaGetElementType (el);
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema),"SVG") &&
      elType.ElTypeNum == SVG_EL_title)
    /* it's a SVG title */
    {
      if (TtaGetParent (el) != TtaGetRootElement(doc))
        /* it's not a child of the root SVG element, ignore */
        return;
      /* this title element is a child of the root element */
      sibling = el;
      do
        {
          TtaPreviousSibling (&sibling);
          if (sibling)
            siblingType = TtaGetElementType (sibling);
        }
      while (sibling && (siblingType.ElTypeNum != SVG_EL_title ||
                         siblingType.ElSSchema != elType.ElSSchema));
      if (sibling)
        /* this title element has a previous title sibling. Ignore */
        return;
    }

  textElem = TtaGetFirstChild (el);
  if (textElem)
    {
      /* what is the length of the title? */
      length = 0;
      next = textElem;
      while (next)
        {
          elType = TtaGetElementType (next);
          if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
            length += TtaGetTextLength (next);
#ifdef TEMPLATES
          if ((elType.ElTypeNum == Template_EL_useEl ||
               elType.ElTypeNum == Template_EL_useSimple) &&
              !strcmp (TtaGetSSchemaName (elType.ElSSchema), "Template"))
            // Ignore the template use element
            next = TtaGetFirstChild (next);
          else
#endif /* TEMPLATES */
            TtaNextSibling (&next);
        }
      /* get the text of the title */
      length++;
      text = (char *)TtaGetMemory (length);
      text[0] = EOS;
      next = textElem;
      i = 0;
      while (next)
        {
          l = length - i;
          elType = TtaGetElementType (next);
          if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
            {
              TtaGiveTextContent (next, (unsigned char *)&text[i], &l, &lang);
              i += l;
            }
#ifdef TEMPLATES
          if ((elType.ElTypeNum == Template_EL_useEl ||
               elType.ElTypeNum == Template_EL_useSimple) &&
              !strcmp (TtaGetSSchemaName (elType.ElSSchema), "Template"))
            // Ignore the template use element
            next = TtaGetFirstChild (next);
          else
#endif /* TEMPLATES */
            TtaNextSibling (&next);
        }
      if (DocumentTypes[doc] != docSource)
        TtaChangeWindowTitle (doc, 0, text, UTF_8);
      if (DocumentTypes[doc] == docSource || DocumentSource[doc])
        {
#ifdef _WX
          /* with wx, document source use the document filename as title,
           * nothing is done here*/
#else /* _WX */
          src = (char *)TtaGetMemory (length + 9);
          sprintf (src, "Source: %s", text);
          if (DocumentTypes[doc] == docSource)
            TtaChangeWindowTitle (doc, 1, src, UTF_8);
          else
            TtaChangeWindowTitle (DocumentSource[doc], 1, src, UTF_8);
          TtaFreeMemory (src);
#endif /* _WX */
        }
      TtaFreeMemory (text);
    }
}

/*----------------------------------------------------------------------
  CloseLogs closes all logs attached to the document.
  ----------------------------------------------------------------------*/
void CloseLogs (Document doc)
{
  int		     i;

  /* are there log documents linked to this document? */
  if (doc)
    {
      for (i = 1; i < DocumentTableLength; i++)
        if (DocumentURLs[i] && DocumentSource[i] == doc &&
            DocumentTypes[i] == docLog)
          {
            /* close the window of the log file attached to the
               current document */
            TtaCloseDocument (i);
            TtaFreeMemory (DocumentURLs[i]);
            DocumentURLs[i] = NULL;
            if (DocumentTypes[i] != docLog)
              /* switch off the button Show Log file */
              UpdateLogFile (doc, FALSE);
            DocumentSource[i] = 0;
            /* restore the default document type */
            DocumentTypes[i] = docFree;
          }
    }
}

/*----------------------------------------------------------------------
  FocusChanged closes all open log windows except the current document
  -----------------------------------------------------------------------*/
void FocusChanged (Document doc)
{
  int		     i;

  if ( DocumentTypes[doc] == docSource)
    {
      doc = GetDocFromSource (doc);
      if (doc == 0)
        // Amaya is creating the source view of the current document
        return;
    }
  if (GoToSection)
    return;
  UpdateStyleList (doc, 1);
#ifdef DAV
  if (DocumentMeta[doc])
    DAVSetLockIndicator (doc, DocumentMeta[doc]->lockState);
#endif /* DAV */
  
  for (i = 1; i < DocumentTableLength; i++)
    if (DocumentURLs[i] && DocumentSource[i] != doc &&
        DocumentTypes[i] == docLog)
      {
        /* close this log file window */
        TtaCloseDocument (i);
        TtaFreeMemory (DocumentURLs[i]);
        DocumentURLs[i] = NULL;
        DocumentSource[i] = 0;
        if (DocumentMeta[i])
          {
            DocumentMetaClear (DocumentMeta[i]);
            TtaFreeMemory (DocumentMeta[i]);
            DocumentMeta[i] = NULL;
          }
        /* restore the default document type */
        DocumentTypes[i] = docFree;
      }
}

/*----------------------------------------------------------------------
  Free the last closed tab
  ----------------------------------------------------------------------*/
void FreeLastClosedTab ()
{
  if (LastClosedTab != NULL)
    {
      TtaFreeMemory (LastClosedTab);
      LastClosedTab = NULL;
    }
}

/*----------------------------------------------------------------------
  UndoCloseTab
  Undo the last closed tab
  ----------------------------------------------------------------------*/
void UndoCloseTab (Document doc, View view)
{
  if (LastClosedTab != NULL)
    {
      DontReplaceOldDoc = TRUE;
      InNewWindow       = FALSE;
      GetAmayaDoc (LastClosedTab, NULL, doc, doc,
		   CE_ABSOLUTE, FALSE, NULL, NULL);
      TtaFreeMemory (LastClosedTab);
      LastClosedTab = NULL;
    }
}

/*----------------------------------------------------------------------
  FreeDocumentResource
  ----------------------------------------------------------------------*/
void FreeDocumentResource (Document doc)
{
  Document	     sourceDoc;
#ifdef TEMPLATES
  XTigerTemplate  t;
#endif /* TEMPLATES */
  char              *tempdocument;
  int                i;

  if (doc == 0)
    return;

  UpdateLogFile (doc, FALSE);
  if (doc == ParsedDoc)
    /* The document to which CSS are to be applied */
    ParsedDoc = 0;
  if (DocumentURLs[doc] != NULL)
    {
      /* remove the temporary copy of the file */
      tempdocument = GetLocalPath (doc, DocumentURLs[doc]);
      TtaFileUnlink (tempdocument);
      TtaFreeMemory (tempdocument);
      if (DocumentTypes[doc] == docLog)
        DocumentSource[doc] = 0;
      else
        {
          /* remove the Parsing errors file */
          RemoveParsingErrors (doc);
          ClearMathFrame (doc);
#ifdef ANNOTATIONS
          ANNOT_FreeDocumentResource (doc);
#endif /* ANNOTATIONS */
#ifdef BOOKMARKS
          BM_FreeDocumentResource (doc);
#endif /* BOOKMARKS */
        }
      /* remove the document from the auto save list */
      RemoveAutoSavedDoc (doc);

      /* Save the last closed document */
      if (DocumentTypes[doc] == docHTML ||
	  DocumentTypes[doc] == docCSS ||
	  DocumentTypes[doc] == docMath ||
	  DocumentTypes[doc] == docSVG ||
	  DocumentTypes[doc] == docXml ||
	  DocumentTypes[doc] == docCSS)
	{
	  if (LastClosedTab != NULL)
	    {
	      TtaFreeMemory (LastClosedTab);
	      LastClosedTab = NULL;
	    }
	  if (DocumentURLs[doc] != NULL)
	    {
	      LastClosedTab  = (char *)TtaGetMemory (strlen (DocumentURLs[doc]) + 1);
	      strcpy (LastClosedTab, DocumentURLs[doc]);
	    }
	}

      TtaFreeMemory (DocumentURLs[doc]);
      DocumentURLs[doc] = NULL;
      if (DocumentMeta[doc])
        {
          DocumentMetaClear (DocumentMeta[doc]);
          TtaFreeMemory (DocumentMeta[doc]);
          DocumentMeta[doc] = NULL;
        }
      if (HighlightDocument == doc)
        ResetHighlightedElement ();

      if (DocumentTypes[doc] != docLog)
        {
          /* switch off the button Show Log file */
          UpdateLogFile (doc, FALSE);
          RemoveDocCSSs (doc, TRUE);
          /* free access keys table */
          TtaRemoveDocAccessKeys (doc);
#ifdef TEMPLATES
          // remove the template structure
          t = GetXTigerDocTemplate(doc);
          if (t)
            Template_Close (t);
#endif /* TEMPLATES */
          if (!Synchronizing && DocumentSource[doc])
            {
              sourceDoc = DocumentSource[doc];
              // set the source unmodified before closing it
              TtaSetDocumentUnmodified (sourceDoc);
              TtcCloseDocument (sourceDoc, 1);
              FreeDocumentResource (sourceDoc);
              DocumentSource[doc] = 0;
            }
          /* is this document the source of another document? */
          for (i = 1; i < DocumentTableLength; i++)
            if (DocumentURLs[i] && DocumentSource[i] == doc)
              {
                DocumentSource[i] = 0;
                if (DocumentTypes[i] == docLog)
                  {
                    /* close the window of the log file attached to the
                       current document */
                    TtaCloseDocument (i);
                    TtaFreeMemory (DocumentURLs[i]);
                    DocumentURLs[i] = NULL;
                  }
              }
          /* avoid to free images of backup documents */
          if (BackupDocument != doc)
            {
              RemoveLoadedResources (doc, &ImageURLs);
              RemoveLoadedResources (doc, &LoadedResources);
            }
        }
      /* restore the default document type */
      DocumentTypes[doc] = docFree;
    }
}

/*----------------------------------------------------------------------
  DocumentClosed
  ----------------------------------------------------------------------*/
void DocumentClosed (NotifyDialog * event)
{
  Document            doc;
#ifdef _SVG
  View                tm_view;
#endif  /* _SVG */

  if (event == NULL)
    return;
#ifdef DAV
  /* NEED : deal with last document when exiting the application.
   *
   * Now, when exiting the application, if the document is locked
   * by the user (the lock information must be in the local base),
   * this function will ask whether the user wants to unlock it.
   * If user agrees, an UNLOCK request will be sent. But, under
   * Windows machines, this request will be killed when the application
   * exit, and no unlock will be done.
   */
  DAVFreeLock (event->document);
#endif /* DAV */

#ifdef _SVG
  Get_timeline_of_doc (event->document, &doc, &tm_view);
  if (doc)
    {
      TtaCloseView (doc, tm_view);
      Free_timeline_of_doc (event->document);
    }
#endif /* _SVG */
  doc = TtaGetSelectedDocument ();
  if (doc == 0 || doc == event->document)
    {
      /* table elements are no longuer selected */
      TableMenuActive = FALSE;
      MTableMenuActive = FALSE;
      SetTableMenuOff (doc, 1);
    }
  FreeDocumentResource (event->document);
  CleanUpParsingErrors ();
}

/*----------------------------------------------------------------------
  IsSelInElement
  Return TRUE if a typed element includes the current selection
  ----------------------------------------------------------------------*/
static ThotBool IsSelInElement (Element firstSel, Element lastSel, ElementType elType,
                                ElementType elTypeFirst, ElementType elTypeLast)
{
  ThotBool            newSelInElem;

  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      newSelInElem = ((elTypeFirst.ElTypeNum == elType.ElTypeNum &&
                       elTypeFirst.ElSSchema == elType.ElSSchema) ||
                      TtaGetExactTypedAncestor (firstSel, elType) != NULL);
      if (newSelInElem)
        newSelInElem = ((elTypeLast.ElTypeNum == elType.ElTypeNum &&
                         elTypeLast.ElSSchema == elType.ElSSchema) ||
                        TtaGetExactTypedAncestor (lastSel, elType) != NULL);
    }
  return newSelInElem;
}

/*----------------------------------------------------------------------
  A new element has been selected. Update menus accordingly.
  ----------------------------------------------------------------------*/
void UpdateContextSensitiveMenus (Document doc, View view)
{
  ElementType         elType, elTypeFirst, elTypeLast;
  Element             firstSel, lastSel, table;
  SSchema             sch;
  int                 firstChar, lastChar;
  ThotBool            newSelInElem, withHTML, withinTable, inMath;

  if (doc == 0)
    return;
  /* check if there are HTML elements in the document */
  sch = TtaGetSSchema ("HTML", doc);
  elType.ElSSchema = sch;
  withHTML = (DocumentTypes[doc] == docHTML && DocumentURLs[doc]);
  if (!withHTML)
    /* this is not a HTML document */
    /* check if there are HTML elements in the document */
    withHTML = (sch != NULL);
  withinTable = FALSE;
  inMath = FALSE;
  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
  TtaGiveLastSelectedElement (doc, &lastSel, &firstChar, &lastChar);
  if (firstSel)
    {
      /* look for an enclosing cell */
      elType = TtaGetElementType (firstSel);
      if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "MathML"))
        /* the current selection starts with a MathML element */
        {
          elType.ElTypeNum = MathML_EL_MTABLE;
          table = TtaGetExactTypedAncestor (firstSel, elType);
          withinTable = (table != NULL && !TtaIsReadOnly (table));
          if (withinTable)
            {
              elType.ElTypeNum = MathML_EL_RowLabel;
              if (TtaGetExactTypedAncestor (firstSel, elType) != NULL)
                withinTable = FALSE;
            }
          inMath = TRUE;
        }
      elType.ElSSchema = sch;
      if (sch && !withinTable)
        {
          elType.ElTypeNum = HTML_EL_Table_;
          table = TtaGetExactTypedAncestor (firstSel, elType);
          withinTable = (table != NULL && !TtaIsReadOnly (table));
        }
    }

  /* update table menu entries */
  if ((!withinTable || inMath) && TableMenuActive)
    {
      TableMenuActive = FALSE;
      TtaSetItemOff (doc, 1, Types, BCaption);
      TtaSetItemOff (doc, 1, Types, BTHead);
      TtaSetItemOff (doc, 1, Types, BTBody);
      TtaSetItemOff (doc, 1, Types, BTFoot);
      TtaSetItemOff (doc, 1, Tools, BDataCell);
      TtaSetItemOff (doc, 1, Tools, BHeadingCell);
    }
  if ((!withinTable || !inMath) && MTableMenuActive)
    MTableMenuActive = FALSE;

  if (withinTable && !inMath && !TableMenuActive)
    {
      TableMenuActive = TRUE;
      TtaSetItemOn (doc, 1, Types, BCaption);
      TtaSetItemOn (doc, 1, Types, BTHead);
      TtaSetItemOn (doc, 1, Types, BTBody);
      TtaSetItemOn (doc, 1, Types, BTFoot);
      TtaSetItemOn (doc, 1, Tools, BDataCell);
      TtaSetItemOn (doc, 1, Tools, BHeadingCell);
    }
  if (withinTable && inMath && !MTableMenuActive)
      MTableMenuActive = TRUE;

  if (TableMenuActive || MTableMenuActive)
    {
      TtaSetItemOn (doc, 1, Tools, BCellHExtend);
      TtaSetItemOn (doc, 1, Tools, BCellVExtend);
      TtaSetItemOn (doc, 1, Tools, BCellHShrink);
      TtaSetItemOn (doc, 1, Tools, BCellVShrink);
      TtaSetItemOn (doc, 1, Tools, BSelectRow);
      TtaSetItemOn (doc, 1, Tools, BCreateRowB);
      TtaSetItemOn (doc, 1, Tools, BCreateRowA);
      TtaSetItemOn (doc, 1, Tools, BSelectColumn);
      TtaSetItemOn (doc, 1, Tools, BCreateColumnB);
      TtaSetItemOn (doc, 1, Tools, BCreateColumnA);
      TtaSetItemOn (doc, 1, Tools, BPasteBefore);
      TtaSetItemOn (doc, 1, Tools, BPasteAfter);
    }
  else
    {
      TtaSetItemOff (doc, 1, Tools, BCellMerge);
      TtaSetItemOff (doc, 1, Tools, BCellHExtend);
      TtaSetItemOff (doc, 1, Tools, BCellVExtend);
      TtaSetItemOff (doc, 1, Tools, BCellHShrink);
      TtaSetItemOff (doc, 1, Tools, BCellVShrink);
      TtaSetItemOff (doc, 1, Tools, BSelectRow);
      TtaSetItemOff (doc, 1, Tools, BCreateRowB);
      TtaSetItemOff (doc, 1, Tools, BCreateRowA);
      TtaSetItemOff (doc, 1, Tools, BSelectColumn);
      TtaSetItemOff (doc, 1, Tools, BCreateColumnB);
      TtaSetItemOff (doc, 1, Tools, BCreateColumnA);
      TtaSetItemOff (doc, 1, Tools, BPasteBefore);
      TtaSetItemOff (doc, 1, Tools, BPasteAfter);
    }
  if (withinTable && (TtaIsColumnSaved (doc) || TtaIsRowSaved (doc)))
    {
      TtaSetItemOn (doc, 1, Tools, BPasteBefore);
      TtaSetItemOn (doc, 1, Tools, BPasteAfter);
    }
  else
    {
      TtaSetItemOff (doc, 1, Tools, BPasteBefore);
      TtaSetItemOff (doc, 1, Tools, BPasteAfter);
    }
  if (withinTable)
    {
      if (CanHShrinkCell (doc))
        TtaSetItemOn (doc, 1, Tools, BCellHShrink);
      else
        TtaSetItemOff (doc, 1, Tools, BCellHShrink);
      if (CanVShrinkCell (doc))
        TtaSetItemOn (doc, 1, Tools, BCellVShrink);
      else
        TtaSetItemOff (doc, 1, Tools, BCellVShrink);
      if (CanMergeSelectedCells (doc))
        {
          TtaSetItemOn (doc, 1, Tools, BCellMerge);
          TtaSetItemOff (doc, 1, Tools, BCellHExtend);
          TtaSetItemOff (doc, 1, Tools, BCellVExtend);
        }
      else
        {
          TtaSetItemOff (doc, 1, Tools, BCellMerge);
          TtaSetItemOn (doc, 1, Tools, BCellHExtend);
          TtaSetItemOn (doc, 1, Tools, BCellVExtend);
        }
    }

  if (!withHTML)
    return;
  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Preformatted;
      newSelInElem = (TtaGetExactTypedAncestor (firstSel, elType) != NULL);
    }

  /*
   * elements PICTURE, Object, Applet, Big_text, Small_text, Subscript,
   * Superscript, Font_  are not permitted in a Preformatted element.
   * The corresponding menu entries must be turned off
   */
  if (newSelInElem != SelectionInPRE)
    {
      SelectionInPRE = newSelInElem;
      if (newSelInElem)
        {
          TtaSetItemOff (doc, 1, Types, BImage);
          TtaSetItemOff (doc, 1, Types, BObject);
          TtaSetItemOff (doc, 1, Types, TBig);
          TtaSetItemOff (doc, 1, Types, TSmall);
          TtaSetItemOff (doc, 1, Types, TSpan);
          TtaSetItemOff (doc, 1, Types, TSub);
          TtaSetItemOff (doc, 1, Types, TSup);
        }
      else
        {
          TtaSetItemOn (doc, 1, Types, BImage);
          TtaSetItemOn (doc, 1, Types, BObject);
          TtaSetItemOn (doc, 1, Types, TBig);
          TtaSetItemOn (doc, 1, Types, TSmall);
          TtaSetItemOn (doc, 1, Types, TSpan);
          TtaSetItemOn (doc, 1, Types, TSub);
          TtaSetItemOn (doc, 1, Types, TSup);
        }
    }
  /*
   * Disable the "Comment" entry of menu "Context" if current selection
   * is within a comment
   */
  if (firstSel == NULL)
    newSelInElem = FALSE;
  else
    {
      elType.ElTypeNum = HTML_EL_Comment_;
      newSelInElem = (TtaGetExactTypedAncestor (firstSel, elType) != NULL);
    }
  if (newSelInElem != SelectionInComment)
    {
      SelectionInComment = newSelInElem;
      if (newSelInElem)
        TtaSetItemOff (doc, 2, Types, BComment);
      else
        TtaSetItemOn (doc, 2, Types, BComment);
    }
  /* update toggle buttons in menus "Information Type" and */
  /* "Character Element" */
  elTypeFirst = TtaGetElementType (firstSel);
  elTypeLast = TtaGetElementType (lastSel);
  elType.ElTypeNum = HTML_EL_Emphasis;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInEM != newSelInElem)
    {
      SelectionInEM = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TEmphasis, newSelInElem);
#ifdef _WX
      TtaSwitchPanelButton( doc, 1, WXAMAYA_PANEL_XHTML, WXAMAYA_PANEL_XHTML_EMPH, newSelInElem );
#endif /* _WX */
    }

  elType.ElTypeNum = HTML_EL_Strong;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInSTRONG != newSelInElem)
    {
      SelectionInSTRONG = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TStrong, newSelInElem);
#ifdef _WX
      TtaSwitchPanelButton( doc, 1, WXAMAYA_PANEL_XHTML, WXAMAYA_PANEL_XHTML_STRONG, newSelInElem );
#endif /* _WX */
    }

  elType.ElTypeNum = HTML_EL_Cite;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInCITE != newSelInElem)
    {
      SelectionInCITE = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TCite, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_ABBR;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInABBR != newSelInElem)
    {
      SelectionInABBR = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TAbbreviation, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_ACRONYM;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInACRONYM != newSelInElem)
    {
      SelectionInACRONYM = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TAcronym, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_ins;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (!newSelInElem)
    {
      // check also the block element
      elType.ElTypeNum = HTML_EL_INS;
      newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
    }

  if (SelectionInINS != newSelInElem)
    {
      SelectionInINS = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TInsertion, newSelInElem);
#ifdef _WX
      TtaSwitchPanelButton( doc, 1, WXAMAYA_PANEL_XHTML, WXAMAYA_PANEL_XHTML_INS, newSelInElem );
#endif /* _WX */
    }

  elType.ElTypeNum = HTML_EL_del;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (!newSelInElem)
    {
      // check also the block element
      elType.ElTypeNum = HTML_EL_DEL;
      newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
    }
  if (SelectionInDEL != newSelInElem)
    {
      SelectionInDEL = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TDeletion, newSelInElem);
#ifdef _WX
      TtaSwitchPanelButton( doc, 1, WXAMAYA_PANEL_XHTML, WXAMAYA_PANEL_XHTML_DEL, newSelInElem );
#endif /* _WX */
    }

  elType.ElTypeNum = HTML_EL_Def;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInDFN != newSelInElem)
    {
      SelectionInDFN = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TDefinition, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_Code;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInCODE != newSelInElem)
    {
      SelectionInCODE = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TCode, newSelInElem);
#ifdef _WX
      TtaSwitchPanelButton( doc, 1, WXAMAYA_PANEL_XHTML, WXAMAYA_PANEL_XHTML_CODE, newSelInElem );
#endif /* _WX */
    }

  elType.ElTypeNum = HTML_EL_Variable_;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInVAR != newSelInElem)
    {
      SelectionInVAR = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TVariable, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_Sample;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInSAMP != newSelInElem)
    {
      SelectionInSAMP = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TSample, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_Keyboard;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInKBD != newSelInElem)
    {
      SelectionInKBD = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TKeyboard, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_Italic_text;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInI != newSelInElem)
    {
      SelectionInI = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TItalic, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_Bold_text;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInB != newSelInElem)
    {
      SelectionInB = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TBold, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_Teletype_text;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInTT != newSelInElem)
    {
      SelectionInTT = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TTeletype, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_Big_text;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInBIG != newSelInElem)
    {
      SelectionInBIG = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TBig, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_Small_text;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInSMALL != newSelInElem)
    {
      SelectionInSMALL = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TSmall, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_Span;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInSpan != newSelInElem)
    {
      SelectionInSpan = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TSpan, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_Subscript;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInSub != newSelInElem)
    {
      SelectionInSub = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TSub, newSelInElem);
#ifdef _WX
      TtaSwitchPanelButton( doc, 1, WXAMAYA_PANEL_XHTML, WXAMAYA_PANEL_XHTML_SUB, newSelInElem );
#endif /* _WX */
    }

  elType.ElTypeNum = HTML_EL_Superscript;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInSup != newSelInElem)
    {
      SelectionInSup = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TSup, newSelInElem);
#ifdef _WX
      TtaSwitchPanelButton( doc, 1, WXAMAYA_PANEL_XHTML, WXAMAYA_PANEL_XHTML_SUP, newSelInElem );
#endif /* _WX */
    }

  elType.ElTypeNum = HTML_EL_Quotation;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInQuote != newSelInElem)
    {
      SelectionInQuote = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TQuotation, newSelInElem);
    }

  elType.ElTypeNum = HTML_EL_BDO;
  newSelInElem = IsSelInElement (firstSel, lastSel, elType, elTypeFirst, elTypeLast);
  if (SelectionInBDO != newSelInElem)
    {
      SelectionInBDO = newSelInElem;
      TtaSetToggleItem (doc, 1, Types, TBDO, newSelInElem);
    }

  // Is it an annotation ?
  if (firstSel)
    {
      elType = TtaGetElementType (firstSel);
      if ((!strcmp (TtaGetSSchemaName (elType.ElSSchema), "XLink")) &&
	  (elType.ElTypeNum == HTML_EL_PICTURE_UNIT))
	{
          TtaSetItemOn (doc, 1, Tools, BDeleteAnnot);
          TtaSetItemOn (doc, 1, Tools, BReplyToAnnotation);
          TtaSetItemOn (doc, 1, Tools, BPostAnnot);
	}
      else
	{
          TtaSetItemOff (doc, 1, Tools, BDeleteAnnot);
          TtaSetItemOff (doc, 1, Tools, BReplyToAnnotation);
          TtaSetItemOff (doc, 1, Tools, BPostAnnot);
	}
    }
}

/*----------------------------------------------------------------------
  LineNumberOfEl
  Returns the line number (position in the source file) of element el.
  ----------------------------------------------------------------------*/
static int LineNumberOfEl (Element el)
{
  int		ln;
  Element	child, sibling, uncle, ancestor, prev, parent;

  ln = TtaGetElementLineNumber (el);
  if (ln == 0)
    /* there is no line number associated with this element: the element
       does not exist in the source file */
    {
      /* get the first line number associated with its descendants */
      child = TtaGetFirstChild (el);
      while (child && ln == 0)
        {
          ln = LineNumberOfEl (child);
          if (ln == 0)
            TtaNextSibling (&child);
        }
      if (ln == 0)
        /* Descendants don't have any line number. Get the first line number
           associated with its following siblings */
        {
          sibling = el;
          do
            {
              TtaNextSibling (&sibling);
              if (sibling)
                ln = LineNumberOfEl (sibling);
            }
          while (sibling && ln == 0);
        }
      if (ln == 0)
        /* Siblings don't have any line number. Get the first line number
           associated with the following siblings of its ancestors */
        {
          ancestor = el;
          do
            {
              ancestor = TtaGetParent (ancestor);
              if (ancestor)
                {
                  uncle = ancestor;
                  do
                    {
                      TtaNextSibling (&uncle);
                      if (uncle)
                        ln = LineNumberOfEl (uncle);
                    }
                  while (uncle && ln == 0);
                }
            }
          while (ancestor && ln == 0);
        }
      if (ln == 0)
        /* Still no line number. Get the line number of the previous
           element with a line number */
        {
          ancestor = el;
          prev = el;
          TtaPreviousSibling (&prev);
          while (prev == NULL && ln == 0 && ancestor != NULL)
            {
              ancestor = TtaGetParent (ancestor);
              ln = TtaGetElementLineNumber (ancestor);
              if (ln == 0)
                {
                  prev = ancestor;
                  TtaPreviousSibling (&prev);
                }
            }
          ancestor = prev;

          while (ancestor && ln == 0)
            {
              prev = TtaGetLastLeaf (ancestor);
              if (prev)
                {
                  parent = TtaGetParent (prev);
                  while (prev && ln == 0)
                    {
                      ln = TtaGetElementLineNumber (prev);
                      if (ln == 0)
                        TtaPreviousSibling (&prev);
                    }
                  if (ln == 0)
                    ln = TtaGetElementLineNumber (parent);
                  if (ln == 0)
                    ancestor = TtaGetPredecessor (parent);
                }
            }
        }
    }
  return ln;
}

/*----------------------------------------------------------------------
  ResetHighlightedElement
  If an element is currently highlighted, remove its Highlight attribute
  ----------------------------------------------------------------------*/
void ResetHighlightedElement ()
{
  if (HighlightElement)
    {
      if (TtaGetElementType (HighlightElement).ElSSchema != NULL)
        TtaRemoveAttribute (HighlightElement, HighLightAttribute,
                            HighlightDocument);
      HighlightDocument = 0;
      HighlightElement = NULL;
      HighLightAttribute = NULL;
    }
}

/*----------------------------------------------------------------------
  SynchronizeSourceView
  A new element has been selected. If the Source view is open,
  synchronize it with the new selection.
  ----------------------------------------------------------------------*/
void SynchronizeSourceView (NotifyElement *event)
{
  Element             firstSel, el, child, otherEl, root;
  ElementType         elType;
  AttributeType       attrType;
  Attribute	      attr;
  Document	      doc, otherDoc;
  char            message[50];
  int             firstChar, lastChar, line, view;
  int		          val, x, y, width, height;
  ThotBool	      otherDocIsStruct, done;

  if (!event)
    return;
  doc = event->document;
  done = FALSE;
  /* get the other Thot document to be synchronized with the one where the
     user has just clicked */
  otherDoc = 0;
  otherDocIsStruct = FALSE;
  if (IsXMLDocType (doc))
    /* the user clicked on a structured document, the other doc is the
       corresponding source document */
    otherDoc = DocumentSource[doc];
  else if (DocumentTypes[doc] == docSource)
    /* the user clicked on a source document, the other doc is the
       corresponding structured document */
    {
      otherDocIsStruct = TRUE;
      otherDoc = GetDocFromSource (doc);
    }

  TtaGiveFirstSelectedElement (doc, &firstSel, &firstChar, &lastChar);
  if (otherDoc)
    /* looks for the element in the other document that corresponds to
       the clicked element */
    {
      if (firstSel == NULL && event->document == doc)
        /* the current selection is not already registered */
        firstSel = event->element;
      if (firstSel)
        {
          otherEl = NULL;
          /* Get the line number associated with the clicked element */
          line = LineNumberOfEl (firstSel);
          if (line == 0)
            return;
          /* look for an element with the same line number in the other doc */
          /* line numbers are increasing in document order */
          root = TtaGetMainRoot (otherDoc);
          el = root;
          elType = TtaGetElementType (el);
          do
            {
              if (el != root && TtaGetElementLineNumber (el) >= line)
                /* that's the right element */
                otherEl = el;
              else
                {
                  child = TtaGetFirstChild (el);
                  if (!child)
                    otherEl = el;
                  else
                    {
                      do
                        {
                          el = child;
                          TtaNextSibling (&child);
                        }
                      while (child && LineNumberOfEl (child) <= line);
                    }
                }
            }
          while (!otherEl && el);

          done = (otherEl == HighlightElement);
          if (otherEl && !done)
            /* different element found */
            {
              /* If an element is currently highlighted, remove its Highlight
                 attribute */
              ResetHighlightedElement ();
              /* Put a Highlight attribute on the element found */
              if (otherDocIsStruct)
                {
                  if (DocumentTypes[otherDoc] == docHTML)
                    {
                      attrType.AttrSSchema = TtaGetSSchema ("HTML",
                                                            otherDoc);
                      attrType.AttrTypeNum = HTML_ATTR_Highlight;
                      val = HTML_ATTR_Highlight_VAL_Yes_;
                    }
                  else if (DocumentTypes[otherDoc] == docMath)
                    {
                      attrType.AttrSSchema = TtaGetSSchema ("MathML",
                                                            otherDoc);
                      attrType.AttrTypeNum = MathML_ATTR_Highlight;
                      val = MathML_ATTR_Highlight_VAL_Yes_;
                    }
#ifdef _SVG
                  else if (DocumentTypes[otherDoc] == docSVG)
                    {
                      attrType.AttrSSchema = TtaGetSSchema ("SVG",
                                                            otherDoc);
                      attrType.AttrTypeNum = SVG_ATTR_Highlight;
                      val = SVG_ATTR_Highlight_VAL_Yes_;
                    }
                  else if (DocumentTypes[otherDoc] == docXml)
                    {
                      /* We take the schema of the main root */
                      /* attrType.AttrSSchema = TtaGetSSchema ("XML",
                         otherDoc); */
                      attrType.AttrSSchema = elType.ElSSchema;
                      attrType.AttrTypeNum = XML_ATTR_Highlight;
                      val = XML_ATTR_Highlight_VAL_Yes_;
                    }
                  else if (DocumentTypes[otherDoc] == docLibrary)
                    {
                      attrType.AttrSSchema = TtaGetSSchema ("HTML",
                                                            otherDoc);
                      attrType.AttrTypeNum = HTML_ATTR_Highlight;
                      val = HTML_ATTR_Highlight_VAL_Yes_;
                    }
#endif /* _SVG */
                  else
                    {
                      attrType.AttrSSchema = NULL;
                      attrType.AttrTypeNum = 0;
                      val = 0;
                    }
                }
              else
                {
                  attrType.AttrSSchema = TtaGetSSchema ("TextFile",
                                                        otherDoc);
                  attrType.AttrTypeNum = TextFile_ATTR_Highlight;
                  val = TextFile_ATTR_Highlight_VAL_Yes_;
                }
              if (attrType.AttrSSchema)
                {
                  attr = TtaNewAttribute (attrType);
                  TtaAttachAttribute (otherEl, attr, otherDoc);
                  TtaSetAttributeValue (attr, val, otherEl, otherDoc);
                  /* record the highlighted element */
                  HighlightDocument = otherDoc;
                  HighlightElement = otherEl;
                  HighLightAttribute = attr;
                  /* Scroll all views where the element appears to show it */
                  for (view = 1; view < 6; view++)
                    if (TtaIsViewOpen (otherDoc, view))
                      {
                        TtaGiveBoxAbsPosition (otherEl, otherDoc, view, UnPixel,
                                               &x, &y);
                        TtaGiveWindowSize (otherDoc, view, UnPixel, &width,
                                           &height);
                        if (y < 0 || y > height - 15)
                          TtaShowElement (otherDoc, view, otherEl, 25);
                      }
                }
              done = TRUE;
            }
        }
    }

  if (!done)
    /* If an element is currently highlighted, remove its Highlight
       attribute */
    ResetHighlightedElement ();

  if (firstSel &&
      (DocumentTypes[doc] == docSource ||
       DocumentTypes[doc] == docText ||
       DocumentTypes[doc] == docCSS ||
       DocumentTypes[doc] == docLog))
    {
      /* display the line position of the selection */
      line = TtaGetElementLineNumber (firstSel);
      elType = TtaGetElementType (firstSel);
      if (elType.ElTypeNum == TextFile_EL_TEXT_UNIT)
        {
          /* take into account previous elements in the same line */
          el = TtaGetParent (firstSel);
          el = TtaGetFirstChild (el);
          while (el && el != firstSel)
            {
              /* add characters of previous elements */
              firstChar += TtaGetElementVolume (el);
              TtaNextSibling (&el);
            }
        }
      sprintf (message, "Character: %d", firstChar);
      TtaSetStatus (doc, 1, message, NULL);
    }
}

/*----------------------------------------------------------------------
  GetCurrentLine returns the current selected line char index in the
  source file.
  -----------------------------------------------------------------------*/
void GetCurrentLine (Document doc, int *line, int *index)
{
  Element             el, child, parent;
  ElementType         elType;
  int                 first, last;

  *line = 0;
  *index = 0;
  /* look for a selection in the current document */
  TtaGiveFirstSelectedElement (doc, &el, &first, &last);
  if (el)
    {
      *line = TtaGetElementLineNumber (el);
      *index = first;
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == TextFile_EL_Line_)
        {
          /* take into account previous elements in the same line */
          parent = TtaGetParent (el);
          child = TtaGetFirstChild (parent);
          while (child != el)
            {
              /* add characters of previous elements */
              *index += TtaGetElementVolume (child);
              TtaNextSibling (&child);
            }
        }
    }
}

/*----------------------------------------------------------------------
  GotoLine points the corresponding line and char index in the
  source file.
  -----------------------------------------------------------------------*/
void GotoLine (Document doc, int line, int index, ThotBool selpos)
{
  Element             el, child, prev;
  ElementType         elType;
  char                message[50];
  int                 i, len;

  GoToSection = TRUE; // protect against a document close
  if (line)
    {
      /* open the source file */
      if (DocumentTypes[doc] == docFree)
        return;
      if (DocumentTypes[doc] != docCSS && DocumentTypes[doc] != docSource &&
          DocumentTypes[doc] != docText)
        {
          if (DocumentSource[doc] == 0)
            ShowSource (doc, 1);
          doc = DocumentSource[doc];
        }
      //TtaRaiseView (doc, 1);
      /* look for an element with the same line number in the other doc */
      /* line numbers are increasing in document order */
      el = TtaGetMainRoot (doc);
      elType = TtaGetElementType (el);
      elType.ElTypeNum = TextFile_EL_Line_;
      el = TtaSearchTypedElement (elType, SearchForward, el);
      for (i = 1; i < line; i++)
        TtaNextSibling (&el);
      if (el)
        {
          child = TtaGetFirstChild (el);
          if (child)
            {
	      if (index > 0)
                {
                  i = index;
                  len = TtaGetElementVolume (child);
                  while (child && len < i)
                    {
                      /* skip previous elements in the same line */
                      i -= len;
                      prev = child;
                      TtaNextSibling (&child);
                      if (child == NULL)
                        {
                          len = i;
                          child = prev;
                        }
                      else
                        len = TtaGetElementVolume (child);
                    }
                  if (selpos)
                    TtaSelectString (doc, child, i, i-1);
                  else
                    TtaSelectString (doc, child, i, i);
                }
              else
                TtaSelectElement (doc, el);
              //#ifndef _MACOS
              TtaHandlePendingEvents ();
              //#endif /* _MACOS */
	      TtaRaiseView (doc, 1);
              // display the char index
              sprintf (message, "Character: %d", index);
              TtaSetStatus (doc, 1, message, NULL);
            }
        }
      else
        TtaSetStatus (doc, 1, "   ", NULL);
    }
  GoToSection = FALSE;
}

/*----------------------------------------------------------------------
  ShowTextLine points the corresponding line.
  -----------------------------------------------------------------------*/
static ThotBool ShowTextLine (Element el, Document doc)
{
  Document	          otherDoc = 0;
  Element             otherEl;
  Language            lang;
  CSSInfoPtr          css;
  PInfoPtr            pInfo;
  char               *utf8value = NULL, *ptr = NULL, *s = NULL;
  int                 len, line = 0, index = 0;
  int				          firstChar = 0 ,lastChar = 0; // add new variables

  if (DocumentTypes[doc] == docLog)
    {
      /* get the target line and index from current string */
      if (el)
        {
          len = TtaGetTextLength (el);
          if (len > 0)
            {
              len++; /* make room for the final null byte */
              utf8value = (char *)TtaGetMemory (len);
              TtaGiveTextContent (el, (unsigned char *)utf8value, &len, &lang);
              /* extract the line number and the index within the line */
              ptr = strstr (utf8value, "line ");
              if (ptr)
                sscanf (&ptr[4], "%d", &line);
              if (ptr)
                ptr = strstr (ptr, "char");
              if (ptr)
                sscanf (&ptr[4], "%d", &index);
              /* Is there a file name in the current line */
              ptr = strstr (utf8value, ", file ");
              if (ptr)
                ptr += 7;
            }

          /* get the target document */
          otherDoc = DocumentSource[doc];
          if (ptr == NULL)
            {
              otherEl = TtaSearchText (doc, el, FALSE, "***",
                                       &firstChar, &lastChar, ISO_8859_1);
              if (otherEl)
                {
                  TtaFreeMemory (utf8value);
                  len = TtaGetTextLength (otherEl) + 1;
                  utf8value = (char *)TtaGetMemory (len);
                  TtaGiveTextContent (otherEl, (unsigned char *)utf8value,
                                      &len, &lang);
                  ptr = strstr (utf8value, " in ");
                  if (ptr)
                    ptr += 4;
                }
            }

          if (ptr)
            s = (char *)TtaConvertMbsToByte ((unsigned char *)ptr,
                                             TtaGetDefaultCharset ());
          if (DocumentURLs[otherDoc] &&
              s && strcmp (s, DocumentURLs[otherDoc]))
            {
              /* it doesn't concern the source document itself
                 look or the target file */
              for (otherDoc = 1; otherDoc < MAX_DOCUMENTS; otherDoc++)
                if (DocumentURLs[otherDoc] &&
                    !strcmp (s, DocumentURLs[otherDoc]))
                  break;
              if (otherDoc == MAX_DOCUMENTS)
                {
                  /* not found: do we have to open a CSS file */
                  css = SearchCSS (0, s, NULL, &pInfo);
                  if (css)
                    {
#ifdef _WX
                      LoadDefaultOpeningLocation (TRUE); // in new frame
                      otherDoc = GetAmayaDoc (s, NULL, DocumentSource[doc],
                                              DocumentSource[doc], CE_CSS,
                                              FALSE, NULL, NULL);
#else /* _WX */
                      otherDoc = GetAmayaDoc (s, NULL, 0, 0, CE_CSS,
                                              FALSE, NULL, NULL);
#endif /* _WX */
                    }
                }
            }
        }

      TtaFreeMemory (s);
      TtaFreeMemory (utf8value);
      /* skip to the line */
      if (line && otherDoc && otherDoc < MAX_DOCUMENTS)
        GotoLine (otherDoc, line, index, FALSE);
      return TRUE; /* don't let Thot perform normal operation */
    }
  else
    return FALSE; /* let Thot perform normal operation */

}

/*----------------------------------------------------------------------
  SimpleClickInText The user has clicked a log message.
  ----------------------------------------------------------------------*/
ThotBool SimpleClickInText (NotifyElement *event)
{
  ThotBool usedouble;

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);
  if (usedouble)
    return TRUE;
  else
    /* don't let Thot perform normal operation if there is an activation */
    return (ShowTextLine (event->element, event->document));
}

/*----------------------------------------------------------------------
  DoubleClickInText The user has double-clicked a log message.
  ----------------------------------------------------------------------*/
ThotBool DoubleClickInText (NotifyElement *event)
{
  ThotBool usedouble;

  TtaGetEnvBoolean ("ENABLE_DOUBLECLICK", &usedouble);
  if (usedouble)
    /* don't let Thot perform normal operation */
    return (ShowTextLine (event->element, event->document));
  else
    return FALSE;
}

/*----------------------------------------------------------------------
  RightClickInText The user has right-clicked a log message.
  ----------------------------------------------------------------------*/
ThotBool RightClickInText (NotifyElement *event)
{
  return (ShowTextLine (event->element, event->document));
}

/*----------------------------------------------------------------------
  CheckSynchronize
  Check if the selected document must be synchronized.
  If the clicked document is not the current one : synchronize it !
  ----------------------------------------------------------------------*/
void CheckSynchronize (NotifyElement *event)
{
  if (event->document != SelectionDoc &&
      DocumentURLs[event->document] != NULL &&
      DocumentTypes[event->document] != docLog) /* it's not a Log file */
    {
      if (SelectionDoc && DocumentURLs[SelectionDoc])
        {
          /* Reset buttons state in previous selected document */
          UpdateContextSensitiveMenus (SelectionDoc, 1);
          /* Synchronize the content of the old document */
          if (DocumentTypes[SelectionDoc] == docSource || /* source of ... */
              (DocumentSource[SelectionDoc] && /* has a source */
               IsXMLDocType (SelectionDoc)))
            {
              if (event->info == 1)
                /* an undo operation was done in event->document */
                DoSynchronize (event->document, 1, event);
              else if (TtaIsDocumentUpdated (SelectionDoc))
                DoSynchronize (SelectionDoc, 1, event);
              else if (event->document == HighlightDocument)
                SynchronizeSourceView (event);
            }
          // update menus with the new selection
          UpdateEditorMenus (event->document);
        }
      else
        {
          /* the document didn't change. Only synchronize the selection. */
          SynchronizeSourceView (event);
        }
      SelectionDoc = event->document;
    }
  else
    {
      /* the document didn't change. Only synchronize the selection. */
      SynchronizeSourceView (event);
    }
  if (DocumentTypes[SelectionDoc] == docHTML)
    UpdateContextSensitiveMenus (SelectionDoc, 1);
}

/*----------------------------------------------------------------------
  A new element has been selected. Update menus accordingly.
  ----------------------------------------------------------------------*/
void SelectionChanged (NotifyElement *event)
{
  Element             child, el = event->element;
  ElementType         elType;
  Document            doc = event->document;
  char                message[50], *s;
  int                 i, index = 0;

  if (SelectionChanging)
    return;
  SelectionChanging = TRUE;

  CheckSynchronize (event);
  TtaSelectView (SelectionDoc, 1);
  /* update the displayed style information */
  SynchronizeAppliedStyle (event);
#ifdef RDFA
  /* update the list of NS declaration */
  SynchronizeNSDeclaration (event);
#endif

  UnFrameMath ();

  if (DocumentTypes[doc] != docLog)
    {
      TtaGiveFirstSelectedElement (doc, &el, &index, &i);
      child = el;
      if (DocumentTypes[doc] != docSource && DocumentTypes[doc] != docCSS)
        {
          // update the XML list
          UpdateXmlElementListTool (el, doc);
          if (!DisplayUrlAnchor (el, doc))
            TtaSetStatus (doc, 1, "  ", NULL);
#ifdef TEMPLATES
          if (!TtaIsTextInserting ())
            // no current text insertion
            CheckPromptIndicator (el, doc);
          if (!IsTemplateDocument (doc))
#endif /* TEMPLATES */
            {
              elType = TtaGetElementType (child);
              s = TtaGetSSchemaName (elType.ElSSchema);
              if (!strcmp (s, "HTML"))
                TtaRaiseDoctypePanels (WXAMAYA_DOCTYPE_XHTML);
            }
        }
#ifdef _WX
      else
        {
          // manage the selection in source or css file
          elType = TtaGetElementType (el);
          if (elType.ElTypeNum == TextFile_EL_TEXT_UNIT)
            {
              TtaPreviousSibling (&child);
              while (child && child != el)
                {
                  index += TtaGetElementVolume (child);
                  TtaPreviousSibling (&child);
                }
              // display the char index
              sprintf (message, "Character: %d", index);
              TtaSetStatus (doc, 1, message, NULL);
            }
          else
            TtaSetStatus (doc, 1, "  ", NULL);
        }
#endif /* _WX */
      if (event->event != TteElemExtendSelect)
        TtaSetStatusSelectedElement(doc, 1, el);
    }
  SelectionChanging = FALSE;
}

/*----------------------------------------------------------------------
  HTMLElementTypeInMenu
  -----------------------------------------------------------------------*/
ThotBool HTMLElementTypeInMenu (NotifyElement *event)
{
  return TRUE; /* prevent Thot from putting this element name in the
                  element creation menu */
}

/*----------------------------------------------------------------------
  ResetFontOrPhrase: The element elem is removed.
  ----------------------------------------------------------------------*/
static void ResetFontOrPhrase (Document doc, Element elem)
{
  Element      next, child, first, last;

  if (elem)
    {
      child = TtaGetFirstChild (elem);
      first = last = NULL;
      while (child)
        {
          // next element
          next = child;
          TtaNextSibling (&next);
          TtaRegisterElementDelete (child, doc);
          TtaRemoveTree (child, doc);
          TtaInsertSibling (child, elem, TRUE, doc);
          TtaRegisterElementCreate (child, doc);
          if (first == NULL)
            first = child;
          last = child;
          child = next;
        }
      TtaRegisterElementDelete (elem, doc);
      TtaRemoveTree (elem, doc);
      TtaSetDocumentModified (doc);
      // update the terminal selection
      TtaSelectElement (doc, first);
      if (last != first)
        TtaExtendSelection (doc, last, TtaGetElementVolume (last) + 1);
    }
}

/*----------------------------------------------------------------------
  SplitFontOrPhrase
  Split the element elem
  ----------------------------------------------------------------------*/
static void SplitFontOrPhrase (Document doc, Element elem, Element first,
                               Element last, int firstChar, int lastChar)
{
  Element             next, child, new_ = NULL, el, top = NULL, start = NULL;
  ElementType         elType;
  Language            lang = TtaGetDefaultLanguage ();
  unsigned char      *buffer;
  int                 i, j, k, lg;
  ThotBool            oldStructureChecking;

  if (elem)
    {
      /* split */
      elType = TtaGetElementType (first);
      next = first;
      TtaGiveNextSelectedElement (doc, &next, &j, &k);
      if (first == last)
        {
          if (lastChar < firstChar)
            i = 0;
          else
            i = lastChar - firstChar; // some characters have to be moved
        }
      else
        i =  TtaGetElementVolume (last) + 1 - firstChar;
      if (TtaBreakElement (elem, first, firstChar, TRUE, FALSE))
        {
          // insert after
          oldStructureChecking = TtaGetStructureChecking (doc);
          TtaSetStructureChecking (FALSE, doc);
          TtaNextSibling (&elem);
          // generate the tree of the new structure
          child = TtaGetFirstChild (elem);
          while (child)
            {
              elType = TtaGetElementType (child);
              el = TtaNewElement (doc, elType);
              TtaCopyAttributes (child, el, doc, doc);
              if (new_ == NULL)
                {
                  // top of the new tree
                  TtaInsertSibling (el, elem, TRUE, doc);
                  top = new_ = el;
                }
              else
                {
                  TtaInsertFirstChild  (&el, new_, doc);
                  new_ = el;
                }
              elem = child;
              child = TtaGetFirstChild (child);
            }
          if (i > 0)
            {
              /* there is some text to be copied */
              child = elem;
              lg = TtaGetTextLength (child);
              buffer = (unsigned char*)TtaGetMemory ((lg+1));
              memset (buffer, 0, lg);
              TtaGiveSubString (child, buffer, 1, i);
              TtaSetTextContent (el, buffer, lang, doc);
              TtaFreeMemory (buffer);
              if (i >= lg)
                {
                  // remove the element
                  TtaRegisterElementDelete (child, doc);
                  TtaRemoveTree (child, doc);
                }
              else
                {
                  TtaRegisterElementReplace (child, doc);
                  TtaDeleteTextContent (child, 1, i, doc);
                }
            }
          TtaRegisterElementCreate (top, doc);
          start = el; // new start selection
          while (next)
            {
              el = next;
              if ((j == 0 && k == 0) || k >  TtaGetTextLength (next))
                {
                  // move the complete element
                  TtaGiveNextSelectedElement (doc, &next, &j, &k);
                  TtaRegisterElementDelete (el, doc);
                  TtaRemoveTree (el, doc);
                  TtaInsertSibling (el, top, FALSE, doc);
                  top = el;
                  TtaRegisterElementCreate (top, doc);
                }
              else
                {
                  /* there is some text to be copied */
                  elType.ElTypeNum = HTML_EL_TEXT_UNIT;
                  el = TtaNewElement (doc, elType);
                  lg = TtaGetTextLength (next);
                  buffer = (unsigned char*)TtaGetMemory ((lg+1));
                  memset (buffer, 0, lg);
                  k--;
                  TtaGiveSubString (next, buffer, 1, k);
                  TtaSetTextContent (el, buffer, lang, doc);
                  TtaFreeMemory (buffer);
                  TtaRegisterElementReplace (next, doc);
                  TtaDeleteTextContent (next, 1, k, doc);
                  TtaInsertSibling (el, top, FALSE, doc);
                  top = el;
                  next = NULL;
                }
            }
          TtaSelectElement (doc, start);
          TtaExtendSelection (doc, top, TtaGetElementVolume (top) + 1);
          TtaSetStructureChecking (oldStructureChecking, doc);
          /* mark the document as modified */
          TtaSetDocumentModified (doc);
          elType.ElTypeNum = HTML_EL_TEXT_UNIT;
        }
    }
}

/*----------------------------------------------------------------------
  SetCharFontOrPhrase
  ----------------------------------------------------------------------*/
void SetCharFontOrPhrase (int doc, int elemtype)
{
  Element             first, last, el, parent;
  ElementType         elType, parentType;
  Attribute           attr;
  int                 firstChar, lastChar, i;
  int                 blocktype;
  ThotBool            remove, charlevel;
  ThotBool            oldStructureChecking;

  if (!TtaGetDocumentAccessMode (doc))
    /* document is ReadOnly */
    return;

  TtaGiveFirstSelectedElement (doc, &first, &firstChar, &lastChar);
  if (first == NULL)
    {
      /* no selection available */
      TtaDisplaySimpleMessage (CONFIRM, AMAYA, AM_NO_INSERT_POINT);
      return;
    }

  TtaGiveLastSelectedElement (doc, &last, &i, &lastChar);
  elType = TtaGetElementType (first);
  parent = NULL;
  remove = FALSE;
  // there are block and inline elements for ins and del
  if (elemtype == HTML_EL_ins)
    blocktype = HTML_EL_INS;
  else if (elemtype == HTML_EL_del)
    blocktype = HTML_EL_DEL;
  else
    blocktype = elemtype;
  if (!strcmp (TtaGetSSchemaName (elType.ElSSchema), "HTML"))
    {
      // check if a typed element is selected
      if (elType.ElTypeNum == elemtype || elType.ElTypeNum == blocktype)
        parent = first;
      remove = (first == last &&
                ((firstChar == 0 && lastChar == 0) ||
                (firstChar == 1 && lastChar >= i)));
    }
  else
    elType.ElSSchema = TtaGetSSchema ("HTML", doc);

  if (parent == NULL)
    {
      if ( TtaIsSelectionEmpty ())
        {
          // check if the user wants to close the current element
          parent = TtaGetParent (first);
          parentType = TtaGetElementType (parent);
          i =  TtaGetElementVolume (first);
          if (parentType.ElSSchema == elType.ElSSchema &&
              parentType.ElTypeNum == elemtype &&
              elType.ElTypeNum == HTML_EL_TEXT_UNIT &&
              ((firstChar == 1 && lastChar == 0 && first == TtaGetFirstChild (parent)) ||
               (lastChar >= i && first == TtaGetLastChild (parent))))
            {
              // insert before or after
              TtaOpenUndoSequence (doc, first, last, firstChar, lastChar);
              oldStructureChecking = TtaGetStructureChecking (doc);
              TtaSetStructureChecking (FALSE, doc);
              el = TtaNewElement (doc, elType);
              if (firstChar == 1 && lastChar == 0)
                TtaInsertSibling (el, parent, TRUE, doc);
              else
                TtaInsertSibling (el, parent, FALSE, doc);
              TtaRegisterElementCreate (el, doc);
              TtaSelectElement (doc, el);
              TtaSetStructureChecking (oldStructureChecking, doc);
              TtaCloseUndoSequence (doc);
              /* mark the document as modified */
              TtaSetDocumentModified (doc);
              UpdateContextSensitiveMenus (doc, 1);
              return;
            }
        }
      elType.ElTypeNum = elemtype;
      parent = TtaGetExactTypedAncestor (first, elType);
      // check if the whole selection is included by the same parent
      el = last;
      while (parent && el && el != parent)
        el = TtaGetParent (el);
      if (el == parent)
        remove = TRUE;
    }

  if (parent && remove)
    {
      TtaClearViewSelections ();
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);

      i =  TtaGetElementVolume (last) + 1;
      if (firstChar != 0 && (firstChar > 1 || lastChar < i))
        /* split */
        SplitFontOrPhrase (doc, parent, first, last, firstChar, lastChar);
      else
        {
          attr = NULL;
          TtaNextAttribute (parent, &attr);
          if (attr)
            {
              // don't remove an element with attributes
              TtaChangeTypeOfElement (parent, doc, HTML_EL_Span);
              TtaRegisterElementTypeChange (parent, elemtype, doc);
              TtaSelectElement (doc, parent);
            }
          else
            ResetFontOrPhrase (doc, parent);
        }
      TtaCloseUndoSequence (doc);
    }
  else
    {
      TtaOpenUndoSequence (doc, NULL, NULL, 0, 0);
      if (elemtype == HTML_EL_ins || elemtype == HTML_EL_del)
        {
          oldStructureChecking = TtaGetStructureChecking (doc);
          TtaSetStructureChecking (FALSE, doc);
          elType = TtaGetElementType (first);
          charlevel = (IsCharacterLevelElement (first) ||
                       elType.ElTypeNum == HTML_EL_Basic_Elem);
          // they could be block or inline elements
          if (!charlevel ||
              (first != last && !IsCharacterLevelElement (last)))
            // create a block element
            CreateHTMLelement (blocktype, doc);
          else if (first != last)
            {
              TtaNextSibling (&first);
              if (first != last && !IsCharacterLevelElement (first))
                // create a block element
                CreateHTMLelement (blocktype, doc);
              else
                // create a inline element
                GenerateInlineElement (elemtype, NULL, 0, "", TRUE);
            }
          else
            // create a inline element
            GenerateInlineElement (elemtype, NULL, 0, "", TRUE);
          TtaSetStructureChecking (oldStructureChecking, doc);
        }
      else
        GenerateInlineElement (elemtype, NULL, 0, "", TRUE);
      TtaCloseUndoSequence (doc);
    }

  UpdateContextSensitiveMenus (doc, 1);
}


/*----------------------------------------------------------------------
  CopyLocation
  Store the current URI (document URL + ID/name) into the clipboard
  ----------------------------------------------------------------------*/
void CopyLocation (Document doc, View view)
{
#ifdef _WX
  Element        el;
  ElementType    elType;
  AttributeType  attrType;
  Attribute      attr;
  int            firstSelectedChar, lastSelectedChar, length;
  char           msgBuffer[MaxMsgLength], *name, *documentname;

  strcpy (msgBuffer, DocumentURLs[doc]);
  TtaGiveFirstSelectedElement (doc, &el, &firstSelectedChar,
                               &lastSelectedChar);
  if (el)
    {
      // add the current  ID/name */
      elType = TtaGetElementType (el);
      if (elType.ElTypeNum == HTML_EL_TEXT_UNIT)
        el = TtaGetParent (el);
      name = TtaGetSSchemaName (elType.ElSSchema);
      attrType.AttrSSchema = elType.ElSSchema;
      if (!strcmp (name, "HTML"))
        attrType.AttrTypeNum = HTML_ATTR_ID;
      else if (!strcmp (name, "SVG"))
        attrType.AttrTypeNum = SVG_ATTR_xmlid;
      else if (!strcmp (name, "MathML"))
        attrType.AttrTypeNum = MathML_ATTR_id;
      else if (!strcmp (name, "XLink"))
        attrType.AttrTypeNum = XLink_ATTR_id;
      else
        attrType.AttrTypeNum = XML_ATTR_xmlid;
      attr = TtaGetAttribute (el, attrType);
      if (attr)
        {
          // add the id to document URL
          length = TtaGetTextAttributeLength (attr) + 1;
          name = (char *)TtaGetMemory (length);
          TtaGiveTextAttributeValue (attr, name, &length);
          strcat (msgBuffer, "#");
          strcat (msgBuffer, name);
          TtaFreeMemory (name);
        }
      else if (!strcmp (name, "HTML"))
        {
          // add the name to document URL
          attrType.AttrTypeNum = HTML_ATTR_NAME;
          attr = TtaGetAttribute (el, attrType);
          if (attr)
            {
              length = TtaGetTextAttributeLength (attr) + 1;
              name = (char *)TtaGetMemory (length);
              TtaGiveTextAttributeValue (attr, name, &length);
              strcat (msgBuffer, "#");
              strcat (msgBuffer, name);
              TtaFreeMemory (name);
            }
          else
            {
              attr = NULL;
              // get the target of the enclosing anchor
              el = SearchAnchor (doc, el, &attr, FALSE);
              if (el && attr)
                {
                  length = TtaGetTextAttributeLength (attr) + 1;
                  name = (char *)TtaGetMemory (length);
                  TtaGiveTextAttributeValue (attr, name, &length);
                  if (name[0] == '#')
                    // it's a local reference
                    strcat (msgBuffer, name);
                  else
                    {
                      documentname = (char *)TtaGetMemory (MAX_LENGTH);
                      NormalizeURL (name, doc, msgBuffer, documentname, NULL);
                      TtaFreeMemory (documentname);
                    }
                  TtaFreeMemory (name);
                }
            }
        }
    }

  TtaStringToClipboard ((unsigned char *)msgBuffer, UTF_8);
#endif /* _WX */
}

/*----------------------------------------------------------------------
  PasteLocation
  Paste the clipboard value into the document address
  ----------------------------------------------------------------------*/
void PasteLocation (Document doc, View view)
{
}
