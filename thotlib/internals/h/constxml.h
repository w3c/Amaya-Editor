/* ------------------------------------------------
   XML Tags definition 
   ---------------------------------------------- */
#define OPEN_XML    	   "<?xml"
#define CLOSE_XML 	   "?>\n"
#define OPEN_COMMENT 	   "<!-- "
#define CLOSE_COMMENT 	   "-->\n"
#define OPEN_TAG 	   "<"
#define OPEN_END_TAG 	   "</"
#define CLOSE_TAG 	   ">\n"
#define CLOSE_EMPTY_TAG    "/>\n"
#define XML_SPACE          " "
#define DEPTH_SPACE        "  "
#define LT_ENTITY          "lt"
#define GT_ENTITY          "gt"
#define AMP_ENTITY         "amp"
#define APOS_ENTITY        "apos"
#define QUOT_ENTITY        "quot"
/* ----------------------------------------------------------------------
   Schemas addition: Warning: they aren't real schema but are used 
		     for application only:
		     - Thot schema is used for special Thot attribute
		       such has element content, presentation element
		     - XML schema is used for native XML and XLL attributes
		       but as it's standart, prefix shall not be putted, it
		       is not the case now
  ---------------------------------------------------------------------- */
#define THOT_SCHEMA        "thot:"  
#define XML_SCHEMA         "xml:"
/* ---------------------------------------------------------------------
   Main tag addition: They are Thot tags added for thot structure extensions
                      so they don't respect the S schema structure
   -------------------------------------------------------------------- */
#define DOCUMENT_TAG  	   "document"   /* main tag for namespace declaration
					   and unique root level */
#define PRES_SCHEMA_TAG    "presschema" /* presentation schema element */
#define PARAM_TAG     	   "parameters" /* parameter element. not used */
#define BR_TAG             "br"         /* BreakLine element */
/* ---------------------------------------------------------------------
   Xml Element parameter addition: XML/XLL attributes addition
   -------------------------------------------------------------------- */
#define XML_LANG_ATTR      "lang"
#define XML_NAMESPACE_ATTR "xmlns"
#define XML_ID_ATTR        "id"
#define XML_HREF_ATTR      "href"
#define XML_LINK_ATTR      "link"
#define XML_INLINE_ATTR    "inline"
/* ---------------------------------------------------------------------
   Thot Element parameter addition: They are artificial attributes 
   -------------------------------------------------------------------- */
#define P_PREFIX_ATTR      "prefix"       /* for PSchema */ 
#define P_SCHEMA_ATTR      "presentation" /* for PSchema */
#define STYLE_ATTR         "style"        /* Specific presentation like CSS */
/* ---------------------------------------------------------------------
   Thot Basic element parameter addition: They are attribute that are
                            not expressed in S schema because they
			    represent elements' contents
   ------------------------------------------------------------------- */
#define HOLOPHRASTE_ATTR   "holophraste"/* holophrased elements */
#define GRAPH_CODE_ATTR    "code"       /* graphic designation code */
#define PG_NUM_ATTR        "number"     /* page number, not used yet */
#define PG_VIEW_ATTR       "view"       /* page view, not used yet */
#define PAIRED_ATTR        "position"   /* paired element position, not used yet */
#define LINE_POINTS_ATTR   "points"     /* polyline points */
#define SRC_ATTR           "src"        /* picture source */
/* ---------------------------------------------------------------------
   Basic element attribute value addition: enumerate value for added 
                                        attributes .
   ------------------------------------------------------------------- */
#define TRUE_VALUE         "true"       /* for holophraste */
#define DEFAULT_VALUE      "default"    /* for prefix */
#define XML_SIMPLE_LINK    "simple"     /* for XLL link attribute */
#define FIRST_ATTR_VALUE   "first"      /* for paired position attribute */
#define SECOND_ATTR_VALUE  "second"     /* idem */

/* ---------------------------------------------------------------------
   Basic element type definition: API basic type definition
   ------------------------------------------------------------------- */
#define ENUM_ATTR_TYPE        0
#define NUM_ATTR_TYPE         1
#define TEXT_ATTR_TYPE        2
#define REF_ATTR_TYPE         3

/* ---------------------------------------------------------------------
   Basic element types: Warning: Pobably already defined ...
   ------------------------------------------------------------------- */
#define TEXT_UNIT             1
#define GRAPHICS_UNIT         2
#define SYMBOL_UNIT           3
#define PICTURE_UNIT          4
#define REFERENCE_UNIT        5
#define PAGE_BREAK            6

/* ---------------------------------------------------------------------
   Tables constants 
   ------------------------------------------------------------------- */
#define MAXNTYPE              255
#define MAXNTABLE             20
