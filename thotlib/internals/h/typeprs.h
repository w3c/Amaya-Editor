
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */
/* Declarations de types pour la presentation des documents */

/* V. Quint	Juin 1984 */

/* DEFINITIONS
   
   Un numero de vue est le rang d'une vue dans la table PresentSchema.PsView.
   Un numero de compteur est le rang d'un compteur dans la table PresentSchema.
   PsCounter.
   Un numero de constantes est le rang d'une constante dans la table
   PresentSchema.PsConstant.
   Un numero de variable est le rang d'une variable dans la table PresentSchema.
   PsVariable.
   Un numero de boite de presentation est le rang d'une boite dans la table
   PresentSchema.PsPresentBox. */

#include "typebase.h"
/******************* a revoir avec les Images descriptors ***********/
/* mode de presentation des images */
#define UNKNOWN_FORMAT   -1
typedef enum
{
        RealSize,
        ReScale,
        FillFrame
} PictureScaling;
/********************************************************************/

/* cotes et axes des boites */
typedef enum
{
    Top, Bottom, Left, Right, HorizRef, VertRef,
    HorizMiddle, VertMiddle, NoEdge
} BoxEdge;

typedef struct _PresRule *PtrPRule;

/* niveau relatif */
typedef enum
{
  RlEnclosing, RlSameLevel, RlEnclosed, RlPrevious, RlNext, RlSelf, RlContainsRef,
  RlRoot, RlReferred, RlCreator
} Level;

/* SRule de positionnement relatif de deux cotes ou axes de boites */
typedef struct _PosRule
{
  BoxEdge	PoPosDef;	/* le cote du pave dont on definit la
				   position */
  BoxEdge	PoPosRef;	/* le cote du pave par rapport auquel on
				   definit la position */
  TypeUnit	PoDistUnit;	/* PoDistance est exprime' en points typo,
				   en 1/10 de caracteres, etc. */
  boolean	PoDistAttr;	/* PoDistance est un numero d'attribut
				   numerique ou une valeur numerique */
  int	PoDistance;	/* distance entre ces deux cotes */
  Level	PoRelation;	/* niveau relatif de l'element de reference
				   dans la representation interne */
  boolean	PoNotRel;	/* si 'false', PoTypeRefElem ou PoRefPresBox
				   indique le type de l'elem. de ref.,
				   si 'true', le type exclus. */
  boolean	PoUserSpecified;	/* la distance peut etre choisie par
				   l'utilisateur lors de la creation */
  boolean	PoRefElem;	/* la reference est la boite d'un element
				   structure, sinon, une boite de pres. */
  union
  {
    struct			/* PoRefElem = True */
    {
      int	 _PoTypeRefElem_;	 /* le type de l'element par rapport
					    auquel on definit la position */
    } s0;
    struct			/* PoRefElem = False */
    {
      int _PoRefPresBox_; /* le type de la boite par rapport a
				      laquelle on definit la position.
				      Zero si type quelconque */
    } s1;
  } u;
} PosRule;

#define PoTypeRefElem u.s0._PoTypeRefElem_
#define PoRefPresBox u.s1._PoRefPresBox_

/* SRule de dimensionnement de boites */
typedef struct _DimensionRule
{
  boolean	 DrPosition;	/* La dimension est definie comme une
				   position (boite elastique) */
  union
  {
    struct			/* DrPosition = True */
    {
      PosRule	_DrPosRule_;     /* la regle de position  qui definit la
				  dimension */
    }   s0;
    struct			/* DrPosition = False */
    {
      boolean	_DrAbsolute_;     /* Valeur absolue,sinon relative */
      boolean	_DrSameDimens_; /* On definit la dimension par rapport a
				  la meme dimension d'une autre boite*/
      TypeUnit	_DrUnit_;    /* DrValue est exprime' en points
				  typo, en 1/10 de caracteres, etc. */
      boolean	_DrAttr_;    /* DrValue est un numero d'attribut
				  numerique ou une valeur numerique */
      boolean	_DrMin_; /* dimension minimum */
      boolean	_DrUserSpecified_;/* la distance peut etre choisie par
				  l'utilisateur */
      int	_DrValue_;  /* valeur du pourcentage, de l'increment
				  ou de la valeur absolue */
      Level	_DrRelation_;  /* niveau relatif de l'element de
				  reference */
      boolean	_DrNotRelat_;  /* si False, DrTypeRefElem ou DrRefPresBox
				  indique le type de l'elem. de ref.,
				  si False, le type exclus. */
      boolean	_DrRefElement_;  /* la reference est la boite d'un
				  element structure, sinon une boite
				  de presentation */
      union
      {
	struct			/* DrRefElement = True */
	{
	  int	_DrTypeRefElem_;	/* le type de l'element par
					   rapport auquel on definit
					   la dimension  */
	} s0;
	struct			/* DrRefElement = False */
	{
	  int _DrRefPresBox_;/* le type de la boite par
					 rapport a laquelle on definit la
					 dimension. Zero si type quelconque */
	} s1;
      } u;
    } s1;
  } u;
} DimensionRule;

