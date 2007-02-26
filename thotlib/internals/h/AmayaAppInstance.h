#ifdef _WX

#ifndef __AMAYAAPPINSTANCE_H__
#define __AMAYAAPPINSTANCE_H__

#include "wx/wx.h"
#include "wx/ipc.h"
#include "wx/snglinst.h"

#include "AmayaApp.h"

class AmayaURLGrabberServer;

class AmayaAppInstance
{
 public:
  AmayaAppInstance( AmayaApp * p_amaya_app );
  virtual ~AmayaAppInstance();
  
  bool IsAnotherAmayaRunning();
  bool SendURLToOtherAmayaInstance(const wxString & url);
  void StartURLGrabberServer();
  void RegisterOpenURLCallback( OpenURLCallback callback );
  void CallOpenURLCallback( char * url );

 protected:
  AmayaApp * m_pAmayaApp;
  wxSingleInstanceChecker * m_pSingleInstance_Checker;
  wxString m_InstanceName;

  AmayaURLGrabberServer * m_pURLGrabberServer;
  wxString m_ServicePort;
  wxString m_ServiceTopic;
  wxString m_ServiceHostname;
};


class AmayaURLGrabberConnection : public wxConnection
{
 public:
  AmayaURLGrabberConnection( AmayaURLGrabberServer & owner ) : m_Owner(owner) {}
  ~AmayaURLGrabberConnection() {}
  bool OnPoke(const wxString& topic, const wxString& item, wxChar *data, int size, wxIPCFormat format);
 protected:
  AmayaURLGrabberServer &m_Owner;
};

class AmayaURLGrabberServer: public wxServer
{
 public:
  friend class AmayaURLGrabberConnection;

  AmayaURLGrabberServer( const wxString & accepted_topic ) :
    m_AcceptedTopic(accepted_topic),
    m_pURLOpenCallback(NULL)
    {}
  ~AmayaURLGrabberServer() {}
  void RegisterOpenURLCallback( OpenURLCallback callback );
  OpenURLCallback GetOpenURLCallback();
  wxConnectionBase *OnAcceptConnection(const wxString& topic)
    {
      wxLogDebug(_T("OnAcceptConnection"));
      if ( topic == m_AcceptedTopic )
	return new AmayaURLGrabberConnection( *this );
      return NULL;
    }
 protected:
  wxString        m_AcceptedTopic;
  OpenURLCallback m_pURLOpenCallback;
};

#endif // __AMAYAAPPINSTANCE_H__
  
#endif /* #ifdef _WX */
