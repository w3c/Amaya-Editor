<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <meta name="GENERATOR" content="amaya 8.2, see http://www.w3.org/Amaya/" />
  <title>Crear una anotación</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="configuring_annotation_settings.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../Annotations.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a> <a
        href="deleting_an_annotation.html.es" accesskey="n"><img
        alt="siguiente" src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div class="Section1" id="page_body">
<h1>Crear una anotación</h1>

<p>Esta versión de Amaya soporta dos tipos de anotaciones: anotaciones que se
aplican a todo el documento y anotaciones que se aplican a un punto o
selección específicos de un documento.</p>
<ul>
  <li>Para anotar un documento completo, elige el comando de menú
    <strong>Anotaciones &gt; Anotar documento</strong>.
    
  </li>
  <li>Para anotar un punto, sitúa el punto de inserción en cualquier lugar
    del documento y elige el comando de menú <strong>Anotaciones &gt; Anotar
    selección</strong>.
    
  </li>
  <li>Para anotar una selección, selecciona una porción del documento y elige
    el comando de menú <strong>Anotaciones &gt; Anotar
  selección</strong>.</li>
</ul>

<p>Tras realizar cualquiera de estas acciones, una caja de diálogo muestra
los metadatos de la anotación y el cuerpo de la anotación.</p>

<p><img src="../../images/annotationwindow-es.png"
alt="Ventana de anotaciones" /></p>

<h1>Metadatos de una anotación</h1>

<p>Actualmente, los metadatos consisten en el nombre del autor, el título del
documento anotado, el tipo de la anotación, la fecha de creación y la fecha
de la última anotación. Algunos de los campos de metadatos tienen propiedades
especiales:</p>
<ul>
  <li>El campo <strong>Source document (Documento origen)</strong> es también
    un enlace que apunta al texto anotado. Si el usuario hace doble clic en
    él, como con cualquier otro enlace en Amaya, el documento anotado se
    mostrará con el texto anotado resaltado.
    
  </li>
  <li>El campo <strong>Annotation type (Tipo de anotación)</strong> te
    permite clasificar la anotación y cambiar su tipo. Haz clic en el texto
    "Tipo de anotación" para ver la lista de tipos disponibles.
    
  </li>
  <li>El campo <strong>Las modified (Última modificación)</strong> se
    actualiza automáticamente cada vez que se guarda una anotación.</li>
</ul>

<p>Bajo el área de encabezado se encuentra el área del cuerpo de la
anotación. Muestra el contenido actual y puedes editarla como cualquier otro
documento HTML.</p>

<h1>Guardar una anotación</h1>

<p>Una anotación se guarda como cualquier otro documento en Amaya: elige el
comando de menú <strong>Archivo &gt; Guardar</strong> (o utiliza el atajo de
teclado o el botón equivalente).</p>

<p>Las anotaciones locales se guardan en el directorio de anotaciones y las
anotaciones remotas se guardan en el servidor de publicación de anotaciones,
en el que se guardan si el usuario tiene permisos de escritura.</p>

<p>Para convertir una anotación local en compartida, elige el comando de menú
<strong>Anotaciones &gt; Enviar al servidor</strong> para guardar la
anotación en el servidor de publicación definido en la caja de diálogo <span
class="Link"><strong>Anotaciones &gt; Configurar</strong></span>. Si la
operación tiene éxito, la anotación local se elimina y los siguientes comando
<strong>Guardar</strong> lo harán directamente en dicho servidor de
anotaciones..</p>

<p>Algunos comandos que puedes aplicar a un documento en la ventana del
documento, los puedes también aplicar a una anotación en la ventana de
anotación. Por ejemplo, puedes imprimir el cuerpo de una anotación eligiendo
el comando de menú <strong>Archivo &gt; Imprimir</strong>, o puedes recargar
una anotación con el comando de menú <strong>Archivo &gt;
Actualizar</strong>.</p>

<h2>Ver también</h2>
<ul>
  <li><span class="Link0"><a
    href="configuring_annotation_settings.html.es">Configuración de
    anotaciones</a></span></li>
  <li><span class="Link0"><a href="deleting_an_annotation.html.es">Borrar una
    anotación</a></span></li>
  <li><span class="Link0"><a
    href="loading_and_presenting_annotations.html.es">Cargar y mostrar
    anotaciones</a></span></li>
  <li><span class="Link0"><a href="navigating_annotations.html.es">Navegar
    por las anotaciones</a></span></li>
  <li><span class="Link0"><a href="moving_annotations.html.es">Mover
    anotaciones</a></span></li>
</ul>
</div>
</body>
</html>
