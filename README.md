# Logic gates library

## Overview

A fast and space-efficient library for handling combinational Boolean circuits composed of the following types of gates:
- NAND, AND, OR, NOR, XOR, XNOR


## Gate implementation details

- A gate has a non-negative integer number of fan-ins (inputs) and one fan-out (output).
- A gate with no fan-ins always outputs a signal with the value false.
- For a positive n, the fan-ins of an n-input gate are numbered from 0 to n - 1.
- A Boolean signal with values false or true can be supplied to the fan-ins of a gate.
- The fan-out signal from a gate can be connected to multiple fan-ins of other gates.
- Each fan-in of a gate can have only one signal source connected to it.

## API Reference

You can find the API reference for this library at the following link: [API Reference](https://iteron-dev.github.io/logic-gates-library/).

## Building and Running

### Prerequisites:
To build and use this library, you need:

- CMake version 3.12 or higher.
- A supported compiler, such as GCC or Clang, with support for the C17 standard.

### Compilation

1. Download or clone the repository.
 
2. Build the library and example program by executing the following commands in the root of the repository:

```bash
cmake -B build/
make -C build/
```

This will create two binaries in `build/` directory: 
- `libgate.so` - the shared library file.
- `example` -  an example program demonstrating the usage of the library.

You can now link `libgate.so` to your own program.

#### Release Build
For potentially better performance, build in Release mode to enable `-O3` and `-march=native`:
```bash
cmake -B build/ -DCMAKE_BUILD_TYPE=Release
make -C build/
```


### Example
For demonstration purposes, you can refer to and modify the `example.c` file. It showcases a sample usage of this library.

Gate from `example.c`:

![image info](./example_gate.svg)


## License

This project is licensed under the **GNU General Public License v3.0**.  
See [LICENSE](LICENSE.md) for the full terms.
