// ES5 规定，函数只能在顶层作用域和函数作用域之中声明，不能在块级作用域声明。
// 在谷歌浏览器中运行并不会报错
if (true) {
    function test(argument) {
        console.log("it is a test");
    }
}
try {
    function test(argument) {
        console.log("it is a test");
    }
} catch (e) {}
// 在谷歌浏览器中报错
// VM79:12 Uncaught TypeError: f is not a function
// 理论上ES6 应该返回 i am outside 
// 但是浏览器的实现：
// 允许在块级作用域内声明函数。
// 函数声明类似于var，即会提升到全局作用域或函数作用域的头部。
// 同时，函数声明还会提升到所在的块级作用域的头部。
function f() {
    console.log("i am outside ");
}
(function() {
    if (false) {
        function f() {
            console.log("i am inside");
        }
    }
    f();
}())
// 暂时性死区
if (true) {
    // Uncaught ReferenceError: MAXIN is not defined
    console.log(MAXIN);
    const MAXIN = "lifeng";
}
// Uncaught SyntaxError: Missing initializer in const declaration
const foo;
// 从 ES6 开始，全局变量将逐步与顶层对象的属性脱钩
let b = 1;
window.b // undefined
// 函数里面的this，如果函数不是作为对象的方法运行，而是单纯作为函数运行，this会指向顶层对象
function f(argument) {
    console.log(this);
}
// 获取顶层对象
function getGlobal() {
    // 浏览器和web Worker(运行在浏览器后台的JavaScript)
    if (self !== 'undefined') {
        return self;
    }
    // 浏览器
    if (window !== 'undefined') {
        return window;
    }
    // Node环境
    if (global !== 'undefined') {
        return global;
    }
}
// 数组的解构赋值
let [a, b, c] = [1, 2, 3];
// x=null,默认值，=== undefined，才会使用默认值
let [x = 1] = [null];
// fooTest "aaa" 先找到同名属性，然后赋值给对应的变量，
let {
    foo: fooTest
} = {
    foo: 'aaa',
    bar: 'bbb'
};
// x = 1
// y = 5
var {
    x,
    y = 5
} = {
    x: 1
};
// 报错
let {
    foo: {
        bar
    }
} = {
    baz: 'baz'
};
let arr = [1, 2, 3];
let {
    0: first,
    [arr.length - 1]: last
} = arr;
// 1
first
// 3
last
// 字符串的解构赋值
const [a, b, c, d, e] = 'hello';
// 数值和布尔值的解构赋值
let {
    toString: s
} = 123;
let {
    toString: s
} = true;
// 函数的解构赋值
function add([a, b]) {
    return a + b;
}
add([1, 2]);
// 函数的解构赋值 使用默认值
function add({
    a = 1,
    b = 2
} = {}) {
    return a + b;
}
// 5
add({
    a: 3
});

function add({
    a,
    b
} = {
    a: 0,
    b: 0
}) {
    return a + b;
}
// [3, undefined],因为已经指定了函数的默认参数
add({
    a: 3
});
// 不能使用括号的情况，1 变量声明的时候 2 函数参数 3 赋值语句的模式
// 下面情况不报错
[(a)] = [3];
({
    p: (b)
} = {});
[(parseInt.prop)] = [3];
// 解构赋值的作用
// 交换变量的值
var x = 3,
    y = 4;
