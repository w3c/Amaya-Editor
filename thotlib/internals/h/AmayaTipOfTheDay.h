#ifdef _WX

#ifndef AMAYATIPOFTHEDAY_H_
#define AMAYATIPOFTHEDAY_H_

#include "wx/wx.h"
#include "wx/tipdlg.h"

class AmayaTipProvider : public wxTipProvider
{
public:
  AmayaTipProvider(size_t currentTip);
  virtual ~AmayaTipProvider();
  
  virtual wxString GetTip();
};


void TtaShowTipOfTheDay();


#endif /*AMAYATIPOFTHEDAY_H_*/

#endif /* #ifdef _WX */
