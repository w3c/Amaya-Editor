#ifdef _WX

#ifndef __AMAYAPANEL_H__
#define __AMAYAPANEL_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/panel.h"
#endif

/*
 *  Description:  - AmayaPanel contains a panel (bookmarks, elemets, attributes ...)
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

class AmayaPanel : public wxPanel
{
public:
  AmayaPanel(   // int            panel_id
      		wxWindow *     p_parent_window = NULL
		,wxWindowID     id = -1
	        ,const wxPoint& pos  = wxDefaultPosition
	        ,const wxSize&  size = wxDefaultSize
		,long style = wxTAB_TRAVERSAL
	      );
  virtual ~AmayaPanel();

 protected:
//  DECLARE_EVENT_TABLE()
  int          m_PanelId;        // amaya panel id
  int          m_ParentWindowId; // amaya parent window id

};

#endif // __AMAYAPANEL_H__

#endif /* #ifdef _WX */

