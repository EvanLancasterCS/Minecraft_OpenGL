# Minecraft_OpenGL
Recreation of Minecraft using C++ and OpenGL, with multi-threaded world generation.

Multi-threaded mesh generation;
https://media0.giphy.com/media/25R8S3GoDVqLYqb0MA/giphy.gif

Real-time chunk mesh rebuilding;

Ambient occlusion lighting effect;

Properly implemented transparency;


Other implementation details:
  - Manual memory management of chunks which are out of load distance
  - Allowance for per-face texturing and vertex deformation within the chunk for non-cube shaped objects
  - Basic collision system with raycasting
  - Allowance for limitless (integer limit) y-limit
  - ...

The files are unorganized, but more on the implementation can be found throughout the code.

This project is a continuation of a group project which tasked us with creating a basic lighting system within a solar system, and some useless files from that still linger. 
Most of the base-level functionality, such as window setup or shader setup are from that project, but the rest was written outside of school because it was fun.
The implementation is definitely sub-optimal, but writing this helped me to learn a lot more about C++, multithreading, OpenGL, etc.

If you have any questions, please feel free to reach out to me.
