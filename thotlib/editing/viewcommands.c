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
 * This module handles document views
 *
 * Author: V. Quint (INRIA)
 *
 */

#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "constmenu.h"
#include "libmsg.h"
#include "message.h"
#include "fileaccess.h"
#include "appdialogue.h"
#include "application.h"
#include "dialog.h"

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "creation_tv.h"
#include "platform_tv.h"
#include "modif_tv.h"
#include "page_tv.h"
#include "select_tv.h"
#include "appdialogue_tv.h"
#include "print_tv.h"
#undef THOT_EXPORT
#define THOT_EXPORT
#include "edit_tv.h"

#include "absboxes_f.h"
#include "appdialogue_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "attributeapi_f.h"
#include "attributes_f.h"
#include "buildboxes_f.h"
#include "callback_f.h"
#include "changeabsbox_f.h"
#include "config_f.h"
#include "content_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "displayview_f.h"
#include "documentapi_f.h"
#include "docs_f.h"
#include "exceptions_f.h"
#include "fileaccess_f.h"
#include "frame_f.h"
#include "memory_f.h"
#include "savedoc_f.h"
#include "search_f.h"
#include "schemas_f.h"
#include "structcreation_f.h"
#include "structlist_f.h"
#include "structmodif_f.h"
#include "structselect_f.h"
#include "structschema_f.h"
#include "textcommands_f.h"
#include "thotmsg_f.h"
#include "tree_f.h"
#include "undo_f.h"
#include "views_f.h"
#include "viewapi_f.h"
#include "viewcommands_f.h"

static PtrDocument      pDocChangeSchPresent;
static PtrSSchema       TableNaturesSchPresent[NbMaxMenuPresNature];
static int              nbNatures;



/*----------------------------------------------------------------------
   CreateWindowWithTitle creates the frame for the current view view of
   the current document pDoc.
   Returns the frame number of ) if the creation failed and the available
   volume in *vol.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 CreateWindowWithTitle (PtrDocument pDoc, int view, Name viewName, int *vol, int X, int Y, int width, int height)
#else  /* __STDC__ */
int                 CreateWindowWithTitle (pDoc, view, viewName, vol, X, Y, width, height)
PtrDocument         pDoc;
int                 view;
Name                viewName;
int                *vol;
int                 X;
int                 Y;
int                 width;
int                 height;

#endif /* __STDC__ */
{
   int    createdFrame;
   CHAR_T buf[MAX_TXT_LEN];

   /* met dans le buffer le nom du document... */
   ustrncpy (buf, pDoc->DocDName, MAX_NAME_LENGTH);
   ustrcat (buf, TEXT("  "));
   /* ...suivi eventuellement de la mention 'Read only' */
   if (pDoc->DocReadOnly)
     {
	ustrcat (buf, TEXT(" "));
	ustrcat (buf, TtaGetMessage (LIB, TMSG_READ_ONLY));
     }
   /* creation d'une frame pour la vue */
   createdFrame = MakeFrame (pDoc->DocSSchema->SsName, view, buf, X, Y, width, height, vol, IdentDocument (pDoc));
   return createdFrame;
   return -1;
}


/*----------------------------------------------------------------------
   ListWithText     retourne TRUE si l'element pEl est une  	
   liste dont les elements peuvent contenir du texte.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     ListWithText (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static ThotBool     ListWithText (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   PtrElement          pChild, pNext, pDesc, pTextEl;
   ThotBool            ok;

   ok = FALSE;
   if (!pEl->ElTerminal)
      if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct == CsList)
	 /* c'est bien une liste */
	{
	   /* essaie de creer une descendance de cette liste qui mene a une */
	   /* feuille de texte */
	   pDesc = CreateDescendant (pEl->ElTypeNumber, pEl->ElStructSchema,
				     pDoc, &pTextEl, pEl->ElAssocNum,
				     CharString + 1, pEl->ElStructSchema);
	   if (pDesc != NULL)
	      /* on a pu creer la descendance */
	     {
		/* c'etait juste pour voir, on la libere */
		DeleteElement (&pDesc, pDoc);
		/* on detruit les fils de l'element liste, qui seront
		   remplace's par d'autres elements crees au cours de
		   l'importation du fichier */
		pChild = pEl->ElFirstChild;
		while (pChild != NULL)
		  {
		     pNext = pChild->ElNext;
		     DeleteElement (&pChild, pDoc);
		     pChild = pNext;
		  }
		/* enfin, on fait un retour positif */
		ok = TRUE;
	     }
	}
   return ok;
}


/*----------------------------------------------------------------------
   ListDescent cherche si l'element pEl ou l'un de ses         	
   descendants est une liste dont les elements peuvent     
   contenir du texte. Retourne l'element List trouve' ou  	
   NULL si echec.                                          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrElement   ListDescent (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static PtrElement   ListDescent (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pListEl, pChild;

   pListEl = NULL;
   if (ListWithText (pEl, pDoc))
      /* l'element pEl lui-meme est une liste dont les elements peuvent */
      /* contenir du texte */
      pListEl = pEl;
   else
      /* on cherche parmi les descendants de pEl */
   if (!pEl->ElTerminal)
     {
	/* on cherche d'abord si les fils de pEl sont des listes comme on veut */
	pChild = pEl->ElFirstChild;
	while (pChild != NULL && pListEl == NULL)
	   if (ListWithText (pChild, pDoc))
	      pListEl = pChild;
	   else
	      pChild = pChild->ElNext;
	if (pListEl == NULL)
	   /* on n'a pas trouve' parmi les fils, on cherche parmi les */
	   /* descendants des fils */
	  {
	     pChild = pEl->ElFirstChild;
	     while (pChild != NULL && pListEl == NULL)
	       {
		  pListEl = ListDescent (pChild, pDoc);
		  if (pListEl == NULL)
		     pChild = pChild->ElNext;
	       }
	  }
     }
   return pListEl;
}

