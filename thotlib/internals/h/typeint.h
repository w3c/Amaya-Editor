/*  Structure interne des documents, arbre abstrait, attribut, reference.
    Declarations de types pour la representation 
    interne des documents.
*/
#ifndef __TYPE_INT_H__
#define __TYPE_INT_H__

#include "typestr.h"
#include "typeprs.h"
#include "language.h"

/* identificateur interne d'un document : identificateur unique pour le */
/* systeme de stockage des documents */
typedef char	DocumentIdentifier[MAX_DOC_IDENT_LEN];

/* mode d'acces aux elements */
typedef enum {AccessReadOnly, AccessReadWrite, AccessHidden,
	      AccessInherited} ElemAccessRight;

/* identificateur externe d'un document: nom du document pour l'utilisateur */
typedef char	ADocumentName[MAX_DOC_NAME_LEN];

typedef struct _ElementDescr *PtrElement;
typedef struct _ReferredElemDescriptor *PtrReferredDescr;
typedef struct _ReferenceDescriptor *PtrReference;
typedef struct _AttributeBlock *PtrAttribute;
typedef struct _ExternalDoc *PtrExternalDoc;
typedef struct _AbstractBox *PtrAbstractBox;	/*pointeur sur un pave d'une image abstraite*/
typedef struct _CopyDescriptor *PtrCopyDescr;

/* les types de references */
typedef enum { 
	RefFollow, 
	RefInclusion
} ReferenceType;

/* descripteur d'un document externe contenant une ou plusieurs references */
/* a un element */
typedef struct _ExternalDoc
{
	PtrExternalDoc   EdNext;
	DocumentIdentifier	EdDocIdent;
} ExternalDoc;


/* Descripteur representant un element qui est copie' par une regle de */
/* presentation Copy appliquee a un element reference */
typedef struct _CopyDescriptor
{
	PtrAbstractBox		CdCopiedAb; /* le pave qui est une copie */
	PtrElement	CdCopiedElem; /* l'element qui est copie' */
	PtrPRule	CdCopyRule;/* la regle de copie */
	PtrCopyDescr	CdPrevious; /* descripteur precedent pour le meme
					element */
	PtrCopyDescr	CdNext;   /* descripteur suivant pour le meme
					element */
} CopyDescriptor;

typedef char	LabelString[MAX_LABEL_LEN];

/* Descripteur d'un element qui est reference' et de la chaine de toutes ses */
/* references. Toutes les references a un meme element forment une chaine */
/* dont le debut est pointe par ReFirstReference. Pour chaque document, tous les */
/* descripteurs ReferredElemDescriptor forment une chaine dont le chainage est assure */
/* par les deux pointeurs suivants. L'ancre de la chaine est DocReferredEl. */
typedef struct _ReferredElemDescriptor
{
  PtrReference    	ReFirstReference;	/* la premiere reference a cet
					   element */
  PtrExternalDoc   	ReExtDocRef;/* debut de la chaine de
					   descripteurs des documents qui
					   contiennent des references a
					   l'element. */
  PtrReferredDescr 	RePrevious;
  PtrReferredDescr 	ReNext;
  LabelString           ReReferredLabel;	/* label de l'elem reference'
					   utile seulement si ReExternalRef */
  boolean         	ReExternalRef; /* l'objet reference' est
					   externe au document qui le reference					   (reference ou inclusion). */
  union
  {
    struct /* ReExternalRef = False */
    {
      PtrElement	_ReReferredElem_;	/* l'elem qui est reference' */
    } s0;
    struct /* ReExternalRef = True */
    {
      DocumentIdentifier	_ReExtDocument_;
      /* identification du document qui contient l'elem reference'.
	 Cet element est identifie' a l'interieur de son document par
	 ReReferredLabel. */
    } s1;
  } u;
} ReferredElemDescriptor;

#define ReReferredElem u.s0._ReReferredElem_
#define ReExtDocument u.s1._ReExtDocument_

