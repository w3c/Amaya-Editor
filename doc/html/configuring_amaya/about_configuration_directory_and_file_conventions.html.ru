<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <title>О директории конфигурации и файле соглашений</title>
  <style type="text/css">
  </style>
  <meta name="GENERATOR" content="amaya 8.6, see http://www.w3.org/Amaya/" />
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="configuring_amaya.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"
        /></a> <a href="../Configure.htm.ru" accesskey="t"><img alt="Вверх"
        src="../../images/up.gif" /></a> <a href="menu_preferences.html.ru"
        accesskey="n"><img alt="Следующий" src="../../images/right.gif"
        /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>О директории конфигурации и файле соглашений</h1>

<p>Amaya использует ряд заданных по умолчанию директорий для хранения
информации о конфигурации.</p>

<h2>Директория конфигурации</h2>

<p>Большинство принятых по умолчанию величин и все диалоговые сообщения
хранятся в директории конфигурации Amaya. Эта директория является одной из
поддиректорий директории инсталяции Amaya (для Unix, по умолчанию это
<code>Amaya/config</code>; для Windows, местоположение этой директории
смотрите в нижеприведённой таблице).</p>

<h2>Домашняя директория Amaya</h2>

<p>Amaya хранит пользовательские настройки и другие файлы конфигурации в
директории называемой AmayaHome. Её местоположение может быть переназначено
пользователем только с помощью переменной окружения
<code>AMAYA_USER_HOME</code>. В противном случае, Amaya использует
местоположение директории принятое по умолчанию.</p>

<p>Нижеследующая таблица показывает позицию этой директории (принятую по
умолчанию) для различных архитектур:</p>

<table border="1" cellspacing="0">
  <tbody>
    <tr>
      <td valign="top"
      style="border:solid .5pt;   padding:0in 5.4pt 0in 5.4pt"><p
        class="TableHead"><b>Архитектура</b></p>
      </td>
      <td valign="top"
      style="border:solid .5pt;   border-left:none;padding:0in 5.4pt 0in 5.4pt"><p
        class="TableHead"><b>Местоположение директории AmayaHome</b></p>
      </td>
    </tr>
    <tr>
      <td valign="top"
      style="border:solid .5pt;   border-top:none;padding:0in 5.4pt 0in 5.4pt"><p
        class="TableText">Unix</p>
      </td>
      <td valign="top"
      style="border-top:none;border-left:   none;border-bottom:solid .5pt;border-right:solid .5pt;   padding:0in 5.4pt 0in 5.4pt"><p
        class="TableText"><code>$HOME/.amaya</code></p>
      </td>
    </tr>
    <tr>
      <td valign="top"
      style="border:solid .5pt;   border-top:none;padding:0in 5.4pt 0in 5.4pt"><p
        class="TableText">Windows 95/ Windows 98</p>
      </td>
      <td valign="top"
      style="border-top:none;border-left:   none;border-bottom:solid .5pt;border-right:solid .5pt;   padding:0in 5.4pt 0in 5.4pt"><p
        class="TableText"><code>AMAYA-INSTALL-DIR\users\username</code> или,
        если пользователь не входит в Windows,
        <code>AMAYA-INSTALL-DIR\users\default</code></p>
      </td>
    </tr>
    <tr>
      <td valign="top"
      style="border:solid .5pt;   border-top:none;padding:0in 5.4pt 0in 5.4pt"><p
        class="TableText">Windows NT/XP</p>
      </td>
      <td valign="top"
      style="border-top:none;border-left:   none;border-bottom:solid .5pt;border-right:solid .5pt;   padding:0in 5.4pt 0in 5.4pt"><p
        class="TableText"><code>$HOMEDRIVE\$HOMEPATH\amaya</code><br />
        по умолчанию c:\Documents и Settings\$user_name\amaya</p>
      </td>
    </tr>
  </tbody>
</table>