/*----------------------------------------------------------------------
   	SearchElemWithExcept	Search an element with exception	
   	exceptNum in the pEl subtree.					
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static PtrElement   SearchElemWithExcept (int exceptNum, PtrElement pEl, PtrSSchema pSS)
#else  /* __STDC__ */
static PtrElement   SearchElemWithExcept (exceptNum, pEl, pSS)
int                 exceptNum;
PtrElement          pEl;
PtrSSchema          pSS;
#endif /* __STDC__ */
{
   PtrElement          pRes, pChild;

   pRes = NULL;
   if (TypeHasException (exceptNum, pEl->ElTypeNumber, pSS))
      pRes = pEl;
   if (pRes == NULL && !pEl->ElTerminal)
     {
	pChild = pEl->ElFirstChild;
	while (pChild != NULL && pRes == NULL)
	  {
	     pRes = SearchElemWithExcept (exceptNum, pChild, pSS);
	     pChild = pChild->ElNext;
	  }
     }
   return pRes;
}

/*----------------------------------------------------------------------
   ReadImportFile  lit le fichier file et met son contenu dans     
   l'arbre abstrait du document pDoc.                      
   Retourne TRUE si succes, FALSE si echec.                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     ReadImportFile (FILE * file, PtrDocument pDoc)
#else  /* __STDC__ */
static ThotBool     ReadImportFile (file, pDoc)
FILE               *file;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
   PtrElement          pEl, pListEl, pTextEl, pDesc, pPrev, pAncest;
   int                 typeNum, exceptNum, len, i;
   UCHAR_T             line[MAX_TXT_LEN];
   ThotBool            ok, paragraph, emptyLine;

   ok = FALSE;
   /* le schema de structure du document comporte-t-il l'une des deux */
   /* exceptions ExcImportLine ou ExcImportParagraph ? */
   exceptNum = ExcImportLine;
   typeNum = GetElemWithException (exceptNum, pDoc->DocSSchema);
   paragraph = FALSE;
   if (typeNum != 0)
      /* exception ImportLine trouvee. on traitera le fichier d'entree */
      /* par lignes */
      paragraph = FALSE;
   else
     {
	exceptNum = ExcImportParagraph;
	typeNum = GetElemWithException (exceptNum, pDoc->DocSSchema);
	if (typeNum != 0)
	   /* exception ImportParagraph trouvee. on traitera le fichier */
	   /* d'entree par paragraphes */
	   paragraph = TRUE;
     }
   if (typeNum != 0)
      /* une des deux exceptions est definie dans le schema du document */
      /* on procede a l'importation. Sinon, on ne fait rien. */
     {
	/* on cherche le premier element qui porte cette exception */
	pEl = SearchElemWithExcept (exceptNum, pDoc->DocRootElement, pDoc->DocSSchema);
	if (pEl != NULL)
	  {
	     /* on cherche si l'element ou l'un de ses descendants est une liste */
	     /* dont les elements puissent contenir du texte */
	     pListEl = ListDescent (pEl, pDoc);
	     if (pListEl != NULL)
		/* on a trouve' un tel element liste, qui va accueillir le contenu */
		/* du fichier a importer */
	       {
		  pPrev = NULL;
		  pTextEl = NULL;
		  ok = TRUE;
		  /* lit tout le fichier d'entree, ligne par ligne */
		  while (ok && ufgets (line, MAX_TXT_LEN - 1, file) != NULL)
		    {
		       /* une ligne a ete lue dans line */
		       /* traite le caractere '\n' en fin de ligne */
		       len = ustrlen (line);
		       if (line[len - 1] == EOL)
			 {
			    if (paragraph)
			       /* en mode paragraphe, on garde le '\n' qui sera remplace' */
			       /* par un blanc */
			       len++;
			    line[len - 1] = EOS;
			    len--;
			 }
		       /* remplace les caracteres non imprimables par des blancs */
		       /* et teste si la ligne est vide */
		       emptyLine = TRUE;
		       for (i = 0; i < len; i++)
			  if (line[i] <= SPACE || ((int) line[i] >= 127 && (int) line[i] < 160))
			     /* transforme les caracteres non imprimables en espace */
			     line[i] = SPACE;
			  else
			     emptyLine = FALSE;
		       /* traite la ligne lue */
		       if (paragraph && emptyLine)
			  /* on est en mode paragraphe et la ligne est vide. L'element */
			  /* en cours de remplissage est termine'. */
			  pTextEl = NULL;
		       else
			 {
			    if (pTextEl == NULL)
			       /* il n'y a pas d'element en cours de remplissage */
			      {
				 /* on cree un element de la liste, avec sa descendance */
				 /* jusqu'a une feuille de texte */
				 pDesc = CreateDescendant (pListEl->ElTypeNumber, pListEl->ElStructSchema, pDoc,
							   &pTextEl, pListEl->ElAssocNum, CharString + 1,
						   pListEl->ElStructSchema);
				 if (pDesc == NULL || pTextEl == NULL)
				    /* la creation a echoue'. On arrete tout */
				   {
				      ok = FALSE;
				      if (pDesc != NULL)
					 DeleteElement (&pDesc, pDoc);
				   }
				 else
				    /* on insere dans l'arbre abstrait l'element cree'  */
				   {
				      if (pPrev == NULL)
					 /* c'est le premier element, on le met comme fils de */
					 /* l'element liste */
					 InsertFirstChild (pListEl, pDesc);
				      else
					 /* on le met comme frere de l'element cree' precedemment */
					 InsertElementAfter (pPrev, pDesc);
				      pPrev = pDesc;
				   }
			      }
			    if (pTextEl != NULL && ok)
			      {
				 /* on copie la ligne dans l'element de texte courant */
				 CopyStringToText (line, pTextEl->ElText, &len);
				 pTextEl->ElTextLength += len;
				 /* met a jour le volume des elements ascendants */
				 pAncest = pEl;
				 while (pAncest != NULL)
				   {
				      pAncest->ElVolume += len;
				      pAncest = pAncest->ElParent;
				   }
				 if (!paragraph)
				    /* on est en mode ligne. L'element en cours de remplissage */
				    /* est termine' */
				    pTextEl = NULL;
			      }
			 }
		    }
	       }
	  }
     }
   return ok;
}

