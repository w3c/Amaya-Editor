#ifdef _WX

/* it's possible to listen on different events type */
enum wxAmayaSocketCondition
  {
    WXAMAYASOCKET_READ = 1,
    WXAMAYASOCKET_WRITE,
    WXAMAYASOCKET_EXCEPTION
  };

/* the callback definition */
/* when a socket is registred, a callback of this forme is an argument */
typedef void (*wxAmayaSocketCallback) (int register_id,
				       int socket,
				       wxAmayaSocketCondition condition);

/* structure used to store the socket data */
class wxAmayaSocketContext
{
 public:
  int                    socket;
  wxAmayaSocketCondition condition;
  wxAmayaSocketCallback  callback;
};

class wxAmayaSocketEventLoop;

/*
 *  Description:  wxAmayaSocketEvent is an interface between libwww and amaya
 *                the interface is clear :
 *                 - register a socket :
 *                     when a condition occure, the accociated callback is called
 *                 - unregister a callback :
 *                     disconnect the callback from the socket
 *                 - check socket status :
 *                     poll the socket array to know if data is comming on a socket
 *       Author:  Stephane GULLY
 *      Created:  29/03/2004 04:45:34 PM CET
 *     Revision:  none
*/
class wxAmayaSocketEvent
{
 public:
  static void InitSocketEvent( wxAmayaSocketEventLoop * p_eventloop );
  static int  RegisterSocket( int socket,
			      wxAmayaSocketCondition condition,
			      wxAmayaSocketCallback  callback );
  static bool UnregisterSocket( int register_id );  
  static bool CheckSocketStatus( int bloking_time = 0 );

 protected:
  static bool RemoveSocketEntry( int entry );
  static int  AddSocketEntry();
  static bool SocketExists( int socket );

  static void Initialize();
  static bool m_IsInitialized;
  
  static const int MAX_SOCKET;

  static wxAmayaSocketContext m_RegistredSocket[];
  static int m_NbRegistredSocket;
  static int m_UsedSocket[];
  static int m_SocketFDMax;

  /* used to optimize the polling */
  /* when there is no socket, do not poll */
  static wxAmayaSocketEventLoop * m_pEventLoop;
};

#endif /* _WX */
