#ifdef _WX

#ifndef __AMAYANOTEBOOK_H__
#define __AMAYANOTEBOOK_H__

#include "wx/wx.h"
#include "wx/notebook.h"


class AmayaWindow;
class AmayaPage;

/*
 *  Description:  - AmayaNotebook is a AmayaPage container
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

class AmayaNotebook : public wxNotebook
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaNotebook)

  AmayaNotebook( wxWindow * p_parent_window = NULL,
		 AmayaWindow * p_amaya_window = NULL );
  virtual ~AmayaNotebook( );
 
  int GetPageId( const AmayaPage * p_page );
  AmayaWindow * GetWindowParent()
    { return m_pAmayaWindow; } 

  void UpdatePageId();

  // OnClose must be public because AmayaNormalWindow call it
  void OnClose(wxCloseEvent& event);

 protected:
  DECLARE_EVENT_TABLE()
  void OnPageChanged(wxNotebookEvent& event);
  void OnPageChanging(wxNotebookEvent& event);
  void OnChar( wxKeyEvent& event );
  void OnContextMenu( wxContextMenuEvent & event );
  void OnContextMenuItem( wxCommandEvent& event );

 protected:
  AmayaWindow *  m_pAmayaWindow; // amaya parent window
};

#endif // __AMAYANOTEBOOK_H__

#endif /* #ifdef _WX */


