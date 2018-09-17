'use strict';
const bindingPath = require.resolve(`./build/Release/plugin`);
const Module = require(bindingPath);

class MyClass{
    constructor(x,y){
        this.target=new Module.MyClass(x,y);
    }
}

module.exports = MyClass;