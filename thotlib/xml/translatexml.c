

#include "thot_sys.h"
#include "thot_gui.h"
#include "attribute.h"
#include "document.h"
#include "application.h"
#include "constxml.h"
#include "typexml.h"

#include "constint.h"
#include "typeint.h"

#include "fileaccess_f.h"

#ifdef __STDC__
extern STRING TtaAllocString (unsigned int);
#else  /* __STDC__ */
extern STRING TtaAllocString (n);
#endif /* __STDC__ */

static PtrTranslationSchema TranslationSchemas = NULL;
static int                  NbTranslationSchema = 0;
static int                  CurrentTypeNum      = 0;
static int                  CurrentAttrNum      = 0;
static int                  CurrentValueNum     = 0;   
static SSchema              CurrentSSchema;
static int                  MaxTypeNum;
static int                  MaxAttrNum;
static int                  CurrentTableRank;
#define CURRENT_ATTR_DEF (((PtrSSchema)CurrentSSchema)->SsAttribute[CurrentAttrNum - 1])


/*------------------------------------------------------------------------
  InsertNameInAlpha : inserts the string name in the alphabetically ordered
                      char * table. returns the rank in wich the string
                      was inserted
----------------------------------------------------------------------- */
#ifdef __STDC__
static int InsertNameInAlpha (STRING* table, STRING string, int size)
#else /*__STDC__*/
static int InsertNameInAlpha (table, string, size)
STRING* table;
STRING  string;
int    size;
#endif /*__STDC__*/

{
  int    result = -1;
  int    index = 0;
  int    i= 0;
  STRING s;
  
  s = TtaAllocString (ustrlen (string) + 1);
  ustrcpy (s, string);
  while (result == -1 && index < size)
    {
      if (table[index] == NULL || s [i] == EOS)	
	/* insert s before table[index] */
	result = index; 
      else if (table[index][i] == EOS)
	{
	  index++;
	  i = 0;
	}
      else if (table[index][i] == s [i])
	i++;
      else if (table[index][i] < s [i])
	{
	  index++;
	   i = 0;
	}
      else if (table[index][i] > s [i])
	/* insert s before table[index] */
	result = index;
      else
	printf ("this should never happen\n");
    }
  if (result != -1)
    {
      for (index = size; index > result; index--)
	{
	  table [index] = table [index-1];
	}
      table [result] = s;
      }
  return result;
}

 
/*------------------------------------------------------------------------
  InsertInTable: Insert an entry in the last table
                 The index of Schema used is always NbSchema-1
----------------------------------------------------------------------- */
#ifdef __STDC__
static void InsertInTable (PtrTranslationSchema TSchema, STRING ThotName,STRING XmlName)
#else /*__STDC__*/
static void InsertInTable (TSchema, ThotName, XmlName)
PtrTranslationSchema TSchema;
STRING ThotName;
STRING XmlName;
#endif /*__STDC__*/

