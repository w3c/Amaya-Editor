/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef AMAYA_HELP_H
#define AMAYA_HELP_H

/****************************************
 Help menu definitions

 This file defines the Help menu entries. 
 Add each entry to the Manual string and
 then associate a macro to its index.
***************************************/

static char *Manual[] = {
  "Index.html",
"Browsing.html",
"Views.html",
"Selecting.html",
"Creating.html",
"Attributes.html",
"EditChar.html",
"Changing.html",
"Links.html",
"Tables.html",
"ImageMaps.html",
"Math.html",
"SVG.html",
"Xml.html",
"StyleSheets.html",
"Searching.html",
"SpellChecking.html",
"Publishing.html",
"Printing.html",
"Numbering.html",
"MakeBook.html",
"Annotations.html",
"Configure.html",
"ShortCuts.html",
"Access.html"
};

/* index name... which relates to the precedent table */
#define INDEX          0
#define BROWSING       1
#define VIEWS          2
#define SELECTING      3
#define CREATING       4
#define ATTRIBUTES     5
#define EDITCHAR       6
#define CHANGING       7
#define LINKS          8
#define TABLES         9
#define IMAGEMAPS     10
#define MATH          11
#define SVG           12
#define XML           13
#define CSS           14
#define SEARCHING     15
#define SPELLCHECKING 16
#define PUBLISHING    17
#define PRINTING      18
#define NUMBERING     19
#define MAKEBOOK      20
#define ANNOTATE      21
#define CONFIGURE     22
#define SHORTCUTS     23
#define ACCESS        24

#endif /* AMAYA_HELP_H */





