<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
  <title>Combinaciones de teclas en las versiones Motif y GTK</title>
  <meta name="GENERATOR" content="amaya 8.3-, see http://www.w3.org/Amaya/" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="es" lang="es">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a
        href="entering_ISOLatin1_characters_in_amaya.html.es"
        accesskey="p"><img alt="anterior" src="../../images/left.gif" /></a>
        <a href="../EditChar.html.es" accesskey="t"><img alt="superior"
        src="../../images/up.gif" /></a> <a
        href="about_white_space_handling.html.es" accesskey="n"><img
        alt="siguiente" src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Combinaciones de teclas en las versiones Motif y GTK</h1>

<p>Si un carácter ISO-Latin no existe en un teclado estándar, puedes insertar
el carácter pulsando la tecla especial <code>Compose</code> seguida de dos
teclas más. En la tabla inferior se describen varios caracteres ISO_latin. En
Amaya puedes intercambiar la primera y segunda tecla.</p>

<p>Si tu teclado no dispone de la tecla <kbd>Compose</kbd>, puedes asociarla
a una tecla como <kbd>AltGr</kbd> mediante el comando <code>xmodmap</code>
siguiente:</p>

<p><code>keycode 113 = Multi_key</code></p>

<p>Puedes incluir este comando en el archivo <code>$HOME/.Xmodmap</code> (en
Unix), y Amaya lo aplicará cuando inicies tu servidor X.</p>

