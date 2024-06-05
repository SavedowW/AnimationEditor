cmake_minimum_required (VERSION 3.20)
 
set(PROJECT_NAME AnimationEditor)

project(${PROJECT_NAME})

find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED)
find_package(SQLite3 REQUIRED)

set(SRC_FILES 
main.cpp
AnimViewLevel.cpp
ImTimelinePropertyInt.cpp
GuidelineViewer.cpp
ColliderViewer.cpp
${CMAKE_SOURCE_DIR}/external/imgui/imgui_demo.cpp
${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_sdl2.cpp
${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_sdlrenderer2.cpp
${CMAKE_SOURCE_DIR}/external/imgui/misc/cpp/imgui_stdlib.cpp
)

set(INCLUDE_DIRS
${CMAKE_CURRENT_LIST_DIR}
${CMAKE_CURRENT_LIST_DIR}/Core
${CMAKE_SOURCE_DIR}/external/imgui
${CMAKE_SOURCE_DIR}/external/imgui/backends
${CMAKE_SOURCE_DIR}/external/imgui/misc/cpp
${SQLite3_INCLUDE_DIRS}
)

set(LINK_LIBRARIES
${sdl_LIBRARIES_DEBUG}
SDL2::SDL2
SDL2_image::SDL2_image
${SQLite3_LIBRARIES}
)

if (WIN32)
    find_package(lz4 REQUIRED)
    list(APPEND LINK_LIBRARIES LZ4::lz4_static)
elseif (UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(lz4 REQUIRED IMPORTED_TARGET liblz4)
    list(APPEND LINK_LIBRARIES PkgConfig::lz4)
endif ()

#message(${LINK_LIBRARIES})

add_executable (${PROJECT_NAME} ${SRC_FILES})

add_subdirectory(Core)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

include_directories(${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES} Core)