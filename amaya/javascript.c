/*
 *
 *  (c) COPYRIGHT INRIA and W3C, 2007
 *  Please first read the full copyright statement in file COPYRIGHT.
 *
 */

/*
 * This module contains functions to deal with javascript
 * thanks to SpiderMonkey (Mozilla's interpreter).
 * See also javascript.h before adding new javascript objects.
 *
 * Author: F. Wang
 *
 */         

/*----------------------------------------------------------------------
 *                 Structure of the javascript tree
 *
 * The object at top level is the "global object". There is no
 * W3C-standard that defines what are its direct children but
 * the organization in Amaya has been chosen to be compatible
 * with the one of usual browsers: 
 *
 *  Window (global object)
 *    |
 *    |_ Navigator
 *    |
 *    |_ Screen
 *    |
 *    |_ History
 *    |
 *    |_ Location
 *    |
 *    |_ Document
 *
 * (A detailled description can be found at
 *         http://www.w3schools.com/js/js_obj_htmldom.asp)
 * 
 * Actually, the global object has no name. Nevertheless, it
 * has a property "window" that returns itself, so everything
 * happens as if it was its name. This has consequences on the
 * javascript syntax: for example, if you want to call the
 * function confirm(), you can do confirm(), window.confirm()
 * or even window.window.window.window.confirm()
 *
 * Navigator, Screen, History and Location are objects that allow
 * to access general informations about supported features and
 * to execute several Amaya commands.
 *
 * Document represents the structure of the current document.
 * Its organization is a W3C standard called "Document Object Model".
 * The specifications can be found at http://www.w3.org/DOM/DOMTR
 * 
 * The javascript object Document is created at initialization according
 * to the structure of the Thot tree. Each javascript object representing
 * a node has a private data where the actual Thot node is stored.
 * Similarly, a Thot Element has a pointer "SpiderMonkey" on the
 * corresponding javascript object.
 *
  -----------------------------------------------------------------------*/

/* Included headerfiles */
#ifdef _WX
#include "wx/wx.h"
#endif /* _WX */

#undef THOT_EXPORT
#define THOT_EXPORT extern
#include "amaya.h"
#include "init_f.h"
#include "HTMLsave_f.h"
#include "HTMLbook_f.h"
#include "AHTURLTools_f.h"
#include "css.h"
#include "trans.h"
#include "undo.h"
#include "interface.h"
#include "MathML.h"
#include "parser.h"
#ifdef _SVG
#include "SVG.h"
#endif /* _SVG */
#include "document.h"

#include "MENUconf.h"
#include "EDITORactions_f.h"
#include "HTMLhistory_f.h"
#include "fetchXMLname_f.h"
#include "javascript_f.h"
#include "SVGbuilder_f.h"
#include "html2thot_f.h"
#include "HTMLtable_f.h"
#include "HTMLactions_f.h"
#include "HTMLedit_f.h"
#include "HTMLpresentation_f.h"
#include "MathMLbuilder_f.h"
#include "styleparser_f.h"
#include "trans_f.h"
#include "UIcss_f.h"
#ifdef _WX
#include "wxdialogapi_f.h"
#endif /* _WX */
#ifdef _WINGUI
#include "wininclude.h"
#endif /* _WINGUI */
#ifdef _WINDOWS
#include <commctrl.h>
#endif /* _WINDOWS */
#include "XLinkedit_f.h"

/* headerfiles and objects used by SpiderMonkey */
#include <jsapi.h>
#include "javascript.h"
static JSRuntime *grt = NULL;
static JSContext *gcx = NULL;
static JSObject *gobj;

/* Informations about the document where the script is run */
static Document jsdocument;
static ThotBool withDocType, useMath;
static View jsview;
static jsval documentElement;
static ThotBool ContinueScript = FALSE;

/* Functions that deal with the scripts */
static JSBool jsContinueScript();
static ThotBool CheckInitJavascript (Document doc, View view);
static ThotBool InitJavascript (Document document, View view);
static ThotBool BuildDocumentTree(Element ThotNode, JSObject *jsParentNode);
static void ExecuteAllScripts();
static void ExecuteScripts (Element Script);
//static void ExecuteJSfile (const char *location);
static ThotBool IsJavascript(Element Script);
static void printError(JSContext *cx, const char *message, JSErrorReport *report);

/* Functions used for conversion with SpiderMonkey's types */
static jsval Element_to_jsval(Element element, int direction);
static jsval str_to_jsval(JSContext *cx, char *string);
static char *jsval_to_str(JSContext *cx, jsval value);

/* Functions used in the DOM interface */
static int GetNodeType(JSObject *obj);
static jsval GetNodeName(JSContext *cx, JSObject *obj);
static jsval GetNodeValue(JSContext *cx, JSObject *obj);


/*----------------------------------------------------------------------
  Switch_JS_DOM
    Switch the javascript engine ON/OFF
  -----------------------------------------------------------------------*/
void Switch_JS_DOM(Document document, View view)
{
  /* Check whether a script can be executed inside the document*/
  if (DocumentTypes[document] == docSource ||
      DocumentTypes[document] == docText || DocumentTypes[document] == docLog
      || !strcmp (DocumentURLs[document], "empty")
      )
    {
    UpdateJavascriptMenus ();
    return;
    }

  if(ContinueScript && document == jsdocument)
    /* Switch OFF */
    StopJavascript (jsdocument);
  else
    {
    /* Switch ON */
    ContinueScript = CheckInitJavascript(document, view);
    UpdateJavascriptMenus ();

    if(ContinueScript)
      /* search and execute all scripts contained in jsdocument */
      ExecuteAllScripts();
    }
}

