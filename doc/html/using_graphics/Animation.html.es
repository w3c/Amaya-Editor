<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type"
  content="application/xhtml+xml; charset=iso-8859-1" />
  <title>Gráficos y animaciones SMIL</title>
  <meta name="generator" content="amaya 8.6, see http://www.w3.org/Amaya/" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body>

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="SVGLibrary.html.es" accesskey="p"><img
        alt="anterior" src="../../images/left.gif" /></a> <a
        href="../SVG.html.es" accesskey="t"><img alt="superior" 
        src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Gráficos y animaciones <acronym
title="Synchronized Multimedia Integration Language" xml:lang="en"
lang="en">SMIL</acronym></h1>

<p><acronym title="Scalable Vector Graphics" xml:lang="en"
lang="en">SVG</acronym> representa las animaciones como elementos
entremezclados en la estructura principal que representa la organización de
los gráficos. Los elementos de la animación aparecen como hijos de los
elementos gráficos que animan. Para permitir al autor centrarse en una
animación, la vista de línea temporal le muestra todos los elementos de
animación y agrupados de acuerdo con los elementos gráficos que animan.</p>

<h2>La vista de línea temporal</h2>

<p>La vista de línea temporal presenta las animaciones asociadas con objetos
gráficos. Cada objeto animado del documento se representa allí con una
representación gráfica de sus elementos de animación.</p>

<p>La siguiente figura muestra los tres objetos animados de un documento.
Cada objeto se representa en el lado izquierdo de la vista por medio de una
etiqueta con un fondo blanco y un cuadro de selección junto a su etiqueta. Al
hacer clic en la etiqueta se destaca el elemento correspondiente en la vista
Formateado, lo que le proporciona al usuario el contexto de ese elemento. Si
el elemento gráfico tiene un único elemento de animación, como el elemento
Rectángulo de la figura, ese elemento de animación se presenta como una barra
coloreada. Si hay varios elementos, una única barra en gris representa a la
animación completa (El Círculo en la parte inferior de la figura) y un botón
marcado con el signo '+' en la etiqueta permite al usuario obtener una
representación expandida. El elemento MiTexto, es un ejemplo de tal
representación expandida, en la que cada elemento de animación se representa
mediante una barra coloreada. El botón se convierte en un signo '-' lo que
permite al usuario obtener de nuevo la representación condensada.</p>

<p>La posición en la línea temporal depende de cuándo comienzan y terminan de
actuar, y el color de cada barra refleja el tipo de animación:</p>
<ul>
  <li>Amarillo para <code>animar</code>,</li>
  <li>rojo para <code>configurar</code>,</li>
  <li>verde para <code>animarMovimiento</code>,</li>
  <li>azul marino para <code>animarColor</code>,</li>
  <li>azul claro para <code>animarTransformación</code>.</li>
</ul>

<p class="figure"><img alt="Vista de animación"
src="//home/vatton/Amaya/doc/images/timeline_view.png" /></p>
</div>

<h2>Editar animaciones</h2>

<p>La vista de línea temporal es útil para percibir de un vistazo la
animación de todos los gráficos de un documento, pero también permite a un
autor editar la animación. Los elementos existentes pueden modificarse. La
mayoría de las manipulaciones se hacen directamente sobre la línea temporal,
por ejemplo, al mover una barra o cambiar su extensión. Esto queda
inmediatamente reflejado en otras vistas en las que los correspondientes
atributos de los elementos de la animación se actualizan (en este caso los
atributos <code>inicio</code> y <code>duración</code>).</p>

<p>Para animar el movimiento de un elemento gráfico existente:</p>
<ul>
  <li>El usuario comienza por seleccionar el momento de inicio de la
    animación:
    <p>Haciendo clic en <abbr title="tecla control">Ctrl</abbr> + botón
    izquierdo del ratón sobre la barra de desplazamiento roja y moviéndola
    hacia la derecha de la posición de inicio.</p>
  </li>
  <li>Entonces selecciona el elemento que le interesa en la vista Formateado:
    <p>Si el elemento tiene un ID éste se usará como etiqueta.</p>
  </li>
  <li>Entonces hace clic sobre el botón superior izquierdo <img
    alt="animar_movimiento" src="../../../amaya/anim_motion_sh_db.png" /> en
    la vista de línea temporal:
    <p>Esto crea un nuevo elemento de animación en esa vista.</p>
  </li>
  <li>Entonces el usuario apunta la posición de inicio y de culminación del
    movimiento en la vista de formato:
    <p>Hace clic en la tecla <abbr title="control">Ctrl</abbr> + botón
    izquierdo del ratón en el icono <img alt="cross"
    src="../../../amaya/timeline_cross.gif" /> que se presenta en la vista
    Formateado. Haciendo esto, tiene control sobre las posiciones clave del
    elemento animado en el contexto de los otors elementos gráficos. De hecho
    el usuario traza el recorrido del movimiento como si estuviera dibujando
    la forma de cualquier curva perteneciente al documento.</p>
  </li>
  <li>Finalmente, volviendo a la vista de línea temporal, puede mover y/o
    redimensionar la nueva barra para ajustar el tiempo.</li>
</ul>

<p>Mainpular el tiempo en la vista de línea temporal es más cómodo, ya que
permite al usuario comprender mejor la sincronización de un elemento en
particular con los otros elementos animados. El usuario puede también
modificar las posiciones clave en la vista principal, tan sólo moviendo los
puntos con el ratón. Pero las otras vistas siguen estando ahí, y algunos
parámetros pueden ajustarse en la vista de estructura editando los atributos,
así como también en la vista del código fuente si fuera necesario.</p>

<p>Para crear un cambio de color de un elemento gráfico existente, el usuario
comienza seleccionando el elemento que le interesa en la vista principal, y
hace clic en el botón superior izquierdo <img alt="animar_color"
src="../../../amaya/anim_color_sh_db.png" />.</p>

<p>El usuario puede manipular una barra que representa una animación, de la
misma manera que manipula un rectángulo en un documento. Obviamente hay
algunas limitaciones en la vista de línea temporal. Por ejemplo, las barras
coloreadas pueden moverse sólo horizontalmente a lo largo de el eje de tiempo
y no puede cambiarse su altura individualmente. Estas limitaciones
representan la semántica del lenguaje gráfico de la línea temporal.</p>
</body>
</html>
