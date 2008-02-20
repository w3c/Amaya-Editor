/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

#ifndef __AMAYA_JAVASCRIPT_H__
#define __AMAYA_JAVASCRIPT_H__

/* javascript.h : defines functions, properties and classes
 * used by the javascript interpreter
 *
 * Author: F. Wang
 *
 */

/*----------------------------------------------------------------------
           Instructions to complete the javascript/DOM support

  * Properties

     - Add a C identifier for your property in the enum list
       properties_names.

     - Complete or create a JSPropertySpec to integrate your new property.
        If your property must not be modified by the user, then add a
        JSPROP_READONLY flag in its definition.

     - In javascript.c, complete the functions that manipulate your property:
        getProperty, setProperty (if your property is not readonly)...

     - If your property does not belong to a class already declared, you
       have to create one first (see the instructions for Classes). Then
       apply JS_DefineProperties to all the object of the classes that
       have the property.

  * Methods (functions)

     - Add a declaration of a C function representing the method.

     - Complete or create a JSFunctionSpec to integrate your new method.

     - In javascript.c, create the C function representing the method.

     - If your method does not belong to a class already declared, you
       have to create one first (see the instructions for Classes). Then
       apply JS_DefineFunctions to all the object of the classes that
       have the method.

  * Classes (in DOM specification, they correspond to "Object" in the
             pages ECMA Script Language Binding)

     - Add a JSClass definition (see all the possible configurations
       at http://www.mozilla.org/js/spidermonkey/):
       
         > The first element is the name of your class

         > It is often useful to have private data for an object (for
            instance to find which Thot Element it represents) so 
           JSCLASS_HAS_PRIVATE flag to the second element.

         > The next four elements indicate the name of the C functions
           (for instance getProperty) that manipulate properties of an
           object, respectively to add, delete, get or set a property.
           If they are not used, then put JS_PropertyStub.
           
         > The last elements indicate other manipulations, that normally
           are not used, so you can let them to JS_EnumerateStub,
           JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub

      - Define Properties and Methods to use with your class. See the
        instructions above.

      - In javascript.c, make Amaya apply your class (and the corresponding
        property and methods) to a certain kind of objects:

        > If they are childs of the global object (such as Navigator,
          Screen...), then only one object is created for this class and
          this must directly be done in the function InitJavascript.

        > Otherwise, you have to analyse the type of Thot's Document/Node
          in the function Element_to_jsval in order to apply the class to
          each object of the expected type.

        >  To create an object with a given class use the function
           JS_NewObject. If the object is actually a property, you have to
           use JS_DefineObject. Note that in this case, it is not mandatory
           to define the property in javascript.h

  * Properties/Methods that need a specific treatment

       For some kinds of properties/methods, the instructions above are not
       enough. For example if X is an element, consider:
 
         X.getElementsByTagName("div").item(2)         [1]
         X.getElementsByTagName("div")                 [2]

       [1] returns the second "div" in the element X.
       [2] returns a NodeList of all the "div" in X. The problem is that the
         list is modified at the same time as the DOM tree, so you have to
         return an object that describes the way you obtain this list.

       This is done by using private data. See the function ObjectWithInfo.

  -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Functions used to manipulate property values
  -----------------------------------------------------------------------*/
static void finalizeObjectWithInfo(JSContext *cx, JSObject *obj);
static JSBool getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
static JSBool setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

/*----------------------------------------------------------------------
  Name of all properties used in getProperty, setProperty...
  -----------------------------------------------------------------------*/
enum properties_names
{
 WINDOW,
 WINDOW_CLOSED, WINDOW_FRAMES, WINDOW_LENGTH, WINDOW_OPENER,
 WINDOW_PARENT, WINDOW_SELF, WINDOW_TOP, WINDOW_NAME,

 NAVIGATOR_APPNAME, NAVIGATOR_APPCODENAME, NAVIGATOR_APPVERSION, 
 NAVIGATOR_USERAGENT, NAVIGATOR_COOKIESENABLED, NAVIGATOR_PLATFORM,
 NAVIGATOR_BROWSERLANGUAGE, /*NAVIGATOR_PLUGINS, */

