<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <title>Нумерация разделов
</title>
  <style type="text/css">
 </style>
  <link href="style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><p><img alt="W3C" src="../images/w3c_home" /> <img alt="Amaya"
        src="../images/amaya.gif" /></p>
      </td>
      <td><p align="right"><a href="Printing.html.ru" accesskey="p"><img
        alt="Предыдущий" src="../images/left.gif" /></a> <a
        href="Manual.html.ru" accesskey="t"><img alt="Вверх"
        src="../images/up.gif" /></a> <a href="MakeBook.html.ru"
        accesskey="n"><img alt="Следующий" src="../images/right.gif"
        /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Нумерация разделов</h1>

<p>Заголовки раздела, элементы от <code>H2</code>до <code>H6</code>, для
отображения структуры документа, могут быть автоматически пронумерованы.
Предполагается, что элемент <code>H1</code> является заголовков документа и
поэтому не нумеруется. Другие заголовки, получают свои номера вычисляемые в
соответствии с их типом и их позицией в структуре документа. Эти номера
генерируются непосредственно в документе, а ранее сгенерированные номера
удаляются, всякий раз, когда вызывается команда <strong>XHTML&gt;Нумеровать
разделы</strong>.</p>

<h1>Оглавление</h1>

<p>Команда <strong>XHTML&gt;Создать оглавление</strong> находит все элементы
документа, от <code>H2</code>до <code>H6</code>, для создания оглавления
текущей позиции.</p>

<p>Следующие команды добавляют:</p>
<ul>
  <li>Атрибут <code>id</code> в каждый элемент, от <code>H2</code> до
    <code>H6</code>, который не имеет этого атрибута.</li>
  <li>Раздел с величиной "toc" атрибута <code>class</code> и каскад
    (последовательность) из ненумерованных списков текущей позиции.</li>
  <li>Пункт списка для каждого элемента, от H2 до H6, включая якорь,
    указывающий на соответствующий элемент.
    <p>Якорь содержания элемента является текущей копией элемента
    <code>Hi</code>. Если элемент <code>Hi</code> пронумерован, его номер
    также копируется.</p>
  </li>
</ul>
</div>
</body>
</html>