#define DrPosRule u.s0._DrPosRule_
#define DrAbsolute u.s1._DrAbsolute_
#define DrSameDimens u.s1._DrSameDimens_
#define DrUnit u.s1._DrUnit_
#define DrAttr u.s1._DrAttr_
#define DrMin u.s1._DrMin_
#define DrUserSpecified u.s1._DrUserSpecified_
#define DrValue u.s1._DrValue_
#define DrRelation u.s1._DrRelation_
#define DrNotRelat u.s1._DrNotRelat_
#define DrRefElement u.s1._DrRefElement_
#define DrTypeRefElem u.s1.u.s0._DrTypeRefElem_
#define DrRefPresBox u.s1.u.s1._DrRefPresBox_

/* type du contenu d'une boite de presentation */
typedef enum
{
    FreeContent, ContVariable, ContConst, ContElement
}	ContentType;

/* une boite de presentation */
typedef struct _PresentationBox
{
  Name		 PbName;		/* Name de la boite */
  PtrPRule PbFirstPRule;	/* premiere regle de presentation
				   definissant la boite */
  boolean	 PbAcceptPageBreak;	/* indique si la boite peut etre */
				/* coupee ou non en bas de page. */
  boolean	 PbAcceptLineBreak;	/* indique si la boite peut etre */
				/* coupee ou non en bout de ligne. */
  boolean	 PbBuildAll;	/* indique si l'image de la boite doit etre
				   construite d'un seul bloc ou peut etre
				   fractionnee */
  boolean	 PbNotInLine;	/* boite non prise en compte dans la mise
				   en lignes */
  boolean	 PbPageFooter;	/* c'est une boite de bas de page */
  boolean	 PbPageHeader;	/* c'est une boite de haut de page */
  boolean	 PbPageBox;		/* c'est une boite page */
  int	 PbFooterHeight;  /* si c'est une boite page, hauteur du bas
				      de page en points typo */
  int	 PbHeaderHeight; /* si c'est une boite page, hauteur du haut
				      de page en points typo */
  int		 PbPageCounter;	/* numero du compteur de page, seulement si
				   PbPageBox est vrai */
  ContentType	 PbContent;		/* contenu impose de la boite */
  union
  {
    struct			/* PbContent = ContVariable */
    {
      int	  _PbContVariable_;	/* numero de la variable */
    }   s0;
    struct			/* PbContent = ContConst */
    {
      int	  _PbContConstant_;	/* numero de la constante */
    } s1;
    struct			/* PbContent = ContElement */
    {
      int	  _PbContElem_;	/* numero du type d'elem */
      int	  _PbContRefElem_;	/* numero de type de la reference a cet elem */
    } s2;
    struct			/* PbContent = FreeContent */
    {
      char  _PbContFree_; /* pour faire plaisir au compilateur */
    } s3;
  } u;
} PresentationBox;

#define PbContVariable u.s0._PbContVariable_
#define PbContConstant u.s1._PbContConstant_
#define PbContElem u.s2._PbContElem_
#define PbContRefElem u.s2._PbContRefElem_
#define PbContFree u.s3._PbContFree_

/* BAlignment des lignes dans un pave mis en ligne */
typedef enum
{
  AlignLeft, AlignRight, AlignCenter, AlignLeftDots
} BAlignment;	/* AlignLeftDots = Cadre' a gauche, la derniere
		   ligne remplie avec des points */
