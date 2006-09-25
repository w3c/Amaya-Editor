<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="content-type" content="text/html; charset=iso-8859-1" />
  <title>Menú Preferencias</title>
  <meta name="GENERATOR" content="amaya 9.52, see http://www.w3.org/Amaya/" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" />
         <img alt="Amaya" src="../../images/amaya.gif" />
      </td>
      <td><p align="right"><a
        href="about_configuration_directory_and_file_conventions.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" />
        </a> <a href="../Configure.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" />
        </a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Menú Preferencias</h1>

<p>Las cajas de diálogo se encuentran en el menú <strong>Editar &gt;
Preferencias</strong>. Hay diez cajas de diálogos:
<strong>Generales</strong>, <strong>Navegación</strong>,
<strong>Publicación</strong>, <strong>Caché</strong>, <strong>Proxy</strong>,
<strong>Colores</strong>, <strong>Geometría de la ventana</strong>, y
<strong>Anotaciones</strong> y <strong>Cooperación</strong>.</p>

<p>Cada una de estas cajas de diálogo contienen tres botones:</p>

<table>
  <tbody>
    <tr>
      <td><p class="TableHead"><b>Botón</b></p>
      </td>
      <td><p class="TableHead"><b>Efecto</b></p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Aplicar</p>
      </td>
      <td><p class="TableText">Confirma y almacena las opciones que se
        muestran en la caja.</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Por omisión</p>
      </td>
      <td><p class="TableText">Restaura, sin guardar, los valores por
        omisión. Para guardar estas opciones, utiliza a continuación el botón
        <strong>Aplicar</strong> .</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Aceptar</p>
      </td>
      <td><p class="TableText">Cierra la caja de diálogo.</p>
      </td>
    </tr>
  </tbody>
</table>

<h2>Preferencias generales</h2>

<p>Para abrir la caja de diálogo <strong>Preferencias generales</strong>,
elige el menú <strong>Editar &gt; Preferencias &gt; Generales</strong>.</p>

<h3>Directorios específicos</h3>

<p>La caja de diálogo <strong>Preferencias generales</strong> muestra el
directorio de usuario, el directorio temporal y la página web por omisión.
Estas opciones se explican a continuación.</p>

<p><strong>Directorio de usuario de Amaya.</strong> Sólo en Windows.
Especifica el directorio en el que se almacenan las preferencias del
usuario.</p>

<p class="Note"><strong>Nota:</strong> La ubicación de este directorio no se
puede modificar.</p>

<p><strong><br />
Directorio temporal.</strong> Sólo en Windows. Especifica el directorio en el
que Amaya almacena los archivos temporales, por ejemplo, al imprimir un
documento. Si no se especifica, el directorio por omisión es
<code>c:\temp</code>.</p>

<p class="Note"><strong>Nota:</strong> Al cambiar el directorio, el
directorio actual no se borra. Debes borrarlo manualmente. Si la
<strong>caché</strong> se encontraba en el directorio temporal anterior, se
creará una nueva caché en el nuevo directorio.</p>

<p><strong><br />
Página de inicio.</strong> Especifica el URI que Amaya cargará al abrir el
programa o cuando el usuario haga clic en el botón Inicio. Por ejemplo, la
página de inicio podría ser: <code><a
href="http://www.w3.org/">http://www.w3.org</a></code></p>

<p class="Note"><strong>Nota:</strong> Debes escribir la URI completa.</p>

<h3>Casillas de verificación</h3>

<p>Las opciones comentadas a continuación pueden activarse o desactivarse
haciendo clic en la casilla de verificación que se encuentra a la izquierda
de cada opción. Una casilla marcada indica que la opción está activada.</p>

<p><strong>Activar pegado línea a línea</strong>. Esta opción afecta al
pegado desde un aplicación externa. Cuando está activada, Amaya mantiene la
separación en líneas de la aplicación original. Si está desactivada, los
caracteres de <code>salto de línea</code> se sustituyen por espacios.</p>

