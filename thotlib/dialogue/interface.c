
/* -- Copyright (c) 1990 - 1994 Inria/CNRS  All rights reserved. -- */

/*=======================================================================*/
/*|                                                                     | */
/*|     Thot Toolkit: Application Program Interface level 3             | */
/*|     --->document windows managment                                  | */
/*|                                                                     | */
/*|                     I. Vatton       September 92                    | */
/*|                                                                     | */
/*|  D. Veillard October 95 : add multikey support                      | */
/*|                                                                     | */
/*=======================================================================*/

/* #define DEBUG_MULTIKEY */
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

#define MAX_ARGS 20

#undef EXPORT
#define EXPORT extern
#include "edit.var"
#include "font.var"
#include "img.var"
#include "frame.var"
#include "appdialogue.var"

#include "displaybox_f.h"
#include "appli_f.h"
#include "context_f.h"
#include "dialogapi_f.h"
#include "views_f.h"
#include "appdialogue_f.h"
#include "callback_f.h"
#include "font_f.h"
#include "inites_f.h"
#include "input_f.h"
#include "keyboards_f.h"
#include "message_f.h"
#include "thotmsg_f.h"


#ifndef NEW_WILLOWS
/* ----------------------------------------------------------------------
   Handling of Multikey sequences used to produce ISO-Latin-1.
   The corresponding keysyms are installed in the keyboard map
   and a filter catch all keypressed event to detect sequence and
   dynamically change the event flow to produce event corresponding
   to the extended character set.
   ---------------------------------------------------------------------- */

/************
 ************/

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

/*
 * definition of a multi-key sequence, is made of three KeySyms :
 *      - a caracter,
 *      - a modifier,
 *      - the result.
 */

typedef struct multi_key
  {
     KeySym              c;
     KeySym              m;
     KeySym              r;
  }
Multi_Key;

/*
 * tab containing the multi-key sequences.
 * It's a one dimentionnal array of multi-key sequences
 * Sequence lookup is based on the order in this table.
 * So insert more important first.
 */

static Multi_Key    mk_tab[] =
{
/* Sequences oubliees de l'ISO-latin-1 */
   {XK_O, XK_e, XK_multiply},	/* Oelig */
   {XK_O, XK_E, XK_multiply},	/* Oelig */
   {XK_o, XK_e, XK_division},	/* oelig */
/* <Alt><key>1<key>4 est deja utilise pour l'entree par numero */
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
 */
/*
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

#define NB_MK (sizeof(mk_tab) / sizeof(Multi_Key))


static Display     *TtaDisplay = NULL;
static int          TtaNbIsoKeySym = 0;
static int          TtaKeyboardMapInstalled = 0;
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

/*
 * TtaXLookupString : This override the standard function provided by the
 *     X11 librarie. This AWFUL glitch is required due to frequent change
 *     of the standard implementation when trying to work with more keysyms
 *     than the usual set and still having Motif functionning properly.
 *  Daniel Veillard 28 March 96 after installing X11R6 rel 1 ...
 */
/*ARGSUSED */
int
                    TtaXLookupString (event, buffer, nbytes, keysym, status)
register XKeyEvent *event;
char               *buffer;	/* buffer */
int                 nbytes;	/* space in buffer for characters */
KeySym             *keysym;
XComposeStatus     *status;	/* not implemented */
{
   KeySym              sym = NoSymbol;
   int                 keycode;
   int                 state;
   struct _XDisplay   *dpy = (struct _XDisplay *) event->display;

   if (event == NULL)
      return (0);
   if ((event->keycode < TtaMinKeyCode) || (event->keycode > TtaMaxKeyCode))
      goto not_found;
   keycode = event->keycode - TtaMinKeyCode;
   state = event->state;

   /*
    * search for the keysym depending on the state flags.
    */
   if (state == 0)
     {
	sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode];
	if ((sym >= XK_A) && (sym <= XK_Z))
	  {
	     sym = sym + (XK_a - XK_A);
	  }
     }
   else if (state == ShiftMask)
     {
	sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + 1];
	if (sym == NoSymbol)
	   sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode];
     }
   else if (state == LockMask)
     {
	sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode];
     }
   else if (state == Mod1Mask)
     {
	sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + 2];
     }
   else if (state == (ShiftMask | Mod1Mask))
     {
	sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + 3];
     }
   else if ((state & (dpy->mode_switch | ShiftMask)) ==
	    (dpy->mode_switch | ShiftMask))
     {
	sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + 3];
     }
   else if ((state & dpy->mode_switch) == (state & dpy->mode_switch))
     {
	sym = TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + 2];
     }
   if (keysym != NULL)
      *keysym = sym;
   if (sym == NoSymbol)
      goto not_found;

