# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)
#set (CMAKE_CXX_STANDARD 14)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)

add_library( # Sets the name of the library.
        native-lib
        SHARED
        SerialPort.cpp
        PFBackgroundService.cpp
        SerialPortManager.cpp
        SPBackgroundService.cpp
        native-lib.cpp)

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib
#        JNI
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})