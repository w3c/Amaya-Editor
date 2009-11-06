/*
 *
 *  (c) COPYRIGHT INRIA, 1996-2008
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifdef _WX
#include "wx/wx.h"
#include "wx/xrc/xmlres.h"              // XRC XML resouces
#include "wx/utils.h"
#include "wx/dcmemory.h"

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
#include "editcommands_f.h"
#include "units_f.h"

#define THOT_EXPORT extern
#include "frame_tv.h"
#include "paneltypes_wx.h"
#include "registry_wx.h"

#include "AmayaSpeCharPanel.h"
#include "AmayaNormalWindow.h"
#include "AmayaToolBar.h"
#include "displayview_f.h"
#include "font_f.h"
#include "openglfont.h"


/**
 * Defines entries for SpecChar panel.
 */
typedef struct
{
  int msg; // Message ident in LIB
  const char* icon_path; // Path to icon
  AmayaSpeChar* table; // address of spechar table
}SpecialCharEntry;

/* Pour retrouver les carateres correspondants aux nombres hexadecimaux
   il suffit d'aller a la page web du site unicode :
   http://www.unicode.org/Public/UNIDATA/UnicodeData.txt*/

extern AmayaSpeChar filtre_greek[];
extern AmayaSpeChar filtre_greek_maj[];
extern AmayaSpeChar filtre_maths[];
extern AmayaSpeChar filtre_operateurs[];
extern AmayaSpeChar filtre_relations_binaires[];
extern AmayaSpeChar filtre_relations_binaires_negation[];
extern AmayaSpeChar filtre_divers[];
extern AmayaSpeChar filtre_fleches[];

extern SpecialCharEntry special_char_entries[]; 

//
//
// AmayaSpeCharToolPanel
//
//

AmayaSpeCharBitmapMap AmayaSpeCharToolPanel::s_bitmapMap;



#define MAX_TOOL_PER_LINE 8

IMPLEMENT_DYNAMIC_CLASS(AmayaSpeCharToolPanel, AmayaToolPanel)

BEGIN_EVENT_TABLE(AmayaSpeCharToolPanel, AmayaToolPanel)
  EVT_TOOL(wxID_ANY, AmayaSpeCharToolPanel::OnTool)
END_EVENT_TABLE()

/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharToolPanel
 *      Method:  Create
