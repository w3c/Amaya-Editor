<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
  <title>Escribir caracteres matemáticos</title>
  <meta name="GENERATOR"
  content="amaya 8.0-pre, see http://www.w3.org/Amaya/">
  <link href="../style.css" rel="stylesheet" type="text/css">
</head>

<body lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home"> <img alt="Amaya"
        src="../../images/amaya.gif"></td>
      <td><p align="right"><a href="the_math_palette_and_the_types.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif"></a> <a
        href="../Math.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif"></a> <a
        href="editing_math_expressions.html.es" accesskey="n"><img
        alt="siguiente" src="../../images/right.gif"></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Escribir caracteres matemáticos</h1>

<p>Cuando escribes una cadena de caracteres en un elemento MathML, Amaya
analiza la cadena y automáticamente genera los elementos  <code>mo</code>
(operador), <code>mn</code> (número), <code>mi</code> (identificador), and
<code>mtext</code>.</p>

<p>Por ejemplo, para escribir la fórmula <code>x=2a+b</code>:</p>
<ol>
  <li>Si no estás en una expresión matemática elige <strong>XML &gt;  MathML &gt; Nueva fórmula (math)</strong>.
    
  </li>
  <li>Escribe la fórmula (6 caracteres): <code>x=2a+b</code></li>
</ol>

<p>Abre la vista Estructura para ver la estructura generada por Amaya:</p>

<p><code>&lt;mi&gt;x&lt;/mi&gt;&lt;mo&gt;=&lt;/mo&gt;&lt;mn&gt;2&lt;/mn&gt;&lt;mi&gt;a&lt;/mi&gt;&lt;mo&gt;+&lt;/mo&gt;&lt;mi&gt;b&lt;/mi&gt;</code></p>

<p>Si el resultado no es exactamente los que quieres, selecciona los
lacaracteres mal interpretados y cambia su tipo con los comandos Texto simple
(<code>mtext</code>), Identificador (<code>mi</code>), o Número del menú
<strong>XML &gt;  MathML</strong>.</p>

<h2>Espaciado</h2>

<p>Amaya tiene en cuenta el espaciado en las expresiones matemáticas, pero en
algunos casos puedes necesitar espacios adicionales. Sitúa el cursor en la
posición en la que quieres insertar espacios y elige <strong>XML &gt;  MathML &gt; Espacio (mspace)</strong>. De esta manera se crea un espacio
horizontal por omisión. Puedes cambiar su anchura mediante su atributo
<strong>width</strong> o puedes añadir otros atributos (height, depth,
linebreak) para cambiar el espaciado vertical. Consulta la recomendación <a
href="http://www.w3.org/TR/MathML2/chapter3.html#N9197">MathML
specification</a> para más detalles.</p>

<h2>Tamaño de paréntesis</h2>

<p>Al escribir paréntesis como los de la expresión siguiente:</p>
<p<When entering brackets as in the following expression:</p>
<p><math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>f</mi>
  <mrow>
    <mo>(</mo>
    <mi>x</mi>
    <mo>)</mo>
  </mrow>
  <mo>=</mo>
  <mfrac>
    <mn>1</mn>
    <mi>x</mi>
  </mfrac>
</math>
</p>

<p>si escribe <code>f(x)=</code> se convertirá en:</p>

<p><math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>f</mi>
  <mo>(</mo>
  <mi>x</mi>
  <mo>)</mo>
  <mo>=</mo>
  <mfrac>
    <mn>1</mn>
    <mi>x</mi>
  </mfrac>
</math>
</p>

<p>Para evitarlo, elige <strong>XML &gt;  MathML &gt; Paréntesis</strong>
o elige la opción Paréntesis de la paleta MathML y se escribirán los
paréntesis adecuados.</p>
</div>
</body>
</html>
