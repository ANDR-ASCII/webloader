mkdir build
cd build
del CMakeCache.txt
cmake -G "Visual Studio 15 2017 Win64" .. -DMSVC_RUNTIME=dynamic -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PROGRAM_BITNESS=x64
cd ..