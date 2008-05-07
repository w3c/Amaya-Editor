/*								       HTInit.c
**	CONFIGURATION-SPECIFIC INITIALIALIZATION
**
**	(c) COPYRIGHT MIT 1995.
**	Please first read the full copyright statement in the file COPYRIGH.
**	@(#) $Id$
**
**	General initialization functions.
**
**      @@@A lot of these should be moved to the various modules instead
**      of being here
*/

/* Library include files */
#include "wwwsys.h"
#include "WWWUtil.h"
#include "WWWCore.h"

#include "HTInit.h"				         /* Implemented here */

/* ------------------------------------------------------------------------- */

/*	BINDINGS BETWEEN A SOURCE MEDIA TYPE AND A DEST MEDIA TYPE (CONVERSION)
**	----------------------------------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTConverterInit (HTList * c)
{
  return;
}

/*	BINDINGS BETWEEN MEDIA TYPES AND EXTERNAL VIEWERS/PRESENTERS
**	------------------------------------------------------------
**	Not done automaticly - may be done by application!
**	The data objects are stored in temporary files before the external
**	program is called
*/
PUBLIC void HTPresenterInit (HTList * c)
{
  return;
}


/*	PRESENTERS AND CONVERTERS AT THE SAME TIME
**	------------------------------------------
**	Not done automaticly - may be done by application!
**	This function is only defined in order to preserve backward
**	compatibility.
*/
PUBLIC void HTFormatInit (HTList * c)
{
  return;
}

/*	BINDINGS BETWEEN A TRANSFER ENCODING AND CODERS / DECODERS
**	----------------------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTTransferEncoderInit (HTList * c)
{
#ifdef HT_ZLIB
    HTCoding_add(c, "deflate", NULL, HTZLib_inflate, 1.0);
#endif
    HTCoding_add(c, "chunked", HTChunkedEncoder, HTChunkedDecoder, 1.0);
}

/*	BINDINGS BETWEEN A CONTENT ENCODING AND CODERS / DECODERS
**	---------------------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTContentEncoderInit (HTList * c)
{
#ifdef HT_ZLIB
    HTCoding_add(c, "deflate", NULL, HTZLib_inflate, 1.0);
#endif /* HT_ZLIB */
}

/*	REGISTER BEFORE FILTERS
**	-----------------------
**	The BEFORE filters handle proxies, caches, rule files etc.
**	The filters are called in the order by which the are registered
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTBeforeInit (void)
{
  return;
}

/*	REGISTER AFTER FILTERS
**	----------------------
**	The AFTER filters handle error messages, logging, redirection,
**	authentication etc.
**	The filters are called in the order by which the are registered
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTAfterInit (void)
{
  return;
}

/*	REGISTER DEFAULT AUTHENTICATION SCHEMES
**	---------------------------------------
**	This function registers the BASIC access authentication
*/
PUBLIC void HTAAInit (void)
{
    HTAA_newModule ("basic", HTBasic_generate, HTBasic_parse, NULL,
		     HTBasic_delete);
#ifdef HT_MD5
    HTAA_newModule ("digest", HTDigest_generate, HTDigest_parse, 
		     HTDigest_updateInfo,  HTDigest_delete);
#endif /* HT_MD5 */
}

/*	REGISTER BEFORE AND AFTER FILTERS
**	---------------------------------
**	We register a commonly used set of BEFORE and AFTER filters.
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTNetInit (void)
{
  return;
}


/*	REGISTER CALLBACKS FOR THE ALERT MANAGER
**	----------------------------------------
**	We register a set of alert messages
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTAlertInit (void)
{
  return;
}

/*	REGISTER ALL KNOWN TRANSPORTS IN THE LIBRARY
**	--------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTTransportInit (void)
{
    HTTransport_add("tcp", HT_TP_SINGLE, HTReader_new, HTWriter_new);
    HTTransport_add("buffered_tcp", HT_TP_SINGLE, HTReader_new, HTBufferWriter_new);
#ifdef HT_MUX
    HTTransport_add("mux", HT_TP_INTERLEAVE, HTReader_new, HTBufferWriter_new);
#endif /* HT_MUX */
#ifndef NO_UNIX_IO
    HTTransport_add("local", HT_TP_SINGLE, HTReader_new, HTWriter_new);
