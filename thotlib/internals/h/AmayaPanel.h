#ifdef _WX

#ifndef __AMAYAPANEL_H__
#define __AMAYAPANEL_H__

#include "wx/wx.h"
#include "wx/panel.h"
#include "wx/scrolwin.h"
#include "wx/dynarray.h"

#include "paneltypes_wx.h"
#include "AmayaParams.h"

#include "wx/hashset.h"
WX_DECLARE_HASH_SET( wxClassInfo *, wxPointerHash, wxPointerEqual, ClassInfoSet );
extern ClassInfoSet g_AmayaToolPanelClassInfoSet; // Set of registered AmayaToolPanel class infos.


class AmayaXHTMLPanel;
class AmayaStylePanel;
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
class AmayaToolPanelItem;

WX_DEFINE_ARRAY_PTR(AmayaToolPanelItem *, AmayaToolPanelItemArray);


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
  
  void MinimizePanel(AmayaToolPanel* panel, bool bMin=true);
  void ToggleMinimize(AmayaToolPanel* panel);
  void UnminimizePanel(AmayaToolPanel* panel){MinimizePanel(panel, false);}
  
  bool IsMinimized(const AmayaToolPanel* panel)const;
  bool IsExpanded(const AmayaToolPanel* panel)const;
  
  void OpenToolPanel( int panel_type, bool bOpen=true);
  void CloseToolPanel( int panel_type ){OpenToolPanel(false);}
  
  AmayaToolPanel* GetToolPanel(int type);
  
  void LoadConfig();
  void SaveConfig();
  
  bool AddPanel(AmayaToolPanel* panel);
  
private:
  void OnClose( wxCommandEvent& event );
  
  const AmayaToolPanelItem* FindItem(const AmayaToolPanel* panel)const;
  AmayaToolPanelItem* FindItem(const AmayaToolPanel* panel);
  
  wxScrolledWindow* m_scwin;
  AmayaToolPanelItemArray m_items;
};

/**
 * Docked tool panel item container
 */
class AmayaToolPanelItem : public wxPanel
{
  DECLARE_EVENT_TABLE()
public:
  AmayaToolPanelItem(AmayaToolPanel* panel,
      wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, 
      const wxSize& size = wxDefaultSize, long style = 0);
  virtual ~AmayaToolPanelItem();
  
  /**
   * Intend to minimize/unminimize the panel.
   * If the Container dont embed the panel, return false
   */
  bool Minimize(bool bMinimize = true);
  bool IsMinimized()const{return m_bMinimized;}
  
  AmayaToolPanel*    GetPanel()const{return m_panel;}
  AmayaToolPanelBar* GetBar()const{return m_bar;}

private:
  void OnMinimize(wxCommandEvent& event);
  
  AmayaToolPanel*    m_panel;
  AmayaToolPanelBar* m_bar;
  
  bool m_bMinimized;
  
  static wxBitmap s_Bitmap_Minimized;
  static wxBitmap s_Bitmap_Expanded;
  static bool s_bmpOk;
};





#endif // __AMAYAPANEL_H__

#endif /* #ifdef _WX */