typedef enum
{
  /* l'ordre determine celui des regles dans le schema de presentation */
  PtVisibility, PtFunction, PtVertRef, PtHorizRef, PtHeight, PtWidth, PtVertPos, PtHorizPos, PtSize,
  PtStyle, PtFont, PtUnderline, PtThickness, PtIndent, PtLineSpacing, PtDepth, PtAdjust,
  PtJustify, PtLineStyle, PtLineWeight, PtFillPattern, PtBackground,
  PtForeground, PtHyphenate,
  /* Les 3 types suivants doivent etre les derniers */
  PtBreak1, PtBreak2, PtImDescr
} PRuleType;

/* mode de calcul des proprietes */
typedef enum
{
  PresImmediate, PresInherit, PresFunction
} PresMode;

typedef enum
{
  InheritParent, InheritPrevious, InheritChild, InheritCreator, InheritGrandFather
} InheritMode;

/* l'ordre determine celui des regles dans le schema de presentation*/
typedef enum
{
  FnLine, FnPage, FnCreateBefore, FnCreateWith, FnCreateFirst, FnCreateLast,
  FnCreateAfter, FnColumn, FnCopy, FnContentRef, FnSubColumn, FnNoLine,
  FnCreateEnclosing
} FunctionType;

typedef enum
{
  CntArabic, CntURoman, CntLRoman, CntUppercase, CntLowercase
} CounterStyle;

/* types des variables de presentation */
typedef enum
{
  VarText, VarCounter, VarDate, VarFDate, VarDirName, VarDocName, VarElemName,
  VarAttrName, VarAttrValue, VarPageNumber
} VariableType;

/* Pour indiquer la nature de la valeur du compteur */
typedef enum
{
  CntMaxVal, CntMinVal, CntCurVal
} CounterValue;

/* un element d'une variable */
typedef struct _PresVarItem
{
  VariableType     ViType;
  CounterStyle  ViStyle;	/* style des chiffres pour */
  /* VarCounter, VarAttrValue et VarPageNumber */
  union
  {
    struct
    {
      int	     _ViConstant_;	 /* numero de la constante */
    } s0;
    struct
    {
      int	     _ViCounter_;   /* numero du compteur */
      CounterValue _ViCounterVal_; /* indique si on s'interesse a la
				        valeur maximale, minimale ou courante du compteur */
    } s1;
    struct
    {
      int  _ViAttr_;  /* numero de l'attribut */
    } s2;
    struct
    {
      int            _ViDate_;      /* pour le compilateur */
    } s3;
    struct			    /* ViType = VarPageNumber */
    {
      int	     _ViView_;/* numero de la vue  ou on compte
				       les pages */
    } s4;
  } u;
} PresVarItem;

#define ViConstant u.s0._ViConstant_
#define ViCounter u.s1._ViCounter_
#define ViCounterVal u.s1._ViCounterVal_
#define ViAttr u.s2._ViAttr_
#define ViDate u.s3._ViDate_
#define ViView u.s4._ViView_

/* Un type de condition de creation de boite */
typedef enum
{
  PcFirst, PcLast, PcReferred, PcFirstRef, PcLastRef, PcExternalRef, PcInternalRef,
  PcCopyRef, PcAnyAttributes, PcFirstAttr, PcLastAttr, PcUserPage, PcStartPage, PcComputedPage,
  PcEmpty, PcEven, PcOdd, PcOne, PcInterval, PcWithin, PcElemType, PcAttribute,
  PcNoCondition, PcDefaultCond
} PresCondition;

/* Pour interpreter le champ CoRelation */
typedef enum
{
  CondGreater, CondLess, CondEquals
} ArithRel;

typedef struct _Condition *PtrCondition;

/* Une condition d'application d'une regle de presentation */
typedef struct _Condition
{
  PtrCondition    CoNextCondition;	/* Condition suivante dans la liste
					   IF cond AND cond AND cond... */
  PresCondition   CoCondition;		/* type de la condition */
  boolean         CoNotNegative;	/* la condition n'est pas negative */
  boolean	  CoTarget;		/* la condition concerne la cible
					   (uniquement pour les references) */
  union
  {
    struct				/* CoCondition = PcInterval, PcEven,
					   PcOdd, PcOne */
    {
      int	  _CoCounter_; /* numero du compteur sur lequel porte
					   la condition */
      int	  _CoMinCounter_; /* valeur minimum du compteur pour que
					   la regle de presentation soit
					   appliquee */
      int	  _CoMaxCounter_; /* valeur maximum du compteur pour que
					   la regle de presentation soit
					   appliquee */
      CounterValue _CoValCounter_; /* Indique si on prend la valeur
					   courante du compteur, sa valeur
					   mini ou sa valeur maxi */
    } s0;
    struct				/* CoCondition = PcWithin */
    {
      int	  _CoRelation_;		/* RelLevel */
      int	  _CoTypeAncestor_;		/* type de l'ancetre */
      boolean	  _CoImmediate_;	/* Immediately */
      ArithRel  _CoAncestorRel_;
      Name	  _CoAncestorName_;	/* nom du type de l'ancetre, si
					   defini dans un autre schema */
      Name	  _CoSSchemaName_;	/* nom du schema ou est defini
					   l'ancetre si CoTypeAncestor=0*/
    } s1;
    struct				/* CoCondition = PcElemType or
					   PcAttribute */
    {
      int	  _CoTypeElAttr_;	/* PcElemType: type de l'elem
					   auquel l'attribut est attache'.
					   PcAttribute: attribut porte' par
					   l'element. */
    } s2;
  } u;
} Condition;

