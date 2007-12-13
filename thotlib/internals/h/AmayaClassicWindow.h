#ifdef _WX

#ifndef AMAYACLASSICWINDOW_H_
#define AMAYACLASSICWINDOW_H_

#include "AmayaNormalWindow.h"
#include "windowtypes_wx.h"

class AmayaQuickSplitButton;
class AmayaToolPanelBar;
class AmayaClassicNotebook;
class AmayaAdvancedNotebook;

/*
 * =====================================================================================
 *        Class:  AmayaClassicWindow
 * 
 *  Description:  - AmayaClassicWindow is a top container
 *                  + A normal window contains :
 *                     - several AmayaPage
 *                     - toolbar
 *                     - menubar
 *                     - statusbar
 * 
 * +[AmayaClassicWindow]----------------------------------+
 * |+----------------------------------------------------+|
 * || MenuBar                                            ||
 * |+----------------------------------------------------+|
 * |+----------------------------------------------------+|
 * || ToolBar                                            ||
 * |+----------------------------------------------------+|
 * |+[AmayaNoteBook]-------------------+ +[AmayaPanel]--+ |
 * ||+-----------+                     | |              | |
 * |||[AmayaPage]+--------------------+| |              | |
 * |||+------------------------------+|| |              | |
 * ||||[AmayaFrame]                  ||| |              | |
 * ||||                              ||| |              | |
 * ||||                              ||| |              | |
 * ||||  (view container)     'Top'  ||| |              | |
 * ||||---------SplitBar-------------||| |              | |
 * ||||[AmayaFrame]          'Bottom'||| |              | |
 * ||||                              ||| |              | |
 * ||||                              ||| |              | |
 * |||+------------------------------+|| |              | |
 * ||+--------------------------------+| |              | |
 * |+----------------------------------+ +--------------+ |
 * |+----------------------------------------------------+|
 * || AmayaStatusBar                                     ||
 * |+----------------------------------------------------+|
 * +------------------------------------------------------+
 *       Author:  Emilien KIA
 *      Created:  02/10/2007
 *     Revision:  
 * =====================================================================================
 */

class AmayaClassicWindow : public AmayaNormalWindow
{
 public:
  DECLARE_CLASS(AmayaClassicWindow)

AmayaClassicWindow (  wxWindow * parent, wxWindowID id=wxID_ANY
                      ,const wxPoint& pos  = wxDefaultPosition
                      ,const wxSize&  size = wxDefaultSize
                      ,int kind = WXAMAYAWINDOW_NORMAL);
  virtual ~AmayaClassicWindow();

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
  void OnSplitterUnsplit( wxSplitterEvent& event );
  void OnSplitterDClick( wxSplitterEvent& event );
  void OnSplitterPosChanged( wxSplitterEvent& event );
  void OnSplitPanelButton( wxCommandEvent& event );
  void OnNotebookPageChanged( wxNotebookEvent& event );
  void OnSize(wxSizeEvent& event);
  
  AmayaToolPanelBar       *m_pPanel;            // current selected panel
//  AmayaClassicNotebook    *m_pNotebook;         // tabs container
  AmayaAdvancedNotebook    *m_pNotebook;         // tabs container
  
  wxSplitterWindow        *m_pSplitterWindow;
  int                      m_SlashPos;          // the slash pos in pixel
  AmayaQuickSplitButton   *m_pSplitPanelButton;
  
  wxSizer* m_pLayoutSizer;
};


#endif /*AMAYACLASSICWINDOW_H_*/

#endif /* #ifdef _WX */
