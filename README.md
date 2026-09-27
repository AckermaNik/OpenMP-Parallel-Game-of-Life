# OpenMP-Parallel-Game-of-Life

A C implementation of Conway's Game of Life that uses OpenMP to update a two-dimensional cellular automaton in parallel. Each generation is computed from the previous generation, with a separate destination grid preventing concurrent workers from interfering with one another's reads.

## How it works

The program reads a rectangular grid of cells, evolves it for a requested number of generations, and writes the resulting grid to a file. An asterisk (`*`) represents a live cell; a space represents a dead cell.

For each generation, `game_of_life` executes a nested row-and-column loop with `#pragma omp parallel for collapse(2)`. `collapse(2)` combines the two loop levels into one iteration space so OpenMP can distribute individual cells across threads. Each worker counts the eight surrounding positions and applies the standard Conway rules:

- A live cell survives with two or three live neighbors.
- A live cell dies with fewer than two neighbors or more than three.
- A dead cell becomes live when it has exactly three live neighbors.

Workers read only from the current grid and write each result into a distinct position in `new_grid`. The OpenMP loop's implicit barrier completes the whole generation before the two grid pointers are swapped. This double-buffering pattern keeps generations consistent: no cell in a generation can observe another cell's partially updated state.

The grid uses finite boundaries. Neighbor positions outside the rows or columns are ignored; the edges do not wrap around.

## Input and output format

The input begins with the number of rows and columns. The remaining characters describe the cells; the reader skips newline, carriage-return, and vertical-bar (`|`) separators. For example:

```text
3 3
 |*| |
 | |*|
 |*|*| 
```

The output uses the same dimensions and writes each row with vertical bars around its cell values. Pass a distinct output path if you want to keep the original input unchanged.

## Build and run

Compile with a C compiler that supports OpenMP, such as GCC:

```bash
gcc -O2 -fopenmp -o game_of_life game_of_life.c
```

Run the executable with an input file, generation count, and output path:

```bash
./game_of_life <input_file> <generations> <output_file>
```

Example:

```bash
./game_of_life input.txt 100 result.txt
```

The number of OpenMP threads is controlled through the `OMP_NUM_THREADS` environment variable, rather than a command-line argument. For example, in a Bash-compatible shell:

```bash
OMP_NUM_THREADS=4 ./game_of_life input.txt 100 result.txt
```

On Windows, compile and run from an OpenMP-capable GCC environment such as WSL, or use a compiler configured for OpenMP.

## Files

- `game_of_life.c` implements grid input/output, neighbor counting, generation updates, and the command-line entry point.
- `declarations.h` includes standard C/OpenMP headers and declares the functions implemented in `game_of_life.c`.
- `report.pdf` contains the accompanying assignment report.
- `.vscode/` contains editor, debugger, and C/C++ configuration.

## Implementation notes

- The program runs exactly the requested number of generations; it does not stop early when the grid reaches a stable state.
- Grid characters other than `*` are treated as dead cells during updates.
- The input reader assumes the dimensions and cell data are valid and that enough cell characters are present.
- The source does not currently include automated tests or explicit handling for invalid dimensions and allocation failures in every allocation path.
