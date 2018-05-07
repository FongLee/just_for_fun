var Person = function(name, email, website) {
    this.name = name;
    this.email = email;
    this.website = website;

    this.sayHello = function() {
        var hello = 'My name is ' + name + ',\n' +
            'my email is ' + email + ',\n' +
            'my website is ' + website + '.';

        alert(hello);
    }
};

var lifeng = new Person('lifeng', '342789295@qq.com', 'fengsite.com');

lifeng.sayHello();


var lifeng = Object.create(null);

Object.defineProperty(lifeng, 'name', {
    value: 'lifeng',
    // 这个属性的值是否可以改
    writable: true,
    // 这个属性的配置是否可以改
    configurable: true,
    // 这个属性是否能在for…in循环中遍历出来或在Object.keys中列举出来
    enumerable: true
});

Object.defineProperties(lifeng, {
    'email': {
        value: '342789295@qq.com',
        enumerable: true,
        writable: true,
        configurable: true
    },
    'website': {
        value: 'fengsite.com',
        enumerable: true,
        configurable: true,
        writable: true
    }
});

var age = 0;
Object.defineProperty(lifeng, 'age', {
    get: function() { return age + 1; },
    set: function(value) { age = 2 * value; },
    configurable: true,
    enumerable: true
});

lifeng.age = 100;
alert(lifeng.age);

var lifeng = {
    name: 'lifeng',
    email: '342789295@qq.com',
    website: 'fengsite.com',
    age: 100,
    get birth_year() {
        var d = new Date();
        var year = d.getFullYear();
        return year - age;
    },
    set birth_year(year) {
        var d = new Date();
        var currYear = d.getFullYear();
        age = currYear - year;
    }
};

lifeng.birth_year;
lifeng.birth_year = 2000;
lifeng.birth_year;

function print(text) {
    console.log('value is ' + this.value + '.text is ' + text);
}

var a = {
    value: 10,
    print: print
};

var b = {
    value: 20,
    print: print
};

a.print('a');
b.print('b');

print.call(a, 'a');
print.call(b, 'b');

print.apply(a, ['a']);
print.apply(a, ['b']);

var p = print.bind(a);
p('a');


p.call(b, 'b');
p.apply(b, ['b']);