/*----------------------------------------------------------------------
   ImportDocument        traite l'importation d'un fichier texte	
   SSchemaName: nom du schema de structure du document a creer.   	
   directory: directory du fichier a importer             		
   fileName: nom du fichier a importer.                        	
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                ImportDocument (Name SSchemaName, PathBuffer directory, CHAR_T* fileName)
#else  /* __STDC__ */
void                ImportDocument (SSchemaName, directory, fileName)
Name                SSchemaName;
PathBuffer          directory;
CHAR_T*             fileName;
#endif /* __STDC__ */
{
   FILE               *file;
   PtrDocument         pDoc;
   PathBuffer          fullName;
   NotifyDialog        notifyDoc;
   Name                PSchemaName;
   int                 i;
   ThotBool            ok;

   if (fileName[0] != WC_EOS && SSchemaName[0] != WC_EOS)
      /* les parametres d'entree sont valides */
     {
	if (directory[0] == WC_EOS)
	   /* pas de directory precise'. On prend le path des documents */
	   ustrncpy (directory, DocumentPath, MAX_PATH);
	/* construit le nom complet du fichier a importer */
	MakeCompleteName (fileName, TEXT(""), directory, fullName, &i);
	TtaDisplaySimpleMessage (INFO, LIB, TMSG_IMPORTING_FILE);
	/* ouvre le fichier a importer */
	file = ufopen (fullName, TEXT("r"));
	if (file != NULL)
	   /* le fichier a importer est ouvert */
	  {
	     /* on cree un descripteur de document */
	     CreateDocument (&pDoc);
	     if (pDoc != NULL)
	       {
		  /* pas de preference pour un schema de presentation particulier */
		  PSchemaName[0] = EOS;
		  /* charge le schema de structure et le schema de presentation */
		  LoadSchemas (SSchemaName, PSchemaName, &pDoc->DocSSchema, NULL, FALSE);
		  if (pDoc->DocSSchema != NULL)
		    {
		       /* envoie l'evenement de creation a l'application qui le demande */
		       notifyDoc.event = TteDocCreate;
		       notifyDoc.document = (Document) IdentDocument (pDoc);
		       notifyDoc.view = 0;
		       if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
			 {
			    /* cree la representation interne d'un document minimum */
			    pDoc->DocRootElement = NewSubtree (pDoc->DocSSchema->SsRootElem,
							       pDoc->DocSSchema, pDoc, 0, TRUE, TRUE, TRUE, TRUE);
			    /* supprime les elements exclus */
			    RemoveExcludedElem (&pDoc->DocRootElement, pDoc);
			 }
		    }
		  if (pDoc->DocRootElement == NULL)
		     /* on n'a pas pu charger les schemas ou l'application a refuse' */
		     UnloadDocument (&pDoc);
		  else
		    {
		       /* complete le descripteur du document */
		       pDoc->DocRootElement->ElAccess = AccessReadWrite;
		       CheckLanguageAttr (pDoc, pDoc->DocRootElement);
		       ustrncpy (pDoc->DocDName, fileName, MAX_NAME_LENGTH);
		       pDoc->DocDName[MAX_NAME_LENGTH - 1] = EOS;
		       ustrncpy (pDoc->DocIdent, fileName, MAX_DOC_IDENT_LEN);
		       pDoc->DocIdent[MAX_DOC_IDENT_LEN - 1] = EOS;
		       ustrncpy (pDoc->DocDirectory, directory, MAX_PATH);
		       /* conserve le path actuel des schemas dans le contexte du doc. */
		       ustrncpy (pDoc->DocSchemasPath, SchemaPath, MAX_PATH);
		       /* lit le fichier a importer et met son contenu dans le document */
		       ok = ReadImportFile (file, pDoc);
		       if (!ok)
			  /* echec */
			  TtaDisplaySimpleMessage (INFO, LIB, TMSG_IMPORT_FILE_IMP);
		       /* indique a l'application interessee qu'un document a ete cree' */
		       notifyDoc.event = TteDocCreate;
		       notifyDoc.document = (Document) IdentDocument (pDoc);
		       notifyDoc.view = 0;
		       CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		       /* traitement des attributs requis */
		       AttachMandatoryAttributes (pDoc->DocRootElement, pDoc);
		       if (pDoc->DocSSchema != NULL)
			  /* le document n'a pas ete ferme' pendant l'attente des */
			  /* attributs requis */
			 {
			    /* traitement des exceptions */
			    if (ThotLocalActions[T_createtable] != NULL)
			       (*ThotLocalActions[T_createtable]) (pDoc->DocRootElement, pDoc);
			    /* ouvre les vues du document cree' */
			    OpenDefaultViews (pDoc);
			 }
		    }
	       }
	     /* fermeture du fichier imnporte' */
	     fclose (file);
	  }
     }
}