#ifdef DEBUG_MULTIKEY
   fprintf (stderr, "code %X, state %X : sym %s\n", event->keycode, event->state,
	    XKeysymToString (sym));
#endif

   /*
    * we found the corresponding symbol, convert it to a char string.
    */
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
   if ((sym >= XK_KP_0) && (sym <= XK_KP_9))
     {
	buffer[0] = (sym - XK_KP_0) + '0';
	return (1);
     }
   else if ((sym >= XK_space) && (sym <= XK_ydiaeresis))
     {
	/* Direct encoding for ISO-Latin 1 */
	buffer[0] = sym;
	return (1);
     }
   switch (sym)
	 {
	    case XK_KP_Space:
	       buffer[0] = ' ';
	       return (1);
	    case XK_KP_Enter:
	       buffer[0] = '\r';
	       return (1);
	    case XK_KP_Equal:
	       buffer[0] = '=';
	       return (1);
	    case XK_KP_Multiply:
	       buffer[0] = '*';
	       return (1);
	    case XK_KP_Add:
	       buffer[0] = '+';
	       return (1);
	    case XK_KP_Separator:
	       buffer[0] = ',';
	       return (1);
	    case XK_KP_Subtract:
	       buffer[0] = '-';
	       return (1);
	    case XK_KP_Decimal:
	       buffer[0] = '.';
	       return (1);
	    case XK_KP_Divide:
	       buffer[0] = '/';
	       return (1);

	    case XK_Return:
	       buffer[0] = '\n';
	       return (1);
	 }
   return (0);

 not_found:
   if (keysym != NULL)
      *keysym = 0;
   return (0);
}

/* ----------------------------------------------------------------------
   TtaIsKeycodeOK

   Heuristic on where to place the new Keysyms on the Keymap layout.
   We should avoid to put them on keycode used for modifiers or
   (this is due to Sun Xserver behavior probably X11R5) on Keycode
   assigned to Keypad keys :-( .
   ---------------------------------------------------------------------- */

#ifdef __STDC__
static int          TtaIsKeycodeOK (int keycode)

#else  /* __STDC__ */
static int          TtaIsKeycodeOK (keycode)
int                 keycode;

#endif /* __STDC__ */

{
   return (TRUE);
}

#ifdef __STDC__
static char        *ks_name (KeySym k)
#else  /* __STDC__ */
static char        *ks_name (k)
KeySym              k;

#endif /* __STDC__ */
{
   char               *res = XKeysymToString (k);

   if (res == NULL)
      return ("NoSymbol");
   return (res);
}

/* ----------------------------------------------------------------------
   TtaInstallMultiKey

   Install support for Iso-Latin-1 KeySyms in the Keyboard Map managed
   by X-Windows.
   ---------------------------------------------------------------------- */

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
   int                 keysymperkeycode;
   KeySym             *keymap;
   Display            *dpy = TtaGetCurrentDisplay ();
   KeyCode             keycode;
   KeySym              keysym;
   int                 no;
   int                 modif;
   int                 res;
   char               *do_dump_keymap;

#ifdef DEBUG_KEYMAP
   int                 dump_keymap = 1;

#else
   int                 dump_keymap = 0;

