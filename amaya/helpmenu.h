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

static STRING       Manual[] = {
  "Index.html",
"Browsing.html",
"Selecting.html",
"Searching.html",
"Views.html",
"Creating.html",
"Links.html",
"Changing.html",
"Tables.html",
"Math.html",
"SVG.html",
"ImageMaps.html",
"StyleSheets.html",
"Attributes.html",
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
#define SELECTING      2
#define SEARCHING      3
#define VIEWS          4
#define CREATING       5
#define LINKS          6
#define CHANGING       7
#define TABLES         8
#define MATH           9
#define SVG           10
#define IMAGEMAPS     11
#define CSS           12
#define ATTRIBUTES    13
#define SPELLCHECKING 14
#define PUBLISHING    15
#define PRINTING      16
#define NUMBERING     17
#define MAKEBOOK      18
#define ANNOTATE      19
#define CONFIGURE     20
#define SHORTCUTS     21
#define ACCESS        22

#endif /* AMAYA_HELP_H */





