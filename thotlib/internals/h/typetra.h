/* -- Copyright (c) 1990 - 1996 Inria/CNRS  All rights reserved. -- */

/*
 * Structures de donnees pour le traducteur.
 *
 * Declarations de types pour la traduction des documents
 * V. Quint	Janvier 1988 
 *
 */

/* 
   DEFINITIONS:

   Un numero de compteur est le rang d'un compteur dans la table
	TranslSchema.TsCounter.
   Un numero de constantes est le rang d'une constante dans la table
	TranslSchema.STConst.
   Un numero de variable est le rang d'une variable dans la table
	TranslSchema.TsVariable. 
 */

/* un type de condition d'application de regle de traduction */
typedef enum
{
	TcondFirst,
	TcondLast,
	TcondDefined,
	TcondReferred,
	TcondFirstRef,
	TcondLastRef,
	TcondWithin,
	TcondFirstWithin,
	TcondAttr,
	TcondPresentation,
	TcondPRule,
	TcondComment,
	TcondAttributes,
	TcondFirstAttr,
	TcondLastAttr,
	TcondAlphabet,
	TcondComputedPage,
	TcondStartPage,
	TcondUserPage,
	TcondReminderPage,
	TcondEmpty,
	TcondExternalRef
} TransCondition;

/* un type de regle de traduction */
typedef enum
{
	TCreate,
	TGet,
	TUse,
	TRemove,
	TWrite,
	TRead,
	TInclude,
	TNoTranslation,
	TCopy,
	TChangeMainFile,
	TSetCounter,
	TAddCounter,
	TNoLineBreak
} TRuleType;

/* la position ou il faut creer les chaines produites par le traducteur */
typedef enum
{
	TAfter, 
	TBefore
} TOrder;

/* Les regles de traduction relatives a un objet sont groupees en blocs
   Les regles d'un meme bloc s'appliquent selon la meme condition.
   Une chaine de blocs de regles s'applique a un meme objet. Ainsi les
   blocs de regles de traduction d'un type defini dans le schema de
   structure sont chaines, de meme que les blocs de regles de traduction
   d'une valeur d'attribut. 
 */

/* un pointeur sur une regle de traduction */
typedef struct _TranslRule *PtrTRule;

/* un pointeur sur un bloc de regles de traduction */
typedef struct _TRuleBlock *PtrTRuleBlock;

/* pour interpreter le champ TcAscendLevel */
typedef enum
{
	RelGreater, RelLess, RelEquals
}       RelatNAscend;

/* une condition d'application d'un bloc de regles de traduction */
typedef struct _TranslCondition
{
	TransCondition    TcCondition;   	/* type de la condition d'application
					   des regles du bloc */
	boolean         TcNegativeCond;	/* la condition est negative */
	boolean		TcTarget;	/* la condition porte sur l'element
					   reference' */
	int		TcAscendType; /* type de l'element ascendant
					   concerne' par la condition */
	Name		TcAscendNature; /* nom du schema ou est
					 defini ce nom de type d'element */
	int		TcAscendRelLevel; /* niveau relatif de l'element
					 ascendant concerne' par la condition.
					 0  = element lui-meme
					 -1 = l'element lui-meme ou n'importe
					      quel ascendant */
	union
	  {
	  struct	/* TcCondition = TcondAlphabet */
	    {
	    char        _BlCondAlphabet_;  /* l'alphabet sur lequel porte la
					   condition */
	    } s0;
	  struct	/* TcCondition = TcondWithin, TcondFirstWithin */
	    {
	    int		_BlCondTypeElem_; /* type de l'ascendant */
	    Name		_BlCondNatureElem_;/* nom de la nature ou est defini
					 TcElemType, 0 si meme schema */
	    boolean	_BlCondNivAncetreImmediat_; /* Immediately within */
	    RelatNAscend	_BlCondNivAncetreSupInfEgal_;
	    int		_BlCondNivAncetreNiveau_;   /* RelLevel */
	    } s1;
	  struct	/* TcCondition = TcondAttr || TcCondition = TcondPRule */
	    {
	    int		_BlCondAttr_;    /*  numero de l'attribut ou type de
					 la regle de presentation */
	    union
	      {
	      struct	/* AttribType = AtNumAttr */
	        {
	        int	_BlCBorneInf_;	/*valeur minimum de l'attribut pour que
					les regles du bloc soient appliquees */
	        int	_BlCBorneSup_;	/*valeur maximum de l'attribut pour que
					les regles du bloc soient appliquees */
	        } s0;
	      struct	/* AttribType = AtTextAttr */
	        {
	        Name	_BlCTxtVal_;	/* la valeur qui declanche
					l'application des regles du bloc */
	        } s1;
	      struct	/* AttribType = AtEnumAttr */
	        {
	        int	 _BlCAttrVal_;	/* Valeur de l'attribut pour laquelle
				        on applique les regles du bloc,
				        0 si on applique pour toute valeur */
	        } s2;
	      struct	/* dans le cas TcondPRule seulement */
	        {
	        char	_BlCCarVal_;    /* Valeur de la presentation pour
				       laquelle on applique les regles du bloc,
				        0 si on applique pour toute valeur */
	        } s3;
	      } u;
	    } s2;
	  } u;

} TranslCondition;