#define CoCounter u.s0._CoCounter_
#define CoMinCounter u.s0._CoMinCounter_
#define CoMaxCounter u.s0._CoMaxCounter_
#define CoValCounter u.s0._CoValCounter_
#define CoRelation u.s1._CoRelation_
#define CoTypeAncestor u.s1._CoTypeAncestor_
#define CoImmediate u.s1._CoImmediate_
#define CoAncestorRel u.s1._CoAncestorRel_
#define CoAncestorName u.s1._CoAncestorName_
#define CoSSchemaName u.s1._CoSSchemaName_
#define CoTypeElAttr u.s2._CoTypeElAttr_

/* Les regles de presentation relatives a un objet sont chainees entre
   elles par le pointeur PrNextPRule. Ainsi les regles de presentation
   d'un type defini dans le schema de structure sont chainees, de meme
   que les regles de presentation d'une valeur d'attribut logique, ou
   d'une boite de presentation, ou encore les regles par defaut. */

/* une regle de presentation */
typedef struct _PresRule
{
  PRuleType       PrType;		/* type de la regle */
  PtrPRule    PrNextPRule;	/* regle suivante pour le meme objet */
  PtrCondition    PrCond;		/* conditions d'application de la
					   regle */
  int          PrViewNum;		/* numero de la vue a laquelle la regle
					   s'applique */
  int  PrSpecifAttr;	/* Seulement pour les regles de pres.
					   specifiques attaches aux elements de
					   l'arbre abstrait: numero de l'attr.
					   auquel correspond la regle, 0 si la
					   regle n'est pas derivee d'une regle
					   d'attribut */
  PtrSSchema    PrSpecifAttrSSchema;	/* pointeur sur le schema de structure
					   definissant l'attr. PrSpecifAttr */
  PresMode	  PrPresMode;		/* mode de calcul de la valeur */ 
  union
  {
    struct				/* PrPresMode = PresInherit */
    {
      InheritMode _PrInheritMode_;
      boolean      _PrInhAttr_;	  /* PrInhDelta est un numero d'attribut
				     numerique et si faux une valeur */
      int      _PrInhDelta_;	  /* positif: increment, nul: egalite,
				     negatif: decrement */
      boolean      _PrMinMaxAttr_; /* PrInhMinOrMax est un numero
				     d'attribut numerique ou une valeur*/
      int      _PrInhMinOrMax_; /* valeur min ou max de l'heritage*/
      TypeUnit     _PrInhUnit_;/* PrInhDelta et PrInhMinOrMax sont
				     exprimes en points typo, pixel,
				     valeur relative, etc. */
    } s0;
    struct			  /* PrPresMode = PresFunction */
    {
      FunctionType    _PrPresFunction_;
      boolean      _PrPresBoxRepeat_;	/* boite de presentation repetee sur
				   tous les paves de l'element */
      boolean	   _PrExternal_;/* si PrElement est vrai, PrExternal
				   indique que le type dont le nom est
				   dans PrPresBoxName est externe */
      boolean      _PrElement_;	/* PrPresBox[1] ou PrPresBoxName est un
				   numero de type d'element et non de
				   boite de presentation */
      int	   _PrNPresBoxes_;	/* nombre de boites de presentation
				   (utile seulement pour la regle
				   Colonne) */
	    int          _PrPresBox_[MAX_COLUMN_PAGE];	/* numero des boites de pres.*/
	    Name          _PrPresBoxName_;	/* Name de la 1ere (ou seule) boite de
					   presentation concernee par la
					   fonction */
    } s1;
    struct			 /* PrPresMode = PresImmediate */
    {
      union
      {
	struct	/* PRuleType = PtVisibility, PtDepth, PtFillPattern, */
	        /* PtBackground, PtForeground */
	{
	  boolean  _PrAttrValue_; /* PrIntValue est un numero d'attribut
				    numerique ou une valeur numerique */
	  int  _PrIntValue_;
	}  s0;
	struct	/* PRuleType = PtFont, PtStyle, PtUnderline, PtThickness,*/
	        /* PtLineStyle */
	{
	  char     _PrChrValue_;
	}  s1;
	struct	/* PRuleType = PtBreak1, PtBreak2, */
		/* PtIndent, PtSize, PtLineSpacing, PtLineWeight */
	{
	  TypeUnit _PrMinUnit_;/* La hauteur min est exprimee en
				     points typo, en 1/10 de
				     caracteres, etc. */
	  boolean  _PrMinAttr_;/* Le champ suivant est un numero
				      d'attribut ou une valeur */
	  int  _PrMinValue_;    /* Valeur de la hauteur minimum */
	} s2;
	struct	/* PRuleType = PtVertRef, PtHorizRef, PtVertPos, PtHorizPos*/
	{
	  PosRule _PrPosRule_;
	} s3;
	struct	/* PRuleType = PtHeight, PtWidth */
	{
	  DimensionRule _PrDimRule_;
	} s4;
	struct	/* PRuleType = PtJustify */
	{
	  boolean _PrJustify_;
	} s5;
	struct	/* PRuleType = PtAdjust */
	{
	  BAlignment _PrAdjust_;
	} s6;
	/* synchroniser cette structure avec ImageDescriptor */
	struct	/* PRuleType = PtImDescr */
	{
	  int		xcf, ycf, wcf, hcf;
	  PictureScaling	imagePres;
	  int		imageType;
	} s7;
      } u;
    } s2;
  } u;
} PresRule;