 SCREEN_PIXELDEPTH, SCREEN_COLORDEPTH,
 SCREEN_AVAILHEIGHT, SCREEN_AVAILWIDTH,  SCREEN_HEIGHT, SCREEN_WIDTH,
 
 HISTORY_LENGTH,

 LOCATION_HREF, LOCATION_PATHNAME, LOCATION_HASH,
 /*, LOCATION_HOST, LOCATION_HOSTNAME, 
 LOCATION_PORT, LOCATION_PROTOCOL, LOCATION_SEARCH, */

 DOCUMENT_IMPLEMENTATION, DOCUMENT_DOCTYPE, DOCUMENT_DOCUMENTELEMENT,

 DOCUMENTTYPE_NAME, DOCUMENTTYPE_ENTITIES, DOCUMENTTYPE_NOTATIONS,

 ATTR_NAME, ATTR_SPECIFIED, ATTR_VALUE,

 ELEMENT_TAGNAME,

 NODE_NODENAME, NODE_NODEVALUE, NODE_NODETYPE, NODE_PARENTNODE, NODE_CHILDNODES, NODE_FIRSTCHILD,
 NODE_LASTCHILD, NODE_PREVIOUSSIBLING, NODE_NEXTSIBLING, NODE_ATTRIBUTES, NODE_OWNERDOCUMENT,

 NODELIST_LENGTH

 /* Put here methods that use the ObjectWithInfo function to create their PrivateData */
  , GETELEMENTSBYTAGNAME,
};

/*----------------------------------------------------------------------
  Global Object Window
  -----------------------------------------------------------------------*/
