
function unique(array) {
	var res = [];
	for(var i = 0; i < array.length; i++) {
		if(res.indexOf(array[i]) === -1) {
			res.push(array[i]);
		}
	}

	return res;

}

// 排序后去重
function uniqueSort(array) {
	var res = [];

	var sortedArray = array.concat().sort();
	var before;
	for(var i = 0; i < sorttedArray.length; i++) {
		if( i === 0 || before !== sortedArray[i]){
			res.push(sortedArray[i]);
		}
		before = sortedArray[i];
	}
	return res;
}

function unique(array) {
	var res = array.filter(function(item, index, array) {
		return array.indexOf(item)  === index;
	});

	return res;
}

function uniqueSort(array) {
	var res = array.concat().sort().filter(function(item, index, array) {
		return index === 0 || item !== array[index-1];
	});
	return res;
}

// ES6 unique
function unique(array) {
	return Array.from(new Set(array));
}

var unique = (a) => [...new Set(a)];

function unique(array) {
	const seen = new Map();
	return arr.filter((a)=>!seen.has(a) && seen.set(a, 1));
}
