<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>Navegando con Amaya</title>
  <meta name="GENERATOR" content="amaya 5.2" />
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
</head>

<body xml:lang="es" lang="es">

<table border="0">
  <tbody>
    <tr>
      <td><p><img alt="W3C" src="../images/w3c_home" /> <img alt="Amaya"
        src="../images/amaya.gif" /></p>
      </td>
      <td><p><a href="Manual.html" accesskey="t"><img alt=""
        src="../images/up.gif" /></a> <a href="Selecting.html"
        accesskey="n"><img alt="next" src="../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="Browsing">
<h2>Navegando versus Editando</h2>

<p><strong>Amaya</strong> es principalmente un editor con capacidades de
navegación. Cuando es usado como navegador, se comporta como cualquier otro
navegador Web, excepto por la <a href="#Activating">activación de
enlaces</a>. En condiciones normales <b>Amaya</b> te permite navegar y editar
documentos simultáneamente, pero puedes cambiar este comportamiento estándar
para cada ventana de documento independientemente. El botón <img
src="../images/Editor.gif" alt="editor-mode" /> muestra que la ventana está
trabajando en modo editor: puedes editar el documento actual y navegar en esa
ventana. Apretando ese botón puedes cambiar la ventana a modo navegador. El
botón aparece entonces como <img src="../images/Browser.gif"
alt="browser-mode" /> y sólo puedes navegar y rellenar formularios. Esta
opción también está disponible desde la opción del menú Editar <b>Modo
Editor</b>, y a través de los atajos de teclado (por defecto el atajo es:
May+Ctrl+<strong>*</strong>).</p>

<p>Amaya te permite navegar y editar documentos HTML así como algunos tipos
de documentos XML: XHTML, MathML y SVG. Te permite navegar por otros tipos de
documentos XML, para presentar sus contenidos y aplicar las hojas de estilo
CSS asociadas.</p>

<p>Los documentos HTML son leídos por defecto como ficheros ISO-latin-1. Si
no hay ninguna información sobre el juego de caracteres en la cabecera HTTP o
en el elemento META, un documento HTML es considerado como codificado en
ISO-latin-1. Los documentos XML son leídos por defecto como ficheros UTF 8.
Cuando no hay información sobre el juego de caracteres en la cabecera HTTP,
en la declaración de XML o en el elemento META, un documento XML es
considerado como codificado en UTF 8. Como el juego de caracteres por defecto
es diferente para los documentos HTML y para los documentos XML, a menudo
ocurre que los autores crean documentos XML ISO-latin-1 en los que no se
ofrece información sobre el juego de caracteres. Tan pronto como un carácter
analizado por el analizador XML no encaja con un carácter UTF 8 (puede
ocurrir si el documento está codificado en ISO-latin-1 pero analizado como un
fichero UTF 8) el documento es considerado como inválido: el análisis se
detiene y se presenta un mensaje de error. En este caso específico, el
comando "Reabrir en ISO-latin-1" se activa. Este comando te permite reabrir
el documento con un juego de caracteres convertido a ISO-latin-1. El
documento reabierto es considerado ahora como modificado (ha sido añadido un
juego de caracteres) y puede ser guardado.</p>

<p><a name="entities">Amaya interpreta entidades de caracteres. Cuando el
correspondiente carácter no está disponible, se presenta un</a>'?' (o se
presenta el valor de la entidad cuando la entidad está dentro de un valor de
atributo o cuando la entidad es desconocida). Cuando se guarda el documento,
estas entidades de caracteres son restituidas excepto si la codificación del
documento es ISO-latin-1 y la entidad se corresponde con un carácter
ISO-latin-1; en este caso, se utiliza el código ISO-latin-1 del carácter. Si
la codificación del documento es ASCII, Amaya traduce los caracteres
ISO-latin-1 no disponibles en entidades ASCII.</p>

<h3><a name="Activating">Activando un enlace</a></h3>

<p>Los enlaces se presentan, normalmente, en azul (o subrayados en los
monitores monocromos). Como <b>Amaya</b> es un editor, deberás hacer doble
clic para activar un enlace. Un único clic sólo mueve el punto de inserción y
presenta el URI del enlace en la barra de estado de la vista principal.</p>

