<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>Anotaciones en Amaya</title>
  <meta name="GENERATOR" content="amaya 5.2" />
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
</head>

<body xml:lang="es" lang="es">

<table border="0" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../images/w3c_home" /> <img alt="Amaya"
        src="../images/amaya.gif" /></td>
      <td><p><a href="MakeBook.html" accesskey="p"><img alt="previa"
        src="../images/left.gif" /></a> <a href="Manual.html"
        accesskey="t"><img alt="arriba" src="../images/up.gif" /></a> <a
        href="Configure.html" accesskey="n"><img alt="siguiente"
        src="../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>
<ul>
  <li><a href="#What">¿Qué es una Anotación?</a></li>
  <li><a href="#Local">Anotaciones Locales y Remotas</a></li>
  <li><a href="#Annotation">El Menú Anotaciones</a></li>
  <li><a href="#Configurat">Preparando la Configuración para las
    Anotaciones</a></li>
  <li><a href="#Creating1">Creando una Anotación</a></li>
  <li><a href="#Deleting">Borrando una Anotación</a></li>
  <li><a href="#Loading">Cargando y Presentando Anotaciones</a></li>
  <li><a href="#Navigating">Navegando entre Anotaciones</a></li>
  <li><a href="#Moving">Moviendo Anotaciones</a></li>
  <li><a href="#Issues1">Problemas con Anotaciones y Documentos
    Modificados</a></li>
</ul>

<div id="Annotations">
<h2>Anotaciones en Amaya</h2>

<h3><a name="What">¿Qué es una Anotación?</a></h3>

<p>La anotaciones son comentarios, notas, explicaciones, u otro tipo de
observaciones que pueden adjuntarse a un documento Web o a una parte
seleccionada del documento. Como son externas es posible anotar cualquier
documento Web de manera independiente, sin necesidad de editar ese documento.
Desde el punto de vista técnico, las anotaciones normalmente son vistas como
<b>metadatos</b>, ya que dan información adicional sobre una porción
existente de datos. En este proyecto, para describir las anotaciones usamos
un <strong><a href="http://www.w3.org/RDF/">RDF</a></strong> <a
href="http://www.w3.org/2000/10/annotation-ns">annotation schema</a>
especial.</p>

<p>Las anotaciones pueden guardarse localmente o en uno o más <b>servidores
de anotaciones</b>. Cuando un documento es hojeado, Amaya llama a cada uno de
esos servidores, solicitando las anotaciones relacionadas con el documento.
Actualmente Amaya presenta las anotaciones con los iconos de anotación ( <img
src="../images/annot.png" alt="Icono lápiz de anotación" /> ) que están
visualmente incrustados en el documento, como se muestra en la figura de
abajo. Si el usuario hace un único clic sobre un icono de anotación, el texto
que ha sido anotado se resalta. Si el usuario hace doble clic sobre este
icono, el texto de la anotación y otros metadatos se presentan en una ventana
aparte.</p>

<p style="text-align: center"><img src="../images/annotationicon-es.png"
alt="icono de anotación (= lápiz)" /></p>

<p>Una anotación tiene muchas propiedades, incluyendo:</p>
<ul>
  <li>Localización física: ¿La anotación está almacenada en un archivo local
    o en una servidor de anotaciones?</li>
  <li>Alcance: ¿Está la anotación asociada a todo el documento o sólo a un
    fragmento de ese documento?</li>
  <li>Tipo de anotación: '<i xml:lang="en" lang="en">Anotación</i>', '<i
    xml:lang="en" lang="en">Comentario</i>', '<i xml:lang="en"
    lang="en">Consulta</i>', ...</li>
</ul>

<h3 id="Local">Anotaciones locales y remotas</h3>

<p>Amaya puede guardar datos de anotaciones en un sistema de archivo local
(llamado "anotaciones locales") o en la Web (llamado "anotaciones remotas").
<b>Anotaciones remotas</b> son las almacenadas en servidores de anotaciones y
pueden ser descargadas y almacenadas por cualquiera que tenga los apropiados
derechos de acceso, tal como en el caso de otros documentos <acronym
title="HyperText Markup Language" xml:lang="en" lang="en">HTML</acronym>.
También nos referimos a anotaciones remotas como <b>anotaciones públicas o
compartidas</b>, ya que pueden ser vistas por otras personas. Si deseas
instalar tu propio servidor de anotaciones, por favor, ve a <a
href="http://www.w3.org/1999/02/26-modules/User/Annotations-HOWTO.html">Annotation-Server-HOWTO</a>.</p>

