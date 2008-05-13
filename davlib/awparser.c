/* --------------------------------------------------------
 ** Parser functions for the awareness protocol. 
 **
 ** Author : Manuele Kirsch Pinheiro
 ** Project CEMT - II/UFRGS - Opera Group/INRIA
 ** Finance by CNPq(Brazil)/INRIA(France) Cooperation
 **
 ** Date : 02 / Nov / 2001
 **
 ** $Id$
 ** $Date$
 ** $Author$
 ** $Revision$
 ** $Log$
 ** Revision 1.6  2008-05-13 09:30:27  kia
 ** More char* fixes
 **
 ** Revision 1.5  2003/11/19 12:33:16  gully
 ** Compilation fix (webdav support)
 **
 ** S. GULLY
 **
 ** Revision 1.4  2002/06/12 10:29:07  kirschpi
 ** - adjusts in code format
 ** - new commentaries
 ** - small fixes
 ** Manuele
 **
 ** Revision 1.3  2002/06/03 14:37:42  kirschpi
 ** The name of some public functions have been changed to avoid conflic with
 ** other libraries.
 **
 ** Revision 1.2  2002/05/31 17:59:19  kirschpi
 ** Functions to give to user some informations about active locks
 ** (a basic awareness support) when the user load or exit a document.
 **
 ** Revision 1.2  2002/03/28 16:19:43  kirschpi
 ** New function AwParser_searchInTree to search a value
 ** of a given element.
 **
 ** Revision 1.1  2001/11/02 16:32:39  kirschpi
 ** Initial revision
 **
 **
 ** --------------------------------------------------------
 */

#include "awparser.h"

/* --------------------------------------------------------
 * this function returns the number of characters that are
 * not spaces or '\n' in the string.
 *
 * Parameters :
 *      char *data : string to verify
 * Return :
 *      int : number of characters, 0 if failed
 * --------------------------------------------------------
 */

extern int verifyString (char *data) 
{
    int i = 0;
    int sum = 0;

    for (i=0; (unsigned)i<strlen (data); i++) 
     {
        if ( isalnum(data[i]) && !isspace(data[i]) )
            sum++;
     }
    return sum;
}



/* --------------------------------------------------------
 * this function creates a new node, with the 
 * informations indicated by the parameters.
 *
 * Parameters :
 *      char *data : node information
 *      char **attr : vector of the node attributes
 *      int len : data vector length
 *      int type : node type (XML_ELEMENT or TEXT_ELEMENT)
 * Return :
 *      AwNode : the created node
 *               (NULL if an error is produced)
 * --------------------------------------------------------
 */
AwNode *AwParser_makeNode (char *data, char **attr,int len, int type) 
{
   AwNode *node=NULL;
   AwString info,val;
   AwPair *pair=NULL;
   int i,flag=1;
   

   info = AwString_new (len+1);           /* creating a new node */
   if (AwString_set (info,data)<0)
       return NULL;
   
   node = AwNode_new(NUM_OF_NODES,NUM_OF_NODES,len+1);   
   if (AwNode_setInfo (node,info) < 0 || AwNode_setType (node,type)< 0) 
    {          
       AwNode_delete (node);
       return NULL;
    }
                   
   AwString_delete (info);

   if (attr!=NULL) 
    {
       for (i = 0; attr[i]; i += 2) 
        {           /* set the node's attributes */
               
           info = AwString_new (strlen(attr[i]));
           val = AwString_new (strlen(attr[i+1]));

           attr[i][strlen(attr[i])]='\0';
           attr[i+1][strlen(attr[i+1])]='\0';
           
           flag *= AwString_set (info,(char *)attr[i]);
           flag *= AwString_set (val,(char *)attr[i+1]);
#ifdef DEBUG
           fprintf (stderr,"attr: %s (%s/%d) = %s (%s/%d)\n", \
                                  AwString_get(info),attr[i],strlen(attr[i]), \
                                  AwString_get(val),attr[i+1],strlen(attr[i+1]));
#endif                            
                           
           pair = AwPair_new (AwString_len(info),AwString_len(val));
           flag *= AwPair_setName(pair,info);
           flag *= AwPair_setValue(pair,val);

           AwString_delete (info);
           AwString_delete (val);
           
           if (flag<0) 
            {                        /* some error have been produced */ 
               AwPair_delete (pair);
               AwNode_delete (node);
               return NULL;            
            }
           
           AwNode_putAttribute (node,pair);
        }
    }

   return node;
}


