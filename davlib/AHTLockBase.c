/* ----------------------------------------------------------- 
** Functions to manipulate the local WebDAV locks base.
**
** Author : Manuele Kirsch Pinheiro 
** Email: Manuele.Kirsch_Pinheiro@inrialpes.fr / manuele@inf.ufrgs.br
** Project CEMT - II/UFRGS - Opera Group/INRIA
** Financed by CNPq(Brazil)/INRIA(France) Cooperation
**
**
** NOTES:
** 1) Functions that known/manipulate the file structure are:
**          LockLine_new, LockLine_readline,LockLine_writeline and removeFromBase.
**
** 2) Functions that deal with time format are:
**          LockLine_newObject and searchLockBase
** ----------------------------------------------------------- 
*/


#include "AHTLockBase.h"
#include "fileaccess.h"


/* ------------------------------------------------------------------ 
 * This function will that date like this "Fri Mar 29 18:36:47 2002" 
 * and convert it in a time_t.
 * Note: this fate format is the output from ctime, and we hope
 *       that month names will be in english
 * ------------------------------------------------------------------       
 */
PUBLIC time_t strtotime (char *time) 
{
    const char * months[12] = { "Jan","Feb","Mar","Apr","May","Jun", 
	                  "Jul","Aug","Sep","Oct","Nov","Dec" };	
    char copy[128];
    time_t t1;
    struct tm t2;
    char * ptr1;
    char * ptr2;
    int i;
    int mes,dia,ano,hora,min,seg;

    mes = dia = ano = hora = min = seg = 0;
    
    if (!time || !(*time)) return 0;
    strcpy (copy,time);
    
    ptr1 = (char *)strchr ((const char*)copy,' ');
    while (ptr1 && *ptr1 && *ptr1==' ') ptr1++;
 
    /* month */
    ptr2 = strchr (ptr1,' ');
    if (!ptr2) return 0;
    *ptr2=EOS;
    ptr2++;
    
    for (i=0;i<12;i++) 
     {
        if (!strcasecomp (months[i],ptr1)) 
	   mes = i;
     }
    
    /* day */
    ptr1 = ptr2; 
    while (ptr1 && *ptr1 && *ptr1==' ') ptr1++;
    ptr2 = strchr (ptr1,' ');
    if (!ptr2) return 0;
    *ptr2=EOS;
    ptr2++;
 
    dia = atoi(ptr1);

    /* hour */
    ptr1 = ptr2; 
    while (ptr1 && *ptr1 && *ptr1==' ') ptr1++;
    ptr2 = strchr (ptr1,':');
    if (!ptr2) return 0;
    *ptr2=EOS;
    ptr2++;
     
    hora = atoi (ptr1);

    /* minute */
    ptr1 = ptr2; 
    ptr2 = strchr (ptr1,':');
    if (!ptr2) return 0;
    *ptr2=EOS;
    ptr2++;
   
    min = atoi (ptr1);
   
    /* second */
    ptr1 = ptr2; 
    ptr2 = strchr (ptr1,' ');
    if (!ptr2) return 0;
    *ptr2=EOS;
    ptr2++;
   
    seg = atoi (ptr1);
 
    /* year */ 
    ptr1 = ptr2; 
    while (ptr1 && *ptr1 && *ptr1==' ') ptr1++;
    
    ano = atoi (ptr1); 
   

    t2.tm_sec = seg;
    t2.tm_min = min;
    t2.tm_hour= hora;
    t2.tm_mday = dia;
    t2.tm_mon = mes;
    t2.tm_year = ano - 1900;
    t2.tm_isdst = -1;

    t1 = mktime(&t2);

    return t1;
}



/* -------------------------------------------------------------
 * This function creates a new date string with the format
 * "Fri Mar 29 18:36:47 2002" from the time_t parameter.
 * -------------------------------------------------------------
 */ 
