/* ------------------------------------------------
   XML Tags definition 
   ---------------------------------------------- */
#ifdef _I18N_
#      ifdef _WINDOWS 
#            define OPEN_XML            L"<?xml"
#            define CLOSE_XML           L"?>\n"
#            define OPEN_COMMENT        L"<!-- "
#            define CLOSE_COMMENT       L"-->\n"
#            define OPEN_TAG            L"<"
#            define OPEN_END_TAG        L"</"
#            define CLOSE_TAG           L">\n"
#            define CLOSE_EMPTY_TAG     L"/>\n"
#            define XML_SPACE           L" "
#            define DEPTH_SPACE         L"  "
#            define LT_ENTITY           L"lt"
#            define GT_ENTITY           L"gt"
#            define AMP_ENTITY          L"amp"
#            define APOS_ENTITY         L"apos"
#            define QUOT_ENTITY         L"quot"
#            define VOID_ENTITY         L"zzzz"
#      else  /* !_WINDOWS */
#      endif /* _WINDOWS */
#else  /* !_I18N_ */
#      define OPEN_XML         "<?xml"
#      define CLOSE_XML        "?>\n"
#      define OPEN_COMMENT     "<!-- "
#      define CLOSE_COMMENT    "-->\n"
#      define OPEN_TAG         "<"
#      define OPEN_END_TAG     "</"
#      define CLOSE_TAG        ">\n"
#      define CLOSE_EMPTY_TAG  "/>\n"
#      define XML_SPACE        " "
#      define DEPTH_SPACE      "  "
#      define LT_ENTITY        "lt"
#      define GT_ENTITY        "gt"
#      define AMP_ENTITY       "amp"
#      define APOS_ENTITY      "apos"
#      define QUOT_ENTITY      "quot"
#      define VOID_ENTITY      "zzzz"
#endif /* _I18N_ */
/* ----------------------------------------------------------------------
   Schemas addition: Warning: they aren't real schema but are used 
		     for application only:
		     - Thot schema is used for special Thot attribute
		       such has element content, presentation element
		     - XML schema is used for native XML and XLL attributes
		       but as it's standart, prefix shall not be putted, it
		       is not the case now
  ---------------------------------------------------------------------- */
#ifdef _I18N_
#      ifdef _WINDOWS
#            define THOT_SCHEMA        L"thot:"  
#            define XML_SCHEMA         L"xml:"
#      else /* !_WINDOWS  */
#      endif /* _WINDOWS  */
#else  /*  !_I18N_  */
#      define THOT_SCHEMA        "thot:"  
#      define XML_SCHEMA         "xml:"
#endif  /* _I18N_ */
/* ---------------------------------------------------------------------
   Main tag addition: They are Thot tags added for thot structure extensions
                      so they don't respect the S schema structure
   -------------------------------------------------------------------- */
#ifdef _I18N_
#      ifdef _WINDOWS 
#            define PRES_SCHEMA_TAG    L"presschema" /* presentation schema element */
#            define PARAM_TAG          L"parameters" /* parameter element. not used */
#            define BR_TAG             L"br"         /* BreakLine element */
#      else /* !_WINDOWS */
#      endif /* _WINDOWS */
#else  /* !_I18N_ */
#      define PRES_SCHEMA_TAG    "presschema" /* presentation schema element */
#      define PARAM_TAG          "parameters" /* parameter element. not used */
#      define BR_TAG             "br"         /* BreakLine element */
#endif /* _I18N_ */
/* ---------------------------------------------------------------------
   Xml Element parameter addition: XML/XLL attributes addition
   -------------------------------------------------------------------- */
#ifdef _I18N_
#      ifdef _WINDOWS 
#            define XML_LANG_ATTR      L"lang"
#            define XML_NAMESPACE_ATTR L"xmlns"
#            define XML_ID_ATTR        L"id"
#            define XML_HREF_ATTR      L"href"
#            define XML_LINK_ATTR      L"link"
#            define XML_INLINE_ATTR    L"inline"
#      else /*  !_WINDOWS  */
#      endif /*  _WINDOWS */
#else  /*  !_I18N_ */
#      define XML_LANG_ATTR      "lang"
#      define XML_NAMESPACE_ATTR "xmlns"
#      define XML_ID_ATTR        "id"
#      define XML_HREF_ATTR      "href"
#      define XML_LINK_ATTR      "link"
#      define XML_INLINE_ATTR    "inline"
#endif /*  _I18N_ */
/* ---------------------------------------------------------------------
   Thot Element parameter addition: They are artificial attributes 
   -------------------------------------------------------------------- */