[x, y] = [y, x];
// 函数返回多个值，方便取值
function test() {
    return [1, 2];
}
let [a, b] = test();
// 函数参数
function f({
    x,
    y,
    z
}) {
    return x + y + z;
}
f({
    z: 1,
    y: 2,
    x: 3
});
// 提取json数据
let jsonData = {
    name: 'lifeng'
}
let {
    name
} = jsonData;
// 函数参数默认值
// 遍历Map解构
const map = new Map();
map.set('name', 'hello');
for (let [key, value] of map) {
    console.log(key + ":" + value);
}
// 表示一个字符
'\z'
'\172'
'\u007A'
'\u{7A}'
// 实际四个字节存储
var s = "𠮷";
//长度显示的2 
s.length
s.charAt(0);
s.charAt(1);
// 返回前两个字节的数据
// 55362
s.charCodeAt(0);
// 返回后两个字节的数据
// 57271
s.charCodeAt(1);
// 视为三个字符
let s = '𠮷a';
// 134071
s.codePointAt(0);
// 57271
s.codePointAt(1);
// 97
s.codePointAt(2);
for (let ch of s) {
    console.log(ch.codePointAt(0).toString(16));
}
// 不能识别32位的UTF-16字符
String.fromCharCode(0x20BB7);
String.fromCodePoint(0x20BB7);
for (let code of 'fos') {
    console.log(code);
}
var str = String.fromCodePoint(0x20BB7);
for (let i = 0; i < str.length; i++) {
    console.log(str[i]);
}
// normalize 用来将不同表示方法统一为同样的形式
'\u01D1'.normalize() == '\u004F\u030C'.normalize();
let s = "hello world";
s.includes('hello');
s.startsWith('hello');
s.endsWith('d');
// "nana"
"na".repeat(2);
// 补全字符串
// "ababx" 头部补全
'x'.padStart(5, 'ab');
// 尾部补全
'x'.padEnd(5, 'ab');
// 为数值补指定位数
// "0000000001"
'1'.padStart(10, '0');
// "0000000012"
'12'.padStart(10, '0');
// 提示字符串格式
'12'.padStart(10, 'YYYY-MD-DD');
// 模板字符串
`test
换行
`
let name = 'lifeng';
let str = `name is ${name}`;
let str = '(name) => `${name}`';
let fn = eval.call(null, str);
fn('jack');
const set = new Set([1, 2, 3, 4, 4]);
// [1, 2, 3, 4]
[...set]
const items = new Set([1, 2, 3, 5, 5]);
items.size;
// NaN !== NaN
// set内部使用一种类似于===的方法判断是否相等，但是NaN添加只有一个个
let set = new Set();
set.add(NaN);
set.add(NaN);
let set = new Set();
set.add({});
set.add({});
set.has({});
set.add(1);
set.add(2);
set.delete(2);
const items = new Set([1, 2, 3, 4, 5]);
const array = Array.from(items);
// 去除重复成员的方法
function dedupe(array) {
    return Array.from(new Set(array));
}
for (let item of items.keys()) {
    console.log(item);
}
for (let item of items.values()) {
    console.log(item);
}
for (let item of items.entries()) {
    console.log(item);
}
for (let item of items) {
    console.log(item);
}
items.forEach((key, value) => console.log(key + ':' + value));
let set = new Set([1, 2]);
let arr = [...set];
// {2, 4}
set = new Set([...set].map(x => x * 2));
let set = new Set([1, 2]);
// {2}
set = new Set([...set].filter(x => (x % 2) == 0));
let arr = [3, 5, 5];
// [3, 5]
let unique = [...new Set(arr)];
let a = new Set([1, 2, 3]);
let b = new Set([2, 3, 4]);
// 并集 {1, 2, 3, 4}
let union = new Set([...a, ...b]);
// 交集 {2, 3}
let intersect = new Set([...a].filter(x => b.has(x)));
// 差集 {1}
let difference = new Set([...a].filter(x => !b.has(x)));
// 遍历产生新的数组
set = new Set([...set].map(val => val * 2));
set = new Set(Array.from(set, val => val * 3));
const a = [
    [1, 2],
    [3, 4]
];
const ws = new WeakSet(a);
const m = new Map();
const o = {
    p: 'hello world'
};
m.set(o, 'content');
// "content"
m.get(o);
// true
m.has(o);
// true
m.delete(o);
// false
m.has(o)
const map = new Map([
    ['name', 'zhangsan'],
    ['title', 'author']
]);
const set = new Set([
    ['foo', 2],
    ['bar', 2]
]);
const map = new Map(set);
// 2
map.get('foo');
// true
map.has('foo');
const m2 = new Map([
    ['foo', 3]
]);
const m3 = new Map(m2);
// 3
m3.get('foo');
const map = new Map();
map.set(['a'], 555);
// undefined
map.get(['a']);
// map 严格相等时，键才认为不同
// undefine 和 null 不同的键
// NaN虽然不是严格相等，但是认为是一样的键
const map = new Map([
    ['no', 3],
    ['fo', 5]
]);
// no fo
for (let key of map.keys()) {
    console.log(key);
}
// 3 5
for (let value of map.values()) {
    console.log(value);
}
// no 3 fo 5
for (let entry of map.entries()) {
    console.log(entry[0], entry[1]);
}
// no 3 fo 5
for (let [key, value] of map) {
    console.log(key, value);
}
// 转换为数组
// ["no", "fo"]
[...map.keys()];
// [3, 5]
[...map.values()];
// ["no", 3] ["fo", 5]
[...map];
// 遍历和过滤
// {"no" => 3}
const map1 = new Map([...map].filter(([key, value]) => value === 3));
// {"no" => 6, "fo" => 10}
const map2 = new Map([...map].map(([key, value]) => [key, value = value * 2]));
// key is no, value is 3
// key is fo, value is 5
map.forEach(function(value, key, map) {
    console.log("key is %s, value is %d", key, value);
});
// key is no, value is 3
// key is fo, value is 5
const reporter = {
    report: function(key, value) {
        console.log("key is %s, value is %d", key, value);
    }
};
map.forEach(function(value, key, map) {
    this.report(key, value);
}, reporter);
// Map转换为对象
function strMapToObj(strMap) {
    let obj = Object.create(null);
    for (let [key, value] of strMap) {
        obj[key] = value;
    }
    return obj;
}
// 对象转换为Map
function objToStrMap(obj) {
    let map = new Map();
    for (let key of Object.keys(obj)) {
        map.set(key, obj[key]);
    }
    return map;
}
// Map转为Json
function strMaptoJson(strMap) {
    return JSON.stringify(strMapToObj(strMap));
}
// 转换为数组Json
function mapToArrayJson(map) {
    return JSON.stringify([...map]);
}
// JSON转换为Map
function jsonTostrMap(jsonStr) {
    return objToStrMap(JSON.parse(jsonStr));
}
// 数组Json转换为Map
function jsonToMap(jsonStr) {
    return new Map(JSON.parse(jsonStr));
}
// WeakMap只接收对象作为键名，键名是弱引用，键名指向的对象的引用对象不会增加，但是
// 键值会正常引用
// 没有keys values entries方法
// 以DOM节点作为键名
let myElement = document.getElementById('logo');
let myWeakMap = new WeakMap();
myWeakMap.set(myElement, {
    count: 0
});
myElement.addEventListener('click', function() {
    let myCount = myWeakMap.get(myElement);
    myCount.count++;
});
// Count内部属性 _count 是实例的弱引用
const _count = new WeakMap();
class Count {
    constructor(counter) {
        _count.set(this, counter);
    }
}
// 函数,默认参数每次是重新求值的
function log(x, y = 1) {
    console.log(x, y);
}

