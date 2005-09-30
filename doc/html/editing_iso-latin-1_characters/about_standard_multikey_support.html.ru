<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <title>Стандартные комбинации кнопок для версий GTK
</title>
  <meta name="GENERATOR" content="amaya 8.5, see http://www.w3.org/Amaya/" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a
        href="entering_ISOLatin1_characters_in_amaya.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"
        /></a> <a href="../EditChar.html.ru" accesskey="t"><img alt="Вверх"
        src="../../images/up.gif" /></a> <a
        href="about_white_space_handling.html.ru" accesskey="n"><img
        alt="Следующий" src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Стандартные комбинации кнопок для версий GTK</h1>

<p>Если ISO-Latin символ отсутствует на клавиатуре, вы можете вставить его
нажав специальный композиционный <kbd>Compose</kbd> ключ (кнопку),
состоящий из последовательности двух других ключей. Смотрите нижеприведённую
таблицу, использующуюся для ввода различных ISO-Latin символов. Заметьте что
в Amaya, вы можете переключать позицию первого и второго ключа.</p>

<p>Если ваша клавиатура не содержит композиционного <kbd>Compose</kbd>
ключа (кнопки), вы можете прикрепить его к существующему ключу, такому как
<kbd>Alt-Gr</kbd> с помощью команды <code>xmodmap</code>:</p>

<p><code>keycode 113 = Multi_key</code></p>

<p>Вы можете поместить эту команду в <code>$HOME/.Xmodmap</code> файл (в
Unix), и Amaya использует его когда вы запустите свой X сервер.</p>