<p><strong>Generar copias de seguridad.</strong> Si esta opción está marcada,
Amaya genera una copia de seguridad de cada documento editado, tras un número
determinado de cambios. Esta opción se aplica nada más marcar la casilla.</p>

<p><strong>Guardar geometría al salir.</strong> Controla si, al salir de
Amaya, la posición y el tamaño de la ventana se guardan y aplican en la
siguiente sesión.</p>

<p><strong>Mostrar barra de botones.</strong> Controla si se muestra la barra
de botones en las vistas principales del documento.</p>

<p><strong>Mostrar dirección.</strong> Controla si se muestra la barra de
dirección en las vistas principales del documento.</p>

<p><strong>Mostrar destinos.</strong> Controla si se muestran los destinos de
enlaces en las vistas principales del documento.</p>

<p><strong>Atajos de teclado.</strong> Especifica la tecla (<kbd>Alt</kbd> o
<kbd>Control</kbd>) utilizada en las <a
href="..\Browsing.html.es#Access">teclas de acceso</a>, o especifica que se
ignoren las teclas de acceso definidas en el documento</p>

<h2>Opciones exclusivas de Unix</h2>

<p>Hay dos opciones de la caja de diálogo <strong>Preferencias
generales</strong> que sólo están disponibles en Unix: <strong>Tamaño de
fuente en menús</strong> y <strong>Retardo de doble clic</strong>. Estas
opciones se explican a continuación.</p>

<p><strong>Tamaño de fuente en menús</strong>. Especifica el tamaño de fuente
en todos los menús de Amaya. Esta opción se tiene en cuenta al iniciar
Amaya.</p>

<p><strong>Retardo de doble clic</strong>. Esta opción te permite controlar
el retardo entre dos clics para que se consideren un doble clic. Esta opción
se tiene en cuenta al iniciar Amaya.</p>

<h2>Opciones de fuente e idioma</h2>

<p>Esta sección de la caja de diálogo <strong>Preferencias generales</strong>
te permite elegir el zoom de fuente y el idioma de diálogo utilizado por
Amaya. Estas opciones se explican a continuación.</p>

<p><strong>Zoom de fuente</strong>. Puedes aumentar o reducir el zoom en las
ventanas. Cada documento se mostrará ampliado o reducido con respecto a su
tamaño real de acuerdo con el valor de zoom establecido. Esta opción te
permite cambiar el zoom por omisión de todas las ventanas. Este cambio afecta
únicamente al tamaño de fuente, pero no al tamaño de las imágenes y se
refleja inmediatamente en todas las ventanas abiertas.</p>

<p><strong>Idioma de diálogo.</strong> Especifica el idioma en todos los
menús y mensajes de diálogo. El idioma por omisión es el inglés americano
(valor: <code>en_US</code>). Actualmente, hay varios idiomas más disponibles:
francés (<code>fr</code>), alemán (<code>de</code>), italiano
(<code>it</code>), castellano (<code>es</code>), portugués (<code>pt</code>),
finés (<code>fi</code>) y turco (<code>tr</code>). Al inicar Amaya, se cargan
los archivos de diálogo correspondiente al idioma de diálogo especificado:
<code>en-</code>, <code>fr-</code>, <code>de-</code>, <code>it-</code>,
<code>es-</code>, <code>pt-</code>, <code>fi-</code> o <code>tr-</code>.
Estos archivos de diálogos se encuentran en la carpeta
<strong>Amaya/config</strong>.</p>

