<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <title>Editando Atributos</title>
  <meta name="GENERATOR" content="amaya 5.2" />
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
</head>

<body xml:lang="es" lang="es">

<table border="0" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../images/w3c_home" /> <img alt="Amaya"
        src="../images/amaya.gif" /></td>
      <td><a href="StyleSheets.html" accesskey="p"><img alt="previa"
        src="../images/left.gif" /></a> <a href="Manual.html"
        accesskey="t"><img alt="arriba" src="../images/up.gif" /></a> <a
        href="SpellChecking.html" accesskey="n"><img alt="siguiente"
        src="../images/right.gif" /></a></td>
    </tr>
  </tbody>
</table>

<div id="Attributes">
<h2>Editando atributos</h2>

<p>Hay dos maneras de editar atributos:</p>
<ul>
  <li>el menú <b>Atributos</b>,</li>
  <li>las vista Estructura.</li>
</ul>

<h3><a name="L1073">Editando atributos a través del menú Atributos</a></h3>

<p>El menú <b>Atributos</b> lista todos los atributos que pueden asociarse
con el elemento seleccionado. Puede usarse para crear una atributo, para
cambiar el valor de un atributo existente o para borrar un atributo.</p>

<p>Para editar un atributo primero selecciona el elemento apropiado. Se debe
seleccionar el elemento completo, no sólo un carácter dentro del elemento,
para ello utiliza la <a href="Selecting.html#Selecting1">tecla Esc (en
plataformas Unix) o F2 (en plataformas Windows)</a>. Cuando el elemento esté
seleccionado, elige el atributo a ser editado desde el menú <b>Atributos</b>,
entonces Amaya actuará de acuerdo al atributo elegido:</p>
<ul>
  <li>Si se trata de un atributo buleano (como lo es <code>ismap</code> para
    una imagen) el comando termina. Eligiendo este atributo en el menú lo
    elimina del elemento seleccionado, si está presente, o lo añade si no lo
    está.</li>
  <li>Si es un atributo con una limitada serie de valores (como lo es
    <code>align</code> para un párrafo o encabezado) se abre un cuadro de
    diálogo. Este cuadro contiene un menú de los valores disponibles y tres
    botones. 
    <ul>
      <li>Si quieres crear este atributo o cambiar su valor, elige el valor
        deseado desde el menú y haz clic sobre el botón <b>Aplicar</b>.</li>
      <li>Si quieres eliminar ese atributo, haz clic sobre el botón
        <b>Borrar</b>.</li>
    </ul>
  </li>
  <li>Si se trata de un atributo con valor libre (tal como <code>href</code>
    para un ancla) aparece un cuadro de diálogo. En este cuadro de diálogo
    puedes editar el valor del atributo, terminando el comando con el botón
    <b>Aplicar</b>. Para borrar el atributo, haz clic en el botón
    <b>Borrar</b>.</li>
</ul>

<h3><a name="Copying">Copiando atributos</a></h3>

<p>Un atributo no es lo mismo que un elemento, y un atributo no puede ser
copiado por medio de los comandos Copiar/Pegar. Para copiar un atributo
necesitas usar el menú Atributo. Primero tienes que seleccionar el elemento
que tiene el atributo que quieres copiar. Selecciona la entrada de atributo
apropiada en el menú Atributos. Luego seleccionar el nuevo elemento con el
que quieres asociar el atributo actual y hacer clic en el botón
<b>Aplicar</b>.</p>

<h3><a name="Editing">Editando atributos en la vista Estructura</a></h3>

<p>En la vista Estructura se muestran todos los atributos. Los atributos cuyo
valor puede ser elegido libremente se presentan en negro y puedes editar su
valor al igual que con cualquier otra cadena de caracteres. Cuando el valor
del atributo se presenta en azul, sólo puedes cambiarlo con el menú
<b>Atributos</b>. Esto garantiza que se elige sólo uno de los valores
correctos.</p>
</div>

<p><a href="StyleSheets.html"><img alt="previa" src="../images/left.gif"
/></a> <a href="Manual.html"><img alt="arriba" src="../images/up.gif" /></a>
<a href="SpellChecking.html"><img alt="siguiente" src="../images/right.gif"
/></a></p>
<hr />

<p></p>
</body>
</html>
