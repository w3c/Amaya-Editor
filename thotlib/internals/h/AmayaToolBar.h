#ifdef _WX

#ifndef __AMAYATOOLBAR_H__
#define __AMAYATOOLBAR_H__

#include "wx/panel.h"
#include "wx/hashmap.h"


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
  void OnButton_Save_All( wxCommandEvent &event );
  void OnButton_New( wxCommandEvent &event );
  void OnButton_Open( wxCommandEvent &event );
  void OnButton_Print( wxCommandEvent &event );
  void OnButton_Find( wxCommandEvent &event );
  void OnButton_CSS( wxCommandEvent &event );
  void OnButton_Logo( wxCommandEvent &event );
  void OnURLTextEnter( wxCommandEvent& event );
  void OnURLSelected( wxCommandEvent& event );
  void OnURLText( wxCommandEvent& event );
  
  AmayaWindow * m_pAmayaWindowParent;
  wxComboBox *  m_pComboBox;
  
  wxToolBar*  m_LeftToolBar;
  wxToolBar*  m_RightToolBar;
};

typedef struct
{
  const char* idname;
  const char* action;
  int         actionid;
  int tooltip_categ, tooltip_msg;
} AmayaToolBarToolDef;

WX_DECLARE_HASH_MAP( int, AmayaToolBarToolDef*, wxIntegerHash, wxIntegerEqual, AmayaToolBarToolDefHashMap );

#define AMAYA_DECLARE_TOOLBAR_DEF_TABLE(name) AmayaToolBarToolDef name[];
#define AMAYA_BEGIN_TOOLBAR_DEF_TABLE(name) AmayaToolBarToolDef name[]={
#define AMAYA_TOOLBAR_DEF(idname, action, tip_categ, tip_msg) {idname, action, -1, tip_categ, tip_msg},
#define AMAYA_END_TOOLBAR_DEF_TABLE() {NULL, NULL, -1, 0, 0} };

class AmayaBaseToolBar : public wxToolBar
{
  DECLARE_DYNAMIC_CLASS(AmayaToolBarEditing)
  DECLARE_EVENT_TABLE()
public:
  AmayaBaseToolBar();
  bool Create( wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxT("AmayaBaseToolBar") );
  virtual ~AmayaBaseToolBar();

  /** Add number of toolitem definitions. def must end by a NULL row.*/
  void Add(AmayaToolBarToolDef* def);
  
  // Called after all tools have been added.
  virtual bool Realize();

protected:
  AmayaToolBarToolDefHashMap m_map;
  
  void OnTool(wxCommandEvent& event);
  
  void OnUpdate(wxUpdateUIEvent& event);
};


class AmayaToolBarEditing : public AmayaBaseToolBar
{
  DECLARE_DYNAMIC_CLASS(AmayaToolBarEditing)
public:
  
  static AmayaToolBarEditing s_tb;
  
  AmayaToolBarEditing();
};

class AmayaToolBarBrowsing : public AmayaBaseToolBar
{
  DECLARE_DYNAMIC_CLASS(AmayaToolBarBrowsing)
public:
  
  static AmayaToolBarBrowsing s_tb;
  
  AmayaToolBarBrowsing();
};

#endif // __AMAYATOOLBAR_H__

#endif /* #ifdef _WX */
