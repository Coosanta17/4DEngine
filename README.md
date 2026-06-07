# 4D Engine 

## building
**Dependencies**
* Python 3
* Cmake
* Compiler
* Conan (with profile)
* Probably some other stuff I forgot
```sh
#If Debug is needed:
conan install . -s build_type=Debug --build=missing 

conan install . -s build_type=Release --build=missing
```

**Build**
```sh
cmake -S . -B build/Release --preset "conan-release"

cmake --build build/Release -j
```