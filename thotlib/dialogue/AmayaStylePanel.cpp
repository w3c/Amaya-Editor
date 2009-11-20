/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2009
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/colordlg.h"
#include "wx/clrpicker.h"
#include "wx/valtext.h"
#include "wx/spinctrl.h"
#include "wx/checkbox.h"

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
#include "select_tv.h"

#include "AmayaStylePanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaColorButton.h"

extern void DoStyleColor (char * color, ThotBool isBg);
extern void ChangeTheme (const char *theme);
//
//
// AmayaStyleToolPanel
//
//
static int Current_StylePanel = -1;

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
  
#ifdef _WINDOWS
  SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#endif /* _WINDOWS */

  /* SVG Style Panel */
  wxXmlResource::Get()->AttachUnknownControl(wxT("wxID_SVG_STROKE_COLOR"),
      new AmayaColorButton(this, XRCID("wxID_SVG_STROKE_COLOR"), wxColour(0,0,0), wxDefaultPosition, wxSize(16,16), wxBORDER_RAISED));
  wxXmlResource::Get()->AttachUnknownControl(wxT("wxID_SVG_FILL_COLOR"),
      new AmayaColorButton(this, XRCID("wxID_SVG_FILL_COLOR"), wxColour(255,255,255), wxDefaultPosition, wxSize(16,16), wxBORDER_RAISED));

  /* HTML Style Panel */
  wxXmlResource::Get()->AttachUnknownControl(wxT("wxID_PANEL_CSS_COLOR"),
      new AmayaColorButton(this, XRCID("wxID_PANEL_CSS_COLOR"), wxColour(0,0,0), wxDefaultPosition, wxSize(16,16), wxBORDER_RAISED));

  wxXmlResource::Get()->AttachUnknownControl(wxT("wxID_PANEL_CSS_BK_COLOR"),
      new AmayaColorButton(this, XRCID("wxID_PANEL_CSS_BK_COLOR"), wxColour(255,255,255), wxDefaultPosition, wxSize(16,16), wxBORDER_RAISED));
 
  m_tbar1 = XRCCTRL(*this,"wxID_TOOLBAR_CSS_1", AmayaBaseToolBar);
  m_tbar2 = XRCCTRL(*this,"wxID_TOOLBAR_CSS_2", AmayaBaseToolBar);

  m_tbar1->Add(AmayaStyleToolDef);
  m_tbar1->Realize();
  m_tbar2->Add(AmayaStyleToolDef);
  m_tbar2->Realize();
  Fit();
  SetAutoLayout(true);
  
  XRCCTRL(*this, "wxID_PANEL_CSS_COLOR", AmayaColorButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_BUTTON_1)));
  XRCCTRL(*this, "wxID_PANEL_CSS_BK_COLOR", AmayaColorButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_BUTTON_2)));

  XRCCTRL(*this, "wxID_BUTTON_TEXTCOLOR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CPCOLORFG)));
  XRCCTRL(*this, "wxID_BUTTON_BKCOLOR", wxBitmapButton)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_CPCOLORBG)));
  
  XRCCTRL(*this, "wxID_CHOICE_FAMILY", wxChoice)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_FONT_FAMILY)));
  
  XRCCTRL(*this, "wxID_COMBO_SIZE", wxComboBox)->SetToolTip(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_BODY_SIZE_PTS)));
  XRCCTRL(*this, "wxID_SVG_FILL_NONE", wxCheckBox)->SetValue(FillEnabled);
  XRCCTRL(*this, "wxID_SVG_STROKE_NONE", wxCheckBox)->SetValue(StrokeEnabled); 
  SetColor (1);
  SetStrokeColor(1);
  SetFillColor(-1);

  XRCCTRL(*this, "wxID_THEME", wxStaticText)->SetLabel(TtaConvMessageToWX(TtaGetMessage(LIB, TMSG_THEME)));
  wxChoice     *theme = XRCCTRL(*this, "wxID_PANEL_CSS_THEME", wxChoice);
  theme->Clear();
  theme->Append (TtaConvMessageToWX(TtaGetMessage(LIB, NoTheme)));
  theme->Append (TtaConvMessageToWX(TtaGetMessage(LIB, Classic)));
  theme->Append (TtaConvMessageToWX(TtaGetMessage(LIB, Modern)));
  SetTheme ("Standard");

  wxTextValidator valid(wxFILTER_NUMERIC);
  XRCCTRL(*this,"wxID_COMBO_SIZE", wxComboBox)->SetValidator(valid);
  GetSizer()->Hide ((size_t)1);
  RaiseDoctypePanels(WXAMAYA_DOCTYPE_XHTML);
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
//  return wxT("");
  return wxT("dir=2;layer=0;row=0;pos=1");
}