/*----------------------------------------------------------------------
   DisplayDoc                                                      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         DisplayDoc (PtrDocument pDoc)
#else  /* __STDC__ */
static void         DisplayDoc (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */
{
   NotifyDialog        notifyDoc;
   int                 i, X, Y, width, height;
   ThotBool            complete;

   /* demande la creation d'une fenetre pour la 1ere vue du document */
   ConfigGetViewGeometry (pDoc, pDoc->DocSSchema->SsPSchema->PsView[0], &X, &Y, &width, &height);
   notifyDoc.event = TteViewOpen;
   notifyDoc.document = (Document) IdentDocument (pDoc);
   notifyDoc.view = 0;
   if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
     {
	pDoc->DocViewFrame[0] =
	   CreateWindowWithTitle (pDoc, 1,
				  pDoc->DocSSchema->SsPSchema->PsView[0], &pDoc->DocViewVolume[0], X, Y, width, height);
	if (pDoc->DocViewFrame[0] != 0)
	  {
	    /* Update Paste entry in menu */
	    if ((FirstSavedElement == NULL && ClipboardThot.BuLength == 0) ||
		pDoc->DocReadOnly)
	      SwitchPaste(pDoc, FALSE);
	    else
	      SwitchPaste(pDoc, TRUE);

	    /* check the Undo state of the document */
	    if (pDoc->DocNbEditsInHistory == 0)
	      SwitchUndo (pDoc, FALSE);
	    else
	      SwitchUndo (pDoc, TRUE);

	    /* check the Redo state of the document */
	    if (pDoc->DocNbUndone == 0)
	      SwitchRedo (pDoc, FALSE);
	    else
	      SwitchRedo (pDoc, TRUE);

	    pDoc->DocView[0].DvSSchema = pDoc->DocSSchema;
	    pDoc->DocView[0].DvPSchemaView = 1;
	    pDoc->DocViewFreeVolume[0] = pDoc->DocViewVolume[0];
	    pDoc->DocView[0].DvSync = TRUE;
	    /* met a jour les menus variables de la fenetre */
	    if (ThotLocalActions[T_chselect] != NULL)
	      (*ThotLocalActions[T_chselect]) (pDoc);
	    if (ThotLocalActions[T_chattr] != NULL)
	      (*ThotLocalActions[T_chattr]) (pDoc);
	    if (pDoc->DocRootElement != NULL)
	      {
		pDoc->DocViewRootAb[0] = AbsBoxesCreate (pDoc->DocRootElement, pDoc, 1, TRUE, TRUE, &complete);
		/* on ne s'occupe pas de la hauteur de page */
		i = 0;
		ChangeConcreteImage (pDoc->DocViewFrame[0], &i,
				     pDoc->DocViewRootAb[0]);
		DisplayFrame (pDoc->DocViewFrame[0]);
		ShowSelection (pDoc->DocViewRootAb[0], TRUE);
		notifyDoc.event = TteViewOpen;
		notifyDoc.document = (Document) IdentDocument (pDoc);
		notifyDoc.view = 1;
		CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		/* ouvre les vues specifiees dans la section open du */
		/* fichier .conf */
		ConfigOpenFirstViews (pDoc);
	      }
	  }
     }
}

/*----------------------------------------------------------------------
   RemovePagesBeginTree detruit dans l'arbre de racine pRoot tous	
   les sauts de page engendre's par le debut d'un element  
   qui a la regle de presentation Page.                    
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RemovePagesBeginTree (PtrElement pRoot, PtrDocument pDoc)
#else  /* __STDC__ */
static void         RemovePagesBeginTree (pRoot, pDoc)
PtrElement          pRoot;
PtrDocument         pDoc;
#endif /* __STDC__ */
{
  PtrElement          pPage, pPrevPage;
  
  if (pRoot == NULL)
    return;
  pPage = pRoot;
  pPrevPage = NULL;
  do
    {
      pPage = FwdSearchTypedElem (pPage, PageBreak + 1, NULL);
      if (pPage != NULL)
	if (pPage->ElPageType == PgBegin)
	  {
	    if (pPrevPage != NULL)
	      DeleteElement (&pPrevPage, pDoc);
	    pPrevPage = pPage;
	  }
    }
  while (pPage != NULL);
  if (pPrevPage != NULL)
    DeleteElement (&pPrevPage, pDoc);
}

/*----------------------------------------------------------------------
   ChangeDocumentPSchema effectue le changement de presentation du 
   document pointe' par pDoc. newPSchemaName est le nom du nouveau	
   schema de presentation a appliquer au document.                 
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void          ChangeDocumentPSchema (PtrDocument pDoc, Name newPSchemaName, ThotBool withEvent)
#else  /* __STDC__ */
static void          ChangeDocumentPSchema (pDoc, newPSchemaName, withEvent)
PtrDocument         pDoc;
Name                newPSchemaName;
ThotBool            withEvent;
#endif /* __STDC__ */
{
   PtrPSchema          pPSchema;

#define MAX_PRES_NATURE 20
   PtrSSchema          naturePSchema[MAX_PRES_NATURE];
   PathBuffer          schemaPath;
   Name                nomPres;
   int                 NnaturePSchemas, nat, assoc;

   /* sauve le path courant des schemas */
   ustrncpy (schemaPath, SchemaPath, MAX_PATH);
   /* le path des schemas valide lors du chargement du document */
   /* devient le nouveau path courant des schemas */
   ustrncpy (SchemaPath, pDoc->DocSchemasPath, MAX_PATH);
   /* charge le nouveau schema de presentation */
   pPSchema = LoadPresentationSchema (newPSchemaName, pDoc->DocSSchema);
   if (pPSchema == NULL)
      /* on n'a pas pu charger le nouveau schema de presentation */
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_CHANGE_PRES_IMP);
   else
     {
	/* ferme toutes les vues du document */
	CloseAllViewsDoc (pDoc);
	/* detruit tous les sauts de page engendre's par le debut d'un
	   element qui a la regle de presentation Page */
	RemovePagesBeginTree (pDoc->DocRootElement, pDoc);
	for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
	   if (pDoc->DocAssocRoot[assoc] != NULL)
	      RemovePagesBeginTree (pDoc->DocAssocRoot[assoc], pDoc);
	/* libere l'ancien schema de presentation du document */
	FreePresentationSchema (pDoc->DocSSchema->SsPSchema, pDoc->DocSSchema);
	pDoc->DocSSchema->SsPSchema = pPSchema;
	/* etablit la liste des natures utilisees dans le document */
	NnaturePSchemas = 0;
	SearchNatures (pDoc->DocSSchema, naturePSchema, &NnaturePSchemas, FALSE);
	/* change de schema de presentation pour chaque nature */
	for (nat = 0; nat < NnaturePSchemas; nat++)
	   if (ConfigGetPSchemaNature (pDoc->DocSSchema,
				       naturePSchema[nat]->SsName, nomPres))
	      /* le fichier .conf donne un schema de presentation pour la */
	      /* nature */
	      if (ustrcmp (nomPres, naturePSchema[nat]->SsDefaultPSchema) != 0)
		 /* c'est un schema different de celui qui est charge' */
		{
		   /* charge le nouveau schema de presentation */
		   pPSchema = LoadPresentationSchema (nomPres, naturePSchema[nat]);
		   if (pPSchema != NULL)
		     {
			/* libere l'ancien schema de presentation */
			FreePresentationSchema (naturePSchema[nat]->SsPSchema,
						naturePSchema[nat]);
			/* prend le nouveau schema pour cette nature */
			naturePSchema[nat]->SsPSchema = pPSchema;
		     }
		}
	/* ouvre les vues definies pour le nouveau schema du document */
	DisplayDoc (pDoc); 

        if (withEvent) {
NotifyNaturePresent notifyDoc;

           notifyDoc.event = TteDocNatPresent;
           notifyDoc.document = (Document) IdentDocument (pDoc);
           notifyDoc.nature = (SSchema) pDoc->DocSSchema;

           CallEventType ((NotifyEvent *) & notifyDoc, FALSE);

        };

     }
   ustrncpy (pDoc->DocSchemasPath, SchemaPath, MAX_PATH);
   /* restaure le path courant des schemas */
   ustrncpy (SchemaPath, schemaPath, MAX_PATH);
}