{
  CHAR_T        tempName [32];
  ElementType   elType;
  AttributeType atType;
  STRING        theName;
  TYPETABLE     index;
  ThotBool      found;
  int           n = 0;

  /* chooses the name to be inserted */
  if (XmlName[0] == EOS)
    theName = NULL;
  else
    theName = XmlName;
  
  elType.ElSSchema = CurrentSSchema;
  atType.AttrSSchema = CurrentSSchema;
  
  /* is it an element name ? */
  if (CurrentTypeNum < MaxTypeNum)
    {
      elType.ElTypeNum = CurrentTypeNum;
      ustrcpy (tempName, TtaGetElementTypeOriginalName (elType));
      while ((CurrentTypeNum < MaxTypeNum-1) && 
	     (ustrcmp (ThotName, tempName) != 0))
	{
	  CurrentTableRank ++;
	  index = InsertNameInAlpha (&(TSchema->XmlNameTable[1]), tempName, CurrentTableRank) + 1;
	  for (n = 1; n<CurrentTypeNum; n++)
	    if (TSchema->XmlElemNameTable[n] >= index)
	      (TSchema->XmlElemNameTable[n])++;
	  for (n = CurrentTableRank-1; n>=index; n--)
	    TSchema->XmlReverseTable[n+1] = TSchema->XmlReverseTable[n];
	  TSchema->XmlElemNameTable[CurrentTypeNum] = index;
	  TSchema->XmlReverseTable[index] = CurrentTypeNum;
	  CurrentTypeNum++;
	  elType.ElTypeNum = CurrentTypeNum;
	  ustrcpy (tempName, TtaGetElementTypeOriginalName (elType));
	}
    }
  if (CurrentTypeNum < MaxTypeNum)
    if (theName!=NULL)
      {
	/* inserts an element name */
	CurrentTableRank ++;
	index = InsertNameInAlpha (&(TSchema->XmlNameTable[1]), theName, CurrentTableRank) + 1;
	for (n = 1; n<CurrentTypeNum; n++)
	  if (TSchema->XmlElemNameTable[n] >= index)
	    (TSchema->XmlElemNameTable[n])++;
	for (n = CurrentTableRank-1; n>=index; n--)
	  TSchema->XmlReverseTable[n+1] = TSchema->XmlReverseTable[n];
	TSchema->XmlElemNameTable[CurrentTypeNum] = index;
	TSchema->XmlReverseTable[index] = CurrentTypeNum;
	CurrentTypeNum++; 
      }
    else
      {
	TSchema->XmlElemNameTable[CurrentTypeNum]=0;
	CurrentTypeNum++; 
      }
  else
    {
      /* all elements names have already been inserted :*/
      /* is it an attribute name? */
      if (CurrentAttrNum == 0)
	{
	  TSchema->FirstAttr = CurrentTableRank + 1;
	  CurrentAttrNum = 1;
	}
      if (CurrentValueNum == 0)
	{
	  if (CurrentAttrNum < MaxAttrNum)
	    {
	      atType.AttrTypeNum = CurrentAttrNum;
	      ustrcpy (tempName, TtaGetAttributeOriginalName (atType));
	    }
	  while ((CurrentAttrNum < MaxAttrNum) && 
		 (ustrcmp (ThotName, tempName) != 0))
	    {
	      /* inserts a new thot attribute name */
	      CurrentTableRank++;
	      index = (TSchema->FirstAttr) + 
		InsertNameInAlpha (&(TSchema->XmlNameTable[TSchema->FirstAttr]), tempName, (CurrentTableRank - (TSchema->FirstAttr))+1);
	      for (n = 1; n < CurrentAttrNum; n++)
		if (TSchema->XmlAttrNameTable[n].AttrNameRank >= index)
		  (TSchema->XmlAttrNameTable[n].AttrNameRank)++;
	      for (n = CurrentTableRank-1; n>=index; n--)
		TSchema->XmlReverseTable[n+1] = TSchema->XmlReverseTable[n];
	      TSchema->XmlAttrNameTable[CurrentAttrNum].AttrNameRank = index;
	      TSchema->XmlAttrNameTable[CurrentAttrNum].AttrFirstValue = 0;
	      TSchema->XmlAttrNameTable[CurrentAttrNum].AttrNbValue = 0;
	      TSchema->XmlReverseTable[index] = CurrentAttrNum;
	      CurrentAttrNum ++;
	      atType.AttrTypeNum = CurrentAttrNum;
	      ustrcpy (tempName, TtaGetAttributeOriginalName (atType));
	    }
	  if (CurrentAttrNum < MaxAttrNum)
	    if (theName!=NULL)
	      {
		/* inserts a new Xml attribute name */
		CurrentTableRank++;
		index = (TSchema->FirstAttr) + 
		  InsertNameInAlpha (&(TSchema->XmlNameTable[TSchema->FirstAttr]), 
				     theName,
				     (CurrentTableRank - (TSchema->FirstAttr))+1);
		for (n = 1; n < CurrentAttrNum; n++)
		  if (TSchema->XmlAttrNameTable[n].AttrNameRank >= index)
		    (TSchema->XmlAttrNameTable[n].AttrNameRank)++;
		for (n = CurrentTableRank-1; n>=index; n--)
		  TSchema->XmlReverseTable[n+1] = TSchema->XmlReverseTable[n];
		TSchema->XmlAttrNameTable[CurrentAttrNum].AttrNameRank = index;
		TSchema->XmlAttrNameTable[CurrentAttrNum].AttrFirstValue = 0;
		TSchema->XmlAttrNameTable[CurrentAttrNum].AttrNbValue = 0;
		TSchema->XmlReverseTable[index] = CurrentAttrNum;
		CurrentAttrNum++;
	      }
	    else
	      {
		TSchema->XmlAttrNameTable[CurrentAttrNum].AttrNameRank = 0;
		TSchema->XmlAttrNameTable[CurrentAttrNum].AttrFirstValue = 0;
		TSchema->XmlAttrNameTable[CurrentAttrNum].AttrNbValue = 0;
		CurrentAttrNum++;
	      }		
	  else 
	    {
	      /* all attributes names have already been inserted */
	      /* it is an attribute value */
	      CurrentValueNum = 1;
	      CurrentAttrNum = 1;
	      TSchema->FirstAttrValue = CurrentTableRank + 1;
	    }
	}
      if (CurrentValueNum != 0)
	{
	  /* search to which attribute the value belongs */
	  found = FALSE;
	  while (!found && CurrentAttrNum < MaxAttrNum)
	    {
	      if (CURRENT_ATTR_DEF.AttrType == AtEnumAttr)
		{
		  n = 0;
		  while (!found && n < CURRENT_ATTR_DEF.AttrNEnumValues)
		    {
		      found = !ustrcmp (CURRENT_ATTR_DEF.AttrEnumOrigValue[n], ThotName);
		      if (!found) n++;
		    }
		}
	      if (!found) 
		{ 
		  CurrentAttrNum++;
		}
	    }
	  if (found)
	    {
	      if (TSchema->XmlAttrNameTable[CurrentAttrNum].AttrFirstValue == 0)
		{
		  CurrentTableRank++;
		  /* no value defined yet for this attribute => init the attribute table */
		  CurrentValueNum = CurrentTableRank;
		  (TSchema->XmlAttrNameTable[CurrentAttrNum].AttrFirstValue) = CurrentTableRank;
		  
		  (TSchema->XmlAttrNameTable[CurrentAttrNum].AttrNbValue) = CURRENT_ATTR_DEF.AttrNEnumValues;
		  for (;CurrentTableRank < TSchema->XmlAttrNameTable[CurrentAttrNum].AttrFirstValue + 
			 TSchema->XmlAttrNameTable[CurrentAttrNum].AttrNbValue; CurrentTableRank++)
		    TSchema->XmlNameTable[CurrentTableRank] = NULL;
		}
	      /* inserts the attribute value name */
	      n = n + TSchema->XmlAttrNameTable[CurrentAttrNum].AttrFirstValue;
	      TSchema->XmlNameTable[n] = TtaGetMemory (32);
	      ustrcpy ( TSchema->XmlNameTable[n], theName);
	    }
	}
    }
}
		  
		   
/*------------------------------------------------------------------------
  NewTranslationSchema: Creates a new translation Schema
----------------------------------------------------------------------- */
#ifdef __STDC__
static PtrTranslationSchema  NewTranslationSchema(SSchema sSchema)
#else /*__STDC__*/
static PtrTranslationSchema  NewTranslationSchema(sSchema)
SSchema sSchema;
#endif /*__STDC__*/
{
  PtrTranslationSchema pts;
  int i;
  
  NbTranslationSchema++;

  pts = TtaGetMemory (sizeof (TranslationSchema));
  for (i=0; i<MAXNTYPE; i++)
    {
      pts->XmlElemNameTable[i] = 0;
      pts->XmlAttrNameTable[i].AttrNameRank = 0;
      pts->XmlAttrNameTable[i].AttrFirstValue = 0;
      pts->XmlAttrNameTable[i].AttrNbValue = 0;
      pts->XmlNameTable[i] = NULL;
    }
  pts->FirstAttr = 0;
  pts->FirstAttrValue = 0;
  pts->SchemaName=TtaStrdup(TtaGetSSchemaName(sSchema));
  pts->Next = TranslationSchemas;
  TranslationSchemas = pts;

  MaxTypeNum = ((PtrSSchema)sSchema)->SsNRules + 1;
  MaxAttrNum = ((PtrSSchema)sSchema)->SsNAttributes + 1;
  CurrentTypeNum = 1;
  CurrentAttrNum = 0;
  CurrentValueNum = 0;
  CurrentTableRank = 0;
  return pts;
}