#define TcAlphabet u.s0._BlCondAlphabet_
#define TcImmediatelyWithin u.s1._BlCondNivAncetreImmediat_
#define TcAscendRel u.s1._BlCondNivAncetreSupInfEgal_
#define TcAscendLevel u.s1._BlCondNivAncetreNiveau_
#define TcElemType u.s1._BlCondTypeElem_
#define TcElemNature u.s1._BlCondNatureElem_
#define TcAttr u.s2._BlCondAttr_
#define TcLowerBound u.s2.u.s0._BlCBorneInf_
#define TcUpperBound u.s2.u.s0._BlCBorneSup_
#define TcTextValue u.s2.u.s1._BlCTxtVal_
#define TcAttrValue u.s2.u.s2._BlCAttrVal_
#define TcPresValue u.s2.u.s3._BlCCarVal_

/* un bloc de regles de traduction */
typedef struct _TRuleBlock
{
	PtrTRuleBlock   TbNextBlock; /* pointeur sur le bloc de regles suivant */
	PtrTRule    TbFirstTRule;  /* pointeur sur la 1ere regle du bloc */
	int		TbNConditions; /* nombre de conditions dans TbCondition */ 
	TranslCondition	TbCondition[MAX_TRANSL_COND]; /* les conditions qui doivent
			    etre realisees pour appliquer les regles du bloc */
} TRuleBlock;

/* type de chose a creer dans le fichier de sortie ou a ecrire au terminal */
typedef enum
{
  	ToConst,
	ToBuffer,
	ToVariable,
	ToAttr,
	ToPRuleValue,
	ToContent,
	ToComment,
	ToAllAttr,
	ToAllPRules,
	ToRefId,
	ToPairId,
	ToReferredElem,
	ToDocumentName,
	ToDocumentDir,
	ToReferredDocumentName,
	ToReferredDocumentDir,
	ToReferredRefId,
	ToFileDir,
	ToFileName,
	ToExtension
} CreatedObject;

/* position relative, dans l'arbre abstrait, de l'element a prendre par une */
/* regle Get ou Copy. */
typedef enum
{
  	RpSibling,
	RpDescend,
	RpReferred,
	RpAssoc
} TRelatPosition;

