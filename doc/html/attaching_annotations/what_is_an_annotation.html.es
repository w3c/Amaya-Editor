<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>¿Qué es una anotación?</title>
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
      <td><p align="right"><a href="../Annotations.html.es"
        accesskey="t"><img alt="superior" src="../../images/up.gif" /></a> <a
        href="about_reading_annotations.html.es" accesskey="n"><img
        alt="siguiente" src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1><a name="What">¿Qué es una anotación?</a></h1>

<p>Las anotaciones son observaciones, notas, explciaciones u otros tipos de
comentarios externos que pueden adjuntarse a un documento Web o an una parte
de un documento. Puesto que son externos, puedes anotar cualquier documento
Web, sin necesidad de editarlo. Desde un punto de vista técnico, las
anotaciones se consideran <strong>metadatos,</strong> puesto que proporcionan
información adicional a un dato ya existente. En este proyecto, empleamos un
<strong><a href="http://www.w3.org/RDF/">esquema de anotaciones
RDF</a></strong> especial para describir las anotaciones.</p>

<p>Puedes almacenar las anotaciones localmente o en uno o más
<strong>servidores de anotaciones</strong>. Al mostrar un documento, Amaya
consulta a cada uno de estos servidores, solicitando las anotaciones de este
documento. Actualmente Amaya muestra las anotaciones mediante un icono de
lápiz ( <img src="../../images/annot.png" alt="Icono de lápiz de anotación"
/> ) incrustado en el documento, como puedes ver en la imagen siguiente. Si
el usuario hace clic en un icono de anotación, el texto anotado se resalta.
Si el usuario hace doble clic en el icono, el texto de la anotación y otros
metadatos se muestran en una ventana aparte.</p>

<p style="text-align: center"><img src="../../images/annotationicon-es.png"
alt="Icono de anotación (lápiz)" /></p>

<p>Una anotación tiene varias propiedades, por ejemplo:</p>
<ul>
  <li>Ubicación física: indica si la anotación se almacena localmente o en un
    servidor de anotaciones.</li>
  <li>Objeto: indica si la anotación está asociada al documento completo o
    simplemente a un fragmento del documento.</li>
  <li>Tipo de anotación: 'Anotación', 'Observación', 'Solicitud', ...</li>
</ul>
</div>
</body>
</html>