<p>El usuario no necesita un servidor para hacer una <b>anotación local</b>.
Las anotaciones locales se almacenan en el <a
href="/home/kahan/Amaya/doc/amaya/Configure.html">directorio de preferencias
de usuario</a>, en un directorio especial llamado "<code>annotations</code>",
y sólo puede ser visto por el dueño (de acuerdo a cómo esté organizado el
sistema de derechos de acceso). Este directorio está conformado por tres
tipos de ficheros:</p>
<dl>
  <dt><strong>annot.index</strong></dt>
    <dd>asocia <acronym title="Uniform Resource Locator" xml:lang="en"
      lang="en">URLs</acronym> con los archivos en los que se almacenan los
      metadatos de las anotaciones.</dd>
  <dt><strong>index + random suffix</strong></dt>
    <dd>un fichero que almacena el metadato de las anotaciones relacionadas
      con una <acronym title="Uniform Resource Locator" xml:lang="en"
      lang="en">URL</acronym> dada. El metadato se especifica con RDF.</dd>
  <dt><strong>annot + random suffix.html</strong></dt>
    <dd>contiene el cuerpo de una anotación, almacenado como <acronym
      title="EXtensible HTML" xml:lang="en" lang="en">XHTML</acronym>.</dd>
</dl>

<p>Un usuario puede, en cualquier momento, convertir una anotación local en
una compartida usando la opción de menú <strong>Anotaciones/Post
anotación</strong>. Si esta operación tiene éxito, la anotación local será
borrada ya que será movida a un servidor de anotacion. La siguiente sección
describe cómo indicar el nombre del servidor en el que deberían colocarse las
anotaciones.</p>

<h3 id="Annotation">Menú Anotaciones</h3>

<p>La mayoría de los comandos necesarios para manejar anotaciones se
encuentran en el menú Anotaciones mostrado abajo. Los comandos se explican
más adelante en este documento, en el contexto de las metas del usuario
cuando maneja anotaciones.</p>

<p style="text-align: center"><img src="../images/annotationsmenu-es.png"
alt="Menú Anotaciones" /></p>

<h3 id="Configurat">Preparando la Configuración para las Anotaciones</h3>

<p>El menú de configuración de anotaciones se encuentra bajo la opción de
menú <strong>Anotaciones/Configurar</strong>. La siguiente figura muestra la
versión Windows de este menú. La versión Unix tiene una interfaz de usuario
un tanto diferente, pero las mismas funcionalidades.</p>

<p style="text-align: center"><img src="../images/configurebox-es.png"
alt="cuadro de diálogo de configuración" /></p>

