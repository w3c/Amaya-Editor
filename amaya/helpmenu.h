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
"Document.html",
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
#define DOCUMENT       4
#define CREATING       5
#define ATTRIBUTES     6
#define EDITCHAR       7
#define CHANGING       8
#define LINKS          9
#define TABLES        10
#define IMAGEMAPS     11
#define MATH          12
#define SVG           13
#define XML           14
#define CSS           15
#define SEARCHING     16
#define SPELLCHECKING 17
#define PUBLISHING    18
#define PRINTING      19
#define NUMBERING     20
#define MAKEBOOK      21
#define ANNOTATE      22
#define CONFIGURE     23
#define SHORTCUTS     24
#define ACCESS        25

#endif /* AMAYA_HELP_H */