/* une reference, attachee a un element de type LtReference ou a un attribut de */
/* type reference. */
typedef struct _ReferenceDescriptor
{
    PtrReference	RdNext;	/* reference suivante au meme elt. */
    PtrReference    	RdPrevious;	/* reference precedente au meme elt. */
    PtrReferredDescr	RdReferred;	/* descripteur de l'element qui est
					reference' */
    PtrElement		RdElement;	/* l'element qui reference, meme si
					c'est une reference par un attribut */
    PtrAttribute		RdAttribute;	/* attribut correspondant ou NULL si ce
					n'est pas une reference par attribut */
    ReferenceType       	RdTypeRef;	/* type de reference */
    boolean         	RdInternalRef;	/* la reference et l'objet designe'
					sont dans le meme document si vrai,
					des documents differents si faux */
    PtrElement		RdPageBreak;	/* l'elt marque de page correspondant
					a la page ou l'element est inclus, NULL
					si l'elt reference n'est pas encore
					inclus (uniquement en cas d'inclusion 
					d'un element associe en haut ou bas de
					page) */
} ReferenceDescriptor;

typedef struct _TextBuffer *PtrTextBuffer;

/* un attribut d'un element d'arbre abstrait */
typedef struct _AttributeBlock
{
    PtrAttribute     AeNext;	/* attribut suivant pour le meme elt */
    PtrSSchema    AeAttrSSchema;	/* schema de structure definissant
					l'attribut */
    int  AeAttrNum;		/* numero de l'attribut de l'element */
    boolean         AeDefAttr;		/* attribut a valeur fixee ou non */
    AttribType    AeAttrType;		/* type de l'attribut */
    union
    {
      struct	/* AeAttrType = AtNumAttr ou AtEnumAttr */
      {
	int	       _AeAttrValue_;	/* valeur ou numero de la valeur de
					l'attribut */
      } s0;
      struct	/* AeAttrType = AtReferenceAttr */
      {
	PtrReference   _AeAttrReference_;	/* reference vers l'elt reference' par
					l'attribut */
      } s1;
      struct	/* AeAttrType = AtTextAttr */
      {
	PtrTextBuffer _AeAttrText_;	/* valeur (textuelle) de l'attribut */
      } s2;
    } u;
} AttributeBlock;

#define AeAttrValue u.s0._AeAttrValue_
#define AeAttrReference u.s1._AeAttrReference_
#define AeAttrText u.s2._AeAttrText_

/* origine d'une marque de page */
typedef enum
{
	PgComputed,
	PgBegin,
	PgUser,
	PgRepeat,
	ColBegin,
	ColComputed,
	ColUser,
	ColGroup
} PageType;

/*  nature d'un element feuille de la representation interne */
typedef enum
{
	LtText,
	LtSymbol,
	LtGraphics,
	LtPicture,
	LtCompound,
	LtPageColBreak,
	LtReference,
	LtPairedElem,
	LtPlyLine
} LeafType;

typedef char    Buffer[MAX_CHAR];

typedef struct _PolyLinePoint
{
	int		XCoord;	    /* Coordonnees relatives a l'origine   */
	int		YCoord;	    /* de la boite exprimees en millipoint */
}PolyLinePoint;

#define MAX_POINT_POLY	MAX_CHAR / sizeof(PolyLinePoint)

#ifndef AMAYA_H__	
typedef char    PathBuffer[MAX_PATH];
#endif 

/* Description d'un buffer de texte */
typedef struct _TextBuffer
{
	PtrTextBuffer  BuNext;		/* Buffer suivant */
	PtrTextBuffer  BuPrevious;		/* Buffer precedent */
	int 		BuLength;		/* Longueur effective du buffer */
					/* ou nombre de points de polyline */
    union
    {
	struct
	{
	    Buffer      _BuContent_;		/* Tableau de caracteres */
	} s0;
	struct
	{
	    PolyLinePoint	_BuPoints_[MAX_POINT_POLY]; /* Tableau de points */
	} s1;
    } u;
} TextBuffer;

#define BuContent u.s0._BuContent_
#define BuPoints u.s1._BuPoints_

/* Descripteur representant un element qui est conserve' a la suite */
/* d'une operation Couper ou Copier et qui est destine' a etre Colle' */

