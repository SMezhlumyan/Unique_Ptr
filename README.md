# Custom `unique_ptr` Implementation in C++

This repository contains a custom implementation of a simplified version of C++'s `std::unique_ptr`, including support for:

- Default deleters for single objects and arrays.
- Small Object Optimization (SOO) for deleter storage.
- Move semantics with move constructor and move assignment.
- Basic smart pointer functionalities like `release()`, `reset()`, `swap()`, `get()`, and dereference operators.
- Compile-time safety for deleting incomplete types.

## Features

- **Default deleters:** Handles both single objects (`delete`) and arrays (`delete[]`).
- **Small Object Optimization:** Optimizes empty deleter types by inheriting from them to reduce storage overhead.
- **Move-only ownership:** Copy constructor and copy assignment are deleted to enforce unique ownership.
- **Exception safety:** Implements noexcept move operations and a safe destructor.
- **Custom deleter support:** Allows specifying custom deleter types.

## Usage

Include `unique_ptr.cpp` in your project and use the `unique_ptr<T, D>` class template to manage dynamic memory with unique ownership semantics.

Example:

```cpp
unique_ptr<int> ptr(new int(42));
std::cout << *ptr << std::endl;

ptr.reset(new int(100));
