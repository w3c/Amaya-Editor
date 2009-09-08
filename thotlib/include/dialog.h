/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef _DIALOG_H_
#define _DIALOG_H_

#if defined(_WX) && !defined(NODISPLAY)
extern int WX_SearchResult;         /* 0 if ok, 1 if no replace, 2 if not found */
extern int Current_Color;           // export the panel Color
extern int Current_BackgroundColor; // export the panel Background Color
extern int Current_FontFamily;      // export the panel Font family
extern int Current_FontSize;        // export the panel Font size
extern int SavePANEL_PREFERENCES;   // say if panel preferences are saved


/* SVG Style panel */
extern int  Current_Opacity;
extern ThotBool FillEnabled;
extern int  Current_FillColor;
extern int  Current_FillOpacity;
extern ThotBool StrokeEnabled;
extern int  Current_StrokeColor;
extern int  Current_StrokeOpacity;
extern int  Current_StrokeWidth;

extern char *LastSVGelement;
extern char *LastSVGelementTitle;
extern ThotBool LastSVGelementIsFilled;

#endif /* _WX */



#include "typebase.h"
#include "tree.h"

#define INTEGER_DATA 1
#define STRING_DATA 2

enum DButtons
  {
     D_CANCEL, D_DONE, D_DISCARD
  };


#ifndef __CEXTRACT__

/*----------------------------------------------------------------------
  TtaIsActionActive
  Returns TRUE if the function is available for that document
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsActionActive (char *name, Document doc);


/*----------------------------------------------------------------------
  TtaIsActionAvailable
  Returns TRUE if the function is available
  ----------------------------------------------------------------------*/
extern ThotBool TtaIsActionAvailable (const char *name);

/*----------------------------------------------------------------------
   TtaNewPulldown cre'e un pull-down menu :                           
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre parent identifie le widget pe`re du pull-down menu.  
   Le parame`tre title donne le titre du catalogue.                   
   Le parame`tre number indique le nombre d'entre'es dans le menu.    
   Le parame`tre text contient la liste des intitule's du catalogue.  
   Chaque intitule' commence par un caracte`re qui donne le type de   
   l'entre'e et se termine par un caracte`re de fin de chai^ne \0.    
   S'il n'est pas nul, le parame`tre equiv donne les acce'le'rateurs  
   des entre'es du menu.                                              
   Retourne un code d'erreur.                                         
  ----------------------------------------------------------------------*/
extern void TtaNewPulldown (int ref, ThotMenu parent, char *title,
			    int number, char *text, char* equiv, int max_length);

/*----------------------------------------------------------------------
   TtaSetPulldownOff suspend le pulldown                           
  ----------------------------------------------------------------------*/
extern void TtaSetPulldownOff (int ref, ThotMenu parent);

/*----------------------------------------------------------------------
   TtaSetPulldownOn reactive le pulldown                           
  ----------------------------------------------------------------------*/
extern void TtaSetPulldownOn (int ref, ThotMenu parent);

/*----------------------------------------------------------------------
   TtaNewPopup cre'e un pop-up menu :                                 
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre title donne le titre du catalogue.                   
   Le parame`tre number indique le nombre d'entre'es dans le menu.    
   Le parame`tre text contient la liste des intitule's du catalogue.  
   Chaque intitule' commence par un caracte`re qui donne le type de   
   l'entre'e et se termine par un caracte`re de fin de chai^ne \0.    
   S'il n'est pas nul, le parame`tre equiv donne les acce'le'rateurs  
   des entre'es du menu.                                              
   Le parame`tre button indique le bouton de la souris qui active le  
   menu : 'L' pour left, 'M' pour middle et 'R' pour right.           
  ----------------------------------------------------------------------*/
extern void TtaNewPopup (int ref, ThotWidget parent, char *title, int number,
			 char *text, char *equiv, char button);
extern void TtaNewScrollPopup (int ref, ThotWidget parent, char *title, 
			       int number, char *text, char *equiv, 
                               ThotBool mulitpleOptions, char button);

