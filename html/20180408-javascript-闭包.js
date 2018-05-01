

for (var i = 0; i < 5; i++) {
	setTimeout(function(){
		console.log(new Date, i);
	}, 1000)
}

console.log(new Date, i);

for( var i = 0; i < 6; i++) {
	(function(j) {
		setTimeout(function() {
			console.log(new Date, j);
		},1000 * j);
	})(i);

	
}

const tasks = [];
for (var i = 0; i < 5; i++) {   // 这里 i 的声明不能改成 let，如果要改该怎么做？
    ((j) => {
        tasks.push(new Promise((resolve) => {
            setTimeout(() => {
                console.log(new Date, j);
                resolve();  // 这里一定要 resolve，否则代码不会按预期 work
            }, 1000 * j);   // 定时器的超时时间逐步增加
        }));
    })(i);
}

// this is functionTest 985
// 985
var objectTest = {
	functionTest: function() {
		console.log("this is functionTest "+ this.value)
		var that = this;
		var help = function() {
			console.log(that.value);
		};

		help();
	},

	value: 985
}


// this is functionTest 985
// undefined
var objectTest = {
	functionTest: function() {
		console.log("this is functionTest "+ this.value);
		var test = 1000;
		var help = function() {
			console.log(this.test);
		};

		help();
	},

	value: 985
}

var add = function (a, b) {
	return a + b;
};

function  add2(a, b) {
	return a + b;
}

var con = function (value) {
	this.value = value;
};

function con2(value) {
	this.value = value;
}

test = new con2('sdfds');



