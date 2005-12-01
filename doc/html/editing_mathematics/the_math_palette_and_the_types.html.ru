<html>
<head>
  <meta http-equiv="content-type" content="text/html; charset=windows-1251">
  <title>Кнопка Математика и меню Математика </title>
  <meta name="GENERATOR" content="amaya 9.3, see http://www.w3.org/Amaya/">
  <link href="../style.css" rel="stylesheet" type="text/css">
</head>

<body lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home"> <img alt="Amaya"
        src="../../images/amaya.gif"></td>
      <td><p align="right"><a
        href="about_entering_math_constructs_using_the_keyboard.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"></a>
        <a href="../Math.html.ru" accesskey="t"><img alt="Вверх"
        src="../../images/up.gif"></a> <a
        href="about_entering_math_characters.html.ru" accesskey="n"><img
        alt="Следующий" src="../../images/right.gif"></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>Кнопка Математика и меню Математика</h1>

<p>Для создания в документе математического выражения, переместите курсок
мыши в ту позицию куда вы хотите вставить выражение, и щёлкните по кнопке
<strong>Математика</strong>, или выберите подменю <strong>Математика</strong>
из меню <strong>Xml</strong>.</p>

<h2>Команды доступа</h2>

<p>Математические выражения могут быть вставлены в документ с помощью подменю
<strong>Математика</strong> из меню <strong>XML</strong>, или выбрав
выражение из таблицы <strong>Математика</strong>.</p>

<p>Щёлкните по кнопке <strong>Математика</strong> <img alt="Math Button"
src="../../images/Math.gif"> в панели инструментов появится таблица
<strong>Математика</strong>. Закройте таблицу, щёлкнув по кнопке
<strong>Выйти</strong>.</p>

<p>Также, вы можете вывести меню Математика щёлкнув по пункту
<strong>Математика</strong> из меню <strong>XML</strong>.</p>

<h2>Меню Математика и команды таблицы</h2>

<p>И в таблице и в меню <strong>Математика</strong>, первым пунктом является
пункт <strong>Новая формула</strong>, позволяющий вам создавать новую
формулу, когда курсор находится в HTML или SVG элементе, но не в пределах
MathML элемента.</p>

<p>Также, меню <strong>Математика</strong> содержит элементы которые не
представлены в таблице. Эти пункты включают в себя: Простой текст
(<code>mtext</code>), идентификатор (<code>mi</code>), число
(<code>mn</code>), оператор (<code>mo</code>). Эти команды могут изменять тип
простого элемента или последовательности элементов.</p>

<p>Пункт символ (<code>&amp;xxx;</code>) позволяет вам вводить символ
недоступный для ввода с клавиатуры. Он выводит диалоговое окно где вы можете
ввести имя символа (например alpha из греческого алфавита).</p>

