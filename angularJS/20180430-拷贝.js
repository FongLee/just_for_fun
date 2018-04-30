
// 浅拷贝
var shallowCopy = function(obj) {
	if(!ojb) {
		return ;
	}

	var newObj = obj instanceof Array ? [] : {};
	for(var key in obj) {
		if(obj.hasOwnProperty(key)) {
			newObj[key] = obj[key];
		}
	}
	return newObj;
}

var deepCopy = function(obj) {
	if(typeof obj !== 'object') {
		return obj;
	}

	var newObj = obj instanceof Array ? [] : {};
	for(var key in obj) {
		if(obj.hasOwnProperty(key)) {
			newObj[key] = typeof obj[key] === 'object' ? deepCopy(obj[key]) : obj[key];
		}
	}

	return newObj;

}