/* --------------------------------------------------------
 * this function treats the text elements parsered.
 *
 * Parameters :
 *      void *userData : user data
 *      XML_char *s : data readed
 *      int len : s vector length
 * --------------------------------------------------------
 */
extern void AwParser_textElement (void *userData,const XML_Char *s,int len) 
{
    AwTree *tree = (AwTree *)userData;
    AwNode *node;
    char *data;
#ifdef DEBUG
    int i;
#endif

     data = (char *) AW_CALLOC (len+1,sizeof (char));
     strncpy (data,s,len);
     data[len]='\0';

     if (verifyString(data)<=0)                         /* ignore lines like "\n" */
           return;

     node = AwParser_makeNode (data,NULL,len,TEXT_ELEMENT);      /* create the new node */

     AwTree_putNewNode(tree,node);                      /* put the new node in the tree */

#ifdef DEBUG
     for (i = 0; i < AwNode_getLevel(node); i++)        /* show it */
            fprintf(stderr,"\t");
     fprintf (stderr,"%s\n",AwNode_getInfo(node));
#endif

     AW_FREE (data);

}


/* --------------------------------------------------------
 * this function treats the start elements parsered.
 *
 * Parameters :
 *      void *userData : user data
 *      char *name : element readed
 *      char **attr : element attributes
 * --------------------------------------------------------
 */
extern void AwParser_startElement(void *userData, const char *name, const char **attr) 
{
    AwTree *tree = (AwTree *)userData;
    AwNode *node;
#ifdef DEBUG
    AwPair *pair;
    int i;
#endif              

    if (verifyString((char *)name)<=0)                  /* ignore lines like "\n" */
           return;

    node = AwParser_makeNode ((char *)name, (char **)attr, strlen(name), XML_ELEMENT);

    AwTree_putNewNode(tree,node);                      /* put the new node in the tree */

#ifdef DEBUG
    for (i = 0; i < AwNode_getLevel(node); i++)        /* show it */
        fprintf(stderr,"\t");
    fprintf (stderr,"%s\n", AwNode_getInfo(node));
    AwNode_resetAttributes(node);
    pair = AwNode_nextAttribute(node);
    while (pair!=NULL) 
     {
        for (i = 0; i < AwNode_getLevel(node) + 1 ; i++)
            fprintf(stderr,"\t");
        fprintf (stderr,"%s = %s \n",AwPair_getName(pair),AwPair_getValue(pair));
        pair = AwNode_nextAttribute(node);
     }
#endif              

}



/* --------------------------------------------------------
 * this function treats the end elements parsered.
 *
 * Parameters :
 *      void *userData : user data
 *      XML_char *name : element readed
 * --------------------------------------------------------
 */
extern void AwParser_endElement(void *userData, const char *name) 
{
    AwTree *tree = (AwTree *)userData;
    AwNode *last = NULL;

    if (verifyString((char *)name)<=0)                 /* ignore lines like "\n" */
           return;

    last = AwTree_getLast(tree);
    if (AwNode_getType(last)!=XML_ELEMENT)
     {                                           /* with text element, we need to get up twice */
                                                 /* to reach the last xml element */       
#ifdef DEBUG 
        fprintf (stderr,"going up twice\n");
#endif      
        AwTree_up(tree);                                
     }   

     AwTree_up(tree);
}



/* --------------------------------------------------------
 * this function frees the tree and all its elements.
 *
 * Parameters :
 *      AwTree *tree : tree to be removed
 * --------------------------------------------------------
 */
extern void AwParser_freeTree (AwTree *tree) 
{
    AwParser_freeNode (AwTree_getRoot(tree));
    AwTree_delete (tree);
}




/* --------------------------------------------------------
 * this function frees a node and all its children.
 *
 * Parameters :
 *      AwNode *node : node to be removed
 * --------------------------------------------------------
 */