<table border="0" cellspacing="0" cellpadding="0" class="characters">
  <thead>
    <tr>
      <td><p class="TableHead"><strong>Nombre</strong></p>
      </td>
      <td><p class="TableHead"><strong>Código</strong></p>
      </td>
      <td><p class="TableHead"><strong>Primera/Segunda tecla</strong></p>
      </td>
      <td><p class="TableHead"><strong>Segunda/Primera tecla</strong></p>
      </td>
      <td><p class="TableHead"><strong>Resultado</strong></p>
      </td>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><p class="TableText">AElig</p>
      </td>
      <td><p class="TableText">198</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; A</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; E</p>
      </td>
      <td><p class="TableText">Æ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Aacute</p>
      </td>
      <td><p class="TableText">193</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; A</p>
      </td>
      <td><p class="TableText">apóstrofo</p>
      </td>
      <td><p class="TableText">Á</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Acirc</p>
      </td>
      <td><p class="TableText">194</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; A</p>
      </td>
      <td><p class="TableText">circunflejo</p>
      </td>
      <td><p class="TableText">Â</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Agrave</p>
      </td>
      <td><p class="TableText">192</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; A</p>
      </td>
      <td><p class="TableText">acento grave</p>
      </td>
      <td><p class="TableText">À</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Aring</p>
      </td>
      <td><p class="TableText">197</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; A</p>
      </td>
      <td><p class="TableText">multiplicación</p>
      </td>
      <td><p class="TableText">Å</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Atilde</p>
      </td>
      <td><p class="TableText">195</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; A</p>
      </td>
      <td><p class="TableText">tilde</p>
      </td>
      <td><p class="TableText">Ã</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Auml</p>
      </td>
      <td><p class="TableText">196</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; A</p>
      </td>
      <td><p class="TableText">comillas</p>
      </td>
      <td><p class="TableText">Ä</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ccedil</p>
      </td>
      <td><p class="TableText">199</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; C</p>
      </td>
      <td><p class="TableText">coma</p>
      </td>
      <td><p class="TableText">Ç</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Eth</p>
      </td>
      <td><p class="TableText">222</p>
      </td>
      <td><p class="TableText">-</p>
      </td>
      <td><p class="TableText">D</p>
      </td>
      <td><p class="TableText">Ð</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Eacute</p>
      </td>
      <td><p class="TableText">201</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; E</p>
      </td>
      <td><p class="TableText">apóstrofo</p>
      </td>
      <td><p class="TableText">É</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ecirc</p>
      </td>
      <td><p class="TableText">202</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; E</p>
      </td>
      <td><p class="TableText">circunflejo</p>
      </td>
      <td><p class="TableText">Ê</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Egrave</p>
      </td>
      <td><p class="TableText">200</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; E</p>
      </td>
      <td><p class="TableText">acento grave</p>
      </td>
      <td><p class="TableText">È</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Euml</p>
      </td>
      <td><p class="TableText">203</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; E</p>
      </td>
      <td><p class="TableText">comillas</p>
      </td>
      <td><p class="TableText">Ë</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Iacute</p>
      </td>
      <td><p class="TableText">205</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; I</p>
      </td>
      <td><p class="TableText">apóstrofo</p>
      </td>
      <td><p class="TableText">Í</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Icirc</p>
      </td>
      <td><p class="TableText">206</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; I</p>
      </td>
      <td><p class="TableText">circunflejo</p>
      </td>
      <td><p class="TableText">Î</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Igrave</p>
      </td>
      <td><p class="TableText">204</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; I</p>
      </td>
      <td><p class="TableText">acento grave</p>
      </td>
      <td><p class="TableText">Ì</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Iuml</p>
      </td>
      <td><p class="TableText">207</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; I</p>
      </td>
      <td><p class="TableText">comillas</p>
      </td>
      <td><p class="TableText">Ï</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ntilde</p>
      </td>
      <td><p class="TableText">209</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; N</p>
      </td>
      <td><p class="TableText">tilde</p>
      </td>
      <td><p class="TableText">Ñ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Oacute</p>
      </td>
      <td><p class="TableText">211</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; O</p>
      </td>
      <td><p class="TableText">apóstrofo</p>
      </td>
      <td><p class="TableText">Ó</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ocirc</p>
      </td>
      <td><p class="TableText">212</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; O</p>
      </td>
      <td><p class="TableText">circunflejo</p>
      </td>
      <td><p class="TableText">Ô</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ograve</p>
      </td>
      <td><p class="TableText">210</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; O</p>
      </td>
      <td><p class="TableText">acento grave</p>
      </td>
      <td><p class="TableText">Ò</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Oslash</p>
      </td>
      <td><p class="TableText">216</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; O</p>
      </td>
      <td><p class="TableText">slash</p>
      </td>
      <td><p class="TableText">Ø</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Otilde</p>
      </td>
      <td><p class="TableText">213</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; O</p>
      </td>
      <td><p class="TableText">tilde</p>
      </td>
      <td><p class="TableText">Õ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ouml</p>
      </td>
      <td><p class="TableText">214</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; O</p>
      </td>
      <td><p class="TableText">comillas</p>
      </td>
      <td><p class="TableText">Ö</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Thorn</p>
      </td>
      <td><p class="TableText">222</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; T</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; H</p>
      </td>
      <td><p class="TableText">Þ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Uacute</p>
      </td>
      <td><p class="TableText">205</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; U</p>
      </td>
      <td><p class="TableText">apóstrofo</p>
      </td>
      <td><p class="TableText">Ú</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ucirc</p>
      </td>
      <td><p class="TableText">206</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; U</p>
      </td>
      <td><p class="TableText">circunflejo</p>
      </td>
      <td><p class="TableText">Û</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ugrave</p>
      </td>
      <td><p class="TableText">204</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; U</p>
      </td>
      <td><p class="TableText">acento grave</p>
      </td>
      <td><p class="TableText">Ù</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Uuml</p>
      </td>
      <td><p class="TableText">207</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; U</p>
      </td>
      <td><p class="TableText">comillas</p>
      </td>
      <td><p class="TableText">Ü</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Yacute</p>
      </td>
      <td><p class="TableText">221</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; Y</p>
      </td>
      <td><p class="TableText">apóstrofo</p>
      </td>
      <td><p class="TableText">Ý</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">aelig</p>
      </td>
      <td><p class="TableText">230</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">e</p>
      </td>
      <td><p class="TableText">æ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">aacute</p>
      </td>
      <td><p class="TableText">225</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">apóstrofo</p>
      </td>
      <td><p class="TableText">á</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">acirc</p>
      </td>
      <td><p class="TableText">226</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">circunflejo</p>
      </td>
      <td><p class="TableText">â</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">agrave</p>
      </td>
      <td><p class="TableText">224</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">acento grave</p>
      </td>
      <td><p class="TableText">à</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">aring</p>
      </td>
      <td><p class="TableText">229</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">multiplicación</p>
      </td>
      <td><p class="TableText">å</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">atilde</p>
      </td>
      <td><p class="TableText">227</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">tilde</p>
      </td>
      <td><p class="TableText">ã</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">auml</p>
      </td>
      <td><p class="TableText">228</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">comillas</p>
      </td>
      <td><p class="TableText">ä</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">brvbar</p>
      </td>
      <td><p class="TableText">166</p>
      </td>
      <td><p class="TableText">bar (pipe)</p>
      </td>
      <td><p class="TableText">bar</p>
      </td>
      <td><p class="TableText">¦</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ccedil</p>
      </td>
      <td><p class="TableText">231</p>
      </td>
      <td><p class="TableText">c</p>
      </td>
      <td><p class="TableText">coma</p>
      </td>
      <td><p class="TableText">ç</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">cent</p>
      </td>
      <td><p class="TableText">162</p>
      </td>
      <td><p class="TableText">c</p>
      </td>
      <td><p class="TableText">slash</p>
      </td>
      <td><p class="TableText">¢</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">copy</p>
      </td>
      <td><p class="TableText">169</p>
      </td>
      <td><p class="TableText">c</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">©</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">current</p>
      </td>
      <td><p class="TableText">164</p>
      </td>
      <td><p class="TableText">x</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">¤</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">deg</p>
      </td>
      <td><p class="TableText">176</p>
      </td>
      <td><p class="TableText">circunflejo</p>
      </td>
      <td><p class="TableText">0 (cero)</p>
      </td>
      <td><p class="TableText">&deg;</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">eacute</p>
      </td>
      <td><p class="TableText">233</p>
      </td>
      <td><p class="TableText">e</p>
      </td>
      <td><p class="TableText">apóstrofo</p>
      </td>
      <td><p class="TableText">é</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ecirc</p>
      </td>
      <td><p class="TableText">234</p>
      </td>
      <td><p class="TableText">e</p>
      </td>
      <td><p class="TableText">circunflejo</p>
      </td>
      <td><p class="TableText">ê</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">egrave</p>
      </td>
      <td><p class="TableText">232</p>
      </td>
      <td><p class="TableText">e</p>
      </td>
      <td><p class="TableText">acento grave</p>
      </td>
      <td><p class="TableText">è</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">euml</p>
      </td>
      <td><p class="TableText">235</p>
      </td>
      <td><p class="TableText">e</p>
      </td>
      <td><p class="TableText">comillas</p>
      </td>
      <td><p class="TableText">ë</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">eth</p>
      </td>
      <td><p class="TableText">240</p>
      </td>
      <td><p class="TableText">-</p>
      </td>
      <td><p class="TableText">d</p>
      </td>
      <td><p class="TableText">ð</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">frac12</p>
      </td>
      <td><p class="TableText">189</p>
      </td>
      <td><p class="TableText">/</p>
      </td>
      <td><p class="TableText">2</p>
      </td>
      <td><p class="TableText">½</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">frac14</p>
      </td>
      <td><p class="TableText">188</p>
      </td>
      <td><p class="TableText">/</p>
      </td>
      <td><p class="TableText">4</p>
      </td>
      <td><p class="TableText">¼</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">frac34</p>
      </td>
      <td><p class="TableText">190</p>
      </td>
      <td><p class="TableText">/</p>
      </td>
      <td><p class="TableText">3</p>
      </td>
      <td><p class="TableText">¾</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">hyphen</p>
      </td>
      <td><p class="TableText">173</p>
      </td>
      <td><p class="TableText">- (menos)</p>
      </td>
      <td><p class="TableText">-</p>
      </td>
      <td><p class="TableText">­</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">iacute</p>
      </td>
      <td><p class="TableText">237</p>
      </td>
      <td><p class="TableText">i</p>
      </td>
      <td><p class="TableText">apóstrofo</p>
      </td>
      <td><p class="TableText">í</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">icirc</p>
      </td>
      <td><p class="TableText">238</p>
      </td>
      <td><p class="TableText">i</p>
      </td>
      <td><p class="TableText">circunflejo</p>
      </td>
      <td><p class="TableText">î</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">igrave</p>
      </td>
      <td><p class="TableText">236</p>
      </td>
      <td><p class="TableText">i</p>
      </td>
      <td><p class="TableText">acento grave</p>
      </td>
      <td><p class="TableText">ì</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">iexcl</p>
      </td>
      <td><p class="TableText">161</p>
      </td>
      <td><p class="TableText">! (exclamación)</p>
      </td>
      <td><p class="TableText">!</p>
      </td>
      <td><p class="TableText">¡</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">iquest</p>
      </td>
      <td><p class="TableText">191</p>
      </td>
      <td><p class="TableText">? (interrogación)</p>
      </td>
      <td><p class="TableText">?</p>
      </td>
      <td><p class="TableText">¿</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">laquo</p>
      </td>
      <td><p class="TableText">171</p>
      </td>
      <td><p class="TableText">&lt; (inferior)</p>
      </td>
      <td><p class="TableText">&lt;</p>
      </td>
      <td><p class="TableText">«</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">macr</p>
      </td>
      <td><p class="TableText">175</p>
      </td>
      <td><p class="TableText">- (menos)</p>
      </td>
      <td><p class="TableText">circunflejo</p>
      </td>
      <td><p class="TableText">¯</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">micro</p>
      </td>
      <td><p class="TableText">181</p>
      </td>
      <td><p class="TableText">/</p>
      </td>
      <td><p class="TableText">u</p>
      </td>
      <td><p class="TableText">µ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">middot</p>
      </td>
      <td><p class="TableText">183</p>
      </td>
      <td><p class="TableText">.</p>
      </td>
      <td><p class="TableText">circunflejo</p>
      </td>
      <td><p class="TableText">·</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">nbsp</p>
      </td>
      <td><p class="TableText">160</p>
      </td>
      <td><p class="TableText">&lt;Ctrl&gt; space</p>
      </td>
      <td><p class="TableText">(ninguna)</p>
      </td>
      <td><p class="TableText">&nbsp;</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">not</p>
      </td>
      <td><p class="TableText">172</p>
      </td>
      <td><p class="TableText">- (menos)</p>
      </td>
      <td><p class="TableText">coma</p>
      </td>
      <td><p class="TableText">¬</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ntilde</p>
      </td>
      <td><p class="TableText">241</p>
      </td>
      <td><p class="TableText">n</p>
      </td>
      <td><p class="TableText">tilde</p>
      </td>
      <td><p class="TableText">ñ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">oacute</p>
      </td>
      <td><p class="TableText">243</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">apóstrofo</p>
      </td>
      <td><p class="TableText">ó</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ocirc</p>
      </td>
      <td><p class="TableText">244</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">circunflejo</p>
      </td>
      <td><p class="TableText">ô</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ograve</p>
      </td>
      <td><p class="TableText">242</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">acento grave</p>
      </td>
      <td><p class="TableText">ò</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">oslash</p>
      </td>
      <td><p class="TableText">248</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">slash</p>
      </td>
      <td><p class="TableText">ø</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">otilde</p>
      </td>
      <td><p class="TableText">245</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">tilde</p>
      </td>
      <td><p class="TableText">õ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ouml</p>
      </td>
      <td><p class="TableText">246</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">comillas</p>
      </td>
      <td><p class="TableText">ö</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ordf</p>
      </td>
      <td><p class="TableText">170</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">_ (subrayado)</p>
      </td>
      <td><p class="TableText">ª</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ordm</p>
      </td>
      <td><p class="TableText">186</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">_ (subrayado)</p>
      </td>
      <td><p class="TableText">º</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">para</p>
      </td>
      <td><p class="TableText">182</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; P</p>
      </td>
      <td><p class="TableText">! (exclamación)</p>
      </td>
      <td><p class="TableText">¶</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">plusmn</p>
      </td>
      <td><p class="TableText">177</p>
      </td>
      <td><p class="TableText">+</p>
      </td>
      <td><p class="TableText">- (menos)</p>
      </td>
      <td><p class="TableText">±</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">pound</p>
      </td>
      <td><p class="TableText">163</p>
      </td>
      <td><p class="TableText">l</p>
      </td>
      <td><p class="TableText">- (menos)</p>
      </td>
      <td><p class="TableText">£</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">raquo</p>
      </td>
      <td><p class="TableText">187</p>
      </td>
      <td><p class="TableText">&gt; (superior)</p>
      </td>
      <td><p class="TableText">&gt;</p>
      </td>
      <td><p class="TableText">»</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">reg</p>
      </td>
      <td><p class="TableText">174</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; R</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; O</p>
      </td>
      <td><p class="TableText">®</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">sec</p>
      </td>
      <td><p class="TableText">167</p>
      </td>
      <td><p class="TableText">s</p>
      </td>
      <td><p class="TableText">! (exclamación)</p>
      </td>
      <td><p class="TableText">§</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">sup1</p>
      </td>
      <td><p class="TableText">185</p>
      </td>
      <td><p class="TableText">s</p>
      </td>
      <td><p class="TableText">1</p>
      </td>
      <td><p class="TableText">¹</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">sup2</p>
      </td>
      <td><p class="TableText">178</p>
      </td>
      <td><p class="TableText">s</p>
      </td>
      <td><p class="TableText">2</p>
      </td>
      <td><p class="TableText">²</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">sup3</p>
      </td>
      <td><p class="TableText">179</p>
      </td>
      <td><p class="TableText">s</p>
      </td>
      <td><p class="TableText">3</p>
      </td>
      <td><p class="TableText">³</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">szlig</p>
      </td>
      <td><p class="TableText">223</p>
      </td>
      <td><p class="TableText">s</p>
      </td>
      <td><p class="TableText">s</p>
      </td>
      <td><p class="TableText">ß</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">thorn</p>
      </td>
      <td><p class="TableText">254</p>
      </td>
      <td><p class="TableText">t</p>
      </td>
      <td><p class="TableText">h</p>
      </td>
      <td><p class="TableText">þ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">uacute</p>
      </td>
      <td><p class="TableText">250</p>
      </td>
      <td><p class="TableText">u</p>
      </td>
      <td><p class="TableText">apóstrofo</p>
      </td>
      <td><p class="TableText">ú</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ucirc</p>
      </td>
      <td><p class="TableText">251</p>
      </td>
      <td><p class="TableText">u</p>
      </td>
      <td><p class="TableText">circunflejo</p>
      </td>
      <td><p class="TableText">û</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ugrave</p>
      </td>
      <td><p class="TableText">249</p>
      </td>
      <td><p class="TableText">u</p>
      </td>
      <td><p class="TableText">acento grave</p>
      </td>
      <td><p class="TableText">ù</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">uuml</p>
      </td>
      <td><p class="TableText">252</p>
      </td>
      <td><p class="TableText">u</p>
      </td>
      <td><p class="TableText">comillas</p>
      </td>
      <td><p class="TableText">ü</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">yacute</p>
      </td>
      <td><p class="TableText">253</p>
      </td>
      <td><p class="TableText">y</p>
      </td>
      <td><p class="TableText">apóstrofo</p>
      </td>
      <td><p class="TableText">ý</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">yen</p>
      </td>
      <td><p class="TableText">165</p>
      </td>
      <td><p class="TableText">y</p>
      </td>
      <td><p class="TableText">- (menos)</p>
      </td>
      <td><p class="TableText">¥</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">yuml</p>
      </td>
      <td><p class="TableText">255</p>
      </td>
      <td><p class="TableText">y</p>
      </td>
      <td><p class="TableText">comillas</p>
      </td>
      <td><p class="TableText">ÿ</p>
      </td>
    </tr>
  </tbody>
</table>
</div>
</body>
</html>
