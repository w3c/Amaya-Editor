#ifdef _WX

#ifndef __AMAYACANVAS_H__
#define __AMAYACANVAS_H__

#include "wx/wx.h"
#ifdef _GL
  #include "wx/glcanvas.h"
#else // #ifdef _GL
  #include "wx/panel.h"
#endif // #ifdef _GL

class AmayaFrame;

/*
 *  Description:  - AmayaCanvas is the OpenGL container
 *                  it is contained by a AmayaFrame (document's view)
 *  
 *                  + A window can contains several documents.
 *                  + Each document is placed into a page.
 *                  + A page is a "tab" placed in
 *                  a "notebook" widget.
 *                  + A page can be splited in 2 parts, each parts
 *                  represent a document's view (AmayaFrame).
 *                  + A frame is a OpenGL canvas with its own
 *                  scrollbars.
 * 
 * +[AmayaWindow]-----------------------------------------+
 * |+----------------------------------------------------+|
 * || MenuBar                                            ||
 * |+----------------------------------------------------+|
 * |+----------------------------------------------------+|
 * || ToolBar                                            ||
 * |+----------------------------------------------------+|
 * |+[AmayaPanel]--+ +[AmayaNoteBook]-------------------+ |
 * ||              | |+-----------+                     | |
 * ||              | ||[AmayaPage]+--------------------+| |
 * ||              | ||+------------------------------+|| |
 * ||              | |||[AmayaFrame]                  ||| |
 * ||              | |||                              ||| |
 * ||              | |||                              ||| |
 * ||              | |||  (view container)     'Top'  ||| |
 * ||              | |||---------SplitBar-------------||| |
 * ||              | |||[AmayaFrame]          'Bottom'||| |
 * ||              | |||                              ||| |
 * ||              | |||                              ||| |
 * ||              | ||+------------------------------+|| |
 * ||              | |+--------------------------------+| |
 * |+--------------+ +----------------------------------+ |
 * |+----------------------------------------------------+|
 * || StatusBar                                          ||
 * |+----------------------------------------------------+|
 * +------------------------------------------------------+
 *       Author:  Stephane GULLY
 *      Created:  12/10/2003 04:45:34 PM CET
 *     Revision:  none
*/

#ifdef _GL
class AmayaCanvas : public wxGLCanvas
#else // #ifdef _GL
class AmayaCanvas : public wxPanel
#endif // #ifdef _GL
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaCanvas)

#ifdef _GL
  AmayaCanvas( AmayaFrame * p_parent_window = NULL,
	       wxGLContext * p_shared_context = NULL );
#else /* _GL */
  AmayaCanvas( AmayaFrame * p_parent_window = NULL );
#endif /* _GL */
  virtual ~AmayaCanvas( );

  bool IsParentPageActive();
  bool IsInit();

  void OnSize( wxSizeEvent& event );
  void OnPaint( wxPaintEvent& event );
  
  void OnMouse( wxMouseEvent& event );
  void OnChar( wxKeyEvent& event );
  void OnKeyDown( wxKeyEvent& event );
    
  void OnIdle( wxIdleEvent& event );
  
protected:
  DECLARE_EVENT_TABLE()
  
  AmayaFrame *  m_pAmayaFrame;  // amaya frame reference (parent)

  bool m_Init;
  void Init();
  void Render();
//  bool m_init; 
  
};

#endif // __AMAYACANVAS_H__

#endif /* #ifdef _WX */



