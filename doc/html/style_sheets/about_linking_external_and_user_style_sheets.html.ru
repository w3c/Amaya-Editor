<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <meta name="GENERATOR" content="amaya 8.5, see http://www.w3.org/Amaya/" />
  <title>&#xce; &#xe2;&#xed;&#xe5;&#xf8;&#xed;&#xe8;&#xf5;
  &#xf1;&#xf1;&#xfb;&#xeb;&#xea;&#xe0;&#xf5; &#xe8;
  &#xef;&#xee;&#xeb;&#xfc;&#xe7;&#xee;&#xe2;&#xe0;&#xf2;&#xe5;&#xeb;&#xfc;&#xf1;&#xea;&#xe8;&#xf5;
  &#xf1;&#xf2;&#xe8;&#xeb;&#xff;&#xf5;</title>
  <style type="text/css">

 </style>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a
        href="about_applying_style_using_html_elements.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"
        /></a> <a href="../StyleSheets.html.ru" accesskey="t"><img
        alt="Вверх" src="../../images/up.gif" /></a> <a
        href="creating_and_updating_a_style_attribute.html.ru"
        accesskey="n"><img alt="Следующий" src="../../images/right.gif"
        /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>О внешних ссылках и пользовательских стилях</h1>

<p>Документ может ссылаться на внешние каскадные таблицы стилей, которые
загружаются вместе с документом и применяются к нему.</p>

<p>Внешняя таблица стилей может применяться ко множеству документов. Когда
некоторая таблица стилей применяется к нескольким документам, Amaya загружает
только один пример этой таблицы стилей.</p>

<p>Любой пользователь может задать определёную таблицу стилей которая будет
применяться ко всем документам, загружаемым с этой системы. Эта таблица будет
называтся <b>пользовательской</b> таблицей стиля. Она хранится в
пользовательской локальной переменной, в директории <span
class="Link0"><code>AmayaHome</code> как локальный файл</span>
<code>amaya.css</code>.</p>

<p>Например, если вы предпочитаете выводить на экран и распечатывать
документы шрифтом Helvetica малого размера, вы можете создать
пользовательскую таблицу стилей со следующими правилами CSS:</p>

<p><code>@media print {<br />
body {font-size: 10pt}<br />
}<br />
@media screen {<br />
body {font-family: Helvetica}<br />
}</code></p>
</div>
</body>
</html>
