/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1996.
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

int                 AMAYA;

/* Index of tamaya message table */

#define AM_FILES                        0
#define AM_OPEN_URL	                1
#define AM_CLEAR	                2
#define AM_SAVE_LOCAL                   3
#define AM_ATTRIBUTE	                4
#define AM_HREF_VALUE                   5
#define AM_GET_ANSWER	                6
#define AM_TEXT		                7
#define AM_NAME		                8
#define AM_PASSWORD	                9
#define AM_DEF_CLASS	               10
#define AM_APPLY_CLASS	               11
#define AM_SEL_CLASS	               12
#define AM_CREATED	               13
#define AM_FETCHING	               14
#define AM_LOADING	               15
#define AM_CANNOT_LOAD	               16
#define AM_DOCUMENT_LOADED             17
#define AM_SAVED	               18
#define AM_SEL_TARGET	               19
#define AM_INVALID_TARGET	       20
#define AM_CANNOT_ACCESS	       21
#define AM_ACCESS_DENIED	       22
#define AM_ACCESS_INTERRUPTED	       23
#define AM_LOCATION	               24
#define AM_TITLE	               25
#define AM_DOC_MODIFIED	               26
#define AM_RED_FETCHING	               27
#define AM_REDIRECTIONS_LIMIT	       28
#define AM_CANNOT_CREATE_FILE	       29
#define AM_WAITING_FOR_SOCKET	       30
#define AM_PROG_READ	               31
#define AM_PROG_WRITE	               32
#define AM_WAITING_FOR_CONNECTION      33
#define AM_CONTACTING_HOST	       34
#define AM_LOOKING_HOST	               35
#define AM_UNKNOWN_STATUS              36
#define AM_XT_ERROR	               37
#define AM_GET_AUTHENTICATION 	       38
#define AM_AUTHENTICATION_FAILURE      39
#define AM_LOADED_NO_DATA	       40
#define AM_LOAD_ABORT	               41
#define AM_NOT_AVAILABLE_RETRY	       42
#define AM_ELEMENT_LOADED	       43
#define AM_REMOTE_SAVING	       44
#define AM_CANNOT_SAVE		       45
#define AM_SYSTEM_ERROR		       46
#define AM_INVALID_ANCHOR1	       47
#define AM_INVALID_ANCHOR2	       48
#define AM_GET_UNSUPPORTED_PROTOCOL    49
#define AM_PUT_UNSUPPORTED_PROTOCOL    50
#define AM_UNSUPPORTED_FEATURE	       51
#define AM_WROTE_OBJECT		       52
#define AM_BAD_URL		       53
#define AM_CACHE_ERROR		       54
#define AM_NO_TRANS		       55
#define AM_TRANS_FAILED		       56
#define AM_TRANS_PARSE_ERROR	       57
#define AM_ADD                         58
#define AM_SHOW                        59
#define AM_BROWSE                      60
#define AM_EXTERNAL_CSS                61
#define AM_DELETE_CSS                  62
#define AM_DELETE                      63
#define AM_SAVE                        64
#define AM_SAVE_AS                     65
#define AM_RELOAD                      66
#define AM_NEW                         67
#define AM_CSS                         68
#define AM_WARNING_SAVE_OVERWRITE      69
#define AM_URL_SAVE_FAILED             70
#define AM_SAVE_DISK                   71
#define AM_BCOPY_IMAGES                72
#define AM_BTRANSFORM_URL              73
#define AM_DOC_LOCATION                74
#define AM_IMAGES_LOCATION             75
#define AM_OBJECT_LOCATION             76
#define AM_DOC_STYLE                   77
#define AM_USER_PREFERENCES            78
#define AM_CSS_FILE_1                  79
#define AM_RULE_LIST_FILE_1            80
#define AM_CSS_FILE_2                  81
#define AM_RULE_LIST_FILE_2            82
#define AM_SELECT_EXTERNAL_STYLE_SHEET 83
#define AM_DELETE_STYLE_SHEET          84
#define AM_SYS_ERROR_TMPL              85
#define AM_GET_USER_NAME               86
#define AM_PAWWWD                      87
#define AM_FILE_TO_SAVE_IN             88
#define AM_ACCOUNT                     89
#define AM_METHOD_NOT_ALLOWED          90
#define AM_LOCATION_MOVED              91
#define AM_SETUP_RULES                 92
#define AM_WAITING_REQUESTS            93
#define AM_CONNECTION_CLOSED           94
#define AM_CACHE_GC                    95
#define AM_AUTHENTICATION_ERROR        96
#define AM_UNKNOWN_URL                 97
#define AM_SERVER_INTERNAL_ERROR       98

#define AMAYA_MSG_MAX                  99
