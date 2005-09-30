<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=windows-1251">
  <title>О вводе математических символов
</title>
  <meta name="GENERATOR" content="amaya 8.5, see http://www.w3.org/Amaya/">
  <link href="../style.css" rel="stylesheet" type="text/css">
</head>

<body lang="ru">

<table border="0" width="100%" summary="toc">
  <tbody>
    <tr>
      <td><img alt="W3C" src="../../images/w3c_home"> <img alt="Amaya"
        src="../../images/amaya.gif"></td>
      <td><p align="right"><a href="the_math_palette_and_the_types.html.ru"
        accesskey="p"><img alt="Предыдущий" src="../../images/left.gif"></a>
        <a href="../Math.html.ru" accesskey="t"><img alt="Вверх"
        src="../../images/up.gif"></a> <a
        href="editing_math_expressions.html.ru" accesskey="n"><img
        alt="Следующий" src="../../images/right.gif"></a></p>
      </td>
    </tr>
  </tbody>
</table>

<div id="page_body">
<h1>О вводе математических символов</h1>

<p>Когда вы вводите строчный символ в элемент MathML, Amaya выполняет анализ
строки и автоматически генерирует элементы <span class="code"><span
style="font-size:11.0pt;font-family:&#34;Courier New&#34;;">mo</span></span>
(оператор), <span class="code"><span
style="font-size:11.0pt;font-family:&#34;Courier New&#34;;">mn</span></span>
(число), <span class="code"><span
style="font-size:11.0pt;font-family:&#34;Courier New&#34;;">mi</span></span>
(идентификатор), и <span class="code"><span
style="font-size:11.0pt;font-family:&#34;Courier New&#34;;">mtext</span></span>.</p>

<p>Например, для ввода формулы <code>x=2a+b</code>:</p>
<ol>
  <li>Если вы находитесь вне математического выражения, выберите
    <strong>Новая формула (math)</strong> из меню <strong>XML</strong>.
    <p></p>
  </li>
  <li>Введите следующую последовательность из 6 символов:
  <code>x=2a+b</code></li>
</ol>

<p>Используйте вид Структура для просмотра получившейся структуры,
сгенерированной Amaya:</p>

<p><code>&lt;mi&gt;x&lt;/mi&gt;&lt;mo&gt;=&lt;/mo&gt;&lt;mn&gt;2&lt;/mn&gt;&lt;mi&gt;a&lt;/mi&gt;&lt;mo&gt;+&lt;/mo&gt;&lt;mi&gt;b&lt;/mi&gt;</code></p>

<p>Если результат не совпадает с желаемым, выберите символы в которых это
выполнено неправильно и измените их тип с помощью одной из команд: Простой
текст (<code>mtext</code>), Идентификатор (<code>mi</code>), или Число
(<code>mnmo</code>) из подменю <strong>Основные элементы</strong> в меню
<strong>XML</strong>.</p>

<h2>Интервал</h2>

<p>Amaya, в пределах математических выражений, заботится об интервалах, но в
некоторых случаях, вам может потребоваться добавить некоторое дополнительное
пространство. Переместите курсор в позицию вставки интервала и выберите из
меню <strong>XML</strong> подменю <strong>Основные элементы</strong> и в нём
пункт <strong>Пространство (mspace)</strong>. Это создаст принятый по
умолчанию горизонтальный интервал. Затем, вы можете изменить его атрибут
<strong>width-ширина</strong> для корректировки его размера или же, вы можете
добавить другие атрибуты (высота, глубина, прерывание линии) для изменения
интервала строк. Для более подробной информации обратитесь к <a
href="http://www.w3.org/TR/MathML2/chapter3.html#N9197">спецификации
MathML</a>.</p>

<h2>Размер скобки</h2>

<p>При вводе скобок как в следующем выражении:</p>

<p>When entering brackets as in the following expression:</p>

<p><math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>f</mi>
  <mrow>
    <mo>(</mo>
    <mi>x</mi>
    <mo>)</mo>
  </mrow>
  <mo>=</mo>
  <mfrac>
    <mn>1</mn>
    <mi>x</mi>
  </mfrac>
</math></p>

<p>введите последовательность <code>f(x)=</code> &#xfd;то приведёт к
следующему:</p>

<p><math xmlns="http://www.w3.org/1998/Math/MathML">
  <mi>f</mi>
  <mo>(</mo>
  <mi>x</mi>
  <mo>)</mo>
  <mo>=</mo>
  <mfrac>
    <mn>1</mn>
    <mi>x</mi>
  </mfrac>
</math></p>

</div>
</body>
</html>
