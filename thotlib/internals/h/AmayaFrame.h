#ifdef _WX

#ifndef __AMAYAFRAME_H__
#define __AMAYAFRAME_H__

#include "wx/wx.h"
#include "wx/panel.h"

class AmayaCanvas;

/*
 * =====================================================================================
 *        Class:  AmayaFrame
 * 
 *  Description:  - AmayaFrame is contained by a AmayaPage
 *  		  - it contains a AmayaCanvas (opengl) and 2 scrollbars (1 H and 1 V)
 * 
 * +[AmayaWindow]-------------------------------------+
 * |+------------------------------------------------+|
 * || MenuBar                                        ||
 * |+------------------------------------------------+|
 * |+------------------------------------------------+|
 * || ToolBar                                        ||
 * |+------------------------------------------------+|
 * |+[AmayaPanel]--+ +[AmayaPage]-------------------+ |
 * ||              | |[AmayaFrame]                  | |
 * ||              | |                              | |
 * ||              | |    (view container)  'Top'   | |
 * ||              | |-----------SplitBar-----------| |
 * ||              | |[AmayaFrame]          'Bottom'| |
 * ||              | |                              | |
 * |+--------------+ +------------------------------+ |
 * |+------------------------------------------------+|
 * || StatusBar                                      ||
 * |+------------------------------------------------+|
 * +--------------------------------------------------+
 *       Author:  Stephane GULLY
 *      Created:  12/10/2003 04:45:34 PM CET
 *     Revision:  none
 * =====================================================================================
 */
class AmayaFrame : public wxPanel
{
public:
  AmayaFrame(  int            frame_id
      	      ,wxWindow *     p_parent_window = NULL
	    );
  virtual ~AmayaFrame( );

  inline int GetFrameId() { return m_FrameId; }
  inline wxScrollBar * GetScrollbarH() { return m_pScrollBarH; }
  inline wxScrollBar * GetScrollbarV() { return m_pScrollBarV; }
   
#ifdef _GL
  void SetCurrent();
  void SwapBuffers();
#endif // #ifdef _GL
  
  void OnScroll( wxScrollEvent& event );

protected:
  DECLARE_EVENT_TABLE()
  
  int          m_FrameId;        // amaya frame id

  AmayaCanvas *   m_pCanvas;

  wxFlexGridSizer * m_pFlexSizer;
  wxScrollBar *   m_pScrollBarH;
  wxScrollBar *   m_pScrollBarV;
};

#endif // __AMAYAFRAME_H__

#endif /* #ifdef _WX */


