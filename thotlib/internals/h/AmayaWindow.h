#ifdef _WX

#ifndef __AMAYAWINDOW_H__
#define __AMAYAWINDOW_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/frame.h"
    #include "wx/tooltip.h"
    #include "wx/fontmap.h"
    #include "wx/strconv.h"
    #include "wx/wfstream.h"
    #include "wx/menu.h"
    #include "wx/splitter.h"
#endif

#include "AmayaCallback.h"
class AmayaPanel;
class AmayaPage;
class AmayaFrame;

//#include "wx/dnd.h"

//#include "DropFileEvent.h"

//class AmayaGLCanvas;
//class TestUnicodeDialogue;
class  AmayaSplitterWindow;


/*
 * =====================================================================================
 *        Class:  AmayaWindow
 * 
 *  Description:  - AmayaWindow is the top container
 * 
 * +[AmayaWindow]-------------------------------------+
 * |+------------------------------------------------+|
 * || MenuBar                                        ||
 * |+------------------------------------------------+|
 * |+------------------------------------------------+|
 * || ToolBar                                        ||
 * |+------------------------------------------------+|
 * |+[AmayaPanel]--+ +[AmayaPage]-------------------+ |
 * ||              | |[AmayaFrame]                  | |
 * ||              | |                              | |
 * ||              | |    (view container)  'Top'   | |
 * ||              | |-----------SplitBar-----------| |
 * ||              | |[AmayaFrame]          'Bottom'| |
 * ||              | |                              | |
 * |+--------------+ +------------------------------+ |
 * |+------------------------------------------------+|
 * || StatusBar                                      ||
 * |+------------------------------------------------+|
 * +--------------------------------------------------+
 *       Author:  Stephane GULLY
 *      Created:  12/10/2003 04:45:34 PM CET
 *     Revision:  none
 * =====================================================================================
 */
class AmayaWindow : public wxFrame
{
public:
//  friend class AmayaApp;

  AmayaWindow ( int             window_id
      		,wxWindow *     frame = NULL
	        ,const wxPoint& pos  = wxDefaultPosition
	        ,const wxSize&  size = wxDefaultSize
	      );
  virtual ~AmayaWindow();

  bool AttachFrame( AmayaFrame * p_frame );
  bool DetachFrame( AmayaFrame * p_frame );
  
  void AppendMenu ( wxMenu * p_menu, const wxString & label );
  void AppendMenuItem ( 
	wxMenu * 		p_menu_parent,
	long 		id,
	const wxString & 	label,
	const wxString & 	help,
	wxItemKind 		kind,
	const AmayaCParam & callback );
 
  void OnClose( wxCloseEvent& event );
  void OnToolBarTool( wxCommandEvent& event );
  void OnMenuItem( wxCommandEvent& event );

//  void OnSplitterPosChanged( wxSplitterEvent& event );
//  void OnSplitterDClick( wxSplitterEvent& event );
  void OnSplitterUnsplit( wxSplitterEvent& event );
  
//  void OnSize( wxSizeEvent& event );
 

  

  void SetURL ( const wxString & new_url );
  void AppendURL ( const wxString & new_url );

//  wxMenuBar *   getMenuBar() { return m_pMenuBar; }
//  
  int		GetWindowId() { return m_WindowId; }
  void          SetWindowId( int window_id ) { m_WindowId = window_id; }

//  inline wxToolBar *   GetToolBar() { return m_pToolBar; }
//  inline wxStatusBar * GetStatusBar() { return m_pStatusBar; }
   
 protected:
    AmayaCallback menuCallback;
    AmayaCallback toolbarCallback;

 protected:
  DECLARE_EVENT_TABLE()
   
  int               m_WindowId;          // amaya window id
//  AmayaPanel *      m_aPanels[4];        // avalaible panels
  AmayaPanel *      m_pCurrentPanel;     // current selected panel
//  AmayaNotebook *   m_pNotebook;         // tabs container
  float             m_SlashRatio; // 0.5 => page is half splitted  
  AmayaPage *       m_pPage;
  
  bool         m_IsFullScreenEnable;
  bool         m_IsToolTipEnable;
  wxMenuItem * m_pMenuItemToggleFullScreen;
  wxMenuItem * m_pMenuItemToggleToolTip;

  wxMenuBar * m_pMenuBar;
  wxMenu * m_pFileMenu;
  wxMenu * m_pViewMenu;
  wxMenu * m_pInfoMenu;
  
  wxSplitterWindow * m_pSplitterWindow;
  
  wxComboBox *		m_pURLBar;
  //   AmayaGLCanvas *	m_pGLCanvas;
  wxStatusBar *        m_pStatusBar;
  wxToolBar *          m_pToolBar;

//  wxLocale & m_Locale;
 public:
  enum
   {
     TOOLBAR_TOOL_START,
     TOOLBAR_TOOL_END = TOOLBAR_TOOL_START+100,

     MENU_ITEM_START,
     MENU_ITEM_END = MENU_ITEM_START+100,

   };
};

#endif // __AMAYAWINDOW_H__

#endif /* #ifdef _WX */
