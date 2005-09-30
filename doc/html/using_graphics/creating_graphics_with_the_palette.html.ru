<?xml version="1.0" encoding="windows-1251"?>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251" />
  <meta name="GENERATOR" content="amaya 8.5, see http://www.w3.org/Amaya/" />
  <title>Создание графики с помощью таблицы
</title>
  <link href="../style.css" rel="stylesheet" type="text/css" />
</head>

<body xml:lang="ru" lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home" /> <img alt="Amaya"
        src="../../images/amaya.gif" /></td>
      <td><p align="right"><a href="working_with_graphics.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"
        /></a> <a href="../SVG.html.ru" accesskey="t"><img alt="Вверх"
        src="../../images/up.gif" /></a> <a href="moving_graphics.html.ru"
        accesskey="n"><img alt="Следующий" src="../../images/right.gif"
        /></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Создание графики с помощью таблицы</h1>

<p>Для создания в документе нового элемента SVG, переместите точку ввода в то
место куда вы его хотите вставить, и щёлкните по кнопке
<strong>Графика</strong>. Кнопка <strong>Графика</strong> (<img
src="../../images/Graph.gif" alt="Graphics button" />) выводит таблицу,
которая останется на экране пока вы не нажмёте кнопку
<strong>Выйти</strong>.</p>

<p>На данный момент доступны следующие графические элементы:</p>
<ul>
  <li>Линия в SVG, <code>line</code>, <svg xmlns="http://www.w3.org/2000/svg"
         width="2cm" height="0.6cm">
      <line y1="3px" x1="0px" x2="17px" y2="20px" style="stroke: #B20000"
            stroke-width="1"/>
    </svg>
  </li>
  <li>Прямоугольник в SVG, <code>rec</code> без атрибутов <code>rx</code> или
    <code>ry</code>, <svg xmlns="http://www.w3.org/2000/svg" width="2cm"
         height="0.6cm">
      <rect x="6px" width="22px" height="18px"
            style="fill: #FFCB69; stroke: #0071FF" stroke-width="1"/>
    </svg>
  </li>
  <li>Прямоугольник с округленными углами в SVG, <code>rect</code>, <svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.6cm">
      <rect rx="6px" stroke-width="1" width="23px" height="18px"
            style="fill: #E5E500; stroke: #4C00E5"/>
    </svg>
  </li>
  <li>Круг в SVG, <code>circle</code>, <svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.6cm">
      <circle cy="10px" cx="12px" r="10px"
              style="fill: #C1FFE9; stroke: #FF0000" stroke-width="1"/>
    </svg>
  </li>
  <li>Эллипс в SVG, <code>ellipse</code>, <svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.6cm">
      <ellipse cy="10px" cx="20px" rx="15px" ry="10px" style="fill: #B795FF"
            stroke-width="1"/>
    </svg>
  </li>
  <li>Ломаная линия в SVG, <code>polyline</code>,<svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.6cm">
      <polyline points="0,16 19,0 33,16 51,6" transform="translate(4,0)"
                stroke-width="1" style="stroke: #0000B2; fill: #FFA069"/>
    </svg>
  </li>
  <li>Многоугольник в SVG, <code>polygon</code>, <svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.7cm">
      <polygon points="0,16 13,0 26,12 43,4 44,16 15,16 12,11"
               transform="translate(2,0)" stroke-width="1"
               style="fill: #C8FF95; stroke: #000000"/>
    </svg>
  </li>
  <li>Открытая кривая в SVG, <code>path</code>, <svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.6cm">
      <path stroke="blue" fill="none"
            d="M 0,16 C 0,15 4,6 9,7 C 13,7 12,16 16,16 C 20,15 24,1 22,0"
            style="stroke: #0071FF" stroke-width="2px"/>
    </svg>
  </li>
  <li>Замкнутая кривая в SVG, <code>path</code>, <svg
         xmlns="http://www.w3.org/2000/svg" width="2cm" height="0.6cm">
      <path stroke="black" fill="none"
            d="M 14,4 C 11,4 8,0 6,1 C 3,1 1,4 1,7 C 0,9 2,12 5,14 C 7,15 13,14 16,14 C 17,13 21,12 23,11 C 24,9 29,7 29,4 C 28,1 24,0 22,0 C 19,0 16,3 14,4 Z"
            style="fill: #FFE9C1; stroke: #B20000"/>
    </svg>
  </li>
