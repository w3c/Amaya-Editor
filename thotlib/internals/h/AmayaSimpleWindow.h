#ifdef _WX

#ifndef __AMAYASIMPLEWINDOW_H__
#define __AMAYASIMPLEWINDOW_H__

#include "AmayaWindow.h"
#include "windowtypes_wx.h"

/*
 * =====================================================================================
 *        Class:  AmayaSimpleWindow
 * 
 *  Description:  - AmayaSimpleWindow is a top container
 *                  + A simple window contains :
 *                      - a AmayaFrame (= a document view)
 *                        ex: Log view
 *                  + Close button
 * 
 * +[AmayaSimpleWindow]----------------------------------+
 * |+[AmayaFrame----------------------------------------+|
 * ||                                                   ||
 * ||                                                   ||
 * ||           OpenGL canvas                           ||
 * ||                                                   ||
 * ||                                                   ||
 * ||                                                   ||
 * ||                                                   ||
 * |+---------------------------------------------------+|
 * |+---------------------------------------------------+|
 * || Buttons                                           ||
 * |+---------------------------------------------------+|
 * +-----------------------------------------------------+
 *       Author:  Stephane GULLY
 *      Created:  29/06/2004 04:45:34 PM CET
 *     Revision:  none
 * =====================================================================================
 */
class AmayaSimpleWindow : public AmayaWindow
{
 public:
  DECLARE_DYNAMIC_CLASS(AmayaSimpleWindow)

  AmayaSimpleWindow ( int             window_id = -1
		      ,wxWindow *     frame = NULL
		      ,const wxPoint& pos  = wxDefaultPosition
		      ,const wxSize&  size = wxDefaultSize );
  virtual ~AmayaSimpleWindow();

  virtual AmayaFrame * GetActiveFrame() const;
  virtual void         CleanUp();

  bool         AttachFrame( AmayaFrame * p_frame );
  AmayaFrame * DetachFrame();

 protected:
  void OnClose( wxCloseEvent& event );
  void OnCloseButton(wxCommandEvent& event);
  void OnSplitterUnsplit( wxSplitterEvent& event );
  void OnSize( wxSizeEvent& event );

 protected:
  DECLARE_EVENT_TABLE()

  AmayaFrame *      m_pFrame;             // for simple window
  wxBoxSizer *      m_pTopSizer;
};

#endif // __AMAYASIMPLEWINDOW_H__

#endif /* #ifdef _WX */