/*----------------------------------------------------------------------
  Execute_ACommand
    Display a dialog box where the user can enter a javascript command
  -----------------------------------------------------------------------*/
void Execute_ACommand(Document document, View view)
{
jsval rval;
char *rchar = NULL;

  if(ContinueScript)
    {
    #ifdef _WX
    /* Ask the user to enter a command */
    *JavascriptPromptValue = EOS;

    CreateTextDlgWX (
      BaseDialog + JavascriptPromptForm, BaseDialog + JavascriptPromptText,
      TtaGetViewFrame (document,  view), "Javascript/DOM",
      TtaGetMessage(AMAYA, AM_JAVASCRIPT_ENTER_A_COMMAND), "main();");

    TtaSetDialoguePosition ();
    TtaShowDialogue (BaseDialog + JavascriptPromptForm, FALSE);
    TtaWaitShowDialogue ();

    /* The script must be stopped. Do not evaluate the command */
    if(!ContinueScript)return;

    /* Execute the command and put its returned value to the status bar */
    JS_EvaluateScript(gcx, gobj, JavascriptPromptValue, strlen(JavascriptPromptValue) , DocumentURLs[jsdocument], 0, &rval);

    if(JSVAL_IS_VOID(rval))
      TtaSetStatus (jsdocument, 1, TtaGetMessage(AMAYA, AM_JAVASCRIPT_NO_RETURNED_VALUE), NULL);
    else
      {
      rchar = jsval_to_str(gcx, rval);
      /* TODO: handle returned value with \n */
      TtaSetStatus (jsdocument, 1, TtaGetMessage(AMAYA, AM_JAVASCRIPT_RETURNED_VALUE), rchar);
      }

    #endif /* _WX */
    }
}

/*----------------------------------------------------------------------
  Execute_External
  -----------------------------------------------------------------------*/
void Execute_External(Document document, View view)
{
  if(ContinueScript)
    {
    /* TODO */
     //ExecuteJSfile
    }
}

/*----------------------------------------------------------------------
  jsContinueScript
    This function returns a JSBool version of ContinueScript
    It must be returned at each end of a javascript functions, so that
    SpiderMonkey can interrupt the execution if necessary.
  -----------------------------------------------------------------------*/
static JSBool jsContinueScript()
{
return (ContinueScript ? JS_TRUE : JS_FALSE);
}

/*----------------------------------------------------------------------
  UpdateJavascriptMenus
    Update the javascript entries:
      - stop scripts
      - direct command
      - external script
  -----------------------------------------------------------------------*/
void UpdateJavascriptMenus ()
{
Document document;

  for (document = 1; document < DocumentTableLength; document++)
    {
    if(document == jsdocument && ContinueScript)
      {
      /* javascript item ON */
      #ifdef _WX
        TtaSetToggleItem (document, 1, JavascriptDom_, BSwitch_JS_DOM, TRUE);
        TtaSetItemOn (document, 1, JavascriptDom_, BExecuteACommand);
        TtaSetItemOn (document, 1, JavascriptDom_, BExecuteExternal);
      #endif /* _WX */
      }
    else
      {
      /* javascript item OFF */
      #ifdef _WX
        TtaSetToggleItem (document, 1, JavascriptDom_, BSwitch_JS_DOM, FALSE);
        TtaSetItemOff (document, 1, JavascriptDom_, BExecuteACommand);
        TtaSetItemOff (document, 1, JavascriptDom_, BExecuteExternal);
      #endif /* _WX */
      }
    }
}

/*----------------------------------------------------------------------
  CheckInitJavascript
    - Check if no script is running
    - Check the View
    - Check if the document is saved
    - Call InitJavascript
    - Check whether an error occured
  -----------------------------------------------------------------------*/
static ThotBool CheckInitJavascript(Document doc, View view)
{
if(gcx && JS_IsRunning(gcx))
  {
  /* Another script is running */
  TtaDisplayMessage(FATAL, TtaGetMessage(AMAYA, AM_JAVASCRIPT_IS_RUNNING));
  return FALSE;
  }

if(view != 1)
  {
  /* Check if script can be executed in this view */
  TtaDisplayMessage(FATAL, TtaGetMessage(AMAYA, AM_JAVASCRIPT_VIEW));
  return FALSE;
  }

/* TODO: check if there is no error in the xml source before executing a script. */

if(TtaIsDocumentModified (doc))
  {
  /* ask if the user wants to continue, save, or cancel */
  ExtraChoice = FALSE;
  UserAnswer = FALSE;
#ifdef _WX
  ConfirmError3L (doc, view, DocumentURLs[doc],
                    TtaGetMessage (AMAYA, AM_DOC_MODIFIED),
                    NULL,
                    TtaGetMessage (AMAYA, AM_DONT_SAVE),
                    TtaGetMessage (LIB, TMSG_BUTTON_SAVE));
#endif /* _WX */

      if (UserAnswer)
        {
          if (DocumentSource[doc] && !TtaIsDocumentUpdated (doc) &&
              TtaIsDocumentUpdated (DocumentSource[doc]))
            // the last updated version is the document source
            SaveDocument (DocumentSource[doc], view);
          else
            SaveDocument (doc, view);
        }
      else if (ExtraChoice)
        {
          TtaSetDocumentUnmodified (doc);
          if (DocumentSource[doc])
            TtaSetDocumentUnmodified (DocumentSource[doc]);
          /* remove the corresponding auto saved doc */
          RemoveAutoSavedDoc (doc);
        }
      else return FALSE;
  }

if(!InitJavascript (doc, view))
  {
  /* The Javascript/DOM objects could not be built */
  TtaDisplayMessage(FATAL, TtaGetMessage(AMAYA, AM_JAVASCRIPT_CAN_NOT_BUILD));
  return FALSE;
  }

return TRUE;
}

