/*
 *
 *  (c) COPYRIGHT INRIA, 2003-2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * Handling of paths with OpenGL
 * Authors: P. Cheyrou-lagreze (INRIA)
 */

#include <stdio.h>
#include <string.h>

#ifdef _WINGUI
#include <windows.h>
#endif /* _WINGUI */

#ifdef _WX
#include "wx/wx.h"
#if defined (_MACOS) && defined (_WX)
#include <glu.h>
#else /* _MACOS */
#include <GL/glu.h>
#endif /* _MACOS */
#else /* _WX */
#ifdef _GL
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#endif /* _WX */

/*win32 GLU special*/
#ifndef CALLBACK
#define CALLBACK
#endif


/*macro defining thotlib mechanism*/
#define TtaGetMemory(A) malloc(A)
#define TtaFreeMemory(A) free(A)
#define TtaExpandMemory(A,B) realloc(A,B)


/* initial number of allocated points*/
#define ALLOC_POINTS 100

#include "ustring.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "thot_gui.h"
#include "typeint.h"

#ifdef _GL
/*----------------------------------------------------------------------
  myGL_Err : prints out GL errors during tesselation
  ----------------------------------------------------------------------*/
void CALLBACK myGL_Err (GLenum errCode, ThotPath *path) 
{
  if(errCode != GL_NO_ERROR)
    {
      printf ("\n%s : points :%i Contours : %i", 
              (char*) gluErrorString (errCode),
              path->nsize, path->cont);      
    }
}

/*----------------------------------------------------------------------
  myCombine : Store New points computed by the tesselation mechanism.
  ----------------------------------------------------------------------*/
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
      ptr->next = (Mesh_list *)TtaGetMemory (sizeof (Mesh_list));
      ptr = ptr->next;
    }
  else
    {
      path->mesh_list = (Mesh_list *)TtaGetMemory (sizeof (Mesh_list));
      ptr = path->mesh_list;      
    }   
  ptr->next = NULL;
  ptr->data[0] = coords[0];
  ptr->data[1] = coords[1];
  ptr->data[2] = 0.0f;
  *dataOut = ptr->data;
}
#endif /* _GL */

/*----------------------------------------------------------------------
  MeshNewPoint : Allocate a new struct describing a path
  ----------------------------------------------------------------------*/
void *GetNewMesh ()
{
#ifdef _GL
  ThotPath        *path;
  int             c;
  
  path = (ThotPath*) TtaGetMemory (sizeof(ThotPath));
  path->maxpoints = ALLOC_POINTS; 
  path->maxcont = ALLOC_POINTS; 
  c = path->maxpoints * sizeof (ThotDblePoint); 
  path->npoints = (ThotDblePoint*) TtaGetMemory (c); 
  memset (path->npoints, 0, c);   
  c = path->maxcont * sizeof(int); 
  path->ncontour = (int *) TtaGetMemory (c);
  memset (path->ncontour, 0, c);   
  path->nsize = 0;    
  path->cont = 0;    
  path->mesh_list = NULL;  
  return ((void *) path);  
#else
  return NULL;
#endif /* _GL */
}

/*----------------------------------------------------------------------
  MeshNewPoint : Add a point in the allocated struct
  ----------------------------------------------------------------------*/
