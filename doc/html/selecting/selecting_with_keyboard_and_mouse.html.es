<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta name="GENERATOR"
  content="amaya 8.0-pre, see http://www.w3.org/Amaya/" />
  <title>Seleccionar con el Teclado y el Ratón</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="../Selecting.html.es" accesskey="t"><img
        alt="superior" src="../../images/up.gif" /></a> <a
        href="selecting_images.html.es" accesskey="n"><img alt="siguiente"
        src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Seleccionar con el Teclado y el Ratón</h1>

<p>Muchos comandos en Amaya actúan sobre la selección actual. Puedes
seleccionar utilizando el teclado o el ratón, como se describe a
continuación:</p>

<p>Hagas como hagas la selección, Amaya muestra el tipo del primer elemento
elegido en la parte inferior de la ventana, seguido de los tipos de elementos
de nivel superior en la estructura. Esto puede ser útil cuando la estructura
del documento es compleja.</p>

<p>Por ejemplo, al seleccionar una palabra de esta frase, en la barra de
estado se mostrará:</p>

<p><samp>text \ p \ div \ body \ html \ Document</samp></p>

<p>lo que significa que lo has elegido es texto contenido en un elemento
<code>p</code> (que está contenido en un elemento div, que a su vez está
contenido en el elemento body etc.).</p>

<p>Al seleccionar <strong>este elemento de estructura</strong> (por ejemplo,
haciendo clic en la palabra estructura y a continuación pulsando
<kbd>Escape</kbd> o <kbd>F2</kbd>), en la barra de estado se mostrará</p>

<p><samp>strong \ p \ div \ body \ html \ Document</samp></p>

<h2>Seleccionar con el ratón</h2>
<ul>
  <li>Para situar el punto de inserción, haz clic con el botón izquierdo del
    ratón.</li>
  <li>Para seleccionar texto, arrastra el ratón por una región: mantén
    pulsado el botón izquierdo del ratón, mueve el ratón y suelta el botón
    cuando esté seleccionado el texto deseado.</li>
  <li>Para seleccionar una palabra completa, haz doble clic en cualquier
    carácter de la palabra.</li>
  <li>Para seleccionar un elemento de lista completo, haz clic en su viñeta o
    número.</li>
  <li>Para extender una selección, haz clic con el botón izquierdo del ratón
    mientras mantienes pulsada la tecla <code>Shift</code>.</li>
  <li>Para seleccionar un elemento de estructura, haz clic en el interior del
    elemento y a continuación el nivel de la estructura que quieras
    utilizando el teclado como se describe a acontinuación.</li>
</ul>

<h2>Seleccionar con el teclado</h2>
<ul>
  <li>Para seleccionar elementos de estructura utilizando el teclado
    <ul>
      <li>Pulsa la tecla <code>Escape</code> en Unix/Mac o la tecla
        <code>F2</code> en Windows para seleccionar el elemento padre en la
        estructura del documento. Pulsa la tecla varias veces para ir
        seleccionando los elementos de nivel superior (y todo su contenido)
        hasta llegar a la raíz.</li>
      <li>Pulsa <code>Control -</code> (tecla menos) para seleccionar un
        elemento hijo.</li>
      <li>Pulsa <code>Control k</code> para seleccionar el siguiente elemento
        del mismo nivel, o del nivel del elemento padre si no hay después o
        antes un elemento hijo del nivel actual.</li>
      <li>Pulsa <code>Control j</code> para seleccionar el elemento anterior
        del mismo nivel, o del nivel del elemento padre si no hay antes un
        elemento hijo del nivel actual.</li>
    </ul>
  </li>
  <li>Para mover el punto de inserción en las cuatro direcciones, utiliza las
    cuatro flechas de desplazamiento (<code>Izquierda</code>,
    <code>Derecha</code>, <code>Arriba</code> y <code>Abajo</code>). Si no
    hay ninguna selección visible en la ventana, Amaya crea una nueva
    selección en la esquina superior izquierda de la ventana.<br />
  </li>
  <li>Para extender la selección actual en una de las cuatro direcciones,
    mantén pulsada la tecla <code>Shift</code> mientras pulsas una de las
    flechas de desplazamiento (<code>Shift-Izquierda</code>,
    <code>Shift-Derecha</code>, <code>Shift-Arriba</code>, and
    <code>Shift-Abajo</code>).<br />
  </li>
  <li>Para mover el punto de inserción al principio de la línea, pulsa la
    tecla <code>Inicio</code>.<br />
  </li>
  <li>Para mover el punto de inserción al final de la línea, pulsa la tecla
    <code>Fin</code>.<br />
  </li>
  <li>Para extender la selección actual a la palabra en la que se encuentra
    el punto de inserción, pulsa <code>Alt-Intro</code>. Si la selección está
    dentro de un enlace, <code>Alt-Intro</code> activará el enlace en vez de
    seleccionar la palabra actual.</li>
  <li>Para mover el punto de inserción al principio de la palabra anterior,
    pulsa <code>Control-Izquierda</code>.<br />
  </li>
  <li>Para mover el punto de inserción al principio de la palabra siguiente,
    pulsa <code>Control-Derecha</code>.<br />
  </li>
  <li>Para extender la selección actual al principio de la palabra anterior,
    pulsa <code>Shift-Control-Izquierda</code>.<br />
  </li>
  <li>Para extender la selección actual al principio de la palabra siguiente,
    pulsa <code>Shift-Control-Derecha</code>.<br />
  </li>
  <li>Para mover el documento una línea hacia arriba, pulsa
    <code>Control-Arriba</code>.<br />
  </li>
  <li>Para mover el documento una línea hacia abajo, pulsa
    <code>Control-Abajo</code>.</li>
  <li>Para mover el punto de inserción al principio del documento, pulsa
    <code>Control-Inicio</code>.<br />
  </li>
  <li>Para mover el punto de inserción al final del documento, pulsa
    <code>Control-Fin</code>.<br />
  </li>
</ul>

<p>También puedes seleccionar elementos utilizando la vista Estructura o
seleccionar imágenes.</p>
</div>
</body>
</html>