#define PrInheritMode u.s0._PrInheritMode_
#define PrInhAttr u.s0._PrInhAttr_
#define PrInhDelta u.s0._PrInhDelta_
#define PrMinMaxAttr u.s0._PrMinMaxAttr_
#define PrInhMinOrMax u.s0._PrInhMinOrMax_
#define PrInhUnit u.s0._PrInhUnit_
#define PrPresFunction u.s1._PrPresFunction_
#define PrPresBoxRepeat u.s1._PrPresBoxRepeat_
#define PrExternal u.s1._PrExternal_
#define PrElement u.s1._PrElement_
#define PrNPresBoxes u.s1._PrNPresBoxes_
#define PrPresBox u.s1._PrPresBox_
#define PrPresBoxName u.s1._PrPresBoxName_
#define PrAttrValue u.s2.u.s0._PrAttrValue_
#define PrIntValue u.s2.u.s0._PrIntValue_
#define PrChrValue u.s2.u.s1._PrChrValue_
#define PrMinUnit u.s2.u.s2._PrMinUnit_
#define PrMinAttr u.s2.u.s2._PrMinAttr_
#define PrMinValue u.s2.u.s2._PrMinValue_
#define PrPosRule u.s2.u.s3._PrPosRule_
#define PrDimRule u.s2.u.s4._PrDimRule_
#define PrJustify u.s2.u.s5._PrJustify_
#define PrAdjust u.s2.u.s6._PrAdjust_
#define PrImageDescr u.s2.u.s7

/* operation sur compteur */
typedef enum
{
  CntrSet, CntrAdd, CntrRank, CntrRLevel
} CounterOp;

/* une operation elementaire pour un compteur */
typedef struct _CntrItem
{
  CounterOp       CiCntrOp;	/* l'operation */
  int             CiElemType;	/* numero du type d'element (si CECatElem =
				   ElementStruct) qui declenche l'operation */
  int		  CiAscendLevel;  /* Level du parent que l'on souhaite compter
				   pour structures recursives */
  int          CiViewNum;	/* numero de la vue a laquelle se rapportent
				   les pages a compter (si CiElemType est une
				   page) */
  int             CiParamValue;	/* valeur du parametre de l'operation */
  int  CiInitAttr;	/* TtAttribute d'un ascendant qui donne sa valeur
				   initiale au compteur, 0 si la valeur initial
				   ne depend pas d'un attribut */
  int  CiReinitAttr;/* TtAttribute qui donne sa valeur au compteur
				   pour l'element qui le porte et les elements
				   suivants, 0 si pas de reinitialisation */
} CntrItem;