void MeshNewPoint (double x, double y, void *v_path)
{
#ifdef _GL
  ThotDblePoint       *tmp;
  int             size;
  ThotPath        *path;
  double          xd, yd;

  tmp = NULL;
  path = (ThotPath *) v_path;  
  /* ignore identical points */
  xd = x;
  yd = y;

  if (path->nsize >= (path->maxpoints-1))
    {
      size = path->maxpoints + ALLOC_POINTS;
      tmp = (ThotDblePoint*) TtaExpandMemory (path->npoints, size * sizeof(ThotDblePoint));
      if (tmp == 0)
        return;
      else
        {
          path->npoints = tmp;
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
/*----------------------------------------------------------------------
  CountourCountAdd : Add a flag indicating that the next point 
  will not be connected with this one
  ----------------------------------------------------------------------*/
void CountourCountAdd (void *v_path)
{
#ifdef _GL
  int             size;
  ThotPath        *path;
  int             *tmp;

  tmp = NULL;
  path = (ThotPath *) v_path;
  if (path->nsize > 0)
    {
      if (path->cont >= (path->maxcont-1))
        {
          size = path->maxcont + ALLOC_POINTS;
          tmp = (int*) TtaExpandMemory (path->ncontour, size * sizeof(int));
          if (tmp == 0)
            return;
          else
            {
              path->ncontour = tmp;
              path->maxcont = size;
            }
        }
      path->ncontour[path->cont] = path->nsize;
      (path->cont)++;
    }  
#endif /* _GL */
}

/*----------------------------------------------------------------------
  FreeMesh : Free Allocated resources
  ----------------------------------------------------------------------*/
void FreeMesh (void *v_path)
{
#ifdef _GL 
  ThotPath        *path;
  Mesh_list       *ptr, *tmp;

  if (v_path)
    {      
      path = (ThotPath *) v_path;  
      TtaFreeMemory (path->npoints);
      TtaFreeMemory (path->ncontour);
      ptr = path->mesh_list;
      while (ptr)
        {
          tmp = ptr->next;
          TtaFreeMemory (ptr);
          ptr = tmp;
        }
      TtaFreeMemory (path);
    }  
#endif /* _GL */
}
/*----------------------------------------------------------------------
  MakeMeshLines : Display path outline 
  ----------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------
  MakeMesh : Tesselate and display path using GLU library (part of OpenGL)
  mode = 0 (GLU_TESS_WINDING_NONZERO), 1 (GLU_TESS_WINDING_ODD)
  ----------------------------------------------------------------------*/
void MakeMesh (void *v_path, int mode)
{
#ifdef _GL
  GLUtesselator *tobj = NULL;
  int           c, p;
  double        *d;     
  ThotPath      *path;

  p = 0;  
  c = 0;
  path = (ThotPath *) v_path;  
  tobj = gluNewTess();   
  if (tobj) 
    {
      /* Winding possibilities are :
         GLU_TESS_WINDING_ODD (= Classique) GLU_TESS_WINDING_NONZERO
         GLU_TESS_WINDING_POSITIVE          GLU_TESS_WINDING_NEGATIVE
         GLU_TESS_WINDING_ABS_GEQ_TWO */ 
      gluTessCallback (tobj, GLU_TESS_BEGIN,     (void (CALLBACK*)()) glBegin);
      gluTessCallback (tobj, GLU_TESS_END,       (void (CALLBACK*)()) glEnd);  
      gluTessCallback (tobj, GLU_TESS_VERTEX,    (void (CALLBACK*)()) glVertex2dv);
      gluTessCallback (tobj, GLU_TESS_COMBINE_DATA,   (void (CALLBACK*)()) myCombine);
      gluTessCallback (tobj, GLU_TESS_ERROR_DATA,     (void (CALLBACK*)()) myGL_Err); 
      gluTessProperty (tobj, GLU_TESS_BOUNDARY_ONLY, 0); 
      //gluTessProperty (tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);
      if (mode == 0)
        gluTessProperty (tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_NONZERO);
      else
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
    }
#endif /* _GL */
}
/*---------------------------------------------------------------
  MakefloatMesh : use this module for computing polygons of points
  mode = 0 (GLU_TESS_WINDING_NONZERO), 1 (GLU_TESS_WINDING_ODD)
  ----------------------------------------------------------------------*/
void MakefloatMesh (ThotPoint *points, int npoints, int mode)
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
  MakeMesh (mesh, mode);
  FreeMesh (mesh);  
#endif /*_GL*/
}
