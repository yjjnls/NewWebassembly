
install clang 6.0.1 on Ubuntu 16.04
```sh
cd /opt
wget http://releases.llvm.org/6.0.1/clang+llvm-6.0.1-x86_64-linux-gnu-ubuntu-16.04.tar.xz

tar -xvf clang+llvm-6.0.1-x86_64-linux-gnu-ubuntu-16.04.tar.xz

cd clang+llvm-6.0.1-x86_64-linux-gnu-ubuntu-16.04/

sudo cp -rf * /usr/local
```

```
node --expose-gc test.js
```
