/*
 *
 *  (c) COPYRIGHT INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */
 
/*
 * document windows API
 *
 * Authors: I. Vatton (INRIA)
 *          D. Veillard (W3C/INRIA) Multikey + Event rewrite
 *          R. Guetari (W3C/INRIA) - Unicode and Windows version
 *
 */

/*#define DEBUG_MULTIKEY 1*/
#define OWN_XLOOKUPSTRING	/* Do NOT remove it, it change thot_gui.h includes */

#include "thot_gui.h"
#include "thot_sys.h"
#include "constmenu.h"
#include "constmedia.h"
/* Needed for a glitch with Motif 2.0 see ResizeEvents below ! */

#include "typemedia.h"
#include "picture.h"
#include "libmsg.h"
#include "dialog.h"
#include "application.h"
#include "document.h"
#include "view.h"
#include "interface.h"
#include "message.h"
#include "appdialogue.h"
#include "typecorr.h"
#include "frame_f.h"
#ifdef _WINDOWS
#include "wininclude.h"
#endif /* _WINDOWS */

#define MAX_ARGS 20

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "edit_tv.h"
#include "font_tv.h"
#include "boxes_tv.h"
#include "frame_tv.h"
#include "appdialogue_tv.h"

#include "actions_f.h"
#include "appdialogue_f.h"
#include "appli_f.h"
#include "applicationapi_f.h"
#include "callback_f.h"
#include "context_f.h"
#include "dialogapi_f.h"
#include "displaybox_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "input_f.h"
#include "keyboards_f.h"
#include "message_f.h"
#include "thotmsg_f.h"
#include "ustring_f.h"
#include "viewapi_f.h"
#include "views_f.h"


static ThotBool      Enable_Multikey;
static int           mk_state = 0;
static int           TtaKeyboardMapInstalled = 0;
static unsigned int  previous_state = 0;

#ifdef _WINDOWS
static CHAR_T        previous_keysym;
static UCHAR_T       previous_value = 0;
#else  /* !_WINDOWS */
static KeySym        previous_keysym;
static KeySym        previous_value = 0;
#endif /* !_WINDOWS */

#ifndef _WINDOWS
/*----------------------------------------------------------------------
   Handling of Multikey sequences used to produce ISO-Latin-1.
   The corresponding keysyms are installed in the keyboard map
   and a filter catch all keypressed event to detect sequence and
   dynamically change the event flow to produce event corresponding
   to the extended character set.
  ----------------------------------------------------------------------*/
/* #define DEBUG_KEYMAP *//* give debug information when installing keymap */
/* #define DEBUG_MULTIKEY *//* give debug information when using multikey */
static KeySym       TtaIsoKeySymTab[256] =
{
   XK_nobreakspace,		/* First keysyms are mapped directly  */
   XK_exclamdown,		/* From the upper part of Iso-Latin-1 */
   XK_cent, XK_sterling, XK_currency, XK_yen, XK_brokenbar, XK_section,
   XK_diaeresis, XK_copyright, XK_ordfeminine, XK_guillemotleft, XK_notsign,
XK_hyphen, XK_registered, XK_macron, XK_degree, XK_plusminus, XK_twosuperior,
   XK_threesuperior, XK_acute, XK_mu, XK_paragraph, XK_periodcentered,
 XK_cedilla, XK_onesuperior, XK_masculine, XK_guillemotright, XK_onequarter,
   XK_onehalf, XK_threequarters, XK_questiondown, XK_Agrave, XK_Aacute,
   XK_Acircumflex, XK_Atilde, XK_Adiaeresis, XK_Aring, XK_AE, XK_Ccedilla,
   XK_Egrave, XK_Eacute, XK_Ecircumflex, XK_Ediaeresis, XK_Igrave, XK_Iacute,
   XK_Icircumflex, XK_Idiaeresis, XK_ETH, XK_Eth, XK_Ntilde, XK_Ograve,
   XK_Oacute, XK_Ocircumflex, XK_Otilde, XK_Odiaeresis, XK_multiply,
   XK_Ooblique, XK_Ugrave, XK_Uacute, XK_Ucircumflex, XK_Udiaeresis,
   XK_Yacute, XK_THORN, XK_Thorn, XK_ssharp, XK_agrave, XK_aacute,
   XK_acircumflex, XK_atilde, XK_adiaeresis, XK_aring, XK_ae, XK_ccedilla,
   XK_egrave, XK_eacute, XK_ecircumflex, XK_ediaeresis, XK_igrave, XK_iacute,
   XK_icircumflex, XK_idiaeresis, XK_eth, XK_ntilde, XK_ograve, XK_oacute,
XK_ocircumflex, XK_otilde, XK_odiaeresis, XK_division, XK_oslash, XK_ugrave,
XK_uacute, XK_ucircumflex, XK_udiaeresis, XK_yacute, XK_thorn, XK_ydiaeresis,
   NoSymbol			/* Needed, do not remove ! */
};
#endif /* !_WINDOWS */
/*
 * definition of a multi-key sequence, is made of three KeySyms :
 *      - a character,
 *      - a modifier,
 *      - the result.
 */

typedef struct multi_key
  {
#ifdef _WINDOWS
     CHAR_T              c;
     CHAR_T              m;
     int                 r;
#else  /* !_WINDOWS */
     KeySym              c;
     KeySym              m;
     KeySym              r;
#endif /* !_WINDOWS */
  }
Multi_Key;

#ifndef _WINDOWS
/*
 * tab containing the xtended multi-key sequences.
 * It's a one dimentionnal array of multi-key sequences
 * Sequence lookup is based on the order in this table.
 * So insert more important first.
 */
