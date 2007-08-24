#ifdef _WX

#ifndef __AMAYAPANEL_H__
#define __AMAYAPANEL_H__

#include "wx/wx.h"
#include "wx/panel.h"
#include "wx/scrolwin.h"
#include "wx/dynarray.h"

#include "paneltypes_wx.h"
#include "AmayaParams.h"


class AmayaXHTMLPanel;
class AmayaAttributePanel;
class AmayaApplyClassPanel;
class AmayaColorsPanel;
class AmayaCharStylePanel;
class AmayaFormatPanel;
class AmayaNormalWindow;

/*
 *  Description:  - AmayaPanel contains a panel (bookmarks, elemets, attributes ...)
 *  
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
*/


/**
 * New tool panel bar class
 */

class AmayaToolPanel;
class AmayaToolPanelContainer;
class AmayaDockedToolPanelContainer;
class AmayaFloatingToolPanelContainer;


struct AmayaToolPanelBarListItem
{
  wxClassInfo* ci;
  AmayaToolPanel* panel;
  AmayaDockedToolPanelContainer*   dock;
  AmayaFloatingToolPanelContainer* floa;
  bool shown;
  bool floating;
  bool minimized;
};

WX_DEFINE_ARRAY_PTR(AmayaToolPanelBarListItem *, AmayaToolPanelBarList);


class AmayaToolPanelBar : public wxPanel
{
  DECLARE_DYNAMIC_CLASS(AmayaToolPanelBar)
  DECLARE_EVENT_TABLE()
public:
  AmayaToolPanelBar();
  AmayaToolPanelBar(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
      const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT("AmayaToolPanelBar"));
  virtual ~AmayaToolPanelBar();
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
      const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT("AmayaToolPanelBar"));
  
  void ShowHeader(bool bShow);
  void HideHeader(){ShowHeader(false);}
  
  void ShowPanel(AmayaToolPanel* panel, bool bShow=true);
  void MinimizePanel(AmayaToolPanel* panel, bool bMin=true);
  void FloatPanel(AmayaToolPanel* panel, bool bFloat=true);

  void ToggleMinimize(AmayaToolPanel* panel);
  void ToggleFloat(AmayaToolPanel* panel);
  
  void HidePanel(AmayaToolPanel* panel){ShowPanel(panel, false);}
  void UnminimizePanel(AmayaToolPanel* panel){MinimizePanel(panel, false);}
  void DockPanel(AmayaToolPanel* panel){FloatPanel(panel, false);}
  
  bool IsMinimized(const AmayaToolPanel* panel)const;
  bool IsExpanded(const AmayaToolPanel* panel)const;
  bool IsFloating(const AmayaToolPanel* panel)const;
  bool IsShown(const AmayaToolPanel* panel)const;
  
  void OpenToolPanel( int panel_type, bool bOpen=true);
  void CloseToolPanel( int panel_type ){OpenToolPanel(false);}
  
  AmayaToolPanel* GetToolPanel(int type);
  
  void LoadConfig();
  void SaveConfig();
  
private:
  void OnClose( wxCommandEvent& event );
  
  void Initialize();
  
  void AddPanel(AmayaToolPanel* panel);
  
  const AmayaToolPanelBarListItem* FindItem(const AmayaToolPanel* panel)const;
  AmayaToolPanelBarListItem* FindItem(const AmayaToolPanel* panel);
  
  AmayaToolPanelBarList m_panels;
  wxScrolledWindow* m_scwin;
};

typedef class AmayaToolPanelBar AmayaPanel;

/**
 * Base tool panel container.
 */
class AmayaToolPanelContainer
{
public:
  AmayaToolPanelContainer(AmayaToolPanel* panel, AmayaToolPanelBar* bar):
    m_panel(panel), m_bar(bar){}
  virtual ~AmayaToolPanelContainer(){}
  
  AmayaToolPanel* GetPanel()const{return m_panel;}
  void SetPanel(AmayaToolPanel* panel){m_panel=panel;}
  
  AmayaToolPanelBar* GetBar()const{return m_bar;}
  void SetBar(AmayaToolPanelBar* bar){m_bar=bar;}

