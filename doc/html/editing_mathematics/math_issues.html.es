<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
    "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
  <title>Problemas conocidos en relación con MathML</title>
  
  <meta content="amaya 6.4+, see http://www.w3.org/Amaya/">
  <link href="../style.css" rel="stylesheet" type="text/css">
</head>

<body lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home"> <img alt="Amaya"
        src="../../images/amaya.gif"></td>
      <td><p align="right"><a href="about_linking_in_mathml.html.es" accesskey="p"><img
        alt="anterior" src="../../images/left.gif"></a> <a
        href="../Math.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif"></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Problemas conocidos en relación con MathML</h1>

<p>Amaya presenta problemas en relación con MathML:</p>
<ul>
  <li>Amaya implementa únicamente las etiquetas de presentación de MathML
    2.0, no el marcado de contenido. Todos los elementos y atributos de
    presentación están disponibles, pero únicamente se muestran en la
    pantalla los siguientes atributos: 
    <p><code>display, alttext, mathvariant, mathsize, mathcolor,
    mathbackground, fontsize, fontweight, fontstyle, fontfamily, color,
    linethickness, numalign, denomalign, bevelled, notation, lquote, rquote,
    lspace, rspace, largeop, movablelimits, subscriptshift, superscriptshift,
    form, width </code>(mspace únicamente), <code>height</code> (mspace
    únicamente), <code>depth</code> (mspace únicamente), <code>align,
    rowalign, columnalign, frame, framespacing, displaystyle, side, rowspan,
    columnspan, rowspacing, columnspacing, rowlines, columnlines</code>.</p>
  </li>
  <li>Los atributos <code>class</code>, <code>id</code> y <code>style</code>
    están disponibles, con la misma semántica que en HTML: puedes asociar
    estilos CSS a elementos MathML (atributos <code>class</code> y
    <code>style</code>) y un elemento MathML puede ser el destino de un
    enlace (atributo <code>id</code>).</li>
  <li>Dada la falta de fuentes matemáticas en la versión actual, tan sólo se
    reconocen un subconjunto limitado de entidades que representan símbolos
    matemáticos, básicamente los disponibles en la fuente Symbol. La versión
    actual soporta las siguientes entidades:
    <p><code>Therefore, SuchThat, DownTee, Or, And, Not, Exists, ForAll,
    Element, NotElement, NotSubset, Subset, SubsetEqual, Superset,
    SupersetEqual, DoubleLeftArrow, DoubleLeftRightArrow, DoubleRightArrow,
    LeftArrow, LeftRightArrow, RightArrow, Congruent, GreaterEqual, le,
    NotEqual, Proportional, VerticalBar, Union, Intersection, PlusMinus,
    CirclePlus, Sum, Integral, CircleTimes, Product, CenterDot, Diamond,
    PartialD, DoubleDownArrow, DoubleUpArrow, DownArrow, UpArrow,
    ApplyFunction, TripleDot, DifferentialD, ImaginaryI, ExponentialE,
    InvisibleComma, UnderBar, OverBar, ThinSpace, ThickSpace, Hat, OverBar,
    UnderBar, prime, Prime.</code></p>
  </li>
</ul>
</div>
</body>
</html>
