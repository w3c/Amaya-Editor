
/*
 * Declarations de types pour les schemas de structure de l'Editeur 
 * V. Quint	Juin 1984
 */

/* DEFINITIONS:

     Quelle que soit la categorie d'un element structure (compose, associe,
     parametre), sa structure est definie par une regle de la table
     StructSchema.SsRule.
     Un numero de regle est le rang de la regle dans la table
        StructSchema.SsRule.
     Un numero d'attribut est le rang d'un attribut dans la table
        StructSchema.SsAttribute.
     Un numero de valeur d'attribut est le rang de la valeur dans la table
        AttrEnumValue de l'attribut. 

*/

#include "app.h"

typedef char    Name[MAX_NAME_LENGTH];	/* le nom se termine par un octet nul */

/* constructeurs des regles de structure */
typedef enum
{
  	CsIdentity,
	CsList,
	CsChoice,
	CsAggregate,
	CsUnorderedAggregate,
	CsConstant,
	CsReference,
	CsBasicElement,
	CsNatureSchema,
	CsPairedElement,
	CsExtensionRule
} RConstruct;
#define MAX_CONSTRUCT	11	/* nombre de valeurs de RConstruct */

/* types de base connus de l'Editeur (voir la constante MAX_BASIC_TYPE) */
typedef enum
{
	CharString,
	GraphicElem,
	Symbol,
	Picture,
	Refer,
	PageBreak,
	UnusedBasicType
} BasicType;
#define MAX_BASIC_TYPE 7		/* nombre de valeurs de BasicType */

typedef enum
{
  	AtNumAttr, 
	AtTextAttr,
	AtReferenceAttr,
	AtEnumAttr
} AttribType;

#define MAX_ATTR_TYPE	4	/* nombre de valeurs de AttribType */

/* Une definition d'attribut dans un schema de structure */
/* Chaque definition d'attribut comprend le nom de l'attribut, les noms de
   ses valeurs possibles. Dans la representation interne d'un document ne
   figurent que les attributs qui ont une valeur. */

typedef struct _TtAttribute
{
	Name             AttrName;	/* nom de l'attribut, eventuellement
					   traduit dans la langue utilisateur*/
	Name             AttrOrigName;	/* nom reel de l'attribut */
	boolean         AttrGlobal;	/* l'attribut peut s'appliquer a tous
					  les elements definis dans le schema*/
	int       AttrFirstExcept;	/* indice dans SsException du 1er
					   numero d'exception associe' a cet
					   attribut, 0 s'il n'y a pas
					   d'exception associee */
	int       AttrLastExcept;	/* indice dans SsException du dernier
					   numero d'exception associe' a cet
					   attribut */
	AttribType 	AttrType;	/* type de l'attribut */
	union
	{
	  struct			/* AttribType = AtReferenceAttr */
	  {
	    /* numero de la regle definissant le type d'elt reference' par
	       l'attribut */
	    int     _AttrTypeRef_;	
	    /* schema de struct ou est defini le type d'elt reference', 0 si
	       meme schema */	
	    Name             _AttrTypeRefNature_;
	  } s2;
	  struct			/* AttribType = AtEnumAttr */
	  {
	    /* nombre de valeurs possibles: (taille effective de la table
	       AttrEnumValue) */
	    int   _AttrNEnumValues_;
	    Name             _AttrEnumValue_[MAX_ATTR_VAL]; /* noms de ces valeurs */
	  } s3;
	} u;
} TtAttribute;

#define AttrTypeRef u.s2._AttrTypeRef_
#define AttrTypeRefNature u.s2._AttrTypeRefNature_
#define AttrNEnumValues u.s3._AttrNEnumValues_
#define AttrEnumValue u.s3._AttrEnumValue_


typedef struct _StructSchema *PtrSSchema;

