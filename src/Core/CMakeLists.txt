cmake_minimum_required (VERSION 3.20)
 
project(Core)

set(CORE_SRC_FILES
Timer.cpp
InputSystem.cpp
TextureManager.cpp
Camera.cpp
Renderer.cpp
Window.cpp
Renderer.cpp
Application.cpp
AnimationManager.cpp
FrameTimer.cpp
ParticleManager.cpp
TimelineProperty.cpp
ResourceCounter.cpp
StateMarker.cpp
Level.cpp
EngineAnimation.cpp
DBAccessor.cpp
DBManager.cpp
UsedFilesManager.cpp
GuidelinesManager.cpp
)

add_library (Core ${CORE_SRC_FILES})

set_property(TARGET Core PROPERTY CXX_STANDARD 23)

include_directories(${INCLUDE_DIRS})
target_link_libraries(Core ${LINK_LIBRARIES})