#ifdef _WX

#ifndef __AMAYAAPP_H__
#define __AMAYAAPP_H__

#include "wx/wx.h"
#include "wx/app.h"
#include "wx/strconv.h"

//class AmayaFrame;

class AmayaApp : public wxApp
{
public:

  virtual bool 	OnInit();
  virtual int 	OnExit();

  //virtual void OnInitCmdLine(wxCmdLineParser& parser);
    // Public function used to create Amaya main frame from Amaya kernel (C functions)
//    AmayaFrame * CreateAmayaFrame ( int frame, const char * frame_name, int x, int y, int w, int h );
 public:
  static wxCSConv conv_ascii;

protected:
  int		amaya_argc;
  char ** 	amaya_argv;


  void InitAmayaArgs();
  void ClearAmayaArgs();
//    wxLocale _locale; // locale we'll be using

    // function used to load/save workspace customisation
//    void LoadWorkspaceParameters( AmayaFrame * pFrame );
//    void SaveWorkspaceParameters( AmayaFrame * pFrame );

};

DECLARE_APP(AmayaApp)

#endif // __AMAYAAPP_H__
  
#endif /* #ifdef _WX */
