<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta name="GENERATOR"
  content="amaya 8.0-pre, see http://www.w3.org/Amaya/" />
  <title>Buscar y reemplazar texto</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="../Searching.html.es" accesskey="t"><img
        alt="superior" src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Buscar y reemplazar texto</h1>

<p>Puedes buscar y reemplazar texto con la caja de diálogo
<strong>Buscar</strong> que se abre con el comando de menú <strong>Editar
&gt; Buscar</strong> o al hacer clic en el botón <strong>Buscar</strong> <img
src="../../images/find.gif" alt="Botón Buscar" />. También puedes utilizar el
atajo de teclado <code>Control-Shift-%</code> en Unix, or
<code>Control+F</code> en Windows.</p>

<p>Esta caja de diálogo te permite buscar o reemplazar texto utilizando
diferentes criterios:</p>
<ul type="disc">
  <li>Escribe la cadena de caracteres que quieres buscar en el campo
    <strong>Buscar</strong>.
    
  </li>
  <li>Si activas la casilla de verificación
    <strong>MAYÚSCULAS=minúsculas</strong>, en la búsqueda no se distinguirá
    entre mayúsculas y minúsculas; si no, se buscará la cadena tal y como
    aparece en el campo <strong>Buscar</strong>.
    
  </li>
  <li>Para sustituir cadenas, escribe el texto que reemplazará a la cadena
    buscada en el campo <strong>Reemplazar por</strong> y elige la forma de
    sustitución en el grupo de opciones <strong>Reemplazar</strong>. Las
    formas de sustitución son:
    <ul>
      <li><strong>No reemplazar</strong>: No se realiza ninguna sustitución
        aunque el campo <strong>Reemplazar por</strong> contenga una cadena
        de caracteres. Se realiza simplemente la búsqueda.</li>
      <li><strong>Reemplazar a solicitud</strong>: Cuando se encuentra la
        cadena, Amaya la selecciona. Puedes entonces hacer clic en uno de los
        siguientes botones:
        <ul>
          <li><strong>Confirmar</strong> para sustituir y repetir la
          búsqueda.</li>
          <li><strong>No reemplazar</strong> para realizar una nueva búsqueda
            sin sustituir la cadena encontrada.</li>
        </ul>
      </li>
      <li><strong>Reemplazar automáticamente</strong>: Se realizan todas las
        sustituciones automáticamente en la porción de documento definida por
        el grupo de opciones <strong>Dónde buscar</strong>
        
      </li>
    </ul>
  </li>
  <li>El grupo de opciones <strong>Dónde buscar</strong> te permite
    especificar la parte del documento en la que se realizará la búsqueda:
    <ul>
      <li><strong>Antes de la selección</strong>: La búsqueda comienza al
        principio de la parte seleccionada y busca hacia atrás hasta el
        principio del documento.</li>
      <li><strong>En la selección</strong>: La búsqueda se realiza únicamente
        en la parte seleccionada, desde el principio hasta el final.</li>
      <li><strong>Tras la selección</strong>: La búsqueda empieza al final de
        la parte seleccionada y busca hacia adelante hasta el final del
        documento.</li>
      <li><strong>En todo el documento</strong>: La búsqueda se realiza en
        todo el documento desde el principio hasta el final,
        independientemente de la parte seleccionada.</li>
    </ul>
  </li>
</ul>

<p>Para empezar la búsqueda, haz clic en el botón <strong>Confirmar</strong>.
Si Amaya encuentra la cadena buscada, se selecciona y el documento se
desplaza de manera que la cadena sea visible. Puedes buscar la siguiente
aparición de la cadena haciendo nuevamente clic en el botón
<strong>Confirmar</strong>. Si la cadena no se encuentra, Amaya muestra un
mensaje <strong>No encontrado</strong>.</p>

<p>Puedes dejar de buscar y sustituir en cualquier momento haciendo clic en
el botón <strong>Aceptar</strong>.</p>

<p>Puedes deshacer las sustituciones eligiendo el comando de menú
<strong>Editar &gt; Deshacer</strong>, o mediante el atajo de teclado
<code>Control-Z</code>.</p>
</div>
</body>
</html>
