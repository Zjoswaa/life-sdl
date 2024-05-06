# Game of Life
John Conway's game of life made using [SDL](https://github.com/libsdl-org/SDL) and [ImGui](https://github.com/ocornut/imgui).

---
## Build instructions
### Windows
###### Visual Studio
- `git clone git@github.com:Zjoswaa/life-sdl.git --recursive`
- `cd .\life-sdl\ `
- `mkdir build`
- `cd .\build\ `
- `cmake .. -DCMAKE_BUILD_TYPE="Release"`
- Open `build\Life-SDL.sln` in Visual Studio
- Change build type from `Debug` to `Release`
- Build solution
###### Make
- `git clone git@github.com:Zjoswaa/life-sdl.git --recursive`
- `cd .\life-sdl\ `
- `mkdir build`
- `cd build`
- `cmake .. -DCMAKE_BUILD_TYPE="Release" -G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM="make"`. This works only if `make.exe` is in your Path.
  - To specify the `make.exe` location, instead use `cmake .. -DCMAKE_BUILD_TYPE="Release" -G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM:PATH=C:/MinGW/bin/make.exe`. Make sure to change the `CMAKE_MAKE_PROGRAM:PATH` to the correct path for your system.
- `make`
### Linux
- `git clone git@github.com:Zjoswaa/life-sdl.git --recursive`
- `cd life-sdl/`
- `mkdir build`
- `cd build/`
- `cmake .. -DCMAKE_BUILD_TYPE="Release"`
- `make`
## Showcase
![Screenshot](showcase/showcase.png)