static Multi_Key    emk_tab[] =
{
/* Remaining ISO-latin-1 sequences */
   {XK_O, XK_e, XK_multiply},	/* Oelig */
   {XK_O, XK_E, XK_multiply},	/* Oelig */
   {XK_o, XK_e, XK_division},	/* oelig */
/* <Alt><key>1<key>4 is already used by numbered entry */
   {XK_slash, XK_4, XK_onequarter},	/* onequarter */
   {XK_slash, XK_2, XK_onehalf},	/* onehalf */
   {XK_slash, XK_3, XK_threequarters},	/* threequarters */
   {XK_asciicircum, XK_0, XK_degree},	/* degree */

/*
 * The ISO 8859 standard includes several 8-bit extensions to
 * the  ASCII  character  set  (also  known  as ISO 646-IRV).
 * Especially important is ISO 8859-1,  the  "Latin  Alphabet
 * No.  1",  which  has  become  widely  implemented  and may
 * already  be  seen   as   the   de-facto   standard   ASCII
 * replacement.
 *
 * ISO  8859-1  supports  the following languages: Afrikaans,
 * Basque,  Catalan,  Danish,   Dutch,   English,   Faeroese,
 * Finnish,   French,  Galician,  German,  Icelandic,  Irish,
 * Italian, Norwegian,  Portuguese,  Scottish,  Spanish,  and
 * Swedish.
 *
 * Note that the ISO 8859-1 characters are also the first 256
 * characters of ISO 10646 (Unicode).
 *
 * $XConsortium: iso8859-1,v 1.4 94/07/06 15:17:11 kaleb Exp $
 *
 * ISO 8859-1 (Latin1) Compose Sequence
 *
 * Sequence Definition
 */
   {XK_plus, XK_plus, XK_numbersign},	/* numbersign */
   {XK_apostrophe, XK_space, XK_apostrophe},	/* apostrophe */
   {XK_A, XK_A, XK_at},		/* at */
   {XK_parenleft, XK_parenleft, XK_bracketleft},	/* bracketleft */
   {XK_slash, XK_slash, XK_backslash},	/* backslash */
   {XK_slash, XK_less, XK_backslash},	/* backslash */
   {XK_parenright, XK_parenright, XK_bracketright},	/* bracketright */
   {XK_asciicircum, XK_space, XK_asciicircum},	/* asciicircum */
   {XK_greater, XK_space, XK_asciicircum},	/* asciicircum */
   {XK_grave, XK_space, XK_grave},	/* grave */
   {XK_parenleft, XK_minus, XK_braceleft},	/* braceleft */
   {XK_slash, XK_asciicircum, XK_bar},	/* bar */
   {XK_V, XK_L, XK_bar},	/* bar */
   {XK_v, XK_l, XK_bar},	/* bar */
   {XK_parenright, XK_minus, XK_asciitilde},	/* braceright */
   {XK_asciitilde, XK_space, XK_asciitilde},	/* asciitilde */
   {XK_minus, XK_space, XK_asciitilde},		/* asciitilde */
   {XK_exclam, XK_exclam, XK_exclamdown},	/* exclamdown */
   {XK_c, XK_slash, XK_cent},	/* cent */
   {XK_C, XK_slash, XK_cent},	/* cent */
   {XK_C, XK_bar, XK_cent},	/* cent */
   {XK_c, XK_bar, XK_cent},	/* cent */
   {XK_l, XK_minus, XK_sterling},	/* sterling */
   {XK_L, XK_minus, XK_sterling},	/* sterling */
   {XK_l, XK_equal, XK_sterling},	/* sterling */
   {XK_L, XK_equal, XK_sterling},	/* sterling */
   {XK_y, XK_minus, XK_yen},	/* yen */
   {XK_Y, XK_minus, XK_yen},	/* yen */
   {XK_y, XK_equal, XK_yen},	/* yen */
   {XK_Y, XK_equal, XK_yen},	/* yen */
   {XK_s, XK_o, XK_section},	/* section */
   {XK_S, XK_O, XK_section},	/* section */
   {XK_S, XK_exclam, XK_section},	/* section */
   {XK_s, XK_exclam, XK_section},	/* section */
   {XK_S, XK_0, XK_section},	/* section */
   {XK_s, XK_0, XK_section},	/* section */
   {XK_x, XK_o, XK_currency},	/* currency */
   {XK_X, XK_O, XK_currency},	/* currency */
   {XK_x, XK_O, XK_currency},	/* currency */
   {XK_X, XK_o, XK_currency},	/* currency */
   {XK_x, XK_0, XK_currency},	/* currency */
   {XK_X, XK_0, XK_currency},	/* currency */
   {XK_c, XK_o, XK_copyright},	/* copyright */
   {XK_C, XK_O, XK_copyright},	/* copyright */
   {XK_c, XK_O, XK_copyright},	/* copyright */
   {XK_C, XK_o, XK_copyright},	/* copyright */
   {XK_c, XK_0, XK_copyright},	/* copyright */
   {XK_C, XK_0, XK_copyright},	/* copyright */
   {XK_a, XK_underscore, XK_ordfeminine},	/* ordfeminine */
   {XK_A, XK_underscore, XK_ordfeminine},	/* ordfeminine */
   {XK_o, XK_underscore, XK_masculine},		/* masculine */
   {XK_O, XK_underscore, XK_masculine},		/* masculine */
   {XK_less, XK_less, XK_guillemotleft},	/* guillemotleft */
   {XK_greater, XK_greater, XK_guillemotright},		/* guillemotright */
   {XK_0, XK_asciicircum, XK_degree},	/* degree */
   {XK_0, XK_asterisk, XK_degree},	/* degree */
   {XK_plus, XK_minus, XK_plusminus},	/* plusminus */
   {XK_slash, XK_u, XK_mu},	/* mu */
   {XK_slash, XK_U, XK_mu},	/* mu */
   {XK_1, XK_asciicircum, XK_onesuperior},	/* onesuperior */
   {XK_S, XK_1, XK_onesuperior},	/* onesuperior */
   {XK_s, XK_1, XK_onesuperior},	/* onesuperior */
   {XK_2, XK_asciicircum, XK_twosuperior},	/* twosuperior */
   {XK_S, XK_2, XK_twosuperior},	/* twosuperior */
   {XK_s, XK_2, XK_twosuperior},	/* twosuperior */
   {XK_3, XK_asciicircum, XK_threesuperior},	/* threesuperior */
   {XK_S, XK_3, XK_threesuperior},	/* threesuperior */
   {XK_s, XK_3, XK_threesuperior},	/* threesuperior */
   {XK_p, XK_exclam, XK_paragraph},	/* paragraph */
   {XK_P, XK_exclam, XK_paragraph},	/* paragraph */
   {XK_period, XK_asciicircum, XK_periodcentered},	/* periodcentered */
   {XK_period, XK_period, XK_periodcentered},	/* periodcentered */
   {XK_1, XK_4, XK_onequarter},	/* onequarter */
   {XK_1, XK_2, XK_onehalf},	/* onehalf */
   {XK_3, XK_4, XK_threequarters},	/* threequarters */
   {XK_question, XK_question, XK_questiondown},		/* questiondown */
   {XK_space, XK_space, XK_nobreakspace},	/* nobreakspace */
   {XK_bar, XK_bar, XK_brokenbar},	/* brokenbar */
   {XK_exclam, XK_asciicircum, XK_brokenbar},	/* brokenbar */
   {XK_V, XK_B, XK_brokenbar},	/* brokenbar */
   {XK_v, XK_b, XK_brokenbar},	/* brokenbar */
   {XK_minus, XK_comma, XK_notsign},	/* notsign */
   {XK_minus, XK_minus, XK_hyphen},	/* hyphen */
   {XK_R, XK_O, XK_registered},	/* registered */
   {XK_minus, XK_asciicircum, XK_macron},	/* macron */
   {XK_underscore, XK_asciicircum, XK_macron},	/* macron */
   {XK_underscore, XK_underscore, XK_macron},	/* macron */
   {XK_minus, XK_colon, XK_division},	/* division */
   {XK_x, XK_x, XK_multiply},	/* multiply */
   {XK_apostrophe, XK_apostrophe, XK_acute},	/* acute */
   {XK_comma, XK_comma, XK_cedilla},	/* cedilla */
   {XK_quotedbl, XK_quotedbl, XK_diaeresis},	/* diaeresis */
/* Accented Alphabet */
   {XK_A, XK_grave, XK_Agrave},	/* Agrave */
   {XK_A, XK_acute, XK_Agrave},	/* Aacute */
   {XK_A, XK_apostrophe, XK_Aacute},	/* Aacute */
   {XK_A, XK_asciicircum, XK_Acircumflex},	/* Acircumflex */
   {XK_A, XK_greater, XK_Acircumflex},	/* Acircumflex */
   {XK_A, XK_asciitilde, XK_Atilde},	/* Atilde */
   {XK_A, XK_minus, XK_Atilde},	/* Atilde */
   {XK_A, XK_quotedbl, XK_Adiaeresis},	/* Adiaeresis */
   {XK_A, XK_asterisk, XK_Aring},	/* Aring */
   {XK_A, XK_E, XK_AE},		/* AE */
   {XK_a, XK_grave, XK_agrave},	/* agrave */
   {XK_a, XK_acute, XK_agrave},	/* aacute */
   {XK_a, XK_apostrophe, XK_aacute},	/* aacute */
   {XK_a, XK_asciicircum, XK_acircumflex},	/* acircumflex */
   {XK_a, XK_greater, XK_acircumflex},	/* acircumflex */
   {XK_a, XK_asciitilde, XK_atilde},	/* atilde */
   {XK_a, XK_minus, XK_atilde},	/* atilde */
   {XK_a, XK_quotedbl, XK_adiaeresis},	/* adiaeresis */
   {XK_a, XK_asterisk, XK_aring},	/* aring */
   {XK_a, XK_e, XK_ae},		/* ae */
   {XK_C, XK_comma, XK_Ccedilla},	/* Ccedilla */
   {XK_c, XK_comma, XK_ccedilla},	/* ccedilla */
   {XK_minus, XK_D, XK_ETH},	/* ETH */
   {XK_minus, XK_d, XK_eth},	/* eth */
   {XK_E, XK_grave, XK_Egrave},	/* Egrave */
   {XK_E, XK_acute, XK_Eacute},	/* Eacute */
   {XK_E, XK_apostrophe, XK_Eacute},	/* Eacute */
   {XK_E, XK_asciicircum, XK_Ecircumflex},	/* Ecircumflex */
   {XK_E, XK_greater, XK_Ecircumflex},	/* Ecircumflex */
   {XK_E, XK_quotedbl, XK_Ediaeresis},	/* Ediaeresis */
   {XK_e, XK_grave, XK_egrave},	/* egrave */
   {XK_e, XK_acute, XK_eacute},	/* eacute */
   {XK_e, XK_apostrophe, XK_eacute},	/* eacute */
   {XK_e, XK_asciicircum, XK_ecircumflex},	/* ecircumflex */
   {XK_e, XK_greater, XK_ecircumflex},	/* ecircumflex */
   {XK_e, XK_quotedbl, XK_ediaeresis},	/* ediaeresis */
   {XK_I, XK_grave, XK_Igrave},	/* Igrave */
   {XK_I, XK_acute, XK_Iacute},	/* Iacute */
   {XK_I, XK_apostrophe, XK_Iacute},	/* Iacute */
   {XK_I, XK_asciicircum, XK_Icircumflex},	/* Icircumflex */
   {XK_I, XK_greater, XK_Icircumflex},	/* Icircumflex */
   {XK_I, XK_quotedbl, XK_Idiaeresis},	/* Idiaeresis */
   {XK_i, XK_grave, XK_igrave},	/* igrave */
   {XK_i, XK_acute, XK_iacute},	/* iacute */
   {XK_i, XK_apostrophe, XK_iacute},	/* iacute */
   {XK_i, XK_asciicircum, XK_icircumflex},	/* icircumflex */
   {XK_i, XK_greater, XK_icircumflex},	/* icircumflex */
   {XK_i, XK_quotedbl, XK_idiaeresis},	/* idiaeresis */
   {XK_N, XK_asciitilde, XK_Ntilde},	/* Ntilde */
   {XK_N, XK_minus, XK_Ntilde},	/* Ntilde */
   {XK_n, XK_asciitilde, XK_ntilde},	/* ntilde */
   {XK_n, XK_minus, XK_ntilde},	/* ntilde */
   {XK_O, XK_grave, XK_Ograve},	/* Ograve */
   {XK_O, XK_acute, XK_Oacute},	/* Oacute */
   {XK_O, XK_apostrophe, XK_Oacute},	/* Oacute */
   {XK_O, XK_asciicircum, XK_Ocircumflex},	/* Ocircumflex */
   {XK_O, XK_greater, XK_Ocircumflex},	/* Ocircumflex */
   {XK_O, XK_asciitilde, XK_Otilde},	/* Otilde */
   {XK_O, XK_minus, XK_Otilde},	/* Otilde */
   {XK_O, XK_quotedbl, XK_Odiaeresis},	/* Odiaeresis */
   {XK_O, XK_slash, XK_Ooblique},	/* Ooblique */
   {XK_o, XK_grave, XK_ograve},	/* ograve */
   {XK_o, XK_acute, XK_oacute},	/* oacute */
   {XK_o, XK_apostrophe, XK_oacute},	/* oacute */
   {XK_o, XK_asciicircum, XK_ocircumflex},	/* ocircumflex */
   {XK_o, XK_greater, XK_ocircumflex},	/* ocircumflex */
   {XK_o, XK_asciitilde, XK_otilde},	/* otilde */
   {XK_o, XK_minus, XK_otilde},	/* otilde */
   {XK_o, XK_quotedbl, XK_odiaeresis},	/* odiaeresis */
   {XK_o, XK_slash, XK_oslash},	/* oslash */
   {XK_U, XK_grave, XK_Ugrave},	/* Ugrave */
   {XK_U, XK_acute, XK_Uacute},	/* Uacute */
   {XK_U, XK_apostrophe, XK_Uacute},	/* Uacute */
   {XK_U, XK_asciicircum, XK_Ucircumflex},	/* Ucircumflex */
   {XK_U, XK_greater, XK_Ucircumflex},	/* Ucircumflex */
   {XK_U, XK_quotedbl, XK_Udiaeresis},	/* Udiaeresis */
   {XK_u, XK_grave, XK_ugrave},	/* ugrave */
   {XK_u, XK_acute, XK_uacute},	/* uacute */
   {XK_u, XK_apostrophe, XK_uacute},	/* uacute */
   {XK_u, XK_asciicircum, XK_ucircumflex},	/* ucircumflex */
   {XK_u, XK_greater, XK_ucircumflex},	/* ucircumflex */
   {XK_u, XK_quotedbl, XK_udiaeresis},	/* udiaeresis */
   {XK_s, XK_s, XK_ssharp},	/* ssharp */
   {XK_T, XK_H, XK_THORN},	/* THORN */
   {XK_t, XK_h, XK_thorn},	/* thorn */
   {XK_Y, XK_acute, XK_Yacute},	/* Yacute */
   {XK_Y, XK_apostrophe, XK_Yacute},	/* Yacute */
   {XK_y, XK_acute, XK_yacute},	/* yacute */
   {XK_y, XK_apostrophe, XK_yacute},	/* yacute */
   {XK_y, XK_quotedbl, XK_ydiaeresis},	/* ydiaeresis */
   {0, 0, 0},
};