PUBLIC char * timetostr (time_t t) 
{
    char s[512];
    char *ptr = ctime (&t);
    int i=0;
    for (i=0;i< (int)strlen(ptr) && *(ptr+i)!='\n';i++) 
     {
        s[i] = *(ptr+i);
     }
    s[i]=EOS;
    ptr = (char *)calloc (i+1,sizeof(char));
    strcpy(ptr,s);
    /*ptr = NULL;
    StrAllocCopy (ptr,s);*/
    return ptr;
}
	

/* ----------------------------------------------------------- 
 * Create a LockLine object with the informations found in
 * line string. This string should have the 'lock base' format.
 * 
 * That is :
 * LINE    ::= URI SEP LOCK SEP DEPTH SEP TIME SEP ITIME
 * URI     ::= relative-uri    ;example: /foo/bar/file.html or /foo
 * SEP     ::= \t              ;separator - a tab char
 * LOCK    ::= lock-token      ;as defined in RFC2518
 * TIME    ::= timeout         ;as defined in RFC2518
 * ITIME   ::= initial-time    ;RFC822
 *
 * Returns : a new LockLine * object, if succeed, NULL if don't.
 *           
 * ----------------------------------------------------------- */
LockLine * LockLine_new (const char * aline) 
{
    char * url = NULL;
    char * lock = NULL ;
    char * timeout = NULL;
    char   depth = ' ';
    char * itime = NULL;
    LockLine * me = NULL;
    char * ptr = NULL;
    char * last = NULL;
    char * line = NULL;
    
    if (aline && *aline) 
     {
        
        StrAllocCopy (line,aline);
    
        /* line structure */
        /* url lock depth timeout itime */
        /* BREAK_CHAR use to be \t */
        last = line;
        if ( (ptr = strchr (last,BREAK_CHAR)) != NULL ) 
         {
            *ptr = EOS;
            url = line;
            lock = ++ptr;
            last = lock;
         }
    
        if ( (ptr = strchr (last,BREAK_CHAR)) != NULL ) 
         {
            *ptr = EOS;
            depth = *(++ptr);
            last = ptr;
         }

        if ( (ptr = strchr (last,BREAK_CHAR)) != NULL ) 
         {
            *ptr = EOS;
            timeout = ++ptr;
            last = timeout;
         }

        if ( (ptr = strchr (last,BREAK_CHAR)) != NULL ) 
         {
            *ptr = EOS;
            itime = ++ptr;
            last = ptr;
         }

        /* we don't want any '\n' at the end of line */
        if ( (ptr = strchr (last,'\n')) != NULL) 
         {
            *ptr = EOS;
         }
    
        if (url && *url && lock && *lock && depth!=' ' && 
            timeout && *timeout && itime && *itime ) 
         {

            if ( (me = (LockLine *)HT_CALLOC (1,sizeof(LockLine))) == NULL)
                HT_OUTOFMEM ((char*)"LockLine new");

            me->relativeURI = me->lockToken = NULL;
            me->timeout =  me->initialTime = NULL;
        
            StrAllocCopy (me->relativeURI,url);
            StrAllocCopy (me->lockToken,lock);
            StrAllocCopy (me->timeout,timeout);
            StrAllocCopy (me->initialTime,itime);
            me->depth = depth;
         }

        HT_FREE (line);
     }

    return me;
}


/* ----------------------------------------------------------- 
 * Creates a new LockLine object. It's an alternative function
 * to create these objects.
 *
 * Parameters :
 *     char * relativeURI :  relative URI
 *     char * locktoken   :  lock-token header
 *     char * depth       :  depth value
 *     char * timeout     :  timeout value
 *     time_t itime       :  initial time value
 *
 * Returns :
 *     LockLine *  : new LockLine object
 * ----------------------------------------------------------- */