<p>En Thot, un documento está especificado por un esquema. En Amaya, los
mensajes de diálogo que se muestran al analizar un documento o al guardarlos
según un esquema específico se encuentran en el archivo
<strong>Amaya/amaya/HTML.</strong><em><strong>code</strong></em>, en el que
el sufijo <em>code</em> es la abreviatura del idioma (por ejemplo,
<code>en</code> o <code>fr</code>). Este archivo tiene 4 secciones:</p>
<ul>
  <li><strong>extension:</strong> No utilizada por Amaya</li>
  <li><strong>presentation:</strong> No utilizada por Amaya</li>
  <li><strong>export</strong>: Identifica el texto mostrado en cada esquema
    de traducción (comando <strong>Guardar como</strong>)</li>
  <li><strong>translation</strong>: Identifica el texto mostrado en cada
    elemento y atributo en la estructura HTML de Thot.</li>
</ul>

<p>Es posible adaptar Amaya a nuevos idiomas ISO-Latin-1 escribiendo los
archivos de diálogo correspondientes. Estos archivos de diálogo deben
ubicarse en la misma carpeta (por ejemplo, Amaya/config) y deben utilizar el
código ISO 639 correspondiente como prefijo (<code>it-</code> para italiano,
<code>de-</code> para alemán, etc.). También debes crear el archivo HTML.code
específico.</p>

<h2>Navegación</h2>

<p>La caja de diálogo <strong>Preferencias de navegación</strong> te
permite...</p>

<h3>Casillas de verificación</h3>

<p>Las opciones siguientes se pueden activar o desactivar haciendo clic en la
casilla de verificación situada a la izquierda de cada opción. Una casilla
marcada indica que la opción está activada.</p>

<p><strong>Cargar imágenes.</strong> Controla si las imágenes deben cargarse.
Esta opción se aplica inmediatamente.</p>

<p><strong>Cargar objetos.</strong> Controla si los objetos deben cargarse.
Esta opción se aplica inmediatamente.</p>

<p><strong>Mostrar imágenes de fondo.</strong> Controla la presentación de
las imágenes de fondo. Esta opción se tiene en cuenta al cargar un
documento.</p>

<p><strong>Cargar CSS.</strong> Controla si las hojas de estilo CSS deben
cargarse y aplicarse. Esta opción se aplica inmediatamente.</p>

<p><strong>Doble clic activa enlaces.</strong> Desde la primera versión de
Amaya, para activar un enlace hay que hacer doble clic en él. Eso te permite
elegir entre editar o activar un enlace. Puedes deshabilitar este
comportamiento desactivando esta opción.</p>

<p><strong>Tipo de pantalla actual.</strong> Define el tipo de pantalla
actual. Esta opción tiene efecto cunado una hoja de estilo CSS especifica un
medio determinado. Esta opción se aplica inmediatamente.</p>

<p><strong>Negociación de idioma.</strong> Si un documento existe en
distintos idiomas y tu servidor está configurado para establecer una
negociación de idioma, puedes utilizar esta caja de diálogo para escribir tus
preferencias de idiomas (el primer idioma tiene la máxima prioridad).
Consulta esta <a
href="http://www.w3.org/1999/05/WCAG-RECPressRelease.html">nota de
prensa</a>, disponible en francés, inglés y japonés. Escribiendo los códigos
ISO de estos idiomas (<strong>fr</strong>, <strong>en</strong>, o
<strong>ja</strong>), puedes consultar de forma transparente cualquiera de
estas versiones.</p>

<h2>Publicación</h2>

<p>La caja de diálogo de <strong>Preferencias de publicación</strong> te
permite establecer las preferencias de ETAGS y precondiciones, tanto para
verificar cada comando PUT con un comando GET, como para definir el nombre
por omisión de los URLs que acaban en barra, y permitir redirecciones PUT en
dominios específicos. Estas opciones se explican a continuación.</p>

<p><strong>Juego de caracteres para nuevos documentos.</strong> Establece el
juego de caracteres a utilizar al crear un documento nuevo.</p>