#define ExtNB_MK (int)((sizeof(emk_tab) / sizeof(Multi_Key)))
#endif /* !_WINDOWS */

/*
 * tab containing the multi-key sequences.
 * It's a one dimentionnal array of multi-key sequences
 * Sequence lookup is based on the order in this table.
 * So insert more important first.
 */
static Multi_Key    mk_tab[] =
{
#ifdef _WINDOWS
   {' ', '\'', 0x27},	/* \' */
   {'A', '`',  0xC0},	/* Agrave */
   {'A', '\'', 0xC1},	/* Aacute */
   {'A', '^',  0xC2},	/* Acircumflex */
   {'A', '~',  0xC3},	/* Atilde */
   {'A', '"',  0xC4},	/* Adiaeresis */
   {'A', '*',  0xC5},	/* Aring */
   {'a', '`',  0xE0},	/* agrave */
   {'a', '\'', 0xE1},	/* aacute */
   {'a', '^',  0xE2},	/* acircumflex */
   {'a', '~',  0xE3},	/* atilde */
   {'a', '"',  0xE4},	/* adiaeresis */
   {'a', '*',  0xE5},	/* aring */
   {'C', '`',  0xC7},	/* Ccedilla */
   {'c', '`',  0xE7},	/* ccedilla */
   {'E', '`',  0xC8},	/* Egrave */
   {'E', '\'', 0xC9},	/* Eacute */
   {'E', '^',  0xCA},	/* Ecircumflex */
   {'E', '"',  0xCB},	/* Ediaeresis */
   {'e', '`',  0xE8},	/* egrave */
   {'e', '\'', 0xE9},	/* eacute */
   {'e', '^',  0xEA},	/* ecircumflex */
   {'e', '"',  0xEB},	/* ediaeresis */
   {'I', '`',  0xCC},	/* Igrave */
   {'I', '\'', 0xCD},	/* Iacute */
   {'I', '^',  0xCE},	/* Icircumflex */
   {'I', '"',  0xCF},	/* Idiaeresis */
   {'i', '`',  0xEC},	/* Igrave */
   {'i', '\'', 0xED},	/* Iacute */
   {'i', '^',  0xEE},	/* Icircumflex */
   {'i', '"',  0xEF},	/* Idiaeresis */
   {'N', '~',  0xD1},	/* Ntilde */
   {'n', '~',  0xF1},	/* ntilde */
   {'O', '`',  0xD2},	/* Ograve */
   {'O', '\'', 0xD3},	/* Oacute */
   {'O', '^',  0xD4},	/* Ocircumflex */
   {'O', '~',  0xD5},	/* Otilde */
   {'O', '"',  0xD6},	/* Odiaeresis */
   {'o', '`',  0xF2},	/* Ograve */
   {'o', '\'', 0xF3},	/* Oacute */
   {'o', '^',  0xF4},	/* Ocircumflex */
   {'o', '~',  0xF5},	/* Otilde */
   {'o', '"',  0xF6},	/* Odiaeresis */
   {'U', '`',  0xD9},	/* Ugrave */
   {'U', '\'', 0xDA},	/* Uacute */
   {'U', '^',  0xDB},	/* Ucircumflex */
   {'U', '"',  0xDC},	/* Udiaeresis */
   {'u', '`',  0xF9},	/* Ugrave */
   {'u', '\'', 0xFA},	/* Uacute */
   {'u', '^',  0xFB},	/* Ucircumflex */
   {'u', '"',  0xFC},	/* Udiaeresis */
   {'Y', '\'', 0xDD},	/* Yacute */
   {'y', '\'', 0xFD},	/* yacute */
   {'y', '"',  0xFF},	/* ydiaeresis */
#else  /* !_WINDOWS */
   {XK_A, XK_grave, XK_Agrave},	/* Agrave */
   {XK_A, XK_acute, XK_Agrave},	/* Aacute */
   {XK_A, XK_apostrophe, XK_Aacute},	/* Aacute */
   {XK_A, XK_asciicircum, XK_Acircumflex},	/* Acircumflex */
   {XK_A, XK_asciitilde, XK_Atilde},	/* Atilde */
   {XK_A, XK_quotedbl, XK_Adiaeresis},	/* Adiaeresis */
   {XK_A, XK_asterisk, XK_Aring},	/* Aring */
   {XK_a, XK_grave, XK_agrave},	/* agrave */
   {XK_a, XK_acute, XK_agrave},	/* aacute */
   {XK_a, XK_apostrophe, XK_aacute},	/* aacute */
   {XK_a, XK_asciicircum, XK_acircumflex},	/* acircumflex */
   {XK_a, XK_asciitilde, XK_atilde},	/* atilde */
   {XK_a, XK_quotedbl, XK_adiaeresis},	/* adiaeresis */
   {XK_a, XK_asterisk, XK_aring},	/* aring */
   {XK_C, XK_grave, XK_Ccedilla},	/* Ccedilla */
   {XK_C, XK_acute, XK_Ccedilla},	/* Ccedilla */
   {XK_c, XK_grave, XK_ccedilla},	/* ccedilla */
   {XK_c, XK_acute, XK_ccedilla},	/* ccedilla */
   {XK_E, XK_grave, XK_Egrave},	/* Egrave */
   {XK_E, XK_acute, XK_Eacute},	/* Eacute */
   {XK_E, XK_apostrophe, XK_Eacute},	/* Eacute */
   {XK_E, XK_asciicircum, XK_Ecircumflex},	/* Ecircumflex */
   {XK_E, XK_quotedbl, XK_Ediaeresis},	/* Ediaeresis */
   {XK_e, XK_grave, XK_egrave},	/* egrave */
   {XK_e, XK_acute, XK_eacute},	/* eacute */
   {XK_e, XK_apostrophe, XK_eacute},	/* eacute */
   {XK_e, XK_asciicircum, XK_ecircumflex},	/* ecircumflex */
   {XK_e, XK_quotedbl, XK_ediaeresis},	/* ediaeresis */
   {XK_I, XK_grave, XK_Igrave},	/* Igrave */
   {XK_I, XK_acute, XK_Iacute},	/* Iacute */
   {XK_I, XK_apostrophe, XK_Iacute},	/* Iacute */
   {XK_I, XK_asciicircum, XK_Icircumflex},	/* Icircumflex */
   {XK_I, XK_quotedbl, XK_Idiaeresis},	/* Idiaeresis */
   {XK_i, XK_grave, XK_igrave},	/* igrave */
   {XK_i, XK_acute, XK_iacute},	/* iacute */
   {XK_i, XK_apostrophe, XK_iacute},	/* iacute */
   {XK_i, XK_asciicircum, XK_icircumflex},	/* icircumflex */
   {XK_i, XK_quotedbl, XK_idiaeresis},	/* idiaeresis */
   {XK_N, XK_asciitilde, XK_Ntilde},	/* Ntilde */
   {XK_n, XK_asciitilde, XK_ntilde},	/* ntilde */
   {XK_O, XK_grave, XK_Ograve},	/* Ograve */
   {XK_O, XK_acute, XK_Oacute},	/* Oacute */
   {XK_O, XK_apostrophe, XK_Oacute},	/* Oacute */
   {XK_O, XK_asterisk, XK_Ooblique},	/* Oacute */
   {XK_O, XK_asciicircum, XK_Ocircumflex},	/* Ocircumflex */
   {XK_O, XK_asciitilde, XK_Otilde},	/* Otilde */
   {XK_O, XK_quotedbl, XK_Odiaeresis},	/* Odiaeresis */
   {XK_o, XK_grave, XK_ograve},	/* ograve */
   {XK_o, XK_acute, XK_oacute},	/* oacute */
   {XK_o, XK_apostrophe, XK_oacute},	/* oslash */
   {XK_o, XK_asterisk, XK_oslash},	/* oslash */
   {XK_o, XK_asciicircum, XK_ocircumflex},	/* ocircumflex */
   {XK_o, XK_asciitilde, XK_otilde},	/* otilde */
   {XK_o, XK_minus, XK_otilde},	/* otilde */
   {XK_o, XK_quotedbl, XK_odiaeresis},	/* odiaeresis */
   {XK_U, XK_grave, XK_Ugrave},	/* Ugrave */
   {XK_U, XK_acute, XK_Uacute},	/* Uacute */
   {XK_U, XK_apostrophe, XK_Uacute},	/* Uacute */
   {XK_U, XK_asciicircum, XK_Ucircumflex},	/* Ucircumflex */
   {XK_U, XK_quotedbl, XK_Udiaeresis},	/* Udiaeresis */
   {XK_u, XK_grave, XK_ugrave},	/* ugrave */
   {XK_u, XK_acute, XK_uacute},	/* uacute */
   {XK_u, XK_apostrophe, XK_uacute},	/* uacute */
   {XK_u, XK_asciicircum, XK_ucircumflex},	/* ucircumflex */
   {XK_u, XK_quotedbl, XK_udiaeresis},	/* udiaeresis */
   {XK_Y, XK_acute, XK_Yacute},	/* Yacute */
   {XK_Y, XK_apostrophe, XK_Yacute},	/* Yacute */
   {XK_y, XK_acute, XK_yacute},	/* yacute */
   {XK_y, XK_apostrophe, XK_yacute},	/* yacute */
   {XK_y, XK_quotedbl, XK_ydiaeresis},	/* ydiaeresis */
#endif /* _WINDOWS */
   {0, 0, 0},
};