<table border="0" cellspacing="0" cellpadding="0" class="characters">
  <thead>
    <tr>
      <td style="border:solid .5pt; padding:0in 5.4pt 0in 5.4pt"><p
        class="TableHead"><strong>Имя</strong></p>
      </td>
      <td
      style="border:solid .5pt;  border-left:none;padding:0in 5.4pt 0in 5.4pt"><p
        class="TableHead"><strong>Код</strong></p>
      </td>
      <td
      style="border:solid .5pt;  border-left:none;padding:0in 5.4pt 0in 5.4pt"><p
        class="TableHead"><strong>Первый/Второй ключ</strong></p>
      </td>
      <td
      style="border:solid .5pt;  border-left:none;padding:0in 5.4pt 0in 5.4pt"><p
        class="TableHead"><strong>Второй/Первый ключ</strong></p>
      </td>
      <td
      style="border:solid .5pt;  border-left:none;padding:0in 5.4pt 0in 5.4pt"><p
        class="TableHead"><strong>Выходные данные</strong></p>
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
      <td><p class="TableText">Ж</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Aacute</p>
      </td>
      <td><p class="TableText">193</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; A</p>
      </td>
      <td><p class="TableText">quote (кавычки)</p>
      </td>
      <td><p class="TableText">Б</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Acirc</p>
      </td>
      <td><p class="TableText">194</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; A</p>
      </td>
      <td><p class="TableText">circumflex (циркумфлекс, диакритический знак
        над гласной)</p>
      </td>
      <td><p class="TableText">В</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Agrave</p>
      </td>
      <td><p class="TableText">192</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; A</p>
      </td>
      <td><p class="TableText">backquote (обратные кавычки)</p>
      </td>
      <td><p class="TableText">А</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Aring</p>
      </td>
      <td><p class="TableText">197</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; A</p>
      </td>
      <td><p class="TableText">star (звёздочка)</p>
      </td>
      <td><p class="TableText">Е</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Atilde</p>
      </td>
      <td><p class="TableText">195</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; A</p>
      </td>
      <td><p class="TableText">tilde (тильда)</p>
      </td>
      <td><p class="TableText">Г</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Auml</p>
      </td>
      <td><p class="TableText">196</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; A</p>
      </td>
      <td><p class="TableText">double quote (двойные кавычки)</p>
      </td>
      <td><p class="TableText">Д</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ccedil</p>
      </td>
      <td><p class="TableText">199</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; C</p>
      </td>
      <td><p class="TableText">comma (запятая)</p>
      </td>
      <td><p class="TableText">З</p>
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
      <td><p class="TableText">Р</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Eacute</p>
      </td>
      <td><p class="TableText">201</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; E</p>
      </td>
      <td><p class="TableText">quote (кавычка)</p>
      </td>
      <td><p class="TableText">Й</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ecirc</p>
      </td>
      <td><p class="TableText">202</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; E</p>
      </td>
      <td><p class="TableText">circumflex (циркумфлекс, диакритический знак
        над гласной)</p>
      </td>
      <td><p class="TableText">К</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Egrave</p>
      </td>
      <td><p class="TableText">200</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; E</p>
      </td>
      <td><p class="TableText">backquote (обратные кавычки)</p>
      </td>
      <td><p class="TableText">И</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Euml</p>
      </td>
      <td><p class="TableText">203</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; E</p>
      </td>
      <td><p class="TableText">double quote (двойные кавычки)</p>
      </td>
      <td><p class="TableText">Л</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Iacute</p>
      </td>
      <td><p class="TableText">205</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; I</p>
      </td>
      <td><p class="TableText">quote (кавычки)</p>
      </td>
      <td><p class="TableText">Н</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Icirc</p>
      </td>
      <td><p class="TableText">206</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; I</p>
      </td>
      <td><p class="TableText">circumflex (циркумфлекс, диакритический знак
        над гласной)</p>
      </td>
      <td><p class="TableText">О</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Igrave</p>
      </td>
      <td><p class="TableText">204</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; I</p>
      </td>
      <td><p class="TableText">backquote (обратные кавычки)</p>
      </td>
      <td><p class="TableText">М</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Iuml</p>
      </td>
      <td><p class="TableText">207</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; I</p>
      </td>
      <td><p class="TableText">double quote (двойные кавычки)</p>
      </td>
      <td><p class="TableText">П</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ntilde</p>
      </td>
      <td><p class="TableText">209</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; N</p>
      </td>
      <td><p class="TableText">tilde (тильда)</p>
      </td>
      <td><p class="TableText">С</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Oacute</p>
      </td>
      <td><p class="TableText">211</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; O</p>
      </td>
      <td><p class="TableText">quote (кавычки)</p>
      </td>
      <td><p class="TableText">У</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ocirc</p>
      </td>
      <td><p class="TableText">212</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; O</p>
      </td>
      <td><p class="TableText">circumflex (циркумфлекс, диакритический знак
        над гласной)</p>
      </td>
      <td><p class="TableText">Ф</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ograve</p>
      </td>
      <td><p class="TableText">210</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; O</p>
      </td>
      <td><p class="TableText">backquote (обратные кавычки)</p>
      </td>
      <td><p class="TableText">Т</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Oslash</p>
      </td>
      <td><p class="TableText">216</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; O</p>
      </td>
      <td><p class="TableText">slash (слэш) наклонная черта с правым
        уклоном</p>
      </td>
      <td><p class="TableText">Ш</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Otilde</p>
      </td>
      <td><p class="TableText">213</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; O</p>
      </td>
      <td><p class="TableText">tilde (тильда)</p>
      </td>
      <td><p class="TableText">Х</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ouml</p>
      </td>
      <td><p class="TableText">214</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; O</p>
      </td>
      <td><p class="TableText">double quote (двойные кавычки)</p>
      </td>
      <td><p class="TableText">Ц</p>
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
      <td><p class="TableText">Ю</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Uacute</p>
      </td>
      <td><p class="TableText">205</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; U</p>
      </td>
      <td><p class="TableText">quote (кавычки)</p>
      </td>
      <td><p class="TableText">Ъ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ucirc</p>
      </td>
      <td><p class="TableText">206</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; U</p>
      </td>
      <td><p class="TableText">circumflex (циркумфлекс, диакритический знак
        над гласной)</p>
      </td>
      <td><p class="TableText">Ы</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Ugrave</p>
      </td>
      <td><p class="TableText">204</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; U</p>
      </td>
      <td><p class="TableText">backquote (обратные кавычки)</p>
      </td>
      <td><p class="TableText">Щ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Uuml</p>
      </td>
      <td><p class="TableText">207</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; U</p>
      </td>
      <td><p class="TableText">double quote (двойные кавычки)</p>
      </td>
      <td><p class="TableText">Ь</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">Yacute</p>
      </td>
      <td><p class="TableText">221</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; Y</p>
      </td>
      <td><p class="TableText">quote (кавычки)</p>
      </td>
      <td><p class="TableText">Э</p>
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
      <td><p class="TableText">ж</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">aacute</p>
      </td>
      <td><p class="TableText">225</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">quote (кавычки)</p>
      </td>
      <td><p class="TableText">б</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">acirc</p>
      </td>
      <td><p class="TableText">226</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">circumflex (циркумфлекс, диакритический знак
        над гласной)</p>
      </td>
      <td><p class="TableText">в</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">agrave</p>
      </td>
      <td><p class="TableText">224</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">backquote (обратные кавычки)</p>
      </td>
      <td><p class="TableText">а</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">aring</p>
      </td>
      <td><p class="TableText">229</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">star (звёздочка)</p>
      </td>
      <td><p class="TableText">е</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">atilde</p>
      </td>
      <td><p class="TableText">227</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">tilde (тильда)</p>
      </td>
      <td><p class="TableText">г</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">auml</p>
      </td>
      <td><p class="TableText">228</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">double quote (двойные кавычки)</p>
      </td>
      <td><p class="TableText">д</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">brvbar</p>
      </td>
      <td><p class="TableText">166</p>
      </td>
      <td><p class="TableText">bar (pipe)</p>
      </td>
      <td><p class="TableText">bar (бар ( единица атмосферного или
        акустического давления)</p>
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
      <td><p class="TableText">comma (запятая)</p>
      </td>
      <td><p class="TableText">з</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">cent</p>
      </td>
      <td><p class="TableText">162</p>
      </td>
      <td><p class="TableText">c</p>
      </td>
      <td><p class="TableText">slash (слэш) наклонная черта с правым
        уклоном</p>
      </td>
      <td><p class="TableText">ў</p>
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
      <td><p class="TableText">circumflex</p>
      </td>
      <td><p class="TableText">0 (zero) (ноль)</p>
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
      <td><p class="TableText">quote (кавычки)</p>
      </td>
      <td><p class="TableText">й</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ecirc</p>
      </td>
      <td><p class="TableText">234</p>
      </td>
      <td><p class="TableText">e</p>
      </td>
      <td><p class="TableText">circumflex (циркумфлекс, диакритический знак
        над гласной)</p>
      </td>
      <td><p class="TableText">к</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">egrave</p>
      </td>
      <td><p class="TableText">232</p>
      </td>
      <td><p class="TableText">e</p>
      </td>
      <td><p class="TableText">backquote (обратные кавычки)</p>
      </td>
      <td><p class="TableText">и</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">euml</p>
      </td>
      <td><p class="TableText">235</p>
      </td>
      <td><p class="TableText">e</p>
      </td>
      <td><p class="TableText">double quote (двойные кавычки)</p>
      </td>
      <td><p class="TableText">л</p>
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
      <td><p class="TableText">р</p>
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
      <td><p class="TableText">Ѕ</p>
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
      <td><p class="TableText">ј</p>
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
      <td><p class="TableText">ѕ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">hyphen</p>
      </td>
      <td><p class="TableText">173</p>
      </td>
      <td><p class="TableText">- (minus)</p>
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
      <td><p class="TableText">quote (кавычки)</p>
      </td>
      <td><p class="TableText">н</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">icirc</p>
      </td>
      <td><p class="TableText">238</p>
      </td>
      <td><p class="TableText">i</p>
      </td>
      <td><p class="TableText">circumflex (циркумфлекс, диакритический знак
        над гласной)</p>
      </td>
      <td><p class="TableText">о</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">igrave</p>
      </td>
      <td><p class="TableText">236</p>
      </td>
      <td><p class="TableText">i</p>
      </td>
      <td><p class="TableText">backquote (обратные кавычки)</p>
      </td>
      <td><p class="TableText">м</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">iexcl</p>
      </td>
      <td><p class="TableText">161</p>
      </td>
      <td><p class="TableText">! (exclaim)</p>
      </td>
      <td><p class="TableText">!</p>
      </td>
      <td><p class="TableText">Ў</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">iquest</p>
      </td>
      <td><p class="TableText">191</p>
      </td>
      <td><p class="TableText">? (question)</p>
      </td>
      <td><p class="TableText">?</p>
      </td>
      <td><p class="TableText">ї</p>
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
      <td><p class="TableText">- (minus)</p>
      </td>
      <td><p class="TableText">circumflex (циркумфлекс, диакритический знак
        над гласной)</p>
      </td>
      <td><p class="TableText">Ї</p>
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
      <td><p class="TableText">circumflex (циркумфлекс, диакритический знак
        над гласной)</p>
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
      <td><p class="TableText">(none) (пробел)</p>
      </td>
      <td><p class="TableText">&nbsp;</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">not</p>
      </td>
      <td><p class="TableText">172</p>
      </td>
      <td><p class="TableText">- (minus)</p>
      </td>
      <td><p class="TableText">comma (запятая)</p>
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
      <td><p class="TableText">tilde (тильда)</p>
      </td>
      <td><p class="TableText">с</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">oacute</p>
      </td>
      <td><p class="TableText">243</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">quote (кавычки)</p>
      </td>
      <td><p class="TableText">у</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ocirc</p>
      </td>
      <td><p class="TableText">244</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">circumflex (циркумфлекс, диакритический знак
        над гласной)</p>
      </td>
      <td><p class="TableText">ф</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ograve</p>
      </td>
      <td><p class="TableText">242</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">backquote (обратные кавычки)</p>
      </td>
      <td><p class="TableText">т</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">oslash</p>
      </td>
      <td><p class="TableText">248</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">slash (слэш) наклонная черта с правым
        уклоном</p>
      </td>
      <td><p class="TableText">ш</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">otilde</p>
      </td>
      <td><p class="TableText">245</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">tilde (тильда)</p>
      </td>
      <td><p class="TableText">х</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ouml</p>
      </td>
      <td><p class="TableText">246</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">double quote (двойные кавычки)</p>
      </td>
      <td><p class="TableText">ц</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ordf</p>
      </td>
      <td><p class="TableText">170</p>
      </td>
      <td><p class="TableText">a</p>
      </td>
      <td><p class="TableText">_ (underscore) (подчёркивание)</p>
      </td>
      <td><p class="TableText">Є</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ordm</p>
      </td>
      <td><p class="TableText">186</p>
      </td>
      <td><p class="TableText">o</p>
      </td>
      <td><p class="TableText">_ (underscore) (подчёркивание)</p>
      </td>
      <td><p class="TableText">є</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">para</p>
      </td>
      <td><p class="TableText">182</p>
      </td>
      <td><p class="TableText">&lt;Shift&gt; P</p>
      </td>
      <td><p class="TableText">! (exclam) (восклицательный знак)</p>
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
      <td><p class="TableText">- (minus) (минус)</p>
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
      <td><p class="TableText">- (minus) (минус)</p>
      </td>
      <td><p class="TableText">Ј</p>
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
      <td><p class="TableText">! (exclam) (восклицательный знак)</p>
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
      <td><p class="TableText">№</p>
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
      <td><p class="TableText">І</p>
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
      <td><p class="TableText">і</p>
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
      <td><p class="TableText">Я</p>
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
      <td><p class="TableText">ю</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">uacute</p>
      </td>
      <td><p class="TableText">250</p>
      </td>
      <td><p class="TableText">u</p>
      </td>
      <td><p class="TableText">quote (кавычки)</p>
      </td>
      <td><p class="TableText">ъ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ucirc</p>
      </td>
      <td><p class="TableText">251</p>
      </td>
      <td><p class="TableText">u</p>
      </td>
      <td><p class="TableText">circumflex (циркумфлекс, диакритический знак
        над гласной)</p>
      </td>
      <td><p class="TableText">ы</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">ugrave</p>
      </td>
      <td><p class="TableText">249</p>
      </td>
      <td><p class="TableText">u</p>
      </td>
      <td><p class="TableText">backquote (обратные кавычки)</p>
      </td>
      <td><p class="TableText">щ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">uuml</p>
      </td>
      <td><p class="TableText">252</p>
      </td>
      <td><p class="TableText">u</p>
      </td>
      <td><p class="TableText">double quote (двойные кавычки)</p>
      </td>
      <td><p class="TableText">ь</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">yacute</p>
      </td>
      <td><p class="TableText">253</p>
      </td>
      <td><p class="TableText">y</p>
      </td>
      <td><p class="TableText">quote (кавычки)</p>
      </td>
      <td><p class="TableText">э</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">yen</p>
      </td>
      <td><p class="TableText">165</p>
      </td>
      <td><p class="TableText">y</p>
      </td>
      <td><p class="TableText">- (minus) (минус)</p>
      </td>
      <td><p class="TableText">Ґ</p>
      </td>
    </tr>
    <tr>
      <td><p class="TableText">yuml</p>
      </td>
      <td><p class="TableText">255</p>
      </td>
      <td><p class="TableText">y</p>
      </td>
      <td><p class="TableText">double quote (двойные кавычки)</p>
      </td>
      <td><p class="TableText">я</p>
      </td>
    </tr>
  </tbody>
</table>
</div>
</body>
</html>
