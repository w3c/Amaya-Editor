<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
	
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <title>Configuring Annotation Icons</title>
  <style type="text/css">
  </style>
  <meta name="GENERATOR" content="amaya 5.2" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="en" lang="en">

<table border="0" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> 
	      <img alt="Amaya" src="../../images/amaya.gif" /></td>
      <td><p><a href="replying_to_annotations.html.es" accesskey="p">
	  		<img alt="previous" src="../../images/left.gif" /></a> 
		<a href="../Annotations.html.es" accesskey="t">
		   <img alt="top" src="../../images/up.gif" /></a> 
		<a href="annotation_issues.html.es" accesskey="n">
		   <img alt="next" src="../../images/right.gif" /></a>
	</p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Configuring Annotation Icons</h1>

<h2>User-definable icons by Annotation type (aka "dynamic icons")</h2>

<p>
As of Release 6.2, the icon used to mark the location of
an annotation within an annotated document may be changed
by the user.</p>

<p>
In release 6.2 the icon that denotes an annotation is chosen
as a property of the annotation type.  By including an RDF
property of each annotation type you wish to use, you select
the icon associated with annotations of that type.</p>

<p>
The sample configuration that is shipped with release 6.2
associates the following icons:</p>

<table>
 <tr>
  <td width="35"><img src="../../../amaya/advice.png" alt="Advice"/></td>
  <td>Advice</td>
 </tr>

 <tr>
  <td width="35"><img src="../../../amaya/change.png" alt="Change"/></td>
  <td>Change</td>
 </tr>

 <tr>
  <td width="35"><img src="../../../amaya/annot.png" alt="Comment"/></td>
  <td>Comment</td>
 </tr>

 <tr>
  <td width="35"><img src="../../../amaya/example.png" alt="Example"/></td>
  <td>Example</td>
 </tr>

 <tr>
  <td width="35"><img src="../../../amaya/explanation.png" alt="Explanation"/></td>
  <td>Explanation</td>
 </tr>

 <tr>
  <td width="35"><img src="../../../amaya/question.png" alt="Question"/></td>
  <td>Question</td>
 </tr>

 <tr>
  <td width="35"><img src="../../../amaya/seealso.png" alt="SeeAlso"/></td>
  <td>SeeAlso</td>
 </tr>

</table>

<p>
The property name for adding annotation icons is
<a href="http://www.w3.org/2001/10/typeIcon#usesIcon"
>http://www.w3.org/2001/10/typeIcon#usesIcon</a>.
For instance, to specify an icon named
file:///home/question-icon.jpg for annotations that have type 
<a href="http://www.w3.org/2000/10/annotationType#Question"
>http://www.w3.org/2000/10/annotationType#Question</a>
you would enter the following RDF/XML into a file that
Amaya reads at startup is:</p>

<pre>
&lt;rdf:RDF
   xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
   xmlns:i = "http://www.w3.org/2001/10/typeIcon#">
&lt;rdf:Description rdf:about="http://www.w3.org/2000/10/annotationType#Question">
  &lt;i:usesIcon rdf:resource="file:///home/question-icon.jpg" />
&lt;/rdf:Description>
&lt;/rdf:RDF>
</pre>

<p>
The simplest way to get such RDF loaded into Amaya at startup is
to include the file in the config/annot.schemas file in the Amaya
program directory.  In order to preserve this file so that it will
not be overwritten when you install a new version of Amaya, you
should copy the config/annot.schemas file into your personal
Amaya home directory; ~/.amaya/annot.schemas (on Unix systems)
or /winnt/profiles/&lt;username>/amaya/annot.schemas (on Microsoft
Windows systems).  You may list as many RDF files as you want
in annot.schemas.  See the comments in the file included in the
Amaya kit for more details.</p>

<p>
Release 6.2 includes a sample file named "typeIcon.rdf" that
declares unique icons for each annotation type declared in the
<a href="http://www.w3.org/2000/10/annotationType#"
>http://www.w3.org/2000/10/annotationType#</a> namespace.  To experiment
with user-defined icons, it may be easiest to copy this typeIcon.rdf
into another directory and modify it.  Copy annot.schemas to your
Amaya home directory and change the line near the end to point to
your revised icon definition file.</p>

<p>
To revert to the previous behavior prior to release 6.2, edit
the config/annot.schemas in the Amaya installation directory and
add a comment character ("#") at the beginning of the line near
the end that refers to typeIcon.rdf:</p>

<pre>
#user-defined icons
#http://www.w3.org/2001/10/typeIcon# $THOTDIR/config/typeIcon.rdf
</pre>

<p>
Amaya supports JPEG, PNG, and GIF bitmap graphics formats for icon
images.  In release 6.2 the icon URI may only be a file: URI; that
is, the icon must appear in a local or mounted directory to Amaya.
Two special forms of non-file: URIs are supported.  If the file path
name starts with "$THOTDIR" or "$APP_HOME" then the corresponding
Amaya installation directory or personal Amaya home directory is
substituted into the pathname.</p>

</div>

</body>
</html>