<p></p>
<dl>
  <dt><strong>Anotación de usuario</strong></dt>
    <dd>Un campo que está asociado con cada nueva anotación como el
      <b>autor</b> de la anotación. Amaya utiliza, por defecto, el
      identificador de usuario de la persona, cuando crea el campo de
      metadato autor. La selección de la anotación de usuario le permite a un
      usuario cambiar este nombre por uno más significativo, por ejemplo, en
      vez de <code>u1723242</code> (que hace felíz a mi administrador de
      sistema) <code>marja</code> (que es el que usan otras personas
      normalmente cuando hablan conmigo).</dd>
  <dt><strong>Servidores de Anotaciones</strong></dt>
    <dd>Este campo le dice a Amaya qué servidores debe contactar cuando
      busque anotaciones. Se pueden especificar uno o más servidores. Hemos
      reservado el nombre de servidor localhost para decirle a Amaya que
      queremos buscar anotaciones locales. Esto no significa que estemos
      poniendo en marcha un servidor local de anotaciones. Si este fuera el
      caso, necesitaríamos indicar la <acronym
      title="Uniform Resource Locator" xml:lang="en" lang="en">URL</acronym>
      completa. Bajo Unix, los servidores de anotación se especifican como
      una lista separada por espacios. Bajo Windows, son añadidos uno tras
      otro tecleando sus nombres directamente en el cuadro de diálogo,
      utilizando la tecla <code>Enter</code> para añadir uno nuevo. Por
      defecto, el campo para servidores de anotaciones indica un
      <code>localhost</code>. 
      <p><em>Truco</em>: Si quieres deshabilitar temporalmente un servidor de
      anotación, añade el carácter "-" antes de su <acronym
      title="Uniform Resource Locator" xml:lang="en" lang="en">URL</acronym>.
      Amaya lo ignorará.</p>
    </dd>
  <dt><strong>Autocargar anotaciones locales/Autocargar anotaciones
  remotas</strong></dt>
    <dd>Esta opción le dice a Amaya si solicita o no anotaciones,
      automáticamente (esto es, consultar a los servidores de anotaciones)
      cada vez que una <acronym title="Uniform Resource Locator"
      xml:lang="en" lang="en">URL</acronym> es navegada. Si no está
      seleccionada, entonces el usuario tiene que invocar manualmente la
      función <b>Cargar anotaciones</b> desde el menú <b>Anotaciones</b>.
      Nótese que si se selecciona la opción <code>autocargar anotaciones
      remotas</code>, puede haber un retraso mientras Amaya resuelve el
      nombre de DNS de los servidores de anotación. Por defecto ambas
      opciones aparecen sin seleccionar.</dd>
  <dt><strong>Deshabilitar autocarga remota en cada inicio</strong></dt>
    <dd>Si esta opción está seleccionada, Amaya restablecerá la opción
      <code>autocargar anotaciones remotas</code> cada vez que se inicie.
      Esta opción puede ser útil si estás trabajando desconectado a ratos,
      pero seguirá queriendo autocargar las anotaciones locales, y las
      remotas de vez en cuando, por ejemplo, cuando te conectas durante un
      rato.</dd>
  <dt><strong>Servidor de publicación</strong></dt>
    <dd><p>Este campo define el servidor en el que serán publicadas las
      anotaciones. Las anotaciones locales se guardan siempre en una base de
      datos local, independientemente del valor de este campo. Por defecto
      este campo está vacío. Advierte que el servidor de publicación no está
      incluido automáticamente en la lista de servidores a ser consultados,
      se debe indicar el nombre del servidor de publicación en ambos campos,
      en cualquier orden que tú elijas.</p>
    </dd>
</dl>

<h3 id="Creating">Creando una anotación</h3>

<p>Esta versión de Amaya soporta dos tipos de anotaciones: anotaciones que se
refieren a un documento completo y anotaciones que se refieren a un punto
específico o selección en un documento.</p>

<p>Para anotar un documento completo, tan solo selecciona la opción de
menú</p>

<h2></h2>

<p><b>Anotaciones/Anotar documento</b>. Para anotar un punto, pon el cursor
en cualquier punto en el documento y selecciona la opción de menú
<b>Anotaciones/Seleccionar anotación</b>. Para anotar una selección,
selecciona algo en el documento, y luego usa la opción de menú
<b>Anotaciones/Anotar selección</b>. En todos estos casos aparecerá una
ventana de anotación (ver la figura siguiente). El contenido de esta ventana
muestra el metadato de la anotación, dentro de un recuadro, y el cuerpo de la
anotación.</p>

<p style="text-align: center"><img src="../images/annotationwindow-es.png"
alt="cuadro de diálogo de configuración de anotaciones" /></p>

<p></p>

<p>Normalmente, el metadato consiste en el <b>nombre del autor</b>, el
<b>título del documento anotado</b>, el <b>tipo de anotación</b>, la <b>fecha
de creación</b>, y la <b>fecha de última modificación</b>. Algunos de los
campos de metadatos tienen propiedades especiales. El campo <b>documento
fuente</b> es también un enlace que apunta de vuelta al texto anotado. Si el
usuario hace doble clic sobre él, al igual que se sigue cualquier otro enlace
en Amaya, el documento anotado será obtenido y el texto anotado aparecerá
resaltado. El campo <b>tipos de anotación</b> permite al usuario clasificar
la anotación y cambiar su tipo. Haz doble clic sobre el texto "tipo de
anotación" para ver la lista de tipos disponibles. Describiremos más abajo
cómo pueden los usuarios definir también, sus propios tipos de anotaciones.
Finalmente, el campo <b>Última modificación</b> se actualiza automáticamente
cada vez que se guarda una anotación.</p>