/*----------------------------------------------------------------------
  InitJavascript
  Build the javascript/DOM objects
  return TRUE if no error occured
  -----------------------------------------------------------------------*/
static ThotBool InitJavascript (Document document, View view)
{
JSObject *object;

/* initialize the JS run time */
  if(!grt)grt = JS_NewRuntime(8L * 1024L * 1024L);
  if (!grt)return FALSE;

/* (re)create a context and associate it with the JS run time
   (for the moment, you can't have several documents that are
    running javascript simultaneously) */

  if(gcx)
    {
    /* Check if no script/function are running, so that gcx
       can be safely destroyed. Normally, this has already
       be checked in CheckInitJavascript  */
    if(JS_IsRunning(gcx))
      return FALSE;
    else
      JS_DestroyContext(gcx);
    }

  gcx = JS_NewContext(grt, 8192);
  if (!gcx)return FALSE;

/* register document and view */
  jsdocument = document;
  jsview = view;

/* Check if jsdocument has a doctype */
  HasADoctype (jsdocument, &withDocType, &useMath); 

/* register error handler */
  JS_SetErrorReporter(gcx, printError);

/* create the global object (window) and the built-in JS objects */
  gobj = JS_NewObject(gcx, &window_class, NULL, NULL);
  if(!gobj)return FALSE;

  JS_InitStandardClasses(gcx, gobj);
  if(!gobj)return FALSE;
  JS_DefineFunctions(gcx, gobj, window_functions);
  JS_DefineProperties(gcx, gobj, window_properties);

/* create objects at top level : navigator, screen, history, location */
  object = JS_DefineObject(gcx, gobj, "navigator", &navigator_class, NULL, JSPROP_READONLY);
  if(!object)return FALSE;
  JS_DefineFunctions(gcx, object, navigator_functions);
  JS_DefineProperties(gcx, object, navigator_properties);

  object = JS_DefineObject(gcx, gobj, "screen", &screen_class, NULL, JSPROP_READONLY);
  if(!object)return FALSE;
  JS_DefineProperties(gcx, object, screen_properties);

  object = JS_DefineObject(gcx, gobj, "history", &history_class, NULL, JSPROP_READONLY);
  if(!object)return FALSE;
  JS_DefineFunctions(gcx, object, history_functions);
  JS_DefineProperties(gcx, object, history_properties);

  object = JS_DefineObject(gcx, gobj, "location", &location_class, NULL, JSPROP_READONLY);
  if(!object)return FALSE;
  JS_DefineFunctions(gcx, object, location_functions);
  JS_DefineProperties(gcx, object, location_properties);

/* recursively create the javascript object Document, according to Amaya's tree */
  documentElement = JSVAL_NULL;
  if(!BuildDocumentTree(TtaGetMainRoot(jsdocument), gobj))return FALSE;

  return TRUE;
}

/*----------------------------------------------------------------------
  StopJavascript
    If a script is running in doc, register the fact that
    the execution must be stopped.
  -----------------------------------------------------------------------*/
void StopJavascript (Document doc)
{
  if(doc == jsdocument)
    {
    jsdocument = 0;
    ContinueScript = FALSE;
    UpdateJavascriptMenus ();

    /* TODO: destroy dialog "enter a command", alert, confirm, and prompt */
    }
}

/*----------------------------------------------------------------------
  DestroyJavascript
  Free all objects used by SpiderMonkey
  This function must be call at exit
  -----------------------------------------------------------------------*/
void DestroyJavascript ()
{
if(gcx)JS_DestroyContext(gcx);
if(grt)JS_DestroyRuntime(grt);
JS_ShutDown();
}

