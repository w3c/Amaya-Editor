<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type"
  content="application/xhtml+xml; charset=iso-8859-1" />
  <title>Atajos de teclado</title>
  <meta name="GENERATOR"
  content="amaya 9.0-pre, see http://www.w3.org/Amaya/" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="../ShortCuts.html.es" accesskey="t"><img
        alt="superior" src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Atajos de teclado</h1>

<p>Resulta útil poder acceder a los comandos de menú directamente desde el
teclado. El kit de herramientas Thot te permite hacerlo mediante atajos de
teclado definidos en un archivo. Este archivo define la lista de asociaciones
entre el comando y sus teclas asociadas.</p>

<p>La sintaxis utilizada para definir una asociación es:</p>

<p><code>Directive = KeySeq ':' IdentCommand ;<br />
 KeysSeq = KeySet [ ',' KeySet ] ;<br />
 KeySet = [ Modifier ] [ 'Shift' ] Key ;<br />
 Key = '&lt;Key&gt;' KeyValue / '&lt;Key&gt;' SpecialKey ;<br />
 Modifier = 'Ctrl' / 'Alt' / 'Meta' ;<br />
 KeyValue = 'a' / 'b' / ... ;<br />
 SpecialKey = 'Escape' / 'Delete' / 'Space' / 'BackSpace' /<br />
              'Intro' / 'Up' / 'Down' / 'Left' / 'Right' /<br />
              'Home' / 'End' / 'F1' / ... / 'L1' / ... / 'R1' / ... ;<br />
 IdentCommand = 'TtcInsertChar(' Char ')' / NAME '()' ;<br />
 Char = KeyValue / OctalValue / Entity ;</code><code><br />
OctalValue = '\' NUMBER ;<br />
Entity = DecEntity / HexaEntity ; <br />
DecEntity = '&amp;' '#' NUMBER ';' ; <br />
HexaEntity = '&amp;' '#' 'x' HEXADECIMAL ';' ;</code></p>

<p>Para ver ejemplos, consulta los archivos estándar amaya.keyboard (en Unix)
y amaya.kb (en Windows), que se encuentran en el directorio <span
class="Link0">Amaya/config</span>.</p>

