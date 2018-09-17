'use strict';
const bindingPath = require.resolve(`./build/Release/plugin`);
const Module = { "MyClass": require(bindingPath) };
// const Module = require(bindingPath);


// var instance = new Module.MyClass(10, "hello");
var instance = Module.MyClass.createObject(10, "hello");

instance.oper(instance);
instance.incrementX();
console.log('instance.x: ' + instance.x);
instance.x = 20;
console.log('instance.x: ' + instance.x);
instance.incrementX(3);
console.log('instance.x: ' + instance.x);

console.log('instance.string: ' + Module.MyClass.getStringFromInstance(instance));
// // console.log('instance.string: ' + Module.getStringFromInstance(instance));
instance.delete();
// instance = null;
// global.gc();
