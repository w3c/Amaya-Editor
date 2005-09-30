<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"

    "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
  <title>Известные проблемы с MathML
</title>
  <meta name="generator" content="amaya 8.5, see http://www.w3.org/Amaya/">
  <link href="../style.css" rel="stylesheet" type="text/css">
</head>

<body lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home"> <img alt="Amaya"
        src="../../images/amaya.gif"></td>
      <td><p align="right"><a href="about_linking_in_mathml.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"></a>
        <a href="../Math.html.ru" accesskey="t"><img alt="Вверх"
        src="../../images/up.gif"></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Известные проблемы с MathML</h1>

<p>Ниже, приведены известные проблемы MathML в Amaya:</p>
<ul>
  <li>Из MathML 2.0 Amaya реализует только тэги представления, но никак не
    маркировки содержания. Доступны все элементы представления и их атрибуты,
    но только следующие атрибуты выводятся на экране: 
    <p><code>display, alttext, mathvariant, mathsize, mathcolor,
    mathbackground, fontsize, fontweight, fontstyle, fontfamily, color,
    linethickness, numalign, denomalign, bevelled, notation, lquote, rquote,
    lspace, rspace, largeop, movablelimits, subscriptshift, superscriptshift,
    form, width </code>(mspace только), <code>height</code> (mspace только),
    <code>depth</code> (mspace только), <code>align, rowalign, columnalign,
    frame, framespacing, displaystyle, side, rowspan, columnspan, rowspacing,
    columnspacing, rowlines, columnlines</code>.</p>
  </li>
  <li><span><span>Доступны атрибуты <code>class</code>, <code>id</code>, и
    <code>style</code>, с той же семантикой как и в HTML: вы можете связывать
    каскадные таблицы стилей с элементами MathML (атрибуты <code>class</code>
    и <code>style</code>), MathML элемент может быть целью ссылки (атрибут
    <code>id</code>).</span></span></li>
  <li><span><span>В текущей версии, из-за недостатка математических шрифтов,
    только ограниченный набор объектов представляющих математические символы
    будет признаваться, главным образом они доступны в шрифте Symbol.
    </span></span>
    <p><span><span>Текущая версия поддерживает следующее:</span></span></p>
    <p><span><code>Therefore, SuchThat, DownTee, Or, And, Not, Exists,
    ForAll, Element, NotElement, NotSubset, Subset, SubsetEqual, Superset,
    SupersetEqual, DoubleLeftArrow, DoubleLeftRightArrow, DoubleRightArrow,
    LeftArrow, LeftRightArrow, RightArrow, Congruent, GreaterEqual, le,
    NotEqual, Proportional, VerticalBar, Union, Intersection, PlusMinus,
    CirclePlus, Sum, Integral, CircleTimes, Product, CenterDot, Diamond,
    PartialD, DoubleDownArrow, DoubleUpArrow, DownArrow, UpArrow,
    ApplyFunction, TripleDot, DifferentialD, ImaginaryI, ExponentialE,
    InvisibleComma, UnderBar, OverBar, ThinSpace, ThickSpace, Hat, OverBar,
    UnderBar, prime, Prime.</code></span></p>
  </li>
</ul>
</div>
</body>
</html>