/* une regle de traduction */
typedef struct _TranslRule
{
  PtrTRule    TrNextTRule;	/* regle suivante du meme bloc */
  TOrder      TrOrder;	/* position relative */
  TRuleType	TrType;		/* type de la regle */
  union
    {
      struct	/* TrType = TCreate, TWrite */
	{
	  CreatedObject 	_RTToken_;	/*type de chose a creer dans le fichier
					   de sortie ou a ecrire au terminal */
	  int	 	_RTNumToken_;	/* numero de la chose a creer ou a
					   ecrire */
	  Name		_RTNatureCr_;	/* Schema ou l'element est defini, */
					/* 0 si meme schema de structure */
	  boolean	_RTRef_;	/* la variable a creer doit etre
					   calculee pour l'element reference',
					   et non pour l'element lui-meme */
	  int		_RTVarFichier_;	/* variable definissant le nom du
					   fichier de sortie */
	} s0;
      struct	/* TrType = TGet, TCopy */
	{
	  int		_RTTypeGet_;	/* type de l'element a prendre */
	  Name	 	_RTNatureGet_;	/* Schema ou l'element est defini, 
					   0 si meme schema de structure */
	  TRelatPosition 	_RTRelPos_;	/* position relative dans l'arbre
					   abstrait de l'element a prendre */
	} s1;
      struct	/* TrType = TUse */
	{
	  Name	 _RTNomNature_;		/* nom de la nature */
	  Name	 _RTNomSchTrad_;	/* nom du schema de trad. a appliquer*/
	} s2;
      struct	/* TrType = TRead */
	{
	  int	_RTBuffer_; 		/* numero du buffer de lecture */
	} s4;
      struct	/* TrType = TInclude */
	{
	  CreatedObject  _RTTkFile_;		/* le nom du fichier a inclure est 
					   dans un buffer ou une constante */
	  int	  _RTNFile_; 		/* numero de la constante ou du
					   buffer qui contient le nom du
					   fichier a inclure */
	} s5;
      struct	/* TrType = TChangeMainFile */
	{
	  int	_RTVarNouvFichier_;	 /* variable definissant le nouveau
					    nom de fichier */
	} s6;
     struct	/* TrType = TSetCounter, TAddCounter */
	{
	  int	_RTNumCompteur_;	 /*numero du compteur a mettre a jour*/
	  int	_RTValCompteur_;	 /* valeur de la mise a jour */
	} s7;
    } u;
} TranslRule;

#define TrObject u.s0._RTToken_
#define TrObjectNum u.s0._RTNumToken_
#define TrObjectNature u.s0._RTNatureCr_
#define TrReferredObj u.s0._RTRef_
#define TrFileNameVar u.s0._RTVarFichier_
#define TrElemType u.s1._RTTypeGet_
#define TrElemNature u.s1._RTNatureGet_
#define TrRelPosition u.s1._RTRelPos_
#define TrNature u.s2._RTNomNature_
#define TrTranslSchemaName u.s2._RTNomSchTrad_
#define TrBuffer u.s4._RTBuffer_
#define TrBufOrConst u.s5._RTTkFile_
#define TrInclFile u.s5._RTNFile_
#define TrNewFileVar u.s6._RTVarNouvFichier_
#define TrCounterNum u.s7._RTNumCompteur_
#define TrCounterParam u.s7._RTValCompteur_

/* type d'une operation sur un compteur */
typedef enum
{
	TCntrRank,
	TCntrRLevel,
	TCntrSet,
	TCntrNoOp
} TCounterOp;

/* un compteur */
typedef struct _TCounter
{
	TCounterOp     TnOperation;	/* type de l'operation du compteur */
	int 		TnElemType1;	/* premier type d'element sur lequel
					   s'applique l'operation du compteur*/
	int		TnAcestorLevel;	/* Si "CntrRank of", niveau du parent que
					   l'on souhaite compter */
	int 		TnElemType2;	/* deuxieme type d'element sur lequel
					   s'applique l'operation du compteur*/
	int             TnParam1;	/* premier parametre de l'operation */
	int             TnParam2;	/* deuxieme parametre de l'operation*/
	int  TnAttrInit;	/* TtAttribute de la racine qui donne sa
					   valeur initiale au compteur, 
					   0 si la valeur initiale ne depend
					   depend pas d'un attribut */
} TCounter;

/* types des elements de variables de traduction */
typedef enum
{
	VtText,
	VtCounter,
	VtBuffer,
	VtAttrVal,
	VtFileName,
	VtExtension,
	VtDocumentName,
	VtDocumentDir,
	VtFileDir	
} TranslVarType;

/* un element d'une variable de traduction */
typedef struct _TranslVarItem
{
	TranslVarType     TvType;		/* type de l'element de variable */
	int             TvItem;	/* numero de la constante, du compteur,
					   de l'attribut ou du buffer qui
					   constitue l'element de variable */
	int             TvLength;	/* longueur du nombre a generer, dans
					   le cas d'un compteur seulement */
	CounterStyle	TvCounterStyle;	/* style du nombre a generer, dans
					   le cas d'un compteur seulement */
} TranslVarItem;

/* une variable de traduction est la concatenation des resultats de
   plusieurs elements */
