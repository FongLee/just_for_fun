
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