<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>Configurar los iconos de anotación</title>
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
      <td><p align="right"><a href="replying_to_annotations.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../Annotations.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a> <a href="annotation_issues.html.es"
        accesskey="n"><img alt="siguiente" src="../../images/right.gif"
        /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Configurar los iconos de anotación</h1>

<h2>Iconos definibles por el usuario por el tipo de anotación (también
llamados "iconos dinámicos")</h2>

<p>En la versión 6.2, puedes modificar el icono utilizado para indicar la
ubicación de una anoación en un documento anotado.</p>

<p>En la versión 6.2, el icono que indica la anotación se elige como
propiedad del tipo de anotación. Al incluir una propiedad RDF de cada tipo de
anotación que quieres utilizar, puedes elegir el icono asociado a las
anotaciones de dicho tipo.</p>

<p>La configuración de ejemplo incluida en la versión 6.2 asocia los
siguientes iconos:</p>

<table>
  <tbody>
    <tr>
      <td width="35"><img src="../../../amaya/advice.png"
        alt="Advice (Consejo)" /></td>
      <td>Advice (Consejo)</td>
    </tr>
    <tr>
      <td width="35"><img src="../../../amaya/change.png"
        alt="Change (Cambio)" /></td>
      <td>Change (Cambio)</td>
    </tr>
    <tr>
      <td width="35"><img src="../../../amaya/annot.png"
        alt="Comment (Comentario)" /></td>
      <td>Comment (Comentario)</td>
    </tr>
    <tr>
      <td width="35"><img src="../../../amaya/example.png"
        alt="Example (Ejemplo)" /></td>
      <td>Example (Ejemplo)</td>
    </tr>
    <tr>
      <td width="35"><img src="../../../amaya/explanation.png"
        alt="Explanation (Explicación)" /></td>
      <td>Explanation (Explicación)</td>
    </tr>
    <tr>
      <td width="35"><img src="../../../amaya/question.png"
        alt="Question (Pregunta)" /></td>
      <td>Question (Pregunta)</td>
    </tr>
    <tr>
      <td width="35"><img src="../../../amaya/seealso.png"
        alt="SeeAlso (Ver también)" /></td>
      <td>See also (VerTambién)</td>
    </tr>
  </tbody>
</table>

<p>El nombre de la propiedad que añade iconos de anotación es <a
href="http://www.w3.org/2001/10/typeIcon#usesIcon">http://www.w3.org/2001/10/typeIcon#usesIcon</a>.
Por ejemplo, para especificar un archivo de icono llamado
file:///home/question-icon.jpg para las anotaciones que tienen el tipo <a
href="http://www.w3.org/2000/10/annotationType#Question">http://www.w3.org/2000/10/annotationType#Question</a>
escribirías el RDF/XML siguiente en un archivo que Amaya lee al iniciarse:</p>
<pre>&lt;rdf:RDF
   xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
   xmlns:i = "http://www.w3.org/2001/10/typeIcon#"&gt;
&lt;rdf:Description rdf:about="http://www.w3.org/2000/10/annotationType#Question"&gt;
  &lt;i:usesIcon rdf:resource="file:///home/question-icon.jpg" /&gt;
&lt;/rdf:Description&gt;
&lt;/rdf:RDF&gt;</pre>

<p>La forma más sencilla de que Amaya cargue este RDF al iniciarse es incluir
este archivo en el archivo config/annot.schemas del directorio de Amaya. Para
conservar este archivo de manera que no se sobreescriba al instalar una nueva
versión de Amaya, debes copiar el archivo config/annot.schemas en tu
directorio personal de Amaya; ~/.amaya/annot.schemas (en Unix) o
/winnt/profiles/&lt;username&gt;/amaya/annot.schemas (en Windows). Puedes
listas tantos archivos RDF como quieras en annot.schemas. Para obtener más
detalles, consulta los comentarios incluidos en el propio archivo.</p>

<p>La versión 6.2 incluye un archivo de ejemplo llamado "typeIcon.rdf" que
declara iconos únicos para cada tipo de anotación declarado en el espacio de
nombres <a
href="http://www.w3.org/2000/10/annotationType#">http://www.w3.org/2000/10/annotationType#</a>.
Para experimentar con los iconos definidos por el usuario, puede ser más
fácil copiar este archivo typeIcon.rdf en otro directorio y modificarlo.
Copia annot.schemas en tu directorio raíz de Amaya y cambia la línea situada
cerca del final para que apunte a tu archivo de icono.</p>

<p>Para volver al comportamiento previo a la versión 6.2, edita el archivo
config/annot.schemas del directorio de instalación de Amaya y añade un
carácter de comentario ("#") al principio de la línea situada cerca del final
que hace referencia al archivo typeIcon.rdf:</p>
<pre>#user-defined icons
#http://www.w3.org/2001/10/typeIcon# $THOTDIR/config/typeIcon.rdf</pre>

<p>Amaya admite como formatos para los iconos imágenes de bitmap JPEG, PNG o
GIF. En la versión 6.2 el URI del icono puede ser un archivo: URI; es decir,
el icono debe aparecer en un directorio local o montado para Amaya. Amaya
admite dos formas especiales de no-archivos: URI. Si la ubicación empieza por
"$THOTDIR" o "$APP_HOME", entonces el directorio de instalación de Amaya o el
directorio raíz personal de Amaya se sustituye en el nombre de la
ubicación</p>
</div>
</body>
</html>