<p>La lista de comandos disponibles es:</p>
<ul>
  <li>Comandos de desplazamiento:
    <ul>
      <li>Moverse carácter a carácter: <code>TtcNextChar</code>,
        <code>TtcPreviousChar</code></li>
      <li>Moverse palabra a palabra: <code>TtcNextWord</code>,
        <code>TtcPreviousWord</code></li>
      <li>Moverse al principio/final de la línea actual:
        <code>TtcStartOfLine</code>, <code>TtcEndOfLine</code></li>
      <li>Moverse línea a
        línea:<code>TtcNextLine</code>,<code>TtcPreviousLine</code></li>
      <li>Moverse elemento a elemento:
        <code>TtcNextElement</code>,<code>TtcPreviousElement</code>,
        <code>TtcChildElement</code>, <code>TtcParentElement</code></li>
      <li>Desplazar un página: <code>TtcPageDown</code>,
        <code>TtcPageUp</code>,<code>TtcPageTop</code>,<code>TtcPageEnd</code></li>
      <li>Desplazar una línea: <code>TtcLineUp</code>,
        <code>TtcLineDown</code></li>
    </ul>
  </li>
  <li>Comandos de selección:
    <ul>
      <li>Selección carácter a carácter: <code>TtcNextSelChar</code>,
        <code>TtcPreviousSelChar</code></li>
      <li>Selección palabra a palabra: <code>TtcNextSelWord</code>,
        <code>TtcPreviousSelWord</code></li>
      <li>Seleccionar hasta el principio/final de la línea actual:
        cStartOfLine<code>TtcSelStartOfLine</code>,
        <code>TtcSelEndOfLine</code></li>
      <li>Seleccionar línea a
        línea:<code>TtcNextSelLine</code>,<code>TtcPreviousSelLine</code></li>
    </ul>
  </li>
  <li>Abrir un enlace a activar un elemento de formulario HTML (menú, botón,
    etc.): <code>DoAction</code>.</li>
  <li>Moverse al siguiente (o al primero si hay algo seleccionado) enlace o
    elemento de formulario HTML del documento:
    <code>NextLinkOrFormElement</code>.</li>
  <li>Moverse al anterior enlace o elemento de formulario HTML del documento:
    <code>PreviousLinkOrFormElement</code>.</li>
  <li>Menú Archivo: <code>New</code>, <code>NewTemplate</code>,
    <code>NewCss</code>, <code>OpenDoc</code>,
    <code>OpenDocInNewWindow</code>, <code>Reload</code>,
    <code>GotoPreviousHTML</code>, <code>GotoNextHTML</code>,
    <code>GoToHome</code>, <code>SaveDocument</code>,
    <code>SaveDocumentAs</code>, <code>Synchronize</code>,
    <code>SetupAndPrint</code>, <code>PrintAs</code>,
    <code>CloseDocument</code>, <code>AmayaClose</code></li>
  <li>Menú Editar: <code>TtcUndo</code>, <code>TtcRedo</code>,
    <code>TtcCutSelection</code>, <code>TtcCopySelection</code>,
    <code>TtcPaste</code>, <code>TtcDeleteSelection</code>,
    <code>SetBrowserEditor</code>, <code>TtcSearchText</code>,
    <code>SpellCheck</code>, <code>TransformType</code>,
    <code>ConfigGeneral</code>, <code>ConfigPublish</code>,
    <code>ConfigCache</code>, <code>ConfigProxy</code>,
    <code>ConfigColor</code>, <code>ConfigGeometry</code>,
    <code>ConfigLanNeg</code>, <code>ConfigProfile</code>,
    <code>ConfigTemplates</code>.</li>
  <li>Menú XHTML:
    <ul>
      <li>Menú XHTML: <code>ChangeTitle</code>, <code>CreateAddress</code>
        <code>CreateParagraph</code>, <code>CreateImage</code>,
        <code>CreateHorizontalRule</code>, <code>CreateBlockQuote</code>,
        <code>CreatePreformatted</code>, <code>CreateAddress</code>,
        <code>CreateDivision</code>, <code>CreateNOSCRIPT</code>,
        <code>CreateObject</code>, <code>CreateParameter</code>,
        <code>CreateIFrame</code>, <code>CreateBreak("\212")</code>,
        <code>MakeBook</code>, <code>CreateAreaRect</code>,
        <code>CreateAreaCircle</code>,
        <code>CreateAreaPoly,</code><code>CreateForm</code>,
        <code>CreateFileInput</code>, <code>CreateImageInput</code>,
        <code>CreatePasswordInput</code>, <code>CreateHiddenInput</code>,
        <code>CreateLabel</code>, <code>CreateFieldset</code>,
        <code>CreateOptGroup</code>, <code>CreateToggle</code>,
        <code>CreateRadio</code>, <code>CreateOption</code>,
        <code>CreateTextInput</code>, <code>CreateTextArea</code>,
        <code>CreateSubmit</code>, <code>CreateReset</code>,
        <code>CreateHeading1</code>, <code>CreateHeading2</code>,
        <code>CreateHeading3</code>, <code>CreateHeading4</code>,
        <code>CreateHeading5</code>, <code>CreateHeading6</code>,
        <code>CreateList</code>, <code>CreateNumberedList</code>,
        <code>CreateDefinitionList</code>, <code>CreateDefinitionTerm</code>,
        <code>CreateDefinitionDef</code>, <code>CreateObject</code>,
        <code>CreateParameter</code>,</li>
      <li>Submenú Tabla: <code>CreateTable</code>,
        <code>CreateCaption</code>, <code>CreateTHead</code>,
        <code>CreateTBody</code>, <code>CreateTFoot</code>,
        <code>CreateRow</code>, <code>CreateDataCell</code>,
        <code>CreateHeadingCell</code>, <code>DeleteColumn</code></li>
      <li>Submenú Estructura: <code>CreateBase</code>,
        <code>CreateMeta</code>, <code>CreateLinkInHead</code>,
        <code>CreateScript</code>, <code>CreateStyle</code>,
        <code>CreateComment.</code></li>
      <li>Submenú Tipo de información: <code>SetOnOffEmphasis</code>,
        <code>SetOnOffStrong</code>, <code>SetOnOffCite</code>,
        <code>SetOnOffDefinition</code>, <code>SetOnOffCode</code>,
        <code>SetOnOffVariable</code>, <code>SetOnOffSample</code>,
        <code>SetOnOffKeyboard</code>, <code>SetOnOffAbbr</code>,
        <code>SetOnOffAcronym</code>, <code>SetOnOffINS</code>,
        <code>SetOnOffDEL</code>.</li>
      <li>Submenú Elemento carácter: <code>SetOnOffItalic</code>,
        <code>SetOnOffBold</code>, <code>SetOnOffTeletype</code>,
        <code>SetOnOffBig</code>, <code>SetOnOffSmall</code>,
        <code>SetOnOffSub</code>, <code>SetOnOffSup</code>,
        <code>SetOnOffQuotation</code>, <code>SetOnOffBDO</code>.</li>
      <li>Submenú Math: <code>CreateInlineMath</code>,
        <code>CreateMath</code>, <code>CreateMROOT</code>,
        <code>CreateMSQRT</code>, <code>CreateMFRAC</code>,
        <code>CreateMSUBSUP</code>, <code>CreateMSUB</code>,
        <code>CreateMSUP</code>, <code>CreateMUNDEROVER</code>,
        <code>CreateMUNDER</code>, <code>CreateMOVER</code>,
        <code>CreateMROW</code>, <code>CreateMMULTISCRIPTS</code>,
        <code>CreateMTABLE</code></li>
    </ul>
  </li>
  <li>Menú Enlaces: <code>LinkToPreviousTarget</code>,
    <code>CreateOrChangeLink</code>, <code>CreateTarget</code>,
    <code>DeleteAnchor</code>, <code>MakeID</code>.</li>
  <li>Menú Ver: <code>ShowButtons</code>, <code>ShowAddress</code>,
    <code>ShowMapAreas</code>, <code>ShowTargets</code>, <code>ZoomIn</code>,
    <code>ZoomOut</code>, <code>ShowStructure</code>,
    <code>ShowAlternate</code>, <code>ShowLinks</code>, <code>ShowToC</code>,
    <code>ShowSource</code>, <code>ShowLogFile</code>.</li>
  <li>Menú Estilo: <code>TtcChangeCharacters</code>,
    <code>TtcChangeColors</code>, <code>TtcChangeFormat</code>,
    <code>ChangeBackgroundImage</code>, <code>CreateClass</code>,
    <code>ApplyClass</code>, <code>LinkCSS</code>, <code>OpenCSS</code>,
    <code>DisableCSS</code>, <code>EnableCSS</code>, <code>RemoveCSS</code>,
    <code>DoLeftAlign</code>, <code>DoRightAlign</code>,
    <code>DoCenter</code>, <code>DoJustify</code>.</li>
  <li>Menú Anotaciones: <code>AnnotateSelection</code>,
    <code>AnnotateDocument</code>, <code>LoadAnnotations</code>,
    <code>PostAnnotation</code>, <code>DeleteAnnotation</code>,
    <code>FilterAnnot</code>, <code>ConfigAnnot</code>.</li>
  <li>Menú Ayuda: <code>HelpAmaya</code>, <code>HelpAtW3C</code>,
    <code>HelpBrowsing</code>, <code>HelpSelecting</code>,
    <code>HelpSearching</code>, <code>HelpViews</code>,
    <code>HelpCreating</code>, <code>HelpLinks</code>,
    <code>HelpChanging</code>, <code>HelpTables</code>,
    <code>HelpMath</code>, <code>HelpSVG</code>, <code>HelpImageMaps</code>,
    <code>HelpStyleSheets</code>, <code>HelpAttributes</code>,
    <code>HelpPublishing</code>, <code>HelpPrinting</code>,
    <code>HelpNumbering</code>, <code>HelpMakeBook</code>,
    <code>HelpAnnotation</code>, <code>HelpConfigure</code>,
    <code>HelpShortCuts</code>, <code>HelpAccess</code>.</li>
</ul>
</div>
</body>
</html>