<p>При установке новой версии Amaya, старая директория сохраняется. Она может
содержать следующие файлы:</p>
<ul>
  <li><strong>thot.rc</strong>: Содержит пользовательские настройки,
    сохранение через подменю <strong>Настройки</strong> из меню
    <strong>Специальные</strong>.
    <p></p>
  </li>
  <li><strong>amaya.keyboard</strong> (Unix) или <strong>amaya.kb</strong>
    (Windows): Определяет ярлыки комбинаций кнопок Amaya.
    <p></p>
  </li>
  <li><strong>amaya.css</strong>: Содержит пользовательские стили
    форматирования.
    <p></p>
  </li>
  <li><strong>dictionary.DCT</strong>: Amaya предоставляет многоязычную
    проверку правописания (русский язык не поддерживается). По умолчанию,
    проверка правописания работает с двумя словарями: английским и
    французским (вы можете загружать словари с W3C серверов). Если вы
    захотите зарегистрировать новое слово, проверка правописания создаст и
    обновит ваш персональный словарь.</li>
</ul>

<p class="Note"><strong>Примечание</strong>:<br />
Файлы <code>amaya.keyboard</code>, <code>amaya.kb</code>, и
<code>amaya.css</code> загружаются во время запуска программы. Если вы
изменили их, вы должны выйти и перезагрузить Amaya чтобы ваши изменения этих
файлов вступили в силу.</p>

<h2>Директория AmayaTemp</h2>

<p>Все созданные Amaya временные файлы хранятся в директории называемой
AmayaTemp. Нижеследующая таблица показывает местоположение этой директории
для различных архитектур:</p>

<table border="1" cellspacing="0">
  <tbody>
    <tr>
      <td valign="top"
      style="border:solid .5pt;   padding:0in 5.4pt 0in 5.4pt"><p
        class="TableHead"><b>Архитектура</b></p>
      </td>
      <td valign="top"
      style="border:solid .5pt;   border-left:none;padding:0in 5.4pt 0in 5.4pt"><p
        class="TableHead"><b>Директория AmayaTemp</b></p>
      </td>
    </tr>
    <tr>
      <td valign="top"
      style="border:solid .5pt;   border-top:none;padding:0in 5.4pt 0in 5.4pt"><p
        class="TableText">Unix</p>
      </td>
      <td valign="top"
      style="border-top:none;border-left:   none;border-bottom:solid .5pt;border-right:solid .5pt;   padding:0in 5.4pt 0in 5.4pt"><p
        class="TableText"><code>$HOME/.amaya</code></p>
      </td>
    </tr>
    <tr>
      <td valign="top"
      style="border:solid .5pt;   border-top:none;padding:0in 5.4pt 0in 5.4pt"><p
        class="TableText">Windows 9x</p>
      </td>
      <td valign="top"
      style="border-top:none;border-left:   none;border-bottom:solid .5pt;border-right:solid .5pt;   padding:0in 5.4pt 0in 5.4pt"><p
        class="TableText"><code>c:\temp\amaya</code> (величина принятая по
        умолчанию)</p>
      </td>
    </tr>
    <tr>
      <td>Window XP</td>
      <td>$HOMEDRIVE\$HOMEPATH\Local Settings\Temp\amaya<br />
        по умолчанию c:\Documents и Settings\$user_name\Local
        Settings\Temp\amaya</td>
    </tr>
  </tbody>
</table>

<p>Amaya создаёт файлы в этой директории при просмотре удалённых документов
или при распечатке документа. В этой директории также хранится и кэш. Вы
можете изменить метоположение директории AmayaTemp и директории кэширования,
используя настройки Основные и Кэш (<span class="Link0">выберите
<strong>Настройки &gt; Основные</strong> или <strong>Настройки &gt;
Кэш</strong> из меню <strong>Специальные</strong></span>).</p>

<p>Amaya всегда пытается удалить созданные ею временные файлы. Тем не менее,
если Amaya "упала", временные файлы не будут удалены. При последующем запуске
Amaya функция удаления временных фалов работать не будет.</p>
</div>
</body>
</html>
