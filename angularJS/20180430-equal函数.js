function eq(a, b) {


    // +0 和 -0 是不相等的，1 / +0 = Infinity
    if (a === b) {
        return a !== 0 || 1 / a === 1 / b;
    }

    if (a === null || b === null) {
        return false;
    }

    if (a !== a) {
        return b !== b;
    }

    // a是基本类型，b不是对象，返回false
    if (typeof a !== 'object' && typeof b !== 'function' && b !== 'object') {
        return false;
    }

    return deepeq(a, b);

}

function deepeq(a, b) {

    var classA = Object.prototype.toString.call(a);
    var classB = Object.prototype.toString.call(b);

    if (classA !== classB) {
        return false;
    }

    switch (classA) {
        case '[Object Stirng]':
        case '[Object Reqrex]':
            return '' + a === '' + b;
        case '[Object Number]':
        	// Number(NaN) 的情况，不等于自身 
            if (+a !== +a) { return +b !== +b; }
            // +0 -0 不应该相等，
            return +a === 0 ? 1 / +a === 1 / b : +a === +b;
        case '[Object Date]':
        case '[Object Boolean]':
            return +a === +b;
    }

    

}