/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
/*
 * Handles Opengl Fonts with Freetype 2 
 * (http://www.freetype.org)
 * ( Here we make glyph to antialiased bitmap font )
 * Based on FTGLIB a very good C++ lib that handles fonts in opengl
 * ( http://homepages.paradise.net.nz/henryj/code/index.html )
 * 
 * Author: P. Cheyrou-lagreze (INRIA)
 *
 */
 
#ifdef _GL

#ifdef _WX
  #include "wx/wx.h"
#endif /* _WX */

#define THOT_EXPORT extern
#include "thot_gui.h"
#include "constmedia.h"
#include "typemedia.h"
#include "typebase.h"
/*For ttagetmemory and free*/
#include "memory_f.h"
/*for ttafileexists*/
#include "fileaccess.h"
#include "openglfont.h"
#include "font_f.h"
#include "glwindowdisplay.h"

#ifdef _SUPERS
/* 
   Supposed to give 
   better antialiasing
   but give more aliased chars..
*/
#define SUPERSAMPLING(X)  (X / 2)
#define SUPERSAMPLINGMUL(X) (X * 2)
#else
#define SUPERSAMPLING(X)  (X)
#define SUPERSAMPLINGMUL(X) (X)
#endif /*_SUPERS*/

#define ANTIALIASINGDEPTH 0
#if defined (_MACOS) && defined (_WX)
#include <glu.h>
#else /* _MACOS */
#include <GL/glu.h>
#endif /* _MACOS */

/*win32 special*/
#ifndef CALLBACK
#define CALLBACK
#endif

#define kBSTEPSIZE   0.2f
#define ALLOC_POINTS 100


/*ft_kerning_unscaled*/
/*ft_kerning_default,*/
/*ft_kerning_unfitted*/
#define KERNING_CHOICE  ft_kerning_default

typedef struct _GlyphThotPoint {
  double         x;
  double         y; 
} GlyphThotPoint;

typedef struct _GlyphMesh {
  double         data[2];
  struct _GlyphMesh   *next;
} GlyphMesh_list;