/*----------------------------------------------------------------------
   RedisplayNatureView reconstruit les paves de tous les elements       
   de la nature pNatSSchema qui sont presents dans l'arbre de paves    
   de racine pAb du document pDoc.                                
   Retourne Vrai si au moins un pave a ete reconstruit.            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static ThotBool     RedisplayNatureView (PtrDocument pDoc, PtrAbstractBox pAb, PtrSSchema pNatSSchema, int view, int frame)
#else  /* __STDC__ */
static ThotBool     RedisplayNatureView (pDoc, pAb, pNatSSchema, view, frame)
PtrDocument         pDoc;
PtrAbstractBox      pAb;
PtrSSchema          pNatSSchema;
int                 view;
int                 frame;
#endif /* __STDC__ */
{
  PtrElement          pEl;
  PtrAbstractBox      redispAb, RlNext;
  int                 h;
  ThotBool            result, bool;

  result = FALSE;
  if (pAb != NULL)
    {
      /* on saute les paves de presentation */
      while (pAb->AbPresentationBox && pAb->AbNext != NULL)
	pAb = pAb->AbNext;
      if (!pAb->AbPresentationBox)
	{
	  pEl = pAb->AbElement;
	  if (pEl->ElStructSchema->SsCode != pNatSSchema->SsCode)
	    /* ce n'est pas un pave d'un element de la nature voulue */
	    /* on cherche parmi ses descendants */
	    {
	      pAb = pAb->AbFirstEnclosed;
	      while (pAb != NULL)
		{
		  RlNext = pAb->AbNext;
		  if (RlNext != NULL)
		    while (RlNext->AbPresentationBox && RlNext->AbNext != NULL)
		      RlNext = RlNext->AbNext;
		  bool = RedisplayNatureView (pDoc, pAb, pNatSSchema, view, frame);
		  result = result || bool;
		  pAb = RlNext;
		}
	    }
	  else
	    /* c'est un pave d'un element de la nature voulue */
	    {
	      result = TRUE;
	      /* on detruit les paves de l'element dans cette vue */
	      DestroyAbsBoxesView (pEl, pDoc, FALSE, view);
	      if (AssocView (pEl))
		redispAb = pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1];
	      else
		redispAb = pDoc->DocViewModifiedAb[view - 1];
	      h = 0;
	      bool = ChangeConcreteImage (frame, &h, redispAb);
	      FreeDeadAbstractBoxes (redispAb, frame);
	      CreateNewAbsBoxes (pEl, pDoc, view);
	      if (AssocView (pEl))
		redispAb = pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1];
	      else
		redispAb = pDoc->DocViewModifiedAb[view - 1];
	      bool = ChangeConcreteImage (frame, &h, redispAb);
	    }
	}
    }
  return result;
}


/*----------------------------------------------------------------------
   RedisplayNature reconstruit les paves de tous les elements      
   de la nature pNatSSchema qui sont presents dans les images      
   abstraites du document pDoc, et reaffiche les vues              
   correspondantes.                                                
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         RedisplayNature (PtrDocument pDoc, PtrSSchema pNatSSchema)
#else  /* __STDC__ */
static void         RedisplayNature (pDoc, pNatSSchema)
PtrDocument         pDoc;
PtrSSchema          pNatSSchema;

