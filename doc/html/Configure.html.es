<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>Configurando Amaya</title>
  <meta name="GENERATOR" content="amaya 5.2" />
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
</head>

<body xml:lang="es" lang="es">

<table border="0" summary="toc">
  <tbody>
    <tr>
      <td><p><img alt="W3C" src="../images/w3c_home" /> <img alt="Amaya"
        src="../images/amaya.gif" /></p>
      </td>
      <td><p><a href="Annotations.html" accesskey="p"><img alt="previous"
        src="../images/left.gif" /></a> <a href="Manual.html"
        accesskey="t"><img alt="top" src="../images/up.gif" /></a> <a
        href="ShortCuts.html" accesskey="n"><img alt="next"
        src="../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="Configuring">
<h2>Configurando Amaya</h2>

<p><strong>Amaya</strong> utiliza diferentes ficheros de configuración para
dar valores iniciales a parámetros globales, diálogos, atajos, tamaño y
posición de ventanas, hojas de estilo del usuario, etc. Se pueden cambiar la
mayoría de los parámetros de configuración por medio de menús de preferencias
específicos. Este documento reseña el directorio de configuración y archivo
<a href="#Conventions">convenciones</a>, dando una panorámica de los
diferentes <a href="#preferences">menús de preferencias</a>, y da información
específica sobre cómo configurar los <a
href="ShortCuts.html#shortcuts">atajos de teclado</a>.</p>

<h3><a name="Conventions">Convenciones</a></h3>

<h4><a name="L979">El directorio <strong>Amaya/Config</strong></a></h4>

<p>Este es el directorio en el que están almacenados la mayoría de los
valores por omisión y todos los mensajes de diálogo. El directorio padre es
en el que fue instalado Amaya.</p>

<h4><a name="AmayaHome">The AmayaHome Directory</a></h4>

<p>Amaya almacena las preferencias de usuario así como otros ficheros de
configuración de usuario en un directorio al que llamamos
<strong>AmayaHome</strong>. La siguiente tabla muestra dónde está guardado
este directorio en diferentes arquitecturas:</p>

<table border="1" summary="directory structure">
  <thead>
    <tr>
      <td><strong>arquitectura</strong></td>
      <td><strong>directorio AmayaHome</strong></td>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Unix</td>
      <td>$HOME/.amaya</td>
    </tr>
    <tr>
      <td>Windows 95/ 

        <p>Windows 98</p>
      </td>
      <td>AMAYA-INSTALL-DIR\users\username 

        <p>o, si el usuario no se ha registrado en Windows,</p>

        <p>AMAYA-INSTALL-DIR\users\default</p>
      </td>
    </tr>
    <tr>
      <td>Windows NT</td>
      <td>c:\winnt\profiles\username\amaya</td>
    </tr>
  </tbody>
</table>

<p>El directorio <strong>AmayaHome</strong> se mantiene incluso cuando se
instalan nuevas versiones de Amaya y puede contener los siguientes
ficheros:</p>
<ul>
  <li><strong>thot.rc</strong> : las preferencias de usuario, cambiado a
    través del menú preferencias.</li>
  <li><a name="keyboard"><strong>amaya.keyboard</strong></a> (Unix) o
    <strong>amaya.kb</strong> (Windows) : este fichero define los <a
    href="ShortCuts.html#shortcuts">atajos de teclado en Amaya</a>.</li>
  <li><strong>amaya.css</strong> : contiene las hojas de estilo del
  usuario.</li>
  <li><strong>dictionary.DCT</strong> : Amaya proporciona un <a
    href="SpellChecking.html#Spell">revisor gramatical multilingüe</a>. El
    revisor gramatical trabaja por defecto con dos diccionarios: el
    diccionario Inglés y el diccionario Francés (estos dos diccionarios
    pueden descargarse desde <a
    href="http://www.w3.org/Amaya/User/BinDist.html">W3C servers</a>). Cuando
    el usuario quiere incluir una nueva palabra, el revisor crea o actualiza
    el "diccionario personal de usuario".</li>
</ul>

