# Introduction

This programming project provides an implementation and unit tests for the `sc::HashTbl<>` class.

This class, as the name indicates, is an implementation of an Hash Table in C++, using double hashing. One of the hash functions should be passed by the client via template-argument. Another is fixed based on division method, using the remainder of the integer division between the number produced by the first hash function and the size of our table, producing numbers (indexes) in the range [0, length - 1], efficiently organizing our data.

Each adress of our table points to a list of items. Each item is a pair of key and value, and the key is unique. So to retrieve information, we need to hash the key, get into the list that contains it and then look for it inside (since it is unique, there is no chance of mistake). The key and value's types are defined by the client via template-argument.

When the load-factor is extrapolated, the table will rehash, so we don't get inefficient hashing later on.

There are two kind of testing in this project: one more "raw" based and one "applied". The applied one is motivated by a simple example application of bank accounts, where all the data is saved in our Hash Table. Details on how to run both tests are given in the sections below.

# Organization

The folders and files of this project are the following:

* `source/driver`: This folder has two source files, (1) `driver_ht.cpp` that demonstrates the hash table in action for the `Account` problem described in the assignment PDF, and; (2) `account.cpp` that contains the implementation of the `Account` class.
* `source/test`: This folder has the file `main.cpp` that contains all the tests. Note that the tests were developed with [**Googletest**](https://github.com/google/googletest).
* `source/include`: This is the folder contains 2 files, (1) `hashtbl.h` with the declaration of the `HashTbl` class, (2) `hashtbl.inl` that should contain the implementation `HasTbl`'s methods.
* `source/CMakeLists.txt`: The cmake script file.
* `README.md`: This file.

# Dependencies

You must install the **Googletest** Library, which you may clone from [here](https://github.com/google/googletest).
After cloning the project, you will need to compile it and install it in your system or locally.

If you need, you may find a good **Googletest** tutorial [here](https://google.github.io/googletest/primer.html), although it would be probably uncessary to access this documentation since the tests have already been coded for you. You just need to install the library and compile the project.

The `CMakeLists.txt` automatically looks for the location of the **Googletest** libraries in your system. If you decide to install **Googletest** locally you will need to change the cmake configuration file to indicate where you've installed the **Googletest** library (path).

# Compiling

This project may be compiled with [CMake](https://cmake.org). CMake is an open-source, cross-platform family of tools designed to build, test and package software. Basically, cmake reads a script (`CMakeLists.txt`) with meta-information on the project and **creates a Makefile automatically**, tailored to your system configuration.
After that, you may run the `make` command, as usual.
Alternatively, the same script might be used to create Xcode projects, Ninja, or Microsoft Visual Studio projects, for instance.

CMake supports **out-of-source** build. This means the _source code_ is stored in **one** folder and the _generated executable files_ should be stored in **another** folder: project should never mix-up the source tree with the build tree.

In particular, this project creates two  **targets** (executable), called `run_tests` and `driver_hash`. The first runs the tests whereas the second demonstrates the application of a hash table to a specific problem.

But don't worry, they are already set up in the `CMakeLists.txt` script.

To compile this project with [cmake](https://cmake.org) follow these steps (from the root folder):

1. `cmake -S source -B build`:  asks cmake to create the build directory and generate the Unix Makefile based on the script found in `source/CMakeLists.txt`, on the current level.
3. `cmake --build build`: triggers the compiling process that creates both targets (executable).D inside `build`.

The executable is created inside the `build` directory.

For further details, please refer to the [cmake documentation website](https://cmake.org/cmake/help/v3.14/manual/cmake.1.html).

# Running

From the project's root folder, run as usual (assuming `$` is the terminal prompt):

```
$ ./build/run_tests
```
and for the driver, which makes an "applied" kind of testing, type in

```
$ ./build/driver_hash
```
