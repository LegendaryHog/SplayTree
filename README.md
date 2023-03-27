# SplayTree

My class of SplayTree in C++.

# How to build?
```bash
cmake -B build/ -DCMAKE_CXX_COMPILER=[compiler] -DCMAKE_BUILD_TYPE=[Release/Debug]
cmake --build build/
```

# How to run?
```bash
./build/task/task_run
```

# How to run unit tests?
```bash
cd build
ctest
```

# How generate tests?
```bash
cd end-to-end
chmod "+x" test_gen.py
./test_gen.py [number of keys] [number of requests] [name test file] [mode=--uniform]
```

mode can be equal to: --normal, --triagular, --uniform

# How to test time of execution?
```bash
cd boost_set
cmake -B build/ -DCMAKE_CXX_COMPILER=[compiler] -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd ../
chmod "+x" timecmp
./timecmp [path to build dir of splaytree] [path to build dir of boost set] [test name]
```