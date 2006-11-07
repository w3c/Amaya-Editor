#ifdef _WX

#ifndef __AMAYAEXPLORERPANEL_H__
#define __AMAYAEXPLORERPANEL_H__

#include "wx/wx.h"
#include "AmayaSubPanel.h"

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

class AmayaExplorerPanel : public AmayaSubPanel
{
public:
  DECLARE_DYNAMIC_CLASS(AmayaExplorerPanel)

  AmayaExplorerPanel( wxWindow * p_parent_window = NULL
           ,AmayaNormalWindow * p_parent_nwindow = NULL );
  virtual ~AmayaExplorerPanel();

  virtual bool IsActive();
  virtual int GetPanelType();

 protected:
  virtual void SendDataToPanel( AmayaParams& params );
  virtual void DoUpdate();

 protected:
  wxGenericDirCtrl* m_dirCtrl;
  void OnDirTreeItemActivate(wxTreeEvent& event);
  DECLARE_EVENT_TABLE()
};

#endif // __AMAYAEXPLORERPANEL_H__

#endif /* #ifdef _WX */