extern void AwParser_freeNode (AwNode *n) 
{
    AwNode *child=NULL;
    AwPair *atr=NULL;

    if (n==NULL)
            return;


    AwNode_resetAttributes(n);
    atr = AwNode_nextAttribute(n);
    while (atr!=NULL) 
     {
#ifdef DEBUG        
        fprintf (stderr,".");
#endif  
        AwPair_delete (atr);
        atr = AwNode_nextAttribute(n);
     }

    AwNode_resetChildren (n);
    child = AwNode_nextChild(n);
    while (child!=NULL) 
     {
       AwParser_freeNode(child);
       child = AwNode_nextChild(n);
     }

    AwNode_delete(n);
#ifdef DEBUG        
    fprintf (stderr,".");
#endif  
}



/* --------------------------------------------------------
 * this function mount a awareness protocol message based
 * in the tree passed in the parameter "tree".
 * 
 * Parameters :
 *      AwTree *tree : the tree that will generate the 
 *                      protocol message
 * Return :
 *      AwString : the message to be used in the awareness
 *                      protocol.
 * --------------------------------------------------------
 */
extern AwString AwParser_mountMessage (AwTree *tree) 
{
    return (AwParser_mountElement(AwTree_getRoot(tree)));        
}
        


/* --------------------------------------------------------
 * this function mount a string XML for the node passed in 
 * the parameter "node".
 * 
 * Parameters :
 *      AwNode *node : the node that will generate the 
 *                      XML element returned.
 * Return :
 *      AwString : the XML element for the node for use in 
 *                      the awareness protocol.
 *                 (NULL if some problem occurs)        
 * --------------------------------------------------------
 */
extern AwString AwParser_mountElement (AwNode *node) 
{
    AwNode *child;
    AwPair *atr;
    int flag=1;

    AwString s,ptr,ptr2;
    AwString elem;

#ifdef DEBUG
    fprintf (stderr,"Mounting string for node %s and its children\n",AwNode_getInfo(node));
#endif    
    
    if (AwNode_getType(node)==XML_ELEMENT) 
     {    /* if is a xml_element, introduce the < */
#ifdef DEBUG
        fprintf (stderr,"node is a XML element\n");
#endif    
        s = AwString_new (2);    
        flag *= AwString_set (s,"<");
        elem = AwString_cat (s,AwNode_getInfo(node));   
        flag *= AwString_delete(s);
     }
    else
     {
#ifdef DEBUG
        fprintf (stderr,"node is a text element (size %d)\n",AwString_len(AwNode_getInfo(node)));
#endif    
        elem = AwString_new (AwString_len(AwNode_getInfo(node)));
        flag *= AwString_copy (elem,AwNode_getInfo(node));
     }
    
    if (flag<0 || elem==NULL)                   /* in case of problems, exits */
            return NULL;
    
    
    AwNode_resetAttributes(node);                       /* introduce the node attributes */
    atr = AwNode_nextAttribute(node);
    while (atr!=NULL) 
     {
#ifdef DEBUG
        fprintf (stderr,"node attribute %s=%s\n",AwPair_getName(atr),AwPair_getValue(atr));
#endif    

        ptr = elem;
        s = AwString_new (2);    
        flag *= AwString_set (s," ");   
        elem = AwString_cat (ptr,s);
        flag *= AwString_delete (ptr);        
        flag *= AwString_delete (s);        
        
        ptr = elem;    
        elem = AwString_cat (ptr,AwPair_getName(atr));
        flag *= AwString_delete (ptr);
        
        ptr = elem;
        s = AwString_new (3);    
        flag *= AwString_set (s,"=\'");         
        elem = AwString_cat (ptr,s);
        flag *= AwString_delete (ptr);
        flag *= AwString_delete (s);        
        
        ptr = elem;
        elem = AwString_cat (ptr,AwPair_getValue(atr));
        flag *= AwString_delete (ptr);

        ptr = elem;
        s = AwString_new (2);    
        flag *= AwString_set (s,"\'");  
        elem = AwString_cat (ptr,s);
        flag *= AwString_delete (ptr);        
        flag *= AwString_delete (s);        
        
        if (flag<0 || elem==NULL)
                return NULL;
        
        atr = AwNode_nextAttribute(node);
     }

    if (AwNode_getType(node)==XML_ELEMENT) 
     {            /* introduce the ">" in the xml elements */
#ifdef DEBUG
        fprintf (stderr,"closing >\n");
#endif    
        ptr = elem;    
        s = AwString_new (2);    
        flag *= AwString_set (s,">");
        elem = AwString_cat (ptr,s);
        flag *= AwString_delete (ptr);
        flag *= AwString_delete (s);        
        if (flag<0 || elem==NULL)
                return NULL;
     } 

    
    AwNode_resetChildren (node);                     /* introduce data from children elements */
    child = AwNode_nextChild(node);
    while (child!=NULL) 
     {
#ifdef DEBUG
        fprintf (stderr,"node child %s\n",AwNode_getInfo(child));
#endif    
        ptr2 = AwParser_mountElement(child);
        ptr = elem;
        elem = AwString_cat (ptr,ptr2);
        flag *=  AwString_delete (ptr);
        flag *=  AwString_delete (ptr2);
        if (flag<0 || elem==NULL)
                return NULL;
        
        child = AwNode_nextChild(node);
     }

    
    if (AwNode_getType(node)==XML_ELEMENT) 
     {            /* close, if it is a XML element */
#ifdef DEBUG
        fprintf (stderr,"closing xml element\n");
#endif    
        s = AwString_new (3);    
        flag *= AwString_set (s,"</");
        ptr = elem;
        elem = AwString_cat (ptr,s);
        flag *= AwString_delete(ptr);
        flag *= AwString_delete (s);        
        
        ptr = elem;
        elem = AwString_cat (ptr,AwNode_getInfo(node));
        flag *= AwString_delete(ptr);

        s = AwString_new (2);    
        flag *= AwString_set (s,">");
        ptr = elem;
        elem = AwString_cat (ptr,s);
        flag *= AwString_delete(ptr);
        flag *= AwString_delete (s);        
        
        if (flag<0 || elem==NULL)
                return NULL;
     }

    
#ifdef DEBUG
    fprintf (stderr,"done %s\n",elem);
#endif    
    return (elem);
}