/*----------------------------------------------------------------------
   TtaNewSubmenu cre'e un sous-menu :                                 
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre ref_parent identifie le formulaire pe`re.            
   Le parametre entry de'signe l'entre'e correspondante dans le menu  
   pe`re. Le parame`tre title donne le titre du catalogue.            
   Le parame`tre number indique le nombre d'entre'es dans le menu.    
   Le parame`tre text contient la liste des intitule's du catalogue.  
   Chaque intitule' commence par un caracte`re qui donne le type de   
   l'entre'e et se termine par un caracte`re de fin de chai^ne \0.    
   S'il n'est pas nul, le parame`tre equiv donne les acce'le'rateurs  
   des entre'es du menu.                                              
   Quand le parame`tre react est vrai, tout changement de se'lection  
   dans le sous-menu est imme'diatement signale' a` l'application.    
  ----------------------------------------------------------------------*/
extern void TtaNewSubmenu (int ref, int ref_parent, int entry, char *title,
			   int number, char *text, char* equiv, int max_length,
			   ThotBool react);

/*----------------------------------------------------------------------
   TtaNewIconMenu cre'e un sous-menu :                                        
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre ref_parent identifie le formulaire pe`re.            
   Le parametre entry de'signe l'entre'e correspondante dans le menu  
   pe`re. Le parame`tre title donne le titre du catalogue.            
   Le parame`tre number indique le nombre d'entre'es dans le menu.    
   Le parame`tre icons contient la liste des icones du catalogue.     
   Tout changement de se'lection dans le sous-menu est imme'diatement 
   signale' a` l'application.                                         
  ----------------------------------------------------------------------*/
extern void TtaNewIconMenu (int ref, int ref_parent, int entry, char *title,
			    int number, ThotIcon * icons, ThotBool horizontal);

/*----------------------------------------------------------------------
   TtaChangeMenuEntry modifie l'intitule' texte de l`entre'e entry    
   du menu de'signe' par sa re'fe'rence ref.                          
  ----------------------------------------------------------------------*/
extern void TtaChangeMenuEntry (int ref, int entry, char *texte);

/*----------------------------------------------------------------------
   TtaRedrawMenuEntry modifie la couleur et/ou la police de l'entre'e 
   entry du menu de'signe' par sa re'fe'rence ref.                    
  ----------------------------------------------------------------------*/
extern void TtaRedrawMenuEntry (int ref, int entry, char *fontname,
				ThotColor color, int activate);

/*----------------------------------------------------------------------
   TtaDestroyDialogue de'truit le catalogue de'signe' par ref.                
  ----------------------------------------------------------------------*/
extern void TtaDestroyDialogue (int ref);

/*----------------------------------------------------------------------
   TtaNewForm cre'e un formulaire :                                   
   Le parame`tre ref donne la reference et parent le ThotWidget pe're 
   Si parent est nul, le menu est attache a la fenetre MainWindow.          
   Le parame'tre title donne le titre du catalogue.            
   Le parame`tre horizontal indique que le formulaire est compose' en 
   lignes (TRUE) ou en colonnes (FALSE).                              
   Le parame`tre package donne le facteur de blocage du formulaire    
   (nombre d'entre'es par ligne ou par colonne).                      
   Le parame'tre button indique le bouton de la souris qui active le  
   menu : 'L' pour left, 'M' pour middle et 'R' pour right.           
  ----------------------------------------------------------------------*/
extern void TtaNewForm (int ref, ThotWidget parent, char *title,
			ThotBool horizontal, int packet, char button, int dbutton);

/*----------------------------------------------------------------------
   TtaNewSheet cre'e un feuillet de commande :                        
   Le parame`tre ref donne la reference et parent le ThotWidget pe're 
   Si parent est nul, le menu est attache a la fenetre MainWindow.          
   Le parame`tre title donne le titre du catalogue.            
   Le parame`tre number indique le nombre de boutons ajoute's au      
   bouton 'QUIT' mis par de'faut.                                     
   Le parame`tre text contient la liste des intitule's des boutons    
   ajoute's au bouton 'QUIT'.                                         
   Chaque intitule' se termine par le caracte`re de fin de chai^ne \0.
   Le parame`tre horizontal indique que le formulaire est compose' en 
   lignes (TRUE) ou en colonnes (FALSE).                              
   Le parame`tre package donne le facteur de blocage du formulaire    
   (nombre d'entre'es par ligne ou par colonne).                      
   Le parame`tre button indique le bouton de la souris qui active le  
   menu : 'L' pour left, 'M' pour middle et 'R' pour right.           
  ----------------------------------------------------------------------*/
