<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <title>Что такое аннотация?
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
      <td><p align="right"><a href="../Annotations.html.ru"
        accesskey="t"><img alt="Вверх" src="../../images/up.gif" /></a> <a
        href="about_reading_annotations.html.ru" accesskey="n"><img
        alt="Следующий" src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1><a name="What">Что такое аннотация?</a></h1>

<p>Aннотации это комментарии, примечания, объяснения, и другие типы внешних
ремарок которые могут быть прикреплены к веб документу или к части документа.
Так как они являются внешними, то возможно аннотировать любой веб документ
без непосредственного редактирования самого документа. С технической точки
зрения, аннотации обычно представлены как <strong>метаданные,</strong> так
как они дают дополнительную информацию о существующей части данных. В этом
проекте, для описания аннотаций, мы используем специальную <strong><a
href="http://www.w3.org/RDF/">RDF</a></strong> <a
href="http://www.w3.org/2000/10/annotation-ns">схему аннотирования</a>.</p>

<p>Aннотации могут сохраняться локально или на одном или более
<strong>серверов аннотации</strong>. Когда документ просматривается, Amaya
посылает запрос на каждый из этих серверов, запрашивая аннотации имеющие
отношения к этому документу. Текущая версия Amaya выделяет аннотацию в виде
изображения карандаша ( <img src="../../images/annot.png"
alt="Иконка карандаш аннотации" /> ) которое визуально вставляется в
документ, как это показано ниже. Если пользователь щёлкает по этому
изображению, то текст имеющий аннотацию, выделяется подсветкой. Если
пользователь дважды щёлкает по изображению, то в отдельном окне выводится
текст аннотации и другие метаданные.</p>

<p style="text-align: center"><img src="../../images/annotationicon.png"
alt="иконка аннотации (= pencil)" /></p>

<p>Aннотация имеет много свойств, включая:</p>
<ul>
  <li><strong>Физичес:ое местоположение</strong>: или аннотации хранятся в
    локальной фаловой системе или на сервере аннотаций</li>
  <li><strong>Область</strong>: или аннотация связана с целым документом или
    с каким то фрагментом документа</li>
  <li><strong>Тип аннотации</strong>: 'Annotation-аннотация',
    'Comment-комментарий', 'Query-запрос', ...</li>
</ul>
</div>
</body>
</html>