#endif /* __STDC__ */
{
  PtrAbstractBox      pRootAb;
  int                 view, assoc, frame, volume;

  if (pDoc != NULL && pNatSSchema != NULL)
    {
      /* traite les vues de l'arbre principal */
      for (view = 0; view < MAX_VIEW_DOC; view++)
	if (pDoc->DocView[view].DvPSchemaView != 0)
	  {
	    frame = pDoc->DocViewFrame[view];
	    pRootAb = pDoc->DocViewRootAb[view];
	    /* on donne une capacite infinie a la vue pour que */
	    /* RedisplayNatureView puisse creer les images completes */
	    volume = pDoc->DocViewFreeVolume[view];
	    pDoc->DocViewFreeVolume[view] = THOT_MAXINT;
	    if (RedisplayNatureView (pDoc, pRootAb, pNatSSchema, view + 1, frame))
	      {
		DisplayFrame (frame);
		ShowSelection (pRootAb, TRUE);
	      }
	    pDoc->DocViewFreeVolume[view] = volume;
	  }
      /* traite les vues des elements associes */
      for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
	{
	  frame = pDoc->DocAssocFrame[assoc];
	  if (frame != 0)
	    {
	      pRootAb = pDoc->DocAssocRoot[assoc]->ElAbstractBox[0];
	      volume = pDoc->DocAssocFreeVolume[assoc];
	      pDoc->DocAssocFreeVolume[assoc] = THOT_MAXINT;
	      if (RedisplayNatureView (pDoc, pRootAb, pNatSSchema, 1, frame))
		{
		  DisplayFrame (frame);
		  ShowSelection (pRootAb, TRUE);
		}
	      pDoc->DocAssocFreeVolume[assoc] = volume;
	    }
	}
    }
}


/*----------------------------------------------------------------------
   ChangeNaturePSchema effectue le changement de presentation d'une 
   nature pour le document pointe' par pDoc. newPSchemaName est le nom 
   du nouveau schema de presentation a appliquer a la nature dont  
   le schema de structure est pointe' par pNatSSchema.             
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ChangeNaturePSchema (PtrDocument pDoc, PtrSSchema pNatSSchema, Name newPSchemaName, ThotBool withEvent)
#else  /* __STDC__ */
static void         ChangeNaturePSchema (pDoc, pNatSSchema, newPSchemaName, withEvent)
PtrDocument         pDoc;
PtrSSchema          pNatSSchema;
Name                newPSchemaName;
ThotBool            withEvent;

#endif /* __STDC__ */

{
   PtrPSchema          pPSchema;
   PtrSSchema          naturePSchema[MAX_PRES_NATURE];
   PathBuffer          schemaPath;
   int                 NnaturePSchemas, nat;

   /* sauve le path courant des schemas */
   ustrncpy (schemaPath, SchemaPath, MAX_PATH);
   /* le path des schemas valide lors du chargement du document */
   /* devient le nouveau path courant des schemas */
   ustrncpy (SchemaPath, pDoc->DocSchemasPath, MAX_PATH);
   /* charge le nouveau schema de presentation */
   pPSchema = LoadPresentationSchema (newPSchemaName, pNatSSchema);
   if (pPSchema == NULL)
      /* on n'a pas pu charger le nouveau schema de presentation */
      TtaDisplaySimpleMessage (INFO, LIB, TMSG_CHANGE_PRES_IMP);
   else
     {
	/* libere l'ancien schema de presentation */
	FreePresentationSchema (pNatSSchema->SsPSchema, pNatSSchema);
	/* prend le nouveau schema pour cette nature */
	pNatSSchema->SsPSchema = pPSchema;
	/*etablit la liste de toutes les natures utilisees dans le document */
	NnaturePSchemas = 0;
	SearchNatures (pDoc->DocSSchema, naturePSchema, &NnaturePSchemas, FALSE);
	/* change de schema de presentation pour chaque occurence de */
	/* la nature concernee */
	for (nat = 0; nat < NnaturePSchemas; nat++)
	   if (naturePSchema[nat]->SsCode == pNatSSchema->SsCode)
	      /* c'est la nature concernee */
	      if (naturePSchema[nat] != pNatSSchema)
		 /* ce n'est pas celle qu'on a deja traite' */
		 if (ustrcmp (newPSchemaName, naturePSchema[nat]->SsDefaultPSchema) != 0)
		    /* c'est un schema different de celui qui est charge' */
		   {
		      /* charge le nouveau schema de presentation */
		      pPSchema = LoadPresentationSchema (newPSchemaName, naturePSchema[nat]);
		      if (pPSchema != NULL)
			{
			   /* libere l'ancien schema de presentation */
			   FreePresentationSchema (naturePSchema[nat]->SsPSchema,
						   naturePSchema[nat]);
			   /* prend le nouveau schema pour cette nature */
			   naturePSchema[nat]->SsPSchema = pPSchema;
			}
		   }
	/* reaffiche les elements de la nature qui change de presentation */
	RedisplayNature (pDoc, pNatSSchema);

        if (withEvent) {
NotifyNaturePresent notifyDoc;

           notifyDoc.event = TteDocNatPresent;
           notifyDoc.document = (Document) IdentDocument (pDoc);
           notifyDoc.nature = (SSchema) pNatSSchema;

           CallEventType ((NotifyEvent *) & notifyDoc, FALSE);

        };

     }
   ustrncpy (pDoc->DocSchemasPath, SchemaPath, MAX_PATH);
   /* restaure le path courant des schemas */
   ustrncpy (SchemaPath, schemaPath, MAX_PATH);

}

