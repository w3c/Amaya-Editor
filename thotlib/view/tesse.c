#ifdef _GL
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif /* _GL */

/*win32 special*/
#ifndef CALLBACK
#define CALLBACK
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define ALLOC_POINTS 100

#define TtaGetMemory(A) malloc(A)
#define TtaFreeMemory(A) malloc(A)
#define TtaExpandMemory(A) calloc(A)

typedef struct _ThotPoint {
  float  x;
  float  y; 
} ThotPoint;

typedef struct _ThotDblePoint {
  double         x;
  double         y; 
  double         z; 
} ThotDblePoint;

typedef struct _Mesh {
  double          data[3];
  struct _Mesh   *next;
} Mesh_list;

typedef struct _ThotPath {
  ThotDblePoint      *npoints;
  int            *ncontour;
  Mesh_list      *mesh_list;
  int            nsize;
  int            maxpoints;
  int            maxcont;  
  int            cont;
  int            height;
} ThotPath;

#ifdef _GL

void CALLBACK myGL_Err (GLenum errCode, ThotPath *path) 
{
  if(errCode != GL_NO_ERROR)
    {
      printf ("\n%s :", (char*) gluErrorString (errCode));      
    }
}

void CALLBACK myCombine (GLdouble coords[3], 
			 void *vertex_data[4], 
			 GLfloat weight[4], 
			 void **dataOut,
			 ThotPath *path)
{
  Mesh_list *ptr;
  
  if (path->mesh_list)
    {
      ptr = path->mesh_list;
      while (ptr->next) 
	ptr = ptr->next;
      ptr->next = TtaGetMemory (sizeof (Mesh_list));
      ptr = ptr->next;
    }
  else
    {
      path->mesh_list = TtaGetMemory (sizeof (Mesh_list));
      ptr = path->mesh_list;      
    }   
  ptr->next = NULL;
  ptr->data[0] = coords[0];
  ptr->data[1] = coords[1];
  ptr->data[2] = 0.0f;
  *dataOut = ptr->data;
}
#endif /* _GL */

void *GetNewMesh ()
{
#ifdef _GL
  ThotPath        *path;
  int             c;
  
  path = malloc (sizeof(ThotPath));
  path->maxpoints = ALLOC_POINTS; 
  path->maxcont = ALLOC_POINTS; 
  c = path->maxpoints * sizeof(ThotDblePoint); 
  path->npoints = TtaGetMemory (c); 
  memset (path->npoints, 0, c);   
  c = path->maxcont * sizeof(int); 
  path->ncontour = TtaGetMemory (c);
  memset (path->ncontour, 0, c);   
  path->nsize = 0;    
  path->cont = 0;    
  path->mesh_list = NULL;  
  return ((void *) path);  
#endif /* _GL */
}

void MeshNewPoint (float x, float y, void *v_path)
{
#ifdef _GL
  ThotDblePoint       *tmp;
  int             size;
  ThotPath        *path;
  double          xd, yd;
  
  path = (ThotPath *) v_path;  
  /* ignore identical points */
  xd = (double) x;
  yd = (double) y;
  if (path->nsize > 0 && 
      path->npoints[path->nsize - 1].x == xd &&
      path->npoints[path->nsize - 1].y == yd)
      if (!(path->cont > 0 && 
	    path->nsize == path->ncontour[path->cont - 1]))
      return;
  if (path->nsize >= path->maxpoints)
    {
      size = path->maxpoints + ALLOC_POINTS;
      if ((tmp = (ThotDblePoint*)realloc(path->npoints, size * sizeof(ThotDblePoint))) ==0)
	return;
      else
	{
	  path->npoints = tmp;
	  memset (path->npoints + path->maxpoints, 0, ALLOC_POINTS);  
	  path->maxpoints = size;
	}
    }
  path->npoints[path->nsize].x = xd;
  path->npoints[path->nsize].y = yd;
  path->npoints[path->nsize].z = 0.0f;
  (path->nsize)++;
  return;
#endif /* _GL */
}

void CountourCountAdd (void *v_path)
{
#ifdef _GL
  int             size;
  ThotPath        *path;
  int             *tmp;
  
  path = (ThotPath *) v_path;  
  if (path->nsize > 0)
    {      
      if (path->cont >= path->maxcont)
	{
	  size = path->maxcont + ALLOC_POINTS;
	  if ((tmp = (int*) realloc(path->npoints, size * sizeof(int))) == 0)
	    return;
	  else
	    {
	      path->ncontour = tmp;
	      memset (path->ncontour + path->maxcont, 0, ALLOC_POINTS);  
	      path->maxcont = size;
	    }
	}
      path->ncontour[path->cont] = path->nsize;
      (path->cont)++;
    }  
#endif /* _GL */
}


void FreeMesh (void *v_path)
{
#ifdef _GL 
  ThotPath        *path;
  Mesh_list       *ptr, *tmp;

  if (v_path)
    {      
      path = (ThotPath *) v_path;  
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
    }  
#endif /* _GL */
}


void MakeMesh (void *v_path)
{
#ifdef _GL
  GLUtesselator *tobj;
  int           c, p;
  double        *d;     
  ThotPath      *path;
  
  p = 0;  
  c = 0;
  path = (ThotPath *) v_path;  
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
  gluTessCallback (tobj, GLU_TESS_COMBINE_DATA,   (void (CALLBACK*)()) myCombine);
  gluTessCallback (tobj, GLU_TESS_ERROR_DATA,     (void (CALLBACK*)()) myGL_Err); 
  gluTessProperty (tobj, GLU_TESS_BOUNDARY_ONLY, 0); 
  /* gluTessProperty (tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE); */
  gluTessProperty (tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
  gluTessProperty (tobj, GLU_TESS_TOLERANCE, 0);
  gluTessNormal (tobj, 0.0f, 0.0f, 1.0f);
  gluTessBeginPolygon (tobj, path);  
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
#endif /* _GL */
}

void MakeMeshLines  (void *v_path)
{
#ifdef _GL  
  ThotPath *path;
  int      c, p;
  
  path = (ThotPath *) v_path;    
  c = p = 0;  
  while (c < path->cont)
    {
      glBegin (GL_LINE_STRIP);
      while (p < path->ncontour[c])
	{
	  glVertex2dv ((double *) &(path->npoints[p].x));
	  p++;
	}   
      glEnd ();
      c++;      
    }  
#endif /*_GL*/
}

void MakefloatMesh (ThotPoint *points, int npoints)
{
#ifdef _GL  
  void     *mesh;
  int      c, p;
  
  c = p = 0;  
  mesh = GetNewMesh ();
  while (c < npoints)
      {
	  MeshNewPoint (points[c].x, points[c].y, mesh);
	  c++;	  
      }  
  CountourCountAdd (mesh);  
  MakeMesh (mesh);
  FreeMesh (mesh);  
#endif /*_GL*/
}
