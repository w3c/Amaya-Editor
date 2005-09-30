<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <meta name="GENERATOR" content="amaya 8.5, see http://www.w3.org/Amaya/" />
  <title>Как изменить представление SVG библиотеки
</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /><img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="AddSVGLib.html.ru" accesskey="p"><img
        alt="Предыдущий" src="../../images/left.gif" /></a> <a
        href="SVGLibrary.html.ru" accesskey="t"><img alt="Вверх"
        src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Как изменить представление SVG библиотеки</h1>

<p>Вы можете использовать каскадные таблицы стилей CSS для изменения
представления SVG библиотеки. SVG библиотека является XHTML документом,
значит вы можете изменять правила представления всего что входит в этот
документ: таблицу, правила безопасности, размер изображения и прочее...
(смотрите спецификацию CSS).</p>

<p>Это пример библиотеки без применения стилей форматирования.</p>

<p><img alt="Библиотека без форматирования"
src="../../images/NoLibraryDocStyle.gif" /></p>

<p>А это пример библиотеки с применением стилей CSS.</p>

<p><img alt="Принятый по умолчанию стиль представления"
src="../../images/LibraryDocDefaultStyle.gif" /></p>

<p>Для изменения представления SVG библиотеки, вы должны просмотреть её
исходный код и изменить header файл:</p>

<p>.g_image реализует представление столбца изображения.</p>

<p>.g_title реализует представление столбца заголовка.</p>

<p>.g_comment реализует представление столбца комментария.</p>
</div>

<p></p>
</body>
</html>