/* un compteur */
typedef struct _Counter
{
  int		 CnNItems;	/* Nombre d'operations sur ce compteur */
  CntrItem        CnItem[MAX_PRES_COUNT_ITEM];	/* les operations */
  int		 CnNPresBoxes;	/* Nombre d'elements dans CnPresBox */
  int   CnPresBox[MAX_PRES_COUNT_USER];  /* liste des numeros de type des
				   boites de presentation utilisant le compteur
				   dans leur contenu */
  boolean	 CnMinMaxPresBox[MAX_PRES_COUNT_USER];
  int		 CnNTransmAttrs; /* nombre d'attributs externes auxquels est
				   transmise la valeur du compteur */
  Name		 CnTransmAttr[MAX_TRANSM_ATTR]; /* noms des attributs auxquels est
				   transmise la valeur du compteur */
  int		 CnTransmSSchemaAttr[MAX_TRANSM_ATTR]; /* numero de type des
				   documents externes ou sont definis les
				   attributs CnTransmAttr */
  int		 CnNCreators;	/* Nombre d'elements dans CnCreator */
  int   CnCreator[MAX_PRES_COUNT_USER]; /* liste des numeros de type de
				   boite qui creent d'autres boites selon la
				   valeur du compteur. */
  boolean	 CnMinMaxCreator[MAX_PRES_COUNT_USER];
  boolean        CnPresBoxCreator[MAX_PRES_COUNT_USER];	/* indique si la boite
				   correspondante de CnCreator est une boite
				   de presentation ou non */
  int		 CnNCreatedBoxes; /* Nombre d'elements dans CnCreatedBox */
  int   CnCreatedBox[MAX_PRES_COUNT_USER]; /* liste des numeros de type des
				   boites de presentation creee selon la valeur
				   du compteur */
  boolean	 CnMinMaxCreatedBox[MAX_PRES_COUNT_USER];
  boolean        CnPageFooter;	/* ce compteur est utilise' en bas de  page */
} Counter;

/* une constante de presentation */
typedef struct _PresConstant
{
  BasicType     PdType;	/* type de la constante */
  char           PdAlphabet;	/* alphabet de la constante */
  char           PdString[MAX_PRES_CONST_LEN];	/* chaine constante de presentation,
				   terminee par un NUL */
} PresConstant;

/* une variable de presentation est la concatenation des resultats de
   plusieurs elements */
typedef struct _PresVariable
{
    int		 PvNItems;	/* nombre effectif d'elements*/
    PresVarItem      PvItem[MAX_PRES_VAR_ITEM];   /* liste des elements */
} PresVariable;

typedef enum 
{
  ComparConstant, ComparAttr
} AttrComparType;


/* un cas d'application de regles de presentation pour un attribut a
   valeur numerique */
typedef struct _NumAttrCase
{
  AttrComparType  CaComparType;/* type des elements de comparaison */
  int	 	  CaLowerBound; /* valeur minimum de l'attribut pour que les
				   regles de presentation soient appliquees */
  int	 	  CaUpperBound;	/* valeur maximum de l'attribut pour que les
				   regles de presentation soient appliquees */
  PtrPRule    CaFirstPRule;/* premiere regle de la chaine des regles a
				   appliquer quand la valeur de l'attribut est
				   dans l'intervalle */
} NumAttrCase;

/* presentation d'un attribut logique, selon son type */
typedef struct _AttributePres
{
  int     ApElemType;/* Type d'element auquel s'applique les regles
				  de presentation, 0 si les regles
				  s'appliquent quel que soit le type d'elem*/
  struct _AttributePres *ApNextAttrPres; /* le paquet de regles de presentation
					  pour l'element suivant */
  union
  {
    struct
    {
      int	  _ApNCases_;	/* nombre de cas d'application de
				   regles de presentation */
      NumAttrCase  _ApCase_[MAX_PRES_ATTR_CASE]; /* les cas d'application de
					      regles de presentation */
    } s0;
    struct
    {
      PtrPRule _ApRefFirstPRule_; /* premiere regle de la chaine des
					 regles a appliquer pour l'attribut*/
    } s1;
    struct
    {
      Name	   _ApString_;	/*la valeur qui declenche l'application
				  des regles de presentation */
      PtrPRule _ApTextFirstPRule_;	/* premiere regle de la chaine des
				   regles a appliquer pour cette val */
    } s2;
    struct
    {
      PtrPRule _ApEnumFirstPRule_[MAX_ATTR_VAL + 1]; /* pour chaque valeur de
					   l'attribut, dans l'ordre de la table
					   AttrEnumValue, adresse de la premiere
					   regle de presentation associee a
					   cette valeur */
    } s3;
  } u;
} AttributePres;

