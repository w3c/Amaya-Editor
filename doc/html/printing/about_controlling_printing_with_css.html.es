<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
	
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <meta name="GENERATOR" content="amaya 5.1" /> 
  <title>About Controlling Printing with CSS</title>
  <style type="text/css">  </style>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="en" lang="en">

<table border="0" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p><a href="print_command.html.es" accesskey="p"><img alt="previous"
        src="../../images/left.gif" /></a> 
		<a href="../Printing.html.es" accesskey="t"><img alt="top" src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>About Controlling Printing with CSS</h1>

<p>Amaya does not print links as underlined. To make this happen, add the
following line to your user style sheet (amaya.css, located in the <span
class="Link0">Amaya home directory</span>):</p>

<p><code>@media print {a:link {text-decoration: underline} ... }</code></p>

<p>Amaya supports the CSS page break property <code>page-break-before:
always</code>. When you attach this property to a specific element using an
attribute style, Amaya generates a page break before that element. When you
attach this property to an element type within the style element of the
document or within an external CSS style sheet, Amaya generates a page break
before each instance of that element.</p>
</div>

</body>
</html>
