#ifdef _WX

#ifndef __AMAYAEXPLORERPANEL_H__
#define __AMAYAEXPLORERPANEL_H__

#include "wx/wx.h"
#include "AmayaToolPanel.h"

class AmayaNormalWindow;
class wxTreeEvent;
class wxGenericDirCtrl;

/*
 *  Description:  - AmayaExplorerPanel is a specific sub-panel which allow to explore
 * and open files from local directories.
 *       Author:  Emilien KIA
 *      Created:  07/11/2006
 *     Revision:  none
*/
class AmayaExplorerToolPanel : public AmayaToolPanel
{
  DECLARE_DYNAMIC_CLASS(AmayaExplorerToolPanel)
public:
  AmayaExplorerToolPanel();
  virtual ~AmayaExplorerToolPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaExplorerToolPanel"), wxObject* extra=NULL);
  
  virtual wxString GetToolPanelName()const;
  virtual int      GetToolPanelType()const{return WXAMAYA_PANEL_EXPLORER;}
  virtual wxString GetToolPanelConfigKeyName()const{return wxT("PANEL_EXPLORER");}

  /** Return a default AUI config for the panel.*/
  virtual wxString GetDefaultAUIConfig();

protected:
  DECLARE_EVENT_TABLE()
  void OnDirTreeItemActivate(wxTreeEvent& event);
  void OnClose(wxCloseEvent& event);
  
};

#endif // __AMAYAEXPLORERPANEL_H__

#endif /* #ifdef _WX */
