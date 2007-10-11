#ifdef _WX

#ifndef __AMAYANORMALWINDOW_H__
#define __AMAYANORMALWINDOW_H__

#include "AmayaWindow.h"
#include "windowtypes_wx.h"

class AmayaQuickSplitButton;

/*
 * =====================================================================================
 *        Class:  AmayaNormalWindow
 * 
 *  Description:  - AmayaNormalWindow is the common interface for all normal
 *                  (classic and advaced) windows.
 *                  A normal window can embed many pages, menu, tools, statusbar ...
 *       Author:  Stephane GULLY
 *      Created:  29/06/2004 04:45:34 PM CET
 *     Revision:  Emilien Kia 02/10/2007
 * =====================================================================================
 */
class AmayaNormalWindow : public AmayaWindow
{
 public:
  DECLARE_CLASS(AmayaNormalWindow)

  static AmayaNormalWindow* CreateNormalWindow(wxWindow * parent, wxWindowID id=wxID_ANY
                    ,const wxPoint& pos  = wxDefaultPosition
                    ,const wxSize&  size = wxDefaultSize
                    ,int kind = WXAMAYAWINDOW_NORMAL);
  
  
  AmayaNormalWindow (  wxWindow * parent, wxWindowID id=wxID_ANY
                      ,const wxPoint& pos  = wxDefaultPosition
                      ,const wxSize&  size = wxDefaultSize
                      ,int kind = WXAMAYAWINDOW_NORMAL);
  virtual ~AmayaNormalWindow();

//  virtual AmayaPage *  GetActivePage() const;   // TODO virtualize !!!
  virtual AmayaFrame * GetActiveFrame() const;

  virtual void CleanUp();
  virtual bool Initialize();

  void Unused();

  virtual void LoadConfig();
  virtual void SaveConfig();
  
  // --------------------------------------------- //
  // WXAMAYAWINDOW_NORMAL interface
//  virtual wxWindow*      GetPageContainer()const;
  virtual AmayaPage *    CreatePage( bool attach = false, int position = 0 );
//  virtual bool           AttachPage( int position, AmayaPage * p_page );  // TODO virtualize !!!
//  virtual bool           DetachPage( int position );
//  virtual bool           ClosePage( int position );  // TODO virtualize !!!
//  virtual bool           CloseAllButPage( int position );   // TODO virtualize !!!
//  virtual AmayaPage *    GetPage( int position ) const;   // TODO virtualize !!!
//  virtual int            GetPageCount() const;   // TODO virtualize !!!

  virtual AmayaStatusBar * GetAmayaStatusBar();
//  virtual void SetPageIcon(int page_id, char *iconpath);

  // url bar control
  virtual wxString GetURL();
  virtual void     SetURL ( const wxString & new_url );
  virtual void     AppendURL ( const wxString & new_url );
  virtual void     EmptyURLBar();
  
  void     GotoSelectedURL();

//  void RefreshToolPanelBar();
//  virtual AmayaToolPanel* GetToolPanel(int kind);
  
//  virtual bool ToolPanelsShown();
//  virtual void HideToolPanels();
//  virtual void ShowToolPanels();
  virtual void RefreshShowToolPanelToggleMenu();

protected:
  DECLARE_EVENT_TABLE()

#ifdef __WXDEBUG__
  void OnMenuClose( wxMenuEvent& event );
  void OnMenuOpen( wxMenuEvent& event );
#endif /* __WXDEBUG__ */

  wxPanel* GetToolBarEditing();
  wxPanel* GetToolBarBrowsing();

  const wxPanel* GetToolBarEditing()const {return m_pToolBarEditing;}
  const wxPanel* GetToolBarBrowsing()const {return m_pToolBarBrowsing;}

  virtual void ToggleFullScreen();
  
  virtual bool RegisterToolPanel(AmayaToolPanel* tool) {return false;}
  
//  void OnMenuItem( wxCommandEvent& event ); // TODO virtualize !!!
  void OnMenuHighlight( wxMenuEvent& event );
//  void OnSplitterUnsplit( wxSplitterEvent& event );
//  void OnSplitterDClick( wxSplitterEvent& event );
//  void OnSplitterPosChanged( wxSplitterEvent& event );
//  void OnSplitPanelButton( wxCommandEvent& event );
//  void OnNotebookPageChanged( wxNotebookEvent& event );

//  void OnSize(wxSizeEvent& event);
  
  void OnURLTextEnter( wxCommandEvent& event );
  void OnURLSelected( wxCommandEvent& event );
  void OnURLText( wxCommandEvent& event );

//  bool IsToolBarShown(int toolbarID);
//  void ShowToolBar(int toolbarID, bool bShow=true);
  virtual void RefreshShowToolBarToggleMenu(int toolbarID);
  
  AmayaStatusBar          *m_pStatusBar;
  wxPanel                 *m_pToolBarEditing;
  wxPanel                 *m_pToolBarBrowsing;
  wxComboBox              *m_pComboBox;         // URL combo box

  static int s_normalWindowCount;   

};

#endif // __AMAYANORMALWINDOW_H__

#endif /* #ifdef _WX */