/*----------------------------------------------------------------------
   TtaChangeNaturePresentSchema

   Change the presentation schema of a document nature.

   Parameters:
   document: the document in which presentation must be changed.
   natureSSchema: the nature
   newPresentation: the name of the new presentation

   Remark:
   This function is only accessible by applications linked
   with the libThotEditor library.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void        TtaChangeNaturePresentSchema (Document document, SSchema natureSSchema, STRING newPresentation)
#else  /* __STDC__ */
void        TtaChangeNaturePresentSchema (document, natureSSchema, newPresentation)
Document            document;
SSchema             natureSSchema;
STRING              newPresentation;
#endif /* __STDC__ */
{
  PtrDocument         pDoc;

  UserErrorCode = 0;
  /* verifies the parameter document */
  if (document < 1 || document > MAX_DOCUMENTS)
    TtaError (ERR_invalid_document_parameter);
  else if (LoadedDocument[document - 1] == NULL)
    TtaError (ERR_invalid_document_parameter);
  else if (natureSSchema == NULL || newPresentation[0] == EOS)
    TtaError (ERR_invalid_parameter);
  else
    /* parameter document is correct */
    {
      pDoc = LoadedDocument[document - 1];
      ChangeNaturePSchema (pDoc, (PtrSSchema) natureSSchema, newPresentation, FALSE);
    }

}

/*----------------------------------------------------------------------
    ChangePresMenuInput traite les retours du menu des schemas de     
            presentation                                           
----------------------------------------------------------------------*/
#ifdef __STDC__
static void         ChangePresMenuInput (int ref, int val)
#else  /* __STDC__ */
static void         ChangePresMenuInput (ref, val)
int                 ref;
int                 val;
#endif /* __STDC__ */
{
  PtrDocument         pDoc;
  Name                newpres;
  int                 nbPres;
  int                 nat;

  if (PrintingDoc == 0)
    pDoc = NULL;
  else
    pDoc = LoadedDocument[PrintingDoc - 1];
  
  nat = ref - (NumMenuPresNature) - 1;
  val++;
  if (val > 0 && nat >= 0)
    if (pDocChangeSchPresent != NULL)
      if (pDocChangeSchPresent->DocSSchema != NULL)
	/* le document est toujours present */
	{
	  nbPres = ConfigMakeMenuPres (TableNaturesSchPresent[nat]->SsName, NULL);
	  if (nbPres > 0)
	    {
	      if (pDoc == pDocChangeSchPresent)
		{
		  /* le document pour lequel a ete construit le formulaire
		   */
		  /* d'impression change au moins un de ses schemas de */
		  /* presentation. Ce formulaire n'est donc plus valide. */
		  /* On ferme le formulaire s'il est present sur l'ecran */
		  /* et on indique qu'il faudra reconstruire ce formulaire. */
		  TtaDestroyDialogue (NumFormPrint);
		  TtaDestroyDialogue (NumPrintingFormat);
		}
	      /* recupere le nom du schema de presentation choisi */
	      ConfigGetPSchemaName (val, newpres);
	      if (nat == 0)
		/* c'est le schema de presentation du doocument */
		ChangeDocumentPSchema (pDocChangeSchPresent, newpres, TRUE);
	      else
		/* c'est une nature dans le document */
		ChangeNaturePSchema (pDocChangeSchPresent,
				     TableNaturesSchPresent[nat], newpres, TRUE);
	    }
	}
}