function foo({
    x,
    y = 2
}) {
    console.log(x, y)
}
// TypeError
foo();
// undefined 2
foo({});
// 5 3
foo({
    x: 5,
    y: 3
});

function foo({
    x,
    y = 2
} = {}) {
    console.log(x, y);
}
// undefined 2
foo();

function foo({
    x,
    y
} = {
    x: 1,
    y: 2
}) {
    console.log(x, y);
}
// 1 2
foo();
// 3 4
foo({
    x: 3,
    y: 4
});
// undefined undefined
foo({});

function foo(x = 1, y) {
    console.log(x, y);
}
// Unexpected token
foo(, 1);
// 1 2 if null no work
foo(undefined, 2);
// length 不计声明了默认值的参数,后面的也不计
// 1
(function(a, b = 2, c) {
    console.log(a, b, c)
}).length;
// 一旦设置了参数的默认值，函数进行声明初始化时，会形成一个单独的作用域。
function add(...values) {
    let sum = 0;
    for (let value of values) {
        sum += value;
    }
    return sum;
}
// 9
add(2, 3, 4);
//函数参数使用了默认值，解构赋值，扩展运算符，函数内部就不准设置严格模式，但是
//可以通过设置全局严格模式，或者将函数包在一个立即执行函数之中。
//
(function foo() {
    'use strict'
    return test(a = 1) {
        return a;
    };
}())

