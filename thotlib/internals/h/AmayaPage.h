#ifdef _WX

#ifndef __AMAYAPAGE_H__
#define __AMAYAPAGE_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/panel.h"
    #include "wx/splitter.h"
#endif

class AmayaFrame;

/*
 * =====================================================================================
 *        Class:  AmayaPage
 * 
 *  Description:  - a page contains 1 or 2 frames which are splited
 *                into a wxSplitterWindow
 *                - the m_SlashRatio is used to control the percentage givent to the
 *                top and bottom areas.
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
 *      Created:  12/10/2003 04:42:56 PM CET
 *     Revision:  none
 * =====================================================================================
 */

class AmayaPage : public wxPanel
{
public:
  AmayaPage( wxWindow * p_parent_window );
  virtual ~AmayaPage();

  AmayaFrame * AttachTopFrame( AmayaFrame * p_frame );
  AmayaFrame * AttachBottomFrame( AmayaFrame * p_frame );
  AmayaFrame * DetachTopFrame( AmayaFrame * p_frame );
  AmayaFrame * DetachBottomFrame( AmayaFrame * p_frame );
  
  void OnSplitterPosChanged( wxSplitterEvent& event );
  void OnSplitterDClick( wxSplitterEvent& event );
  void OnSplitterUnsplit( wxSplitterEvent& event );
  
  void OnSize( wxSizeEvent& event );
  void OnClose( wxCloseEvent& event );

 protected:
  DECLARE_EVENT_TABLE()

  wxSplitterWindow * m_pSplitterWindow;
  AmayaFrame *       m_pTopFrame;
  AmayaFrame *       m_pBottomFrame;  

  float              m_SlashRatio; // 0.5 => page is half splitted
};

#endif // __AMAYAPAGE_H__

#endif /* #ifdef _WX */
