node-gyp clean 

mkdir -p build 
cd  build

node-gyp configure --arch=x86_64 -C ..
node-gyp build -C ..

cd -