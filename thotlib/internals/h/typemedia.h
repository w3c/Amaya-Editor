
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*
 * Declarations des types du Mediateur, pour la gestion du dialogue.  
 * typemedia.h : Declarations des types du Mediateur	
 * I. Vatton - Juillet 87
 *
 */

#include "typeint.h"
#include "typecorr.h"

/* typedef int    *ptrfont; moved to thot_gui.h - EGP */
typedef unsigned char RuleSet[32];

/* CsList des pointeurs utilises */
typedef struct _Box *PtrBox;
typedef Buffer *PtrBuffer;
typedef struct _Line *PtrLine;

/* Type d'une boite decrite */
typedef enum
{
  BoComplete, 
  BoSplit, 
  BoPiece, 
  BoBlock, 
  BoPicture, 
  BoGhost,
  BoDotted
} BoxType;

/* Type d'operation de relation entre boites : */
/* Lie(H,V) -> la relation relie deux boites soeurs. */
/* Inclus(H,V) -> la boite est liee a sa boite mere. */
/* Axe(H,V) -> la boite est liee a un axe de reference. */
/* Dim(H,V) -> la boite modifie la dimension elastique de l'autre boite. */
typedef enum
{
  OpHorizDep, 
  OpVertDep,
  OpHorizInc,
  OpVertInc,
  OpHorizRef,
  OpVertRef,
  OpWidth,
  OpHeight
} OpRelation;

/* Domaine de propagation des modifications entre boites */
typedef enum
{
  ToSiblings,
  ToChildren,
  ToAll
} Propagation;

typedef struct _ViewSelection
{
  PtrLine        VsLine;	/* Line contenant la selection	    */
  PtrBox        VsBox;	/* Numero boite marque de selection */
  PtrTextBuffer  VsBuffer;	/* Adresse buffer marque selection  */
  int             VsIndBox;	/* Index caractere marque selection */
  int             VsNSpaces;	/* Nombre de blancs qui precedent   */
  int             VsXPos;		/* Decalage boite de la selection   */
  int 		  VsIndBuf;	/* Index du caractere dans le buffer*/
} ViewSelection;

/* Description d'une ligne affichee */
typedef struct _Line
{
  PtrLine        LiPrevious;	/* Chainage des lignes              */
  PtrLine        LiNext;
  int             LiXOrg;	/* Origine X relative au bloc       */
  int             LiYOrg;	/* Origine Y relative au bloc       */
  int	          LiXMax;	/* X maximum en pixels              */
  int	          LiRealLength;	/* Largeur courante                 */
  int	          LiMinLength;	/* Largeur courante compressee      */
  int	          LiHeight;	/* Hauteur courante                 */
  int	          LiHorizRef;	/* Base courante                    */
  int	          LiSpaceWidth;	/* Taille pixel des blancs ajustes  */
  int	          LiNSpaces;	/* Nombre de blancs dans la ligne   */
  int	          LiNPixels;	/* Pixels restant apres ajsutement  */
  PtrBox        LiFirstBox;	/* Indice premiere boite contenue   */
  PtrBox        LiFirstPiece;	/* Indice si boite coupee           */
  PtrBox        LiLastBox;	/* Indice derniere boite contenue   */
  PtrBox        LiLastPiece;	/* Indice si boite coupee           */
} Line;

typedef struct _BoxRelation
{
  PtrBox        ReBox;	/* Indice de la boite dependante    */
  BoxEdge     ReRefEdge;	/* Repere de la boite               */
  OpRelation      ReOp;		/* Relation appliquee               */
} BoxRelation;

typedef struct _PosRelations *PtrPosRelations;
typedef struct _PosRelations
{
  PtrPosRelations  PosRNext;	/* Bloc suivant                     */
  BoxRelation        PosRTable[MAX_RELAT_POS];
} PosRelations;

typedef struct _DimRelations *PtrDimRelations;
typedef struct _DimRelations
{
  PtrDimRelations  DimRNext;	/* Bloc suivant                     */
  PtrBox        DimRTable[MAX_RELAT_DIM];
  boolean         DimRSame[MAX_RELAT_DIM];/* Description d'une boite affichee */
} DimRelations;

