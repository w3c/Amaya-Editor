<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
	
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <title>About Local Annotations</title>
  <style type="text/css">
  </style>
  <meta name="GENERATOR" content="amaya 5.1" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="en" lang="en">

<table border="0" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p><a href="about_reading_annotations.html.es" accesskey="p"><img alt="previous"
        src="../../images/left.gif" /></a> 
		<a href="../Annotations.html.es" accesskey="t"><img alt="top" src="../../images/up.gif" /></a> 
		<a href="about_storing_remote_annotations.html.es" accesskey="n"><img alt="next"
        src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>About Local Annotations</h1>

<p>Amaya can store annotation data in a local file system (often called
"local annotations") or it can store annotations <span style="color:blue"><a
href="about_storing_remote_annotations.html.es">remotely</a></span>, accessed
through the World Wide Web (called "remote annotations").</p>

<p>You do not need a server to create local annotations. Local annotations
are stored under the user preferences directory, in a special directory
called <strong>annotations</strong>, and can be seen only by their owner
(according to the system access right setup). This directory contains three
kinds of files:</p>
<ul>
  <li><strong>annot.index:</strong>  Associates URLs with the files
    containing the metadata of the annotations.
  </li>
  <li><strong>index + random suffix:</strong>  Stores the metadata of the
    annotations related to a given URL. The metadata is specified with RDF.
	<br />
  </li>
  <li><strong>annot + random suffix.html:</strong>  Contains the body of an
    annotation, stored as XHTML.</li>
</ul>

<p>At any time, you can convert a local annotation to a shared one by
choosing <strong>Post annotation</strong> from the
<strong>Annotations</strong> menu. Once this is completed, the local
annotation is deleted because it has been moved to an annotation server.</p>

<h2>See also:</h2>
<ul>
	<li><span class="Link0"><a href="about_storing_remote_annotations.html.es">About Stored Annotations</a></span></li>
</ul>

</div>

</body>
</html>