#endif

   TtaDisplay = dpy;


   do_dump_keymap = TtaGetEnvString ("THOTDEBUGKEYMAP");
   if (do_dump_keymap != NULL)
     {
	if (!strcasecmp (do_dump_keymap, "yes"))
	   dump_keymap = 1;
	else
	   dump_keymap = 0;
     }

   /*
    * load the current keyboard mapping.
    */
   XDisplayKeycodes (dpy, &TtaMinKeyCode, &TtaMaxKeyCode);
   if (dump_keymap)
     {
	fprintf (stderr, "display keycodes : min %d , max %d\n",
		 TtaMinKeyCode, TtaMaxKeyCode);
     }

   keymap = XGetKeyboardMapping (dpy, TtaMinKeyCode,
		    (TtaMaxKeyCode - TtaMinKeyCode + 1), &keysymperkeycode);
   if ((int) keymap == BadValue)
     {
	fprintf (stderr, "XGetKeyboardMapping failed : BadValue\n");
	return;
     }
   TtaNbKeySymPerKeyCode = keysymperkeycode;

   if (dump_keymap)
     {
	fprintf (stderr, "Max %d KeySym per KeyCode\n", TtaNbKeySymPerKeyCode);
	for (keycode = 0; keycode < TtaMaxKeyCode - TtaMinKeyCode + 1; keycode++)
	  {
	     fprintf (stderr, "keycode %03d = ", keycode);
	     for (modif = 0; modif < TtaNbKeySymPerKeyCode; modif++)
		fprintf (stderr, "%s ",
		      ks_name (keymap[keycode * keysymperkeycode + modif]));
	     fprintf (stderr, "\n");
	  }
     }

   /*
    * due to problems accessing keysyms on row >= 4 we force installation
    * of new keysyms on row 3. Row 0 is non modified, row 1 is Shifted
    * and row 2 is Compose1 + Key . Row 3 is assigned to Shift + Compose +
    * key .
    */
   TtaModifierNumber = 3;
   if (TtaNbKeySymPerKeyCode < 4)
      TtaNbKeySymPerKeyCode = 4;

   /*
    * first, clean the TtaKeyboardMap.
    */
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
    * sending an event with this keycode and the <Mod1> + <Shift> state
    * won't generate the expected adiaresis !
    * We also need to avoid putting new keysyms on key mapped to a modifier
    */
   for (TtaNbIsoKeySym = 0; TtaIsoKeySymTab[TtaNbIsoKeySym] != NoSymbol;
	TtaNbIsoKeySym++) ;

   for (no = 0, keycode = 0; no < TtaNbIsoKeySym; no++)
     {
	/*
	 * look if current keysym is already present.
	 */
	keysym = TtaIsoKeySymTab[no];
	res = XKeysymToKeycode (TtaDisplay, keysym);
	if (res != 0)
	  {

	     /*
	      * the keysym is already installed in the Keyboard map
	      */
	     int                 codeline = res - TtaMinKeyCode;
	     int                 index;

	     /*
	      * search the row where the keysym is installed.
	      */
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
	TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + TtaModifierNumber] =
	   keysym;
     }

   if (dump_keymap)
     {
	fprintf (stderr, "%d KeySym per KeyCode\n", TtaNbKeySymPerKeyCode);
	for (keycode = 0; keycode < TtaMaxKeyCode - TtaMinKeyCode + 1; keycode++)
	  {
	     fprintf (stderr, "keycode %03d = ", keycode);
	     for (modif = 0; modif < TtaNbKeySymPerKeyCode; modif++)
		fprintf (stderr, "%s ",
			 ks_name (TtaKeyboardMap[keycode * TtaNbKeySymPerKeyCode + modif]));
	     fprintf (stderr, "\n");
	  }
     }

   TtaKeyboardMapInstalled = 1;

}

/* ----------------------------------------------------------------------
   TtaGetIsoKeysym

   Modify the XEvent given as the argument to reference a given KeySym.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
int                 TtaGetIsoKeysym (XEvent * event, KeySym keysym)
#else  /* __STDC__ */
int                 TtaGetIsoKeysym (event, keysym)
XEvent             *event;
KeySym              keysym;