/*----------------------------------------------------------------------
  TtcChangePresentation affiche le menu de changement de schema de 
  presentation d'un document et de ses natures.
----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtcChangePresentation (Document document, View view)
#else  /* __STDC__ */
void                TtcChangePresentation (document, view)
Document            document;
View                view;
#endif /* __STDC__ */
{
#define LgMaxTableNature 20
   PtrSSchema          TableNatures[LgMaxTableNature];
   int                 entreeDesact[LgMaxTableNature];
   int                 LgTableNatures;
   int                 nbPres;
   CHAR_T              BufMenuNatures[MAX_TXT_LEN];
   CHAR_T              BufMenu[MAX_TXT_LEN];
   CHAR_T              BufMenuB[MAX_TXT_LEN];
   STRING              ptrBufNat;
   STRING              src;
   STRING              dest;
   int                 nat;
   int                 NumSousMenu;
   int                 MenuAActiver;
   int                 i, k, l;
   Name                NomPres;
   CHAR_T              NomUtilisateur[50];
   PtrDocument         pDoc;

   if (ThotLocalActions[T_rchangepres] == NULL)
     TteConnectAction (T_rchangepres, ChangePresMenuInput);

   pDoc = LoadedDocument[document - 1];
   MenuAActiver = 0;
   /* conserve un pointeur sur le contexte du document dont on veut */
   /* changer le schema de presentation */
   pDocChangeSchPresent = pDoc;
   /* met d'abord le schema de structure du document dans la table */
   /* des natures du document */
   TableNatures[0] = pDoc->DocSSchema;
   LgTableNatures = 1;
   /* met ensuite la liste des natures utilisees dans le document */
   SearchNatures (pDoc->DocSSchema, TableNatures, &LgTableNatures, True);
   /* construit un pop-up menu pour la liste des natures dont on */
   /* peut changer le schema de presentation */
   ptrBufNat = &BufMenuNatures[0];
   nbNatures = 0;
   for (nat = 0; nat < LgTableNatures; nat++)
     {
        entreeDesact[nat] = 0;
        /* demande la liste des schemas de presentation definis */
        /* pour cette nature dans la configuration de l'utilisateur */
        nbPres = ConfigMakeMenuPres (TableNatures[nat]->SsName, NULL);
        if (nbPres == 0)
           /* pas de schema de presentation defini */
           TableNatures[nat] = NULL;
        else
           /* il y a des schemas de presentation definis pour cette nature */
          {
             /* on cherche dans cette liste le schema de presentation utilise' */
             /* actuellement dans le document */
             for (k = 1; k <= nbPres && entreeDesact[nat] == 0; k++)
               {
                  /* demande le nom reel du schema de presentation */
                  ConfigGetPSchemaName (k, NomPres);
                  if (ustrcmp (TableNatures[nat]->SsPSchema->PsPresentName, NomPres) == 0)
                     /* c'est le nom du schema de presentation actuel */
                     /* on desactivera l'entree correspondante dans le sous-menu */
                     /* des schemas de presentation de cette nature */
                     entreeDesact[nat] = k;
               }
             if (nbPres == 1 && entreeDesact[nat] != 0)
                /* il n'y a qu'un schema de presentation prevu par la */
                /* configuration et c'est celui qui est utilise' actuellement. */
                /* On ne peut donc pas changer de schema pour cette nature */
                TableNatures[nat] = NULL;
             else if (nbNatures >= NbMaxMenuPresNature)
                /* saturation, on arrete */
                nat = LgTableNatures;
             else
                /* on ajoute cette nature dans le menu en construction */
               {
                  nbNatures++;
                  *ptrBufNat = TEXT('M');     /* il y aura un sous-menu */
                  ptrBufNat++;
                  if (TableNatures[nat] == pDoc->DocSSchema)
                     i = CONFIG_DOCUMENT_STRUCT; /* schema du document */
                  else if (TableNatures[nat]->SsExtension)
                     i = CONFIG_EXTENSION_STRUCT; /* schema d'extension */
                  else
                     i = CONFIG_NATURE_STRUCT; /* schema de nature */
                  TtaConfigSSchemaExternalName (NomUtilisateur, TableNatures[nat]->SsName, i);
                  if (NomUtilisateur[0] == EOS)
                     ustrcpy (ptrBufNat, TableNatures[nat]->SsName);
                  else
                     ustrcpy (ptrBufNat, NomUtilisateur);
                  l = ustrlen (ptrBufNat) + 1;
                  ptrBufNat += l;
               }
          }
     }
   if (nbNatures > 0)
      /* il y a au moins une nature dont on peut changer le schema de */
      /* presentation */
     {
        /* on cree le menu de ces natures s'il y en a plus d'une */
        if (nbNatures > 1)
          {
             TtaNewPopup (NumMenuPresNature, 0, TtaGetMessage (LIB, TMSG_GLOBAL_LAYOUT),
                          nbNatures, BufMenuNatures, NULL, TEXT('L'));
             MenuAActiver = NumMenuPresNature;
          }
        /* pour chacune de ces natures, on cree les sous-menus des */
        /* presentation prevues dans la configuration de l'utilisateur */
        NumSousMenu = 0;
        for (nat = 0; nat < LgTableNatures; nat++)
           if (TableNatures[nat] != NULL)
              /* cette entree de la table des natures n'a pas ete invalidee */
             {
                /* demande la liste des presentations prevues dans la */
                /* configuration de l'utilisateur */
                nbPres = ConfigMakeMenuPres (TableNatures[nat]->SsName, BufMenu);
                /* compose un sous-menu a partir de cette liste */
                if (nbPres > 0)
                  {
                     dest = &BufMenuB[0];
                     src = &BufMenu[0];
                     for (k = 1; k <= nbPres; k++)
                       {
                          /* ajoute 'B' au debut de chaque entree */
                          ustrcpy (dest, TEXT("B"));
                          dest++;
                          l = ustrlen (src);
                          ustrcpy (dest, src);
                          dest += l + 1;
                          src += l + 1;
                       }
                     /* cree le sous-menu des presentations proposees pour cette
 */
                     /* nature */
                     NumSousMenu++;
                     if (TableNatures[nat] == pDoc->DocSSchema)
                        i = CONFIG_DOCUMENT_STRUCT; /* schema du document */
                     else if (TableNatures[nat]->SsExtension)
                        i = CONFIG_EXTENSION_STRUCT; /* schema d'extension */
                     else
                        i = CONFIG_NATURE_STRUCT; /* schema de nature */
                     TtaConfigSSchemaExternalName (NomUtilisateur, TableNatures[nat]->SsName, i);
                     if (NomUtilisateur[0] == EOS)
                        ustrcpy (NomUtilisateur, TableNatures[nat]->SsName);
                     if (nbNatures == 1)
                       {
                          /* il n'y a qu'une nature, c'est un pop-up menu */
                          MenuAActiver = NumMenuPresNature + NumSousMenu;
                          TtaNewPopup (MenuAActiver, 0, NomUtilisateur, nbPres, BufMenuB, NULL, 'L');
                       }
                     else
                        /* il y a plusieurs natures, c'est un sous-menu du menu */
                        /* des natures */
                        TtaNewSubmenu (NumMenuPresNature + NumSousMenu,
                                       NumMenuPresNature, NumSousMenu - 1,
                             NomUtilisateur, nbPres, BufMenuB, NULL, False);
                     /* met a jour la table qui servira au retour du sous-menu */
                     TableNaturesSchPresent[NumSousMenu - 1] = TableNatures[nat];
                     /* desactive l'entree de ce sous-menu qui correspond a la */
                     /* presentation actuelle */
#                    ifndef _WINDOWS
                     if (entreeDesact[nat] > 0)
                        UnsetEntryMenu (NumMenuPresNature + NumSousMenu, entreeDesact[nat] - 1);
#                    endif /* !_WINDOWS */
                  }
             }
        /* active le pop-up menu */
#       ifndef _WINDOWS
        TtaSetDialoguePosition ();
        TtaShowDialogue (MenuAActiver, False);
#       endif /* !_WINDOWS */
     }
}


