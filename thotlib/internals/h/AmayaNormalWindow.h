#ifdef _WX

#ifndef __AMAYANORMALWINDOW_H__
#define __AMAYANORMALWINDOW_H__

#include "AmayaWindow.h"
#include "windowtypes_wx.h"

class AmayaQuickSplitButton;
class AmayaPageContainer;
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
  
  static int GetNormalWindowCount(){return s_normalWindowCount;}
  
  static void RegisterThotToolPanels();
  
  AmayaNormalWindow (  wxWindow * parent, wxWindowID id=wxID_ANY
                      ,const wxPoint& pos  = wxDefaultPosition
                      ,const wxSize&  size = wxDefaultSize
                      ,int kind = WXAMAYAWINDOW_NORMAL);
  virtual ~AmayaNormalWindow();

  // Init and config :
  virtual void CleanUp();
  virtual bool Initialize();
  virtual void LoadConfig();
  virtual void SaveConfig();

  // Window decorations :
  virtual AmayaStatusBar * CreateStatusBar();
  virtual void             CreateMenuBar();

  
  virtual AmayaPage *  GetActivePage() const;
  virtual AmayaFrame * GetActiveFrame() const;


  void Unused();

  
  // --------------------------------------------- //
  // WXAMAYAWINDOW_NORMAL interface
  virtual AmayaPage *    CreatePage( Document doc, bool attach = false, int position = 0 )=0;

  // url bar control
  virtual wxString GetURL();
  virtual void     SetURL ( const wxString & new_url );
  virtual void     AppendURL ( const wxString & new_url );
  virtual void     EmptyURLBar();
  
  void     GotoSelectedURL(ThotBool noreplace);
  virtual void RefreshShowToolPanelToggleMenu();
  virtual void SendDataToPanel(int panel_type, AmayaParams& params);
  virtual void RaisePanel(int panel_type);
  virtual void RaiseDoctypePanels(int doctype);
  
  virtual bool AttachPage( int position, AmayaPage * p_page );
  virtual bool ClosePage( int position );
  virtual bool CloseAllButPage( int position );
  virtual AmayaPage *    GetPage( int position ) const;
  virtual int            GetPageCount() const;

  virtual void PrepareRecentDocumentMenu(wxMenuItem* item);
  void OnRecentDocMenu(wxCommandEvent& event);
  
protected:
  DECLARE_EVENT_TABLE()

#ifdef __WXDEBUG__
  void OnMenuClose( wxMenuEvent& event );
  void OnMenuOpen( wxMenuEvent& event );
#endif /* __WXDEBUG__ */

  virtual AmayaPage* DoCreatePage( wxWindow* parent, Document doc, bool attach = false, int position = 0 );
  
  virtual const AmayaPageContainer* GetPageContainer()const{return NULL;}
  virtual AmayaPageContainer* GetPageContainer(){return NULL;}
  
  wxPanel* GetToolBarEditing();
  wxPanel* GetToolBarBrowsing();

  const wxPanel* GetToolBarEditing()const {return m_pToolBarEditing;}
  const wxPanel* GetToolBarBrowsing()const {return m_pToolBarBrowsing;}

  bool HaveToolBarEditing()const{return m_haveTBEditing;}
  bool HaveToolBarBrowsing()const{return m_haveTBBrowsing;}
  
  virtual void ToggleFullScreen();  
  virtual bool RegisterToolPanel(AmayaToolPanel* tool) {return false;}
  
  void OnMenuItem( wxCommandEvent& event );
  void OnMenuHighlight( wxMenuEvent& event );
  void OnURLTextEnter( wxCommandEvent& event );
  void OnURLSelected( wxCommandEvent& event );
  void OnURLText( wxCommandEvent& event );
  
  void OnClose(wxCloseEvent& event);

  virtual void RefreshShowToolBarToggleMenu(int toolbarID);
  
  wxPanel                 *m_pToolBarEditing;
  wxPanel                 *m_pToolBarBrowsing;
  
  bool                    m_haveTBEditing, m_haveTBBrowsing;
  
  wxComboBox              *m_pComboBox;         // URL combo box
  
  wxArrayString           m_URLs;
  wxString                m_enteredURL; // Setted URL, used when no combo is create (amaya lite). 

  static int s_normalWindowCount;   

};

#endif // __AMAYANORMALWINDOW_H__

#endif /* #ifdef _WX */