#endif /* __STDC__ */
{
   KeyCode             keycode;
   int                 codeline;
   int                 index;
   XKeyEvent          *ev = (XKeyEvent *) event;

   /*
    * look for the index in the key corresponding to this keycode
    */
   for (codeline = 0; codeline < TtaMaxKeyCode - TtaMinKeyCode; codeline++)
      for (index = 0; index < TtaNbKeySymPerKeyCode; index++)
	 if (TtaKeyboardMap[codeline * TtaNbKeySymPerKeyCode + index] == keysym)
	    goto found;

   /*
    * not found
    */
   return (0);

 found:
   keycode = codeline + TtaMinKeyCode;
   switch (index)
	 {
	    case 0:		/* normal key, no specific state */
	       ev->keycode = keycode;
	       ev->state = 0;
	       break;
	    case 1:		/* shifted key */
	       ev->keycode = keycode;
	       ev->state = ShiftMask;
	       break;
	    case 2:		/* Modified key, standard */
	       ev->keycode = keycode;
	       ev->state = Mod1Mask;
	       break;
	    case 3:		/* Here comes the trouble, Modified key, non-standard */
	       ev->keycode = keycode;
	       ev->state = Mod1Mask | ShiftMask;
	       break;
	    default:
	       fprintf (stderr, "TtaGetIsoKeysym :internal error, index too big\n");
	       return (0);
	 }
   return (0);
}

/* ----------------------------------------------------------------------
   TtaHandleMultiKeyEvent

   handle multi-key input to provide Iso-Latin-1 in a consistent manner
   for the whole application.
   It uses an automata, and KeyPressed events can change its state.

   Initial = 0      ->       1      ->     2       ->      0 
   Alt or Compose      Key            Key

   ->     3       ->      5      ->      0
   Num            Num            Num

   If the whole sequence correspond to a valid MultiKey sequence, the
   event corresponding to a KeyPress for the result character is generated
   ---------------------------------------------------------------------- */

#ifdef __STDC__
int                 TtaHandleMultiKeyEvent (XEvent * event)
#else  /* __STDC__ */
int                 TtaHandleMultiKeyEvent (event)
XEvent             *event;