<p><strong>Nota:</strong> los ficheros
<strong>amaya.keyboard/amaya.kb</strong> y <strong>amaya.css</strong> son
descargados y lanzados al tiempo. Si los cambias, necesitarás salir y volver
a iniciar Amaya para que los tome en cuenta.</p>

<h4><a name="AmayaTemp">El directorio AmayaTemp</a></h4>

<p>Amaya almacena todos los archivos temporales que se han creado durante su
ejecución en un directorio al que llamamos <strong>AmayaTemp</strong>. La
siguiente tabla muestra dónde está almacenado este directorio en las diversas
arquitecturas:</p>

<table border="1" summary="directory structure">
  <thead>
    <tr>
      <td><strong>arquitectura</strong></td>
      <td><strong>directorio AmayaTemp</strong></td>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Unix</td>
      <td>$HOME/.amaya</td>
    </tr>
    <tr>
      <td>Windows</td>
      <td>c:\temp\amaya (valor por omisión)</td>
    </tr>
  </tbody>
</table>

<p>Amaya crea ficheros en este directorio mientras navega por documentos
remotos o imprime un documento. El cache se almacena en este directorio. El
usuario puede cambiar tanto el AmayaTemp como el directorio para el cache por
medio de los menús <b>PreferenciasGenerales</b> y <b>Cache</b>.</p>

<p>Amaya intenta siempre eliminar todos los ficheros temporales que ha
creado. Sin embargo, si Amaya falla los ficheros temporales no serán
borrados. Esto no tendrá impacto en las subsecuentes ejecuciones de Amaya.</p>

<h3><a name="preferences">Menús de preferencias</a></h3>

<p>Estos menús se encuentran en el menú <b>Especial/Preferencias</b>. Hay
disponibles nueve menús diferentes de preferencias:: <a
href="#General">General</a>, <a href="#Publishing">Publicar</a>, <a
href="#Cache">Cache</a>, <a href="#Specifying1">Proxy</a>, <a
href="#color">Colores</a>, <a href="#HTML">Geometría de la ventana</a>, <a
href="#lang">Negociación de idioma</a>, <a href="#Profiles">Perfiles</a>, y
<a href="#Templates">plantillas</a>. En cada uno de estos menús hay 3 botones
de acción comunes:</p>

<table border="0" cellspacing="0" cellpadding="5"
summary="action button chart">
  <tbody>
    <tr>
      <th>Aplicar</th>
      <td>Este botón valida y almacena la opción mostrada en el menú.</td>
    </tr>
    <tr>
      <th>Por omisión</th>
      <td>Este botón restaura, pero no salva, los valores por omisión.
        Utiliza el botón aplicar para validar las opciones.</td>
    </tr>
    <tr>
      <th>Hecho</th>
      <td>Este botón despide el menú.</td>
    </tr>
  </tbody>
</table>

<h4><a name="General">Preferencias generales</a></h4>

