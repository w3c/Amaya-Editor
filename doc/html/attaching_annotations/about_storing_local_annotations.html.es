<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>Anotaciones locales</title>
  <meta name="GENERATOR"
  content="amaya 8.0-pre, see http://www.w3.org/Amaya/" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="about_reading_annotations.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../Annotations.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a> <a
        href="about_storing_remote_annotations.html.es" accesskey="n"><img
        alt="siguiente" src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Anotaciones locales</h1>

<p>Amaya puede almacenar las anotaciones en el sistema de archivos local
("anotaciones locales") o puede almacenarlas <a
href="about_storing_remote_annotations.html.es">remotamente</a>, accediendo a
ella a través de la World Wide Web ("anotaciones remotas").</p>

<p>No necesitas tener un servidor para crear anotaciones locales. Las
anotaciones locales se almacenan en el directorio de preferencias de usuario,
en un directorio especial llamado <strong>anotaciones</strong>, y sólo pueden
ser vistas por su propietario (de acuerdo con la configuración de los
derechos de acceso del sistema). Este directorio contiene tres tipos de
archivos:</p>
<ul>
  <li><strong>annot.index:</strong> Asocia los URLs con los archivos que
    contienen los metadatos de las anotaciones.</li>
  <li><strong>index + sufijo aleatorio:</strong> Almacena los metadatos de
    las anotaciones relacionadas con un determinado URL. Los metadatos se
    especifican mediante RDF.</li>
  <li><strong>annot + sufijo aleatorio.html:</strong> contiene el contenido
    de la anotación, almacenado en formato XHTML.</li>
</ul>

<p>En cualquier momento, puedes convertir una anotación local en remota
eligiendo el comando de menú <strong>Anotaciones &gt; Enviar
anotación</strong>. Una vez enviada, la anotación local se borra puesto que
ya se encuentra en un servidor de anotaciones.</p>

<h2>Ver también:</h2>
<ul>
  <li><span class="Link0"><a
    href="about_storing_remote_annotations.html.es">Anotaciones
    remotas</a></span></li>
</ul>
</div>
</body>
</html>
