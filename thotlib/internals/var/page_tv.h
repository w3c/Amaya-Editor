/*
 * page.var
 */



#ifdef __COLPAGE__
/* Hauteur de la page sans le bas de page minimum */
EXPORT int     	BreakPageHeight; /* hauteur de la coupure corps+haut*/
EXPORT int  PageHeaderHeight; /* hauteur haut*/
EXPORT int  WholePageHeight; /* hauteur totale calculee a partir */
        /* de la hauteur du haut, du bas et du corps de page */
        /* cette variable est reevaluee apres creation d'une */
        /* page de type PgBegin */
EXPORT boolean StopBeforeCreation;
				 /* evaluation avec arret sur MP ou ref*/
EXPORT boolean FoundPageHF; /* pour savoir la cause de l'arret de */
 /* creation des paves: soit volume soit MP ou d'un elt asscoc HB */
EXPORT boolean StopGroupCol; 
                 /* detection d'une colonne groupee pour l'equilibrage */
EXPORT boolean ToBalance; 
           /* demande de detection d'une colonne groupee pour l'equilibrage */
/* Pointeur sur la reference a l' element associe'
 * dont l'image apparait dans une boite de haut ou de bas de page
 * et a provoque un debordement de page.
 */
EXPORT PtrElement 	HFPageRefAssoc;
/* Pointeur sur le pave a detruire dans le cas d'un debordement de page
 * c'est soit le pave de l'element associe soit son englobant si c'etait
 * le seul place dans la page. C'est le pointeur sur le pave principal, 
 *les paves de presentation avant et apres seront detruits dans le module page.
*/
EXPORT PtrAbstractBox		AbsBoxAssocToDestroy;
#else /* __COLPAGE__ */
EXPORT int     	PageHeight; /* hauteur de la coupure corps+haut*/
#endif /* __COLPAGE__ */

EXPORT int      RealPageHeight; /* hauteur de la page pour le Mediateur */
/* NOTE: le Mediateur ne coupe pas exactement a la hauteur indiquee par */
/* l'editeur, pour eviter de couper une ligne de texte en deux, */
/* par exemple. RealPageHeight est la hauteur de coupure de */
/* page effectivement utilisee par le Mediateur. */


/* Boites de haut de page dont la creation a ete differee */
EXPORT int  PageFooterHeight; /* hauteur bas */
EXPORT int     	NbBoxesPageHeaderToCreate;
EXPORT PtrElement      PageHeaderRefAssoc;

EXPORT PtrElement 	WorkingPage;
EXPORT PtrPRule 	PageCreateRule;
EXPORT PtrPSchema 	PageSchPresRule;


EXPORT boolean		RunningPaginate;