<p style="text-align: center"><img src="../images/GeneralMenu-es.gif"
alt="General Preferences Menu" /></p>
<dl>
  <dt></dt>
  <dt><strong>Directorio del usuario en Amaya</strong> (Sólo en la plataforma
  Windows)</dt>
    <dd>Especifica el directorio en el que se almacenan las preferencias de
      usuario.<br />
      <strong>Nota:</strong> el usuario no puede cambiar la localización de
      este directorio.<br />
      <br />
    </dd>
  <dt><strong>Directorio temporal de Amaya</strong> (Sólo en plataformas
  Windows)</dt>
    <dd>Especifica el directorio en el que Amaya almacena sus ficheros
      temporales, por ejemplo mientras imprime un documento. Si no se
      especifica, el directorio temporal por omisión será
      <code>c:\temp</code>.<br />
      <strong>Nota:</strong> el cambio de este directorio no elemina el
      anterior. Tendrás que eliminar manualmente el directorio viejo. Si el
      <a href="#Cache">cache</a> estaba localizado en el directorio temporal
      previo, se creará un nuevo cache bajo el nuevo directorio.<br />
    </dd>
  <dt><strong>Página principal</strong></dt>
    <dd>Especifica el URI que Amaya cargará cuando se inicie o cuando el
      usuario presione el botón para ir a la página principal. Un ejemplo es
      <code>http://www.w3.org</code>.<br />
      <strong>Nota:</strong> Debe utilizarse un URI completo.<br />
      <br />
    </dd>
  <dt><strong><a name="Enable">Habilitar combinación de
  teclas</a></strong></dt>
    <dd>Habilita o deshabilita el método de entrada opcional de combinación
      de teclas. 
      <p>El método de entrada opcional (Activar combinación de teclas) se
      base en una secuencia de dos teclas comenzando por el "acento" seguido
      de la "letra". Por ejemplo, para obtener una 'á' presiona primero '´' y
      luego la letra 'a'. Para obtener una '´' tienes que presionar dos veces
      '´'.</p>
      <p>Las secuencias de caracteres utilizadas por el método de entrada de
      combinación de teclas estándar y el método de entrada de combinación de
      teclas opcional están resumidas en <a href="Keyboard.html#Keyboard">dos
      tablas diferentes</a>.</p>
      <p></p>
    </dd>
  <dt> <strong>Mostrar imágenes del fondo</strong></dt>
    <dd>Controla si deberán mostrarse las imágenes del fondo. Esta opción se
      hace efectiva sólo cuando se carga un nuevo documento. 
      <p></p>
    </dd>
  <dt><strong>Doble clic activa los enlaces</strong></dt>
    <dd>Desde la primera versión de Amaya los usuarios tienen que hacer doble
      clic en un enlace para activarlo. Esto le permite a uno diferenciar
      entre la edición de un enlace o su activación. Desactivando esta opción
      puedes anular este comportamiento. 
      <p></p>
    </dd>
  <dt><strong>Mostrar destinos</strong></dt>
    <dd>Controla si los destinos de los enlaces deberían ser mostrados.
      Mostrar los destinos puede ser una ayuda cuando se quiere <a
      href="Links.html#L245">crear nuevos enlaces</a>. Esta opción se hace
      efectiva inmediatamente en todas las vistas de formateo de documento. 
      <p></p>
    </dd>
  <dt><strong>Numeración de sección</strong></dt>
    <dd><p>Controla si se activa o no la <a href="Numbering.html">numeración
      de sección</a>. Esta opción se hace efectiva inmediatamente en todas
      las vistas de formateo de documento.</p>
      <p></p>
    </dd>
  <dt><strong>Exportar CR/LF</strong> (sólo en Windows)</dt>
    <dd><p>Especifica si las líneas en ficheros de salida terminan con un
      CR/LF (formato DOS) o un simple LF (formato estándar).</p>
      <p></p>
    </dd>
  <dt><strong>Activar FTP (experimental)</strong></dt>
    <dd>Permite que Amaya siga URLs FTP. Esta capacidad es experimental no ha
      sido aún depurado. Por ejemplo, para URLs FTP que necesitan la
      autentificación del usuario, la información de autentificación no
      siempre se memoriza. También, presionando el botón de parada durante
      una descarga FTP puede "colgar" Amaya. A pesar de estos problemas hemos
      añadido esta opción a Amaya, de manera que otros desarrolladores puedan
      ayudarnos a depurarla más rápidamente. Por omisión esta opción está
      deshabilitada.</dd>
  <dt><strong>Teclas de acceso</strong></dt>
    <dd>Especifica la tecla <b>Alt</b> o <b>Control</b> a ser usada en la
      implantación del documento <a href="Browsing.html#Access">teclas de
      acceso</a> o notifica que las teclas de acceso definidas en el
      documento deben ser ignoradas. 
      <p></p>
    </dd>
  <dt><strong>Menú Tamaño de fuente</strong> (sólo en plataforma Unix)</dt>
    <dd>Especifica el tamaño de fuente para todos los menús de Amaya. Esta
      opción sólo es tomada en cuenta en el momento de iniciar Amaya. 
      <p><br />
      </p>
    </dd>
  <dt><strong>Zoom de fuente</strong></dt>
    <dd>Es posible hacer zoom para ampliar o disminuir la fuente, ventana de
      documento a ventana de documento. Cada documento (texto e imágenes)
      presentado en una ventana ampliada se presentará ampliado o reducido
      respecto a su tamaño real de acuerdo al zoom elegido. <br />
      Esta opción te permite cambiar el tamaño de fuente por defecto en todas
      las ventanas. Este cambio afecta sólo al tamaño de la fuente, pero no a
      las imágenes, y se verá reflejado inmediatamente en todas las ventanas
      de documento abiertas.<br />
      <br />
    </dd>
  <dt><strong>Retraso del doble clic</strong> (sólo plataforma Unix)</dt>
    <dd>Esta opción le permite a uno controlar el retraso por el que dos clic
      del ratón se considerarán un doble clic. Esta opción sólo se toma en
      cuenta en el momento del inicio.<br />
      <br />
    </dd>
  <dt><strong>Idioma de diálogo</strong></dt>
    <dd>Especifica el idioma para todos los menús y cuadros de diálogo de
      Amaya. El idioma por omisión es Inglés Americano (el valor
      <code>en_US</code>). En este momento sólo hay disponibles tres idiomas
      alternativos: Inglés (<code>en</code>), Francés (<code>fr</code>), y
      Alemán (<code>de</code>). When launched, Amaya loads dialogue files
      according to the current dialogue language: en-, fr-, or de- files.
      These dialogue files are located in the <strong>Amaya/config</strong>
      directory. 
      <p>En Thot, un documento se especifica de acuerdo a un esquema. En
      Amaya, el cuadros de diálogo que se presenta cuando se analiza
      gramaticalmente un documento o cuando se salva bajo un esquema
      específico, están localizados en el fichero
      <strong>Amaya/amaya/HTML.<em>code</em></strong>, en el que la extensión
      <em>code</em> es una abreviación del idioma (ej.: <code>en</code> o
      <code>fr</code>). Estos ficheros tienen 4 secciones:</p>
      <ul>
        <li><strong>extension:</strong> no utilizada por Amaya</li>
        <li><strong>presentation:</strong> no utilizada por Amaya</li>
        <li><strong>export</strong> : da el texto presentado para cada
          esquema de traducción (el comando "Guardar como..")</li>
        <li><strong>translation</strong> : da el texto presentado para cada
          elemento y atributo en la estructura Thot HTML.</li>
      </ul>
      <p>Es posible adaptar Amaya a los nuevos lenguajes ISO-Latin-1
      escribiendo los correspondientes ficheros de diálogo. Estos ficheros de
      diálogo tienen que estar localizados en el mismo directorio (ej.:
      <strong>Amaya/config</strong>) y deben usar código correcto ISO 639
      como prefijo (it- para Italiano, de- para Alemán, etc.). También tiene
      que haberse creado el fichero específico HTML.code.</p>
      <p></p>
    </dd>
