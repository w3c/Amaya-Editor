/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
#ifndef _THOTLIB_TYPETYP_H_
#define _THOTLIB_TYPETYP_H_
/*
 * Structures de donnees pour le correcteur typographique.
 *
 *
 */

#define MaxCasTyAttrNum 10
#define MaxTyCond 10

#define MTyPremier 'S'
#define MTyDernier 'D'
#define MTyDansType 'T'
#define MTyAvantType 'A'
#define MTyApresType 'P'
#define MTyLangue 'L'
#define MTyFonction 'F'

#define MTyCapital 'c'
#define MTyWord 'w'
#define MTySpace 's'
#define MTyPunct 'p'
#define MTyPair 'P'
#define MTyDistance 'd'
#define MTyExponent 'e'
#define MTyAbbrev 'a'
#define MTyAttribute 'A'

/* les fonctions typographiques */
typedef enum
{
	TySpace,
	TyDistance,
	TyPunct,
	TyCapital,
	TyWord,
	TyPair,
	TyExponent,
	TyAbbrev,
	TyAttribute
} TypeFunct;

/* un pointeur sur un descripteur de function typographique */
typedef struct _TypoFunction *PtrTypoFunction;

/* une fonction typographique definie dans la section FUNCTION du schema
de typographie */
typedef struct _TypoFunction
{
   PtrTypoFunction	TFSuiv;		/* fonction typographique suivante */
   Name			TFIdent;	/* identificateur de la fonction */
   TypeFunct		TFTypeFunct;	/* type de fonction */
} TypoFunction;

/* un type de condition elementaire d'application de regle de typographie */
typedef enum
{
        TyPremier,
        TyDernier,
        TyDansType,
	TyAvantType,
	TyApresType,
        TyLangue,
	TyFonction
} TypeCondTypo;

/* une condition elementaire d'application d'un bloc de regles de typographie*/
typedef struct _TypoCondition
{
   TypeCondTypo    CondTypeCond;  /* type de la condition d'application
                                     des regles du bloc */
   ThotBool        CondNegative;  /* la condition est negative */
   int             CondTypeElem;  /* 1 .. MAX_RULES_SSCHEMA, si la condition est
                                     relative a un type */
   union
   {
	struct	/* TyAvantType, TyApresType */
	{
	ThotBool	_CondOpTypeIn_;	/* in ou  = */
	} s0;
	struct /* TyLangue, TyFonction */
	{
	Name	_CondNom_;	/* Language ou LexicalUnit */
	} s1;
   } u;
} TypoCondition;

#define CondOpTypeIn u.s0._CondOpTypeIn_
#define CondNom u.s1._CondNom_

/* un pointeur sur une regle de typographie */
typedef struct _RegleTypo *PtrRegleTypo;

/* une regle de typographie */
typedef struct _RegleTypo
{
  PtrRegleTypo    RTyRegleSuiv;  /* regle suivante du meme bloc */
  TypeFunct	  RTyTypeFunct;  /* type de fonction typographique */
  Name		  RTyIdentFunct; /* la procedure de controle typographique */
  int		  RTyNbCond;	 /* nombre de conditions elementaires dans RTyCondition */
  TypoCondition   RTyCondition[MaxTyCond];  /* les conditions elementaires */
} RegleTypo;

/* un pointeur sur un modele de composition */
typedef struct _ModeleCompo *PtrModeleCompo;

/* un modele de composition */
typedef struct _ModeleCompo
{
   PtrModeleCompo       MCSuiv;         /* modele de composition suivant */
   Name                  MCIdent;        /* nom du modele de composition */
   PtrRegleTypo	        MCBlocRegles;	/* debut de la chaine des regles */
} ModeleCompo;