/*----------------------------------------------------------------------
  BuildDocumentTree
  Create the javascript object representing ThotNode and all its children.
  jsParent represents the parent of ThotNode if it has any, or the global
  object (Window) otherwise.
-----------------------------------------------------------------------*/
static ThotBool BuildDocumentTree(Element ThotNode, JSObject *jsParent)
{
ThotBool BuildSubTree = TRUE;
int TypeOfNode;
JSClass *ClassToApply;
JSObject *jsNode, *object;
Element child;

if(jsParent == gobj)
  {
  /* It's the Document object */

  /* Create the object */
  jsNode = JS_DefineObject(gcx, jsParent, "document", &Document_class, NULL, JSPROP_READONLY);
  if(!jsNode)return FALSE;

  /* Create its basic properties and methods (DOM Core) */

  //JS_DefineFunctions(gcx, jsNode, Document_functions);
  JS_DefineFunctions(gcx, jsNode, Node_functions);
  JS_DefineProperties(gcx, jsNode, Node_properties);
  JS_DefineProperties(gcx, jsNode, Document_properties);

  /* Create its implementation property */
  object = JS_DefineObject(gcx, jsNode, "implementation", &DOMImplementation_class, NULL, JSPROP_READONLY);
  if(!object)return FALSE;
  JS_DefineFunctions(gcx, object, DOMImplementation_functions);

  /* Create its childNodes property */
  object = JS_DefineObject(gcx, jsNode, "childNodes", &NodeList_class, NULL, JSPROP_READONLY);
  if(!object)return FALSE;
  JS_DefineProperties(gcx, object, NodeList_properties);
  JS_DefineFunctions(gcx, object, NodeList_functions);
    
  /* Add properties and methods according to the type of document */
     /* TODO: DOM HTML */
  }
else
  {
  /* It's not the root : analyse the type of node */
  TypeOfNode = TtaGetElementType(ThotNode).ElTypeNum;

  /* Choose the class to apply according to the type of ThotNode */
  switch(TypeOfNode)
    {
    case HTML_EL_DOCTYPE_line:
    case HTML_EL_Document_URL:
    case HTML_EL_C_Empty:
      return TRUE; /* These elements are not taken into account to build the DOM tree */
    break;

    case HTML_EL_DOCTYPE:
      ClassToApply = NULL;
      BuildSubTree = FALSE;

      jsNode = JS_DefineObject(gcx, jsParent, "doctype", &DocumentType_class, NULL, JSPROP_READONLY);
      if(!jsNode)return FALSE;

    break;

    case TEXT_UNIT:
      ClassToApply = &Text_class;
      BuildSubTree = FALSE;
    break;

    case HTML_EL_Comment_:
      ClassToApply = &Comment_class;
      BuildSubTree = FALSE;
    break;

    case HTML_EL_CDATA:
      ClassToApply = &CDATASection_class;
      BuildSubTree = FALSE;
    break;

    default:
      ClassToApply = &Element_class;
    break;
    }

  if(ClassToApply)
    {
    /* The object has not been already created. Create it with the given Class */
    jsNode = JS_NewObject(gcx, ClassToApply, NULL, jsParent);
    if(!jsNode)return FALSE;
    }

  /* Create its basic properties and methods (DOM Core) */
  JS_DefineProperties(gcx, jsNode, Node_properties);
  JS_DefineFunctions(gcx, jsNode, Node_functions);

  /* Create its childNodes property */
  object = JS_DefineObject(gcx, jsNode, "childNodes", &NodeList_class, NULL, JSPROP_READONLY);
  if(!object)return FALSE;
  JS_DefineProperties(gcx, object, NodeList_properties);
  JS_DefineFunctions(gcx, object, NodeList_functions);

  /* Add properties and methods according to the type of ThotNode */
  switch(TypeOfNode)
    {
    case HTML_EL_DOCTYPE:
      JS_DefineProperties(gcx, jsNode, DocumentType_properties);
    break;

    case HTML_EL_HTML:
      documentElement = OBJECT_TO_JSVAL(jsNode);
    break;

    case TEXT_UNIT:
      //JS_DefineFunctions(gcx, jsNode, Text_functions);
    break;

    default:
    break;
    }
  }

/* register ThotNode and jsNode as a private data of each other
   in order to easily manipulate both trees */
  JS_SetPrivate(gcx, jsNode, (void *)ThotNode);
  TtaSetSpiderMonkey(ThotNode, (void *)jsNode);

/* Apply BuildDocumentTree to the ThotNode's children */
if(BuildSubTree)
  {
  child = TtaGetFirstChild(ThotNode);

  while(child)
    {
    BuildDocumentTree(child, jsNode);
    TtaNextSibling(&child);
    }
  }
return TRUE;
}

/*----------------------------------------------------------------------
  ExecuteAllScripts
  Execute all scripts contained in the jsdocument
  -----------------------------------------------------------------------*/
static void ExecuteAllScripts()
{
Element child, child2;

  child = TtaGetMainRoot(jsdocument);

  while(child)
    {
    child2 = child;

    while(child2)
      {
      child = child2;

      if(IsJavascript(child))
        {
        ExecuteScripts (child);
        if(!ContinueScript)return;
        child2 = child;
        TtaNextSibling(&child2);
        }
      else child2 = TtaGetFirstChild(child);
      }

    child = TtaGetSuccessor(child);
    }
}

/*----------------------------------------------------------------------
  ExecuteScripts
  Execute scripts contained in an element SCRIPT_
  -----------------------------------------------------------------------*/