</dl>

<h4><a name="Publishing">Preferencias de publicación</a></h4>

<p style="text-align: center"><img src="../images/PublishMenu-es.gif"
alt="Publish Preferences Menu" /></p>
<dl>
  <dt><strong>Usar ETAGS y precondiciones</strong></dt>
    <dd>HTTP/1.1 introduces new headers that allow to avoid the <a
      href="http://www.w3.org/WinCom/NoMoreLostUpdates.html">lost updates</a>
      problem when publishing. With this mechanism, you can detect conflicts
      when two users are publishing the same document.</dd>
  <dt><strong>Verify each PUT with a GET</strong></dt>
    <dd>If you don't trust your server, this option tells Amaya to retrieve
      each document as soon as it has been published and compare it with the
      copy stored in your workstation. If the documents differ, Amaya will
      warn you.</dd>
  <dt><strong>Default name for URLs finishing in '/'</strong></dt>
    <dd>An HTTP server usually associate the URLs that finish in '/' with a
      default name, e.g., Overview.html. This option allows specifying the
      default name that Amaya should use when publishing such a URL. Please
      consult with your Webmaster to find out what the default name is for
      your server.</dd>
  <dt><strong>Allow automatic PUT redirections on these domains</strong></dt>
    <dd>According to the HTTP/1.1 specification, PUT operations should not be
      automatically redirected unless it can be confirmed by the user, since
      this might change the conditions under which the request was issued.
      You can avoid having Amaya prompt with a PUT redirection warning by
      entering the name of the server's domain to which you are publishing.
      You may specify one or more domains, each separated with a
      <em>space</em>, e.g., <code>www.w3.org groucho.marx.edu</code>.</dd>