typedef struct _PasteElemDescr *PtrPasteElem;
#define MAX_PASTE_LEVEL 5
typedef struct _PasteElemDescr
{
	PtrPasteElem	PePrevious;	/* descripteur de l'element a coller
					precedent */
	PtrPasteElem	PeNext;	/* descripteur de l'element a coller
					suivant */
	PtrElement	PeElement;	/* l'element a coller */
	int	 	PeElemLevel;	/* niveau dans l'arbre abstrait de
					l'element original */
	int	PeAscendTypeNum[MAX_PASTE_LEVEL];	/* numero de type des
					anciens ascendants de l'elem a coller*/
	PtrSSchema	PeAscendSSchema[MAX_PASTE_LEVEL]; /* schema de
					structure des anciens ascendants */
	PtrElement	PeAscend[MAX_PASTE_LEVEL]; /* ident. des anciens
					elements ascendants */		
} PasteElemDescr;


/* Numero d'une vue d'un document */
typedef int	DocViewNumber;

/* Un element d'un arbre abstrait */
typedef struct _ElementDescr
{
    PtrElement		ElParent;		/* pointeurs dans l'arbre abstrait */
    PtrElement		ElPrevious;
    PtrElement		ElNext;
    PtrReferredDescr	ElReferredDescr;	/* Pointeur sur le descripteur
					   d'element reference', s'il y a des 
					   references a cet elt */
    PtrAttribute     	ElFirstAttr;	/* pointeur sur le premier attribut de
					 l'element, ou nil si pas d'attribut */
    PtrPRule    	ElFirstPRule;	/* Pointeur sur la premiere regle de
					   la chaine des regles de presentation
					   a appliquer a l'elt */
    PtrTextBuffer  	ElComment;	/* pointeur sur le buffer contenant le
				        debut du commentaire associe a
					l'element */
    PtrAbstractBox         	ElAbstractBox[MAX_VIEW_DOC]; /* pour chaque vue, pointeur sur le 
					1er pave correspondant a l'element */
    PtrSSchema    	ElStructSchema;	/* pointeur sur le schema de nature
					ou de document */
    int     	ElTypeNumber;		/* numero de la regle definissant le
					type de l'element dans le schema de
					structure */
    int		    	ElVolume;    	/* volume (nombre de caracteres) du
					sous-arbre de l'element */
    PtrCopyDescr    	ElCopyDescr; 	/* debut de la chaine des descripteurs
					des paves qui copient l'elt par une
					regle de presentation Copy */
    int			ElAssocNum; 	/* numero de type d'element associe
					auquel appartient l'element, 0 si
					arbre principal */
    boolean         	ElIsCopy;	/* l'elem est une copie non modifiable
					(parametre ou copie par inclusion). */
    PtrReference    	ElSource;	/* pointeur sur le bloc reference
					designant la source dont l'element est
					une copie par inclusion, NULLsi l'elem
				        n'est pas une copie par inclusion. */
    LabelString		ElLabel;	/* identificateur unique de l'element*/
    ElemAccessRight	ElAccess;	/* Droits d'acces a l'element */
    boolean		ElHolophrast;	/* l'element est holophraste'*/
    boolean		ElTransContent;	/* le contenu de l'element a deja ete
					traduit */
    boolean		ElTransAttr;	/* les attributs de l'element ont deja
					ete traduit */
    boolean		ElTransPres;	/* les regles de presentation de
					l'element ont deja ete traduites */
    boolean  		ElTerminal;	/* type de noeud */
    union
    {
	struct		/* ElTerminal = False */
	{
	    PtrElement _ElFirstChild_;	/* premier fils */
	} s0;
	struct		/* ElTerminal = True */
	{
	    LeafType _ElLeafType_;
	    union
	    {
		struct	/* ElLeafType = LtText */
		{
		    PtrTextBuffer  _ElText_;  /* pointeur sur le buffer
						  contenant le debut du texte*/
		    int             _ElTextLength_; /* longueur du texte */
		    Language        _ElLanguage_;   /* langue du texte */
		} s0;
		struct	/* ElLeafType = LtGraphics ou LtSymbol */
		{
		    char            _ElGraph_; /* code du contenu */
		} s1;
		struct	/* ElLeafType = LtPageColBreak */
		{
		    boolean         _ElAssocHeader_; /* il faut creer les
				           elements associes de haut de page */
		    boolean         _ElPageModified_; /* la page a ete modifiee */
		    PageType        _ElPageType_;  /* origine de la page */
		    int             _ElPageNumber_;   /* numero de la page */
		    int          _ElViewPSchema_;   /* numero de vue dans le
						   schema de presentation */
		} s2;
		struct	/* ElLeafType = LtReference */
		{
		    PtrReference    _ElReference_;	   /* pointeur sur l'element
						   reference */
		} s3;
		struct	/* ElLeafType = LtPairedElem */
		{
		    int		    _ElPairIdent_;  /* identificateur unique
						de la paire dans le document */
		    PtrElement	    _ElOtherPairedEl_; /* pointeur sur l'autre
						     element de la meme paire*/
		} s4;
		struct	/* ElLeafType = LtPlyLine */
		{
		    PtrTextBuffer  _ElPolyLineBuffer_; /* buffer contenant les
					     points qui definissent la ligne */
		    int		    _ElNPoints_; /* nombre de points*/
		    char	    _ElPolyLineType_; /* type de ligne */
		} s5;
		struct	/* TypeImage = LtPicture */
		{
		    PtrTextBuffer  _ElPictureName_;  /* pointeur sur le buffer
						 contenant le nom de l'image */
		    int             _ElNameLength_; /*longueur du nom de l'image */
		    int            *_ElImageDescriptor_;/* Descripteur image */
		} s6;
	    } u;
	} s1;
    } u;
} ElementDescr;