/* un cas d'application de regles de typographie pour un attribut a valeur numerique */
typedef struct _TyCasAttrNum
{
	int             TyANBorneInf;	/* valeur minimum de l'attribut pour
					   que les regles de typographie soient appliquees */
	int             TyANBorneSup;	/* valeur maximum de l'attribut pour
					   que les regles de typographie soient appliquees */
	PtrRegleTypo   TyANBlocRegles;	/* bloc de regles de regles a
					   appliquer quand la valeur de l'attribut 
					   est dans l'intervalle */
} TyCasAttrNum;

/* un pointeur sur un bloc de regles de typographie associees a un attribut logique */
typedef struct _RTypoAttribut *PtrRTypoAttribut;

/* typographie associee a un attribut logique */
typedef struct _RTypoAttribut
{
  int	RTyATypeElem;	/* Type d'element auquel s'appliquent
				les regles de typographie ; 0 si les regles
				s'appliquent quel que soit le type d'element */
  union
    {
      struct	/* AttribType = AtNumAttr */
	{
	  /* 0 .. MaxCasTyAttrNum, nombre de cas d'application de regles de typographie */ 
	  int _RTyANbCas_;
	  /* les cas d'application de regles de traduction */
	  TyCasAttrNum     _RTyACas_[MaxCasTyAttrNum];
	} s0;
      struct	/* AttribType = AtReferenceAttr */
	{
	  PtrRegleTypo   _RTyARefPremRegle_; /* bloc de regles a appliquer pour l'attribut */
	} s1;
      struct	/* AttribType = AtTextAttr */
	{
	  Name             _RTyATxtVal_;	/* la valeur qui declanche l'application 
					   des regles de typographie */
	  PtrRegleTypo   _RTyATxt_;	/* bloc de regles a appliquer pour cette valeur */
	} s2;
      struct	/* AttribType = AtEnumAttr */
	{
	  /* 
	    Pour chaque valeur de l'attribut, dans l'ordre de
	    la table AttrEnumValue, adresse du premier bloc de regles
	    de typographie associe' a cette valeur.
	   */
	  PtrRegleTypo   _RTyAValEnum_[MAX_ATTR_VAL + 1];
	} s3;
    } u;
} RTypoAttribut;

#define RTyANbCas u.s0._RTyANbCas_
#define RTyACas u.s0._RTyACas_
#define RTyARefPremRegle u.s1._RTyARefPremRegle_
#define RTyATxtVal u.s2._RTyATxtVal_
#define RTyATxt u.s2._RTyATxt_
#define RTyAValEnum u.s3._RTyAValEnum_

/* pointeur sur un schema de typographie */
typedef struct _SchTypo *PtrSchTypo;

/* schema de typographie associe a une classe de document ou d'objet */
typedef struct _SchTypo
{
	PtrSchTypo	STySuivant;	/* pour le chainage des blocs libres */
	Name		STyNomStruct;	/* nom du schema de structure auquel */
					/* le schema de typo est associe' */
	int		STyStructCode;	/* code identifiant la version du */
					/* schema de structure */
	ThotBool		STyElemAlinea[MAX_RULES_SSCHEMA]; /* pour chaque type
					d'element, dans le meme ordre que dans
					la table StructSchema.SsRule, indique
					si l'elem. est un alinea de texte */
	PtrTypoFunction STyFunction;     /* liste des fct typo */
	PtrRegleTypo   STyRegleElem[MAX_RULES_SSCHEMA];	/* pointeurs sur le
					debut de la chaine des regles de
					typographie associees a chaque type
					d'element, dans le meme ordre que dans
					la table StructSchema.SsRule */
	ThotBool		STyElemHeritAttr[MAX_RULES_SSCHEMA]; /* pour chaque type
					d'element, dans le meme ordre que dans
					la table StructSchema.SsRule, indique
					si l'elem. herite d'un attribut d'un
					elem. ascendant */
        PtrRTypoAttribut STyAttribSem[MAX_ATTR_SSCHEMA]; /* regles de typographie
					des attributs logiques, dans l'ordre
					de la table StructSchema.SsAttribute */
} SchTypo;
#endif /* _THOTLIB_TYPETYP_H_ */