#define NB_MK (int)((sizeof(mk_tab) / sizeof(Multi_Key)))

#ifndef _WINDOWS
static Display     *TtaDisplay = NULL;
static int          TtaNbIsoKeySym = 0;
static int          TtaNbKeySymPerKeyCode = 0;
static int          TtaModifierNumber = 0;
static int          TtaMinKeyCode = 0;
static int          TtaMaxKeyCode = 0;
static KeyCode      TtaMode_switchKeyCode = NoSymbol;
static KeySym       TtaKeyboardMap[8 * 256];

/*
 * This switch indicate whether we will try to do our own MultiKey
 * detection using TtaXLookupString or the normal, Locale based
 * provided by X11 and the operating system.
 */
int                 TtaUseOwnXLookupString = 0;

/*----------------------------------------------------------------------
  TtaXLookupString : This override the standard function provided by the
  X11 librarie. This AWFUL glitch is required due to frequent change
  of the standard implementation when trying to work with more keysyms
  than the usual set and still having Motif functionning properly.
  Daniel Veillard 28 March 96 after installing X11R6 rel 1 ...
  ----------------------------------------------------------------------*/
int               TtaXLookupString (event, buffer, nbytes, keysym, status)
register ThotKeyEvent *event;
STRING              buffer;	/* buffer */
int                 nbytes;	/* space in buffer for characters */
KeySym             *keysym;
ThotComposeStatus  *status;	/* not implemented */
{
  KeySym              sym = NoSymbol;
  int                 keycode;
  int                 state;
  struct _XDisplay   *dpy;

  if (event == NULL)
    return (0);
  dpy = (struct _XDisplay *) event->display;
  keycode = event->keycode;
  if (keycode < TtaMinKeyCode || keycode > TtaMaxKeyCode)
    {
      if (keysym != NULL)
	*keysym = 0;
      return (0);
    }

  keycode = keycode - TtaMinKeyCode;
  state = event->state;
  state = state & (ShiftMask | LockMask | Mod1Mask | Mod3Mask);

  /* search for the keysym depending on the state flags */
  if (state == 0)
    {
      sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode];
      if ((sym >= XK_A) && (sym <= XK_Z))
	sym = sym + (XK_a - XK_A);
    }
  else if (state == ShiftMask || state == LockMask)
    {
      sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + 1];
      if (sym == NoSymbol)
	sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode];
    }
  else if (state == Mod3Mask || state == Mod1Mask)
    sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + 2];
  else if (state == (ShiftMask | Mod3Mask) || state == (ShiftMask | Mod1Mask))
    sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + 3];
  else if ((state & (dpy->mode_switch | ShiftMask)) == (dpy->mode_switch | ShiftMask))
    sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + 3];
  else if ((state & dpy->mode_switch) == dpy->mode_switch)
    sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + 2];
  if (keysym != NULL)
    *keysym = sym;
  if (sym != NoSymbol)
    {
#ifdef DEBUG_MULTIKEY
      fprintf (stderr, "code %X, state %X : sym %s\n", event->keycode, event->state, XKeysymToString (sym));
#endif

      /* we found the corresponding symbol, convert it to a char string */
      if ((buffer == NULL) || (nbytes < 1))
	return (0);
      if (IsModifierKey (sym))
	return (0);
      if (IsCursorKey (sym))
	return (0);
      if (IsPFKey (sym))
	return (0);
      if (IsFunctionKey (sym))
	return (0);
      if (IsMiscFunctionKey (sym))
	return (0);
      if (sym >= XK_KP_0 && sym <= XK_KP_9)
	{
	  buffer[0] = (sym - XK_KP_0) + '0';
	  return (1);
	}
      else if (sym >= XK_space && sym <= XK_ydiaeresis)
	{
	  /* Direct encoding for ISO-Latin 1 */
	  buffer[0] = sym;
	  return (1);
	}

      switch (sym)
	{
	case XK_KP_Space:
	  buffer[0] = SPACE;
	  return (1);
	case XK_KP_Enter:
	  buffer[0] = __CR__;
	  return (1);
	case XK_KP_Equal:
	  buffer[0] = TEXT('=');
	  return (1);
	case XK_KP_Multiply:
	  buffer[0] = TEXT('*');
	  return (1);
	case XK_KP_Add:
	  buffer[0] = TEXT('+');
	  return (1);
	case XK_KP_Separator:
	  buffer[0] = TEXT(',');
	  return (1);
	case XK_KP_Subtract:
	  buffer[0] = TEXT('-');
	  return (1);
	case XK_KP_Decimal:
	  buffer[0] = TEXT('.');
	  return (1);
	case XK_KP_Divide:
	  buffer[0] = TEXT('/');
	  return (1);
	case XK_Return:
	  buffer[0] = EOL;
	  return (1);
	}
    }
  else if (keysym != NULL)
    *keysym = 0;
  return (0);
}

