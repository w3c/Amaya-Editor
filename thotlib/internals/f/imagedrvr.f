
#ifndef __CEXTRACT__
#ifdef __STDC__

extern void SwapAllBits ( register unsigned char *b, register long n );
extern void CentreImage ( int wimage, int himage, int wbox, int hbox, PictureScaling pres, int *xtranslate, int *ytranslate, int *pxorig, int *pyorig );
extern void InitPictureHandlers (boolean printing);
extern void GetImageDriversList (int *count, char *buffer);
extern void DrawImageBox (PtrBox box, ImageDescriptor * imageDesc, int frame, int wlogo, int hlogo);
extern void ReadImage (int frame, PtrBox box, ImageDescriptor * imageDesc );
extern void DrawImage ( PtrBox box, ImageDescriptor * imageDesc, int frame );
extern void EditImage (int frame, PtrBox box, ImageDescriptor * imageDesc );
extern void PrintImage ( int typeImage, char * name, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, FILE * fd, unsigned long BackGroundPixel );
extern void FreeImage (ImageDescriptor *imageDesc );
/* extern void Escape ( int typeImage, char * arg ); */
extern int GetImageType ( int menuIndex );
extern int GetImTypeIndex ( int imageType );
extern int GetImPresIndex ( PictureScaling imagePres );
extern PictureScaling GetImagePresentation ( int menuIndex );
extern void TestPixmap ( Pixmap pix );
extern void usage ( void );
extern int TtaSetMainWindowBackgroundColor(ThotWindow w, int color);

#else /* __STDC__ */

extern void SwapAllBits (/* register unsigned char *b, register long n */);
extern void CentreImage (/* int wimage, int himage, int wbox, int hbox, PictureScaling pres, int *xtranslate, int *ytranslate, int *pxorig, int *pyorig */);
extern void InitPictureHandlers (/*boolean printing*/);
extern void GetImageDriversList (/*int *count, char *buffer*/);
extern void DrawImageBox (/*PtrBox box, ImageDescriptor * imageDesc, int frame, int wlogo, int hlogo*/);
extern void ReadImage (/*int frame, PtrBox box, ImageDescriptor * imageDesc */);
extern void DrawImage (/* PtrBox box, ImageDescriptor * imageDesc, int frame */);
extern void EditImage (/*int frame, PtrBox box, ImageDescriptor * imageDesc */);
extern void PrintImage (/* int typeImage, char * name, PictureScaling pres, int xif, int yif, int wif, int hif, int xcf, int ycf, int wcf, int hcf, FILE * fd, unsigned long BackGroundPixel */);
extern void FreeImage (/* ImageDescriptor *imageDesc */);
extern void Escape (/* int typeImage, char * arg */);
extern int GetImageType (/* int menuIndex */);
extern int GetImTypeIndex (/* int imageType */);
extern int GetImPresIndex (/* PictureScaling imagePres */);
extern PictureScaling GetImagePresentation (/* int menuIndex */);
extern void TestPixmap (/* Pixmap pix */);
extern void usage (/* void */);
extern int TtaSetMainWindowBackgroundColor(/*ThotWindow w, int color*/);

#endif /* __STDC__ */
#endif /* __CEXTRACT__ */
