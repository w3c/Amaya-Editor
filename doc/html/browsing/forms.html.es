<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>Navegar formularios</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="about_access_keys.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../Browsing.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Navegar formularios</h1>

<p>Como Amaya es al mismo tiempo navegador y editor, algunos de los controles
asociados con los elementos de formularios HTML son distintos de los que
suelen utilizarse en los navegadores. Esta sección comenta las diferencias
más notables.</p>

<h2>Selector de entrada de formulario, selección simple (sólo para GTK y
WIN32)</h2>

<p>Para activar el selector, el usuario tiene que hacer doble clic en la
entrada. Aparecerá entonces un control que mostrará todas las entradas. El
control permite que el usuario utilice tanto el ratón como el teclado.</p>

<p>Con el teclado, debes seleccionar primero una entrada utilizando las
flechas de desplazamiento. Al pulsar la tecla Intro, validarás la elección.
Las teclas de Avanzar página y Retroceder página permiten desplazarse
rápidamente por las entradas de la lista. En cualquier momento puedes abortar
la acción del control pulsando Escape.</p>

<p>Con el ratón, hay dos formas de utilización. Al hacer clic se selecciona
una entrada, mientras que el doble clic significa que el usuario quiere
activar la selección. Para abortar la acción del control, haz clic en
cualquier área fuera del control (N.B., esta opción todavía no funciona en
GTK). También puedes utilizar la barra de desplazamiento para navegar
rápidamente entre las entradas del control.</p>

<h2>Selector de entrada de formulario, selección múltiple</h2>

<p>El comportamiento de este control es similar al de la selección simple. Es
decir, el control admite únicamente un cambio en el estado de la selección
cada vez. Debes invocarlo tantas veces como cambios necesites. Cuando el
control está activado, mostrará el estado actual de la selección. En WIN32,
normalemente se añade el carácter '&gt;' a las entradas seleccionadas. En
GTK, se muestra una barra azul en las entradas seleccionadas. Como antes,
puedes abortar el control mediante la tecla Escape o haciendo clic en
cualquier otro sitio (sólo en WIN32).</p>
</div>
</body>
</html>
