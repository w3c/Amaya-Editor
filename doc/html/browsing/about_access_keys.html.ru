<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <meta name="GENERATOR" content="amaya 8.5, see http://www.w3.org/Amaya/" />
  <title>О ключах доступа
</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="reloading_a_page.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"
        /></a> <a href="../Browsing.html.ru" accesskey="t"><img alt="Вверх"
        src="../../images/up.gif" /></a> <a href="forms.html.ru"
        accesskey="n"><img alt="Следующий" src="../../images/right.gif"
        /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>О ключах доступа</h1>

<p>Ключи доступа предоставляют возможность создания ярлыков для перемещения
курсора по полям формы, областям или ссылкам. В основном, эта возможность
необходима людям с ограниченной трудоспособностью, но есть и другие условия,
где она могла бы быть необходимой или полезной.</p>

<p>Когда создаётся веб документ, вы можете связать атрибут
<code>accesskey</code> со следующими элементами: <code>AREA</code>,
<code>BUTTON</code>, <code>INPUT</code>, <code>LABEL</code>,
<code>LEGEND</code>, и <code>TEXTAREA</code>.</p>

<p>Например, если вы свяжете атрибут <code>accesskey</code> со ссылкой в
документе <code>accesskey="n"</code> или <code>accesskey="N"</code>, то
команда <kbd>Alt accesskey</kbd> (<kbd>Alt n</kbd> или <kbd>Alt N</kbd>)
активирует эту ссылку. Если вы свяжете атрибут <code>accesskey</code> с
элементом формы, то нажатие Alt accesskey переместит фокус на этот элемент
(или на следующий элемент, если он связан с элементом <code>LABEL</code> и
элементом <code>LEGEND</code>).</p>

<p>Ключи доступа перезаписывают ярлыки комбинаций кнопок Amaya. Например,
справочная страница Amaya объявляет эти ключи доступа:</p>
<ul>
  <li>"n" для перехода на следующую страницу</li>
  <li>"p" для перехода на предыдущую страницу</li>
  <li>"t" на Оглавление</li>
</ul>

<p>Если вы назначите комбинацию кнопок <kbd>Alt-p</kbd> для команды Amaya,
эта комбинация не будет работать пока открыта справочная система Amaya.</p>

<p>По умолчанию, Amaya использует ключ <kbd>Alt</kbd> как модификатор для
ключей доступа. Тем не менее, вы можете изменить это правило, принятое по
умолчанию, через диалоговое окно <span
style="color: #0000FF"><strong>Специальные &gt; Настройки &gt;
Основные</strong></span>. Вы можете выбрать ключ <kbd>Control</kbd> в
качестве модификатора, взамен ключа <kbd>Alt</kbd>, или отключить обработчик
ключей доступа вообще.</p>
</div>
</body>
</html>