#define ElFirstChild u.s0._ElFirstChild_
#define ElLeafType u.s1._ElLeafType_
#define ElText u.s1.u.s0._ElText_
#define ElTextLength u.s1.u.s0._ElTextLength_
#define ElLanguage u.s1.u.s0._ElLanguage_
#define ElGraph u.s1.u.s1._ElGraph_
#define ElAssocHeader u.s1.u.s2._ElAssocHeader_
#define ElPageModified u.s1.u.s2._ElPageModified_
#define ElPageType u.s1.u.s2._ElPageType_
#define ElPageNumber u.s1.u.s2._ElPageNumber_
#define ElViewPSchema u.s1.u.s2._ElViewPSchema_
#define ElReference u.s1.u.s3._ElReference_
#define ElPairIdent u.s1.u.s4._ElPairIdent_
#define ElOtherPairedEl u.s1.u.s4._ElOtherPairedEl_
#define ElPolyLineBuffer u.s1.u.s5._ElPolyLineBuffer_
#define ElNPoints u.s1.u.s5._ElNPoints_
#define ElPolyLineType u.s1.u.s5._ElPolyLineType_
#define ElPictureName u.s1.u.s6._ElPictureName_
#define ElNameLength u.s1.u.s6._ElNameLength_
#define ElPictInfo u.s1.u.s6._ElImageDescriptor_

    /* Le pointeur ElFirstChild est interprete suivant le constructeur de la
     regle definissant l'element :
        - LISTE : premier element de la liste,
        - AGREGAT : premier composant de l'agregat,
        - IDENTITE ou SchNATURE : element identique,
        - CONST : element constant,
        - REFERENCE : l'element que l'on reference.
     Les pointeurs ElPrevious et ElNext sont utilises pour chainer un
     element structure aux elements precedent et suivant de la liste ou de
     l'agregat auquel il appartient, si le constructeur de son pere est
     CsList ou CsAggregate.
     Ces pointeurs sont egalement utilises pour chainer entre eux les elements
     associes de meme type. Ils sont inutilises si le constructeur du pere est
     CsIdentity ou CsConstant.
     Ils sont encore utilises pour chainer entre eux les differents element
     REFERENCE qui pointent sur le meme element. */

/* un descripteur de reference sortante creee ou detruite */

typedef struct _OutReference *PtrOutReference;

typedef struct _OutReference
{
  	PtrOutReference	OrNext;  /* descripteur suivant */
  	LabelString	OrLabel;    /* label de l'element externe reference' */
	DocumentIdentifier	OrDocIdent; /* ident du document contenant cet elem */
} OutReference;