/*----------------------------------------------------------------------
   TtaIsKeycodeOK

   Heuristic on where to place the new Keysyms on the Keymap layout.
   We should avoid to put them on keycode used for modifiers or
   (this is due to Sun Xserver behavior probably X11R5) on Keycode
   assigned to Keypad keys :-( .
  ----------------------------------------------------------------------*/
#ifdef __STDC__
static int          TtaIsKeycodeOK (int keycode)
#else  /* __STDC__ */
static int          TtaIsKeycodeOK (keycode)
int                 keycode;
#endif /* __STDC__ */
{
   return (TRUE);
}
#endif /* _WINDOWS */


/*----------------------------------------------------------------------
   TtaInstallMultiKey

   Install support for Iso-Latin-1 KeySyms in the Keyboard Map managed
   by X-Windows.
  ----------------------------------------------------------------------*/
/*
 * Seems there is no more problems, keep the ability
 * to avoid remapping modifiers on some servers.
 * Just put a significant substring of the VendorName
 * string associated to the server.
 *
 * e.g. : "Network Computing Devices"
 *
 * the RemapModifier X property allows to force the
 * default behaviour. The THOTNOMODIFIER environment
 * variable is for debugging purposes only.
 */
void                TtaInstallMultiKey ()
{
  CHAR_T* ptr;

#ifdef _WINDOWS 
  ptr = TtaGetEnvString ("ENABLE_MULTIKEY");
  if (ptr != NULL && !ustrcasecmp (ptr, TEXT("yes")))
    Enable_Multikey = TRUE;
  else
    Enable_Multikey = FALSE;
   TtaKeyboardMapInstalled = 1;
#else  /* _WINDOWS */
  KeySym             *keymap;
  Display            *dpy = TtaGetCurrentDisplay ();
  KeyCode             keycode;
  KeySym              keysym;
  int                 keysymperkeycode;
  int                 no;
  int                 modif;
  int                 res;
  int                 codeline;
  int                 index;

  TtaDisplay = dpy;
  /* check whether multi-key is enabled */
  ptr = TtaGetEnvString ("ENABLE_MULTIKEY");
  if (ptr != NULL && !ustrcasecmp (ptr, "yes"))
    Enable_Multikey = TRUE;
  else
    Enable_Multikey = FALSE;

  /* load the current keyboard mapping */
  XDisplayKeycodes (dpy, &TtaMinKeyCode, &TtaMaxKeyCode);
  keymap = XGetKeyboardMapping (dpy, TtaMinKeyCode,
				TtaMaxKeyCode - TtaMinKeyCode + 1,
				&keysymperkeycode);
  if ((int) keymap == BadValue)
    {
      fprintf (stderr, "XGetKeyboardMapping failed : BadValue\n");
      return;
    }
  TtaNbKeySymPerKeyCode = keysymperkeycode;

  /*
   * due to problems accessing keysyms on row >= 4 we force installation
   * of new keysyms on row 3. Row 0 is non modified, row 1 is Shifted
   * and row 2 is Compose1 + Key . Row 3 is assigned to Shift + Compose +
   * key .
   */
  TtaModifierNumber = 3;
  if (TtaNbKeySymPerKeyCode < 4)
    TtaNbKeySymPerKeyCode = 4;

  /* first, clean the TtaKeyboardMap. */
  for (keycode = 0; keycode < TtaMaxKeyCode - TtaMinKeyCode + 1; keycode++)
    for (modif = 0; modif < TtaNbKeySymPerKeyCode; modif++)
      TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + modif] = NoSymbol;
  
  /*
   * second, install the current mapping.
   * We are also looking for the Mode_switch keycode.
   */
  for (keycode = 0; keycode < TtaMaxKeyCode - TtaMinKeyCode + 1; keycode++)
    for (modif = 0; modif < keysymperkeycode; modif++)
      {
	TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + modif] =
	  keymap[keycode * keysymperkeycode + modif];
	if (keymap[keycode * keysymperkeycode + modif] == XK_Mode_switch)
	  TtaMode_switchKeyCode = keycode + TtaMinKeyCode;
      }

  XFree (keymap);

  /*
   * for each element of TtaIsoKeySymTab, look at the current keyboard map
   * for it and if it's not available, assign it to a free slot in the
   * TtaModifierNumber's row.
   *
   * WARNING, SunOS and Solaris rule :-( :
   * a keysym placed next to a Keypad key will be ignored , exemple :
   *    KP_7 Home KP_7 adiaresis
   * sending an event with this keycode and the <Mod3> + <Shift> state
   * won't generate the expected adiaresis !
   * We also need to avoid putting new keysyms on key mapped to a modifier
   */
  for (TtaNbIsoKeySym = 0; TtaIsoKeySymTab[TtaNbIsoKeySym] != NoSymbol;
       TtaNbIsoKeySym++) ;

  for (no = 0, keycode = 0; no < TtaNbIsoKeySym; no++)
    {
      /* look if current keysym is already present */
      keysym = TtaIsoKeySymTab[no];
      res = XKeysymToKeycode (TtaDisplay, keysym);
      if (res != 0)
	{
	  /* the keysym is already installed in the Keyboard map */
	  codeline = res - TtaMinKeyCode;
	     /* search the row where the keysym is installed */
	     for (index = 0; index < TtaNbKeySymPerKeyCode; index++)
		if (TtaKeyboardMap[codeline * TtaNbKeySymPerKeyCode + index] ==
		    keysym)
		   break;

	     /*
	      * if the row is < TtaModifierNumber means that
	      * this keysym was already installed by base software
	      * and will always be available, skip to next one ...
	      */
	     if (index < TtaModifierNumber)
		continue;

	     /*
	      * if the row is TtaModifierNumber the keysym was probably
	      * installed previously by another run of the application
	      * on this display, but verify that the Sun rule doesn't apply
	      * or that this key is not a modifier.
	      */
	     if ((index == TtaModifierNumber) &&
		 (TtaIsKeycodeOK (codeline)))
		continue;

	     /*
	      * if the row is > TtaModifierNumber, the Sun rule apply
	      * or if the keysym is placed on a modifier key,
	      * the keysym probably won't be usable as if, so remove it
	      * and place it in the TtaModifierNumber row.
	      TtaKeyboardMap[codeline * TtaNbKeySymPerKeyCode + index] = NoSymbol;
	      */
	  }

	/*
	 * find the next empty slot in the TtaModifierNumber's row
	 * which fits for inserting the new keysym.
	 */
	for (; keycode < (TtaMaxKeyCode - TtaMinKeyCode + 1); keycode++)
	   if ((TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode +
			       TtaModifierNumber] == NoSymbol) &&
	       (TtaIsKeycodeOK (keycode)))
	      break;

	if (keycode >= (TtaMaxKeyCode - TtaMinKeyCode + 1))
	  {
	     fprintf (stderr, "Unable to install full Iso-Latin-1 : %d of %d key\n",
		      no, TtaNbIsoKeySym);
	     break;
	  }

	/*
	 * install the new keysym.
	 */
	TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + TtaModifierNumber] = keysym;
     }
   TtaKeyboardMapInstalled = 1;
