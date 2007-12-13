#ifdef _WX

#ifndef AMAYAADVANCEDWINDOW_H__
#define AMAYAADVANCEDWINDOW_H__

#include "AmayaNormalWindow.h"
#include "wx/aui/aui.h"
#include "wx/hashmap.h"

#include "windowtypes_wx.h"

class AmayaAdvancedNotebook;

WX_DECLARE_HASH_MAP(int, AmayaToolPanel*, wxIntegerHash, wxIntegerEqual, AmayaAdvanceToolPanelMap);


/*
 * =====================================================================================
 *        Class:  AmayaAdvancedWindow
 * 
 *  Description:  - AmayaAdvancedWindow is a top container
 *       Author:  Emilien KIA
 *      Created:  03/10/2007
 *     Revision:  
 * =====================================================================================
 */
class AmayaAdvancedWindow : public AmayaNormalWindow
{
 public:
  DECLARE_CLASS(AmayaAdvancedWindow)

AmayaAdvancedWindow (  wxWindow * parent, wxWindowID id=wxID_ANY
                      ,const wxPoint& pos  = wxDefaultPosition
                      ,const wxSize&  size = wxDefaultSize
                      ,int kind = WXAMAYAWINDOW_NORMAL);

  virtual ~AmayaAdvancedWindow();

  virtual bool Initialize();
  virtual void CleanUp();
  
  virtual void LoadConfig();
  virtual void SaveConfig();
  
  // --------------------------------------------- //
  // WXAMAYAWINDOW_NORMAL interface
  virtual void SetPageIcon(int page_id, char *iconpath);

  virtual void UpdateToolPanelLayout();
  virtual AmayaToolPanel* GetToolPanel(int kind);
  
  virtual bool ToolPanelsShown();
  virtual void HideToolPanels();
  virtual void ShowToolPanels();

protected:
  DECLARE_EVENT_TABLE()

  virtual AmayaPageContainer* GetPageContainer();
  virtual const AmayaPageContainer* GetPageContainer()const;
  
  virtual AmayaPage *    CreatePage( Document doc, bool attach = false, int position = 0 );
  
  virtual void ToggleFullScreen();
  
  virtual bool RegisterToolPanel(AmayaToolPanel* tool);

  void OnClose(wxCloseEvent& event);

  bool IsToolBarShown(int toolbarID);
  void ShowToolBar(int toolbarID, bool bShow=true);

private:
  void OnNotebookPageChanged( wxNotebookEvent& event );
  void OnSize(wxSizeEvent& event);
  
  void OnToggleToolPanelMenu(wxCommandEvent&);
  void OnUpdateToolPanelMenu(wxUpdateUIEvent&);

  wxAuiManager   m_manager;
  AmayaAdvancedNotebook* m_notebook;
  
  AmayaAdvanceToolPanelMap m_panels;
  AmayaAdvanceToolPanelMap m_panelMenus;
  
  bool     m_bShowPanels;
  wxString m_strPanelPerspective;
  
};

#endif /*AMAYAADVANCEDWINDOW_H__*/
#endif /* #ifdef _WX */
