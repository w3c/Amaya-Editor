<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta name="GENERATOR"
  content="amaya 8.0-pre, see http://www.w3.org/Amaya/" />
  <title>Enlazar hojas de estilo externas y de usuario</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a
        href="about_applying_style_using_html_elements.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../StyleSheets.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a> <a
        href="creating_and_updating_a_style_attribute.html.es"
        accesskey="n"><img alt="siguiente" src="../../images/right.gif"
        /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Enlazar hojas de estilo externas y de usuario</h1>

<p>Un documento pueda enlazar a varias hojas de estilo externas que se cargan
junto con el documento y se aplican a él.</p>

<p>Una hoja de estilo externa se puede aplicar a varios documentos. Cuando la
misma hoja de estilo se aplica a varios documentos, Amaya sólo carga la hoja
de estilo una vez.</p>

<p>El usuario puede definir una hoja de estilo específica que se aplicará a
todos los documentos abiertos por Amaya en su ordenador. Esta hoja de estilo
se llama hoja de estilo <b>de usuario</b>. Se guarda en el ordenador del
usuario, en la carpeta <span class="Link0"><code>AmayaHome</code></span>, en
el archivo <code>amaya.css</code>.</p>

<p>Por ejemplo, si prefieres mostrar los documentos en Helvética en la
pantalla e imprimirlos en un tipo de letra pequeño, puedes crear una hoja de
estilo de usuario con las siguientes rglas CSS:</p>

<p><code>@media print {<br />
body {font-size: 10pt}<br />
}<br />
@media screen {<br />
body {font-family: Helvetica}<br />
}</code></p>
</div>
</body>
</html>
