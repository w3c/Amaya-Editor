#ifndef __INCLUDE_P2CSS_H__
#define __INCLUDE_P2CSS_H__

#include "genericdriver.h"

typedef enum {
    UnknownRPI,
    NormalRPI,
    ModifiedRPI,
    RemovedRPI
} RPIstate;

typedef struct PRuleInfo {
    struct PRuleInfo *NextRPI;

    RPIstate state;

    PSchema pschema;

    GenericContext ctxt;

    /* the CSS rule */
    char *selector;
    char *css_rule;
} PRuleInfo, *PRuleInfoPtr;

#ifdef __STDC__
extern void PrintRPI(PRuleInfoPtr rpi);
extern int PSchema2CSS(Document doc,PSchema gPres,int zoom, int back,
                       char *output_file);
extern PRuleInfoPtr NewRPI(Document doc);
extern void FreeRPI(PRuleInfoPtr rpi);
extern void CleanListRPI(PRuleInfoPtr *list);
extern int CmpRPI(PRuleInfoPtr cour,PRuleInfoPtr rpi);
extern PRuleInfoPtr SearchRPI(PRuleInfoPtr cour, PRuleInfoPtr list);
extern void RemoveRPI(Document doc, PRuleInfoPtr cour);
extern PRuleInfoPtr PSchema2RPI(Document doc,PSchema gPres,int zoom, int back);
extern int BuildRPIList(Document doc, PSchema gPres, int zoom, int back, 
                        char *buf, int size, char *first);
#else
extern void PrintRPI(/*PRuleInfoPtr rpi*/);
extern int PSchema2CSS(/*doc,gPres,zoom,back,output_file*/);
extern PRuleInfoPtr NewRPI(/* Document doc */);
extern void FreeRPI(/*PRuleInfoPtr rpi*/);
extern void CleanListRPI(/* PRuleInfoPtr *list */);
extern int CmpRPI(/*PRuleInfoPtr cour,PRuleInfoPtr rpi*/);
extern PRuleInfoPtr SearchRPI(/*PRuleInfoPtr cour, PRuleInfoPtr list*/);
extern void RemoveRPI(/* Document doc, PRuleInfoPtr cour */);
extern PRuleInfoPtr PSchema2RPI(/*Document doc,PSchema gPres,zoom,back*/);
extern int BuildRPIList(/* doc, gPres, zoom,back,buf, size, first */);
#endif

#endif /* __INCLUDE_P2CSS_H__ */
