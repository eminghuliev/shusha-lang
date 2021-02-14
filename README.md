<p align="center">
<img src="https://user-images.githubusercontent.com/14012405/97899295-8cb30b80-1d52-11eb-994a-09833e0b0978.png" align="center" height="80" width="300" >
</p>
<p align="center">
Shusha programming language (*WIP*)</p>

## Build from source code
### Dependencies:
* Clang >= 10.0.0
* LLVM  >= 10.0.0
### Compilation instructions:
```
bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-11 100
sudo update-alternatives --install /usr/bin/llvm-config llvm-config /usr/bin/llvm-config-11 100
make
```
### Dump IR with Shusha 
```
./build/bin/shusha test/source.sl --dump-ir
```

Copyright
---------------------------------
Copyright (c) 2021 GOUP. All rights reserved.

