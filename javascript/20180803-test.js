
// 将浮点数点左边的数每三位添加一个逗号
function addPot(num) {
	return num && num.toString().replace(/(\d)(?=(\d{3})+\.)/g, function($1, $2) {
		return $2 + ',';
	});

}