#endif /* _WINDOWS */
}

#ifdef _WINDOWS 
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 WIN_TtaHandleMultiKeyEvent (UINT msg, WPARAM wParam, LPARAM lParam, int* k)
#else  /* __STDC__ */
int                 WIN_TtaHandleMultiKeyEvent (msg, wParam, lParam, k)
UINT    msg; 
WPARAM wParam; 
LPARAM lParam;
int*   k;
#endif /* __STDC__ */
{
   int          index;
   int          keycode;
   CHAR_T         KS;
   unsigned int state;

   if (!Enable_Multikey) /* no multi-key allowed */
      return 1;

   if (msg == WM_CHAR)
      KS = (CHAR_T) wParam;
   
   if (mk_state == 1 && msg == WM_CHAR) {
      /* we have already read the stressed character */ 
      /* We look for the result in the list */

      mk_state = 0;
      for (index = 0; index < NB_MK; index++)
          if ((mk_tab[index].m == previous_keysym) && (mk_tab[index].c == (CHAR_T) wParam)) {
             /*
              * The corresponding sequence is found. 
              * Generation of the corresponding character
              */
	         (CHAR_T) *k = mk_tab[index].r;
             return 1;
		  }

      return (1);
   }
   if (KS == '`' || KS == '\'' || KS == '^' || KS == '~' || KS == '"'|| KS == '*') {
      /* start of a compose sequence */
       mk_state = 1;
       previous_keysym = KS;
       previous_value  = keycode;
       previous_state  = state;
       return (0);
   }
   return (1);
}
#else /* _WINDOWS */

/*----------------------------------------------------------------------
   TtaGetIsoKeysym

   Modify the ThotEvent given as the argument to reference a given KeySym.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaGetIsoKeysym (ThotEvent * event, KeySym keysym)
#else  /* __STDC__ */
int                 TtaGetIsoKeysym (event, keysym)
ThotEvent             *event;
KeySym              keysym;
#endif /* __STDC__ */
{
  KeyCode             keycode;
  int                 codeline;
  int                 index;
  ThotKeyEvent          *ev = (ThotKeyEvent *) event;

  /* look for the index in the key corresponding to this keycode */
  for (codeline = 0; codeline < TtaMaxKeyCode - TtaMinKeyCode; codeline++)
    for (index = 0; index < TtaNbKeySymPerKeyCode; index++)
      if (TtaKeyboardMap[codeline * TtaNbKeySymPerKeyCode + index] == keysym)
	{
	  keycode = codeline + TtaMinKeyCode;
	  switch (index)
	    {
	    case 0:
	      /* normal key, no specific state */
	      ev->keycode = keycode;
	      ev->state = 0;
	      break;
	    case 1:
	      /* shifted key */
	      ev->keycode = keycode;
	      ev->state = ShiftMask;
	      break;
	    case 2:
	      /* Modified key, standard */
	      ev->keycode = keycode;
	      ev->state = Mod3Mask;
	      break;
	    case 3:
	      /* Here comes the trouble, Modified key, non-standard */
	      ev->keycode = keycode;
	      ev->state = Mod3Mask | ShiftMask;
	      break;
	    default:
	      fprintf (stderr, "TtaGetIsoKeysym :internal error, index too big\n");
	      return (0);
	    }
	}
  /* not found */
  return (0);
}

/*
 * static values are part of the automata which interprets
 * a multiple sequence of event to produce the corresponding
 * keysym.
 */

/*----------------------------------------------------------------------
   TtaHandleMultiKeyEvent

   handle multi-key input to provide Iso-Latin-1 in a consistent manner
   for the whole application.
   It uses an automata, and KeyPressed events can change its state.

   Initial = 0 -> 1      -> 2       -> 0 
   Compose     Key       Key

   -> 3       -> 5      -> 0
   Num        Num       Num

   If the whole sequence correspond to a valid MultiKey sequence, the
   event corresponding to a KeyPress for the result character is generated
  ----------------------------------------------------------------------*/
#ifdef __STDC__
int                 TtaHandleMultiKeyEvent (ThotEvent * event)
#else  /* __STDC__ */
int                 TtaHandleMultiKeyEvent (event)
ThotEvent             *event;
#endif /* __STDC__ */
{
   KeySym              KS;
   CHAR_T              buf[2];
   ThotComposeStatus   status;
   unsigned int        state, state2;
   int                 keycode;
   int                 index;
   int                 ret;

   /* control, alt and mouse status bits of the state are ignored */
   if (Enable_Multikey)
     {
       state = event->xkey.state & (ShiftMask | LockMask | Mod3Mask | ButtonMotionMask);
       if (event->xkey.state != state)
	 {
	   /* control, alt and mouse status bits of the state are not Multikeys */
	   mk_state = 0;
	   return (1);
	 }
       keycode = event->xkey.keycode;
       ret = TtaXLookupString (&event->xkey, buf, 2, &KS, &status);
       if (ret == 0)
	 return (1);

       if (mk_state == 1)
	 {
	   /* we have already read the stressed character */ 
	   /* We look for the result in the list */
	   mk_state = 0;
	   for (index = 0; index < NB_MK; index++)
	     if (mk_tab[index].m == previous_keysym && mk_tab[index].c == KS)
	       {
		 /*
		  * The corresponding sequence is found. 
		  * Generation of the corresponding character
		  */
		 
#ifdef DEBUG_MULTIKEY
fprintf (stderr, " mapped to %c\n", mk_tab[index].r);
#endif
	          TtaGetIsoKeysym (event, mk_tab[index].r);
		  return (1);
	       }
	   /* in other cases keep the first character */
	   event->xkey.keycode = previous_value;
	   event->xkey.state = previous_state;
	   return (1);
	 }
       else if (KS == XK_grave ||
		KS == XK_acute ||
		KS == XK_apostrophe ||
		KS == XK_asciicircum ||
		KS == XK_asciitilde ||
		KS == XK_quotedbl ||
		KS == XK_asterisk)
	 {
	   /* start of a compose sequence */
	   mk_state = 1;
	   previous_keysym = KS;
	   previous_value = keycode;
	   previous_state = state;
	   return (0);
	 }
       else
	 return (1);
     }
   else
     {
       state = event->xkey.state & (ShiftMask | LockMask | ControlMask | Mod1Mask | Mod2Mask | Mod3Mask | ButtonMotionMask);
       ret = TtaXLookupString (&event->xkey, buf, 2, &KS, &status);
       if (ret == 0)
	 {
	   /* try without the shift */
	   state2 = event->xkey.state;
	   event->xkey.state &= ShiftMask;
	   ret = TtaXLookupString (&event->xkey, buf, 2, &KS, &status);
	   event->xkey.state = state2;
	 }

       if (KS == XK_Multi_key || KS == XK_Alt_R)
	 {
	   /*
	    * start of a compose sequence using the Compose key.
	    */
#ifdef DEBUG_MULTIKEY
fprintf (stderr, "Start of compose sequence\n");
#endif
	   mk_state = 1;
	   return (0);
	 }

       if (ret == 0)
	 return (1);
       if (mk_state == 2)
	 {
	   /*
	    * The have already read the character modified by compose. 
	    * We look for the result in the list. 
	    */
#ifdef DEBUG_MULTIKEY
fprintf (stderr, "      Multikey : <Alt>%c %c\n", previous_keysym, KS);
#endif
            mk_state = 0;
	    for (index = 0; index < ExtNB_MK; index++)
	      if (emk_tab[index].c == previous_keysym && emk_tab[index].m == KS)
		{
		  /*
		   * The corresponding sequence is found. 
		   * Generation of the corresponding character
		   */
#ifdef DEBUG_MULTIKEY
		  fprintf (stderr, "      mapped to %c\n", emk_tab[index].r);
#endif
		  TtaGetIsoKeysym (event, emk_tab[index].r);
		  return (1);
		}

	    /*
	     * The corresponding sequence does not exist.
	     * Generation of the character gotten (dead keys).
	     */
	    TtaGetIsoKeysym (event, previous_value);
	    event->xkey.state = previous_value;
	    event->xkey.keycode = previous_state;
	    /*XtDispatchEvent (event);*/
	    return (1);
	 }

       if (mk_state == 1)
	 {
	   /*
	    * Memorizing the first element and changing the state
	    */
	   previous_keysym = KS;
	   previous_value = keycode;
	   previous_state = state;
	   mk_state++;
	   return (0);
	 }
       return (1);
     }
}
#endif /* !_WINDOWS */


