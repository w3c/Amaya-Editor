<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"

    "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
  <title>Enlazar en MathML</title>
  <meta name="generator" content="amaya 8.5, see http://www.w3.org/Amaya/">
  <link href="../style.css" rel="stylesheet" type="text/css">
</head>

<body lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home"> <img alt="Amaya"
        src="../../images/amaya.gif"></td>
      <td><p align="right"><a href="viewing_structure_in_mathml.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif"></a> <a
        href="../Math.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif"></a> <a href="math_issues.html.es"
        accesskey="n"><img alt="siguiente"
        src="../../images/right.gif"></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body" class="Section1">
<h1>Enlazar en MathML</h1>

<p>Puedes crear y utilizar enlaces en expresiones matemáticas. Estos enlaces
se basan en una versión preliminar del Lenguage de Enlaces XML (XLink).
ünicamente están disponibles hiperenlaces unidireccionales, pero puedes
asociar un enlace a cualquier parte de una fórmula. Por ejemplo, la fracción
de la fórmula siguiente es un enlace a la página de inicio del W3C:</p>

<p><math xmlns="http://www.w3.org/1998/Math/MathML">
  <mrow>
    <mi>y</mi>
    <mo>=</mo>
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

<p>Al hacer doble clic en cualquier carácter de la fracción se carga la
página de inicio del W3C.</p>

<p>Para crear un enlace así, selecciona una expresión matemática (en el caso
anterior, la fracción), haz clic en el <strong>Enlace</strong> (elige el menú
<strong>Enlaces &gt; Crear o cambiar enlace</strong>), y haz clic en el
destino deseado.</p>

<p>Si el destino es una expresión de una fórmula, primero debes convertir la
expresión en un destino. Selecciona la expresión y elige el comando
<strong>Enlaces &gt; Crear destino</strong> para crear un atributo
<code>id</code> para la expresión destino.</p>
</div>
</body>
</html>