-----------------------------------------------------------------------*/
AmayaSpeCharToolPanel::AmayaSpeCharToolPanel():
  AmayaToolPanel(),
  m_pBook(NULL),
  m_imagelist(16,16)
{
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharToolPanel
 *      Method:  Create
-----------------------------------------------------------------------*/
bool AmayaSpeCharToolPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
          const wxSize& size, long style, const wxString& name, wxObject* extra)
{
  if(!wxPanel::Create(parent, id, pos, size, style, name))
    return false;
#ifdef _WINDOWS
  SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
#endif /* _WINDOWS */
  Initialize();
  return true;
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharToolPanel
 *      Method:  GetToolPanelName
-----------------------------------------------------------------------*/
wxString AmayaSpeCharToolPanel::GetToolPanelName()const
{
  return TtaConvMessageToWX(TtaGetMessage(LIB,TMSG_SPECHAR));
}


/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharToolPanel
 *      Method:  GetDefaultAUIConfig
 * Description:  Return a default AUI config for the panel.
 -----------------------------------------------------------------------*/
wxString AmayaSpeCharToolPanel::GetDefaultAUIConfig()
{
  return wxT("dir=2;layer=0;row=0;pos=2");
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharToolPanel
 *      Method:  GetToolPanelName
-----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::Initialize()
{
  SpecFont  fontset;
  wxSizer*  sz = new wxBoxSizer(wxVERTICAL);

  m_pBook = new wxChoicebook(this, wxID_ANY);
  sz->Add(m_pBook, 1, wxEXPAND);
  SetSizer(sz);
  
  InitializeBitmapMap();
  
  m_pBook->SetImageList(&m_imagelist);
  
  SpecialCharEntry *entry = special_char_entries;
  while(entry->table!=NULL)
    {
      AmayaSpeChar *car = entry->table;
      wxBitmap bmp;
      bmp.LoadFile(TtaGetResourcePathWX( WX_RESOURCES_ICON_16X16, entry->icon_path), wxBITMAP_TYPE_ANY);
      int img = m_imagelist.Add(bmp);
      
      wxPanel*   panel = new wxPanel(m_pBook, wxID_ANY);
      wxToolBar* tb = NULL;
      m_pBook->AddPage(panel, TtaConvMessageToWX(TtaGetMessage(LIB,entry->msg)), false, img);
      sz = new wxBoxSizer(wxVERTICAL);
      int line = 0;
      fontset = ThotLoadFont ('L', 1, 0, 12, UnPixel, 1);
      while (car->unicode != -1)
        {
          if (++line >= MAX_TOOL_PER_LINE || !tb)
            {
              if(tb)
                tb->Realize();
              tb = new wxToolBar(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                  wxTB_HORIZONTAL|wxNO_BORDER|/*wxTB_TEXT|wxTB_NOICONS|*/wxTB_NODIVIDER);
              tb->SetToolBitmapSize(wxSize(16,16));
              tb->SetToolPacking(4);
              sz->Add(tb, 0);
              line = 0;
            }
          int toolid = wxNewId();
          m_hash[toolid] = car;
          wxString str = wxChar(car->unicode);
          tb->AddTool(toolid, str, s_bitmapMap[car->unicode],
                      TtaConvMessageToWX(car->name));
          car++;
        }
      entry++;
      panel->SetSizer(sz);
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharToolPanel
 *      Method:  InitializeBitmapMap
-----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::InitializeBitmapMap()
{
  ThotFont  font;
  SpecFont  fontset;
  int       index;
  unsigned char     *data;

  if(s_bitmapMap.empty())
    {
      SpecialCharEntry *entry = special_char_entries;
      while(entry->table!=NULL)
        {
          AmayaSpeChar *car = entry->table;
          fontset = ThotLoadFont ('L', 1, 0, 12, UnPixel, 1);
          while (car->unicode != -1)
            {
              index = GetFontAndIndexFromSpec (car->unicode, fontset, 0, &font);
              data = GetCharacterGlyph ((GL_font *)font, index, 16, 16);
              wxImage img(16, 16, data, false);          
              s_bitmapMap[car->unicode] = wxBitmap(img);
              car++;
            }
          entry++;
        }
      // now free loaded fonts
      ThotFreeFont (1);
    }
}

/*----------------------------------------------------------------------
 *       Class:  AmayaSpeCharToolPanel
 *      Method:  OnTool
 -----------------------------------------------------------------------*/
void AmayaSpeCharToolPanel::OnTool(wxCommandEvent& event)
{
  Document doc;
  int      view, value;

  value = m_hash[event.GetId()]->unicode;
  FrameToView (TtaGiveActiveFrame(), &doc, &view);
  TtcInsertChar (doc, view, value);
  CloseTextInsertion ();
  TtaRedirectFocus();
}


//
//
// Special character lists.
//
//

/* Pour retrouver les carateres correspondants aux nombres hexadecimaux
   il suffit d'aller a la page web du site unicode :
   http://www.unicode.org/Public/UNIDATA/UnicodeData.txt*/

AmayaSpeChar filtre_greek[] = 
{
  {0x3b1, "alpha"},
  {0x3b2, "beta"},
  {0x3b3, "gamma"},
  {0x3b4, "delta"},
  {0x3b5, "epsilon"},
  {0x3b6, "zeta"},
  {0x3b7, "eta"},
  {0x3b8, "theta"},
  {0x3b9, "iota"},
  {0x3ba, "kappa"},
  {0x3bb, "lamda"},
  {0x3bc, "mu"},
  {0x3bd, "nu"},
  {0x3be, "xi"},
  {0x3bf, "omicron"},
  {0x3c0, "pi"},
  {0x3c1, "rho"},
  {0x3c2, "final sigma"},
  {0x3c3, "sigma"},
  {0x3c4, "tau"},
  {0x3c5, "upsilon"},
  {0x3c6, "phi"},
  {0x3c7, "chi"},
  {0x3c8, "psi"},
  {0x3c9, "omega"},
  {0x3ca, "iota with dialytika"},
  {0x3cb, "upsilon with dialytika"},
  {0x3cc, "omicron with tonos"},
  {0x3cd, "upsilon with tonos"},
  {0x3ce, "omega with tonos"},
  {0x3d0, "curled beta"},
  {0x3d1, "script theta"},
  {0x3d2, "upsilon with hook"},
  {0x3d3, "upsilon with acute and hook"},
  {0x3d4, "upsilon with diaeresis and hook"},
  {0x3d5, "script phi"},
  {0x3d6, "omega pi"},
  {0x3d7, "kai"},
  {0x3db, "stigma"},
  {0x3dd, "digamma"},
  {0x3df, "koppa"},
  {0x3e1, "sampi"},
  {0x3f0, "script kappa"},
  {0x3f1, "tailed rho"},
  {0x3f2, "lunate sigma"},
  {0x3f3, "yot"},
  {0x3ac, "alpha with tonos"},
  {0x3ad, "epsilon with tonos"},
  {0x3ae, "eta with tonos"},
  {0x3af, "iota with tonos"},
  {0x3b0, "upsilon with dialytika and tonos"},
   {0x3db, "stigma"},
  {-1, ""}
};


AmayaSpeChar filtre_greek_maj[] =
  {
    {0x391, "ALPHA"}, 
    {0x392, "BETA"},
    {0x393, "GAMMA"},
    {0x394, "DELTA"},
    {0x395, "EPSILON"},
    {0x396, "ZETA"},
    {0x397, "ETA"},
    {0x398, "THETA"},
    {0x399, "IOTA"},
    {0x39a, "KAPPA"},
    {0x39b, "LAMDA"},
    {0x39c, "MU"},
    {0x39d, "NU"},
    {0x39e, "XI"},
    {0x39f, "OMICRON"},
    {0x3a0, "PI"},
    {0x3a1, "RHO"},
    {0x3a3, "SIGMA"},
    {0x3a4, "TAU"},
    {0x3a5, "UPSILON"},
    {0x3a6, "PHI"},
    {0x3a7, "CHI"},
    {0x3a8, "PSI"},
    {0x3a9, "OMEGA"},
    {0x3aa, "IOTA WITH DIALYTIKA"},
    {0x3ab, "UPSILON WITH DIALYTIKA"},
    {0x386, "APLHA WITH TONOS"},
    {0x388, "EPSILON WITH TONOS"},
    {0x389, "ETA WITH TONOS"},
    {0x38a, "IOTA WITH TONOS"},
    {0x38c, "OMICRON WITH TONOS"},
    {0x38e, "UPSILON WITH DIALYTIKA AND TONOS"},
    {0x38f, "OMEGA WITH TONOS"},
    {0x3da, "STIGMA"},
    {0x3dc, "DIGAMA"},
    {0x3de, "KOPPA"},
    {0x3e0, "SAMPI"},
    {-1, ""}
  };

AmayaSpeChar filtre_maths[] =
  {
    {0x221d, "proportional"},
    {0x221e, "infinity"},
    {0x2113, "liter"},
    {0x210E, "planck constant"},
    {0x210F, "planck constant over 2 pi"},
    {0x2118, "Weierstrass elliptic function"},
    {0x2200, "for all"},
    {0x2203, "there exists"},
    {0x2204, "there does not exist"},
    {0x2215, "division slash"},
    {0x2216, "set minus"},
    {0x22A2, "right tack"},
    {0x22A3, "left tack"},
    {0x22A4, "down tack"},
    {0x22a5, "up tack"},
    {0x019B, "barred lambda"},
    {0x00F0, "icelandic"},
    {0x2220, "angle"},
    {0x2221, "measured angle"},
    {0x2222, "spherical angle"},
    {0x2112, "script L"},
    {0x2131, "script F"},
    {0x2201, "complement"},
    {0x2132, "turned F"},
    {0x2141, "turned G"},
    {0x2225, "parallel to"},
    {0x2226, "not parallel to"},
    {0x2209, "not an element of"},
    {0x2208, "element of"},
    {0x220A, "small element of"},
    {0x220B, "contains as member"},
    {0x220d, "small contains as member"},
    {0x220C, "does not contain as member"},
    {0x2102, "double-struck C"},
    {0x2115, "double-struck N"},
    {0x2119, "double-struck P"},
    {0x211A, "double-struck Q"},
    {0x211D, "double-struck R"},
    {0x2124, "double-struck Z"},
    {0x2135, "aleph"},
    {0x2136, "bet"},
    {0x2137, "gimel"},
    {0x2138, "dalet"},
    {0x2103, "degree celsius"},
    {0x2109, "degree fahrenheit"},
    {0x210B, "script H"},
    {0x212B, "angstrom unit"},
    {0x212F, "script e"},
    {0x212E, "estimated symbol"},
    {0x00B5, "micro"},
    {0x2127, "inverted ohm"},
    {0x2129, "inverted iota"},
    {0x212A, "kelvin sign"},
    {0x2223, "divides"},
    {0x220f, "n-ary product"},
    {0x22EE, "vertical ellipsis"},
    {0x22EF, "midline horizontal ellipsis"},
    {0x22F0, "up right diagonal ellipsis"},
    {0x22F1, "down right diagonal ellipsis"},
    {-1, ""}
  };

AmayaSpeChar filtre_operateurs[] =
  {
    {0x2207, "nabla"},
    {0x2202, "Partial differential"},
    {0x221a, "quare root"},
    {0x00B1, "plus or minus"},
    {0x2213, "minus or plus"},
    {0x2208, "element of"},
    {0x2209, "not an element of"},
    {0x2217, "asterisk operator"},
    {0x002B, "plus"},
    {0x2212, "minus"},
    {0x002E, "period"},
    {0x2044, "fraction slash"},
    {0x00F7, "division"},
    {0x00D7, "multiplication"},
    {0x2022, "bullet"},
    {0x2229, "intersection"},
    {0x222a, "union"},
    {0x2218, "ring"},
    {0x2293, "square cap"},
    {0x2294, "square cup"},
    {0x2216, "set minus"},
    {0x2295, "circled plus"},
    {0x2297, "circled times"},
    {0x2205, "empty set"},
    {0x2296, "circled minus"},
    {0x2299, "circled dot operator"},
    {0x229B, "circled asterisk operator"},
    {0x2298, "circled division slash"},
    {0x229A, "circled ring operator"},
    {0x2227, "logical and"},
    {0x2228, "logical or"},
    {0x22B2, "normal subgroup of"},
    {0x22B3, "contains as normal subgroup"},
    {0x22BB, "xor"},
    {0x22BD, "nor"},
    {0x2240, "wreath product"},
    {0x2306, "perspective"},
    {0x22B4, "normal subgroup of or equal to"},
    {0x22B5, "contains as normal subgroup or equal to"},
    {0x229E, "squared plus"},
    {0x229F, "squared minus"},
    {0x22A0, "squared times"},
    {0x22A1, "squared dot operator"},
    {0x22D3, "double union"},
    {0x22D2, "double intersection"},
    {0x22C9, "left normal factor semidirect product"},
    {0x22CA, "right normal factor semidirect product"},
    {0x22CE, "curly logical or"},
    {0x22CF, "curly logical and"},
    {0x22CB, "left semidirect product"},
    {0x22CC, "right semidirect product"},
    {0x2214, "dot plus"},
    {0x22BA, "intercalate"},
    {0x22C7, "division times"},
    {0x00B7, "middle dot"},
    {0x222b, "integral"},
    {0x222C, "double integral"},
    {0x222D, "triple integral"},
    {0x222E, "contour integral"},
    {0x222F, "surface integral"},
    {0x2230, "volume integral"},
    {0x2231, "clockwise integral"},
    {0x2232, "clockwise contour integral"},
    {0x2233, "anticlockwise contour integral"},
    {0x2211, "n-ary summation"},
    {0x220F, "n-ary product"},
    {-1, ""}
  };

AmayaSpeChar filtre_relations_binaires[] =
  {
    {0x2264, "less-than or equal to"},
    {0x2265, "greater-thar or equal to"},
    {0x003C, "less-than"},
    {0x003E, "greater-than"},
    {0x003D, "equals"},
    {0x2208, "element of"},
    {0x220B, "contains as member"},
    {0x2245, "approximately equal to"},
    {0x2248, "almost equal to"},
    {0x223c, "tilde"},
    {0x2261, "identical to"},
    {0x224A, "almost equal or equal to"},
    {0x2243, "asymptotically equal to"},
    {0x227A, "precedes"},
    {0x227C, "precedes or equal to"},
    {0x227B, "succeeds"},
    {0x227D, "succeeds or equal to"},
    {0x226A, "much less-than"},
    {0x226B, "much greater-than"},
    {0x2282, "subset of"},
    {0x2286, "subset of or equal to"},
    {0x2283, "superset of"},
    {0x2287, "superset or equal to"},
    {0x228F, "sqare image of"},
    {0x2291, "sqare image of or equal to"},
    {0x2290, "square original of"},
    {0x2292, "square original of or equal to"},
    {0x22A2, "right tack"},
    {0x224D, "equivalent to"},
    {0x22A3, "left tack"},
    {0x2225, "parallel to"},
    {0x2223, "divides"},
    {0x22a5, "up tack"},
    {0x22C8, "bowtie"},
    {0x22A8, "true"},
    {0x2250, "approaches the limit"},
    {0x22A9, "forces"},
    {0x22AA, "triple vertical bar right turnstile"},
    {0x22A7, "models"},
    {0x22AB, "double vertical bar double right turnstile"},
    {0x2257, "ring equal to"},
    {0x227E, "precedes or equivalent to"},
    {0x2272, "less-than or equivalent to"},
    {0x2273, "greater-than or equivalent to"},
    {0x227C, "precedes or equal to"},
    {0x227D, "succeeds or equal to"},
    {0x22DE, "equal to or precedes"},
    {0x22DF, "equal to or cucceeds"},
    {0x2266, "less-than over equal to"},
    {0x2267, "greater-than over equal to"},
    {0x2276, "less-than or greater-than"},
    {0x2277, "greater-than or less-than"},
    {0x2252, "approximately equal to or the image of"},
    {0x2253, "image of or approximately equal to"},
    {0x226C, "between"},
    {0x2256, "ring in equal to"},
    {0x22D0, "double subset"},
    {0x22D1, "double superset"},
    {0x224F, "difference between"},
    {0x224E, "geometrically equivalent to"},
    {0x22D8, "very much less-than"},
    {0x22D9, "very much greater-than"},
    {0x223E, "inverted lazy s"},
    {0x22D4, "pitchfork"},
    {0x2242, "minus tilde"},
    {0x2243, "asymptotically equal to"},
    {0x22D6, "less-than with dot"},
    {0x22D7, "greater-than with dot"},
    {0x22DA, "less-than equal to or greater-than"},
    {0x22DB, "greater-than equal to or less-than"},
    {-1, ""}
  };

AmayaSpeChar filtre_relations_binaires_negation[] =
  {
    {0x2284, "not a subset of"},
    {0x2285, "not a superset of"},
    {0x2209, "not an element of"},
    {0x220C, "doesn't contain as member"},
    {0x2260, "not equal to"},
    {0x2262, "not identical to"},
    {0x2268, "less-than but not equal to"},
    {0x2269, "greater-than but not equal to"},
    {0x2270, "neither less-than nor equal to"},
    {0x2271, "neither greater-than nor equal to"},
    {0x226E, "not less-than"},
    {0x226F, "not greater-than"},
    {0x2280, "does not precede"},
    {0x2281, "does not succeed"},
    {0x22E8, "precedes but not equivalent to"},
    {0x22E9, "succeeds but not equivalent to"},
    {0x2241, "not tilde"},
    {0x2247, "neither approximately not actually equal to"},
    {0x228A, "subset of with not equal to"},
    {0x228B, "superset of with not equal to"},
    {0x2288, "neither a subset of nor equal to"},
    {0x2289, "neither a superset of nor equal to"},
    {0x2226, "not parallel to"},
    {0x2224, "does not divide"},
    {0x22AC, "does not prove"},
    {0x22AD, "not true"},
    {0x22AE, "does not force"},
    {0x22AF, "negated double vertical bar double right turnstile"},
    {0x22EA, "not normal subgroup of"},
    {0x22EB, "does not contain as normal subgroup"},
    {0x22EC, "not normal subgroup of or equal to"},
    {0x22ED, "does not contain as normal subgroup or equal"},
    {0x22E2, "not square image of or equal"},
    {0x22E3, "not square original of or equal"},
    {0x22E4, "square image of or not equal to"},
    {0x22E5, "square original of or not equal to"},
    {-1, ""}
  };

AmayaSpeChar filtre_divers[] =
  {
    {0x00A0, "non-breaking space"},
    {0x00BF, "inverted question mark"},
    {0x2018, "left single quotation mark"},
    {0x2019, "right single quotation mark"},
    {0x201C, "left double quotation mark"},
    {0x201D, "right double quotation mark"},
    {0x00AB, "left guillemet"}, /* << */
    {0x00BB, "right guillemet"}, /* >> */

    {0x00A1, "inverted exclamation mark"},
    {0x2030, "per mille sign"},
    {0x00B0, "degree sign"},
    {0x2026, "horizontal ellipsis"},
    {0x2013, "en dash"},
    {0x2014, "em dash"},
    {0x00F7, "division sign"},
    {0x00A6, "broken bar"},

    {0x0153, "oe"},
    {0x0152, "OE"},
    {0x00E6, "ae"},
    {0x00C6, "AE"},
    {0x00FE, "thorn"},
    {0x00DE, "THORN"},
    {0x00DF, "eszett"},
    {0x017F, "long s"},

    {0x20AC, "euro"},
    {0x00A2, "cent"},
    {0x00A3, "pound"},
    {0x00A5, "yen"},
    {0x0024, "dollar"},
    {0x00A4, "currency"},
    {0x2020, "dagger"},
    {0x2021, "double dagger"},

    {0x00B6, "paragraph sign"},
    {0x00A7, "section"},
    {0x00A9, "copyright"},
    {0x00AE, "registered"},
    {0x2122, "trade mark"},
    {0x2234, "therefore"},
    {0x00D8, "O slash"},
    {0x2720, "maltese cross"},

    {0x2605, "black star"},
    {0x2606, "white star"},
    {0x2663, "black club suit"},
    {0x2666, "black diamond suit"},
    {0x2665, "black heart suit"},
    {0x2660, "black spade suit"},
    {0x25A0, "black square"},
    {0x25A1, "white square"},

    {0x25B2, "black up-pointing triangle"},
    {0x25B3, "white up-pointing triangle"},
    {0x25BC, "black down-pointing triangle"},
    {0x25BD, "white down-pointing triangle"},
    {0x25C6, "black diamond"},
    {0x25C7, "white diamond"},
    {0x25CB, "white circle"},
    {0x25CF, "black circle"},

    {0x25D0, "circle with left half"},
    {0x25D1, "circle with right half"},
    {0x25D2, "circle with lower half"},
    {0x25D3, "circle with upper half"},
    {0x25ca, "lozenge"},
    {0x25B1, "white parallelogram"},
    {0x260E, "black telephone"},
    {0x2460, "circled 1"},
    {-1, ""}
  };

AmayaSpeChar filtre_fleches[] =
  {
    {0x2190, "leftwards"},
    {0x2191, "upwards"},
    {0x2192, "rightwards"},
    {0x2193, "downwards"},
    {0x2194, "left right"},
    {0x2195, "up down"},
    {0x2196, "north west"},
    {0x2197, "north east"},
    {0x2198, "south east"},
    {0x2199, "south west"},
    {0x219A, "leftwards with stroke"},
    {0x219B, "rightwards with stroke"},
    {0x219C, "leftwards wave"},
    {0x219D, "rightwards wave"},
    {0x219E, "leftwards two headed"},
    {0x219F, "upwards two headed"},
    {0x21A0, "rightwards two headed"},
    {0x21A1, "downwards two headed"},
    {0x21A2, "leftwards with tail"},
    {0x21A3, "rightwards with tail"},
    {0x21A4, "leftwards from bar"},
    {0x21A5, "upwards from bar"},
    {0x21A6, "rightwards from bar"},
    {0x21A7, "downwards from bar"},
    {0x21A8, "up down with base"},
    {0x21A9, "leftwards with hook"},
    {0x21AA, "rightwards with hook"},
    {0x21AB, "leftwards with loop"},
    {0x21AC, "rightwards with loop"},
    {0x21AD, "left right wave"},
    {0x21AE, "left right with stroke"},
    {0x21AF, "downwards zigzag"},
    {0x21B0, "upwards with tip leftwards"},
    {0x21B1, "upwards with tip rightwards"},
    {0x21B2, "downwards with tip leftwards"},
    {0x21B3, "downwards with tip rightwards"},
    {0x21B4, "rightwards with corner downwards"},
    {0x21B5, "downwards with corner leftwards"},
    {0x21B6, "anticlockwise top semicircle"},
    {0x21B7, "clockwise top semicircle"},
    {0x21B8, "north west to long bar"},
    {0x21B9, "leftwards to bar over rightwards to bar"},
    {0x21BA, "anticlockwise open circle"},
    {0x21BB, "clockwise open circle"},
    {0x21BC, "leftwards harpoon with barb upwards"},
    {0x21BD, "leftwards harpoon with barb downwards"},
    {0x21BE, "upwards harpoon with barb rightwards"},
    {0x21BF, "upwards harpoon with barb leftwards"},
    {0x21C0, "rightwards harpoon with barb upwards"},
    {0x21C1, "rightwards harpoon with barb downwards"},
    {0x21C2, "downwards harpoon with barb rightwards"},
    {0x21C3, "downwards harpoon with barb leftwards"},
    {0x21C4, "rightwards over leftwards"},
    {0x21C5, "upwards arrow leftwards of downwards arrow"},
    {0x21C6, "leftwards over rightwards"},
    {0x21C7, "leftwards pair"},
    {0x21C8, "upwards pair"},
    {0x21C9, "rightwards pair"},
    {0x21CA, "downwards pair"},
    {0x21CB, "leftwards harpoon over rightwards harpoon"},
    {0x21CC, "rightwards harpoon over leftwards harpoon"},
    {0x21CD, "leftwards double with stroke"},
    {0x21CE, "left right double with stroke"},
    {0x21CF, "rightwards double with stroke"},
    {0x21D0, "leftwards double"},
    {0x21D1, "upwards double"},
    {0x21D2, "rightwards double"},
    {0x21D3, "downwards double"},
    {0x21D4, "left right double"},
    {0x21D5, "up down double"},
    {0x21D6, "north west double"},
    {0x21D7, "north east double"},
    {0x21D8, "south east double"},
    {0x21D9, "south west double"},
    {0x21DA, "leftwards triple"},
    {0x21DB, "rightwards triple"},
    {0x21DC, "leftwards squiggle"},
    {0x21DD, "rightwards squiggle"},
    {0x21DE, "upwards with double stroke"},
    {0x21DF, "downwards with double stroke"},
    {-1, ""}
  };

  
SpecialCharEntry special_char_entries[]=
{
    {TMSG_MISC, "MATHML_F_divers.png", filtre_divers},
    {TMSG_GREEK_ALPHABET, "MATHML_F_greek.png", filtre_greek},
    {TMSG_GREEK_CAP, "MATHML_F_greek_maj.png", filtre_greek_maj},
    {TMSG_MATHML, "MATHML_F_maths.png", filtre_maths},
    {TMSG_OPERATOR, "MATHML_F_operateurs.png", filtre_operateurs},
    {TMSG_BINARY_REL, "MATHML_F_relations_binaires.png",
          filtre_relations_binaires},
    {TMSG_BINARY_REL_NEG, "MATHML_F_relations_negation.png",
          filtre_relations_binaires_negation},
    {TMSG_ARROW, "MATHML_F_fleches.png", filtre_fleches},
    {-1, NULL, NULL}
    
};


#endif /* #ifdef _WX */