static void ExecuteScripts (Element Script)
{
Element child, child2, child3;
jsval rval;
const char *url = DocumentURLs[jsdocument];
char *text = NULL;
int len, i, len2, line;
Language	lang;

ElementType    elType;
AttributeType  attrType;
Attribute      attr;

/* Execute script located at the location indicated by src */
elType = TtaGetElementType (Script);
attrType.AttrSSchema = elType.ElSSchema;
attrType.AttrTypeNum = HTML_ATTR_script_src;
attr = TtaGetAttribute (Script, attrType);
if(attr)
  {
  /* TODO */
  //TtaGiveTextAttributeValue(attr, text, &len);
  if(!ContinueScript)return;
  }

/* Execute each script of (SCRIPT_)'s children
   Normally there should only be one child but it is also possible to have something like :
    SCRIPT_
      TEXT_UNIT <-- contained only spaces and '\n'
      CDATA     <-- contained the script that have to be executed
      TEXT_UNIT <-- contained only spaces and '\n'
*/

child = TtaGetFirstChild(Script);
while(child)
  {
  line = TtaGetElementLineNumber(child);

  switch(TtaGetElementType(child).ElTypeNum)
    {
    case HTML_EL_TEXT_UNIT:
       /* Execute the script contained in the TEXT_UNIT */
       len = TtaGetTextLength(child);
       text = (char *)TtaGetMemory (len + 1);
       if(text)
         {
         TtaGiveTextContent(child, (unsigned char *)text, &len, &lang);
         JS_EvaluateScript(gcx, gobj, text, len , url, line, &rval);
         }
    break;

    case HTML_EL_CDATA:
    case HTML_EL_Comment_:

      /* Compute size of script */
      len = 0;
      child2 = TtaGetFirstChild(child);
      while(child2)
        {
        child3 = TtaGetFirstChild(child2);
        if(TtaGetElementType(child3).ElTypeNum == HTML_EL_TEXT_UNIT)len+=TtaGetTextLength(child3);
        len++;
        TtaNextSibling(&child2);
        }

      text = (char *)TtaGetMemory (len + 1);

      if(text)
        {
        /* Group all the text in one string */

        child2 = TtaGetFirstChild(child);
        i = 0;
        while(child2)
          {
          child3 = TtaGetFirstChild(child2);
          if(TtaGetElementType(child3).ElTypeNum == HTML_EL_TEXT_UNIT)
            {
            len2 = TtaGetTextLength(child3);
            TtaGiveTextContent(child3, (unsigned char *)(&(text[i])), &len2, &lang);
            i+=len2;
            }
          text[i] = '\n';i++;
          TtaNextSibling(&child2);
          }

        text[i] = '\0';
      
        /* Execute the script */
        JS_EvaluateScript(gcx, gobj, text, len , url, line, &rval);
        }
    break;

    default:
    break;
    }
  if(text)TtaFreeMemory (text);

  if(!ContinueScript)return;
  TtaNextSibling(&child);
  }

}

/*----------------------------------------------------------------------
  ExecuteJSfile
  Execute the js file located at url
  -----------------------------------------------------------------------*/
/*static void ExecuteJSfile (const char *location)
{
JSScript *ScriptInFile;

         TODO

    //ScriptInFile = JS_CompileFile(gcx, gobj, text);
    //if(ScriptInFile)JS_ExecuteScript(gcx, gobj, ScriptInFile, &rval);
}*/

/*----------------------------------------------------------------------
  IsJavascript
  Check whether an element is a SCRIPT_ of type javascript
  -----------------------------------------------------------------------*/
static ThotBool IsJavascript(Element element)
{
ElementType    elType;
AttributeType  attrType;
Attribute      attr;
#define BUFLEN 20
char buffer[BUFLEN];
int len;

if(!element)return FALSE;
if(TtaGetElementType(element).ElTypeNum != HTML_EL_SCRIPT_)return FALSE;

elType = TtaGetElementType (element);
attrType.AttrSSchema = elType.ElSSchema;
attrType.AttrTypeNum = HTML_ATTR_content_type;
attr = TtaGetAttribute (element, attrType);
if(!attr)return FALSE;

len = TtaGetTextAttributeLength(attr);
if(len >= BUFLEN)return FALSE;

TtaGiveTextAttributeValue(attr, buffer, &len);
if(strcmp("text/javascript", buffer))return FALSE; else return TRUE;
}

/*----------------------------------------------------------------------
  printError
  Report javascript error
  -----------------------------------------------------------------------*/
static void printError(JSContext *cx, const char *message, JSErrorReport *report)
{
StopJavascript (jsdocument);
TtaDisplayMessage(FATAL, TtaGetMessage(AMAYA, AM_JAVASCRIPT_SCRIPT_ERROR),
   report->filename, report->lineno, message);
}

/*----------------------------------------------------------------------
  Element_to_jsval
  Convert a Thot element into an element of the javascript tree
  If there is no corresponding element in the javascript tree, look
  the next one in the given direction. If no direction is specified
  or no element is found, JSVAL_NULL is returned.
  -----------------------------------------------------------------------*/
static jsval Element_to_jsval(Element element, int direction)
{
JSObject *jsobj = NULL;

while(element)
  {
  jsobj = (JSObject *)TtaGetSpiderMonkey(element);

  if(jsobj)
    return OBJECT_TO_JSVAL(jsobj);
  else
    {
    /* look for another element */
    switch(direction)
      {
      case -1:
        TtaPreviousSibling(&element);
      break;

      case +1:
        TtaNextSibling(&element);
      break;

      default:
        return JSVAL_NULL;
      break;
      }
    }

  }

return JSVAL_NULL;
}

/*----------------------------------------------------------------------
  str_to_jsval
  Convert a string to a javascript value
  -----------------------------------------------------------------------*/
static jsval str_to_jsval(JSContext *cx, char *string)
{
return STRING_TO_JSVAL(JS_NewStringCopyZ(cx, string));
}

