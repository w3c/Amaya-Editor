/*
 *
 *  (c) COPYRIGHT INRIA, Grif, 1996.
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

#undef EXPORT
#define EXPORT extern
#include "creation_tv.h"
#include "platform_tv.h"
#include "modif_tv.h"
#include "page_tv.h"
#include "select_tv.h"
#include "appdialogue_tv.h"
#undef EXPORT
#define EXPORT
#include "edit_tv.h"

#include "appli_f.h"
#include "tree_f.h"
#include "attributes_f.h"
#include "search_f.h"
#include "textcommands_f.h"
#include "config_f.h"
#include "structcreation_f.h"
#include "createabsbox_f.h"
#include "createpages_f.h"
#include "structlist_f.h"
#include "views_f.h"
#include "viewapi_f.h"

#include "appdialogue_f.h"
#include "callback_f.h"
#include "viewcommands_f.h"
#include "exceptions_f.h"
#include "absboxes_f.h"
#include "buildboxes_f.h"
#include "memory_f.h"
#include "structmodif_f.h"
#include "changeabsbox_f.h"
#include "schemas_f.h"
#include "structselect_f.h"
#include "structschema_f.h"
#include "content_f.h"
#include "thotmsg_f.h"
#include "applicationapi_f.h"
#include "docs_f.h"
#include "frame_f.h"
#include "fileaccess_f.h"

/*----------------------------------------------------------------------
   CloseDocument ferme toutes les vue d'un document et decharge ce	
   document. Si pDoc est NULL, demande a` l'utilisateur de 
   designer le document a` fermer et lui demande           
   confirmation, sinon pDoc designe le contexte du document
   a` fermer.                                              
   Detruit egalement le fichier .BAK du document.          
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                CloseDocument (PtrDocument pDoc)

#else  /* __STDC__ */
void                CloseDocument (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   NotifyDialog        notifyDoc;

   if (pDoc != NULL)
     {
	notifyDoc.event = TteDocClose;
	notifyDoc.document = (Document) IdentDocument (pDoc);
	notifyDoc.view = 0;
	if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
	  {
	     /* detruit toutes les vues ouvertes du document */
	     if (ThotLocalActions[T_corrector] != NULL)
		(*ThotLocalActions[T_rscorrector]) (-1, 0, (char *) pDoc);
	     CloseAllViewsDoc (pDoc);
	     notifyDoc.event = TteDocClose;
	     notifyDoc.document = (Document) IdentDocument (pDoc);
	     notifyDoc.view = 0;
	     CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	     UnloadDocument (&pDoc);
	  }
     }
}


/*----------------------------------------------------------------------
   CreateWindowWithTitle						
   	       demande a` l'utilisateur, par l'intermediaire du		
   mediateur, la creation d'une fenetre pour la vue viewName
   du document pointe' par pDoc. Retourne le numero de la   
   frame creee ou 0 si echec, et rend dans vol le volume    
   de la fenetre creee.                                     
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
   int                 createdFrame;
   char                buf[MAX_TXT_LEN];

   if (width == 0 || height == 0)
      TtaDisplayMessage (INFO, TtaGetMessage (LIB, TMSG_CREATE_FRAME_FOR_VIEW), &viewName);
   /* met dans le buffer le nom du document... */
   strncpy (buf, pDoc->DocDName, MAX_NAME_LENGTH);
   strcat (buf, "  ");
   /* ...suivi du nom de la vue */
   strncat (buf, viewName, MAX_NAME_LENGTH);
   /* ...suivi eventuellement de la mention 'Read only' */
   if (pDoc->DocReadOnly)
     {
	strcat (buf, " ");
	strcat (buf, TtaGetMessage (LIB, TMSG_READ_ONLY));
     }
   /* creation d'une frame pour la vue */
   createdFrame = MakeFrame (pDoc->DocSSchema->SsName, view, buf, X, Y, width,
			     height, vol, IdentDocument (pDoc));
   return createdFrame;
}


