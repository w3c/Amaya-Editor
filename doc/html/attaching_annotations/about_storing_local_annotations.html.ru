<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <title>&#xce; &#xeb;&#xee;&#xea;&#xe0;&#xeb;&#xfc;&#xed;&#xfb;&#xf5;
  &#xe0;&#xed;&#xed;&#xee;&#xf2;&#xe0;&#xf6;&#xe8;&#xff;&#xf5;</title>
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
      <td><p align="right"><a href="about_reading_annotations.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"
        /></a> <a href="../Annotations.html.ru" accesskey="t"><img
        alt="Вверх" src="../../images/up.gif" /></a> <a
        href="about_storing_remote_annotations.html.ru" accesskey="n"><img
        alt="Следующий" src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>О локальных аннотациях</h1>

<p>Amaya может хранить данные аннотаций в локальной фаловой системе (часто
называются "local annotations") или может хранить аннотации <span
style="color:blue"><a
href="about_storing_remote_annotations.html.ru">удалённо</a></span>,
осуществляя доступ к ним через World Wide Web (часто называются "remote
annotations").</p>

<p>Для хранения локальных аннотаций вам не нужен сервер. Локальные аннотации
хранятся внутри пользовательской директории настроек, в специальной
директории называемой <strong>annotations</strong>, и могут просматриваться
только их владельцем (согласно установленных прав доступа к системе). Эта
директория содержит три типа файлов:</p>
<ul>
  <li><strong>annot.index:</strong> Связывает URLs с файлами, содержащими
    метаданные аннотаций.</li>
  <li><strong>index + random suffix:</strong> Хранит метаданные аннотаций
    имеющих отношение к данному URL. Метаданные определяются с использованием
    RDF. <br />
  </li>
  <li><strong>annot + random suffix.html:</strong> Coдержание аннотации
    сохраняется как XHTML.</li>
</ul>

<p>В любое время, вы можете преобразовать локальную аннотацию в общедоступную
выбрав пункт <strong>Опубликовать (Post) на сервере</strong> из меню
<strong>Аннотации</strong>. Как только это будет выполнено, локальная
аннотация будет удалена, потому что она будет перемещена на сервер
аннотаций.</p>

<h2>Смотрите также:</h2>
<ul>
  <li><span class="Link0"><a
    href="about_storing_remote_annotations.html.ru">Об удалённых
    аннотациях</a></span></li>
</ul>
</div>
</body>
</html>
