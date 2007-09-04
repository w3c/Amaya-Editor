#ifdef _WX

#ifndef __AMAYAMATHMLPANEL_H__
#define __AMAYAMATHMLPANEL_H__

#include "wx/wx.h"
#include "AmayaPanel.h"
#include "AmayaToolBar.h"

class AmayaNormalWindow;

/*
 *  Description:  - AmayaMathMLPanel is a specific sub-panel
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
 */
class AmayaMathMLToolPanel : public AmayaToolPanel
{
  DECLARE_DYNAMIC_CLASS(AmayaMathMLToolPanel)
public:
  typedef enum
    {
      wxMATHML_ACTION_UNKNOWN,
      wxMATHML_ACTION_INIT,
      wxMATHML_ACTION_REFRESH,
    } wxMATHML_ACTION;

  
  AmayaMathMLToolPanel();
  virtual ~AmayaMathMLToolPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaMathMLToolPanel"), wxObject* extra=NULL);
  
  virtual wxString GetToolPanelName()const;
  virtual int      GetToolPanelType()const{return WXAMAYA_PANEL_MATHML;}
  virtual wxString GetToolPanelConfigKeyName()const{return wxT("OPEN_PANEL_MATHML");}
};


class AmayaMathMLToolBar : public AmayaBaseToolBar
{
  DECLARE_DYNAMIC_CLASS(AmayaMathMLToolBar)
public:
  AmayaMathMLToolBar();
private:
  static AmayaToolBarToolDefHashMap s_mymap;
  static bool s_isinit;
};

#endif // __AMAYAMATHMLPANEL_H__

#endif /* #ifdef _WX */