extern void TtaNewSheet (int ref, ThotWidget parent, char *title, int number,
			 char *text, ThotBool horizontal, int packet,
			 char button, int dbutton);

/*----------------------------------------------------------------------
   TtaNewDialogSheet cre'e un feuillet de dialogue :                  
   Le parame`tre ref donne la reference et parent le ThotWidget pe're 
   Si parent est nul, le menu est attache a la fenetre MainWindow.          
   Le parame`tre title donne le titre du catalogue.            
   Le parame`tre number indique le nombre de boutons.      
   Le parame`tre text contient la liste des intitule's des boutons    
   ajoute's au bouton 'QUIT'.                                         
   Chaque intitule' se termine par le caracte`re de fin de chai^ne \0.
   Le parame`tre horizontal indique que le formulaire est compose' en 
   lignes (TRUE) ou en colonnes (FALSE).                              
   Le parame`tre package donne le facteur de blocage du formulaire    
   (nombre d'entre'es par ligne ou par colonne).                      
   Le parame`tre button indique le bouton de la souris qui active le  
   menu : 'L' pour left, 'M' pour middle et 'R' pour right.           
  ----------------------------------------------------------------------*/
extern void TtaNewDialogSheet (int ref, ThotWidget parent, char *title, int number,
			       char *text, ThotBool horizontal, int packet, char button);
/*----------------------------------------------------------------------
  TtaSetDefaultButton
  Defines the default result for the GTK event Double-click.
  ----------------------------------------------------------------------*/
extern void TtaSetDefaultButton (int ref, int button);

/*----------------------------------------------------------------------
   TtaChangeFormTitle change le titre d'un formulaire ou d'une feuille        
   de dialogue :                                                   
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame'tre title donne le titre du catalogue.                   
  ----------------------------------------------------------------------*/
extern void TtaChangeFormTitle (int ref, char *title);

/*----------------------------------------------------------------------
   TtaAttachForm attache le catalogue au formulaire ou au feuillet    
   dont il de'pend. Les catalogues sont cre'e's attache's.            
  ----------------------------------------------------------------------*/
extern void TtaAttachForm (int ref);

/*----------------------------------------------------------------------
   TtaDetachForm detache le catalogue au formulaire ou au feuillet    
   dont il de'pend. Les catalogues sont cre'e's attache's.            
  ----------------------------------------------------------------------*/
extern void TtaDetachForm (int ref);

/*----------------------------------------------------------------------
   TtaNewSizedSelector creates a selector of a given width in a dialogue form:
   The parameter ref donne la re'fe'rence pour l'application.
   The parameter title donne le titre du catalogue.             
   The parameter number donne le nombre d'intitule's.          
   The parameter text contient la liste des intitule's.   
   The parameter height donne le nombre d'intitule's visibles a` la
   fois (hauteur de la fenetree^tre de visualisation).
   The parameter label ajoute un choix supple'mentaire a` la liste
   des choix possibles. Ce choix est affiche' se'pare'ment et donc
   mis en e'vidence. 
   The parameter withText indique s'il faut creer une zone texte.
   Quand le parame`tre react est vrai, tout changement de se'lection
   dans le se'lecteur est imme'diatement signale' a` l'application.
  ----------------------------------------------------------------------*/
extern void TtaNewSizedSelector (int ref, int ref_parent, char *title,
				 int number, char *text, int width, int height,
				 char *label, ThotBool withText,
				 ThotBool react);

