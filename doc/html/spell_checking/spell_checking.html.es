<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta name="GENERATOR"
  content="amaya 8.0-pre, see http://www.w3.org/Amaya/" />
  <title>Corrector ortográfico</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="../SpellChecking.html.es"
        accesskey="t"><img alt="superior" src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Corrector ortográfico</h1>

<p>Amaya contiene un corrector ortográfico para varios idiomas y selecciona
el lenguaje apropiado de acuerdo con el atributo <code>LANG</code>. El
atributo <code>LANG</code> puede asociarse a cualquier lemento en un
documento, incluso a una sola palabra, eligiendo el comando de menú
<strong>Atributos &gt; lang</strong>.</p>

<p>Para corregir un documento, elige el comando de menú <strong>Editar &gt;
Verificar ortografía</strong>.</p>

<h2>El diccionario personal</h2>

<p>Al registrar una nueva palabra, el corrector ortográfico crea o actualiza
el <b>diccionario personal de usuario</b>. El diccionario personal de usuario
se encuentra en el archivo <code>AmayaHome/dictionary.DCT</code>. En modo de
lectura y escritura, el corrector ortográfico utiliza el diccionario de
usuario al revisar cualquier documento, además del diccionario específico.
Cuando haces clic en los botones <strong>Saltar (+dic)</strong> y
<strong>Reemplazar (+dic)</strong> de la caja de diálogo <strong>Corrector
ortográfico</strong>, el corrector ortográfico crea y actualiza
automáticamente el diccionario de usuario.</p>

<h2>La caja de diálogo del corrector ortográfico</h2>

<p>El comando de menú <strong>Editar &gt; Verificar ortografía</strong> abre
una caja de diálogo que te permite elegir los parámetros e interactuar con el
corrector ortográfico. Los parámetros son los siguientes:</p>
<ul>
  <li>El campo <strong>Número de propuestas</strong> define el número máximo
    de correcciones propuestas: puede tomar cualquier valor entre 1 y 10, el
    valor por omisión es 3. Cuando se detecta un error, al modificar este
    valor se actualiza la lista de correcciones.
    
  </li>
  <li>La sección <strong>Ignorar</strong> le pide al corrector ortográfico
    ignorar ciertas palabras, por ejemplo, las palabras que contienen
    mayúsculas, dígitos, número romanos o caracteres especiales.&#x20ac;
    
  </li>
  <li>El grupo de opciones <strong>Comprobar</strong>, similar al grupo
    <strong>Dónde buscar</strong>del comando <strong>Buscar</strong>, te
    permite especificar la parte del documento que comprobará el corrector
    ortográfico.
    
  </li>
  <li>El campo <strong>Verificando ortografía</strong> muestra las palabras
    incorrectas. Las palabras propuestas por el corrector se muestran en el
    campo selector situado justo debajo de la palabra incorrecta. Por
    omisión, Amaya selecciona la primera palabra propuesta. Para seleccionar
    otra, haz clic sobre la palabra preferida. Esa palabra se seleccionará.
    Esta corrección puede editarse directamente en este formulario. La
    corrección se realiza cuando haces clic en el botón <strong>Reemplazar y
    siguiente</strong> o en <strong>Reemplazar (+dic)</strong>.
    
  </li>
  <li>El <strong>idioma</strong> en el que se realiza la correción se
    especifica en la esquina superior izquierda de la caja de diálogo. Para
    cambiar el idioma, aplica el atributo Language al texto mediante el menú
    <strong>Atributos</strong> y reinicia el corrector ortográfico.</li>
</ul>

<h2>Utilizar el corrector ortográfico</h2>

<p>Para iniciar el corrector, haz clic en el botón
<strong>Buscar/Saltar</strong> de la caja de diálogo. Si se encuentra un
error, la palabra incorrecta se selecciona y resalta en el documento. Si no
se detecta ningún error, Amaya muestra el mensaje <b>No encontrado</b>.</p>

<p>Cuando se detecta una palabra incorrecta, pulsa el botón correspondiente
para realizar las operaciones siguientes:</p>
<ul>
  <li><strong>Aceptar</strong>: Amaya cierra la caja de diálogo y detiene el
    corrector ortográfico. Las correcciones anteriores no se deshacen.
    
  </li>
  <li><strong>Buscar/Saltar</strong>: Amaya considera la palabra incorrecta
    aceptable. No se realiza ningún cambio ni en el texto ni en los
    diccionarios. El corrector busca el siguiente error.
    
  </li>
  <li><strong>Saltar (+dic)</strong>: Amaya considera la palabra incorrecta
    aceptable y además la añade al diccionario, por lo que el corrector
    ignorará cualquier aparición posterior de la palabra.
    
  </li>
  <li><strong>Reemplazar y siguiente</strong>: Amaya sustituye la palabra
    incorrecta por la palabra correcta (que puede haber sido editada por el
    usuario). El corrector busca el siguiente error.
    
  </li>
  <li><strong>Reemplazar (+dic):</strong> Amaya sustituye la palabra
    incorrecta por la palabra correcta (que puede haber sido editada por el
    usuario), pero además la palabra correcta se añade al diccionario.</li>
</ul>
</div>
</body>
</html>
