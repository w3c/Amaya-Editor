#ifndef BOOKMARKS_H
#define BOOKMARKS_H
/*
 *
 *  (c) COPYRIGHT MIT and INRIA, 1999-2005.
 *  Please first read the full copyright statement in file COPYRIGHT.
 * 
 */

/*
 * Author: J. Kahan (W3C/ERCIM)
 *
 */

#define THOT_EXPORT extern

/* RDF Property names */
#define BOOKMARK_NS "http://www.w3.org/2002/01/bookmark"
#define BM_BOOKMARK "Bookmark"
#define BMNS_BOOKMARK BOOKMARK_NS"#"BM_BOOKMARK
/* @@ JK: Change BOOKMARKS fo recalls everyhwere */
#define BM_BOOKMARKS "recalls"
#define BMNS_BOOKMARKS BOOKMARK_NS"#"BM_BOOKMARKS
#define BM_TOPIC "Topic"
#define BMNS_TOPIC BOOKMARK_NS"#"BM_TOPIC
#define BM_SUBTOPICOF "subTopicOf"
#define BMNS_SUBTOPICOF BOOKMARK_NS"#"BM_SUBTOPICOF
#define BM_HASTOPIC "hasTopic"
#define BMNS_HASTOPIC BOOKMARK_NS"#"BM_HASTOPIC
#define BM_NICKNAME "nickname"
#define BMNS_NICKNAME BOOKMARK_NS"#"BM_NICKNAME
#define BM_SEPARATOR "Separator"
#define BMNS_SEPARATOR BOOKMARK_NS"#"BM_SEPARATOR
#define BM_COLLECTION "collection"
#define BMNS_COLLECTION BOOKMARK_NS"#"BM_COLLECTION
#define BM_STATE "state"
#define BMNS_STATE BOOKMARK_NS"#"BM_STATE
#define BM_COLLAPSED "Collapsed"
#define BMNS_COLLAPSED BOOKMARK_NS"#"BM_COLLAPSED
#define BM_EXPANDED "Expanded"
#define BMNS_EXPANDED BOOKMARK_NS"#"BM_EXPANDED
#endif /* BOOKMARKS_H */

/* The local bookmarks file */
#define LOCAL_BOOKMARKS_FILE "bookmarks.rdf"

/* The default home topic name */
#define HOME_TOPIC_ANCHOR "#MyHomeTopic"

/* the default home topic title */
#define HOME_TOPIC_TITLE "My Home Topic"

/* the different kind of bookmark elements */
 
typedef enum _BookmarkElements {
  BME_UNKNOWN   = 0,
  BME_TOPIC     = 1, 
  BME_BOOKMARK  = 2, 
  BME_SEPARATOR = 3, 
  BME_SEEALSO   = 4
} BookmarkElements;

typedef struct _Bookmark
{
  ThotBool  isUpdate;   /* says if we're creating or updating an item */
  BookmarkElements bm_type; /* the type of element */
  char *parent_url;     /* the parent topic */
  List *parent_url_list;
  char *self_url;       /* the id that talks about this item */
  char *blank_id;       /* used for seeAlso's that are reifered */
  char *bookmarks;      /* for bookmarks, what we're bookmarking */
  char *title;      
  char *nickname;       /* an alternate title */
  char *author;
  char *created;
  char *modified;
  char *description;
  ThotBool collapsed;   /* says whether a topic is collapsed */
  char *context;   /* not used yet, but XPointer like */
} Bookmark, *BookmarkP;

typedef struct _BM_dyn_buffer {
  char *buffer;
  unsigned int lgbuffer;
} BM_dyn_buffer;


typedef struct _dynBookmark
{
  BM_dyn_buffer *parent_url;     /* the parent topic */
  BM_dyn_buffer *self_url;       /* the id that talks about this item */
  BM_dyn_buffer *bookmarks;      /* for bookmarks, what we're bookmarking */
  BM_dyn_buffer *title;      
  BM_dyn_buffer *author;
  BM_dyn_buffer *description;
  int ref;                       /* context reference */
  Document doc;                  /* the document from which this widget was 
				    called */
} dynBookmark, *dynBookmarkP;


/* @@ JK: a quick hack to be able to compile under Windows */
#ifdef _WINDOWS
#ifndef vsnprintf
#  define vsnprintf _vsnprintf
#endif
#endif