function foo() {}
// 返回函数的名字
foo.name;
// 这个和ES6标准不符，在Chrome测试，返回""，理论上应该返回"bound foo"
foo.bind({}).name;
var f = v => v;
// 等价于
function f(v) {
    return v;
}
// 返回一个对象
let = getTempItem = id => ({
    id: id
});
// 箭头函数体内的this，指向的是定义时所在的对象，而不是使用时所在的对象
function foo() {
    setTimeout(() => {
        console.log(this.id);
    }, 100)
}
// ES5
function foo() {
    var _this = this;
    setInterval(function() {
        console.log(_this.id);
    })
}

function insert(value) {
    return {
        info: function(array) {
            return {
                after: function(afterValue) {
                    array.splice(array.indexOf(afterValue) + 1, 0, value);
                    return array;
                }
            }
        }
    }
}
let insert = (value) => ({
    info: (array) => ({
        after: (afterValue) => {
            array.splice(array.indexOf(afterValue) + 1, 0, value);
            return array;
        }
    })
});
// [1, 2, 3]
insert(2).info([1, 3]).after(1);
const pipeline = (...funcs) => val => funcs.reduce((a, b) => b(a), val);
const plus1 = a => a + 1;
const mult2 = a => a * 2;
const addThenMult = pipeline(plus1, mult2);
addThenMult(5);
//尾递归
function factorial(n, total) {
    if (n === 1) {
        return total;
    }
    return factorial(n - 1, n * total);
}
// 尾递归优化过后的数列
function fibonacci(n, ac1 = 1, ac2 = 1) {
    if (n <= 1) {
        return ac2;
    }
    return fibonacci(n - 1, ac2, ac1 + ac2);
}
// 柯里化是将使用多个参数的函数转换成一系列使用一个参数的函数的技术
// 通用版本
var curry = function(func) {
    var args = [].slice.call(arguments, 1);
    return function() {
        var newArgs = args.concat([].slice.call(arguments));
        return func.apply(this, newArgs);
    }
}
// improvement
var curry = function(func, args) {
    var args = args || [];
    return function() {
        var newArgs = args.concat([].slice.call(arguments));
        debugger;
        if (newArgs.length < 2) {
            return curry.apply(this, func, newArgs)
        } else {
            return func.apply(this, newArgs);
        }
    }
}
// 尾递归优化的实现
function trampoline(f) {
    while (f && f instanceof Function) {
        f = f();
    }
    return f;
}

function sum(x, y) {
    if (y > 0) {
        return sum(x + 1, y - 1);
    }
    return x;
}
trampoline(sum(1, 10000));
foo: bar;
// = 
bar.bind(foo);
let s = 'hello world';
// true
s.startsWith('hello');
// false
s.endsWith('!');
// "xx"
'x'.repeat(2);
let str = 'return ' + '`${name}`';
let fun = new Function('name', str);
// "jack"
fun('jack');
let str = '(name) => `hello ${name}`';
let fun = eval.call(null, str);
// "hello jack"
fun('jack');
let template = `
<ul>
	<% for(var i = 0; i < data.supllies.length; i++) { %>
		<li><%= data.supllies[i]  %></li>
	<% } %>
</ul>`;

function compile(template) {
    const evalExpr = /<%=(.+?)%>/g;
    const expr = /<%([\s\S]+?)%>/g;
    template = template.replace(evalExpr, '`); \n echo ($1) ;\n echo(`').replace(expr, '`); \n $1 \n echo(`');
    template = 'echo(`' + template + '`);';
    let script = `(function parse(data) {
		let outPut = '';
		
		function echo(html) {
			outPut += html;
		}
		${template}
		return outPut;
	})`;
    return script;
}
let parse = eval(compile(template));
// "
// <ul>
// <li>broom</li>
// <li>mop</li>
// <li>cleaner</li>
// </ul>"
var outPutHtml = parse({
    supllies: ["broom", "mop", "cleaner"]
});
// 标签模板
let a = 5;
let b = 10;
// hello 
//   world 
// 15
// 50
tag `hello ${a+b} world ${a*b}`;

function tag(str, v1, v2) {
    debugger;
    console.log(str[0]);
    console.log(str[1]);
    console.log(v1);
    console.log(v2);
}