<p>Debajo, el área principal es el <b>área del cuerpo de anotación</b>.
Muestra el contenido actual y puede ser editada como si estuviéramos editando
cualquier otro documento <acronym title="HyperText Markup Language"
xml:lang="en" lang="en">HTML</acronym>, .N.B., aunque todavía no se pueden
incluir imágenes en el cuerpo. Algunas de las características pueden no estar
listas en la versión inicial, tales como la vista Estructura.</p>

<p>Guardar una anotación es equivalente a guardar cualquier otro documento en
Amaya. El usuario sólo necesita seleccionar el comando <b>Archivo/Guardar</b>
(o utilizar su equivalente atajo de teclado o botón del menú). Las
anotaciones locales se guardan en el directorio "<span xml:lang="en"
lang="en">annotations</span>" y las anotaciones remotas se guardan en el
servidor de publicación de anotaciones donde son almacenadas si el usuario ha
escrito el acceso. Para convertir una anotación local en una compartida, el
usuario necesita usar el comando <b>Anotaciones/Publicar anotación</b> y la
anotación será guardada en el <b>Servidor de publicación</b> que esté
definido en el menú de configuración. Si esta operación tiene éxito, la
anotación local será borrada y las futuras operaciones de guardado irán
directamente al servidor de anotaciones. En la versión inicial, Amaya no
soporta una operación de guardado de una copia de una anotación remota en el
directorio local de anotaciones.</p>

<p>Algunos comandos aplicados al documento en la <b>ventana Documento
Amaya</b> se aplicarán al documento de anotación en la <b>ventana
Anotación</b>. Por ejemplo, el cuerpo de una anotación puede imprimirse con
el comando <b>Archivo/Imprimir</b> o recargado con el comando
<b>Archivo/Recargar documento</b>. (Nota: Algunas de estas características
pueden no estar incluídas aún).</p>

<h3 id="Deleting">Borrando una Anotación</h3>

<p>El comando del menú Anotaciones/Borrar anotación permite borrar una
anotación. Se puede invocar este comando desde una ventana de anotación
abierta. También se puede borrar una anotación desde el documento anotado
seleccionando primero la anotación, haciendo un único clic sobre el icono de
anotación, y luego llamando a este comando de menú.</p>

<h3 id="Loading">Cargando y Presentando Anotaciones</h3>

<p>El comando Cargar Anotaciones le dice a Amaya que cargue las anotaciones
asociadas a la <acronym title="Uniform Resource Locator" xml:lang="en"
lang="en">URL</acronym> del documento que está siendo navegado en esa
ventana. Amaya consultará al servidor de anotaciones, que esté indicado en el
cuadro de diálogo Anotaciones/Configurar, pidiéndole cualquier anotación
relevante.</p>

<p>Las anotaciones pueden descargarse también automáticamente siempre que se
carga una nueva página seleccionando la casilla de verificación <b>Autocargar
anotaciones</b> en el cuadro de diálogo <b>Anotaciones/Configurar</b>.</p>

<p>Notese que en esta versión de Amaya, al interrogar a un servidor de
anotaciones devolverá <i>todas</i> las anotaciones que están asociadas a un
documento. En una versión futura será posible usar un menú personalizado para
refinar la cadena de interrogantes que se envía a los servidores.</p>

<p>El menú <b>Anotaciones/Filtro</b> local permite al usuario mostrar o
esconder los iconos de anotación desde la vista de documento, por ejemplo,
para concentrarse en aquello que realmente le interesan o para hacer más
fácil la lectura de un documento lleno de anotaciones. El usuario puede
mostrar o esconder anotaciones por tres tipos de metadatos: por el nombre de
<b>autor</b> de la anotación, por el <b>tipo</b> de anotación, y por el
nombre de <b>servidor de anotación</b>. Para aplicar uno de estos filtros,
necesitas hacer clic en el botón de opción para seleccionar un tipo de
anotación dado y, luego en el correspondiente botón de acción. Los comandos
<b>Mostrar todo</b> y <b>Esconder todo</b> se aplican a todas las
anotaciones. Un pequeño e incómodo caracter a modo de prefijo señala el
estado de una entrada dada. Este caracter puede ser tanto un espacio (' '),
un asterisco ('*') o un guión ('-') para declarar que esas anotaciones que
pertenecen a esa entrada dada están todas visibles, todas escondidas,o
parcialmente visibles, respectivamente.</p>