<p>Следующий пункт меню или таблицы <strong>Математика</strong> позволяет вам
создавать новые конструкции в пределах формулы. Если точка ввода находится не
в формуле, Amaya сначала создаёт математический элемент чтобы принять новую
конструкцию. На данный момент доступны следующие конструкции:</p>
<ul>
  <li>Корень в степени, <code>mroot</code>: <math
    xmlns="http://www.w3.org/1998/Math/MathML">
      <mroot>
        <mrow>
          <mi>x</mi>
          <mo>+</mo>
          <mn>1</mn>
        </mrow>
        <mn>3</mn>
      </mroot>
    </math></li>
  <li>Квадратный корень, <code>msqrt</code>: <math
    xmlns="http://www.w3.org/1998/Math/MathML">
      <msqrt>
        <mrow>
          <mi>x</mi>
          <mo>+</mo>
          <mn>1</mn>
        </mrow>
      </msqrt>
    </math></li>
  <li>Вложение, <code>menclose</code>: <math
    xmlns="http://www.w3.org/1998/Math/MathML">
      <menclose>
        <mn>1234</mn>
      </menclose>
    </math></li>
  <li>Дробь, <code>mfrac</code>: <math
    xmlns="http://www.w3.org/1998/Math/MathML">
      <mfrac>
        <mn>1</mn>
        <mrow>
          <mi>x</mi>
          <mo>+</mo>
          <mn>1</mn>
        </mrow>
      </mfrac>
    </math></li>
  <li>Выражение с нижним и верхним индексом, <code>msubsup</code>: <math
    xmlns="http://www.w3.org/1998/Math/MathML">
      <msubsup>
        <mi>x</mi>
        <mrow>
          <mi>i</mi>
          <mo>+</mo>
          <mn>1</mn>
        </mrow>
        <mi>n</mi>
      </msubsup>
    </math>или <math xmlns="http://www.w3.org/1998/Math/MathML">
      <mrow>
        <msubsup>
          <mo largeop="true">&#x222b;</mo>
          <mn>0</mn>
          <mo>&#x221e;</mo>
        </msubsup>
        <mo> </mo>
      </mrow>
    </math></li>
  <li>Выражение с нижним индексом, <code>msub</code>: <math
    xmlns="http://www.w3.org/1998/Math/MathML">
      <msub>
        <mi>x</mi>
        <mi>i</mi>
      </msub>
    </math></li>
  <li>Выражение с верхним индексом, <code>msup</code>: <math
    xmlns="http://www.w3.org/1998/Math/MathML">
      <msup>
        <mi>x</mi>
        <mi>n</mi>
      </msup>
    </math></li>
  <li>Выражение с подсимвольным и надсимвольным индексом,
    <code>munderover</code>: <math xmlns="http://www.w3.org/1998/Math/MathML">
      <munderover>
        <mo movablelimits="false">&#x2211;</mo>
        <mrow>
          <mi>i</mi>
          <mo>=</mo>
          <mn>1</mn>
        </mrow>
        <mi>n</mi>
      </munderover>
    </math></li>
  <li>Выражение с подсимвольным индексом, <code>munder</code>: <math
    xmlns="http://www.w3.org/1998/Math/MathML">
      <munder>
        <mi>x</mi>
        <mo>&#xaf;</mo>
      </munder>
    </math></li>
  <li>Выражение с надсимвольным индексом, <code>mover</code>: <math
    xmlns="http://www.w3.org/1998/Math/MathML">
      <mrow>
        <mi>x</mi>
        <mover>
          <mo>&#x2192;</mo>
          <mtext>maps to</mtext>
        </mover>
        <mi>y</mi>
      </mrow>
    </math></li>
  <li>Выражение с круглыми скобками, <code>mrow</code>: <math
    xmlns="http://www.w3.org/1998/Math/MathML">
      <mrow>
        <mo fence="true">(</mo>
        <mrow>
          <mi>a</mi>
          <mo>+</mo>
          <mi>b</mi>
        </mrow>
        <mo fence="true">)</mo>
      </mrow>
    </math></li>
  <li>Выражение с предсимвольным и послесимвольным индексами,
    <code>mmultiscripts</code>: <math
    xmlns="http://www.w3.org/1998/Math/MathML">
      <mmultiscripts>
        <mi>X</mi>
        <mi>i</mi>
        <mi>j</mi>
        <mprescripts/>
        <mi>k</mi>
        <mi>l</mi>
      </mmultiscripts>
    </math></li>
  <li>Матрица или таблица, <code>mtable</code>: <math
    xmlns="http://www.w3.org/1998/Math/MathML">
      <mtable>
        <mtr>
          <mtd>
            <mi>a</mi>
          </mtd>
          <mtd>
            <mi>b</mi>
          </mtd>
        </mtr>
        <mtr>
          <mtd>
            <mi>c</mi>
          </mtd>
          <mtd>
            <mi>d</mi>
          </mtd>
        </mtr>
      </mtable>
    </math></li>
</ul>

<h2>Смотрите также:</h2>
<ul>
  <li><a href="editing_math_expressions.html.ru">Редактирование
  математики</a></li>
  <li><a href="about_entering_math_characters.html.ru">Создание
    математических выражений</a></li>
</ul>
</div>
</body>
</html>
