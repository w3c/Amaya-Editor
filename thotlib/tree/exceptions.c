
/* -- Copyright (c) 1996 Inria  --  All rights reserved  -- */


/*
   Traitement des exceptions definies dans les schemas  
   de structure. 
 */

#include "thot_sys.h"
#include "message.h"
#include "constmedia.h"
#include "typemedia.h"
#include "functions.h"

#define EXPORT extern
#include "edit.var"

#include "except.f"
#include "structure.f"

/* ---------------------------------------------------------------------- */
/* |    ExceptTypeElem retourne Vrai si le numero d'exception exceptNum | */
/* |            est associe' au type typeNum du schema de structure pSS	| */
/* ---------------------------------------------------------------------- */
#ifdef __STDC__
boolean             ExceptTypeElem (int exceptNum, int typeNum, PtrSSchema pSS)
#else  /* __STDC__ */
boolean             ExceptTypeElem (exceptNum, typeNum, pSS)
int                 exceptNum;
int                 typeNum;
PtrSSchema        pSS;

#endif /* __STDC__ */
{
   boolean             ret;
   int           e;
   SRule              *pSRule;
   PtrSSchema        pExtSS;

   ret = False;
   if (pSS != NULL)
     {
	pSRule = &pSS->SsRule[typeNum - 1];
	if (pSRule->SrFirstExcept != 0)
	   /* il y a des exceptions pour ce type d'element */
	  {
	     e = pSRule->SrFirstExcept;
	     /* compare les exceptions de ce type d'element avec exceptNum */
	     do
		if (pSS->SsException[e - 1] == exceptNum)
		   ret = True;
		else
		   e++;
	     while (!(ret || e > pSRule->SrLastExcept));
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
		  pSRule = RegleExtens (pSS, typeNum, pExtSS);
		  if (pSRule != NULL)
		     /* examine les exceptions de cette regle d'extension */
		     if (pSRule->SrFirstExcept != 0)
		       {
			  e = pSRule->SrFirstExcept;
			  do
			     if (pExtSS->SsException[e - 1] == exceptNum)
				ret = True;
			     else
				e++;
			  while (!(ret || e > pSRule->SrLastExcept));
		       }
		  /* passe a l'extension de schema suivante */
		  pExtSS = pExtSS->SsNextExtens;
	       }
	  }
     }
   return ret;
}


/* ---------------------------------------------------------------------- */
/* |    ExceptAttr retourne Vrai si le numero d'exception exceptNum est | */
/* |            associe' a` l'attribut attr defini dans le schema de    | */
/* |            structure pSS.						| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
boolean             ExceptAttr (int exceptNum, int attr, PtrSSchema pSS)

#else  /* __STDC__ */
boolean             ExceptAttr (exceptNum, attr, pSS)
int                 exceptNum;
int                 attr;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   boolean             ret;
   int           e;
   TtAttribute           *pAtt;

   ret = False;
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
		   ret = True;	/* trouve' */
		else
		   e++;
	     /* exception suivante pour ce type d'element */
	     while (!(ret || e > pAtt->AttrLastExcept));
	  }
     }
   return ret;
}

/* ---------------------------------------------------------------------- */
/* |    ExceptNumAttr recherche dans le schema de structure pSS		| */
/* |            l'attribut auquel est associe' le numero d'exception	| */
/* |            exceptNum et retourne son numero d'attribut.		| */
/* |            Retourne 0 en cas d'echec.				| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 ExceptNumAttr (int exceptNum, PtrSSchema pSS)

#else  /* __STDC__ */
int                 ExceptNumAttr (exceptNum, pSS)
int                 exceptNum;
PtrSSchema        pSS;

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
	   if (ExceptAttr (exceptNum, attr, pSS))
	      /* on a trouve' l'attribut cherche' */
	      ret = attr;
	}
   return ret;
}

/* ---------------------------------------------------------------------- */
/* |    ExceptNumType recherche dans le schema de structure pSS	le	| */
/* |            type d'element auquel est associe' le numero		| */
/* |            d'exception exceptNum et retourne son numero de type.	| */
/* |            Retourne 0 en cas d'echec.				| */
/* ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 ExceptNumType (int exceptNum, PtrSSchema pSS)

#else  /* __STDC__ */
int                 ExceptNumType (exceptNum, pSS)
int                 exceptNum;
PtrSSchema        pSS;

#endif /* __STDC__ */

{
   int                 typ, ret;

   ret = 0;
   typ = 0;
   /* examine successivement tous les types d'element definis dans le schema */
   while (ret == 0 && typ < pSS->SsNRules)
     {
	typ++;
	if (ExceptTypeElem (exceptNum, typ, pSS))
	   /* on a trouve' le type d'element cherche' */
	   ret = typ;
     }
   return ret;
}