<p>Ten en cuenta que el menú filtro sólo muestra las anotaciones que conoce
que existen en el momento de su invocación. Si añades nuevas anotaciones
entretanto, tendrás que destruir este menú e invocarlo de nuevo para
verlas.</p>

<p>Advierte que para cada anotación, el usuario de anotación se muestra
concatenando el nombre del autor con el nombre del servidor de anotación en
el que está almacenada la anotación, tal como se muestra en la siguiente
figura.</p>

<p></p>

<p style="text-align: center"><img src="../images/localfilter-es.png"
alt="cuadro de diálogo del filtro local" /></p>

<h3 id="Navigating">Navegando entre Anotaciones</h3>

<p>La anotaciones aparecen como enlaces en la <b>ventana Enlaces</b> que
puede abrirse mediante el comando <b>Vistas/Mostrar enlaces</b>. En esta
ventana las anotaciones aparecen marcadas con el mismo icono del lápiz, usado
en las anotaciones en la ventana de documento. La ventana Enlace muestra
todas las anotaciones, sin tener en cuenta si han sido escondidas con el menú
<b>Filtro Local <span
style="font-weight: normal; font-family: times">de</span> Anotaciones</b>. Al
igual que en la ventana de documento, un único clic sobre la anotación
seleccionará el texto anotado en la ventana de documento y un doble clic
abrirá la anotación. Este es un ejemplo de cómo navegar de un enlace de
anotación a otro, incluso si la anotación no pude ser vista por el usuario
debido a su discapacidad o debido a las características del dispositivo
usado.</p>

<p></p>

<p style="text-align: center"><img src="../images/linkwindow-es.png"
alt="La vista enlaces muestra las anotaciones" /></p>

<h3 id="Moving">Moviendo anotaciones</h3>

<p>Amaya también te permite mover una anotación a cualquier otra parte del
mismo documento. Esto es interesante, por ejemplo, para manejar <a
href="#Issues1">anotaciones huérfanas y corruptas</a>. Se puede mover una
anotación a la selección actual o también a el valor de un XPointer
almacenado. Por el momento sólo es posible mover anotaciones en el mismo
documento en el que han sido creadas.</p>

<p>Para mover una anotación a la selección actual, primero hay que abrir la
ventana de anotación. Entonces hay que hacer clic en el documento anotado y
seleccionar algo en él. Luego, en la Ventana Anotación, invocar el menú
<b>Anotaciones/Mover a selección</b>. Amaya moverá entonces el icono de
anotación al texto seleccionado y marcará el documento de anotación como
modificado. Para conseguir que este cambio se efectivo hay que guardar la
anotación, de otra manera se perderán los cambios. Al igual que en la
creación de anotaciones se puede mover una anotación a la posición actual del
cursor, sin tener que hacer una selección.</p>

<p>También se puede memorizar la posición a la que se quiere mover la
anotación y, entonces, moverla ahí. Esto es útil, por ejemplo, si se quieren
mover varias anotaciones a la misma posición o si se quiere desplazarse a
cualquier parte del documento antes de moverlas. Para hacer esto, haz una
selección (o sólo posiciona el cursor) en el lugar en el que quieres mover la
anotación. Entonces utiliza el menú <b>Enlace/Almacenar enlace como
XPointer</b> para almacenar internamente un XPointer que represente la
selección. Finalmente, en la ventana Anotaciones, utiliza el menú
<b>Anotaciones/Mover para almacenar XPointer</b> para mover la anotación a su
nueva localización. Al igual que antes deberás guardar la anotación para
hacer efectivo el cambio.</p>
</div>

<div id="Issues">
<h3 id="Issues1">Problemas con Anotaciones y Documentos Modificados</h3>

<p>Si se está usando anotaciones con documentos en vivo (documentos cuyos
contenidos pueden ser modificados) puedes encontrar dos tipos de problemas:
<b>anotaciones huérfanas</b> y <b>anotaciones corruptas</b>. Déjanos antes
describir un poco más en detalle cómo Amaya añade anotaciones a los
documentos.</p>

