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
 * || StatusBar                                          ||
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
  DECLARE_DYNAMIC_CLASS(AmayaNormalWindow)

  AmayaNormalWindow ( int             window_id = -1
		      ,wxWindow *     frame = NULL
		      ,const wxPoint& pos  = wxDefaultPosition
		      ,const wxSize&  size = wxDefaultSize );
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
  virtual AmayaPage *    GetPage( int position ) const;
  virtual int            GetPageCount() const;

  virtual AmayaToolBar * GetAmayaToolBar();

  virtual void           SetMenuBar( wxMenuBar * p_menu_bar );
  virtual void           DesactivateMenuBar();
  virtual void           ActivateMenuBar();
  virtual void AppendMenu ( wxMenu * p_menu, const wxString & label );
  virtual void AppendMenuItem ( wxMenu * 		p_menu_parent,
				long                    id,
				const wxString & 	label,
				const wxString & 	help,
				wxItemKind 		kind,
				const AmayaCParam &     callback );

  // url bar control
  virtual wxString GetURL();
  virtual void     SetURL ( const wxString & new_url );
  virtual void     AppendURL ( const wxString & new_url );
  virtual void     EmptyURLBar();
  virtual void     SetEnableURL( bool urlenabled );
  virtual void     SetupURLBar();

  AmayaPanel * GetAmayaPanel() const;
  bool IsPanelOpened();
  void ClosePanel();
  void OpenPanel();
  
 protected:
  void OnClose( wxCloseEvent& event );
  void OnMenuItem( wxCommandEvent& event );
  void OnMenuClose( wxMenuEvent& event );
  void OnMenuOpen( wxMenuEvent& event );
  void OnSplitterUnsplit( wxSplitterEvent& event );
  void OnSplitterDClick( wxSplitterEvent& event );
  void OnSplitterPosChanged( wxSplitterEvent& event );
  void OnSplitPanelButton( wxCommandEvent& event );

 protected:
  DECLARE_EVENT_TABLE()
  
  AmayaPanel *      m_pPanel;     // current selected panel
  AmayaNotebook *   m_pNotebook;         // tabs container
  wxPanel *         m_pNotebookPanel;
  int               m_SlashPos;          // the slash pos in pixel

  bool         m_IsFullScreenEnable;
  bool         m_IsToolTipEnable;

  wxMenuItem * m_pMenuItemToggleFullScreen;
  wxMenuItem * m_pMenuItemToggleToolTip;

  wxMenuBar *        m_pDummyMenuBar;
  wxSplitterWindow * m_pSplitterWindow;  
  AmayaURLBar *	     m_pURLBar;
  AmayaToolBar *     m_pToolBar;

  AmayaQuickSplitButton * m_pSplitPanelButton;
};

#endif // __AMAYANORMALWINDOW_H__

#endif /* #ifdef _WX */
