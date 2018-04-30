function eq(a, b, astack, bstack) {


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
    if (typeof a !== 'object' && typeof a !== 'function' && typeof b !== 'object') {
        return false;
    }

    return deepeq(a, b, astack, bstack);

}

function deepeq(a, b, astack, bstack) {
    debugger;
    var classA = Object.prototype.toString.call(a);
    var classB = Object.prototype.toString.call(b);

    if (classA !== classB) {
        return false;
    }

    switch (classA) {
        case '[object String]':
        case '[object Reqrex]':
            return '' + a === '' + b;
        case '[object Number]':
            // Number(NaN) 的情况，不等于自身 
            if (+a !== +a) { return +b !== +b; }
            // +0 -0 不应该相等，
            return +a === 0 ? 1 / +a === 1 / b : +a === +b;
        case '[object Date]':
        case '[object Boolean]':
            return +a === +b;
    }

    var isArray = classA === '[object Array]';
    if (!isArray) {
        var afn = a.constructor,
            bfn = b.constructor;

        // 构造函数不相等，我们认为对象也不相等。constructor 排除一个是Object.create(null)的情况 ，排除Object.create()的情况
        if (afn !== bfn && ('constructor' in a && 'constructor' in b) && !(isFuntion(afn) && afn instanceof afn && isFuntion(bfn) && bfn instanceof bfn)) {
            return false;
        }
    }

    var length;

    astack = astack || [];
    bstack = bstack || [];

    length = astack.length;
    while (length--) {
        if (astack[length] === a) {
            return bstack[length] === b;
        }
    }

    astack.push(a);
    bstack.push(b);

    if (isArray) {
        length = a.length;
        if (length !== b.length) {
            return false;
        }

        while (length--) {
            if (!eq(a[length], b[length], astack, bstack)) {
                return false;
            }
        }

    } else {
        var aKeys = Object.keys(a);
        length = aKeys.length;
        while (length--) {
            if (!(b.hasOwnProperty(aKeys[length]) && eq(a[aKeys[length]], b[aKeys[length]], astack, bstack))) {
                return false;
            }
        }
    }

    astack.pop();
    bstack.pop();

    return true;

}

function isFuntion(obj) {
    return Object.prototype.toString.call(obj) === '[object Function]';
}