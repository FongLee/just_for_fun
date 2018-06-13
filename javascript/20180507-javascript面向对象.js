var Person = function(name, email, website) {
    this.name = name;
    this.email = email;
    this.website = website;

    this.sayHello = function() {
        var hello = 'My name is ' + name + ',\n' +
            'my email is ' + email + ',\n' +
            'my website is ' + website + '.';

        alert(hello);
    }
};

var lifeng = new Person('lifeng', '342789295@qq.com', 'fengsite.com');

lifeng.sayHello();
// constructor: ƒ (name, email, website)
// __proto__
Person.prototype
// constructor: ƒ (name, email, website)
// __proto__
lifeng.__proto__
lifeng.constructor.prototype

var lifeng = Object.create(null);

Object.defineProperty(lifeng, 'name', {
    value: 'lifeng',
    // 这个属性的值是否可以改
    writable: true,
    // 这个属性的配置是否可以改
    configurable: true,
    // 这个属性是否能在for…in循环中遍历出来或在Object.keys中列举出来
    enumerable: true
});

Object.defineProperties(lifeng, {
    'email': {
        value: '342789295@qq.com',
        enumerable: true,
        writable: true,
        configurable: true
    },
    'website': {
        value: 'fengsite.com',
        enumerable: true,
        configurable: true,
        writable: true
    }
});

var age = 0;
Object.defineProperty(lifeng, 'age', {
    get: function() { return age + 1; },
    set: function(value) { age = 2 * value; },
    configurable: true,
    enumerable: true
});

lifeng.age = 100;
alert(lifeng.age);

// undefined
lifeng.__proto__
lifeng.prototype

var lifeng = {
    name: 'lifeng',
    email: '342789295@qq.com',
    website: 'fengsite.com',
    age: 100,
    get birth_year() {
        var d = new Date();
        var year = d.getFullYear();
        return year - this.age;
    },
    set birth_year(year) {
        var d = new Date();
        var currYear = d.getFullYear();
        this.age = currYear - year;
    }
};

lifeng.birth_year;
lifeng.birth_year = 2000;
lifeng.birth_year;

// 有值
lifeng.__proto__

function print(text) {
    console.log('value is ' + this.value + '.text is ' + text);
}

var a = {
    value: 10,
    print: print
};

var b = {
    value: 20,
    print: print
};

a.print('a');
b.print('b');

print.call(a, 'a');
print.call(b, 'b');

print.apply(a, ['a']);
print.apply(a, ['b']);

var p = print.bind(a);
p('a');


p.call(b, 'b');
p.apply(b, ['b']);


var lifeng = Object.create(null);

Object.defineProperties(lifeng, {
    'name': {
        value: 'lifeng',
        configurable: true,
        enumerable: true,
        writable: true
    },
    'email': { value: '342789295@qq.com' },
    'website': { value: 'fengsite.com' }
});

lifeng.sayHello = function() {
    console.log('lifeng sayHello');
}

// 继承
var lifengChild = Object.create(lifeng);

lifengChild.name = 'lifeng child';

// 只有一个属性 name
Object.keys(lifengChild);

// 子对象的函数调用父对象的函数
lifengChild.sayHello = function() {
    Object.getPrototypeOf(this).sayHello.call(this);

    console.log('this is lifengChild hello');
}

// 取不到值,但是Object.getPrototypeOf(lifengChild) 有值
lifengChild.__proto__

// 组合

function composition(target, source) {

    Object.getOwnPropertyNames(source).forEach(
        function(key) {
            Object.defineProperty(target, key, Object.getOwnPropertyDescriptor(source, key));
        });
}

var artist = Object.create(null);

artist.sing = function() {
    console.log('artist is singing');
}

composition(lifengChild, artist);

// ["name", "sayHello", "sing"]
Object.keys(lifengChild)

// Prototype 

function Person(name, email, website) {
    debugger;
    this.name = name;
    this.email = email;
    this.website = website;
}

Person.prototype.sayHello = function() {
    console.log('Person sayHello');
}

function Student(name, email, website, sex) {

    Object.getPrototypeOf(Student.prototype).constructor.call(this, name, email, website);
    this.sex = sex;
}

Student.prototype = Object.create(Person.prototype);

Student.prototype.constructor = Student;

var student = new Student('lifeng', '342789295@qq.com', 'fengsite.com', 'man');
// constructor
Student.prototype

// Object.create函数实现
function clone(proto) {
    function NewObj {}
    NewObj.prototype = proto;

    NewObj.prototype.contructor = NewObj;

    return NewObj;
}