PUBLIC LockLine * LockLine_newObject (char * relativeURI, char * locktoken, 
                                      char * depth, char * timeout, time_t itime ) 
{
    LockLine * me = NULL;
    char * dt;
    
    if (relativeURI && *relativeURI && locktoken && *locktoken 
        && depth && *depth && timeout && *timeout) 
     {

        if ( (me = (LockLine *)HT_CALLOC (1,sizeof(LockLine))) == NULL)
            HT_OUTOFMEM ((char*)"LockLine newObject");

        me->relativeURI = me->lockToken = me->timeout = me->initialTime = NULL;

#ifdef DEBUG_LOCK_BASE		
        fprintf (stderr,"AHTLockBase.... creating LockLine object \n");
#endif	
        StrAllocCopy (me->relativeURI,relativeURI);
        StrAllocCopy (me->lockToken,locktoken);
        StrAllocCopy (me->timeout,timeout);
        me->depth = depth[0];
	
	dt = timetostr(itime);
        StrAllocCopy (me->initialTime,dt);       
     } 
    return me;
}



/* ----------------------------------------------------------- 
 * Deletes a LockLine object. 
 * ----------------------------------------------------------- */
PUBLIC BOOL LockLine_delete (LockLine *me) 
{

    if (me) 
     {
#ifdef DEBUG_LOCK_BASE		
        fprintf (stderr,"AHTLockBase.... deleting LockLine object \n");
#endif

        if (me->relativeURI) HT_FREE (me->relativeURI);
        if (me->lockToken) HT_FREE (me->lockToken);
        if (me->timeout) HT_FREE (me->timeout);
        if (me->initialTime) HT_FREE (me->initialTime);
	me->relativeURI = me->lockToken = NULL;
        me->timeout = me->initialTime = NULL;
        HT_FREE (me);
	me = NULL;
        return YES;
     }
    
    return NO; 
}




/* ----------------------------------------------------------- 
 * Gets a line from the file pointed by 'fp' parameter.
 * Returns: a new LockLine * object or NULL, if it reach EOF
 *          with no more data. 
 * ----------------------------------------------------------- */
PUBLIC LockLine * LockLine_readline (FILE *fp) 
{
    char   line [DAV_LINE_MAX];
    LockLine * info = NULL;
    
    if (fp && fgets (line,DAV_LINE_MAX,fp) != NULL )
     { 
        info = LockLine_new (line);
     }

    return info;    
}


/* ----------------------------------------------------------- 
 * Writes in file a new line with the informations in LockLine 
 * object.
 * Return: BOOL YES if it succeed, NO, if it doesn't. 
 * ----------------------------------------------------------- */
PUBLIC BOOL LockLine_writeline (FILE *fp, LockLine *line) 
{
    BOOL status = NO;
    char fileline[DAV_LINE_MAX];
    int len;
    
    if (fp && line) 
     {
        len = strlen (line->relativeURI) +  strlen (line->lockToken);
        len += strlen (line->timeout) +  strlen (line->initialTime) + 6;
	if (len >= DAV_LINE_MAX) return NO;
	
        sprintf (fileline,"%s%c%s%c%c%c%s%c%s\n",line->relativeURI,BREAK_CHAR,
                                                 line->lockToken,BREAK_CHAR,
                                                 line->depth,BREAK_CHAR,
                                                 line->timeout,BREAK_CHAR,
                                                 line->initialTime);

#ifdef DEBUG_LOCK_BASE
	fprintf	(stderr,"AHTLockBase.... writing %s \n", fileline);
#endif	
        status = (fputs(fileline,fp)!=EOF)?YES:NO;
	if (status) fflush (fp);
     }
    
    return status;
}



/* ----------------------------------------------------------- 
 *  Test if the requestUri matches with the lockUri. 
 *  ----------------------------------------------------------- */
