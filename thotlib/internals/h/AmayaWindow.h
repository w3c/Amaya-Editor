#ifdef _WX

#ifndef __AMAYAWINDOW_H__
#define __AMAYAWINDOW_H__

#include "wx/wx.h"
#include "wx/frame.h"
#include "wx/tooltip.h"
#include "wx/fontmap.h"
#include "wx/strconv.h"
#include "wx/wfstream.h"
#include "wx/menu.h"
#include "wx/splitter.h"


//#include "AmayaCallback.h"
class AmayaPanel;
class AmayaPage;
class AmayaFrame;
class AmayaNotebook;
class AmayaCParam;
class AmayaURLBar;
class AmayaToolBar;

#define MAX_DOC 50

/*
 * =====================================================================================
 *        Class:  AmayaWindow
 * 
 *  Description:  - AmayaWindow is the top container
 *                  + A window can contains several documents.
 *                  + Each document is placed into a page.
 *                  + A page is a "tab" placed in
 *                  a "notebook" widget.
 *                  + A page can be splited in 2 parts, each parts
 *                  represent a document's view (AmayaFrame).
 *                  + A frame is a OpenGL canvas with its own
 *                  scrollbars.
 * 
 * +[AmayaWindow]-----------------------------------------+
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
 *      Created:  12/10/2003 04:45:34 PM CET
 *     Revision:  none
 * =====================================================================================
 */
class AmayaWindow : public wxFrame
{
public:
//  friend class AmayaApp;
  DECLARE_DYNAMIC_CLASS(AmayaWindow)

  AmayaWindow ( int             window_id = -1
      		,wxWindow *     frame = NULL
	        ,const wxPoint& pos  = wxDefaultPosition
	        ,const wxSize&  size = wxDefaultSize
	      );
  virtual ~AmayaWindow();

  AmayaPage * CreatePage( bool attach = false, int position = 0 );
  bool AttachPage( int position, AmayaPage * p_page );
  bool DetachPage( int position );
  AmayaPage * GetPage( int position ) const;
  int GetPageCount() const;
  bool IsClosing();

  AmayaToolBar * GetAmayaToolBar();

  void DesactivateMenuBar();
  void ActivateMenuBar();

  void AppendMenu ( wxMenu * p_menu, const wxString & label );
  void AppendMenuItem ( 
	wxMenu * 		p_menu_parent,
	long                    id,
	const wxString & 	label,
	const wxString & 	help,
	wxItemKind 		kind,
	const AmayaCParam &     callback );
 
  void OnClose( wxCloseEvent& event );
  void OnToolBarTool( wxCommandEvent& event );
  void OnMenuItem( wxCommandEvent& event );

//  void OnSplitterPosChanged( wxSplitterEvent& event );
//  void OnSplitterDClick( wxSplitterEvent& event );
  void OnSplitterUnsplit( wxSplitterEvent& event );
  
  void OnSize( wxSizeEvent& event );
  
  // url bar control
  wxString GetURL();
  void     SetURL ( const wxString & new_url );
  void     AppendURL ( const wxString & new_url );
  void     EmptyURLBar();
  void     SetEnableURL( bool urlenabled );

  int		GetWindowId() { return m_WindowId; }
  void          SetWindowId( int window_id ) { m_WindowId = window_id; }

  AmayaPage * GetActivePage() const;
  AmayaFrame * GetActiveFrame() const;

  void SetupURLBar();

  void SetMenuBar( wxMenuBar * p_menu_bar );

 public:

 protected:
//    AmayaCallback menuCallback;
//    AmayaCallback toolbarCallback;

 protected:
  DECLARE_EVENT_TABLE()
 
  int               m_WindowId;          // amaya window id
  int               m_DocsId[MAX_DOC];    // documents contained by this window
  
//  AmayaPanel *      m_aPanels[4];        // avalaible panels
  AmayaPanel *      m_pCurrentPanel;     // current selected panel
  AmayaNotebook *   m_pNotebook;         // tabs container
  float             m_SlashRatio; // 0.5 => page is half splitted  
  bool              m_IsClosing;

  bool         m_IsFullScreenEnable;
  bool         m_IsToolTipEnable;
  wxMenuItem * m_pMenuItemToggleFullScreen;
  wxMenuItem * m_pMenuItemToggleToolTip;

  wxMenuBar * m_pDummyMenuBar;
  
  wxSplitterWindow * m_pSplitterWindow;
  
  AmayaURLBar *	     m_pURLBar;
  AmayaToolBar *     m_pToolBar;


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
