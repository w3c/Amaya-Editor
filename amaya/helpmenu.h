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

#define BROWSING       0
#define SELECTING      1
#define SEARCHING      2
#define VIEWS          3
#define CREATING       4
#define LINKS          5
#define CHANGING       6
#define TABLES         7
#define MATH           8
#define SVG            9
#define IMAGEMAPS     10
#define CSS           11
#define ATTRIBUTES    12
#define SPELLCHECKING 13
#define PUBLISHING    14
#define PRINTING      15
#define NUMBERING     16
#define MAKEBOOK      17
#define ANNOTATE      18
#define CONFIGURE     19
#define SHORTCUTS     20
#define ACCESS        21

#endif /* AMAYA_HELP_H */





