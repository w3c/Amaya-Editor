<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <title>Soporte de marcadores</title>
  <style type="text/css">

  </style>
  <meta name="GENERATOR" content="amaya 8.7, see http://www.w3.org/Amaya/" />
  <link href="style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../images/w3c_home" /> <img alt="Amaya"
        src="../images/amaya.gif" /></td>
      <td><p align="right"><a href="Annotations.html.es" accesskey="p"><img
        alt="anterior" src="../images/left.gif" /></a><a
        href="Manual.html.es" accesskey="t"><img alt="superior"
        src="../images/up.gif" /></a> <a href="Configure.html.es"
        accesskey="n"><img alt="siguiente" src="../images/right.gif"
        /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1><a name="What">Soporte de marcadores</a></h1>

<p><strong>NOTA: Esta es una versión beta de la implementación de marcadores
Annotea en Amaya. El interfaz de usuario necesita mejorarse y sólo funciona
actualmente en Linux.</strong></p>

<p>Desde la versión 8.1, Amaya soporta el <a
href="http://www.w3.org/2003/07/Annotea/BookmarkSchema-20030707.html">esquema
de marcadores Annotea</a>. Haz clic en el enlace para consultar una
descripción detallada de este esquema RDF. Este documento describe el
interfaz de usuario y los problemas conocidos de la implementación en
Amaya.</p>

<p>El interfaz de usuario es muy similar al de otros navegadores y te permite
hacer prácticamente las mismas operaciones. Una notable diferencia es que el
esquema de marcadores Annotea usa el término <em>Tema</em> en vez de
<em>Carpeta</em>. El motivo de esta elección es que la organización de este
tipo de marcadores no se limita a jerarquías estrictas, sino que puede
extenderse a grafos completos.</p>

<h2>Menú marcadores</h2>

<p>El usuario puede acceder a los marcadores a través del menú
<em>Marcadores</em> de la barra de menús:</p>

<p style="text-align: center"><img src="../images/bmmenu-es.png"
alt="Menú Marcadores en la barra de menús" border="1" /></p>

<p>Las opciones de este menú son:</p>
<ul>
  <li><em>Nuevo marcador</em>: abre una ventana para añadir el documento
    actual al archivo de marcadores.</li>
  <li><em>Nuevo Tema</em>: abre una ventana para añadir un nuevo tema al
    archivo de marcadores.</li>
  <li><em>Ver marcadores</em>: muestra el archivo de marcadores.</li>
</ul>

<p>Estas opciones se detallan en las siguientes secciones.</p>

<h2>Crear un nuevo marcador</h2>

<p>Cuando quieres marcar un determinado documento que estés navegando, elige
la opción <em>Nuevo marcador</em> de la barra de menús. Se abrirá la
siguiente ventana (<em>Propiedades de marcador</em>):</p>

<p style="text-align: center"><img src="../images/bmbprop.png"
alt="Ventana Propiedades de marcador" border="1" /></p>
</div>

<p>Un marcador puede pertenecer a uno o más temas. El campo <strong>Jerarquía
de temas</strong> muestra los temas existentes y te deja elegir los temas en
los que quieres incluir el marcador. Por ejemplo, la imagen anterior muestra
un marcador que pertenece a los temas INRIA y W3C. Ten en cuenta que un
marcador debe pertenecer al menos a un tema. Amaya no te permitirá crear un
marcador si no es así. Puedes crear un nuevo tema desde esta ventana mediante
el botón <strong>Nuevo tema</strong>, situado en la parte inferior de la
ventana.</p>

<p>El campo <strong>Título</strong> da el título al marcador. Si el documento
que estás marcando está escrito en un lenguaje de marcas que Amaya entiende
(p.e., XHTML, HTML, ...) y si el documento tiene un elemento de título
reconocible, en este campo se mostrará el valor del elemento. En caso
contrario, se mostrará el URL del documento.</p>

<p>El campo <strong>Destino</strong> proporciona el URL del documento al que
apunta el marcador.</p>

<p>Los campos <strong>Creado</strong> y <strong>Última modificación</strong>
indican respectivamente la fecha de creación y de la última modificación del
marcador.</p>

<p>El campo <strong>Descripción</strong> te permite añadir un texto opcional
que describa el marcador.</p>

<p>Al botón <strong>Aplicar</strong> te permite añadir el marcador al archivo
de marcadores o, si lo estás modificando, actualizarlo.</p>