/* le descripteur d'un element reference' depuis l'exterieur et qui a ete
detruit ou qui a change' de document */
typedef struct _ChangedReferredEl *PtrChangedReferredEl;

typedef struct _ChangedReferredEl
{
	PtrChangedReferredEl	CrNext;	/* descripteur suivant */
	LabelString	CrOldLabel;	/* ancien label de l'element */
	LabelString	CrNewLabel;	/* nouveau label, 0 si elem detruit */
 	DocumentIdentifier	CrOldDocument;	/* ident du document qui contenait 
					l'element */
	DocumentIdentifier	CrNewDocument;	/* ident du document qui contient */
					/* l'element, si element deplace' */
 	PtrExternalDoc	CrReferringDoc;	/* liste des documents qui referencent
					l'element */
} ChangedReferredEl;

/* descripteur d'un fichier .EXT charge' en memoire : liste des elements d'un
document qui sont reference's par d'autres documents */
typedef struct _EnteringReferences *PtrEnteringReferences;

typedef struct _EnteringReferences
{
	PtrEnteringReferences	ErNext;	/* descripteur suivant */
	PtrReferredDescr ErFirstReferredEl;	/* premier element reference' par */
					/* d'autres documents */
	DocumentIdentifier	ErDocIdent;	/* identificateur du document */
	PathBuffer	ErFileName;	/* nom complet du fichier .EXT */
} EnteringReferences;

/* descripteur d'un fichier .REF charge' en memoire : liste des elements
reference's qui appartenaient a un document qui ont ete detruits ou qui ont
change' de document */
typedef struct _ReferenceChange *PtrReferenceChange;

typedef struct _ReferenceChange
{
	PtrReferenceChange	RcNext;	/* descripteur suivant */
	PtrChangedReferredEl	RcFirstChange;	/* premier element reference' change'*/
	DocumentIdentifier		RcDocIdent;   /* identificateur du document */
	PathBuffer		RcFileName; /* nom complet du fichier .REF*/
} ReferenceChange;
	
/* descripteur d'une vue d'un document */
typedef struct _DocViewDescr
{
	PtrSSchema    DvSSchema;	/* schema de structure du schema de
					presentation auquel correspond la vue*/
	int          DvPSchemaView;	/* Numero de la vue dans le schema 
					de presentation */
	boolean         DvSync;	/* Cette vue doit etre synchronisee 
					avec la vue active */
} DocViewDescr;

typedef struct _DocumentDescr *PtrDocument;

