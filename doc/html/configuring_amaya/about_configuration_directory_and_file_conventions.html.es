<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <title>Directorio de configuración y convenciones de archivo</title>
  <meta name="GENERATOR" content="amaya 8.5, see http://www.w3.org/Amaya/" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="configuring_amaya.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../Configure.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a> <a href="menu_preferences.html.es"
        accesskey="n"><img alt="siguiente" src="../../images/right.gif"
        /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Directorio de configuración y convenciones de archivo</h1>

<p>Amaya utiliza una serie de directorios por omisión para almacenar la
información de configuración.</p>

<h2>El directorio config</h2>

<p>Amaya almacena la mayoría de valores por omisión y todos los mensajes de
las cajas de diálogo en el directorio config. Este directorio se encuentra
dentro del directorio en el que se ha instalado Amaya (por omisión, el
directorio es <code>Amaya/config</code> en Unix; consulta la tabla inferior
con su ubicación en Windows).</p>

<h2>El directorio AmayaHome</h2>

<p>Amaya almacena las preferencias de usuario y otros archivos de
configuración del usuario en un directorio llamado AmayaHome. El usuario
puede elegir un directorio ya existente mediante la variable de entorno del
sistema <code>AMAYA_USER_HOME</code>. En caso contrario, Amaya elige la
pocisión por omisión.</p>

<p>La tabla siguiente indica dónde se encuentra el directorio en los
diferentes sistemas operativos:</p>

<table border="1">
  <tbody>
    <tr>
      <td><p class="TableHead"><b>Sistema operativo</b></p>
      </td>
      <td><p class="TableHead"><b>Ubicación del directorio AmayaHome</b></p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Unix</p>
      </td>
      <td><p class="TableText"><code>$HOME/.amaya</code></p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Windows 95/ Windows 98</p>
      </td>
      <td><p class="TableText"><code>AMAYA-INSTALL-DIR\users\username</code>
        o, si el usuario no está definido,
        <code>AMAYA-INSTALL-DIR\users\default</code></p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Windows NT/XP</p>
      </td>
      <td><p class="TableText"><code>$HOMEDRIVE\$HOMEPATH\amaya</code>, por
        omisión, c:\Documents and Settings\$user_name\amaya</p>
      </td>
    </tr>
  </tbody>
</table>

<p>Este directorio se conserva incluso cuando se instalan nuevas versiones de
Amaya. Contiene los siguientes archivos:</p>
<ul>
  <li><strong>thot.rc</strong>: Contiene las preferencias de usuario,
    modificadas a través del menú
  <strong>Editar&gt;Preferencias</strong>.</li>
  <li><strong>amaya.keyboard</strong> (Unix) o <strong>amaya.kb</strong>
    (Windows): Define los atajos de teclado de Amaya.</li>
  <li><strong>amaya.css</strong>: Hoja de estilo de usuario.</li>
  <li><strong>dictionary.DCT</strong>: Amaya proporciona un corrector
    ortográfico multilingüe. Por omisión, el corrector ortográfico trabaja
    con dos diccionarios: inglés y francés (puedes descargar estas
    diccionarios de los servidores W3C). Cuando quieres registrar una nueva
    palabra, el corrector ortográfico crea o actualiza el diccionario
    personal.</li>
</ul>

<p class="Note"><strong>Nota:</strong> Los archivos
<code>amaya.keyboard</code>, <code>amaya.kb</code> y <code>amaya.css</code>
files se cargan al iniciar Amaya. Si los cambias, debes cerrar y reiniciar
Amaya para que los cambios tengan efecto.</p>

<h2>El directorio AmayaTemp</h2>

<p>Amaya almacena todos los archivos temporales que crea en un directorio
llamado AmayaTemp. La tabla siguiente indica dónde se encuentra el directorio
en los diferentes sistemas operativos:</p>

<table border="1">
  <tbody>
    <tr>
      <td><p class="TableHead"><b>Sistema operativo</b></p>
      </td>
      <td><p class="TableHead"><b>Directorio AmayaTemp</b></p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Unix</p>
      </td>
      <td><p class="TableText"><code>$HOME/.amaya</code></p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Windows 9x</p>
      </td>
      <td><p class="TableText"><code>c:\temp\amaya</code> (valor por
        omisión)</p>
      </td>
    </tr>
    <tr>
      <td>Windows XP</td>
      <td><code>$HOMEDRIVE\$HOMEPATH\Local Settings\Temp\amaya</code>, por
        omisión, c:\Documents and Settings\$user_name\Local
        Settings\Temp\amaya</td>
    </tr>
  </tbody>
</table>

<p>Amaya crea archivos en este directorio al navegar documentos remotos o al
imprimir un documento. La caché también se guarda en este directorio. Puedes
modificar los directorios AmayaTemp y caché mediante los menús <span
class="Link0"><strong>Editar &gt; Preferencias &gt; Generales</strong> y
<strong>Editar &gt; Preferencias &gt; Caché</strong></span>.</p>

<p>Amaya siempre intenta borrar los archivos temporales que crea. Sin embargo
si Amaya se bloquea, los archivos temperales no se borran. Este hecho no
tiene ninguna consecuencia en posteriores sesiones.</p>
</div>
</body>
</html>
