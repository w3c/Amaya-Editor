#ifdef _WX

#ifndef __AMAYAFRAME_H__
#define __AMAYAFRAME_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/panel.h"
#endif

class AmayaCanvas;
class AmayaPage;

/*
 *  Description:  - AmayaFrame is contained by a AmayaPage
 *  		  - it contains a AmayaCanvas (opengl) and 2 scrollbars (1 H and 1 V)
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

class AmayaFrame : public wxPanel
{
public:
  AmayaFrame(  int            frame_id
      	      ,wxWindow *     p_parent_window = NULL
	    );
  virtual ~AmayaFrame( );

  int GetFrameId();

  wxScrollBar * GetScrollbarH();
  wxScrollBar * GetScrollbarV();
  void ShowScrollbar( int scrollbar_id );
  void HideScrollbar( int scrollbar_id );  

  void     SetWindowTitle( const wxString & window_name );
  wxString GetWindowTitle();
  
  void     SetPageTitle(const wxString & page_name);
  wxString GetPageTitle();

  void        SetPageParent( AmayaPage * p_page );
  AmayaPage * GetPageParent();
  
#ifdef _GL
  void SetCurrent();
  void SwapBuffers();
#endif // #ifdef _GL
  
  void OnScroll( wxScrollEvent& event );
  void OnMouse( wxMouseEvent& event );
  void OnSize( wxSizeEvent& event );

protected:
  DECLARE_EVENT_TABLE()
  
  int          m_FrameId;        // amaya frame id
  wxString     m_PageTitle;
  wxString     m_WindowTitle;
 
  AmayaPage *  m_pPageParent;
  
  AmayaCanvas *   m_pCanvas;

  //wxFlexGridSizer * m_pFlexSizer;
  wxBoxSizer * m_pHSizer;
  wxBoxSizer * m_pVSizer;
  
  wxScrollBar *   m_pScrollBarH;
  wxScrollBar *   m_pScrollBarV;
};

#endif // __AMAYAFRAME_H__

#endif /* #ifdef _WX */