#define ApNCases u.s0._ApNCases_
#define ApCase u.s0._ApCase_
#define ApRefFirstPRule u.s1._ApRefFirstPRule_
#define ApString u.s2._ApString_
#define ApTextFirstPRule u.s2._ApTextFirstPRule_
#define ApEnumFirstPRule u.s3._ApEnumFirstPRule_

/* table de noms de vues */
typedef Name     ViewTable[MAX_VIEW];

/* description d'une vue a imprimer */
typedef struct _PrintedView
{
    boolean     VpAssoc;	/* c'est une vue d'elements associes */
    int	 	VpNumber;    	/* numero de la vue, ou du type de la liste
				   d'elements associes, si VpAssoc. */
} PrintedView;

typedef PtrPRule PtrPRuleTable[MAX_RULES_SSCHEMA];    /* Table des adresses de
				  regles de presentation des Types */

/* une regle de transmission d'une valeur d'element a un attribut */
/* d'un document inclus */
typedef struct _TransmitElem
{
    int		TeTargetDoc;	/* indice de la regle de structure */
				/* qui definit le type de document inclus */
    Name		TeTargetAttr;	/* nom de l'attribut du document */
				/* inclus auquel on transmet la valeur de */
				/* l'element */
} TransmitElem;

typedef boolean InheritAttrTable[MAX_ATTR_SSCHEMA];	/* cette table attachee a un
					  element indique quels sont les
					  attributs dont herite cet element */
typedef boolean ComparAttrTable[MAX_ATTR_SSCHEMA]; /* cette table attachee a un
					  attribut indique quels sont les
					  attributs qui se comparent a lui
					  pour la presentation */

