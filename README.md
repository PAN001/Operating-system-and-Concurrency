# Operating-system-and-Concurrency
Coursework

The goal of this work is to make use of operating system APIs (specifically, the POSIX API in Linux) and simple concurrency directives to implement a continuous file allocation algorithm. The final algorithm should use a single producer that generates files (in this case simple ASCII strings) of different random sizes. They are placed into a fixed sized disk (size configurable through define statement). In this coursework, we shall use a memory buffer to emulate the disk. Each file (e.g string) is allocated a continuous space in the buffer using the best-fit algorithm. The first fit algorithm is a generic heuristic approach that tries to find a continuous free block in the buffer that fits the given file (string) in the best way (i.e. the minimum residual capacity after file insertion).