typedef struct _Box
{
  PtrAbstractBox         BxAbstractBox;	/* Pointeur sur le pave associe     */
  int             BxIndChar;	/* Position boite dans le volume doc*/
  int             BxXOrg;	/* Origine en X relative a la RlRoot*/
  int             BxYOrg;	/* Origine en Y relative a la RlRoot*/
  PtrPosRelations  BxPosRelations;	/* Dependances sur les positions    */
  PtrDimRelations  BxWidthRelations;	/* Dependances en hauteur           */
  PtrDimRelations  BxHeightRelations;	/* Dependances en largeur           */
  int             BxHeight;	/* Hauteur courante                 */
  int             BxWidth;	/* Largeur avec les blancs          */
  int             BxHorizRef;	/* Base courante                    */
  int             BxVertRef;	/* Axe Vertical courant             */
  ptrfont         BxFont;	/* Fonte liee a la boite            */
  PtrBox        BxPrevious;	/* Box affichable precedente      */
  PtrBox        BxNext;	/* Box affichable suivante        */
  PtrBox        BxMoved;	/* Chainage des boites deplacees    */
  PtrBox        BxHorizInc;	/* Box qui la relie a l'englobante*/
  PtrBox        BxVertInc;	/* Box qui la relie a l'englobante*/
  int	          BxNChars;	/* Nb total de caracteres dans boite*/
  BoxEdge     BxHorizEdge;	/* Point de position fixe en x      */
  BoxEdge     BxVertEdge;	/* Point de position fixe en y      */
  boolean         BxXOutOfStruct;	/* Position horizontale hors struct.*/
  boolean         BxYOutOfStruct;	/* Position verticale hors struct.  */
  boolean         BxWOutOfStruct;	/* Dimension horiz. hors struct.    */
  boolean         BxHOutOfStruct;	/* Dimension verticale hors struct. */
  boolean         BxHorizInverted;	/* Reperes horizontaux  inverses    */
  boolean         BxVertInverted;	/* Reperes verticaux inverses       */
  boolean         BxHorizFlex; /* La boite est elastique           */
  boolean         BxVertFlex; /* La boite est elastique           */
  boolean         BxXToCompute;   /* Box en cours de placement en X */
  boolean         BxYToCompute;   /* Box en cours de placement en Y */
  boolean	  BxNew;	/* Box nouvellement affichee      */
  boolean	  BxContentHeight;	/* Hauteur reelle = celle du contenu*/
  boolean	  BxContentWidth;	/* Largeur reelle = celle du contenu*/
  int		  BxRuleHeigth;	/* Hauteur du contenu ou minimum    */
  int		  BxRuleWidth;	/* Largeur du contenu ou minimum    */
  
  PtrTextBuffer  BxBuffer;	/* Ptr sur la liste des buffers     */
  int             BxEndOfBloc;	/* Longueur du remplissage :        */
  				/* >0 pour la derniere boite d'un   */
				/* bloc de lignes AlignLeftDots.        */
  int		  BxUnderline;	/* Type de soulignement             */
  int             BxThickness;	/* Epaisseur du trait               */
  BoxType       BxType;
  union
  {
    struct
    {
      PtrBox   _BtSuiv_;	/* Box coupee suivante    */
      int	 _BtNbBl_;	/* Nombre blancs dans texte */
      int	 _BtPixel_;	/* Nombre pixels a repartir */
      int	 _BtBlanc_;	/* >0 si la boite justifiee */
      int	 _BtICar_;	/* 1er car dans buf.	    */
    } s0;
    struct
    {
      int	 *_BtImageDescriptor_;/* Pointeur image en memoire*/
      /* liste des points de controles d'une courbe */
      /* + rapports de deformation des points de controle */
      float 	 _BtRapportX_;
      float 	 _BtRapportY_;
    } s1;
    struct
    {
      PtrLine 	 _BtLigne1_;	/* 1ere ligne s'il y en a   */
      PtrLine 	 _BtLigneN_;	/* Derniere ligne           */
    } s2;
  } u;
} Box;
#define BxNexChild u.s0._BtSuiv_
#define BxNSpaces u.s0._BtNbBl_
#define BxNPixels u.s0._BtPixel_
#define BxSpaceWidth u.s0._BtBlanc_
#define BxFirstChar u.s0._BtICar_
#define BxImageDescriptor u.s1._BtImageDescriptor_
#define BxXRatio u.s1._BtRapportX_
#define BxYRation u.s1._BtRapportY_
#define BxFirstLine u.s2._BtLigne1_
#define BxLastLine u.s2._BtLigneN_

