cmake_minimum_required(VERSION 3.28)
project(Life-SDL)
set(CMAKE_CXX_STANDARD 23)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_VERBOSE_MAKEFILE 1)
    add_definitions(-DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_VERBOSE_MAKEFILE 0)
endif()

# Add subdirectories
add_subdirectory(vendor/SDL)

# Set include paths
include_directories(src/ vendor/SDL/include vendor/imgui)

# Add files
file(GLOB VENDOR_SOURCES vendor/imgui/*.cpp)
file(GLOB_RECURSE PROJECT_HEADERS src/*.hpp src/*.h)
file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp src/*.cxx src/*.cc src/*.c)
file(GLOB PROJECT_CONFIGS CMakeLists.txt README.md .gitignore .gitmodules)

# Organizing files
source_group("headers" FILES ${PROJECT_HEADERS})
source_group("sources" FILES ${PROJECT_SOURCES})

# Set executable and target link libraries
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_CONFIGS} ${VENDOR_SOURCES} resources.rc)
target_link_libraries(${PROJECT_NAME} SDL3::SDL3)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# Copy SDL3.dll to binary directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_BINARY_DIR}/vendor/SDL/SDL3.dll
        ${PROJECT_BINARY_DIR}
)

# Try to run without console in release mode
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(WIN32)
        if(MSVC)
            target_link_options(${PROJECT_NAME} PRIVATE "//SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
        elseif(MINGW)
            target_link_options(${PROJECT_NAME} PRIVATE "-mwindows")
        endif()
    else()
        if(MINGW)
            target_link_options(${PROJECT_NAME} PRIVATE "-mwindows")
        endif()
    endif()
endif()
