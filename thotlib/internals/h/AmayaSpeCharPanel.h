#ifdef _WX

#ifndef __AMAYASPECHARPANEL_H__
#define __AMAYASPECHARPANEL_H__

#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/choicebk.h"
#include "wx/toolbook.h"
#include "wx/imaglist.h"
#include "AmayaToolPanel.h"


class AmayaNormalWindow;

typedef struct
{
  int unicode;
  const char* name;
}AmayaSpeChar;

WX_DECLARE_HASH_MAP( int, AmayaSpeChar*, wxIntegerHash, wxIntegerEqual, AmayaSpeCharMap );

WX_DECLARE_HASH_MAP( int, wxBitmap, wxIntegerHash, wxIntegerEqual, AmayaSpeCharBitmapMap );


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
  virtual wxString GetToolPanelConfigKeyName()const{return wxT("PANEL_SPECHAR");}
  virtual bool GetDefaultVisibilityState()const{return true;}

  /** Return a default AUI config for the panel.*/
  virtual wxString GetDefaultAUIConfig();

protected:
  void Initialize();
  
  void OnTool(wxCommandEvent& event);
  
  DECLARE_EVENT_TABLE()
  
  AmayaSpeCharMap m_hash;
  wxChoicebook *m_pBook;
  wxImageList m_imagelist;
  
  static AmayaSpeCharBitmapMap s_bitmapMap;
  static void InitializeBitmapMap();
};

#endif // __AMAYASPECHARPANEL_H__

#endif /* #ifdef _WX */