/*----------------------------------------------------------------------
 *       Class:  AmayaStyleToolPanel
 *      Method:  RaiseDoctypePanels
 * Description:  Raiser the specified panels
 -----------------------------------------------------------------------*/
void AmayaStyleToolPanel::RaiseDoctypePanels(int doctype)
{
  
  // manage a svg root within a xml document as default
  if (doctype == WXAMAYA_DOCTYPE_SVG &&
      FirstSelectedElement == LastSelectedElement &&
      FirstSelectedElement &&
      FirstSelectedElement->ElParent &&
      FirstSelectedElement->ElStructSchema != FirstSelectedElement->ElParent->ElStructSchema)
    doctype = WXAMAYA_DOCTYPE_XHTML;
  switch(doctype)
  {
    case WXAMAYA_DOCTYPE_SVG:
      if (Current_StylePanel == -1 || Current_StylePanel == 0)
        {
          if (Current_StylePanel == 0)
            GetSizer()->Hide((size_t)0);
          GetSizer()->Show ((size_t)1);
          Current_StylePanel = 1;
        }
      break;      
    default:
      if (Current_StylePanel == -1 || Current_StylePanel == 1)
        {
          if (Current_StylePanel == 1)
            GetSizer()->Hide ((size_t)1);
          GetSizer()->Show ((size_t)0);
          Current_StylePanel = 0;
        }
      break;
  }
  Layout();
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
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::SetBackgroundColor(int color)
{
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  wxColour            col;

  if (color == Current_BackgroundColor)
    // already set
    return;

  Current_BackgroundColor = color;
  if (color == -1)
    col = GetBackgroundColour();
  else
    {
      TtaGiveThotRGB (color, &red, &green, &blue);
      col = wxColour ( red, green, blue );
    }
  XRCCTRL(*this, "wxID_PANEL_CSS_BK_COLOR", AmayaColorButton)->SetColour( col );  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::SetStrokeColor(int color)
{
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  wxColour            col;

  if (color == Current_StrokeColor)
    // already set
    return;

  Current_StrokeColor = color;
  TtaGiveThotRGB (color, &red, &green, &blue);
  col = wxColour ( red, green, blue );
  XRCCTRL(*this, "wxID_SVG_STROKE_COLOR", AmayaColorButton)->SetColour( col );
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::SetFillColor(int color)
{
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  wxColour            col;

  if (color == Current_FillColor)
    // already set
    return;

  Current_FillColor = color;
  if (color == -1)
    col = GetBackgroundColour();
  else
    {
      TtaGiveThotRGB (color, &red, &green, &blue);
      col = wxColour ( red, green, blue );
    }
  XRCCTRL(*this, "wxID_SVG_FILL_COLOR", AmayaColorButton)->SetColour( col );  
}

/*----------------------------------------------------------------------
  OnThemeChange
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnThemeChange( wxCommandEvent& event )
{
  wxString  value;
  char      theme[100];
  char    *s;

  value = XRCCTRL(*this, "wxID_PANEL_CSS_THEME", wxChoice)->GetStringSelection();
  strcpy (theme, (const char*)value.mb_str(wxConvUTF8));
  CloseTextInsertion ();
  s = TtaGetMessage(LIB, NoTheme);
  if (s && !strcasecmp (theme, s))
    ChangeTheme ("Standard");
  else
    {
      s = TtaGetMessage(LIB, Classic);
      if (s && !strcasecmp (theme, s))
        ChangeTheme ("Classic");
      else
        {
          s = TtaGetMessage(LIB, Modern);
          if (s && !strcasecmp (theme, s))
            ChangeTheme ("Modern");
        }
    }
}

/*----------------------------------------------------------------------
  SetTheme
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::SetTheme(const char *theme)
{
  char    *s;
  if (!strcasecmp (theme, "Standard"))
    s = TtaGetMessage(LIB, NoTheme);
  else if (!strcasecmp (theme, "Classic"))
    s = TtaGetMessage(LIB, Classic);
  else if (!strcasecmp (theme, "Modern"))
    s = TtaGetMessage(LIB, Modern);
  XRCCTRL(*this, "wxID_PANEL_CSS_THEME", wxChoice)->SetStringSelection(TtaConvMessageToWX(s));
}

/*----------------------------------------------------------------------
  GenerateFontColour
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::GenerateFontColour(wxColour c)
{
  char     color_string[100];
  int      color;

  color = TtaGetThotColor (c.Red(), c.Green(), c.Blue());
  if (color != Current_Color)
    Current_Color = color;
  // generate a color style
  sprintf( color_string, "#%02x%02x%02x", c.Red(), c.Green(), c.Blue());
  CloseTextInsertion ();
  DoStyleColor (color_string, FALSE);
}

/*----------------------------------------------------------------------
  OnChooseFontColor
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnChooseFontColor(wxCommandEvent& event)
{
  wxColour c;

  c = XRCCTRL(*this, "wxID_PANEL_CSS_COLOR", AmayaColorButton)->ChooseColour();
  if (c != wxNullColour)
    GenerateFontColour(c);  
}

/*----------------------------------------------------------------------
  OnColorFontPalette
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnColorFontPalette( AmayaColorButtonEvent& event )
{
  GenerateFontColour(event.GetColour());
}

/*----------------------------------------------------------------------
  GenerateBackgroundColour
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::GenerateBackgroundColour(wxColour c)
{
  char     color_string[100];
  int      color;

  color = TtaGetThotColor (c.Red(), c.Green(), c.Blue());
  if (color != Current_BackgroundColor)
    Current_BackgroundColor = color;
  // generate a color style
  sprintf( color_string, "#%02x%02x%02x", c.Red(), c.Green(), c.Blue());
  CloseTextInsertion ();
  DoStyleColor (color_string, TRUE);
}

/*----------------------------------------------------------------------
  OnChooseBackgroundColor
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnChooseBackgroundColor(wxCommandEvent& event)
{
  wxColour c;

  c = XRCCTRL(*this, "wxID_PANEL_CSS_BK_COLOR", AmayaColorButton)->ChooseColour();
  if (c != wxNullColour)
    GenerateBackgroundColour(c);    
}

/*----------------------------------------------------------------------
  OnColorBackgroundPalette
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnColorBackgroundPalette( AmayaColorButtonEvent& event )
{
  GenerateBackgroundColour(event.GetColour());  
}

/*----------------------------------------------------------------------
  GenerateStrokeColour
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::GenerateStrokeColour(wxColour c)
{
  char     color_string[100];
  int      color;

  color = TtaGetThotColor (c.Red(), c.Green(), c.Blue());
  if (color != Current_StrokeColor)
    Current_StrokeColor = color;
  // generate a color style
  sprintf( color_string, "#%02x%02x%02x", c.Red(), c.Green(), c.Blue());
  CloseTextInsertion ();
  DoStyleColor (color_string, FALSE);
}

/*----------------------------------------------------------------------
  OnChooseStrokeColor
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnChooseStrokeColor(wxCommandEvent& event)
{
  wxColour c;

  c = XRCCTRL(*this, "wxID_SVG_STROKE_COLOR", AmayaColorButton)->ChooseColour();
  if (c != wxNullColour)
  GenerateStrokeColour(c);
}

/*----------------------------------------------------------------------
  OnColorStrokePalette
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnColorStrokePalette( AmayaColorButtonEvent& event )
{
  GenerateStrokeColour(event.GetColour());
}

/*----------------------------------------------------------------------
  GenerateFillColour
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::GenerateFillColour(wxColour c)
{
  char     color_string[100];
  int      color;

  color = TtaGetThotColor (c.Red(), c.Green(), c.Blue());
  if (color != Current_FillColor)
    Current_FillColor = color;
  // generate a color style
  sprintf( color_string, "#%02x%02x%02x", c.Red(), c.Green(), c.Blue());
  CloseTextInsertion ();
  DoStyleColor (color_string, TRUE);
}

/*----------------------------------------------------------------------
  OnChooseFillColor
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnChooseFillColor(wxCommandEvent& event)
{
  wxColour c;

  c = XRCCTRL(*this, "wxID_SVG_FILL_COLOR", AmayaColorButton)->ChooseColour();
  if (c != wxNullColour)
    GenerateFillColour(c);    
}

/*----------------------------------------------------------------------
  OnColorFillPalette
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnColorFillPalette( AmayaColorButtonEvent& event )
{
  GenerateFillColour(event.GetColour());  
}

/*----------------------------------------------------------------------
  SendDataToPanel refresh the attribute list or show the value panels
  params:
  returns:
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::SendDataToPanel( AmayaParams& p )
{
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  wxColour            col;
  char                string_size[50];

  if (Current_Color != -1)
    {
      TtaGiveThotRGB (Current_Color, &red, &green, &blue);
      col = wxColour ( red, green, blue );
      XRCCTRL(*this, "wxID_PANEL_CSS_COLOR", AmayaColorButton)->SetColour( col );
    }
  if (Current_BackgroundColor != -1)
    {
      TtaGiveThotRGB (Current_BackgroundColor, &red, &green, &blue);
      col = wxColour ( red, green, blue );
    }
  else
    col = GetBackgroundColour();
  XRCCTRL(*this, "wxID_PANEL_CSS_BK_COLOR", AmayaColorButton)->SetColour( col );  

  if (Current_StrokeColor != -1)
    {
      TtaGiveThotRGB (Current_StrokeColor, &red, &green, &blue);
      col = wxColour ( red, green, blue );
      XRCCTRL(*this, "wxID_SVG_STROKE_COLOR", AmayaColorButton)->SetColour( col );
    }
  if (Current_FillColor != -1)
    {
      TtaGiveThotRGB (Current_FillColor, &red, &green, &blue);
      col = wxColour ( red, green, blue );
    }
  else
    col = GetBackgroundColour();
  XRCCTRL(*this, "wxID_SVG_FILL_COLOR", AmayaColorButton)->SetColour( col );  

  if (Current_Opacity != -1)
    XRCCTRL(*this, "wxID_SPIN_SVG_OPACITY", wxSpinCtrl)->SetValue(Current_Opacity);

  if (Current_StrokeOpacity != -1)
    XRCCTRL(*this, "wxID_SPIN_SVG_STROKE_OPACITY", wxSpinCtrl)->SetValue(Current_StrokeOpacity);
  
  if (Current_FillOpacity != -1)
    XRCCTRL(*this, "wxID_SPIN_SVG_FILL_OPACITY", wxSpinCtrl)->SetValue(Current_FillOpacity);

  if (Current_StrokeWidth != -1)
    XRCCTRL(*this, "wxID_SPIN_SVG_STROKE_WIDTH", wxSpinCtrl)->SetValue(Current_StrokeWidth);
  
  XRCCTRL(*this, "wxID_SVG_FILL_NONE", wxCheckBox)->SetValue(FillEnabled);
  XRCCTRL(*this, "wxID_SVG_STROKE_NONE", wxCheckBox)->SetValue(StrokeEnabled); 

  if (Current_FontFamily > 0)
    {
      switch (Current_FontFamily)
        {
        case 3:
          XRCCTRL(*this, "wxID_CHOICE_FAMILY", wxChoice)->SetStringSelection(TtaConvMessageToWX("Courier"));
          break;
        case 2:
          XRCCTRL(*this, "wxID_CHOICE_FAMILY", wxChoice)->SetStringSelection(TtaConvMessageToWX("Arial"));
          break;
        default:
          XRCCTRL(*this, "wxID_CHOICE_FAMILY", wxChoice)->SetStringSelection(TtaConvMessageToWX("Times"));
        }
    }
  if (Current_FontSize > 0)
    {
      sprintf (string_size, "%d", Current_FontSize);
      XRCCTRL(*this, "wxID_COMBO_SIZE", wxComboBox)->SetValue(TtaConvMessageToWX(string_size));
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnChooseFontFamily(wxCommandEvent& event)
{
  int num = event.GetSelection() + 1;

  if (num == 0)
    num = 1;
  if (num != Current_FontFamily)
    {
      Document doc;
      View view;
      
      Current_FontFamily = (int)num;

      TtaGiveActiveView( &doc, &view );
      /* force the refresh */
      if (doc > 0)
        TtaExecuteMenuAction ("DoSelectFontFamily", doc, view, TRUE);
    }
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnChooseFontSize(wxCommandEvent& event)
{
  long size;

   if (!event.GetString().ToLong(&size))
     size = 12;
   if (size != Current_FontSize)
     {
       Document doc;
       View view;

       Current_FontSize = (int)size;
       TtaGiveActiveView( &doc, &view );
       /* force the refresh */
       if (doc > 0)
         TtaExecuteMenuAction ("DoSelectFontSize", doc, view, TRUE);
     }

}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::SpinValueHasChanged(int id)
{
  Document doc;
  View view;

  switch(id)
    {
    case OPACITY:
      Current_Opacity = XRCCTRL(*this, "wxID_SPIN_SVG_OPACITY", wxSpinCtrl)->GetValue();
      break;
      
    case STROKE_OPACITY:
      Current_StrokeOpacity = XRCCTRL(*this, "wxID_SPIN_SVG_STROKE_OPACITY", wxSpinCtrl)->GetValue();
      break;
      
    case FILL_OPACITY:
      Current_FillOpacity = XRCCTRL(*this, "wxID_SPIN_SVG_FILL_OPACITY", wxSpinCtrl)->GetValue();
      break;
      
    case STROKE_WIDTH:
      Current_StrokeWidth = XRCCTRL(*this, "wxID_SPIN_SVG_STROKE_WIDTH", wxSpinCtrl)->GetValue();
      break;
    }

  TtaGiveActiveView( &doc, &view );

  if(doc > 0)
    {
      switch(id)
	{
	case OPACITY:
	  TtaExecuteMenuAction ("DoSelectOpacity", doc, view, TRUE);
	  XRCCTRL(*this, "wxID_SPIN_SVG_OPACITY", wxSpinCtrl)->SetFocus();
	  break;

	case STROKE_OPACITY:
	  TtaExecuteMenuAction ("DoSelectStrokeOpacity", doc, view, TRUE);
	  XRCCTRL(*this, "wxID_SPIN_SVG_STROKE_OPACITY", wxSpinCtrl)->SetFocus();
	  break;

	case FILL_OPACITY:
	  TtaExecuteMenuAction ("DoSelectFillOpacity", doc, view, TRUE);
	  XRCCTRL(*this, "wxID_SPIN_SVG_FILL_OPACITY", wxSpinCtrl)->SetFocus();
	  break;

	case STROKE_WIDTH:
	  TtaExecuteMenuAction ("DoSelectStrokeWidth", doc, view, TRUE);
	  XRCCTRL(*this, "wxID_SPIN_SVG_STROKE_WIDTH", wxSpinCtrl)->SetFocus();
	  break;
	}
    }

}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnChooseOpacity(wxCommandEvent& event)
{
  SpinValueHasChanged(OPACITY);
}
void AmayaStyleToolPanel::OnChooseOpacity2(wxSpinEvent& event)
{
  SpinValueHasChanged(OPACITY);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnChooseStrokeOpacity(wxCommandEvent& event)
{
  SpinValueHasChanged(STROKE_OPACITY);
}
void AmayaStyleToolPanel::OnChooseStrokeOpacity2(wxSpinEvent& event)
{
  SpinValueHasChanged(STROKE_OPACITY);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnChooseFillOpacity(wxCommandEvent& event)
{
  SpinValueHasChanged(FILL_OPACITY);
}
void AmayaStyleToolPanel::OnChooseFillOpacity2(wxSpinEvent& event)
{
  SpinValueHasChanged(FILL_OPACITY);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnChooseStrokeWidth(wxCommandEvent& event)
{
  SpinValueHasChanged(STROKE_WIDTH);
}
void AmayaStyleToolPanel::OnChooseStrokeWidth2(wxSpinEvent& event)
{
  SpinValueHasChanged(STROKE_WIDTH);
}


/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnUpdateFillUI(wxUpdateUIEvent& event)
{
  event.Enable(XRCCTRL(*this, "wxID_SVG_FILL_NONE", wxCheckBox)->IsChecked());
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnUpdateStrokeUI(wxUpdateUIEvent& event)
{
  Document doc;
  View view;
  TtaGiveActiveView( &doc, &view );

  event.Enable(XRCCTRL(*this, "wxID_SVG_STROKE_NONE", wxCheckBox)->IsChecked());  
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnUpdateFill(wxCommandEvent&event)
{
  Document doc;
  View view;
  TtaGiveActiveView( &doc, &view );
  if(doc > 0)
    TtaExecuteMenuAction ("DoUpdateFillStatus", doc, view, TRUE);
}

/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
void AmayaStyleToolPanel::OnUpdateStroke(wxCommandEvent&event)
{
  Document doc;
  View view;
  TtaGiveActiveView( &doc, &view );
  if(doc > 0)
    TtaExecuteMenuAction ("DoUpdateStrokeStatus", doc, view, TRUE);
}

/*----------------------------------------------------------------------
 *  this is where the event table is declared
 *  the callbacks are assigned to an event type
 *----------------------------------------------------------------------*/
BEGIN_EVENT_TABLE(AmayaStyleToolPanel, AmayaToolPanel)

/* SVG colors */
  EVT_AMAYA_COLOR_CHANGED(XRCID("wxID_SVG_STROKE_COLOR"), AmayaStyleToolPanel::OnColorStrokePalette )
  EVT_BUTTON(XRCID("wxID_BUTTON_SVG_STROKE_COLOR"), AmayaStyleToolPanel::OnChooseStrokeColor)
  EVT_AMAYA_COLOR_CHANGED(XRCID("wxID_SVG_FILL_COLOR"), AmayaStyleToolPanel::OnColorFillPalette )
  EVT_BUTTON(XRCID("wxID_BUTTON_SVG_FILL_COLOR"), AmayaStyleToolPanel::OnChooseFillColor)

/* SVG spin control */
  EVT_SPINCTRL(XRCID("wxID_SPIN_SVG_OPACITY"), AmayaStyleToolPanel::OnChooseOpacity2)
  EVT_SPINCTRL(XRCID("wxID_SPIN_SVG_STROKE_OPACITY"), AmayaStyleToolPanel::OnChooseStrokeOpacity2)
  EVT_SPINCTRL(XRCID("wxID_SPIN_SVG_FILL_OPACITY"), AmayaStyleToolPanel::OnChooseFillOpacity2)
  EVT_SPINCTRL(XRCID("wxID_SPIN_SVG_STROKE_WIDTH"), AmayaStyleToolPanel::OnChooseStrokeWidth2)
  EVT_TEXT_ENTER(XRCID("wxID_SPIN_SVG_OPACITY"), AmayaStyleToolPanel::OnChooseOpacity)
  EVT_TEXT_ENTER(XRCID("wxID_SPIN_SVG_STROKE_OPACITY"), AmayaStyleToolPanel::OnChooseStrokeOpacity)
  EVT_TEXT_ENTER(XRCID("wxID_SPIN_SVG_FILL_OPACITY"), AmayaStyleToolPanel::OnChooseFillOpacity)
  EVT_TEXT_ENTER(XRCID("wxID_SPIN_SVG_STROKE_WIDTH"), AmayaStyleToolPanel::OnChooseStrokeWidth)

  
  EVT_UPDATE_UI(XRCID("wxID_BUTTON_SVG_FILL_COLOR"), AmayaStyleToolPanel::OnUpdateFillUI)
  EVT_UPDATE_UI(XRCID("wxID_SPIN_SVG_FILL_OPACITY"), AmayaStyleToolPanel::OnUpdateFillUI)
  EVT_UPDATE_UI(XRCID("wxID_BUTTON_SVG_STROKE_COLOR"), AmayaStyleToolPanel::OnUpdateStrokeUI)
  EVT_UPDATE_UI(XRCID("wxID_SPIN_SVG_STROKE_OPACITY"), AmayaStyleToolPanel::OnUpdateStrokeUI)
  EVT_UPDATE_UI(XRCID("wxID_SPIN_SVG_STROKE_WIDTH"), AmayaStyleToolPanel::OnUpdateStrokeUI)
  
  EVT_CHECKBOX(XRCID("wxID_SVG_FILL_NONE"), AmayaStyleToolPanel::OnUpdateFill)
  EVT_CHECKBOX(XRCID("wxID_SVG_STROKE_NONE"), AmayaStyleToolPanel::OnUpdateStroke)

/* HTML Colors */
  EVT_AMAYA_COLOR_CHANGED(XRCID("wxID_PANEL_CSS_COLOR"), AmayaStyleToolPanel::OnColorFontPalette )
  EVT_BUTTON(XRCID("wxID_BUTTON_TEXTCOLOR"), AmayaStyleToolPanel::OnChooseFontColor)
  EVT_AMAYA_COLOR_CHANGED(XRCID("wxID_PANEL_CSS_BK_COLOR"), AmayaStyleToolPanel::OnColorBackgroundPalette )
  EVT_BUTTON(XRCID("wxID_BUTTON_BKCOLOR"), AmayaStyleToolPanel::OnChooseBackgroundColor)

  EVT_CHOICE(XRCID("wxID_PANEL_CSS_THEME"), AmayaStyleToolPanel::OnThemeChange)

  EVT_CHOICE(XRCID("wxID_CHOICE_FAMILY"), AmayaStyleToolPanel::OnChooseFontFamily)
  EVT_COMBOBOX(XRCID("wxID_COMBO_SIZE"), AmayaStyleToolPanel::OnChooseFontSize)
  EVT_TEXT_ENTER(XRCID("wxID_COMBO_SIZE"), AmayaStyleToolPanel::OnChooseFontSize)

END_EVENT_TABLE()

#endif /* #ifdef _WX */
