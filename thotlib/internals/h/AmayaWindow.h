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
#include "wx/notebook.h"

class AmayaPage;
class AmayaFrame;
class AmayaCParam;
class AmayaURLBar;
class AmayaToolBar;
class AmayaToolPanel;
class AmayaStatusBar;

#include "windowtypes_wx.h"
#include "AmayaParams.h"

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
  DECLARE_ABSTRACT_CLASS(AmayaWindow)

  AmayaWindow ( wxWindow *      parent
                ,wxWindowID     id = wxID_ANY
                ,const wxPoint& pos  = wxDefaultPosition
                ,const wxSize&  size = wxDefaultSize
                ,int kind = WXAMAYAWINDOW_UNKNOWN
                ,long style = wxDEFAULT_FRAME_STYLE | wxWANTS_CHARS | wxTAB_TRAVERSAL
                );
  virtual ~AmayaWindow();
  
  static void   DoAmayaAction( int command, int doc, int view );

  // Init and config :
  virtual void CleanUp();
  virtual bool Initialize();
  virtual void LoadConfig();
  virtual void SaveConfig();

  // Window properties :
  int           GetWindowId() { return m_WindowId; }
  void          SetWindowId( int window_id ) { m_WindowId = window_id; }
  int           GetKind() const { return m_Kind; }
  static int    GetActiveWindowId() { return m_ActiveWindowId; }
  static AmayaWindow * GetActiveWindow();

  // Window decorations :
  virtual AmayaStatusBar * GetStatusBar();
  virtual AmayaStatusBar * CreateStatusBar();
  virtual void             CreateMenuBar();

  
  // Page and frame management :
  virtual AmayaPage *  GetActivePage() const;
  virtual AmayaFrame * GetActiveFrame() const;
  virtual AmayaPage *    CreatePage( Document doc, bool attach = false, int position = 0 );
  virtual bool           AttachPage( int position, AmayaPage * p_page );
  virtual bool           DetachPage( int position );
  virtual bool           ClosePage( int position );
  virtual bool           CloseAllButPage( int position );
  virtual AmayaPage *    GetPage( int position ) const;
  virtual int            GetPageCount() const;
  
  

  

  virtual void ToggleFullScreen();
  
  
  // --------------------------------------------- //
  // WXAMAYAWINDOW_NORMAL interface
  virtual void UpdateToolPanelLayout();
  virtual AmayaToolPanel* GetToolPanel(int kind){return NULL;}
  virtual bool ToolPanelsShown();
  virtual void HideToolPanels();
  virtual void ShowToolPanels();
  virtual void RefreshShowToolPanelToggleMenu();
  virtual void SendDataToPanel(int panel_type, AmayaParams& params){}
  virtual void RaisePanel(int panel_type){}
  virtual void RaiseDoctypePanels(int doctype){}


  virtual void SetPageIcon(int page_id, char *iconpath);

  // url bar control
  virtual wxString GetURL();
  virtual void     SetURL ( const wxString & new_url );
  virtual void     AppendURL ( const wxString & new_url );
  virtual void     EmptyURLBar();

  // toolbars
  virtual bool IsToolBarShown(int toolbarID){return false;}
  virtual void ShowToolBar(int toolbarID, bool bShow=true){}
  virtual void HideToolBar(int toolbarID){ShowToolBar(toolbarID, false);}
  
  
  // --------------------------------------------- //
  // WXAMAYAWINDOW_SIMPLE interface
  virtual bool         AttachFrame( AmayaFrame * p_frame );
  virtual AmayaFrame * DetachFrame();

  
 public:
  void OnChar( wxKeyEvent& event );
  
 protected:
  DECLARE_EVENT_TABLE()
  void OnSize( wxSizeEvent& event );
  void OnIdle( wxIdleEvent& event );
  void OnActivate( wxActivateEvent & event );
  void OnAmayaAction( wxCommandEvent& event );
  void OnCloseEvent(wxCloseEvent& event);
  
  void OnPopupMenuEvent(wxCommandEvent& event);

 protected:
  int               m_Kind;               // window kind
  int               m_WindowId;           // amaya window id
  static int        m_ActiveWindowId;
  int               m_ActiveFrameId;
  bool              m_MustCheckFocusIsNotLost;
};

#endif // __AMAYAWINDOW_H__

#endif /* #ifdef _WX */