/* Une regle definissant un type dans un schema de structure */
typedef struct _SRule
{
	Name             SrName;	/* symbole gauche de la regle = type
					   defini par la regle */
	int 		SrNDefAttrs; 	/* 0..MAX_DEFAULT_ATTR, nombre d'attributs
					   a valeur imposee */ 
	/* numeros des attributs a valeur imposee */
	int  SrDefAttr[MAX_DEFAULT_ATTR];  
	/* valeurs imposees de ces attributs, dans le meme ordre */
	int             SrDefAttrValue[MAX_DEFAULT_ATTR];  
	boolean		SrDefAttrModif[MAX_DEFAULT_ATTR];
	/* 0..MAX_LOCAL_ATTR, nombre d'attributs pouvant s'appliquer au type */ 
	int 		SrNLocalAttrs;
	/* numeros des attributs pouvant s'appliquer au type */
	int  SrLocalAttr[MAX_LOCAL_ATTR];  
	/* l'attribut local de meme rang est obligatoire */
	boolean		SrRequiredAttr[MAX_LOCAL_ATTR];
	boolean         SrAssocElem;	/* c'est un element associe */
	boolean         SrParamElem;	/* c'est un parametre */
	boolean         SrUnitElem;	/* c'est une unite exportee */
	boolean         SrRecursive;	/* regle recursive */
	boolean         SrRecursDone;	/* regle recursive deja appliquee */
	boolean         SrExportedElem;	/* ce type d'element est exporte' */
	int     SrExportContent;	/* type d'element constituant le
					   contenu si l'element est exporte'*/
	Name             SrNatExpContent;	/* schema de structure ou est defini
					   SrExportContent, octet 0 si meme schema*/
	int       SrFirstExcept;	/* indice dans SsException du premier
					   numero d'exception associe' a ce
					   type d'element, 0 s'il n'y a pas
					   d'exception associee */
	int       SrLastExcept;	/* indice dans SsException du dernier
					   numero d'exception associe' a ce
					   type d'element */
	int		SrNInclusions;	/* nombre d'inclusions au sens SGML */
	int	SrInclusion[MAX_INCL_EXCL_SRULE]; /* les types d'elements
					   inclus au sens SGML */
	int		SrNExclusions;	/* nombre d'exclusions au sens SGML */
	int	SrExclusion[MAX_INCL_EXCL_SRULE]; /* les types d'elements
					   exclus au sens SGML */
	boolean         SrRefImportedDoc;	/* c'est un lien d'inclusion d'un
					   document externe */
	RConstruct    SrConstruct;
	union
	{
	  struct			/* SrConstruct = CsNatureSchema */
	  {
	    	PtrSSchema    _SrSSchemaNat_;	/* schema de str de la nature*/
		Name             _SrOrigNat_;	/* nom (traduit) de la nature*/
	  } s0;
	  struct			/* SrConstruct = CsBasicElement */
	  {
		BasicType      _SrBasicType_;
	  } s1;
	  struct			/* SrConstruct = CsReference */
	  {
	    	/*numero de la regle definissant le type d'element reference'*/
	    	int     _SrReferredType_;
		/*schema de structure ou est defini le type d'element
		  reference', octet nul si meme schema */
		Name             _SrRefTypeNat_;	
	  } s2;
	  struct			/* SrConstruct = CsIdentity */
	  {
		int     _SrIdentRule_; /* numero de la regle definissant
						 le symbole droit */
	  } s3;
	  struct			/* SrConstruct = CsList */
	  {
		int     _SrListItem_;/*numero de la regle definissant 
						 les elements de la liste */
		int             _SrMinItems_;   /* nombre minimum d'elts */
		int             _SrMaxItems_;   /* nombre maximum d'elts */
	  } s4;
	  struct			/* SrConstruct = CsChoice */
	  {
	    	int 		_SrNChoices_; /* -1..MAX_OPTION_CASE, nombre
						d'elements du choix, ou  0 si
						unite quelconque (UNIT), ou -1
						si nature quelconque (NATURE)*/
		int	_SrChoice_[MAX_OPTION_CASE];	/* numeros des
				regles definissant chaque element du choix */
	  } s5;
	  struct			/* SrConstruct = CsAggregate | CsUnorderedAggregate */
	  {
	    	int		_SrNComponents_; /* 0..MAX_COMP_AGG, nombre de 
						   composants de l'agregat */
		int	_SrComponent_[MAX_COMP_AGG]; /* numeros des regles
						definissant chaque composant */
		boolean         _SrOptComponent_[MAX_COMP_AGG]; /* table des
						composants optionnels */
	  } s6;
	  struct			/* SrConstruct = CsConstant */
	  {
	    	int 		_SrIndexConst_;	/* 1..MAX_LEN_ALL_CONST, Indice dans 
						SsConstBuffer du debut de la
						chaine constante */
	  } s7;
	  struct			/* SrConstruct = CsPairedElement */
	  {
		boolean		_SrFirstOfPair_;	/* marque de debut ou de fin */
	  } s8;
	} u;
} SRule;

