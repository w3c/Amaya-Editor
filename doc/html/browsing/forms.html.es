<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <title>Botones y Menú Tipos</title>
  <style type="text/css">
 </style>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="en" lang="en">

<table border="0" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p><a href="about_access_keys.html.es" accesskey="p"><img
        alt="previous" src="../../images/left.gif" /></a> <a
        href="../Browsing.html.es" accesskey="t"><img alt="top"
        src="../../images/up.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Browsing forms</h1>

<p>As Amaya is a browser/editor, some of the widgets associated with HTMl
form elements are different from those you are used to in browsers. This
section mentions the most notable differences.</p>

<h2>Form input selector, single selection (GTK and WIN32 only)</h2>

<p>To activate the selector, the user has to click twice on the entry. This
will result in a popup widget appearing showing all the entries. The widget
supports a user interaction both with the keyboard as well as with the
mouse.</p>

<p>With the keyboard, you must first select an entry using the arrow keys.
Pressing the Enter key will validate the choice. the Page Up and Page Down
keys allow the user to quickly scroll amont the list entries. You can abort
the action of this widget by pressing the Escape key anytime.</p>

<p>With the mouse, there are two kinds of behavior. A single click allows the
user to select an entry, whereas a double click on an entry means that the
user wants to activate the selection. To abort the action of the widget, you
just need to click in any area outside of the pop-up widget (N.B., this
option doesn't work yet on GTK). You can also use the scroll bar to quickly
navigate thru the widget entries.</p>

<h2>Form input selector, multiple selection</h2>

<p>The behavior of this widget is similar to the single selection one. That
is, the widget supports only one toggle of a selection state at the time. You
need to invoke it as many times as toggle changes you need. When the widget
is activated, it will show the current state of selections. On WIN32, this is
currently done by appending a '&gt;' character to the selected entries. On
GTK, this is done by means of a blue color bar. As before, you can abort the
widget by using either the Escape key or clicking elsewhere (only on WIN32
systems).</p>
</div>
</body>
</html>
