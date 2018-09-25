'use strict';
const bindingPath = require.resolve(`./build/Release/plugin`);
const Module = require(bindingPath);

class MyClass {
    constructor(x, y) {
        if (!!x && !!y)
            this.target = new Module.MyClass(x, y);
        else if (!!x)
            this.target = new Module.MyClass(x);
        else
            this.target = new Module.MyClass();

        // this.x = (()=>{return this.target.x})();
        this.x= this.target.x.bind(this.x);
        // this.bind(this.target)
        // this= this.target
    }

    oper(val) {
        return this.target.oper(val.target);
    }
    incrementX(val1, val2) {
        if (!!val1 && !!val2)
            this.target.incrementX(val1, val2);
        if (!!val1 && !val2)
            this.target.incrementX(val1);
        if (!val1 && !val2)
            this.target.incrementX();
    }
    static getStringFromInstance(val) {
        return Module.getStringFromInstance(val.target);
    }
    delete() {
        this.target = null;
        this.x = null;
        global.gc();
    }
}

module.exports = { "MyClass": MyClass };


// test

