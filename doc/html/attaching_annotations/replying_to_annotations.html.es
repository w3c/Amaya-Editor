<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <title>Problemas conocidos con las anotaciones y los documentos
  modificados</title>
  <meta name="GENERATOR" content="amaya 8.2, see http://www.w3.org/Amaya/" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="moving_annotations.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../Annotations.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a> <a href="configuring_icons.html.es"
        accesskey="n"><img alt="siguiente" src="../../images/right.gif"
        /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Contestar anotaciones / Hilos de discusión</h1>

<p>Una anotación puede considerarse como un comentario a una página. La
opción de menú <strong>Anotaciones &gt; Responder a anotación</strong> mejora
el espacio virtual de colaboración al permitir a los usuarioes contestar a
las anotaciones o a otras respuestas.</p>

<p>La opción de menú <strong>Anotaciones &gt; Responder a anotación</strong>
te permite crear una respuesta a una anotación existente o a una respuesta.
Puedes elegir este comando desde una anotación abierta o desde una ventana de
respuesta. Este comando abre una nueva ventana de respuesta. Puedes editar
los campos de una ventana de respuesta como en una ventana de anotación, tal
y como se explica en <a href="creating_an_annotation.html.es">Crear una
anotación</a>.</p>

<p>Cuando la respuesta está preparada, puedes enviarla a un servidor con la
opción de menú <strong>Anotaciones &gt; Enviar al servidor</strong> o
guardarla localmente con la opción de menú <strong>Archivo &gt;
Guardar</strong>. Para borrar una respuesta, puedes utilizar la opción de
menú <strong>Anotaciones &gt; Borrar</strong>.</p>

<p>Las respuestas también pueden anotarse, como cualquier otro documento,
como se explica en <a href="creating_an_annotation.html.es">Crear una
anotación</a>.</p>

<h2>El interface de usuario</h2>

<p><img alt="Una anotación con un hilo de discusión"
src="../../images/threads.png" /></p>

<p>En el interface de usuario actual, todas las respuestas relacionadas con
una anotación se muestran al pie de la anotación en una sección organizada en
hilos. Cada elemento en el hilo muestra la fecha de respuesta, el autor y el
título de la respuesta. El contenido de cualquiera de estas respuestas puede
consultarse haciendo doble clic en la respuesta. La respuesta seleccionada se
resaltará y presentará en una ventana de respuesta. Al seleccionar otra
respuesta, se muestra en la misma ventana.</p>

<h2>Problemas conocidos: hilos incompletos</h2>

<p>No existe por el momento ninguna manera de controlar qué respuestas deben
enviarse. En principio no debería ser posible guardar una respuesta a una
respuesta en servidores diferentes. De la misma manera, al borrar una
respuesta se deberían borrar todas las respuestas relacionadas. En caso
contrario, existirían fragmentos de hilos de discusión que no podrían
relacionarse correctamente. Por ejemplo, sea R1 una respuesta a la anotación
A1 y R2 una respuesta a R1. Si envías R1 y almacenas R2 localmente, al
consultar A1 desacargando únicamente sus anotaciones locales, sólo verás R2.
Como Amaya no sabe que R1 existe, supone que R2 ha perdido a su padre. Estos
hilos "huérfanos" se identifican en Amaya mediante un signo de interrogación
<strong>?</strong> delante de ellos.Si posteriormente Amaya encuentra nuevos
elementos del hilo, por ejemplo, si descargas R1, Amaya reordenará la vista
de los hilos, insertando los hilos como corresponda. En el ejemplo, R2 se
convertiría en hijo de R1, como sería de esperar.</p>
</div>
</body>
</html>