BOOL matchURI (const char *requestUri, const char *lockUri) 
{
    BOOL status = NO;
    char * ptr;

    if (requestUri && *requestUri && lockUri && *lockUri) 
     {
            
#ifdef DEBUG_LOCK_BASE		
        fprintf (stderr,"AHTLockBase...Matching %s and %s\n", 
                        requestUri, lockUri);
#endif

        /* lockUri should matches in the first "position" of the requestUri.
         * for example: if lockUri is /foo, it should match in /foo/bar, but
         * not in /bar/foo, because only in the first case a lock in 
         * lockUri may affect operations in requestUri. */
        if ( (ptr = HTStrCaseStr((char *)requestUri,(char *)lockUri)) != NULL) 
         {
            if (ptr == requestUri) status = YES; 
         }
        else status = NO;
     }

    return status;    
}


/* ----------------------------------------------------------- 
 * Creates a string to be used in an If header.
 * Format: "<http://filename/relative-uri> (lock-token)"
 * ----------------------------------------------------------- */
PUBLIC char * makeIfItem (const char * filename, char * relUri, char *lockToken) 
{
    char * ptr = NULL;
    int sum = 0;
    
    if (filename && *filename && relUri && *relUri 
        && lockToken && *lockToken) 
     {
        sum += strlen(filename);
        sum += strlen(relUri);
        sum += strlen (lockToken);
        sum += 15;
        
        if ( (ptr = (char *)HT_CALLOC (sum,sizeof(char))) == NULL)
             HT_OUTOFMEM((char*)"AHTLockFile makeIfItem");
        
        sprintf (ptr,"<http://%s%s> (%s) ",filename,relUri,lockToken);
     }
    return ptr;
}



/* ----------------------------------------------------------- 
 * Process a file from 'lock base'. It searchs locks that 
 * should affect the relative URI in 'reqUri' parameter and
 * returns a list of LockLine objects with the lines that
 * matches in filename. 
 *
 * Returns : an HTList LockLine objects. This list may be 
 *           empty or NULL.
 *
 * Note: the caller must delete all LockLine objects after
 *       use them. 
 * ----------------------------------------------------------- */
PUBLIC HTList *processLockFile (const char *filename, const char *reqUri) 
{
    FILE     *fp;
    LockLine *info = NULL;
    BOOL      match = NO;
    HTList   *list = NULL;
    char      path[DAV_LINE_MAX];
    
    if (filename && *filename && reqUri && *reqUri) 
     {
	/*file "filename" should be at the lock base */
	sprintf (path,"%s%s", DAVHome, filename);	
#ifdef DEBUG_LOCK_BASE		
        fprintf (stderr,"AHTLockBase... opening %s\n",path);
#endif
        /* open the file "filename" for read purposes */
	fp = TtaReadOpen (path);
        if (fp == NULL) 
            return (HTList *)NO;

        list  = HTList_new(); 
        /* read all file to found all matches */
        while ( (info = LockLine_readline (fp)) != NULL) 
         {
            if (info) 
             {
               /* fprintf (stderr,"AHTLockBase...Line - %s %s %c %s %s \n", \
                     info->relativeURI, info->lockToken, info->depth, \
                     info->timeout, info->initialTime); */

                match = matchURI (reqUri,(const char*)info->relativeURI);
                if (match)
                 {
                    int slash_uri = 0, slash_info = 0;
                    int i = 0;
#ifdef DEBUG_LOCK_BASE
                    fprintf (stderr,"AHTLockBase... checking hierarchical level\n");
#endif
                    /* counts number of "/" - hierarchical level */
                    for (i=0; i< (int)strlen (reqUri); i++) 
                        if (reqUri[i]=='/')
			  slash_uri++;

                    for (i=0; i < (int)strlen (info->relativeURI); i++) 
                        if (info->relativeURI[i] == '/') slash_info++;            

                    /* "exact" match */
                    if (slash_uri == slash_info) 
                        match = YES; /* confirm match */
                    else if ( (slash_uri > slash_info) ) 
                     {
                        if (TOLOWER(info->depth) == 'i' || 
                             slash_uri== (slash_info+1)) 
                            /* reqUri is a member of info->relativeUri */
                            match = YES; /* confirm match */
                     }
                    else match = NO; /* depth is 0, so lock in info->relativeURI 
                                        should not affect operations over reqUri */    
                }/* if match */
               else match = NO;            
               
               if (match == YES) HTList_addObject (list,info);  
               else LockLine_delete (info);
               
            } /* if (info) */        
        } /* while */

        TtaReadClose (fp); /* closing file */
    }
    
    return list;        
}