/*
 * Global variables : external functions used when the application
 * overrides the built-in event-handling mechanisms.
 */

static ExternalInitMainLoop NewInitMainLoop = NULL;
static ExternalMainLoop NewMainLoop = NULL;
static ExternalFetchEvent NewFetchEvent = NULL;
static ExternalFetchAvailableEvent NewFetchAvailableEvent = NULL;
static ExternalLockMainLoop NewLockMainLoop = NULL;
static ExternalUnlockMainLoop NewUnlockMainLoop = NULL;

/*----------------------------------------------------------------------
   TtaSetMainLoop
   Provide a new main loop for processing all events in an application.
  ----------------------------------------------------------------------*/
#ifdef __STDC
void                TtaSetMainLoop (ExternalInitMainLoop init,
      ExternalMainLoop loop, ExternalFetchEvent fetch,
      ExternalFetchAvailableEvent fetchavail,
      ExternalLockMainLoop lock,
      ExternalUnlockMainLoop unlock)
#else  /* __STDC__ */
void                TtaSetMainLoop (init, loop, fetch, fetchavail, lock, unlock)
ExternalInitMainLoop init;
ExternalMainLoop    loop;
ExternalFetchEvent fetch;
ExternalFetchAvailableEvent fetchavail;
ExternalLockMainLoop lock;
ExternalUnlockMainLoop unlock;
#endif /* __STDC__ */
{
   NewInitMainLoop = init;
   NewMainLoop = loop;
   NewFetchEvent = fetch;
   NewFetchAvailableEvent = fetchavail;
   NewLockMainLoop = lock;
   NewUnlockMainLoop = unlock;
}

/*----------------------------------------------------------------------
   TtaLockMainLoop

   Used when events are to be handled outside of the application mainloop
   e.g. dialogs callback. This is useful only in case of multithreaded
   applications, so the default handling is empty.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaLockMainLoop (void)
#else  /* __STDC__ */
void                TtaLockMainLoop ()
#endif /* __STDC__ */
{
    if (NewLockMainLoop) NewLockMainLoop();
}

/*----------------------------------------------------------------------
   TtaUnlockMainLoop

   Used when events are to be handled outside of the application mainloop
   e.g. dialogs callback. This is useful only in case of multithreaded
   applications, so the default handling is empty.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaUnlockMainLoop (void)
#else  /* __STDC__ */
void                TtaUnlockMainLoop ()
#endif /* __STDC__ */
{
    if (NewUnlockMainLoop) NewUnlockMainLoop();
}

/*----------------------------------------------------------------------
   TtaFetchOneEvent

   retrieve one X-Windows Event from the queue, this is a blocking call.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaFetchOneEvent (ThotEvent *ev)
#else  /* __STDC__ */
void                TtaFetchOneEvent (ev)
ThotEvent             *ev;
#endif /* __STDC__ */
{
  if (NewFetchEvent)
    {
      NewFetchEvent (ev);
      return;
    }
#ifndef _WINDOWS
  XtAppNextEvent (app_cont, ev);
#endif /* ! _WINDOWS */
}

#ifndef _WINDOWS
/*----------------------------------------------------------------------
  ----------------------------------------------------------------------*/
static void *TimerCallback (XtPointer cdata, XtIntervalId *id)
{
  return (0);
}
#endif /* ! _WINDOWS */