#else
    HTTransport_add("local", HT_TP_SINGLE, HTANSIReader_new, HTANSIWriter_new);
#endif
}

/*	REGISTER ALL KNOWN PROTOCOLS IN THE LIBRARY
**	-------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTProtocolInit (void)
{
  return;
}

/*	REGISTER ALL KNOWN PROTOCOLS IN THE LIBRARY PREEMPTIVELY
**	--------------------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTProtocolPreemptiveInit (void)
{
  return;
}

/*	BINDINGS BETWEEN ICONS AND MEDIA TYPES
**	--------------------------------------
**	Not done automaticly - may be done by application!
**	For directory listings etc. you can bind a set of icons to a set of
**	media types and special icons for directories and other objects that
**	do not have a media type.
*/
PUBLIC void HTIconInit (const char * url_prefix)
{
  return;
}

/*	REGISTER ALL HTTP/1.1 MIME HEADERS
**	--------------------------------------------
**	Not done automaticly - may be done by application!
*/
PUBLIC void HTMIMEInit (void)
{
    struct {
        const char * string;
	HTParserCallback * pHandler;
    } fixedHandlers[] = {
	{"accept", &HTMIME_accept}, 
	{"accept-charset", &HTMIME_acceptCharset}, 
	{"accept-encoding", &HTMIME_acceptEncoding}, 
	{"accept-language", &HTMIME_acceptLanguage}, 
	{"accept-ranges", &HTMIME_acceptRanges}, 
	{"authorization", NULL},
	{"cache-control", &HTMIME_cacheControl},
	{"connection", &HTMIME_connection}, 
	{"content-encoding", &HTMIME_contentEncoding}, 
	{"content-length", &HTMIME_contentLength}, 
	{"content-range", &HTMIME_contentRange},
	{"content-transfer-encoding", &HTMIME_contentTransferEncoding}, 
	{"content-type", &HTMIME_contentType},
	{"digest-MessageDigest", &HTMIME_messageDigest}, 
	{"keep-alive", &HTMIME_keepAlive}, 
	{"link", &HTMIME_link},
	{"location", &HTMIME_location},
	{"max-forwards", &HTMIME_maxForwards}, 
	{"mime-version", NULL}, 
	{"pragma", &HTMIME_pragma},
        {"protocol", &HTMIME_protocol},
        {"protocol-info", &HTMIME_protocolInfo},
        {"protocol-request", &HTMIME_protocolRequest},
	{"proxy-authenticate", &HTMIME_authenticate},
	{"proxy-authorization", &HTMIME_proxyAuthorization},
	{"public", &HTMIME_public},
	{"range", &HTMIME_range},
	{"referer", &HTMIME_referer},
	{"retry-after", &HTMIME_retryAfter}, 
	{"server", &HTMIME_server}, 
	{"trailer", &HTMIME_trailer},
	{"transfer-encoding", &HTMIME_transferEncoding}, 
	{"upgrade", &HTMIME_upgrade},
	{"user-agent", &HTMIME_userAgent},
	{"vary", &HTMIME_vary},
	{"via", &HTMIME_via},
	{"warning", &HTMIME_warning},
	{"www-authenticate", &HTMIME_authenticate}, 
        {"authentication-info", &HTMIME_authenticationInfo},
        {"proxy-authentication-info", &HTMIME_proxyAuthenticationInfo}
    };
    unsigned int i;

    for (i = 0; i < sizeof(fixedHandlers)/sizeof(fixedHandlers[0]); i++)
        HTHeader_addParser(fixedHandlers[i].string, NO, 
			   fixedHandlers[i].pHandler);
}

