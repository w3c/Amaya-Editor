/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2001
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
#ifdef IV

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
#include "checkermenu_f.h"
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
static int CreateWindowWithTitle (PtrDocument pDoc, int view, Name viewName,
				  int *vol, int X, int Y, int width, int height)
{
   int    createdFrame;
   char   buf[MAX_TXT_LEN];

   /* met dans le buffer le nom du document... */
   strncpy (buf, pDoc->DocDName, MAX_NAME_LENGTH);
   strcat (buf, "  ");
   /* ...suivi eventuellement de la mention 'Read only' */
   if (pDoc->DocReadOnly)
     {
	strcat (buf, " ");
	strcat (buf, TtaGetMessage (LIB, TMSG_READ_ONLY));
     }
   /* creation d'une frame pour la vue */
   createdFrame = MakeFrame (pDoc->DocSSchema->SsName, view, buf, X, Y,
			     width, height, vol, IdentDocument (pDoc));
   return createdFrame;
}

/*----------------------------------------------------------------------
   ListWithText     retourne TRUE si l'element pEl est une  	
   liste dont les elements peuvent contenir du texte.      
  ----------------------------------------------------------------------*/
static ThotBool     ListWithText (PtrElement pEl, PtrDocument pDoc)
{
   PtrElement          pChild, pNext, pDesc, pTextEl;
   ThotBool            ok;

   ok = FALSE;
   if (!pEl->ElTerminal)
      if (pEl->ElStructSchema->SsRule->SrElem[pEl->ElTypeNumber - 1]->SrConstruct ==
	                                                               CsList)
	 /* c'est bien une liste */
	{
	   /* essaie de creer une descendance de cette liste qui mene a une */
	   /* feuille de texte */
	   pDesc = CreateDescendant (pEl->ElTypeNumber, pEl->ElStructSchema,
				     pDoc, &pTextEl, CharString + 1,
				     pEl->ElStructSchema);
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
static PtrElement   ListDescent (PtrElement pEl, PtrDocument pDoc)
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
static PtrElement   SearchElemWithExcept (int exceptNum, PtrElement pEl,
					  PtrSSchema pSS)
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
static ThotBool     ReadImportFile (FILE * file, PtrDocument pDoc)
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
      pEl = SearchElemWithExcept (exceptNum, pDoc->DocDocElement,
				  pDoc->DocSSchema);
      if (pEl != NULL)
	 {
	 /* on cherche si l'element ou l'un de ses descendants est une liste */
	 /* dont les elements puissent contenir du texte */
	 pListEl = ListDescent (pEl, pDoc);
	 if (pListEl != NULL)
	    /* on a trouve' un tel element liste, qui va accueillir le */
	    /* contenu du fichier a importer */
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
		     /* en mode paragraphe, on garde le '\n' qui sera */
		     /*  remplace' par un blanc */
		     len++;
		  line[len - 1] = EOS;
		  len--;
		  }
	       /* remplace les caracteres non imprimables par des blancs */
	       /* et teste si la ligne est vide */
	       emptyLine = TRUE;
	       for (i = 0; i < len; i++)
		  if (line[i] <= SPACE ||
		      ((int) line[i] >= 127 && (int) line[i] < 160))
		     /* transforme les caracteres non imprimables en espace */
		     line[i] = SPACE;
		  else
		     emptyLine = FALSE;
	       /* traite la ligne lue */
	       if (paragraph && emptyLine)
		  /* on est en mode paragraphe et la ligne est vide. */
		  /* L'element en cours de remplissage est termine'. */
		  pTextEl = NULL;
	       else
		  {
		  if (pTextEl == NULL)
		     /* il n'y a pas d'element en cours de remplissage */
		     {
		     /* on cree un element de la liste, avec sa descendance */
		     /* jusqu'a une feuille de texte */
		     pDesc = CreateDescendant (pListEl->ElTypeNumber,
			        pListEl->ElStructSchema, pDoc, &pTextEl,
				CharString + 1, pListEl->ElStructSchema);
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
			   /* c'est le premier element, on le met comme fils */
			   /* de l'element liste */
			   InsertFirstChild (pListEl, pDesc);
			else
			   /* on le met comme frere de l'element cree'
			      precedemment */
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
		        /* on est en mode ligne. L'element en cours de */
		        /* remplissage est termine' */
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
void                ImportDocument (Name SSchemaName, PathBuffer directory,
				    CHAR_T* fileName)
{
  FILE               *file;
  PtrDocument         pDoc;
  PathBuffer          fullName;
  NotifyDialog        notifyDoc;
  Name                PSchemaName;
  Document            doc;
  PtrElement          pEl;
  int                 i;
  ThotBool            ok;

  if (fileName[0] != WC_EOS && SSchemaName[0] != WC_EOS)
    /* les parametres d'entree sont valides */
    {
      if (directory[0] == WC_EOS)
	/* pas de directory precise'. On prend le path des documents */
	ustrncpy (directory, DocumentPath, MAX_PATH);
      /* construit le nom complet du fichier a importer */
      MakeCompleteName (fileName, "", directory, fullName, &i);
      /* ouvre le fichier a importer */
      file = ufopen (fullName, "r");
      if (file != NULL)
	/* le fichier a importer est ouvert */
	{
	  /* on cree un descripteur de document */
	  doc = 0;
	 CreateDocument (&pDoc, &doc);
	 if (pDoc != NULL)
	   {
	     /* pas de preference pour un schema de presentation particulier */
	     PSchemaName[0] = EOS;
	     /* charge le schema de structure et le schema de presentation */
	     LoadSchemas (SSchemaName, PSchemaName, &pDoc->DocSSchema, pDoc,
			  NULL, FALSE);
	     pEl = NULL;
	     if (pDoc->DocSSchema != NULL)
	       {
		 /* send the event notification to the application */
		 notifyDoc.event = TteDocCreate;
		 notifyDoc.document = doc;
		 notifyDoc.view = 0;
		 if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
		   {
		     /* cree la representation interne d'un document minimum */
		     pEl = NewSubtree (pDoc->DocSSchema->SsRootElem,
				       pDoc->DocSSchema, pDoc, TRUE, TRUE,
				       TRUE, TRUE);
		     if (pEl)
		       {
			 pDoc->DocDocElement = NewSubtree (pDoc->DocSSchema->SsDocument,
						        pDoc->DocSSchema, pDoc,
							FALSE, TRUE, TRUE,
							TRUE);
			 InsertFirstChild (pDoc->DocDocElement, pEl);
			 /* supprime les elements exclus */
			 RemoveExcludedElem (&pDoc->DocDocElement, pDoc);
		       }
		   }
	       }
	     if (pDoc->DocDocElement == NULL)
	       /* on n'a pas pu charger les schemas ou l'application refuse */
	       UnloadDocument (&pDoc);
	     else
	       {
		 /* complete le descripteur du document */
		 pDoc->DocDocElement->ElAccess = AccessReadWrite;
		 if (pEl)
		   CheckLanguageAttr (pDoc, pEl);
		 ustrncpy (pDoc->DocDName, fileName, MAX_NAME_LENGTH);
		 pDoc->DocDName[MAX_NAME_LENGTH - 1] = EOS;
		 ustrncpy (pDoc->DocIdent, fileName, MAX_DOC_IDENT_LEN);
		 pDoc->DocIdent[MAX_DOC_IDENT_LEN - 1] = EOS;
		 ustrncpy (pDoc->DocDirectory, directory, MAX_PATH);
		 /* conserve le path actuel des schemas dans le contexte
		    du document */
		 ustrncpy (pDoc->DocSchemasPath, SchemaPath, MAX_PATH);
		 /* lit le fichier a importer et met son contenu dans le
		    document */
		 ok = ReadImportFile (file, pDoc);
		 /* indique a l'application interessee qu'un document a ete
		    cree' */
		 notifyDoc.event = TteDocCreate;
		 notifyDoc.document = doc;
		 notifyDoc.view = 0;
		 CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
		 /* traitement des attributs requis */
		 AttachMandatoryAttributes (pDoc->DocDocElement, pDoc);
		 if (pDoc->DocSSchema != NULL)
		   /* le document n'a pas ete ferme' pendant l'attente des */
		   /* attributs requis */
		   {
		     /* traitement des exceptions */
		     if (ThotLocalActions[T_createtable] != NULL)
		       (*ThotLocalActions[T_createtable])(pDoc->DocDocElement,
							   pDoc);
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
static void         DisplayDoc (PtrDocument pDoc)
{
   NotifyDialog        notifyDoc;
   int                 i, X, Y, width, height;
   PtrPSchema          pPS;
   ThotBool            complete;

   /* demande la creation d'une fenetre pour la 1ere vue du document */
   pPS = PresentationSchema (pDoc->DocSSchema, pDoc);
   ConfigGetViewGeometry (pDoc, pPS->PsView[0], &X, &Y, &width, &height);
   notifyDoc.event = TteViewOpen;
   notifyDoc.document = (Document) IdentDocument (pDoc);
   notifyDoc.view = 0;
   if (!CallEventType ((NotifyEvent *) & notifyDoc, TRUE))
      {
      pDoc->DocViewFrame[0] = CreateWindowWithTitle (pDoc, 1, pPS->PsView[0],
				 &pDoc->DocViewVolume[0], X, Y, width, height);
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
	 pDoc->DocView[0].DvSync = TRUE;
	 pDoc->DocView[0].DvFirstGuestView = NULL;
	 /* create the guest view list for that view */
	 CreateGuestViewList (pDoc, 1);

	 pDoc->DocViewFreeVolume[0] = pDoc->DocViewVolume[0];
	 /* met a jour les menus variables de la fenetre */
	 if (ThotLocalActions[T_chselect] != NULL)
	    (*ThotLocalActions[T_chselect]) (pDoc);
	 if (ThotLocalActions[T_chattr] != NULL)
	    (*ThotLocalActions[T_chattr]) (pDoc);
	 if (pDoc->DocDocElement != NULL)
	    {
	    pDoc->DocViewRootAb[0] = AbsBoxesCreate (pDoc->DocDocElement,
					       pDoc, 1, TRUE, TRUE, &complete);
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
static void         RemovePagesBeginTree (PtrElement pRoot, PtrDocument pDoc)
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
static void ChangeDocumentPSchema (PtrDocument pDoc, Name newPSchemaName,
				   ThotBool withEvent)
{
#define MAX_PRES_NATURE 20
   PtrSSchema          naturePSchema[MAX_PRES_NATURE];
   PathBuffer          schemaPath;
   Name                nomPres;
   int                 NnaturePSchemas, nat;
   NotifyNaturePresent notifyDoc;

   /* sauve le path courant des schemas */
   ustrncpy (schemaPath, SchemaPath, MAX_PATH);
   /* le path des schemas valide lors du chargement du document */
   /* devient le nouveau path courant des schemas */
   ustrncpy (SchemaPath, pDoc->DocSchemasPath, MAX_PATH);
   /* charge le nouveau schema de presentation */
   if (LoadPresentationSchema (newPSchemaName, pDoc->DocSSchema, pDoc))
     {
      /* ferme toutes les vues du document */
      CloseAllViewsDoc (pDoc);
      /* detruit tous les sauts de page engendre's par le debut d'un
	 element qui a la regle de presentation Page */
      RemovePagesBeginTree (pDoc->DocDocElement, pDoc);
      /* etablit la liste des natures utilisees dans le document */
      SearchNatures (pDoc, naturePSchema, &NnaturePSchemas);
      /* change de schema de presentation pour chaque nature
       (on saute la premiere entree: schema du document) */
      for (nat = 1; nat < NnaturePSchemas; nat++)
	if (ConfigGetPSchemaNature (pDoc->DocSSchema,
				    naturePSchema[nat]->SsName, nomPres))
	  /* le fichier .conf donne un schema de presentation pour la */
	  /* nature */
	  if (ustrcmp (nomPres, naturePSchema[nat]->SsDefaultPSchema) != 0)
	    /* c'est un schema different de celui qui est charge' */
	    /* charge le nouveau schema de presentation */
	    LoadPresentationSchema (nomPres, naturePSchema[nat], pDoc);
      /* ouvre les vues definies pour le nouveau schema du document */
      DisplayDoc (pDoc); 
      
      if (withEvent)
	 {
         notifyDoc.event = TteDocNatPresent;
	 notifyDoc.document = (Document) IdentDocument (pDoc);
	 notifyDoc.nature = (SSchema) pDoc->DocSSchema;
	 CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	 }
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
static ThotBool     RedisplayNatureView (PtrDocument pDoc, PtrAbstractBox pAb,
					 PtrSSchema pNatSSchema, int view,
					 int frame)
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
	  if (ustrcmp (pEl->ElStructSchema->SsName, pNatSSchema->SsName))
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
		  bool = RedisplayNatureView (pDoc, pAb, pNatSSchema, view,
					      frame);
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
	      redispAb = pDoc->DocViewModifiedAb[view - 1];
	      h = 0;
	      bool = ChangeConcreteImage (frame, &h, redispAb);
	      FreeDeadAbstractBoxes (redispAb, frame);
	      CreateNewAbsBoxes (pEl, pDoc, view);
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
static void         RedisplayNature (PtrDocument pDoc, PtrSSchema pNatSSchema)
{
  PtrAbstractBox      pRootAb;
  int                 view, frame, volume;

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
	    if (RedisplayNatureView (pDoc, pRootAb, pNatSSchema, view + 1,
				     frame))
	      {
		DisplayFrame (frame);
		ShowSelection (pRootAb, TRUE);
	      }
	    pDoc->DocViewFreeVolume[view] = volume;
	  }
    }
}

/*----------------------------------------------------------------------
   ChangeNaturePSchema effectue le changement de presentation d'une 
   nature pour le document pointe' par pDoc. newPSchemaName est le nom 
   du nouveau schema de presentation a appliquer a la nature dont  
   le schema de structure est pointe' par pNatSSchema.             
  ----------------------------------------------------------------------*/
static void  ChangeNaturePSchema (PtrDocument pDoc, PtrSSchema pNatSSchema,
				  Name newPSchemaName, ThotBool withEvent)
{
   PtrSSchema          naturePSchema[MAX_PRES_NATURE];
   PathBuffer          schemaPath;
   int                 NnaturePSchemas, nat;
   NotifyNaturePresent notifyDoc;

   /* sauve le path courant des schemas */
   ustrncpy (schemaPath, SchemaPath, MAX_PATH);
   /* le path des schemas valide lors du chargement du document */
   /* devient le nouveau path courant des schemas */
   ustrncpy (SchemaPath, pDoc->DocSchemasPath, MAX_PATH);
   /* charge le nouveau schema de presentation */
   if (LoadPresentationSchema (newPSchemaName, pNatSSchema, pDoc))
     {
	AddSchemaGuestViews (pDoc, pNatSSchema);
	/* etablit la liste de toutes les natures utilisees dans le document */
	SearchNatures (pDoc, naturePSchema, &NnaturePSchemas);
	/* change de schema de presentation pour chaque occurence de */
	/* la nature concernee. On saute la 1ere entree: schema du document */
	for (nat = 1; nat < NnaturePSchemas; nat++)
	   if (!ustrcmp (naturePSchema[nat]->SsName, pNatSSchema->SsName))
	      /* c'est la nature concernee */
	      if (naturePSchema[nat] != pNatSSchema)
		 /* ce n'est pas celle qu'on a deja traite' */
		 if (ustrcmp (newPSchemaName,
			      naturePSchema[nat]->SsDefaultPSchema) != 0)
		    /* c'est un schema different de celui qui est charge' */
		   {
		      /* charge le nouveau schema de presentation */
		      if (LoadPresentationSchema (newPSchemaName,
						  naturePSchema[nat], pDoc))
			AddSchemaGuestViews (pDoc, naturePSchema[nat]);
		   }
	/* reaffiche les elements de la nature qui change de presentation */
	RedisplayNature (pDoc, pNatSSchema);

        if (withEvent)
	   {
           notifyDoc.event = TteDocNatPresent;
           notifyDoc.document = (Document) IdentDocument (pDoc);
           notifyDoc.nature = (SSchema) pNatSSchema;
           CallEventType ((NotifyEvent *) & notifyDoc, FALSE);
	   }

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
void TtaChangeNaturePresentSchema (Document document, SSchema natureSSchema,
				   STRING newPresentation)
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
      ChangeNaturePSchema (pDoc, (PtrSSchema) natureSSchema, newPresentation,
			   FALSE);
    }
}

/*----------------------------------------------------------------------
    ChangePresMenuInput traite les retours du menu des schemas de     
            presentation                                           
----------------------------------------------------------------------*/
static void         ChangePresMenuInput (int ref, int val)
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
	  nbPres = ConfigMakeMenuPres (TableNaturesSchPresent[nat]->SsName,
				       NULL);
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
				     TableNaturesSchPresent[nat], newpres,
				     TRUE);
	    }
	}
}

/*----------------------------------------------------------------------
  TtcChangePresentation affiche le menu de changement de schema de 
  presentation d'un document et de ses natures.
----------------------------------------------------------------------*/
void                TtcChangePresentation (Document document, View view)
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
   PtrPSchema          pPS;

   if (ThotLocalActions[T_rchangepres] == NULL)
     TteConnectAction (T_rchangepres, ChangePresMenuInput);

   pDoc = LoadedDocument[document - 1];
   MenuAActiver = 0;
   /* conserve un pointeur sur le contexte du document dont on veut */
   /* changer le schema de presentation */
   pDocChangeSchPresent = pDoc;
   /* etablit la liste des natures utilisees dans le document */
   SearchNatures (pDoc, TableNatures, &LgTableNatures);
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
             /* on cherche dans cette liste le schema de presentation
		utilise' */
             /* actuellement dans le document */
             for (k = 1; k <= nbPres && entreeDesact[nat] == 0; k++)
               {
                  /* demande le nom reel du schema de presentation */
                  ConfigGetPSchemaName (k, NomPres);
		  pPS = PresentationSchema (TableNatures[nat], pDoc);
                  if (ustrcmp (pPS->PsPresentName, NomPres) == 0)
                     /* c'est le nom du schema de presentation actuel */
                     /* on desactivera l'entree correspondante dans le
			sous-menu des schemas de presentation de cette nature*/
                     entreeDesact[nat] = k;
               }
             if (nbPres == 1 && entreeDesact[nat] != 0)
                /* il n'y a qu'un schema de presentation prevu par la */
                /* configuration et c'est celui qui est utilise' actuellement*/
                /* On ne peut donc pas changer de schema pour cette nature */
                TableNatures[nat] = NULL;
             else if (nbNatures >= NbMaxMenuPresNature)
                /* saturation, on arrete */
                nat = LgTableNatures;
             else
                /* on ajoute cette nature dans le menu en construction */
               {
                  nbNatures++;
                  *ptrBufNat = 'M';     /* il y aura un sous-menu */
                  ptrBufNat++;
                  if (TableNatures[nat] == pDoc->DocSSchema)
                     i = CONFIG_DOCUMENT_STRUCT; /* schema du document */
                  else if (TableNatures[nat]->SsExtension)
                     i = CONFIG_EXTENSION_STRUCT; /* schema d'extension */
                  else
                     i = CONFIG_NATURE_STRUCT; /* schema de nature */
                  TtaConfigSSchemaExternalName (NomUtilisateur,
						TableNatures[nat]->SsName, i);
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
             TtaNewPopup (NumMenuPresNature, 0,
			  TtaGetMessage (LIB, TMSG_GLOBAL_LAYOUT),
                          nbNatures, BufMenuNatures, NULL, 'L');
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
                nbPres = ConfigMakeMenuPres (TableNatures[nat]->SsName,
					     BufMenu);
                /* compose un sous-menu a partir de cette liste */
                if (nbPres > 0)
                  {
                     dest = &BufMenuB[0];
                     src = &BufMenu[0];
                     for (k = 1; k <= nbPres; k++)
                       {
                          /* ajoute 'B' au debut de chaque entree */
                          ustrcpy (dest, "B");
                          dest++;
                          l = ustrlen (src);
                          ustrcpy (dest, src);
                          dest += l + 1;
                          src += l + 1;
                       }
                     /* cree le sous-menu des presentations proposees pour */
                     /* cette nature */
                     NumSousMenu++;
                     if (TableNatures[nat] == pDoc->DocSSchema)
                        i = CONFIG_DOCUMENT_STRUCT; /* schema du document */
                     else if (TableNatures[nat]->SsExtension)
                        i = CONFIG_EXTENSION_STRUCT; /* schema d'extension */
                     else
                        i = CONFIG_NATURE_STRUCT; /* schema de nature */
                     TtaConfigSSchemaExternalName (NomUtilisateur,
						   TableNatures[nat]->SsName,
						   i);
                     if (NomUtilisateur[0] == EOS)
                        ustrcpy (NomUtilisateur, TableNatures[nat]->SsName);
                     if (nbNatures == 1)
                       {
                          /* il n'y a qu'une nature, c'est un pop-up menu */
                          MenuAActiver = NumMenuPresNature + NumSousMenu;
                          TtaNewPopup (MenuAActiver, 0, NomUtilisateur, nbPres, BufMenuB, NULL, 'L');
                       }
                     else
                        /* il y a plusieurs natures, c'est un sous-menu du */
                        /* menu des natures */
                        TtaNewSubmenu (NumMenuPresNature + NumSousMenu,
                                       NumMenuPresNature, NumSousMenu - 1,
                             NomUtilisateur, nbPres, BufMenuB, NULL, False);
                     /* met a jour la table qui servira au retour du
			sous-menu */
                     TableNaturesSchPresent[NumSousMenu-1] = TableNatures[nat];
                     /* desactive l'entree de ce sous-menu qui correspond a */
                     /* la presentation actuelle */
#ifndef _WINDOWS
                     if (entreeDesact[nat] > 0)
                        UnsetEntryMenu (NumMenuPresNature + NumSousMenu,
					entreeDesact[nat] - 1);
#endif /* !_WINDOWS */
                  }
             }
        /* active le pop-up menu */
#ifndef _WINDOWS
        TtaSetDialoguePosition ();
        TtaShowDialogue (MenuAActiver, False);
#endif /* !_WINDOWS */
     }
}
#endif /* IV */