typedef struct C_points_
{
  float lx, ly, rx, ry;
} C_points; /* stockage des points de controle des courbes */

/* Description du positonnement relatif de deux paves */
typedef struct _AbPosition
{
  PtrAbstractBox         PosAbRef;	/* Pointeur sur le pave de refence  */
  int         PosDistance;	/* Distance entre les deux          */
  BoxEdge     PosEdge;	/* Repere du pave positionne        */
  BoxEdge     PosRefEdge;	/* Repere du pave reference         */
  TypeUnit        PosUnit;	/* Unite utilisee pour la distance  */
  boolean	  PosUserSpecified;	/* Distance fournie par utilisateur */
} AbPosition;

typedef struct _AbDimension
{
  boolean	  DimIsPosition;
  union
  {
    struct
    {
      AbPosition     _PDimP_;
    } s0;
    struct
    {
      PtrAbstractBox 	 _PavDimRef_;	/* AbstractBox de reference        	*/
      int 	 _PavDim_;	/* Dimension imposee du pave	*/
				/* -1: dim definie par le contenu
				    0 : taille nulle 		*/
      TypeUnit	 _PavDimUnit_;	/* Dimension en %, points, etc.	*/
      boolean	 _PavMemeDim_;	/* Sur la meme dimension	*/
      boolean	 _PavDimUser_;	/* Dimension fournie par 	*/
				/* l'utilisateur 		*/
      boolean	 _PavDimMin_;	/* la dimension donne est la valeur 
				   minimum (possible uniquement 
				   si DimAbRef=null et DimValue=-1*/
    } s1;
  } u;
} AbDimension;

#define DimPosition u.s0._PDimP_
#define DimAbRef u.s1._PavDimRef_
#define DimValue u.s1._PavDim_
#define DimUnit u.s1._PavDimUnit_
#define DimSameDimension u.s1._PavMemeDim_
#define DimUserSpecified u.s1._PavDimUser_
#define DimMinimum u.s1._PavDimMin_

/* Element decrivant une regle presentation  en attente d'application */
typedef struct _DelayedPRule *PtrDelayedPRule;
typedef struct _DelayedPRule
{
  PtrPRule    DpPRule;	/* pointeur sur la regle en attente */
  PtrAbstractBox         DpAbsBox;	/* pointeur sur le pave auquel il   */
				/* faut appliquer la regle          */
  PtrPSchema      DpPSchema;	/* pointeur sur schema presentation */
				/* auquel appartient la regle       */
  PtrAttribute     DpAttribute;	/* pointeur sur attribut auquel     */
				/* correspond la regle, si une      */
				/* regle d'attribut, sinon nil      */
  PtrDelayedPRule DpNext;	/* element suivant */
} DelayedPRule;