#ifdef _I18N_
#      ifdef _WINDOWS 
#            define P_PREFIX_ATTR     L"prefix"       /* for PSchema */ 
#            define P_SCHEMA_ATTR     L"presentation" /* for PSchema */
#            define STYLE_ATTR        L"style"        /* Specific presentation like CSS */
#      else  /* !_WINDOWS */
#      endif  /* _WINDOWS */
#else /* !_I!8N_  */
#     define P_PREFIX_ATTR      "prefix"       /* for PSchema */ 
#     define P_SCHEMA_ATTR      "presentation" /* for PSchema */
#     define STYLE_ATTR         "style"        /* Specific presentation like CSS */
#endif /* _I18N_ */
/* ---------------------------------------------------------------------
   Thot Basic element parameter addition: They are attribute that are
                            not expressed in S schema because they
			    represent elements' contents
   ------------------------------------------------------------------- */
#ifdef _I18N_
#      ifdef _WINDOWS
#            define HOLOPHRASTE_ATTR   L"holophraste"/* holophrased elements */
#            define GRAPH_CODE_ATTR    L"code"       /* graphic designation code */
#            define PG_NUM_ATTR        L"pgnumber"   /* page number */
#            define PG_VIEW_ATTR       L"pgview"     /* page view */
#            define PG_TYPE_ATTR       L"pgtype"     /* page type */
#            define PAIRED_ATTR        L"position"   /* paired element position, not used yet */
#            define LINE_POINTS_ATTR   L"points"     /* polyline points */
#            define SRC_ATTR           L"src"        /* picture source */
#            define FPAIR_ATTR         L"firstpair"  /* first paired elem */
#            define ASSOC_TREE_ATTR    L"float"      /* associated tree */
#            define NLABEL_ATTR        L"maxid"      /* number of ids used in the doc */
#      else  /* !_WINDOWS */
#      endif /* _WINDOWS */
#else  /* !_I18N_ */
#      define HOLOPHRASTE_ATTR   "holophraste"/* holophrased elements */
#      define GRAPH_CODE_ATTR    "code"       /* graphic designation code */
#      define PG_NUM_ATTR        "pgnumber"   /* page number */
#      define PG_VIEW_ATTR       "pgview"     /* page view */
#      define PG_TYPE_ATTR       "pgtype"     /* page type */
#      define PAIRED_ATTR        "position"   /* paired element position, not used yet */
#      define LINE_POINTS_ATTR   "points"     /* polyline points */
#      define SRC_ATTR           "src"        /* picture source */
#      define FPAIR_ATTR         "firstpair"  /* first paired elem */
#      define ASSOC_TREE_ATTR    "float"      /* associated tree */
#      define NLABEL_ATTR        "maxid"      /* number of ids used in the doc */
#endif /* !_I18N_ */

/* ---------------------------------------------------------------------
   Basic element attribute value addition: enumerate value for added 
                                        attributes .
   ------------------------------------------------------------------- */
#ifdef _I18N_
#      ifdef _WINDOWS 
#            define TRUE_VALUE         L"true"       /* for holophraste */
#            define DEFAULT_VALUE      L"default"    /* for prefix */
#            define XML_SIMPLE_LINK    L"simple"     /* for XLL link attribute */
#            define FIRST_ATTR_VALUE   L"first"      /* for paired position attribute */
#            define SECOND_ATTR_VALUE  L"second"     /* idem */
#      else /* !_WINDOWS  */
#      endif /* _WINDOWS */
#else  /* !_I18N_ */
#      define TRUE_VALUE         "true"       /* for holophraste */
#      define DEFAULT_VALUE      "default"    /* for prefix */
#      define XML_SIMPLE_LINK    "simple"     /* for XLL link attribute */
#      define FIRST_ATTR_VALUE   "first"      /* for paired position attribute */
#      define SECOND_ATTR_VALUE  "second"     /* idem */
#endif /* _I18N_ */
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
#define TEXT_UNIT             ((int)CharString)+1
#define GRAPHICS_UNIT         ((int)GraphicElem)+1
#define SYMBOL_UNIT           ((int)Symbol)+1
#define PICTURE_UNIT          ((int)Picture)+1
#define REFERENCE_UNIT        ((int)Refer)+1
#define PAGE_BREAK            ((int)PageBreak)+1

/* ---------------------------------------------------------------------
   Tables constants 
   ------------------------------------------------------------------- */
#define MAXNTYPE              255
#define MAXNTABLE             20
