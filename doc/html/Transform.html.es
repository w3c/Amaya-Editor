<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <meta name="GENERATOR" content="amaya 8.3-, see http://www.w3.org/Amaya/" />
  <title>Utilizar el archivo HTML.trans</title>
  <link href="style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><p><img alt="W3C" src="../images/w3c_home" /> <img alt="Amaya"
        src="../images/amaya.gif" /></p>
      </td>
      <td><p align="right"><a
        href="editing_documents/about_merging_elements.html.es"
        accesskey="p"><img alt="anterior" src="../images/left.gif" /></a> <a
        href="Changing.html.es" accesskey="t"><img alt="superior"
        src="../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Utilizar el archivo HTML.trans</h1>

<p>Este documento es una introducción al mecanismo de
transformaciónestructural incluido en Amaya. Describe la sintaxis del
lenguaje de transformación y la manera en que esas transformaciones se
efectúan en el editor.</p>

<p>El archivo <code>amaya/HTML.trans</code> contiene la descripción de las
transformaciones disponibles. Puedes editar este archivo mientras estás
utilizando Amaya. El archivo se analiza cada vez que solicitas una
transformación, así que puedes añadir transformaciones mientras estás
editando un documento.</p>

<p><strong>Atención:</strong> Puesto que la descripción de las
transformaciones puede incluir etiquetas, <strong>no</strong> edites el
archivo <code>HTML.trans</code> con Amaya. Puedes utilizar cualquier otro
editor de texto.</p>
<hr />

<h2>Sintáxis del lenguaje de transformación de Amaya</h2>

<p>Los comentarios comienzan por <code><strong>!</strong></code> y continúan
hasta el final de la línea.</p>

<p>El archivo consiste en un lista de descripciones de tranformaciones. Cada
transformación se describe en tres partes:</p>
<ul>
  <li>un <em>nombre</em> terminado con dos
    puntos<strong><code>:</code></strong></li>
  <li>un <em>patrón origen</em> terminado con punto y coma
    <strong><code>;</code></strong></li>
  <li>y una lista de <em>reglas</em> entre llaves <strong><code>{
    }</code></strong> en la que cada regla se termina por un punto y coma
    <strong><code>;</code></strong></li>
</ul>

<p>El nombre aparece en el menú <strong>Transformar</strong> e identifica la
transformación de cara al usuario.</p>

<h3>El patrón</h3>

<p>El patrón describe la organización específica de los elementos a
transformar. Actúa como filtro de la DTD HTML. El patrón identifica la
combinación de elementos a la que puede aplicarse la transformación. El
patrón puede incluir condiciones sobre la secuencia de etiquetas, sobre el
contenido de una etiqueta y sobre la existencia y valor de los atributos.</p>

<p>Formalmente un patrón contiene etiquetas HTML (en su caso, con atributos)
y algunos operadores:</p>

<p><strong><code>|</code></strong> indica elección</p>

<p><strong><code>,</code></strong> indica hermano</p>

<p><strong><code>+</code></strong> indica secuencia</p>

<p><strong><code>?</code></strong> indica elección</p>

<p><strong><code>( )</code></strong> indica agrupación de nodos</p>

<p>Las llaves <code><strong>{</strong></code> <code><strong>}</strong></code>
definen el contenido de un nodo.</p>

<p>El símbolo <strong><code>*</code></strong> inidca que se seleccionará
cualquier tipo de elemento.</p>

<p>Puedes renombrar una etiqueta escribiendo antes de ella un nombre seguido
de dos puntos (<strong><code>:</code></strong>).</p>

<p>La etiqueta puede tener atributos. Si no se especifica ningún valor para
un atributo, se seleccionará cualquier elemento que tenga el atributo. Si se
especifica un valor para un atributo, tan sólo se seleccionarán los elementos
que contengan el atributo y el valor especificado.</p>

<p>Al final del documento puedes consultar <a href="#L235">ejemplos</a> de
patrones.</p>

<h3>Las reglas</h3>

<p>Una regla expresa cómo se transformarán los elementos identificados por el
patrón. Una regla tiene dos partes separadas por el símbolo
<strong><code>&gt;</code></strong>:</p>
<ul>
  <li>una etiqueta origen o un nombre definido en el patrón,</li>
  <li>una lista de etiquetas destino, que indica las etiquetas que deben
    generarse y el lugar en que se insertan al transformar el elemento
  origen.</li>
</ul>

<p>La lista de etiquetas destino se divide a su vez en dos partes separadas
por dos puntos (<strong><code>:</code></strong>):</p>
<ul>
  <li>el camino de generación (que identifica el lugar en que se insertan las
    nuevas etiquetas)</li>
  <li>la lista de etiquetas a generar</li>
</ul>

<p>El camino de generación se recorre a partir de la rama situada más a la
izquierda en el árbol del documento, comenzando en el padre del elemento que
coincide con el símbolo más alto del patrón.</p>