/* Description d'un pave d'une Vue de document */
typedef struct _AbstractBox
{
  PtrElement      AbElement;	/* Element correspondant dans la R.I*/
  PtrAbstractBox         AbEnclosing;	/* Chainage vers le pave englobant  */
  PtrAbstractBox         AbNext;	/* Chainage vers pave frere suivant */
  PtrAbstractBox         AbPrevious;	/* Chainage vers le pave precedent  */
  PtrAbstractBox         AbFirstEnclosed;/* Chainage vers le 1er pave fils  */
  PtrAbstractBox         AbNextRepeated;/* Chainage vers le pave duplique'  */
				/* suivant (mise en page et col)    */
  PtrAbstractBox	  AbPreviousRepeated;/* Chainage vers le pave duplique */
				/* precedent (mise en page et col)  */
  PtrPSchema      AbPSchema;	/* Schema de presentation du pave   */
  PtrCopyDescr	AbCopyDescr;	/* Descripteur de l'element copie' si
				   pave produit par une regle Copy  */
  PtrAttribute     AbCreatorAttr;/* pointeur sur l'attribut qui a  */
				/* cree' le pave, si c'est un pave' */
				/* de pres. cree par un attribut.   */
  AbPosition     AbVertRef;	/* Position axe reference vertical  */
  AbPosition     AbHorizRef;	/* Position axe reference horizont  */
  AbPosition     AbVertPos;	/* Position verticale de la boite   */
  AbPosition     AbHorizPos;	/* Position horizontale de la boite */
  AbDimension    AbHeight;	/* Dimension verticale de la boite  */
  AbDimension    AbWidth;	/* Dimension horizontale            */
  int             AbVolume;	/* Nbre equivalant caractere du pave*/
  int		  AbUnderline;	/* Type de souligne		    */
  int		  AbThickness;	/* Epaisseur du souligne	    */
  PtrBox        AbBox;	/* Indice du descripteur de boite   */
  int         AbIndent;	/* Indentation pour la mise en ligne*/
  int         AbDepth;	/* plan d'affichage du pave         */
  int	          AbTypeNum;	/* Numero de type/boite presentation*/
  int	          AbNum;	/* Numero du pave pour debug        */
  int	          AbVisibility;/* Degre de visibilite du pave      */
  char            AbFont;	/* Caracteristique police utilisee  */
  int		  AbHighlight;	/* Mise en evidence du pave	    */
  int		  AbSize;	/* Taille logique/reelle caractere  */
  int		  AbDocView;	/* Numero de la vue du document     */
  int		  AbFillPattern;	/* motif de remplissage		    */
  int		  AbBackground;/* couleur du fond		    */
  int		  AbForeground;/* couleur des trace's	    */
  char		  AbLineStyle;/* Style du trace' des traits       */
  int         AbLineWeight;/* Epaisseur des traits graphiques  */
  TypeUnit	  AbLineWeightUnit;/* Unite l'epaisseur des traits */
  TypeUnit        AbSizeUnit;/* Unite d'expression de la taille  */
  boolean         AbHorizEnclosing;	/* Vrai->pave englobe en horizontal */
  boolean         AbVertEnclosing;	/* Vrai->pave englobe en vertical   */
  boolean         AbCanBeModified;	/* Modification du pave autorisee   */
  boolean         AbSelected;	/* Le pave est selectionne          */
  boolean         AbPresentationBox;	/* Un pave de presentation          */
  boolean	  AbRepeatedPresBox;	/* pave de presentation repete'	    */
  boolean         AbOnPageBreak;	/* Le pave coupe la limite de page  */
  boolean         AbAfterPageBreak;	/* AbstractBox au dela de la limite de page*/
  int         AbLineSpacing;	/* Espace entre deux lignes */
  BAlignment         AbAdjust;	/* BAlignment des lignes dans le pave  */
  boolean         AbJustify;	/* Les lignes sont justifiees       */
  TypeUnit        AbLineSpacingUnit;/* Unite des interligne             */
  TypeUnit        AbIndentUnit;/* Unite de l'indentation           */
  boolean         AbAcceptLineBreak;/* Peut etre coupe en lignes      */
  boolean	  AbAcceptPageBreak;/* Peut etre coupe par saut de page*/
  boolean         AbSensitive;	/* Vrai -> le pave est actif        */
  boolean	  AbReadOnly;	/* pave en lecture seule	    */
  boolean	  AbHyphenate;	/* le contenu peut etre hyphene'    */
  boolean	  AbNotInLine;	/* la pave ne participe pas a la    */
				/* mise en ligne		    */
  /* Les indications de modification du pave */
  boolean         AbNew;	/* AbstractBox nouvellement cree           */
  boolean         AbDead;      /* AbstractBox a  detruire                 */
  boolean         AbWidthChange;	/* Change la dimension horizontale  */
  boolean         AbHeightChange;	/* Change la dimension verticale    */
  boolean         AbHorizPosChange;	/* Change la position horizontale   */
  boolean         AbVertPosChange;	/* Change l'axe vertical du pave    */
  boolean         AbHorizRefChange;	/* Change l'axe horizontal du pave  */
  boolean         AbVertRefChange;	/* Change le positionnement vertical*/
  boolean         AbSizeChange;	/* Change la taille des caracteres  */
  boolean         AbAspectChange;/* Change l'aspect graphique:      */
				/* plan, couleur, pattern, trait.   */
  boolean         AbChange;	/* changement d'un autre type       */
  PtrDelayedPRule AbDelayedPRule;/* Utilise' par l'editeur         */
  LeafType   AbLeafType;
  union
  {
    struct 			/* AbLeafType = LtCompound */
    {
      boolean	 _PavMiseEnLigne_;
				/* Les deux champs suivants n'ont de
				 * sens que si AbInLine=False */
      boolean	 _PavCoupTete_; /* Manque debut du contenu pave     */
      boolean	 _PavCoupQueue_;/* Manque fin du contenu pave       */
    } s0;
    struct			/* AbLeafType = LtText 	    */
    {
      PtrTextBuffer _PavTexte_;/* Sur premier buffer texte	    */
      unsigned char  _PavCLangue_;   /* Langue utilisee             */
    } s1;
    struct  /* AbLeafType = LtGraphics ou LtSymbol               */
    {
      char       _PavTrace_;    /* Code du graphique                */
      char       _PavGAlphabet_;/* Alphabet utilise                 */
      char       _PavTraceReel_;/* Code trace effectif              */
    } s2;
    struct			/* AbLeafType = LtPicture 	    */
    {
      int *_PavImageDescriptor_;
    } s3;
    struct			/* AbLeafType = NatPolyline	    */
    {
      PtrTextBuffer _PavBufPolyLine_; /* premier buffert	    */
      char           _PavTypePolyLine_;/* Type du trace		    */
    } s4;
  } u;
} AbstractBox;

