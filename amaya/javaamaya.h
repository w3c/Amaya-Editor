/*
 * javaamaya.h : exports a programmable API to the Amaya specific
 *               functions not directly related to the Thot library.
 */

#ifndef __AMAYA_API_H__
#define __AMAYA_API_H__

/*
 * Part of the interface extracted from init.c
 */

#ifdef __STDC__
extern Document IsDocumentLoaded ( char *documentURL );
extern void ExtractParameters ( char *aName,
                                char *parameters );
extern void ExtractSuffix ( char *aName,
                            char *aSuffix );
extern void ResetStop ( Document document );
extern void ActiveTransfer ( Document document );
extern void StopTransfer ( Document document,
                           View view );
extern void SetCharEmphasis ( Document document,
                              View view );
extern void SetCharStrong ( Document document,
                            View view );
extern void SetCharCode ( Document document,
                          View view );
extern void InitFormAnswer ( Document document,
                             View view );
extern void InitConfirm ( Document document,
                          View view,
                          char *label );
extern void OpenDocInNewWindow ( Document document,
                                 View view );
extern void OpenDoc ( Document document,
                      View view );
extern void Reload ( Document document,
                     View view );
extern void ShowStructure ( Document document,
                            View view );
extern void ShowAlternate ( Document document,
                            View view );
extern void ShowLinks ( Document document,
                        View view );
extern void ShowToC ( Document document,
                      View view );
extern void ShowMapAreas ( Document document,
                           View view );
extern boolean NormalizeFile ( char *src,
                               char *target );
extern void SectionNumbering ( Document document,
                               View view );
extern void MakeBook ( Document document,
                       View view );
extern void RealTimeHTML ( Document document,
                           View view );
extern void HelpAmaya ( Document document,
                        View view );
extern void AmayaClose ( Document document,
                         View view );

#else /* __STDC__ */

extern Document IsDocumentLoaded (/* char *documentURL */);
extern void ExtractParameters (/* char *aName,
                                  char *parameters */);
extern void ExtractSuffix (/* char *aName,
                              char *aSuffix */);
extern void ResetStop (/* Document document */);
extern void ActiveTransfer (/* Document document */);
extern void StopTransfer (/* Document document,
                             View view */);
extern void SetCharEmphasis (/* Document document,
                                View view */);
extern void SetCharStrong (/* Document document,
                              View view */);
extern void SetCharCode (/* Document document,
                            View view */);
extern void InitFormAnswer (/* Document document,
                               View view */);
extern void InitConfirm (/* Document document,
                            View view,
                            char *label */);
extern void OpenDocInNewWindow (/* Document document,
                                   View view */);
extern void OpenDoc (/* Document document,
                        View view */);
extern void Reload (/* Document document,
                       View view */);
extern void ShowStructure (/* Document document,
                              View view */);
extern void ShowAlternate (/* Document document,
                              View view */);
extern void ShowLinks (/* Document document,
                          View view */);
extern void ShowToC (/* Document document,
                        View view */);
extern void ShowMapAreas (/* Document document,
                             View view */);
extern boolean NormalizeFile (/* char *src,
                                 char *target */);
extern void CallbackDialogue (/* int ref,
                                 int typedata,
                                 char *data */);
extern void InitAmaya (/* NotifyEvent * event */);
extern void SectionNumbering (/* Document document,
                                 View view */);
extern void MakeBook (/* Document document,
                         View view */);
extern void RealTimeHTML (/* Document document,
                             View view */);
extern void HelpAmaya (/* Document document,
                          View view */);
extern void AmayaClose (/* Document document,
                           View view */);
#endif

/*
 * Part of the interface extracted from javaamaya.c
 */

#ifdef __STDC__

extern char *GetUserAgentName ( void );
extern char *GetDocumentURL ( Document doc );
extern char *AmayaGetMessage ( int msg );
extern int PopUpAuthDialog ( int doc );
extern char *GetUserString ( void );
extern char *GetPasswdString ( void );
extern char *GetAnswerString ( void );
extern char *GetTempDirectory ( void );

#else /* __STDC__ */

extern char *GetUserAgentName (/* void */);
extern char *GetDocumentURL (/* Document doc */);
extern char *AmayaGetMessage (/* int msg */);
extern int PopUpAuthDialog (/* int doc */);
extern char *GetUserString (/* void */);
extern char *GetPasswdString (/* void */);
extern char *GetAnswerString (/* void */);
extern char *GetTempDirectory (/* void */);

#endif /* __STDC__ */
#endif /* __AMAYA_API_H__ */