/*----------------------------------------------------------------------
   TtaNewSelector cre'e un se'lecteur dans un formulaire :            
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre title donne le titre du catalogue.                   
   Le parame`tre number donne le nombre d'intitule's.                 
   Le parame`tre text contient la liste des intitule's.               
   Le parame`tre height donne le nombre d'intitule's visibles a` la   
   fois (hauteur de la fenetree^tre de visualisation).                        
   Le parame`tre label ajoute un choix supple'mentaire a` la liste    
   des choix possibles. Ce choix est affiche' se'pare'ment et donc    
   mis en e'vidence.                                                  
   Le parame`tre withText indique s'il faut creer une zone texte.     
   Quand le parame`tre react est vrai, tout changement de se'lection  
   dans le se'lecteur est imme'diatement signale' a` l'application.   
  ----------------------------------------------------------------------*/
extern void TtaNewSelector (int ref, int ref_parent, char *title, int number,
			    char *text, int height, char *label,
			    ThotBool withText, ThotBool react);

/*----------------------------------------------------------------------
   TtaSetSelector initialise l'entre'e et/ou le texte du se'lecteur : 
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre entry positif ou nul donne l'index de l'entre'e      
   se'lectionne'e.                                                    
   Le parame`tre text donne le texte si entry vaut -1.                
  ----------------------------------------------------------------------*/
extern void TtaSetSelector (int ref, int entry, const char *text);

/*----------------------------------------------------------------------
   TtaNewLabel cre'e un intitule' constant dans un formulaire :       
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre text donne l'intitule'.                              
  ----------------------------------------------------------------------*/
extern void TtaNewLabel (int ref, int ref_parent, const char *text);

/*----------------------------------------------------------------------
   TtaNewPaddedLabel cre'e un intitule' constant dans un formulaire :       
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre text donne l'intitule'.                              
   Padding says how many extra characters to add if the text is inferior
   to it.
  ----------------------------------------------------------------------*/
extern void TtaNewPaddedLabel (int ref, int ref_parent, char *text, int padding);

/*----------------------------------------------------------------------
   TtaNewTextForm cre'e une feuille de saisie de texte :              
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre title donne le titre du catalogue.                   
   Les parame`tres width et height indiquent la taille en caracte`ree 
   de la feuille de saisie de texte.                                  
   Quand le parame`tre react est vrai, tout changement dans la        
   feuille de saisie est imme'diatement signale' a` l'application.    
  ----------------------------------------------------------------------*/
extern void TtaNewTextForm (int ref, int ref_parent, char *title, int width,
			    int height, ThotBool react);


/*----------------------------------------------------------------------
   TtaNewPwdForm creates a dialogue element to input password text:   
   Parameter ref gives the Thot reference.
   Parameter title gives the dialogue title.
   Parameters width and height give the box size. 
   If the parameter react is TRUE, any change in the input box generates a
   callback to the application.
  ----------------------------------------------------------------------*/
extern void TtaNewPwdForm (int ref, int ref_parent, char *title, int width,
			   int height, ThotBool react);

/*----------------------------------------------------------------------
   TtaSetTextForm initialise une feuille de saisie de texte :         
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre text donne la valeur initiale.                       
  ----------------------------------------------------------------------*/
extern void TtaSetTextForm (int ref, char *text);

/*----------------------------------------------------------------------
   TtaNewNumberForm cre'e une feuille de saisie de nombre :           
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre ref_parent identifie le formulaire pe`re.            
   Le parame`tre title donne le titre du catalogue.                   
   Les parame`tres min et max fixent les bornes valides du nombre.    
   Quand le parame`tre react est vrai, tout changement dans           
   la feuille de saisie est imme'diatement signale' a` l'application. 
  ----------------------------------------------------------------------*/
extern void TtaNewNumberForm (int ref, int ref_parent, char *title, int min,
			      int max, ThotBool react);

/*----------------------------------------------------------------------
   TtaSetNumberForm fixe le contenu de la feuille de saisie de texte :        
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre val donne la valeur initiale.                        
  ----------------------------------------------------------------------*/
extern void TtaSetNumberForm (int ref, int val);

/*----------------------------------------------------------------------
   TtaClearTree
   Clears (destroys) the contents of a tree
  ----------------------------------------------------------------------*/
extern ThotWidget TtaClearTree (ThotWidget tree);

