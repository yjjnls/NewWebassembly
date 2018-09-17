'use strict';
const bindingPath = require.resolve(`./build/Release/plugin`);
const Module = require(bindingPath);

// console.log('Module.hello() =', Module.hello());

// var instance = new Module.MyClass(10, "hello");
var instance = Module.createObject(10, "hello");

instance.oper(instance);
instance.incrementX();
// console.log('instance.x: ' + instance.x);
// instance.x = 20;
// console.log('instance.x: ' + instance.x);
// instance.incrementX(3);
// console.log('instance.x: ' + instance.x);

// console.log('instance.string: ' + Module.MyClass.getStringFromInstance(instance));
// console.log('instance.string: ' + Module.getStringFromInstance(instance));
// instance.delete();
// delete instance;
instance = null
// global.gc();

