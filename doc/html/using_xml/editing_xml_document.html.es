<?xml version="1.0" encoding="iso-8859-1"?>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta name="GENERATOR" content="amaya 7.2, see http://www.w3.org/Amaya/" />
  <title>Editar documentos XML</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="xml_using_style.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../Xml.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Editar documentos XML</h1>

<p>El soporte de documentos XML genéricos en Amaya es todavía experimental.
En los tipos admitidos (XHTML, SVG, MathML), la edición sigue las mismas
reglas que en el correspondiente documento tipo. Para el resto de elementos,
en esta versión sólo está disponible el comando <kbd>Intro</kbd>:</p>
<ul>
  <li>Si la selección se encuentra en el interior de un elemento, Amaya
    divide el elemento seleccionado y crea un nuevo elemento del mismo tipo a
    continuación.</li>
  <li>Si se selecciona un elemento no vacío completo, Amaya crea un nuevo
    elemento del mismo tipo a continuación.</li>
  <li>Si se selecciona un elemento vacío completo, Amaya crea un elemento del
    mismo tipo del padre del elemento elegido.</li>
</ul>

<p>y no hay un interface de edición especial en esta versión. Por eso se
activa la marca de <strong>sólo-lectura</strong> en la <strong>vista
Formateada</strong> cuando se muestran esos documentos. Para modificar esos
documentos, debes editar directamente el código fuente del documento (</p>

<p>También puedes editar los documentos XML en la <strong>vista
Fuente</strong>, utilizando a continuación el comando
<strong>Sincronizar</strong>. La modificaciones se aplican entonces en la
<strong>vista Formateada</strong>.</p>
</div>
</body>
</html>
