## AirlineReservationSystem
Simple Airline Reservation System with GUI and JSON support.

<img src="https://github.com/HawtStrokes/AirlineReservationSystem/blob/master/Resources/SampleImage.png" width=90% height=90%>

## **Build**

### Cloning

In a terminal, enter the following commands.

```plaintext
git clone --recursive https://github.com/HawtStrokes/AirlineReservationSystem .git
cd AirlineReservationSystem/
git submodule update --recursive --init
```

### GLFW

Use Cmake to generate the solution file. For ease of building, I suggest using cmake-gui. Open cmake-gui and point the source code to `<complete absolute path to>\AirlineReservationSystem\External\glfw` and set the binaries location to `<complete absolute path to>\AirlineReservationSystem\External\glfw\build`

Open the generated solution file in the build folder and build `Debug|x64` and `Release|x64` configurations.

### HawtLib

Open the solution file in the root directory, and build `Release_DLL` and `Debug_DLL` configurations of HawtLib.

In file explorer, go to the following directories and open the appropriate batch files:

*   In`...\External\HawtLib\HawtLib\Scripts` open `MakeIncludes.bat`
*   In`...\Scripts` open `CopyDLL.bat`

## **Dependencies**

- https://github.com/glfw/glfw
- https://github.com/ocornut/imgui
- https://github.com/fmtlib/fmt
- https://github.com/HawtStrokes/HawtLib
- https://github.com/nlohmann/json