#define AbInLine u.s0._PavMiseEnLigne_
#define AbTruncatedHead u.s0._PavCoupTete_
#define AbTruncatedTail u.s0._PavCoupQueue_
#define AbText u.s1._PavTexte_
#define AbLanguage u.s1._PavCLangue_
#define AbShape u.s2._PavTrace_
#define AbGraphAlphabet u.s2._PavGAlphabet_
#define AbRealShape u.s2._PavTraceReel_
#define AbImageDescriptor u.s3._PavImageDescriptor_
#define AbPolyLineBuffer u.s4._PavBufPolyLine_
#define AbPolyLineShape u.s4._PavTypePolyLine_

typedef struct _ViewFrame
{
  PtrAbstractBox         FrAbstractBox;	/* Pteur sur pave racine de la Vue  */
  int             FrXOrg;	/* Origine X relative a la RlRoot   */
  int             FrYOrg;       /* Origine Y relative a la RlRoot   */
  int             FrVolume;	/* Volume caracteres de la fenetre  */
  int             FrClipXBegin;	/* X debut du rectangle d'affichage */
  int             FrClipXEnd;	/* X fin du rectangle d'affichage   */
  int             FrClipYBegin;	/* Y debut du rectangle d'affichage */
  int             FrClipYEnd;	/* Y fin du rectangle d'affichage   */
  boolean         FrReady;	/* La fenetre est affichable        */
  boolean         FrSelectOneBox;	/* Un seul pave est selectionne     */
  boolean         FrSelectShown;	/* La selection est allumee         */
  boolean	  FrSelectOnePosition;/* La selection est limitee	    */
  ViewSelection         FrSelectionBegin;	/* Marque de debut de selection     */
  ViewSelection         FrSelectionEnd;	/* Marque de fin de selection       */
  int             FrVisibility;	/* Seuil de visibilite de la fenetre*/
  int             FrMagnification;	/* Facteur de zoom de la fenetre    */
} ViewFrame;

typedef struct _VueDeDoc
{
  int             VdView;
  int             VdAssocNum;
  Name             VdViewName;
  PtrSSchema    VdSSchema;
  boolean         VdOpen;
  boolean         VdAssoc;
  boolean	  VdExist;
  boolean         VdNature;
  boolean         VdPaginated;
} DocumentView;

typedef DocumentView AvailableView[MAX_VIEW_OPEN];

/* Declaration d'un contexte de domaine de recherche */
typedef struct _SearchContext
{
  PtrDocument	SDocument;	/* Document ou on recherche        */
  int		STree;		/* Numero de l'arbre courant pour  */
				/* recherche dans tout le document */
  PtrElement	SStartElement;	/* Element de debut de la recherche*/
  int		SStartChar;	/* Caractere debut de la recherche */
  PtrElement	SEndElement;	/* Element de fin de la recherche  */
  int           SEndChar;	/* Caractere de fin de la recherche*/
  boolean	SStartToEnd;	/* Recherche en avant              */
  boolean	SWholeDocument;	/* Recherche dans tout le document */
} SearchContext;
typedef SearchContext *PtrSearchContext;

/* Declarations pour la gestion du dialogue */

/* CsList des pointeurs utilises */
typedef char    FontName[30];
typedef char    ButtonLabel[10];
typedef int     CatIndex[MAX_ENTRY];
typedef char    CatalogText[MAX_TXT_LEN];
typedef int	UnAQuatre;
