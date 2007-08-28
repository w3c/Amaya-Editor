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
 *  Description:  - AmayaNormalWindow is a top container
 *                  + A normal window contains :
 *                     - several AmayaPage
 *                     - toolbar
 *                     - menubar
 *                     - statusbar
 * 
 * +[AmayaNormalWindow]-----------------------------------+
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
 * || AmayaStatusBar                                     ||
 * |+----------------------------------------------------+|
 * +------------------------------------------------------+
 *       Author:  Stephane GULLY
 *      Created:  29/06/2004 04:45:34 PM CET
 *     Revision:  none
 * =====================================================================================
 */
class AmayaNormalWindow : public AmayaWindow
{
 public:
  DECLARE_CLASS(AmayaNormalWindow)

  AmayaNormalWindow (  wxWindow * parent, wxWindowID id=wxID_ANY
                      ,const wxPoint& pos  = wxDefaultPosition
                      ,const wxSize&  size = wxDefaultSize
                      ,int kind = WXAMAYAWINDOW_NORMAL);
  virtual ~AmayaNormalWindow();

  virtual AmayaPage *  GetActivePage() const;
  virtual AmayaFrame * GetActiveFrame() const;
  virtual void         CleanUp();

  // --------------------------------------------- //
  // WXAMAYAWINDOW_NORMAL interface
  virtual AmayaPage *    CreatePage( bool attach = false, int position = 0 );
  virtual bool           AttachPage( int position, AmayaPage * p_page );
  virtual bool           DetachPage( int position );
  virtual bool           ClosePage( int position );
  virtual bool           CloseAllButPage( int position );
  virtual AmayaPage *    GetPage( int position ) const;
  virtual int            GetPageCount() const;

  virtual AmayaToolBar * GetAmayaToolBar();
  virtual AmayaStatusBar * GetAmayaStatusBar();
  virtual void SetPageIcon(int page_id, char *iconpath);

  // url bar control
  virtual wxString GetURL();
  virtual void     SetURL ( const wxString & new_url );
  virtual void     AppendURL ( const wxString & new_url );
  virtual void     EmptyURLBar();
  
  void     GotoSelectedURL();

  AmayaToolPanelBar * GetToolPanelBar() const;
  void RefreshToolPanelBar();
  
  bool IsPanelOpened();
  void ClosePanel();
  void OpenPanel();
  virtual void RefreshShowPanelToggleMenu();

 protected:
  DECLARE_EVENT_TABLE()

#ifdef __WXDEBUG__
  void OnMenuClose( wxMenuEvent& event );
  void OnMenuOpen( wxMenuEvent& event );
#endif /* __WXDEBUG__ */

  void OnMenuItem( wxCommandEvent& event );
  void OnMenuHighlight( wxMenuEvent& event );
  void OnSplitterUnsplit( wxSplitterEvent& event );
  void OnSplitterDClick( wxSplitterEvent& event );
  void OnSplitterPosChanged( wxSplitterEvent& event );
  void OnSplitPanelButton( wxCommandEvent& event );
  void OnNotebookPageChanged( wxNotebookEvent& event );

  void OnClose(wxCloseEvent& event);
  void OnSize(wxSizeEvent& event);
  
  void OnURLTextEnter( wxCommandEvent& event );
  void OnURLSelected( wxCommandEvent& event );
  void OnURLText( wxCommandEvent& event );

  AmayaToolPanelBar *      m_pPanel;     // current selected panel
  wxComboBox  *     m_pComboBox;            // URL combo box
  
  AmayaNotebook *   m_pNotebook;         // tabs container
  int               m_SlashPos;          // the slash pos in pixel

  wxMenuItem * m_pMenuItemToggleFullScreen;
  wxMenuItem * m_pMenuItemToggleToolTip;

  wxSplitterWindow * m_pSplitterWindow;  
  AmayaToolBar *     m_pToolBar;

  AmayaQuickSplitButton * m_pSplitPanelButton;
  AmayaStatusBar * m_pStatusBar;
  
  wxSizer* m_pLayoutSizer;
};

#endif // __AMAYANORMALWINDOW_H__

#endif /* #ifdef _WX */
