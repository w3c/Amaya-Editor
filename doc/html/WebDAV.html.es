<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
      "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
  <title>Funciones WebDAV en Amaya</title>
  <meta name="generator" content="amaya 8.2, see http://www.w3.org/Amaya/">
  <link href="style.css" rel="stylesheet" type="text/css">
</head>

<body>

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../images/w3c_home"> <img alt="Amaya"
        src="../images/amaya.gif"></td>
      <td><p align="right"><a href="Publishing.html.es" accesskey="p"><img
        alt="previous" src="../images/left.gif"></a> <a href="Manual.html.es"
        accesskey="t"><img alt="superior" src="../images/up.gif"></a> <a
        href="Printing.html.es" accesskey="n"><img alt="siguiente" 
        src="../images/right.gif"></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Funciones WebDAV en Amaya</h1>

<h2>Acerca de WebDAV</h2>

<p><a href="http://www.webdav.org/">WebDAV (WWW Distributed Authoring and
Versioning)</a> es un conjunto de extensiones al protocolo HTTP que permite a
los usuarios colaborar en la edición y gestión de recursos web en servidores
remotos. Su objetivo es extender el protocolo HTTP para permitir una
arquitectura abierta en el nivel del protocolo, para desarrollar nuevas
herramientas de creación  distribuida en la web, haciendo hincapié en la
creación de páginas web en colaboración (ver <a
href="http://andrew2.andrew.cmu.edu/rfc/rfc2518.html">RFC 2518</a>). WebDAV
define operaciones sobre las propiedades, las colecciones, los espacios de
nombres y la potección de sobreescritura (mecanismo de bloqueo) y, para estas
operaciones, define nuevos métodos, encabezados, organismos de entidad de
petición y respuesta. Sin embargo, las características de tratamiento de
versiones incluidas en la propuesta original se han trasladado al grupo de
trabajo de la IEFT  <a href="http://www.webdav.org/">Delta-V</a>, que
pretende extender  WebDAV y HTTP/1.1 en estas características (ver el
artículo de E. J. Whitehead "<a
href="http://www.webtechniques.com/  archives/1999/10/whitehead/">El futuro
del desarrollo de software distribuido en Internet</a>").</p>

<p>El <strong>mecanismo de bloqueo</strong> definido en WebDAV ha sido
diseñado para prevenir la sobreescritura de recursos (es decir, prevenir el
problema de la actualización perdida), empleando las operaciones de bloqueo y
desbloqueo. Un bloqueo controla el acceso de escritura a un recurso limitando
las operaciones de escritura HTTP, como  PUT, POST y DELETE. Esto significa
que solamente alguien que conozca el bloqueo podrá ejectuar estas
operaciones. El protocolo WebDAV también define dos <strong>ámbitos de
bloqueo</strong>: <em>bloqueo exclusivo y bloqueo compartido</em>. Un bloqueo
exclusivo garantiza que una persona posee el bloqueo y que nadie más podrá
bloquear el recurso (esto no afecta a la lectura del recurso). Un bloqueo
exclusivo permite a un grupo de usuarios crear sus bloqueos sobre un recurso,
pero deben confiar unos en otros para prevenir problemas de acceso. Además,
cada bloqueo tiene una duración límite y el usuario no puede olvidar que los
bloqueos pueden desaparecer en el servidor debido a excepciones en el
servidor.</p>

<p>Puedes encontrar más información sobre  WebDAV en <a
href="http://andrew2.andrew.cmu.edu/rfc/rfc2518.html">RFC 2518</a> y también
en <a href="http://www.webdav.org/">WebDAV.org</a>.</p>

<h2>Soporte de WebDAV en Amaya</h2>

<p>Amaya tiene un soporte de WebDAV opcional y limitado. Este soporte incluye
las operaciones siguientes:  bloqueo/desbloqueo de un recurso compilante
WebDAV, ver las propiedades WebDAV de un recurso y capacidades de
descubrimiento de bloqueos. Además, este soporte incluye algunas funciones de
reconocimiento, que pueden informar automáticamente al usuario sobre los
recursos bloqueados.</p>

<h3>¿Cómo utilizarlo?</h3>

<p>El soporte de WebDAV presente en Amaya pretende ayudar a pequeños grupos
de usuarios a editar en colaboración sus páginas Web. Para mostrar cómo
funciona, he aquí un ejemplo; supón que tienes un servidor WebDAV (en
realidad, <strong>necesitas</strong> un servidor Web que soporte WebDAV para
trabajar con él) que se llama  <code>servidordav.miempresa.com</code>, y que
tú y tus compañeros queréis editar en colaboración algunos documentos que
están en la carpeta WebDAV <code>/shared/</code> (las carpetas WebDAV se
denominan colecciones).</p>

<p>Entonces, imagina que necesitas modificar el documento
<code>http://servidordav.miempresa.com/shared/Doc1.html</code>. Para ello,
abres el documento en tu editor Amaya y a continuación
<strong>bloqueas</strong> el documento (<em>Bloquear recurso</em> en el <a
href="#L654">menú Cooperación</a> ver <a href="#L654">Figura 1</a>). Una vez
has bloqueado el documento, si uno de tus compañeros intenta guardar algún
cambio en el documento, será avisado de tu bloqueo y las actualizaciones no
se perderán automáticamente. Mientras el documento está bloqueado puedes
modificarlo, y cuando hayas terminado tu trabajo, al
<strong>desbloquear</strong> el documento, permitirás que otros compañeros
puedan bloquearlo y realizar sus modificaciones.</p>

<p><a id="L654"><img alt="Cooperation Menu"
src="../images/CooperationMenu.gif"></a></p>

<p>Por otro lado, si quieres saber si algún compañero ha bloqueado una página
Web, puedes utilizar la opción <em>Ver propiedades del recurso</em> en el <a
href="#L654">menú Cooperación</a> para ver todas las propiedades de la página
o puedes configurar Amaya para que te informe automáticamente cuando se
bloquee la página. para ello, tendrás que abrir la caja de diálogo
<em>Cooperación&gt;Preferencias</em>, marcar la opción de <em>Conocimiento
general</em> y escribir la dirección de tu carpeta compartida en el campo
<em>Lista de recursos WebDAV de usuario</em>
(<code>http://servidordav.miemmpresa.com/shared/</code>, como muestra la
Figura 2). Una vez lo hayas hecho, al abrir una página bloqueada por un
compañero de la carpeta
<code>http://servidordev.miempresa.com/shared/</code>, Amaya te informará.</p>

<p><img alt="Cooperation - Preferences "
src="../images/CooperationPreferences.gif"></p>

<h3>Opciones WebDAV en Amaya</h3>

<p>Estas opciones se encuentran en el menú Cooperación &gt; Preferencias. Se
indican los valores por omisión:</p>
<dl>
  <dt><code>DAV_DEPTH= [ infinity | <strong>0</strong> ]</code></dt>
    <dd>Profundidad del bloqueo. Si la profundidad es infinita, el bloqueo de
      una colección (carpeta) afectará a todos sus componentes. En caso
      contrario el bloqueo afectará únicamente a la colección, impidiendo al
      usuario que no conozca el bloqueo la creación o eliminación de
      componentes de la colección.</dd>
  <dt><code>DAV_TIMEOUT= [ <strong>Infinite</strong> | Second-XXXX
  ]</code></dt>
    <dd>Duración del bloqueo. El valor "infinita" significa que el bloqueo no
      termina nunca y "Second-xxxx" intentará obtener un bloqueo durante XXXX
      segundos.</dd>
  <dt><code>DAV_LOCK_SCOPE= [ <strong>exclusive</strong> | shared
  ]</code></dt>
    <dd>Ámbito del bloqueo.</dd>
  <dt><code>DAV_USER_URL= [ URL ]</code></dt>
    <dd>URL que identifica al usuario. Ejemplo: mailto:yomismo@miservidor.com
      o http://miservidor.com/mihomepage.html</dd>
  <dt><code>DAV_AWARENESS= [ yes
  |</code><code><strong>no</strong></code><code>]</code></dt>
    <dd>Indica si el usuario desea información básica sobre los recursos
      bloqueados.</dd>
  <dt><code>DAV_AWARENESS_ONEXIT=[ yes
  |</code><em><code></code></em><code><strong>no</strong></code><code>]</code></dt>
    <dd>Indica si el usuario quiere recibir información sobre sus propios
      bloqueos al abandonar un recurso.</dd>
  <dt><code>DAV_URLS= [ URL URL URL ... ]</code></dt>
    <dd>Lista de URLs de recursos empleados por el usuario como recursos
      WebDAV</dd>
</dl>

<h2>¿Cómo compilar Amaya con funciones WebDAV?</h2>

<p>WebDAV es una característica opcional en Amaya. Para compilarla y poderla
utilizar, necesitarás:</p>
<ul>
  <li>Libwww con soporte  WebDAV</li>
  <li>opciones de configuración de Amaya</li>
  <li>un servidor WebDAV para poder utilizar estas funciones... <a
    href="http://www.w3.org/Jigsaw/">Jigsaw</a> y <a
    href="http://httpd.apache.org/">Apache</a> son algunos de los servidores
    web que soportan WebDAV.</li>
</ul>

<p>Una vez has comprobado el <em>cvs</em> de <a
href="http://www.w3.org/Library/">Libwww con soporte WebDAV</a>  y has
obtenido el <a href="http://www.w3.org/Amaya/User/cvs.html">código fuente de
Amaya</a>, debes seguir los pasos siguientes en un sistema Linux/Unix (para
sistema Windows, consulta las páginas de <a
href="http://www.w3.org/Amaya/">Amaya</a> y <a
href="http://www.w3.org/Library/">Libwww</a>):</p>

<h4>En el directorio libwww:</h4>
<ul>
  <li>libtoolize -c -f</li>
  <li>perl config/winConfigure.pl</li>
  <li>aclocal; autoheader; automake; autoconf</li>
</ul>

<h4>En el directorio Amaya:</h4>
<ul>
  <li>autoconf</li>
  <li>mkdir LINUX-ELF (or GTK, or obj)</li>
  <li>cd LINUX-ELF</li>
  <li>../configure <strong>--with-dav</strong> ... (and all other options
    that you need)</li>
  <li>make</li>
</ul>
</div>
<hr>

<p><em>Escrito por <a href="mailto:mkirsch@terra.com.br">Manuele Kirsch
Pinheiro</a>, 13 de junio de 2002.</em></p>
</body>
</html>