</dl>

<h4><a name="Cache">Cache preferences</a></h4>

<p style="text-align: center"><img src="../images/CacheMenu.gif"
alt="Cache Preferences Menu" /></p>

<p>Amaya provides a cache through libwww. By default, the cache is enabled by
default and is stored in the <strong>AmayaTemp/libwww-cache</strong>
directory. This menu also provides a <strong>Flush Cache</strong> button, for
erasing the cache directory.</p>
<dl>
  <dt><strong>Enable cache</strong></dt>
    <dd>Activates or deactivates the cache</dd>
  <dt><strong>Cache protected documents</strong></dt>
    <dd>By default, documents which are protected by a password are stored in
      the cache. Use this flag to override this behavior.</dd>
  <dt><strong>Disconnected mode</strong></dt>
    <dd>Always retrieves documents from the cache.</dd>
  <dt><strong>Ignore Expires: header</strong></dt>
    <dd>Does not expire cache entries.</dd>
  <dt><strong>Cache directory</strong></dt>
    <dd>Gives the parent directory for the libwww-cache directory. Changing
      this directory does not erase the contents of the previous one. You
      will have to remove it yourself.<br />
      <strong>Note:</strong> File protection mechanisms prevent the use of
      NFS (Network File System) directories for the cache directory as NFS
      allows reuse by multiple processes or users. Use a 'local' directory
      for the cache.</dd>
  <dt><strong>Cache size</strong></dt>
    <dd>Gives the size, in megabytes, of the cache repository.</dd>
  <dt><strong>Cache entry size limit</strong></dt>
    <dd>Gives the maximum size for a cache entry.</dd>
</dl>

<h4><a name="Proxy">Proxy preferences</a></h4>

<p style="text-align: center"><img src="../images/ProxyMenu.gif"
alt="Proxy Preferences Menu" /></p>

<p>You can specify a proxy and a set of domains where the proxy should or
should not be used by means of this menu.</p>
<dl>
  <dt><strong>HTTP proxy</strong></dt>
    <dd>Gives the proxy that should be used for HTTP requests. You must
      specify the full name of the server. If the server is running in a port
      other than 80, then, like in a URL, you need to add a ":" char to the
      proxy name, followed by the port number. For example:: 
      <pre>   groucho.marx.edu
           </pre>
      <p>declares a proxy called groucho.marx.edu running at the default port
      80, and</p>
      <pre>   groucho.marx.edu:1234
           </pre>
      <p>declares a proxy called groucho.marx.edu, running at port 1234</p>
    </dd>
</dl>
<dl>
  <dt><strong>Proxy domain list</strong></dt>
    <dd>Gives a list of space separated domains where you want to use or not
      use your proxy. For example: 
      <pre>harpo.marx.edu zeppo.marx.edu chico.marx.edu
              ^--------------^----&gt; space separated entries
           </pre>
      <p>Use the switch buttons to specify if the proxy should or shouldn't
      be used with this domain list.</p>
    </dd>
</dl>

<h4><a name="color">Color preferences</a></h4>

<p style="text-align: center"><img src="../images/ColorMenu.gif"
alt="Color Preferences Menu" /></p>

<p>This menu allows selection of the default foreground and background colors
for documents. It allows also to control the colors: used by Amaya to display
the current selection</p>
<ul type="square">
  <li><strong>Caret color</strong> gives the foreground color that shows the
    selection when it's limited to an insert point;</li>
  <li><strong>Selection colo</strong> r gives the background that shows the
    current selection when it's extended.</li>
</ul>

<p>The Unix version also provides entries for changing the menu colors. To
change menu colors in Windows, you will need to use the standard Windows
configuration control, found in the Control Panel folder.</p>

