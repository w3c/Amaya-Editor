<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <title>&#xca;&#xee;&#xed;&#xf4;&#xe8;&#xe3;&#xf3;&#xf0;&#xe8;&#xf0;&#xee;&#xe2;&#xe0;&#xed;&#xe8;&#xe5;
  &#xe8;&#xea;&#xee;&#xed;&#xee;&#xea;
  &#xe0;&#xed;&#xed;&#xee;&#xf2;&#xe0;&#xf6;&#xe8;&#xe8;</title>
  <style type="text/css">

  </style>
  <meta name="GENERATOR" content="amaya 8.5, see http://www.w3.org/Amaya/" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="replying_to_annotations.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"
        /></a> <a href="../Annotations.html.ru" accesskey="t"><img
        alt="Вверх" src="../../images/up.gif" /></a> <a
        href="annotation_issues.html.ru" accesskey="n"><img alt="Следующий"
        src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Конфигурирование иконок аннотации</h1>

<h2>Определяемые пользователем типы иконок аннотации (также известны под
именем "динамические иконки")</h2>

<p>Как и в версии 6.2, иконки используются для маркировки местоположения
аннотации в пределах аннотируемого документа и могут быть изменены
пользователем.</p>

<p>В версии 6.2 иконка обозначает аннотацию и выбирается как свойства типа
аннотации. Для каждого использующегося типа аннотации, включая RDF свойство,
вы выбираете иконку связанную с аннотацией этого типа.</p>

<p>Коллекция иконок в версии 6.2 связана с определёнными типами аннотаций и
приведена ниже:</p>

<table>
  <tbody>
    <tr>
      <td width="35"><img src="../../../amaya/advice.png" alt="Совет" /></td>
      <td>Advice (Совет)</td>
    </tr>
    <tr>
      <td width="35"><img src="../../../amaya/change.png" alt="Изменение"
      /></td>
      <td>Change (Изменение)</td>
    </tr>
    <tr>
      <td width="35"><img src="../../../amaya/annot.png" alt="Комментарий"
      /></td>
      <td>Comment (Комментарий)</td>
    </tr>
    <tr>
      <td width="35"><img src="../../../amaya/example.png" alt="Пример"
      /></td>
      <td>Example (Пример)</td>
    </tr>
    <tr>
      <td width="35"><img src="../../../amaya/explanation.png"
        alt="Объяснение" /></td>
      <td>Explanation (Объяснение)</td>
    </tr>
    <tr>
      <td width="35"><img src="../../../amaya/question.png" alt="Вопрос"
      /></td>
      <td>Question (Вопрос)</td>
    </tr>
    <tr>
      <td width="35"><img src="../../../amaya/seealso.png"
        alt="Смотрите также" /></td>
      <td>SeeAlso (Смотрите также)</td>
    </tr>
  </tbody>
</table>

<p>Именем свойства для связывания иконок с аннотациями является <a
href="http://www.w3.org/2001/10/typeIcon#usesIcon">http://www.w3.org/2001/10/typeIcon#usesIcon</a>.
Например, для определения иконки имеющей файл с названием:
///home/question-icon.jpg для аннотаций имеющих тип <a
href="http://www.w3.org/2000/10/annotationType#Question">http://www.w3.org/2000/10/annotationType#Question</a>
вы должны ввести следующее RDF/XML описание в файл, который Amaya читает при
загрузке:</p>
<pre>&lt;rdf:RDF

   xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"

   xmlns:i = "http://www.w3.org/2001/10/typeIcon#"&gt;

&lt;rdf:Description rdf:about="http://www.w3.org/2000/10/annotationType#Question"&gt;

  &lt;i:usesIcon rdf:resource="file:///home/question-icon.jpg" /&gt;

&lt;/rdf:Description&gt;

&lt;/rdf:RDF&gt;</pre>

<p>Простейшим способом получить такой RDF это загрузить его в Amaya при
загрузке файла config/annot.schemas из програмной директории Amaya. Для того
чтобы сохранить этот файл и чтобы он не был перезаписан, когда вы
устанавливаете новую версию Amaya, вы должны скопировать файл
config/annot.schemas в вашу персональную домашнюю (home) директорию Amaya;
~/.amaya/annot.schemas (на Unix системах) или
/winnt/profiles/&lt;username&gt;/amaya/annot.schemas (на Microsoft Windows
системах). Вы можете указывать как угодно много RDF описаний в файле
annot.schemas. Для более подробных сведений смотрите комментарии в файле,
находящемся в дистрибутиве Amaya.</p>

<p>Версия 6.2 включает в себя пример файла, именуемый "typeIcon.rdf", который
объявляет уникальность иконок для каждого объявляемого типа аннотации в
пространстве имён <a
href="http://www.w3.org/2000/10/annotationType#">http://www.w3.org/2000/10/annotationType#</a>.
Для эксперимента с определяемыми пользователем иконками, будет лучше
скопировать typeIcon.rdf в другую директорию и там изменить его. Скопировать
annot.schemas в вашу домашнюю (home) Amaya и изменить линию в конце, чтобы
указать на ваш пересмотренный файл определения иконок.</p>

<p>Чтобы вернуться к предыдущим настройкам, предшествующим версии 6.2,
отредактируйте config/annot.schemas в инсталяционной директории Amaya и
добавьте символ комментария ("#") в начало линии, а в конце линии то, что
имеет отношение к typeIcon.rdf:</p>
<pre>#user-defined icons

#http://www.w3.org/2001/10/typeIcon# $THOTDIR/config/typeIcon.rdf</pre>

<p>Amaya поддерживает JPEG, PNG, и GIF графические форматы изображения
иконок. В версии 6.2 иконка URI может быть только файлом URI; это значит, что
иконка должна появляться в локальной или монтируемой директории Amaya.
Поддерживаются две специальные формы для нефайловых URIs. Если path name
файла начинается с "$THOTDIR" или "$APP_HOME" то оно связано с директорией
инсталяции Amaya или персональной домашней (home) директорией Amaya
заменённой в pathname.</p>
</div>
</body>
</html>
