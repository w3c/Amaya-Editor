<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta content="text/html; charset=iso-8859-1" />
  <meta name="GENERATOR"
  content="amaya 8.0-pre, see http://www.w3.org/Amaya/" />
  <title>Crear estructuras anidadas</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="exiting_anchors.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../Creating.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a> <a
        href="changing_the_document_title.html.es" accesskey="n"><img
        alt="siguiente" src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Crear estructuras anidadas</h1>

<p>Cuando el punto de inserción está situado en un elemento vacío, al pulsar
<strong>Intro</strong> se sustituye ese elemento por otro elemento vacío del
nivel inmediatamente superior en la estructura del documento. Esta
característica te permite crear estructuras anidadas complejas muy
rápidamente.</p>

<p>Por ejemplo, considera la estructura siguiente:</p>
<ul>
  <li>El primer elemento de la lista
    <p>Un párrafo en el primer elemento</p>
  </li>
  <li>El segundo elemento de la lista
    <ul>
      <li>Primer elemento de una lista anidada</li>
      <li>Segundo elemento de la lista anidada</li>
    </ul>
  </li>
  <li>Último elemento de la lista</li>
</ul>

<p class="ProcedureCaption">Para crear la estructura anterior:</p>
<ol>
  <li>Haz clic en el botón <strong>Lista no numerada</strong> para crear el
    primer elemento de la lista.
    <p></p>
  </li>
  <li>Escribe el texto correspondiente y pulsa <strong>Intro</strong> una
    vez. Amaya crea un nuevo párrafo dentro del elemento.
    <p></p>
  </li>
  <li>Escribe el párrafo y pulsa <strong>Intro</strong> dos veces. El primer
    <strong>Intro</strong> crea un nuevo párrafo, pero el segundo elimina ese
    párrafo y crea un segundo elemento de la lista no numerada inicial.
    <p></p>
  </li>
  <li>Escribe el texto correpsondiente al segundo elemento de la lista
    <p></p>
  </li>
  <li>Haz clic en el botón <strong>Lista no numerada</strong> para crear una
    nueva lista. La nueva lista se anida automáticamente dentro de la primera
    lista.
    <p></p>
  </li>
  <li>Escribe el texto del primer elemento de la nueva lista y pulsa
    <strong>Intro</strong> dos veces para crear un nuevo elemento en la lista
    anidada.
    <p></p>
  </li>
  <li>Escribe el texto del segundo elemento y pulsa <strong>Intro</strong>
    cuatro veces para crear un nuevo elemento de la lista inicial.
    <p></p>
  </li>
  <li>Escribe el último elemento de la lista y pulsa <strong>Intro</strong>
    tres veces para cerrar la lista original y empezar un nuevo párrafo.
    <p></p>
  </li>
</ol>

<p>La tecla <strong>Intro</strong> funciona de la misma manera cuando el
punto de inserción está al principio de un elemento, pero crea el nuevo
elemento antes del elemento actual. Por ejemplo, para añadir un elemento de
lista entre los dos primeros elementos de la estructura anterior, sitúa el
cursor al comienzo del segundo elemento de la lista y pulsa
<strong>Intro</strong> dos veces.</p>

<p class="Note"><strong>Nota:</strong> El funcionamiento de la tecla
<strong>Intro</strong> no se limita a listas y párrafos, sino que se aplica a
todos los elementos. Por ejemplo, resulta muy útil para crear tablas.</p>
</div>
</body>
</html>
