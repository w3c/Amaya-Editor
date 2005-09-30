<?xml version="1.0" encoding="windows-1251"?>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <meta name="GENERATOR" content="amaya 8.5, see http://www.w3.org/Amaya/" />
  <title>Раскраска графики
</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="moving_graphics.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"
        /></a> <a href="../SVG.html.ru" accesskey="t"><img alt="Вверх"
        src="../../images/up.gif" /></a> <a href="resizing_graphics.html.ru"
        accesskey="n"><img alt="Следующий" src="../../images/right.gif"
        /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Раскраска графики</h1>

<p>С Amaya, вы можете раскрашивать (заливка или штрихование) графические
элементы SVG простыми цветами. По умолчанию, цвет заливки чёрный, а цвет
штрихования прозрачный, но вы можете изменить эти величины.</p>

<p>Существует два пути изменения принятых по умолчанию цветов заливки и
штрихования: редактирование атрибутов SVG из меню <strong>Атрибуты</strong>,
или редактирование атрибутов SVG с помощью каскадных таблиц стиля.</p>

<p class="ProcedureCaption">Для редактирования атрибутов SVG:</p>
<ol>
  <li>Выберите графику.</li>
  <li>Откройте меню <strong>Атрибуты</strong>. Там вы увидите оба атрибута: и
    <strong>Fill-заливка</strong> и <strong>Stroke-штриховка.</strong></li>
  <li>Выберите атрибут для редактирования.</li>
  <li>В диалоговом окне измените атрибут (например,<kbd>fill="#C8ff95";
    stroke="#000000"</kbd>), щёлкните по кнопке <strong>Применить</strong>, а
    затем по кнопке <strong>Закрыть</strong>.
    <p></p>
    <p class="Note"><strong>Примечание:</strong> &nbsp;Для указания цвета, вы
    можете использовать шестнадцатеричное число цвета, или специальные имена
    основных цветов, таких как black-чёрный, white-белый, blue-голубой,
    green-зелёный, red-красный, pink-розовый, purple-пурпурный,
    violet-фиолетовый или yellow-жёлтый.</p>
  </li>
</ol>

<p class="ProcedureCaption">Для редактирования атрибутов SVG с помощью
каскадных таблиц стиля:</p>
<ol>
  <li>Выберите графику.</li>
  <li>Откройте меню <strong>Стиль</strong> и выберите пункт
    <strong>Цвет</strong>. Откроется палитра цветов.</li>
  <li>Из палитры цветов выберите желаемый цвет. Когда вы щёлкните левой
    кнопкой мыши, вы сгенерируете свойство <kbd>stroke</kbd>, &#xe0; когда вы
    щёлкните правой кнопкой мыши, вы сгенерируете свойство <code>fill</code>.
    <p></p>
    <p class="Note"><strong>Примечание:</strong> &nbsp;&nbsp; SVG текст
    рассматривается как графическая форма, поэтому свойство <kbd>fill</kbd>
    окрашивает символьное содержание, а свойство <kbd>stroke</kbd> окрашивает
    границу символа. Amaya применяет к символам свойство <kbd>fill</kbd>, но
    никак не свойство <kbd>stroke</kbd>. Поэтому, щёлчок левой кнопки мыши не
    даёт немедленного эффекта окрашивания, в то же время как щелчок средней
    или правой кнопкой мыши окрашивает элемент.</p>
  </li>
</ol>
</div>
</body>
</html>
