
// 将浮点数点左边的数每三位添加一个逗号
function addPot(num) {
	return num && num.toString().replace(/(\d)(?=(\d{3})+\.)/g, function($1, $2) {
		return $2 + ',';
	});

}

// 实现数组的随机排序
function randSort(arr) {
	for(var i = 0, len = arr.length; i < arr.length; i++) {
		var rand = parseInt(Math.random() * len);
		var temp = arr[rand];
		arr[rand] = arr[i];
		arr[i] = temp;
	}
	return arr;
}

arr.sort(function() {
	return Math.random() - 0.5;
});

function randSort(arr) {
	var mixedArray = [];

	while(arr.length > 0) {
		var randIndex = parseInt(arr.length * Math.random());
		mixedArray.push(arr[randIndex]);
		arr.splice(randIndex, 1);

	}

	return mixedArray;
}