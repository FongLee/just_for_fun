
// 伪类实现继承
function Mammal (name) {
	this.name = 'mamal';
}

Mammal.prototype.says = function() {
	console.log('Mammal says ' + this.name);
};

Mammal.prototype.getName = function() {
	console.log('super getname');
	return this.name;
}

function Cat (name) {
	this.name = name;
	this.saying = 'is Cat';
}



Cat.prototype = new Mammal();

Cat.prototype.purr = function() {
	console.log('Cat ' + this.name);
};

var myCat = new Cat('little cat');

// Mammal says little cat
myCat.says();
// Cat little cat
myCat.purr();

// 原型继承实现继承
var myMammel = {
	name: 'my Mammel',
	says: function() {
		console.log('Mammal says ' + this.name);
	}

};

var myCat = Object.create(myMammel);
myCat.saying = 'is Cat';
myCat.name = 'little cat';
myCat.purr = function() {
	console.log('Cat ' + this.name);
};

// Mammal says little cat
myCat.says();
// Cat little cat
myCat.purr();

// 函数化实现继承
var mamal = function(spec) {
	var that = {};
	that.says = function() {
		console.log('Mammal says ' + spec.name);
	}
	return that;
}

var cat = function(spec) {
	var that = mamal(spec);
	
	that.purr= function() {
		console.log('Cat ' + spec.name);
	} 

	return that;
}

var myCat = cat({'name': 'little cat'});

// Mammal says little cat
myCat.says();
// Cat little cat
myCat.purr();