#define SrSSchemaNat u.s0._SrSSchemaNat_
#define SrOrigNat u.s0._SrOrigNat_
#define SrBasicType u.s1._SrBasicType_
#define SrReferredType u.s2._SrReferredType_
#define SrRefTypeNat u.s2._SrRefTypeNat_
#define SrIdentRule u.s3._SrIdentRule_
#define SrListItem u.s4._SrListItem_
#define SrMinItems u.s4._SrMinItems_
#define SrMaxItems u.s4._SrMaxItems_
#define SrNChoices u.s5._SrNChoices_
#define SrChoice u.s5._SrChoice_
#define SrNComponents u.s6._SrNComponents_
#define SrComponent u.s6._SrComponent_
#define SrOptComponent u.s6._SrOptComponent_
#define SrIndexConst u.s7._SrIndexConst_
#define SrFirstOfPair u.s8._SrFirstOfPair_

typedef struct _PresentSchema *PtrPSchema;
typedef struct _HandlePSchema *PtrHandlePSchema;

/* un element de chainage des schemas de presentation */
typedef struct _HandlePSchema
{
    PtrPSchema		HdPSchema;   /* pointeur sur le schema de pres.     */
    PtrHandlePSchema	HdNextPSchema;	     /* handle du schema de pres. suivant   */
    PtrHandlePSchema	HdPrevPSchema;  /* handle du schema de pres. precedent */
}       HandlePSchema;

typedef struct _ExtensBlock *PtrExtensBlock;

/* bloc contenant l'ensemble des regles d'extension (section EXTENS) */
/* d'un schema d'extension */
typedef struct _ExtensBlock
{
    PtrExtensBlock	EbNextBlock;	
    SRule		EbExtensRule[MAX_EXTENS_SSCHEMA];
} ExtensBlock;

/*       
     Un schema de structure (de document ou de nature) en memoire.

     Tous les elements structures, quelle que soit leur categorie (composes,
     associes, parametres), sont definis dans la table de regles
     SsRule. Dans cette table, les regles sont regroupees par categories.
     La premiere regle definit l'element racine de la structure, puis suivent
     toutes les regles definissant les elements composes. Les regles qui
     definissent les elements associes sont consecutives, de meme que celles
     qui definissent les parametres. Pour chacun de ces groupes de regles on
     a le numero de la premiere et de la derniere regle du groupe.
     Les attributs du schema sont ranges dans un tableau : SsAttribute.
 */

typedef struct _StructSchema
{
	PtrSSchema    SsNextExtens;	/* Pointeur sur l'extension de schema*/
					/* suivante */
	PtrSSchema    SsPrevExtens;	/* Pointeur sur l'extension de schema*/
					/* precedente ou nil si pas extension*/
	Name             SsName;	/* nom de la structure generique */
	int             SsCode;	/* code identifiant la version */
	Name             SsDefaultPSchema;	/* nom du schema de presentation par */
					/* defaut associe a cette structure */
	PtrPSchema      SsPSchema;	/* pointeur sur le schema de */
					/* presentation effectivement associe*/
	PtrHandlePSchema SsFirstPSchemaExtens;	/* premier schema de presentation */
					/* additionnel */
	PtrEventsSet    SsActionList;	/* Pointer to the list of actions */
					/* that can be applied in documents */
					/* with this schema. */
	boolean		SsExtension;	/* Faux si schema de document ou de */
					/* nature,Vrai si extension de schema*/
	int		SsNExtensRules; /* nombre de regles d'extension si*/
					/* il c'est une extension de schema */
	PtrExtensBlock	SsExtensBlock;/* bloc des regles d'extension, s'il */
					/* s'agit d'une extension de schema */
	int     SsRootElem;	/* numero de la regle racine */
	int             SsNObjects;	/* nombre d'elements existant du type*/
					/* de la regle racine */
	int  SsNAttributes;	/* nombre d'attributs du schema */
	int     SsNRules;	/* nombre courant de regles definissant */
					/* les elements structures, y compris*/
					/* les regles de nature ajoutees */
					/* dynamiquement */
	boolean         SsExport;	/* ce schema exporte des elements */
	int             SsNExceptions;	/* Nombre d'entrees dans SsException*/
	/* tous les numeros d'exception, associes aux types d'elts et aux
	   attributs */
	int             SsException[MAX_EXCEPT_SSCHEMA];
	/* buffer pour le texte des constantes */
	char            SsConstBuffer[MAX_LEN_ALL_CONST];
	/* numero de la regle definissant la premiere nature chargee
	dynamiquement */
	int     SsFirstDynNature;
	/* les attributs definis pour ce schema */
	TtAttribute        SsAttribute[MAX_ATTR_SSCHEMA];
	/* regles definissant les elements structures */
	SRule           SsRule[MAX_RULES_SSCHEMA + 2]; /* on fait + 2 pour se
			      garantir deux regles libres en fin de tableaux */
} StructSchema;
