#ifdef _WX

#ifndef __AMAYACANVAS_H__
#define __AMAYACANVAS_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #ifdef _GL
      #include "wx/glcanvas.h"
    #else // #ifdef _GL
      #include "wx/panel.h"
    #endif // #ifdef _GL
#endif

class AmayaFrame;

#ifdef _GL
class AmayaCanvas : public wxGLCanvas
#else // #ifdef _GL
class AmayaCanvas : public wxPanel
#endif // #ifdef _GL
{
public:
  AmayaCanvas( AmayaFrame * p_parent_window );
  virtual ~AmayaCanvas( );

  void OnSize( wxSizeEvent& event );
  void OnPaint( wxPaintEvent& event );
  
  void OnMouse( wxMouseEvent& event );
  void OnChar( wxKeyEvent& event );
  void OnKeyDown( wxKeyEvent& event );
   
  void OnInit( wxInitDialogEvent& event );
  void OnActivate( wxActivateEvent& event );
  void OnClose( wxCloseEvent& event );
 
  void OnIdle( wxIdleEvent& event );
  
protected:
  DECLARE_EVENT_TABLE()
  
  AmayaFrame *  m_pAmayaFrame;  // amaya frame reference (parent)

  ThotBool m_Selecting; 

  bool m_Init;
  void Init();
  void Render();
//  bool m_init; 
  
};

#endif // __AMAYACANVAS_H__

#endif /* #ifdef _WX */



