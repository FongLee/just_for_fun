
function Animal() {
	this.species = "动物";
}

function Cat(name, color) {
	this.name = name;
	this.color = color;
}

// 构造函数绑定
function Cat(name, color) {
	Animal.apply(this, arguments);
	this.name = name;
	this.color = color;
}

// prototype继承
Cat.prototype = new Animal();
Cat.prototype.constructor = Cat;

// 利用空对象进行prototype继承
function F(){}
F.prototype = Animal.prototype;

Cat.prototype = new F();
Cat.prototype.constructor = Cat;

function extend(Child, Parent) {
	function F() {}

	F.prototype = Parent.prototype;
	Child.prototype = new F();
	Child.prototype.constructor = Child;
	Child.uber = Parent.prototype;
}

// 非构造函数的继承
var animal = {
	species: "动物"
};

var cat = Object.create(animal);

cat.name = 'cat';

// 浅拷贝
function copy(obj) {
	// 检测对象的原型链是否指向构造函数的prototype对象的
	// obj.constructor === Array
	var ret = obj instanceof Array ? [] : {}; 
	for(var key in obj) {
		if(obj.hasOwnProperty(key)) {
			ret[key] = obj[key];
		}
	}
	return ret;
}

function deepCopy(obj) {
	// typeof null "object"
	// typeof undefined "undefined"
	if(typeof obj !== 'object') {
		return obj;
	}

	// Array.isArray ECMAScript5
	var ret = Array.isArray(obj) ? : {};
	for(var key in obj) {
		if(obj.hasOwnProperty(key)) {
			if(typeof obj[key] === 'object') {
				ret[key] = deepCopy(obj[key]);
			} else {
				ret[key] = obj[key];
			}

		}
	}
	return ret;
}