<p>Este comportamiento puede cambiarse al igual que muchos otros parámetros
usando el menú preferencias. Cambiando la opción "Doble clic activa enlaces"
puedes navegar por los documentos con un solo clic. En ese caso no podrás
poner el punto de inserción fácilmente dentro de las anclas: tendrás que usar
el arrastre y/o las flechas para llevar el punto de inserción dentro del
ancla. También es posible utilizar el teclado para activar los enlaces como
en los controles de formulario. El comando de teclado por defecto es
Alt-Enter pero el comando <em>Ejecutar</em> puede ser relacionado con un <a
href="ShortCuts.html">atajo</a> diferente. El comando
<em>SiguienteEnlaceOElementodeFormulario</em>, relacionado por defecto con el
atajo Tab, te permite encontrar el siguiente (o el primer) enlace o elemento
de formulario HTML en el documento. El comando
<em>EnlaceAnteriorOElementodeFormulario</em>, relacionado por defecto con el
atajo May-Tab, te permite encontrar el anterior (o el último) enlace o
elemento de formulario HTML en el documento.</p>

<p>El documento obtenido con esta operación <em>reemplaza</em> al documento
actual en la misma ventana, en el mismo modo (editor o navegador). Si el
documento actual ha sido modificado y no ha sido guardado todavía, Amaya
presenta un cuadro de diálogo que te pregunta si aceptas perder los
cambios.</p>

<p>Un solo clic o un doble clic con el botón derecho del ratón (funciona sólo
con el botón del medio en plataformas Windows) <em>abre</em> el documento
enlazado en una nueva ventana.</p>

<h3><a name="Access">Teclas de acceso</a></h3>

<p>Un autor puede asociar un atributo <code>accesskey</code> a un campo de
formulario, un área o a un enlace. Los siguientes elementos soportan el
<code>accesskey</code>: AREA, BUTTON, INPUT, LABEL, LEGEND, y TEXTAREA. Esto
puede ser crucial para personas con deficiencias motóricas pero hay otras
condiciones en las que puede ser necesario o útil.</p>

<p>Cuando el autor ha asociado un atributo <code>accesskey</code> a un enlace
en un documento (por ejemplo:
<code>accesskey="n"</code>o<code>accesskey="N"</code>), el comando
<strong>Alt-accesskey</strong> (en el ejemplo Alt-a) activará ese enlace.
Presionando <strong>Alt- accesskey</strong> cuando ha sido asignado a un
elemento de formulario llevará el foco a ese elemento (el siguiente elemento
para LABEL y LEGEND).</p>

<p>Las teclas de acceso se superponen a los atajos de Amaya. Por ejemplo, en
las páginas de Ayuda se dice que la tecla de acceso "n" lleva a la página
siguiente, "p" para ir a la página anterior y "t" para la tabla de
contenidos. Si el usuario ha asignado un atajo Alt-p a un comando Amaya, ese
atajo no será accesible cuando se presenten esas páginas de ayuda. Por
defecto la tecla modificadora usada para teclas de acceso es <b>Alt</b>. El
menú de preferencias <code>Especial/Preferencias/General</code> permite
cambiar esta configuración por defecto. El usuario puede usar la tecla
<b>Control</b> en vez de <b>Alt</b> o puede decidir deshabilitar el manejador
de teclas de acceso.</p>

<h3><a name="L515">Anclas destino</a></h3>

<p>Algunos trozos de texto pueden presentar un icono de <a
name="target">diana <img src="../images/target.gif" alt=" target " /></a>.
Estos iconos no son parte del documento y pueden ser mostrados o escondidos
en la vista actual tan sólo controlando el estado de <b>Mostrar destinos</b>,
que es una opción del menú Vistas. El comando de teclado por defecto es
<b>Control-v Control-t</b> en Unix y <b>Alt-v Alt-l</b>en Windows.</p>

<p>Estos iconos están asociados a <em>elementos destino</em>, por ejemplo,
elementos con un atributo <code>ID</code>, <em>anclas destino</em>, por
ejemplo, anclas con un atributo <code>name</code>. Identifican posibles
destinos de enlaces. Estos iconos pretenden hacer la creación de los enlaces
más fácil, como se puede reconocer rápidamente los posibles destinos de un
enlace, puedes <a href="Links.html#L245">crear enlaces</a> rápidamente.</p>