/*------------------------------------------------------------------------
  LoadTable: Try to open file and then read it
             return the index int the main table or -1 in case of error
----------------------------------------------------------------------- */

#ifdef __STDC__
static PtrTranslationSchema LoadTable(SSchema sSchema)
#else /*__STDC__*/
static PtrTranslationSchema LoadTable(Schema)
SSchema sSchema;
#endif /*__STDC__*/

{
  BinFile       file;
  int           i=0;
  int           f=0;
  ThotBool       ok = FALSE;
  UCHAR_T       c = EOS;
  CHAR_T    filename[200];
  CHAR_T    paths[1000];
  CHAR_T    thotbuf[50];
  CHAR_T    xmlbuf[50];
  CHAR_T   *currentbuf=thotbuf;
  PtrTranslationSchema pts;

  /* Gets all SchemasPaths */
  TtaGetSchemaPath(paths,1000);
  file = NULL;
#ifdef DEBUG_IV
  printf("Paths:%s.\n",paths);
#endif
  while ((paths[i] != EOS) &&( !ok))
    {
      switch (paths[i])
	{
	case ':':
	  /* Tries to open the file or go to next path */
	  filename[f++]='/';
	  filename[f++]=EOS;
	  ustrcat(filename,TtaGetSSchemaName(sSchema));
	  ustrcat(filename, X_EXT);
	  if (TtaFileExist(filename))
	    {
	      file = TtaReadOpen (filename);
	      ok = (file != NULL);
	    }
	  else {
	    f=0;
	    i++;
	  }
	  break;
	default:
	    filename[f]=paths[i];
	    i++;
	    f++;
	    break;
	}
    }
  if (!ok)
      return(NULL);
  else
    {
      char inoct = -1;
      char icode = 0;
      /* Insert a new TXTable */
      pts = NewTranslationSchema(sSchema);

      i=0;
      thotbuf[0]=EOS;
      xmlbuf[0]=EOS;
      
      while ( ( c = (unsigned char) fgetc (file) ) != (unsigned char) EOF )
	{
	  switch (c)
	    {
            case '\\':
              inoct = 0;
	      for (inoct = 0; inoct<3; inoct ++)
		{
		  c = (unsigned char) fgetc (file);
		  icode = (icode * 8) + (c - '0');
		}
	      currentbuf[i++] = (char)icode;
	      break;
	    case EOL:
	      xmlbuf[i]='\0';
	      InsertInTable (pts, thotbuf,xmlbuf);
	      currentbuf=thotbuf;
	      i=0;
	      break;
	    case ':':
	      thotbuf[i]='\0';
	      currentbuf=xmlbuf;
	      i=0;
	      break;
	    case SPACE:
	    case TAB:
	      break;
	    default:
	      currentbuf[i++]=c;
	      break;
	    }
	}
    }
  return pts;
}


