# 4D Engine 

## building
**Dependencies**
```sh
#If Debug is needed: TODO: Broken??
#conan install . -s build_type=Debug --build=missing 

conan install . -s build_type=Release --build=missing
```

**Build**
```sh
cmake -S . -B build/Release --preset "conan-release"

cmake --build build/Release -j
```