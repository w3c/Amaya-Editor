/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
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
 
#ifndef _DIALOG_H_
#define _DIALOG_H_


#ifndef _WINDOWS
#include <X11/Intrinsic.h>
#endif
#include "typebase.h"
#include "tree.h"

#define INTEGER_DATA 1
#define STRING_DATA 2

enum DButtons
  {
     D_CANCEL, D_DONE
  };

#ifndef __CEXTRACT__
#ifdef __STDC__
#ifdef _GTK 
#include <gtk/gtk.h> 
/*-------------------------------------------------------------------------------
  ExposeEvent reaffichee la page lorsqu'un :
  evenement de type "expose_event" est emis.
  Le parametre widget donne la reference de la drawing_area dans laquelle
  la page doit etre redessinee.
  Le parametre event donne des informations sur l'evenement.
  Le parametre data contient le numero de la frame.
--------------------------------------------------------------------------------*/
extern gint ExposeEvent (ThotWidget widget, GdkEventExpose *event, gpointer data);
#endif /* _GTK */


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
extern void         TtaNewPulldown (int ref, ThotMenu parent, STRING title, int number, STRING text, STRING equiv);

/*----------------------------------------------------------------------
   TtaSetPulldownOff suspend le pulldown                           
  ----------------------------------------------------------------------*/
extern void         TtaSetPulldownOff (int ref, ThotWidget parent);

/*----------------------------------------------------------------------
   TtaSetPulldownOn reactive le pulldown                           
  ----------------------------------------------------------------------*/
extern void         TtaSetPulldownOn (int ref, ThotWidget parent);

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
extern void         TtaNewPopup (int ref, ThotWidget parent, STRING title, int number, STRING text, STRING equiv, CHAR_T button);

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
extern void         TtaNewSubmenu (int ref, int ref_parent, int entry, STRING title, int number, STRING text, STRING equiv, ThotBool react);

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
extern void         TtaNewIconMenu (int ref, int ref_parent, int entry, STRING title, int number, Pixmap * icons, ThotBool horizontal);

/*----------------------------------------------------------------------
   TtaSetMenuForm fixe la selection dans un sous-menu de formulaire : 
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre val de'signe l'entre'e se'lectionne'e.               
  ----------------------------------------------------------------------*/
extern void         TtaSetMenuForm (int ref, int val);

/*----------------------------------------------------------------------
   TtaNewToggleMenu cre'e un sous-menu a` choix multiples :           
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre ref_parent identifie le formulaire pe`re.            
   Le parame`tre title donne le titre du catalogue.                   
   Le parame`tre number indique le nombre d'entre'es dans le menu.    
   Le parame`tre text contient la liste des intitule's du catalogue.  
   Chaque intitule' commence par un caracte`re qui donne le type de   
   l'entre'e et se termine par un caracte`re de fin de chai^ne \0.    
   S'il n'est pas nul, le parame`tre equiv donne les acce'le'rateurs  
   des entre'es du menu.                                              
   Quand le parame`tre react est vrai, tout changement de se'lection  
   dans le sous-menu est imme'diatement signale' a` l'application.    
  ----------------------------------------------------------------------*/
extern void         TtaNewToggleMenu (int ref, int ref_parent, STRING title, int number, STRING text, STRING equiv, ThotBool react);

/*----------------------------------------------------------------------
   TtaSetToggleMenu fixe la selection dans un toggle-menu :           
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre val de'signe l'entre'e se'lectionne'e (-1 pour       
   toutes les entre'es). Le parame`tre on indique que le bouton       
   correspondant doit e^tre allume' (on positif) ou e'teint (on nul). 
  ----------------------------------------------------------------------*/
extern void         TtaSetToggleMenu (int ref, int val, ThotBool on);

/*----------------------------------------------------------------------
   TtaChangeMenuEntry modifie l'intitule' texte de l`entre'e entry    
   du menu de'signe' par sa re'fe'rence ref.                          
  ----------------------------------------------------------------------*/
extern void         TtaChangeMenuEntry (int ref, int entry, STRING texte);

/*----------------------------------------------------------------------
   TtaRedrawMenuEntry modifie la couleur et/ou la police de l'entre'e 
   entry du menu de'signe' par sa re'fe'rence ref.                    
  ----------------------------------------------------------------------*/
extern void         TtaRedrawMenuEntry (int ref, int entry, STRING fontname, Pixel color, int activate);

