#ifdef _WX

#ifndef __AMAYASTYLEPANEL_H__
#define __AMAYASTYLEPANEL_H__

#include "wx/wx.h"
#include "AmayaPanel.h"
#include "AmayaToolBar.h"
#include "AmayaColorButton.h"

class AmayaNormalWindow;

/*
 *  Description:  - AmayaXHTMLPanel is a specific sub-panel
 *       Author:  Irene Vatton
*/
class AmayaStyleToolPanel : public AmayaToolPanel
{
  DECLARE_DYNAMIC_CLASS(AmayaStyleToolPanel)
public:
  AmayaStyleToolPanel();
  virtual ~AmayaStyleToolPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaStyleToolPanel"), wxObject* extra=NULL);
  
  virtual wxString GetToolPanelName()const;
  virtual int      GetToolPanelType()const{return WXAMAYA_PANEL_STYLE;}
  virtual wxString GetToolPanelConfigKeyName()const{return wxT("OPEN_PANEL_STYLE");}
  void OnColorPalette( AmayaColorButtonEvent& event );
  void SetColor(int color);
  void OnThemeChange( wxCommandEvent& event );
  void SetTheme(char *theme);

protected:
  // Any class wishing to process wxWindows events must use this macro
  DECLARE_EVENT_TABLE()
  AmayaBaseToolBar *m_tbar1, *m_tbar2;
  wxColourData colour_data;
};


#endif // __AMAYASTYLEPANEL_H__

#endif /* #ifdef _WX */