<p>El botón <strong>Nuevo Tema</strong> abre la ventana <em>Propiedades del
tema</em>, que te permite crear un nuevo tema.</p>

<p>El botón <strong>Aceptar</strong> cierra la ventana y descarta cualquier
cambio no guardado.</p>

<div id="page_body1">
<h2>Crear un tema nuevo</h2>

<p>Cuando quieras crear un tema nuevo, elige la opción <em>Nuevo Tema</em>
del menú <em>Marcadores</em>. Se abrirá la ventana <em>Propiedades de
tema</em>:</p>

<p style="text-align: center"><img src="../images/bmtprop.png"
alt="Ventana Propiedades de Tema" border="1" /></p>
</div>

<p>En este momento, los temas tienen que tener una estrutura jerárquica. Es
decir, cada tema tiene un tema padre y puede tener subtemas almacenados bajo
él. Por omisión, hay un tema raíz, denominado <em>Tema Inicial</em>, que se
crea automáticamente. Todos los temas que crees se almacenarán bajo él.</p>

<p>El campo <strong>Jerarquía de temas</strong> muestra los temas existentes
que hayas creado y te deja elegir el tema bajo el que se almacenará el nuevo
tema. Por ejemplo, la imagen anterior muestra el tema <em>W3C</em> que se
almacenará bajo el <em>Tema Inicial</em>.</p>

<p>El resto de campos de esta ventana tiene el mismo significado que el
descrito anteriormente en la ventana de marcadores.</p>

<p>El botón <strong>Aplicar</strong> te permite añadir un tema al archivo de
marcadores o, si lo estás modificando, actualizarlo.</p>

<p>El botón <strong>Aceptar</strong> cierra la ventana y descarta cualquier
cambio no guardado.</p>

<div id="page_body11">
<h2>Utilizar el archivo de marcadores</h2>

<p>Cuando quieras ver y utilizar el archivo de marcadores, elige la opción
<em>Ver marcadores</em> del menú <em>Marcadores</em>. Se abrirá una nueva
ventana que muestra tus marcadores y temas:</p>

<p style="text-align: center"><img src="../images/bmview1.png"
alt="Ventana Propiedades de tema" border="2" /></p>
</div>

<p>En esta ventana de marcadores, los temas están precedidos con el icono
<img alt="collapsed topic" src="../images/closed.png" /> o <img
alt="expanded topic" src="../images/open.png" />. El resto de items son
marcadores. Puedes activar un item haciendo clic con el botón izquierdo del
ratón o puedes ver sus propiedades haciendo clic con el botón derecho del
ratón. Si ves las propiedades de un item, también puedes editarlo y guardar
las modificaciones.</p>

<p>En el caso de un marcador, al hacer clic izquierdo se abrirá una nueva
ventana que mostrará el documento al que apunta el marcador. En el caso de un
tema, al hacer clic izquierdo se desplegará o plegará su contenido. El icono
<img alt="collapsed topic" src="../images/closed.png" /> representa un tema
plegado, y el icono <img alt="expanded topic" src="../images/open.png" />
representa un tema desplegado.</p>

<p>Para borrar un item, coloca el cursor sobre él y selecciónalo pulsando la
tecla <kbd>F2</kbd>:</p>

<p style="text-align: center"><img src="../images/bmview2.png"
alt="Tema e items seleccionados" border="2" /></p>

<p>Una vez seleccionado el item, pulsa la tecla <em>Suprimir</em> para
borrarlo. Si seleccionas un tema, todos sus hijos se seleccionarán y borrarán
con él. Ten en cuenta que si un marcador pertenece a varios temas, al
borrarlo sólo se destruye la propiedad RDF que indica que pertenece al tema
borrado. Para borrar completamente un marcador que pertenece a varios temas,
debes borrarlo de todos los temas. También puedes editar sus propiedades,
hacer que pertenezca sólo a un tema y entonces borrarlo.</p>

<h2>Para expertos</h2>

<p>Los marcadores se guardan en la carpeta de preferencias de Amaya con el
nombre <em>bookmarks.rdf</em>. Si editas este archivo y le añades propiedades
adicionales, éstas serán conservadas, gracias a que utilizamos Redland para
analizar y consultar el modelo RDF. Para más información, consulta la página
web del proyecto <a href="http://www.w3.org/2001/Annotea/">Annotea</a>.</p>
</body>
</html>
