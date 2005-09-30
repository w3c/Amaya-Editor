<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <meta name="GENERATOR" content="amaya 8.5, see http://www.w3.org/Amaya/" />
  <title>О контроле за печатью с помощью CSS
</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="unix_platforms.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"
        /></a> <a href="../Printing.html.ru" accesskey="t"><img alt="Вверх"
        src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>О контроле за печатью с помощью CSS</h1>

<p>Amaya не печатает ссылки подчёркнутыми. Чтобы сделать это возможным,
добавьте нижеследующую запись в пользовательскую таблицу стилей (amaya.css,
размещённую в домашней директории <span class="Link0">Amaya</span>):</p>

<p><code>@media print {a:link {text-decoration: underline} ... }</code></p>

<p>Amaya поддерживает свойство прерывания страницы <code>page-break-before:
</code>всегда. Если вы прикрепляете это свойство к определённому элементу,
используя атрибут стиля, Amaya генерирует прерывание страницы перед этим
элементом. Если вы прикрепляете это свойство к типу элемента в пределах
элемента стиля документа или в пределах внешней таблицы стиля, Amaya
генерирует прерывание страницы перед каждым примером этого элемента.</p>
</div>
</body>
</html>