</ul>
<ul>
  <li>Вложенный HTML фрагмент в SVG, <code>foreignObject</code>,<svg
         xmlns="http://www.w3.org/2000/svg" width="6cm" height="45px">
      <rect y="0" x="27px" width="140px" height="45px" style="fill: #C1FFFF"
            stroke-width="1"/>
      <foreignObject width="140px" y="0" x="20px">

        <div xmlns="http://www.w3.org/1999/xhtml">
        <ul>
          <li>Первый пункт</li>
          <li>Второй пункт</li>
        </ul>
        </div>
      </foreignObject>
    </svg>
  </li>
  <li>Текст SVG в SVG, <code>text</code><svg
         xmlns="http://www.w3.org/2000/svg" width="9.5cm" height="1.2cm">
      <text y="33px" x="13px"
            style="font-family: helvetica; font-size: 12pt; fill: #0000B2">Символьная
        строка SVG
      </text>
    </svg>
  </li>
</ul>

<h2>Выбор формы из таблицы</h2>

<p>Когда вы выбираете форму из таблицы и выбираете её в HTML структуре
документа, Amaya создаёт новый SVG элемент только в начале текущего выбора и
вставляет выбранную форму в этот новый SVG. Amaya генерирует рисунки SVG как
блоки между параграфами. Для отображения встроенного рисунка, вы просто
должны создать его между параграфами, а для его перемещения отредактировать
исходный код.</p>

<p>Когда выбранный элемент является элементом SVG, новая форма добавится к
текущему рисунку SVG. Новая форма будет добавлена позднее и возможно
наложится на предыдущую форму.</p>

<p>Для создания формы требуются следующие действия:</p>
<ul>
  <li>Для создания прямоугольника, сначала укажите точку ввода в левом углу,
    нажав левую кнопку мыши. Продолжая удерживать кнопку, переместите точку
    ввода в противоположный угол и отпустите кнопку. Теперь форма задана.
    <p></p>
  </li>
  <li>Для создания круга или эллипса, сначала укажите точку ввода центра,
    нажав левую кнопку мыши. Продолжая удерживать кнопку, переместите точку
    ввода вправо и/или на основание формы и отпустите кнопку. Теперь форма
    задана.
    <p></p>
  </li>
  <li>Для создания ломаной линии и многоугольника, вводите каждую точку в
    соответствующей позиции, щёлкая левой кнопкой мыши. Для указания
    последней точки, щёлкните средней или правой кнопкой.</li>
</ul>

<h2>Вставка изображения</h2>

<p>Вы можете вставить изображение в пределах рисунка SVG. Выберите пункт
<strong>Изображение (img)</strong> из меню <strong>XHTML</strong> или
щёлкните по кнопке <strong>Вставить изображение</strong> на панели кнопок,
всё это выполняется таким же образом как и в HTML.</p>

<p>Для вставки математического выражения в рисунок SVG, щёлкните по таблице
<strong>Математика</strong>, если выбранное находится в пределах SVG. Amaya
сгенерирует вложенный SVG <code>foreignObject</code> объект MathML.</p>

<h2>Смотрите также:</h2>
<ul>
  <li><a href="working_with_graphics.html.ru">Работа с графикой</a></li>
  <li><a href="../Math.html.ru">Редактирование математики</a></li>
  <li><a
    href="../editing_mathematics/the_math_palette_and_the_types.html.ru">Таблица
    Математика и меню Математика</a></li>
</ul>

<p>&nbsp;</p>
</div>
</body>
</html>