typedef struct _TranslVariable
{
	int 		TrvNItems;	/* nombre effectif d'elements*/
	TranslVarItem     TrvItem[MAX_TRANSL_VAR_ITEM];   /* liste des elements */
} TranslVariable;

/* un buffer de lecture */
typedef char    TranslBuffer[MAX_TRANSL_BUFFER_LEN];

/* un cas d'application de regles de traduction pour un attribut a valeur
   numerique */
typedef struct _TranslNumAttrCase
{
	int             TaLowerBound;	/* valeur minimum de l'attribut pour
					   que les regles de traduction soient
					   appliquees */
	int             TaUpperBound;	/* valeur maximum de l'attribut pour
					   que les regles de traduction soient
					   appliquees */
	PtrTRuleBlock   TaTRuleBlock;	/* bloc de regles de regles a appliquer
					   quand la valeur de l'attribut est
					   dans l'intervalle */
} TranslNumAttrCase;

/* traduction d'un attribut logique */
typedef struct _AttributeTransl
{
  int	AtrElemType;	/* Type d'element auquel s'appliquent les
				 regles de traduction, 0 si les regles
				 s'appliquent quel que soit le type d'element*/
  union
    {
      struct	/* AttribType = AtNumAttr */
	{
	  /* nombre de cas d'application de regles de traduction*/ 
	  int _RTANbCas_;
	  /* les cas d'application de regles de traduction */
	  TranslNumAttrCase     _RTACas_[MAX_TRANSL_ATTR_CASE];
	} s0;
      struct	/* AttribType = AtReferenceAttr */
	{
	  PtrTRuleBlock   _RTARefPremRegle_; /* bloc de regles a appliquer
						pour l'attribut */
	} s1;
      struct	/* AttribType = AtTextAttr */
	{
	  Name             _RTATxtVal_;	/*la valeur qui declanche l'application
					   des regles de traduction */
	  PtrTRuleBlock   _RTATxt_;	/* bloc de regles a appliquer pour
					   cette valeur */
	} s2;
      struct	/* AttribType = AtEnumAttr */
	{
	  /* Pour chaque valeur de l'attribut, dans l'ordre de
	     la table AttrEnumValue, adresse du premier bloc de regles
	     de traduction associe a cette valeur. */
	  PtrTRuleBlock   _RTAValEnum_[MAX_ATTR_VAL + 1];
	} s3;
    } u;
} AttributeTransl;

#define AtrNCases u.s0._RTANbCas_
#define AtrCase u.s0._RTACas_
#define AtrRefTRuleBlock u.s1._RTARefPremRegle_
#define AtrTextValue u.s2._RTATxtVal_
#define AtrTxtTRuleBlock u.s2._RTATxt_
#define AtrEnuTRuleBlock u.s3._RTAValEnum_

/* traduction d'une presentation specifique */
typedef struct _PRuleTransl
{
  boolean	RtExist;	/* Il y a des regles de traduction pour cette
				   presentation */
  union
    {
      struct	/* presentation a valeur numerique */
	{
	  int		_RTPNbCas_; 	/* nombre de cas d'application de
					   regles de traduction */ 
	  TranslNumAttrCase	_RTPCas_[MAX_TRANSL_PRES_CASE]; /* les cas d'application de
					   regles de traduction */
	} s0;
      struct	/* autres presentation */
	{
	  char		_RTPVal_[MAX_TRANSL_PRES_VAL + 1]; /* les valeurs possibles de
						     la presentation */
	  PtrTRuleBlock	_RTPValRegle_[MAX_TRANSL_PRES_VAL + 1]; /* Pour chaque valeur
					de la presentation, dans l'ordre de la
					table RtPRuleValue, adresse du premier bloc
					de regles de traduction associe a
					cette valeur. */
	} s1;
    } u;
} PRuleTransl;

#define RtNCase u.s0._RTPNbCas_
#define RtCase u.s0._RTPCas_
#define RtPRuleValue u.s1._RTPVal_
#define RtPRuleValueBlock u.s1._RTPValRegle_

/* chaines pour une regle de traduction de caracteres */
typedef char    SourceString[MAX_SRCE_LEN + 1];	/* chaine source */
typedef char    TargetString[MAX_TARGET_LEN + 1];	/* chaine cible */

