<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta name="GENERATOR" content="amaya 7.2, see http://www.w3.org/Amaya/" />
  <title>Controlar la impresión con CSS</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="unix_platforms.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../Printing.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Controlar la impresión con CSS</h1>

<p>Amaya no imprime los enlace subrayándolos. Para que se subrayen, añade la
línea siguiente a la hoja de estilo de usuario (amaya.css, ubicada en la
carpeta raíz de Amaya):</p>

<p><code>@media print {a:link {text-decoration: underline} ... }</code></p>

<p>Amaya soporta la propiedad de salto de página CSS <code>page-break-before:
always</code>. Cuando asignas esta propiedad a un elemento específico
mediante un atributo style, Amaya genera un salto de página antes de este
elemento. Cuando asignas esta propiedad a un tipo de elemento en el elemento
style del documento o en una hoja de estilo CSS externa, Amaya genera un
salto de página antes de cada aparición de dicho elemento.</p>
</div>
</body>
</html>
