#ifdef _WX

#ifndef __AMAYATOOLBAR_H__
#define __AMAYATOOLBAR_H__

#include "wx/panel.h"

class AmayaFrame;
class AmayaWindow;

/*
 * =====================================================================================
 *        Class:  AmayaToolBar
 * 
 *       Author:  Stephane GULLY
 *      Created:  11/05/2004 04:45:34 PM CET
 * =====================================================================================
 */
class AmayaToolBar : public wxPanel
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaToolBar)

  AmayaToolBar ( AmayaWindow * amaya_window_parent = NULL );
  virtual ~AmayaToolBar();

  void OnSize( wxSizeEvent& event );
  void OnButtonActivated( wxCommandEvent &event );

  void AddTool( wxWindow * p_tool, bool expand = FALSE, int border = 0 );
  void AddSeparator();

 protected:
  DECLARE_EVENT_TABLE()
  
  AmayaWindow * m_pAmayaWindowParent;
  wxBoxSizer *  m_pTopSizer;
};

#endif // __AMAYATOOLBAR_H__

#endif /* #ifdef _WX */