<h3><a name="Opening">Abriendo documentos</a></h3>

<p>Se pueden abrir documentos locales o remotos de varias maneras:</p>
<dl>
  <dt>Menú Archivo</dt>
    <dd>Hay dos entradas en el menú Archivo, <strong>Abrir documento</strong>
      y <strong>Abrir en ventana nueva</strong>, que te permiten abrir un
      nuevo documento en la misma ventana o en una nueva ventana,
      respectivamente. Los comandos de teclado por defecto son,
      repsectivamente, <strong>Control-x Control-f</strong> y
      <strong>Control-x Control-o</strong> en Unix,
      <strong>Control-o</strong> y <strong>Shift-Control-O</strong> en
      Windows. Ambos comandos abren un cuadro de diálogo en el que se puede
      indicar tanto un nombre de fichero local como un URI. También puedes
      seleccionar un archivo desde la opción <b>Buscar</b> que contiene un
      buscador de archivos y te ayuda a localizar ficheros locales. 
      <p>Cuando el documento se ha especificado correctamente, haz clic en el
      botón <b>Confirme</b>. Este comando también puede usarse para crear un
      nuevo documento local.</p>
    </dd>
  <dt>Línea de comando "Abrir"</dt>
    <dd>Este área presenta el nombre de archivo o el URI del documento
      actual. Puedes cambiarlo para especificar un documento diferente (local
      o remoto) a ser presentado en la misma ventana. Presiona la tecla
      "Enter" mientras el cursor esté en el área, para cargar el documento
      realmente. Este comando puede usarse para crear un nuevo documento
      local.</dd>
  <dt>Siguiendo enlaces</dt>
    <dd><a href="#Activating">Activa los enlaces</a> para obtener el
      documento correspondiente.</dd>
</dl>

<h3><a name="Moving">Moviéndose atrás y adelante</a></h3>

<p>Hay varias maneras de moverse atrás o adelante a través de la lista
histórica:</p>
<dl>
  <dt>Menú Archivo</dt>
    <dd>En el menú archivo hay opciones para Volver y Avanzar. Los atajos de
      teclado por defecto son <strong>Alt-b</strong> y <strong>Alt-f</strong>
      en Unix, y Alt-flecha-izquierda y Alt-flecha-derecha en Windows</dd>
  <dt>Barra de botones</dt>
    <dd>Dos botones: <img alt="Back " src="../images/Back.gif" /> y <img
      alt="Forward " src="../images/Forward.gif" /> te permiten volver a la
      página anterior o ir a la siguiente.</dd>
</dl>

<h3><a name="Reloading">Recargando una página</a></h3>

<p>Los documentos pueden ser recargados utilizando la opción <strong>Recargar
documento</strong> en el menú <strong>Archivo</strong>, el atajo
<strong>Alt-r</strong> en Unix y <strong>Control-r</strong> en Windows, o el
botón <img src="../images/Reload.png" alt="reload" /> en la barra de
botones.</p>

<h3><a name="Keyboard">Control a través del teclado</a></h3>

<p>Amaya proporciona control a través del teclado para muchas funciones. En
la documentación puede encontrarse información sobre los comandos de teclado
por defecto. La sección de ayuda <a href="Configure.html">Configurando
Amaya</a> describe cómo cambiar las opciones por defecto, y en todos los
menús junto a las opciones más relevantes aparece la combinación de teclas
actual.</p>

<p>Para los usuarios de Emacs está disponible un fichero de configuración en
<code>Amaya/config/amaya.emacs</code>. El usuario puede crear un enlace desde
su propio espacio privado <code>$HOME/.amaya/amaya.keyboard</code> a este
fichero si quiere trabajar con los atajos de Emacs.</p>

<p>Algunas teclas están predefinidas, como:</p>
<ul>
  <li>La tecla <strong>Enter</strong> dentro de una <code>entrada</code>
    (escribir texto o palabra clave) envía el correspondiente formulario.</li>
  <li>La tecla <strong>Tab</strong> dentro de un elemento de formulario move
    el foco del cursor al siguiente elemento de formulario, o salta al primer
    elemento de formulario dentro de un formulario.</li>
</ul>
</div>

<p><a href="Manual.html"><img alt="" /></a></p>
</body>
</html>
