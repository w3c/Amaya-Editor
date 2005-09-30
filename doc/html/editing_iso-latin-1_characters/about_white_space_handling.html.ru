<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <meta name="generator" content="amaya 8.5, see http://www.w3.org/Amaya/" />
  <title>Об обработке пробельных символов
</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="about_standard_multikey_support.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"
        /></a> <a href="../EditChar.html.ru" accesskey="t"><img alt="Вверх"
        src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Об обработке пробельных символов</h1>

<p>Amaya удаляет незначащие пробельные символы когда он загружает документ,
если они не должны быть явно сохранены. Это верно для XHTML, MathML, и SVG
документов, а также для MathML или SVG элементов, включённых в HTML
документ,</p>

<p>Нижеследующее считается незначащими пробельными символами:</p>
<ul>
  <li>Символы конца линии (<code>#xD</code> и <code>#xA</code>)</li>
  <li>Символ табуляции (<code>#X9</code>)</li>
  <li>Подача (leading) пространства элемента</li>
  <li>Прослеживание (trailing) пространства элемента</li>
  <li>Непрерывные (contiguous) пространства (одно пространство
    зарезервировано)</li>
</ul>

<p>Чтобы сохранить все символы для некоторых элементов, используйте атрибут
<code>xml:space</code> или элемент <code>PRE</code> (верно только для
правильных XHTML документов). Возможной величиной для атрибута
<code>xml:space</code> является <code>default</code> и
<code>preserve</code>.</p>
<ul>
  <li>Величина <code>default</code> означает, что для этих элементов доступна
    обработка пробельных символов. В Amaya, обработка по умолчанию, удаляет
    незначащие пробельные символы для всех поддерживаемых DTDs (смотрите
    ниже).
    <p></p>
  </li>
  <li>Величина <code>preserve</code> показывает что приложения сохраняют все
    символы, кроме первого, за которым сразу идёт конец тэга элемента в
    котором он определён, если этот элемент находится в конце линии.</li>
</ul>

<p>Вы применяете атрибут <code>xml:space</code> ко всем элементам в пределах
содержимого элемента где он определён, если это не аннулируется другим
примером атрибута <code>xml:space</code>.</p>

<p>В XHTML DTD, элемент <code>PRE</code> ведёт себя также, как и атрибут
<code>xml:space</code> с величиной <code>preserve</code>.</p>

<p>Amaya применяет обработку пробельных символов, когда загружает документ,
но не во время редактирования документа. Так что, если вы набираете
незначащие пробельные символы, они будут видимыми и будут сохраненны, но при
следующей перезагрузке документа будут удалены. Например, следующий исходный
код:</p>

<p><code>&lt;p&gt;Amaya удаляет &lt;strong&gt; незначащий пробельный символ
&lt;/strong&gt;при его загрузке ...&lt;/p&gt;</code></p>

<p>во времы редактирования выводится следующим образом:</p>

<p><code>Amaya удаляет <strong>незначащий пробельный символ</strong> при его
загрузке...</code></p>

<p>Сохраняем текст с таким синтаксисом. Но при следующей загрузке документа,
текст будет выведен следующим образом:</p>

<p><code>Amaya удаляет<strong>незначащий пробельный символ</strong>при его
загрузке...</code></p>

<p>потому что leading и trailing пространства элемента
<code>&lt;strong&gt;</code> считаются незначащими и удаляются.</p>

<p>Правильный исходный код должен быть таким:</p>

<p><code>&lt;p&gt;Amaya удаляет &lt;strong&gt;незначащий пробельный
символ&lt;/strong&gt; при его загрузке ...&lt;/p&gt;</code></p>
</div>
</body>
</html>