/*----------------------------------------------------------------------
   ListWithText     retourne TRUE si l'element pEl est une  	
   liste dont les elements peuvent contenir du texte.      
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static boolean      ListWithText (PtrElement pEl, PtrDocument pDoc)

#else  /* __STDC__ */
static boolean      ListWithText (pEl, pDoc)
PtrElement          pEl;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pChild, pNext, pDesc, pTextEl;
   boolean             ok;

   ok = FALSE;
   if (!pEl->ElTerminal)
      if (pEl->ElStructSchema->SsRule[pEl->ElTypeNumber - 1].SrConstruct ==
	  CsList)
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
		DeleteElement (&pDesc);
		/* on detruit les fils de l'element liste, qui seront
		   remplace's par d'autres elements crees au cours de
		   l'importation du fichier */
		pChild = pEl->ElFirstChild;
		while (pChild != NULL)
		  {
		     pNext = pChild->ElNext;
		     DeleteElement (&pChild);
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
static boolean      ReadImportFile (FILE * file, PtrDocument pDoc)

#else  /* __STDC__ */
static boolean      ReadImportFile (file, pDoc)
FILE               *file;
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   PtrElement          pEl, pListEl, pTextEl, pDesc, pPrev, pAncest;
   int                 typeNum, exceptNum, len, i;
   unsigned char       line[MAX_TXT_LEN];
   boolean             ok, paragraph, emptyLine;

   ok = FALSE;
   /* le schema de structure du document comporte-t-il l'une des deux */
   /* exceptions ExcImportLine ou ExcImportParagraph ? */
   exceptNum = ExcImportLine;
   typeNum = GetElemWithException (exceptNum, pDoc->DocSSchema);
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
		  while (ok && fgets (line, MAX_TXT_LEN - 1, file) != NULL)
		    {
		       /* une ligne a ete lue dans line */
		       /* traite le caractere '\n' en fin de ligne */
		       len = strlen (line);
		       if (line[len - 1] == '\n')
			 {
			    if (paragraph)
			       /* en mode paragraphe, on garde le '\n' qui sera remplace' */
			       /* par un blanc */
			       len++;
			    line[len - 1] = '\0';
			    len--;
			 }
		       /* remplace les caracteres non imprimables par des blancs */
		       /* et teste si la ligne est vide */
		       emptyLine = TRUE;
		       for (i = 0; i < len; i++)
			  if (line[i] <= ' ' || ((int) line[i] >= 127 && (int) line[i] < 160))
			     /* transforme les caracteres non imprimables en espace */
			     line[i] = ' ';
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
					 DeleteElement (&pDesc);
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
void                ImportDocument (Name SSchemaName, PathBuffer directory, Name fileName)

#else  /* __STDC__ */
void                ImportDocument (SSchemaName, directory, fileName)
Name                SSchemaName;
PathBuffer          directory;
Name                fileName;

#endif /* __STDC__ */
{
   FILE               *file;
   PtrDocument         pDoc;
   PathBuffer          fullName;
   NotifyDialog        notifyDoc;
   Name                PSchemaName;
   int                 i;
   boolean             ok;

   if (fileName[0] != '\0' && SSchemaName[0] != '\0')
      /* les parametres d'entree sont valides */
     {
	if (directory[0] == '\0')
	   /* pas de directory precise'. On prend le path des documents */
	   strncpy (directory, DocumentPath, MAX_PATH);
	/* construit le nom complet du fichier a importer */
	MakeCompleteName (fileName, "", directory, fullName, &i);
	TtaDisplaySimpleMessage (INFO, LIB, TMSG_IMPORTING_FILE);
	/* ouvre le fichier a importer */
	file = fopen (fullName, "r");
	if (file != NULL)
	   /* le fichier a importer est ouvert */
	  {
	     /* on cree un descripteur de document */
	     CreateDocument (&pDoc);
	     if (pDoc != NULL)
	       {
		  /* pas de preference pour un schema de presentation particulier */
		  PSchemaName[0] = '\0';
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
			    RemoveExcludedElem (&pDoc->DocRootElement);
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
		       strncpy (pDoc->DocDName, fileName, MAX_NAME_LENGTH);
		       strncpy (pDoc->DocIdent, fileName, MAX_NAME_LENGTH);
		       strncpy (pDoc->DocDirectory, directory, MAX_PATH);
		       /* conserve le path actuel des schemas dans le contexte du doc. */
		       strncpy (pDoc->DocSchemasPath, SchemaPath, MAX_PATH);
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
   boolean             complete;

#ifdef __COLPAGE__
   PtrPSchema          pSchPage;

#endif /* __COLPAGE__ */

   /* demande la creation d'une fenetre pour la 1ere vue du document */
   ConfigGetViewGeometry (pDoc, pDoc->DocSSchema->SsPSchema->PsView[0],
			  &X, &Y, &width, &height);
   notifyDoc.event = TteViewOpen;
   notifyDoc.document = (Document) IdentDocument (pDoc);
   notifyDoc.view = 0;
   if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
     {
	pDoc->DocViewFrame[0] =
	   CreateWindowWithTitle (pDoc, 1,
				  pDoc->DocSSchema->SsPSchema->PsView[0],
			      &pDoc->DocViewVolume[0], X, Y, width, height);
	if (pDoc->DocViewFrame[0] != 0)
	  {
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
#ifdef __COLPAGE__
		  /* test si pagine */
		  if (GetPageBoxType (pDoc->DocRootElement->ElFirstChild, 1,
				 &pSchPage) != 0)
		     /* document pagine */
		    {
		       pDoc->DocViewNPages[0] = 0;
		       pDoc->DocViewFreeVolume[0] = THOT_MAXINT;
		    }
#endif /* __COLPAGE__ */
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
   CloseAllViewsDoc ferme toutes les vues ouvertes du document pDoc 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                CloseAllViewsDoc (PtrDocument pDoc)

#else  /* __STDC__ */
void                CloseAllViewsDoc (pDoc)
PtrDocument         pDoc;

#endif /* __STDC__ */

{
   int                 view, assoc;

   if (pDoc != NULL)
     {
	/* detruit les vues de l'arbre principal */
	for (view = 0; view < MAX_VIEW_DOC; view++)
	   if (pDoc->DocView[view].DvPSchemaView != 0)
	     {
		DestroyFrame (pDoc->DocViewFrame[view]);
		CloseDocumentView (pDoc, view + 1, FALSE, FALSE);
	     }
	/* detruit les fenetres des elements associes */
	for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
	   if (pDoc->DocAssocFrame[assoc] != 0)
	     {
		DestroyFrame (pDoc->DocAssocFrame[assoc]);
		CloseDocumentView (pDoc, assoc + 1, TRUE, FALSE);
	     }
     }
}

/*----------------------------------------------------------------------
   RemovePagesBeginTree detruit dans l'arbre de racine pRoot tous	
   les sauts de page engendre's par le debut d'un element  
   qui a la regle de presentation Page.                    
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static void         RemovePagesBeginTree (PtrElement pRoot)

#else  /* __STDC__ */
static void         RemovePagesBeginTree (pRoot)
PtrElement          pRoot;

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
		   DeleteElement (&pPrevPage);
		pPrevPage = pPage;
	     }
     }
   while (pPage != NULL);
   if (pPrevPage != NULL)
      DeleteElement (&pPrevPage);
}

/*----------------------------------------------------------------------
   ChangeDocumentPSchema effectue le changement de presentation du 
   document pointe' par pDoc. newPSchemaName est le nom du nouveau	
   schema de presentation a appliquer au document.                 
  ----------------------------------------------------------------------*/

#ifdef __STDC__
void                ChangeDocumentPSchema (PtrDocument pDoc, Name newPSchemaName)

#else  /* __STDC__ */
void                ChangeDocumentPSchema (pDoc, newPSchemaName)
PtrDocument         pDoc;
Name                newPSchemaName;

#endif /* __STDC__ */

{
   PtrPSchema          pPSchema;

#define MAX_PRES_NATURE 20
   PtrSSchema          naturePSchema[MAX_PRES_NATURE];
   PathBuffer          schemaPath;
   Name                nomPres;
   int                 NnaturePSchemas, nat, assoc;

   /* sauve le path courant des schemas */
   strncpy (schemaPath, SchemaPath, MAX_PATH);
   /* le path des schemas valide lors du chargement du document */
   /* devient le nouveau path courant des schemas */
   strncpy (SchemaPath, pDoc->DocSchemasPath, MAX_PATH);
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
	RemovePagesBeginTree (pDoc->DocRootElement);
	for (assoc = 0; assoc < MAX_ASSOC_DOC; assoc++)
	   if (pDoc->DocAssocRoot[assoc] != NULL)
	      RemovePagesBeginTree (pDoc->DocAssocRoot[assoc]);
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
	      if (strcmp (nomPres, naturePSchema[nat]->SsDefaultPSchema) != 0)
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
     }
   strncpy (pDoc->DocSchemasPath, SchemaPath, MAX_PATH);
   /* restaure le path courant des schemas */
   strncpy (SchemaPath, schemaPath, MAX_PATH);
}


/*----------------------------------------------------------------------
   RedisplayNatureView reconstruit les paves de tous les elements       
   de la nature pNatSSchema qui sont presents dans l'arbre de paves    
   de racine pAb du document pDoc.                                
   Retourne Vrai si au moins un pave a ete reconstruit.            
  ----------------------------------------------------------------------*/

#ifdef __STDC__
static boolean      RedisplayNatureView (PtrDocument pDoc, PtrAbstractBox pAb, PtrSSchema pNatSSchema, int view, int frame)

#else  /* __STDC__ */
static boolean      RedisplayNatureView (pDoc, pAb, pNatSSchema, view, frame)
PtrDocument         pDoc;
PtrAbstractBox      pAb;
PtrSSchema          pNatSSchema;
int                 view;
int                 frame;

#endif /* __STDC__ */

{
   PtrElement          pEl;
   PtrAbstractBox      RedispAbsBox, RlNext;
   int                 h;
   boolean             result, bool;

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
		     RedispAbsBox = pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1];
		  else
		     RedispAbsBox = pDoc->DocViewModifiedAb[view - 1];
		  h = 0;
		  bool = ChangeConcreteImage (frame, &h, RedispAbsBox);
		  FreeDeadAbstractBoxes (RedispAbsBox);
		  CreateNewAbsBoxes (pEl, pDoc, view);
		  if (AssocView (pEl))
		     RedispAbsBox = pDoc->DocAssocModifiedAb[pEl->ElAssocNum - 1];
		  else
		     RedispAbsBox = pDoc->DocViewModifiedAb[view - 1];
		  bool = ChangeConcreteImage (frame, &h, RedispAbsBox);
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
void                ChangeNaturePSchema (PtrDocument pDoc, PtrSSchema pNatSSchema, Name newPSchemaName)
#else  /* __STDC__ */
void                ChangeNaturePSchema (pDoc, pNatSSchema, newPSchemaName)
PtrDocument         pDoc;
PtrSSchema          pNatSSchema;
Name                newPSchemaName;

#endif /* __STDC__ */

{
   PtrPSchema          pPSchema;
   PtrSSchema          naturePSchema[MAX_PRES_NATURE];
   PathBuffer          schemaPath;
   int                 NnaturePSchemas, nat;

   /* sauve le path courant des schemas */
   strncpy (schemaPath, SchemaPath, MAX_PATH);
   /* le path des schemas valide lors du chargement du document */
   /* devient le nouveau path courant des schemas */
   strncpy (SchemaPath, pDoc->DocSchemasPath, MAX_PATH);
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
		 if (strcmp (newPSchemaName, naturePSchema[nat]->SsDefaultPSchema) != 0)
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
     }
   strncpy (pDoc->DocSchemasPath, SchemaPath, MAX_PATH);
   /* restaure le path courant des schemas */
   strncpy (SchemaPath, schemaPath, MAX_PATH);
}

