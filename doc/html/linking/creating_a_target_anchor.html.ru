<?xml version="1.0" encoding="windows-1251"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
       "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <meta name="GENERATOR" content="amaya 8.5, see http://www.w3.org/Amaya/" />
  <title>Создание якоря цели
</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="../Links.html.ru" accesskey="t"><img
        alt="Вверх" src="../../images/up.gif" /></a> <a
        href="creating_a_target_element.html.ru" accesskey="n"><img
        alt="Следующий" src="../../images/right.gif" /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Создание якоря цели</h1>

<p>Якорем цели является часть текста которая связана с ссылкой (якорь
источника). Перед созданием ссылки из текстовой строки, сначала вы должны
превратить её в якорь цели.</p>

<p class="ProcedureCaption">Для создания якоря цели:</p>
<ol>
  <li>Чтобы выбрать соответствующую текстовую строку, щёлкните и перемещайте
    курсор по выбранной строке.
    <p></p>
  </li>
  <li>Выберите пункт <strong>Создать цель</strong> из меню
    <strong>Ссылки</strong>. Выбранный текст превратится в цель. В виде
    Форматированный, если включена опция <strong>Показать цели</strong>,
    перед текстом цели появляется значок. В виде Ссылки, значки
    идентифицирующие якоря цели показываются всегда.
    <p></p>
  </li>
  <li>Когда якорь цели создан, Amaya автоматически генерирует атрибут с величиной <code>
    NAME</code>:
    <p></p>
    <ul>
      <li>Распаковка (извлечение) слов длиной более 3 символов из текста
        якоря.
        <p>или</p>
      </li>
      <li>Генерирование номера.
        <p></p>
      </li>
    </ul>
    <p>В обоих случаях, Amaya гарантирует, что генерируемые величины, в
    пределах документа, будут уникальны.</p>
  </li>
</ol>
</div>
</body>
</html>
