#ifdef _WX

#ifndef __AMAYATOOLBAR_H__
#define __AMAYATOOLBAR_H__

#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/panel.h"
#include "wx/hashmap.h"
#include "wx/dcmemory.h"

#include "units_f.h" // for PixelToPoint function

class AmayaFrame;
class AmayaWindow;

/**
 * Function to create a bitmap from a text.
 */
template<int width, int height>
wxBitmap wxCharToIcon(const wxString str)
{
  wxBitmap bmp(width, height);
  wxMemoryDC dc;
  dc.SelectObject(bmp);
  dc.SetBrush(*wxWHITE_BRUSH);
  dc.Clear();
  
  wxFont font = dc.GetFont();
  font.SetPointSize(7);
  dc.SetFont(font);
  
  dc.DrawLabel(str, wxRect(0, 0, width, height), wxALIGN_CENTER);
  dc.SelectObject(wxNullBitmap);
  return bmp;
}



typedef struct
{
  const char* idname;
  const char* action;
  int         actionid;
  int tooltip_categ, tooltip_msg;
} AmayaToolBarToolDef;

WX_DECLARE_HASH_MAP( int, AmayaToolBarToolDef*, wxIntegerHash, wxIntegerEqual, AmayaToolBarToolDefHashMap );

#define AMAYA_DECLARE_TOOLBAR_DEF_TABLE(name) AmayaToolBarToolDef name[];
#define AMAYA_BEGIN_TOOLBAR_DEF_TABLE(name) AmayaToolBarToolDef name[]={
#define AMAYA_TOOLBAR_DEF(idname, action, tip_categ, tip_msg) {idname, action, -1, tip_categ, tip_msg},
#define AMAYA_END_TOOLBAR_DEF_TABLE() {NULL, NULL, -1, 0, 0} };

class AmayaBaseToolBar : public wxToolBar
{
  DECLARE_DYNAMIC_CLASS(AmayaToolBarEditing)
  DECLARE_EVENT_TABLE()
public:
  AmayaBaseToolBar();
  bool Create( wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxT("AmayaBaseToolBar") );
  virtual ~AmayaBaseToolBar();

  /** Add number of toolitem definitions. def must end by a NULL row.*/
  void Add(AmayaToolBarToolDef* def);
  
  // Called after all tools have been added.
  virtual bool Realize();

protected:
  AmayaToolBarToolDefHashMap* m_map;
  
  void OnTool(wxCommandEvent& event);
  
  void OnUpdate(wxUpdateUIEvent& event);
  
  void ShowAllTools(bool show){m_bShowAllTools = show;}
  
private:
  bool    m_bShowAllTools;
  AmayaToolBarToolDefHashMap  m_mymap;
};



class AmayaToolBarEditing : public AmayaBaseToolBar
{
  DECLARE_DYNAMIC_CLASS(AmayaToolBarEditing)
public:
  AmayaToolBarEditing();
};

class AmayaToolBarBrowsing : public AmayaBaseToolBar
{
  DECLARE_DYNAMIC_CLASS(AmayaToolBarBrowsing)
public:
  AmayaToolBarBrowsing();
};

class AmayaToolBarLogo : public AmayaBaseToolBar
{
  DECLARE_DYNAMIC_CLASS(AmayaToolBarLogo)
public:
  AmayaToolBarLogo();
  virtual bool Realize();
};


#endif // __AMAYATOOLBAR_H__

#endif /* #ifdef _WX */
