<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <title>Элементы стиля символов в HTML</title>
  <meta name="GENERATOR" content="amaya 9.1, see http://www.w3.org/Amaya/" />
  <style type="text/css">

  </style>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<div>
<img border="0" alt="Amaya" src="../../../resources/icons/22x22/logo.png" />

<p class="nav"><a href="inline.html.ru"><img alt="Предыдущий" border="0"
src="../../../resources/icons/misc/left.png" /></a> <a
href="../HTML.html.ru"><img alt="Вверх" border="0"
src="../../../resources/icons/misc/up.png" /></a></p>
</div>

<div id="page_body">
<h1>Элементы стиля символов в HTML</h1>

<h2>Что <strong>Не</strong> надо делать и как это должно быть ...</h2>

<p>HTML 4.0 (Strict) предлагает несколько элементов которые дают возможность
стилизации символов. Эти элементы должны быть заменены при помощи структурных
элементов или таблицы стиля везде, где возможно.</p>

<p>Все эти элементы, исключая <code>PRE</code>, можно найти в подменю
<strong>Стиль символа</strong> из меню <strong>XHTML</strong>:</p>
<dl>
  <dt><code>b</code> и <code>i</code></dt>
    <dd>Эти элементы должны быть замещены использованием <code></code><a
      href="inline.html.ru#Emphasis">элементjd акцентирования и строгого
      акцетирования</a> которые используюся для акцентирования слова или
      фразы. Существуют <a href="infoTypes.html.ru">специальные элементы</a>
      для обозначения заголовков, терминов, которые определены, и некоторых
      специальных: цитаты, часть компьютерного кода, и.т.д. Если элементы
      используются просто для обеспечения более тяжелого шрифта, в этом
      случае, необходимо использовать <a
      href="../StyleSheets.html.ru">таблицу стилей</a>.</dd>
  <dt><code>big</code> и <code>small</code></dt>
    <dd>Эти эффекты должны достигаться с помощью <a
      href="../StyleSheets.html.ru">таблицы стилей</a>. Там где они
      используются для обозначения определенных типов информации таких как:
      источник цитаты, пример выходного и входного потока компьютера и.т.д,
      должны использоваться <a href="inline.html.ru">соответствующие HTML
      элементы</a>.</dd>
  <dt><code>sub</code> и <code>sup</code></dt>
    <dd>Для достижения эффекта этих элементов необходимо использовать <a
      href="../StyleSheets.html.ru">таблицу стилей</a>. В случаях с
      математическими выражениями, маркировать, где только это возможно,
      соответствующим языком т.е. <a href="../Math.html.ru">MathML</a>.</dd>
  <dt><code>tt</code> и <code>pre</code></dt>
    <dd>Авторы должны использовать <a href="../StyleSheets.html.ru">таблицу
      стилей</a> для достижения эффекта этих элементов.</dd>
</dl>
</div>
</body>
</html>