/* -----------------------------------------------------------
 * this functions searchs the locks in "filename" that matches
 * or affect an operation over the resource "reqUri". It test
 * the lock timeout in the file, and only returns matches that
 * are still valids. 
 * 
 * Returns: a list of "tagged lists" to be used in a If header
 * (format "<http://filename/uri> (<lock-token>)" ).
 * ----------------------------------------------------------- */
PUBLIC HTList * searchLockBase ( char * filename,  char * reqUri) 
{
    LockLine          *info;
    HTList            *list, *if_list = NULL;
    time_t             now;
    unsigned long int  itime, tout;
    char              *ptr;
    
    /* process filename - returns a list of LockLine that matches */
    if ( (list = processLockFile(filename,reqUri)) == NULL ||
         (HTList_isEmpty (list)) )  
        return (NULL);
    

    /* list to be returned */
    if_list = HTList_new ();
    
    
    while ( (info = (LockLine* )HTList_nextObject(list)) != NULL) 
     {
#ifdef DEBUG_LOCK_BASE		
        fprintf (stderr,"AHTLockBase...... %s matches %s\n",info->relativeURI, reqUri);

        /* is the lock still valid? Did it expire? */
        fprintf (stderr,"AHTLockBase...... Checking lock timeout ");
#endif

        time (&now);
	itime = strtotime (info->initialTime);
    
        if (HTStrCaseStr(info->timeout, (char*)"Infinite")!=NULL) tout = now;
        else if (HTStrCaseStr(info->timeout, (char*)"Second-")!=NULL) 
         {
            ptr = strchr(info->timeout,'-') + 1;
            tout = (unsigned long int) atol(ptr);
         }
	else tout = 0; /*a unknown timeout notation*/

        if ((itime+tout) < (unsigned long int)now)  
         {
#ifdef DEBUG_LOCK_BASE		
            fprintf (stderr,"- expired\n");
#endif	    
         }
        else
         { /* lock valid, create a tagged list for If header */
#ifdef DEBUG_LOCK_BASE		
            fprintf (stderr,"- valid\n");
#endif
            ptr = makeIfItem (filename, info->relativeURI, info->lockToken);
            if (ptr) HTList_addObject(if_list,ptr);
         }
        
        /* free memory - delete the LockLine object */
        LockLine_delete (info);
     }  

    /* free memory - delete the LockLine list returned by processLockFile */
    HTList_delete (list);

    return (if_list);
}


/* -----------------------------------------------------------
 * This function creates the content for a If header based in 
 * a HTlist of "tagged list" string, like the list returned by 
 * the function searchLockBase.
 * ----------------------------------------------------------- */
PUBLIC char * mountIfHeader (HTList *if_list) 
{
    char * header = NULL;
    char * ptr;
    
    /* creatingif header */
    if (if_list && !HTList_isEmpty(if_list)) 
     {
        while ( (ptr = (char *)HTList_nextObject(if_list))!=NULL) 
         {
            StrAllocCat (header,ptr);
         }
     } 
        
    return (header); 
}



/* ---------------------------------------------------------------------------
 * This function separates the host name and the relative part
 * from a URL.
 *
 * Parameters:
 *     const char *  URI : request URI 
 *     const char *  localFQDN : local hostname with full qualify domain name
 *     char ** hostname : char pointer for return purposes
 *     char ** relative : char pointer for return purposes
 * Returns:
 *     BOOL :  YES if it succeed, NO if fails.
 *     char ** hostname : if succeed, this pointer should contain the complete
 *                        hostname (with domain name) of the host in URI
 *     char ** relative : if succeed, this pointer should contain the relative
 *                        URI used in URI      
 * -------------------------------------------------------------------------- */
