#ifdef _WX

#ifndef __AMAYASPECHARPANEL_H__
#define __AMAYASPECHARPANEL_H__

#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/imaglist.h"
#include "AmayaPanel.h"


class AmayaNormalWindow;

WX_DECLARE_HASH_MAP( int, wxString, wxIntegerHash, wxIntegerEqual, MathMLEntityHash );
WX_DECLARE_HASH_MAP( int, int, wxIntegerHash, wxIntegerEqual, MathMLWxEntityHash );

class AmayaSpeCharToolPanel :  public AmayaToolPanel
{
  DECLARE_DYNAMIC_CLASS(AmayaSpeCharToolPanel)
public:
  typedef enum
    {
      wxSPECHAR_ACTION_UNKNOWN,
      wxSPECHAR_ACTION_INIT,
      wxSPECHAR_ACTION_REFRESH,
    } wxSPECHAR_ACTION;
  
    AmayaSpeCharToolPanel();
  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaSpeCharToolPanel"), wxObject* extra=NULL);
  
  virtual wxString GetToolPanelName()const;
  virtual int      GetToolPanelType()const{return WXAMAYA_PANEL_SPECHAR;}
  virtual wxString GetToolPanelConfigKeyName()const{return wxT("OPEN_PANEL_SPECHAR");}
  
protected:
  void Initialize();
  
  void OnTool(wxCommandEvent& event);
  
  DECLARE_EVENT_TABLE()

  MathMLWxEntityHash m_hash;
  wxNotebook *m_pNotebook;
  wxImageList m_imagelist;
  
  static MathMLEntityHash s_MathMLEntityHash;
};

#endif // __AMAYASPECHARPANEL_H__

#endif /* #ifdef _WX */
