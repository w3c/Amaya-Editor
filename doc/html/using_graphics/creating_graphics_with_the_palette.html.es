<?xml version="1.0" encoding="iso-8859-1"?>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta name="GENERATOR"
  content="amaya 8.0-pre, see http://www.w3.org/Amaya/" />
  <title>Crear gráficos con la paleta</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="working_with_graphics.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../SVG.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a> <a href="moving_graphics.html.es"
        accesskey="n"><img alt="siguiente" src="../../images/right.gif"
        /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Crear gráficos con la paleta</h1>

<p>Para crear un nuevo elemento SVG en un documento, sitúa el punto de
inserción en el lugar en el que quieres insertarlo y haz clic en el botón
<strong>Gráficos</strong>. El botón <strong>Gráficos</strong> (<img
src="../../images/Graph.gif" alt="Botón Gráficos" />) muestra una paleta que
permanece en la pantalla hasta que haces clic en el botón
<strong>Aceptar</strong> de la paleta.</p>

<p>Los elementos gráficos disponibles son:</p>
<ul>
  <li>La línea, <code>line</code> en SVG, <svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.6cm">
      <line y1="3px" x1="0px" x2="17px" y2="20px" style="stroke: #B20000"
            stroke="1"/>
    </svg>
  </li>
  <li>El rectángulo, <code>rect</code> en SVG sin atributos <code>rx</code> o
    <code>ry</code>, <svg xmlns="http://www.w3.org/2000/svg" width="2cm"
         height="0.6cm">
      <rect x="6px" width="22px" height="18px"
            style="fill: #FFCB69; stroke: #0071FF" stroke="1"/>
    </svg>
  </li>
  <li>El rectángulo de esquinas redondeadas, <code>rect</code> en SVG, <svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.6cm">
      <rect rx="6px" stroke="1" width="23px" height="18px"
            style="fill: #E5E500; stroke: #4C00E5"/>
    </svg>
  </li>
  <li>El círculo, <code>circle</code> en SVG, <svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.6cm">
      <circle cy="10px" cx="12px" r="10px"
              style="fill: #C1FFE9; stroke: #FF0000" stroke="1"/>
    </svg>
  </li>
  <li>La elipse, <code>ellipse</code> en SVG, <svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.6cm">
      <ellipse cy="10px" cx="20px" rx="15px" ry="10px" style="fill: #B795FF"
            stroke="1"/>
    </svg>
  </li>
  <li>La polilínea, <code>polyline</code> en SVG,<svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.6cm">
      <polyline points="0,16 19,0 33,16 51,6" transform="translate(4,0)"
                stroke="1" style="stroke: #0000B2; fill: #FFA069"/>
    </svg>
  </li>
  <li>El polígono, <code>polygon</code> en SVG, <svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.7cm">
      <polygon points="0,16 13,0 26,12 43,4 44,16 15,16 12,11"
               transform="translate(2,0)" stroke="1"
               style="fill: #C8FF95; stroke: #000000"/>
    </svg>
  </li>
  <li>La curva abierta, <code>path</code> en SVG,<svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.6cm">
      <path stroke="blue" fill="none"
            d="M 0,16 C 0,15 4,6 9,7 C 13,7 12,16 16,16 C 20,15 24,1 22,0"
            style="stroke: #0071FF" stroke-width="2px"/>
    </svg>
  </li>
  <li>La curva cerrada, <code>path</code> en SVG, <svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.6cm">
      <path stroke="black" fill="none"
            d="M 14,4 C 11,4 8,0 6,1 C 3,1 1,4 1,7 C 0,9 2,12 5,14 C 7,15 13,14 16,14 C 17,13 21,12 23,11 C 24,9 29,7 29,4 C 28,1 24,0 22,0 C 19,0 16,3 14,4 Z"
            style="fill: #FFE9C1; stroke: #B20000"/>
    </svg>
  </li>
</ul>
<ul>
  <li>El fragmento HTML incrustado, <code>foreignObject</code> en SVG,<svg
         xmlns="http://www.w3.org/2000/svg" width="7cm" height="45px">
      <rect y="0" x="27px" width="200px" height="45px" style="fill: #C1FFFF"
            stroke="1"/>
      <foreignObject width="200px" y="0" x="20px">

        <div xmlns="http://www.w3.org/1999/xhtml">
        <ul>
          <li>Primer elemento</li>
          <li>Segundo elemento</li>
        </ul>
        </div>
      </foreignObject>
    </svg>
  </li>
  <li>El texto SVG, <code>text</code> en SVG<svg
         xmlns="http://www.w3.org/2000/svg" width="9.5cm" height="1.2cm">
      <text y="33px" x="13px"
            style="font-family: helvetica; font-size: 12pt; fill: #0000B2">Una
        cadena de caracteres en SVG
      </text>
    </svg>
  </li>
</ul>

<h2>Elegir una forma en la paleta</h2>

<p>Al seleccionar una forma en la paleta, si la selección está en la
estructura HTML del documento, Amaya crea un nuevo elemento SVG justo al
principio de la selección actual e inserta la forma elegida en este nuevo
SVG. Amaya genera los dibujos SVG como bloques, entre párrafos. Para mostrar
un dibujo en línea, debes crearlo entre párrafos y editar el código fuente
para desplazarlo.</p>

<p>Cuando el elemento seleccionado es un elemento SVG, la nueva forma se
añade al dibujo SVG actual. La nueva forma se añade después y puede solaparse
con el resto de formas.</p>

<p>La creación de una forma se hace directamente:</p>
<ul>
  <li>Para crear un rectángulo, elige primero la posición de la esquina
    izquierda apretando el botón izquierdo del ratón. manteniendo pulsado el
    botón, mueve el ratón a la posición de la esquina opuesta y suelta el
    botón. La forma ya está definida.
    
  </li>
  <li>Para crear un círculo o una elipse, selecciona primero la posición del
    centro pulsando el botón izquierdo del ratón. Manteniendo pulsado el
    botón, mueve el ratón hacia la derecha y/o hacia abajo y acontinuación,
    suelta el ratón. La forma ya está definida.
    
  </li>
  <li>Para crear una polilínea o un polígono, crea cada punto haciendo clic
    en la posición correspondiente con el botón izquierdo del ratón. Para
    indicar el último punto, haz clic con el botón central o derecho.</li>
</ul>

<h2>Insertar una imagen</h2>

<p>Puedes insertar una imagen en un dibujo SVG. Elige el comando de menú
<strong>XHTML &gt; Imagen</strong> o haz clic en el botón <strong>Insertar
imagen</strong> de la barra de botones y sigue los mismos pasos que en
HTML.</p>

<p>para insertar una expresión matemática en un dibujo SVG, haz clic en la
paleta <strong>Math</strong> cuando la selección está en un SVG. Amaya
generará una etiqueta <code>foreignObject</code> para incrustar las marcas
MathML.</p>

<h2>Ver también:</h2>
<ul>
  <li><a href="working_with_graphics.html.es">Trabajar con gráficos</a></li>
  <li><a href="../Math.html.es">Editar Matemáticas</a></li>
  <li><a
    href="../editing_mathematics/the_math_palette_and_the_types.html.es">La
    paleta MathML y el menú XML &gt;  MathML</a></li>
</ul>

<p>&nbsp;</p>
</div>
</body>
</html>