#endif /* __STDC__ */
{
   /*
    * static values are part of the automata which interprets
    * a multiple sequence of event to produce the corresponding
    * keysym.
    */
   static int          mk_state = 0;
   static KeySym       previous_keysym;
   static unsigned char previous_value = 0;

   unsigned int        state;
   int                 keycode;
   KeySym              KS;
   int                 retour;
   char                buf[2];
   XComposeStatus      status;
   unsigned int        state2;

   /*
    * save the current values, mouse status bits of the state are ignored.
    */
   state = event->xkey.state & (ShiftMask | LockMask | ControlMask | Mod1Mask);
   keycode = event->xkey.keycode;

   retour = TtaXLookupString (&event->xkey, buf, 2, &KS, &status);
   if (retour == 0)
     {
	state2 = event->xkey.state;
	event->xkey.state &= ShiftMask;
	retour = TtaXLookupString (&event->xkey, buf, 2, &KS, &status);
	event->xkey.state = state2;
     }

#ifdef DEBUG_MULTIKEY
   fprintf (stderr, "Event : key %d, lookup %d, state %X,  KS %X\n",
	    keycode, retour, state, KS);
#endif

   /*
    * Deal with Keysym definition by typing <Alt>#number_of_iso_char
    */
   if (((mk_state == 1) && (KS >= XK_0) && (KS <= XK_9)) ||
       (mk_state == 3) || (mk_state == 4))
     {
	if ((retour != 0) && (KS >= XK_0) && (KS <= XK_9))
	  {
	     if (mk_state == 1)
		mk_state = 2;
	     /*
	      * It's a char of the octal string.
	      */
#ifdef DEBUG_MULTIKEY
	     fprintf (stderr, "      inserting by code\n");
#endif
	     previous_value *= 8;
	     previous_value += KS - XK_0;

	     /*
	      * Octal number cannot be encoded on more than 3 char.
	      */
	     if (mk_state == 4)
	       {
#ifdef DEBUG_MULTIKEY
		  fprintf (stderr, "      inserting code \\%03o : '%c'\n",
			   previous_value, previous_value);
#endif
		  mk_state = 0;
		  TtaGetIsoKeysym (event, previous_value);
		  return (1);
	       }

	     /*
	      * finished with this event.
	      */
	     mk_state++;
	     return (0);
	  }
	else
	  {
	     /*
	      * Simulate an intermediate event for previous char
	      * and continue.
	      */
#ifdef DEBUG_MULTIKEY
	     fprintf (stderr, "      inserting code \\%03o : '%c'\n",
		      previous_value, previous_value);
#endif
	     TtaGetIsoKeysym (event, previous_value);
	     XtDispatchEvent (event);
	     event->xkey.state = state;
	     event->xkey.keycode = keycode;
	     mk_state = 0;
	  }
     }
   else
      previous_value = 0;

   if ((KS == XK_Multi_key) || (KS == XK_Alt_L) || (KS == XK_Alt_R))
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

   if (retour == 0)
      return (1);
   if (mk_state == 2)
     {
	int                 index;

	/*
	 * on a deja lu une touche modifiee par le compose,
	 * on recherche dans la liste le resultat
	 */

#ifdef DEBUG_MULTIKEY
	fprintf (stderr, "      Multikey : <Alt>%c %c\n",
		 previous_keysym, KS);
#endif

	mk_state = 0;
	for (index = 0; index < NB_MK; index++)
	   if ((mk_tab[index].c == previous_keysym) &&
	       (mk_tab[index].m == KS))
	     {
		/*
		 * on a trouve la sequence correspondante,
		 * on genere le caractere associe.
		 */

#ifdef DEBUG_MULTIKEY
		fprintf (stderr, "      mapped to %c\n",
			 mk_tab[index].r);
#endif

		TtaGetIsoKeysym (event, mk_tab[index].r);
		return (1);
	     }

	/*
	 * on n'a pas trouve la sequence correspondante,
	 * on genere les caracteres lus (dead keys).
	 */

#ifdef DEBUG_MULTIKEY
	fprintf (stderr, "      not mapped !\n");
#endif

	TtaGetIsoKeysym (event, previous_value);
	XtDispatchEvent (event);
	event->xkey.state = state;
	event->xkey.keycode = keycode;
	return (1);
     }

   if (mk_state == 1)
     {
	/*
	 * First key of a compose sequence ...
	 */
#ifdef DEBUG_MULTIKEY
	fprintf (stderr, "      premier de la sequence : '%c'\n", KS);
#endif
	/*
	 * on memorise le premier element de la composition
	 * et le fait qu'on ait chage d'etat
	 */
	previous_keysym = KS;
	mk_state++;
	return (0);
     }

   return (1);
}


/* ---------------------------------------------------------------------- */
/* | DisplayEmptyBoxLoadResources visualise les boites vides.           | */
/* ---------------------------------------------------------------------- */
void                DisplayEmptyBoxLoadResources ()
{
   if (ThotLocalActions[T_emptybox] == NULL)
      /* Connecte l'affichage des boites vides */
      TteConnectAction (T_emptybox, (Proc) AfTrame);
}


/* ----------------------------------------------------------------------
   TtaHandleOneEvent

   Processes one given event in the application.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaHandleOneEvent (XEvent * ev)
#else  /* __STDC__ */
void                TtaHandleOneEvent (ev)
XEvent             *ev;

