#ifdef _WX

#ifndef __AMAYAFRAME_H__
#define __AMAYAFRAME_H__

#include "wx/wx.h"
#include "wx/panel.h"

//class AmayaTextGraber;
class AmayaCanvas;
class AmayaPage;
class AmayaWindow;
class AmayaContext;
class wxGLContext;
class AmayaScrollBar;

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
      	      ,AmayaWindow *     p_parent_window = NULL
	    );
  virtual ~AmayaFrame( );

  int GetFrameId();
  void RaiseFrame();

  int GetMasterFrameId();

  AmayaScrollBar * GetScrollbarH();
  AmayaScrollBar * GetScrollbarV();
  void ShowScrollbar( int scrollbar_id );
  void HideScrollbar( int scrollbar_id );  

  void SetStatusBarText( const wxString & text );

  void     SetWindowTitle( const wxString & window_name );
  wxString GetWindowTitle();
  
  void     SetFrameTitle(const wxString & frame_name);
  wxString GetFrameTitle();

  void     UpdateFrameURL( const wxString & new_url );
  void     SetFrameURL( const wxString & new_url );
  wxString GetFrameURL();
  void     SetFrameEnableURL( bool urlenabled );
  bool     GetFrameEnableURL( );

  void        SetPageParent( AmayaPage * p_page );
  AmayaPage * GetPageParent();

  AmayaWindow * GetWindowParent();

  AmayaCanvas * GetCanvas();

  bool DisplayIsReady();
  
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
  bool IsMenuActive();

  void FreeFrame();

  AmayaCanvas * CreateDrawingArea();
  void ReplaceDrawingArea( AmayaCanvas * p_new_canvas );

#ifdef _GL
  void SetCurrent();
  void SwapBuffers();
#endif // #ifdef _GL

  void OnClose(wxCloseEvent& event);

protected:
  DECLARE_EVENT_TABLE()
  void OnMouse( wxMouseEvent& event );
  void OnSize( wxSizeEvent& event );
  void OnIdle( wxIdleEvent& event );
  void OnContextMenu( wxContextMenuEvent & event );

  int          m_FrameId;        // amaya frame id
  bool         m_IsActive;
  bool         m_ToDestroy;
  wxString     m_FrameTitle;
  wxString     m_WindowTitle;
  wxString     m_StatusBarText;
  wxString     m_FrameUrl;
  bool         m_FrameUrlEnable;

  int          m_HOldPosition;
  int          m_VOldPosition;
  
  AmayaPage *     m_pPageParent;  
  AmayaCanvas *   m_pCanvas;

  wxBoxSizer * m_pHSizer;
  wxBoxSizer * m_pVSizer;
  
  AmayaScrollBar *   m_pScrollBarH;
  AmayaScrollBar *   m_pScrollBarV;

  //  AmayaTextGraber * m_pTextGraber;

  wxMenuBar *     m_pMenuBar;
};

#endif // __AMAYAFRAME_H__

#endif /* #ifdef _WX */


