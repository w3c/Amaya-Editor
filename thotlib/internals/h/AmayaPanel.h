#ifdef _WX

#ifndef __AMAYAPANEL_H__
#define __AMAYAPANEL_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/panel.h"
#endif


class AmayaXHTMLPanel;
class AmayaNormalWindow;

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
  DECLARE_DYNAMIC_CLASS(AmayaPanel)
    
  AmayaPanel(   wxWindow *     p_parent_window = NULL
		,AmayaNormalWindow * p_parent_nwindow = NULL
		,wxWindowID     id = -1
	        ,const wxPoint& pos  = wxDefaultPosition
	        ,const wxSize&  size = wxDefaultSize
		,long style = wxTAB_TRAVERSAL
	      );
  virtual ~AmayaPanel();

  void ShowWhenUnsplit( bool show );

  AmayaXHTMLPanel * GetXHTMLPanel() const;
  void RefreshToolTips();

 protected:
  DECLARE_EVENT_TABLE()
  void OnClose( wxCommandEvent& event );

 protected:
  int          m_PanelId;        // amaya panel id
  int          m_ParentWindowId; // amaya parent window id
  AmayaNormalWindow * m_pParentNWindow  ;

  AmayaXHTMLPanel *  m_pPanel_xhtml;
  wxPanel *        m_pTitlePanel;
};

#endif // __AMAYAPANEL_H__

#endif /* #ifdef _WX */

