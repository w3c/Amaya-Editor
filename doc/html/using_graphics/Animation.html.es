<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type"
  content="application/xhtml+xml; charset=iso-8859-1" />
  <title>Gr&aacute;ficos y animaciones SMIL</title>
  <meta name="generator" content="amaya 8.6, see http://www.w3.org/Amaya/" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body>

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="SVGLibrary.html" accesskey="p"><img
        alt="anterior" src="../../images/left.gif" /></a> <a
        href="../SVG.html" accesskey="t"><img alt="arriba"
        src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Gr&aacute;ficos y animaciones <acronym title="Synchronized Multimedia Integration Language" xml:lang="en" lang="en">SMIL</acronym></h1>

<p><acronym title="Scalable Vector Graphics" xml:lang="en" lang="en">SVG</acronym> representa las animaciones como elementos entremezclados en la estructura principal que representa la organizaci&oacute;n de los gr&aacute;ficos. Los elementos de la animación aparecen como hijos de los elementos gráficos que animan. Para permitir al autor centrarse en una animaci&oacute;n, la vista de l&iacute;nea temporal le muestra todos los elementos de animaci&oacute;n y agrupados de acuerdo con los elementos gr&aacute;ficos que animan.</p>

<h2>La vista de l&iacute;nea temporal</h2>

<p>La vista de l&iacute;nea temporal presenta las animaciones asociadas con objetos gr&aacute;ficos. Cada objeto animado del documento se representa all&iacute; con una representaci&oacute;n gr&aacute;fica de sus elementos de animaci&oacute;n.</p>

<p>La siguiente figura muestra los tres objetos animados de un documento. Cada objeto se representa en el lado izquierdo de la vista por medio de una etiqueta con un fondo blanco y un cuadro de selecci&oacute;n junto a su etiqueta. Al hacer clic en la etiqueta se destaca el elemento correspondiente en la vista Formateado, lo que le proporciona al usuario el contexto de ese elemento. Si el elemento gr&aacute;fico tiene un &uacute;nico elemento de animaci&oacute;n, como el elemento Rect&aacute;ngulo de la figura, ese elemento de animaci&oacute;n se presenta como una barra coloreada. Si hay varios elementos, una &uacute;nica barra en gris representa a la animaci&oacute;n completa (El C&iacute;rculo en la parte inferior de la figura) y un bot&oacute;n marcado con el signo '+' en la etiqueta permite al usuario obtener una representaci&oacute;n expandida. El elemento MiTexto, es un ejemplo de tal representaci&oacute;n expandida, en la que cada elemento de animaci&oacute;n se representa mediante una barra coloreada. El bot&oacute;n se convierte en un signo '-' lo que permite al usuario obtener de nuevo la representaci&oacute;n condensada.</p>


<p>La posici&oacute;n en la l&iacute;nea temporal depende de cu&aacute;ndo comienzan y terminan de actuar, y el color de cada barra refleja el tipo de animaci&oacute;n:</p>
<ul>
  <li>Amarillo para <code>animar</code>,</li>
  <li>rojo para <code>configurar</code>,</li>
  <li>verde para <code>animarMovimiento</code>,</li>
  <li>azul marino para <code>animarColor</code>,</li>
  <li>azul claro para <code>animarTransformaci&oacute;n</code>.</li>
</ul>

<p class="figure"><img alt="Vista de animaci&oacute;n"
src="//home/vatton/Amaya/doc/images/timeline_view.png" /></p>
</div>

<h2>Editar animaciones</h2>

<p>La vista de l&iacute;nea temporal es &uacute;til para percibir de un vistazo la animaci&oacute;n de todos los gr&aacute;ficos de un documento, pero tambi&eacute;n permite a un autor editar la animaci&oacute;n. Los elementos existentes pueden modificarse. La mayor&iacute;a de las manipulaciones se hacen directamente sobre la l&iacute;nea temporal, por ejemplo, al mover una barra o cambiar su extensi&oacute;n. Esto queda inmediatamente reflejado en otras vistas en las que los correspondientes atributos de los elementos de la animaci&oacute;n se actualizan (en este caso los atributos <code>inicio</code> y
<code>duraci&oacute;n</code>).</p>

<p>Para animar el movimiento de un elemento gr&aacute;fico existente:</p>
<ul>
  <li>El usuario comienza por seleccionar el momento de inicio de la animaci&oacute;n 
    <p>Haciendo clic en <abbr title="tecla control">Ctrl</abbr> + bot&oacute;n izquierdo del rat&oacute;n sobre la barra de desplazamiento roja y movi&eacute;ndola hacia la derecha de la posici&oacute;n de inicio.</p>
  </li>
  <li>Entonces selecciona el elemento que le interesa en la vista Formateado 
    <p>Si el elemento tiene un ID &eacute;ste se usar&aacute; como etiqueta,</p>
  </li>
  <li>Entonces hace clic sobre el bot&oacute;n superior izquierdo <img alt="animar_movimiento"
    src="../../../amaya/anim_motion_sh_db.png" /> en la vista de l&iacute;nea temporal
    <p>Esto crea un nuevo elemento de animaci&oacute;n en esa vista.</p>
  </li>
  <li>Entonces el usuario apunta la posici&oacute;n de inicio y de culminaci&oacute;n del movimiento en la vista de formato
    <p>Hace clic en la tecla <abbr title="control">Ctrl</abbr> + bot&oacute;n izquierdo del rat&oacute;n en el icono <img alt="cross"
    src="../../../amaya/timeline_cross.gif" /> que se presenta en la vista Formateado. Haciendo esto, tiene control sobre las posiciones clave del elemento animado en el contexto de los otors elementos gr&aacute;ficos. De hecho el usuario traza el recorrido del movimiento como si estuviera dibujando la forma de cualquier curva perteneciente al documento.</p>
  </li>
  <li>Finalmente, volviendo a la vista de l&iacute;nea temporal, puede mover y/o redimensionar la nueva barra para ajustar el tiempo.</li>
</ul>

<p>Mainpular el tiempo en la vista de l&iacute;nea temporal es m&aacute;s c&oacute;modo, ya que permite al usuario comprender mejor la sincronizaci&oacute;n de un elemento en particular con los otros elementos animados. El usuario puede tambi&eacute;n modificar las posiciones clave en la vista principal, tan s&oacute;lo moviendo los puntos con el rat&oacute;n. Pero las otras vistas siguen estando ah&iacute;, y algunos par&aacute;metros pueden ajustarse en la vista de estructura editando los atributos, as&iacute; como tambi&eacute;n en la vista del c&oacute;digo fuente si fuera necesario.</p>

<p>Para crear un cambio de color de un elemento gr&aacute;fico existente, el usuario comienza seleccionando el elemento que le interesa en la vista principal, y hace clic en el bot&oacute;n superior izquierdo <img alt="animar_color"
src="../../../amaya/anim_color_sh_db.png" />.</p>

<p>El usuario puede manipular una barra que representa una animaci&oacute;n, de la misma manera que manipula un rect&aacute;ngulo en un documento. Obviamente hay algunas limitaciones en la vista de l&iacute;nea temporal. Por ejemplo, las barras coloreadas pueden moverse s&oacute;lo horizontalmente a lo largo de el eje de tiempo y no puede cambiarse su altura individualmente. Estas limitaciones representan la sem&aacute;ntica del lenguaje gr&aacute;fico de la l&iacute;nea temporal.</p>


</body>
</html>

