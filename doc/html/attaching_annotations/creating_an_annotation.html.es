<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <meta name="GENERATOR" content="amaya 5.2" />
  <title>Creating an Annotation</title>
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
      <td><p><a href="configuring_annotation_settings.html.es"
        accesskey="p"><img alt="previous" src="../../images/left.gif" /></a>
        <a href="../Annotations.html.es" accesskey="t"><img alt="top"
        src="../../images/up.gif" /></a> <a
        href="deleting_an_annotation.html.es" accesskey="n"><img alt="next"
        src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div class="Section1" id="page_body">
<h1>Creating an Annotation</h1>

<p>This version of Amaya supports two kinds of annotations: annotations that
apply to a whole document and annotations that apply to a specific point or
selection in a document.</p>
<ul>
  <li>To annotate an entire document, choose <strong>Annotate
    document</strong> from the <strong>Annotations</strong> menu. 
    <p></p>
  </li>
  <li>To annotate a point, put the insertion point at any place in the
    document and choose <strong>Annotate selection</strong> from the
    <strong>Annotations</strong> menu. 
    <p></p>
  </li>
  <li>To annotate a selection, make a selection in the document and choose
    <strong>Annotate selection</strong> from the <strong>Annotations</strong>
    menu.</li>
</ul>

<p>After any of these options are performed, an annotation dialog displays
the annotation metadata and the body of the annotation.</p>

<p><img src="../../images/annotationwindow.png" alt="annotation window" /></p>

<h1>Annotation Metadata</h1>

<p>Currently, the metadata consists of the author's name, title of the
annotated document, annotation type, creation date, and last modification
date. Some of the metadata fields have special properties:</p>
<ul>
  <li>The <strong>Source Document</strong> field is also a link that points
    back to the annotated text. If the user double-clicks it, as when
    following any other link with Amaya, the annotated document will be
    displayed with the annotated text highlighted. 
    <p></p>
  </li>
  <li>The <strong>Annotation Types </strong>field lets you classify the
    annotation and change its type. Double-click on the text "annotation
    type" to see the list of types available. 
    <p></p>
  </li>
  <li>The <strong>Last Modified </strong>field is updated automatically each
    time that an annotation is saved.</li>
</ul>

<p>Below the header area is the annotation body area. It shows the current
content and can be edited like any other HTML document.</p>

<h1>Saving an Annotation</h1>

<p>Saving an annotation is the same as saving any other document with Amaya:
choose <span class="Link"><strong>Save </strong></span>from the
<strong>File</strong> menu (or use its equivalent shortcut or button).</p>

<p>Local annotations are saved to the annotations directory and remote
annotations are saved to the annotation post server, where they are stored if
the user has write access.</p>

<p>To convert a local annotation into a shared one, choose <strong>Post
Annotation</strong> from the <strong>Annotations</strong> menu to save the
annotation to the Post server as defined in the <span
class="Link"><strong>Configuration for Annotations</strong> dialog</span>. If
this operation is successful, the local annotation will be removed and future
<strong>Save</strong> operations will go directly to that annotation
server.</p>

<p>Some commands that can be applied to a document in the Amaya document
window also can be applied to an annotation document in the Annotation
window. For example, the body of an annotation can be printed by choosing
<strong>Print</strong> from the <strong>File</strong> menu, or reloaded by
choosing <strong>Reload document</strong> from the <strong>File</strong>
menu.</p>

<h2>See Also</h2>
<ul>
  <li><span class="Link0"><a
    href="configuring_annotation_settings.html.es">Configuring Annotation
    Settings</a></span></li>
  <li><span class="Link0"><a href="deleting_an_annotation.html.es">Deleting an
    Annotation</a></span></li>
  <li><span class="Link0"><a
    href="loading_and_presenting_annotations.html.es">Loading and Presenting
    Annotations</a></span></li>
  <li><span class="Link0"><a href="navigating_annotations.html.es">Navigating
    Annotations</a></span></li>
  <li><span class="Link0"><a href="moving_annotations.html.es">Moving
    Annotations</a></span></li>
</ul>
</div>

</body>
</html>
