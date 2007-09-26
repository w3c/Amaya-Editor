#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/colordlg.h"


#include "thot_gui.h"
#include "thot_sys.h"
#include "constmedia.h"
#include "typemedia.h"
#include "appdialogue.h"
#include "dialog.h"
#include "selection.h"
#include "application.h"
#include "dialog.h"
#include "document.h"
#include "message.h"
#include "libmsg.h"
#include "frame.h"
#include "message_wx.h"
#include "paneltypes_wx.h"
#include "appdialogue_wx.h"
#include "appdialogue_wx_f.h"
#include "panel.h"
#include "displayview_f.h"
#include "registry_wx.h"

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "paneltypes_wx.h"

#include "AmayaStylePanel.h"
#include "AmayaNormalWindow.h"
extern void GenerateStyle (char * color, ThotBool add);

//
//
// AmayaStyleToolPanel
//
//
static int Current_Color = -1;

static
AMAYA_BEGIN_TOOLBAR_DEF_TABLE(AmayaStyleToolDef)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_LEFT",    "DoLeftAlign", LIB, TMSG_FORMATLEFT)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_CENTER",  "DoCenter", LIB, TMSG_FORMATCENTER)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_RIGHT",   "DoRightAlign", LIB, TMSG_FORMATRIGHT)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_JUSTIFY", "DoJustify", LIB, TMSG_FORMATJUSTIFY)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_S_LINE",  "LineSpacingSingle", LIB, TMSG_LINE_SPACING)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_H_LINE",  "LineSpacingHalf", LIB, TMSG_HLINE_SPACING)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_D_LINE",  "LineSpacingDouble", LIB, TMSG_DLINE_SPACING)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_INDENT",  "MarginLeftIncrease", LIB, TMSG_INDENT)
  AMAYA_TOOLBAR_DEF("wxID_PANEL_CSS_NOINDENT","MarginLeftDecrease", LIB, TMSG_NOINDENT)
AMAYA_END_TOOLBAR_DEF_TABLE()

IMPLEMENT_DYNAMIC_CLASS(AmayaStyleToolPanel, AmayaToolPanel)

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaStyleToolPanel::AmayaStyleToolPanel():
  AmayaToolPanel()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
AmayaStyleToolPanel::~AmayaStyleToolPanel()
{
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
bool AmayaStyleToolPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxXmlResource::Get()->LoadPanel((wxPanel*)this, parent, wxT("wxID_TOOLPANEL_STYLE")))
    return false;
  
  m_tbar1 = XRCCTRL(*this,"wxID_TOOLBAR_CSS_1", AmayaBaseToolBar);
  m_tbar2 = XRCCTRL(*this,"wxID_TOOLBAR_CSS_2", AmayaBaseToolBar);

  m_tbar1->Add(AmayaStyleToolDef);
  m_tbar1->Realize();
  m_tbar2->Add(AmayaStyleToolDef);
  m_tbar2->Realize();
  SetAutoLayout(true);
  
  XRCCTRL(*this, "wxID_PANEL_CSS_COLOR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CPCOLORBUTTON)));
  XRCCTRL(*this, "wxID_FONT_COLOR", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CPCOLORFG)));
  XRCCTRL(*this, "wxID_THEME", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_THEME)));
  SetColor (1);
  return true;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxString AmayaStyleToolPanel::GetToolPanelName()const
{
  return TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CSSStyle));
}


/*----------------------------------------------------------------------
  OnColorPalette is called when the user click on the color palette button
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnColorPalette( wxCommandEvent& event )
{
  char                color_string[100];
  int                 color;

  // open the color dialog and ask user to select a color.
  wxColourDialog dialog (this, &colour_data);
  if (dialog.ShowModal() == wxID_OK)
    {
      colour_data = dialog.GetColourData();
      wxColour col = colour_data.GetColour();
      XRCCTRL(*this, "wxID_PANEL_CSS_COLOR", wxBitmapButton)->SetBackgroundColour( col );
      color = TtaGetThotColor (col.Red(), col.Green(), col.Blue());
      if (color != Current_Color)
        Current_Color = color;
      // generate a color style
      sprintf( color_string, "color:#%02x%02x%02x", col.Red(), col.Green(), col.Blue());
      GenerateStyle (color_string, TRUE);
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::SetColor(int color)
{
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  wxColour            col;

  if (color == Current_Color)
    // already set
    return;

  Current_Color = color;
  TtaGiveThotRGB (color, &red, &green, &blue);
  col = wxColour ( red, green, blue );
  XRCCTRL(*this, "wxID_PANEL_CSS_COLOR", wxBitmapButton)->SetBackgroundColour( col );
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaStyleToolPanel, AmayaToolPanel)
  EVT_BUTTON(XRCID("wxID_PANEL_CSS_COLOR"), AmayaStyleToolPanel::OnColorPalette )
END_EVENT_TABLE()

#endif /* #ifdef _WX */
