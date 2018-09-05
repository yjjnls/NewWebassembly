'use strict';
const bindingPath = require.resolve(`./build/Release/plugin`);
const Module = require(bindingPath);

console.log('Module.hello() =', Module.hello());

var instance = new Module.MyClass(10);
// instance.incrementX();
// console.log('instance.x: ' + instance.x);
// instance.x = 20;
// console.log('instance.x: ' + instance.x);
console.log('instance.string: ' + Module.MyClass.getStringFromInstance(instance));
// instance.delete();

