const Module = require('./example.js');
Module.onRuntimeInitialized = function () {
    var instance = new Module.MyClass(10, "hello");
    instance.incrementX();
    console.log('instance.x: ' + instance.x);
    instance.x = 20;
    console.log('instance.x: ' + instance.x);
    console.log('instance.string: ' + Module.MyClass.getStringFromInstance(instance));
    instance.delete();
};
