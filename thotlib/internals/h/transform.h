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
 
/* interface pour le stretch des pixmaps */


Pixmap StretchPixmap( /* image, new_w, new_h */ );
    /* int  new_w, new_h; */
    /* XImage     *image; */

void GetStretchSize( /* PicWArea, PicHArea, wif, hif, pres, &presW, &presH */);
