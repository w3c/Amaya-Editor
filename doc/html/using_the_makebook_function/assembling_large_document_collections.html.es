<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type"
  content="application/xhtml+xml; charset=iso-8859-1" />
  <meta name="GENERATOR" content="amaya 8.6, see http://www.w3.org/Amaya/" />
  <title>Unir varios documentos</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="../MakeBook.html.es" accesskey="t"><img
        alt="superior" src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Unir varios documentos</h1>

<p>Amaya te permite gestionar colecciones de documentos. Una colección puede,
por ejemplo, ser una documentación técnica dividida en varias páginas web.
Una de las páginas web contiene el título de la documentación completa (un
elemento <code>&lt;h1&gt;</code>), una introducción (otros elementos) y una
lista <code>&lt;ol&gt;</code> o <code>&lt;ul&gt;</code> cuyos items
<code>&lt;li&gt;</code> contienen enlaces a cada capítulo. Los capítulos son
documentos separados que pueden tener una estructura similar.</p>

<p>Esta organización es adecuada para su navegación, pero resulta incómoda
cuando quieres imprimir toda la documentación. Amaya resuelve el problema con
el comando Hacer libro (<strong>XHTML&gt;Hacer libro</strong>).</p>

<p>Para enlazar capítulos, tipifica los enlaces asociándoles el atributo
<code>rel="chapter"</code> o <code>rel="subdocument"</code> al ancla que
apunta al capítulo (para ello, selecciona el elemento <code>a</code> y
utiliza el menú <strong>Atributos</strong>.</p>

<p>Cada capítulo o subdocumento enlazado puede ser:</p>
<ul>
  <li>un documento externo</li>
  <li>un subconjunto de un documento externo mayor</li>
</ul>

<p>Para hacer referencia a un subconjunto de un documento, normalmente debe
definir un elemento <code>div</code> para identificar la parte del documento
destino que quieres incluir y enlazar a este elemento.</p>

<p>Al utilizar el comando Hacer libro, los bloques (los elementos
<code>&lt;li&gt;</code> en el ejemplo anterior) que contienen un enlace
tipificado a un capítulo se sustituirán por las páginas web correspondientes
(o por los subconjuntos de páginas web) y Amaya mostrará un único documento
que contiene toda la colección</p>
<ul>
  <li>Si el enlace apunta a una página web, Amaya incluye todo el contenido
    de <code>body</code> del documento destino.</li>
  <li>Si el enlace apunta a un elemento destino, Amaya incluye el elemento y
    su contenido.</li>
  <li>Si el enlace apunta a un ancla destino, Amaya incluye el contenido del
    ancla, pero no la propia ancla.</li>
</ul>

<p>Antes de cada sustitución, el comando Hacer libro genera un nuevo elemento
<code>div</code> con su atributo <code>id</code> para separar claramente cada
porción añadida.</p>

<p>Las porciones añadidas en el documento generado pueden contener enlaces
normales, anclas destino y elementos destino. Mientras se hace el libro,
Amaya se asegura de que cada atributo <code>name</code> e <code>id</code>
tienen un valor único en el nuevo documento. Cuando es necesario, Amaya
cambia esos valores y actualiza los enlaces.</p>

<p>Al mismo tiempo, Amaya actualiza automáticamente los enlaces externos a
los documentos o subdocumentos incluidos, cambiándolos a enlaces internos al
texto incluido. Por ejemplo, si un enlace apuntaba a un documento externo, el
enlace apuntará ahora al elemento <code>div</code> creado con el comando
Hacer libro. Esto asegura que el documento que contiene toda la documentación
es coherente.</p>

<p>Este documento puede a continuación numerarse e imprimirse con su tabla de
contenido y la lista de todos los enlaces.</p>
</div>
</body>
</html>
