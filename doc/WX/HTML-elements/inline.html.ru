<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <title>Встроенная маркировка</title>
  <meta name="GENERATOR" content="amaya 9.1, see http://www.w3.org/Amaya/" />
  <style type="text/css">
  </style>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<div>
<img border="0" alt="Amaya" src="../../../resources/icons/22x22/logo.png" />

<p class="nav"><a href="address.html.ru"><img alt="Предыдущий" border="0"
src="../../../resources/icons/misc/left.png" /></a> <a
href="../HTML.html.ru"><img alt="Вверх" border="0"
src="../../../resources/icons/misc/up.png" /></a> <a
href="40styling.html.ru"><img alt="Следующий" border="0"
src="../../../resources/icons/misc/right.png" /></a></p>
</div>

<div id="page_body">
<h1>Встроенная маркировка</h1>

<p>Текст в пределах параграфа, заголовка, или элемента блока может быть
промаркирован различными путями, например: его акцентированием или подсветкой
текста которая должна быть выполнена пользователем компьютера. Большинство
этих встроенных элементов описывается на этой странице, хотя ссылки являются
встроенными элементами, они будут описаны отдельно.</p>

<div>
<h2><a name="Inline21"></a><a name="Definition">Определения</a></h2>

<p>Элемент <a
href="http://www.w3.org/TR/REC-html40/struct/text.html#edef-DFN"><code>dfn</code></a>
используется для маркировки <dfn>встроенного описания</dfn> (слово, которое
даётся как пример определения)</p>

<p><a href="../StyleSheets.html.ru">Таблица стилей</a> может использоваться
для изменения представления встроенных описаний.</p>
</div>

<div>
<h2><a name="Inline1">Встроенное цитирование</a></h2>

<p>Встроенное цитирование может быть промаркировано, используя HTML элемент
<a
href="http://www.w3.org/TR/REC-html40/struct/text.html#edef-Q"><code>q</code></a>
"цитирование". Этот элемент может быть выбран в подменю
<strong>Информационный тип</strong> из меню <strong>XHTML</strong>, или
использовать комбинацию кнопок <var>Ctrl i Ctrl q</var>. Возможно указать URI
источника цитаты, используя атрибут <code>cite</code>.</p>

<p><a href="../StyleSheets.html.ru">Таблица стилей</a> может использоваться
для изменения представления inline quotations. Спецификация HTML
устанавливает, что встроенная цитата должна быть предоставлена с
соответствующими знаками препинания для представляемого языка.</p>
</div>

<div>
<h2><a name="Sample">Примеры входного, выходного потока и компьютерного
кода</a></h2>

<p>Элемент предоставляющий возможность маркировки компьютерного кода -
<code>statement:do-something</code>, маркировка <var>переменных</var>,
<kbd>user input</kbd> и <samp>program output</samp> - элементы <a
href="http://www.w3.org/TR/REC-html40/struct/text.html#edef-CODE"><code>code</code></a>,
<a
href="http://www.w3.org/TR/REC-html40/struct/text.html#edef-VAR"><code>var</code></a>,
<a
href="http://www.w3.org/TR/REC-html40/struct/text.html#edef-KBD"><code>kbd</code></a>
и <a
href="http://www.w3.org/TR/REC-html40/struct/text.html#edef-SAMP"><code>samp</code></a>.
Эти элементы могут быть найдены в подменю <strong>Информационный тип</strong>
из меню <strong>XHTML</strong>, или используя комбинации кнопок: Для всех
элементов - <kbd>Ctrl i</kbd> и далее: <kbd>Ctrl t</kbd> для кода, <kbd>Ctrl
v</kbd> для переменных, <kbd>Ctrl k</kbd> для пользовательского входного
потока, <kbd>Ctrl m</kbd> для примера выходного потока.</p>

<p><a href="../StyleSheets.html.ru">Таблица стилей</a> может использоваться
для изменения представления этих элементов.</p>
</div>

<div>
<h2><a name="Inline">Акронимы и абревиатура</a></h2>

