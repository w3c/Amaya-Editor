<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <meta name="GENERATOR" content="amaya 8.5, see http://www.w3.org/Amaya/" />
  <title>&#xd1;&#xee;&#xe7;&#xe4;&#xe0;&#xed;&#xe8;&#xe5;
  &#xe0;&#xed;&#xed;&#xee;&#xf2;&#xe0;&#xf6;&#xe8;&#xe8;</title>
  <style type="text/css">

  </style>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="configuring_annotation_settings.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"
        /></a> <a href="../Annotations.html.ru" accesskey="t"><img
        alt="Вверх" src="../../images/up.gif" /></a> <a
        href="deleting_an_annotation.html.ru" accesskey="n"><img
        alt="Следующий" src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div class="Section1" id="page_body">
<h1>Создание аннотации</h1>

<p>Эта версия Amaya поддерживает два типа аннотаций: аннотации применяемые ко
целому документу и аннотации применяемые к определённой точке или выбранной
части документа.</p>
<ul>
  <li>Для аннотации целого документа, выберите пункт <strong>Аннотация
    документа</strong> из меню <strong>Аннотации</strong>.
    <p></p>
  </li>
  <li>Для аннотации определённой точки, укажите курсором точку ввода в любом
    месте документа и выберите пункт <strong>Аннотация выбранного</strong> из
    меню <strong>Аннотации</strong>.
    <p></p>
  </li>
  <li>Для аннотации выбранного, выделите любую часть документа и выберите
    пункт <strong>Аннотация выбранного</strong> из меню
    <strong>Аннотации</strong>.</li>
</ul>

<p>После выполнения любой из этих опций, выводится диалоговое окно,
содержащее метаданные аннотации и поле ввода для самой аннотации.</p>

<p><img src="../../images/annotationwindow-ru.png" alt="Окно аннотации" /></p>

<h1>Метаданные аннотации</h1>

<p>На данный момент, метаданные включают в себя имя автора, название
аннотируемого документа, тип аннотации, дату создания и дату обновления. Поля
некоторых метаданных имеют специальные свойства:</p>
<ul>
  <li>Поле ввода <strong>Source Document (исходный документ)</strong>
    является ссылкой, которая возвращает вас назад, к аннотируемому
    документу. При двойном щелчке по ней, впрочем как и по любой другой
    ссылке в Amaya, выводится аннотируемый документ, а аннотируемый текст
    подсвечивается.
    <p></p>
  </li>
  <li>Поле ввода <strong>Annotation Types (типы аннотации)</strong> позволяет
    вам классифицировать аннотацию и изменять её тип. Двойным щелчком по
    тексту "annotation type" вы выведите список доступных, на данный момент,
    типов аннотаций.
    <p></p>
  </li>
  <li>Поле ввода <strong>Last Modified (Дата обновления)</strong>
    автоматически обновляется всякий раз, когда аннотация сохраняется.</li>
</ul>

<p>Область, находящаяся ниже заголовка, является местом ввода аннотации. Она
показывает текущее содержание аннотации и может редактироваться как любой
другой HTML документ.</p>

<h1>Сохранение аннотации</h1>

<p>Сохранение аннотации происходит точно также, как и с любым другим
документом в Amaya: выберите пункт <span
class="Link"><strong>Сохранить</strong></span> из меню <strong>Файл</strong>
(или используйте его эквивалентный ярлык или кнопку).</p>

<p>Локальные аннотации сохраняются в директории аннотаций, а удалённые
аннотации сохраняются на сервере публикации аннотаций, т.е. (post) сервере,
разумеется, если пользователь имеет соответствующие права на их запись.</p>

<p>Для конвертации локальных аннотаций в общедоступные, выберите пункт
<strong>Опубликовать (Post) на сервере</strong> из меню
<strong>Аннотации</strong> чтобы сохранить аннотацию на сервере публикации,
как это определено в диалоговом окне <span class="Link"><strong>Конфигурация
для аннотаций</strong>.</span> Если операция выполнена успешно, то локальная
аннотация будет перемещена на сервер и все последующие операции
<strong>Сохранить</strong> будут адресоваться уже напрямую к серверу
аннотаций.</p>

<p>Некоторые команды, которые применяются к документу в окне документа Amaya,
также могут применяться и к аннотируемому документу в окне Аннотации. Для
примера, содержимое аннотации (его тело) можно распечатать через пункт
<strong>Печать</strong> из меню <strong>Файл</strong>, или обновить его,
выбрав пункт <strong>Перезагрузить документ</strong> из меню
<strong>Файл</strong>.</p>

<h2>Смотрите также</h2>
<ul>
  <li><span class="Link0"><a
    href="configuring_annotation_settings.html.ru">Конфигурирование настроек
    аннотации</a></span></li>
  <li><span class="Link0"><a href="deleting_an_annotation.html.ru">Удаление
    аннотации</a></span></li>
  <li><span class="Link0"><a
    href="loading_and_presenting_annotations.html.ru">Загрузка и
    представление аннотаций</a></span></li>
  <li><span class="Link0"><a
    href="navigating_annotations.html.ru">Перемещение по
  аннотациям</a></span></li>
  <li><span class="Link0"><a href="moving_annotations.html.ru">Перемещение
    аннотации</a></span></li>
</ul>
</div>
</body>
</html>
