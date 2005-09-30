<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <title>О ярлыках комбинаций кнопок
</title>
  <meta name="GENERATOR" content="amaya 8.5, see http://www.w3.org/Amaya/" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="../ShortCuts.html.ru" accesskey="t"><img
        alt="Вверх" src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>О ярлыках комбинаций кнопок</h1>

<p>Ярлыки используются для вызова команд с помощью клавиатуры. Набор
инструментов Thot позволяет вам использовать для этих целей прикладной файл с
описаниями этих комбинаций кнопок. Этот файл определяет список связей между
вызваемой командой и последовательностью нажатий кнопок клавиатуры.</p>

<p>Синтаксис описания связей таков:</p>

<p><code>Directive = KeySeq ':' IdentCommand ;<br />
&nbsp;KeysSeq = KeySet [ ',' KeySet ] ;<br />
&nbsp;KeySet = [ Modifier ] [ 'Shift' ] Key ;<br />
&nbsp;Key = '&lt;Key&gt;' KeyValue / '&lt;Key&gt;' SpecialKey ;<br />
&nbsp;Modifier = 'Ctrl' / 'Alt' / 'Meta' ;<br />
&nbsp;KeyValue = 'a' / 'b' / ... ;<br />
&nbsp;SpecialKey = 'Escape' / 'Delete' / 'Space' / 'BackSpace' /<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
'Enter' / 'Up' / 'Down' / 'Left' / 'Right' /<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
'Home' / 'End' / 'F1' / ... / 'L1' / ... / 'R1' / ... ;<br />
&nbsp;IdentCommand = 'TtcInsertChar(' Char ')' / NAME '()' ;<br />
&nbsp;Char = KeyValue / OctalValue / Entity ;</code><code><br />
OctalValue = '\' NUMBER ;<br />
Entity = DecEntity / HexaEntity ; <br />
DecEntity = '&amp;' '#' NUMBER ';' ; <br />
HexaEntity = '&amp;' '#' 'x' HEXADECIMAL ';' ;</code></p>

<p>Для ознакомления, смотрите стандартные файлы amaya.keyboard (для Unix
платформ) и amaya.kb (для Windows платформ), которые находятся в директории
<span class="Link0">Amaya/config</span>.</p>