/* un schema de presentation charge' en memoire */
typedef struct _PresentSchema
{
  PtrPSchema    PsNext;    		/* pour le chainage des blocs libres */
  Name	 	PsStructName;    	/* nom du schema de structure */
  Name		PsPresentName;		/* nom de ce schema de presentation */
  int	 	PsStructCode;    	/* code identifiant la version de
					   ce schema de structure */
  int		PsNViews;	    	/* nombre de vues */
  ViewTable       PsView;    		/* definition des vues */
  boolean       PsPaginatedView[MAX_VIEW]; /* indique les vues qui */
 				        /* sont decoupees en pages */
  boolean       PsColumnView[MAX_VIEW];	/* indique les vues qui */
					/* sont decoupees en colonnes */
  int		PsNPrintedViews;		/* nombre de vues a imprimer */
  PrintedView    PsPrintedView[MAX_PRINT_VIEW];/* les vues a imprimer */
  boolean       PsExportView[MAX_VIEW];    /* indique les vues qui affichent les
					   seuls elements exportes */
  int		PsNCounters;		/* nombre de compteurs */
  int		PsNConstants;		/* nombre de constantes de present. */
  int		PsNVariables;		/* nombre de variables de present. */
  int		PsNPresentBoxes;	/* nombre de boites de presentation et
					   de mise en page */
  PtrPRule  PsFirstDefaultPRule;	/* debut de la chaine des regles par
					   defaut */
  Counter      PsCounter[MAX_PRES_COUNTER]; /* compteurs */
  PresConstant     PsConstant[MAX_PRES_CONST];	/* constantes de presentation */
  PresVariable       PsVariable[MAX_PRES_VARIABLE];	/* variables de presentation */
  PresentationBox        PsPresentBox[MAX_PRES_BOX]; /* descriptions des boites de
					   presentation et de mise en page */
	/* Pour la presentation en colonnes, la boite numero 0 */
	/* contient la boite Groupe de colonnes, la boite 1 contient */
	/* la colonne de gauche, etc. */
    
  AttributePres *PsAttrPRule[MAX_ATTR_SSCHEMA];/* pointeurs sur les regles de 
					   presentation des attributs logiques,
					   dans l'ordre de la table
					   StructSchema.SsAttribute */
  int		PsNAttrPRule[MAX_ATTR_SSCHEMA]; /* nombre de paquets de
					   regles de presentation pour chaque
					   attribut logique, ie taille des
					   chaines de AttributePres dans la
					   table PsAttrPRule, dans l'ordre de
					   la table StructSchema.SsAttribute */
  PtrPRuleTable	PsElemPRule;    	/* pointeurs sur le debut de la chaine
					  de regles de presentation specifiques
					  a chaque type d'element, dans le meme
					  ordre que dans la table
					  StructSchema.SsRule */
    
  int    	PsNHeirElems[MAX_ATTR_SSCHEMA]; /* pour chaque attribut,
					  dans le meme ordre que dans la table
					  StructSchema.SsAttribute, indique le
					  nombre d'elements pouvant heriter de
					  l'attribut */
  int    	PsNInheritedAttrs[MAX_RULES_SSCHEMA]; /* pour chaque type d'element,
					  dans le meme ordre que dans la table
					  StructSchema.SsRule, indique le
					  nombre d'attributs herites par
					  l'element */
  InheritAttrTable *PsInheritedAttr[MAX_RULES_SSCHEMA]; /* pour chaque type
					  d'element, dans le meme ordre que
					  dans la table StructSchema.SsRule,
					  pointe sur la table indiquant quels
					  sont les attr herites par l'element*/
  int    	PsNComparAttrs[MAX_ATTR_SSCHEMA]; /* pour chaque attribut
					  dans le meme ordre que dans la table
					  StructSchema.SsAttribute, indique le
					  nombre d'attributs se comparant a
					  l'attribut pour en deduire de la
					  presentation */
  ComparAttrTable *PsComparAttr[MAX_ATTR_SSCHEMA]; /* pour chaque attr,
					  dans le meme ordre que dans la table
					  StructSchema.SsAttribute, pointe sur la
					  table indiquant quels attributs se
					  comparent a l'attribut pour en
					  deduire une presentation */
  boolean     PsAcceptPageBreak[MAX_RULES_SSCHEMA];/* pour chaque type d'element,
					  dans le meme ordre que dans la table
					  StructSchema.SsRule,	indique si
					  l'element peut etre coupe' en bas de
					  page. */
  boolean     PsAcceptLineBreak[MAX_RULES_SSCHEMA];/* pour chaque type d'element,
					  dans le meme ordre que dans la table
					  StructSchema.SsRule,	indique si
					  l'element peut etre coupe' en bout
					  de ligne. */
  boolean     PsBuildAll[MAX_RULES_SSCHEMA];    /* pour chaque type d'element,
					  dans le meme ordre que dans la table
					  StructSchema.SsRule, indique si
					  l'image de la boite doit etre
					  construite d'un seul bloc ou peut
					  etre fractionnee. */
  boolean	PsNotInLine[MAX_RULES_SSCHEMA];	/* pour chaque type d'element,
					  dans le meme ordre que dans la table
					  StructSchema.SsRule,	indique que
					  l'element ne doit pas etre pris en
					  compte dans la mise en lignes */
  boolean     PsInPageHeaderOrFooter[MAX_RULES_SSCHEMA];    /* pour chaque type d'element,
					  dans le meme ordre que dans la table
					  StructSchema.SsRule,indique si
					  l'element est affiche'dans le corps
					  des pages (si faux) ou dans une boite
					  de haut ou bas de page (si vrai)  */
  boolean     PsAssocPaginated[MAX_RULES_SSCHEMA];/* pour chaque type d'element,
					  dans le meme ordre que dans la table
					  StructSchema.SsRule, indique si
					  l'element est mis en pages
					  (significatif uniquement pour les
					  listes d'elements associes). */
  int		PsElemTransmit[MAX_RULES_SSCHEMA];	/* pour chaque type d'element,
					  dans le meme ordre que dans la table
					  StructSchema.SsRule, indice dans la
					  table SPTransmit de l'entree qui
					  donne la transmission des valeurs de
					  l'element aux attributs des documents
					  inclus */
  int		PsNTransmElems;		/* nombre d'entrees dans la table
					  PsTransmElem */
  TransmitElem	PsTransmElem[MAX_TRANSM_ELEM];/* table des transmissions des
					  des valeurs d'element a des attributs
					  de documents inclus */
} PresentSchema;
