# No GPU Lab

No GPU Lab is a software renderer built from scratch following the amazing tutorial by *Dimitry V. Sokolov*, known as ssloy, to understand the fundamentals of computer graphics and rendering.

## About the project

This project is my implementation of a software renderer while learning the fundamentals of computer graphics. The goal is to understand how the graphics APIs such as OpenGL and Vulkan work under the hood by implementing a rendering pipeline from scratch, with as little external dependencies as possible.

## Features

- Bresenham's line drawing algorithm
- Triangle rasterization
- Rendering wireframes
- Z-Buffering for depth testing
- Loading and processing 3D models

## Technologies used

- C++
- CMake
- TGA Image format for output

## Getting Started

###Prerequisites
- C++ Compiler
- Git
- CMake

### Clone the Repository
```
git clone https://github.com/KlutzKnight/no-gpu-lab.git
cd no-gpu-lab
```

### Build and Run
```
cmake -B build
cmake --build build
```

## Credit

This project is inspired by and follows TinyRenderer by Dimitry V. Sokolov (@ssloy).

The original tutorial and its author deserve the credit for the concepts and guidance used in this learning project.