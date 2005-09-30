<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"

    "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
  <title>О ссылках в MathML
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
      <td><p align="right"><a href="viewing_structure_in_mathml.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"></a>
        <a href="../Math.html.ru" accesskey="t"><img alt="Вверх"
        src="../../images/up.gif"></a> <a href="math_issues.html.ru"
        accesskey="n"><img alt="Следующий"
        src="../../images/right.gif"></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body" class="Section1">
<h1>О ссылках в MathML</h1>

<p>В математических выражениях вы можете создавать и использовать ссылки. Эти
ссылки предоставляются на основе черновой версии языка XML Linking Language
(XLink). На данный момент, доступны только простые однонаправленные
гиперссылки, но вы можете связать ссылку с любой частью формулы. Например,
дробь в нижеприведённой ссылке является ссылкой на домашнюю страницу W3C:</p>

<p><math xmlns="http://www.w3.org/1998/Math/MathML">
  <mrow>
    <mi>y</mi>
    <mo>=</mo>
    <mi></mi>
    <mfrac xmlns:xlink="http://www.w3.org/1999/xlink"
    xlink:href="http://www.w3.org/">
      <mn>1</mn>
      <msqrt>
        <mrow>
          <msup>
            <mi>x</mi>
            <mn>2</mn>
          </msup>
          <mo>+</mo>
          <mn>1</mn>
        </mrow>
      </msqrt>
    </mfrac>
  </mrow>
</math></p>

<p>Двойной щелчок по любому символу в пределах дроби загрузит домашнюю
страницу W3C.</p>

<p>Для создания такой ссылки, выберите математическое выражение (в нашем
случае дробь), щёлкните по кнопке <strong>Ссылки</strong> (или выберите пункт
<strong>Создать или изменить ссылку</strong> из меню
<strong>Ссылки</strong>), и щёлкните по желаемой цели.</p>

<p>Если целью является выражение в формуле, вы должны сначала превратить
выражение в цель. Выберите выражение и выберите пункт <strong>Создать
цель</strong> из меню <strong>Ссылки</strong> для создания атрибута
<code>id</code>, чтобы превратить выражение в цель.</p>
</div>
</body>
</html>
