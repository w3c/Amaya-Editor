#ifdef _WX

#ifndef __AMAYAPAGE_H__
#define __AMAYAPAGE_H__

#include "wx/wx.h"
#include "wx/panel.h"
#include "wx/splitter.h"


class AmayaFrame;
class AmayaNotebook;

/*
 *  Description:  - a page contains 1 or 2 frames which are splited
 *                into a wxSplitterWindow
 *                - the m_SlashRatio is used to control the percentage givent to the
 *                top and bottom areas.
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

class AmayaPage : public wxPanel
{
public:
  AmayaPage( wxWindow * p_parent_window = NULL );
  virtual ~AmayaPage();

  AmayaFrame * AttachTopFrame( AmayaFrame * p_frame );
  AmayaFrame * AttachBottomFrame( AmayaFrame * p_frame );
  AmayaFrame * DetachTopFrame( AmayaFrame * p_frame );
  AmayaFrame * DetachBottomFrame( AmayaFrame * p_frame );
  
  bool IsClosed();
  bool IsSelected();
  void SetSelected( bool isSelected );
//  wxString GetPageTitle();
  
  void SetNotebookParent( AmayaNotebook * p_notebook );
  AmayaNotebook * GetNotebookParent();
  
  AmayaWindow * GetWindowParent();
  void SetWindowParent( AmayaWindow * p_window );
  
  void SetPageId( int page_id );
  int GetPageId();

  void OnSplitterPosChanged( wxSplitterEvent& event );
  void OnSplitterDClick( wxSplitterEvent& event );
  void OnSplitterUnsplit( wxSplitterEvent& event );
  
  void OnSize( wxSizeEvent& event );
  void OnClose( wxCloseEvent& event );
  void OnPaint( wxPaintEvent& event );

 protected:
  DECLARE_EVENT_TABLE()

  wxSplitterWindow * m_pSplitterWindow;
  AmayaFrame *       m_pTopFrame;
  AmayaFrame *       m_pBottomFrame;
  AmayaNotebook *    m_pNoteBookParent;
  AmayaWindow *      m_pWindowParent;
  
  float              m_SlashRatio; // 0.5 => page is half splitted

  int                m_PageId;
  bool               m_IsClosed;
//  bool               m_IsSelected;
};

#endif // __AMAYAPAGE_H__

#endif /* #ifdef _WX */
