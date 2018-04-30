
var class2type = [];

['Number', 'Object', 'String', 'Null', 'Boolean', 'Function', 'Array', 'Undefined', 'Date', 'RegExp', 'Error'].map(function (item) {
	class2type['[Object ' + item + ']'] = item.toLowerCase();
});

function type(obj) {
	
	// ES6 null 和 undefined 返回 object
	if(obj == null) {
		return obj + '';
	}

	return class2type(Object.prototype.toString.call(obj));
}