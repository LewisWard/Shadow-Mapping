# README #

Project: Shadow Mapping

Version: 1.3

Author: Lewis Ward

## How to get set up ##
Create a directory called 'build' in 3DRenderer, afterwards you should have: 3DRenderer/build 


###Building the project (cmake cmd)###
1. go to 3DRenderer/build
2. run this command from the command line: cmake -G "Visual Studio 14 2015 Win64" ../../
3. cmake should of made you a visual studio project "ShadowMapping.sln", open this VS solution.
4. In the solution explorer, right click on "ShadowMapping" project and "Set as StartUp Project"
5. F5 to compile the project, by default builds for x64 debug (also a x64 release).

###Building the project (cmake GUI)###
1. "Where is the source code:" = set this path to the directory that contains the CMakeLists.txt (should be in the same directory as this file)
2. "Where to build the binaries:" = set this to the directory 3DRenderer/build to the end of the path you set for the source code.

    CMAKE_CONFIGURATION_TYPES = can get away with the default although I use: Debug;Release; 

    CMAKE_INSTALL_PREFIX = same path as the where to build the binaries path.

    Set to use Visual Studio, I've tested with: Visual Studio 14 2015 Win64

3. Click 'Generate' and now if you go to 3DRenderer/build you should see a visual studio project "ShadowMapping.sln", open this VS solution.
4. Follow from 'Building the project (cmake cmd)' step 4.

## Controls ##
Moveable cube: Q/E/W/A/S/D

Rotate camera: X

Esc: Quit