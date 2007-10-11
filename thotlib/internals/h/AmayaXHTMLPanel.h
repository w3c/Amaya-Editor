#ifdef _WX

#ifndef __AMAYAXHTMLPANEL_H__
#define __AMAYAXHTMLPANEL_H__

#include "wx/wx.h"
#include "AmayaPanel.h"
#include "AmayaToolBar.h"

class AmayaNormalWindow;

/*
 *  Description:  - AmayaXHTMLPanel is a specific sub-panel
 *       Author:  Stephane GULLY
 *      Created:  13/09/2004 04:45:34 PM CET
 *     Revision:  none
*/
class AmayaXHTMLToolPanel : public AmayaToolPanel
{
  DECLARE_DYNAMIC_CLASS(AmayaXHTMLToolPanel)
public:
  AmayaXHTMLToolPanel();
  virtual ~AmayaXHTMLToolPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaXHTMLToolPanel"), wxObject* extra=NULL);

  /** Return a default AUI config for the panel.*/
  virtual wxString GetDefaultAUIConfig();

  virtual bool CanResize() {return false;}
  
  virtual wxString GetToolPanelName()const;
  virtual int      GetToolPanelType()const{return WXAMAYA_PANEL_XHTML;}
  virtual wxString GetToolPanelConfigKeyName()const{return wxT("PANEL_XHTML");}
  
protected:

  AmayaBaseToolBar *m_tbar1, *m_tbar2, *m_tbar3, *m_tbar4;
 
};



#endif // __AMAYAXHTMLPANEL_H__

#endif /* #ifdef _WX */
