<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta name="GENERATOR" content="amaya 7.2, see http://www.w3.org/Amaya/" />
  <title>Crear un estilo genérico para un elemento o clase</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a
        href="creating_and_updating_a_style_attribute.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../StyleSheets.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a> <a
        href="creating_html_style_elements.html.es" accesskey="n"><img
        alt="siguiente" src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Crear un estilo genérico para un elemento o clase</h1>

<p>Cuando se asocia un atributo de estilo a un elemento, puedes utilizar ese
elemento para crear o modificar una regla de estilo, utilizando el comando
<strong>Crear regla</strong>.</p>

<h2>Utilizar el comando Crear regla</h2>

<p class="ProcedureCaption">Para crear un estilo genérico para un elemento o
clase:</p>
<ol>
  <li>Selecciona el elemento que quieres utilizar como plantilla. 
    <p class="StepNote"><strong>Nota:</strong> Debes elegir un único elemento
    y este elemento debe tener un atributo de estilo.</p>
  </li>
  <li>Elige el comando de menú <strong>Estilo &gt; Crear regla</strong>.
    Amaya abre una caja de diálogo con una lista de selectores CSS. La lista
    contiene todas las clases definidas en el documento actual (incluyendo
    las clases definidas en las hojas de estilo CSS enlazadas), así como el
    tipo de elemento al que se asocia el estilo 
    
  </li>
  <li>Elige un selector de esta lista o escribe un nuevo nombre de clase y
    haz clic en <strong>Confirmar</strong>. El estilo del elemento
    seleccionado se añde al selector elegido y el atributo de estilo se
    elimina del elemento elegido. Su contenido se traslada al elemento
    <code>style</code> del encabezado (<code>head</code>) del documento y
    todos los elementos que coincidan con el selector mostrarán el nuevo
    estilo.</li>
</ol>

<h2>Asociar clases con elemento</h2>

<p>Cuando creas nuevas clases con el el comando <strong>Crear regla</strong>,
puedes asociar las clases con algunos elementos del documento.</p>
<ol>
  <li>Elige el comando de menú <strong>Estilo &gt; Aplicar clase</strong> del
    menú <strong>Estilo</strong>. 
    
  </li>
  <li>Una caja de diálogo muestra todas las clases existentes. Elige una de
    estas clases. 
    <p>Amaya muestra los elementos seleccionados con el estilo asociado con
    la clase elegida.</p>
  </li>
</ol>

<p>Para asociar una clase a un elemento, también puedes utilizar el menú
<strong>Atributos</strong>. Este menú te permite también eliminar o modificar
el atributo clase de un elemento (el atributo clase se gestiona como
cualquier otro atributo).</p>
</div>
</body>
</html>