/* un document sous sa representation interne */
typedef struct _DocumentDescr
{
	PtrDocument     DocNext;	/* pour le chainage des blocs libres */
	PtrTextBuffer  DocComment;	/* commentaire du document */
	PtrSSchema    DocSSchema;	/* schema de structure du document */
	PtrElement      DocRootElement; /* pointeur sur l'element racine du */
                                        /* document en representation interne*/
	PtrElement      DocAssocRoot[MAX_ASSOC_DOC];	/* pointeurs sur le 1er
					element associe de chaque type */
	PtrElement	DocAssocSubTree[MAX_ASSOC_DOC];  /* racine du sous-arbre
					a afficher dans la vue d'elts associes,
	                                ou NULL si on affiche tout l'arbre */
	int             DocAssocFrame[MAX_ASSOC_DOC];     /* numero de la fenetre
					d'affichage des elements associes de
					chaque type */
	int             DocAssocVolume[MAX_ASSOC_DOC];     /* volume total de la
					fenetre d'affichage des elements
					associes de chaque type */
	int             DocAssocFreeVolume[MAX_ASSOC_DOC];/* volume libre restant
					dans la fenetre d'affichage des
					elements associes de chaque type */
	int             DocAssocNPages[MAX_ASSOC_DOC];   /* nbres de pages */
	PtrAbstractBox         DocAssocModifiedAb[MAX_ASSOC_DOC];  /* pointeur sur le pave
					a reafficher pour les elts associes de
                                        chaque type */
	PtrElement      DocParameters[MAX_PARAM_DOC];      /* pointeurs sur le
					parametre de chaque type */
	PtrReferredDescr DocReferredEl;	/* pointeur sur le descripteur bidon,
					debut de la chaine des descripteurs
					d'elements qui sont reference's dans le
					document */
	DocViewDescr      DocView[MAX_VIEW_DOC]; /* correspondance entre les vues du
	                                document et celles definies dans les
	                                schemas de presentation utilise's dans
					le document */
	PtrAbstractBox         DocViewRootAb[MAX_VIEW_DOC];	/* pointeur sur le pave
					racine de chacune des vues du document,
	                                dans le meme ordre que dans DocView. */
	PtrElement	DocViewSubTree[MAX_VIEW_DOC];	/* racine du sous-arbre
					a afficher dans la vue, ou nil par
					defaut */
	int		DocViewFrame[MAX_VIEW_DOC];	/* numero de la fenetre
	                                correspondant a la vue */
	int		DocViewVolume[MAX_VIEW_DOC];   /* volume total de la vue */
	int             DocViewFreeVolume[MAX_VIEW_DOC]; /* volume libre de la vue */
	int             DocViewNPages[MAX_VIEW_DOC];  /* nbres de pages */
	PtrAbstractBox         DocViewModifiedAb[MAX_VIEW_DOC];   /* pointeur sur le pave a
						      reafficher pour la vue */
	ADocumentName	DocDName;		/* nom du document pour l'utilisateur*/
	DocumentIdentifier DocIdent;	/* identificateur unique du document
					pour le systeme de stockage */
	PathBuffer      DocDirectory;	/* directory du document */
	PathBuffer	DocSchemasPath;	/* path des schemas du document */
	int		DocBackUpInterval; /* intervalle entre sauvegardes
					automatiques */
	boolean         DocReadOnly;	/* document en lecture seule */
	boolean         DocExportStructure;	/* la structure logique du document ne
	                                contient que les elements visibles de
					l'exterieur (exportables) */
	int             DocLabelExpMax;	/* valeur maximum des labels 
					d'elements du document */
	int		DocMaxPairIdent;/* valeur la plus grande des ident.
					de paires  du document */
	boolean         DocModified;	/* le document a ete modifie' */
	int             DocNTypedChars;	/* nombre de caracteres saisis depuis
					la derniere sauvegarde */
	PtrOutReference	DocNewOutRef;	/* debut de la liste des references
					sortantes creees depuis la derniere
					sauvegarde du document */
	PtrOutReference	DocDeadOutRef;	/* debut de la liste des references
					sortantes detruites depuis la
					derniere sauvegarde du document */
	PtrChangedReferredEl	DocChangedReferredEl;	/* debut de la liste des elements
					reference's de l'exterieur qui ont ete
					detruits ou deplace's depuis la
					derniere sauvegarde */

#define MAX_LANGUAGES_DOC 10
	int		DocNLanguages;	/* Nombre de langues utilisees */
	Language	DocLanguages[MAX_LANGUAGES_DOC]; /* les langues utilisees
					dans le document */
	/* table des natures utilisees dans un document */
#define MAX_NATURES_DOC 20  /* longueur maximum de la table des natures */
	int		DocNNatures;	/* nombres de natures */
	Name		DocNatureName[MAX_NATURES_DOC]; /* nom des natures */
	Name		DocNaturePresName[MAX_NATURES_DOC]; /* nom des schemas
					de presentation de ces natures */
	PtrSSchema	DocNatureSSchema[MAX_NATURES_DOC];/* schemas de structure
					de ces natures */
	/* informations sur la lecture du pivot */
	int		DocPivotVersion;  /* numero de la version pivot */
	PtrReferredDescr DocLabels; /* labels externes */
	boolean		DocToBeChecked;
	boolean		DocPivotError;
	boolean		DocNotifyAll;	/* Thot doit indiquer a l'application
					la creation de TOUS les elements des
					sous-arbres cree's ou colle's */
} DocumentDescr;

/* Dans le champ DocParameters, les pointeurs sont dans le meme ordre que les
regles qui definissent la structure des elements pointes dans le tableau
StructSchema.SsRule. */

#endif /* __TYPE_INT_H__ */
