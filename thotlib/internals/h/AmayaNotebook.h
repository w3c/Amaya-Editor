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
  DECLARE_CLASS(AmayaNotebook)

  AmayaNotebook( wxWindow * window , wxWindowID id=wxID_ANY);
  virtual ~AmayaNotebook( );
 
  int GetPageId( const AmayaPage * p_page );
  AmayaWindow * GetWindowParent()
    { return (AmayaWindow*)wxGetTopLevelParent(this); } 
  
  int GetMContextFrame();

  void UpdatePageId();
  
  bool ClosePage(int page_id);
  bool CloseAllButPage(int position);

  void CleanUp();

 protected:
  DECLARE_EVENT_TABLE()
#ifdef __WXDEBUG__
  void OnPageChanging(wxNotebookEvent& event);
#endif /* __WXDEBUG__ */
  void OnPageChanged(wxNotebookEvent& event);
  void OnContextMenu( wxContextMenuEvent & event );
  void OnContextMenuItem( wxCommandEvent& event );

  void OnClose(wxCloseEvent& event);

 protected:
  int            m_MContextFrameId;
};

#endif // __AMAYANOTEBOOK_H__

#endif /* #ifdef _WX */