<p>Amaya utiliza <strong><a
href="http://www.w3.org/XML/Linking">XPointer</a></strong> para describir
dónde debe añadirse una anotación a un documento. XPointer está basado en la
estructura del documento. Para construir un XPointer, por ejemplo para una
selección, comenzamos desde el primer punto de la selección y vamos
regresando a través de la estructura del documento hasta encontrar la ruta
del documento. Si un elemento tiene un atributo ID, paramos de construir el
XPointer y consideramos que el comienzo de ese XPointer es el elemento que
tiene ese valor de atributo ID. Por ejemplo, si miras el código <acronym
title="HyperText Markup Language" xml:lang="en" lang="en">HTML</acronym> de
este documento, notarás que esta sección (Problemas con anotaciones...) está
incluida dentro de un elemento DIV que tiene un atributo ID con el valor
"Issues". Aquí hay un extracto del código fuente:</p>
<pre>  &lt;div id="Issues"&gt;
  &lt;h3&gt;Problemas con ....&lt;/h3&gt;
  &lt;p&gt;Si se está usando...&lt;/p&gt;
  &lt;p&gt;Amaya utiliza &lt;strong&gt;XPointer&lt;/strong&gt;...&lt;/p&gt;
  ...
  &lt;/div&gt;</pre>

<p>Y aquí el XPointer que apunta al segundo párrafo:</p>

<p style="text-align: center"><code>xpointer(id("Issues")/p[2])</code></p>

<p>El XPointer de arriba apunta al segundo elemento p, desde el elemento
padre que tiene un atributo ID con el valor "Issues". (Para seleccionar un
párrafo completo, ponemos el cursor en el párrafo y presionamos la tecla
escape [Unix] o F2 [Windows]). Nótese que el uso del atributo ID permite al
autor del documento mover la selección completa, donde quiera en el
documento, sin necesidad de actualizar el XPointer. El XPointer no depende
del elemento que precede ese particular elemento DIV.</p>

<p>Decimos que una anotación se queda <b>huérfana</b> cuando ya no puede ser
añadida a un documento, esto es, cuando el XPointer no determina nunca más
ningún elemento en la estructura. Esto pasa cuando una estructura de
documento se modifica. Por ejemplo, el XPointer de arriba no podría
determinar nunca más si eliminamos el atributo ID "Issues". Amaya te
advertirá si detecta cualquier anotación huérfana mientras descarga una serie
de anotaciones desde un servidor de anotaciones. Todas las anotaciones
huérfanas son visibles desde la vista Enlaces y están asociadas con un icono
que muestra un signo de interrogación superpuesto al lápiz de anotaciones
<img src="../images/annotorp.png" alt="icono de anotaciones huérfanas" />.</p>

<p>Decimos que una anotación es <b>corrupta</b> cuando apunta a una pieza de
información errónea. Volviendo a nuestro ejemplo, podemos crear una anotación
corrupta si intercambiamos el primer y segundo párrafo. El XPointer
continuará apuntando al segundo párrafo, a pesar de que nos gustaría que
apuntara al primer párrafo. Este problema se hace incluso más común cuando
anotas una porción de texto que puede cambiar. En la primera versión, Amaya
no advierte al usuario si una anotación está corrupta. Una futura versión
puede avisar al usuario de la posibilidad de que una anotación pueda llegar a
ser corrupta.</p>

<p>¿Cómo puedes protegerte a ti mismo?</p>

<p>Si eres el autor de un documento, intenta usar el atributo ID en lugares
estratégicos, por ejemplo, en los elementos DIV. Amaya te permite asociar
automáticamente o eliminar un atributo ID a/desde una serie de elementos por
medio del comando de menú Especial/Crear/Suprimir atributos ID. En el ejemplo
de arriba, nosotros podríamos haber evitado el problema de las anotaciones
corruptas si hubiésemos añadido un atributo ID al segundo párrafo:</p>
<pre>  &lt;p id="Amaya"&gt;Amaya utiliza...&lt;/p&gt;</pre>

<p>Un XPointer que apunta a este párrafo es:</p>

<p style="text-align: center"><code>xpointer(id("Amaya"))</code></p>

<p>De esta manera, el XPointer apuntará al mismo párrafo, independientemente
de su posición en la estructura del documento.</p>
</div>

<p><a href="MakeBook.html"><img alt="previa" src="../images/left.gif" /></a>
<a href="Manual.html"><img alt="arriba" src="../images/up.gif" /></a> <a
href="Configure.html"><img alt="siguiente" src="../images/right.gif"
/></a></p>
<hr />
</body>
</html>