/**** Extern functions ***/

/*------------------------------------------------------------------------
  NameXmlToThot: returns the element or attr type number or an attribute
                 value corresponding to a given name
	  schema : the schema of the searched type
         xmlName : the xml Name
       elTypeNum : the type number of the element 
                   if an attribute type or value is searched else 0 
     attrTypeNum : the type number of the attribute if an enumerated attr
                   value is searched else 0
----------------------------------------------------------------------- */
#ifdef __STDC__
int NameXmlToThot(SSchema schema, STRING xmlName, int elTypeNum, int attrTypeNum)
#else /*__STDC__*/
int NameXmlToThot(schema, xmlName, elTypeNum, attrTypeNum)
SSchema schema;
char *xmlName;
int elTypeNum;
int attrTypeNum;
#endif /*__STDC__*/
{
  PtrTranslationSchema curTS;
  TYPETABLE            i = 0;
  TYPETABLE            index, first, end;
  int                  result, j;
  ElementType          elType;
  AttributeType        attrType;
  ThotBool             found = FALSE;
  SRule               *pRule;

  result = 0;
  CurrentSSchema = schema;
  elType.ElSSchema = schema;
  attrType.AttrSSchema = schema;
  attrType.AttrTypeNum = attrTypeNum;
  /* searches the translation schema */
  curTS = TranslationSchemas;
  while (curTS != NULL && ustrcmp (curTS->SchemaName, TtaGetSSchemaName(schema)))
    curTS = curTS->Next;
  if (curTS == NULL)
    curTS = LoadTable (schema);
  if (curTS != NULL)
    {
      if (attrTypeNum == 0)	
	{
	  if (elTypeNum == 0)
	    {
	      /* searching an element type Number */
	      first = 1;
	      end = curTS->FirstAttr;
	    }
	  else
	    {
	      /* searching an attribute type Number */
	      first = curTS->FirstAttr;
	      end = curTS->FirstAttrValue;
	    
	    }
	  index = first;
	  while (result == 0 && index < end)
	    {
	      if (curTS->XmlNameTable[index][i] == EOS && xmlName [i] == EOS)
		result = index;
	      else if (curTS->XmlNameTable[index][i] == xmlName [i])
		i++;
	      else if (curTS->XmlNameTable[index][i] < xmlName [i])
		{
		  index++;
		  i = 0;
		}
	      else
		index = end;
	    }
	}
      else
	/* searching an attribute value */
	{
	  first = curTS->XmlAttrNameTable[(TYPETABLE)attrTypeNum].AttrFirstValue;
	  end = first + curTS->XmlAttrNameTable[(TYPETABLE)attrTypeNum].AttrNbValue;
	  index = first;
	  while (result == 0 && index < end)
	    {
	      if (curTS->XmlNameTable[index] == NULL || 
		  ustrcmp (curTS->XmlNameTable[index], xmlName) != 0)
		index++;
	      else
		result = index;
	    }
	}
      return curTS->XmlReverseTable[result];
    }
  else   /* No translation table */
    {
      if (attrTypeNum == 0)	
	if (elTypeNum == 0)
	  {
	    /* searching an element type Number */
	    TtaGiveTypeFromOriginalName(&elType, xmlName);
	    result = elType.ElTypeNum;
	  }
	else
	  {
	    result = 0;
	    pRule = &(((PtrSSchema)schema)->SsRule[elTypeNum-1]);
	    /* Searching manualy the attribute */
	    while (result < (((PtrSSchema)schema)->SsNAttributes+1) && !found)
	      {
		result++;
		/* Checking if it is a local or global attribute */
		if (((PtrSSchema)schema)->SsAttribute[result - 1].AttrGlobal)
		  /* Global attribute */
		  found = (!ustrcmp (xmlName, ((PtrSSchema)schema)->SsAttribute[result - 1].AttrOrigName));

		else
		  {/* Local Attribute */
		    /* checking if the attribute is legal for the elem type */
		    for (j = 0; j < pRule->SrNLocalAttrs && pRule->SrLocalAttr[j]!=result; j++);
		    if (j < pRule->SrNLocalAttrs)
		      /* the attribute is legal : checking the name */
		      found = (!ustrcmp (xmlName, ((PtrSSchema)schema)->SsAttribute[result - 1].AttrOrigName));
		    
		  }
	      }
	    if (!found) result = 0;
	  }
      else
	{
	  /* searching the attribute value */
	  result = TtaGetAttributeValueFromOriginalName (xmlName, attrType);
	}
      return result;
    }
}