#endif /* __STDC__ */
{
   int                 frame;
   PtrDocument         pDoc;
   int                 vue, i;
   boolean             assoc;
   ThotWindow          w;
   char               *s;

   /* Keep client messages */
   if (ev->type == ClientMessage)
     {
	s = XGetAtomName (ev->xany.display, ((XClientMessageEvent *) ev)->message_type);
	if (s == NULL)
	   return;
	if (!strcmp (s, "WM_PROTOCOLS"))
	  {
	     /* The client message comes from the Window Manager */
	     w = ev->xany.window;
	     s = XGetAtomName (ev->xany.display, ((XClientMessageEvent *) ev)->data.l[0]);
	     if (!strcmp (s, "WM_DELETE_WINDOW"))
	       {
		  for (frame = 0; frame <= MAX_FRAME; frame++)
		    {
		       if (FrRef[frame] != 0 && XtWindowOfObject (XtParent (XtParent (XtParent (FrameTable[frame].WdFrame)))) == w)
			  break;
		    }
		  if (frame == 0)
		     TtcQuit (0, 0);
		  else if (frame <= MAX_FRAME)
		    {
		       DocVueFen (frame, &pDoc, &vue, &assoc);
		       FermerVueDoc (pDoc, vue, assoc);
		    }
		  return;
	       }
	  }
	else if (!strcmp (s, "THOT_MESSAGES"))
	  {
	     s = XGetAtomName (ev->xany.display, ((XClientMessageEvent *) ev)->data.l[0]);
	     i = ((XClientMessageEvent *) ev)->data.l[1];
	     TtaDisplayMessage (CONFIRM, TtaGetMessage(LIB, i), s);
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
   frame = GetFenetre (ev->xany.window);
   /* the event does not concern a document */
   if ((frame > 0) && (frame <= MAX_FRAME))
     {
	if (FrameTable[frame].WdFrame != 0)
	  {
	     if (ev->type == GraphicsExpose || ev->type == Expose)
		TraiteExpose (0, frame, (XExposeEvent *) ev);
	     else
		RetourFntr (frame, ev);
	  }
     }
}

#endif /* !NEW_WILLOWS */

#ifdef NEW_WILLOWS
/* ----------------------------------------------------------------------
   TtaHandleOneWindowEvent

   Processes one given event in the Window application.
   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaHandleOneWindowEvent (MSG * msg)
#else  /* __STDC__ */
void                TtaHandleOneWindowEvent (msg)
MSG                *msg;

#endif /* __STDC__ */
{
   if (msg->message == WM_QUIT)
      exit (0);

   TranslateMessage (msg);
   DispatchMessage (msg);
}
#endif


/* ----------------------------------------------------------------------
 * TtaHandlePendingEvents
 *
 * Processes all pending events in an application.
 ---------------------------------------------------------------------- */
void                TtaHandlePendingEvents ()
{
#ifndef NEW_WILLOWS
   XtInputMask         status;
   XEvent              ev;

   /* Boucle d'attente des evenements pendants */
   status = XtAppPending (app_cont);
   while (status != 0)
     {
	if (status == XtIMXEvent)
	  {
	     XtAppNextEvent (app_cont, &ev);
	     TtaHandleOneEvent (&ev);
	  }
	else
	   XtAppProcessEvent (app_cont, (XtIMAll & (~XtIMXEvent)));

	status = XtAppPending (app_cont);
     }

#endif /* NEW_WILLOWS */
}


/* ----------------------------------------------------------------------
   TtaMainLoop

   Starts the main loop for processing all events in an application. This
   function must be called after all initializations have been made.

   ---------------------------------------------------------------------- */
void                TtaMainLoop ()
{
   NotifyEvent         notifyEvt;

#ifndef NEW_WILLOWS
   XEvent              ev;
   XtInputMask         status;

#endif /* NEW_WILLOWS */
#ifdef NEW_WILLOWS
   MSG                 msg;

#endif

#ifndef NEW_WILLOWS
   TtaInstallMultiKey ();
#endif /* !NEW_WILLOWS */
   UserErrorCode = 0;
   /* envoie le message Init.Pre */
   notifyEvt.event = TteInit;
   if (CallEventType (&notifyEvt, TRUE))
      /* l'application refuse le lancement de l'editeur, on quitte */
      exit (0);
   /* envoie le message Init.Post */
   notifyEvt.event = TteInit;
   CallEventType (&notifyEvt, FALSE);

   /* Boucle d'attente des evenements */
   while (1)
     {
#ifdef WWW_XWINDOWS
	status = XtAppPending (app_cont);
	if (status & XtIMXEvent)
	  {
	     XtAppNextEvent (app_cont, &ev);
	     TtaHandleOneEvent (&ev);
	  }
	else if (status & (XtIMAll & (~XtIMXEvent)))
	  {
	     XtAppProcessEvent (app_cont, (XtIMAll & (~XtIMXEvent)));
	  }
	else
	  {
	     XtAppNextEvent (app_cont, &ev);
	     TtaHandleOneEvent (&ev);
	  }
#else  /* WWW_XWINDOWS */
	GetMessage (&msg, NULL, 0, 0);
	TranslateMessage (&msg);
	TtaHandleOneWindowEvent (&msg);
#endif /* !WWW_XWINDOWS */
     }
}				/*TtaMainLoop */

/* ----------------------------------------------------------------------
   TtaGetMenuColor

   Returns the color used for the background of dialogue windows.
   ---------------------------------------------------------------------- */
Pixel               TtaGetMenuColor ()
{
   UserErrorCode = 0;
   return (Pixel) BgMenu_Color;
}


/* ----------------------------------------------------------------------
   TtaGetButtonColor

   Returns the color used for displaying active buttons in forms.

   ---------------------------------------------------------------------- */
Pixel               TtaGetButtonColor ()
{
   UserErrorCode = 0;
   return (Pixel) Button_Color;
}


#ifdef WWW_XWINDOWS
/* ----------------------------------------------------------------------
   TtaGetCurrentDisplay

   Returns the current display descriptor.

   ---------------------------------------------------------------------- */
Display            *TtaGetCurrentDisplay ()
{
   UserErrorCode = 0;
   return TtDisplay;
}
#endif /* WWW_XWINDOWS */


/* ----------------------------------------------------------------------
   TtaGetScreenDepth

   Returns the current screen depth.

   ---------------------------------------------------------------------- */
int                 TtaGetScreenDepth ()
{
   UserErrorCode = 0;
   return TtWDepth;
}

/* ----------------------------------------------------------------------
   TtaClickElement

   Returns document and element clicked.

   ---------------------------------------------------------------------- */
#ifdef __STDC__
void                TtaClickElement (Document * document, Element * element)
#else  /* __STDC__ */
void                TtaClickElement (document, element)
Document           *document;
Element            *element;

#endif /* __STDC__ */

{
   int                 frame;
   PtrAbstractBox             pave;
   PtrDocument         pDoc;
   int                 Vue;
   boolean             Assoc;

   UserErrorCode = 0;
   if (element == NULL || document == NULL)
      TtaError (ERR_invalid_parameter);
   else
     {
	*element = (Element) None;
	*document = (Document) None;

	DesignationPave (&frame, (int *) &pave);
	if (frame == 0 || pave == 0)
	   return;
	else
	  {
	     *element = (Element) pave->AbElement;
	     DocVueFen (frame, &pDoc, &Vue, &Assoc);
	     *document = (Document) IdentDocument (pDoc);
	  }
     }
}


/* ---------------------------------------------------------------------- */
/* | TtaGiveSelectPosition retourne la position de la souris lors du    | */
/* |            dernier clic de designation par rapport a l'element     | */
/* |            Ces positions sont exprimees en pixels.                 | */
/* ---------------------------------------------------------------------- */
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
   PtrAbstractBox             pAb;
   ViewFrame            *pFrame;

   /* verifie le parametre document */
   UserErrorCode = 0;
   if (document == 0)
      TtaError (ERR_invalid_document_parameter);
   else if (view == 0 || element == 0)
      TtaError (ERR_invalid_parameter);
   else
     {
	frame = GetWindowNumber (document, view);
	if (frame != DesFen)
	   TtaError (ERR_no_selection_in_view);
	else
	  {
	     /* calcule la position de la souris dans la boite designee */
	     pFrame = &FntrTable[frame - 1];
	     pAb = ((PtrElement) element)->ElAbstractBox[view - 1];
	     if (pAb == NULL)
		TtaError (ERR_no_selection_in_view);
	     else
	       {
		  while (pAb->AbPresentationBox && pAb->AbNext != NULL)
		     pAb = pAb->AbNext;
		  *X = DesX + pFrame->FrXOrg - pAb->AbBox->BxXOrg;
		  *Y = DesY + pFrame->FrYOrg - pAb->AbBox->BxYOrg;
	       }
	  }
     }
}

/* End Of Module */
