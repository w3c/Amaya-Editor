/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Warning:
 * This module is part of the Thot library, which was originally
 * developed in French. That's why some comments are still in
 * French, but their translation is in progress and the full module
 * will be available in English in the next release.
 * 
 */
 
/*
 * Traitement des exceptions definies dans les schemas  
 * de structure. 
 *
 * Author: V. Quint (INRIA)
 *
 */

#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"


#define THOT_EXPORT extern
#include "edit_tv.h"

#include "exceptions_f.h"
#include "structschema_f.h"

/*----------------------------------------------------------------------
   TypeHasException retourne Vrai si le numero d'exception exceptNum 
   est associe' au type typeNum du schema de structure pSS	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TypeHasException (int exceptNum, int typeNum, PtrSSchema pSS)
#else  /* __STDC__ */
ThotBool            TypeHasException (exceptNum, typeNum, pSS)
int                 exceptNum;
int                 typeNum;
PtrSSchema          pSS;

#endif /* __STDC__ */
{
   ThotBool            ret;
   int                 e;
   SRule              *pSRule;
   PtrSSchema          pExtSS;

   ret = FALSE;
   if (pSS != NULL && typeNum > 0)
     {
	pSRule = &pSS->SsRule[typeNum - 1];
	if (pSRule->SrFirstExcept != 0)
	   /* il y a des exceptions pour ce type d'element */
	  {
	     e = pSRule->SrFirstExcept;
	     /* compare les exceptions de ce type d'element avec exceptNum */
	     do
		if (pSS->SsException[e - 1] == exceptNum)
		   ret = TRUE;
		else
		   e++;
	     while (!ret && e <= pSRule->SrLastExcept);
	  }
	if (!ret)
	   /* on n'a pas trouve', on cherche dans les extensions du schema */
	  {
	     /* passe a la premiere extension du schema */
	     pExtSS = pSS->SsNextExtens;
	     /* on examine les extensions successives tant qu'on n'a pas trouve' */
	     while (pExtSS != NULL && !ret)
	       {
		  /* cherche la regle d'extension du type d'element traite' */
		  pSRule = ExtensionRule (pSS, typeNum, pExtSS);
		  if (pSRule != NULL)
		     /* examine les exceptions de cette regle d'extension */
		     if (pSRule->SrFirstExcept != 0)
		       {
			  e = pSRule->SrFirstExcept;
			  do
			     if (pExtSS->SsException[e - 1] == exceptNum)
				ret = TRUE;
			     else
				e++;
			  while (!ret && e <= pSRule->SrLastExcept);
		       }
		  /* passe a l'extension de schema suivante */
		  pExtSS = pExtSS->SsNextExtens;
	       }
	  }
     }
   return ret;
}


/*----------------------------------------------------------------------
   AttrHasException retourne Vrai si le numero d'exception exceptNum est 
   associe' a` l'attribut attr defini dans le schema de    
   structure pSS.						
  ----------------------------------------------------------------------*/

#ifdef __STDC__
ThotBool            AttrHasException (int exceptNum, int attr, PtrSSchema pSS)

#else  /* __STDC__ */
ThotBool            AttrHasException (exceptNum, attr, pSS)
int                 exceptNum;
int                 attr;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   ThotBool            ret;
   int                 e;
   TtAttribute        *pAtt;

   ret = FALSE;
   if (pSS != NULL)
     {
	pAtt = &pSS->SsAttribute[attr - 1];
	if (pAtt->AttrFirstExcept != 0)
	   /* il y a des exceptions pour cet attribut */
	  {
	     e = pAtt->AttrFirstExcept;
	     /* compare les exceptions de cet attribut avec exceptNum */
	     do
		if (pSS->SsException[e - 1] == exceptNum)
		   ret = TRUE;	/* trouve' */
		else
		   e++;
	     /* exception suivante pour ce type d'element */
	     while (!ret && e <= pAtt->AttrLastExcept);
	  }
     }
   return ret;
}

/*----------------------------------------------------------------------
   GetAttrWithException recherche dans le schema de structure pSS		
   l'attribut auquel est associe' le numero d'exception	
   exceptNum et retourne son numero d'attribut.		
   Retourne 0 en cas d'echec.				
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 GetAttrWithException (int exceptNum, PtrSSchema pSS)

#else  /* __STDC__ */
int                 GetAttrWithException (exceptNum, pSS)
int                 exceptNum;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   int                 attr, ret;

   ret = 0;
   attr = 0;
   /* examine successivement tous les attributs definis dans le schema */
   if (pSS != NULL && exceptNum > 0)
      while (ret == 0 && attr < pSS->SsNAttributes)
	{
	   attr++;
	   if (AttrHasException (exceptNum, attr, pSS))
	      /* on a trouve' l'attribut cherche' */
	      ret = attr;
	}
   return ret;
}

/*----------------------------------------------------------------------
   GetElemWithException recherche dans le schema de structure pSS	le	
   type d'element auquel est associe' le numero		
   d'exception exceptNum et retourne son numero de type.	
   Retourne 0 en cas d'echec.				
  ----------------------------------------------------------------------*/

#ifdef __STDC__
int                 GetElemWithException (int exceptNum, PtrSSchema pSS)

#else  /* __STDC__ */
int                 GetElemWithException (exceptNum, pSS)
int                 exceptNum;
PtrSSchema          pSS;

#endif /* __STDC__ */

{
   int                 typ, ret;

   ret = 0;
   typ = 0;
   /* examine successivement tous les types d'element definis dans le schema */
   while (ret == 0 && typ < pSS->SsNRules)
     {
	typ++;
	if (TypeHasException (exceptNum, typ, pSS))
	   /* on a trouve' le type d'element cherche' */
	   ret = typ;
     }
   return ret;
}