PUBLIC BOOL separateUri (const char *URI, const char *localFQDN,
                         char ** hostname, char **relative) 
{
    char *host, *port, *rel, *dom;
    char *filename, *relUri;
    char *address, *fqdn;
    char pnumber[10];
    BOOL status;
    int i;

    host = port = rel = dom = NULL;
    filename = relUri = NULL;
    address = fqdn = NULL;
    pnumber[0] = EOS;
    status = NO;
    i = 0;

    /* no parameters, no work to do */
    if (!URI || !(*URI) || !localFQDN || !(*localFQDN)) 
        return NO;
    
    /* make a copy of URI to change it */
    StrAllocCopy (address, URI);
    /* we search these pointer positions :
     *  address ->  http://host.domaine:port/relative 
     *                     ^   ^        ^   ^
     *                     |   |        |   |
     *                   host dom     port rel
     */                 
    host = HTStrCaseStr (address, (char*)"://");
    status = (host) ? YES:NO;
    host = (host) ? host+3:host;
    port = (status) ? HTStrCaseStr (host, (char*)":"):NULL;
    rel  = (status) ? HTStrCaseStr (host, (char*)"/"):NULL;
    
    /* if port is pointing for somewhere after the first "/" (rel),
     * so it's not indicating the port number */
    if (port && rel && port>rel)
      port = NULL;
    /* if we didn't find the positions, return NO */
    if (status == NO)
      {
	HT_FREE (address);
	return NO;
      }
    /* copy the ":port_number" from port to pnumber */
    for (i=0; port && *(port+i) && *(port+i)!='/'; i++) 
        pnumber[i]=*(port+i);
    
    pnumber[i] = EOS;
    
    /* use PORT_CHAR instead ':' (from ":port_number") in the filename 
     * (Windows does not allow ':' in filename) */
    if (pnumber[0] != EOS)
        pnumber[0] = PORT_CHAR;
    
    /* copy the relativeURI in "rel" to relUri, or
     * define it as "/" */
    if (rel)
      StrAllocCopy (relUri, rel);
    else
      StrAllocCopy (relUri, "/");
    /* end the host string at the port number or at 
     * the relative URI part */
    if (port)
     {
        (*port) = EOS;
        port++;
     }
    else if (rel) 
      (*rel) = EOS;
    /* try to find the domain name in the host */
    StrAllocCopy (filename, host);
    dom = HTStrCaseStr (host, (char*)".");
    
    if (dom == NULL) 
     {
        /* if it isn't localhost, then we try to set the domain 
         * name looking at the local domain name in localFQDN */ 
       StrAllocCopy (fqdn, localFQDN);
        if (strcasecomp (host,"localhost") != 0) 
         {
            dom = HTStrCaseStr (fqdn, (char*)".");
            if (dom) 
	      StrAllocCat (filename, dom);
            HT_FREE (fqdn);
         }
        else
         {/* if it is localhost, use FQDN insteed */
            HT_FREE (filename);
            filename = fqdn;
         }
     }

    /* copy the port number to the host */
    if (pnumber[0]!=EOS)
      StrAllocCat (filename, pnumber);

    /* returning */
    (*hostname) = filename;
    (*relative) = relUri;
    HT_FREE (address);
    return status;
}


/* ---------------------------------------------------------------------------
 * This function process the lock information in the parameters and returns
 * a new LockLine object with these informations.  
 * 
 * Returns: LockLine * object or NULL, if failed. 
 *  -------------------------------------------------------------------------- */