/*----------------------------------------------------------------------
  jsval_to_str
  Convert a javascript value to a string
  -----------------------------------------------------------------------*/
static char *jsval_to_str(JSContext *cx, jsval value)
{
return (JSVAL_IS_VOID(value) ? NULL : JS_GetStringBytes(JS_ValueToString(cx, value)) );
}

/*----------------------------------------------------------------------
  GetNodeType
  return the type of a javascript object representing a node
  -----------------------------------------------------------------------*/
static int GetNodeType(JSObject *obj)
{
char *text = (char *)(((JSClass *)JS_GetClass(obj))->name);

if(!strcmp((&Element_class)->name, text))return ELEMENT_NODE;
if(!strcmp((&Attr_class)->name, text))return ATTRIBUTE_NODE;
if(!strcmp((&Text_class)->name, text))return TEXT_NODE;
if(!strcmp((&CDATASection_class)->name, text))return CDATA_SECTION_NODE;
if(!strcmp((&EntityReference_class)->name, text))return ENTITY_REFERENCE_NODE;
if(!strcmp((&Entity_class)->name, text))return ENTITY_NODE;
if(!strcmp((&ProcessingInstruction_class)->name, text))return PROCESSING_INSTRUCTION_NODE;
if(!strcmp((&Comment_class)->name, text))return COMMENT_NODE;
if(!strcmp((&Document_class)->name, text))return DOCUMENT_NODE;
if(!strcmp((&DocumentType_class)->name, text))return DOCUMENT_TYPE_NODE;
if(!strcmp((&DocumentFragment_class)->name, text))return DOCUMENT_FRAGMENT_NODE;
if(!strcmp((&Notation_class)->name, text))return NOTATION_NODE;

/* unknown type of node */
return 0;
}

/*----------------------------------------------------------------------
  GetNodeName
  return the name of a javascript object representing a node
  -----------------------------------------------------------------------*/
static jsval GetNodeName(JSContext *cx, JSObject *obj)
{
Element element;

switch(GetNodeType(obj))
  {
  case ELEMENT_NODE:
    element = (Element)JS_GetPrivate(cx, obj);
    return str_to_jsval(cx, TtaGetElementTypeOriginalName(TtaGetElementType(element)) );
  break;
  case ATTRIBUTE_NODE:
  break;
  case TEXT_NODE:
    return str_to_jsval(cx, "#text");
  break;
  case CDATA_SECTION_NODE:
    return str_to_jsval(cx, "#cdata-section");
  break;
  case ENTITY_REFERENCE_NODE:
  break;
  case ENTITY_NODE:
  break;
  case PROCESSING_INSTRUCTION_NODE:
  break;
  case COMMENT_NODE:
    return str_to_jsval(cx, "#comment");
  break;
  case DOCUMENT_NODE:
    return str_to_jsval(cx, "#document");
  break;
  case DOCUMENT_TYPE_NODE:
  break;
  case DOCUMENT_FRAGMENT_NODE:
    return str_to_jsval(cx, "#document-fragment");
  break;
  case NOTATION_NODE:
  break;
  default:
  break;
  }
return JSVAL_NULL;
}


/*----------------------------------------------------------------------
  GetNodeValue
  return the value of a javascript object representing a node
  -----------------------------------------------------------------------*/
static jsval GetNodeValue(JSContext *cx, JSObject *obj)
{
jsval name;
char *text = NULL;
Element element;
int len;
Language	lang;

switch(GetNodeType(obj))
  {
  case ATTRIBUTE_NODE:
  break;
  case TEXT_NODE:
    element = (Element)JS_GetPrivate(cx, obj);
    len = TtaGetTextLength(element);
    text = (char *)TtaGetMemory (len + 1);
    if(text)
      {
      TtaGiveTextContent(element, (unsigned char *)text, &len, &lang);
      name = str_to_jsval(cx, text);
      TtaFreeMemory(text);
      return name;
      }
  break;
  case CDATA_SECTION_NODE:
  break;
  case PROCESSING_INSTRUCTION_NODE:
  break;
  case COMMENT_NODE:
  break;
  default:
  break;
  }
return JSVAL_NULL;
}

/*----------------------------------------------------------------------
  setProperty
  -----------------------------------------------------------------------*/
static JSBool setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
char *urlname;

if(JSVAL_IS_INT(id))
  {
  switch (JSVAL_TO_INT(id))
    {
    case LOCATION_HREF:
      urlname = jsval_to_str(cx, *vp);
      if(urlname)
        {
        GetAmayaDoc (urlname, NULL, jsdocument, jsdocument, CE_ABSOLUTE, TRUE, NULL, NULL);
        return JS_FALSE;
        }
    break;

    default:
    break;
    }
  }
return jsContinueScript();
}

/*----------------------------------------------------------------------
  getProperty
  -----------------------------------------------------------------------*/
static JSBool getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
Element element;
ElementType    elType;
char *text;
int n, width, height;