/*------------------------------------------------------------------------
  NameThotToXml : Return the Xml name of a thot type (or attr value)
           schema : the schema of the type which name is searched
        elTypeNum : the type number of the element which name is searched
                    or that have the attribute which the name is searched
      attrTypeNum : the type of the attribute which name or value 
                    is searched ( 0 if an element name is searched)
	  attrVal : The name of the attibute enum value is searched 
                    (0 if an attribute name is searched)       
----------------------------------------------------------------------- */
#ifdef __STDC__
STRING NameThotToXml(SSchema schema, int elTypeNum, int attrTypeNum, int attrVal)
#else /*__STDC__*/
STRING NameThotToXml(schema, elTypeNum, attrTypeNum, attrVal)
SSchema schema;
int elTypeNum;
int attrTypeNum;
int attrVal;
#endif /*__STDC__*/
{
  PtrTranslationSchema curTS;
  TYPETABLE            i;
  STRING               res;
  AttributeType        attrType;
  ElementType          elType;

  CurrentSSchema = schema;
  elType.ElSSchema = schema;
  elType.ElTypeNum = elTypeNum;
  attrType.AttrSSchema = schema;
  attrType.AttrTypeNum = attrTypeNum;
  i = 0;
  res = NULL;
  /* searches the translation schema */
  curTS = TranslationSchemas;
  while (curTS != NULL && ustrcmp (curTS->SchemaName, TtaGetSSchemaName(schema)))
    curTS = curTS->Next;
 
  if (curTS == NULL)
    curTS = LoadTable (schema);

  if (curTS != NULL)
    if (attrTypeNum != 0)
      if (attrVal != 0)
	/*searching an attr value */
	{
	  i = (curTS->XmlAttrNameTable[(TYPETABLE)attrTypeNum].AttrFirstValue);
	  if (i != 0)
	    {
	      i = i + attrVal - 1;
	      res = curTS->XmlNameTable[i];
	    }
	}
      else
	{
	  /*searching an attribute name */
	  i = curTS->XmlAttrNameTable[(TYPETABLE)attrTypeNum].AttrNameRank;
	  if (i != 0)
	    res = curTS->XmlNameTable[i];
	}
    else
      {
	/*searching an element name */
	i = curTS->XmlElemNameTable[(TYPETABLE)elTypeNum];
	if (i != 0)
	  res = curTS->XmlNameTable[i];
      }
  else
    if (attrTypeNum != 0)
      if (attrVal != 0)
	/*searching an attr value */
	res = TtaGetAttributeValueName(attrType,attrVal); 
      else 
	/*searching an attribute name */
	res = TtaGetAttributeOriginalName(attrType); 
    else 
      res = TtaGetElementTypeOriginalName(elType);      
    
  return res;
}