PUBLIC LockLine * processLockInfo (char *relative, AwTree *xmlbody, HTAssocList *headers) 
{
    LockLine *me = NULL;
    char     *depth, *timeout, *lock;
    time_t    itime;
    HTAssoc  *h = NULL;
    char     *ptr = NULL;

    if (relative && *relative && xmlbody && headers) 
     {
        time (&itime);
	
        depth = timeout = lock = NULL;

        /* getting lock-token */
        while (headers && (h = (HTAssoc *)HTAssocList_nextObject(headers))) 
         {
            if (!strcasecomp(HTAssoc_name(h),"Lock-Token"))
                ptr = HTAssoc_value(h);
         }

        if (ptr) StrAllocCopy (lock,ptr);
        else return NULL;
        	
        /* getting depth */
        depth = AwParser_searchInTree (xmlbody,"depth");

        /* getting timeout */
        timeout = AwParser_searchInTree (xmlbody,"timeout");

        me = LockLine_newObject (relative,lock,depth,timeout,itime);

        if (lock) HT_FREE(lock);
        if (depth) HT_FREE(depth);
        if (timeout) HT_FREE(timeout);
     }
    
    return me;
}

/* ---------------------------------------------------------------------------
 * This function saves the LockLine object in LockBase
 *
 * Parameters:
 * 	char * absolute: the request hostname with domain name and port number
 * 	LockLine * lockinfo: informatio to be saved
 *
 * Returns: BOOL YESS if it succeed, NO if it doesn't.
 *  -------------------------------------------------------------------------- */
PUBLIC BOOL saveLockLine (char *absolute, LockLine *lockinfo) 
{
    FILE *fp = NULL;
    BOOL status = NO;
    char filename[DAV_LINE_MAX];

    if (absolute && *absolute && lockinfo) 
     {
	    
        sprintf (filename,"%s%s",DAVHome,absolute);
			
#ifdef DEBUG_LOCK_BASE		
	fprintf (stderr,"AHTLockBase.... DAVHome is %s\n",DAVHome);
        fprintf (stderr,"AHTLockBase.... open file %s\n", filename);
#endif
	fp = TtaAddOpen (filename);
        if (fp == NULL) 
            return NO;
        status = LockLine_writeline (fp, lockinfo);
     }

    return status;
}



/* ---------------------------------------------------------------------------
 * This function saves the lock information present in xmlbody in the LockBase
 *
 * Parameters:
 *     char * absolute: the request hostname with domain name and port number
 *     char * relative: the relative URI (collections should terminate
 *                            with a slash "/")
 *     char * xmlbody: XML response body from lock request
 *     HTAssocList * headers: response headers
 *     
 * Returns: BOOL YES if it suceed, NO if it doesn't.
 *  -------------------------------------------------------------------------- */
PUBLIC BOOL saveLockBase (char *absolute, char *relative, 
                          char *xmlbody, HTAssocList *headers)
{
    FILE *fp = NULL;
    BOOL status = NO;
    LockLine *line = NULL;
    AwTree * tree = NULL;
    AwString data;
    char filename[DAV_LINE_MAX];
    
    if (absolute && *absolute && relative && *relative && 
        xmlbody && *xmlbody && headers ) 
     {
 	    
        sprintf (filename,"%s%s",DAVHome,absolute);
			
#ifdef DEBUG_LOCK_BASE		
	fprintf (stderr,"AHTLockBase.... DAVHome is %s\n",DAVHome);
        fprintf (stderr,"AHTLockBase.... open file %s\n", filename);
#endif
	fp = TtaAddOpen (filename);
        if (fp == NULL) 
            return NO;
 
        /* creates the AwTree with the xml body */
#ifdef DEBUG_LOCK_BASE		
        fprintf (stderr,"AHTLockBase.... creating XML tree\n");
#endif		
        data = AwString_new ((strlen (xmlbody)+1));
        AwString_set (data,xmlbody);
        tree = AwParser_umountMessage(data);
        AwString_delete(data);
        
        /* creates the LockLine object */
#ifdef DEBUG_LOCK_BASE		
        fprintf (stderr,"AHTLockBase.... processing lock info\n");
#endif		
        line = (tree)?processLockInfo(relative,tree,headers):NULL;
        
        /* saving it */ 
        if (line) 
            status = LockLine_writeline(fp,line);
        else 
            status = NO;
	
        if (tree) AwTree_delete(tree);
        if (line) LockLine_delete(line);
#ifdef DEBUG_LOCK_BASE		
        fprintf (stderr,"AHTLockBase.... closing file\n");
#endif		
        TtaWriteClose (fp);
     }
        
    return status;
}