// keys函数
function keys(obj) {
    var ret = [];
    for (var key in obj) {
        if (obj.hasOwnProperty(key)) {
            ret.push(key);
        }
    }
    return ret;
}

function Person(name) {
    this.name = 'lifeng';
    this.closure = function() {
        console.log(name);
    }
}
// 执行上下文：包括变量对象,函数链，this
// 变量对象包括形参，函数声明，变量声明	
function foo() {
    console.log(a);
    a = 1;
}

// ReferenceError
foo();

function foo() {
    a = 1;
    console.log(a);
}
// 1
foo();

function foo() {
    console.log(a);
    var a = 1;
}
// undefined
foo();

var foo = function() {
    console.log('foo1');

}

// foo1
foo();

var foo = function() {
    console.log('foo2');
}
// foo2
foo();


function foo() {
    console.log('foo1');
}
// foo2
// 这个函数会被下面的函数覆盖
foo();
// 函数声明会覆盖砂上面的声明
function foo() {
    console.log('foo2');
}
// foo2
foo();

// 共享传递是传递对象的引用的副本
var obj = {
    value: 1
}

function foo(o) {
    o = 2;
    console.log(o);
}

foo(obj);
console.log(obj.value);

// 实现call
var foo = {
    value: 1
};

function bar() {
    console.log(this.value);
}

bar.call(foo);

Function.prototype.call2 = function(context) {
    context.fn = this;

    context.fn();

    delete context.fn;
}

// 1
Function.prototype.call3 = function(context) {
    var args = [];
    for (var i = 0; i < arguments.length; i++) {
        args.push('arguments[' + i + ']');
    }

    context.fn = this;

    eval('context.fn (' + args + ')');

    delete context.fn;


}

Function.prototype.call4 = function(context) {
    debugger;
    var context = context || window;
    context.fn = this;

    var args = [];
    for (var i = 1; i < arguments.length; i++) {
        args.push('arguments[' + i + ']');
    }

    var ret = eval('context.fn(' + args + ')');
    delete context.fn;
    return ret;
}

function bar(name, age) {
    console.log(value);
    return {
        value: this.value,
        name: name,
        age: age
    }
}

var value = 3;

bar.call4(null);

bar.call4(obj, 'lifeng', 23);

Function.prototype.apply = function(context, arr) {
	debugger;
    var context = Object(context) || window;

    context.fn = this;

    var args = [];

    var ret;

    if (!arr) {
        ret = context.fn();
    } else {
        for (var i = 0; i < arr.length; i++) {
            args.push('arr[' + i + ']');
        }
        ret = eval('context.fn(' + args + ')');
    }

    delete context.fn;
    return ret;
}

// bind的模拟实现
Function.prototype.bind2 = function(context) {
	var self = this;

	return function() {
		return self.apply(context);
	}
}

Function.prototype.bind2 = function (context) {

	var self = this;

	var args = Array.prototype.slice.call(arguments, 1);

	return function() {
		args = args.concat(Array.prototype.slice.call(arguments));
		return self.apply(context, args);
	}
}

// 一个绑定函数也可以使用new创建对象，这时候提供的this值会被忽略
Function.prototype.bind2 = function(context) {
	debugger;
	var self = this;
	var args = Array.prototype.slice.call(arguments, 1);

	var fRet = function() {
		debugger;
		var bindArgs = Array.prototype.slice.call(arguments);
		return self.apply(this instanceof fRet ? this : context, args.concat(bindArgs));
	}
	// rRet的原型指向实例的原型
	fRet.prototype = this.prototype;
	return fRet;
}

Function.prototype.bind2 = function(context) {
	debugger;

	if(typeof this != 'function') {
		throw new Error('it is not a funciton.');
	}
	var self = this;

	var args = Array.prototype.slice.call(arguments, 1);

	var FNOP = function() {};
	var fRet = function () {
		var bindArgs = Array.prototype.slice.call(arguments);
		return self.apply(this instanceof FNOP ? this : context, args.concat(bindArgs));
	}

	FNOP.prototype = this.prototype;
	fRet.prototype = new FNOP();

	return fRet;
}

var value = 2;

var foo = {
    value: 1
};

function bar(name, age) {
    this.habit = 'shopping';
    console.log(this.value);
    console.log(name);
    console.log(age);
}

bar.prototype.friend = 'kevin';

var bindFoo = bar.bind(foo, 'daisy');
var bindFoo2 = bar.bind2(foo, 'daisy');

var obj = new bindFoo('18');
var obj = new bindFoo2('18');