/* --------------------------------------------------------
 * this function "umount" a awareness protocol message and
 * returns a tree with the message information.
 * 
 * Parameters :
 *      AwString message : the awareness protocol message
 * Return :
 *      AwTree * : the tree with awareness protocol message
 *                      information.
 *                 (NULL if an allocation problem occurs)
 * Note :
 *      Even when a parser error occurs, a tree with the
 *      information already parsed is returned.
 * --------------------------------------------------------
 */
extern AwTree *AwParser_umountMessage (AwString message) 
{
   AwTree *tree = NULL;
   XML_Parser parser;
   char *input;

   tree = AwTree_new ();
   if (tree != NULL) 
    {
#ifdef DEBUG
       fprintf (stderr,"Creating XML parser for %s \n",(char *) message);
#endif       
       parser = XML_ParserCreate(NULL);
       XML_SetUserData(parser, tree);
       XML_SetElementHandler(parser, AwParser_startElement, AwParser_endElement);
       XML_SetCharacterDataHandler(parser,AwParser_textElement);

       input = AwString_get (message);
       /*do {*/
           if (!XML_Parse(parser, input, strlen(input), 0 )) 
            {
               fprintf(stderr, "%s at line %d\n", \
                              XML_ErrorString(XML_GetErrorCode(parser)), \
                              XML_GetCurrentLineNumber(parser));
            }
       /*while ();*/

#ifdef DEBUG
       fprintf (stderr,"Message parsed\n");
#endif       
       XML_ParserFree(parser);
       AW_FREE(input);
    }   
   return tree;
}


/* -----------------------------------------------------------
 * This function searches the value of an element in a XML
 * tree. It returns a new string with the value or NULL.
 * ----------------------------------------------------------- */
extern char * AwParser_searchInTree (AwTree *tree, const char *elem) 
{
    AwString pattern;
    AwString value;
    AwNode * node=NULL, *father=NULL;
    char *cvalue = NULL;
    
    if (tree && elem) 
     {
        pattern = AwString_new (strlen(elem)+1);
        AwString_set (pattern,elem);
        
        /*if the pattern "elem" is in the node,
         *then its value is in its child */
        father =  AwTree_search(AwTree_getRoot(tree),pattern);
        if (father) 
         {
                node = AwNode_nextChild (father);
                AwNode_resetChildren (father);
         }
        if (node) 
         {
                value = AwNode_getInfo (node);
                cvalue = AwString_get(value);
                AwString_delete(value);
         }
        AwString_delete(pattern);
     }

    return cvalue;
}