typedef struct _GlyphPath {
  GlyphThotPoint      *npoints;
  int                 *ncontour;
  int                 nsize;
  int                 maxpoints;
  int                 cont;
  int                 height;  
  GlyphMesh_list      *mesh_list;
} GlyphPath;


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void CALLBACK GlyphmyGL_Err (GLenum errCode, GlyphPath *path) 
{
  if(errCode != GL_NO_ERROR)
    printf ("\n%s :", (char*) gluErrorString (errCode));      
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void CALLBACK GlyphmyCombine (GLdouble coords[3], void *vertex_data[4], 
			 GLfloat weight[4], void **dataOut, GlyphPath *path)
{
  GlyphMesh_list *ptr = path->mesh_list;
  
  while (ptr->next) 
    ptr = ptr->next;
  ptr->next = (GlyphMesh_list*)TtaGetMemory (sizeof (GlyphMesh_list));
  ptr = ptr->next;
  ptr->next = 0;
  ptr->data[0] = (double) coords[0];
  ptr->data[1] = (double) coords[1];
  *dataOut = ptr->data;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static int GlyphPolyNewPoint (FT_Pos x, FT_Pos y, GlyphPath *path)
{
  GlyphThotPoint       *tmp;
  int             size;

  if (path->nsize >= path->maxpoints)
    {
      size = path->maxpoints + ALLOC_POINTS;
      if ((tmp = (GlyphThotPoint*)realloc (path->npoints, size * sizeof(GlyphThotPoint))) == 0)
	return (FALSE);
      else
	{
	  path->npoints = tmp;
	  memset (path->npoints + path->maxpoints, 0,
		  ALLOC_POINTS * sizeof(GlyphThotPoint));  
	  path->maxpoints = size;
	}
    }
  path->npoints[path->nsize].x = (double) ((double) x / 64.0);
  path->npoints[path->nsize].y = ((double) (path->height)) - 
    ((double)((double) y / 64.0));
  (path->nsize)++;
  return (1);
}

/*----------------------------------------------------------------------
 De Casteljau algorithm contributed by Jed Soane
  ----------------------------------------------------------------------*/
static void deCasteljau (const float t, const int n, GlyphPath *path, 
			 float bValues[4][4][2])
{
  int i, k;
  
  /* Calculating successive b(i)'s using de Casteljau algorithm. */
  for (i = 1; i <= n; i++)
    for (k = 0; k <= (n - i); k++)
      {
	bValues[i][k][0] = (1 - t) * bValues[i - 1][k][0] + 
	                         t * bValues[i - 1][k + 1][0];
	bValues[i][k][1] = (1 - t) * bValues[i - 1][k][1] + 
	                         t * bValues[i - 1][k + 1][1];
      }        
  /* Specify next vertex to be included on curve */
  GlyphPolyNewPoint ((long int)bValues[n][0][0], (long int)bValues[n][0][1], path);
}


/*----------------------------------------------------------------------
 De Casteljau algorithm contributed by Jed Soane
  ----------------------------------------------------------------------*/
static void evaluateCurve (const int n, GlyphPath *path, float ctrlPtArray[4][2])
{
  int   i, k;
  float t; /* parameter for curve point calc. [0.0, 1.0] */
  float bValues[4][4][2];  /* 3D array storing values of de Casteljau algorithm. */

  /* setting the b(0) equal to the control points */
  for (i = 0; i <= n; i++)
    {
      bValues[0][i][0] = ctrlPtArray[i][0];
      bValues[0][i][1] = ctrlPtArray[i][1];
    }
  k = (int)(1.0f / kBSTEPSIZE);
  for (i = 0; i <= k; i++)
    {
      t = i * kBSTEPSIZE;
      deCasteljau (t, n, path, bValues);  /* calls to evaluate point on curve att. */
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static int Conic (const int index, const int first, const int last,
		  FT_Outline ftOutline, GlyphPath *path)
{
  int   next, next2, prev;
  float x, y;
  float ctrlPtArray[4][2]; /* Magic numbers */
  
  next = index + 1;
  prev = index - 1;
  if (index == last)
    next = first;     
  if (index == first)
    prev = last;     
  if (ftOutline.tags[next] != FT_Curve_Tag_Conic)
    {
      ctrlPtArray[0][0] = ftOutline.points[prev].x;   
      ctrlPtArray[0][1] = ftOutline.points[prev].y;
      ctrlPtArray[1][0] = ftOutline.points[index].x;  
      ctrlPtArray[1][1] = ftOutline.points[index].y;
      ctrlPtArray[2][0] = ftOutline.points[next].x;   
      ctrlPtArray[2][1] = ftOutline.points[next].y;        
      evaluateCurve (2, path, ctrlPtArray);
      return 1;
    }
  else
    {
      next2 = next + 1;
      if (next == last)
	next2 = first;        
      /* create a phantom point */
      x = (ftOutline.points[index].x + ftOutline.points[next].x) / 2;
      y = (ftOutline.points[index].y + ftOutline.points[next].y) / 2;        
      /* process first curve */
      ctrlPtArray[0][0] = ftOutline.points[prev].x;   
      ctrlPtArray[0][1] = ftOutline.points[prev].y;
      ctrlPtArray[1][0] = ftOutline.points[index].x;  
      ctrlPtArray[1][1] = ftOutline.points[index].y;
      ctrlPtArray[2][0] = x;                          
      ctrlPtArray[2][1] = y;        
      evaluateCurve (2, path, ctrlPtArray);        
      /* process second curve */
      ctrlPtArray[0][0] = x;                          
      ctrlPtArray[0][1] = y;
      ctrlPtArray[1][0] = ftOutline.points[next].x;   
      ctrlPtArray[1][1] = ftOutline.points[next].y;
      ctrlPtArray[2][0] = ftOutline.points[next2].x;  
      ctrlPtArray[2][1] = ftOutline.points[next2].y;
      evaluateCurve (2, path, ctrlPtArray);        
      return 2;
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static int Cubic (const int index, const int first, const int last,
		  FT_Outline ftOutline, GlyphPath *path)
{ 
  int   next, next2, prev;
  float ctrlPtArray[4][2]; /*  Magic numbers */
  
  next = index + 1;
  prev = index - 1;    
  if (index == last)
    next = first;     
  next2 = next + 1;    
  if (next  == last)
    next2 = first;    
  if (index == first)
    prev = last;     
  ctrlPtArray[0][0] = ftOutline.points[prev].x;       
  ctrlPtArray[0][1] = ftOutline.points[prev].y;
  ctrlPtArray[1][0] = ftOutline.points[index].x;      
  ctrlPtArray[1][1] = ftOutline.points[index].y;
  ctrlPtArray[2][0] = ftOutline.points[next].x;       
  ctrlPtArray[2][1] = ftOutline.points[next].y;
  ctrlPtArray[3][0] = ftOutline.points[next2].x;      
  ctrlPtArray[3][1] = ftOutline.points[next2].y;
  evaluateCurve (3, path, ctrlPtArray);
  return 2;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void GlyphMakeMesh (GlyphPath *path, int contourFlag)
{
  GLUtesselator *tobj;
  int           c, p;
  double        *d;     
  
  tobj = gluNewTess();    
  /* Winding possibilities are :
     GLU_TESS_WINDING_ODD = Classique
     GLU_TESS_WINDING_NONZERO
     GLU_TESS_WINDING_POSITIVE
     GLU_TESS_WINDING_NEGATIVE
     GLU_TESS_WINDING_ABS_GEQ_TWO */ 

  gluTessCallback (tobj, GLU_TESS_BEGIN,     (void (CALLBACK*)()) glBegin);
  gluTessCallback (tobj, GLU_TESS_END,       (void (CALLBACK*)()) glEnd);  
  gluTessCallback (tobj, GLU_TESS_VERTEX,    (void (CALLBACK*)()) glVertex2dv);

  gluTessCallback (tobj, GLU_TESS_COMBINE_DATA,   (void (CALLBACK*)()) GlyphmyCombine);
  gluTessCallback (tobj, GLU_TESS_ERROR_DATA,     (void (CALLBACK*)()) GlyphmyGL_Err);
 
 /*  gluTessProperty (tobj, */
  /* 		   GLU_TESS_BOUNDARY_ONLY, */
  /* 		   1); */
  /*   FT_OUTLINE_EVEN_ODD_FILL   :: By default, outlines are filled using */
  /*                                 the non-zero winding rule.  If set to */
  /*                                 1, the outline will be filled using   */
  /*                                 the even-odd fill rule (only works    */
  /*                                 with the smooth raster).              */
  /*       
   */
  if (contourFlag & FT_OUTLINE_EVEN_ODD_FILL) 
    gluTessProperty (tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
  else
    gluTessProperty (tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);

  /*   FT_OUTLINE_REVERSE_FILL    :: By default, outside contours of an    */
  /*                                 outline are oriented in clock-wise    */
  /*                                 direction, as defined in the TrueType */
  /*                                 specification.  This flag is set if   */
  /*                                 the outline uses the opposite         */
  /*                                 direction (typically for Type 1       */
  /*                                 fonts).  This flag is ignored by the  */
  /*                                 scan-converter.  However, it is very  */
  /*        */
  if (contourFlag & FT_OUTLINE_REVERSE_FILL)
    gluTessProperty (tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NEGATIVE);
  else
    gluTessProperty (tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);
  gluTessProperty (tobj, GLU_TESS_TOLERANCE, 0);
  gluTessNormal (tobj, 0.0f, 0.0f, 1.0f);  
  gluTessBeginPolygon (tobj, path); 
  p = 0;  
  c = 0;
  while (c < path->cont)
    {
      gluTessBeginContour (tobj);            
      while (p < path->ncontour[c])
	{
	  d = (double *) &(path->npoints[p].x);
	  gluTessVertex (tobj, d, d);
	  p++;	  
	}    
      gluTessEndContour (tobj); 
      c++;      
    }        
  gluTessEndPolygon (tobj);
  gluDeleteTess (tobj);
}

/*----------------------------------------------------------------------
  MakePolygonGlyph : Make bitmap and struct to handle the glyph
  ----------------------------------------------------------------------*/
void MakePolygonGlyph (GL_font *font, unsigned int g, GL_glyph *BitmapGlyph)
{
  FT_Outline       ftOutline;
  FT_Glyph         Glyph;
  GlyphPath        *path;
  GlyphMesh_list   *ptr, *tmp;
  int              p, last, first, c;
  GLuint           *glList;
 
  if (g == 0)
    return;    
  if (FT_Load_Glyph (font->face, g, FT_LOAD_DEFAULT))
    return;	
  if (FT_Get_Glyph (font->face->glyph, &Glyph))
    return;    
  if (ft_glyph_format_outline != Glyph->format)
    {
      FT_Done_Glyph (Glyph);
      return;
    }

  BitmapGlyph->data_type = GL_GLYPH_DATATYPE_NONE;
  BitmapGlyph->data = 0;
  BitmapGlyph->advance = 0;
  BitmapGlyph->pos.y = 0;
  ftOutline.n_contours = 0;
  ftOutline.n_points = 0;
  FT_Glyph_Get_CBox (Glyph, ft_glyph_bbox_subpixels, &(BitmapGlyph->bbox)); 
  BitmapGlyph->advance = (int) (Glyph->advance.x >> 16);
  ftOutline = ((FT_OutlineGlyph) Glyph)->outline;

  if (ftOutline.n_contours && ftOutline.n_points)
    {  
      path = (GlyphPath *)malloc (sizeof(GlyphPath));
      memset (path, 0, sizeof(GlyphPath));
      path->cont = ftOutline.n_contours;
      path->maxpoints = (ALLOC_POINTS + ftOutline.n_points); 
      c = path->cont * sizeof(int);
      path->ncontour = (int *)malloc (c);
      memset (path->ncontour, 0, c); 
      c = path->maxpoints * sizeof(GlyphThotPoint);  
      path->npoints = (GlyphThotPoint*)malloc (c); 
      memset (path->npoints, 0, c);   
      path->nsize = 0;    
      path->height = (BitmapGlyph->bbox.yMax - BitmapGlyph->bbox.yMin) >> 6;
      if (BitmapGlyph->bbox.yMin >= 0)
	BitmapGlyph->pos.y = (BitmapGlyph->bbox.yMax) >> 6; 
      else
	BitmapGlyph->pos.y = path->height; 
      first = 0;  
      for (c = 0; c < path->cont; ++c)
	{
	  last = ftOutline.contours[c];
	  for (p = first; p <= last; ++p)
	    {
	      switch (ftOutline.tags[p])
		{
		case FT_CURVE_TAG_CONIC:
		  p += Conic (p, first, last, ftOutline, path);
		  break;
		case FT_CURVE_TAG_CUBIC:
		  p += Cubic (p, first, last, ftOutline, path);
		  break;
		case FT_CURVE_TAG_ON:
		default:
		  GlyphPolyNewPoint (ftOutline.points[p].x, ftOutline.points[p].y, path);
		}
	    }        
	  path->ncontour[c] = path->nsize;
	  first = last + 1;
	}
      path->nsize--;
      path->mesh_list = (GlyphMesh_list *)malloc (sizeof (GlyphMesh_list));
      path->mesh_list->next = NULL;  
      glList = (GLuint *)malloc (sizeof (GLuint));
      *glList = glGenLists (1);
      glNewList (*glList, GL_COMPILE);
      GlyphMakeMesh (path,  ftOutline.flags);  
      glEndList ();
      free (path->npoints);
      free (path->ncontour);
      ptr = path->mesh_list;
      while (ptr)
	{
	  tmp = ptr->next;
	  free (ptr);
	  ptr = tmp;
	}
      free (path);
      BitmapGlyph->data = glList;
      /* datatype is important in order to delete the data with glDeleteLists and not TtaFreeMemory */
      BitmapGlyph->data_type = GL_GLYPH_DATATYPE_GLLIST;
    }
  else
    {
#ifdef _TRACE_GLGLYPH
      printf("MakePolygonGlyph(Warning): the glyph poly is empty (g = %d)\n", g);
#endif /* #ifdef _TRACE_GLGLYPH */
    }
  /*
    BitmapGlyph->pos.x = bitmap->left;
    BitmapGlyph->pos.y = source->rows - bitmap->top;   
    BitmapGlyph->dimension.x = source->width;
    BitmapGlyph->dimension.y = source->rows;  	  
  */
  FT_Done_Glyph (Glyph); 
}

/*----------------------------------------------------------------------
  UnicodeFontRender : Render an unicode string 
  (no more than a word)
  in a Bitmap.
  Using Two step computation :
  1) computes each glyphs position in the word (advance and kerning).
  2) place them in a bitmap.
  ----------------------------------------------------------------------*/
int UnicodeFontRenderPoly (void *gl_font, wchar_t *text, float x, float y, int size)
{
  FT_Vector          delta;   
  FT_Bool            use_kerning;
  FT_UInt            previous, glyph_index;
  GL_font           *font;
  GL_glyph          *glyph;
  Char_Cache_index  *cache;
  register int       n;
  register float     pen_x, pen_y;
  
  if (text == NULL) 
    return 0;  
  font = (GL_font *) gl_font;
  use_kerning = font->kerning;
  previous = 0;
  pen_x = 0;
  glPushMatrix ();
  glTranslatef (x, y, 0.0f);
  n = 0; 
  glyph_index = 0;  
  while (n < size && text[n])
    {
      /* convert character code to glyph index */
      cache = Char_index_lookup_cache (font, text[n], TRUE);
      if (cache)
	{
	  glyph = &(cache->glyph);
	  glyph_index = cache->character;
#ifdef _WX
	  wxASSERT_MSG( !glyph->data ||
			glIsList(*((GLuint *)glyph->data)),
			_T("GLBUG - UnicodeFontRenderPoly : glIsList returns false"));
#endif /* _WX */
	  if (glyph->data && glyph->data_type == GL_GLYPH_DATATYPE_GLLIST)
	    {
	      /* retrieve kerning distance 
		 and move pen position */
	      if (use_kerning && previous && glyph_index)
		{
		  FT_Get_Kerning (font->face, previous, glyph_index,
				  KERNING_CHOICE, &delta);
		  pen_x += delta.x >> 6;
		  /* record current glyph index */
		  previous = glyph_index;
		}
	      pen_y = (float) glyph->pos.y;	      
	      glTranslatef (pen_x, -pen_y, 0.0f);
	      glCallList (*((GLuint *)glyph->data));
	      glTranslatef (-pen_x, pen_y, 0.0f);
	    }
	  else
	    {
#ifdef _TRACE_GLGLYPH
	      printf("UnicodeFontRenderPoly(Error): GL_glyph not created correctly\n");
#endif /* _TRACE_GLGLYPH */
	    }
	  /* increment pen position*/
	  pen_x += glyph->advance;
	}
      n++;      
    }
  glPopMatrix ();
  return ((int)SUPERSAMPLING(pen_x));
}
#endif /* _GL */