/* une entree de la table de traduction */
typedef struct _StringTransl
{
	SourceString        StSource;	/* chaine source */
	TargetString        StTarget;	/* chaine cible */
} StringTransl;

/* indices des regles de traduction de texte pour un alphabet */
typedef struct _AlphabetTransl
{
	char	AlAlphabet;
	int 	AlBegin; /* indice de la 1ere regle de traduction de
				   caracteres dans la table TsCharTransl */
	int	AlEnd;	/* indice de la derniere regle de traduction
				   de caracteres dans la meme table */
} AlphabetTransl;

/* pointeur sur un schema de traduction */
typedef struct _TranslSchema *PtrTSchema;

/* schema de traduction associe a une classe de document ou d'objet */
typedef struct _TranslSchema
{
	PtrTSchema    TsNext;	/* pour le chainage des blocs libres*/
	Name             TsStructName;	/* nom du schema de structure */
	int             TsStructCode;	/* code identifiant la version de ce
					   schema de structure */
	int             TsLineLength;/* longueur max des lignes traduites*/
	Name		TsEOL; /* caractere delimitant la fin de
					    la ligne */
	Name             TsTranslEOL; /* chaine de caracteres a inserer
					    a la fin des lignes traduites */
	int 		TsNConstants;	/* nombre de constantes */
	int 		TsNCounters;	/* nombre de compteurs */
	int 		TsNVariables;	/* nombre de variables de traduction */
	int 		TsNBuffers;	/* nombre de buffers */
	int             TsConstBegin[MAX_TRANSL_CONST]; /* indice du premier caract.
					   de chaque constante dans le buffer
					   TsConstant */
	TCounter      TsCounter[MAX_TRANSL_COUNTER];/* les compteurs */
	TranslVariable         TsVariable[MAX_TRANSL_VARIABLE];	  /* les variables */
	int 		TsPictureBuffer;		  /*  numero du buffer image */
	TranslBuffer          TsBuffer[MAX_TRANSL_BUFFER];  /* les buffers */
	PtrTRuleBlock   TsElemTRule[MAX_RULES_SSCHEMA]; /* pointeurs sur le debut de
					   la chaine des regles de traduction
					   associees a chaque type d'element,
					   dans le meme ordre que dans la table
					   StructSchema.SsRule */
	boolean		TsInheritAttr[MAX_RULES_SSCHEMA]; /* pour chaque type
					   d'element, dans le meme ordre que
					   dans la table StructSchema.SsRule,
					   indique si l'elem. herite d'un
					   attribut d'un elem. ascendant */
	AttributeTransl   TsAttrTRule[MAX_ATTR_SSCHEMA]; /* regles de traduction
					   des attributs logiques, dans l'ordre
					   de la table StructSchema.SsAttribute*/
	PRuleTransl	TsPresTRule[MAX_TRANSL_PRULE];	/* regles de traduction de la
					   presentation specifique, dans
					   l'ordre des types de regles de
					   presentation */
	int 		TsNTranslAlphabets;/* nombre d'elements dans le tableau
					   TsTranslAlphabet */
	AlphabetTransl         TsTranslAlphabet[MAX_TRANSL_ALPHABET]; /* traduction des chaines
					   de caracteres pour les differents
					   alphabets */
	int 		TsSymbolFirst;/* indice de la 1ere regle de traduc.de
					   symboles dans la table TsCharTransl */
	int 		TsSymbolLast;  /* indice de la derniere regle de
					   traduction de symboles dans la meme
					   table */
	int 		TsGraphicsFirst; /* indice de la 1ere regle de traduc de
					   graphiques dans la table TsCharTransl*/
	int 		TsGraphicsLast;   /* indice de la derniere regle de
					   traduction de graphiques dans la
					   meme table */
	int 		TsNCharTransls;	/* nombre total de regles de traduction
					   de caracteres */
	StringTransl         TsCharTransl[MAX_TRANSL_CHAR]; /* la table de traduction des
					   caracteres */
	char            TsConstant[MAX_TRANSL_CONST_LEN]; /* chaine de toutes les
					   constantes de traduction, separees
					   les unes des autres par un NULL */
} TranslSchema;
