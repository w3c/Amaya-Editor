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
TEXT("Browsing.html"),
TEXT("Selecting.html"),
TEXT("Searching.html"),
TEXT("Views.html"),
TEXT("Creating.html"),
TEXT("Links.html"),
TEXT("Changing.html"),
TEXT("Tables.html"),
TEXT("Math.html"),
TEXT("ImageMaps.html"),
TEXT("StyleSheets.html"),
TEXT("Attributes.html"),
TEXT("SpellChecking.html"),
TEXT("Publishing.html"),
TEXT("Printing.html"),
TEXT("Numbering.html"),
TEXT("MakeBook.html"),
TEXT("Configure.html"),
TEXT("Access.html")
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
#define IMAGEMAPS      9
#define CSS           10
#define ATTRIBUTES    11
#define SPELLCHECKING 12
#define PUBLISHING    13
#define PRINTING      14
#define NUMBERING     15
#define MAKEBOOK      16
#define CONFIGURE     17
#define ACCESS        18

#endif /* AMAYA_HELP_H */