<p><strong>En documentos XHTML usar tipo MIME application/xhtml+xml.</strong>
Esta opción, si está activada, asocia el nuevo tipo MIME oficial XHTML a
todos los documentos XTHML nuevos que se publican en la Web. Esta opción está
deshabilita por omisión porque no todos los servidores pueden gestionar este
tipo MIME en el momento de escribir esta página. Fíjate en que esta opción
requiere un cambio en la configuración del servidor, si no está almacenando
en algún sitio los metadatos asociados con cada documento. Esta opción no
modifica el tipo MIME de los documentos ya existentes: el mismo tipo MIME que
se recibe se utiliza al guardar el documento, salvo si <strong>Guardas
como</strong>, evidentemente.</p>

<p><strong>Usar ETAGS y precondiciones.</strong> HTTP/1.1 introduce nuevos
encabezados que resuelven el problema de las <a
href="http://www.w3.org/WinCom/NoMoreLostUpdates.html">actualizaciones
perdidas</a> al publicar. Con esta opción, puedes detectar conflicto sentre
dos usuarios que están editando el mismo documento.</p>

<p><strong>Verificar cada PUT con un GET.</strong> Si no confías en tu
servidor, esta opción hace que Amaya descargue cada documento nada más ser
publicado y que lo compare con la copia almacenada en tu ordenador. Si hay
diferencias entre ambos documentos, Amayas te avisará.</p>

<p><strong>Exportar CR/LF</strong> (sólo en Windows). Esta opción, si está
seleccionada, coloca un retorno de carro delante de cada salto de línea.</p>

<p><strong>El nombre por omisión de URL termina en '/'.</strong> Los
servidores HTTP suelen asociar los URLs que acaban en '/' con un nombre
determinado, por ejemplo, Overview.html. Esta opción te permite especificar
el nombre por omisión que Amaya debe utilizar al publicar tal URL. Pregunta
al webmaster cuál es el nombre por omisión en tu servidor.</p>

<p><strong>Permitir redirecciones PUT autómaticas en estos dominios.</strong>
De acuerdo con la especificación HTTP/1.1 , las operaciones PUT no se deben
redirigir automáticamente a menos que pueda ser confirmado por el usuario,
puesto que esto puede cambiar las condiciones bajo las que se hizo la
petición. Puedes evitar que Amaya muestre un aviso de redirección PUT
escribiendo el nombre de dominio del servidor en el que estás publicando.
Puedes especificar uno o más dominios, separándolos por un espacio, por
ejemplo, <code>www.w3.org groucho.marx.edu</code>.</p>

<h2>Caché</h2>

<p>Amaya incluye una caché mediante libwww. La caché está activada por
omisión y se almacena en la carpeta AmayaTemp/libwww-cache. Este menú también
contiene un botón <strong>Vaciar caché</strong> para borrar la carpeta
caché.</p>

<p><strong>Activar caché.</strong> Activa o desactiva la caché.</p>

<p><strong>Documentos protegidosen la Caché.</strong> Por omisión, los
documentos protegidos por contraseña se almacenan en la caché. Utiliza esta
opción para evitarlo.</p>

<p><strong>Modo desconectado.</strong> Siempre recupera los documentos de la
caché.</p>

<p><strong>Ignorar Expires: cabecera.</strong> Las entradas de caché no
expiran.</p>

<p><strong>Directorio Caché.</strong> Identifica el directorio padre del
directorio libwww-cache. Al cambiar este directorio no se borra el contenido
del directorio anterior. Debes eliminarlo tú mismo.</p>

<p class="Note"><strong>Nota:</strong> Los mecanismos de protección de
archivos impiden el uso de directorios NFS (Network File System) como
directorio caché, ya que NFS permite el uso compartido por múltiples
procesadores o usuarios. Utiliza un directorio local para la caché.</p>

<p><strong>Tamaño de caché.</strong> Indica el tamaño, en megabytes, de la
caché.</p>

<p><strong>Tamaño máximo de entrada de caché.</strong> Indica el tamaño
máximo de una entrada de caché.</p>