<p>Абревиатура и акронимы должны маркироваться элементами <a
href="http://www.w3.org/TR/REC-html40/struct/text.html#edef-ABBR"><code>abbr</code></a>
и <a
href="http://www.w3.org/TR/REC-html40/struct/text.html#edef-ACRONYM"><code>acronym</code></a>
которые можно найти в подменю <strong>Информационный тип</strong> из меню
<strong>XHTML</strong>. Для акронимов можно использовать комбинацию кнопок
<kbd>Ctrl i Ctrl y</kbd> . Это позволяет выполнять распознавание для
различных типов систем. Атрибут <code>title</code> может использоваться для
определения расширения акронима. Выберите текст для маркировки, например
<acronym title="World Wide Web Consortium">W3C</acronym>, выберите меню
<strong>XHTML</strong>, в подменю тип информации выберите пункт акроним.
Затем выберите полный элемент (<kbd>F2 key</kbd>), откройте меню Атрибуты,
выберите заголовок, введите <kbd>World Wide Web Consortium</kbd>, и примените
его.</p>

<p><a href="../StyleSheets.html.ru">Таблица стилей</a> может использоваться
для изменения представления акронимов и абревиатур.</p>
</div>

<div>
<h2><a name="Inline2">Вставка и удаление</a></h2>

<p>Элементы <a
href="http://www.w3.org/TR/REC-html40/struct/text.html#edef-del"
name="DEL"><code>del</code></a> и <a
href="http://www.w3.org/TR/REC-html40/struct/text.html#edef-ins"><code>ins</code></a>
доступны в подменю <strong>Информационный тип</strong> из меню
<strong>XHTML</strong> и используются для маркировки текста <del>который
должен</del> должен быть удалён или который <ins>всегда</ins> должен
вставляться.</p>

<p><a href="../StyleSheets.html.ru">Таблица стилей</a> может использоваться
для изменения представления вставки и удаления.</p>
</div>

<div>
<h2><a name="Emphasis">Элементы акцентирования</a></h2>

<p>HTML имеет элементы для определения уровня акцентирования:</p>
<ol>
  <li><em>Обычное акцентирование</em>, используется элемент <a
    href="http://www.w3.org/TR/REC-html40/struct/text.html#edef-EM">em</a>
  и</li>
  <li><strong>Строгое акцентирование</strong>, используется элемент <code><a
    href="http://www.w3.org/TR/REC-html40/struct/text.html#edef-STRONG">strong</a></code>.</li>
</ol>

<p>Оба типа акцентирования могут быть применены из панели инструментов с
помощью кнопок <img src="../../../resources/icons/16x16/XHTML_I.png"
alt="Акцент" /> и <img src="../../../resources/icons/16x16/XHTML_B.png"
alt="Строгий" /> , или в подменю <strong>Информационный тип</strong> из меню
<strong>XHTML</strong>. Существуют стандартные комбинации кнопок - <kbd>Ctrl
i Ctrl e</kbd> для ввода обычного акцентирования (потому что комбинация
кнопок используемая по умолчанию часто выделяет текст) и <kbd>Ctrl i Ctrl
s</kbd> для строгого акцентирования (потому что комбинация кнопок
используемая по умолчанию выделяет как полужирный текст). Если что то
выбрано, то акцентирование применяется именно к нему, в противном случае,
соответствующий элемент создаётся как потомок текущего элемента.</p>

<p>В Amaya элементы акцентирования переключаемы - для удаления
акцентирования, выбирают элемент и повторно применяют к нему другое, нужное
акцентирование. Для примера, поместите курсор где-нибудь в элементе
<code>STRONG</code>, используйте кнопки <kbd>F2</kbd> (Windows) или
<kbd>Escape</kbd> (Unix) для выбора элемента, и затем используйте комбинации
кнопок, пункт меню, или кнопку <img
src="../../../resources/icons/16x16/XHTML_B.png" alt="Строгий" /> - для
удаления элемента <code>STRONG</code>.</p>

<p><a href="../StyleSheets.html.ru">Таблица стилей</a> может использоваться
для изменения представления обоих типов акцентирования.</p>

<h2 id="L260">Элемент SPAN</h2>

<p>Элемент <a
href="http://www.w3.org/TR/REC-html40/struct/global.html#edef-SPAN">span</a>
вместе с <a href="http://www.w3.org/TR/REC-html40/struct/global.html#adef-id"
name="id">id</a> атрибутами (<a href="../StyleSheets.html.fr#Creating"
name="style">style</a> или <a
href="http://www.w3.org/TR/REC-html40/struct/global.html#adef-class"
name="class">class</a>) предлагает общий механизм добавления структуры к
документу. Этот элемент генерируется Amaya когда пользователь применяет
атрибут к части текста. При удалении атрибута удаляется и текст.</p>
</div>
</div>
</body>
</html>