/*----------------------------------------------------------------------
   TtaDestroyDialogue de'truit le catalogue de'signe' par ref.                
  ----------------------------------------------------------------------*/
extern void         TtaDestroyDialogue (int ref);

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
extern void         TtaNewForm (int ref, ThotWidget parent, STRING title, ThotBool horizontal, int packet, CHAR_T button, int dbutton);

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
extern void         TtaNewSheet (int ref, ThotWidget parent, STRING title, int number, STRING text, ThotBool horizontal, int packet, CHAR_T button, int dbutton);

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
void                TtaNewDialogSheet (int ref, ThotWidget parent, STRING title, int number, STRING text, ThotBool horizontal, int packet, CHAR_T button);

/*----------------------------------------------------------------------
   TtaChangeFormTitle change le titre d'un formulaire ou d'une feuille        
   de dialogue :                                                   
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame'tre title donne le titre du catalogue.                   
  ----------------------------------------------------------------------*/
extern void         TtaChangeFormTitle (int ref, STRING title);

/*----------------------------------------------------------------------
   TtaAttachForm attache le catalogue au formulaire ou au feuillet    
   dont il de'pend. Les catalogues sont cre'e's attache's.            
  ----------------------------------------------------------------------*/
extern void         TtaAttachForm (int ref);

/*----------------------------------------------------------------------
   TtaDetachForm detache le catalogue au formulaire ou au feuillet    
   dont il de'pend. Les catalogues sont cre'e's attache's.            
  ----------------------------------------------------------------------*/
extern void         TtaDetachForm (int ref);

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
extern void         TtaNewSelector (int ref, int ref_parent, STRING title, int number, STRING text, int height, STRING label, ThotBool withText, ThotBool react);

/*----------------------------------------------------------------------
   TtaSetSelector initialise l'entre'e et/ou le texte du se'lecteur : 
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre entry positif ou nul donne l'index de l'entre'e      
   se'lectionne'e.                                                    
   Le parame`tre text donne le texte si entry vaut -1.                
  ----------------------------------------------------------------------*/
extern void         TtaSetSelector (int ref, int entry, STRING text);

/*----------------------------------------------------------------------
   TtaNewLabel cre'e un intitule' constant dans un formulaire :       
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre text donne l'intitule'.                              
  ----------------------------------------------------------------------*/
extern void         TtaNewLabel (int ref, int ref_parent, STRING text);

/*----------------------------------------------------------------------
   TtaNewTextForm cre'e une feuille de saisie de texte :              
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre title donne le titre du catalogue.                   
   Les parame`tres width et height indiquent la taille en caracte`ree 
   de la feuille de saisie de texte.                                  
   Quand le parame`tre react est vrai, tout changement dans la        
   feuille de saisie est imme'diatement signale' a` l'application.    
  ----------------------------------------------------------------------*/
extern void         TtaNewTextForm (int ref, int ref_parent, STRING title, int width, int height, ThotBool react);

/*----------------------------------------------------------------------
   TtaSetTextForm initialise une feuille de saisie de texte :         
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre text donne la valeur initiale.                       
  ----------------------------------------------------------------------*/
extern void         TtaSetTextForm (int ref, STRING text);

/*----------------------------------------------------------------------
   TtaNewNumberForm cre'e une feuille de saisie de nombre :           
   Le parame`tre ref donne la re'fe'rence pour l'application.         
   Le parame`tre ref_parent identifie le formulaire pe`re.            
   Le parame`tre title donne le titre du catalogue.                   
   Les parame`tres min et max fixent les bornes valides du nombre.    
   Quand le parame`tre react est vrai, tout changement dans           
   la feuille de saisie est imme'diatement signale' a` l'application. 
  ----------------------------------------------------------------------*/
extern void         TtaNewNumberForm (int ref, int ref_parent, STRING title, int min, int max, ThotBool react);

/*----------------------------------------------------------------------
   TtaSetNumberForm fixe le contenu de la feuille de saisie de texte :        
   Le parame`tre ref donne la re'fe'rence du catalogue.               
   Le parame`tre val donne la valeur initiale.                        
  ----------------------------------------------------------------------*/
extern void         TtaSetNumberForm (int ref, int val);

/*----------------------------------------------------------------------
   TtaSetDialoguePosition me'morise la position actuelle de la souris 
   comme position d'affichage des TtaShowDialogue().               
  ----------------------------------------------------------------------*/