<p>All these colors may be selected by name, hex code, or rgb format:</p>
<ul type="square">
  <li>Valid names are: Aqua, Black, Blue, Fuchsia, Gray, Green, Lime, Maroon,
    Navy, Olive, Purple, Red, Silver, Teal, Yellow, and White.</li>
  <li>Valid hexcode format is #FF00FF as an example.</li>
  <li>Valid rgb format is rgb(255, 0, 255) as an example.</li>
</ul>

<p>If you enter an invalid color, the default colors will be used instead.</p>

<p>In Windows, you can also select colors by means of a color palette.</p>

<p><strong>Note:</strong> the color changes will be effective when you open a
new document window or when reloading a document.</p>

<p>Setting or changing colors for links, visited links, and active links is
done through the Amaya.css style sheet.</p>

<h4><a name="HTML">Window geometry preferences</a></h4>

<p style="text-align: center"><img src="../images/GeomMenu.gif"
alt="Geometry Preferences Menu" /></p>

<p>This menu allows you to save the current window position and size size of
the different views of a document (formatted, structure, alternate, links,
and Table of Contents.) or to restore the default values. Note that the
values that are saved are those belonging to the views of the document where
you called this menu. These changes will be reflected when you open new
document windows.</p>

<h4><a name="lang">Language negotiation preferences</a></h4>

<p style="text-align: center"><img src="../images/LangueNegotiation.gif"
alt="Language negotiation menu" /></p>

<p>If a document exists in different languages and your server is configured
to do language negotiation, you can use this menu to enter your language
preferences (first one has the highest priority). Try browsing this <a
href="http://www.w3.org/1999/05/WCAG-RECPressRelease.html">press release</a>,
which exists in French, English, and Japanese versions. By changing the
language negotiation to the ISO codes for the above languages
(<strong>fr</strong>, <strong>en</strong>, or <strong>ja</strong> you can
transparently request any of these versions.</p>

<h4><a name="Profiles">Profiles</a></h4>

<p style="text-align: center"><img src="../images/ProfilesMenu.gif"
alt="Profiles  menu" /></p>

<p>In Amaya, a <strong>profile</strong> defines the look and feel of the
Amaya user interface. You can use profiles to hide or display the Amaya
buttons and menus. This menu allows you to choose a given profile definition
file and select among any of the profiles proposed by that file. Amaya
proposes five editor profiles: <em>XHTML 1.0 Transitional (editor and
mini-editor), XHTML 1.0 Strict, XHTML Basic, XHTML 1.1</em> and two browser
profile: <em>XHTML 1.0 Transitional (browser and mini-browser)</em>, each one
with different capabilities. Any profile change will be effective upon the
next launch of Amaya.</p>

<h4><a name="Templates">Templates</a></h4>

<p style="text-align: center"><img src="../images/TemplatesMenu.gif"
alt="Templates " /></p>

<p>A <strong>template</strong> is a model, or skeleton, of a document that
you may use for creating your own documents. This menu allows you to specify
the URL of a template server, which may be either local to your station or
stored in some server. You'll need to quit Amaya to validate this option.
When you launch Amaya again., there will be a new <code>File/New/From
Template</code> entry. Following this entry will open a new document showing
you the results of browsing the template URL. The idea is that this page acts
as an index to the templates available in your station or in the template
server. A sample template server is available at <a
href="http://cgi.w3.org/cgi-bin/createform.pl">http://cgi.w3.org/cgi-bin/createform.pl</a>.
For more info, browse the <a
href="http://www.w3.org/Amaya/User/Templates.html">using templates with
Amaya</a> page.</p>
</div>

<p><a href="Annotations.html"><img alt="previous" src="../images/left.gif"
/></a><a href="Manual.html"><img alt="top" src="../images/up.gif" /></a> <a
href="ShortCuts.html"><img alt="next" src="../images/right.gif" /></a></p>
<hr />
<address>
  <a href="mailto:vatton@w3.org">Irène Vatton</a><br />
  <br />

</address>
</body>
</html>