/*----------------------------------------------------------------------
   TtaFetchOneAvailableEvent

   retrieve one X-Windows Event from the queue if one is immediately
   available.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaFetchOrWaitEvent (ThotEvent *ev)
#else  /* __STDC__ */
void                TtaFetchOrWaitEvent (ev)
ThotEvent          *ev;
#endif /* __STDC__ */
{
#ifndef _GTK
#ifndef _WINDOWS
   XtInputMask         status;
#endif /* ! _WINDOWS */

   if (NewFetchAvailableEvent)
      NewFetchAvailableEvent(ev);

#ifndef _WINDOWS
   status = XtAppPending (app_cont);
   if (status & XtIMXEvent)
       XtAppNextEvent (app_cont, ev);
   else
     {
       /* active a time-out and wait */
       XtAppAddTimeOut (app_cont, 1000, (XtTimerCallbackProc) TimerCallback, NULL);
       XtAppNextEvent (app_cont, ev);
     }
#endif /* ! _WINDOWS */
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   TtaFetchOneAvailableEvent

   retrieve one X-Windows Event from the queue if one is immediately
   available.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
ThotBool            TtaFetchOneAvailableEvent (ThotEvent *ev)
#else  /* __STDC__ */
ThotBool            TtaFetchOneAvailableEvent (ev)
ThotEvent             *ev;
#endif /* __STDC__ */
{
#ifndef _GTK
#ifndef _WINDOWS
   XtInputMask         status;
#endif /* ! _WINDOWS */

   if (NewFetchAvailableEvent)
      return (NewFetchAvailableEvent(ev));

#ifndef _WINDOWS
   status = XtAppPending (app_cont);
   if (status & XtIMXEvent)
     {
       XtAppNextEvent (app_cont, ev);
       return (TRUE);
     }
   else if (status != 0)
     XtAppProcessEvent (app_cont, XtIMAll);
#endif /* ! _WINDOWS */
#endif /* _GTK */
   return (FALSE);
}


#ifndef _WINDOWS
/*----------------------------------------------------------------------
   TtaHandleOneEvent

   Processes one given event in the application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaHandleOneEvent (ThotEvent * ev)
#else  /* __STDC__ */
void                TtaHandleOneEvent (ev)
ThotEvent             *ev;
#endif /* __STDC__ */
{
#ifndef _GTK
  PtrDocument         pDoc;
  ThotWindow          w;
  STRING              s;
  int                 frame;
  int                 view, i;
  ThotBool            assoc;

  /* Keep client messages */
  if (ev->type == ClientMessage)
    {
      s = XGetAtomName (ev->xany.display, ((XClientMessageEvent *) ev)->message_type);
      if (s == NULL)
	return;
      if (!ustrcmp (s, "WM_PROTOCOLS"))
	{
	  /* The client message comes from the Window Manager */
	  w = ev->xany.window;
	  s = XGetAtomName (ev->xany.display, ((XClientMessageEvent *) ev)->data.l[0]);
	  if (!ustrcmp (s, "WM_DELETE_WINDOW"))
	    {
	      if (FrRef[0] != 0 && XtWindowOfObject (XtParent (FrameTable[0].WdFrame)) == w)
		TtcQuit (0, 0);
	      else
		{
		  for (frame = 1; frame <= MAX_FRAME; frame++)
		    {
		      if (FrRef[frame] != 0 && XtWindowOfObject (XtParent (XtParent (XtParent (FrameTable[frame].WdFrame)))) == w)
			break;
		    }
		  if (frame <= MAX_FRAME)
		    {
		      GetDocAndView (frame, &pDoc, &view, &assoc);
		      CloseView (pDoc, view, assoc);
		    }
		  for (frame = 0; frame <= MAX_FRAME; frame++)
		    if (FrRef[frame] != 0)
		      /* there is still an active frame */
		      return;
		  TtaQuit();
		}
	    }
	}
      else if (!ustrcmp (s, "THOT_MESSAGES"))
	{
	  /* The client message comes from print */
	  s = XGetAtomName (ev->xany.display, ((XClientMessageEvent *) ev)->data.l[0]);
	  i = ((XClientMessageEvent *) ev)->data.l[1];
	  TtaDisplayMessage (CONFIRM, TtaGetMessage (LIB, i), s);
	}
    }
  else if (ev->type == KeyPress)
    {
      if (!TtaHandleMultiKeyEvent (ev))
	return;
    }
  else
    {
      /* Manage color events */
      if (ThotLocalActions[T_colors] != NULL)
	(*ThotLocalActions[T_colors]) (ev);
      /* Manage selection events */
      SelectionEvents ((XSelectionEvent *) ev);
    }
  
  XtDispatchEvent (ev);
  /* Manage document events */
  frame = GetWindowFrame (ev->xany.window);
  /* the event does not concern a document */
  if ((frame > 0) && (frame <= MAX_FRAME))
    {
      if (FrameTable[frame].WdFrame != 0)
	{
	  if (ev->type == GraphicsExpose || ev->type == Expose)
	    FrameToRedisplay (0, frame, (XExposeEvent *) ev);
	  else
	    FrameCallback (frame, ev);
	}
    }
#endif /* _GTK */
}
#endif /* !_WINDOWS */

#ifdef _WINDOWS
/*----------------------------------------------------------------------
   TtaHandleOneWindowEvent

   Processes one given event in the Window application.
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaHandleOneWindowEvent (MSG* msg)
#else  /* __STDC__ */
void                TtaHandleOneWindowEvent (msg)
MSG                *msg;
#endif /* __STDC__ */
{
   if (msg->message != WM_QUIT)
     {
       TranslateMessage (msg);
       DispatchMessage (msg);
     }
}
#endif


/*----------------------------------------------------------------------
  TtaHandlePendingEvents
  Processes all pending events in an application.
  ----------------------------------------------------------------------*/
void                TtaHandlePendingEvents ()
{
#ifndef _WINDOWS
   ThotEvent              ev;

   while (TtaFetchOneAvailableEvent(&ev))
     TtaHandleOneEvent (&ev);
#endif /* _WINDOWS */
}

/*----------------------------------------------------------------------
   TtaMainLoop

   Starts the main loop for processing all events in an application. This
   function must be called after all initializations have been made.

  ----------------------------------------------------------------------*/
void TtaMainLoop () {
     NotifyEvent         notifyEvt;
#    ifdef _WINDOWS
     MSG                 msg;
#    else /* ! _WINDOWS */
     ThotEvent           ev;
#    endif /* _WINDOWS */

     if (NewInitMainLoop)
        NewInitMainLoop(app_cont);

#ifndef _GTK
  TtaInstallMultiKey ();
#endif /* _GTK */

  UserErrorCode = 0;
  /* Sends the message Init.Pre */
  notifyEvt.event = TteInit;
  if (CallEventType (&notifyEvt, TRUE))
    /* The application is not able to start the editor => quit */
    exit (0);
  /* Sends the message Init.Post */
  notifyEvt.event = TteInit;
  CallEventType (&notifyEvt, FALSE);
  
#    ifdef _WINDOWS
#    endif /* _WINDOWS */

  /* Loop wainting for the events */
#ifndef _GTK
  while (1)
    {
      if (NewMainLoop != NULL)
	{
	  NewMainLoop ();
	  continue;
	}
#ifndef _WINDOWS
      TtaFetchOneEvent (&ev);
      TtaHandleOneEvent (&ev);
#else  /* !_WINDOWS */
      if (GetMessage (&msg, NULL, 0, 0)) {
         /* if (currentFrame >= 1)
            SetFocus (FrRef[currentFrame]); */
         TtaHandleOneWindowEvent (&msg);
	  }
#endif /* _WINDOWS */
    }
#else /* _GTK */
  gtk_main ();
#endif /* _GTK */
}

/*----------------------------------------------------------------------
   TtaGetMenuColor

   Returns the color used for the background of dialogue windows.
  ----------------------------------------------------------------------*/
Pixel               TtaGetMenuColor ()
{
   UserErrorCode = 0;
   return (Pixel) BgMenu_Color;
}


/*----------------------------------------------------------------------
   DisplayEmptyBoxLoadResources: displays the empty boxes.            
  ----------------------------------------------------------------------*/
void                DisplayEmptyBoxLoadResources ()
{
   if (ThotLocalActions[T_emptybox] == NULL)
      TteConnectAction (T_emptybox, (Proc) DisplayEmptyBox);
}



#ifndef _WINDOWS
/*----------------------------------------------------------------------
   TtaGetCurrentDisplay

   Returns the current display descriptor.

  ----------------------------------------------------------------------*/
Display            *TtaGetCurrentDisplay ()
{
   UserErrorCode = 0;
   return TtDisplay;
}
#endif /* !_WINDOWS */


/*----------------------------------------------------------------------
   TtaGetScreenDepth

   Returns the current screen depth.

  ----------------------------------------------------------------------*/
int                 TtaGetScreenDepth ()
{
   UserErrorCode = 0;
   return TtWDepth;
}

/*----------------------------------------------------------------------
   TtaClickElement

   Returns document and element clicked.

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaClickElement (Document * document, Element * element)
#else  /* __STDC__ */
void                TtaClickElement (document, element)
Document           *document;
Element            *element;

#endif /* __STDC__ */

{
   int                 frame;
   PtrAbstractBox      absBox;
   PtrDocument         pDoc;
   int                 view;
   ThotBool            Assoc;

   UserErrorCode = 0;
   if (element == NULL || document == NULL)
      TtaError (ERR_invalid_parameter);
   else
     {
	*element = (Element) None;
	*document = (Document) None;

	GiveClickedAbsBox (&frame, &absBox);
	if (frame == 0 || absBox == 0)
	   return;
	else
	  {
	     *element = (Element) absBox->AbElement;
	     GetDocAndView (frame, &pDoc, &view, &Assoc);
	     *document = (Document) IdentDocument (pDoc);
	  }
     }
}


/*----------------------------------------------------------------------
   TtaGiveSelectPosition: returns the mouse position for the last click 
   with respect to the element (position in pixel)            
  ----------------------------------------------------------------------*/
#ifdef __STDC__
void                TtaGiveSelectPosition (Document document, Element element, View view, int *X, int *Y)
#else  /* __STDC__ */
void                TtaGiveSelectPosition (document, element, view, X, Y)
Document            document;
Element             element;
View                view;
int                *X;
int                *Y;

#endif /* __STDC__ */

{
   int                 frame;
   PtrAbstractBox      pAb;
   ViewFrame          *pFrame;

   /* verifies the parameter document */
   UserErrorCode = 0;
   if (document == 0)
      TtaError (ERR_invalid_document_parameter);
   else if (view == 0 || element == 0)
      TtaError (ERR_invalid_parameter);
   else
     {
	frame = GetWindowNumber (document, view);
	if (frame != ClickFrame)
	   TtaError (ERR_no_selection_in_view);
	else
	  {
	     /* Determine the mouse position in the specified box */
	     pFrame = &ViewFrameTable[frame - 1];
	     pAb = ((PtrElement) element)->ElAbstractBox[view - 1];
	     if (pAb == NULL)
		TtaError (ERR_no_selection_in_view);
	     else
	       {
		  while (pAb->AbPresentationBox && pAb->AbNext != NULL)
		     pAb = pAb->AbNext;
		  *X = ClickX + pFrame->FrXOrg - pAb->AbBox->BxXOrg;
		  *Y = ClickY + pFrame->FrYOrg - pAb->AbBox->BxYOrg;
	       }
	  }
     }
}

/*----------------------------------------------------------------------
   TtaSetMultiKey

   Enables or disables the multikey support

   Parameters:
   value : TRUE/FALSE

  ----------------------------------------------------------------------*/
#ifdef __STDC__
void TtaSetMultikey (ThotBool value)
#else
void TtaSetMultikey (value)
ThotBool value;
#endif /*__STDC__*/
{
  Enable_Multikey = value;
  mk_state = 0;
  previous_state = 0;
  previous_value = 0;
}
/* End Of Module */