if(JSVAL_IS_INT(id))
  {
  switch (JSVAL_TO_INT(id))
    {
    case WINDOW:
    case WINDOW_FRAMES:
    case WINDOW_OPENER:
    case WINDOW_SELF:
    case WINDOW_TOP:
    case WINDOW_PARENT:
      *vp = OBJECT_TO_JSVAL(gobj);
    break;
    case WINDOW_CLOSED: *vp = JSVAL_FALSE; break;
    case WINDOW_LENGTH: *vp = JSVAL_ZERO; break;

    case NAVIGATOR_BROWSERLANGUAGE:
      *vp = str_to_jsval(cx, TtaGetLanguageName(TtaGetDefaultLanguage()));
    break;

    case NAVIGATOR_APPNAME:
    case NAVIGATOR_USERAGENT:
      *vp = str_to_jsval(cx, "Amaya");
    break;

    case NAVIGATOR_APPCODENAME:
      *vp = str_to_jsval(cx, "The W3C's editor/browser");
    break;

    case NAVIGATOR_APPVERSION:
      *vp = str_to_jsval(cx, "9.55");
    break;

    case NAVIGATOR_PLATFORM:
       #ifdef _WINDOWS
          *vp = str_to_jsval(cx, "Windows");
       #else
          #ifdef _MACOS
          *vp = str_to_jsval(cx, "MacOS");
          #else
            #ifdef _UNIX
               *vp = str_to_jsval(cx, "Unix");
            #endif
          #endif
       #endif
    break;

    case NAVIGATOR_COOKIESENABLED:
      *vp = JSVAL_FALSE;
    break;

    case SCREEN_PIXELDEPTH:
    case SCREEN_COLORDEPTH:
      *vp = INT_TO_JSVAL(TtaGetScreenDepth());
    break;

    /* TODO : WIDTH and HEIGHT must be for the whole screen */
    case SCREEN_AVAILWIDTH:
    case SCREEN_WIDTH:
      TtaGiveWindowSize(jsdocument, jsview, UnPixel, &width, &height);
      *vp = INT_TO_JSVAL(width);
    break;

    case SCREEN_AVAILHEIGHT:
    case SCREEN_HEIGHT:
      TtaGiveWindowSize(jsdocument, jsview, UnPixel, &width, &height);
      *vp = INT_TO_JSVAL(height);
    break;

    case LOCATION_HREF:
    case LOCATION_HASH:
      /* TODO: for href, return the complete url (with #xxx) */       
      *vp = str_to_jsval(cx, DocumentURLs[jsdocument]);
    break;

    case LOCATION_PATHNAME:
      text = GetBaseURL (jsdocument);
      if(text)
        {
        *vp = str_to_jsval(cx, text);
        TtaFreeMemory (text);
        }
    break;

    case DOCUMENT_DOCUMENTELEMENT:
      *vp = documentElement;
    break;

    case DOCUMENT_DOCTYPE:
      /* If the document has no doctype, return null */
      if(!withDocType)*vp = JSVAL_NULL;
    break;

    case DOCUMENTTYPE_NAME:
      elType = TtaGetElementType (TtaGetMainRoot (jsdocument));
      text = TtaGetSSchemaName (elType.ElSSchema);
      *vp = str_to_jsval(cx, text);
    break;

    case NODE_NODETYPE:
      *vp = INT_TO_JSVAL(GetNodeType(obj));
    break;

    case NODE_NODENAME:
      *vp = GetNodeName(cx, obj);
    break;

    case NODE_NODEVALUE:
      *vp = GetNodeValue(cx, obj);
    break;

    case NODE_ATTRIBUTES:
      if(GetNodeType(obj) == ELEMENT_NODE)
        {
        /* TODO: return a NamedNodeMap */
        *vp = JSVAL_NULL;
        }
      else *vp = JSVAL_NULL;
    break;

    case NODE_PARENTNODE:
      element = (Element)JS_GetPrivate(cx, obj);
      element = TtaGetParent(element);
      *vp = Element_to_jsval(element, 0);
    break;

    case NODE_FIRSTCHILD:
      element = (Element)JS_GetPrivate(cx, obj);
      element = TtaGetFirstChild(element);
      *vp = Element_to_jsval(element, +1);
    break;

    case NODE_LASTCHILD:
      element = (Element)JS_GetPrivate(cx, obj);
      element = TtaGetLastChild(element);
      *vp = Element_to_jsval(element, -1);
    break;

    case NODE_PREVIOUSSIBLING:
      element = (Element)JS_GetPrivate(cx, obj);
      TtaPreviousSibling(&element);
      *vp = Element_to_jsval(element, -1);
    break;

    case NODE_NEXTSIBLING:
      element = (Element)JS_GetPrivate(cx, obj);
      TtaNextSibling(&element);
      *vp = Element_to_jsval(element, +1);
    break;

    case NODE_OWNERDOCUMENT:
      element = (Element)JS_GetPrivate(cx, obj);
      if(TtaGetParent(element))
        {
        element = TtaGetMainRoot(jsdocument);
        *vp = Element_to_jsval(element, 0);
        }
      else
        *vp = JSVAL_NULL;
    break;

    case NODELIST_LENGTH:
      /* TODO: deal with the case getElementsByTagName */
      element = (Element)JS_GetPrivate(cx, JS_GetParent(cx, obj));
      element = TtaGetFirstChild(element);
      for(n = 0; element != NULL ; n++)TtaNextSibling(&element);
      *vp = INT_TO_JSVAL(n);
    break;

    default:
    break;
    }
  }
return jsContinueScript();
}

