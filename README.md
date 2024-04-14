# What is this?
This project is essentially a yet another custom animation editor for SDL2, which includes both tools to convert images into animation and GUI application.
# What is this format?
"*.panm" file is just a binary file that includes pixel data of original images and preprocessed images in ARGB8888 format, compressed using LZ4 algorithm, and some metadata. More specifically, current version of format includes:
- Format version 
- Image size (might be scaled comparing to the original)
- Real image size
- Number of frames
- Coordinates of image origin
- Data about frames (pairs of frame index and corresponding real frame)
- Duration of the animation
- Actual real sprites (full size after decompression is realW * realH * 4, the file contains compressed size and compressed data)
- White version of sprites with some fake blur applied
# Dependencies
- SDL2 / SDL2_image
- LZ4
- Cmake
All dependencies are included in conanfile
# Building
Assuming that you have CMake and conan installed and configured and you are in the project folder, run this:
```
conan install . --output-folder=build --build=missing --profile=<debug / release / any other cool profile you have> (can also use one of *.bat files in the folder if you are using windows)
cd ./build
cmake .. -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"
cmake --buid .
```
If you are not using conan, make sure to provide dependencies to CMake manually
# Planned changes:
- As this editor and format were mostly created with my fighting game in mind, I most likely will add some primitive collider / movement data editing toolkit
- Proper file dialog instead of just plain string edit field
- Way to add sprites manually instead of just choosing a folder with them (and by extention, support for formats other than PNG and BPM)
- Saving using multithreading to avoid screen freezing (and to speed up preprocessing)
- The ability to restart the process without relaunching the application
- File history, possibly using DB
- Might transition to OpenGL in the future
- Add tools to make animation without tying speed to framerate
- Remove all the junk from other projects that got copied along with necessary stuff
