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

  AmayaToolBar ( wxWindow * p_parent = NULL, AmayaWindow * amaya_window_parent = NULL );
  virtual ~AmayaToolBar();

  /* tools methodes */
  void EnableTool( const wxString & xrc_id, bool enable = true );

  /* url bar methodes */
  void     ClearURL();
  void     AppendURL( const wxString & newurl );
  void     SetURLValue( const wxString & newurl );
  wxString GetURLValue();
  void     GotoSelectedURL();

 protected:
  DECLARE_EVENT_TABLE()
  void OnButton_Back( wxCommandEvent &event );
  void OnButton_Forward( wxCommandEvent &event );
  void OnButton_Reload( wxCommandEvent &event );
  void OnButton_Stop( wxCommandEvent &event );
  void OnButton_Home( wxCommandEvent &event );
  void OnButton_Save( wxCommandEvent &event );
  void OnButton_Print( wxCommandEvent &event );
  void OnButton_Find( wxCommandEvent &event );
  void OnButton_Logo( wxCommandEvent &event );
  void OnURLTextEnter( wxCommandEvent& event );
  void OnURLSelected( wxCommandEvent& event );
  void OnURLText( wxCommandEvent& event );
  
  AmayaWindow * m_pAmayaWindowParent;
  wxComboBox *  m_pComboBox;
};

#endif // __AMAYATOOLBAR_H__

#endif /* #ifdef _WX */