<p>En la lista de etiquetas destino, el símbolo punto
(<code><strong>.</strong></code>) se emplea para bajar en la estructura del
árbol.</p>

<p>Si el símbolo especial asterisco (<code><strong>*</strong></code>)
completa la lista de etiquetas a generar, la etiqueta origen no cambia, pero
puede situarse en otro lugar en el destino.</p>

<p>Si la etiqueta origen o el nombre situado en la parte izquierda de una
regla aparece más de una vez en el patrón, la regla transforma todos los
elementos del patrón que coincidan.</p>

<h2>Proceso de transformación</h2>

<p>Cuando el usuario elige el comando <strong>Transformar</strong> del menú
<strong>Editar</strong>, Amaya analiza el archivo <code>HTML.trans</code>. A
continuación, los elementos seleccionados se comparan con el patrón de cada
transformación. Los nombres de las trasnformaciones coincidentes se proponen
al usuario en un menú emergente.</p>

<p>Si se pueden aplicar varias transformaciones con el mismo nombre a los
elementos seleccionados, se propone al usuario la transformación que coincide
al nivel más alto. Si se pueden aplicar varias transformaciones del mismo
nivel, se propone la que aparezca primero en el archivo
<code>HTML.trans</code>. En consecuencia, se recomienda especificar antes las
transformaciones específicas que las generales.</p>

<p>Una vez que el usuario ha elegido un transformación, la estructura de
destino se construye siguiendo las reglas mientras se recorren los elementos
seleccionados.</p>

<p>Finalmente, el contenido de los elementos origen (texto e imágenes, pero
también contenido estructurado) se sitúa en los elementos generados.</p>

<p>Este proceso de transformación de los documentos HTML se describe en
detalle en el artículo <a
href="http://opera.inrialpes.fr/opera/papers9696.html">Reestructurar
interactivamente documentos HTML</a>, presentado en la <a
href="http://www5conf.inria.fr/">5ª conferencia internacional WWW</a> en
Paris, Mayo 1996, por Cécile Roisin y Stéphane Bonhomme.</p>

<h2><a name="L235" id="L235">Ejemplos</a></h2>
<ul>
  <li>El primer ejemplo une varias listas no ordenadas (ul) consecutivas en
    una única lista.
    <pre>Merge Lists: (ul{li+})+;

    { 

    li &gt; ul:li; 

    }</pre>
    <p>El patrón coincide con una secuencia de listas no ordenadas (ul), que
    contengan items (li).</p>
    <p>La regla significa que cada vez que se encuentra un item al recorrer
    los elementos seleccionados, se crea una nueva etiqueta li dentro de un
    ul. Cuando se aplica la regla por primera vez, la estructura resultante
    está vacía, por lo que no existe elemento ul en el que crear el li. Una
    vez se ha cereado el ul, se puede aplicar la regla.</p>
  </li>
  <li>El segundo ejemplo transforma una lista de definición en una tabla.
    <pre>Table: dl{(dt|dd)+}; 

   { 

   dt &gt; &lt;table border=1&gt;.tbody:tr.td; 

   dd &gt; &lt;table border=1&gt;.tbody.tr:td; 

   }</pre>
    <p>El patrón coincide con cualquier elemento lista definición
    (<code>dl</code>).</p>
    <p>Las reglas explican cómo se crea la tabla al recorrer la estructura de
    las listas de definición seleccionadas.</p>
    <ul>
      <li>Cada <code>dt</code> implica la creación de una nueva fila
        (<code>tr</code>) en el cuerpo de la tabla.</li>
      <li>Cada <code>dd</code> implica la creación de una nueva celda
        (<code>td</code>) en la última fila de la tabla.</li>
    </ul>
  </li>
  <li>El tercer ejemplo elimina una tabla, manteniendo su contenido sin
    cambios, pero fuera de la tabla.
    <pre>Remove Table:

table{?caption,?(body:*{(tr{(td{(?cell_content:*)+}|

                             th{(?cell_content:*)+}

                           )})+})+};

     { 

     caption&gt;h3; 

     cell_content&gt;:*;

     }</pre>
    <p>El patrón coincide con cualquier tabla e identifica el contenido de
    cada celda de la tabla (cell_content).</p>
    <p>La segunda regla significa que el contenido de cada celda se situará
    en el lugar donde se encontraba la tabla original.</p>
  </li>
  <li>Ver el archivo <kbd>amaya/HTML.trans</kbd> para consultar otros
    ejemplos de transformaciones.</li>
</ul>
</div>
<hr />
<address>
  <a href="mailto:Stephane.Bonhomme@inrialpes.fr">Stéphane Bonhomme</a> <br />
  $Fecha 2002/09/10 07:07:21 $
</address>
</body>
</html>
