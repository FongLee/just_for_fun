
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