<p>Список доступных команд таков:</p>
<ul>
  <li>Команды перемещения:
    <ul>
      <li>Посимвольное перемещение: <code>TtcNextChar</code>,
        <code>TtcPreviousChar</code></li>
      <li>Пословное перемещение: <code>TtcNextWord</code>,
        <code>TtcPreviousWord</code></li>
      <li>Перемещение в начало/конец текущей строки:
        <code>TtcStartOfLine</code>, <code>TtcEndOfLine</code></li>
      <li>Построчное
        перемещение:<code>TtcNextLine</code>,<code>TtcPreviousLine</code></li>
      <li>Поэлементное перемещение:
        <code>TtcNextElement</code>,<code>TtcPreviousElement</code>,
        <code>TtcChildElement</code>, <code>TtcParentElement</code></li>
      <li>Постраничная прокрутка: <code>TtcPageDown</code>,
        <code>TtcPageUp</code>,<code>TtcPageTop</code>,<code>TtcPageEnd</code></li>
      <li>Построчная прокрутка вверх/вниз: <code>TtcLineUp</code>,
        <code>TtcLineDown</code>
        <p></p>
      </li>
    </ul>
  </li>
  <li>Команды расширения выделения:
    <ul>
      <li>Посимвольный выбор: <code>TtcNextSelChar</code>,
        <code>TtcPreviousSelChar</code></li>
      <li>Пословный выбор: <code>TtcNextSelWord</code>,
        <code>TtcPreviousSelWord</code></li>
      <li>Выбор до начала/конца текущей строки:
        <code>cStartOfLine</code><code>TtcSelStartOfLine</code>,
        <code>TtcSelEndOfLine</code></li>
      <li>Построчный
        выбор:<code>TtcNextSelLine</code>,<code>TtcPreviousSelLine</code></li>
    </ul>
    <p></p>
  </li>
  <li>Перейти по ссылке или активировать элемент HTML-формы (меню, кнопка,
    прочее.): <code>DoAction</code>.
    <p></p>
  </li>
  <li>Пропустить и перейти к следующей (или первой, если имеется выделение)
    ссылке или элементу HTML-формы в документе:
    <code>NextLinkOrFormElement</code>.
    <p></p>
  </li>
  <li>Пропустить и перейти к предыдущей ссылке или элементу HTML-формы в
    документе: <code>PreviousLinkOrFormElement</code>.
    <p></p>
  </li>
  <li>В меню <strong>Файл</strong> <code>New</code>,
    <code>NewTemplate</code>, <code>NewCss</code>, <code>OpenDoc</code>,
    <code>OpenDocInNewWindow</code>, <code>Reload</code>,
    <code>GotoPreviousHTML</code>, <code>GotoNextHTML</code>,
    <code>GoToHome</code>, <code>SaveDocument</code>,
    <code>SaveDocumentAs</code>, <code>Synchronize</code>,
    <code>SetupAndPrint</code>, <code>PrintAs</code>,
    <code>CloseDocument</code>, <code>AmayaClose</code>
    <p></p>
  </li>
  <li>В меню <strong>Правка</strong>: <code>TtcUndo</code>,
    <code>TtcRedo</code>, <code>TtcCutSelection</code>,
    <code>TtcCopySelection</code>, <code>TtcPaste</code>,
    <code>TtcDeleteSelection</code>, <code>SetBrowserEditor</code>,
    <code>TtcSearchText</code>, <code>SpellCheck</code>,
    <code>TransformType</code>.
    <p></p>
  </li>
  <li>В меню <strong>XHTML</strong>:
    <ul>
      <li>Главное меню: <code>ChangeTitle</code>, <code>CreateAddress</code>
        <code>CreateParagraph</code>, <code>CreateImage</code>,
        <code>CreateHorizontalRule</code>, <code>CreateBlockQuote</code>,
        <code>CreatePreformatted</code>, <code>CreateAddress</code>,
        <code>CreateDivision</code>, <code>CreateNOSCRIPT</code>,
        <code>CreateObject</code>, <code>CreateParameter</code>,
        <code>CreateIFrame</code>, <code>CreateBreak("\212")</code></li>
      <li>В подменю <strong>Область (AREA)</strong>:
        <code>CreateAreaRect</code>, <code>CreateAreaCircle</code>,
        <code>CreateAreaPoly</code></li>
      <li>В подменю <strong>Структура</strong>: <code>CreateBase</code>,
        <code>CreateMeta</code>, <code>CreateLinkInHead</code>,
        <code>CreateScript</code>, <code>CreateStyle</code>,
        <code>CreateComment.</code></li>
      <li>В подменю <strong>Форма (Form)</strong>: <code>CreateForm</code>,
        <code>CreateFileInput</code>, <code>CreateImageInput</code>,
        <code>CreatePasswordInput</code>, <code>CreateHiddenInput</code>,
        <code>CreateLabel</code>, <code>CreateFieldset</code>,
        <code>CreateOptGroup</code>, <code>CreateToggle</code>,
        <code>CreateRadio</code>, <code>CreateOption</code>,
        <code>CreateTextInput</code>, <code>CreateTextArea</code>,
        <code>CreateSubmit</code>, <code>CreateReset</code></li>
      <li>Подменю <strong>Заголовок (Heading)</strong>:
        <code>CreateHeading1</code>, <code>CreateHeading2</code>,
        <code>CreateHeading3</code>, <code>CreateHeading4</code>,
        <code>CreateHeading5</code>, <code>CreateHeading6</code></li>
      <li>Подменю <strong>Список (List)</strong>: <code>CreateList</code>,
        <code>CreateNumberedList</code>, <code>ThotCreateMenu</code>,
        <code>ThotCreateDirectory</code>, <code>CreateDefinitionList</code>,
        <code>CreateDefinitionTerm</code>,
      <code>CreateDefinitionDef</code></li>
      <li>Подменю <strong>Объект (OBJECT)</strong>:
        <code>CreateObject</code>,<code>CreateParameter</code></li>
      <li>Подменю <strong>Таблица (Table)</strong>: <code>CreateTable</code>,
        <code>CreateCaption</code>, <code>CreateTHead</code>,
        <code>CreateTBody</code>, <code>CreateTFoot</code>,
        <code>CreateRow</code>, <code>CreateDataCell</code>,
        <code>CreateHeadingCell</code>, <code>DeleteColumn</code></li>
      <li>Подменю <strong>Информационный тип</strong>:
        <code>SetOnOffEmphasis</code>, <code>SetOnOffStrong</code>,
        <code>SetOnOffCite</code>, <code>SetOnOffDefinition</code>,
        <code>SetOnOffCode</code>, <code>SetOnOffVariable</code>,
        <code>SetOnOffSample</code>, <code>SetOnOffKeyboard</code>,
        <code>SetOnOffAbbr</code>, <code>SetOnOffAcronym</code>,
        <code>SetOnOffINS</code>, <code>SetOnOffDEL</code></li>
      <li>Подменю <strong>Символьный элемент</strong>:
        <code>SetOnOffItalic</code>, <code>SetOnOffBold</code>,
        <code>SetOnOffTeletype</code>, <code>SetOnOffBig</code>,
        <code>SetOnOffSmall</code>, <code>SetOnOffSub</code>,
        <code>SetOnOffSup</code>, <code>SetOnOffQuotation</code>,
        <code>SetOnOffBDO</code>.</li>
    </ul>
    <p></p>
  </li>
  <li>В меню <strong>XML</strong>:
    <ul>
      <li>Подменю <strong>Математика (Math)</strong>:
        <code>CreateInlineMath</code>, <code>CreateMath</code>,
        <code>CreateMROOT</code>, <code>CreateMSQRT</code>,
        <code>CreateMFRAC</code>, <code>CreateMSUBSUP</code>,
        <code>CreateMSUB</code>, <code>CreateMSUP</code>,
        <code>CreateMUNDEROVER</code>, <code>CreateMUNDER</code>,
        <code>CreateMOVER</code>, <code>CreateMROW</code>,
        <code>CreateMMULTISCRIPTS</code>, <code>CreateMTABLE</code></li>
    </ul>
    <p></p>
  </li>
  <li>В меню <strong>Ссылки</strong>: <code>LinkToPreviousTarget</code>,
    <code>CreateOrChangeLink</code>, <code>CreateTarget</code>,
    <code>DeleteAnchor</code>.
    <p></p>
  </li>
  <li>В меню <strong>Вид</strong>: <code>ShowButtons</code>,
    <code>ShowAddress</code>, <code>ShowMapAreas</code>,
    <code>ShowTargets</code>, <code>ZoomIn</code>, <code>ZoomOut</code>,
    <code>ShowStructure</code>, <code>ShowAlternate</code>,
    <code>ShowLinks</code>, <code>ShowToC</code>, <code>ShowSource</code>
    <p></p>
  </li>
  <li>В меню <strong>Стиль</strong>
    <ul>
      <li>Главное меню: <code>TtcChangeCharacters</code>,
        <code>TtcChangeColors</code>, <code>TtcChangeFormat</code>,
        <code>ChangeBackgroundImage</code>, <code>CreateClass</code>,
        <code>ApplyClass</code>.</li>
      <li>Подменю Таблица стилей: <code>LinkCSS</code>, <code>OpenCSS</code>,
        <code>DisableCSS</code>, <code>EnableCSS</code>,
        <code>RemoveCSS</code>.</li>
    </ul>
    <p></p>
  </li>
  <li>В меню <strong>Специальные</strong>: <code>MakeBook</code>,
    <code>MakeID</code>, <code>ShowLogFile</code>,
    <code>ConfigGeneral</code>, <code>ConfigPublish</code>,
    <code>ConfigCache</code>, <code>ConfigProxy</code>,
    <code>ConfigColor</code>, <code>ConfigGeometry</code>,
    <code>ConfigLanNeg</code>, <code>ConfigProfile</code>,
    <code>ConfigTemplates</code>.
    <p></p>
  </li>
  <li>В меню <strong>Аннотации</strong>: <code>AnnotateSelection</code>,
    <code>AnnotateDocument</code>, <code>LoadAnnotations</code>,
    <code>PostAnnotation</code>, <code>DeleteAnnotation</code>,
    <code>FilterAnnot</code>, <code>ConfigAnnot</code>.
    <p></p>
  </li>
  <li>В меню <strong>Справка</strong>: <code>HelpAmaya</code>,
    <code>HelpAtW3C</code>, <code>HelpBrowsing</code>,
    <code>HelpSelecting</code>, <code>HelpSearching</code>,
    <code>HelpViews</code>, <code>HelpCreating</code>,
    <code>HelpLinks</code>, <code>HelpChanging</code>,
    <code>HelpTables</code>, <code>HelpMath</code>, <code>HelpSVG</code>,
    <code>HelpImageMaps</code>, <code>HelpStyleSheets</code>,
    <code>HelpAttributes</code>, <code>HelpPublishing</code>,
    <code>HelpPrinting</code>, <code>HelpNumbering</code>,
    <code>HelpMakeBook</code>, <code>HelpAnnotation</code>,
    <code>HelpConfigure</code>, <code>HelpShortCuts</code>,
    <code>HelpAccess</code>.</li>
</ul>
</div>
</body>
</html>
