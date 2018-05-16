
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
} catch(e) {

}

// 在谷歌浏览器中报错
// VM79:12 Uncaught TypeError: f is not a function
// 理论上ES6 应该返回 i am outside 
// 但是浏览器的实现：
// 允许在块级作用域内声明函数。
// 函数声明类似于var，即会提升到全局作用域或函数作用域的头部。
// 同时，函数声明还会提升到所在的块级作用域的头部。
function  f () {
	console.log("i am outside ");
}

(function(){
	if(false) {
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
	const MAXIN="lifeng";
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
function getGlobal () {
	// 浏览器和web Worker(运行在浏览器后台的JavaScript)
	if(self !== 'undefined') {
		return self;
	}

	// 浏览器
	if(window !== 'undefined') {
		return window;
	}
	// Node环境
	if(global !== 'undefined') {
		return global;
	}
}

// 数组的解构赋值
let [a, b, c] = [1, 2, 3];

// x=null,默认值，=== undefined，才会使用默认值
let [x=1] = [null];


// fooTest "aaa" 先找到同名属性，然后赋值给对应的变量，
let {foo: fooTest} = {foo: 'aaa', bar: 'bbb'};

// x = 1
// y = 5
var {x, y=5} = {x: 1};
