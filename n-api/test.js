'use strict';
const bindingPath = require.resolve(`./build/Release/plugin`);
const Module = require(bindingPath);
console.log('Module.hello() =', Module.hello());