  virtual bool Detach()=0;
  virtual bool Attach()=0;
  
protected:
  AmayaToolPanel* m_panel;
  AmayaToolPanelBar* m_bar;
};


/**
 * Docked tool panel container
 */
class AmayaDockedToolPanelContainer : public wxPanel, public AmayaToolPanelContainer
{
  DECLARE_EVENT_TABLE()
public:
  AmayaDockedToolPanelContainer(AmayaToolPanel* panel, AmayaToolPanelBar* bar,
      wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
      const wxSize& size = wxDefaultSize, long style = 0);
  virtual ~AmayaDockedToolPanelContainer();
  
  /**
   * Intend to minimize/unminimize the panel.
   * If the Container dont embed the panel, return false
   */
  bool Minimize(bool bMinimize);
  bool IsMinimized();

  virtual bool Detach();
  virtual bool Attach();
private:
  void OnMinimize(wxCommandEvent& event);
  void OnDetach(wxCommandEvent& event);
  
  void OnUpdateMinimizeUI(wxUpdateUIEvent& event);
  
  bool m_bMinimized;
  bool m_bDetached;
  
  static wxBitmap s_Bitmap_DetachOn;
  static wxBitmap s_Bitmap_DetachOff;
  static wxBitmap s_Bitmap_ExpandOn;
  static wxBitmap s_Bitmap_ExpandOff;
  static bool s_bmpOk;
};


/**
 * New Floating tool panel container.
 */
class AmayaFloatingToolPanelContainer : public wxDialog, public AmayaToolPanelContainer
{
  DECLARE_EVENT_TABLE()
public:
  AmayaFloatingToolPanelContainer(AmayaToolPanel* panel, AmayaToolPanelBar* bar,
      wxWindow * parent = NULL
          ,wxWindowID     id             = wxID_ANY
          ,const wxPoint& pos            = wxDefaultPosition
          ,const wxSize&  size           = wxDefaultSize
          ,long style                    = 
          wxDEFAULT_DIALOG_STYLE |
          wxSTAY_ON_TOP | 
          wxTHICK_FRAME |
          wxRESIZE_BORDER);
  virtual ~AmayaFloatingToolPanelContainer();
  
  virtual bool Detach();
  virtual bool Attach();
  
protected:
  void OnClose(wxCloseEvent& event);
};

/**
 * New class for tool panel.
 */
class AmayaToolPanel : public wxPanel
{
  DECLARE_ABSTRACT_CLASS(AmayaToolPanel)
  DECLARE_EVENT_TABLE()
public:
  AmayaToolPanel();
  virtual ~AmayaToolPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaToolPanel"), wxObject* extra=NULL)=0;
  
  /** Panel title. */
  virtual wxString GetToolPanelName()const=0;
  /** Panel type code. */
  virtual int      GetToolPanelType()const;
  /** Panel state config key name. */
  virtual wxString GetToolPanelConfigKeyName()const;
  
  AmayaToolPanelBar* GetBar(){return m_bar;}
  const AmayaToolPanelBar* GetBar()const{return m_bar;}
  void SetBar(AmayaToolPanelBar* bar){m_bar = bar;}
  
  bool IsExpanded();
  bool IsFloating();
  bool IsVisible();

  /**
   * Setup a flag used to force DoUpdate call when the
   * sub panel is ready to be updated (when it is visible).
   */
  void ShouldBeUpdated( bool should_update = true );

  /**
   * Refresh the button widgets of the frame's panel,
   * should be override in inherited class
   */
  virtual void SendDataToPanel( AmayaParams& params );

protected:
  /**
   * This method is called when the sub-panel is ready to be updated
   * it should be redefined into inherited classes but do not forget to call
   * AmayaToolPanel::DoUpdate() in order to update the flags. */
  virtual void DoUpdate();
  
private:
  AmayaToolPanelBar* m_bar;
  bool m_ShouldBeUpdated;
};

#endif // __AMAYAPANEL_H__

#endif /* #ifdef _WX */

