#ifdef _WX

#ifndef __AMAYAFRAME_H__
#define __AMAYAFRAME_H__

#include "wx/wx.h"
#include "wx/panel.h"

class AmayaCanvas;
class AmayaPage;
class AmayaWindow;
class AmayaContext;
class wxGLContext;

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
  DECLARE_DYNAMIC_CLASS(AmayaFrame)

  AmayaFrame(  int            frame_id = -1
      	      ,wxWindow *     p_parent_window = NULL
	    );
  virtual ~AmayaFrame( );

  int GetFrameId();
  void RaiseFrame();

  wxScrollBar * GetScrollbarH();
  wxScrollBar * GetScrollbarV();
  void ShowScrollbar( int scrollbar_id );
  void HideScrollbar( int scrollbar_id );  

  void SetStatusBarText( const wxString & text );

  void     SetWindowTitle( const wxString & window_name );
  wxString GetWindowTitle();
  
  void     SetPageTitle(const wxString & page_name);
  wxString GetPageTitle();

  void        SetPageParent( AmayaPage * p_page );
  AmayaPage * GetPageParent();

  AmayaWindow * GetWindowParent();

  AmayaCanvas * GetCanvas();

  wxMenuBar * GetMenuBar();
  static wxMenuItem * AppendMenuItem ( wxMenu * 		p_menu_parent,
				       long 		        id,
				       const wxString & 	label,
				       const wxString & 	help,
				       wxItemKind 		kind,
				       const AmayaContext &    context );
  static wxMenuItem * AppendSubMenu ( wxMenu * 		p_menu_parent,
				  long                 id,
				  const wxString & 	label,
				  const wxString & 	help );

  void SetActive( bool active = TRUE );
  bool IsActive();

  void DestroyFrame();

  AmayaCanvas * CreateDrawingArea();
  void ReplaceDrawingArea( AmayaCanvas * p_new_canvas );

#ifdef _GL
  void SetCurrent();
  void SwapBuffers();
#endif // #ifdef _GL

  void SetNewParent( wxWindow * p_new_parent );

  void OnScrollLineUp( wxScrollEvent& event );
  void OnScrollLineDown( wxScrollEvent& event );
  void OnScroll( wxScrollEvent& event );
  void OnMouse( wxMouseEvent& event );
  void OnSize( wxSizeEvent& event );
  void OnClose(wxCloseEvent& event);
  void OnMenuItem( wxCommandEvent& event );
  void OnIdle( wxIdleEvent& event );

protected:
  DECLARE_EVENT_TABLE()
  
  int          m_FrameId;        // amaya frame id
  bool         m_IsActive;
  bool         m_ToDestroy;
  wxString     m_PageTitle;
  wxString     m_WindowTitle;
  wxString     m_StatusBarText;

  int          m_HOldPosition;
  int          m_VOldPosition;
  
  AmayaPage *     m_pPageParent;  
  AmayaCanvas *   m_pCanvas;

  static wxGLContext * m_pSharedContext;

  //wxFlexGridSizer * m_pFlexSizer;
  wxBoxSizer * m_pHSizer;
  wxBoxSizer * m_pVSizer;
  
  wxScrollBar *   m_pScrollBarH;
  wxScrollBar *   m_pScrollBarV;

  wxMenuBar *     m_pMenuBar;
};

#endif // __AMAYAFRAME_H__

#endif /* #ifdef _WX */