/*----------------------------------------------------------------------
   TtaAddTreeItem
   parent points to the parent of the tree. If it's NULL, it's
   the first item.
   sibling points to the immediate sibling of this item. If it's NULL
   the item will be added as the first child of parent.
   item_label gives the text that will be visible on the widget.
   selected and expanded gives info on how to display the item.
   user_data is what the user wants to feed to the callback function.
   Returns the reference of the new widget.
  ----------------------------------------------------------------------*/
extern ThotWidget TtaAddTreeItem (ThotWidget tree, ThotWidget parent, 
				  ThotWidget sibling, char *item_label, 
				  ThotBool selected, ThotBool expanded, 
				  void *user_data);

/*----------------------------------------------------------------------
   TtaNewTreeForm
   The parameter ref gives the catalog reference
   The paramet ref_parent gives the parents reference
   The parameter label gives the form's label
   The parameter multiple says if mutliple selections are allowed inside
   the tree.
   The Parameter callback gives the callback function.
   Returns the pointer of the widget that was created or NULL.
  ----------------------------------------------------------------------*/
extern ThotWidget TtaNewTreeForm (int ref, int ref_parent, char *label, 
				  ThotBool multiple, void *callback);

/*----------------------------------------------------------------------
   TtaInitComboBox
   Initializes the labels in a combo box.
   The parameter w points to a combo box widget.
   The parameter nb_items says how many items are in the list
   The parametes item_labels gives the list of labels.
  ----------------------------------------------------------------------*/
extern void TtaInitComboBox (ThotWidget w, int nb_items, char *item_labels[]);

/*----------------------------------------------------------------------
   TtaNewComboBox
   The parameter ref gives the catalog reference
   The parameter ref_parent gives the parents reference
   The parameter label gives the form's label
   The parameter callback gives the callback function.
   The parameter react says if the widget is reactive.
   Returns the pointer of the widget that was created or NULL.
  ----------------------------------------------------------------------*/
extern ThotWidget TtaNewComboBox (int ref, int ref_parent, char *label, 
				  ThotBool react);


/*----------------------------------------------------------------------
   TtaSetDialoguePosition me'morise la position actuelle de la souris 
   comme position d'affichage des TtaShowDialogue().               
  ----------------------------------------------------------------------*/
extern void TtaSetDialoguePosition (void);

/*----------------------------------------------------------------------
   TtaShowDialogue active le catalogue de'signe.                      
  ----------------------------------------------------------------------*/
extern void TtaShowDialogue (int ref, ThotBool remanent, ThotBool move);

/*----------------------------------------------------------------------
   TtaWaitShowProcDialogue waits for a catalogue (that has its own
   callback handler) to end.
  ----------------------------------------------------------------------*/
extern void TtaWaitShowProcDialogue (void);

/*----------------------------------------------------------------------
   TtaWaitShowDialogue attends le retour du catalogue affiche par     
   TtaShowDialogue.                                                   
  ----------------------------------------------------------------------*/
extern void TtaWaitShowDialogue (void);

/*----------------------------------------------------------------------
   TtaTestWaitShowDialogue retourne Vrai (1) si un TtaWaitShowDialogue        
   est en cours, sinon Faux (0).                                      
  ----------------------------------------------------------------------*/
extern ThotBool TtaTestWaitShowDialogue (void);

/*----------------------------------------------------------------------
   TtaAbortShowDialogue abandonne le TtaShowDialogue.                 
  ----------------------------------------------------------------------*/
extern void TtaAbortShowDialogue (void);

/*----------------------------------------------------------------------
  TtaRaiseDialogue raises the dialogue if it exists,
  ----------------------------------------------------------------------*/
extern ThotBool TtaRaiseDialogue (int ref);

/*----------------------------------------------------------------------
   TtaUnmapDialogue desactive le dialogue s'il est actif.             
  ----------------------------------------------------------------------*/
extern void TtaUnmapDialogue (int ref);

/*----------------------------------------------------------------------
  TtaFreeAllCatalogs frees the memory associated with catalogs.                      
  ----------------------------------------------------------------------*/
extern void TtaFreeAllCatalogs (void);

#endif /* __CEXTRACT__ */

#endif