/* need this function from thotlib ... */
extern ThotBool TtaFileUnlink ( const char *filename );

/* ---------------------------------------------------------------------------
 * This function removes the lock information in LockLine object from the base.
 * Returns: BOOL YES it it succed, NO if it doesn't.
 * --------------------------------------------------------------------------- */
PUBLIC BOOL removeFromBase (char *filename, LockLine *line) 
{
    FILE *fp = NULL;
    char buf[DAV_LINE_MAX];
    char *ptr,*cp;
    char old;
    BOOL status = NO;
    BOOL eof = NO;
    HTList *list = NULL;
    char path[DAV_LINE_MAX];
    
    if (filename!=NULL && line!=NULL && (line->relativeURI)!=NULL && 
        *filename && *line->relativeURI ) 
     {

	/* filename should be in the lockbase */
        sprintf (path,"%s%s",DAVHome,filename);

#ifdef DEBUG_LOCK_BASE		
        fprintf (stderr,"AHTLockBase.... opening %s\n", path);
#endif	
        /* open the file "filename" for read purposes */
	fp = TtaReadOpen (path);
        if (fp == NULL ) 
            return NO;
        
        status = YES; /* thinking positif! */
        list = HTList_new();

        /* copying file to memory */
        while (fgets (buf,DAV_LINE_MAX,fp) != NULL && status) 
         { 
#ifdef DEBUG_LOCK_BASE		
            fprintf (stderr,"AHTLockBase.... reading %s", buf);
#endif		    
            cp = NULL;
            ptr = strchr (buf,BREAK_CHAR);
            if (ptr) 
             {
                old = (*ptr);
                (*ptr) = EOS;
                
                /* if doesn't match, it will be save */
                if (strcasecomp (buf,line->relativeURI)!=0) 
                 {
                    (*ptr) = old;
                    StrAllocCopy (cp,buf);                        
                    HTList_addObject(list,(void *)cp);
                 }
                else 
                 {  /* it matches, won't be save */
                    (*ptr) = old;
#ifdef DEBUG_LOCK_BASE		
                    fprintf (stderr,"AHTLockBase.... Discarting %s\n", buf);
#endif			    
                 }
             } /* if ptr */
            else status = NO; /* invalid file format */
         }

        /* reopen file for writing */
#ifdef DEBUG_LOCK_BASE		
        fprintf (stderr,"AHTLockBase.... reopening %s (status %s)\n", filename,
		       	(status==YES)?"YES":"NO");
#endif		

        if (status) 
         {
            /* close and remove old file */
            TtaReadClose (fp);
            fp = NULL;
            
            /* try to remove the old file, to write a new one.
             * if remove failed, we force the creation of a new empty file.
             * if we have something to write, create a new file. */
            if (!TtaFileUnlink(path) || (list && !HTList_isEmpty (list))) 
                fp = TtaWriteOpen (path);
         }      
	

        /* saving entries in list */
        while (status && fp && !eof && 
               (cp = (char *)HTList_nextObject(list))!=NULL) 
         {
#ifdef DEBUG_LOCK_BASE		
            fprintf (stderr,"AHTLockBase.... saving %s", cp);
#endif		    
            eof = (fputs(cp,fp)!=EOF)?NO:YES;
            HT_FREE(cp);
         }
        
        /* freeing everybody */
        HT_FREE(list);

        if (status && fp) 
         {
	    fflush (fp);
	    TtaReadClose (fp);
	 }
     }

    return status;  
}