static JSBool window_alert(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_confirm(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_prompt(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_blur(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

/*
static JSBool window_clearInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_clearTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_focus(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_moveBy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_moveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_open(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_resizeBy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_resizeTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_scrollBy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_scrollTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_setInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool window_setTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
*/

static JSFunctionSpec window_functions[] =
{
  /* name of javascript function     name of C function       number of arguments  */
  {"alert"                        , window_alert          ,        1},
  {"confirm"                      , window_confirm        ,        1},
  {"prompt"                       , window_prompt         ,        2},
  {"close"                        , window_close          ,        0},
  {"blur"                         , window_blur           ,        0},
  {"print"                        , window_print          ,        0},

/*
  {"clearInterval"                , window_clearInterval  ,        1},
  {"clearTimeout"                 , window_clearTimeout   ,        1},
  {"focus"                        , window_focus          ,        0},
  {"moveBy"                       , window_moveBy         ,        2},
  {"moveTo"                       , window_moveTo         ,        2},
  {"open"                         , window_open           ,        4},
  {"resizeBy"                     , window_resizeBy       ,        2},
  {"resizeTo"                     , window_resizeTo       ,        2},
  {"scrollBy"                     , window_scrollBy       ,        2},
  {"scrollTo"                     , window_scrollTo       ,        2},
  {"setInterval"                  , window_setInterval    ,        3},
  {"setTimeout"                   , window_setTimeout     ,        3},
*/
  {0}
};

static JSPropertySpec window_properties[] =
{
  {"window"     ,      WINDOW,             JSPROP_READONLY},
  {"closed"     ,      WINDOW_CLOSED,      JSPROP_READONLY},
  {"frames"     ,      WINDOW_FRAMES,      JSPROP_INDEX & JSPROP_READONLY},
  {"length"     ,      WINDOW_LENGTH,      JSPROP_READONLY},
  {"opener"     ,      WINDOW_OPENER,      JSPROP_READONLY},
  {"parent"     ,      WINDOW_PARENT,      JSPROP_READONLY},
  {"self"       ,      WINDOW_SELF  ,      JSPROP_READONLY},
  {"top"        ,      WINDOW_TOP   ,      JSPROP_READONLY},
  {0}
};

static JSClass window_class =
{
  "Window",0,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

/*----------------------------------------------------------------------
  Object Navigator
  -----------------------------------------------------------------------*/

static JSBool navigator_javaEnabled(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool navigator_taintEnabled(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

static JSFunctionSpec navigator_functions[] =
{
  /* name of javascript function     name of C function       number of arguments  */
  {"javaEnabled"                  , navigator_javaEnabled  ,        0},
  {"taintEnabled"                 , navigator_taintEnabled ,        0},
  {0}
};

static JSPropertySpec navigator_properties[] =
{
/*  {"plugins"         ,      NAVIGATOR_PLUGINS          , JSPROP_INDEX & JSPROP_READONLY},*/
  {"appCodeName"     ,      NAVIGATOR_APPCODENAME      , JSPROP_READONLY},
  {"appName"         ,      NAVIGATOR_APPNAME          , JSPROP_READONLY},
  {"appVersion"      ,      NAVIGATOR_APPVERSION       , JSPROP_READONLY},
  {"browserLanguage" ,      NAVIGATOR_BROWSERLANGUAGE  , JSPROP_READONLY},
  {"cookieEnabled"   ,      NAVIGATOR_COOKIESENABLED   , JSPROP_READONLY},
  {"platform"        ,      NAVIGATOR_PLATFORM         , JSPROP_READONLY},
  {"userAgent"       ,      NAVIGATOR_USERAGENT        , JSPROP_READONLY},
  {0}
};

static JSClass navigator_class =
{
  "Navigator",0,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};


/*----------------------------------------------------------------------
  Object Screen
  -----------------------------------------------------------------------*/

static JSPropertySpec screen_properties[] =
{
  {"availHeight",      SCREEN_AVAILHEIGHT      , JSPROP_READONLY},
  {"availWidth" ,      SCREEN_AVAILWIDTH       , JSPROP_READONLY},
  {"colorDepth" ,      SCREEN_COLORDEPTH       , JSPROP_READONLY},
  {"pixelDepth" ,      SCREEN_PIXELDEPTH       , JSPROP_READONLY},
  {"height"     ,      SCREEN_HEIGHT           , JSPROP_READONLY},
  {"width"      ,      SCREEN_WIDTH            , JSPROP_READONLY},
  {0}
};

static JSClass screen_class =
{
  "Screen",0,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

/*----------------------------------------------------------------------
  Object History
  -----------------------------------------------------------------------*/

static JSBool history_back(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool history_forward(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
/*static JSBool history_go(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);*/

static JSFunctionSpec history_functions[] =
{
  /* name of javascript function     name of C function       number of arguments  */
  {"back"                            , history_back          ,        0},
  {"forward"                         , history_forward       ,        0},
/*  {"go"                              , history_go            ,        1},*/
  {0}
};

static JSPropertySpec history_properties[] =
{
  {"length"      ,      HISTORY_LENGTH            , JSPROP_READONLY},
  {0}
};

static JSClass history_class =
{
  "History",0,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

/*----------------------------------------------------------------------
  Object Location
  -----------------------------------------------------------------------*/

static JSBool location_replace(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool location_assign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool location_reload(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

static JSFunctionSpec location_functions[] =
{
  /* name of javascript function     name of C function       number of arguments  */
  {"reload"                         , location_reload          ,        0},
  {"assign"                         , location_assign          ,        1},
  {"replace"                        , location_replace         ,        1},
  {0}
};


static JSPropertySpec location_properties[] =
{
  {"href"            ,      LOCATION_HREF               , 0},
  {"pathname"        ,      LOCATION_PATHNAME           , JSPROP_READONLY},
  {"hash"            ,      LOCATION_HASH               , JSPROP_READONLY},
/*   {"host"            ,      LOCATION_HOST               , 0},
  {"hostname"        ,      LOCATION_HOSTNAME           , 0},
  {"port"            ,      LOCATION_PORT               , 0},
  {"protocol"        ,      LOCATION_PROTOCOL           , 0},
  {"search"          ,      LOCATION_SEARCH             , 0},*/
  {0}
};


static JSClass location_class =
{
  "Location",0,
  JS_PropertyStub,JS_PropertyStub,getProperty,setProperty,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

/*----------------------------------------------------------------------
  Object DOMException
  -----------------------------------------------------------------------*/
/*
static JSClass DOMException_class =
{
  "DOMException",0,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};*/

/*----------------------------------------------------------------------
  Object ExceptionCode
  -----------------------------------------------------------------------*/
/*
static JSClass ExceptionCode_class =
{
  "ExceptionCode",0,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};
*/

/* Definition ExceptionCode */
#define INDEX_SIZE_ERR                   1
#define DOMSTRING_SIZE_ERR               2
#define HIERARCHY_REQUEST_ERR            3
#define WRONG_DOCUMENT_ERR               4
#define INVALID_CHARACTER_ERR            5
#define NO_DATA_ALLOWED_ERR              6
#define NO_MODIFICATION_ALLOWED_ERR      7
#define NOT_FOUND_ERR                    8
#define NOT_SUPPORTED_ERR                9
#define INUSE_ATTRIBUTE_ERR             10 

/*----------------------------------------------------------------------
  Object DOMImplementation
  -----------------------------------------------------------------------*/
static JSBool DOMImplementation_hasFeature(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

static JSFunctionSpec DOMImplementation_functions[] =
{
  /* name of javascript function     name of C function             number of arguments  */
  {"hasFeature"                   , DOMImplementation_hasFeature   ,        2},
  {0}
};

static JSClass DOMImplementation_class =
{
  "DOMImplementation",0,
  JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};


/*----------------------------------------------------------------------
  Object DocumentFragment
  -----------------------------------------------------------------------*/
static JSClass DocumentFragment_class =
{
  "DocumentFragment",0,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

/*----------------------------------------------------------------------
  Object Document
  -----------------------------------------------------------------------*/
static JSBool _getElementsByTagName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

/*

static JSBool Document_createElement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Document_createDocumentFragment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Document_createCDATASection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Document_createProcessingInstruction(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Document_createAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Document_createEntityReference(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
*/

static JSFunctionSpec Document_functions[] =
{
  /* name of javascript function     name of C function                   number of arguments  */
/*  {"createElement"                , Document_createElement               ,        1},
  {"createDocumentFragment"       , Document_createDocumentFragment      ,        0},
  {"createCDATASection"           , Document_createCDATASection          ,        1},
  {"createProcessingInstruction"  , Document_createProcessingInstruction ,        2},
  {"createAttribute"              , Document_createAttribute             ,        1},
  {"createEntityReference"        , Document_createEntityReference       ,        1},*/
  {"getElementsByTagName"         , _getElementsByTagName        ,        1},
  {0}
};

static JSPropertySpec Document_properties[] =
{
  {"doctype"            ,    DOCUMENT_DOCTYPE          , JSPROP_READONLY},
  {"implementation"     ,    DOCUMENT_IMPLEMENTATION   , JSPROP_READONLY},
  {"documentElement"    ,    DOCUMENT_DOCUMENTELEMENT  , JSPROP_READONLY},
  {0}
};

static JSClass Document_class =
{
  "Document", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};


/*----------------------------------------------------------------------
  Object Node
  -----------------------------------------------------------------------*/
/*
static JSBool Node_createElement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Node_replaceChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Node_removeChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Node_appendChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);*/
static JSBool Node_hasChildNodes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
//static JSBool Node_cloneNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);*/

static JSFunctionSpec Node_functions[] =
{
/*  {"insertBefore"                 , Node_createElement                   ,        2},
  {"replaceChild"                 , Node_replaceChild                    ,        1},
  {"removeChild"                  , Node_removeChild                     ,        0},
  {"appendChild"                  , Node_appendChild                     ,        1},*/
  {"hasChildNodes"                , Node_hasChildNodes                   ,        0},
/*  {"cloneNode"                    , Node_cloneNode                       ,        1},*/
  {0}
};

static JSPropertySpec Node_properties[] =
{
  {"nodeName"            ,      NODE_NODENAME               , JSPROP_READONLY},
  {"nodeValue"           ,      NODE_NODEVALUE              , JSPROP_READONLY},
  {"nodeType"            ,      NODE_NODETYPE               , JSPROP_READONLY},
  {"parentNode"          ,      NODE_PARENTNODE             , JSPROP_READONLY},
  {"childNodes"          ,      NODE_CHILDNODES             , JSPROP_READONLY},
  {"firstChild"          ,      NODE_FIRSTCHILD             , JSPROP_READONLY},
  {"lastChild"           ,      NODE_LASTCHILD              , JSPROP_READONLY},
  {"previousSibling"     ,      NODE_PREVIOUSSIBLING        , JSPROP_READONLY},
  {"nextSibling"         ,      NODE_NEXTSIBLING            , JSPROP_READONLY},
  {"attributes"          ,      NODE_ATTRIBUTES             , JSPROP_READONLY},
  {"ownerDocument"       ,      NODE_OWNERDOCUMENT          , JSPROP_READONLY},
  {0}
};

/*static JSClass Node_class =
{
  "Node", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};*/

/* Definition NodeType */
#define ELEMENT_NODE                    1
#define ATTRIBUTE_NODE                  2
#define TEXT_NODE                       3
#define CDATA_SECTION_NODE              4
#define ENTITY_REFERENCE_NODE           5
#define ENTITY_NODE                     6
#define PROCESSING_INSTRUCTION_NODE     7
#define COMMENT_NODE                    8
#define DOCUMENT_NODE                   9
#define DOCUMENT_TYPE_NODE             10
#define DOCUMENT_FRAGMENT_NODE         11
#define NOTATION_NODE                  12

/*----------------------------------------------------------------------
  Object NodeList
  -----------------------------------------------------------------------*/
static JSBool NodeList_item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

static JSFunctionSpec NodeList_functions[] =
{
  {"item"                 , NodeList_item                   ,        1},
  {0}
};

static JSPropertySpec NodeList_properties[] =
{
  {"length"            ,      NODELIST_LENGTH               , JSPROP_READONLY},
  {0}
};

static JSClass NodeList_class =
{
  "NodeList", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,finalizeObjectWithInfo
};

/*----------------------------------------------------------------------
  Object NamedNodeMap
  -----------------------------------------------------------------------*/
static JSBool NamedNodeMap_getNamedItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool NamedNodeMap_setNamedItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
/*
static JSBool NamedNodeMap_removeNamedItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool NamedNodeMap_item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
*/

static JSFunctionSpec NamedNodeMap_functions[] =
{
  {"getNamedItem"                 , NamedNodeMap_getNamedItem           ,        1},
  {"setNamedItem"                 , NamedNodeMap_setNamedItem           ,        2},
  /*  {"removeNamedItem"              , NamedNodeMap_removeNamedItem        ,        1},
  {"item"                         , NamedNodeMap_item                   ,        1},*/
  {0}
};

static JSPropertySpec NamedNodeMap_properties[] =
{
/*  {"length"            ,      NAMEDNODEMAP_LENGTH               , JSPROP_READONLY},*/
  {0}
};

static JSClass NamedNodeMap_class =
{
  "NamedNodeMap", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,finalizeObjectWithInfo
};

/*----------------------------------------------------------------------
  Object CharacterData
  -----------------------------------------------------------------------*/
/*
static JSBool CharacterData_substringData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool CharacterData_appendData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool CharacterData_insertData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool CharacterData_deleteData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool CharacterData_replaceData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

static JSFunctionSpec CharacterData_functions[] =
{
  {"substringData"              , CharacterData_createElement                   ,        2},
  {"appendData"                 , CharacterData_createElement                   ,        1},
  {"insertData"                 , CharacterData_createElement                   ,        2},
  {"deleteData"                 , CharacterData_createElement                   ,        2},
  {"replaceData"                , CharacterData_createElement                   ,        3},
  {0}
};

static JSPropertySpec CharacterData_properties[] =
{
  {"data"                ,      CHARACTERDATA_DATA               , JSPROP_READONLY},
  {"length"              ,      CHARACTERDATA_LENGTH             , JSPROP_READONLY},
  {0}
};

static JSClass CharacterData_class =
{
  "CharacterData", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};
*/

/*----------------------------------------------------------------------
  Object Attr
  -----------------------------------------------------------------------*/
static JSPropertySpec Attr_properties[] =
{
  {"name"            ,      ATTR_NAME                   , JSPROP_READONLY},
  {"specified"       ,      ATTR_SPECIFIED              , JSPROP_READONLY},
  {"value"           ,      ATTR_VALUE                  , JSPROP_READONLY},
  {0}
};

static JSClass Attr_class =
{
  "Attr", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};


/*----------------------------------------------------------------------
  Object Element
  -----------------------------------------------------------------------*/
/*
static JSBool Element_getAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Element_setAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Element_removeAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Element_getAttributeNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Element_setAttributeNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Element_removeAttributeNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);*/
/*static JSBool Element_normalize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);*/

static JSFunctionSpec Element_functions[] =
{
/*  {"getAttribute"                 , Element_getAttribute                ,        1},
  {"setAttribute"                 , Element_setAttribute                ,        1},
  {"removeAttribute"              , Element_removeAttribute             ,        1},
  {"getAttributeNode"             , Element_getAttributeNode            ,        1},
  {"setAttributeNode"             , Element_setAttributeNode            ,        1},
  {"removeAttributeNode"          , Element_removeAttributeNode         ,        1},*/
  {"getElementsByTagName"         , _getElementsByTagName        ,        1},
/*  {"normalize"                    , Element_normalize                   ,        0},*/
  {0}
};

static JSPropertySpec Element_properties[] =
{
  {"tagName"            ,      ELEMENT_TAGNAME               , JSPROP_READONLY},
  {0}
};

static JSClass Element_class =
{
  "Element", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

/*----------------------------------------------------------------------
  Object Text
  -----------------------------------------------------------------------*/
/*
static JSBool Element_splitText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

static JSFunctionSpec Text_functions[] =
{
  {"splitText"                 , Text_splitText                   ,        1},
  {0}
};*/

static JSClass Text_class =
{
  "Text", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

/*----------------------------------------------------------------------
  Object Comment
  -----------------------------------------------------------------------*/
static JSClass Comment_class =
{
  "Comment", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

/*----------------------------------------------------------------------
  Object CDATASection
  -----------------------------------------------------------------------*/
static JSClass CDATASection_class =
{
  "CDATASection", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

/*----------------------------------------------------------------------
  Object DocumentType
  -----------------------------------------------------------------------*/

static JSPropertySpec DocumentType_properties[] =
{
  {"name"                ,      DOCUMENTTYPE_NAME           , JSPROP_READONLY},
  {"entities"            ,      DOCUMENTTYPE_ENTITIES       , JSPROP_READONLY},
  {"notations"           ,      DOCUMENTTYPE_NOTATIONS      , JSPROP_READONLY},
  {0}
};

static JSClass DocumentType_class =
{
  "DocumentType", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

/*----------------------------------------------------------------------
  Object Notation
  -----------------------------------------------------------------------*/
/*
static JSPropertySpec Notation_properties[] =
{
  {"publicId"            ,      NOTATION_PUBLICID            , JSPROP_READONLY},
  {"systemId"            ,      NOTATION_SYSTEMID            , JSPROP_READONLY},
  {0}
};
*/
static JSClass Notation_class =
{
  "Notation", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

/*----------------------------------------------------------------------
  Object Entity
  -----------------------------------------------------------------------*/
/*
static JSPropertySpec Entity_properties[] =
{
  {"publicId"            ,      ENTITY_PUBLICID               , JSPROP_READONLY},
  {"systemId"            ,      ENTITY_SYSTEMID               , JSPROP_READONLY},
  {"notationName"        ,      ENTITY_NOTATIONNAME           , JSPROP_READONLY},
  {0}
};
*/
static JSClass Entity_class =
{
  "Entity_properties", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

/*----------------------------------------------------------------------
  Object EntityReference
  -----------------------------------------------------------------------*/
static JSClass EntityReference_class =
{
  "EntityReference", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

/*----------------------------------------------------------------------
  Object ProcessingInstruction
  -----------------------------------------------------------------------*/
/*
static JSPropertySpec ProcessingInstruction_properties[] =
{
  {"target"            , PROCESSINGINSTRUCTION_TARGET     , JSPROP_READONLY},
  {"data"              , PROCESSINGINSTRUCTION_DATA       , JSPROP_READONLY},
  {0}
};
*/
static JSClass ProcessingInstruction_class =
{
  "ProcessingInstruction", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub,JS_PropertyStub,getProperty,JS_PropertyStub,
  JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};

#endif /* __AMAYA_JAVASCRIPT_H__ */