/*----------------------------------------------------------------------
  Object Window
  -----------------------------------------------------------------------*/

static JSBool window_alert(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  char *msg = NULL;

  if(JSVAL_IS_VOID(*argv))
    {
    JS_ReportError(gcx, "Missing mandatory argument 1 of function alert");
    return JS_FALSE;
    }
  else
    {
    msg = jsval_to_str(cx, *argv);
    /* TODO: make a non-modal window, to allow javascript engine to be switch OFF */
    TtaDisplayMessage(INFO, msg);
    return jsContinueScript();
    }
}

static JSBool window_confirm(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  char *msg = jsval_to_str(cx, *argv);
  InitConfirm (jsdocument, jsview, msg);
  *rval = BOOLEAN_TO_JSVAL(UserAnswer);
  return jsContinueScript();
}

static JSBool window_prompt(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  char *msg = jsval_to_str(cx, argv[0]);
  char *value = jsval_to_str(cx, argv[1]);

#ifdef _WX
  *JavascriptPromptValue = EOS;
  CreateTextDlgWX (BaseDialog + JavascriptPromptForm, BaseDialog + JavascriptPromptText, TtaGetViewFrame (jsdocument, jsview), "Prompt", msg, value);
  TtaSetDialoguePosition ();
  TtaShowDialogue (BaseDialog + JavascriptPromptForm, FALSE);
  TtaWaitShowDialogue ();
  if(!ContinueScript)return JS_FALSE;

  *rval = str_to_jsval(cx, JavascriptPromptValue);
#endif /* _WX */

  return jsContinueScript();
}

static JSBool window_close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  AmayaClose(jsdocument, jsview);
  return jsContinueScript();
}

static JSBool window_blur(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  TtaUnselect(jsdocument);
  return jsContinueScript();
}

static JSBool window_print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  SetupAndPrint(jsdocument, jsview);
  return jsContinueScript();
}


/*----------------------------------------------------------------------
  Object Navigator
  -----------------------------------------------------------------------*/
static JSBool navigator_javaEnabled(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
*rval = JSVAL_FALSE;
return jsContinueScript();
}

static JSBool navigator_taintEnabled(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
*rval = JSVAL_FALSE;
return jsContinueScript();
}


/*----------------------------------------------------------------------
  Object Screen
  -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
  Object History
  -----------------------------------------------------------------------*/
static JSBool history_back(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
GotoPreviousHTML (jsdocument, jsview);
return JS_FALSE;
}

static JSBool history_forward(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
GotoNextHTML (jsdocument, jsview);
return JS_FALSE;
}

/*----------------------------------------------------------------------
  Object Location
  -----------------------------------------------------------------------*/
/* TODO: search what is the difference between assign and replace... */

static JSBool location_assign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
char *urlname = jsval_to_str(cx, *argv);
if(urlname)
  {
  GetAmayaDoc (urlname, NULL, jsdocument, jsdocument, CE_ABSOLUTE, TRUE, NULL, NULL);
  return JS_FALSE;
  }
return jsContinueScript();
}

static JSBool location_replace(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
char *urlname = jsval_to_str(cx, *argv);
if(urlname)
  {
  GetAmayaDoc (urlname, NULL, jsdocument, jsdocument, CE_ABSOLUTE, TRUE, NULL, NULL);
  return JS_FALSE;
  }
return jsContinueScript();
}

static JSBool location_reload(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
Reload(jsdocument, jsview);
return JS_FALSE;
}

/*----------------------------------------------------------------------
  Object DOMImplementation
  -----------------------------------------------------------------------*/
static JSBool DOMImplementation_hasFeature(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  char *feature	= NULL;
  char *version	= NULL;

  if(JSVAL_IS_VOID(*argv))
    {
    JS_ReportError(gcx, "Missing mandatory argument 1 of function hasFeature");
    return JS_FALSE;
    }
  else
    {
    feature = jsval_to_str(cx, argv[0]);
    version = jsval_to_str(cx, argv[1]);
    *rval = BOOLEAN_TO_JSVAL((/*!strcmp(feature, "HTML") ||*/ !strcmp(feature, "XML")) && (version == NULL || !strcmp(version, "1.0")) );
    return jsContinueScript();
    }
}

/*----------------------------------------------------------------------
  Object Node
  -----------------------------------------------------------------------*/
/*
static JSBool Node_createElement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Node_replaceChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Node_removeChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
static JSBool Node_appendChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);*/

static JSBool Node_hasChildNodes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  Element element;
  element = (Element)JS_GetPrivate(cx, obj);
  element = TtaGetFirstChild(element);
  *rval = BOOLEAN_TO_JSVAL(Element_to_jsval(element, +1) != JSVAL_NULL);
  return jsContinueScript();
}

//static JSBool Node_cloneNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);*/

/*----------------------------------------------------------------------
  Object NodeList
  -----------------------------------------------------------------------*/
static JSBool NodeList_item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
Element element;
int n;
/* TODO: deal with the case getElementsByTagName */

  n = JSVAL_TO_INT(argv[0]);
  element = (Element)JS_GetPrivate(cx, JS_GetParent(cx, obj));
  element = TtaGetFirstChild(element);
  for( ; n > 0 && element != NULL ; n--)TtaNextSibling(&element);
  *rval = Element_to_jsval(element, +1);
  return jsContinueScript();
}
