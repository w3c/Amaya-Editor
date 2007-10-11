#ifdef _WX

#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/colordlg.h"
#include "wx/clrpicker.h"


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
#include "editcommands_f.h"

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "paneltypes_wx.h"

#include "AmayaStylePanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaColorButton.h"

extern void DoStyleColor (char * color);
extern void ChangeTheme (char *theme);
//
//
// AmayaStyleToolPanel
//
//
static int  Current_Color = -1;
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
  
  wxXmlResource::Get()->AttachUnknownControl(wxT("wxID_PANEL_CSS_COLOR"),
      new AmayaColorButton(this, XRCID("wxID_PANEL_CSS_COLOR"), wxColour(0,0,0), wxDefaultPosition, wxSize(16,16), wxBORDER_RAISED));

  
  m_tbar1 = XRCCTRL(*this,"wxID_TOOLBAR_CSS_1", AmayaBaseToolBar);
  m_tbar2 = XRCCTRL(*this,"wxID_TOOLBAR_CSS_2", AmayaBaseToolBar);

  m_tbar1->Add(AmayaStyleToolDef);
  m_tbar1->Realize();
  m_tbar2->Add(AmayaStyleToolDef);
  m_tbar2->Realize();
  SetAutoLayout(true);
  
//  XRCCTRL(*this, "wxID_PANEL_CSS_COLOR", AmayaColorButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CPCOLORBUTTON)));
  
  XRCCTRL(*this, "wxID_FONT_COLOR", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CPCOLORFG)));
  XRCCTRL(*this, "wxID_THEME", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_THEME)));
  SetColor (1);
  SetTheme ("Standard");
  return true;
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
wxString AmayaStyleToolPanel::GetToolPanelName()const
{
  return TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_CSSStyle));
}

/*----------------------------------------------------------------------
 *       Class:  AmayaStyleToolPanel
 *      Method:  GetDefaultAUIConfig
 * Description:  Return a default AUI config for the panel.
 -----------------------------------------------------------------------*/
wxString AmayaStyleToolPanel::GetDefaultAUIConfig()
{
  return wxT("state=18875644;dir=2;layer=0;row=0;pos=1;prop=100000;bestw=187;besth=119;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=1073;floaty=587;floatw=195;floath=143");
}


/*----------------------------------------------------------------------
  OnColorPalette is called when the user click on the color palette button
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnColorPalette( AmayaColorButtonEvent& event )
{
  char     color_string[100];
  int      color;
  wxColour col = event.GetColour();
  
  color = TtaGetThotColor (col.Red(), col.Green(), col.Blue());
  if (color != Current_Color)
    Current_Color = color;
  // generate a color style
  sprintf( color_string, "color:#%02x%02x%02x", col.Red(), col.Green(), col.Blue());
  CloseTextInsertion ();
  DoStyleColor (color_string);
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
  XRCCTRL(*this, "wxID_PANEL_CSS_COLOR", AmayaColorButton)->SetColour( col );
}

/*----------------------------------------------------------------------
  OnColorPalette is called when the user click on the color palette button
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnThemeChange( wxCommandEvent& event )
{
  wxString  value;
  char      theme[100];

  value = XRCCTRL(*this, "wxID_PANEL_CSS_THEME", wxChoice)->GetStringSelection();
  strcpy (theme, (const char*)value.mb_str(wxConvUTF8));
  CloseTextInsertion ();
  if (!strcasecmp (theme, "No theme"))
    ChangeTheme ("Standard");
  else if (!strcasecmp (theme, "classic"))
    ChangeTheme ("Classic");
  else if (!strcasecmp (theme, "modern"))
    ChangeTheme ("Modern");
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::SetTheme(char *theme)
{
  if (!strcasecmp (theme, "Standard"))
    XRCCTRL(*this, "wxID_PANEL_CSS_THEME", wxChoice)->SetStringSelection(TtaConvMessageToWX("No theme"));
  else if (!strcasecmp (theme, "Classic"))
    XRCCTRL(*this, "wxID_PANEL_CSS_THEME", wxChoice)->SetStringSelection(TtaConvMessageToWX("Classic"));
  else if (!strcasecmp (theme, "Modern"))
    XRCCTRL(*this, "wxID_PANEL_CSS_THEME", wxChoice)->SetStringSelection(TtaConvMessageToWX("Modern"));
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaStyleToolPanel, AmayaToolPanel)
  EVT_AMAYA_COLOR_CHANGED(wxID_ANY, AmayaStyleToolPanel::OnColorPalette )
  EVT_CHOICE(XRCID("wxID_PANEL_CSS_THEME"), AmayaStyleToolPanel::OnThemeChange)
END_EVENT_TABLE()

#endif /* #ifdef _WX */
