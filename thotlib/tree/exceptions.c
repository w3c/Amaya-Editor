/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2003
 *  Please first read the full copyright statement in file COPYRIGHT.
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
ThotBool TypeHasException (int exceptNum, int typeNum, PtrSSchema pSS)
{
   ThotBool            ret;
   int                 e;
   PtrSRule            pSRule;
   PtrSSchema          pExtSS;

   ret = FALSE;
   if (pSS != NULL && typeNum > 0)
     {
	pSRule = pSS->SsRule->SrElem[typeNum - 1];
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
ThotBool AttrHasException (int exceptNum, int attr, PtrSSchema pSS)
{
   ThotBool            ret;
   int                 e;
   PtrTtAttribute         pAtt;

   ret = FALSE;
   if (pSS != NULL)
     {
	pAtt = pSS->SsAttribute->TtAttr[attr - 1];
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
int GetAttrWithException (int exceptNum, PtrSSchema pSS)
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
int GetElemWithException (int exceptNum, PtrSSchema pSS)
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
