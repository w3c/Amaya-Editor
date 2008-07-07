#ifdef _WX

#ifndef __AMAYAELEMENTPANEL_H__
#define __AMAYAELEMENTPANEL_H__

#include "wx/wx.h"
#include "wx/aui/auibook.h"
#include "wx/imaglist.h"
#include "AmayaToolPanel.h"


class AmayaNormalWindow;

class wxNotebook;
class AmayaXHTMLPanel;
class AmayaMathMLPanel;
class AmayaXMLPanel;



/*
 *  Description:  - AmayaElementPanel is the panel which embed XHTML, MathML, XML panels
*/
class AmayaElementToolPanel : public AmayaToolPanel
{
  DECLARE_DYNAMIC_CLASS(AmayaElementToolPanel)
public:
  AmayaElementToolPanel();
  virtual ~AmayaElementToolPanel();
  
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaElementToolPanel"), wxObject* extra=NULL);

  /** Return a default AUI config for the panel.*/
  virtual wxString GetDefaultAUIConfig();
  virtual bool GetDefaultVisibilityState()const{return true;}

  virtual wxString GetToolPanelName()const;
  virtual int      GetToolPanelType()const{return WXAMAYA_PANEL_ELEMENTS;}
  virtual wxString GetToolPanelConfigKeyName()const{return wxT("PANEL_ELEMENTS");}
  
  virtual void SendDataToPanel(int panel_type, AmayaParams& params );
  
  int PanelTypeToIndex(int panel_type)const;
  void RaisePanel(int panel_type);
  
protected:
//  wxNotebook* m_notebook;
  wxAuiNotebook* m_notebook;
  wxImageList m_imageList;
  
  AmayaXMLPanel* m_xml;
 
};



#endif // __AMAYAXHTMLPANEL_H__

#endif /* #ifdef _WX */
