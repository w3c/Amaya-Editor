<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <meta name="GENERATOR" content="amaya 8.3-, see http://www.w3.org/Amaya/" />
  <title>El comando Guardar como</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="the_save_command.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../Publishing.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>El comando Guardar como</h1>

<p>Al elegir el comando de menú <strong>Archivo &gt; Guardar como</strong> se
abre la caja de diálogo <strong>Guardar como</strong>, con la que puedes:
guardar el documento como XML, HTML o como archivo de texto en un disco local
o en un URI remoto, guardar las imágenes incrustadas en la misma carpeta que
el documento o en otro lugar y transformar los URIs incrustados.</p>

<h2>La caja de diálogo Guardar como</h2>

<p>Los elementos de esta caja de diálogo se utilizan así:</p>
<ul>
  <li>El <strong>Formato de documento</strong> permite elegir entre
    <strong>HTML</strong>, <strong>XML</strong> (XHTML) y
    <strong>Texto</strong> para los documentos HTML. Este submenú no se
    muestra cuando guardas SVG, MathML, CSS o cualquier otro tipo de
    documento. Por omisión, Amaya mantiene el formato del documento actual.
    Si eliges <strong>Text</strong> se elimina todo el marcado HTML y se
    sutituye por espacios, slatos de línea, etc.</li>
  <li>El campo <strong>Ubicación de documento</strong> indica dónde se
    guardará el archivo. Puede ser un archivo local (por ejemplo,
    <code>/pub/html/welcome.html</code>) o un URI remoto (por ejemplo,
    <code>http://www.w3.org/pub/WWW/Welcome.html</code>).
    <p class="Note"><strong>Nota:</strong> Escribe el camino completo para el
    URL. Asímismo, debes estar seguro de que tiene derecho a ejecutar un
    método <code>PUT</code> si se trata de un lugar remoto. Consulta la
    página <a href="http://www.w3.org/Amaya/User/Put.html">configurar el
    servidor para aceptar el método PUT</a> o pregunta al administrador de tu
    Web.</p>
  </li>
</ul>
<ul>
  <li>La casilla de verificación <strong>Copiar imágenes</strong> indica si
    las imágenes incrustadas en el documento actual se debn copiar junto con
    el documento a la nueva ubicación. Normalmente es mejor tener esta
    casilla marcada porque si no no estarías seguro de que las imágenes se
    encontraran al abrir el documento posteriormente. Sin embargo, es útil
    tener esta casilla desmarcada cuando simplemente has cambiado el texto
    del documento y vas a guardar el documento en el mismo servidor. Al
    copiar las imágenes, Amaya actualizará automáticamente todos los
    atributos <strong>SRC</strong> para que apunten a la nueva ubicación de
    las imágenes. Las nuevas ubicaciones se escribirán como URIs
  relativos.</li>
  <li>La casilla de verificación <strong>Transformar URIs</strong> transforma
    los URIs actuales en URIs relativos siempre que sea posible. Por
    supuesto, si el documento se guarda en un archivo local y los URIs
    apuntan a archivos remotos, los URIs serán absolutos. Esto asegura que
    todos los enlaces del documento permanezcan correctos después de la
    copia. Sin embargo, si tienes varios documentos enlazados entre sí con
    URIs relativos y quieres moverlos a una ubicación remota, debes
    desactivar la conversión de URIs para evitar las referencias a las
    ubicaciones iniciales.</li>
  <li>El campo <strong>Directorio de imágenes</strong> te permite guardar
    imágenes (si la casilla <strong>Copiar imágenes</strong> está activada)
    en un lugar distinto al del archivo HTML. Este campo debe contener el
    nombre de una carpeta del sistema de archivos local o la ubicación en el
    servidor remoto.
    <ul>
      <li>Al utilizar caminos relativos, las imágenes se guardan en una
        ubicación relativa a la carpeta del documento. Por ejemplo, si la
        ubicación del documento es <code><a
        href="http://www.w3.org/pub/WWW/Welcome.html">http://www.w3.org/pub/WWW/Welcome.html</a></code>
        y la ubicación de las imágenes es "Images", la imagen "W3C.png"
        contenida en el documento se almacenaría con el URI <code><a
        href="http://www.w3.org/pub/WWW/Images/W3C.png">http://www.w3.org/pub/WWW/Images/W3C.png</a></code>
        y el archivo HTML se modificará para que la referencia sea:
        <p><code>&lt;img src="Images/W3C.png"&gt;</code></p>
        <p>Sucedería lo mismo en caso de que la ubicación del documento fuera
        en el sistema de archivo local.</p>
      </li>
      <li>Al utilizar caminos absolutos, las imágenes se almacenan en la
        ubicación exacta, independientemente de la ubicación del documento.
        En el ejemplo previo, si la ubicación especificada de las imágenes es
        <code>http://pub/WWW/Images</code>, la imagen se almacena en
        <code>http://www.w3.org/pub/WWW/Images/W3C.png</code> y el archivo
        HTML se modificará para que la referencia sea:
        <p><code>&lt;img src="Images/W3C.png"&gt;</code></p>
      </li>
    </ul>
  </li>
</ul>

<p>En la parte inferior hay cuatro botones:</p>
<ul>
  <li>Al hacer clic en <strong>Confirmar</strong> se pone en marcha la
    transformación del documento. Una caja de diálogo solicita confirmación
    en los siguientes casos:
    <ul>
      <li>El documento debe guardarse en un servidor remoto. Amaya muestra el
        URI completo para que lo compruebes.</li>
      <li>El documento debe guardarse localmente y el archivo ya existe.</li>
    </ul>
  </li>
  <li>Al guardar en una ubicación remota, comprueba que no está activado el
    http_proxy, o que el servidor proxy y el <a
    href="http://www.w3.org/Amaya/User/Put.html">servidor destino están
    configurados para trabajar con el método PUT</a>.</li>
  <li>En estos momentos Amaya no contacta con el servidor remoto (en caso de
    ser <code>http://...</code> destino) para comprobar si este documento ya
    existe.</li>
  <li>Al hacer clic en <strong>Buscar</strong> puedes seleccionar una carpeta
    y un nombre de archivo locales:
    <ul>
      <li>El campo <strong>Directorios de documentos</strong> muestra los
        directorios existentes en el directorio local actual. Debes
        utilizarlo para guardar un archivo en el sistema de archivo
      local.</li>
      <li>El campo <strong>Archivos</strong> muestra los archivos existentes
        en el directorio local actual.</li>
    </ul>
  </li>
  <li>Al hacer clic en <strong>Limpiar</strong> eliminas el contenido actual
    del campo <strong>Ubicación del documento</strong> y del campo
    <strong>Directorio de imágenes</strong>.</li>
  <li>Al hacer clic en <strong>Cambiar Charset</strong> se abre una caja de
    diálogo en la que puedes cambiar el juego de caracteres del documento.
    Esta opción sólo funciona en los documentos que permiten definir su juego
    de caracteres, como HTML, XML. Esta opción está disponible tanto para
    guardar archivos remotos como locales.</li>
  <li>Al hacer clic en <strong>Cambiar Mime Type</strong> se abre una caja de
    diálogo en la que puedes seleccionar de tipo MIME del documento. Si el
    tipo MIME que buscas no está en la lista, puedes escribirlo tú mismo.
    Este botón está activo únicamente cuando guardas un documento en un
    servidor (los sistemas de archivo local no almacenan la información de
    metadatos MIME).</li>
  <li>Al hacer clic en <strong>Cancelar</strong>, anulas el comando.</li>
</ul>

<h2>Ver también:</h2>
<ul>
  <li><a href="the_save_command.html.es">El comando Guardar</a></li>
</ul>
</div>
</body>
</html>