extern void         TtaSetDialoguePosition (void);

/*----------------------------------------------------------------------
   TtaShowDialogue active le catalogue de'signe.                      
  ----------------------------------------------------------------------*/
extern void         TtaShowDialogue (int ref, ThotBool remanent);

/*----------------------------------------------------------------------
   TtaWaitShowDialogue attends le retour du catalogue affiche par     
   TtaShowDialogue.                                                   
  ----------------------------------------------------------------------*/
extern void         TtaWaitShowDialogue (void);

/*----------------------------------------------------------------------
   TtaTestWaitShowDialogue retourne Vrai (1) si un TtaWaitShowDialogue        
   est en cours, sinon Faux (0).                                      
  ----------------------------------------------------------------------*/
extern ThotBool     TtaTestWaitShowDialogue (void);

/*----------------------------------------------------------------------
   TtaAbortShowDialogue abandonne le TtaShowDialogue.                 
  ----------------------------------------------------------------------*/
extern void         TtaAbortShowDialogue (void);

/*----------------------------------------------------------------------
   TtaUnmapDialogue desactive le dialogue s'il est actif.             
  ----------------------------------------------------------------------*/
extern void         TtaUnmapDialogue (int ref);

#else  /* __STDC__ */

extern void         TtaNewPulldown ( /* int ref, ThotWidget parent, char *title, int number, char *text, char *equiv */ );
extern void         TtaSetPulldownOff ( /* int ref, ThotWidget parent */ );
extern void         TtaSetPulldownOn ( /* int ref, ThotWidget parent */ );
extern void         TtaNewPopup ( /* int ref, ThotWidget parent, char *title, int number, char *text, char *equiv, char button */ );
extern void         TtaNewSubmenu ( /* int ref, int ref_parent, int entry, char *title, int number, char *text, char *equiv, ThotBool react */ );
extern void         TtaNewIconMenu ( /*int ref, int ref_parent, int entry, char *title, int number, Pixmap *icons,  ThotBool horizontal */ );
extern void         TtaSetMenuForm ( /* int ref, int val */ );
extern void         TtaNewToggleMenu ( /* int ref, int ref_parent, char *title, int number, char *text, char *equiv, ThotBool react */ );
extern void         TtaSetToggleMenu ( /* int ref, int val, ThotBool on */ );
extern void         TtaChangeMenuEntry ( /* int ref, int entry, char *texte */ );
extern void         TtaRedrawMenuEntry ( /* int ref, int entry, char *fontname, Pixel color, int activate */ );
extern void         TtaDestroyDialogue ( /* int ref */ );
extern void         TtaNewForm (int ref, ThotWidget parent, STRING title, ThotBool horizontal, int packet, CHAR_T button, int dbutton);
extern void         TtaNewSheet (int ref, ThotWidget parent, STRING title, int number, STRING text, ThotBool horizontal, int packet, CHAR_T button, int dbutton);
void                TtaNewDialogSheet (int ref, ThotWidget parent, STRING title, int number, STRING text, ThotBool horizontal, int packet, CHAR_T button);
extern void         TtaChangeFormTitle ( /* int ref, char *title */ );
extern void         TtaAttachForm ( /* int ref */ );
extern void         TtaDetachForm ( /* int ref */ );
extern void         TtaNewSelector ( /* int ref, int ref_parent, char *title, int number, char *text, int height, char *label, ThotBool withText, ThotBool react */ );
extern void         TtaSetSelector ( /* int ref, int entry, char *text */ );
extern void         TtaNewLabel ( /* int ref, int ref_parent, char *text */ );
extern void         TtaNewTextForm ( /* int ref, int ref_parent, char *title, int width, int height, ThotBool react */ );
extern void         TtaSetTextForm ( /* int ref, char *text */ );
extern void         TtaNewNumberForm ( /* int ref, int ref_parent, char *title, int min, int max, ThotBool react */ );
extern void         TtaSetNumberForm ( /* int ref, int val */ );
extern void         TtaSetDialoguePosition ( /* void */ );
extern void         TtaShowDialogue ( /* int ref, ThotBool remanent */ );
extern void         TtaWaitShowDialogue ( /* void */ );
extern ThotBool     TtaTestWaitShowDialogue ( /* void */ );
extern void         TtaAbortShowDialogue ( /* void */ );
extern void         TtaUnmapDialogue ( /* int ref */ );

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */

#endif
