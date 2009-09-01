#ifdef _WX

#ifndef STYLELISTDLGWX_H_
#define STYLELISTDLGWX_H_

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
#include "AmayaDialog.h"
#include "AmayaToolPanel.h"

#include "thot_sys.h"
#include "tree.h"
#include "document.h"

#include "wx/hashmap.h"

typedef struct _PInfo *PInfoPtr;

WX_DECLARE_HASH_MAP( int, PInfoPtr, wxIntegerHash, wxIntegerEqual, StyleListInfoMap );

//-----------------------------------------------------------------------------
// Class definition: StyleListToolPanel
//-----------------------------------------------------------------------------

class StyleListToolPanel : public AmayaToolPanel
{
  DECLARE_DYNAMIC_CLASS(StyleListToolPanel)
  DECLARE_EVENT_TABLE()
public:
  StyleListToolPanel();
  virtual ~StyleListToolPanel();

  virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize, long style = 0,
            const wxString& name = wxT("AmayaToolPanel"), wxObject* extra=NULL);
  
  /** Panel title. */
  virtual wxString GetToolPanelName()const;
  /** Panel type code. */
  virtual int      GetToolPanelType()const{return WXAMAYA_PANEL_STYLE_LIST;}
  /** Panel state config key name. */
  virtual wxString GetToolPanelConfigKeyName()const{return wxT("PANEL_STYLE_LIST");}

  /** Return a default AUI config for the panel.*/
  virtual wxString GetDefaultAUIConfig();

  void Update(Document doc);
protected:
  virtual void SendDataToPanel( AmayaParams& params );
private:
  StyleListInfoMap m_map;
  wxCheckListBox* m_list;
  Document m_doc;

  void OnCheckSheet(wxCommandEvent& event);
  void OnOpenSheet( wxCommandEvent& event );
  void OnAddSheet(wxCommandEvent& event);
  void OnRemSheet(wxCommandEvent& event);
  void OnActivateSheet(wxCommandEvent& event);
  void OnDesactivateSheet(wxCommandEvent& event);
  void OnShowSheet(wxCommandEvent& event);

  void OnUpdateAddSheet(wxUpdateUIEvent& event);
  void OnUpdateRemSheet(wxUpdateUIEvent& event);
  void OnUpdateActivateSheet(wxUpdateUIEvent& event);
  void OnUpdateDesactivateSheet(wxUpdateUIEvent& event);
  void OnUpdateShowSheet(wxUpdateUIEvent& event);
};

#endif /*STYLELISTDLGWX_H_*/

#endif /* _WX */
