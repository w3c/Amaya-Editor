#ifdef _WX

#ifndef __AMAYAFRAME_H__
#define __AMAYAFRAME_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/frame.h"
    #include "wx/tooltip.h"
    #include "wx/fontmap.h"
    #include "wx/strconv.h"
    #include "wx/wfstream.h"
    #include "wx/menu.h"
#endif

#include "AmayaCallback.h"

//#include "wx/dnd.h"

//#include "DropFileEvent.h"

//class AmayaGLCanvas;
//class TestUnicodeDialogue;

class AmayaFrame : public wxFrame
{
public:
//  friend class AmayaApp;

  AmayaFrame ( wxFrame * frame
	       ,const wxPoint& pos
	       ,const wxSize& size
	       ,int frameid
//		,wxLocale & locale
	      );
  virtual ~AmayaFrame();

  void appendMenu ( wxMenu * p_menu, const wxString & label );
  void appendMenuItem ( 
	wxMenu * 		p_menu_parent,
	long 		id,
	const wxString & 	label,
	const wxString & 	help,
	wxItemKind 		kind,
	const AmayaCParam & callback );
 
  void OnClose( wxCloseEvent& event );
  void OnToolBarTool( wxCommandEvent& event );
  void OnMenuItem( wxCommandEvent& event );



  

  void SetURL ( const wxString & new_url );
  void AppendURL ( const wxString & new_url );

//  wxMenuBar *   getMenuBar() { return m_pMenuBar; }
  int		getFrameId() { return m_FrameId; }
  void          setFrameId( int frame ) { m_FrameId = frame; }

   /*   wxToolBar *   getToolBar() { return m_pToolBar; }
   wxStatusBar * getStatusBar() { return m_pStatusBar; }
   */
 protected:
    AmayaCallback menuCallback;
    AmayaCallback toolbarCallback;

 protected:
  DECLARE_EVENT_TABLE()
   
  int          m_FrameId;			// amaya frame index

  bool         m_IsFullScreenEnable;
  bool         m_IsToolTipEnable;
  wxMenuItem * m_pMenuItemToggleFullScreen;
  wxMenuItem * m_pMenuItemToggleToolTip;

  wxMenuBar * m_pMenuBar;
  wxMenu * m_pFileMenu;
  wxMenu * m_pViewMenu;
  wxMenu * m_pInfoMenu;

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

#endif //__AMAYAFRAME_H__

#endif /* #ifdef _WX */