#ifdef DEBUGrr
void PrintTable ()
{
  int i;
  printf ("Thot Table :\n");
  for (i=0;i<TXTables[0]->TNbElem;i++) printf ("%s %d\n",TXTables[0]->ThotTable[i]->Name, TXTables[0]->ThotTable[i]->Ref);
  printf ("Xml  Table :\n");
  for (i=0;i<TXTables[0]->XNbElem;i++) printf ("%s %d\n",TXTables[0]->XmlTable[i]->Name, TXTables[0]->XmlTable[i]->Ref);
}
#endif

/* Search the Thot and Xml Name in alphabetical order by dichotomie*/
/*int   SearchXmlIndex2(Entry *Table[MAXNAME], char *Name, int Min, int Max)
  {
  int Middle;
  int i=0;
  
  printf ("%s  min:%d, max:%d\n",Name,Min,Max);
  if ((Max==Min)||(Max==(Min+1))) 
  
  return (Max);
  
  Middle=(Min+Max)/2;
  
  i=strcmp(Table[Middle]->Name,Name);
  if (i<0) i=-1;
  if (i>0) i=1;
  
  switch(i)
  {
  case 0: return (Middle);
  break;
  case -1: return (SearchXmlIndex(Table,Name,Middle,Max));
  break;
  case 1:return (SearchXmlIndex(Table,Name,Min,Middle));
  break;
  }
  }*/