<h2>Proxy</h2>

<p>En esta caja de diálogo puedes especificar un proxy y un conjunto de
dominios en el que el proxy debe o no utilizarse.</p>

<p><strong>Proxy HTTP.</strong> Identifica el proxy a utilizar en las
peticiones HTTP. Debes especificar el nombre completo del servidor. Si el
servidor está funcionando en un puerto distinto al puerto 80, debes añadir el
carácter ":" al nombre del proxy, seguido del número de puerto. La tabla
siguiente muestra dos ejemplos.</p>

<table border="1">
  <tbody>
    <tr>
      <th><p class="TableHead">Entrada HTTP Proxy</p>
      </th>
      <th><p class="TableHead">Resultado</p>
      </th>
    </tr>
    <tr>
      <td><p>groucho.marx.edu</p>
      </td>
      <td><p>Declara un proxy llamado groucho.marx.edu que funciona el el
        puerto 80.</p>
      </td>
    </tr>
    <tr>
      <td><p>groucho.marx.edu:1234</p>
      </td>
      <td><p>Declara un proxy llamado groucho.marx.edu que funciona en el
        puerto 1234.</p>
      </td>
    </tr>
  </tbody>
</table>

<p><strong>Lista de dominios proxy.</strong> Lista de dominios separados por
espacios con los que quieres utilizar el proxy. Utiliza los botones radio
para especificar si el proxy debe o no debe utilizarse con esta lista de
dominios.Por ejemplo:</p>
<pre style="margin-left:.5in"><code>harpo.marx.edu zeppo.marx.edu chico.marx.edu</code></pre>
<pre style="margin-left:.5in"><code>              ^--------------^----&gt; entradas separadas por espacios</code></pre>

<h2>Color</h2>

<p>Esta caja de diálogo te permite definir los colores de texto y fondo de
los documentos. También te permite definir los colores utilizados por Amaya
para mostrar la selección actual.</p>
<ul>
  <li><strong>Color del cursor</strong> es el color de fondo de la selección
    cuando ésta se limita al punto de inserción.</li>
  <li><strong>Color de selección</strong> es el color de fondo de la
    selección.</li>
</ul>

<p>La versión Unix también proporciona entradas para modificar los colores
del menú. Para cambiar los colores del menú en Windows, debes utilizar el
control estándar de configuración de pantalla que se encuentra en el Panel de
Control.</p>

<p>Los siguientes colores pueden seleccionarse por su nombre, código
hexadecimal o RGBt:</p>
<ul>
  <li>Los nombres válidos de colores son: Aqua, Black, Blue, Fuchsia, Gray,
    Green, Lime, Maroon, Navy, Olive, Purple, Red, Silver, Teal, Yellow y
    White.</li>
  <li>El formato válido de un código hexadecimal es #FF00FF, por ejemplo.
    <p>El formato válido de un código RGB es rgb(255, 0, 255), por
    ejemplo.</p>
  </li>
</ul>

<p>Si escribes un color inválido, los colores por omisión se utilizan en su
lugar. En Windows también puedes elegir colores con la paleta de color.</p>

<p class="Note"><strong>Nota:</strong> Los cambios de colroes se harán
efectivos al abrir una nueva ventana de documento o al recargar un
documento.</p>

<p>Los colores de los enlaces, los enlaces visitados y los enlaces activos se
definen en la hoja de estilo Amaya.css.</p>

<h2>Geometría de ventana</h2>

<p>La caja de diálogo te permite guardar la posición y tamaño actual de las
distintas ventanas de vistas de un documento (formateada, estructura,
alternativa, enlaces y tabla de contenidos) o restaurar los valores por
omisión.</p>

<p>Los valores que se guardan son los de la ventana de la vista desde la que
has abierto la caja de diálogo. Estos cambios se aplican al abrir nuevas
ventanas de documentos.</p>
</div>
</body>
</html>
